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

#ifndef __UPNPMCPRFACTORY_H
#define __UPNPMCPRFACTORY_H

#include <comms-infras/ss_metaconnprov.h>

class CUPnPMetaConnectionProviderFactory : public ESock::CMetaConnectionProviderFactoryBase
	{		
public:
	// ECOM interface implementation ID
	static const TUint iUid = 0x2000D055;

public:
	static CUPnPMetaConnectionProviderFactory* NewL(TAny* aParentContainer);
	
protected:
	CUPnPMetaConnectionProviderFactory(
		TUid aFactoryId, 
		ESock::CMetaConnectionFactoryContainer& aParentContainer);

protected:
	// CMetaConnectionProviderFactoryBase overrides
	ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};

#endif // UPNPMCPRFACTORY_H
