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
// System Includes
// 
//

#include <ss_glob.h>
#include <http/thttptable.h>
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_logext.h>
#include <upnp/tupnptable.h>

#include <httperr.h>
#include <inetprottextutils.h>
#include <rmemchunk.h>

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
//===========================UPnP SCPR states and activities==========================   	   

DEFINE_SMELEMENT ( TFindOrCreateUdpClientFlow, NetStateMachine::MStateTransition, UPnPStateMachine::TContext )	
void TFindOrCreateUdpClientFlow::DoL ( )
	{
	THttpUdpFlowQuery flowQuery ( iContext.NodeId (), EHttpUdpFlow, iContext.Node ().BaseConnectionProvider ().MemoryManager() );
	const TUid requestedUid = { CUPnPFlowFactory::iUid };	
	TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
		
	iContext.iNodeActivity->PostRequestTo ( factoryContainer, TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
	}

DEFINE_SMELEMENT ( TFindOrCreateUdpServerFlow, NetStateMachine::MStateTransition, UPnPStateMachine::TContext )	
void TFindOrCreateUdpServerFlow::DoL ( )
	{	
	THttpUdpFlowQuery flowQuery ( iContext.NodeId (), EHttpMUdpFlow );
	const TUid requestedUid = { CUPnPFlowFactory::iUid };		
	TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
		
	iContext.iNodeActivity->PostRequestTo ( factoryContainer, TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
	}

	
DEFINE_SMELEMENT ( THandleControlChannelDataClientIdle, NetStateMachine::MStateTransition, UPnPStateMachine::TContext )
void THandleControlChannelDataClientIdle::DoL ( )
	{
	//This transition may only be triggered by a peer message from a data client
	ASSERT ( iContext.iPeer );
	ASSERT ( iContext.iPeer->Type() == TCFClientType::EData );
	
   	iContext.iPeer->PostMessage( TNodeCtxId ( ( iContext.iNodeActivity )? iContext.iNodeActivity->ActivityId( ) : MeshMachine::KActivityNull, iContext.NodeId () ),
   								 TEChild::TDestroy ().CRef () );
   	iContext.iNode.RemoveClient ( iContext.iPeer->RecipientId() );
    }	
    
DEFINE_SMELEMENT ( TStoreBindToOriginator, NetStateMachine::MStateTransition, UPnPStateMachine::TContext )
void TStoreBindToOriginator::DoL ()
	{
	ABindingActivity* bindingActivity = reinterpret_cast<ABindingActivity*>(iContext.iNodeActivity->FetchExtInterface(ABindingActivity::KInterfaceId));
    ASSERT(bindingActivity);
    bindingActivity->StoreOriginator(iContext.iSender);
	}
	
DEFINE_SMELEMENT ( TAwaitingSetParamRequest, NetStateMachine::MState, UPnPStateMachine::TContext )
TBool TAwaitingSetParamRequest::Accept ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("TAwaitingSetParamRequest::Accept")));
	TUint16 senderActivityId = address_cast<TNodeCtxId> ( iContext.iSender ).NodeCtx ();
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	if ( senderActivityId == ECFActivityParamRequest && 
		iContext.iMessage.IsMessage<TCFScpr::TSetParamsRequest> () )
#else
	if ( senderActivityId == ECFActivityParamRequest && 
		iContext.iMessage.IsMessage<TCFScpr::TParamsRequest> () )
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW		
		{
		LOG(ESockLogExternal::Printf(KSubsysCtrlScprSts, KComponent, _L8("TAwaitingSetParamRequest::Accept - Message=TCFMessage::TSubConnParamsRequest")));
		return ETrue;
		}
	return EFalse;
	}	   	   	
	
DEFINE_SMELEMENT ( TStartServerFlow, NetStateMachine::MStateTransition, UPnPStateMachine::TContext )
void TStartServerFlow::DoL ( )
	{
	TCFFactory::TPeerFoundOrCreated& msg = message_cast< TCFFactory::TPeerFoundOrCreated > ( iContext.iMessage );
	iContext.iNodeActivity->PostRequestTo ( msg.iNodeId, TCFDataClient::TStart ().CRef () );
	}

		
DEFINE_SMELEMENT ( TRequestLocalHost, NetStateMachine::MStateTransition, UPnPStateMachine::TContext )
void TRequestLocalHost::DoL ( )
	{
	iContext.iNodeActivity->PostRequestTo( iContext.Node ().ControlProvider()->RecipientId(), TUpnpMessage::TRequestLocalHost ().CRef () );
	}
	
DEFINE_SMELEMENT ( TSendIgnoreResponse, NetStateMachine::MStateTransition, UPnPStateMachine::TContext )	
void TSendIgnoreResponse::DoL ( )
	{
	TUpnpMessage::TUPnPRequestInfo& requestMsg = message_cast< TUpnpMessage::TUPnPRequestInfo > ( iContext.iMessage );
	
	RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.iSender,
		TUpnpMessage::TUPnPResponseInfo ( static_cast<CTransaction*> ( requestMsg.iPtr ),
		HTTPStatus::EInternalServerError, NULL ).CRef () );
	}
	
DEFINE_SMELEMENT ( TDiscardSearchRequestData, NetStateMachine::MStateTransition, UPnPStateMachine::TContext )
void TDiscardSearchRequestData::DoL ( )
	{
	TUpnpMessage::TUPnPSearchRequest& reqMsg = message_cast< TUpnpMessage::TUPnPSearchRequest > ( iContext.iMessage );
	reqMsg.iSsdpInfo.iSearchTarget.Free ( );
	}


DEFINE_SMELEMENT ( TDestroyDataClients, NetStateMachine::MStateTransition, UPnPStateMachine::TContext )
void TDestroyDataClients::DoL ( )
	{
	//Send Destroy to data clients and remove client information
	RNodeInterface* dc;
	TClientIter<TDefaultClientMatchPolicy> iter =  iContext.iNode.GetClientIter<TDefaultClientMatchPolicy> ( TClientType ( TCFClientType::EData ) );
	TUint dcCount = 0;
	while ( ( dc = iter++ ) != NULL )
		{
		dc->PostMessage ( iContext.NodeId (), TEChild::TDestroy ().CRef () );
		++dcCount;
		}
	
	for ( TInt i = 0; i < dcCount; i++ )
		{
		RNodeInterface* dc =  iContext.iNode.GetFirstClient<TDefaultClientMatchPolicy> ( TClientType ( TCFClientType::EData ) );
    	iContext.Node ( ).RemoveClient ( dc->RecipientId( ) );
		}
	}


DEFINE_SMELEMENT ( TStopDataClients, NetStateMachine::MStateTransition, UPnPStateMachine::TContext )
void TStopDataClients::DoL ( )
	{
	TClientIter<TDefaultClientMatchPolicy> iter = iContext.iNode.GetClientIter<TDefaultClientMatchPolicy> ( TClientType ( TCFClientType::EData ) );
	RNodeInterface* dc;
	while ( ( dc = iter++ ) != NULL )
		{
		if ( dc->Flags( ) != TClientType::ELeaving )
			{
			dc->PostMessage ( TNodeCtxId ( iContext.ActivityId (), iContext.NodeId () ), TCFDataClient::TStop ( KErrNone ).CRef () );
			}
		}	
	iContext.iNodeActivity->SetPostedTo ( TNodeId::NullId ( ) );
	}
	
DEFINE_SMELEMENT ( TAwaitingToStopDataClients, NetStateMachine::MState, UPnPStateMachine::TContext )
TBool TAwaitingToStopDataClients::Accept ( )
	{
	if ( iContext.iMessage.IsMessage<TCFDataClient::TStopped>() )
		{
		RClientInterface::OpenPostMessageClose ( iContext.NodeId (), iContext.iSender, TEChild::TDestroy ().CRef () );
		iContext.iNode.RemoveClient ( address_cast<TNodeId> ( iContext.iSender ) );
		if ( iContext.iNode.CountClients<TDefaultClientMatchPolicy> ( TClientType ( TCFClientType::EData ) ) == 0 )
			{
			//Last one - accept
			return ETrue;
			}
		return EFalse;
		}
   	return EFalse;
	}


DEFINE_SMELEMENT ( TRequestOrIgnoreTag, NetStateMachine::MStateFork, UPnPStateMachine::TContext )
TInt TRequestOrIgnoreTag::TransitionTag ( )
	{
	TUpnpMessage::TUPnPRequestInfo& requestInfoMsg = message_cast<TUpnpMessage::TUPnPRequestInfo> ( iContext.iMessage );
	CTransaction* transaction = static_cast<CTransaction*> ( requestInfoMsg.iPtr );
	
	CRequest* request = transaction->Request( );
	RRequest rRequest = request->Handle( );
	const RStringF& header =  rRequest.Method ( );
	
	if ( header == request->StringPool ( ).StringF ( UPnP::ENotify, TUPnPTable::Table() ) )
		{		
		CUPnPControlSubConnectionProvider& node = static_cast<CUPnPControlSubConnectionProvider&> ( iContext.Node ( ) );						
		if ( node.IsClosing ( ) )
			{
			return CoreStates::KIgnore;
			}
		
		return KNotifyRequest;
		}
	else
		{
		CUPnPServiceSubConnectionProvider& node1 = static_cast<CUPnPServiceSubConnectionProvider&> ( iContext.Node ( ) );
		if ( node1.IsClosing ( ) )
			{
			return CoreStates::KIgnore;
			}
		else
			{
			if ( header == request->StringPool ( ).StringF ( HTTP::EGET, THTTPTable::Table() ) )
				{
				return KDescribeRequest;	
				}
			else if ( header == request->StringPool ( ).StringF ( UPnP::ESubscribe, TUPnPTable::Table() ) )
				{
				return KSubscribeRequest;
				}
			else if ( header == request->StringPool ( ).StringF ( UPnP::EUnsubscribe, TUPnPTable::Table() ) )
				{
				return KUnsubscribeRequest;
				}
			}
		}
	return MeshMachine::KNoTag;	
	}
	
TInt CUPnPUtils::ExtractVersionNumber ( const TDesC8& aSearchTarget, TInt& aUriIndex )
	{	
	TLex8 parser ( aSearchTarget );
	
	TChar ch = ':';
	// skip 4 KColon's so that u reach version number
	for ( TInt i = 0; i < 4; i++ )
		{			
		while ( !parser.Eos ( ) && parser.Peek ( ) != ch )
			parser.Inc ( );		
		parser.Inc ( );
		}		
	parser.Mark();
		
	while ( !parser.Eos ( ) && parser.Peek ( ) != ch )
		parser.Inc ( );
	
	aUriIndex = parser.MarkedOffset(); 
	
	TPtrC8 version = parser.MarkedToken ( );
	
	TInt versionNumber = 0;
	InetProtTextUtils::ConvertDescriptorToInt ( version, versionNumber );
	return versionNumber;
	}

void CUPnPUtils::CleanupMBufChain ( TAny* aAny )
	{
	RMemChunk* chain = ( RMemChunk* ) aAny;
	chain->Free( );
	}	

void CUPnPUtils::FindAndSendIconDataL ( CTransaction* aTransaction, TInt& aStatus, RMemoryAllocator& aAllocator  )
	{
	HBufC* uriPath =  aTransaction->Request()->URI()->Uri().GetFileNameL ( EUriFileNamePath);
	TPtr ptr ( uriPath->Des () );
	TInt fileNameStart = ptr.LocateReverse ( TChar ('\\') ) + 1;
	TInt fileNameLength = ptr.Length () - fileNameStart;
	TPtrC filePtr = ptr.Mid ( fileNameStart, fileNameLength );
	
	RFs fs;
	User::LeaveIfError ( fs.Connect () );
    CleanupClosePushL ( fs );
    
	_LIT ( KIconResFileNamePath, "\\resource\\upnp\\icons\\" );
	TFindFile fileFinder ( fs );
	
	CDir* dir= NULL;	
	TInt ret = fileFinder.FindWildByDir ( filePtr, KIconResFileNamePath, dir );
	if ( ret == KErrNone )
		{
		CleanupStack::PushL ( dir );
				
		RFile fileHandle;
		fileHandle.Open ( fs, fileFinder.File (), EFileShareReadersOnly );
		CleanupClosePushL ( fileHandle );
		TInt fileSize = 0;
		fileHandle.Size ( fileSize );

		HBufC8* buf = HBufC8::NewLC ( fileSize );
		TPtr8 fileData = buf->Des ();
		
		User::LeaveIfError ( fileHandle.Read ( fileData, fileSize ) );
		
		RMemChunk dataChian;
		dataChian.CreateL ( fileData, aAllocator );
		aTransaction->AddBodyPart ( dataChian );
		aStatus = HTTPStatus::EOk;
		
		CleanupStack::PopAndDestroy ( 3 ); // buf, fileHandle , dir
		}
	else
		{
		aStatus = HTTPStatus::ENotFound;
		}
	
	CleanupStack::PopAndDestroy ( &fs );
	}


