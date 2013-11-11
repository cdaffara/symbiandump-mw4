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











#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "wsstarplugin.h"
#include "wsstarvalidatehandler.h"
#include "wsstarregisterhandler.h"
#include "wsstarserviceupdatehandler.h"
#include "wsstarpolicyhandler.h"

//#ifdef RD_SEN_VTCP_SUPPORT
//#include "WSStarVtcpHandler.h"
//#endif

#ifdef RD_SEN_VTCP_SUPPORT
#include "wsstarcobrandinghandler.h"
#endif//RD_SEN_VTCP_SUPPORT

#ifdef RD_SEN_WS_STAR__WBXML
#include "WBXMLPlugin.h"
#endif

#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES
    const TInt32 KStarPlugin = 0x101F9718;
    const TInt32 KWSStarValidateHandler = 0x101F9719;
    const TInt32 KWSStarRegisterHandler = 0x101F971A;
    const TInt32 KWSStarServiceUpdateHandler = 0x101F971B;
    const TInt32 KWSStarPolicyHandler = 0x101F9796;
	#ifdef RD_SEN_VTCP_SUPPORT
    	//const TInt32 KWSStarVtcpHandler = 0x101F9722;
	    const TInt32 KWSStarCoBrandingHandler = 0x101F9798;
	#endif//RD_SEN_VTCP_SUPPORT
#else
    const TInt32 KStarPlugin = 0x10282C50;
    const TInt32 KWSStarValidateHandler = 0x10282C51;
    const TInt32 KWSStarRegisterHandler = 0x10282C52;
    const TInt32 KWSStarServiceUpdateHandler = 0x10282C53;
    const TInt32 KWSStarPolicyHandler = 0x10282C54;
	#ifdef RD_SEN_VTCP_SUPPORT
	    const TInt32 KWSStarVtcpHandler = 0x10282C55;
	    const TInt32 KWSStarCoBrandingHandler = 0x10282C6D;
	#endif//RD_SEN_VTCP_SUPPORT		
#endif
// ---------------------------------------------------------------------------
// Provides a key value pair table, this is used to identify
// the correct construction function for the requested interface.
// ---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KStarPlugin,  CWSStarPlugin::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KWSStarValidateHandler,  CWSStarValidateHandler::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KWSStarRegisterHandler,  CWSStarRegisterHandler::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KWSStarServiceUpdateHandler,  CWSStarServiceUpdateHandler::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KWSStarPolicyHandler,  CWSStarPolicyHandler::NewL),
#ifdef RD_SEN_VTCP_SUPPORT // wsstarvtcphandler plugin is moved from wsstarplugin to vtcptransportplugin
    //IMPLEMENTATION_PROXY_ENTRY(KWSStarVtcpHandler,  CWSStarVtcpHandler::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KWSStarCoBrandingHandler,  CWSStarCoBrandingHandler::NewL)
#endif//RD_SEN_VTCP_SUPPORT
#ifdef RD_SEN_WS_STAR__WBXML
    ,IMPLEMENTATION_PROXY_ENTRY(0x00191919,  CWBXMLPlugin::NewL)
#endif
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

