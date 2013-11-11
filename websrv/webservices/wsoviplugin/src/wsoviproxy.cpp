/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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








#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "wsoviplugin.h"
#include "wsovivalidatehandler.h"
#include "wsoviregisterhandler.h"
#include "wsovidevicehandler.h"
#include "wsovisecurityhandler.h"
#include "wsovicredentialcollectorhandler.h"
#include "wsoviencoderhandler.h"
#include "wsoviserviceupdatehandler.h"
          
const TInt32 KWSOviPlugin = 0x2001950F;
const TInt32 KWSOviValidateHandler = 0x20019510;
const TInt32 KWSOviRegisterHandler  = 0x20019511;
const TInt32 KWSOviDeviceHandler  = 0x20019512;
const TInt32 KWSOviSecurityHandler  = 0x20019513;
const TInt32 KWSOviCredentialCollectorHandler  = 0x20019514;
const TInt32 KWSOviEncoderHandler  = 0x20019515;
const TInt32 KWSOviServiceUpdateHandler  = 0x20019516;

// ---------------------------------------------------------------------------
// Provides a key value pair table, this is used to identify
// the correct construction function for the requested interface.
// ---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KWSOviPlugin,  CWSOviPlugin::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KWSOviValidateHandler,  CWSOviValidateHandler::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KWSOviRegisterHandler,  CWSOviRegisterHandler::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KWSOviDeviceHandler,  CWSOviDeviceHandler::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KWSOviSecurityHandler,  CWSOviSecurityHandler::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KWSOviCredentialCollectorHandler,  CWSOviCredentialCollectorHandler::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KWSOviEncoderHandler,  CWSOviEncoderHandler::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KWSOviServiceUpdateHandler,  CWSOviServiceUpdateHandler::NewL)
    };

// ---------------------------------------------------------------------------
// Function used to return an instance of the proxy table.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
                                                            TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

