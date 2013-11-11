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

//System Includes
#include <comms-infras/ss_msgintercept.h>

//Local Includes
#include "upnpctrldeftscpr.h"
#include "upnpctrldeftscpractivities.h"

#if defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
#define KUPnPDefaultSCprTag KESockSubConnectionTag
#endif

using namespace ESock;
using namespace Messages;

CUPnPControlDefaultSubConnectionProvider* CUPnPControlDefaultSubConnectionProvider::NewL(ESock::CSubConnectionProviderFactoryBase& aFactory)
	{
	CUPnPControlDefaultSubConnectionProvider* provider = new (ELeave) CUPnPControlDefaultSubConnectionProvider(aFactory, UPnPControlDefaultSCprActivities::activityMap::Self());
	CleanupStack::PushL(provider);
	provider->ConstructL();
	CleanupStack::Pop(provider);
	return provider;
	}

CUPnPControlDefaultSubConnectionProvider::~CUPnPControlDefaultSubConnectionProvider()
	{
	LOG_NODE_DESTROY(KUPnPDefaultSCprTag, CUPnPControlDefaultSubConnectionProvider);
	}

CUPnPControlDefaultSubConnectionProvider::CUPnPControlDefaultSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap)
	: CUPnPSubConnectionProvider(aFactory, aActivityMap)
	{
	LOG_NODE_CREATE(KUPnPDefaultSCprTag, CUPnPControlDefaultSubConnectionProvider);    
	}

void CUPnPControlDefaultSubConnectionProvider::ConstructL()
	{
	CCoreSubConnectionProvider::ConstructL();
	}

void CUPnPControlDefaultSubConnectionProvider::ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage )
	{
	ESOCK_DEBUG_MESSAGE_INTERCEPT(aSender, aMessage, aRecipient);
	MeshMachine::TNodeContext<CUPnPControlDefaultSubConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);	    
	CCoreSubConnectionProvider::ReceivedL(aSender, aRecipient, aMessage);
	User::LeaveIfError(ctx.iReturn);
	}
