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
#include <ss_glob.h>
#include <upnp/tupnptable.h>
#include <httpstringconstants.h>
#include <httperr.h>
#include <rmemchunk.h>

//Local Includes
#include "upnpflowbase.h"
#include "upnpctrlscpractivities.h"
#include "upnpctrlscprstates.h"
#include "upnpstatemachine.h"
#include "upnpserverconstants.h"
#include "upnplog.h"

using namespace MeshMachine;
using namespace CoreNetStates;
using namespace CoreStates;
using namespace UPnPControlSCprActivities;
using namespace PRActivities;
using namespace UPnPStateMachine;
using namespace UPnPControlSCprStates;


__FLOG_STMT(_LIT8(KComponent,"UPnPCPActivities");)


namespace UPnPControlSCprParamRequestActivity
{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityParamRequest, UPnPControlSCprParamRequest, TCFScpr::TSetParamsRequest )
	FIRST_NODEACTIVITY_ENTRY ( TAwaitingSetParamRequest, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TStoreParamsAndSendResponse )
NODEACTIVITY_END ()
#else
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityParamRequest, UPnPControlSCprParamRequest, TCFScpr::TParamsRequest )
	FIRST_NODEACTIVITY_ENTRY ( TAwaitingSetParamRequest, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TStoreParamsAndSendResponse )
NODEACTIVITY_END ()
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
}

namespace UPnPControlSCprApplyRequestActivity
{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityApplyChanges, UPnPControlSCprApplyRequest, TCFScpr::TApplyRequest )
	FIRST_NODEACTIVITY_ENTRY ( PRStates::TAwaitingApplyRequest, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, TInitiateActivities, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, PRStates::TSendApplyResponse )
NODEACTIVITY_END ()
#else
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityApplyChanges, UPnPControlSCprApplyRequest, TCFScpr::TApplyRequest )
	FIRST_NODEACTIVITY_ENTRY ( SCprStates::TAwaitingApplyRequest, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, TInitiateActivities, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, SCprStates::TSendApplyResponse )
NODEACTIVITY_END ()
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
}

namespace UPnPControlSCprClientLeaveActivity
{
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityClientLeave, UPnPControlSCprClientLeave, TNodeSignal::TNullMessageId ) //May be waiting for both messages
	FIRST_NODEACTIVITY_ENTRY ( TAwaitingClientLeave, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, CUPnPActivityBase::TProcessClientLeave, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, CoreNetStates::TSendDataClientIdle )
NODEACTIVITY_END ()
}

namespace UPnPControlSCprDestroyActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( ECFActivityDestroy, UPnPControlSCprDestroy, TEChild::TDestroy, CoreActivities::CDestroyActivity::New )
	FIRST_NODEACTIVITY_ENTRY ( TAwaitingDestroy, CUPnPActivityBase::TStopActivityOrStopDataClient)
	NODEACTIVITY_ENTRY ( UPnPStateMachine::KActivityStop, CUPnPActivityBase::TStopCurrentActivities, CUPnPActivityBase::TAwaitingResponseToStopActivities, CUPnPActivityBase::TStopActivityOrStopDataClient )	
	NODEACTIVITY_ENTRY ( CoreNetStates::KDataClientsToStop, UPnPStateMachine::TStopDataClients, UPnPStateMachine::TAwaitingToStopDataClients, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, CoreNetStates::TSendClientLeavingAndRemoveControlProvider )
NODEACTIVITY_END ( )
}

// Activity to create UPnP Flow for RSocket used in UPnP CONTROL Phase
namespace UPnPControlSCprBinderRequestActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( ECFActivityBinderRequest, UPnPControlSCprBinderRequest, TCFServiceProvider::TCommsBinderRequest, CCommsBinderActivity::NewL )
	FIRST_NODEACTIVITY_ENTRY ( CoreNetStates::TAwaitingBinderRequest, CCommsBinderActivity::TNoTagOrUseExistingBlockedByBinderRequest )
	NODEACTIVITY_ENTRY ( KNoTag, TCreateControlDataClient, CoreNetStates::TAwaitingDataClientJoin, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, CCommsBinderActivity::TProcessDataClientCreation, TTag<KUseExisting> )	
	NODEACTIVITY_ENTRY ( KUseExisting, CCommsBinderActivity::TSendBinderResponse, UPnPStateMachine::TAwaitingBindToComplete, TNoTagOrErrorTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TDoNothing )
	LAST_NODEACTIVITY_ENTRY ( KErrorTag, TClearError )
NODEACTIVITY_END ()
}

// Handle DataClientIdle from UPnP Flow created for RSocket used in UPnP CONTROL Phase
namespace UPnPControlSCprDataClientIdleActivity
{
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityDataClientIdle, UPnPControlSCprDataClientIdle, TCFControlProvider::TIdle )
    NODEACTIVITY_ENTRY ( KNoTag, UPnPStateMachine::THandleControlChannelDataClientIdle, CoreNetStates::TAwaitingDataClientIdle, TNoTag )
NODEACTIVITY_END ( )
}

// UPnP discovery activity
namespace UPnPSearchActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( TUpnpMessage::EActivitySearchRequest, UPnPSearchRequest, TUpnpMessage::TSearchRequest, CUPnPSearchActivity::NewL )
	FIRST_NODEACTIVITY_ENTRY ( CUPnPSearchActivity::TAwaitingSearchRequest, UPnPSearchStates::TFindUdpClientFlowOrSendRequest )	
	NODEACTIVITY_ENTRY ( KNoTag, TFindOrCreateUdpClientFlow, UPnPControlSCprStates::TAwaitingDataClientJoin, TErrorTagOr<TTag<KNoTag> > )
	LAST_NODEACTIVITY_ENTRY ( KErrorTag, TClearError )
	
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, PRStates::TProcessDataClientCreation, UPnPSearchStates::TStoreUdpClientFlowAndSendRequest )
	THROUGH_NODEACTIVITY_ENTRY ( KSendRequest, CUPnPSearchActivity::TSendSearchRequest, TTag<KLoopTag> )
	NODEACTIVITY_ENTRY ( KLoopTag, TDoNothing, CUPnPSearchActivity::TAwaitingReceiveSearchResponse, UPnPSearchStates::TCancelSearchOrProcessResponseTag )
	THROUGH_NODEACTIVITY_ENTRY ( KProcessResponse, CUPnPSearchActivity::TProcessResponse, TTag<KLoopTag | NetStateMachine::EBackward> )
	
	THROUGH_NODEACTIVITY_ENTRY ( KCancelTag, CUPnPSearchActivity::TCancelSearchRequest, TTag<KLoopTag> )
	NODEACTIVITY_ENTRY ( KLoopTag, TDoNothing, CUPnPSearchActivity::TAwaitingDataClientStopResponse, UPnPSearchStates::TNoTagOrResponseTag )
	THROUGH_NODEACTIVITY_ENTRY ( KProcessResponse, CUPnPSearchActivity::TCleanupActivity, TTag<KLoopTag | NetStateMachine::EBackward> )
	
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TClearError )
NODEACTIVITY_END ()
}

// UPnP register notify activity
namespace UPnPRegisterNotifyActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( TUpnpMessage::EActivityRegisterNotifyRequest, UPnPRegisterNotifyRequest, TUpnpMessage::TRegisterNotifyRequest, CUPnPRegisterNotifyActivity::NewL )
	FIRST_NODEACTIVITY_ENTRY ( CUPnPRegisterNotifyActivity::TAwaitingRegisterNotifyRequest, UPnPRegisterNotifyStates::TFindMUdpFlowOrSendRequest )
	
	NODEACTIVITY_ENTRY ( KNoTag, TFindOrCreateUdpServerFlow, UPnPControlSCprStates::TAwaitingDataClientJoin, TErrorTagOr<TTag<KNoTag> > )
	LAST_NODEACTIVITY_ENTRY ( KErrorTag, TClearError )
	
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, PRStates::TProcessDataClientCreation, UPnPRegisterNotifyStates::TStoreMUdpFlowAndStartServer )	
	NODEACTIVITY_ENTRY ( KNoTag, CUPnPRegisterNotifyActivity::TStartServer, CUPnPRegisterNotifyActivity::TAwaitingServerStarted, TTag<KSendRequest> )
	
	THROUGH_NODEACTIVITY_ENTRY ( KSendRequest, CUPnPRegisterNotifyActivity::TSendRegisterRequest, TTag<KLoopTag> )
	NODEACTIVITY_ENTRY ( KLoopTag, TDoNothing, CUPnPRegisterNotifyActivity::TAwaitingReceiveNotifivation, UPnPRegisterNotifyStates::TCancelRegisterOrProcessResponseTag )
	THROUGH_NODEACTIVITY_ENTRY ( KProcessResponse, CUPnPRegisterNotifyActivity::TProcessNotification, TTag< KLoopTag | NetStateMachine::EBackward > )

	THROUGH_NODEACTIVITY_ENTRY ( KCancelTag, CUPnPRegisterNotifyActivity::TCancelRegisterRequest, TTag< KLoopTag > )
	NODEACTIVITY_ENTRY ( KLoopTag, TDoNothing, CUPnPRegisterNotifyActivity::TAwaitingDataClientStopResponse, UPnPRegisterNotifyStates::TNoTagOrResponseTag )
	THROUGH_NODEACTIVITY_ENTRY ( KProcessResponse, CUPnPRegisterNotifyActivity::TCleanupActivity, TTag< KLoopTag | NetStateMachine::EBackward > )
	
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TClearError )
NODEACTIVITY_END ()
}    

// UPnP describe activity
namespace UPnPDescribeActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( TUpnpMessage::EActivityDescribeRequest, UPnPDescribeRequest, TUpnpMessage::TDescribeRequest, CUpnpDescribeActivity::NewL )
	FIRST_NODEACTIVITY_ENTRY ( CUpnpDescribeActivity::TAwaitingDescribeRequest, UPnPControlSCprStates::TFindHttpClientFlowOrSendRequest )
	
	NODEACTIVITY_ENTRY ( KNoTag, UPnPControlSCprStates::TFindOrCreateHttpClientFlow, UPnPControlSCprStates::TAwaitingDataClientJoin, TErrorTagOr<TTag<KNoTag> > )
	LAST_NODEACTIVITY_ENTRY ( KErrorTag, TClearError )
	
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, PRStates::TProcessDataClientCreation, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY( KNoTag, UPnPControlSCprStates::TProcessHttpDataClient, TTag<KSendRequest> )
	
	NODEACTIVITY_ENTRY ( KSendRequest, CUpnpDescribeActivity::TSendDescribeRequest, CUpnpDescribeActivity::TAwaitingErrorOrResponseOrCancel, UPnPDescribeStates::TCancelDescribeOrProcessResponseTag )
	LAST_NODEACTIVITY_ENTRY ( KProcessResponse, CUpnpDescribeActivity::TProcessResponse )
	LAST_NODEACTIVITY_ENTRY ( KErrorTag, CUpnpDescribeActivity::TReportError )
	
	NODEACTIVITY_ENTRY ( KCancelTag, CUpnpDescribeActivity::TDataClientStop, CUpnpDescribeActivity::TAwaitingDataClientStopResponse, UPnPDescribeStates::TCancelDescribeOrProcessResponseTag )
	LAST_NODEACTIVITY_ENTRY ( KProcessResponse, CUpnpDescribeActivity::TCleanupActivity )
	LAST_NODEACTIVITY_ENTRY ( KErrorTag, TClearError )
	
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TClearError )
NODEACTIVITY_END ( )
}

//UPnP subscribe activity.
namespace UpnpSubscribeActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( TUpnpMessage::EActivitySubscribeRequest, UPnPSubscribeRequest, TUpnpMessage::TSubscribeRequest, CUpnpSubscribeActivity::NewL )
	FIRST_NODEACTIVITY_ENTRY ( CUpnpSubscribeActivity::TAwaitingSubscribeRequest, UPnPControlSCprStates::TFindHttpClientFlowOrSendRequest )
	
	NODEACTIVITY_ENTRY ( KNoTag, UPnPControlSCprStates::TFindOrCreateHttpClientFlow, UPnPControlSCprStates::TAwaitingDataClientJoin, TErrorTagOr<TTag<KNoTag> > )
	LAST_NODEACTIVITY_ENTRY ( KErrorTag, TClearError )
	
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, PRStates::TProcessDataClientCreation, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, UPnPControlSCprStates::TProcessHttpDataClient, TTag<KSendRequest> )
	
	NODEACTIVITY_ENTRY ( KSendRequest, CUpnpSubscribeActivity::TSendSubscribeRequest, CUpnpSubscribeActivity::TAwaitingErrorOrResponseOrCancel, UPnPSubscribeStates::TErrorOrProcessResponseTag )
	
	// flow responds with TError or TSubscribeResponse
	LAST_NODEACTIVITY_ENTRY ( KErrorTag, CUpnpSubscribeActivity::TReportError )	
	THROUGH_NODEACTIVITY_ENTRY ( KProcessResponse, CUpnpSubscribeActivity::TProcessSubscribeResponse, UPnPSubscribeStates::TNoTagOrCancelSubscribe )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TDoNothing )
	
	// if node is in closing state, subscription is completed, then perform unsubscription
	NODEACTIVITY_ENTRY ( KCancelTag, CUpnpSubscribeActivity::TSendUnsubscribeRequest, CUpnpUnSubscribeActivity::TAwaitingErrorOrResponse, TNoTag )
	// even if received TError remove the subscription as already closing
	LAST_NODEACTIVITY_ENTRY ( KNoTag, CUpnpSubscribeActivity::TProcessUnsubscribeResponse )
NODEACTIVITY_END ( )
}

//UPnP unsubscribe activity.
namespace UpnpUnSubscribeActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( TUpnpMessage::EActivityUnSubscribeRequest, UPnPUnSubscribeRequest, TUpnpMessage::TCancelParamSet, CUpnpUnSubscribeActivity::NewL )
	FIRST_NODEACTIVITY_ENTRY ( CUpnpUnSubscribeActivity::TAwaitingUnSubscribeRequest, TNoTag )
	NODEACTIVITY_ENTRY ( KNoTag, CUpnpUnSubscribeActivity::TSendUnSubscribeRequest, CUpnpUnSubscribeActivity::TAwaitingErrorOrResponse, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, CUpnpUnSubscribeActivity::TProcessResponse )
NODEACTIVITY_END ( )
}


//UPnP notification activity.
namespace UpnpNotificationActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( TUpnpMessage::EActivityEventNotify, UPnPNotifyRequest, TUpnpMessage::TUPnPRequestInfo, CUpnpNotificationActivity::NewL )
	FIRST_NODEACTIVITY_ENTRY ( UPnPStateMachine::TAwaitingUpnpRequestInfo, UPnPStateMachine::TRequestOrIgnoreTag )
	THROUGH_NODEACTIVITY_ENTRY ( UPnPStateMachine::KNotifyRequest, CUpnpNotificationActivity::TProcessNotifyRequest, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, CUpnpNotificationActivity::TSendResponse )
	
	LAST_NODEACTIVITY_ENTRY ( KIgnore, UPnPStateMachine::TSendIgnoreResponse )
NODEACTIVITY_END ( )
}

// UPnP Flow finder.
namespace UpnpCallbackFlowFinderActivity
	{
DECLARE_DEFINE_NODEACTIVITY ( TUpnpMessage::EActivityFindOrCreateFlow, FindOrCreateFlow, TUpnpMessage::TCreateServerFlow )
	FIRST_NODEACTIVITY_ENTRY ( UPnPStateMachine::TAwaitingServerFlowRequest, TNoTag )
	
	NODEACTIVITY_ENTRY ( KNoTag, UPnPStateMachine::TCreateServerFlow<ECallback>, UPnPControlSCprStates::TAwaitingDataClientJoin, TErrorTagOr<TTag<KNoTag> > )
	LAST_NODEACTIVITY_ENTRY ( KErrorTag, UPnPControlSCprStates::TSendActivityStopped )
	
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, PRStates::TProcessDataClientCreation, TNoTag )
	NODEACTIVITY_ENTRY ( KNoTag, UPnPControlSCprStates::TProcessCallbackDataClient , CoreNetStates::TAwaitingDataClientStarted, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TDoNothing )
NODEACTIVITY_END ( )
	}

//ActivityMap
namespace UPnPControlSCprActivities
{
DEFINE_ACTIVITY_MAP ( activityMap )
	ACTIVITY_MAP_ENTRY ( UPnPControlSCprParamRequestActivity, UPnPControlSCprParamRequest )
	ACTIVITY_MAP_ENTRY ( UPnPControlSCprApplyRequestActivity, UPnPControlSCprApplyRequest )	
	ACTIVITY_MAP_ENTRY ( UPnPControlSCprBinderRequestActivity, UPnPControlSCprBinderRequest )
	ACTIVITY_MAP_ENTRY ( UPnPControlSCprDataClientIdleActivity, UPnPControlSCprDataClientIdle )
	ACTIVITY_MAP_ENTRY ( UPnPControlSCprClientLeaveActivity, UPnPControlSCprClientLeave )
	ACTIVITY_MAP_ENTRY ( UPnPSearchActivity, UPnPSearchRequest )
	ACTIVITY_MAP_ENTRY ( UPnPRegisterNotifyActivity, UPnPRegisterNotifyRequest )
	ACTIVITY_MAP_ENTRY ( UPnPDescribeActivity, UPnPDescribeRequest )
	ACTIVITY_MAP_ENTRY ( UpnpSubscribeActivity, UPnPSubscribeRequest )
	ACTIVITY_MAP_ENTRY ( UpnpUnSubscribeActivity, UPnPUnSubscribeRequest )
	ACTIVITY_MAP_ENTRY ( UpnpNotificationActivity, UPnPNotifyRequest )
	ACTIVITY_MAP_ENTRY ( UpnpCallbackFlowFinderActivity, FindOrCreateFlow )
	ACTIVITY_MAP_ENTRY ( UPnPControlSCprDestroyActivity, UPnPControlSCprDestroy )
ACTIVITY_MAP_END_BASE ( SCprActivities, coreSCprActivities )
}

//---------------------------------------Control Point Activity base class-------------------------------------

CUPnPActivityBase::~CUPnPActivityBase()
	{
	//This is the last upnp activity. Post TActivityStopped
	if ( static_cast<CUPnPControlSubConnectionProvider&>(iNode).IsClosing ( ) 
			&& iNode.CountAllActivities () == 1 ) // only - destroy and no upnp activity is pending
		{
		RClientInterface::OpenPostMessageClose ( iNode.Id (), iNode.Id (), TUpnpMessage::TActivityStopped ().CRef () );
		}
	};
	
TBool CUPnPActivityBase::Next ( TNodeContextBase& aContext )
	{
	if ( aContext.iMessage.IsMessage<TUpnpMessage::TCancelParamSet> () || aContext.iMessage.IsMessage<TUpnpMessage::TCancelRequest> () )
        	return ACore::Next ( &aContext );
	return CNodeActivityBase::Next ( aContext );
	}

DEFINE_SMELEMENT ( CUPnPActivityBase::TAwaitingResponseToStopActivities, NetStateMachine::MState, CUPnPActivityBase::TContext )
TBool CUPnPActivityBase::TAwaitingResponseToStopActivities::Accept ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysBaseActivity, KComponent, _L8("CUPnPActivityBase::TAwaitingResponseToStopActivities::Accept - Message=TActivityStopped")));
	if(iContext.iMessage.IsMessage<TUpnpMessage::TActivityStopped>())
		{
		//Check whether all UPnP activities are closed or not.
		if(iContext.iNode.CountAllActivities() > 1 )
			{
			return EFalse;
			}	
		return ETrue;	
		}
	return 	 EFalse;
	}

DEFINE_SMELEMENT ( CUPnPActivityBase::TStopCurrentActivities, NetStateMachine::MStateTransition, CUPnPActivityBase::TContext )
void CUPnPActivityBase::TStopCurrentActivities::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysBaseActivity, KComponent, _L8("CUPnPActivityBase::TStopCurrentActivities::DoL")));
	TInt activityCount = iContext.Node().CountAllActivities();
	//Cancel all activities and allow them to do cleanup 
	for( TInt index = 0; index < activityCount-1; index++ )
		{
		RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.NodeId (), TUpnpMessage::TCancelRequest ().CRef () );
		}		
	}

DEFINE_SMELEMENT ( CUPnPActivityBase::TStopActivityOrStopDataClient, NetStateMachine::MStateFork, CUPnPActivityBase::TContext )
TInt CUPnPActivityBase::TStopActivityOrStopDataClient::TransitionTag ( )
	{
	if ( iContext.Node().CountAllActivities() > 1 )
		{
		//There are some active data clients
		return UPnPStateMachine::KActivityStop;
		}
	if ( iContext.Node ().CountClients<TDefaultClientMatchPolicy> ( TClientType ( TCFClientType::EData) ) > 0 )
		{
		return CoreNetStates::KDataClientsToStop;
		}
	return KNoTag;
	}


//---------------------------------------Control Point Search Request Activity-------------------------------------
CNodeActivityBase* CUPnPSearchActivity::NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
	{
	TUint activityId = GetNextActivityCountL ( aActivitySig, aNode );
	return ( new ( ELeave ) CUPnPSearchActivity( aActivitySig, aNode, activityId ) );
	}

CUPnPSearchActivity::~CUPnPSearchActivity ( )
	{
	SetError ( KErrNone );
	delete static_cast<CUPnPDiscoverRequestParamSet*>(iRequestParamSet);
	}

DEFINE_SMELEMENT (CUPnPSearchActivity::TAwaitingReceiveSearchResponse, NetStateMachine::MState, CUPnPSearchActivity::TContext )
TBool CUPnPSearchActivity::TAwaitingReceiveSearchResponse::Accept ( )
	{
	if(iContext.iMessage.IsMessage<TUpnpMessage::TUPnPSearchResponse>())
		{
		LOG(ESockLogExternal::Printf(KSubsysSearchActivity, KComponent, _L8("CUPnPSearchActivity::TAwaitingReceiveSearchResponse::Accept - Message=TUpnpMessage::TUPnPSearchResponse")));
		return ETrue;
		}
	if(iContext.iMessage.IsMessage<TUpnpMessage::TCancelParamSet>())
		{
		LOG(ESockLogExternal::Printf(KSubsysSearchActivity, KComponent, _L8("CUPnPSearchActivity::TAwaitingReceiveSearchResponse::Accept - Message=TCancelParamSet")));
		CUPnPSearchActivity& activity = static_cast<CUPnPSearchActivity&> ( *iContext.iNodeActivity );
		TUpnpMessage::TCancelParamSet& msg = message_cast< TUpnpMessage::TCancelParamSet > ( iContext.iMessage );
		CUPnPRequestParamSetBase* paramSet = static_cast<CUPnPRequestParamSetBase*>(msg.iParamSet);
		if( activity.iRequestParamSet->Uri().CompareF(paramSet->Uri()) == 0
				&& paramSet->GetTypeId().iType == EUPnPCancelDiscoverParamSet )
			{
			delete static_cast<CUPnPCancelDiscoverParamSet*>(msg.iParamSet);		
			return ETrue;
			}
		}
	if(iContext.iMessage.IsMessage<TUpnpMessage::TCancelRequest>())
		{
		return ETrue;
		}
	return EFalse;
	}

DEFINE_SMELEMENT ( CUPnPSearchActivity::TProcessResponse, NetStateMachine::MStateTransition, CUPnPSearchActivity::TContext )
void CUPnPSearchActivity::TProcessResponse::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSearchActivity, KComponent, _L8("CUPnPSearchActivity::TProcessResponse::DoL")));
	
	TUpnpMessage::TUPnPSearchResponse& msg = message_cast< TUpnpMessage::TUPnPSearchResponse > ( iContext.iMessage );
	
	TUPnPEvent event;
	event.iStatus = TUPnPEvent::ESuccess;
	event.iErrorCode = KErrNone;
	RParameterFamily family ;
	CUPnPDiscoverResponseParamSet* resParamSet = CUPnPDiscoverResponseParamSet::NewL( family );
	CleanupStack::PushL( resParamSet );
	resParamSet->SetUPnPEvent( event );
	
	RBuf8 searchTarget;
	searchTarget.CreateMax(msg.iSsdpInfo.iSearchTarget.Length());
	msg.iSsdpInfo.iSearchTarget.CopyOut(searchTarget);
	msg.iSsdpInfo.iSearchTarget.Free();
	resParamSet->SetUriL(searchTarget);
	
	RBuf8 location;
	location.CreateMax(msg.iSsdpInfo.iLocation.Length());
	msg.iSsdpInfo.iLocation.CopyOut(location);
	msg.iSsdpInfo.iLocation.Free();
	resParamSet->SetDeviceLocationL(location);
	
	resParamSet->SetExpiryTime(msg.iSsdpInfo.iMx);
	
	RBuf8 usn;
	usn.CreateMax(msg.iSsdpInfo.iUsn.Length());
	msg.iSsdpInfo.iUsn.CopyOut(usn);
	msg.iSsdpInfo.iUsn.Free();
	resParamSet->SetUSNL(usn);
	
	iContext.Node().AddResponseParamSetL( resParamSet, EUPnPDiscoverResponseParamSet );
	CleanupStack::Pop( resParamSet );
	}

DEFINE_SMELEMENT ( CUPnPSearchActivity::TSendSearchRequest, NetStateMachine::MStateTransition, CUPnPSearchActivity::TContext )
void CUPnPSearchActivity::TSendSearchRequest::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSearchActivity, KComponent, _L8("CUPnPSearchActivity::TSendSearchRequest::DoL")));
	
	TAppProtAddr protAddr( KSsdpMulticastAddr, KUpnpMCPort );	
	CUPnPSearchActivity& activity = static_cast<CUPnPSearchActivity&> ( *iContext.iNodeActivity );
	activity.iFlowId = iContext.Node ().UdpClientFlow ();
	
	RMemChunk searchTarget;
	searchTarget.CreateL ( static_cast<CUPnPDiscoverRequestParamSet *> ( activity.iRequestParamSet )->Uri (), iContext.Node ( ).ConnectionProvider().MemoryAllocator());
	TSsdpInfo searchInfo ( static_cast<CUPnPDiscoverRequestParamSet *> ( activity.iRequestParamSet )->ResponseTime (), searchTarget);
	
	activity.PostRequestTo ( activity.iFlowId, TUpnpMessage::TUPnPSearchRequest ( searchInfo ).CRef () );
	}

DEFINE_SMELEMENT ( CUPnPSearchActivity::TCancelSearchRequest, NetStateMachine::MStateTransition, CUPnPRegisterNotifyActivity::TContext )
void CUPnPSearchActivity::TCancelSearchRequest::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSearchActivity, KComponent, _L8("CUPnPSearchActivity::TCancelSearchRequest::DoL")));
	
	RClientInterface::OpenPostMessageClose ( TNodeCtxId (  iContext.iNodeActivity->ActivityId (), iContext.NodeId () ), iContext.Node ().UdpClientFlow (),
											 TUpnpMessage::TCancelRequest ().CRef () );
	}

DEFINE_SMELEMENT ( CUPnPSearchActivity::TCleanupActivity, NetStateMachine::MStateTransition, CUPnPSearchActivity::TContext )
void CUPnPSearchActivity::TCleanupActivity::DoL ( )
	{
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TUPnPSearchResponse> () )
		{
		TUpnpMessage::TUPnPSearchResponse& msg = message_cast< TUpnpMessage::TUPnPSearchResponse > ( iContext.iMessage );
		msg.iSsdpInfo.iLocation.Free ();
		msg.iSsdpInfo.iUsn.Free ();
		}
	}
//---------------------------------------Register Notify Request Activity-------------------------------------
CNodeActivityBase* CUPnPRegisterNotifyActivity::NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
	{
	TUint activityId = GetNextActivityCountL ( aActivitySig, aNode );
	return ( new ( ELeave ) CUPnPRegisterNotifyActivity( aActivitySig, aNode, activityId ) );
	}

CUPnPRegisterNotifyActivity::~CUPnPRegisterNotifyActivity ( )
	{
	SetError ( KErrNone );
	delete static_cast<CUPnPRegisterAnnouncementParamSet *>(iRequestParamSet);
	}

DEFINE_SMELEMENT (CUPnPRegisterNotifyActivity::TAwaitingReceiveNotifivation, NetStateMachine::MState, CUPnPRegisterNotifyActivity::TContext )
TBool CUPnPRegisterNotifyActivity::TAwaitingReceiveNotifivation::Accept ( )
	{
	CUPnPRegisterNotifyActivity& activity = static_cast<CUPnPRegisterNotifyActivity&> ( *iContext.iNodeActivity );
	if( iContext.iMessage.IsMessage<TUpnpMessage::TUPnPPublishAliveRequest>() 
	 	|| iContext.iMessage.IsMessage<TUpnpMessage::TUPnPPublishByeRequest>() )
		{
		LOG(ESockLogExternal::Printf(KSubsysRegNotifyActivity, KComponent, _L8("CUPnPRegisterNotifyActivity::TAwaitingReceiveNotification::Accept - Message=TUpnpMessage::TUPnPPublishAliveRequest")));
		return ETrue;
		}
	if( iContext.iMessage.IsMessage<TUpnpMessage::TCancelParamSet>())
		{
		LOG(ESockLogExternal::Printf(KSubsysRegNotifyActivity, KComponent, _L8("CUPnPRegisterNotifyActivity::TAwaitingReceiveNotification::Accept - Message=TDataClientStop")));
		TUpnpMessage::TCancelParamSet& msg = message_cast< TUpnpMessage::TCancelParamSet > ( iContext.iMessage );
		CUPnPRequestParamSetBase* paramSet = static_cast<CUPnPRequestParamSetBase*>(msg.iParamSet);
		if( activity.iRequestParamSet->Uri().CompareF(paramSet->Uri()) == 0
				&& paramSet->GetTypeId().iType == EUPnPCancelRegisterNotifyParamSet )
			{
			delete static_cast<CUPnPCancelRegisterNotifyParamSet*>(msg.iParamSet);		
			return ETrue;
			}
		}
	if(iContext.iMessage.IsMessage<TUpnpMessage::TCancelRequest>())
		{
		LOG(ESockLogExternal::Printf(KSubsysRegNotifyActivity, KComponent, _L8("CUPnPRegisterNotifyActivity::TAwaitingReceiveNotification::Accept - Message=TUpnpMessage::TCancelRequest")));
		return ETrue;
		}
	return EFalse;
	}

DEFINE_SMELEMENT ( CUPnPRegisterNotifyActivity::TStartServer, NetStateMachine::MStateTransition, CUPnPRegisterNotifyActivity::TContext )
void CUPnPRegisterNotifyActivity::TStartServer::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysRegNotifyActivity, KComponent, _L8("CUPnPRegisterNotifyActivity::TStartServer::DoL")));		
	iContext.iNodeActivity->PostRequestTo ( iContext.Node ( ).UdpServerFlow ( ), TCFDataClient::TStart ().CRef () );
	}

DEFINE_SMELEMENT ( CUPnPRegisterNotifyActivity::TProcessNotification, NetStateMachine::MStateTransition, CUPnPRegisterNotifyActivity::TContext )
void CUPnPRegisterNotifyActivity::TProcessNotification::DoL ( )
	{
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TUPnPPublishAliveRequest> () )
		{
		LOG(ESockLogExternal::Printf(KSubsysRegNotifyActivity, KComponent, _L8("CUPnPRegisterNotifyActivity::TProcessNotification::DoL - Message=TUpnpMessage::TUPnPPublishAliveRequest")));
		TUpnpMessage::TUPnPPublishAliveRequest& notifyMsg = message_cast<TUpnpMessage::TUPnPPublishAliveRequest> ( iContext.iMessage );
		RParameterFamily family ;
	  	CUPnPPresenceAnnouncementParamSet*	notifyMsgParamSet = CUPnPPresenceAnnouncementParamSet::NewL ( family );
	  	CleanupStack::PushL ( notifyMsgParamSet );

		notifyMsgParamSet->SetDuration ( notifyMsg.iSsdpInfo.iMx );

		RBuf8 location;
		location.CreateMaxL ( notifyMsg.iSsdpInfo.iLocation.Length () );
		notifyMsg.iSsdpInfo.iLocation.CopyOut ( location );
		notifyMsg.iSsdpInfo.iLocation.Free ();
		notifyMsgParamSet->SetServiceLocationL ( location );

		RBuf8 usn;
		usn.CreateMaxL ( notifyMsg.iSsdpInfo.iUsn.Length() );
		notifyMsg.iSsdpInfo.iUsn.CopyOut ( usn );
		notifyMsg.iSsdpInfo.iUsn.Free ();
		notifyMsgParamSet->SetUSNL ( usn);

		RBuf8 searchTarget;
		searchTarget.CreateMaxL ( notifyMsg.iSsdpInfo.iSearchTarget.Length () );
		notifyMsg.iSsdpInfo.iSearchTarget.CopyOut ( searchTarget );
		notifyMsg.iSsdpInfo.iSearchTarget.Free ();
		notifyMsgParamSet->SetUriL ( searchTarget );
		
	  	CleanupStack::Pop ( notifyMsgParamSet );
		iContext.Node ().AddResponseParamSetL ( notifyMsgParamSet, EUPnPPresenceAnnouncementParamSet );
		}
		
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TUPnPPublishByeRequest> () )
		{
		LOG(ESockLogExternal::Printf(KSubsysRegNotifyActivity, KComponent, _L8("CUPnPRegisterNotifyActivity::TProcessNotification::DoL - Message=TUpnpMessage::TUPnPPublishByeRequest")));
		
		TUpnpMessage::TUPnPPublishByeRequest& notifyMsg = message_cast<TUpnpMessage::TUPnPPublishByeRequest> ( iContext.iMessage );
		RParameterFamily family ;
	  	CUPnPAbsenceAnnouncementParamSet* notifyMsgParamSet = CUPnPAbsenceAnnouncementParamSet::NewL ( family );
	  	CleanupStack::PushL ( notifyMsgParamSet );

		RBuf8 usn;
		usn.CreateMaxL ( ( notifyMsg.iSsdpInfo.iUsn ).Length () );
		notifyMsg.iSsdpInfo.iUsn.CopyOut ( usn );
		notifyMsg.iSsdpInfo.iUsn.Free ();
		notifyMsgParamSet->SetUSNL ( usn);

		RBuf8 searchTarget;
		searchTarget.CreateMaxL ( ( notifyMsg.iSsdpInfo.iSearchTarget ).Length () );
		notifyMsg.iSsdpInfo.iSearchTarget.CopyOut ( searchTarget );
		notifyMsg.iSsdpInfo.iSearchTarget.Free ();
		notifyMsgParamSet->SetUriL ( searchTarget );
		
	  	CleanupStack::Pop ( notifyMsgParamSet );
		iContext.Node ().AddResponseParamSetL ( notifyMsgParamSet, EUPnPAbsenceAnnouncementParamSet );
		}
		
	}

DEFINE_SMELEMENT ( CUPnPRegisterNotifyActivity::TCancelRegisterRequest, NetStateMachine::MStateTransition, CUPnPRegisterNotifyActivity::TContext )
void CUPnPRegisterNotifyActivity::TCancelRegisterRequest::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysRegNotifyActivity, KComponent, _L8("CUPnPRegisterNotifyActivity::TCancelRegisterRequest::DoL")));
	
	RClientInterface::OpenPostMessageClose ( TNodeCtxId (  iContext.iNodeActivity->ActivityId (), iContext.NodeId () ), iContext.Node ().UdpServerFlow (),
											 TUpnpMessage::TCancelRequest ().CRef () );	
	}

DEFINE_SMELEMENT ( CUPnPRegisterNotifyActivity::TSendRegisterRequest, NetStateMachine::MStateTransition, CUPnPRegisterNotifyActivity::TContext )
void CUPnPRegisterNotifyActivity::TSendRegisterRequest::DoL ( )
	{	
	LOG(ESockLogExternal::Printf(KSubsysRegNotifyActivity, KComponent, _L8("CUPnPRegisterNotifyActivity::TSendRegisterRequest::DoL")));	
	
	CUPnPRegisterNotifyActivity& activity = static_cast<CUPnPRegisterNotifyActivity&> ( *iContext.iNodeActivity );
	activity.iFlowId = iContext.Node().UdpServerFlow();
		
	RMemChunk notifyTarget;
	notifyTarget.CreateL ( activity.iRequestParamSet->Uri (), iContext.Node ( ).ConnectionProvider().MemoryAllocator() );
	TSsdpInfo registerInfo ( notifyTarget );
	
	TAppProtAddr protAddr ( KSsdpMulticastAddr, KUpnpMCPort );		
	activity.PostRequestTo ( activity.iFlowId, TUpnpMessage::TUPnPNotifyRegistration ( registerInfo ).CRef () );
	}
	
DEFINE_SMELEMENT ( CUPnPRegisterNotifyActivity::TCleanupActivity, NetStateMachine::MStateTransition, CUPnPRegisterNotifyActivity::TContext )
void CUPnPRegisterNotifyActivity::TCleanupActivity::DoL ( )
	{
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TUPnPPublishAliveRequest> () )
		{
		TUpnpMessage::TUPnPPublishAliveRequest& notifyMsg = message_cast<TUpnpMessage::TUPnPPublishAliveRequest> ( iContext.iMessage );
		notifyMsg.iSsdpInfo.iLocation.Free ();
		notifyMsg.iSsdpInfo.iUsn.Free ();
		notifyMsg.iSsdpInfo.iSearchTarget.Free ();
		}
	else if ( iContext.iMessage.IsMessage<TUpnpMessage::TUPnPPublishByeRequest> () )
		{
		TUpnpMessage::TUPnPPublishByeRequest& notifyMsg = message_cast<TUpnpMessage::TUPnPPublishByeRequest> ( iContext.iMessage );
		notifyMsg.iSsdpInfo.iUsn.Free ();
		notifyMsg.iSsdpInfo.iSearchTarget.Free ();
		}
	}

//==================================================Describe request activity==========================================//

CNodeActivityBase* CUpnpDescribeActivity::NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
	{
	TUint activityId = GetNextActivityCountL ( aActivitySig, aNode );
	return( new ( ELeave ) CUpnpDescribeActivity ( aActivitySig, aNode, activityId ) );
	}

CUpnpDescribeActivity::~CUpnpDescribeActivity ( )
	{
	SetError( KErrNone );
	delete static_cast<CUPnPDescribeRequestParamSet *>(iRequestParamSet);	
	}

// States
DEFINE_SMELEMENT( CUpnpDescribeActivity::TAwaitingErrorOrResponseOrCancel, NetStateMachine::MState, CUpnpDescribeActivity::TContext )
TBool CUpnpDescribeActivity::TAwaitingErrorOrResponseOrCancel::Accept ()
	{
	if ( iContext.iMessage.IsMessage<TEBase::TError > () )
		{
		LOG(ESockLogExternal::Printf(KSubsysDescribeActivity, KComponent, _L8("CUpnpDescribeActivity::TAwaitingErrorOrResponseOrCancel::Accept - Message=TCFMessage::TError")));
		iContext.iNodeActivity->SetError ( message_cast<TEBase::TError> ( iContext.iMessage ).iValue );
		return ETrue;
		}
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TResponse> () )
		{
		LOG(ESockLogExternal::Printf(KSubsysDescribeActivity, KComponent, _L8("CUpnpDescribeActivity::TAwaitingErrorOrResponseOrCancel::Accept - Message=TUpnpMessage::TResponse")));
		return ETrue;
		}
	if ( iContext.iMessage.IsMessage< TUpnpMessage::TCancelParamSet> () )
		{
		LOG(ESockLogExternal::Printf(KSubsysDescribeActivity, KComponent, _L8("CUpnpDescribeActivity::TCancelParamSet::Accept - Message=TCancelParamSet")));

		CUpnpDescribeActivity& activity = static_cast<CUpnpDescribeActivity&> ( *iContext.iNodeActivity );
		TUpnpMessage::TCancelParamSet& msg = message_cast< TUpnpMessage::TCancelParamSet > ( iContext.iMessage );
		CUPnPRequestParamSetBase* paramSet = static_cast<CUPnPRequestParamSetBase*> ( msg.iParamSet );
		if ( activity.iRequestParamSet->Uri ().CompareF ( paramSet->Uri () ) == 0 )
			{
			delete static_cast<CUPnPCancelDescribeParamSet*> ( msg.iParamSet );
			
			iContext.iMessage.ClearMessageId ( );
			return ETrue;
			}
		}
	 if ( iContext.iMessage.IsMessage<TUpnpMessage::TCancelRequest> () )
	 	{
		LOG(ESockLogExternal::Printf(KSubsysDescribeActivity, KComponent, _L8("CUpnpDescribeActivity::TCancelRequest::Accept - Message=TCancelRequest")));
		return ETrue; 	
	 	}
	
	return EFalse;	
	}


DEFINE_SMELEMENT (CUpnpDescribeActivity::TAwaitingDataClientStopResponse, NetStateMachine::MState, CUpnpDescribeActivity::TContext )
TBool CUpnpDescribeActivity::TAwaitingDataClientStopResponse::Accept ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysDescribeActivity, KComponent, _L8("CUpnpDescribeActivity::TAwaitingDataClientStopResponse::Accept - Message=TDataClientStopped")));
	return ( iContext.iMessage.IsMessage<TCFDataClient::TStopped> () 
			|| iContext.iMessage.IsMessage<TUpnpMessage::TResponse>()
			|| iContext.iMessage.IsMessage<TEBase::TError> () );
	}

// Transitions 
DEFINE_SMELEMENT( CUpnpDescribeActivity::TSendDescribeRequest, NetStateMachine::MStateTransition, CUpnpDescribeActivity::TContext )
void CUpnpDescribeActivity::TSendDescribeRequest::DoL()
	{
	LOG(ESockLogExternal::Printf(KSubsysDescribeActivity, KComponent, _L8("CUpnpDescribeActivity::TSendDescribeRequest::DoL")));
	CUpnpDescribeActivity& activity = static_cast< CUpnpDescribeActivity& >( *iContext.iNodeActivity );
	
	RMemChunk bodyBuf;
	RMemChunk uriBuf;
	uriBuf.CreateL ( activity.iRequestParamSet->Uri (), iContext.Node ( ).ConnectionProvider().MemoryAllocator() );		
	activity.PostRequestTo ( activity.iFlowId, TUpnpMessage::TUPnPClientRequestInfo ( HTTP::EGET, uriBuf, bodyBuf, NULL ).CRef () );
	}


DEFINE_SMELEMENT( CUpnpDescribeActivity::TProcessResponse, NetStateMachine::MStateTransition, CUpnpDescribeActivity::TContext )
void CUpnpDescribeActivity::TProcessResponse::DoL ()
	{
	LOG(ESockLogExternal::Printf(KSubsysDescribeActivity, KComponent, _L8("CUpnpDescribeActivity::TProcessResponse::DoL")));
	CUpnpDescribeActivity& activity = static_cast< CUpnpDescribeActivity& > ( *iContext.iNodeActivity );

	TUpnpMessage::TResponse& msg = message_cast< TUpnpMessage::TResponse > ( iContext.iMessage );

	TUPnPEvent event;
	event.iStatus = TUPnPEvent::ESuccess;
	event.iErrorCode = KErrNone;
	RParameterFamily family ;
	CUPnPDescribeResponseParamSet* resParamSet = CUPnPDescribeResponseParamSet::NewL ( family );
	CleanupStack::PushL ( resParamSet );
	resParamSet->SetUPnPEvent ( event );
	
	RBuf8 responseBodyData;
	responseBodyData.CreateMax ( msg.iBodyData.Length () );
	responseBodyData.FillZ ( msg.iBodyData.Length () );
	msg.iBodyData.CopyOut ( responseBodyData );	
	resParamSet->SetDescriptionL ( responseBodyData );
	resParamSet->SetUriL ( activity.iRequestParamSet->Uri () );
	responseBodyData.Close ();
	msg.iBodyData.Free ();		
	iContext.Node ().AddResponseParamSetL ( resParamSet, EUPnPDescribeResponseParamSet );
	CleanupStack::Pop ( resParamSet );
	}


void CUpnpDescribeActivity::TProcessResponse::Error ( TInt aError )
	{
	LOG(ESockLogExternal::Printf(KSubsysDescribeActivity, KComponent, _L8("CUpnpDescribeActivity::TProcessResponse::Error")));
	iContext.iNodeActivity->SetError ( aError );
	CUpnpDescribeActivity::TReportError reportError ( iContext );
	TRAP_IGNORE ( reportError.DoL () );
	}


DEFINE_SMELEMENT ( CUpnpDescribeActivity::TCleanupActivity, NetStateMachine::MStateTransition, CUpnpDescribeActivity::TContext )
void CUpnpDescribeActivity::TCleanupActivity::DoL ()
	{
	LOG(ESockLogExternal::Printf(KSubsysDescribeActivity, KComponent, _L8("CUpnpDescribeActivity::TCleanupActivity::DoL")));
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TResponse>() )
		{
		TUpnpMessage::TResponse& msg = message_cast< TUpnpMessage::TResponse > ( iContext.iMessage );
		if( !msg.iBodyData.IsEmpty () )
			{
			msg.iBodyData.Free ();
			}
		}
	}

DEFINE_SMELEMENT ( CUpnpDescribeActivity::TDataClientStop, NetStateMachine::MStateTransition, CUpnpDescribeActivity::TContext )
void CUpnpDescribeActivity::TDataClientStop::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysBaseActivity, KComponent, _L8("CUPnPSearchActivity::TDataClientStop::DoL")));

	CUpnpDescribeActivity& activity = static_cast< CUpnpDescribeActivity& > ( *iContext.iNodeActivity );
	iContext.iNodeActivity->PostRequestTo ( activity.iFlowId, TCFDataClient::TStop ( KErrNone ).CRef () );
	}

DEFINE_SMELEMENT ( CUpnpDescribeActivity::TReportError, NetStateMachine::MStateTransition, CUpnpDescribeActivity::TContext )
void CUpnpDescribeActivity::TReportError::DoL ()
	{
	LOG(ESockLogExternal::Printf(KSubsysDescribeActivity, KComponent, _L8("CUpnpDescribeActivity::TReportError::DoL")));
	CUpnpDescribeActivity& activity = static_cast< CUpnpDescribeActivity& > ( *iContext.iNodeActivity );

	TUPnPEvent event;
	event.iStatus = TUPnPEvent::EFail;
	event.iErrorCode = activity.Error ();
	
	if ( activity.Error() != KErrCancel )
		{
		RParameterFamily family ;
		CUPnPDescribeResponseParamSet* resParamSet = CUPnPDescribeResponseParamSet::NewL ( family );
		CleanupStack::PushL ( resParamSet );
		resParamSet->SetUPnPEvent ( event );
		resParamSet->SetUriL ( activity.iRequestParamSet->Uri () );
		resParamSet->SetDescriptionL ( KNullDesC8 );
		iContext.Node ().AddResponseParamSetL ( resParamSet, EUPnPDescribeResponseParamSet );
		CleanupStack::Pop ( resParamSet );
		}
	}


//---------------------------------------Control Point Subscribe Request Activity-------------------------------------
CNodeActivityBase* CUpnpSubscribeActivity::NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
	{
	TUint activityId = GetNextActivityCountL ( aActivitySig, aNode );
	return ( new ( ELeave ) CUpnpSubscribeActivity ( aActivitySig, aNode, activityId ) );
	}

CUpnpSubscribeActivity::~CUpnpSubscribeActivity ( )
	{
	SetError ( KErrNone );
	delete static_cast<CUPnPSubscribeRequestParamSet *> ( iRequestParamSet );	
	}


// States
DEFINE_SMELEMENT ( CUpnpSubscribeActivity::TAwaitingErrorOrResponseOrCancel, NetStateMachine::MState, CUpnpSubscribeActivity::TContext )
TBool CUpnpSubscribeActivity::TAwaitingErrorOrResponseOrCancel::Accept ()
	{
	if ( iContext.iMessage.IsMessage< TEBase::TError > () )
		{
		LOG(ESockLogExternal::Printf(KSubsysSubscribeActivity, KComponent, _L8("CUpnpSubscribeActivity::TAwaitingErrorOrResponseOrCancel::Accept - Message=TCFMessage::TError")));
		iContext.iNodeActivity->SetError ( message_cast<TEBase::TError> ( iContext.iMessage ).iValue );
		return ETrue;
		}
	if ( iContext.iMessage.IsMessage< TUpnpMessage::TSubscribeResponse > () )
		{
		LOG(ESockLogExternal::Printf(KSubsysSubscribeActivity, KComponent, _L8("CUpnpSubscribeActivity::TAwaitingErrorOrResponseOrCancel::Accept - Message=TUpnpMessage::TSubscribeResponse")));
		return ETrue;
		}
	if ( iContext.iMessage.IsMessage< TUpnpMessage::TCancelParamSet > () )
		{
		LOG(ESockLogExternal::Printf(KSubsysDescribeActivity, KComponent, _L8("CUpnpSubscribeActivity::TAwaitingErrorOrResponseOrCancel::Accept - Message=TCancelParamSet")));

		CUpnpSubscribeActivity& activity = static_cast<CUpnpSubscribeActivity&> ( *iContext.iNodeActivity );
		TUpnpMessage::TCancelParamSet& msg = message_cast< TUpnpMessage::TCancelParamSet > ( iContext.iMessage );
		CUPnPRequestParamSetBase* paramSet = static_cast<CUPnPRequestParamSetBase*> ( msg.iParamSet );
		if ( activity.iRequestParamSet->Uri ().CompareF ( paramSet->Uri () ) == 0 )
			{
			delete static_cast<CUPnPCancelSubscribeParamSet*> ( msg.iParamSet );
			
			// We can't send TCancel to the flow, we better eat the message
        	// here and switch the activity into a cancelling mode.
        	iContext.iMessage.ClearMessageId ( );
        	iContext.iNodeActivity->SetError ( KErrCancel );
			}
		}
	 if ( iContext.iMessage.IsMessage<TUpnpMessage::TCancelRequest> ()
	 		&& iContext.iNodeActivity->Error () != KErrCancel ) // in order to avoid eating more than one TCancelRequest Message
	 	{
		LOG(ESockLogExternal::Printf(KSubsysDescribeActivity, KComponent, _L8("CUpnpSubscribeActivity::AwaitingErrorOrResponseOrCancel::Accept - Message=TCancelRequest")));
		
		// We can't send TCancel to the Flow, we better eat the message
        // here and switch the activity into a cancelling mode.
        iContext.iMessage.ClearMessageId ( );
        iContext.iNodeActivity->SetError ( KErrCancel );
	 	}
	
	return EFalse;
	}

// Transitions	
DEFINE_SMELEMENT ( CUpnpSubscribeActivity::TSendSubscribeRequest, NetStateMachine::MStateTransition, CUpnpSubscribeActivity::TContext )
void CUpnpSubscribeActivity::TSendSubscribeRequest::DoL ()
	{
	LOG(ESockLogExternal::Printf(KSubsysSubscribeActivity, KComponent, _L8("CUpnpSubscribeActivity::TSendSubscribeRequest::DoL")));
	CUpnpSubscribeActivity& activity = static_cast<CUpnpSubscribeActivity&> ( *iContext.iNodeActivity );

	RMemChunk callBack;
	RMemChunk bodyBuf;
	RMemChunk uriBuf;
	RMemoryAllocator allocator = iContext.Node ( ).ConnectionProvider().MemoryAllocator();
	
	uriBuf.CreateL ( activity.iRequestParamSet->Uri ( ), allocator );
	TCleanupItem item ( &CUPnPUtils::CleanupMBufChain, &uriBuf );
	CleanupStack::PushL ( item );
	
	callBack.CreateL ( iContext.Node ( ).Callback ( ), allocator);
	TCleanupItem item1 ( &CUPnPUtils::CleanupMBufChain, &callBack );
	CleanupStack::PushL ( item1 );
	
	TSubscribeRequestInfo subscribeRequest;
	subscribeRequest.iTimeOut = static_cast<CUPnPSubscribeRequestParamSet *>(activity.iRequestParamSet)->Duration();
	
	CSubscriptionInfo* subscriptionInfo = iContext.Node ().GetSubscriptionInfoByUrl ( activity.iRequestParamSet->Uri () );
	if ( subscriptionInfo )
		{
		LOG(ESockLogExternal::Printf(KSubsysSubscribeActivity, KComponent, _L8("CUpnpSubscribeActivity::TSendSubscribeRequest::DoL - Re-Subscribe")));
		// Re-Subscribe.
		subscribeRequest.iSid.CreateL ( subscriptionInfo->iSid, allocator );
		}
	else
		{
		subscribeRequest.iCallback = callBack;
		}
		
	activity.PostRequestTo ( activity.iFlowId, TUpnpMessage::TUPnPClientRequestInfo ( UPnP::ESubscribe,  uriBuf, bodyBuf, &subscribeRequest ).CRef () );
	
	CleanupStack::Pop ( ); // item
	CleanupStack::Pop ( ); // item1
	}


DEFINE_SMELEMENT ( CUpnpSubscribeActivity::TProcessSubscribeResponse, NetStateMachine::MStateTransition, CUpnpSubscribeActivity::TContext )
void CUpnpSubscribeActivity::TProcessSubscribeResponse::DoL ()
	{
	LOG(ESockLogExternal::Printf(KSubsysSubscribeActivity, KComponent, _L8("CUpnpSubscribeActivity::TProcessResponse::DoL")));
	CUpnpSubscribeActivity& activity = static_cast< CUpnpSubscribeActivity& > ( *iContext.iNodeActivity );

	TUpnpMessage::TSubscribeResponse& msg = message_cast< TUpnpMessage::TSubscribeResponse > ( iContext.iMessage );	
	
	CSubscriptionInfo* subscriptionInfo = iContext.Node ().GetSubscriptionInfoByUrl ( activity.iRequestParamSet->Uri () );	
	// new subscription is accepted, update elements and create callback flow.
	if ( subscriptionInfo == NULL )
		{
		subscriptionInfo = new ( ELeave ) CSubscriptionInfo ( activity.iFlowId );
		CleanupStack::PushL ( subscriptionInfo );
		subscriptionInfo->SetPublishUrlL ( activity.iRequestParamSet->Uri() );
		subscriptionInfo->SetSidL ( msg.iURN );
		iContext.Node ().AddSubscriptionInfoL ( subscriptionInfo );
		CleanupStack::Pop ( subscriptionInfo );
			
		if ( iContext.iNodeActivity->Error () != KErrCancel && !iContext.Node ( ).iTcpSrvFlowCreateRequested )
			{
			iContext.Node ( ).iTcpSrvFlowCreateRequested = ETrue;
			// intiate flow creation with the call back Url			
			RClientInterface::OpenPostMessageClose ( iContext.NodeId (), TNodeCtxId ( TUpnpMessage::EActivityFindOrCreateFlow, iContext.NodeId () ), 
													TUpnpMessage::TCreateServerFlow ().CRef() );
			}
		}
	
	if ( iContext.iNodeActivity->Error() != KErrCancel )
		{
		RParameterFamily family ;
		CUPnPSubscribeResponseParamSet* resParamSet = CUPnPSubscribeResponseParamSet::NewL ( family );
		CleanupStack::PushL ( resParamSet );
	
		TUPnPEvent event;
		event.iStatus = TUPnPEvent::ESuccess;
		event.iErrorCode = KErrNone;
			
		resParamSet->SetUPnPEvent ( event );
		resParamSet->SetTimerExpiry ( msg.iTimeOut );
		resParamSet->SetUriL ( activity.iRequestParamSet->Uri () );
		iContext.Node ( ).AddResponseParamSetL ( resParamSet, EUPnPSubscribeResponseParamSet );
		CleanupStack::Pop ( resParamSet );
		}
	
	activity.iSubscriptionInfo = subscriptionInfo; // for unsubscription in cancel/destruction phase	
	msg.iURN.Free ( );	
	}


void CUpnpSubscribeActivity::TProcessSubscribeResponse::Error( TInt aError )
	{
	LOG(ESockLogExternal::Printf(KSubsysSubscribeActivity, KComponent, _L8("CUpnpSubscribeActivity::TProcessResponse::Error")));
	iContext.iNodeActivity->SetError( aError );
	CUpnpSubscribeActivity::TReportError reportError(iContext);
	TRAP_IGNORE ( reportError.DoL() );
	}
	
DEFINE_SMELEMENT( CUpnpSubscribeActivity::TReportError, NetStateMachine::MStateTransition, CUpnpSubscribeActivity::TContext )
void CUpnpSubscribeActivity::TReportError::DoL()
	{
	LOG(ESockLogExternal::Printf(KSubsysSubscribeActivity, KComponent, _L8("CUpnpSubscribeActivity::TReportError::DoL")));
	CUpnpSubscribeActivity& activity = static_cast< CUpnpSubscribeActivity& > ( *iContext.iNodeActivity );

	TUPnPEvent event;
	event.iStatus = TUPnPEvent::EFail;
	event.iErrorCode = activity.Error();
	
	CSubscriptionInfo* subscriptionInfo = iContext.Node().GetSubscriptionInfoByUrl(activity.iRequestParamSet->Uri());
	if(subscriptionInfo)
		{
		iContext.Node().RemoveSubscriptionInfo(subscriptionInfo->iSid);
		}
	
	if( activity.Error() != KErrCancel )
		{
		RParameterFamily family ;
		CUPnPSubscribeResponseParamSet* resParamSet = CUPnPSubscribeResponseParamSet::NewL( family );
		CleanupStack::PushL( resParamSet );
		resParamSet->SetUriL( activity.iRequestParamSet->Uri() );
		resParamSet->SetUPnPEvent( event );
		iContext.Node().AddResponseParamSetL( resParamSet, EUPnPSubscribeResponseParamSet );
		CleanupStack::Pop( resParamSet );
		}
	}
	

DEFINE_SMELEMENT( CUpnpSubscribeActivity::TSendUnsubscribeRequest, NetStateMachine::MStateTransition, CUpnpSubscribeActivity::TContext )
void CUpnpSubscribeActivity::TSendUnsubscribeRequest::DoL()
	{
	LOG(ESockLogExternal::Printf(KSubsysSubscribeActivity, KComponent, _L8("CUpnpSubscribeActivity::TSendUnSubscribeRequest::DoL")));
	CUpnpSubscribeActivity& activity = static_cast< CUpnpSubscribeActivity& > ( *iContext.iNodeActivity );
	RMemoryAllocator allocator = iContext.Node ( ).ConnectionProvider().MemoryAllocator();
	
	TSubscribeRequestInfo subscribeRequest;
	subscribeRequest.iSid.CreateL ( activity.iSubscriptionInfo->iSid, allocator );
	TCleanupItem item ( &CUPnPUtils::CleanupMBufChain, &subscribeRequest.iSid );
	CleanupStack::PushL ( item );
		
	RMemChunk uriBuf;
	uriBuf.CreateL ( activity.iSubscriptionInfo->iPublishUrl, allocator );
	
	RMemChunk bodyBuf;	
	activity.PostRequestTo ( activity.iSubscriptionInfo->iDataClient, 
							TUpnpMessage::TUPnPClientRequestInfo ( UPnP::EUnsubscribe, uriBuf, bodyBuf, &subscribeRequest ).CRef () );
	
	CleanupStack::Pop ( ); // item
	}

DEFINE_SMELEMENT( CUpnpSubscribeActivity::TProcessUnsubscribeResponse, NetStateMachine::MStateTransition, CUpnpSubscribeActivity::TContext )
void CUpnpSubscribeActivity::TProcessUnsubscribeResponse::DoL()
	{
	LOG(ESockLogExternal::Printf(KSubsysUnSubscribeActivity, KComponent, _L8("CUpnpUnSubscribeActivity::TProcessResponse::DoL")));
	
	CUpnpSubscribeActivity& activity = static_cast< CUpnpSubscribeActivity& >( *iContext.iNodeActivity );	
	iContext.Node().RemoveSubscriptionInfo(activity.iSubscriptionInfo->iSid);
	}


//---------------------------------------Control Point UnSubscribe Request Activity-------------------------------------
CNodeActivityBase* CUpnpUnSubscribeActivity::NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
	{
	TUint activityId = GetNextActivityCountL ( aActivitySig, aNode );
	return ( new ( ELeave ) CUpnpUnSubscribeActivity ( aActivitySig, aNode, activityId ) );
	}

CUpnpUnSubscribeActivity::~CUpnpUnSubscribeActivity ( )
	{	
	SetError( KErrNone );
	delete static_cast<CUPnPCancelSubscribeParamSet*> ( iRequestParamSet );	
	}


// States
DEFINE_SMELEMENT( CUpnpUnSubscribeActivity::TAwaitingUnSubscribeRequest, NetStateMachine::MState, CUpnpUnSubscribeActivity::TContext )
TBool CUpnpUnSubscribeActivity::TAwaitingUnSubscribeRequest::Accept()
	{
	LOG(ESockLogExternal::Printf(KSubsysUnSubscribeActivity, KComponent, _L8("CUpnpUnSubscribeActivity::TAwaitingUnSubscribeRequest::Accept - Message=TCancelParamSet")));
	
	if ( iContext.iMessage.IsMessage< TUpnpMessage::TCancelParamSet >() )
		{
		TUpnpMessage::TCancelParamSet& msg = message_cast< TUpnpMessage::TCancelParamSet > ( iContext.iMessage );	
		
		if ( EUPnPCancelSubscribeParamSet == msg.iParamSet->GetTypeId().iType )
			{
			CUPnPCancelSubscribeParamSet* paramSet = static_cast<CUPnPCancelSubscribeParamSet*> ( msg.iParamSet );
			
			CSubscriptionInfo* info = iContext.Node ().GetSubscriptionInfoByUrl ( paramSet->Uri () );
			if ( info ) // subscription response is received from network, accept for performing unsubscribe.
				{
				return ETrue;
				}
			}
		}
	
	return EFalse;	
	}

// Transitions.
DEFINE_SMELEMENT( CUpnpUnSubscribeActivity::TSendUnSubscribeRequest, NetStateMachine::MStateTransition, CUpnpUnSubscribeActivity::TContext )
void CUpnpUnSubscribeActivity::TSendUnSubscribeRequest::DoL()
	{
	LOG(ESockLogExternal::Printf(KSubsysUnSubscribeActivity, KComponent, _L8("CUpnpUnSubscribeActivity::TSendUnSubscribeRequest::DoL")));
	CUpnpUnSubscribeActivity& activity = static_cast< CUpnpUnSubscribeActivity& > ( *iContext.iNodeActivity );
	TUpnpMessage::TCancelParamSet& msg = message_cast< TUpnpMessage::TCancelParamSet > ( iContext.iMessage );	
	
	activity.iRequestParamSet = static_cast<CUPnPCancelSubscribeParamSet*> ( msg.iParamSet );	
	CSubscriptionInfo* subscriptionInfo = iContext.Node ( ).GetSubscriptionInfoByUrl ( activity.iRequestParamSet->Uri( ) );	
	activity.iSubscriptionInfo = subscriptionInfo;
	RMemoryAllocator allocator = iContext.Node ( ).ConnectionProvider().MemoryAllocator();
	
	TSubscribeRequestInfo subscribeRequest;
	subscribeRequest.iSid.CreateL ( subscriptionInfo->iSid, allocator );
	TCleanupItem item ( &CUPnPUtils::CleanupMBufChain, &subscribeRequest.iSid );
	CleanupStack::PushL ( item );
	
	RMemChunk uriBuf;
	uriBuf.CreateL ( subscriptionInfo->iPublishUrl, allocator );
	
	RMemChunk bodyBuf;	
	activity.PostRequestTo ( subscriptionInfo->iDataClient, 
							TUpnpMessage::TUPnPClientRequestInfo ( UPnP::EUnsubscribe,  uriBuf, bodyBuf, &subscribeRequest ).CRef () );
	
	CleanupStack::Pop ( ); // item
	}


DEFINE_SMELEMENT( CUpnpUnSubscribeActivity::TProcessResponse, NetStateMachine::MStateTransition, CUpnpUnSubscribeActivity::TContext )
void CUpnpUnSubscribeActivity::TProcessResponse::DoL()
	{
	LOG(ESockLogExternal::Printf(KSubsysUnSubscribeActivity, KComponent, _L8("CUpnpUnSubscribeActivity::TProcessResponse::DoL")));
	
	CUpnpUnSubscribeActivity& activity = static_cast< CUpnpUnSubscribeActivity& >( *iContext.iNodeActivity );	
	iContext.Node().RemoveSubscriptionInfo(activity.iSubscriptionInfo->iSid);
	}

//---------------------------------------Control Point Notification Activity-------------------------------------


// CUpnpNotificationActivity
CNodeActivityBase* CUpnpNotificationActivity::NewL( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
	{
	TUint activityId = GetNextActivityCountL ( aActivitySig, aNode );
	return ( new ( ELeave ) CUpnpNotificationActivity ( aActivitySig, aNode, activityId ) );
	}


CUpnpNotificationActivity::~CUpnpNotificationActivity()
	{
	iBodyBuf.Close();
	SetError( KErrNone );
	}


CUpnpNotificationActivity::CUpnpNotificationActivity( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aActivityCount )
	:CNodeParallelActivityBase ( aActivitySig, aNode, aActivityCount )
	{
	// do nothing.
	}

// Transitions
DEFINE_SMELEMENT( CUpnpNotificationActivity::TProcessNotifyRequest, NetStateMachine::MStateTransition, CUpnpNotificationActivity::TContext )
void CUpnpNotificationActivity::TProcessNotifyRequest::DoL()
	{
	LOG(ESockLogExternal::Printf(KSubsysEventNotifyActivity, KComponent, _L8("CUpnpNotificationActivity::TProcessNotifyRequest::DoL")));
	
	CUpnpNotificationActivity& activity = static_cast< CUpnpNotificationActivity& > ( *iContext.iNodeActivity );
	
	TUpnpMessage::TUPnPRequestInfo& msg = message_cast< TUpnpMessage::TUPnPRequestInfo > ( iContext.iMessage );
	activity.iTransaction = static_cast<CTransaction*>(msg.iPtr);
	CRequest* request = activity.iTransaction->Request();

	// Validate request & construct response
	RStringPool strP = request->StringPool();
	RHTTPHeaders reqHdrs = request->Handle().GetHeaderCollection();
	RStringF sid = strP.StringF(UPnP::ESID, TUPnPTable::Table());
	RStringF nts = strP.StringF(UPnP::ENTS, TUPnPTable::Table());
	RStringF nt = strP.StringF(UPnP::ENT, TUPnPTable::Table());
	RStringF seq = strP.StringF(UPnP::ESEQ, TUPnPTable::Table());
	THTTPHdrVal ntsVal;
	THTTPHdrVal ntVal;
	THTTPHdrVal	sidVal;
	TBool preConditionFailed = ETrue;
	TBool invalidSequence = ETrue;
	
	// Check if the SID header is present.
	if( reqHdrs.GetField( sid, 0, sidVal ) != KErrNotFound )
		{
		CSubscriptionInfo* subscriptionInfo = iContext.Node ().GetSubscriptionInfoBySid( sidVal.StrF().DesC() );
		if(subscriptionInfo != NULL)
			{
			THTTPHdrVal seqVal;
			if( (reqHdrs.GetField(seq, 0, seqVal) != KErrNotFound) && (seqVal.Int() == subscriptionInfo->iSeqKey+1) )
				{
				invalidSequence = EFalse;
				subscriptionInfo->iSeqKey = seqVal.Int();
				activity.iPublisherUrl.Set(subscriptionInfo->iPublishUrl.Ptr(),subscriptionInfo->iPublishUrl.Length());
				}
			preConditionFailed = EFalse;
			}
		else
			{
			preConditionFailed = ETrue;
			}
		}
	else
		{
		preConditionFailed = ETrue;
		}
	
	if(preConditionFailed) 
		{
		activity.iResponseStatus = KPreConditionFailedStatusCode;
		}
	else if(invalidSequence)
		{
		activity.iResponseStatus = KInvalidSeqStatusCode;
		}
	else
		{
		activity.iResponseStatus = KOkStatusCode;
		
		activity.iBodyBuf.Close();
		TInt bodyBufLen = activity.iTransaction->BodyParts().Length();
		activity.iBodyBuf.CreateMaxL(bodyBufLen);
		activity.iBodyBuf.FillZ(bodyBufLen);
		activity.iTransaction->BodyParts().CopyOut(activity.iBodyBuf);
		activity.iTransaction->RemoveAllBodyParts( );
		}
	}


DEFINE_SMELEMENT( CUpnpNotificationActivity::TSendResponse, NetStateMachine::MStateTransition, CUpnpNotificationActivity::TContext )
void CUpnpNotificationActivity::TSendResponse::DoL()
	{
	LOG(ESockLogExternal::Printf(KSubsysEventNotifyActivity, KComponent, _L8("CUpnpNotificationActivity::TSendResponse::DoL")));
	CUpnpNotificationActivity& activity = static_cast< CUpnpNotificationActivity& > ( *iContext.iNodeActivity );
	
	if ( activity.iResponseStatus == KOkStatusCode ||
		 activity.iResponseStatus == KInvalidSeqStatusCode	 )
		{
		TUPnPEvent event;
		event.iStatus = TUPnPEvent::ESuccess;
		event.iErrorCode = KErrNone;
		RParameterFamily family ;
		CUPnPNotifyEventParamSet* notifyEventParamSet = CUPnPNotifyEventParamSet::NewL( family );
		CleanupStack::PushL( notifyEventParamSet );
			
		if (activity.iResponseStatus == KOkStatusCode)
			{			
			notifyEventParamSet->SetNotificationL(activity.iBodyBuf);
			}		
		else if (activity.iResponseStatus == KInvalidSeqStatusCode)
			{
			// - should have constants for all the status codes.
			event.iErrorCode = KInvalidSeqStatusCode;
			}
		notifyEventParamSet->SetUPnPEvent(event);
		notifyEventParamSet->SetUriL(activity.iPublisherUrl);
		
		iContext.Node().AddResponseParamSetL( notifyEventParamSet, EUPnPNotifyEventParamSet );
		CleanupStack::Pop(notifyEventParamSet);
		}				
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.Node ().TcpServerFlow (),
											TUpnpMessage::TUPnPResponseInfo ( activity.iTransaction, activity.iResponseStatus, NULL ).CRef () );
	}
