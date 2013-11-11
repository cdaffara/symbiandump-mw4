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

#ifndef WLANMGMTCLIENT_CONTEXT_H
#define WLANMGMTCLIENT_CONTEXT_H

#include "wlanmgmtclient.h"

class WlanMgmtClientContext
{
public:
    WlanMgmtClientContext();
    ~WlanMgmtClientContext();

    void setObject(CWlanMgmtClient *eapQtConfig);
public:
    
    void setRunProtectedSetup(
        const QString &ssid, 
        int completionCode,
        bool pushButtonMode );
    
    void appendResult(
        const QString &ssid,
        int securityMode,
        int operatingMode,
        int defaultWepKeyIndex,
        const QString &wepKey1 = QString(),
        const QString &wepKey2 = QString(),
        const QString &wepKey3 = QString(),
        const QString &wepKey4 = QString(),
        const QString &wpaKey  = QString());
    
private:
    CWlanMgmtClient* mWlanMgmtClient;
    
};

#endif /* WLANMGMTCLIENT_CONTEXT_H */

// End of File
