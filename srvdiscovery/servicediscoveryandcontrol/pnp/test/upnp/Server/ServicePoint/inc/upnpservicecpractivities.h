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

#ifndef __UPNPSERVICECPRACTIVITIES_H
#define __UPNPSERVICECPRACTIVITIES_H

#include <comms-infras/corecpractivities.h>

#include "upnpservicecpr.h"
#include "upnp_cf_msgs.h"

using namespace MeshMachine;

namespace UPnPServiceCprActivities
{
	DECLARE_ACTIVITY_MAP(activityMap)
	
	class CUpnpNewServiceRegisterActivity: public CNodeActivityBase	
		{
	public:
		static CNodeActivityBase* NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );
		~CUpnpNewServiceRegisterActivity ( );
		
	private:
		CUpnpNewServiceRegisterActivity ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
		: CNodeActivityBase ( aActivitySig, aNode )
			{	}	
	public:
		typedef MeshMachine::TNodeContext<CUPnPServiceConnectionProvider, CprStates::TContext> TContext;
		
		typedef TAwaitingMessageState<TUpnpMessage::TRootDeviceRegistered> TAwiatingRootDeviceRegistered;

		DECLARE_SMELEMENT_HEADER ( TRegisterRootDevice, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			virtual void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TRegisterRootDevice )

		DECLARE_SMELEMENT_HEADER ( TReplyToPendingServices, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			virtual void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TReplyToPendingServices )
		
		DECLARE_SMELEMENT_HEADER ( TReplyToCurrentService, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			virtual void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TReplyToCurrentService )		
		};
		
	class CUpnpSubscriptionTimerActivity: public CNodeActivityBase
		{
	public:
		static CNodeActivityBase* NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );
		~CUpnpSubscriptionTimerActivity ( );
		
		static TInt SubscriptionTimerCallBackL ( TAny* aPtr );	
		
	private:
		CUpnpSubscriptionTimerActivity ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
			: CNodeActivityBase ( aActivitySig, aNode )
			{	}
			
		void ConstructL ( );
		
	private:
		CPeriodic*	iPeriodicTimer;
		
	public:
		typedef MeshMachine::TNodeContext<CUPnPServiceConnectionProvider, CprStates::TContext> TContext;
		
		typedef TAwaitingMessageState<TUpnpMessage::TStartTimer> TAwaitingStartTimer;		
		typedef TAwaiting2MessagesState<TUpnpMessage::TStartTimer, TUpnpMessage::TStopTimer> TAwaitingStartOrStopTimer;
	
		DECLARE_SMELEMENT_HEADER ( TStartTimer, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			virtual void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TStartTimer )

		DECLARE_SMELEMENT_HEADER ( TDequeueClient, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			virtual void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TDequeueClient )
		
		DECLARE_SMELEMENT_HEADER ( TStartOrStopTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
			virtual TInt TransitionTag ( );
		DECLARE_SMELEMENT_FOOTER ( TStartOrStopTag )

		DECLARE_SMELEMENT_HEADER ( TLoopOrCancelTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
			virtual TInt TransitionTag ( );
		DECLARE_SMELEMENT_FOOTER ( TLoopOrCancelTag )		
		};		
}

#endif // UPNPSERVICECPRACTIVITIES_H
