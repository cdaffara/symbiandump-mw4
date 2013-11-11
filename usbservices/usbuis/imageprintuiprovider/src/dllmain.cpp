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
* Description:  Ecom plugin info for provider
*
*/




/// INCLUDES
#include <e32std.h>
#include <implementationproxy.h>
#include "aiwprintingprovideruid.h"
#include "caiwprintingprovider.h"


// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
 
    IMPLEMENTATION_PROXY_ENTRY(KAiwPrintingProviderImplUid,
                               CAiwPrintingProvider::NewL),                        
    };
    
// The one and only exported function that is the ECom entry point
EXPORT_C const TImplementationProxy* ImplementationGroupProxy
        (TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

#ifndef EKA2
/**
 * Standard Symbian OS DLL entry point.
 */
TBool E32Dll(TDllReason)
    {
    return ETrue;
    }
#endif

