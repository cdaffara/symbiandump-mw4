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
// UPnPCtrlCpr.cpp
// @file
// @internalComponent
// 
//

#include <comms-infras/ss_msgintercept.h>

#include "upnpservicecpractivities.h"
#include "upnpmcpr.h"
#include "upnp_cf_msgs.h"
#include "upnpserverconstants.h"

#if defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
#define KUPnPCprTag KESockConnectionTag
#endif

using namespace ESock;
using namespace MeshMachine;

CUPnPServiceConnectionProvider* CUPnPServiceConnectionProvider::NewL ( CConnectionProviderFactoryBase& aFactory )
	{
    CUPnPServiceConnectionProvider* provider = new ( ELeave ) CUPnPServiceConnectionProvider ( aFactory, UPnPServiceCprActivities::activityMap::Self() );
    CleanupStack::PushL ( provider );
    provider->ConstructL ( );
    CleanupStack::Pop ( provider );
	return provider;
	}

CUPnPServiceConnectionProvider::~CUPnPServiceConnectionProvider ( )
	{
	LOG_NODE_DESTROY ( KUPnPCprTag, CUPnPServiceConnectionProvider );	
	delete iRootDeviceInfoContainer;
	if ( iRepublishTimer )
		iRepublishTimer->Cancel( );
	delete iRepublishTimer;
	}

CUPnPServiceConnectionProvider::CUPnPServiceConnectionProvider ( CConnectionProviderFactoryBase& aFactory, const TNodeActivityMap& aActivityMap )
	: CUPnPConnectionProvider ( aFactory, aActivityMap ),
	iRootDeviceRegistered ( EFalse )
	{
	LOG_NODE_CREATE ( KUPnPCprTag, CUPnPServiceConnectionProvider );
	}

void CUPnPServiceConnectionProvider::ConstructL ( )
	{		
	iRootDeviceInfoContainer = CUPnPRootDeviceInfoContainer::NewL ( );
	CCoreConnectionProvider::ConstructL ( );
	
	// create the timer object
	iRepublishTimer = CPeriodic::NewL ( CActive::EPriorityLow );	
	}


#ifdef SYMBIAN_NETWORKING_UPS
TInt CUPnPServiceConnectionProvider::GetProcessAndThreadId ( TProcessId& aProcessId, TThreadId& aThreadId ) const
	{
	return iPlatsecApiExt->GetProcessAndThreadId ( aProcessId, aThreadId );	
	}
#endif  	

CUPnPRootDeviceInfoContainer& CUPnPServiceConnectionProvider::GetRootDeviceInfoContainer ( )
	{
	return *iRootDeviceInfoContainer;
	}
	
CStringPoolManager& CUPnPServiceConnectionProvider::StringPoolManager ( )
	{
	return iRootDeviceInfoContainer->StringPoolManager( );
	}	


void CUPnPServiceConnectionProvider::ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage )
	{
	TNodeContext<CUPnPServiceConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);
	ESOCK_DEBUG_MESSAGE_INTERCEPT ( aSender, aMessage, aRecipient );	
	if ( aMessage.IsMessage<TCFServiceProvider::TStart> ( ) || aMessage.IsMessage<TCFServiceProvider::TStop> ( ) )
		{
  		iPlatsecApiExt = reinterpret_cast<MPlatsecApiExt*> ( address_cast<TNodeId> ( aSender ).Node ( ).FetchNodeInterfaceL ( MPlatsecApiExt::KInterfaceId ) );
  		}
    
   	CCoreConnectionProvider::ReceivedL ( aSender, aRecipient, aMessage );
	User::LeaveIfError ( ctx.iReturn );
	}

void CUPnPServiceConnectionProvider::ProcessRootDeviceRegisteredL ( )
	{
	__ASSERT_DEBUG ( iRootDeviceRegistered == EFalse, User::Invariant ( ) );
  	SetRootDeviceRegistered ( ETrue );

	TCallBack updateCallback ( &CUPnPServiceConnectionProvider::RootDeviceRepublishTimerCallbackL, this );
	iRepublishTimer->Start( TTimeIntervalMicroSeconds32 ( KRootDeviceRepublishFrequency ), TTimeIntervalMicroSeconds32 ( KRootDeviceRepublishFrequency ), updateCallback );
	}

TInt CUPnPServiceConnectionProvider::RootDeviceRepublishTimerCallbackL ( TAny* aPtr )
	{
	CUPnPServiceConnectionProvider* self = static_cast<CUPnPServiceConnectionProvider*> ( aPtr );
	
	TClientIter<TDefaultClientMatchPolicy> dciter = self->GetClientIter<TDefaultClientMatchPolicy> ( TCFClientType::EData );
 	
    TInt i = 0;
    // skip the first data client i.e default SCPR (root device)
    RNodeInterface* dc = dciter[i++];
	TBool canStop = ETrue;
	while ( ( dc = dciter[i++] ) != NULL )
    	{
       	if ( dc->Flags( ) != TClientType::ELeaving )
       		{
			canStop = EFalse;
			break;
       		}
       	}
       
    if ( canStop || !self->IsRootDeviceRegistered( ) )
    	{
    	self->iRepublishTimer->Cancel( );
    	self->SetRootDeviceRegistered ( EFalse );
    	}
    else
    	{
    	RNodeInterface* dc = self->GetFirstClient<TDefaultClientMatchPolicy> ( TCFClientType::EData );				
		RClientInterface::OpenPostMessageClose ( self->NodeId (), dc->RecipientId (), TUpnpMessage::TDeviceRepublish ().CRef () );
		}
    
    return KErrNone;
	}
