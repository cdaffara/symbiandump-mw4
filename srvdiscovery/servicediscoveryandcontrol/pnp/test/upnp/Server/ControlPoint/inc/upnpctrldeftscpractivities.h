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
// UPnPControlDeftScprActivities.h
// @file
// @internalComponent
// 
//

#ifndef __UPNPCONTROLDEFAULTSCPRACTIVITIES_H
#define __UPNPCONTROLDEFAULTSCPRACTIVITIES_H

#include "upnpctrldeftscpr.h"
#include <comms-infras/corescprstates.h>
#include <comms-infras/corescpractivities.h>


using namespace ESock;


namespace UPnPControlDefaultSCprActivities
	{
	DECLARE_ACTIVITY_MAP(activityMap)
	}

namespace UPnPDefaultSCprLeaveActivity
	{
 	typedef MeshMachine::TNodeContext<CUPnPControlDefaultSubConnectionProvider, CoreStates::TContext> TContext;

	DECLARE_SMELEMENT_HEADER ( TNoTagOrDataClientIdle, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
    	virtual TInt TransitionTag ( );    	
	DECLARE_SMELEMENT_FOOTER ( TNoTagOrDataClientIdle )    

    DECLARE_AGGREGATED_TRANSITION2 (
       TProcessClientLeave,
       MeshMachine::TRemoveClient,
       CoreNetStates::TSendLeaveCompleteIfRequest       
	   )

	}


#endif // UPNPCONTROLDEFAULTSCPRACTIVITIES_H
