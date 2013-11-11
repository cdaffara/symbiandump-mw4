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

#include "SenIdWsfAnonymousSaslMechanism.h"
#include "SenIdWsfPlainSaslMechanism.h"
#include "SenIdWsfCrammd5SaslMechanism.h"

#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES    
const TInt32 KAnonymousSaslPluginUid = 0x101F96FD;
const TInt32 KIdwsfPlainSaslPluginUid = 0x101F96FE;
const TInt32 KIdwsfCrammd5SaslPluginUid = 0x101F96FF;

#else
const TInt32 KAnonymousSaslPluginUid = 0x101F9734;
const TInt32 KIdwsfPlainSaslPluginUid = 0x101F9735;
const TInt32 KIdwsfCrammd5SaslPluginUid = 0x101F9736;

#endif
// Provides a key value pair table, this is used to identify
// the correct construction function for the requested interface.
const TImplementationProxy ImplementationTable[] =
    {
#ifdef IMPLEMENTATION_PROXY_ENTRY       
        IMPLEMENTATION_PROXY_ENTRY(KAnonymousSaslPluginUid,  CSenIdWsfAnonymousSaslMechanism::NewL),
        IMPLEMENTATION_PROXY_ENTRY(KIdwsfPlainSaslPluginUid,  CSenIdWsfPlainSaslMechanism::NewL),
        IMPLEMENTATION_PROXY_ENTRY(KIdwsfCrammd5SaslPluginUid,  CSenIdWsfCrammd5SaslMechanism::NewL)
#else
        {{KAnonymousSaslPluginUid},  CSenIdWsfAnonymousSaslMechanism::NewL},
        {{KIdwsfPlainSaslPluginUid},  CSenIdWsfPlainSaslMechanism::NewL},
        {{KIdwsfCrammd5SaslPluginUid},  CSenIdWsfCrammd5SaslMechanism::NewL}
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
