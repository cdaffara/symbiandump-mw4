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

#ifndef __UPNPSERVICEDEFAULTSCPRSTATES_H
#define __UPNPSERVICEDEFAULTSCPRSTATES_H

//System Includes
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/corescprstates.h>

//Local Includes
#include "upnpservicedeftscpr.h"
#include "upnp_cf_msgs.h"

using namespace MeshMachine;

namespace UPnPServiceDefaultStates
{
typedef TNodeContext<CUPnPServiceDefaultSubConnectionProvider, SCprStates::TContext> TContext;


//========================================States==============================================
typedef TAwaitingMessageState<TUpnpMessage::TRegisterRootDevice> TAwaitingRootDeviceRegister;
typedef TAwaitingMessageState<TUpnpMessage::TDeviceRepublish> TAwaitingDeviceRepublish;

//=================================Transitions================================================
DECLARE_SMELEMENT_HEADER ( TStoreUdpClient, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TStoreUdpClient )			

DECLARE_AGGREGATED_TRANSITION2 (
	TProcessUdpClientFlowCreation,
	PRStates::TProcessDataClientCreation,
	TStoreUdpClient
	)	


DECLARE_SMELEMENT_HEADER ( TStoreUdpServer, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TStoreUdpServer )

DECLARE_AGGREGATED_TRANSITION2 (
	TProcessUdpServerFlowCreation,
	PRStates::TProcessDataClientCreation,
	TStoreUdpServer
	)


DECLARE_SMELEMENT_HEADER ( TStartMuClient, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TStartMuClient )			


DECLARE_SMELEMENT_HEADER ( TStartMuServer, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TStartMuServer )			


DECLARE_SMELEMENT_HEADER ( TBuildPublishAndRootDeviceInfo, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TBuildPublishAndRootDeviceInfo )


DECLARE_SMELEMENT_HEADER ( TSendPublishRequest, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	void DoL ( );
	void DoPublishL ( const TDesC8& aSearchTarget, const TDesC8& aUsn, TInt aMaxAge );
DECLARE_SMELEMENT_FOOTER ( TSendPublishRequest )			


DECLARE_SMELEMENT_HEADER ( TRegisterRootDeviceWithMuServer, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	void DoL ( );
	void RegisterForMSearchL ( const TDesC8& aSearchTarget );
DECLARE_SMELEMENT_FOOTER ( TRegisterRootDeviceWithMuServer )


DECLARE_SMELEMENT_HEADER ( TSendRootDeviceRegistered, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TSendRootDeviceRegistered )


DECLARE_SMELEMENT_HEADER ( TSendSearchResponse, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	void DoL ( );
	void SendSearchResponseL ( const TDesC8& aSearchTarget, const TDesC8& aUsn, TInt aMaxAge );
DECLARE_SMELEMENT_FOOTER ( TSendSearchResponse )


DECLARE_SMELEMENT_HEADER ( TLeaveRequest, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TLeaveRequest )


DECLARE_AGGREGATED_TRANSITION3 (
	TProcessClientLeave,
   	MeshMachine::TRemoveClient,
   	CoreNetStates::TSendLeaveCompleteIfRequest,
   	TLeaveRequest
   	)


DECLARE_SMELEMENT_HEADER ( TProcessDataClientStop, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	void DoL ( );
	void SendByeRequestL ( const TDesC8& aSearchTarget, const TDesC8& aUsn );
DECLARE_SMELEMENT_FOOTER ( TProcessDataClientStop )			


//=================================StateForks================================================
DECLARE_SMELEMENT_HEADER ( TRequestOrIgnoreTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TRequestOrIgnoreTag )


DECLARE_SMELEMENT_HEADER ( TNoTagOrIgnoreTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TNoTagOrIgnoreTag )


DECLARE_SMELEMENT_HEADER ( TNoTagOrDataClientIdle, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TNoTagOrDataClientIdle )

}
#endif // UPNPSERVICEDEFAULTSCPRSTATES_H
