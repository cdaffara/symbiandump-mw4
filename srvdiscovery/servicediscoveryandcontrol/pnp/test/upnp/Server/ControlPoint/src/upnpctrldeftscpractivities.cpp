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
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/ss_corepractivities.h>

//Local Includes
#include "upnpctrldeftscpractivities.h"
#include "upnpctrlscprstates.h"
#include "upnpstatemachine.h"
#include "upnpctrlscpractivities.h"
using namespace ESock;
using namespace MeshMachine;
using namespace CoreActivities;
using namespace CoreNetStates;
using namespace CoreStates;
using namespace UPnPStateMachine;
using namespace PRActivities;

namespace UPnPDefaultSCprBindToActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( ECFActivityBindTo, UPnPDefaultSCprBindToRequest, TCFDataClient::TBindTo, PRActivities::CBindToActivity::NewL )
	FIRST_NODEACTIVITY_ENTRY ( CoreNetStates::TAwaitingBindTo, PRActivities::CBindToActivity::TNoTagOrBearerReady )
    
    THROUGH_NODEACTIVITY_ENTRY ( KNoTag, TStoreBindToOriginator, TNoTag )
    NODEACTIVITY_ENTRY ( KNoTag, CoreNetStates::TSendControlClientJoinRequest, CoreStates::TAwaitingJoinComplete, TNoTag )
	
	NODEACTIVITY_ENTRY ( KNoTag, TFindOrCreateUdpClientFlow, CoreNetStates::TAwaitingDataClientJoin, TErrorTagOr<TTag<KNoTag> > )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, PRStates::TProcessDataClientCreation, TTag<KBindToComplete> )
	
	LAST_NODEACTIVITY_ENTRY ( KBindToComplete, ABindingActivity::TSendBindToComplete )

	NODEACTIVITY_ENTRY ( KErrorTag, CoreNetStates::TSendClientLeavingRequestToServiceProvider, CoreStates::TAwaitingLeaveComplete, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TRaiseAndClearActivityError )		
NODEACTIVITY_END ( )
}

namespace UPnPCtrlDeftScprLeaveActivity
{
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityClientLeave, UPnPCtrlDeftScprLeave, TNodeSignal::TNullMessageId ) //May be waiting for both messages
	FIRST_NODEACTIVITY_ENTRY ( CoreStates::TAwaitingClientLeave, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, UPnPDefaultSCprLeaveActivity::TProcessClientLeave, UPnPDefaultSCprLeaveActivity::TNoTagOrDataClientIdle )
	THROUGH_NODEACTIVITY_ENTRY ( KDataClientIdle, CoreNetStates::TSendDataClientIdle, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TDoNothing )
NODEACTIVITY_END ()
}

namespace UPnPCtrlDeftScprDestroyActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( ECFActivityDestroy, UPnPCtrlDeftScprDestroy, TEChild::TDestroy, CoreActivities::CDestroyActivity::New )
	FIRST_NODEACTIVITY_ENTRY ( TAwaitingDestroy, TNoTag )
	NODEACTIVITY_ENTRY ( KNoTag, UPnPStateMachine::TStopDataClients, UPnPStateMachine::TAwaitingToStopDataClients, CoreNetStates::TNoTagOrUnbindOnStop )
	NODEACTIVITY_ENTRY ( CoreNetStates::KUnbind, CoreNetStates::TSendClientLeavingRequestToServiceProvider, CoreStates::TAwaitingLeaveComplete, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, CoreNetStates::TSendClientLeavingAndRemoveControlProvider )
NODEACTIVITY_END ( )
}

namespace UPnPControlDefaultSCprActivities
{
DEFINE_ACTIVITY_MAP ( activityMap )
	ACTIVITY_MAP_ENTRY ( UPnPDefaultSCprBindToActivity, UPnPDefaultSCprBindToRequest )
	ACTIVITY_MAP_ENTRY ( UPnPCtrlDeftScprLeaveActivity, UPnPCtrlDeftScprLeave )
	ACTIVITY_MAP_ENTRY ( UPnPCtrlDeftScprDestroyActivity, UPnPCtrlDeftScprDestroy )
ACTIVITY_MAP_END_BASE ( SCprActivities, coreSCprActivities )
}


DEFINE_SMELEMENT ( UPnPDefaultSCprLeaveActivity::TNoTagOrDataClientIdle, NetStateMachine::MStateFork, UPnPDefaultSCprLeaveActivity::TContext )
TInt UPnPDefaultSCprLeaveActivity::TNoTagOrDataClientIdle::TransitionTag ( )
	{
	TUint ccCount = iContext.Node ( ).CountClients<TDefaultClientMatchPolicy> ( TClientType ( TCFClientType::ECtrl ) );
	return ccCount == 0 ? KDataClientIdle : KNoTag;	
	}
