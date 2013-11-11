// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// ECOM Implementation for the UPnP Comms Framework Factories
// @file
// @internalComponent
// 
//

#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>

#include "upnptiermanagerfactory.h"
#include "upnpmcprfactory.h"
#include "upnpctrlcprfactory.h"
#include "upnpctrlscprfactory.h"
#include "upnpservicecprfactory.h"
#include "upnpservicescprfactory.h"
#include "upnpflowfactory.h"
#include "app_protintf_fact.h"


const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(CUPnPTierManagerFactory::iUid, CUPnPTierManagerFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(CUPnPMetaConnectionProviderFactory::iUid, CUPnPMetaConnectionProviderFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(CUPnPControlConnectionProviderFactory::iUid, CUPnPControlConnectionProviderFactory::NewL),
    IMPLEMENTATION_PROXY_ENTRY(CUPnPControlSubConnectionProviderFactory::iUid, CUPnPControlSubConnectionProviderFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(CUPnPServiceConnectionProviderFactory::iUid, CUPnPServiceConnectionProviderFactory::NewL),
    IMPLEMENTATION_PROXY_ENTRY(CUPnPServiceSubConnectionProviderFactory::iUid, CUPnPServiceSubConnectionProviderFactory::NewL),
    IMPLEMENTATION_PROXY_ENTRY(CUPnPFlowFactory::iUid, CUPnPFlowFactory::NewL),    
	IMPLEMENTATION_PROXY_ENTRY( CApplicationProtocolIntfFactory::iUid, CApplicationProtocolIntfFactory::NewL )
    };


EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
