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

#ifndef __UPNPTIERMANAGERFACTORY_H
#define __UPNPTIERMANAGERFACTORY_H

#include <comms-infras/ss_tiermanager.h>

class CUPnPTierManagerFactory : public ESock::CTierManagerFactoryBase
	{		
public:
	// ECOM interface implementation ID
	static const TUint iUid = 0x2000D054;

public:
	static CUPnPTierManagerFactory* NewL(TAny* aParentContainer);

protected:
	CUPnPTierManagerFactory(
		TUid aFactoryId, 
		ESock::CTierManagerFactoryContainer& aParentContainer);

protected:
    // CTierManagerFactoryBase overrides    
    ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& /* aQuery */);
	};
	
#endif // UPNPTIERMANAGERFACTORY_H
