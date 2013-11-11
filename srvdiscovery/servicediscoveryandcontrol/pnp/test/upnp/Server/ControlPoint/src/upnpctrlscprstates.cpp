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

//System Includes
#include <ss_glob.h>
#include <http/thttptable.h>
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_logext.h>
#include <upnp/tupnptable.h>

#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <httperr.h>
#include <inetprottextutils.h>

//Local Includes
#include "upnpctrlscprstates.h"
#include "upnpstatemachine.h"
#include "upnp_cf_msgs.h"
#include "upnpctrlscpractivities.h"
#include "upnpserverconstants.h"
#include "upnplog.h"

using namespace ESock;
using namespace CorePanics;
using namespace UPnPControlSCprStates;
using namespace UPnPStateMachine;
using namespace PRActivities;
using namespace UPnPSearchStates;
using namespace UPnPRegisterNotifyStates;
using namespace CoreActivities;

__FLOG_STMT(_LIT8(KComponent,"UPnPCtrlScprSts");)

//===========================UPnP Control SCPR states and activities==========================   	   

	
DEFINE_SMELEMENT ( TAwaitingDataClientJoin, NetStateMachine::MState, UPnPControlSCprStates::TContext )
TBool TAwaitingDataClientJoin::Accept ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("TAwaitingDataClientJoin")));
	
	TCFFactory::TPeerFoundOrCreated* dcJoined = message_cast<TCFFactory::TPeerFoundOrCreated> ( &iContext.iMessage );	
	
	if ( dcJoined )
    	{
    	if ( iContext.iNodeActivity->Error( ) != KErrNone )
        	{
            RNodeInterface* clientAlreadyAdded = iContext.Node ( ).FindClient ( dcJoined->iNodeId );
            if ( clientAlreadyAdded == NULL && dcJoined->iNodeId != TNodeId::NullId ( ) )
                {
                RClientInterface::OpenPostMessageClose ( iContext.NodeId (), dcJoined->iNodeId, TEChild::TDestroy ().CRef () );
                }
        	}
        return ETrue;
    	}
    else if ( iContext.iMessage.IsMessage<TUpnpMessage::TCancelRequest> ( ) )
        {
        // We can't send TCancel to the factory, we better eat the message
        // here and switch the activity into a cancelling mode.
        iContext.iMessage.ClearMessageId ( );
        iContext.iNodeActivity->SetError( KErrCancel );
        }
    else if ( iContext.iMessage.IsMessage<TUpnpMessage::TCancelParamSet> ( ) )
    	{    	
    	CUPnPActivityBase& activity = static_cast<CUPnPActivityBase&> ( *iContext.iNodeActivity );
		TUpnpMessage::TCancelParamSet& msg = message_cast< TUpnpMessage::TCancelParamSet > ( iContext.iMessage );
		CUPnPRequestParamSetBase* paramSet = static_cast<CUPnPRequestParamSetBase*>(msg.iParamSet);
		
		if ( activity.iRequestParamSet->Uri ( ).CompareF ( paramSet->Uri() ) == 0 )
			{
    		// We can't send TCancel to the factory, we better eat the message
        	// here and switch the activity into a cancelling mode.
        	iContext.iMessage.ClearMessageId ( );
        	iContext.iNodeActivity->SetError( KErrCancel );
        	
        	// delete cancel paramset based on type id.        	
        	STypeId metaType = activity.iRequestParamSet->GetTypeId( );
        	switch ( metaType.iType )
        		{
        		case EUPnPCancelDiscoverParamSet:
        			delete static_cast<CUPnPCancelDiscoverParamSet*>(paramSet);
        		break;
        		
        		case EUPnPCancelRegisterNotifyParamSet:
    	        	delete static_cast<CUPnPCancelRegisterNotifyParamSet *>(paramSet);
	       		break;
	       		
        		case EUPnPCancelDescribeParamSet:
        			delete static_cast<CUPnPCancelDescribeParamSet *>(paramSet);
        		break;
        		
        		case EUPnPCancelSubscribeParamSet:
        			delete static_cast<CUPnPCancelSubscribeParamSet *>(paramSet);
        		break;
        		
        		default:
				break;      	
        		}
        	paramSet = NULL;
			}        
    	}
		 	
    return EFalse;
	}	
	

DEFINE_SMELEMENT ( TStoreRequestParams, NetStateMachine::MStateTransition, UPnPControlSCprStates::TContext )
void TStoreRequestParams::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("TStoreRequestParams")));
	
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	TCFScpr::TSetParamsRequest* setParamsRequest = message_cast<TCFScpr::TSetParamsRequest>( &iContext.iMessage );
#else
	TCFScpr::TParamsRequest* setParamsRequest = message_cast<TCFScpr::TParamsRequest>( &iContext.iMessage );
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	ASSERT ( setParamsRequest );
	
	//Extract the parambundle from the request message.	
	if ( !setParamsRequest->iFamilyBundle.IsNull() )
    	{
    	HBufC8* buf = HBufC8::NewL ( setParamsRequest->iFamilyBundle.Length() );
    	TPtr8 bufPtr ( buf->Des() );
    	
    	setParamsRequest->iFamilyBundle.Store ( bufPtr );
    	iContext.Node().iRequestBundle.LoadL ( bufPtr );
    	delete buf;
    	}
	}


DEFINE_SMELEMENT ( TSendRequestParamResponse, NetStateMachine::MStateTransition, UPnPControlSCprStates::TContext )
void TSendRequestParamResponse::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("TSendRequestParamResponse")));
	
	if ( iContext.Node().iRequestBundle.GetFamilyAtIndex (0).Id () == EUPnPClearParamSet )
		{
		iContext.Node().iResponseBundle.Close ();	// DEF126893 fix pending
		iContext.Node().iResponseBundle.CreateL ();
		iContext.Node().iResponseBundle.Open ();
		}
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.iSender, TCFScpr::TSetParamsResponse ( iContext.Node().iResponseBundle ).CRef () );
#else
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.iSender, TCFScpr::TParamsResponse ( iContext.Node().iResponseBundle ).CRef () );
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

	}

DEFINE_SMELEMENT ( TInitiateActivities, NetStateMachine::MStateTransition, UPnPControlSCprStates::TContext )
void TInitiateActivities::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("TInitiateActivities")));	
	iContext.Node ( ).InitiateActivitiesL ( );
	}

DEFINE_SMELEMENT ( TCreateControlDataClient, NetStateMachine::MStateTransition, UPnPControlSCprStates::TContext )
void TCreateControlDataClient::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("TCreateControlDataClient")));
	
	TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
	const TUid uid = { CUPnPFlowFactory::iUid };
	THttpClientFlowQuery flowQuery ( TAppProtAddr ( 0 , 0 ), iContext.NodeId (), EHttpClientFlow, THttpClientFlowQuery::ECreateNew );
		
	iContext.iNodeActivity->PostRequestTo( factoryContainer, TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane , uid, &flowQuery ).CRef () );
	}

	
DEFINE_SMELEMENT ( TProcessCallbackDataClient, NetStateMachine::MStateTransition, UPnPControlSCprStates::TContext )
void TProcessCallbackDataClient::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("TProcessCallbackDataClient")));
	
	const TNodeId& dataClient = (message_cast<TCFFactory::TPeerFoundOrCreated>(iContext.iMessage)).iNodeId;
	iContext.Node ().SetTcpServerFlow ( dataClient );
	
	// start the data client
	iContext.iNodeActivity->PostRequestTo ( dataClient, TCFDataClient::TStart ().CRef () );
	}

DEFINE_SMELEMENT ( TSendActivityStopped, NetStateMachine::MStateTransition, UPnPControlSCprStates::TContext )
void TSendActivityStopped::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("TSendActivityStopped")));
	
	if ( iContext.Node ( ).IsClosing ( ) 
			&& iContext.iNode.CountAllActivities () == 2 ) // only - destroy and this upnp activity is pending
		{
		RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.NodeId (), TUpnpMessage::TActivityStopped ().CRef () );	
		}
	
	// since upnp control scpr ativities are self originated, clear activity error
	iContext.iNodeActivity->SetError( KErrNone );
	}
	
DEFINE_SMELEMENT ( TFindHttpClientFlowOrSendRequest, NetStateMachine::MStateFork, UPnPControlSCprStates::TContext )
TInt TFindHttpClientFlowOrSendRequest::TransitionTag ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("TFindHttpClientFlowOrSendRequest")));
	
	TAppProtAddr addr ( 0, 0 );
	CUPnPActivityBase* activity = static_cast<CUPnPActivityBase*> ( iContext.iNodeActivity );		
	
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TDescribeRequest> ( ) )
		{
		TUpnpMessage::TDescribeRequest& describeMsg = message_cast<TUpnpMessage::TDescribeRequest> ( iContext.iMessage );
		activity->iRequestParamSet =  static_cast<CUPnPDescribeRequestParamSet*> ( describeMsg.iParamSet );	
		addr = static_cast<CUPnPDescribeRequestParamSet *>(describeMsg.iParamSet)->RemoteHost();
		}
	else if ( iContext.iMessage.IsMessage<TUpnpMessage::TSubscribeRequest> ( ) )
		{
		TUpnpMessage::TSubscribeRequest& subscribeMsg = message_cast<TUpnpMessage::TSubscribeRequest> ( iContext.iMessage );
		activity->iRequestParamSet =  static_cast<CUPnPSubscribeRequestParamSet*> ( subscribeMsg.iParamSet );
		addr = static_cast<CUPnPSubscribeRequestParamSet *> ( subscribeMsg.iParamSet )->RemoteHost();					
		}	
		
	TInt idx = 0;
	TBool dcFound = EFalse;
	TBool dcInitialising = EFalse;
	while ( iContext.Node().iFlowInfo.Count ( ) != idx )
		{
		if ( iContext.Node().iFlowInfo[idx].iAppProtAddr == addr )
			{
			if ( iContext.Node().iFlowInfo[idx].iDataClient != TNodeId::NullId ( ) )
				{
				dcFound = ETrue;
				activity->iFlowId = iContext.Node().iFlowInfo[idx].iDataClient;
				}
			else
				{
				dcInitialising = ETrue;
				}
			break;
			}	
		idx++;
		}
	if ( dcFound )
		{
		return UPnPStateMachine::KSendRequest;
		}
	if ( !dcInitialising )
		{
		CUPnPControlSubConnectionProvider::TFlowInfo newDataClient;
		newDataClient.iAppProtAddr = addr;
		iContext.Node().iFlowInfo.Append ( newDataClient );
		}
	return MeshMachine::KNoTag;
	}

DEFINE_SMELEMENT ( TFindOrCreateHttpClientFlow, NetStateMachine::MStateTransition, UPnPControlSCprStates::TContext )
void TFindOrCreateHttpClientFlow::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("TFindOrCreateHttpClientFlow::DoL")));
	CUPnPActivityBase& activity = static_cast<CUPnPActivityBase&> ( *iContext.iNodeActivity );
	
	TAppProtAddr remoteHost ( 0, 0 );
	STypeId metaType = activity.iRequestParamSet->GetTypeId( );
	if (  metaType.iType == EUPnPDescribeRequestParamSet )
		{
	 	CUPnPDescribeRequestParamSet* paramSet = static_cast<CUPnPDescribeRequestParamSet*> ( activity.iRequestParamSet );
	 	remoteHost = paramSet->RemoteHost( );
		}
	else if ( metaType.iType == EUPnPSubscribeRequestParamSet )
		{
		CUPnPSubscribeRequestParamSet* paramSet = static_cast<CUPnPSubscribeRequestParamSet*> ( activity.iRequestParamSet );
		remoteHost = paramSet->RemoteHost( );
		}
	
	const TUid requestedUid = { CUPnPFlowFactory::iUid };
	
	THttpClientFlowQuery flowQuery ( remoteHost,
									iContext.NodeId (),
									EHttpClientFlow,
									THttpClientFlowQuery::EUseExisting );		
	TCFPlayerRole playerRole ( TCFPlayerRole::EDataPlane );
									
	TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( playerRole );
	activity.PostRequestTo ( factoryContainer, TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
	}


DEFINE_SMELEMENT ( TProcessHttpDataClient, NetStateMachine::MStateTransition, UPnPControlSCprStates::TContext )
void TProcessHttpDataClient::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("TProcessHttpDataClient")));
	
	CUPnPActivityBase& activity = static_cast< CUPnPActivityBase& >( *iContext.iNodeActivity );
	
	TAppProtAddr remoteHost;
	STypeId metaType = activity.iRequestParamSet->GetTypeId( );
	if (  metaType.iType == EUPnPDescribeRequestParamSet )
		{
	 	CUPnPDescribeRequestParamSet* paramSet = static_cast<CUPnPDescribeRequestParamSet*> ( activity.iRequestParamSet );
	 	remoteHost = paramSet->RemoteHost( );
		}
	else if ( metaType.iType == EUPnPSubscribeRequestParamSet )
		{
		CUPnPSubscribeRequestParamSet* paramSet = static_cast<CUPnPSubscribeRequestParamSet*> ( activity.iRequestParamSet );
		remoteHost = paramSet->RemoteHost( );
		}
	
	TInt iDcIdx = 0;
	for ( iDcIdx = 0; iDcIdx < iContext.Node ( ).iFlowInfo.Count ( ); iDcIdx++ )
		{
		if ( iContext.Node ( ).iFlowInfo[iDcIdx].iAppProtAddr == remoteHost )
			{
			break;
			}
		}
	
	TCFFactory::TPeerFoundOrCreated &msg = message_cast<TCFFactory::TPeerFoundOrCreated>(iContext.iMessage );
	iContext.Node ( ).iFlowInfo[iDcIdx].iDataClient = msg.iNodeId;
	activity.iFlowId = msg.iNodeId;
	}
	

//======================================UPnP Search States============================================//

DEFINE_SMELEMENT ( UPnPSearchStates::TFindUdpClientFlowOrSendRequest, NetStateMachine::MStateFork, UPnPSearchStates::TContext )
TInt UPnPSearchStates::TFindUdpClientFlowOrSendRequest::TransitionTag ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("UPnPSearchStates::TFindUdpClientFlowOrSendRequest")));
	
	TUpnpMessage::TSearchRequest& msg = message_cast<TUpnpMessage::TSearchRequest> ( iContext.iMessage );
	CUPnPSearchActivity* activity = static_cast<CUPnPSearchActivity*> ( iContext.iNodeActivity );
	activity->iRequestParamSet =  static_cast<CUPnPDiscoverRequestParamSet*> ( msg.iParamSet );

	CUPnPControlSubConnectionProvider& node = static_cast<CUPnPControlSubConnectionProvider&>(iContext.Node());
	if ( node.UdpClientFlow() == TNodeId::NullId () )
		{
		return MeshMachine::KNoTag;
		}
	return UPnPStateMachine::KSendRequest;
	}

DEFINE_SMELEMENT ( UPnPSearchStates::TStoreUdpClientFlowAndSendRequest, NetStateMachine::MStateFork, UPnPSearchStates::TContext )
TInt UPnPSearchStates::TStoreUdpClientFlowAndSendRequest::TransitionTag ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("UPnPSearchStates::TStoreUdpClientFlowAndSendRequest")));
	
	TCFFactory::TPeerFoundOrCreated &msg = message_cast<TCFFactory::TPeerFoundOrCreated>(iContext.iMessage );
	iContext.Node().SetUdpClientFlow ( msg.iNodeId );
	return UPnPStateMachine::KSendRequest;
	}
	
DEFINE_SMELEMENT ( UPnPSearchStates::TCancelSearchOrProcessResponseTag, NetStateMachine::MStateFork, UPnPSearchStates::TContext )
TInt UPnPSearchStates::TCancelSearchOrProcessResponseTag::TransitionTag ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("UPnPSearchStates::TCancelSearchOrProcessResponseTag")));
	
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TUPnPSearchResponse>() )
		{
		return UPnPStateMachine::KProcessResponse;
		}
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TCancelParamSet>() 
		 || iContext.iMessage.IsMessage<TUpnpMessage::TCancelRequest>() )
		{
		return MeshMachine::KCancelTag;
		}
	return  MeshMachine::KErrorTag;
	}

DEFINE_SMELEMENT ( UPnPSearchStates::TNoTagOrResponseTag, NetStateMachine::MStateFork, UPnPSearchStates::TContext )
TInt UPnPSearchStates::TNoTagOrResponseTag::TransitionTag ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("UPnPSearchStates::TNoTagOrResponseTag")));
	
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TUPnPSearchResponse> () )
		{
		return UPnPStateMachine::KProcessResponse;
		}
	return  MeshMachine::KNoTag;
	}

//=================================UPnP Register Notify States =============================================================//

DEFINE_SMELEMENT ( UPnPRegisterNotifyStates::TFindMUdpFlowOrSendRequest, NetStateMachine::MStateFork, UPnPRegisterNotifyStates::TContext )
TInt UPnPRegisterNotifyStates::TFindMUdpFlowOrSendRequest::TransitionTag ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("UPnPRegisterNotifyStates::TFindMUdpFlowOrSendRequest")));
	
	TUpnpMessage::TRegisterNotifyRequest& msg = message_cast<TUpnpMessage::TRegisterNotifyRequest> ( iContext.iMessage );
	CUPnPRegisterNotifyActivity* activity = static_cast<CUPnPRegisterNotifyActivity*> ( iContext.iNodeActivity );
	activity->iRequestParamSet =  static_cast<CUPnPRegisterAnnouncementParamSet*> ( msg.iParamSet );

	CUPnPControlSubConnectionProvider& node = static_cast<CUPnPControlSubConnectionProvider&>(iContext.Node());
	if ( node.UdpServerFlow () == TNodeId::NullId () )
		{
		return MeshMachine::KNoTag;
		}
	return UPnPStateMachine::KSendRequest;
	}

DEFINE_SMELEMENT ( UPnPRegisterNotifyStates::TStoreMUdpFlowAndStartServer, NetStateMachine::MStateFork, UPnPRegisterNotifyStates::TContext )
TInt UPnPRegisterNotifyStates::TStoreMUdpFlowAndStartServer::TransitionTag ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("UPnPRegisterNotifyStates::TStoreMUdpFlowAndStartServer")));
	
	TCFFactory::TPeerFoundOrCreated &msg = message_cast<TCFFactory::TPeerFoundOrCreated> ( iContext.iMessage );
	iContext.Node ().SetUdpServerFlow ( msg.iNodeId );
	return MeshMachine::KNoTag;
	}
	
DEFINE_SMELEMENT ( UPnPRegisterNotifyStates::TCancelRegisterOrProcessResponseTag, NetStateMachine::MStateFork, UPnPRegisterNotifyStates::TContext )
TInt UPnPRegisterNotifyStates::TCancelRegisterOrProcessResponseTag::TransitionTag ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("UPnPRegisterNotifyStates::TCancelRegisterOrProcessResponseTag")));
	
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TUPnPPublishAliveRequest> () || iContext.iMessage.IsMessage<TUpnpMessage::TUPnPPublishByeRequest> () )
		{
		return UPnPStateMachine::KProcessResponse;
		}
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TCancelParamSet> () || iContext.iMessage.IsMessage<TUpnpMessage::TCancelRequest> () )
		{
		return MeshMachine::KCancelTag;
		}
	return  MeshMachine::KErrorTag;
	}
	
DEFINE_SMELEMENT ( UPnPRegisterNotifyStates::TNoTagOrResponseTag, NetStateMachine::MStateFork, UPnPRegisterNotifyStates::TContext )
TInt UPnPRegisterNotifyStates::TNoTagOrResponseTag::TransitionTag ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("UPnPRegisterNotifyStates::TNoTagOrResponseTag")));	
	return iContext.iMessage.IsMessage<TUpnpMessage::TUPnPPublishAliveRequest> () || iContext.iMessage.IsMessage<TUpnpMessage::TUPnPPublishByeRequest>() ? UPnPStateMachine::KProcessResponse : MeshMachine::KNoTag;
	}
	
//=================================UPnP Describe States =============================================================//

DEFINE_SMELEMENT ( UPnPDescribeStates::TCancelDescribeOrProcessResponseTag, NetStateMachine::MStateFork, UPnPDescribeStates::TContext )
TInt UPnPDescribeStates::TCancelDescribeOrProcessResponseTag::TransitionTag ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("CancelDescribeOrProcessResponseTag")));
	
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TResponse> () )
		{
		return KProcessResponse;
		}
	else if(iContext.iMessage.IsMessage<TUpnpMessage::TCancelParamSet>()
			|| iContext.iMessage.IsMessage<TUpnpMessage::TCancelRequest>() )
		{
		return MeshMachine::KCancelTag;
		}
	else if (iContext.iMessage.IsMessage<TEBase::TError>())
		{
		TEBase::TError& msg = message_cast<TEBase::TError> ( iContext.iMessage );
		if ( msg.iValue != KErrCancel )
			{
			return  MeshMachine::KErrorTag;
			}
		}
	return MeshMachine::KNoTag;
	}

//==============================================================================================//
DEFINE_SMELEMENT ( UPnPSubscribeStates::TErrorOrProcessResponseTag, NetStateMachine::MStateFork, UPnPDescribeStates::TContext )
TInt UPnPSubscribeStates::TErrorOrProcessResponseTag::TransitionTag ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("UPnPSubscribeStates::ErrorOrProcessResponseTag")));
	
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TSubscribeResponse> () )
		{
		return KProcessResponse;
		}
	else if ( iContext.iMessage.IsMessage<TEBase::TError> () )
		{
		return  MeshMachine::KErrorTag;
		}
	return MeshMachine::KNoTag;
	}


DEFINE_SMELEMENT ( UPnPSubscribeStates::TNoTagOrCancelSubscribe, NetStateMachine::MStateFork, UPnPSubscribeStates::TContext )
TInt UPnPSubscribeStates::TNoTagOrCancelSubscribe::TransitionTag ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("NoTagOrCancelSubscribe")));
	
	TUpnpMessage::TSubscribeResponse& msg = message_cast< TUpnpMessage::TSubscribeResponse > ( iContext.iMessage );
	
	if ( msg.iStatus == HTTPStatus::EOk && iContext.iNodeActivity->Error() == KErrCancel )
		{
		return MeshMachine::KCancelTag;
		}
		
	return MeshMachine::KNoTag;
	}
//==============================================================================================//
DEFINE_SMELEMENT ( TAwaitingBindToComplete, NetStateMachine::MState, UPnPStateMachine::TContext )
TBool TAwaitingBindToComplete::Accept ( )
	{
	CoreNetStates::TAwaitingBindToComplete awaitingBindToComplete ( iContext );
	if ( awaitingBindToComplete.Accept () )
		{
	    CCommsBinderActivity* binderActivity = reinterpret_cast<CCommsBinderActivity*> ( iContext.iNodeActivity->FetchExtInterface ( CCommsBinderActivity::KInterfaceId ) );
	    ASSERT ( binderActivity );
	    RNodeInterface* pendingBinder = binderActivity->Binder ();
		TUint c = iContext.Node ().CountActivities ( iContext.iNodeActivity->ActivitySigId () );
		if ( c == 1 )
			{
			pendingBinder->ClearFlags ( TCFClientType::EActivating );
			}
		//binderActivity->StoreBinder(NULL);

		return ETrue;
		}
	return EFalse;
	}
		
