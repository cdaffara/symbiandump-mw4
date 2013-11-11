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

#include "upnp_cf_msgs.h"
#include "upnpcpr.h"
#include "upnpmcpr.h"

using namespace ESock;
using namespace MeshMachine;

CUPnPConnectionProvider::CUPnPConnectionProvider ( CConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap )
	: CCoreConnectionProvider ( aFactory, aActivityMap ), 
	TIfStaticFetcherNearestInHierarchy ( this )
	{
	
	}

CUPnPConnectionProvider::~CUPnPConnectionProvider()
	{
	
	}

CChunkManager* CUPnPConnectionProvider::MemoryManager ( )
	{
#if !defined(__GCCXML__)
	CMetaConnectionProviderBase& mcprBase = mnode_cast<CMetaConnectionProviderBase>(ControlProvider()->RecipientId().Node());	
#else
	CMetaConnectionProviderBase& mcprBase =
			reinterpret_cast<CMetaConnectionProviderBase&> ( ControlProvider()->RecipientId().Node() );
#endif

	CUPnPMetaConnectionProvider& mcpr = static_cast<CUPnPMetaConnectionProvider&> ( mcprBase );
	return mcpr.ChunkManager();
	}

RMemoryAllocator& CUPnPConnectionProvider::MemoryAllocator()
	{
	iAllocator.SetChunkManager(MemoryManager());
	return iAllocator;
	}

TInt CUPnPConnectionProvider::CheckPolicy ( const TSecurityPolicy& aPolicy ) const
	{		
	ASSERT ( iPlatsecApiExt );
	return iPlatsecApiExt->CheckPolicy ( aPolicy );
	}

const TDesC8& CUPnPConnectionProvider::RootDeviceLocation ( )
	{
#if !defined(__GCCXML__)
	CMetaConnectionProviderBase& mcprBase = mnode_cast<CMetaConnectionProviderBase>(ControlProvider()->RecipientId().Node());	
#else
	CMetaConnectionProviderBase& mcprBase =
			reinterpret_cast<CMetaConnectionProviderBase&> ( ControlProvider()->RecipientId().Node() );
#endif

	CUPnPMetaConnectionProvider& mcpr = static_cast<CUPnPMetaConnectionProvider&> ( mcprBase );
	return mcpr.DeviceLocationUrl ();
	}

void CUPnPConnectionProvider::ReturnInterfacePtrL ( MPlatsecApiExt*& aInterface )
  	{
  	aInterface = this;
  	}






