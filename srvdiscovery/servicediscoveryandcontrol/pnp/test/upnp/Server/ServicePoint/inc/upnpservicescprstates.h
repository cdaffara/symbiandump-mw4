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

#ifndef __UPNPSERVICESCPRSTATES_H
#define __UPNPSERVICESCPRSTATES_H

//System Includes
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/corescprstates.h>

//Local Includes
#include "upnpservicescpr.h"

using namespace MeshMachine;

namespace UPnPServiceSCprStates
{
typedef MeshMachine::TNodeContext<CUPnPServiceSubConnectionProvider, SCprStates::TContext> TContext;


//-------------------------------------------------States-----------------------------------------------------
DECLARE_SMELEMENT_HEADER ( TAwaitingServiceValidated, TState<TContext>, NetStateMachine::MState, TContext )
	TBool Accept ( );
DECLARE_SMELEMENT_FOOTER ( TAwaitingServiceRegistration )		


DECLARE_SMELEMENT_HEADER ( TAwaitingServiceRegisteredFromCpr, TState<TContext>, NetStateMachine::MState, TContext )
	TBool Accept ( );
DECLARE_SMELEMENT_FOOTER ( TAwaitingServiceRegisteredFromCpr )
		
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


DECLARE_SMELEMENT_HEADER ( TLeaveRequest, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TLeaveRequest )

DECLARE_AGGREGATED_TRANSITION3 (
	TProcessClientLeave,
   	MeshMachine::TRemoveClient,
   	CoreNetStates::TSendLeaveCompleteIfRequest,
   	TLeaveRequest
   	)
   	
DECLARE_SMELEMENT_HEADER ( TSendPublishResponseToClient, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TSendPublishResponseToClient )


DECLARE_SMELEMENT_HEADER ( TSendDataClientStart, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TSendDataClientStart )			


DECLARE_SMELEMENT_HEADER ( TProcessDataClientStop, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	void DoL ( );
	void SendByeRequestL ( const TDesC8& aSearchTarget, const TDesC8& aUsn );
DECLARE_SMELEMENT_FOOTER ( TProcessDataClientStop )			


DECLARE_SMELEMENT_HEADER ( TAwaitingDataClientsStopped, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	void DoL ( );	
DECLARE_SMELEMENT_FOOTER ( TAwaitingDataClientsStopped )			


DECLARE_SMELEMENT_HEADER ( TSendDataClientIdleOrUnregister, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TSendDataClientIdleOrUnregister )			


DECLARE_SMELEMENT_HEADER ( TValidateAndSendResponse, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
	void SendSearchResponseL ( const TDesC8& aSearchTarget, const TDesC8& aUsn, TInt aMaxAge );
DECLARE_SMELEMENT_FOOTER ( TValidateAndSendResponse )
 	    

//------------------------------------------State Forks---------------------------------------------
DECLARE_SMELEMENT_HEADER ( TStoreUdpClientAndSendRequest, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TStoreUdpClientAndSendRequest )


DECLARE_SMELEMENT_HEADER ( TStoreUdpServer, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TStoreUdpServer )
   	
// Note! will return KIgnore Tag if node is closing
DECLARE_SMELEMENT_HEADER ( TNoTagOrIgnoreTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TNoTagOrIgnoreTag )
}

#endif // UPNPSERVICESCPRSTATES_H
