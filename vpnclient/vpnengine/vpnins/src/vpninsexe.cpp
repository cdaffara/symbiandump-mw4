/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: VPN Installer.
*
*/



#include <e32base.h>
#include <certificateapps.h>

LOCAL_C void setSettingsL();

// The starting point
GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TRAPD(error, setSettingsL());
    if (error != KErrNone)
        {
        ;
        }
    delete cleanup;
    __UHEAP_MARKEND;
    return 0;
    }

 
LOCAL_C void setSettingsL()
    {
    CCertificateAppInfoManager* manager = CCertificateAppInfoManager::NewLC();
    
    const TUid KUidVpn = {0x101F7993};
    _LIT(KVpnApplicationName, "VPN");
    TName vpnApplicationName(KVpnApplicationName);
    TCertificateAppInfo client(KUidVpn, vpnApplicationName);
    
    manager->AddL(client);
    CleanupStack::PopAndDestroy(); // manager
    }
