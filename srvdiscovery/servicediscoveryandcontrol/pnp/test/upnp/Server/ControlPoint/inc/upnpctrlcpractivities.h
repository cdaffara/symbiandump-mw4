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

#ifndef __UPNPCONTROLCPRACTIVITIES_H
#define __UPNPCONTROLCPRACTIVITIES_H

#include <comms-infras/corecpractivities.h>
#include <elements/sm_statetriple.h>

#include "upnpctrlcpr.h"

namespace UPnPControlCprActivities
{
typedef MeshMachine::TNodeContext<CUPnPControlConnectionProvider, CprStates::TContext> TContext;

DECLARE_SMELEMENT_HEADER( RequestLocalHost, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( RequestLocalHost )

DECLARE_SMELEMENT_HEADER( AwaitingLocalHostFromMCpr, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( AwaitingLocalHostFromMCpr )


DECLARE_ACTIVITY_MAP(activityMap)
}

#endif // UPNPCONTROLCPRACTIVITIES_H