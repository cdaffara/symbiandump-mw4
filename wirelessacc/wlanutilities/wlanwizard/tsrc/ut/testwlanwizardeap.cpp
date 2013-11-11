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
* WLAN Wizard UT.
*/

// System includes

#include <HbParameterLengthLimiter>

#include <QTest>
#include <QDebug>
#include <QList>

#include <cmmanagerdefines_shim.h>
#include <wlanerrorcodes.h>

// User includes

#include "testwlanwizardeap.h"
#include "testwlanwizardeap_conf.h"
#include "hbautotest.h"
#include "wlanwizard.h"
#include "wlanwizard_p.h"
#include "wlanqtutils_context.h"
#include "wlanqtutils.h"
#include "wlanqtutilsap.h"
#include "wlanwizardpageinternal.h"
#include "eapwizardpage.h"
#include "eapqtvalidator_stub.h"

#include "eapqtconfiginterface_context.h"

// External function prototypes

// Local constants


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------
// TEST CASES
// ---------------------------------------------------------
void TestWlanWizardEap::tc_eap_outer_page_orientation_change()
{
#ifdef tc_eap_outer_page_orientation_change_enabled
   
    const QString ssid("tc_eap_outer_page_orientation_change");
    
    mEapQtUtilsContext->createEapAkaSim(EapQtPluginHandle::PluginEapAka);
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa2, 
        false,
        false);

    mView->showWizard();
    
    verify_outertype(ButtonDisabled);
    
    mMainWindow->setOrientation(Qt::Horizontal, false);
    QTest::qWait(WaitTimeForUi);

    verify_outertype(ButtonDisabled);
    
    mMainWindow->setOrientation(Qt::Vertical, false);
    QTest::qWait(WaitTimeForUi);
    
    verify_outertype(ButtonDisabled);
#endif
}

void TestWlanWizardEap::tc_eap_aka()
{
#ifdef tc_eap_aka_enabled
    tc_eap_aka_sim(EapQtPluginHandle::PluginEapAka);
#endif
}

void TestWlanWizardEap::tc_eap_sim()
{
#ifdef tc_eap_sim_enabled
    tc_eap_aka_sim(EapQtPluginHandle::PluginEapSim);
#endif
}

void TestWlanWizardEap::tc_eap_leap()
{
#ifdef tc_eap_leap_enabled
    const QString username("user");
    const QString password("passwd");
    mWlanQtUtilsContext->setCreateWlanIapResult(3);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(3);
    mWlanQtUtilsContext->setSignalIctResult(3, WlanQtUtils::IctPassed);
    
    mEapQtUtilsContext->setConfigurationReference(3, true);
    mEapQtUtilsContext->createLeap(username,password);
    
    const QString ssid("tc_eap_leap");
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa, 
        false,
        false);

    mView->showWizard();
    
    verify_outertype_select_first(ButtonDisabled);
    verify_username_password_click_next(EapQtPluginHandle::PluginLeap, username, password);
    
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPage::PageProcessSettings,ssid), true );
    QCOMPARE( verifyActionButtons(ButtonDisabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QTest::qWait(WaitTimeForUi);

    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonHidden, ButtonEnabled), true );
    QTest::qWait(WaitTimeForUi);
    
    QCOMPARE( verifySummaryPage(
        ssid,
        CMManagerShim::Infra,
        false, 
        CMManagerShim::WlanSecModeWpa, 
        false,
        hbTrId("txt_occ_dblist_destination_val_internet"),
        EapQtPluginHandle::PluginLeap),true);
    
    QTest::qWait(WaitTimeForUi);
#endif
}

void TestWlanWizardEap::tc_eap_peap_gtc()
{
#ifdef tc_eap_peap_gtc_enabled
    tc_eap_peap_ttls(
        EapQtPluginHandle::PluginPeap, 
        EapQtPluginHandle::PluginEapGtc);
#endif
}

void TestWlanWizardEap::tc_eap_peap_mschapv2()
{
#ifdef tc_eap_peap_mschapv2_enabled
    tc_eap_peap_ttls(
        EapQtPluginHandle::PluginPeap, 
        EapQtPluginHandle::PluginEapMschapv2);
#endif
}

void TestWlanWizardEap::tc_eap_ttls_mschapv2()
{
#ifdef tc_eap_ttls_mschapv2_enabled
    tc_eap_peap_ttls(
        EapQtPluginHandle::PluginEapTtls, 
        EapQtPluginHandle::PluginPlainMschapv2);
#endif
}

void TestWlanWizardEap::tc_eap_ttls_pap()
{
#ifdef tc_eap_ttls_pap_enabled
    tc_eap_peap_ttls(
        EapQtPluginHandle::PluginEapTtls, 
        EapQtPluginHandle::PluginPap);
#endif
}

void TestWlanWizardEap::tc_eap_ttls_gtc()
{
#ifdef tc_eap_ttls_gtc_enabled
    tc_eap_peap_ttls(
        EapQtPluginHandle::PluginEapTtls, 
        EapQtPluginHandle::PluginEapGtc);
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanUserRejected()
{
#ifdef tc_eap_aka_connect_KErrWlanUserRejected_enabled
    tc_eap_aka_connect_error(
        KErrWlanUserRejected,
        HbParameterLengthLimiter("txt_occ_dialog_1_auth_failed_user_cert_rej").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanUserCertificateExpired()
{
#ifdef tc_eap_aka_connect_KErrWlanUserCertificateExpired_enabled
    tc_eap_aka_connect_error(
        KErrWlanUserCertificateExpired,
        HbParameterLengthLimiter("txt_occ_dialog_1_auth_failed_user_cert_exp").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanServerCertificateExpired()
{
#ifdef tc_eap_aka_connect_KErrWlanServerCertificateExpired_enabled
    tc_eap_aka_connect_error(
        KErrWlanServerCertificateExpired,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed_server_ce").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanCerficateVerifyFailed()
{
#ifdef tc_eap_aka_connect_KErrWlanCerficateVerifyFailed_enabled
    tc_eap_aka_connect_error(
        KErrWlanCerficateVerifyFailed,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed_could_not").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));
#endif    
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanNoCipherSuite()
{
#ifdef tc_eap_aka_connect_KErrWlanNoCipherSuite_enabled
    tc_eap_aka_connect_error(
        KErrWlanNoCipherSuite,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed_cipher_su").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));
#endif     
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanSimNotInstalled()
{
#ifdef tc_eap_aka_connect_KErrWlanSimNotInstalled_enabled
    tc_eap_aka_connect_error(
        KErrWlanSimNotInstalled,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed_check_sim").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));
#endif      
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanEapFastPacStoreCorrupted()
{
#ifdef tc_eap_aka_connect_KErrWlanEapFastPacStoreCorrupted_enabled
    tc_eap_aka_connect_error(
        KErrWlanEapFastPacStoreCorrupted,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed_reset_pac").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapFast)));
#endif      
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanEapSimFailed()
{
#ifdef tc_eap_aka_connect_KErrWlanEapSimFailed_enabled
    tc_eap_aka_connect_error(
        KErrWlanEapSimFailed,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));
#endif      
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanNotSubscribed()
{
#ifdef tc_eap_aka_connect_KErrWlanNotSubscribed_enabled
    tc_eap_aka_connect_error(
        KErrWlanNotSubscribed,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));
#endif      
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrCancel()
{
#ifdef tc_eap_aka_connect_KErrCancel_enabled
    tc_eap_aka_connect_error(
        KErrCancel,
        hbTrId("txt_occ_dialog_connection_failed"));
#endif       
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanEapTlsFailed()
{
#ifdef tc_eap_aka_connect_KErrWlanEapTlsFailed_enabled
    tc_eap_aka_connect_error(
        KErrWlanEapTlsFailed,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));
#endif
}
void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanEapPeapFailed()
{
#ifdef tc_eap_aka_connect_KErrWlanEapPeapFailed_enabled
    tc_eap_aka_connect_error(
        KErrWlanEapPeapFailed,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));    
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanEapAkaFailed()
{
#ifdef tc_eap_aka_connect_KErrWlanEapAkaFailed_enabled
    tc_eap_aka_connect_error(
        KErrWlanEapAkaFailed,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanEapTtlsFailed()
{
#ifdef tc_eap_aka_connect_KErrWlanEapTtlsFailed_enabled
    tc_eap_aka_connect_error(
        KErrWlanEapTtlsFailed,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanLeapFailed()
{
#ifdef tc_eap_aka_connect_KErrWlanLeapFailed_enabled
    tc_eap_aka_connect_error(
        KErrWlanLeapFailed,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanNoUserCertificate()
{
#ifdef tc_eap_aka_connect_KErrWlanNoUserCertificate_enabled 
    tc_eap_aka_connect_error(
        KErrWlanNoUserCertificate,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanEapFastTunnelCompromiseError()
{
#ifdef tc_eap_aka_connect_KErrWlanEapFastTunnelCompromiseError_enabled
    tc_eap_aka_connect_error(
        KErrWlanEapFastTunnelCompromiseError,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));    
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanEapFastUnexpextedTlvExhanged()
{
#ifdef tc_eap_aka_connect_KErrWlanEapFastUnexpextedTlvExhanged_enabled
    tc_eap_aka_connect_error(
        KErrWlanEapFastUnexpextedTlvExhanged,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));    
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanEapFastNoPacNorCertsToAuthenticateWithProvDisabled()
{
#ifdef tc_eap_aka_connect_KErrWlanEapFastNoPacNorCertsToAuthenticateWithProvDisabled_enabled
    tc_eap_aka_connect_error(
        KErrWlanEapFastNoPacNorCertsToAuthenticateWithProvDisabled,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanEapFastNoMatchingPacForAid()
{
#ifdef tc_eap_aka_connect_KErrWlanEapFastNoMatchingPacForAid_enabled
    tc_eap_aka_connect_error(
        KErrWlanEapFastNoMatchingPacForAid,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanEapFastAuthFailed()
{
#ifdef tc_eap_aka_connect_KErrWlanEapFastAuthFailed_enabled
    tc_eap_aka_connect_error(
        KErrWlanEapFastAuthFailed,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));    
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanAccessBarred()
{
#ifdef tc_eap_aka_connect_KErrWlanAccessBarred_enabled
    tc_eap_aka_connect_error(
        KErrWlanAccessBarred,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));    
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanPasswordExpired()
{
#ifdef tc_eap_aka_connect_KErrWlanPasswordExpired_enabled
    tc_eap_aka_connect_error(
        KErrWlanPasswordExpired,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));    
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanNoDialinPermissions()
{
#ifdef tc_eap_aka_connect_KErrWlanNoDialinPermissions_enabled
    tc_eap_aka_connect_error(
        KErrWlanNoDialinPermissions,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));    
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanAccountDisabled()
{
#ifdef tc_eap_aka_connect_KErrWlanAccountDisabled_enabled
    tc_eap_aka_connect_error(
        KErrWlanAccountDisabled,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));
#endif
}

void TestWlanWizardEap::tc_eap_aka_connect_KErrWlanRestrictedLogonHours()
{
#ifdef tc_eap_aka_connect_KErrWlanRestrictedLogonHours_enabled
    tc_eap_aka_connect_error(
        KErrWlanRestrictedLogonHours,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapAka)));    
#endif
}

void TestWlanWizardEap::tc_eap_ttls_mschapv2_connect_KErrWlanEapGtcFailed()
{
#ifdef tc_eap_ttls_mschapv2_connect_KErrWlanEapGtcFailed_enabled
    tc_eap_peap_ttls(
        EapQtPluginHandle::PluginEapTtls, 
        EapQtPluginHandle::PluginPlainMschapv2,
        KErrWlanEapGtcFailed,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginPlainMschapv2)));
#endif
}

void TestWlanWizardEap::tc_eap_ttls_mschapv2_connect_KErrWlanEapMsChapv2()
{
#ifdef tc_eap_ttls_mschapv2_connect_KErrWlanEapMsChapv2_enabled
    tc_eap_peap_ttls(
        EapQtPluginHandle::PluginEapTtls, 
        EapQtPluginHandle::PluginPlainMschapv2,
        KErrWlanEapMsChapv2,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginPlainMschapv2)));
#endif
}

void TestWlanWizardEap::tc_eap_ttls_mschapv2_connect_KErrWlanNotSubscribed()
{
#ifdef tc_eap_ttls_mschapv2_connect_KErrWlanNotSubscribed_enabled
    tc_eap_peap_ttls(
        EapQtPluginHandle::PluginEapTtls, 
        EapQtPluginHandle::PluginPlainMschapv2,
        KErrWlanNotSubscribed,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginPlainMschapv2)));
#endif
}

void TestWlanWizardEap::tc_eap_ttls_gtc_back_and_forward()
{
#ifdef tc_eap_ttls_gtc_back_and_forward_enabled
    tc_eap_peap_ttls(
        EapQtPluginHandle::PluginEapTtls, 
        EapQtPluginHandle::PluginEapGtc,
        KErrWlanNotSubscribed,
        HbParameterLengthLimiter("txt_occ_dialog_1_authentication_failed").arg(
            eapTypeToString(EapQtPluginHandle::PluginEapGtc)));
    
    // Go back to outertype selection page
    QCOMPARE(mouseClickPrevious(), true);
    QTest::qWait(WaitTimeForUi);
    verify_username_password(EapQtPluginHandle::PluginEapGtc);

    QCOMPARE(mouseClickPrevious(), true);
    QTest::qWait(WaitTimeForUi);
    verify_innertype(
        EapQtPluginHandle::PluginEapTtls,
        ButtonEnabled);

    QCOMPARE(mouseClickPrevious(), true);
    QTest::qWait(WaitTimeForUi);
    verify_identity_page(eapTypeToString(EapQtPluginHandle::PluginEapTtls));
    
    QCOMPARE(mouseClickPrevious(), true);
    QTest::qWait(WaitTimeForUi);
    verify_ca_cert_page();
    
    QCOMPARE(mouseClickPrevious(), true);
    QTest::qWait(WaitTimeForUi);
    verify_outertype(ButtonEnabled);
    
    // Go forward to username & password page
    QCOMPARE(mouseClickNext(), true);
    QTest::qWait(WaitTimeForUi);
    verify_ca_cert_page();
    
    // Create new validators
    mEapQtUtilsContext->createEapIdentifyValidators(EapQtPluginHandle::PluginEapTtls);
    mEapQtUtilsContext->createInnerTypeValidators(EapQtPluginHandle::PluginEapGtc);
    
    QCOMPARE(mouseClickNext(), true);
    QTest::qWait(WaitTimeForUi);
    verify_identity_page(eapTypeToString(EapQtPluginHandle::PluginEapTtls));
    
    QCOMPARE(mouseClickNext(), true);
    QTest::qWait(WaitTimeForUi);
    verify_innertype(
        EapQtPluginHandle::PluginEapTtls,
        ButtonEnabled);
    
    QCOMPARE(mouseClickNext(), true);
    QTest::qWait(WaitTimeForUi);
    verify_username_password(EapQtPluginHandle::PluginEapGtc);
#endif
}

void TestWlanWizardEap::tc_eap_aka_set_outer_type_fails()
{
#ifdef tc_eap_aka_set_outer_type_fails_enabled
    const QString ssid("tc_eap_aka_set_outer_type_fails");
    
    mWlanQtUtilsContext->setCreateWlanIapResult(3);
    mWlanQtUtilsContext->setConnectionSuccessed(false);
    mWlanQtUtilsContext->setSignalWlanNetworkClosed(3, KErrCancel);
    
    mEapQtUtilsContext->setConfigurationReference(3, true);
    mEapQtUtilsContext->createEapAkaSim(EapQtPluginHandle::PluginEapAka);
    mEapQtUtilsContext->setSelectectedOuterTypesReturn(false);
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa2, 
        false,
        false);

    mView->showWizard();
    
    verify_outertype_select_first(ButtonDisabled);
    
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, ssid), true );
    QCOMPARE( verifyActionButtons(ButtonDisabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( 
        verifyCurrentPageWithInfo(
            WlanWizardPageInternal::PageGenericError, 
            hbTrId("txt_occ_dialog_unable_to_save_settings_please_ret")), true );
    
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
#endif
}

void TestWlanWizardEap::tc_eap_aka_save_outer_configuration_fails()
{
#ifdef tc_eap_aka_save_outer_configuration_fails_enabled
    const QString ssid("tc_eap_aka_save_outer_configuration_fails");
    
    mWlanQtUtilsContext->setCreateWlanIapResult(3);
    mWlanQtUtilsContext->setConnectionSuccessed(false);
    mWlanQtUtilsContext->setSignalWlanNetworkClosed(3, KErrCancel);
    
    mEapQtUtilsContext->setConfigurationReference(3, true);
    mEapQtUtilsContext->createEapAkaSim(EapQtPluginHandle::PluginEapAka);
    mEapQtUtilsContext->setOuterConfigReturn(false);
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa2, 
        false,
        false);

    mView->showWizard();
    
    verify_outertype_select_first(ButtonDisabled);
    
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, ssid), true );
    QCOMPARE( verifyActionButtons(ButtonDisabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( 
        verifyCurrentPageWithInfo(
            WlanWizardPageInternal::PageGenericError, 
            hbTrId("txt_occ_dialog_unable_to_save_settings_please_ret")), true );
    
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
#endif
}

void TestWlanWizardEap::tc_eap_aka_set_configuration_reference_fails()
{
#ifdef tc_eap_aka_set_configuration_reference_fails_enabled
    const QString ssid("tc_eap_aka_set_configuration_reference_fails");
    
    mWlanQtUtilsContext->setCreateWlanIapResult(3);
    mWlanQtUtilsContext->setConnectionSuccessed(false);
    mWlanQtUtilsContext->setSignalWlanNetworkClosed(3, KErrCancel);
    
    mEapQtUtilsContext->setConfigurationReference(3, false);
    mEapQtUtilsContext->createEapAkaSim(EapQtPluginHandle::PluginEapAka);
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa2, 
        false,
        false);

    mView->showWizard();
    
    verify_outertype_select_first(ButtonDisabled);
    
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, ssid), true );
    QCOMPARE( verifyActionButtons(ButtonDisabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    QCOMPARE( 
        verifyCurrentPageWithInfo(
            WlanWizardPageInternal::PageGenericError, 
            hbTrId("txt_occ_dialog_unable_to_save_settings_please_ret")), true );
    
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
#endif
}

void TestWlanWizardEap::tc_eap_peap_gtc_save_inner_configuration_fails()
{
#ifdef tc_eap_peap_gtc_save_inner_configuration_fails_enabled
    tc_eap_peap_ttls(
        EapQtPluginHandle::PluginEapTtls, 
        EapQtPluginHandle::PluginEapGtc,
        KErrGeneral, // Not used directly in sub test case
        hbTrId("txt_occ_dialog_unable_to_save_settings_please_ret"),
        false );
#endif
}


void TestWlanWizardEap::tc_eap_peap_identity_validators()
{
#ifdef tc_eap_peap_identity_validators_enabled
    const QString ssid("tc_eap_peap_identity_validators");
    const QString username("u");
    const QString password("p");
    const QString usernameIdentity("identity");
    const QString realm("realm.com");
    EapQtPluginHandle::Plugin outerType(EapQtPluginHandle::PluginPeap);
    EapQtPluginHandle::Plugin innerType(EapQtPluginHandle::PluginEapGtc);
    
    mWlanQtUtilsContext->setCreateWlanIapResult(3);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(3);
    mWlanQtUtilsContext->setSignalIctResult(3, WlanQtUtils::IctPassed);

    mEapQtUtilsContext->setConfigurationReference(3, true);
    mEapQtUtilsContext->createEapTtlsPeap(
        outerType,
        true,
        true,
        usernameIdentity,
        true,
        realm,
        innerType );

    EapQtValidatorStub* validatorUser = (EapQtValidatorStub*)mEapQtUtilsContext->validator(
        EapQtExpandedEapType::TypePeap, EapQtConfig::Username);
    EapQtValidatorStub* validatorRealm = (EapQtValidatorStub*)mEapQtUtilsContext->validator(
        EapQtExpandedEapType::TypePeap,EapQtConfig::Realm);
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa, 
        false,
        false);

    mView->showWizard();
    
    verify_outertype_select_first(ButtonDisabled);
    verify_ca_cert_page_select_and_click_next(true);
    
    QCOMPARE( verifyCurrentPageWithInfo(EapWizardPage::PageIdentity, eapTypeToString(outerType)), true );
    // Check box checked next enabled
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    QCOMPARE(verifyCheckBoxState("setlabel_53_val", true), true);
    QCOMPARE(verifyCheckBoxState("setlabel_54_val", true), true);

    // 1. Username validator returns false, checkbox not checked
    validatorUser->setReturnValue(EapQtValidator::StatusInvalid);
    validatorRealm->setReturnValue(EapQtValidator::StatusOk);
    
    QCOMPARE(selectCheckBoxState("setlabel_53_val", false), true);
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
  
    // 2. Username validator returns true, checkbox not checked
    validatorUser->setReturnValue(EapQtValidator::StatusOk);
    validatorRealm->setReturnValue(EapQtValidator::StatusOk);
    QCOMPARE( mouseClickObject("lineEditUsername"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditUsername", username), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );    

    // 3. back to valid state, checkboc checked
    QCOMPARE(selectCheckBoxState("setlabel_53_val", true), true);
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );

    // 4. Realm validator returns false, checkbox not checked
    validatorUser->setReturnValue(EapQtValidator::StatusOk);
    validatorRealm->setReturnValue(EapQtValidator::StatusInvalid);

    QCOMPARE(selectCheckBoxState("setlabel_54_val", false), true);
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    
    // 5. Realm validator returns true, checkbox not checked
    validatorUser->setReturnValue(EapQtValidator::StatusOk);
    validatorRealm->setReturnValue(EapQtValidator::StatusOk);
    QCOMPARE( mouseClickObject("lineEditRealm"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditRealm", username), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );    

    // 6. back to valid state, checkboc checked
    QCOMPARE(selectCheckBoxState("setlabel_54_val", true), true);
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
#endif
}

void TestWlanWizardEap::tc_eap_leap_user_password_validators()
{
#ifdef tc_eap_leap_user_password_validators_enabled
    const QString ssid("tc_eap_leap_user_password_validators");
    const QString username("u");
    const QString password("p");
    
    mWlanQtUtilsContext->setCreateWlanIapResult(3);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(3);
    mWlanQtUtilsContext->setSignalIctResult(3, WlanQtUtils::IctPassed);
    
    mEapQtUtilsContext->setConfigurationReference(3, true);
    mEapQtUtilsContext->createLeap(username,password);
    
    EapQtValidatorStub* validatorUser = (EapQtValidatorStub*)mEapQtUtilsContext->validator(
        EapQtExpandedEapType::TypeLeap, EapQtConfig::Username);
    EapQtValidatorStub* validatorPassword = (EapQtValidatorStub*)mEapQtUtilsContext->validator(
        EapQtExpandedEapType::TypeLeap, EapQtConfig::Password);
    validatorUser->setReturnValue(EapQtValidator::StatusInvalid);
    validatorPassword->setReturnValue(EapQtValidator::StatusInvalid);

    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa, 
        false,
        false);

    mView->showWizard();
    
    verify_outertype_select_first(ButtonDisabled);

    // 1. By default both validator return invalid
    QCOMPARE( 
        verifyCurrentPageWithInfo(
            EapWizardPage::PageUsernamePassword, 
            eapTypeToString(EapQtPluginHandle::PluginLeap)), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    
    // 2. User return ok, password invalid 
    validatorUser->setReturnValue(EapQtValidator::StatusOk);
    QCOMPARE( mouseClickObject("lineEditUsername"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditUsername", username), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    
    // 3. User return ok, password ok
    validatorPassword->setReturnValue(EapQtValidator::StatusOk);
    QCOMPARE( mouseClickObject("lineEditPassword"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditPassword", password), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
#endif
}

void TestWlanWizardEap::tc_eap_tls_no_user_certificate()
{
#ifdef tc_eap_tls_no_user_certificate_enabled
    
    const QString ssid("tc_eap_tls_no_user_certificate");
    
    QList<EapQtCertificateInfo> caCerts;
    QList<EapQtCertificateInfo> userCerts;
    
    EapQtCertificateInfo caCert;
    caCert.setValue(EapQtCertificateInfo::CertificateLabel, "My Super Cert 1");
    caCerts.append(caCert);
    
    mEapQtUtilsContext->createEapTls(
        caCerts,
        0,
        userCerts,
        -1);
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa2, 
        false,
        false);

    mView->showWizard();
    
    verify_outertype_select_first(ButtonDisabled);
    verify_ca_cert_page_select_and_click_next(false);
    verify_user_cert_page_select_and_click_next(ButtonDisabled, -1);

#endif
}

void TestWlanWizardEap::tc_eap_tls_one_user_certificate_with_ca_selection()
{
#ifdef tc_eap_tls_one_user_certificate_with_ca_selection_enabled
    const QString ssid("tc_eap_tls_one_user_certificate_with_ca_selection");
    
    mWlanQtUtilsContext->setCreateWlanIapResult(3);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(3);
    mWlanQtUtilsContext->setSignalIctResult(3, WlanQtUtils::IctPassed);
    
    mEapQtUtilsContext->setConfigurationReference(3, true);
    
    QList<EapQtCertificateInfo> caCerts;
    QList<EapQtCertificateInfo> userCerts;
    
    EapQtCertificateInfo cert;
    cert.setValue(EapQtCertificateInfo::CertificateLabel, "My CA Cert");
    caCerts.append(cert);
    
    cert.setValue(EapQtCertificateInfo::CertificateLabel, "My User Cert");
    userCerts.append(cert);
    
    mEapQtUtilsContext->createEapTls(
        caCerts,
        0,
        userCerts,
        0);
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa2, 
        false,
        false);

    mView->showWizard();
    
    verify_outertype_select_first(ButtonDisabled);
    verify_ca_cert_page_select_and_click_next(false);
    verify_user_cert_page_select_and_click_next(ButtonEnabled, 0);
    verify_identity_and_click_next(eapTypeToString(EapQtPluginHandle::PluginEapTls));
    
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPage::PageProcessSettings,ssid), true );
    QCOMPARE( verifyActionButtons(ButtonDisabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );

    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonHidden, ButtonEnabled), true );
    
    QCOMPARE( verifySummaryPage(
        ssid,
        CMManagerShim::Infra,
        false, 
        CMManagerShim::WlanSecModeWpa2, 
        false,
        hbTrId("txt_occ_dblist_destination_val_internet"),
        EapQtPluginHandle::PluginEapTls),true);
    
#endif
}

void TestWlanWizardEap::tc_eap_tls_two_user_certificates()
{
#ifdef tc_eap_tls_two_user_certificates_enabled
    const QString ssid("tc_eap_tls_two_user_certificates");
    
    mWlanQtUtilsContext->setCreateWlanIapResult(3);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(3);
    mWlanQtUtilsContext->setSignalIctResult(3, WlanQtUtils::IctPassed);
    
    mEapQtUtilsContext->setConfigurationReference(3, true);
    
    QList<EapQtCertificateInfo> caCerts;
    QList<EapQtCertificateInfo> userCerts;
    
    EapQtCertificateInfo cert;
    cert.setValue(EapQtCertificateInfo::CertificateLabel, "My CA Cert");
    caCerts.append(cert);
    cert.setValue(EapQtCertificateInfo::CertificateLabel, "My Super CA Cert");
    caCerts.append(cert);
    
    cert.setValue(EapQtCertificateInfo::CertificateLabel, "My User Cert");
    userCerts.append(cert);
    cert.setValue(EapQtCertificateInfo::CertificateLabel, "My User Cert Second");
    userCerts.append(cert);
    
    mEapQtUtilsContext->createEapTls(
        caCerts,
        0,
        userCerts,
        1);
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa2, 
        false,
        false);

    mView->showWizard();
    
    verify_outertype_select_first(ButtonDisabled);
    verify_ca_cert_page_select_and_click_next(false);
    verify_user_cert_page_select_and_click_next(ButtonDisabled, 1);
    verify_identity_and_click_next(eapTypeToString(EapQtPluginHandle::PluginEapTls));
    
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPage::PageProcessSettings,ssid), true );
    QCOMPARE( verifyActionButtons(ButtonDisabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );

    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonHidden, ButtonEnabled), true );
    
    QCOMPARE( verifySummaryPage(
        ssid,
        CMManagerShim::Infra,
        false, 
        CMManagerShim::WlanSecModeWpa2, 
        false,
        hbTrId("txt_occ_dblist_destination_val_internet"),
        EapQtPluginHandle::PluginEapTls),true);
    
#endif 
}

void TestWlanWizardEap::tc_eap_tls_back_and_forward_certs_with_cert_update()
{
#ifdef tc_eap_tls_back_and_forward_certs_with_cert_update_enabled
    const QString ssid("tc_eap_tls_two_user_certificates");
    
    const QString caCert1("My CA Cert");
    const QString caCert2("My CA Cert Second");

    const QString userCert1("My User Cert");
    const QString userCert2("My User Cert Second");
    
    QList<EapQtCertificateInfo> caCerts;
    QList<EapQtCertificateInfo> userCerts;
    
    mWlanQtUtilsContext->setCreateWlanIapResult(3);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(3);
    mWlanQtUtilsContext->setSignalIctResult(3, WlanQtUtils::IctPassed);
    
    mEapQtUtilsContext->setConfigurationReference(3, true);
    
    EapQtCertificateInfo cert;
    cert.setValue(EapQtCertificateInfo::CertificateLabel, caCert1);
    caCerts.append(cert);
    
    cert.setValue(EapQtCertificateInfo::CertificateLabel, userCert1);
    userCerts.append(cert);
    
    mEapQtUtilsContext->createEapTls(
        caCerts,
        0,
        userCerts,
        0);
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa2, 
        false,
        false);

    mView->showWizard();
    verify_outertype_select_first(ButtonDisabled);
    verify_ca_cert_page_select_and_click_next(true);
    verify_user_cert_page_select_and_click_next(ButtonEnabled, 0);
    verify_identity_page(eapTypeToString(EapQtPluginHandle::PluginEapTls));

    // backwards
    QCOMPARE(mouseClickPrevious(), true);
    QTest::qWait(WaitTimeForUi);
    verify_user_cert_page(ButtonEnabled, userCert1, 0 );

    QCOMPARE(mouseClickPrevious(), true);
    QTest::qWait(WaitTimeForUi);
    verify_ca_cert_page();

    QCOMPARE(mouseClickPrevious(), true);
    QTest::qWait(WaitTimeForUi);
    verify_outertype(ButtonEnabled);

    cert.setValue(EapQtCertificateInfo::CertificateLabel, caCert2);
    caCerts.append(cert);
    mEapQtUtilsContext->setCertsCa(caCerts);
    
    cert.setValue(EapQtCertificateInfo::CertificateLabel, userCert2);
    userCerts.append(cert);
    mEapQtUtilsContext->setCertsUser(userCerts);

    mEapQtUtilsContext->createEapIdentifyValidators(EapQtPluginHandle::PluginEapTls);
    
    // Forward
    QCOMPARE(mouseClickNext(), true);
    verify_ca_cert_page_select_and_click_next(false);
    verify_user_cert_page(ButtonDisabled);

    // backwards
    QCOMPARE(mouseClickPrevious(), true);
    QTest::qWait(WaitTimeForUi);
    verify_ca_cert_page(caCert1, 1);

    QCOMPARE(mouseClickPrevious(), true);
    QTest::qWait(WaitTimeForUi);
    verify_outertype(ButtonEnabled);

    QCOMPARE(mouseClickNext(), true);
    verify_ca_cert_page(caCert1, 1);

    QCOMPARE(mouseClickNext(), true);
    verify_user_cert_page_select_and_click_next(ButtonDisabled, 0);

    verify_identity_and_click_next(eapTypeToString(EapQtPluginHandle::PluginEapTls));        

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPage::PageProcessSettings,ssid), true );
    QCOMPARE( verifyActionButtons(ButtonDisabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );

    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonHidden, ButtonEnabled), true );
    
    QCOMPARE( verifySummaryPage(
        ssid,
        CMManagerShim::Infra,
        false, 
        CMManagerShim::WlanSecModeWpa2, 
        false,
        hbTrId("txt_occ_dblist_destination_val_internet"),
        EapQtPluginHandle::PluginEapTls),true);
    
#endif 
}

void TestWlanWizardEap::tc_eap_sim_finish()
{
#ifdef tc_eap_sim_finish_enabled
    tc_eap_aka_sim(EapQtPluginHandle::PluginEapAka);
    
    QCOMPARE( mouseClickFinish(), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyStatus(WizardStatusSignalFinished, 3), true);
#endif
}

void TestWlanWizardEap::tc_eap_sim_cancel()
{
#ifdef tc_eap_sim_cancel_enabled
    tc_eap_aka_sim(EapQtPluginHandle::PluginEapAka);
    
    QCOMPARE( mouseClickCancel(), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE(verifyStatus(WizardStatusSignalCancelled), true);
#endif
}

void TestWlanWizardEap::tc_eap_fast_confirmed()
{
#ifdef tc_eap_fast_confirmed_enabled
    tc_eap_fast(
        EapQtPacStoreConfig::PacStoreStatePasswordStored,
        true);
#endif
}

void TestWlanWizardEap::tc_eap_fast_new_store()
{
#ifdef tc_eap_fast_new_store_enabled
    tc_eap_fast(
        EapQtPacStoreConfig::PacStoreStateStoreNotExists,
        true);
#endif
}

void TestWlanWizardEap::tc_eap_fast_new_store_save_fails()
{
#ifdef tc_eap_fast_new_store_save_fails_enabled
    tc_eap_fast(
        EapQtPacStoreConfig::PacStoreStateStoreNotExists,
        false);
#endif
}

void TestWlanWizardEap::tc_eap_fast_new_store_and_validators()
{
#ifdef tc_eap_fast_new_store_and_validators_enabled
    const QString ssid("tc_eap_fast_new_store_and_validators");
    const QString username("user");

    mWlanQtUtilsContext->setCreateWlanIapResult(4);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(4);
    mWlanQtUtilsContext->setSignalIctResult(4, WlanQtUtils::IctPassed);

    mEapQtUtilsContext->setConfigurationReference(4, true);
    mEapQtUtilsContext->createEapFast(EapQtPacStoreConfig::PacStoreStateStoreNotExists);
    mEapQtUtilsContext->createInner(
        EapQtPluginHandle::PluginEapFast,
        EapQtPluginHandle::PluginEapMschapv2,
        "u",
        "p");
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa, 
        false,
        false);

    mView->showWizard();
    EapQtValidatorStub *validator = (EapQtValidatorStub*)mEapQtUtilsContext->validatorPacStore(
        EapQtPacStoreConfig::PacStorePassword);
    validator->setReturnValue(EapQtValidator::StatusInvalid);
    
    verify_outertype_select_first(ButtonDisabled);
    verify_new_pac_store_password_page(ButtonDisabled);
    
    QCOMPARE( mouseClickObject("lineEditUsername"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditUsername", username), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    
    validator->setReturnValue(EapQtValidator::StatusOk);
    
    QCOMPARE( mouseClickObject("lineEditPassword"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditPassword", username), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    
    QCOMPARE( mouseClickNext(), true );
    verify_username_password(EapQtPluginHandle::PluginEapMschapv2);

#endif
}


void TestWlanWizardEap::tc_eap_fast_new_store_back_and_forwards()
{
#ifdef tc_eap_fast_new_store_back_and_forwards_enabled
    const QString ssid("tc_eap_fast_new_store_back_and_forwards");
    const QString username("u");
    const QString password("p");

    mWlanQtUtilsContext->setCreateWlanIapResult(4);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(4);
    mWlanQtUtilsContext->setSignalIctResult(4, WlanQtUtils::IctPassed);

    mEapQtUtilsContext->setConfigurationReference(4, true);
    mEapQtUtilsContext->createEapFast(EapQtPacStoreConfig::PacStoreStateStoreNotExists);
    mEapQtUtilsContext->createInner(
        EapQtPluginHandle::PluginEapFast,
        EapQtPluginHandle::PluginEapMschapv2,
        username,
        password);
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa, 
        false,
        false);

    mView->showWizard();
    
    verify_outertype_select_first(ButtonDisabled);
    verify_new_pac_store_password_page(ButtonEnabled);
    QCOMPARE( mouseClickNext(), true );
    verify_username_password(EapQtPluginHandle::PluginEapMschapv2);
    
    // backwards
    QCOMPARE( mouseClickPrevious(), true );
    verify_new_pac_store_password_page(ButtonEnabled);
    QCOMPARE( mouseClickPrevious(), true );
    
    mEapQtUtilsContext->createInnerTypeValidators(EapQtPluginHandle::PluginEapMschapv2);
    
    // forwards
    verify_outertype_select_first(ButtonEnabled);
    verify_new_pac_store_password_page(ButtonEnabled);
    QCOMPARE( mouseClickNext(), true );
    verify_username_password(EapQtPluginHandle::PluginEapMschapv2);
#endif
}

void TestWlanWizardEap::tc_eap_fast_new_store_read_config_fail()
{
#ifdef tc_eap_fast_new_store_read_config_fail_enabled
    tc_eap_fast(
        EapQtPacStoreConfig::PacStoreStateStoreNotExists,
        true,
        false);
#endif
}

void TestWlanWizardEap::tc_eap_fast_confirm_store()
{
#ifdef tc_eap_fast_confirm_store_enabled
    tc_eap_fast(
        EapQtPacStoreConfig::PacStoreStatePasswordRequired,
        true);
#endif
}

void TestWlanWizardEap::tc_eap_fast_confirm_store_and_validators()
{
#ifdef tc_eap_fast_confirm_store_and_validators_enabled
    const QString ssid("tc_eap_fast_confirm_store_and_validators");
    const QString username_invalid("invalid");
    const QString username_valid("valid");

    mWlanQtUtilsContext->setCreateWlanIapResult(4);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(4);
    mWlanQtUtilsContext->setSignalIctResult(4, WlanQtUtils::IctPassed);

    mEapQtUtilsContext->setConfigurationReference(4, true);
    mEapQtUtilsContext->createEapFast(EapQtPacStoreConfig::PacStoreStatePasswordRequired);
    mEapQtUtilsContext->createInner(
        EapQtPluginHandle::PluginEapFast,
        EapQtPluginHandle::PluginEapMschapv2,
        "u",
        "p");
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa, 
        false,
        false);

    mView->showWizard();
    EapQtValidatorStub *validator = (EapQtValidatorStub*)mEapQtUtilsContext->validatorPacStore(
        EapQtPacStoreConfig::PacStorePasswordConfirmation);
    validator->setReturnValue(EapQtValidator::StatusInvalid);
    
    verify_outertype_select_first(ButtonDisabled);
    verify_confirm_pac_store_password_page(ButtonEnabled);
    
    QCOMPARE( mouseClickObject("lineEditPasswordConfirm"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditPasswordConfirm", username_invalid), true );
    QCOMPARE( mouseClickObject("labelError"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    
    QCOMPARE( mouseClickNext(), true );
    verify_confirm_pac_store_password_page(ButtonEnabled);
    QCOMPARE( verifyDialogText("labelError", hbTrId("txt_occ_info_incorrect_password")), true);


    validator->setReturnValue(EapQtValidator::StatusOk);
    
    QCOMPARE( mouseClickObject("lineEditPasswordConfirm"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditPasswordConfirm", username_valid), true );
    QCOMPARE( mouseClickObject("labelError"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    QTest::qWait(WaitTimeForUi);
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyDialogText("labelError", hbTrId("txt_occ_info_incorrect_password")), false);
    
    QCOMPARE( mouseClickNext(), true );
    verify_username_password(EapQtPluginHandle::PluginEapMschapv2);

#endif
}

void TestWlanWizardEap::tc_eap_fast_confirm_store_back_and_forwards()
{
#ifdef tc_eap_fast_confirm_store_back_and_forwards_enabled
    const QString ssid("tc_eap_fast_confirm_store_back_and_forwards");
    const QString username("u");
    const QString password("p");

    mWlanQtUtilsContext->setCreateWlanIapResult(4);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(4);
    mWlanQtUtilsContext->setSignalIctResult(4, WlanQtUtils::IctPassed);

    mEapQtUtilsContext->setConfigurationReference(4, true);
    mEapQtUtilsContext->createEapFast(EapQtPacStoreConfig::PacStoreStatePasswordRequired);
    mEapQtUtilsContext->createInner(
        EapQtPluginHandle::PluginEapFast,
        EapQtPluginHandle::PluginEapMschapv2,
        username,
        password);
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa, 
        false,
        false);

    mView->showWizard();
    
    verify_outertype_select_first(ButtonDisabled);
    verify_confirm_pac_store_password_page(ButtonEnabled);
    QCOMPARE( mouseClickNext(), true );
    verify_username_password(EapQtPluginHandle::PluginEapMschapv2);
    
    // backwards
    QCOMPARE( mouseClickPrevious(), true );
    verify_confirm_pac_store_password_page(ButtonEnabled);
    QCOMPARE( mouseClickPrevious(), true );
    
    mEapQtUtilsContext->createInnerTypeValidators(EapQtPluginHandle::PluginEapMschapv2);
    
    // forwards
    verify_outertype_select_first(ButtonEnabled);
    verify_confirm_pac_store_password_page(ButtonEnabled);
    QCOMPARE( mouseClickNext(), true );
    verify_username_password(EapQtPluginHandle::PluginEapMschapv2);
#endif
}


// ---------------------------------------------------------
// HELPPER VERIFICATORS & NAVIGATORS
// ---------------------------------------------------------

void TestWlanWizardEap::verify_outertype_select_first(
    TestWlanWizardContext::ButtonStatus nextButtonStatus)
{
    verify_outertype(nextButtonStatus);
    
    QCOMPARE( selectRadioButton( "list", 0 ), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    QCOMPARE( mouseClickNext(), true );
}

void TestWlanWizardEap::verify_outertype(
    TestWlanWizardContext::ButtonStatus nextButtonStatus)
{
    QCOMPARE( verifyCurrentPage(EapWizardPage::PageOuterType), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, nextButtonStatus, ButtonHidden), true );
}

void TestWlanWizardEap::verify_innertype_select_first(
    int outerType,
    TestWlanWizardContext::ButtonStatus nextButtonStatus)
{
    verify_innertype(outerType, nextButtonStatus);
    
    QCOMPARE( selectRadioButton( "list", 0 ), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    QCOMPARE( mouseClickNext(), true );
}

void TestWlanWizardEap::verify_innertype(
    int outerType, 
    TestWlanWizardContext::ButtonStatus nextButtonStatus)
{
    int pageId;
    if (outerType == EapQtPluginHandle::PluginPeap){
        pageId = EapWizardPage::PageInnerTypePeap;
        
    } else {
        Q_ASSERT(outerType == EapQtPluginHandle::PluginEapTtls);
        pageId = EapWizardPage::PageInnerTypeEapTtls;
    }
    QCOMPARE( verifyCurrentPageWithInfo(pageId, eapTypeToString(outerType)), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, nextButtonStatus, ButtonHidden), true );
}

void TestWlanWizardEap::verify_username_password_click_next(
    int type, 
    const QString &username, 
    const QString &password)
{
    verify_username_password(type);
    
    QCOMPARE( mouseClickObject("lineEditUsername"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditUsername", username), true );
    QTest::qWait(WaitTimeForUi);

    QCOMPARE( mouseClickObject("lineEditPassword"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditPassword", password), true );
    QTest::qWait(WaitTimeForUi);
    
    QCOMPARE( mouseClickObject("setlabel_56"), true );
    QCOMPARE( mouseClickNext(), true );
}

void TestWlanWizardEap::verify_username_password(int type)
{
    QCOMPARE( verifyCurrentPageWithInfo(EapWizardPage::PageUsernamePassword, eapTypeToString(type)), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
}

void TestWlanWizardEap::verify_ca_cert_page_select_and_click_next(bool automatic)
{
    verify_ca_cert_page();
    
    int index = 0;
    if (automatic == false) {
        index = 1;
    }
    QCOMPARE( selectRadioButton( "list", index ), true );
    QTest::qWait(WaitTimeForUi);
    
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    QCOMPARE( mouseClickNext(), true );
}


void TestWlanWizardEap::verify_ca_cert_page()
{
    QCOMPARE( verifyCurrentPage(EapWizardPage::PageCertificateCa), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
 
    QCOMPARE( verifyRadioButtonState(
        "list", 
        0, 
        hbTrId("txt_occ_setlabel_val_select_automatically")), true);
}

void TestWlanWizardEap::verify_ca_cert_page(const QString &text, int index)
{
    QCOMPARE( verifyCurrentPage(EapWizardPage::PageCertificateCa), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    
    QCOMPARE( verifyRadioButtonState("list", index, text), true);
}

void TestWlanWizardEap::verify_user_cert_page_select_and_click_next(
    ButtonStatus nextButtonStatus,
    int index)
{
    verify_user_cert_page(nextButtonStatus);

    if (index != -1){
        QCOMPARE( selectRadioButton( "list", index ), true );
        QTest::qWait(WaitTimeForUi);
    
        QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
        QCOMPARE( mouseClickNext(), true );
    } else {
        QCOMPARE( 
            verifyDialogText(
                "errorLabel",
                hbTrId("txt_occ_dialog_no_certificates_installed_wizard_c")), true);
        QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    }
}

void TestWlanWizardEap::verify_user_cert_page(ButtonStatus nextButtonStatus)
{
    QCOMPARE( verifyCurrentPage(EapWizardPage::PageCertificateUser), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, nextButtonStatus, ButtonHidden), true );
}


void TestWlanWizardEap::verify_user_cert_page(
    ButtonStatus nextButtonStatus,
    const QString &text,
    int index)
{
    QCOMPARE( verifyCurrentPage(EapWizardPage::PageCertificateUser), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, nextButtonStatus, ButtonHidden), true );
    
    QCOMPARE( verifyRadioButtonState("list", index, text), true);
}

void TestWlanWizardEap::verify_identity_modify_and_click_next(
    const QString &outerType,
    bool usernameAutomatic,
    const QString &username,
    bool realmAutomatic,
    const QString &realm)
{
    QCOMPARE( verifyCurrentPageWithInfo(EapWizardPage::PageIdentity, outerType), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    QTest::qWait(WaitTimeForUi);
    
    QCOMPARE(verifyCheckBoxState("setlabel_53_val", true), true);
    QCOMPARE(verifyCheckBoxState("setlabel_54_val", true), true);
    
    QCOMPARE(selectCheckBoxState("setlabel_53_val", usernameAutomatic), true);
    QCOMPARE(selectCheckBoxState("setlabel_54_val", realmAutomatic), true);
        
    QTest::qWait(WaitTimeForUi);
    
    QCOMPARE(verifyCheckBoxState("setlabel_53_val", usernameAutomatic), true);
    QCOMPARE(verifyCheckBoxState("setlabel_54_val", realmAutomatic), true);
    
    QCOMPARE( mouseClickObject("lineEditUsername"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditUsername", username), true );
    QTest::qWait(WaitTimeForUi);

    QCOMPARE( mouseClickObject("lineEditRealm"), true );
    QTest::qWait(WaitTimeForUi);
    QCOMPARE( insertTextToObject("lineEditRealm", realm), true );
    QTest::qWait(WaitTimeForUi);
    
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
    QTest::qWait(WaitTimeForUi);
    
    QCOMPARE( mouseClickObject("setlabel_53"), true );
    QCOMPARE( mouseClickNext(), true );
}

void TestWlanWizardEap::verify_identity_and_click_next(const QString &outerType)
{
    verify_identity_page(outerType);
    
    QCOMPARE(verifyCheckBoxState("setlabel_53_val", true), true);
    QCOMPARE(verifyCheckBoxState("setlabel_54_val", true), true);
    
    QCOMPARE( mouseClickObject("setlabel_53"), true );
    QCOMPARE( mouseClickNext(), true );
}

void TestWlanWizardEap::verify_identity_page(const QString &outerType)
{
    QCOMPARE( verifyCurrentPageWithInfo(EapWizardPage::PageIdentity, outerType), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonEnabled, ButtonHidden), true );
}


void TestWlanWizardEap::verify_new_pac_store_password_page(
    ButtonStatus nextButtonStatus )
{
    QCOMPARE( verifyCurrentPage(EapWizardPage::PageNewPacStorePassword), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, nextButtonStatus, ButtonHidden), true );
}


void TestWlanWizardEap::verify_confirm_pac_store_password_page(
    ButtonStatus nextButtonStatus)
{
    QCOMPARE( verifyCurrentPage(EapWizardPage::PagePromptPacStorePassword), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, nextButtonStatus, ButtonHidden), true );
}

// ---------------------------------------------------------
// SUBTEST CASES
// ---------------------------------------------------------


void TestWlanWizardEap::tc_eap_aka_sim(int type)
{
    // TODO: verify IAP settings
    // TODO: verify call stack
    
    mWlanQtUtilsContext->setCreateWlanIapResult(3);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(3);
    mWlanQtUtilsContext->setSignalIctResult(3, WlanQtUtils::IctPassed);
    
    mEapQtUtilsContext->setConfigurationReference(3, true);
    mEapQtUtilsContext->createEapAkaSim(type);
    
    const QString ssid("tc_eap_aka_sim");
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecMode802_1x, 
        false,
        false);

    mView->showWizard();
    
    verify_outertype_select_first(ButtonDisabled);

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPage::PageProcessSettings,ssid), true );
    QCOMPARE( verifyActionButtons(ButtonDisabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );

    QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
    QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonHidden, ButtonEnabled), true );
    
    QCOMPARE( verifySummaryPage(
        ssid,
        CMManagerShim::Infra,
        false, 
        CMManagerShim::WlanSecMode802_1x, 
        true,
        hbTrId("txt_occ_dblist_destination_val_internet"),
        type),true);
}

void TestWlanWizardEap::tc_eap_peap_ttls(
    int outerType, 
    int innerType,
    int errorCode,
    const QString &errorString,
    bool saveInnerConfReturn )
{
    // TODO: verify IAP settings
    // TODO: verify call stack
    const QString ssid("tc_eap_peap_ttls");
    const QString username("u");
    const QString password("p");

    mWlanQtUtilsContext->setCreateWlanIapResult(3);
    
    if (errorCode == KErrNone) {
        mWlanQtUtilsContext->setConnectionSuccessed(true);
        mWlanQtUtilsContext->setSignalWlanNetworkOpened(3);
        mWlanQtUtilsContext->setSignalIctResult(3, WlanQtUtils::IctPassed);
    } else {
        mWlanQtUtilsContext->setConnectionSuccessed(false);
        mWlanQtUtilsContext->setSignalWlanNetworkClosed(3, errorCode);
    }
    mEapQtUtilsContext->setConfigurationReference(3, true);
    mEapQtUtilsContext->createEapTtlsPeap(
        (EapQtPluginHandle::Plugin)outerType,
        true,
        true,
        QString(), //usernameIdentity,
        true,
        QString(), //realm,
        (EapQtPluginHandle::Plugin)innerType );

    mEapQtUtilsContext->createInner(
        (EapQtPluginHandle::Plugin)outerType,
        (EapQtPluginHandle::Plugin)innerType,
        username,
        password);
    
    mEapQtUtilsContext->setInnerConfigReturn(saveInnerConfReturn);
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa, 
        false,
        false);

    mView->showWizard();
    
    verify_outertype_select_first(ButtonDisabled);
    verify_ca_cert_page_select_and_click_next(true);
    verify_identity_and_click_next(eapTypeToString((EapQtPluginHandle::Plugin)outerType));
    verify_innertype_select_first(
        (EapQtPluginHandle::Plugin)outerType,
        ButtonDisabled);
    
    verify_username_password_click_next(innerType, username, password);
    
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPage::PageProcessSettings,ssid), true );
    QCOMPARE( verifyActionButtons(ButtonDisabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );

    if (errorCode == KErrNone){
        QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
        QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonHidden, ButtonEnabled), true );
        
        QCOMPARE( verifySummaryPage(
            ssid,
            CMManagerShim::Infra,
            false, 
            CMManagerShim::WlanSecModeWpa, 
            false,
            hbTrId("txt_occ_dblist_destination_val_internet"),
            (EapQtPluginHandle::Plugin)outerType,
            (EapQtPluginHandle::Plugin)innerType),true);
        
    } else {
        QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageGenericError, errorString), true );
        QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
    }
}

void TestWlanWizardEap::tc_eap_aka_connect_error(
    int errorCode, 
    const QString &errorString)
{
    // TODO: verify IAP settings
    // TODO: verify call stack
    
    const QString ssid(errorString);
    
    mWlanQtUtilsContext->setCreateWlanIapResult(3);
    mWlanQtUtilsContext->setConnectionSuccessed(false);
    mWlanQtUtilsContext->setSignalWlanNetworkClosed(3, errorCode);
    
    mEapQtUtilsContext->setConfigurationReference(3, true);
    mEapQtUtilsContext->createEapAkaSim(EapQtPluginHandle::PluginEapAka);
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa2, 
        false,
        false);

    mView->showWizard();
    
    verify_outertype_select_first(ButtonDisabled);
    
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageProcessSettings, ssid), true );
    QCOMPARE( verifyActionButtons(ButtonDisabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );

    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPageInternal::PageGenericError, errorString), true );
    QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );
}


void TestWlanWizardEap::tc_eap_fast(
    int pacStoreState, 
    bool savePacStoreReturn,
    bool readPacStoreReturn)
{
    // TODO: verify IAP settings
    // TODO: verify call stack
    
    const QString ssid("tc_eap_fast");
    const QString username("u");
    const QString password("p");

    mWlanQtUtilsContext->setCreateWlanIapResult(4);
    mWlanQtUtilsContext->setConnectionSuccessed(true);
    mWlanQtUtilsContext->setSignalWlanNetworkOpened(4);
    mWlanQtUtilsContext->setSignalIctResult(4, WlanQtUtils::IctPassed);

    mEapQtUtilsContext->setConfigurationReference(4, true);
    mEapQtUtilsContext->createEapFast(pacStoreState);
    mEapQtUtilsContext->setPacStoreConfigSaveReturn(savePacStoreReturn);
    mEapQtUtilsContext->setPacStoreConfigReadReturn(readPacStoreReturn);
    
    if (!readPacStoreReturn) {
        EapQtPacStoreConfig pacStoreConfig;
        pacStoreConfig.setValue(
            EapQtPacStoreConfig::PacStoreReset, 
            true);
        
        mEapQtUtilsContext->setPacStoreConfigSave(pacStoreConfig, true);
    }
    
    mEapQtUtilsContext->createInner(
        EapQtPluginHandle::PluginEapFast,
        EapQtPluginHandle::PluginEapMschapv2,
        username,
        password);
    
    mView->mWizard->setParameters(
        ssid,
        ssid.toUtf8(),
        CMManagerShim::Infra,
        CMManagerShim::WlanSecModeWpa, 
        false,
        false);

    mView->showWizard();
    
    verify_outertype_select_first(ButtonDisabled);
    
    if (!readPacStoreReturn) {
        EapQtPacStoreConfig pacStoreConfig;
        
        pacStoreConfig.setValue(
            EapQtPacStoreConfig::PacStorePassword, 
            "");
        
        pacStoreConfig.setValue(
            EapQtPacStoreConfig::PacStoreSavePassword, 
            true);

        
        mEapQtUtilsContext->setPacStoreConfigSave(pacStoreConfig, true);
    }
    
    switch (pacStoreState){
    case EapQtPacStoreConfig::PacStoreStateStoreNotExists:
        verify_new_pac_store_password_page(ButtonEnabled);
        QCOMPARE( mouseClickNext(), true );
        break;
        
    case EapQtPacStoreConfig::PacStoreStatePasswordRequired:
        verify_confirm_pac_store_password_page(ButtonEnabled);
        QCOMPARE( mouseClickNext(), true );
        break;
    }

    verify_username_password_click_next(EapQtPluginHandle::PluginEapMschapv2,username,password);
    
    QCOMPARE( verifyCurrentPageWithInfo(WlanWizardPage::PageProcessSettings,ssid), true );
    QCOMPARE( verifyActionButtons(ButtonDisabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );

    if (savePacStoreReturn) {
        
        QCOMPARE( verifyCurrentPage(WlanWizardPageInternal::PageSummary, 10, 500), true );
        QCOMPARE( verifyActionButtons(ButtonHidden, ButtonEnabled, ButtonHidden, ButtonEnabled), true );
        
        QCOMPARE( verifySummaryPage(
            ssid,
            CMManagerShim::Infra,
            false, 
            CMManagerShim::WlanSecModeWpa, 
            false,
            hbTrId("txt_occ_dblist_destination_val_internet"),
            EapQtPluginHandle::PluginEapFast,
            EapQtPluginHandle::PluginEapMschapv2,
            true) ,true);
    } else {
        QCOMPARE( 
            verifyCurrentPageWithInfo(
                WlanWizardPageInternal::PageGenericError, 
                hbTrId("txt_occ_dialog_unable_to_save_settings_please_ret")), true );
        QCOMPARE( verifyActionButtons(ButtonEnabled, ButtonEnabled, ButtonDisabled, ButtonHidden), true );        
    }
}
