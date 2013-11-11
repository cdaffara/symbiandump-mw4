/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementations using basic personality
*
*/


#include <e32base.h>
#include <e32std.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <bldvariant.hrh>
#include "CUsbActiveBasicHandler.h"

/**
 * Define the private interface UIDs
 */
const TImplementationProxy UsbPersonalityImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x102068DF, CUsbActiveBasicHandler::NewL)
	,IMPLEMENTATION_PROXY_ENTRY(0x10274792, CUsbActiveBasicHandler::NewL)
	,IMPLEMENTATION_PROXY_ENTRY(0x10282C70, CUsbActiveBasicHandler::NewL)
    ,IMPLEMENTATION_PROXY_ENTRY(0x2000B5D2, CUsbActiveBasicHandler::NewL) // PCS+MTP
    ,IMPLEMENTATION_PROXY_ENTRY(0x20029E41, CUsbActiveBasicHandler::NewL) // PhoneAsModem
    };

/**
 * Return number of implementations
 */
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(UsbPersonalityImplementationTable) / sizeof(TImplementationProxy);

    return UsbPersonalityImplementationTable;
    }
