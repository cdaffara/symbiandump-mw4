/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/









// INCLUDE FILES
#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "wsstarenveloperhandler.h"
#include "wsstaraddressinghandler.h"
#include "wsstarcredentialcollectorhandler.h"
#include "wsstarsecurityhandler.h"
#include "wsstarpassporthandler.h"

#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES
    const TInt32 KWSStarEnveloperHandler = 0x101F971D;
    const TInt32 KWSStarPassportHandler = 0x101F971E;
    const TInt32 KWSStarAddressingHandler = 0x101F971F;
    const TInt32 KWSStarCredentialCollectorHandler = 0x101F9720;
    const TInt32 KWSStarSecurityHandler = 0x101F9721;
#else
    const TInt32 KWSStarEnveloperHandler = 0x10282C56;
    const TInt32 KWSStarPassportHandler = 0x10282C57;
    const TInt32 KWSStarAddressingHandler = 0x10282C58;
    const TInt32 KWSStarCredentialCollectorHandler = 0x10282C59;
    const TInt32 KWSStarSecurityHandler = 0x10282C5A;
#endif
// Provides a key value pair table, this is used to identify
// the correct construction function for the requested interface.
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KWSStarEnveloperHandler,  CWSStarEnveloperHandler::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KWSStarPassportHandler,  CWSStarPassportHandler::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KWSStarAddressingHandler,  CWSStarAddressingHandler::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KWSStarCredentialCollectorHandler,  CWSStarCredentialCollectorHandler::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KWSStarSecurityHandler,  CWSStarSecurityHandler::NewL)
    };

// Function used to return an instance of the proxy table.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
                                                            TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
