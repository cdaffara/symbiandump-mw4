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

#include "upnptiermanagerfactory.h"
#include "upnptiermanager.h"

#if defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
#define KUPnPTmFactoryTag KESockTierTag
#endif

using namespace ESock;

CUPnPTierManagerFactory* CUPnPTierManagerFactory::NewL(TAny* aParentContainer)
	{
	return new (ELeave) CUPnPTierManagerFactory(
		TUid::Uid(CUPnPTierManagerFactory::iUid), 
		*reinterpret_cast<CTierManagerFactoryContainer*>(aParentContainer));
	}


CUPnPTierManagerFactory::CUPnPTierManagerFactory(
	TUid aFactoryId, 
	CTierManagerFactoryContainer& aParentContainer)
	: CTierManagerFactoryBase(aFactoryId, aParentContainer)
	{
//	LOG_NODE_CREATE(KUPnPTmFactoryTag, CUPnPTierManagerFactory);
	}

ACommsFactoryNodeId* CUPnPTierManagerFactory::DoCreateObjectL(TFactoryQueryBase& /* aQuery */)
	{
	return CUPnPTierManager::NewL(*this);
	}

