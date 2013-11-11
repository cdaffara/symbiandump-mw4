/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements ECOM methods for this plug-in
*
*/


#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

#include "cremotepersonalityhandler.h"

/**
 * Define the private interface UIDs
 */
const TImplementationProxy UsbRemotePersonalityImplementationTable[] =
    {
  IMPLEMENTATION_PROXY_ENTRY(0x10283308, CRemotePersonalityHandler::NewL),
    };

/**
 * Return number of implementations
 */
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(UsbRemotePersonalityImplementationTable) / sizeof(TImplementationProxy);

    return UsbRemotePersonalityImplementationTable;
    }

