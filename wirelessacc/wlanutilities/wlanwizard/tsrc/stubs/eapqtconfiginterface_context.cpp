/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 */
#include <QTest>

#include "eapqtconfiginterface_context.h"
#include "eapqtvalidator_stub.h"
#include "eapqtexpandedeaptype.h"

EapQtConfigInterfaceContext::EapQtConfigInterfaceContext() : 
    mEapQtConfig(NULL)
{
}

EapQtConfigInterfaceContext::~EapQtConfigInterfaceContext()
{
    mEapQtConfig->mValidator.clear();
}

void EapQtConfigInterfaceContext::setObject(EapQtConfigInterface *eapQtConfig)
{
    mEapQtConfig = eapQtConfig;
}

void EapQtConfigInterfaceContext::createEapAkaSim(int type)
{
    QList<EapQtPluginHandle> types;
    types.append((EapQtPluginHandle::Plugin)type);
    setSupportedOuterTypes(types);

    setSelectectedOuterTypes(types, true);
    
    EapQtConfig config;
    config.setValue(EapQtConfig::UsernameAutomatic, true);
    config.setValue(EapQtConfig::RealmAutomatic, true);
    setOuterConfig((EapQtPluginHandle::Plugin)type, config, true);
}

void EapQtConfigInterfaceContext::createLeap(
    const QString& username, 
    const QString& passwd)
{
    QList<EapQtPluginHandle> types;
    types.append(EapQtPluginHandle::PluginLeap);
    setSupportedOuterTypes(types);

    setSelectectedOuterTypes(types, true);
    
    EapQtConfig config;
    config.setValue(EapQtConfig::UsernameAutomatic, false);
    config.setValue(EapQtConfig::Username, username);
    config.setValue(EapQtConfig::PasswordPrompt, false);
    config.setValue(EapQtConfig::Password, passwd);
    setOuterConfig(EapQtPluginHandle::PluginLeap, config, true);
    
    // validators
    createInnerTypeValidators(EapQtPluginHandle::PluginLeap);
}

void EapQtConfigInterfaceContext::createEapIdentifyValidators(EapQtPluginHandle::Plugin outerType)
{
    EapQtPluginHandle typeHandle(outerType);
    
    EapQtValidatorStub *valStub = new EapQtValidatorStub(EapQtValidator::StatusOk);
    setValidator(typeHandle.type().type(), EapQtConfig::Username, valStub);
    
    valStub = new EapQtValidatorStub(EapQtValidator::StatusOk);
    setValidator(typeHandle.type().type(), EapQtConfig::Realm, valStub);
}

void EapQtConfigInterfaceContext::createEapTtlsPeap(
    EapQtPluginHandle::Plugin type,
    bool automaticCert,
    bool usernameAutomatic, 
    const QString &username,
    bool realmAutomatic, 
    const QString &realm,
    EapQtPluginHandle::Plugin innerType)
{
    EapQtPluginHandle typeHandle(type);
    
    QList<EapQtPluginHandle> types;
    types.append(typeHandle);
    setSupportedOuterTypes(types);
    setSelectectedOuterTypes(types, true);
    
    // Certs
    QList<EapQtCertificateInfo> certs;
    EapQtCertificateInfo caCert;
    caCert.setValue(EapQtCertificateInfo::CertificateLabel, "My Super Cert");
    certs.append(caCert);
    setCertsCa(certs);
    
    // CREATE VALIDATOR
    createEapIdentifyValidators(type);
    
    // CREATE CONFIGURATION FOR TTLS/PEAP
    
    EapQtConfig config;
    config.setValue(EapQtConfig::UseIdentityPrivacy, false);
    config.setValue(EapQtConfig::VerifyServerRealm, false);
    config.setValue(EapQtConfig::ClientAuthenticationRequired, false);
    
    config.setValue(EapQtConfig::UsernameAutomatic, usernameAutomatic);
    config.setValue(EapQtConfig::Username, username);
    config.setValue(EapQtConfig::RealmAutomatic, realmAutomatic);
    config.setValue(EapQtConfig::Realm, realm);
    if (automaticCert){
        config.setValue(EapQtConfig::AuthorityCertificateAutomatic, true);
    }
    else {
        QList<QVariant> caCerts;
        caCerts.append(qVariantFromValue(caCert));
        config.setValue(EapQtConfig::AuthorityCertificate, caCerts);
        config.setValue(EapQtConfig::AuthorityCertificateAutomatic, false);
    }
    if (type == EapQtPluginHandle::PluginPeap){
        if (innerType == EapQtPluginHandle::PluginEapMschapv2){
            config.setValue(EapQtConfig::PeapVersion0Allowed, true);
            config.setValue(EapQtConfig::PeapVersion1Allowed, false);  
        }
        else {
            config.setValue(EapQtConfig::PeapVersion0Allowed, false);
            config.setValue(EapQtConfig::PeapVersion1Allowed, true);
        }
        config.setValue(EapQtConfig::PeapVersion2Allowed, false);
    }
    QList<QVariant> innerTypeList;
    EapQtPluginHandle inner(innerType);
    innerTypeList.append(qVariantFromValue(inner));
    config.setValue(EapQtConfig::InnerType, innerTypeList);
    
    setOuterConfig(type, config, true);
}

void EapQtConfigInterfaceContext::createEapTls(
    QList<EapQtCertificateInfo> &caCerts,
    int indexForCaCert,
    QList<EapQtCertificateInfo> &userCerts,
    int indexForUserCert)
{
    QList<EapQtPluginHandle> types;
    types.append(EapQtPluginHandle::PluginEapTls);
    setSupportedOuterTypes(types);
    setSelectectedOuterTypes(types, true);
    
    // Certs
    setCertsCa(caCerts);
    setCertsUser(userCerts);
    
    // CREATE VALIDATOR
    createEapIdentifyValidators(EapQtPluginHandle::PluginEapTls);
    
    // CREATE CONFIGURATION FOR TTLS/PEAP
    
    EapQtConfig config;
    config.setValue(EapQtConfig::UseIdentityPrivacy, false);
    config.setValue(EapQtConfig::VerifyServerRealm, false);
    config.setValue(EapQtConfig::ClientAuthenticationRequired, true);
    
    config.setValue(EapQtConfig::UsernameAutomatic, true);
    config.setValue(EapQtConfig::Username, QString());
    config.setValue(EapQtConfig::RealmAutomatic, true);
    config.setValue(EapQtConfig::Realm, QString());
    
    if (caCerts.length() == 0){
        config.setValue(EapQtConfig::AuthorityCertificateAutomatic, true);
    } else {
        QList<QVariant> caCertsVariant;
        caCertsVariant.append(qVariantFromValue(caCerts[indexForCaCert]));    
        config.setValue(EapQtConfig::AuthorityCertificate, caCertsVariant);
        config.setValue(EapQtConfig::AuthorityCertificateAutomatic, false);
    }
    
    if (indexForUserCert != -1) {
        QList<QVariant> certs;
        certs.append(qVariantFromValue(userCerts[indexForUserCert]));
        config.setValue(EapQtConfig::UserCertificate, certs);
    }
    
    setOuterConfig(EapQtPluginHandle::PluginEapTls, config, true);
}

void EapQtConfigInterfaceContext::createInner(
    EapQtPluginHandle::Plugin outerType,
    EapQtPluginHandle::Plugin innerType, 
    const QString &username,
    const QString &password)
{
    EapQtPluginHandle innerHandle(innerType);
    
    QList<EapQtPluginHandle> types;
    types.append(innerHandle);
    setSupportedInnerTypes(outerType, types);

    EapQtConfig config;
    EapQtPluginHandle outerTypeHandle(outerType);
    config.setValue(EapQtConfig::OuterType, qVariantFromValue(outerTypeHandle));
    config.setValue(EapQtConfig::UsernameAutomatic, false);
    config.setValue(EapQtConfig::Username, username);
    config.setValue(EapQtConfig::PasswordPrompt, false);
    config.setValue(EapQtConfig::Password, password);
    setInnerConfig(innerType, config, true);
    
    createInnerTypeValidators(innerType);
}

void EapQtConfigInterfaceContext::createInnerTypeValidators(EapQtPluginHandle::Plugin innerType)
{
    EapQtPluginHandle innerHandle(innerType);
    
    // validators
    EapQtValidatorStub *valStub = new EapQtValidatorStub(EapQtValidator::StatusOk);
    setValidator(innerHandle.type().type(), EapQtConfig::Username, valStub);
    
    valStub = new EapQtValidatorStub(EapQtValidator::StatusOk);
    setValidator(innerHandle.type().type(), EapQtConfig::Password, valStub);    
}

void EapQtConfigInterfaceContext::createEapFast(
    int pacState, const QString &password)
{
    QList<EapQtPluginHandle> types;
    types.append(EapQtPluginHandle::PluginEapFast);
    setSupportedOuterTypes(types);
    setSelectectedOuterTypes(types, true);

    EapQtPacStoreConfig pacStoreConfig;
    pacStoreConfig.setValue(EapQtPacStoreConfig::PacStoreState, pacState);
    setPacStoreConfigRead(pacStoreConfig, true);
    
    if (pacState == EapQtPacStoreConfig::PacStoreStateStoreNotExists ||
        pacState == EapQtPacStoreConfig::PacStoreStatePasswordRequired) {
        pacStoreConfig.clear();
        pacStoreConfig.setValue(
            EapQtPacStoreConfig::PacStorePassword, 
            password);
        
        pacStoreConfig.setValue(
            EapQtPacStoreConfig::PacStoreSavePassword, 
            true);
        
        setPacStoreConfigSave(pacStoreConfig, true);
    }
    
    if (pacState == EapQtPacStoreConfig::PacStoreStateStoreNotExists) {
        EapQtValidatorStub *validator = new EapQtValidatorStub(EapQtValidator::StatusOk);
        setValidatorPacStore(EapQtPacStoreConfig::PacStorePassword, validator);
    }

    if (pacState == EapQtPacStoreConfig::PacStoreStatePasswordRequired) {
        EapQtValidatorStub *validator = new EapQtValidatorStub(EapQtValidator::StatusOk);
        setValidatorPacStore(EapQtPacStoreConfig::PacStorePasswordConfirmation, validator);
    }
    
    EapQtConfig config;
    config.setValue(EapQtConfig::ProvisioningModeAuthenticated, true);
    config.setValue(EapQtConfig::ProvisioningModeUnauthenticated, true);
    config.setValue(EapQtConfig::VerifyServerRealm, false);
    config.setValue(EapQtConfig::UseIdentityPrivacy, false);
    
    QList<QVariant> innerTypeList;
    EapQtPluginHandle inner(EapQtPluginHandle::PluginEapMschapv2);
    innerTypeList.append(qVariantFromValue(inner));
    config.setValue(EapQtConfig::InnerType, innerTypeList);
    
    setOuterConfig(EapQtPluginHandle::PluginEapFast, config, true);
}

QStringList EapQtConfigInterfaceContext::calledMethods()
{
    QStringList methods = mEapQtConfig->mCalledMethods;
    mEapQtConfig->mCalledMethods.clear();
    return methods;
}

void EapQtConfigInterfaceContext::setConfigurationReference(
    int iapId, 
    bool retValue)
{
    mEapQtConfig->mIapId = iapId;
    setConfigurationReferenceReturn(retValue);
}

void EapQtConfigInterfaceContext::setConfigurationReferenceReturn(bool retValue)
{
    mEapQtConfig->mReturnSetConfigurationReference = retValue;    
}

void EapQtConfigInterfaceContext::setCertsCa(
    QList<EapQtCertificateInfo> &certs)
{
    mEapQtConfig->mCertsCa = certs;
}

void EapQtConfigInterfaceContext::setCertsUser(
    QList<EapQtCertificateInfo> &certs)
{
    mEapQtConfig->mCertsUser = certs;
}

void EapQtConfigInterfaceContext::setSupportedOuterTypes(
    QList<EapQtPluginHandle> &outerTypes)
{
    mEapQtConfig->mSupportedOuterTypes = outerTypes;
}

void EapQtConfigInterfaceContext::setSupportedInnerTypes(
    EapQtPluginHandle outerHandle, 
    QList<EapQtPluginHandle> &innerTypes)
{
    mEapQtConfig->mSupportedInnerTypes = innerTypes;
    mEapQtConfig->mSupportedInnerTypesOuterHandle = outerHandle.pluginId();
}

void EapQtConfigInterfaceContext::setSelectectedOuterTypes(
    QList<EapQtPluginHandle> &types,
    bool retValue)
{
    mEapQtConfig->mSetSelectectedOuterTypes = types;
    setSelectectedOuterTypesReturn(retValue);
}

void EapQtConfigInterfaceContext::setSelectectedOuterTypesReturn(bool retValue)
{
    mEapQtConfig->mReturnSetSelectedOuterTypes = retValue;
}

void EapQtConfigInterfaceContext::setOuterConfig(
    EapQtPluginHandle::Plugin handle,
    EapQtConfig &config, 
    bool retValue)
{
    mEapQtConfig->mOuterPlugin = handle;
    mEapQtConfig->mSaveConfigurationsOuter = config;
    setOuterConfigReturn(retValue);
}

void EapQtConfigInterfaceContext::setOuterConfigReturn(bool retValue)
{
    mEapQtConfig->mReturnSaveConfigurationOuter = retValue;
}

void EapQtConfigInterfaceContext::setInnerConfig(
    EapQtPluginHandle::Plugin handle,
    EapQtConfig &config, 
    bool retValue)
{
    mEapQtConfig->mInnerPlugin = handle;
    mEapQtConfig->mSaveConfigurationsInner = config;
    setInnerConfigReturn(retValue);
}

void EapQtConfigInterfaceContext::setInnerConfigReturn(bool retValue)
{
    mEapQtConfig->mReturnSaveConfigurationInner = retValue;
}

void EapQtConfigInterfaceContext::setValidator(
    EapQtExpandedEapType::Type eapType,
    EapQtConfig::SettingsId id, 
    EapQtValidator *validator)
{
    QCOMPARE(mEapQtConfig->mValidator[eapType][id] == NULL, true);
    mEapQtConfig->mValidator[eapType][id] = validator;
}


EapQtValidator *EapQtConfigInterfaceContext::validator(
    EapQtExpandedEapType::Type eapType,
    EapQtConfig::SettingsId id)
{
    return mEapQtConfig->mValidator[eapType][id];
}

void EapQtConfigInterfaceContext::setPacStoreConfigRead(
    EapQtPacStoreConfig &config,
    bool retValue)
{
    mEapQtConfig->mReadPacStoreConfiguration = config;
    setPacStoreConfigReadReturn(retValue);
}

void EapQtConfigInterfaceContext::setPacStoreConfigReadReturn(bool retValue)
{
    mEapQtConfig->mReadPacStoreConfigurationReturn = retValue;
}

void EapQtConfigInterfaceContext::setPacStoreConfigSave(
    EapQtPacStoreConfig &config,
    bool retValue)
{
    mEapQtConfig->mSavePacStoreConfiguration = config;
    setPacStoreConfigSaveReturn(retValue);
}

void EapQtConfigInterfaceContext::setPacStoreConfigSaveReturn(bool retValue)
{
    mEapQtConfig->mSavePacStoreConfigurationReturn = retValue;
}

void EapQtConfigInterfaceContext::setValidatorPacStore(int id, EapQtValidator * validator)
{
    mEapQtConfig->mValidatorPacStore[id] = validator;
}

EapQtValidator *EapQtConfigInterfaceContext::validatorPacStore(int id)
{
    return mEapQtConfig->mValidatorPacStore[id];
}
