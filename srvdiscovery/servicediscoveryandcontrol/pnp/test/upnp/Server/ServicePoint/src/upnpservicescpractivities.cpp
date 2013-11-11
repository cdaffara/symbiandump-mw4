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
#include <comms-infras/ss_nodemessages_factory.h>
#include <ss_glob.h>
#include <httpstringconstants.h>
#include <http/thttptable.h>
#include <upnp/tupnptable.h>
#include <inetprottextutils.h>
#include "upnpdescriptionschema.h"
#include <httperr.h>
#include <rmemchunk.h>

//Local Includes
#include "upnpservicescpractivities.h"
#include "upnpservicescprstates.h"
#include "upnpctrlscprstates.h"
#include "upnpstatemachine.h"
#include "upnpuuid.h"
#include "upnpserverconstants.h"
#include "upnplog.h"

using namespace UPnPServiceSCprActivities;
using namespace ESock;
using namespace MeshMachine;
using namespace CoreNetStates;
using namespace CoreStates;
using namespace CorePanics;
using namespace PRActivities;
using namespace CoreActivities;
using namespace UPnPStateMachine;
using namespace UPnPServiceSCprStates;

__FLOG_STMT(_LIT8(KComponent,"UPnPSPScpr");)
const TUint KHttpDefaultPort = 80;

namespace UPnPServiceSCprParamRequestActivity
{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityParamRequest, UPnPServiceSCprParamRequest, TCFScpr::TSetParamsRequest )
	FIRST_NODEACTIVITY_ENTRY ( TAwaitingSetParamRequest, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, UPnPServiceSCprStates::TStoreParamsAndSendResponse )
NODEACTIVITY_END ( )
#else
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityParamRequest, UPnPServiceSCprParamRequest, TCFScpr::TParamsRequest )
	FIRST_NODEACTIVITY_ENTRY ( TAwaitingSetParamRequest, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, UPnPServiceSCprStates::TStoreParamsAndSendResponse )
NODEACTIVITY_END ( )
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
}

namespace UPnPServiceSCprApplyRequestActivity
{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityApplyChanges, UPnPServiceSCprApplyRequest, TCFScpr::TApplyRequest )
	FIRST_NODEACTIVITY_ENTRY ( PRStates::TAwaitingApplyRequest, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, UPnPServiceSCprStates::TInitiateActivities, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, PRStates::TSendApplyResponse )
NODEACTIVITY_END ( )
#else
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityApplyChanges, UPnPServiceSCprApplyRequest, TCFScpr::TApplyRequest )
	FIRST_NODEACTIVITY_ENTRY ( SCprStates::TAwaitingApplyRequest, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, UPnPServiceSCprStates::TInitiateActivities, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, SCprStates::TSendApplyResponse )
NODEACTIVITY_END ( )
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
}

//Activity to create UPnP Server Flow for RSocket used in UPnP Control Phase
namespace UPnPServiceSCprBinderRequestActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( ECFActivityBinderRequest, UPnPServiceSCprBinderRequest, TCFServiceProvider::TCommsBinderRequest, CCommsBinderActivity::NewL )
	FIRST_NODEACTIVITY_ENTRY ( CoreNetStates::TAwaitingBinderRequest, CCommsBinderActivity::TNoTagOrUseExistingBlockedByBinderRequest )
	
	NODEACTIVITY_ENTRY ( KNoTag, TCreateServerFlow<EControl>, CoreNetStates::TAwaitingDataClientJoin, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, CCommsBinderActivity::TProcessDataClientCreation, TNoTag )
	
	NODEACTIVITY_ENTRY ( KNoTag, TSendDataClientStart, CoreNetStates::TAwaitingDataClientStarted, TTag<KUseExisting> )
	NODEACTIVITY_ENTRY ( CoreStates::KUseExisting, CCommsBinderActivity::TSendBinderResponse, CCommsBinderActivity::TAwaitingBindToComplete, TNoTagOrErrorTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TDoNothing )
	
	LAST_NODEACTIVITY_ENTRY ( KErrorTag, TClearError )
NODEACTIVITY_END ( )
}

// Note! Handle DataClientIdle from UPnP Flow created for RSocket used in UPnP CONTROL Phase
namespace UPnPServiceSCprDataClientIdleActivity
{
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityDataClientIdle, UPnPServiceSCprDataClientIdle, TCFControlProvider::TIdle )
    NODEACTIVITY_ENTRY ( KNoTag, UPnPStateMachine::THandleControlChannelDataClientIdle, CoreNetStates::TAwaitingDataClientIdle, TNoTag )
NODEACTIVITY_END ( )
}


namespace UPnPServiceSCprClientLeaveActivity
{
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityClientLeave, UPnPServiceSCprClientLeave, TNodeSignal::TNullMessageId ) //May be waiting for both messages
	FIRST_NODEACTIVITY_ENTRY ( CoreStates::TAwaitingClientLeave, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, UPnPServiceSCprStates::TProcessClientLeave, TTag<KDataClientIdle> )
	LAST_NODEACTIVITY_ENTRY ( KDataClientIdle, TSendDataClientIdleOrUnregister )
NODEACTIVITY_END ( )
}


namespace UPnPServiceSCprDataClientStopActivity
{
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityStopDataClient, UPnPServiceSCprDataClientStop, TCFDataClient::TStop  )
	FIRST_NODEACTIVITY_ENTRY ( CoreNetStates::TAwaitingDataClientStop, CoreNetStates::TNoTagOrDataClientsToStop )
	
	NODEACTIVITY_ENTRY ( CoreNetStates::KDataClientsToStop, UPnPServiceSCprStates::TProcessDataClientStop, CoreNetStates::TAwaitingDataClientsStopped, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, TDoNothing, CoreNetStates::TNoTagOrUnbindOnStop )
	
	NODEACTIVITY_ENTRY ( CoreNetStates::KUnbind, CoreNetStates::TSendClientLeavingRequestToServiceProvider, CoreStates::TAwaitingLeaveComplete, TNoTag )
	
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, UPnPStateMachine::TDestroyDataClients, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, PRStates::TSendDataClientStopped )
NODEACTIVITY_END ( )
}

namespace UPnPServiceSCprDestroyActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( ECFActivityDestroy, UPnPServiceSCprDestroy, TEChild::TDestroy, CoreActivities::CDestroyActivity::New )
	FIRST_NODEACTIVITY_ENTRY ( TAwaitingDestroy, UPnPStateMachine::TBlockedByAllActivitiesStopped )
	
	//Stop self first
    NODEACTIVITY_ENTRY ( KNoTag, CoreNetStates::TStopSelf, CoreNetStates::TAwaitingDataClientStopped, TNoTag )
	
    LAST_NODEACTIVITY_ENTRY ( KNoTag, CoreNetStates::TSendClientLeavingAndRemoveControlProvider )
NODEACTIVITY_END()
}


namespace UPnPServiceScprServiceRegistrationActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( TUpnpMessage::EActivityServiceRegistration, UPnPServiceSCprServiceRegistration, TUpnpMessage::TServiceRegistration, CUpnpServiceRegistrationActivity::NewL )
	FIRST_NODEACTIVITY_ENTRY ( CUpnpServiceRegistrationActivity::TAwaitingServiceRegistration, TNoTag )
	
	NODEACTIVITY_ENTRY ( KNoTag, CUpnpServiceRegistrationActivity::TRequestServiceJoin, TAwaitingServiceValidated, TErrorTagOr<TTag<KNoTag> > )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, CUpnpServiceRegistrationActivity::TBuildPublishAndServiceInfo, TNoTag )	
	NODEACTIVITY_ENTRY ( KNoTag, CUpnpServiceRegistrationActivity::TRegisterServiceInfoWithCPR, TAwaitingServiceRegisteredFromCpr, TErrorTagOr<TTag<KNoTag> > )
	
	NODEACTIVITY_ENTRY ( KNoTag, TFindOrCreateUdpClientFlow, CoreNetStates::TAwaitingDataClientJoin, TErrorTagOr<TTag<KNoTag> > )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, PRStates::TProcessDataClientCreation, UPnPServiceSCprStates::TStoreUdpClientAndSendRequest )
	NODEACTIVITY_ENTRY ( KSendRequest, TSendDataClientStart, CoreNetStates::TAwaitingDataClientStarted, TNoTag )	
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, CUpnpServiceRegistrationActivity::TSendPublishRequest, TNoTag )
	// sendpublish request should be written as utility
	
	NODEACTIVITY_ENTRY ( KNoTag, TFindOrCreateUdpServerFlow, CoreNetStates::TAwaitingDataClientJoin, TErrorTagOr<TTag<KNoTag> > )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, PRStates::TProcessDataClientCreation, UPnPServiceSCprStates::TStoreUdpServer )
	NODEACTIVITY_ENTRY ( KNoTag, TSendDataClientStart, CoreNetStates::TAwaitingDataClientStarted, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, CUpnpServiceRegistrationActivity::TRegisterSsdpInfoWithServer, TNoTag )
	
	LAST_NODEACTIVITY_ENTRY ( KNoTag, CUpnpServiceRegistrationActivity::TInitiateTcpFlowCreation )	
	// makes use of activity.Error ( ) && activity destruction clears error
	LAST_NODEACTIVITY_ENTRY ( KErrorTag, TSendPublishResponseToClient ) 
NODEACTIVITY_END ( )
}

namespace UPnPServiceScprDeviceRegistrationActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( TUpnpMessage::EActivityDeviceRegistration, UPnPServiceSCprDeviceRegistration, TUpnpMessage::TDeviceRegistration, CUpnpDeviceRegistrationActivity::NewL )
	FIRST_NODEACTIVITY_ENTRY ( CUpnpDeviceRegistrationActivity::TAwaitingDeviceRegistration, TNoTag )
	
	NODEACTIVITY_ENTRY ( KNoTag, CUpnpDeviceRegistrationActivity::TRequestDeviceJoin, TAwaitingServiceValidated, TErrorTagOr<TTag<KNoTag> > )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, CUpnpDeviceRegistrationActivity::TBuildPublishAndDeviceInfo, TNoTag )		
	NODEACTIVITY_ENTRY ( KNoTag, CUpnpDeviceRegistrationActivity::TRegisterDeviceInfoWithCPR, TAwaitingServiceRegisteredFromCpr, TErrorTagOr<TTag<KNoTag> > )
	
	NODEACTIVITY_ENTRY ( KNoTag, TFindOrCreateUdpClientFlow, CoreNetStates::TAwaitingDataClientJoin, TErrorTagOr<TTag<KNoTag> > )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, PRStates::TProcessDataClientCreation, UPnPServiceSCprStates::TStoreUdpClientAndSendRequest )
	NODEACTIVITY_ENTRY ( KSendRequest, TSendDataClientStart, CoreNetStates::TAwaitingDataClientStarted, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, CUpnpDeviceRegistrationActivity::TSendPublishRequest, TNoTag )
	// sendpublish request should be written as utility
	
	NODEACTIVITY_ENTRY ( KNoTag, TFindOrCreateUdpServerFlow, CoreNetStates::TAwaitingDataClientJoin, TErrorTagOr<TTag<KNoTag> > )	
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, PRStates::TProcessDataClientCreation, UPnPServiceSCprStates::TStoreUdpServer )
	NODEACTIVITY_ENTRY ( KNoTag, TSendDataClientStart, CoreNetStates::TAwaitingDataClientStarted,  TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, CUpnpDeviceRegistrationActivity::TRegisterSsdpInfoWithServer, CUpnpDeviceRegistrationActivity::TIconCreateOrNoTag )
	
	NODEACTIVITY_ENTRY ( KCreateIconTag, CUpnpDeviceRegistrationActivity::TCreateIconFlow, CoreNetStates::TAwaitingDataClientJoin, TErrorTagOr<TTag<KIconCreatedTag> > )
	NODEACTIVITY_ENTRY ( KIconCreatedTag, TProcessServerFlowCreation, CoreNetStates::TAwaitingDataClientStarted, CUpnpDeviceRegistrationActivity::TIconOrNoTag )
	
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TSendPublishResponseToClient )
	// activity destruction clears error
	LAST_NODEACTIVITY_ENTRY ( KErrorTag, TSendPublishResponseToClient )
	// no TCP flows for embedded device scpr
NODEACTIVITY_END ( )
}

namespace UPnPServiceSCprFlowCreationActivity
{
DECLARE_DEFINE_NODEACTIVITY ( TUpnpMessage::EActivityCreateServerFlow, UPnPServiceSCprFlowCreation, TUpnpMessage::TCreateServerFlow )
	FIRST_NODEACTIVITY_ENTRY ( TAwaitingServerFlowRequest, TNoTag )
	NODEACTIVITY_ENTRY ( KNoTag, TCreateServerFlow<EDescription>, CoreNetStates::TAwaitingDataClientJoin, TErrorTagOr<TTag<KNoTag> > )
	NODEACTIVITY_ENTRY ( KNoTag, TProcessServerFlowCreation, CoreNetStates::TAwaitingDataClientStarted, TNoTag )
	NODEACTIVITY_ENTRY ( KNoTag, TCreateServerFlow<ESubscription>, CoreNetStates::TAwaitingDataClientJoin, TErrorTagOr<TTag<KNoTag> > )
	NODEACTIVITY_ENTRY ( KNoTag, TProcessServerFlowCreation, CoreNetStates::TAwaitingDataClientStarted, TNoTag )
	
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TSendPublishResponseToClient )
	THROUGH_NODEACTIVITY_ENTRY ( KErrorTag, TSendPublishResponseToClient, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TClearError )
NODEACTIVITY_END ( )
}

namespace UPnPSerivceSearchResponseActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( TUpnpMessage::EActivitySearchResponse, UPnPServiceSearchResponse, TUpnpMessage::TUPnPSearchRequest, CNodeParallelActivityBase::NewL )
	FIRST_NODEACTIVITY_ENTRY ( UPnPStateMachine::TAwaitingSearchRequest, UPnPServiceSCprStates::TNoTagOrIgnoreTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, UPnPServiceSCprStates::TValidateAndSendResponse )
	LAST_NODEACTIVITY_ENTRY ( CoreStates::KIgnore, UPnPStateMachine::TDiscardSearchRequestData )
NODEACTIVITY_END ( )
}

namespace UpnpServiceRequestInfoActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( TUpnpMessage::EActivityRequestInfo, UpnpServiceRequestInfo, TUpnpMessage::TUPnPRequestInfo, CUpnpSPRequestInfoActivity::NewL )
	FIRST_NODEACTIVITY_ENTRY ( TAwaitingUpnpRequestInfo, UPnPStateMachine::TRequestOrIgnoreTag )
	
	LAST_NODEACTIVITY_ENTRY ( UPnPStateMachine::KDescribeRequest, CUpnpSPRequestInfoActivity::TSendDescribeResponse )
	LAST_NODEACTIVITY_ENTRY ( UPnPStateMachine::KSubscribeRequest, CUpnpSPRequestInfoActivity::TSendSubscribeResponse )
	LAST_NODEACTIVITY_ENTRY ( UPnPStateMachine::KUnsubscribeRequest, CUpnpSPRequestInfoActivity::TSendUnsubscribeResponse )
	
	LAST_NODEACTIVITY_ENTRY ( CoreStates::KIgnore, UPnPStateMachine::TSendIgnoreResponse )																 
NODEACTIVITY_END ( )
}


namespace UpnpServiceClientFlowCreationActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( TUpnpMessage::EActivityFindOrCreateFlow, FindOrCreateFlow, TUpnpMessage::TCreateNotifyFlow, CNotifyFlowFinder::NewL )
	FIRST_NODEACTIVITY_ENTRY ( CNotifyFlowFinder::TAwaitingFlowCreationRequest, TNoTag )
	
	NODEACTIVITY_ENTRY ( KNoTag, CNotifyFlowFinder::TFindOrCreateFlow, CoreNetStates::TAwaitingDataClientJoin,  TNoTag ) 
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, PRStates::TProcessDataClientCreation, TNoTag )
	NODEACTIVITY_ENTRY ( KNoTag, TSendDataClientStart, CoreNetStates::TAwaitingDataClientStarted, TNoTag )
	
	LAST_NODEACTIVITY_ENTRY ( KNoTag, CNotifyFlowFinder::TProcessDataClient )
NODEACTIVITY_END ( )
}

namespace UpnpEventNotificationActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( TUpnpMessage::EActivityNotifyRequest, UpnpEventNotification, TUpnpMessage::TNotifyRequest, CUpnpEventNotificationActivity::NewL )
	FIRST_NODEACTIVITY_ENTRY ( CUpnpEventNotificationActivity::TAwaitingNotifyRequest, TNoTag )
	NODEACTIVITY_ENTRY ( KNoTag, CUpnpEventNotificationActivity::TSendRequest, CUpnpEventNotificationActivity::TAwaitingErrorOrResponse, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TClearError )
NODEACTIVITY_END ( )
}


namespace UPnPServiceSCprActivities
{
DEFINE_ACTIVITY_MAP ( activityMap )
	ACTIVITY_MAP_ENTRY ( UPnPServiceSCprParamRequestActivity, UPnPServiceSCprParamRequest )
	ACTIVITY_MAP_ENTRY ( UPnPServiceSCprApplyRequestActivity, UPnPServiceSCprApplyRequest )
	ACTIVITY_MAP_ENTRY ( UPnPServiceSCprBinderRequestActivity, UPnPServiceSCprBinderRequest )
	ACTIVITY_MAP_ENTRY ( UPnPServiceSCprDataClientIdleActivity, UPnPServiceSCprDataClientIdle )	
	ACTIVITY_MAP_ENTRY ( UPnPServiceSCprClientLeaveActivity, UPnPServiceSCprClientLeave )
	ACTIVITY_MAP_ENTRY ( UPnPServiceSCprDataClientStopActivity, UPnPServiceSCprDataClientStop ) 
	ACTIVITY_MAP_ENTRY ( UPnPServiceSCprDestroyActivity, UPnPServiceSCprDestroy )
	ACTIVITY_MAP_ENTRY ( UPnPServiceScprServiceRegistrationActivity, UPnPServiceSCprServiceRegistration )
	ACTIVITY_MAP_ENTRY ( UPnPServiceScprDeviceRegistrationActivity, UPnPServiceSCprDeviceRegistration )
	ACTIVITY_MAP_ENTRY ( UPnPServiceSCprFlowCreationActivity, UPnPServiceSCprFlowCreation )
	ACTIVITY_MAP_ENTRY ( UPnPSerivceSearchResponseActivity, UPnPServiceSearchResponse )
	ACTIVITY_MAP_ENTRY ( UpnpServiceRequestInfoActivity, UpnpServiceRequestInfo )
	ACTIVITY_MAP_ENTRY ( UpnpServiceClientFlowCreationActivity, FindOrCreateFlow )
	ACTIVITY_MAP_ENTRY ( UpnpEventNotificationActivity, UpnpEventNotification )
ACTIVITY_MAP_END_BASE ( SCprActivities, coreSCprActivities )
}


//-------------------------Service Registration Activity-------------------------------------
CNodeActivityBase* CUpnpServiceRegistrationActivity::NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
	{
	return new ( ELeave ) CUpnpServiceRegistrationActivity ( aActivitySig, aNode ) ;
	}


CUpnpServiceRegistrationActivity::CUpnpServiceRegistrationActivity ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode  )
	: CNodeActivityBase ( aActivitySig, aNode )
	{	  	
	}

CUpnpServiceRegistrationActivity::~CUpnpServiceRegistrationActivity ( )
	{
	SetError ( KErrNone	);
	delete iParamSet;
	}
	
DEFINE_SMELEMENT ( CUpnpServiceRegistrationActivity::TRequestServiceJoin, NetStateMachine::MStateTransition, CUpnpServiceRegistrationActivity::TContext )
void CUpnpServiceRegistrationActivity::TRequestServiceJoin::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPSrvcRegActivity, KComponent, _L8("CUpnpServiceRegistrationActivity::TRequestServiceJoin::DoL")));
	TUpnpMessage::TServiceRegistration& msg = message_cast<TUpnpMessage::TServiceRegistration>( iContext.iMessage );
	
	CUpnpServiceRegistrationActivity& activity = static_cast<CUpnpServiceRegistrationActivity&> ( *iContext.iNodeActivity );
	activity.iParamSet = static_cast<CUPnPServiceRegisterParamSet*> ( msg.iParamSet );
	
	LOG(ESockLogExternal::Printf(KSubsysSPSrvcRegActivity, KComponent, _L8("CUpnpServiceRegistrationActivity::TRequestServiceJoin::DoL - Posting message TUpnpMessage::TServiceJoinRequest")));	
	activity.PostRequestTo ( iContext.Node ().ControlProvider ()->RecipientId (), TUpnpMessage::TServiceJoinRequest ( activity.iParamSet ).CRef () );
	}
	
DEFINE_SMELEMENT ( CUpnpServiceRegistrationActivity::TBuildPublishAndServiceInfo, NetStateMachine::MStateTransition, CUpnpServiceRegistrationActivity::TContext )
void CUpnpServiceRegistrationActivity::TBuildPublishAndServiceInfo::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPSrvcRegActivity, KComponent, _L8("CUpnpServiceRegistrationActivity::TBuildPublishAndServiceInfo::DoL")));
	CUpnpServiceRegistrationActivity& activity = static_cast<CUpnpServiceRegistrationActivity&> ( *iContext.iNodeActivity );
	
	CStringPoolManager& stringPoolMgr = iContext.Node ().ConnectionProvider ().StringPoolManager ();
	RStringPool& sp = stringPoolMgr.StringPool ( );

	CUPnPPublishInfoElement* publishInfo = CUPnPPublishInfoElement::NewL ( );
	CleanupStack::PushL ( publishInfo );
	
	publishInfo->SetUuidL( activity.iParamSet->DeviceUid( ) );
	publishInfo->SetSearchTargetL( activity.iParamSet->Uri( ) ) ;
	publishInfo->SetServiceDescriptionL( activity.iParamSet->ServiceDescription( ) );
	publishInfo->SetCacheControlL( activity.iParamSet->CacheControlData( ) ) ;
	publishInfo->SetTimeOut( activity.iParamSet->SubscriptionDuration() );
	
	const CUPnPSubscribeInfoContainer* subContainer = iContext.Node ( ).GetSubscribeInfoContainer ( );
	subContainer->SetEventDataL( activity.iParamSet->InitialMessage( ) );
	
	//Set USN
	RBuf8 resultUrl;
	TInt length = activity.iParamSet->DeviceUid( ).Length ( )
					+ activity.iParamSet->Uri( ).Length ( )
					+ KSeperator ( ).Length ( );
	resultUrl.CreateL ( length );
	CleanupClosePushL ( resultUrl );
	resultUrl.Copy ( activity.iParamSet->DeviceUid( ) );
	resultUrl.Append ( KSeperator );
	resultUrl.Append ( activity.iParamSet->Uri( ) );
	publishInfo->SetUsnL( resultUrl );
	CleanupStack::PopAndDestroy ( &resultUrl );
	
	activity.iServiceInfo = CUPnPServiceInfo::NewL ( );
	activity.iServiceInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EServiceType, stringPoolMgr.GetUPnPTable() ), activity.iParamSet->Uri( ) );
	
	// construct  serviceid
	RBuf8 serviceId;
	activity.GenerateServiceIdL ( stringPoolMgr, activity.iParamSet->Uri( ), serviceId );	
	CleanupClosePushL ( serviceId );
	activity.iServiceInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EServiceId, stringPoolMgr.GetUPnPTable() ), serviceId );
	
	
	// SCPD Url
	activity.CreateRelativeUrlL ( serviceId,
					 sp.String ( UPNPDESCRIPTIONXMLTAGS::EScpdUrl, stringPoolMgr.GetUPnPTable()).DesC ( ),
					 resultUrl );
	CleanupClosePushL ( resultUrl );
	activity.iServiceInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EScpdUrl, stringPoolMgr.GetUPnPTable ( ) ), resultUrl );	
	//Note! result url is a relative url that starts with '/', avoid this in scpr properties, bare url will take care
	TPtr8 urlPtr = resultUrl.MidTPtr ( 1 );
	publishInfo->SetKeyL( urlPtr );
	iContext.Node ( ).SetKeyL ( urlPtr );	
	
	CleanupStack::PopAndDestroy ( &resultUrl );
	
	// ControlUrl
	activity.CreateRelativeUrlL ( serviceId,
					sp.String ( UPNPDESCRIPTIONXMLTAGS::EContorlUrl, stringPoolMgr.GetUPnPTable() ).DesC ( ),
					resultUrl );
	CleanupClosePushL ( resultUrl );	
	activity.iServiceInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EContorlUrl, stringPoolMgr.GetUPnPTable() ), resultUrl );	
	urlPtr.Set ( resultUrl.MidTPtr ( 1 ) );
	iContext.Node ( ).SetControlUrlL ( urlPtr );	
	CleanupStack::PopAndDestroy ( &resultUrl );
		
	// EventSubUrl
	activity.CreateRelativeUrlL ( serviceId,
						sp.String ( UPNPDESCRIPTIONXMLTAGS::EEventSubUrl, stringPoolMgr.GetUPnPTable() ).DesC ( ),
						resultUrl );
	CleanupClosePushL ( resultUrl );	
	activity.iServiceInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EEventSubUrl, stringPoolMgr.GetUPnPTable() ), resultUrl );	
	urlPtr.Set ( resultUrl.MidTPtr ( 1 ) );
	iContext.Node ( ).SetEventUrlL ( urlPtr );
	CleanupStack::PopAndDestroy ( &resultUrl );
	
	
	CleanupStack::PopAndDestroy ( &serviceId );	
	CleanupStack::Pop ( publishInfo );
	
	iContext.Node ( ).SetPublishInfo ( publishInfo );
	}

void CUpnpServiceRegistrationActivity::GenerateServiceIdL ( CStringPoolManager& aStringPoolMgr, const TDesC8& aUri, RBuf8& aServiceId )
	{
	TLex8 parser ( aUri );
	TChar ch = ':';
	parser.Mark ( );
	while ( !parser.Eos ( ) && parser.Peek ( ) != ch )
		parser.Inc ( );
	
	parser.Inc ( );
	while ( !parser.Eos ( ) && parser.Peek ( ) != ch )
		parser.Inc ( );
	
	TPtrC8 schemaType = parser.MarkedToken ( );
	
	if ( schemaType.Compare ( KUrnSchema ( ) ) == 0 )
		{
		// Standard Upnp devices
		aServiceId.CreateL ( KUrnService );
		}
	else
		{
		RStringPool& sp = aStringPoolMgr.StringPool ( );
			
		// Vendor specific devices
		aServiceId.CreateL ( schemaType );
		aServiceId.ReAllocL ( aServiceId.Length ( )
							 + sp.String ( UPNPDESCRIPTIONXMLTAGS::EServiceId, aStringPoolMgr.GetUPnPTable() ).DesC ( ).Length ( )
							 + KColon ( ).Length ( ) * 2 );
		
		aServiceId.Append ( KColon ( ) );
		aServiceId.Append ( sp.String ( UPNPDESCRIPTIONXMLTAGS::EServiceId, aStringPoolMgr.GetUPnPTable() ).DesC ( ) );
		aServiceId.Append ( KColon ( ) );
		}
	
	parser.Inc ( );
	
	while ( !parser.Eos ( ) && parser.Peek ( ) != ch )
		parser.Inc ( );

	parser.Inc ( );
	parser.Mark ( );

	while ( !parser.Eos ( ) && parser.Peek ( ) != ch )
		parser.Inc ( );
	
	TPtrC8 serviceType = parser.MarkedToken ( );
    
    RBuf8 uniqueId;
	UpnpUuid::GenerateUuidL ( uniqueId );
	CleanupClosePushL ( uniqueId );
	
    aServiceId.ReAllocL ( aServiceId.Length ( ) + serviceType.Length ( ) + uniqueId.Length ( ) );
    aServiceId.Append ( serviceType );
    aServiceId.Append ( uniqueId );
    
    CleanupStack::PopAndDestroy ( &uniqueId );
	}
	
	

void CUpnpServiceRegistrationActivity::CreateRelativeUrlL ( const TDesC8& aServiceId, const TDesC8& aUri, RBuf8& aResultUrl )
	{	
	_LIT8 ( KForwardSlash, "/" );
	RBuf8 tempUrl;	
	TInt length = KForwardSlash ().Length () + aServiceId.Length ( )
			 + KUnderScore ( ).Length ( )
			 + aUri.Length ( );	
	
	tempUrl.CreateL ( length );
	tempUrl.Copy ( KForwardSlash );
	tempUrl.Append ( aServiceId );
	tempUrl.Append ( KUnderScore );
	tempUrl.Append ( aUri );
	aResultUrl.Assign ( tempUrl );
	}

	
DEFINE_SMELEMENT ( CUpnpServiceRegistrationActivity::TRegisterServiceInfoWithCPR, NetStateMachine::MStateTransition, CUpnpServiceRegistrationActivity::TContext )
void CUpnpServiceRegistrationActivity::TRegisterServiceInfoWithCPR::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPSrvcRegActivity, KComponent, _L8("CUpnpServiceRegistrationActivity::TRegisterServiceInfoWithCPR::DoL")));
	CUpnpServiceRegistrationActivity& activity = static_cast<CUpnpServiceRegistrationActivity&> ( *iContext.iNodeActivity );
	activity.PostRequestTo ( iContext.Node ().ControlProvider ()->RecipientId (), TUpnpMessage::TRegisterService ( activity.iParamSet, activity.iServiceInfo ).CRef () );
	}

DEFINE_SMELEMENT ( CUpnpServiceRegistrationActivity::TSendPublishRequest, NetStateMachine::MStateTransition, CUpnpServiceRegistrationActivity::TContext )
void CUpnpServiceRegistrationActivity::TSendPublishRequest::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPSrvcRegActivity, KComponent, _L8("CUpnpServiceRegistrationActivity::TSendPublishRequest::DoL")));
	CUPnPPublishInfoElement* publishInfo = iContext.Node ( ).PublishInfo ( );
	
	// Publish once for uuid:device-UUID::urn:domain-name-service:serviceType:v
	iContext.Node ( ).DoPublishL ( publishInfo->SearchTarget ( ), publishInfo->Usn ( ), publishInfo->CacheControl ( ) );
	}
	

DEFINE_SMELEMENT ( CUpnpServiceRegistrationActivity::TRegisterSsdpInfoWithServer, NetStateMachine::MStateTransition, CUpnpServiceRegistrationActivity::TContext )
void CUpnpServiceRegistrationActivity::TRegisterSsdpInfoWithServer::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPSrvcRegActivity, KComponent, _L8("CUpnpServiceRegistrationActivity::TRegisterSsdpInfoWithServer::DoL")));
	CUPnPPublishInfoElement* publishInfo = iContext.Node ( ).PublishInfo ( );
	
	RMemChunk stBuf;
	stBuf.CreateL ( publishInfo->SearchTarget ( ), iContext.Node ( ).ConnectionProvider().MemoryAllocator() );	
	
	LOG(ESockLogExternal::Printf(KSubsysSPSrvcRegActivity, KComponent, _L8("CUpnpServiceRegistrationActivity::TInitiateTcpFlowCreation::DoL - Posting message TUpnpMessage::TUPnPPublishRegistration")));	
	TSsdpInfo ssdpInfo ( stBuf );
	RClientInterface::OpenPostMessageClose ( TNodeCtxId ( MeshMachine::KActivityNull, iContext.NodeId () ), iContext.Node ().UdpServerFlow (), TUpnpMessage::TUPnPPublishRegistration ( ssdpInfo ).CRef () );
	}
	
	
DEFINE_SMELEMENT ( CUpnpServiceRegistrationActivity::TInitiateTcpFlowCreation, NetStateMachine::MStateTransition, CUpnpServiceRegistrationActivity::TContext )
void CUpnpServiceRegistrationActivity::TInitiateTcpFlowCreation::DoL ( )
	{
	// kickoff self activity to create tcp server flows
	LOG(ESockLogExternal::Printf(KSubsysSPSrvcRegActivity, KComponent, _L8("CUpnpServiceRegistrationActivity::TInitiateTcpFlowCreation::DoL - Posting message TUpnpMessage::TCreateServerFlow")));		
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.NodeId (), TUpnpMessage::TCreateServerFlow ().CRef () );
	}


//-------------------------Embedded Device Session Registration Activity-------------------------------------
CNodeActivityBase* CUpnpDeviceRegistrationActivity::NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
	{
	return new ( ELeave ) CUpnpDeviceRegistrationActivity ( aActivitySig, aNode ) ;
	}


CUpnpDeviceRegistrationActivity::CUpnpDeviceRegistrationActivity ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode  )
	: CNodeActivityBase ( aActivitySig, aNode ), iIconUrlIdx ( 0 )
	{	  	
	}

CUpnpDeviceRegistrationActivity::~CUpnpDeviceRegistrationActivity ( )
	{
	SetError ( KErrNone	);
	delete iParamSet;
	iIconUrl.Close ();
	}
	
DEFINE_SMELEMENT ( CUpnpDeviceRegistrationActivity::TRequestDeviceJoin, NetStateMachine::MStateTransition, CUpnpDeviceRegistrationActivity::TContext )
void CUpnpDeviceRegistrationActivity::TRequestDeviceJoin::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPDeviceRegActivity, KComponent, _L8("CUpnpDeviceRegistrationActivity::TRequestDeviceJoin::DoL")));
	TUpnpMessage::TDeviceRegistration& msg = message_cast<TUpnpMessage::TDeviceRegistration>( iContext.iMessage );
	
	CUpnpDeviceRegistrationActivity& activity = static_cast<CUpnpDeviceRegistrationActivity&> ( *iContext.iNodeActivity );
	activity.iParamSet = static_cast<CUPnPDeviceRegisterParamSet*> ( msg.iParamSet );
	
	LOG(ESockLogExternal::Printf(KSubsysSPDeviceRegActivity, KComponent, _L8("CUpnpDeviceRegistrationActivity::TRequestDeviceJoin::DoL - Posting message TUpnpMessage::TDeviceJoinRequest")));	
	activity.PostRequestTo ( iContext.Node ().ControlProvider ()->RecipientId (), TUpnpMessage::TDeviceJoinRequest ( activity.iParamSet ).CRef () );
	}
	
DEFINE_SMELEMENT ( CUpnpDeviceRegistrationActivity::TBuildPublishAndDeviceInfo, NetStateMachine::MStateTransition, CUpnpDeviceRegistrationActivity::TContext )
void CUpnpDeviceRegistrationActivity::TBuildPublishAndDeviceInfo::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPDeviceRegActivity, KComponent, _L8("CUpnpDeviceRegistrationActivity::TBuildPublishAndDeviceInfo::DoL")));
	CUpnpDeviceRegistrationActivity& activity = static_cast<CUpnpDeviceRegistrationActivity&> ( *iContext.iNodeActivity );
	
	CUPnPPublishInfoElement* publishInfo = CUPnPPublishInfoElement::NewL ( );
	CleanupStack::PushL ( publishInfo );
	
	publishInfo->SetUuidL( activity.iParamSet->UDN( ) );
	publishInfo->SetSearchTargetL( activity.iParamSet->Uri( ) ) ;
	publishInfo->SetCacheControlL( activity.iParamSet->CacheControlData( ) ) ;	
	// for an embedded device both key and UDN are same, so...it can key can be used
	// for deregistering embedded device.
	publishInfo->SetKeyL( activity.iParamSet->UDN( ) );
	
	//Set USN
	RBuf8 tempUrl;
	TInt length = activity.iParamSet->UDN( ).Length ( ) 
				+ activity.iParamSet->Uri( ).Length ( )
				+ KSeperator ( ).Length ( );
				
	tempUrl.CreateMaxL ( length );
	CleanupClosePushL ( tempUrl );
	
	tempUrl.Copy ( activity.iParamSet->UDN( ) );		
	tempUrl.Append ( KSeperator );
	tempUrl.Append ( activity.iParamSet->Uri( ) );
	publishInfo->SetUsnL( tempUrl );
	
	activity.iDeviceInfo = CUPnPDevice::NewL ( );
	
	CStringPoolManager& stringPoolMgr = iContext.Node ().ConnectionProvider ().StringPoolManager ();
	RStringPool& sp = stringPoolMgr.StringPool ( );

	// set device info		
	activity.iDeviceInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EPresentationUrl, stringPoolMgr.GetUPnPTable() ), activity.iParamSet->PresentationUri( ) );
	// set the device attributes
	activity.iDeviceInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EDeviceType, stringPoolMgr.GetUPnPTable() ), activity.iParamSet->Uri( ) );
	activity.iDeviceInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EFriendlyName, stringPoolMgr.GetUPnPTable() ),activity.iParamSet->FriendlyName( ) );
	activity.iDeviceInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EUdn,stringPoolMgr.GetUPnPTable()), activity.iParamSet->UDN( ) );
	activity.iDeviceInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EUpc, stringPoolMgr.GetUPnPTable()), activity.iParamSet->UPC( ) );
	
	// set manufacturer attributes
	activity.iDeviceInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EManufacturer, stringPoolMgr.GetUPnPTable() ), activity.iParamSet->Manufacturer( ) );
	activity.iDeviceInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EManufacturerUrl, stringPoolMgr.GetUPnPTable() ), activity.iParamSet->ManufacturerUrl( ) );
	activity.iDeviceInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::ESerialNumber, stringPoolMgr.GetUPnPTable() ), activity.iParamSet->SerialNumber( ) );

	// set device model attributes
	activity.iDeviceInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EModelName, stringPoolMgr.GetUPnPTable() ), activity.iParamSet->ModelName( ) );
	activity.iDeviceInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EModelDescription,stringPoolMgr.GetUPnPTable() ), activity.iParamSet->ModelDesc( ) );
	activity.iDeviceInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EModelNumber, stringPoolMgr.GetUPnPTable()), activity.iParamSet->ModelNumber( ) );
	activity.iDeviceInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EModel,stringPoolMgr.GetUPnPTable() ), activity.iParamSet->ModelUri( ) );

	for (TInt i = 0; i <  activity.iParamSet->IconList ().Count (); i++ )
		{
		CUPnPIconParams* iconParams = activity.iParamSet->IconList ()[i];
		CUPnPIconInfo* iconInfo = CUPnPIconInfo::NewL ();
		CleanupStack::PushL ( iconInfo );
		iconInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EMimeType, stringPoolMgr.GetUPnPTable() ), iconParams->MimeType () );
		iconInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EWidth,stringPoolMgr.GetUPnPTable() ), iconParams->Width () );
		iconInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EHeight, stringPoolMgr.GetUPnPTable()), iconParams->Height () );
		iconInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EDepth,stringPoolMgr.GetUPnPTable() ), iconParams->Depth () );
		iconInfo->SetPropertyL( sp.String ( UPNPDESCRIPTIONXMLTAGS::EUrl,stringPoolMgr.GetUPnPTable() ), iconParams->Url () );
		
		activity.iDeviceInfo->AppendToIconInfoTableL ( iconInfo );
		CleanupStack::Pop ( iconInfo );
		}
		
	CleanupStack::PopAndDestroy ( &tempUrl );
	CleanupStack::Pop ( publishInfo );
	
	iContext.Node ( ).SetPublishInfo ( publishInfo );
	iContext.Node ( ). SetDevice ( ETrue );
	}
	
DEFINE_SMELEMENT ( CUpnpDeviceRegistrationActivity::TRegisterDeviceInfoWithCPR, NetStateMachine::MStateTransition, CUpnpDeviceRegistrationActivity::TContext )
void CUpnpDeviceRegistrationActivity::TRegisterDeviceInfoWithCPR::DoL ( )
	{
	CUpnpDeviceRegistrationActivity& activity = static_cast<CUpnpDeviceRegistrationActivity&> ( *iContext.iNodeActivity );		
	LOG(ESockLogExternal::Printf(KSubsysSPDeviceRegActivity, KComponent, _L8("CUpnpDeviceRegistrationActivity::TRegisterDeviceInfoWithCPR::DoL - Posting message TUpnpMessage::TRegisterDevice")));	
	activity.PostRequestTo ( iContext.Node ().ControlProvider ()->RecipientId (), TUpnpMessage::TRegisterDevice ( activity.iParamSet, activity.iDeviceInfo ).CRef () );	
	}

DEFINE_SMELEMENT ( CUpnpDeviceRegistrationActivity::TSendPublishRequest, NetStateMachine::MStateTransition, CUpnpDeviceRegistrationActivity::TContext )
void CUpnpDeviceRegistrationActivity::TSendPublishRequest::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPDeviceRegActivity, KComponent, _L8("CUpnpDeviceRegistrationActivity::TSendPublishRequest::DoL")));
	CUPnPPublishInfoElement* publishInfo = iContext.Node ( ).PublishInfo ( );		
		
	// Publish once for uuid:device-UUID
	iContext.Node ( ).DoPublishL ( publishInfo->Uuid ( ), publishInfo->Uuid ( ), publishInfo->CacheControl ( ) );
	
	// Publish once for uuid:device-UUID::urn:domain-name-device:deviceType:v
	iContext.Node ( ).DoPublishL ( publishInfo->SearchTarget ( ), publishInfo->Usn ( ), publishInfo->CacheControl ( ) );
	}

DEFINE_SMELEMENT ( CUpnpDeviceRegistrationActivity::TRegisterSsdpInfoWithServer, NetStateMachine::MStateTransition, CUpnpDeviceRegistrationActivity::TContext )
void CUpnpDeviceRegistrationActivity::TRegisterSsdpInfoWithServer::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPDeviceRegActivity, KComponent, _L8("CUpnpDeviceRegistrationActivity::TRegisterSsdpInfoWithServer::DoL")));
	CUPnPPublishInfoElement* publishInfo = iContext.Node ( ).PublishInfo ( );
	
	// Register once for uuid:device-UUID
	RegisterForMSearchL ( publishInfo->Uuid ( ) );
	
	// Register once for uuid:device-UUID::urn:domain-name-device:deviceType:v
	RegisterForMSearchL ( publishInfo->SearchTarget ( ) );
	}

void CUpnpDeviceRegistrationActivity::TRegisterSsdpInfoWithServer::RegisterForMSearchL ( const TDesC8& aSearchTarget )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPDeviceRegActivity, KComponent, _L8("CUpnpDeviceRegistrationActivity::TRegisterSsdpInfoWithServer::DoL")));
	RMemChunk stBuf;
	stBuf.CreateL ( aSearchTarget, iContext.Node ( ).ConnectionProvider().MemoryAllocator() );
	
	LOG(ESockLogExternal::Printf(KSubsysSPDeviceRegActivity, KComponent, _L8("CUpnpDeviceRegistrationActivity::TRegisterSsdpInfoWithServer::DoL - Posting message TUpnpMessage::TUPnPPublishRegistration")));		
	TSsdpInfo ssdpInfo ( stBuf );
	RClientInterface::OpenPostMessageClose ( TNodeCtxId ( MeshMachine::KActivityNull, iContext.NodeId () ), iContext.Node ().UdpServerFlow () , TUpnpMessage::TUPnPPublishRegistration ( ssdpInfo ).CRef () );
	}	

DEFINE_SMELEMENT ( CUpnpDeviceRegistrationActivity::TCreateIconFlow, NetStateMachine::MStateTransition, CUpnpDeviceRegistrationActivity::TContext )
void CUpnpDeviceRegistrationActivity::TCreateIconFlow::DoL ()
	{
	CUpnpDeviceRegistrationActivity& activity = static_cast<CUpnpDeviceRegistrationActivity&> ( *iContext.iNodeActivity );		
	activity.iIconUrl.Close ();
	
	const TDesC8& rootLoc = iContext.Node ().ConnectionProvider ().RootDeviceLocation ();
	const TDesC8& url = activity.iParamSet->IconList()[activity.iIconUrlIdx]->Url ();
	if ( url[0] == TChar ('/') )
		{
		activity.iIconUrl.CreateL ( rootLoc.Length () + ( url.Length () - 1 ) );
		activity.iIconUrl.Copy ( rootLoc );
		activity.iIconUrl.Append ( url.Mid (1) );
		}
	else
		{
		activity.iIconUrl.CreateL ( rootLoc.Length () + url.Length () );
		activity.iIconUrl.Copy ( rootLoc );
		activity.iIconUrl.Append ( url );
		}
	
	const TUint8* uriPtr = activity.iIconUrl.Ptr ();
	TUint size = activity.iIconUrl.Size ();
	const TUid requestedUid = { CUPnPFlowFactory::iUid };
	THttpServerFlowQuery flowQuery ( uriPtr,
									 size,
									 iContext.NodeId (),
									 EHttpServerFlow );
	
	ESock::TCFFactory::TFindOrCreatePeer msg ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery );
	
	TCFPlayerRole playerRole ( TCFPlayerRole::EDataPlane );
	Messages::TNodeId factoryContainer = SockManGlobals::Get()->GetPlaneFC( playerRole );
	activity.PostRequestTo( factoryContainer, msg );
	}

DEFINE_SMELEMENT ( CUpnpDeviceRegistrationActivity::TIconCreateOrNoTag, NetStateMachine::MStateFork, CUpnpDeviceRegistrationActivity::TContext )
TInt CUpnpDeviceRegistrationActivity::TIconCreateOrNoTag::TransitionTag ()
	{
	CUpnpDeviceRegistrationActivity& activity = static_cast<CUpnpDeviceRegistrationActivity&> ( *iContext.iNodeActivity );	
	return activity.iParamSet->IconList ().Count () != 0 ? KCreateIconTag : KNoTag;
	}

DEFINE_SMELEMENT ( CUpnpDeviceRegistrationActivity::TIconOrNoTag, NetStateMachine::MStateFork, CUpnpDeviceRegistrationActivity::TContext )
TInt CUpnpDeviceRegistrationActivity::TIconOrNoTag::TransitionTag ()
	{
	CUpnpDeviceRegistrationActivity& activity = static_cast<CUpnpDeviceRegistrationActivity&> ( *iContext.iNodeActivity );
	
	activity.iIconUrlIdx++;
	if ( activity.iParamSet->IconList ().Count () != activity.iIconUrlIdx )
		{
		return NetStateMachine::EBackward | KCreateIconTag;
		}
	
	return NetStateMachine::EForward | KNoTag;
	}	

// -------------- Request Info Activity--------------------------------------------------

CNodeActivityBase* CUpnpSPRequestInfoActivity::NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
	{
	return ( new ( ELeave ) CUpnpSPRequestInfoActivity ( aActivitySig, aNode ) );
	}

CUpnpSPRequestInfoActivity::~CUpnpSPRequestInfoActivity ( )
	{
	SetError ( KErrNone	);
	}	

// Transitions
DEFINE_SMELEMENT ( CUpnpSPRequestInfoActivity::TSendDescribeResponse, NetStateMachine::MStateTransition, CUpnpSPRequestInfoActivity::TContext )
void CUpnpSPRequestInfoActivity::TSendDescribeResponse::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPServiceReqInfoActivity, KComponent, _L8("CUpnpSPRequestInfoActivity::TSendDescribeResponse::DoL")));
	
	TUpnpMessage::TUPnPRequestInfo& requestMsg = message_cast< TUpnpMessage::TUPnPRequestInfo > ( iContext.iMessage );
	CTransaction* transaction = static_cast<CTransaction*> ( requestMsg.iPtr );
	TInt statusCode;
	RMemoryAllocator allocator = iContext.Node ( ).ConnectionProvider().MemoryAllocator();
	
	if ( transaction->Request()->URI()->Uri().UriDes().Compare ( iContext.Node ().Key () ) == 0 )
		{
		RMemChunk dataChian;
		dataChian.CreateL( iContext.Node ( ).SCPDData ( ), allocator );
		transaction->AddBodyPart( dataChian );
		statusCode = HTTPStatus::EOk;
		}
	else
		{
		CUPnPUtils::FindAndSendIconDataL ( transaction, statusCode, allocator );
		}
	
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.iSender,
									TUpnpMessage::TUPnPResponseInfo ( transaction, statusCode, NULL ).CRef () );
	}


DEFINE_SMELEMENT ( CUpnpSPRequestInfoActivity::TSendSubscribeResponse, NetStateMachine::MStateTransition, CUpnpSPRequestInfoActivity::TContext )
void CUpnpSPRequestInfoActivity::TSendSubscribeResponse::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPServiceReqInfoActivity, KComponent, _L8("CUpnpSPRequestInfoActivity::TSendSubscribeResponse::DoL")));
	CUpnpSPRequestInfoActivity& activity = static_cast< CUpnpSPRequestInfoActivity& > ( *iContext.iNodeActivity );
	
	TUpnpMessage::TUPnPRequestInfo& requestMsg = message_cast< TUpnpMessage::TUPnPRequestInfo > ( iContext.iMessage );
	CTransaction* transaction = static_cast<CTransaction*> ( requestMsg.iPtr );
		
	TInt statusCode;
	TSubsribeResponseInfo subscribeResponse;
	
	THTTPHdrVal fieldVal;
	CUPnPPublishInfoElement* publishInfo = iContext.Node().PublishInfo();
	if ( KErrNotFound == activity.GetHeaderValue ( *transaction->Request( ), UPnP::ESID, fieldVal, TUPnPTable::Table() ) )
		{
		statusCode = activity.HandleSubscribeRequestL ( *transaction->Request( ), subscribeResponse, *publishInfo );
		
		if ( statusCode == HTTPStatus::EOk && IsFirstSubscription ( ) )
			{
			// trigger CPR to start subscription timer			
			RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.Node ().ControlProvider ()->RecipientId (),
													TUpnpMessage::TStartTimer ().CRef () );
			}
		}
	else
		{
		statusCode = activity.HandleResubscribeRequestL ( *transaction->Request( ), subscribeResponse, *publishInfo );
		}
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.iSender,
											TUpnpMessage::TUPnPResponseInfo ( transaction, statusCode, &subscribeResponse ).CRef () );
	}

TInt CUpnpSPRequestInfoActivity::HandleSubscribeRequestL (  CRequest& aRequest, TSubsribeResponseInfo& aSubscribeResponse, CUPnPPublishInfoElement& aPublishInfo )
	{
	TInt statusCode = HTTPStatus::EOk;
	// Note! as of now there is no limitation on subscribers or else send 5xx status code
	// Note! duration is accepted what ever the client is requesting
	
	THTTPHdrVal timeOutField;
	TInt err = GetHeaderValue ( aRequest, UPnP::ETimeout, timeOutField, TUPnPTable::Table() );  	
	TInt timeout = aPublishInfo.TimeOut();
	
	// If the SUBSCRIBE request doesn't have the Timeout header,
	// assume the standard DLNA specified value of 300 seconds.
	if(err != KErrNotFound)
		{
		if(timeOutField.Int() < aPublishInfo.TimeOut())
			timeout = timeOutField.Int ( );			
		}
	
	THTTPHdrVal callbackField;
	err = GetHeaderValue ( aRequest, UPnP::ECallback, callbackField, TUPnPTable::Table() );
	__ASSERT_DEBUG ( err != KErrNotFound, User::Invariant ( ) );
	const TDesC8& callBackUrl = callbackField.StrF ( ).DesC ( );
	
	// construct the subscribeinfoelement	
	CUPnPSubscribeInfoElement* newSubscription = CUPnPSubscribeInfoElement::NewL ( );
	newSubscription->SetEventKey ( 0 ); // initial value
		
	newSubscription->SetSubscriptionDuration ( timeout );
	
	RBuf8 uuid;
	CleanupClosePushL ( uuid );
	UpnpUuid::GenerateUuidL ( uuid );
	newSubscription->SetSubscriptionIdentifierL ( uuid );
	
	// extract and cache callback urls
	ProcessCallBackUrlL ( callBackUrl, newSubscription );
	
	// append element in subscription container list	
	CUPnPServiceSubConnectionProvider& subConn = static_cast<CUPnPServiceSubConnectionProvider&> ( iNode );
	const CUPnPSubscribeInfoContainer* container = subConn.GetSubscribeInfoContainer ( );
	container->AppendSubscribeInfoElementL( newSubscription );
	
	// construct the response;
	aSubscribeResponse.iTimeout = timeout;
	aSubscribeResponse.iSid.CreateL ( uuid, subConn.ConnectionProvider().MemoryAllocator() );
	CleanupStack::PopAndDestroy ( &uuid );
	
	// send a message find or create a flow.
	// NOTE: This will also send the intial notify message and add it into the container.	
	RClientInterface::OpenPostMessageClose ( iNode.Id(), iNode.Id (),
											TUpnpMessage::TCreateNotifyFlow ( newSubscription ).CRef () );			
	return statusCode;
	}
	
	
void CUpnpSPRequestInfoActivity::ProcessCallBackUrlL ( const TDesC8& aUrl, CUPnPSubscribeInfoElement* aSubscribeElement )
	{
	// extract the ip address and flow and add it to the info element
	aSubscribeElement->SetCallBackUrlL( aUrl );
	
	TUriParser8 parsedUri;
	parsedUri.Parse ( aUrl );
	
	TInt ip = 0;
	InetProtTextUtils::ConvertDescriptorToInt ( parsedUri.Extract ( EUriHost ), ip );
	
	TInt port = KHttpDefaultPort;
	if ( parsedUri.IsPresent ( EUriPort ) )
		{
		InetProtTextUtils::ConvertDescriptorToInt ( parsedUri.Extract ( EUriPort ), port );
		}
	
	TAppProtAddr addr ( ip, port );
	aSubscribeElement->SetRemoteAddr ( addr );
	}
	

TBool CUpnpSPRequestInfoActivity::TSendSubscribeResponse::IsFirstSubscription ( )
	{
	const CUPnPSubscribeInfoContainer* container = iContext.Node ( ).GetSubscribeInfoContainer ( );
	return container->CountOfSubscribeInfoElementArray( ) == 1 ? ETrue : EFalse;
	}
	
	
TInt CUpnpSPRequestInfoActivity::HandleResubscribeRequestL (  CRequest& aRequest, TSubsribeResponseInfo& aSubscribeResponse, CUPnPPublishInfoElement& aPublishInfo )
	{
	CUPnPServiceSubConnectionProvider& provider = static_cast<CUPnPServiceSubConnectionProvider&> ( iNode );
		
	// Note! as of now there is no limitation on subscribers or else send 5xx status code
	// Note! duration is accepted what ever the client is requesting		
	TInt statusCode = HTTPStatus::EOk;
	THTTPHdrVal timeOutField;
	TInt err = GetHeaderValue ( aRequest, UPnP::ETimeout, timeOutField, TUPnPTable::Table() );  	
	TInt timeout = aPublishInfo.TimeOut();
	
	// If the Re-SUBSCRIBE request doesn't have the Timeout header,
	// assume the standard DLNA specified value of 300 seconds.
	if(err != KErrNotFound)
		{
		if(timeOutField.Int() < aPublishInfo.TimeOut())
			timeout = timeOutField.Int ( );			
		}
	
	THTTPHdrVal sidField;
	err = GetHeaderValue ( aRequest, UPnP::ESID, sidField, TUPnPTable::Table() );
	__ASSERT_DEBUG ( err != KErrNotFound, User::Invariant ( ) );
		
	const CUPnPSubscribeInfoContainer* container = provider.GetSubscribeInfoContainer ( );
	CUPnPSubscribeInfoElement* element = container->GetSubscribeInfoBySid ( sidField.StrF ( ).DesC ( ) );
		
	if ( element == NULL )
		{
		// susbscription is expired send 412 error condition
		statusCode = HTTPStatus::EPreconditionFailed;
		}
	else
		{
		element->SetSubscriptionDuration ( timeout );
		
		// construct the response;
		aSubscribeResponse.iTimeout = timeout;
		aSubscribeResponse.iSid.CreateL ( element->SubscriptionIdentifier( ), provider.ConnectionProvider().MemoryAllocator() );
		}
		
	return statusCode;	
	}
	
	
DEFINE_SMELEMENT ( CUpnpSPRequestInfoActivity::TSendUnsubscribeResponse, NetStateMachine::MStateTransition, CUpnpSPRequestInfoActivity::TContext )
void CUpnpSPRequestInfoActivity::TSendUnsubscribeResponse::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPServiceReqInfoActivity, KComponent, _L8("CUpnpSPRequestInfoActivity::TSendUnsubscribeResponse::DoL")));
	
	TUpnpMessage::TUPnPRequestInfo& requestMsg = message_cast< TUpnpMessage::TUPnPRequestInfo > ( iContext.iMessage );
	CTransaction* transaction = static_cast<CTransaction*> ( requestMsg.iPtr );	
	
	
	CUpnpSPRequestInfoActivity& activity = static_cast< CUpnpSPRequestInfoActivity& > ( *iContext.iNodeActivity );
	THTTPHdrVal sidField;
	TInt err = activity.GetHeaderValue ( *transaction->Request( ), UPnP::ESID, sidField, TUPnPTable::Table() );
	__ASSERT_DEBUG ( err != KErrNotFound, User::Invariant ( ) );
	
	
	CUPnPSubscribeInfoContainer* container = iContext.Node ( ).GetSubscribeInfoContainer ( );
	CUPnPSubscribeInfoElement* element = container->GetSubscribeInfoBySid( sidField.StrF ( ).DesC ( ) );
	
	TInt statusCode = HTTPStatus::EOk;
	if ( element == NULL )
		{
		// susbscription is expired send 412 error condition
		statusCode = HTTPStatus::EPreconditionFailed;
		}
	else
		{
		container->DeleteSubscribeInfoElement(element);
		}
	if ( IsLastSubscription ( ) )
		{
		// trigger CPR to stop subscription timer				
		RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.Node ().ControlProvider ()->RecipientId (),
												TUpnpMessage::TStopTimer ().CRef () );	
		}			
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.iSender,
											TUpnpMessage::TUPnPResponseInfo ( transaction, statusCode, NULL ).CRef () );
	}


TBool CUpnpSPRequestInfoActivity::TSendUnsubscribeResponse::IsLastSubscription ( )
	{
	const CUPnPSubscribeInfoContainer* container = iContext.Node ( ).GetSubscribeInfoContainer ( );
	return container->CountOfSubscribeInfoElementArray( ) == 0 ? ETrue : EFalse;
	}


TInt CUpnpSPRequestInfoActivity::GetHeaderValue ( CRequest& aRequest, TInt aFieldIndex, THTTPHdrVal& aFieldVal, const TStringTable& aTable )
	{	
	RRequest request = aRequest.Handle ( );
	RHTTPHeaders headers = request.GetHeaderCollection ( );
	RStringF fieldStr = aRequest.StringPool ( ).StringF ( aFieldIndex , aTable );
	return headers.GetField ( fieldStr, 0, aFieldVal );
	}
	
// -----------------------Client Flow Creation---------------------------------------------	
CNodeActivityBase* CNotifyFlowFinder::NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
	{
	TUint activityId = GetNextActivityCountL ( aActivitySig, aNode );
	return ( new ( ELeave ) CNotifyFlowFinder ( aActivitySig, aNode, activityId ) );
	}


CNotifyFlowFinder::~CNotifyFlowFinder ( )
	{
	SetError( KErrNone );
	}


CNotifyFlowFinder::CNotifyFlowFinder ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aActivityCount )
:CNodeParallelActivityBase ( aActivitySig, aNode, aActivityCount )
	{	
	}

DEFINE_SMELEMENT ( CNotifyFlowFinder::TFindOrCreateFlow, NetStateMachine::MStateTransition, CNotifyFlowFinder::TContext )
void CNotifyFlowFinder::TFindOrCreateFlow::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPClntFlowCreationActivity, KComponent, _L8("CNotifyFlowFinder::TFindOrCreateFlow::DoL")));
	CNotifyFlowFinder& activity = static_cast< CNotifyFlowFinder& > ( *iContext.iNodeActivity );
	
	TUpnpMessage::TCreateNotifyFlow& createFlowMsg = message_cast< TUpnpMessage::TCreateNotifyFlow > ( iContext.iMessage );
	activity.iSubscribeInfo = static_cast< CUPnPSubscribeInfoElement* > ( createFlowMsg.iPtr );
	
	TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
	
	const TUid requestedUid = { CUPnPFlowFactory::iUid };
	THttpClientFlowQuery flowQuery ( activity.iSubscribeInfo->RemoteAddr( ), iContext.NodeId (), EHttpClientFlow, THttpClientFlowQuery::EUseExisting );
		
	activity.PostRequestTo ( factoryContainer, TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
	}


DEFINE_SMELEMENT ( CNotifyFlowFinder::TProcessDataClient, NetStateMachine::MStateTransition, CNotifyFlowFinder::TContext )
void CNotifyFlowFinder::TProcessDataClient::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPClntFlowCreationActivity, KComponent, _L8("CNotifyFlowFinder::TProcessDataClient::DoL")));
	CNotifyFlowFinder& activity = static_cast< CNotifyFlowFinder& > ( *iContext.iNodeActivity );
	
	//TNodeId dataClient = ( message_cast<TCFDataClient::TDataClientStarted> ( iContext.iMessage ) ).iSender;
	activity.iSubscribeInfo->SetFlowId ( address_cast<TNodeId> ( iContext.iSender ) );
	
	if ( !iContext.Node ( ).IsClosing ( ) )
		{		
		// send the intial notify message		
		RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.NodeId (), TUpnpMessage::TNotifyRequest ( activity.iSubscribeInfo ).CRef () );
		}
	}
	

//---------------------Event Notification Activity-------------------------------------
CNodeActivityBase* CUpnpEventNotificationActivity::NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
	{
	TUint activityId = GetNextActivityCountL ( aActivitySig, aNode );
	return ( new ( ELeave ) CUpnpEventNotificationActivity ( aActivitySig, aNode, activityId ) );
	}


CUpnpEventNotificationActivity::CUpnpEventNotificationActivity ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aActivitiesCount )
:CNodeParallelActivityBase ( aActivitySig, aNode, aActivitiesCount )
	{	  	
	}
	
// Transitions
DEFINE_SMELEMENT ( CUpnpEventNotificationActivity::TSendRequest, NetStateMachine::MStateTransition, CUpnpEventNotificationActivity::TContext )
void CUpnpEventNotificationActivity::TSendRequest::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPEventNotifyActivity, KComponent, _L8("CUpnpEventNotificationActivity::TSendRequest::DoL")));
	CUpnpEventNotificationActivity& activity = static_cast< CUpnpEventNotificationActivity& > ( *iContext.iNodeActivity );

	RMemoryAllocator allocator = iContext.Node ( ).ConnectionProvider().MemoryAllocator();
	
	const CUPnPSubscribeInfoContainer*  cont = iContext.Node ( ).GetSubscribeInfoContainer ( );

	TUpnpMessage::TNotifyRequest& msg = message_cast< TUpnpMessage::TNotifyRequest > ( iContext.iMessage );
	CUPnPSubscribeInfoElement* subscribeInfo = static_cast< CUPnPSubscribeInfoElement* > ( msg.iPtr );;

	RMemChunk uriBuf;
	uriBuf.CreateL( subscribeInfo->CallBackUrl( ), allocator );
	TCleanupItem item ( &UPnPStateMachine::CUPnPUtils::CleanupMBufChain, &uriBuf );
	CleanupStack::PushL ( item );

	RMemChunk bodyBuf;
	bodyBuf.CreateL( cont->EventData(), allocator );
	TCleanupItem item1 ( &UPnPStateMachine::CUPnPUtils::CleanupMBufChain, &bodyBuf );
	CleanupStack::PushL ( item1 );
	
	TEventNotifyRequestInfo notifyRequest;
	RMemChunk sid;
	notifyRequest.iSid.CreateL( subscribeInfo->SubscriptionIdentifier(), allocator );
	notifyRequest.iSeqKey = subscribeInfo->EventKey();
		
	activity.PostRequestTo( subscribeInfo->FlowId(), 
							TUpnpMessage::TUPnPClientRequestInfo ( UPnP::ENotify, uriBuf, bodyBuf, &notifyRequest ).CRef ()	);
	
	CleanupStack::Pop ( ); // item
	CleanupStack::Pop ( ); // item1
	}
