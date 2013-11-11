/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description: 
 *   EAP Wizard: Private implementation.
 */

// System includes
#include <QDebug>
#include <QDataStream>
#include <HbTranslator>
#include <HbParameterLengthLimiter>
#include <eapqtconfiginterface.h>
#include <eapqtplugininfo.h>
#include <eapqtpluginhandle.h>
#include <eapqtconfig.h>
#include <eapqtpacstoreconfig.h>
#include <eapqtcertificateinfo.h>
#include <wlanerrorcodes.h>

// User includes
#include "wlanwizardhelper.h"
#include "wlanwizardplugin.h"
#include "eapwizard.h"
#include "eapwizard_p.h"
#include "eapwizarduistrings.h"
#include "eapwizardpageoutertype.h"
#include "eapwizardpagecertca.h"
#include "eapwizardpagecertuser.h"
#include "eapwizardpageidentity.h"
#include "eapwizardpageinnertype.h"
#include "eapwizardpageusernamepassword.h"
#include "eapwizardpagenewpacstore.h"
#include "eapwizardpagepacstorepasswordconfirm.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "eapwizard_pTraces.h"
#endif

/*!
   \class EapWizardPrivate
   \brief Implements the EAP wizard plugin for WLAN wizard.
 */

// External function prototypes

// Local constants


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
   Constructor of EAP Wizard.

   @param [in] wizardHelper pointer to the helper instance.
 */
EapWizardPrivate::EapWizardPrivate(
    WlanWizardHelper *wizardHelper) :
    mWizardHelper(wizardHelper), 
    mTranslator(new HbTranslator("eapwizard")), 
    mEapConfIf(new EapQtConfigInterface(
        EapQtConfigInterface::EapBearerTypeWlan, 
        EapQtConfigInterface::IapIdUndefined))
{
    OstTraceFunctionEntry0( EAPWIZARDPRIVATE_EAPWIZARDPRIVATE_ENTRY );
    Q_ASSERT(wizardHelper);
    createPages();
    OstTraceFunctionExit0( EAPWIZARDPRIVATE_EAPWIZARDPRIVATE_EXIT );
}

/*!
   Destructor.
 */
EapWizardPrivate::~EapWizardPrivate()
{
    OstTraceFunctionEntry0( DUP1_EAPWIZARDPRIVATE_EAPWIZARDPRIVATE_ENTRY );
    OstTraceFunctionExit0( DUP1_EAPWIZARDPRIVATE_EAPWIZARDPRIVATE_EXIT );
}

/*!
   See EapWizard::summary().
 */
bool EapWizardPrivate::summary(
    WlanWizardPlugin::Summary sum, 
    QString &item, 
    QString &value)
{
    OstTraceFunctionEntry0( EAPWIZARDPRIVATE_SUMMARY_ENTRY );
    bool ret = false;
    int outerType = configurations(EapWizardPrivate::OuterType).toInt();

    switch (sum) {
    case WlanWizardPlugin::SummaryEapInnerType:
        if (hasInnerMethod()) {
            item = hbTrId("txt_occ_dblist_inner_eap");
            value = eapTypeToString(configurations(EapWizardPrivate::InnerType).toInt());
            ret = true;
        }
        break;

    case WlanWizardPlugin::SummaryEapFastProvisioningMode:
        if (outerType == EapQtPluginHandle::PluginEapFast) {
            item = hbTrId("txt_occ_dblist_provisioning_mode_for_eapfast");
            value = hbTrId("txt_occ_dblist_provisioning_mode_for_val_unauthent");
            ret = true;
        }
        break;

    default:
        Q_ASSERT(sum == WlanWizardPlugin::SummaryEapOuterType);
        item = hbTrId("txt_occ_dblist_outer_eap");
        value = eapTypeToString(outerType);
        ret = true;
        break;

    }
    OstTraceFunctionExit0( EAPWIZARDPRIVATE_SUMMARY_EXIT );
    return ret;
}

/*!
   See EapWizard::storeSettings().

   @return true - ok, false - failed
 */
bool EapWizardPrivate::storeSettings()
{
    OstTraceFunctionEntry0( EAPWIZARDPRIVATE_STORESETTINGS_ENTRY );
    bool ret = false;
    EapQtPluginHandle outerType(static_cast<EapQtPluginHandle::Plugin>(
        configurations(OuterType).toInt()));

    int iapId = mWizardHelper->configuration(WlanWizardHelper::ConfIapId).toInt();

    if (mEapConfIf->setConfigurationReference(iapId) 
        && storeOuterTypeSettings(outerType)
        && storeInnerTypeSettings(outerType)) {
        ret = true;
    }

    OstTraceFunctionExit0( EAPWIZARDPRIVATE_STORESETTINGS_EXIT );
    return ret;
}

/*!
   See WlanWizardPlugin::errorString().

   Returns EAP spesific error string.
 */
QString EapWizardPrivate::errorString(int errorCode)
{
    OstTraceFunctionEntry0( EAPWIZARDPRIVATE_ERRORSTRING_ENTRY );
    char* textId = NULL;
    int eapType = EapQtPluginHandle::PluginUndefined;

    switch (errorCode){
    case KErrWlanUserRejected:
        textId = "txt_occ_dialog_1_auth_failed_user_cert_rej";
        eapType = configurations(OuterType).toInt();
        break;

    case KErrWlanUserCertificateExpired:
        textId = "txt_occ_dialog_1_auth_failed_user_cert_exp";
        eapType = configurations(OuterType).toInt();
        break;

    case KErrWlanServerCertificateExpired:
        textId = "txt_occ_dialog_1_authentication_failed_server_ce";
        eapType = configurations(OuterType).toInt();
        break;

    case KErrWlanCerficateVerifyFailed:
        textId = "txt_occ_dialog_1_authentication_failed_could_not";
        eapType = configurations(OuterType).toInt();
        break;

    case KErrWlanNoCipherSuite:
        textId = "txt_occ_dialog_1_authentication_failed_cipher_su";
        eapType = configurations(OuterType).toInt();
        break;

    case KErrWlanSimNotInstalled:
        textId = "txt_occ_dialog_1_authentication_failed_check_sim";
        eapType = configurations(OuterType).toInt();
        break;

    case KErrWlanEapFastPacStoreCorrupted:
        textId = "txt_occ_dialog_1_authentication_failed_reset_pac";
        eapType = EapQtPluginHandle::PluginEapFast;
        break;

    case KErrWlanEapSimFailed:
    case KErrWlanEapTlsFailed:
    case KErrWlanEapPeapFailed:
    case KErrWlanEapAkaFailed:
    case KErrWlanEapTtlsFailed:
    case KErrWlanLeapFailed:
    case KErrWlanNoUserCertificate:
    case KErrWlanEapFastTunnelCompromiseError:
    case KErrWlanEapFastUnexpextedTlvExhanged:
    case KErrWlanEapFastNoPacNorCertsToAuthenticateWithProvDisabled:
    case KErrWlanEapFastNoMatchingPacForAid:
    case KErrWlanEapFastAuthFailed:
        textId = "txt_occ_dialog_1_authentication_failed";
        eapType = configurations(OuterType).toInt();
        break;

    case KErrWlanEapMsChapv2:
    case KErrWlanEapGtcFailed:
        textId = "txt_occ_dialog_1_authentication_failed";
        eapType = configurations(InnerType).toInt();
        break;

    case KErrWlanNotSubscribed:
    case KErrWlanAccessBarred:
    case KErrWlanPasswordExpired:
    case KErrWlanNoDialinPermissions:
    case KErrWlanAccountDisabled:
    case KErrWlanRestrictedLogonHours:
        textId = "txt_occ_dialog_1_authentication_failed";
        if (hasInnerMethod()){
            eapType = configurations(InnerType).toInt();
        } else {
            eapType = configurations(OuterType).toInt();
        }
        break;

    default:
        // Return empty string
        break;
    }

    QString string;
    if (textId) {
        string = HbParameterLengthLimiter(textId).arg(eapTypeToString(eapType));
    }

    OstTraceFunctionExit0( EAPWIZARDPRIVATE_ERRORSTRING_EXIT );
    return string;
}

/*!
   Reader method for eap configurations.

   See ConfigurationId for further details about the data types in QVariant.

   @param [in] confId Defines what configuration is read.

   @return configuration value.
 */
QVariant EapWizardPrivate::configurations(ConfigurationId confId) const
{
    OstTraceFunctionEntry0( EAPWIZARDPRIVATE_CONFIGURATIONS_ENTRY );
    Q_ASSERT(mConfigurations.contains(confId));

#ifdef OST_TRACE_COMPILER_IN_USE
    QString tmp;
    QDebug tmpStream(&tmp );
    tmpStream << mConfigurations[confId];
    TPtrC16 string(tmp.utf16(), tmp.length() );

    OstTraceExt2( 
        TRACE_NORMAL, 
        EAPWIZARDPRIVATE_CONFIGURATIONS, 
        "EapWizardPrivate::configurations;confId=%{ConfigurationId};string=%S", 
        (TUint)confId, string );
#endif

    OstTraceFunctionExit0( EAPWIZARDPRIVATE_CONFIGURATIONS_EXIT );
    return mConfigurations[confId];
}

/*!
   Sets EAP configuration value for given configuration identifier.
   See ConfigurationId for further details about the data types in QVariant.

   @param [in] confId Configuration Identifier do to be set
   @param [in] value  Value for configuration.
 */
void EapWizardPrivate::setConfigurations(
    ConfigurationId confId, const QVariant &value)
{
    OstTraceFunctionEntry0( EAPWIZARDPRIVATE_SETCONFIGURATIONS_ENTRY );
#ifdef OST_TRACE_COMPILER_IN_USE
    QString tmp;
    QDebug tmpStream(&tmp );
    tmpStream << value;
    TPtrC16 string( tmp.utf16(), tmp.length() );

    OstTraceExt2( 
        TRACE_NORMAL, 
        EAPWIZARDPRIVATE_SETCONFIGURATIONS, 
        "EapWizardPrivate::setConfigurations;confId=%{ConfigurationId};string=%S", 
        (TUint)confId, string );
#endif

    mConfigurations[confId] = value;
    OstTraceFunctionExit0( EAPWIZARDPRIVATE_SETCONFIGURATIONS_EXIT );
}

/*!
   @return pointer to EapQtConfigInterface.
 */
EapQtConfigInterface* EapWizardPrivate::eapConfigurationInterface() const
{
    OstTraceFunctionEntry0( EAPWIZARDPRIVATE_EAPCONFIGURATIONINTERFACE_ENTRY );
    Q_ASSERT(mEapConfIf.data());
    OstTraceFunctionExit0( EAPWIZARDPRIVATE_EAPCONFIGURATIONINTERFACE_EXIT );
    return mEapConfIf.data();
}

/*!
   Maps given EAP Qt Plugin handle into string.

   @param [in] id EapQtPluginHandle::Handle, id to be mapped to string.

   @return EAP String.
 */
QString EapWizardPrivate::eapTypeToString(int id) const
{
    OstTraceFunctionEntry0( EAPWIZARDPRIVATE_EAPTYPETOSTRING_ENTRY );
    QString locId;
    switch (id) {
    case EapQtPluginHandle::PluginEapAka:
        locId = EapWizardUiStrings::EapAka;
        break;

    case EapQtPluginHandle::PluginEapFast:
        locId = EapWizardUiStrings::EapFast;
        break;

    case EapQtPluginHandle::PluginLeap:
        locId = EapWizardUiStrings::Leap;
        break;

    case EapQtPluginHandle::PluginPeap:
        locId = EapWizardUiStrings::Peap;
        break;

    case EapQtPluginHandle::PluginEapSim:
        locId = EapWizardUiStrings::EapSim;
        break;

    case EapQtPluginHandle::PluginEapTls:
        locId = EapWizardUiStrings::EapTls;
        break;

    case EapQtPluginHandle::PluginEapTtls:
        locId = EapWizardUiStrings::EapTtls;
        break;

    case EapQtPluginHandle::PluginEapMschapv2:
        locId = EapWizardUiStrings::EapMschapv2;
        break;

    case EapQtPluginHandle::PluginEapGtc:
        locId = EapWizardUiStrings::EapGtc;
        break;

    case EapQtPluginHandle::PluginPap:
        locId = EapWizardUiStrings::Pap;
        break;

    default:
        // Invalid enum
        Q_ASSERT(EapQtPluginHandle::PluginPlainMschapv2 == id);
        locId = EapWizardUiStrings::Mschapv2;
        break;
    }

    OstTraceFunctionExit0( EAPWIZARDPRIVATE_EAPTYPETOSTRING_EXIT );
    return locId;
}

/*!
   Accessor to WLAN Wizard Helper objects.

   @return a pointer to helper object.
 */
WlanWizardHelper *EapWizardPrivate::wizardHelper() const
{
    OstTraceFunctionEntry0( EAPWIZARDPRIVATE_WIZARDHELPER_ENTRY );
    Q_ASSERT(mWizardHelper);
    OstTraceFunctionExit0( EAPWIZARDPRIVATE_WIZARDHELPER_EXIT );
    return mWizardHelper;
}

/*!
   Creates EAP Wizard pages and add those to the wizard framework.
 */
void EapWizardPrivate::createPages()
{
    OstTraceFunctionEntry0( EAPWIZARDPRIVATE_CREATEPAGES_ENTRY );
    mWizardHelper->addPage(
        EapWizardPage::PageOuterType,
        new EapWizardPageOuterType(this));

    mWizardHelper->addPage(
        EapWizardPage::PageCertificateCa,
        new EapWizardPageCertCa(this));

    mWizardHelper->addPage(
        EapWizardPage::PageCertificateUser,
        new EapWizardPageCertUser(this));

    mWizardHelper->addPage(
        EapWizardPage::PageIdentity,
        new EapWizardPageIdentity(this));

    mWizardHelper->addPage(
        EapWizardPage::PageInnerTypeEapTtls, 
        new EapWizardPageInnerType(
            this, 
            EapQtPluginHandle::PluginEapTtls));

    mWizardHelper->addPage(
        EapWizardPage::PageInnerTypePeap, 
        new EapWizardPageInnerType(
            this,
            EapQtPluginHandle::PluginPeap));

    mWizardHelper->addPage(
        EapWizardPage::PageUsernamePassword, 
        new EapWizardPageUsernamePassword(this));

    mWizardHelper->addPage(
        EapWizardPage::PageNewPacStorePassword, 
        new EapWizardPageNewPacStore(this));

    mWizardHelper->addPage(
        EapWizardPage::PagePromptPacStorePassword,
        new EapWizardPagePacStorePasswordConfirm(this));
    OstTraceFunctionExit0( EAPWIZARDPRIVATE_CREATEPAGES_EXIT );
}

/*!
   Check whether selected outer type has inner method.

   @return true if inner method exists, false otherwise.
 */
bool EapWizardPrivate::hasInnerMethod() const
{
    OstTraceFunctionEntry0( EAPWIZARDPRIVATE_HASINNERMETHOD_ENTRY );
    int outerType = configurations(EapWizardPrivate::OuterType).toInt();
    bool ret = false;

    switch (outerType) {
    case EapQtPluginHandle::PluginEapTtls:
    case EapQtPluginHandle::PluginPeap:
    case EapQtPluginHandle::PluginEapFast:
        ret = true;
        break;
    }

    OstTraceFunctionExit0( EAPWIZARDPRIVATE_HASINNERMETHOD_EXIT );
    return ret;
}


/*
   Handles TLS methods (PEAP, EAP-TLS and EAP-TTLS) settings.

   Stores configurations to eapConf

   @param [in,out] eapConf configuration is written to this object.
   @param [in] outerType Outer EAP method
 */
void EapWizardPrivate::handleTlsMethodsSettings(
    EapQtConfig &eapConf,
    EapQtPluginHandle &outerType)
{
    OstTraceFunctionEntry0( EAPWIZARDPRIVATE_HANDLETLSMETHODSSETTINGS_ENTRY );
    // Common stuff for all tls methods
    eapConf.setValue(EapQtConfig::UseIdentityPrivacy, false);
    eapConf.setValue(EapQtConfig::VerifyServerRealm, false);
    eapConf.setValue(EapQtConfig::ClientAuthenticationRequired, false);
    eapConf.setValue(EapQtConfig::UsernameAutomatic, configurations(TunnelUsernameAutomatic));
    eapConf.setValue(EapQtConfig::Username, configurations(TunnelUsername));
    eapConf.setValue(EapQtConfig::RealmAutomatic, configurations(TunnelRealmAutomatic));
    eapConf.setValue(EapQtConfig::Realm, configurations(TunnelRealm));

    QVariant certVariant = configurations(CertificateCa);

    if (certVariant.canConvert<EapQtCertificateInfo> ()) {
        QList<QVariant> caCerts;
        caCerts.append(certVariant);
        eapConf.setValue(EapQtConfig::AuthorityCertificate, caCerts);
        eapConf.setValue(EapQtConfig::AuthorityCertificateAutomatic, false);
    } else {
        eapConf.setValue(EapQtConfig::AuthorityCertificateAutomatic, true);
    }

    // type specific configurations
    if (outerType == EapQtPluginHandle::PluginEapTls) {
        QList<QVariant> userCerts;
        userCerts.append(configurations(CertificateUser));
        eapConf.setValue(EapQtConfig::ClientAuthenticationRequired, true);
        eapConf.setValue(EapQtConfig::UserCertificate, userCerts);

    } else if (outerType == EapQtPluginHandle::PluginPeap) {

        switch (configurations(InnerType).toInt()) {
        case EapQtPluginHandle::PluginEapMschapv2:
            // EAP-MSCHAPv2: enable v0 only
            eapConf.setValue(EapQtConfig::PeapVersion0Allowed, true);
            eapConf.setValue(EapQtConfig::PeapVersion1Allowed, false);
            break;

        case EapQtPluginHandle::PluginEapGtc:
            // EAP-GTC: enable v1 only
            eapConf.setValue(EapQtConfig::PeapVersion0Allowed, false);
            eapConf.setValue(EapQtConfig::PeapVersion1Allowed, true);
            break;
        }
        eapConf.setValue(EapQtConfig::PeapVersion2Allowed, false);
    }
    OstTraceFunctionExit0( EAPWIZARDPRIVATE_HANDLETLSMETHODSSETTINGS_EXIT );
}

/*!
   Handles configurations for EAP-FAST.

   @param [in,out] eapConf EAP Configuration

   @param false in case of failure.
 */
bool EapWizardPrivate::handleEapFastSettings(EapQtConfig &eapConf)
{
    OstTraceFunctionEntry0( EAPWIZARDPRIVATE_HANDLEEAPFASTSETTINGS_ENTRY );
    bool ret = true;
    EapQtPacStoreConfig pacStoreConf;

    eapConf.setValue(EapQtConfig::ProvisioningModeAuthenticated, true);
    eapConf.setValue(EapQtConfig::ProvisioningModeUnauthenticated, true);
    eapConf.setValue(EapQtConfig::VerifyServerRealm, false);
    eapConf.setValue(EapQtConfig::UseIdentityPrivacy, false);

    switch (configurations(PacStoreState).toInt()) {
    case EapQtPacStoreConfig::PacStoreStateStoreNotExists:
    case EapQtPacStoreConfig::PacStoreStatePasswordRequired:            
        pacStoreConf.setValue(
            EapQtPacStoreConfig::PacStorePassword, 
            configurations(PacStorePassword));

        pacStoreConf.setValue(
            EapQtPacStoreConfig::PacStoreSavePassword, 
            true);

        if (!mEapConfIf->savePacStoreConfiguration(pacStoreConf)) {
            // no cleaning required
            ret = false;
        }
        break;

    default:
        // Do nothing
        break;
    }

    OstTraceFunctionExit0( EAPWIZARDPRIVATE_HANDLEEAPFASTSETTINGS_EXIT );
    return ret;
}

/*!
   Handles configurations for EAP-AKA and EAP-SIM.

   @param [in,out] eapConf EAP Configuration
 */
void EapWizardPrivate::handleEapAkaSimSettings(EapQtConfig &eapConf)
{
    OstTraceFunctionEntry0( EAPWIZARDPRIVATE_HANDLEEAPAKASIMSETTINGS_ENTRY );
    eapConf.setValue(EapQtConfig::UsernameAutomatic, true);
    eapConf.setValue(EapQtConfig::RealmAutomatic, true);    
    OstTraceFunctionExit0( EAPWIZARDPRIVATE_HANDLEEAPAKASIMSETTINGS_EXIT );
}

/*!
   Handles configurations for LEAP.

   @param [in,out] eapConf EAP Configuration
 */
void EapWizardPrivate::handleLeapSettings(EapQtConfig &eapConf)
{
    OstTraceFunctionEntry0( EAPWIZARDPRIVATE_HANDLELEAPSETTINGS_ENTRY );
    eapConf.setValue(EapQtConfig::UsernameAutomatic, false);
    eapConf.setValue(EapQtConfig::Username, configurations(Username));
    eapConf.setValue(EapQtConfig::PasswordPrompt, false);
    eapConf.setValue(EapQtConfig::Password, configurations(Password));    
    OstTraceFunctionExit0( EAPWIZARDPRIVATE_HANDLELEAPSETTINGS_EXIT );
}

/*!
   Store outer type settings.

   @param outerType reference to outertype

   @return false in case of failure.
 */
bool EapWizardPrivate::storeOuterTypeSettings(EapQtPluginHandle &outerType)
{
    OstTraceFunctionEntry0( EAPWIZARDPRIVATE_STOREOUTERTYPESETTINGS_ENTRY );
    EapQtConfig eapConf;

    // 1. Store outer type settings
    switch (outerType.pluginId()) {
    case EapQtPluginHandle::PluginEapTtls: 
    case EapQtPluginHandle::PluginPeap:
    case EapQtPluginHandle::PluginEapTls:
        handleTlsMethodsSettings(eapConf, outerType);
        break;

    case EapQtPluginHandle::PluginEapFast:
        if (!handleEapFastSettings(eapConf)) {
            OstTraceFunctionExit0( EAPWIZARDPRIVATE_STOREOUTERTYPESETTINGS_EXIT );
            return false;
        }
        break;

    case EapQtPluginHandle::PluginEapAka: 
    case EapQtPluginHandle::PluginEapSim:
        handleEapAkaSimSettings(eapConf);
        break;

    default:
        Q_ASSERT(outerType == EapQtPluginHandle::PluginLeap);
        handleLeapSettings(eapConf);
        break;
    }

    if (hasInnerMethod()) {
        EapQtPluginHandle inner = static_cast<EapQtPluginHandle::Plugin>(
            configurations(InnerType).toInt());

        QList<QVariant> innerType;
        innerType.append(qVariantFromValue(inner));
        eapConf.setValue(EapQtConfig::InnerType, innerType);
    }

    // store outer type configurations
    if (!mEapConfIf->saveConfiguration(outerType, eapConf)){
        mEapConfIf->deleteConfiguration();
        OstTraceFunctionExit0( DUP1_EAPWIZARDPRIVATE_STOREOUTERTYPESETTINGS_EXIT );
        return false;
    }

    QList<EapQtPluginHandle> selectedOuterTypes;
    selectedOuterTypes.append(outerType);
    if (!mEapConfIf->setSelectedOuterTypes(selectedOuterTypes)){
        mEapConfIf->deleteConfiguration();
        OstTraceFunctionExit0( DUP2_EAPWIZARDPRIVATE_STOREOUTERTYPESETTINGS_EXIT );
        return false;
    }

    OstTraceFunctionExit0( DUP3_EAPWIZARDPRIVATE_STOREOUTERTYPESETTINGS_EXIT );
    return true;
}


/*!
   Store inner type settings if exists.

   @param outerType reference to outertype

   @return false in case of failure.
 */
bool EapWizardPrivate::storeInnerTypeSettings(EapQtPluginHandle &outerType)
{
    OstTraceFunctionEntry0( EAPWIZARDPRIVATE_STOREINNERTYPESETTINGS_ENTRY );
    bool ret = true;

    if (hasInnerMethod()) {
        EapQtPluginHandle inner = static_cast<EapQtPluginHandle::Plugin>(
            configurations(InnerType).toInt());

        // All inner methods supported by wizard use password / username. 
        EapQtConfig eapConfInner;
        eapConfInner.setValue(EapQtConfig::OuterType, qVariantFromValue(outerType));
        eapConfInner.setValue(EapQtConfig::UsernameAutomatic, false);
        eapConfInner.setValue(EapQtConfig::Username, configurations(Username));
        eapConfInner.setValue(EapQtConfig::PasswordPrompt, false);
        eapConfInner.setValue(EapQtConfig::Password, configurations(Password));

        if (!mEapConfIf->saveConfiguration(inner, eapConfInner)){
            mEapConfIf->deleteConfiguration();
            ret = false;
        }
    }
    OstTraceFunctionExit0( EAPWIZARDPRIVATE_STOREINNERTYPESETTINGS_EXIT );
    return ret;
}
