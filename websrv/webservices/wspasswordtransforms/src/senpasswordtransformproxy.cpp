/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "sentruncatetransform.h"
#include "senlowercasetransform.h"
#include "senuppercasetransform.h"
#include "senselecttransform.h"

#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES
const TInt32 KTruncateTransformPluginUid = 0x101F9708;
const TInt32 KLowercaseTransformPluginUid = 0x101F9709;
const TInt32 KUppercaseTransformPluginUid = 0x101F970A;
const TInt32 KSelectTransformPluginUid = 0x101F970B;

#else

const TInt32 KTruncateTransformPluginUid = 0x101F973E;
const TInt32 KLowercaseTransformPluginUid = 0x101F973F;
const TInt32 KUppercaseTransformPluginUid = 0x101F9740;
const TInt32 KSelectTransformPluginUid = 0x101F9741;
#endif
// Provides a key value pair table, this is used to identify
// the correct construction function for the requested interface.
const TImplementationProxy ImplementationTable[] =
    {
#ifdef IMPLEMENTATION_PROXY_ENTRY       
        IMPLEMENTATION_PROXY_ENTRY(KTruncateTransformPluginUid,  CSenTruncateTransform::NewL),
        IMPLEMENTATION_PROXY_ENTRY(KLowercaseTransformPluginUid,  CSenLowercaseTransform::NewL),
        IMPLEMENTATION_PROXY_ENTRY(KUppercaseTransformPluginUid,  CSenUppercaseTransform::NewL),
        IMPLEMENTATION_PROXY_ENTRY(KSelectTransformPluginUid,  CSenSelectTransform::NewL)
#else
        {{KTruncateTransformPluginUid},  CSenTruncateTransform::NewL},
        {{KLowercaseTransformPluginUid},  CSenLowercaseTransform::NewL},
        {{KUppercaseTransformPluginUid},  CSenUppercaseTransform::NewL},
        {{KSelectTransformPluginUid},  CSenSelectTransform::NewL}
#endif
    };

// Function used to return an instance of the proxy table.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
                                                             TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// End of File
