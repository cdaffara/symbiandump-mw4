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

#include "upnpctrlcpractivities.h"
#include "upnpmcpr.h"


#if defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
#define KUPnPCprTag KESockConnectionTag
#endif

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;
using namespace NetInterfaces;

CUPnPControlConnectionProvider* CUPnPControlConnectionProvider::NewL ( CConnectionProviderFactoryBase& aFactory )
	{
    CUPnPControlConnectionProvider* provider = new ( ELeave ) CUPnPControlConnectionProvider ( aFactory, UPnPControlCprActivities::activityMap::Self() );
    CleanupStack::PushL ( provider );
    provider->ConstructL ( );
    CleanupStack::Pop ( provider );
	return provider;
	}

CUPnPControlConnectionProvider::~CUPnPControlConnectionProvider ( )
	{	
	LOG_NODE_DESTROY ( KUPnPCprTag, CUPnPControlConnectionProvider );		
	}

CUPnPControlConnectionProvider::CUPnPControlConnectionProvider ( CConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap )
	: CUPnPConnectionProvider ( aFactory, aActivityMap )
	{
	LOG_NODE_CREATE ( KUPnPCprTag, CUPnPControlConnectionProvider );	
	}

void CUPnPControlConnectionProvider::ConstructL ( )
	{		
	CCoreConnectionProvider::ConstructL ( );
	}

#ifdef SYMBIAN_NETWORKING_UPS
TInt CUPnPControlConnectionProvider::GetProcessAndThreadId ( TProcessId& aProcessId, TThreadId& aThreadId ) const
	{
	return iPlatsecApiExt->GetProcessAndThreadId ( aProcessId, aThreadId );	
	}
#endif	

void CUPnPControlConnectionProvider::ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage )
	{
	TNodeContext<CUPnPControlConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);	
	ESOCK_DEBUG_MESSAGE_INTERCEPT ( aSender, aMessage, aRecipient );
	if ( aMessage.IsMessage<TCFServiceProvider::TStart> ( ) || aMessage.IsMessage<TCFServiceProvider::TStop> ( ) )
		{
		iPlatsecApiExt = reinterpret_cast<MPlatsecApiExt*> ( address_cast<TNodeId> ( aSender ).Node ( ).FetchNodeInterfaceL ( MPlatsecApiExt::KInterfaceId ) );
		}
	CCoreConnectionProvider::ReceivedL ( aSender, aRecipient, aMessage );
	User::LeaveIfError ( ctx.iReturn );
	}



