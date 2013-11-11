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

#ifndef __UPNPSERVICECPRSTATES_H
#define __UPNPSERVICECPRSTATES_H

//System Includes
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/corecprstates.h>

//Local Includes
#include "upnpservicecpr.h"
#include "upnp_cf_msgs.h"

using namespace MeshMachine;

namespace UPnPServiceCprStates
{
typedef TNodeContext<CUPnPServiceConnectionProvider, CprStates::TContext> TContext;

// States
typedef TAwaiting2MessagesState<TUpnpMessage::TServiceJoinRequest, TUpnpMessage::TDeviceJoinRequest> TAwaitingServiceOrDeviceJoin;
typedef TAwaiting2MessagesState<TUpnpMessage::TRegisterService, TUpnpMessage::TRegisterDevice> TAwaitingServiceOrDeviceRegister;
typedef TAwaiting2MessagesState<TUpnpMessage::TUnregisterService, TUpnpMessage::TUnregisterDevice> TAwaitingServiceOrDeviceUnregister;

// State Forks
DECLARE_SMELEMENT_HEADER ( TServiceOrDeviceTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TServiceOrDeviceTag )


DECLARE_SMELEMENT_HEADER ( TRootRegisteredOrNoTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TRootRegisteredOrNoTag )


// Transitions
DECLARE_SMELEMENT_HEADER ( TPerformServiceOrDeviceValidationAndSendResponse, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TPerformServiceOrDeviceValidationAndSendResponse )


DECLARE_SMELEMENT_HEADER ( TAppendServiceInfo, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TAppendServiceInfo )


DECLARE_SMELEMENT_HEADER ( TAppendDeviceInfo, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TAppendDeviceInfo )


DECLARE_SMELEMENT_HEADER ( TDeleteServiceInfo, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TDeleteServiceInfo )


DECLARE_SMELEMENT_HEADER ( TDeleteDeviceInfo, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TDeleteDeviceInfo )

}


#endif // UPNPSERVICECPRSTATES_H


