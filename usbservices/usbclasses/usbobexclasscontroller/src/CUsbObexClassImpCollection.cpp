/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines the implementation collection for the USB Obex class controller.
*
*/


#include <e32base.h>
#include <e32std.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include "CUsbObexClassController.h"

// Define the private interface UIDs
const TImplementationProxy UsbCCImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x10281F2A, CUsbObexClassController::NewL),
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(UsbCCImplementationTable) / sizeof(TImplementationProxy);

    return UsbCCImplementationTable;
    }
