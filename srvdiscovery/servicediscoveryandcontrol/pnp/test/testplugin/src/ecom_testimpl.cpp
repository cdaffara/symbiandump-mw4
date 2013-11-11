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
//

#include <ecom/implementationproxy.h>
#include "testpnpparamsextfactory.h"
#include "testpnpparamset.h"
#include "ctestpnpservicediscoveryimpl.h"
#include "ctestpnpservicepublisherimpl.h"
  
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KTestPnPParamsFactoryUid, CTestPnPParamsExtFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KTestPnPDiscoveryUid, CTestPnPServiceDiscoveryImpl::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KTestPnPPublisherUid, CTestPnPServicePublisherImpl::NewL)
	};


EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
