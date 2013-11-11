/*
 * Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "wlanmgmtclient_context.h"

WlanMgmtClientContext::WlanMgmtClientContext() : 
    mWlanMgmtClient(NULL)
{
}

WlanMgmtClientContext::~WlanMgmtClientContext()
{
}

void WlanMgmtClientContext::setObject(CWlanMgmtClient *wlanMgmtClient)
{
    mWlanMgmtClient = wlanMgmtClient;
}


void WlanMgmtClientContext::setRunProtectedSetup(
    const QString &ssid, 
    int completionCode,
    bool pushButtonMode )
{
    mWlanMgmtClient->iSsid.SetLength(0);
    mWlanMgmtClient->iSsid.Copy((unsigned char*)ssid.toAscii().constData()); 
    mWlanMgmtClient->iCompletionCode = completionCode;
    mWlanMgmtClient->iPushButtonMode = pushButtonMode ? ETrue : EFalse;
}

void WlanMgmtClientContext::appendResult(
    const QString &ssid,
    int securityMode,
    int operatingMode,
    int defaultWepKeyIndex,
    const QString &wepKey1,
    const QString &wepKey2,
    const QString &wepKey3,
    const QString &wepKey4,
    const QString &wpaKey)
{
    TWlanProtectedSetupCredentialAttribute attr;
    
    attr.iOperatingMode =(TWlanOperatingMode)operatingMode;
    attr.iSecurityMode = (TWlanIapSecurityMode)securityMode;
    
    attr.iSsid.Copy(
        (unsigned char*)ssid.toLatin1().constData(), 
        ssid.length());
    
    attr.iWepKey1.Copy(
        (unsigned char*)wepKey1.toLatin1().constData(), 
        wepKey1.length());
    
    attr.iWepKey2.Copy(
        (unsigned char*)wepKey2.toLatin1().constData(), 
        wepKey2.length());
    
    attr.iWepKey3.Copy(
        (unsigned char*)wepKey3.toLatin1().constData(), 
        wepKey3.length());
    
    attr.iWepKey4.Copy(
        (unsigned char*)wepKey4.toLatin1().constData(), 
        wepKey4.length());
    
    attr.iWepDefaultKey = (TWlanDefaultWepKey)defaultWepKeyIndex;
    
    attr.iWpaPreSharedKey.Copy(
        (unsigned char*)wpaKey.toLatin1().constData(), 
            wpaKey.length());

    mWlanMgmtClient->iResults->AppendL(attr);
}
