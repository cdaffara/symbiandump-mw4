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

#ifndef __UPNPCONTROLSCPRSTATES_H
#define __UPNPCONTROLSCPRSTATES_H

//System Includes
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/corescprstates.h>
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/ss_corepractivities.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <ss_glob.h>

//Local Includes
#include "upnpctrlscpr.h"
#include "upnpservicescpr.h"
#include "upnpservicedeftscpr.h"
#include "upnpflowfactory.h"
#include "upnpflowbase.h"


using namespace MeshMachine;

	

namespace UPnPControlSCprStates
{
typedef TNodeContext<CUPnPControlSubConnectionProvider, SCprStates::TContext> TContext;

//-------------------------------------------------States-----------------------------------------------------

DECLARE_SMELEMENT_HEADER ( TAwaitingDataClientJoin, TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept ( );
DECLARE_SMELEMENT_FOOTER ( TAwaitingDataClientJoin )

	
//---------------------------------------------Transitions-----------------------------------------------------
DECLARE_SMELEMENT_HEADER ( TStoreRequestParams, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TStoreRequestParams )

DECLARE_SMELEMENT_HEADER ( TSendRequestParamResponse, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TSendRequestParamResponse )

DECLARE_AGGREGATED_TRANSITION2 (
	TStoreParamsAndSendResponse,
   	TStoreRequestParams,
   	TSendRequestParamResponse
   	)

DECLARE_SMELEMENT_HEADER ( TInitiateActivities, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TInitiateActivities )
     
DECLARE_SMELEMENT_HEADER ( TCreateControlDataClient, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( ); 
DECLARE_SMELEMENT_FOOTER ( TCreateControlDataClient )

DECLARE_SMELEMENT_HEADER ( TProcessCallbackDataClient, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual	void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TProcessCallbackDataClient )

DECLARE_SMELEMENT_HEADER ( TSendActivityStopped, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TSendActivityStopped )

DECLARE_SMELEMENT_HEADER ( TFindOrCreateHttpClientFlow, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TFindOrCreateHttpClientFlow )

DECLARE_SMELEMENT_HEADER ( TProcessHttpDataClient, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TProcessHttpDataClient )

//--------------------------------------------State Forks--------------------------------------------
DECLARE_SMELEMENT_HEADER ( TFindHttpClientFlowOrSendRequest, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TFindHttpClientFlowOrSendRequest )


} //end of UPnPControlSCprStates

namespace UPnPSearchStates
{
typedef TNodeContext<CUPnPControlSubConnectionProvider, SCprStates::TContext> TContext;
//Search Request 
DECLARE_SMELEMENT_HEADER ( TFindUdpClientFlowOrSendRequest, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TFindUdpClientFlowOrSendRequest )

DECLARE_SMELEMENT_HEADER ( TStoreUdpClientFlowAndSendRequest, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TStoreUdpClientFlowAndSendRequest )

DECLARE_SMELEMENT_HEADER ( TCancelSearchOrProcessResponseTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TCancelSearchOrProcessResponseTag )	

DECLARE_SMELEMENT_HEADER ( TNoTagOrResponseTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TNoTagOrResponseTag )	

}//end of UPnP Search states.


namespace UPnPRegisterNotifyStates
{
typedef TNodeContext<CUPnPControlSubConnectionProvider, SCprStates::TContext> TContext;
//RegisterNotify Request 
DECLARE_SMELEMENT_HEADER ( TFindMUdpFlowOrSendRequest, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TFindMUdpFlowOrSendRequest )

DECLARE_SMELEMENT_HEADER ( TStoreMUdpFlowAndStartServer, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TStoreMUdpFlowAndStartServer )

DECLARE_SMELEMENT_HEADER ( TCancelRegisterOrProcessResponseTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TCancelRegisterOrProcessResponseTag )	

DECLARE_SMELEMENT_HEADER ( TNoTagOrResponseTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TNoTagOrResponseTag )	

}//end of Register Notify states.

namespace UPnPDescribeStates
{
typedef TNodeContext<CUPnPControlSubConnectionProvider, SCprStates::TContext> TContext;

DECLARE_SMELEMENT_HEADER ( TCancelDescribeOrProcessResponseTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TCancelDescribeOrProcessResponseTag )	
}

namespace UPnPSubscribeStates
{
typedef TNodeContext<CUPnPControlSubConnectionProvider, SCprStates::TContext> TContext;

DECLARE_SMELEMENT_HEADER ( TErrorOrProcessResponseTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TErrorOrProcessResponseTag )

DECLARE_SMELEMENT_HEADER ( TNoTagOrCancelSubscribe, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TNoTagOrCancelSubscribe )
}

#endif // UPNPCONTROLSCPRSTATES_H
