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

#include "upnpctrlcprfactory.h"
#include "upnpctrlcpr.h"

#if defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
#define KUPnPCprFactoryTag KESockConnectionTag
#endif

CUPnPControlConnectionProviderFactory* CUPnPControlConnectionProviderFactory::NewL(TAny* aParentContainer)
	{
	return new (ELeave) CUPnPControlConnectionProviderFactory(
		TUid::Uid(CUPnPControlConnectionProviderFactory::iUid), 
		*reinterpret_cast<CConnectionFactoryContainer*>(aParentContainer));
	}
	
CUPnPControlConnectionProviderFactory::CUPnPControlConnectionProviderFactory(TUid aFactoryId, CConnectionFactoryContainer& aParentContainer)
	: CConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
//	LOG_NODE_CREATE(KUPnPCprFactoryTag, CUPnPControlConnectionProviderFactory);
    }

ACommsFactoryNodeId* CUPnPControlConnectionProviderFactory::DoCreateObjectL(ESock::TFactoryQueryBase& /*aQuery*/)	
    {    
    CConnectionProviderBase* provider = CUPnPControlConnectionProvider::NewL(*this);    
	ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);
    return provider;
    }
