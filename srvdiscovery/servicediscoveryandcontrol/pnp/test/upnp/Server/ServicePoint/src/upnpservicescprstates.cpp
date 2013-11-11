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
#include <inetprottextutils.h>
#include <rmemchunk.h>
#include <rmemcell.h>

//Local Includes
#include "upnpservicescprstates.h"
#include "upnpstatemachine.h"
#include "upnp_cf_msgs.h"
#include "upnpflowfactory.h"
#include "upnpserverconstants.h"
#include "upnplog.h"

__FLOG_STMT(_LIT8(KComponent,"SPScprSts");)



using namespace ESock;
using namespace CorePanics;
using namespace UPnPServiceSCprStates;


//-=========================================================
//
//States
//
//-=========================================================  	
DEFINE_SMELEMENT ( TAwaitingServiceValidated, NetStateMachine::MState, TContext )
TBool TAwaitingServiceValidated::Accept ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TAwaitingServiceValidated::Accept")));
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TServiceValidated> ( ) )
		{
		LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TAwaitingServiceValidated::Accept - Message=TUpnpMessage::TServiceValidated")));
		TUpnpMessage::TServiceValidated& msg = message_cast<TUpnpMessage::TServiceValidated> ( iContext.iMessage );
		iContext.iNodeActivity->SetError( msg.iValue );	
		return ETrue;
		}
	return EFalse;
	}
	
DEFINE_SMELEMENT ( TAwaitingServiceRegisteredFromCpr, NetStateMachine::MState, TContext )
TBool TAwaitingServiceRegisteredFromCpr::Accept ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TAwaitingServiceRegisteredFromCpr::Accept")));
	if ( iContext.iMessage.IsMessage<TUpnpMessage::TServiceRegistered> ( ) )
		{
		LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TAwaitingServiceRegisteredFromCpr::Accept - Message=TUpnpMessage::TServiceRegistered")));
		TUpnpMessage::TServiceRegistered& msg = message_cast<TUpnpMessage::TServiceRegistered> ( iContext.iMessage );
		iContext.iNodeActivity->SetError( msg.iValue );		
		return ETrue;
		}
	return EFalse;
	}
	
//-=========================================================
//
//Transitions
//
//-=========================================================

DEFINE_SMELEMENT ( TStoreRequestParams, NetStateMachine::MStateTransition, TContext )
void TStoreRequestParams::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TStoreRequestParams::DoL")));	

#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	TCFScpr::TSetParamsRequest* setParamsRequest = message_cast<TCFScpr::TSetParamsRequest>( &iContext.iMessage );
#else
	TCFScpr::TParamsRequest* setParamsRequest = message_cast<TCFScpr::TParamsRequest>( &iContext.iMessage );
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	ASSERT ( setParamsRequest );
	
	//Extract the parambundle from the request message.
	if ( !setParamsRequest->iFamilyBundle.IsNull() )
    	{
    	iContext.Node().iRequestBundle.DeleteFamilyAtIndex (0);
		
    	HBufC8* buf = HBufC8::NewL ( setParamsRequest->iFamilyBundle.Length() );
    	TPtr8 bufPtr ( buf->Des() );
    	
    	setParamsRequest->iFamilyBundle.Store ( bufPtr );        	
    	iContext.Node().iRequestBundle.LoadL ( bufPtr );
    	delete buf;
    	}
	}


DEFINE_SMELEMENT ( TSendRequestParamResponse, NetStateMachine::MStateTransition, TContext )
void TSendRequestParamResponse::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TSendRequestParamResponse::DoL")));	
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.iSender, TCFScpr::TSetParamsResponse ( iContext.Node().iResponseBundle ).CRef () );
#else
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.iSender, TCFScpr::TParamsResponse ( iContext.Node().iResponseBundle ).CRef () );
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	}

DEFINE_SMELEMENT ( TInitiateActivities, NetStateMachine::MStateTransition, TContext )
void TInitiateActivities::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TInitiateActivities::DoL")));		
	iContext.Node ( ).InitiateActivitiesL ( );
	}


DEFINE_SMELEMENT ( TLeaveRequest, NetStateMachine::MStateTransition, TContext )
void TLeaveRequest::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TLeaveRequest::DoL")));	
	iContext.Node ( ).SetClosing ( );
	}
	
DEFINE_SMELEMENT ( TSendPublishResponseToClient, NetStateMachine::MStateTransition, TContext )
void TSendPublishResponseToClient::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TSendPublishResponseToClient::DoL")));		
	
	TUPnPEvent event;
		
	event.iErrorCode = iContext.iNodeActivity->Error ( );
	if ( event.iErrorCode != KErrNone )
		event.iStatus = TUPnPEvent::EFail;
	else 
		event.iStatus = TUPnPEvent::ESuccess;
	
	iContext.Node ( ).AddResponseParamSetL ( event);
	
	//clear error code
	iContext.iNodeActivity->SetError ( KErrNone );
	}

DEFINE_SMELEMENT ( TSendDataClientStart, NetStateMachine::MStateTransition, TContext )
void TSendDataClientStart::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TSendDataClientStart::DoL")));	
	TCFFactory::TPeerFoundOrCreated& msg = message_cast<TCFFactory::TPeerFoundOrCreated>( iContext.iMessage );
	iContext.iNodeActivity->PostRequestTo ( msg.iNodeId, TCFDataClient::TStart ().CRef () );
	}

DEFINE_SMELEMENT ( TProcessDataClientStop, NetStateMachine::MStateTransition, TContext )
void TProcessDataClientStop::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TProcessDataClientStop::DoL")));		
	
	CUPnPPublishInfoElement* publishInfo = iContext.Node ( ).PublishInfo ( );
	
	if ( publishInfo )
		{		
		// Send UPnP ByeBye packets
		if ( iContext.Node ( ).IsDevice ( ) )
			{					
			// Publish once for uuid:device-UUID
			SendByeRequestL ( publishInfo->Uuid( ), publishInfo->Uuid( ) );
			}
		
		// Publish once for uuid:device-UUID::urn:domain-name-device:deviceType:v
		// (or) Publish once for uuid:device-UUID::urn:domain-name-service:serviceType:v
		SendByeRequestL ( publishInfo->SearchTarget( ), publishInfo->Usn( ) );
	
		const CUPnPSubscribeInfoContainer* subscribeCont = iContext.Node ( ).GetSubscribeInfoContainer ( );	
		if ( subscribeCont->CountOfSubscribeInfoElementArray( ) != 0 )
			{
			// trigger CPR to stop subscription timer			
			RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.Node ().ControlProvider ()->RecipientId (), TUpnpMessage::TStopTimer ().CRef () );
			}
							 
		// StopDataClients
		iContext.Node().PostToClients<TDefaultClientMatchPolicy> ( TNodeCtxId ( iContext.ActivityId (), iContext.NodeId () ),
									TCFDataClient::TStop ( KErrNone ).CRef (), TClientType ( TCFClientType::EData ) );		
		iContext.iNodeActivity->SetPostedTo( TNodeId::NullId ( ) );
		}	
	}

void TProcessDataClientStop::SendByeRequestL ( const TDesC8& aSearchTarget, const TDesC8& aUsn )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TProcessDataClientStop::SendByeRequestL")));		
	RMemoryAllocator allocator = iContext.Node ( ).ConnectionProvider().MemoryAllocator();
	
	RMemChunk stBuf;
	stBuf.CreateL ( aSearchTarget, allocator );
	TCleanupItem item ( &UPnPStateMachine::CUPnPUtils::CleanupMBufChain, &stBuf );
	CleanupStack::PushL ( item );
	
	RMemChunk usnBuf;
	usnBuf.CreateL ( aUsn, allocator );
	
	TSsdpInfo ssdpInfo ( usnBuf, stBuf );	
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.Node ().UdpClientFlow () , TUpnpMessage::TUPnPPublishByeRequest ( ssdpInfo ).CRef () );
	CleanupStack::Pop ();
	}

DEFINE_SMELEMENT ( TSendDataClientIdleOrUnregister, NetStateMachine::MStateTransition, TContext )
void TSendDataClientIdleOrUnregister::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TSendDataClientIdleOrUnregister::DoL")));
	
	CUPnPPublishInfoElement* publishInfo = iContext.Node ( ).PublishInfo ( );
	
	if ( publishInfo == NULL )
		{		 
		RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.Node ().ControlProvider ()->RecipientId (), TCFControlProvider::TIdle ().CRef () );	
		}
	else
		{
		const TDesC8& key = publishInfo->Key( );
		if ( iContext.Node ( ).IsDevice ( ) )
			{
			RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.Node ().ControlProvider ()->RecipientId (), TUpnpMessage::TUnregisterDevice ( key.Ptr ( ), key.Size ( ) ).CRef () );		
			}
		else
			{
			RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.Node ().ControlProvider ()->RecipientId (), TUpnpMessage::TUnregisterService ( key.Ptr ( ), key.Size ( ) ).CRef () );
			}
		}
	}


//-------------------M-Search Response Activity----------------------------
DEFINE_SMELEMENT ( TValidateAndSendResponse, NetStateMachine::MStateTransition, TContext )
void TValidateAndSendResponse::DoL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TValidateAndSendResponse::DoL")));
	
	TUpnpMessage::TUPnPSearchRequest& reqMsg = message_cast< TUpnpMessage::TUPnPSearchRequest > ( iContext.iMessage );
		
	RMemCell* mBuf = reqMsg.iSsdpInfo.iSearchTarget.First ( );
	TPtr8 stPtr ( mBuf->Ptr( ), mBuf->Length( ), mBuf->Length( ) );
		
	CUPnPPublishInfoElement*  publishInfo = iContext.Node ( ).PublishInfo ( );
	
	if ( stPtr.CompareF ( publishInfo->Uuid ( ) ) == 0 )
		{
		// Respond once for uuid:device-UUID
		SendSearchResponseL ( publishInfo->Uuid ( ), publishInfo->Uuid ( ), publishInfo->CacheControl ( ) );
		}
	else
		{
		SendSearchResponseL ( stPtr, publishInfo->Usn ( ), publishInfo->CacheControl ( ) );
		}				

	reqMsg.iSsdpInfo.iSearchTarget.Free ( );
	}
			
void TValidateAndSendResponse::SendSearchResponseL ( const TDesC8& aSearchTarget, const TDesC8& aUsn, TInt aMaxAge )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TValidateAndSendResponse::SendSearchResponseL")));
	
	TUpnpMessage::TUPnPSearchRequest& reqMsg = message_cast< TUpnpMessage::TUPnPSearchRequest > ( iContext.iMessage );		
	TAppProtAddr remoteUnicastAddr = reqMsg.iSsdpInfo.iAddr;
	RMemoryAllocator allocator = iContext.Node ( ).ConnectionProvider().MemoryAllocator();
	
	RMemChunk locationBuf;
	locationBuf.CreateL ( iContext.Node ().ConnectionProvider ().RootDeviceLocation (), allocator );
	TCleanupItem item ( &UPnPStateMachine::CUPnPUtils::CleanupMBufChain, &locationBuf );
	CleanupStack::PushL ( item );
	
	RMemChunk stBuf;
	stBuf.CreateL ( aSearchTarget, allocator );	
	TCleanupItem item1 ( &UPnPStateMachine::CUPnPUtils::CleanupMBufChain, &stBuf );
	CleanupStack::PushL ( item1 );
	
	RMemChunk usnBuf;
	usnBuf.CreateL ( aUsn, allocator );
	
	TSsdpInfo ssdpInfo ( aMaxAge, locationBuf, usnBuf, stBuf, remoteUnicastAddr );
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.Node ().UdpClientFlow (), TUpnpMessage::TUPnPSearchResponse ( ssdpInfo ).CRef () );

	CleanupStack::Pop ( ); // item
	CleanupStack::Pop ( ); // item1
	}
		
			
//-=========================================================
//
//State Forks
//
//-=========================================================
DEFINE_SMELEMENT ( TStoreUdpClientAndSendRequest, NetStateMachine::MStateFork, TContext )
TInt TStoreUdpClientAndSendRequest::TransitionTag ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TStoreUdpClientAndSendRequest::TransitionTag")));	
	TCFFactory::TPeerFoundOrCreated &msg = message_cast<TCFFactory::TPeerFoundOrCreated> ( iContext.iMessage );
	iContext.Node ( ).SetUdpClientFlow ( msg.iNodeId );
	return UPnPStateMachine::KSendRequest;
	}
	
DEFINE_SMELEMENT ( TStoreUdpServer, NetStateMachine::MStateFork, TContext )
TInt TStoreUdpServer::TransitionTag ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TStoreUdpServer::TransitionTag")));	
	TCFFactory::TPeerFoundOrCreated &msg = message_cast<TCFFactory::TPeerFoundOrCreated>( iContext.iMessage );
	iContext.Node ( ).SetUdpServerFlow ( msg.iNodeId );
	return MeshMachine::KNoTag;
	}

	
DEFINE_SMELEMENT ( TNoTagOrIgnoreTag, NetStateMachine::MStateFork, TContext )
TInt TNoTagOrIgnoreTag::TransitionTag ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysSPScprSts, KComponent, _L8("TNoTagOrIgnoreTag::TransitionTag")));	
	return iContext.Node ( ).IsClosing ( ) ? CoreStates::KIgnore : MeshMachine::KNoTag;
	}
