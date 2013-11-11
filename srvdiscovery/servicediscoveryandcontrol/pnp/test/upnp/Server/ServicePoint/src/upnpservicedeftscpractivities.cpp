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


#include <httperr.h>

//Local Includes
#include "upnpservicedeftscpractivities.h"
#include "upnpservicedeftscprstates.h"
#include "upnpctrlscprstates.h"
#include "upnpstatemachine.h"
#include "upnplog.h"
#include "upnpdescriptionschema.h"

using namespace ESock;
using namespace MeshMachine;
using namespace CoreActivities;
using namespace CoreNetStates;
using namespace CoreStates;
using namespace UPnPServiceDefaultStates;
using namespace UPnPServiceDefaultActivities;
using namespace UPnPStateMachine;


__FLOG_STMT(_LIT8(KComponent,"UPnPSPDefltScpr");)

namespace UPnPServiceDefaultBindToActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( ECFActivityBindTo, UPnPServiceDefaultBindToRequest, TCFDataClient::TBindTo, PRActivities::CBindToActivity::NewL )
	FIRST_NODEACTIVITY_ENTRY ( CoreNetStates::TAwaitingBindTo, PRActivities::CBindToActivity::TNoTagOrBearerReady )
    
    THROUGH_NODEACTIVITY_ENTRY ( KNoTag, TStoreBindToOriginator, TNoTag )    
    NODEACTIVITY_ENTRY ( KNoTag, CoreNetStates::TSendControlClientJoinRequest, CoreStates::TAwaitingJoinComplete, TTag<KNoTag> )
	
	NODEACTIVITY_ENTRY ( KNoTag, TFindOrCreateUdpClientFlow, CoreNetStates::TAwaitingDataClientJoin, TErrorTagOr<TTag<KNoTag> > )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, TProcessUdpClientFlowCreation, TTag<KDataClientReady> )
	
	NODEACTIVITY_ENTRY ( KDataClientReady, TFindOrCreateUdpServerFlow, CoreNetStates::TAwaitingDataClientJoin, TErrorTagOr<TTag<KNoTag> > )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, TProcessUdpServerFlowCreation, TTag<KBindToComplete> )
	
	LAST_NODEACTIVITY_ENTRY ( KBindToComplete, ABindingActivity::TSendBindToComplete )
	
	NODEACTIVITY_ENTRY ( KErrorTag, CoreNetStates::TSendClientLeavingRequestToServiceProvider, CoreStates::TAwaitingLeaveComplete, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TRaiseAndClearActivityError )
NODEACTIVITY_END ( )
}


// UPnP Default SCRP will only udp data client
// unless an actual service is registered with CPR i.e UPnP root device
namespace UPnPServiceDefaultDataClientStartActivity
{
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityStartDataClient, UPnPServiceDefaultDataClientStart, TCFDataClient::TStart )
    FIRST_NODEACTIVITY_ENTRY ( CoreNetStates::TAwaitingDataClientStart, TNoTag )
    NODEACTIVITY_ENTRY ( KNoTag, UPnPServiceDefaultStates::TStartMuClient, CoreNetStates::TAwaitingDataClientStarted, TNoTag )    
	LAST_NODEACTIVITY_ENTRY ( KNoTag, PRStates::TSendDataClientStarted )
NODEACTIVITY_END ( )
}

namespace UPnPServiceDefaultClientLeaveActivity
{
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityClientLeave, UPnPServiceDefaultClientLeave, TNodeSignal::TNullMessageId ) //May be waiting for both messages
	FIRST_NODEACTIVITY_ENTRY ( CoreStates::TAwaitingClientLeave, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, UPnPServiceDefaultStates::TProcessClientLeave, UPnPServiceDefaultStates::TNoTagOrDataClientIdle )
	THROUGH_NODEACTIVITY_ENTRY ( KDataClientIdle, CoreNetStates::TSendDataClientIdle, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TDoNothing )
NODEACTIVITY_END ( )
}


namespace UPnPServiceDefaultDataClientStopActivity
{
DECLARE_DEFINE_NODEACTIVITY ( ECFActivityStopDataClient, UPnPServiceDefaultDataClientStop, TCFDataClient::TStop  )
	FIRST_NODEACTIVITY_ENTRY ( CoreNetStates::TAwaitingDataClientStop, CoreNetStates::TNoTagOrDataClientsToStop )
	
	NODEACTIVITY_ENTRY ( CoreNetStates::KDataClientsToStop, UPnPServiceDefaultStates::TProcessDataClientStop,  CoreNetStates::TAwaitingDataClientsStopped, TNoTag )	
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, TDoNothing, CoreNetStates::TNoTagOrUnbindOnStop )
	
	NODEACTIVITY_ENTRY ( CoreNetStates::KUnbind, CoreNetStates::TSendClientLeavingRequestToServiceProvider, CoreStates::TAwaitingLeaveComplete, TNoTag )
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, UPnPStateMachine::TDestroyDataClients, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, PRStates::TSendDataClientStopped )
NODEACTIVITY_END ( )
}


namespace UPnPServiceDefaultDestroyActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( ECFActivityDestroy, UPnPServiceDefaultDestroy, TEChild::TDestroy, CoreActivities::CDestroyActivity::New )
	FIRST_NODEACTIVITY_ENTRY ( TAwaitingDestroy, UPnPStateMachine::TBlockedByAllActivitiesStopped )

	//Stop self first
    NODEACTIVITY_ENTRY ( KNoTag, CoreNetStates::TStopSelf, CoreNetStates::TAwaitingDataClientStopped, TNoTag )

 	LAST_NODEACTIVITY_ENTRY ( KNoTag, CoreNetStates::TSendClientLeavingAndRemoveControlProvider )
NODEACTIVITY_END()
}


namespace UPnPServiceDefaultRegisterRootDeviceActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( TUpnpMessage::EActivityRegisterRootDevice, UPnPServiceDefaultRegisterRootDevice, TUpnpMessage::TRegisterRootDevice, CUpnpRegisterRootDeviceActivity::NewL )
    FIRST_NODEACTIVITY_ENTRY ( TAwaitingRootDeviceRegister, TNoTag )
    
    THROUGH_NODEACTIVITY_ENTRY ( KNoTag, TBuildPublishAndRootDeviceInfo, TTag<KSendRequest> )
	THROUGH_NODEACTIVITY_ENTRY ( KSendRequest, UPnPServiceDefaultStates::TSendPublishRequest, TNoTag )
	
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, TRegisterRootDeviceWithMuServer, TNoTag )
	NODEACTIVITY_ENTRY ( KNoTag, UPnPServiceDefaultStates::TStartMuServer, CoreNetStates::TAwaitingDataClientStarted, TNoTag )
	
	NODEACTIVITY_ENTRY ( KNoTag, TCreateServerFlow<ERootDescription>, CoreNetStates::TAwaitingDataClientJoin, TErrorTagOr<TTag<KNoTag> > )
	NODEACTIVITY_ENTRY ( KNoTag, UPnPStateMachine::TProcessServerFlowCreation, CoreNetStates::TAwaitingDataClientStarted, CUpnpRegisterRootDeviceActivity::TIconCreateOrNoTag )
	
	NODEACTIVITY_ENTRY ( KCreateIconTag, CUpnpRegisterRootDeviceActivity::TCreateIconFlow, CoreNetStates::TAwaitingDataClientJoin, TErrorTagOr<TTag<KIconCreatedTag> > )
	NODEACTIVITY_ENTRY ( KIconCreatedTag, TProcessServerFlowCreation, CoreNetStates::TAwaitingDataClientStarted, CUpnpRegisterRootDeviceActivity::TIconOrNoTag )
	
	THROUGH_NODEACTIVITY_ENTRY ( KNoTag, TSendRootDeviceRegistered, TNoTag )
	
	THROUGH_NODEACTIVITY_ENTRY ( KErrorTag, TSendRootDeviceRegistered, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, TClearError )
NODEACTIVITY_END ( )
}


namespace UPnPSerivceDefaultSearchResponseActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( TUpnpMessage::EActivitySearchResponse, UPnPServiceDefaultSearchResponse, TUpnpMessage::TUPnPSearchRequest, CNodeParallelActivityBase::NewL )
	FIRST_NODEACTIVITY_ENTRY ( UPnPStateMachine::TAwaitingSearchRequest, UPnPServiceDefaultStates::TNoTagOrIgnoreTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, UPnPServiceDefaultStates::TSendSearchResponse )
	LAST_NODEACTIVITY_ENTRY ( CoreStates::KIgnore, UPnPStateMachine::TDiscardSearchRequestData )
NODEACTIVITY_END ( )
}


namespace UPnPSerivceDefaultDeviceRepublishActivity
{
DECLARE_DEFINE_NODEACTIVITY ( TUpnpMessage::EActivityDeviceRepublish, UPnPServiceDefaultDeviceRepublish, TUpnpMessage::TDeviceRepublish )
	FIRST_NODEACTIVITY_ENTRY ( UPnPServiceDefaultStates::TAwaitingDeviceRepublish, TNoTag )
	LAST_NODEACTIVITY_ENTRY ( KNoTag, UPnPServiceDefaultStates::TSendPublishRequest )
NODEACTIVITY_END ( )
}
	
namespace UpnpRootDeviceInfoRequestActivity
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY ( TUpnpMessage::EActivityRequestInfo, UpnpRootDeviceInfoRequestInfo, TUpnpMessage::TUPnPRequestInfo, CUpnpRootDeviceInfoRequestActivity::NewL )
	FIRST_NODEACTIVITY_ENTRY ( TAwaitingUpnpRequestInfo, UPnPServiceDefaultStates::TRequestOrIgnoreTag )
	LAST_NODEACTIVITY_ENTRY ( KDescribeRequest, CUpnpRootDeviceInfoRequestActivity::TComposeAndSendRootDescription )
	LAST_NODEACTIVITY_ENTRY ( KIconRequest, CUpnpRootDeviceInfoRequestActivity::TFindAndSendIconData )

	LAST_NODEACTIVITY_ENTRY ( CoreStates::KIgnore, UPnPStateMachine::TSendIgnoreResponse )
NODEACTIVITY_END ( )
}


namespace UPnPServiceDefaultActivities
{
DEFINE_ACTIVITY_MAP ( activityMap )
	ACTIVITY_MAP_ENTRY ( UPnPServiceDefaultBindToActivity, UPnPServiceDefaultBindToRequest )
	ACTIVITY_MAP_ENTRY ( UPnPServiceDefaultDataClientStartActivity, UPnPServiceDefaultDataClientStart )
	ACTIVITY_MAP_ENTRY ( UPnPServiceDefaultClientLeaveActivity, UPnPServiceDefaultClientLeave )
	ACTIVITY_MAP_ENTRY ( UPnPServiceDefaultDataClientStopActivity, UPnPServiceDefaultDataClientStop )
	ACTIVITY_MAP_ENTRY ( UPnPServiceDefaultDestroyActivity, UPnPServiceDefaultDestroy )
	ACTIVITY_MAP_ENTRY ( UPnPServiceDefaultRegisterRootDeviceActivity, UPnPServiceDefaultRegisterRootDevice )
	ACTIVITY_MAP_ENTRY ( UPnPSerivceDefaultSearchResponseActivity, UPnPServiceDefaultSearchResponse )
	ACTIVITY_MAP_ENTRY ( UPnPSerivceDefaultDeviceRepublishActivity, UPnPServiceDefaultDeviceRepublish )
	ACTIVITY_MAP_ENTRY ( UpnpRootDeviceInfoRequestActivity, UpnpRootDeviceInfoRequestInfo )
ACTIVITY_MAP_END_BASE ( SCprActivities, coreSCprActivities )
}


// -------------- device description request activity------------------------
CNodeActivityBase* CUpnpRootDeviceInfoRequestActivity::NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
	{
	TUint activityId = GetNextActivityCountL ( aActivitySig, aNode );
	return ( new ( ELeave ) CUpnpRootDeviceInfoRequestActivity ( aActivitySig, aNode, activityId ) );
	}


CUpnpRootDeviceInfoRequestActivity::CUpnpRootDeviceInfoRequestActivity ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aActivitiesCount )
:CNodeParallelActivityBase ( aActivitySig, aNode, aActivitiesCount )
	{	  	
	}


CUpnpRootDeviceInfoRequestActivity::~CUpnpRootDeviceInfoRequestActivity ( )
	{
	SetError ( KErrNone );
	}


DEFINE_SMELEMENT ( CUpnpRootDeviceInfoRequestActivity::TComposeAndSendRootDescription, NetStateMachine::MStateTransition, CUpnpRootDeviceInfoRequestActivity::TContext )
void CUpnpRootDeviceInfoRequestActivity::TComposeAndSendRootDescription::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPRootDeviceInfoReqActivity, KComponent, _L8("CUpnpRootDeviceInfoRequestActivity::TComposeAndSendRootDescription::DoL")));
	// compose current root device description xml data.
	CUPnPServiceConnectionProvider& ctrlProv = iContext.Node ( ).ConnectionProvider ( );
	
	CUPnPRootDeviceInfoContainer& container = ctrlProv.GetRootDeviceInfoContainer ( );
	CUPnPDeviceDescription& deviceDesc = container.GetRootDeviceDesciption ( );
    
    CUPnPDescriptionComposer& composer =  iContext.Node ( ).DeviceDescriptionComposerL ( );
    RBuf8 deviceXmlBuf;
    composer.ComposeDescriptionXmlL ( &deviceDesc,CUPnPDescriptionComposer::EDevice , deviceXmlBuf );
	
	CleanupClosePushL ( deviceXmlBuf );
	//Send composed xml as response body to TCP Server Flow i.e current msg sender		
	RMemChunk bufChain;
	bufChain.CreateL ( deviceXmlBuf, ctrlProv.MemoryAllocator() );	
	CleanupStack::PopAndDestroy ( &deviceXmlBuf );
	
	TUpnpMessage::TUPnPRequestInfo& requestMsg = message_cast< TUpnpMessage::TUPnPRequestInfo > ( iContext.iMessage );
	CTransaction* trans = static_cast<CTransaction*> ( requestMsg.iPtr );
	trans->AddBodyPart ( bufChain );
		
	LOG(ESockLogExternal::Printf(KSubsysSPRootDeviceInfoReqActivity, KComponent, _L8("CUpnpRootDeviceInfoRequestActivity::TComposeAndSendRootDescription::DoL - Posting messge TUpnpMessage::TUPnPResponseInfo")));	
	TUpnpMessage::TUPnPResponseInfo msg ( trans, HTTPStatus::EOk, NULL );
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.iSender, msg );
	}
	
DEFINE_SMELEMENT ( CUpnpRootDeviceInfoRequestActivity::TFindAndSendIconData, NetStateMachine::MStateTransition, CUpnpRootDeviceInfoRequestActivity::TContext )
void CUpnpRootDeviceInfoRequestActivity::TFindAndSendIconData::DoL ( )
	{
	TUpnpMessage::TUPnPRequestInfo& requestInfoMsg = message_cast<TUpnpMessage::TUPnPRequestInfo> ( iContext.iMessage );
	
	CTransaction* transaction = static_cast<CTransaction*> ( requestInfoMsg.iPtr );	
	TInt statusCode;	
	CUPnPUtils::FindAndSendIconDataL ( transaction, statusCode, iContext.Node ( ).ConnectionProvider().MemoryAllocator() );
	
	TUpnpMessage::TUPnPResponseInfo msg ( transaction, statusCode, NULL );
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.iSender, msg );
	}
//-------------------------Embedded Device Session Registration Activity-------------------------------------
CNodeActivityBase* CUpnpRegisterRootDeviceActivity::NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
	{
	return new ( ELeave ) CUpnpRegisterRootDeviceActivity ( aActivitySig, aNode ) ;
	}


CUpnpRegisterRootDeviceActivity::CUpnpRegisterRootDeviceActivity ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode  )
	: CNodeActivityBase ( aActivitySig, aNode ), iIconUrlIdx ( 0 )
	{	  	
	}

CUpnpRegisterRootDeviceActivity::~CUpnpRegisterRootDeviceActivity ( )
	{
	SetError ( KErrNone	);
	iIconUrl.Close ();
	}

DEFINE_SMELEMENT ( CUpnpRegisterRootDeviceActivity::TCreateIconFlow, NetStateMachine::MStateTransition, CUpnpRegisterRootDeviceActivity::TContext )
void CUpnpRegisterRootDeviceActivity::TCreateIconFlow::DoL ()
	{	
	CUpnpRegisterRootDeviceActivity& activity = static_cast<CUpnpRegisterRootDeviceActivity&> ( *iContext.iNodeActivity );
	CUPnPServiceConnectionProvider& ctrlProv = iContext.Node ().ConnectionProvider ();
	
	const CUPnPDevice& device = ctrlProv.RootDevice ();    
    CStringPoolManager& stringPoolMgr = ctrlProv.StringPoolManager ();
	RStringPool& sp = stringPoolMgr.StringPool ();

	activity.iIconUrl.Close ();
	const TDesC8& url = device.AtIconInfoTable ( activity.iIconUrlIdx )->Property ( sp.String ( UPNPDESCRIPTIONXMLTAGS::EUrl, stringPoolMgr.GetUPnPTable () ) );
	const TDesC8& rootLoc = ctrlProv.RootDeviceLocation ();
	
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

DEFINE_SMELEMENT ( CUpnpRegisterRootDeviceActivity::TIconCreateOrNoTag, NetStateMachine::MStateFork, CUpnpRegisterRootDeviceActivity::TContext )
TInt CUpnpRegisterRootDeviceActivity::TIconCreateOrNoTag::TransitionTag ()
	{
	CUPnPServiceConnectionProvider& ctrlProv = iContext.Node ().ConnectionProvider ();
	const CUPnPDevice& device = ctrlProv.RootDevice ();
    
	return device.CountOfIconInfoTable () != 0 ? KCreateIconTag : KNoTag;
	}

DEFINE_SMELEMENT ( CUpnpRegisterRootDeviceActivity::TIconOrNoTag, NetStateMachine::MStateFork, CUpnpRegisterRootDeviceActivity::TContext )
TInt CUpnpRegisterRootDeviceActivity::TIconOrNoTag::TransitionTag ()
	{
	CUpnpRegisterRootDeviceActivity& activity = static_cast<CUpnpRegisterRootDeviceActivity&> ( *iContext.iNodeActivity );
	CUPnPServiceConnectionProvider& ctrlProv = iContext.Node ( ).ConnectionProvider ( );	
	const CUPnPDevice& device = ctrlProv.RootDevice ( );
	
	activity.iIconUrlIdx++;
	if ( device.CountOfIconInfoTable () != activity.iIconUrlIdx )
		{
		return NetStateMachine::EBackward | KCreateIconTag;
		}
	
	return NetStateMachine::EForward | KNoTag;
	}	

