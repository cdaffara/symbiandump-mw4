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
* Description:  Cleans VPN data from device
*
*/

#include <e32base.h>
#include <f32file.h>

#include "vpncleaner.h"


LOCAL_C TBool vpnOnRom();


// The starting point
GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;

    CTrapCleanup* cleanup = CTrapCleanup::New();

    if( EFalse == vpnOnRom() )
        {
        TVpnCleaner vpnc;
        vpnc.Clean();
        }

    delete cleanup;

    __UHEAP_MARKEND;
    return KErrNone;
    }


LOCAL_C TBool vpnOnRom()
    {
    TBool ret = EFalse;

    TUint fileAttr;
    RFs fs;
    TInt err = fs.Connect();

    if ( KErrNone == err )
        {
        _LIT( KRomPath, "z:\\resource\\vpnlogmessages.rsc" );

        if ( KErrNone == fs.Att( KRomPath, fileAttr ) )
            {
            ret = ETrue;
            }
        }

    fs.Close();

    return ret;
    }


// End of File
