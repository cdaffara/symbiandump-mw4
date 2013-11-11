/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
// INCLUDES
#include <e32std.h>
#include <implementationproxy.h>
#include "AiwBrowserProviderOpenUrl.h"
#include "AiwBrowserProviderSaveUrl.h"
#include "logger.h"

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
        {{0x101FD683}, (TProxyNewLPtr) CAiwBrowserProviderSaveUrl::NewL},
        {{0x101FD684}, (TProxyNewLPtr) CAiwBrowserProviderOpenUrl::NewL}
    };

// The one and only exported function that is the ECom entry point
EXPORT_C const TImplementationProxy* ImplementationGroupProxy
        (TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    LOG_CREATE;
    return ImplementationTable;
    }

/**
 * Standard Symbian OS DLL entry point.
 */
#ifndef EKA2     
TBool E32Dll(TDllReason)
    {
    return ETrue;
    }
#endif // EKA2
// End of File
    
