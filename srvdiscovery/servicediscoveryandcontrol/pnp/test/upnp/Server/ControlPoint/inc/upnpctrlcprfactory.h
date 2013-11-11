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

#ifndef __UPNPCONTROLCPRFACTORY_H
#define __UPNPCONTROLCPRFACTORY_H

#include <comms-infras/ss_connprov.h>

class CUPnPControlConnectionProviderFactory : public ESock::CConnectionProviderFactoryBase
	{		
public:
	// ECOM interface implementation ID
	static const TUint iUid = 0x2000D056;

public:
	static CUPnPControlConnectionProviderFactory* NewL(TAny* aParentContainer);

protected:
	CUPnPControlConnectionProviderFactory(
		TUid aFactoryId, 
		ESock::CConnectionFactoryContainer& aParentContainer);

protected:
    // CConnectionProviderFactoryBase overrides
    virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);	
	};
	
#endif // UPNPCONTROLCPRFACTORY_H
