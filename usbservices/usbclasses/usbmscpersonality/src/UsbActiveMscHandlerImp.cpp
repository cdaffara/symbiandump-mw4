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
* Description:  Implementation proxy
*
*/

#include <e32base.h>
#include <e32std.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include "CUsbActiveMscHandlerMdrv.h"

/**
 * Define the private interface UIDs
 */
const TImplementationProxy UsbPersonalityImplementationTable[] =
    {
	IMPLEMENTATION_PROXY_ENTRY(0x1020DF7B, CUsbActiveMscHandler::NewL),
    };

/**
 * Return number of implementation proxies
 */
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(UsbPersonalityImplementationTable) / sizeof(TImplementationProxy);

    return UsbPersonalityImplementationTable;
    }
