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

#ifndef __UPNPSERVICEDEFAULTSCPRACTIVITIES_H
#define __UPNPSERVICEDEFAULTSCPRACTIVITIES_H

#include <comms-infras/corescpractivities.h>
#include "upnpservicedeftscpr.h"


using namespace ESock;
using namespace MeshMachine;


namespace UPnPServiceDefaultActivities
{
	DECLARE_ACTIVITY_MAP ( activityMap )
	
	class CUpnpRootDeviceInfoRequestActivity: public CNodeParallelActivityBase
		{
		public:
			static CNodeActivityBase* NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );
			~CUpnpRootDeviceInfoRequestActivity ( );			
			
		private:
			CUpnpRootDeviceInfoRequestActivity ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aActivityCount );			
			
		private: // Context
			typedef TNodeContext<CUPnPServiceDefaultSubConnectionProvider, CoreStates::TContext> TContext;
			
		public:
			// Transitions
			DECLARE_SMELEMENT_HEADER ( TComposeAndSendRootDescription, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
				void DoL ( );
			DECLARE_SMELEMENT_FOOTER ( TComposeAndSendRootDescription )
			
			DECLARE_SMELEMENT_HEADER ( TFindAndSendIconData, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
				void DoL ( );
			DECLARE_SMELEMENT_FOOTER ( TFindAndSendIconData )
		};
	
	class CUpnpRegisterRootDeviceActivity: public CNodeActivityBase
		{
	public:
		static CNodeActivityBase* NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );
		~CUpnpRegisterRootDeviceActivity ();
		
	private:
		CUpnpRegisterRootDeviceActivity ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );
		
	private:
		RBuf8						iIconUrl;
		TInt						iIconUrlIdx;
		
	public:
		typedef TNodeContext<CUPnPServiceDefaultSubConnectionProvider, CoreStates::TContext> TContext;
		
		DECLARE_SMELEMENT_HEADER ( TCreateIconFlow, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			void DoL ();
		DECLARE_SMELEMENT_FOOTER ( TCreateIconFlow )
				
		// StateForks
		DECLARE_SMELEMENT_HEADER ( TIconCreateOrNoTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
			TInt TransitionTag ();
		DECLARE_SMELEMENT_FOOTER ( TIconCreateOrNoTag )
		
		DECLARE_SMELEMENT_HEADER ( TIconOrNoTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
			TInt TransitionTag ();
		DECLARE_SMELEMENT_FOOTER ( TIconOrNoTag )		
		};		
}

#endif // UPNPSERVICEDEFAULTSCPRACTIVITIES_H
