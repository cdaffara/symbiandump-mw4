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

#include <comms-infras/ss_msgintercept.h>

#include "upnpmcprfactory.h"
#include "upnpmcpr.h"

#if defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
#define KUPnPMCprFactoryTag KESockMetaConnectionTag
#endif

using namespace ESock;

CUPnPMetaConnectionProviderFactory* CUPnPMetaConnectionProviderFactory::NewL(TAny* aParentContainer)
	{
	return new (ELeave) CUPnPMetaConnectionProviderFactory(
		TUid::Uid(CUPnPMetaConnectionProviderFactory::iUid), 
		*reinterpret_cast<CMetaConnectionFactoryContainer*>(aParentContainer));
	}
	
CUPnPMetaConnectionProviderFactory::CUPnPMetaConnectionProviderFactory(TUid aFactoryId, CMetaConnectionFactoryContainer& aParentContainer)
	: CMetaConnectionProviderFactoryBase(aFactoryId, aParentContainer)
	{
//	LOG_NODE_CREATE(KUPnPMCprFactoryTag, CUPnPMetaConnectionProviderFactory);
	}
	
ACommsFactoryNodeId* CUPnPMetaConnectionProviderFactory::DoCreateObjectL(TFactoryQueryBase& aQuery)
	{
	const TMetaConnectionFactoryQuery& query = static_cast<const TMetaConnectionFactoryQuery&>(aQuery);	
	CMetaConnectionProviderBase* provider = CUPnPMetaConnectionProvider::NewL(*this, query.iProviderInfo);
	ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);
    return provider;	
	}


