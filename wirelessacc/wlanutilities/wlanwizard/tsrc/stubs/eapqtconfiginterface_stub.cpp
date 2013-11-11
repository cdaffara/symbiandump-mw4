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
 *   Stub for EAP method configuration QT interface
 *
 */

/*
 * %version: 7 %
 */

#include <QList>
#include <QDebug>
#include <QTest>
#include "eapqtconfiginterface.h"

//----------------------------------------------------------------------------
//              EapQtConfigInterface
//----------------------------------------------------------------------------

EapQtConfigInterface::EapQtConfigInterface(
    const EapBearerType bearerType, 
    const int iapId)
{
    mCalledMethods.append("EapQtConfigInterface");
    QCOMPARE(bearerType, EapQtConfigInterface::EapBearerTypeWlan);
    int tmp = EapQtConfigInterface::IapIdUndefined;
    QCOMPARE(iapId, tmp);
}

EapQtConfigInterface::~EapQtConfigInterface()
{
    mCalledMethods.append("~EapQtConfigInterface");
}


EapQtValidator *EapQtConfigInterface::validatorEap(
    const EapQtExpandedEapType &type,
    const EapQtConfig::SettingsId id)
{
    mCalledMethods.append("validatorEap");
    
    QCOMPARE(mValidator[type.type()][id] != NULL, true);
    EapQtValidator *validator = mValidator[type.type()][id];
    mValidator[type.type()].remove(id);
    return validator;
}

EapQtValidator *EapQtConfigInterface::validatorPacStore(
    const EapQtPacStoreConfig::PacStoreSettings id)
{
    mCalledMethods.append("validatorPacStore");
    
    QCOMPARE(mValidatorPacStore.contains(id), true);
    EapQtValidator *validator = mValidatorPacStore[id];
    mValidatorPacStore.remove(id);
    return validator;
}

bool EapQtConfigInterface::setConfigurationReference(const int iapId)
{
    mCalledMethods.append("setConfigurationReference");
    QCOMPARE(iapId, mIapId);
    
    return mReturnSetConfigurationReference;
}

QList<EapQtCertificateInfo> EapQtConfigInterface::certificateAuthorityCertificates()
{
    mCalledMethods.append("certificateAuthorityCertificates");
    return mCertsCa;
}

QList<EapQtCertificateInfo> EapQtConfigInterface::userCertificates()
{
    mCalledMethods.append("userCertificates");
    return mCertsUser;
}


bool EapQtConfigInterface::updateCertificates()
{
    mCalledMethods.append("updateCertificates");
    return true;
}

bool EapQtConfigInterface::isSupportedOuterType(const EapQtPluginHandle& handle)
{
    mCalledMethods.append("isSupportedOuterType");
    return mSupportedOuterTypes.contains(handle);
}

bool EapQtConfigInterface::isSupportedInnerType(
    const EapQtPluginHandle& outerHandle,
    const EapQtPluginHandle& innerHandle)
{
    mCalledMethods.append("isSupportedOuterType");
    QCOMPARE(mSupportedInnerTypesOuterHandle, (int)outerHandle.pluginId());
    return mSupportedInnerTypes.contains(innerHandle);
}

bool EapQtConfigInterface::setSelectedOuterTypes(
    const QList<EapQtPluginHandle>& outerHandles)
{
    mCalledMethods.append("setSelectedOuterTypes");
    QCOMPARE(mSetSelectectedOuterTypes, outerHandles);
    return mReturnSetSelectedOuterTypes;
}

bool EapQtConfigInterface::saveConfiguration(
    const EapQtPluginHandle& pluginInfo,
    const EapQtConfig &config)
{
    mCalledMethods.append("saveConfiguration");
    
    if (pluginInfo == mOuterPlugin) {
        QCOMPARE(config == mSaveConfigurationsOuter, true);
        return mReturnSaveConfigurationOuter;
    } else {
        QCOMPARE(pluginInfo == mInnerPlugin, true);
        QCOMPARE(config == mSaveConfigurationsInner, true);
        return mReturnSaveConfigurationInner;
    }
}

bool EapQtConfigInterface::deleteConfiguration()
{
    mCalledMethods.append("deleteConfiguration");
    return true;
}


bool EapQtConfigInterface::readPacStoreConfiguration(EapQtPacStoreConfig &config)
{
    config = mReadPacStoreConfiguration;
    return mReadPacStoreConfigurationReturn;
}

bool EapQtConfigInterface::savePacStoreConfiguration(EapQtPacStoreConfig &config)
{
    QCOMPARE(config == mSavePacStoreConfiguration, true);
    return mSavePacStoreConfigurationReturn;
}


