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


#ifndef __UPNPCPR_H_
#define __UPNPCPR_H_

#include <comms-infras/corecpr.h>
#include <cchunkmanager.h>
#include <rmemchunk.h>

using namespace ESock;

class CUPnPConnectionProvider : public CCoreConnectionProvider,
								public MPlatsecApiExt,										
								public ITFHIERARCHY_1 ( CUPnPConnectionProvider, MPlatsecApiExt )

	{
public:
	typedef ITFHIERARCHY_1 ( CUPnPConnectionProvider, MPlatsecApiExt ) TIfStaticFetcherNearestInHierarchy;
	
	void ReturnInterfacePtrL ( MPlatsecApiExt*& aInterface );
	virtual ~CUPnPConnectionProvider();
	CChunkManager* MemoryManager ( );
	RMemoryAllocator& MemoryAllocator();
	const TDesC8& RootDeviceLocation ( );
protected:
	CUPnPConnectionProvider ( CConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap );
	inline virtual TInt SecureId ( TSecureId& aResult ) const;
    inline virtual TInt VendorId ( TVendorId& aResult ) const;
    inline virtual TBool HasCapabilityL ( const TCapability aCapability ) const;
    TInt CheckPolicy ( const TSecurityPolicy& aPolicy ) const;   

protected:
	MPlatsecApiExt* 			iPlatsecApiExt;
	RMemoryAllocator 			iAllocator;
	};

TInt CUPnPConnectionProvider::SecureId ( TSecureId& aResult ) const
	{	
	ASSERT ( iPlatsecApiExt );
	return iPlatsecApiExt->SecureId ( aResult );
	}

TInt CUPnPConnectionProvider::VendorId ( TVendorId& aResult ) const
	{	
	ASSERT ( iPlatsecApiExt );
	return iPlatsecApiExt->VendorId ( aResult );
	}

TBool CUPnPConnectionProvider::HasCapabilityL ( const TCapability aCapability ) const
	{
	ASSERT ( iPlatsecApiExt );
	return iPlatsecApiExt->HasCapabilityL ( aCapability );
	}


#endif /*__UPNPCPR_H_*/
