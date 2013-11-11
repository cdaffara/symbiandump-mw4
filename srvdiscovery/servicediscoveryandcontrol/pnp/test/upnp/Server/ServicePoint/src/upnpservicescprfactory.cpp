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

#include <comms-infras/ss_msgintercept.h>

#include "upnpservicescprfactory.h"
#include "upnpservicescpr.h"
#include "upnpservicedeftscpr.h"

#if defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
#define KUPnPSCprFactoryTag KESockSubConnectionTag
#endif

using namespace ESock;

CUPnPServiceSubConnectionProviderFactory* CUPnPServiceSubConnectionProviderFactory::NewL(TAny* aParentContainer)
	{
	return new (ELeave) CUPnPServiceSubConnectionProviderFactory(
		TUid::Uid(CUPnPServiceSubConnectionProviderFactory::iUid), 
		*reinterpret_cast<CSubConnectionFactoryContainer*>(aParentContainer));
	}
	
CUPnPServiceSubConnectionProviderFactory::CUPnPServiceSubConnectionProviderFactory(TUid aFactoryId, CSubConnectionFactoryContainer& aParentContainer)
	: CSubConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
//	LOG_NODE_CREATE(KUPnPSCprFactoryTag, CUPnPServiceSubConnectionProviderFactory);
    }

ACommsFactoryNodeId* CUPnPServiceSubConnectionProviderFactory::DoCreateObjectL(TFactoryQueryBase& aQuery)
    {
    const TDefaultSCPRFactoryQuery& query = static_cast<const TDefaultSCPRFactoryQuery&>(aQuery);
	CSubConnectionProviderBase* provider = NULL;
    if (query.iSCPRType == RSubConnection::EAttachToDefault)
        {
        provider = CUPnPServiceDefaultSubConnectionProvider::NewL(*this);
		ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);
        }
    else if (query.iSCPRType == RSubConnection::ECreateNew)
        {
        provider = CUPnPServiceSubConnectionProvider::NewL(*this);
		ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);
        }
    else
        {
        User::Leave(KErrNotSupported);
        }
    return provider;
    }
