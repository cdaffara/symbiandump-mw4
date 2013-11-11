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
// @file
// @internalComponent
// 
//

#ifndef __UPNPSERVICECPRFACTORY_H
#define __UPNPSERVICECPRFACTORY_H

#include <comms-infras/ss_connprov.h>
#include <comms-infras/ss_linkcprnodemessages.h>

class CUPnPServiceConnectionProviderFactory : public ESock::CConnectionProviderFactoryBase
	{		
public:
	// ECOM interface implementation ID
	static const TUint iUid = 0x2000D058;

public:
	static CUPnPServiceConnectionProviderFactory* NewL(TAny* aParentContainer);

protected:
	CUPnPServiceConnectionProviderFactory(
		TUid aFactoryId, 
		ESock::CConnectionFactoryContainer& aParentContainer);

protected:
    // CConnectionProviderFactoryBase overrides
    virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);	
	};
	
#endif // UPNPSERVICECPRFACTORY_H
