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
// UPnPCtrCprActivities.cpp
// @file
// @internalComponent
// 
//

#include <comms-infras/ss_corepractivities.h>
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/ss_nodemessages_internal_esock.h>

#include "upnpctrlcpractivities.h"
#include "upnpctrlscprstates.h"
#include "upnpstatemachine.h"
#include "upnp_cf_msgs.h"


using namespace CoreNetStates;
using namespace CoreStates;
using namespace MeshMachine;
using namespace UPnPControlCprActivities;

namespace UPnPControlCprStartActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStart, UPnPControlCprStart, TCFServiceProvider::TStart, PRActivities::CStartActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingStart, CoreNetStates::TNoTagOrBearerPresentBlockedByStop)    
	NODEACTIVITY_ENTRY(KBearerPresent, CoreNetStates::TBindSelfToPresentBearer, CoreNetStates::TAwaitingBindToComplete, TTag<KBearerPresent>)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendNoBearer, MeshMachine::TAwaitingMessageState<TCFControlProvider::TBearer>, TTag<KBearerPresent>)

	//Start the service provider, use the default cancellation.
	//Forward TCancel to the service provider, wait for TStarted or TError (via the Error Activity)
	//When TStarted arrives after TCancel the activity will move to the nearest KErrorTag
	NODEACTIVITY_ENTRY(KBearerPresent, CoreNetStates::TStartServiceProviderRetry, CoreNetStates::TAwaitingStarted, TNoTagOrErrorTag)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing)
	//Start data clients, use the default cancellation.
	//Forward TCancel to the self, wait for TDataClientStarted or TError (via the Error Activity)
	//When TDataClientStarted arrives after TCancel the activity will move to the nearest KErrorTag
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TStartSelf, CoreNetStates::TAwaitingDataClientStarted, TNoTagOrErrorTag)
	NODEACTIVITY_ENTRY(KNoTag, UPnPStateMachine::TRequestLocalHost, UPnPStateMachine::TAwaitingLocalHostFromMCpr, TNoTagOrErrorTag)
	
	NODEACTIVITY_ENTRY(KErrorTag, CoreNetStates::TStopSelf, CoreNetStates::TAwaitingDataClientStopped, TErrorTag)
	LAST_NODEACTIVITY_ENTRY(KErrorTag, MeshMachine::TDoNothing)
	
	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendStarted)
NODEACTIVITY_END()
}

namespace UPnPControlCprActivities
{
	DEFINE_ACTIVITY_MAP(activityMap)
	ACTIVITY_MAP_ENTRY(UPnPControlCprStartActivity, UPnPControlCprStart)
	ACTIVITY_MAP_END_BASE(CprActivities, coreCprActivities)
}
