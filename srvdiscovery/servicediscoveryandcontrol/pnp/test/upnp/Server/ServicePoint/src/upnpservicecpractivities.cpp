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

#include <comms-infras/ss_corepractivities.h>
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_logext.h>

#include "upnpservicecprstates.h"
#include "upnpstatemachine.h"
#include "upnpservicecpractivities.h"
#include "upnpserverconstants.h"
#include "upnplog.h"
__FLOG_STMT(_LIT8(KComponent,"UPnPSPScpr");)

using namespace ESock;
using namespace PRActivities;
using namespace MeshMachine;
using namespace UPnPServiceCprStates;
using namespace UPnPStateMachine;
using namespace UPnPServiceCprActivities;
using namespace Messages;

namespace UpnPServiceCprServiceJoinRequestActivity
{
DECLARE_DEFINE_NODEACTIVITY ( TUpnpMessage::EActivityServiceJoinRequest, UpnpServiceJoinRequest, TNodeSignal::TNullMessageId  ) //Waiting for both Service / Embedded Device Join messages
	FIRST_NODEACTIVITY_ENTRY ( TAwaitingServiceOrDeviceJoin, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TPerformServiceOrDeviceValidationAndSendResponse )
NODEACTIVITY_END ( )
}

namespace UPnPServiceCprNewServiceRegisterActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( TUpnpMessage::EActivityNewServiceRegister, UpnpNewServiceRegister, TNodeSignal::TNullMessageId , CUpnpNewServiceRegisterActivity::NewL )
	FIRST_NODEACTIVITY_ENTRY ( TAwaitingServiceOrDeviceRegister, TServiceOrDeviceTag )
	THROUGH_NODEACTIVITY_ENTRY ( KServiceTag, TAppendServiceInfo, TRootRegisteredOrNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KDeviceTag, TAppendDeviceInfo, TRootRegisteredOrNoTag )
	NODEACTIVITY_ENTRY ( KNoTag, CUpnpNewServiceRegisterActivity::TRegisterRootDevice, CUpnpNewServiceRegisterActivity::TAwiatingRootDeviceRegistered, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, CUpnpNewServiceRegisterActivity::TReplyToPendingServices )
	
	LAST_NODEACTIVITY_ENTRY ( KRootRegisteredTag, CUpnpNewServiceRegisterActivity::TReplyToCurrentService )
NODEACTIVITY_END ( )
}
	
namespace UPnPServiceCprUnregisterServiceActivity
{
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityDataClientIdle, UPnPServiceCprUnregisterService, TNodeSignal::TNullMessageId )
	FIRST_NODEACTIVITY_ENTRY ( TAwaitingServiceOrDeviceUnregister, TServiceOrDeviceTag )
	
	THROUGH_NODEACTIVITY_ENTRY ( KServiceTag, TDeleteServiceInfo, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KDeviceTag, TDeleteDeviceInfo, TNoTag )
	
    LAST_NODEACTIVITY_ENTRY ( KNoTag, CprStates::THandleDataClientIdle )
NODEACTIVITY_END ( )
}

namespace UpnpServiceCprSubscriptionTimerActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( TUpnpMessage::EActivitySubscriptionTimer, UpnpServiceCprSubscriptionTimer, TUpnpMessage::TStartTimer, CUpnpSubscriptionTimerActivity::NewL )
	FIRST_NODEACTIVITY_ENTRY ( CUpnpSubscriptionTimerActivity::TAwaitingStartTimer, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, CUpnpSubscriptionTimerActivity::TStartTimer, TTag< CoreStates::KLoopTag | NetStateMachine::EForward > )
	
	NODEACTIVITY_ENTRY ( CoreStates::KLoopTag, TDoNothing, CUpnpSubscriptionTimerActivity::TAwaitingStartOrStopTimer, CUpnpSubscriptionTimerActivity::TStartOrStopTag )	
	THROUGH_NODEACTIVITY_ENTRY ( UPnPStateMachine::KStart, TDoNothing, TTag< CoreStates::KLoopTag | NetStateMachine::EBackward > )
	
	THROUGH_NODEACTIVITY_ENTRY ( UPnPStateMachine::KStop, CUpnpSubscriptionTimerActivity::TDequeueClient, CUpnpSubscriptionTimerActivity::TLoopOrCancelTag )	
	LAST_NODEACTIVITY_ENTRY ( KCancelTag, TDoNothing )
NODEACTIVITY_END ( )
}


namespace UPnPServiceCprStartActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityStart, UPnPServiceCprStart, TCFServiceProvider::TStart, PRActivities::CStartActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingStart, CoreNetStates::TNoTagOrBearerPresentBlockedByStop)
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, CoreNetStates::TBindSelfToPresentBearer, CoreNetStates::TAwaitingBindToComplete, TTag<CoreNetStates::KBearerPresent>)
	NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendNoBearer, MeshMachine::TAwaitingMessageState<TCFControlProvider::TBearer>, TTag<CoreNetStates::KBearerPresent>)

	//Start the service provider, use the default cancellation.
	//Forward TCancel to the service provider, wait for TStarted or TError (via the Error Activity)
	//When TStarted arrives after TCancel the activity will move to the nearest KErrorTag
	NODEACTIVITY_ENTRY(CoreNetStates::KBearerPresent, CoreNetStates::TStartServiceProviderRetry, CoreNetStates::TAwaitingStarted, TNoTagOrErrorTag)
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


	
namespace UPnPServiceCprActivities
{
DEFINE_ACTIVITY_MAP ( activityMap )
	ACTIVITY_MAP_ENTRY ( UpnPServiceCprServiceJoinRequestActivity, UpnpServiceJoinRequest )
	ACTIVITY_MAP_ENTRY ( UPnPServiceCprNewServiceRegisterActivity, UpnpNewServiceRegister )
	ACTIVITY_MAP_ENTRY ( UPnPServiceCprUnregisterServiceActivity, UPnPServiceCprUnregisterService )
	ACTIVITY_MAP_ENTRY ( UpnpServiceCprSubscriptionTimerActivity, UpnpServiceCprSubscriptionTimer )
	ACTIVITY_MAP_ENTRY ( UPnPServiceCprStartActivity, UPnPServiceCprStart )
ACTIVITY_MAP_END_BASE ( CprActivities, coreCprActivities )
}


CNodeActivityBase* CUpnpNewServiceRegisterActivity::NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
	{
	return new ( ELeave ) CUpnpNewServiceRegisterActivity ( aActivitySig, aNode ) ;
	}

CUpnpNewServiceRegisterActivity::~CUpnpNewServiceRegisterActivity ( )
	{
	SetError ( KErrNone );
	}

DEFINE_SMELEMENT ( CUpnpNewServiceRegisterActivity::TRegisterRootDevice, NetStateMachine::MStateTransition, CUpnpNewServiceRegisterActivity::TContext )
void CUpnpNewServiceRegisterActivity::TRegisterRootDevice::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPNewSrvcRegActivity, KComponent, _L8("CUpnpNewServiceRegisterActivity::TRegisterRootDevice::DoL")));
	CUpnpNewServiceRegisterActivity& activity = static_cast< CUpnpNewServiceRegisterActivity& > ( *iContext.iNodeActivity );
	
	if ( activity.iOriginators.Count ( ) == KFirstRegistration )
		{
		// send data client to default SCPR to publish root device and start listening for root device.
		RNodeInterface* dc = iContext.Node ( ).GetFirstClient<TDefaultClientMatchPolicy> ( TClientType ( TCFClientType::EData, TCFClientType::EDefault ) );	
				
		const CUPnPRootDeviceInfoContainer& container = iContext.Node ( ).GetRootDeviceInfoContainer ( );
		const CUPnPDeviceDescription& deviceDesc = container.GetRootDeviceDesciption ( );
		//root device info
		CUPnPDevice* deviceObj = deviceDesc.DeviceObject ( );
		__ASSERT_DEBUG ( deviceObj != NULL, User::Invariant ( ) );
		
		LOG(ESockLogExternal::Printf(KSubsysSPNewSrvcRegActivity, KComponent, _L8("CUpnpNewServiceRegisterActivity::TRegisterRootDevice::DoL - Posting message TUpnpMessage::TRegisterRootDevice")));
		activity.PostRequestTo ( *dc, TUpnpMessage::TRegisterRootDevice ( deviceObj ).CRef () );
		}
	}
	
DEFINE_SMELEMENT ( CUpnpNewServiceRegisterActivity::TReplyToPendingServices, NetStateMachine::MStateTransition, CUpnpNewServiceRegisterActivity::TContext )
void CUpnpNewServiceRegisterActivity::TReplyToPendingServices::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPNewSrvcRegActivity, KComponent, _L8("CUpnpNewServiceRegisterActivity::TReplyToPendingServices::DoL")));
	TUpnpMessage::TRootDeviceRegistered& currentMsg = message_cast< TUpnpMessage::TRootDeviceRegistered > ( iContext.iMessage );
	
	if ( currentMsg.iValue == KErrNone )
		{
		iContext.Node ( ).ProcessRootDeviceRegisteredL ( );
		}
	LOG(ESockLogExternal::Printf(KSubsysSPNewSrvcRegActivity, KComponent, _L8("CUpnpNewServiceRegisterActivity::TReplyToPendingServices::DoL - Posting message TUpnpMessage::TServiceRegistered")));	
	TUpnpMessage::TServiceRegistered msg ( currentMsg.iValue );	
  	iContext.iNodeActivity->PostToOriginators( msg );
	}
	
DEFINE_SMELEMENT ( CUpnpNewServiceRegisterActivity::TReplyToCurrentService, NetStateMachine::MStateTransition, CUpnpNewServiceRegisterActivity::TContext )
void CUpnpNewServiceRegisterActivity::TReplyToCurrentService::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPNewSrvcRegActivity, KComponent, _L8("CUpnpNewServiceRegisterActivity::TReplyToCurrentService::DoL")));
	LOG(ESockLogExternal::Printf(KSubsysSPNewSrvcRegActivity, KComponent, _L8("CUpnpNewServiceRegisterActivity::TReplyToCurrentService::DoL - Posting message TUpnpMessage::TServiceRegistered")));
	TUpnpMessage::TServiceRegistered msg ( KErrNone );	
	RClientInterface::OpenPostMessageClose ( iContext.NodeId ( ), address_cast<TNodeCtxId> ( iContext.iSender ), msg );
  	}
	
	
CNodeActivityBase* CUpnpSubscriptionTimerActivity::NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
	{
	CUpnpSubscriptionTimerActivity* self = new ( ELeave ) CUpnpSubscriptionTimerActivity ( aActivitySig, aNode ) ;
	CleanupStack::PushL ( self );
	self->ConstructL( );
	CleanupStack::Pop ( self );
	return self;
	}
	
void CUpnpSubscriptionTimerActivity::ConstructL ( )	
	{
	// create the timer object
	iPeriodicTimer = CPeriodic::NewL ( CActive::EPriorityLow );	
	}

CUpnpSubscriptionTimerActivity::~CUpnpSubscriptionTimerActivity ( )
	{
	SetError ( KErrNone );
	if ( iPeriodicTimer )
		iPeriodicTimer->Cancel( );
	delete iPeriodicTimer;
	}

DEFINE_SMELEMENT ( CUpnpSubscriptionTimerActivity::TStartTimer, NetStateMachine::MStateTransition, CUpnpSubscriptionTimerActivity::TContext )
void CUpnpSubscriptionTimerActivity::TStartTimer::DoL ( )
	{	
	LOG(ESockLogExternal::Printf(KSubsysSPSubsTimerActivity, KComponent, _L8("CUpnpSubscriptionTimerActivity::TStartTimer::DoL")));
	CUpnpSubscriptionTimerActivity& activity = static_cast<CUpnpSubscriptionTimerActivity&> ( *iContext.iNodeActivity );
	
	TCallBack updateCallback ( &CUpnpSubscriptionTimerActivity::SubscriptionTimerCallBackL, &activity );
	activity.iPeriodicTimer->Start ( TTimeIntervalMicroSeconds32 ( KSubscriptionCleanupFrequency ), TTimeIntervalMicroSeconds32 ( KSubscriptionCleanupFrequency ), updateCallback );
	}
	
DEFINE_SMELEMENT ( CUpnpSubscriptionTimerActivity::TDequeueClient, NetStateMachine::MStateTransition, CUpnpSubscriptionTimerActivity::TContext )
void CUpnpSubscriptionTimerActivity::TDequeueClient::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPSubsTimerActivity, KComponent, _L8("CUpnpSubscriptionTimerActivity::TDequeueClient::DoL")));
	TInt pos = iContext.iNodeActivity->FindOriginator ( iContext.iSender );
	if ( pos != KErrNotFound )
		{		
		iContext.iNodeActivity->RemoveOriginator( pos );
		}
	}
	
DEFINE_SMELEMENT ( CUpnpSubscriptionTimerActivity::TStartOrStopTag, NetStateMachine::MStateFork, CUpnpSubscriptionTimerActivity::TContext )
TInt CUpnpSubscriptionTimerActivity::TStartOrStopTag::TransitionTag ( )
	{
	if  ( iContext.iMessage.IsMessage<TUpnpMessage::TStartTimer> ( ) )
		{
		return KStart;
		}	
	return KStop;
	}

DEFINE_SMELEMENT ( CUpnpSubscriptionTimerActivity::TLoopOrCancelTag, NetStateMachine::MStateFork, CUpnpSubscriptionTimerActivity::TContext )
TInt CUpnpSubscriptionTimerActivity::TLoopOrCancelTag::TransitionTag ( )
	{
	CUpnpSubscriptionTimerActivity& activity = static_cast<CUpnpSubscriptionTimerActivity&> ( *iContext.iNodeActivity );	
	return activity.iOriginators.Count() == 0 ? KCancelTag : (CoreStates::KLoopTag | NetStateMachine::EBackward);
	}


TInt CUpnpSubscriptionTimerActivity::SubscriptionTimerCallBackL ( TAny* aPtr )
	{
	CUpnpSubscriptionTimerActivity* activity = static_cast<CUpnpSubscriptionTimerActivity*> ( aPtr );
	TUint orgCount = activity->iOriginators.Count ( );
	
    if ( orgCount != 0 )
    	{
    	for ( TInt i = 0; i < orgCount; i++ )
	    	{
	    	TNodeId dc = address_cast<TNodeId> ( activity->iOriginators[i].RecipientId () );
 	    #if !defined(__GCCXML__)
 			CSubConnectionProviderBase& scprBase = mnode_cast<CSubConnectionProviderBase> ( dc.Node () );
 		#else
 			CSubConnectionProviderBase& scprBase = reinterpret_cast<CSubConnectionProviderBase&> ( dc.Node () );
 		#endif
 
 	    	CUPnPServiceSubConnectionProvider& subConnProv = static_cast<CUPnPServiceSubConnectionProvider&> ( scprBase );
 	    	subConnProv.DeprecateExpiredSubscriptions ();
	    	}
        }
    else
    	{
    	activity->SetIdle( );
    	activity->iPeriodicTimer->Cancel();
    	}   
    return KErrNone;
	}	
