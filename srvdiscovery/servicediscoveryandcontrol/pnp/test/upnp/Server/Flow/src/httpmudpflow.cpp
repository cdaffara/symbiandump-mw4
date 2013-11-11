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
// @internalComponent
// 
//

//System Includes
#include <comms-infras/ss_nodeinterfaces.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <ss_glob.h>

//Local Includes
#include "httpmudpflow.h"
#include "upnppint.h"
#include "upnpctrlscprstates.h"
#include "upnpstatemachine.h"
#include "upnpserverconstants.h"
#include "upnplog.h"
#include "upnpmemoryutils.h"

#include <http/thttptable.h>
#include <http/framework/cheadercodecplugin.h>
#include <httpstringconstants.h>
#include <upnp/tupnptable.h>
#include <e32math.h>

const TUint KDeftMaxAgeVal = 1800;

__FLOG_STMT(_LIT8(KComponent,"Flow");)
CHttpMUdpFlow* CHttpMUdpFlow::NewL ( CSubConnectionFlowFactoryBase& aFactory,
						  			CUPnPProtocolIntfBase* aProtocolIntf,
					  				const TNodeId& aSubConnId )
	{
	CHttpMUdpFlow* self = new ( ELeave )CHttpMUdpFlow ( aFactory, aProtocolIntf, aSubConnId );
	CleanupStack::PushL ( self );
	self->ConstructL ();
	CleanupStack::Pop (); // self
	return self;
	}

CHttpMUdpFlow::CHttpMUdpFlow ( CSubConnectionFlowFactoryBase& aFactory,
					CUPnPProtocolIntfBase* aProtocolIntf,
					const TNodeId& aSubConnId )
	: CUPnPFlowBase ( aFactory, aProtocolIntf, EHttpMUdpFlow, aSubConnId ), iIsStarted ( EFalse ), iIsLeaving ( EFalse )
	{
	LOG_NODE_CREATE(KESockFlowTag, CHttpMUdpFlow);
	}

CHttpMUdpFlow::~CHttpMUdpFlow ()
	{
	delete iRequestParser;
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("Destroying iReceivedData")));
	if ( !iReceivedData.IsEmpty () )
		iReceivedData.Free ();
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("Destroying iUPnPRequest")));
	delete iUPnPRequest;
	iRecvElements.Reset ();
	//delete iCodec;

	__ASSERT_DEBUG ( iSearchTargetArray.Count () == 0, User::Invariant () );
	iSearchTargetArray.Close ();
	__ASSERT_DEBUG ( iNotifyTargetArray.Count () == 0, User::Invariant () );
	iNotifyTargetArray.Close ();
	iListenRequestors.Reset ();

	iStringPool.Close();
	
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("Destroying iSubConnectionProvider")));
	iSubConnectionProvider.Close ();
	__ASSERT_DEBUG ( iTimerMappedArray.Count () == 0, User::Invariant () );
	iTimerMappedArray.ResetAndDestroy();
	__ASSERT_DEBUG ( iCallbackArray.Count () == 0, User::Invariant () );
	iCallbackArray.ResetAndDestroy();
	if ( iDeltaTimer )
		iDeltaTimer->Cancel();	
	delete iDeltaTimer;
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("Destroyed CHttpMUdpFlow")));
	LOG_NODE_DESTROY(KESockFlowTag, CHttpMUdpFlow);
	}


void CHttpMUdpFlow::ConstructL ()
	{
	TAppProtIntfMessage::RegisterL (); // Register the message with COMMS transport
	iStringPool.OpenL(TUPnPTable::Table());
	iStringPool.OpenL(THTTPTable::Table());
	iCodec = CHeaderCodecPlugin::NewL ( KUPnPCodecName, iStringPool );
	iRequestParser = CUpnpRequestParser::NewL ( *this );
	iUPnPRequest = CRequest::NewL ( *iCodec, iStringPool );
	// Create a delta timer with the given priority
	iDeltaTimer=CDeltaTimer::NewL(EPriorityLow);
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("Created CHttpMUdpFlow")));
	}

void CHttpMUdpFlow::ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ReceivedL")));

	if ( aMessage.IsMessage<TEChild::TDestroy> () )
		{
    	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ReceivedL - TCFMessage::TDestroy")));
    	if ( RemoveControlClient ( FindControlClient ( address_cast<TNodeId> ( aSender ) ) ) == 0 )
    		{
    		LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ReceivedL - TCFMessage::TDestroy - Deleting current flow")));
   			DeleteThisFlow ();
    		}
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStart> () )
		{
        LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ReceivedL - TCFMessage::TDataClientStart")));
        StartFlow ( address_cast<TNodeCtxId> ( aSender ) );
    	}
    else if ( aMessage.IsMessage<TCFDataClient::TStop> () )
		{
		LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ReceivedL - TUpnpMessage::TUPnPStopSearch")));

		Unregister ( address_cast<TNodeCtxId> ( aSender ) );
        StopFlow ( address_cast<TNodeCtxId> ( aSender ) );
        }
	else if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ReceivedL - TCFMessage::TFOCreated")));
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iAppProtIntfId = msg.iNodeId;
		
		TAppProtAddr mCastAddr( KSsdpMulticastAddr, KUpnpMCPort );
		RClientInterface::OpenPostMessageClose ( TNodeCtxId ( MeshMachine::KActivityNull, NodeId () ), iAppProtIntfId, TAppProtIntfMessage::TJoinWithMulticastAddress ( mCastAddr ).CRef () );
		}
	else if ( TAppProtIntfMessage::ERealmId == aMessage.MessageId ().Realm () )
		{
        switch ( aMessage.MessageId ().MessageId () )
	        {
			case TAppProtIntfMessage::TJoinComplete::EId:
				{
				LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ReceivedL - TAppProtIntfMessage::TJoinComplete")));
				iIsStarted = ETrue;
				for ( TInt i = 0; i < iListenRequestors.Count (); i++ )
					{
					RClientInterface::OpenPostMessageClose ( NodeId (), iListenRequestors[i], TCFDataClient::TStarted ().CRef () );
					}

				iListenRequestors.Reset ();
				}
			break;

			case TAppProtIntfMessage::TLeaveComplete::EId:
				{
				LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ReceivedL - TAppProtIntfMessage::TLeaveComplete")));
				// post destroy to service provider
				RClientInterface::OpenPostMessageClose ( NodeId (), iAppProtIntfId, TAppProtIntfMessage::TDestroy ().CRef () );
				// post dataclient stopped to controlprovider
				PostDataClientStopped ( iLastLeavingClient );
				}
			break;

	        case TAppProtIntfMessage::TDataReceived::EId:
	        	{
	        	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ReceivedL - TAppProtIntfMessage::TDataReceived")));
	        	TAppProtIntfMessage::TDataReceived& msg = message_cast <TAppProtIntfMessage::TDataReceived> ( aMessage );
	        	
	        	if ( !iIsLeaving )
		        	{
		        	RMemoryAllocator allocator(static_cast<CUPnPProtocolIntfBase *>( ProtocolIntf () )->GetMemoryChunkManager());
		        	RMemChunk memChunk;
		        	TUPnPMemoryUtils::CreateMemChunk(memChunk, msg.iData, allocator);

					CRecvElement* element = new ( ELeave ) CRecvElement ( memChunk, msg.iAddr );
					CleanupStack::PushL ( element );
					iRecvElements.AppendL ( element );
					CleanupStack::Pop ( element );
					ParseIfOneRecvElement ();
		        	}
	        	msg.iData.Free ();
	        	}
	        break;

	        case TAppProtIntfMessage::TError::EId:
	        	{
	        	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ReceivedL - TAppProtIntfMessage::TError")));
	        	// Note! Nothing to do if already leaving state,
				// else need to send error to control clients and do listener cleanup
	        	}
	        break;

	        default:
		    break;
	        }
		}
	else if ( TUpnpMessage::ERealmId == aMessage.MessageId ().Realm () )
		{
		switch ( aMessage.MessageId ().MessageId () )
			{
			case TUpnpMessage::TUPnPNotifyRegistration::EId:
				{
				LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ReceivedL - TUpnpMessage::TUPnPNotifyRegistration")));
				RegisterL ( address_cast<TNodeCtxId> ( aSender ),
							static_cast < TUpnpMessage::TUPnPNotifyRegistration& > ( aMessage ).iSsdpInfo.iSearchTarget,
							iNotifyTargetArray );
				}
			break;

			case TUpnpMessage::TUPnPPublishRegistration::EId:
				{
				LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ReceivedL - TUpnpMessage::TUPnPPublishRegistration")));
				RegisterL ( address_cast<TNodeCtxId> ( aSender ),
							static_cast < TUpnpMessage::TUPnPPublishRegistration& > ( aMessage ).iSsdpInfo.iSearchTarget,
							iSearchTargetArray );
				}
			break;

 			case TUpnpMessage::TCancelRequest::EId:
 				{
            	Unregister ( address_cast<TNodeCtxId> ( aSender ) );
				RClientInterface::OpenPostMessageClose ( NodeId (), address_cast<TNodeCtxId> ( aSender ), TUpnpMessage::TCancelled ().CRef () );
 				}
            break;
			}
		}
	}
	

void CHttpMUdpFlow::StartFlow ( TNodeCtxId aSender )
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("HttpMUdpFlow::StartFlow")));
	if ( iIsStarted )
		{
		RClientInterface::OpenPostMessageClose ( NodeId (), aSender, TCFDataClient::TStarted ().CRef () );
		return;
		}

	if ( !iIsStarted && iListenRequestors.Count () == 0 )
		{
		iListenRequestors.Append ( aSender );
		CreateMulticastListener  ();
		iIsLeaving = EFalse;
		}

	}

void CHttpMUdpFlow::StopFlow ( TNodeCtxId aSender )
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::StopFlow")));

	SetClientLeaving ( aSender ); // set the control client leaving

	if ( iIsStarted && CheckIfAllClientsLeaving ( ) )
		{
		MarkMeForDeletion ();
		iLastLeavingClient = aSender;
		iIsLeaving = ETrue;
		iIsStarted = EFalse;

		LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::StopFlow - Posting message TAppProtIntfMessage::TLeave")));
		// leaving service provider
		RClientInterface::OpenPostMessageClose ( TNodeCtxId ( MeshMachine::KActivityNull, NodeId () ), iAppProtIntfId, TAppProtIntfMessage::TClientLeavingRequest ().CRef () );
		}
	else
		{
		PostDataClientStopped ( aSender );
		}
	}

void CHttpMUdpFlow::ParseIfOneRecvElement ()
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ParseIfOneRecvElement")));
	if ( iRecvElements.Count () == 1 )
		{
		ParseElement ();
		}
	}

void CHttpMUdpFlow::ParseElement ()
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ParseElement")));
	if ( iRecvElements.Count () > 0 )
		{
		LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ParseElement - Parsing request")));
		iRequestParser->ParseRequest ( iRecvElements[0]->Data (), iUPnPRequest );
		}
	}

void CHttpMUdpFlow::RemoveFirstRecvElement ()
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::RemoveFirstRecvElement")));
	ASSERT ( iRecvElements.Count () > 0 );
	CRecvElement* element = iRecvElements[0];
	iRecvElements.Remove (0);
	delete element;
	}

// From MParserObserver
void CHttpMUdpFlow::GotHeaders ()
	{
	if ( ValidateRequest () != KErrNone )
		{
		DataParsed ();
		}

	// Else we will notify to the SCPR(s) from ParsingComplete.
	}

void CHttpMUdpFlow::DataParsed()
	{
	// Bad data. This is UDP. we should get everything or none
	iRequestParser->DataFailed ();
	iRequestParser->ResetParser ();
	RemoveFirstRecvElement ();
	ParseElement ();
	}

void CHttpMUdpFlow::ParsingComplete ( RMemChunk& /*aExcessData*/ )
	{
	NotifyClients ();
	DataParsed ();
	iUPnPRequest->Reset();
	}

void CHttpMUdpFlow::ParserError ( TInt /*aError*/ )
	{
	DataParsed (); // Bad data.
	}


void CHttpMUdpFlow::NotifyClients ()
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::NotifyClients")));
	TInt method = iUPnPRequest->Handle ().Method ().Index ( TUPnPTable::Table() );
	if ( method == UPnP::ENotify )	//control point end listening for notify packets
		{
		LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::NotifyClients - Handling NOTIFY request")));
		TRAP_IGNORE ( HandleNotifyRequestL () );
		}
	else //if ( method == UPnP::EMSearch ) //service point end listening for m-search packets
		{
		LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::NotifyClients - Handling MSEARCH request")));
		// CreateTimerEntry sets iEntry to the new timer entry and queues it in the deltatimer
		TRAP_IGNORE ( CreateTimerEntryL() );
		}
	
	return;
	}

void CHttpMUdpFlow::ReadRequestValues ( RStringF& aLocation, TInt& aMaxAge )
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ReadRequestValues - ST, USN, Location values")));
	THTTPHdrVal value;
	GetHeaderValue ( *iUPnPRequest, HTTP::ELocation, value , THTTPTable::Table());
	aLocation = value.StrF ();

	value.SetInt(KDeftMaxAgeVal);
	GetParamValue ( *iUPnPRequest, HTTP::ECacheControl, HTTP::EMaxAge, value, THTTPTable::Table() );
	aMaxAge = value.Int();
	}

void CHttpMUdpFlow::ReadRequestValues ( RStringF& aSt, RStringF& aUsn, RStringF& aNts)
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ReadRequestValues - ST, USN values")));
	THTTPHdrVal value = RStringF();
	GetHeaderValue ( *iUPnPRequest, UPnP::EUSN, value, TUPnPTable::Table() );
	aUsn = value.StrF ();

	GetHeaderValue ( *iUPnPRequest, UPnP::ENT, value, TUPnPTable::Table() );
	aSt = value.StrF ();

	GetHeaderValue ( *iUPnPRequest, UPnP::ENTS, value, TUPnPTable::Table() );
	aNts = value.StrF ();
	}

void CHttpMUdpFlow::ReadRequestValues ( TInt& aMaxAge, RStringF& aSt )
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ReadRequestValues - Max-Age, NT values")));
	THTTPHdrVal value;
	GetHeaderValue ( *iUPnPRequest, UPnP::EMX, value, TUPnPTable::Table() );
	aMaxAge = value.Int();

	GetHeaderValue ( *iUPnPRequest, UPnP::EST, value, TUPnPTable::Table() );
	aSt = value.StrF ();
	}


void CHttpMUdpFlow::HandleNotifyRequestL ()
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::HandleNotifyRequest")));
	RStringF usn;
	RStringF st;
	RStringF nts;
	RStringF location;
	TInt maxAge = 0;
	THTTPHdrVal value;
	ReadRequestValues ( st, usn, nts );
	for ( TInt i = 0; i < iNotifyTargetArray.Count (); ++i )
		{
		const CSearchTarget& target = *(iNotifyTargetArray[i]);

		if ( target.Match ( st.DesC() ) != KErrNotFound &&
		 							!IsClientLeaving ( target.Originator () ) )
			{
			// We have a match. Post the information to our control client
			RMemoryAllocator allocator(static_cast<CUPnPProtocolIntfBase *>( ProtocolIntf () )->GetMemoryChunkManager());
			
			RMemChunk usnBuf;
			RMemChunk stBuf;
			usnBuf.CreateL ( usn.DesC (), allocator );
			TCleanupItem item ( &UPnPStateMachine::CUPnPUtils::CleanupMBufChain, &usnBuf );
			CleanupStack::PushL ( item );
			stBuf.CreateL ( st.DesC (), allocator );
			TCleanupItem item2 ( &UPnPStateMachine::CUPnPUtils::CleanupMBufChain, &stBuf );
			CleanupStack::PushL ( item2 );
			if ( nts.DesC().CompareF ( KSsdpAlive ) == 0 )
				{
				LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::HandleNotifyRequest - Posting message TUpnpMessage::TUPnPPublishAliveRequest")));
				ReadRequestValues ( location, maxAge );
				RMemChunk locationBuf;
				locationBuf.CreateL ( location.DesC (), allocator );
				TCleanupItem item3 ( &UPnPStateMachine::CUPnPUtils::CleanupMBufChain, &locationBuf );				
				CleanupStack::PushL ( item3 );
				
				TSsdpInfo info ( maxAge, locationBuf, usnBuf, stBuf );
				RClientInterface::OpenPostMessageClose ( NodeId (), target.Originator (), TUpnpMessage::TUPnPPublishAliveRequest ( info ).CRef () );
				
				CleanupStack::Pop (); // item3
				}
			else
				{
				LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::HandleNotifyRequest - Posting message TUpnpMessage::TUPnPPublishByeRequest")));
				TSsdpInfo info ( usnBuf, stBuf );
				RClientInterface::OpenPostMessageClose ( NodeId (), target.Originator (), TUpnpMessage::TUPnPPublishByeRequest ( info ).CRef () );
				}
			CleanupStack::Pop (); // item2
			CleanupStack::Pop (); // item3
			}

		}
	}

TInt CHttpMUdpFlow::HandleMSearchRequestL ( TAny* aPtr )
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::HandleMSearchRequest")));	
	CCallbackArgument* pointerData= static_cast<CCallbackArgument*> (aPtr);
	CHttpMUdpFlow* thisObject= static_cast < CHttpMUdpFlow* > (pointerData->GetThisPointer());
	RPointerArray<CTimerMappedUri>& rArray=thisObject->GetTimerArray();
	RPointerArray<CCallbackArgument>& argArray = thisObject->GetArgumentArray();
	CTimerMappedUri* timerUri = NULL;
	for(TInt index=0; index < rArray.Count();index++)
		{
		if(rArray[index]->MatchTimerEntry( pointerData->GetTimerEntry() ))
			{
			// Removes the uri entry from the array
			timerUri = rArray[index];
			rArray.Remove(index);
			break;
			}
		}
	TInt index = argArray.Find( pointerData );
	__ASSERT_DEBUG(index >= 0 , User::Invariant());
	argArray.Remove( index );
	const CSearchTarget& target = *(pointerData->SearchTarget());
	// We have a match. Post the information to our control client
	if ( !(thisObject->IsClientLeaving ( target.Originator () ) ))
		{
		RMemoryAllocator allocator(static_cast<CUPnPProtocolIntfBase *>( thisObject->ProtocolIntf () )->GetMemoryChunkManager());
		RMemChunk stBuf;
		if ( timerUri->GetUri().CompareF(KSsdpAll) == 0 )
			{
			stBuf.CreateL ( target.SearchTarget(), allocator );
			}
		else
			{
			stBuf.CreateL ( timerUri->GetUri(), allocator );			
			}

		TSsdpInfo info ( stBuf, pointerData->GetAddress()  );
		RClientInterface::OpenPostMessageClose ( thisObject->NodeId (), target.Originator (), TUpnpMessage::TUPnPSearchRequest ( info ).CRef () );	
		}
	
	delete timerUri;
	delete pointerData;
	return KErrNone;
	}

void CHttpMUdpFlow::RegisterL ( TNodeCtxId aChannelId, RMemChunk& aST, CSearchTargetArray& aArray )
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::RegisterL")));
	CSearchTarget* searchTarget = new ( ELeave ) CSearchTarget ( aChannelId );
	CleanupStack::PushL ( searchTarget );
	searchTarget->AddTargetL ( aST );
	aArray.AppendL ( searchTarget );
	CleanupStack::Pop (); // searchTarget
	}

void CHttpMUdpFlow::Unregister ( TNodeCtxId aSender )
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::Unregister")));
	
	TBool isPublisher = EFalse;
	TInt i = iSearchTargetArray.Count () - 1;
	while ( i >= 0)
		{	
		if ( aSender.Ptr () == iSearchTargetArray[i]->Originator ().Ptr () )
					  			// for Published SCPR there is no activity running,
								// For Embedded and Root Device SCPR, there will be
								// multiple Entries in SearchTargetArray, which will be removed with
								// one TDataClientStop Message
			{
			LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::Unregister - target found in search array")));
			CSearchTarget *target = iSearchTargetArray[i];
			iSearchTargetArray.Remove(i);
			CCallbackArgument* callArgument;
			TInt index = iCallbackArray.Count() - 1;
			while ( index >= 0 )
				{
				if (target == iCallbackArray[index]->SearchTarget())
					{
					callArgument  = iCallbackArray[index];
					iCallbackArray.Remove( index );
					for ( TInt j = 0; j < iTimerMappedArray.Count (); j++ )
						{
						if ( iTimerMappedArray[j]->MatchTimerEntry( callArgument->GetTimerEntry() ) )
							{
							// Removes the uri entry from the array
							CTimerMappedUri* timerUri = iTimerMappedArray[j];
							iDeltaTimer->Remove(timerUri->GetTimerEntry());
							iTimerMappedArray.Remove(j);
							delete timerUri;
							break;
							}
						}
					delete callArgument;	
					}
				--index;
				}
				
			delete target;
			isPublisher = ETrue;
			}
		
		--i;
		}
	if ( isPublisher )
		{
		return;
		}
		
	for ( TInt i = 0; i < iNotifyTargetArray.Count (); ++i )
		{
		if ( aSender == iNotifyTargetArray[i]->Originator () )	// for Register Notify if the activity is not running,
															// there will not be any Entry in NotifyTargetArray
			{
			LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::Unregister - target found in notify array")));
			CSearchTarget *target = iNotifyTargetArray[i];
			iNotifyTargetArray.Remove(i);
			delete target;
			return;
			}
		}	
	}

void CHttpMUdpFlow::CreateMulticastListener ()
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::CreateMulticastListener")));
	const TInt KAppProtIntfFactoryUid = 0x2000D05B; // Put in common place
	//const TInt KGenericContainerIndex = 1;
	TAppProtIntfQuery query ( KProtocolInetUdp, KUpnpMCPort );
		
	TCFFactory::TFindOrCreatePeer foMsg ( TCFPlayerRole::EDataPlane, TUid::Uid( KAppProtIntfFactoryUid ), &query );
   // foMsg.PostTo ( SockManGlobals::Get()->GetPlaneFC ( EDataPlane, KGenericContainerIndex ) );
    TNodeId containerId = static_cast<CUPnPProtocolIntfBase*>(ProtocolIntf())->GetAppProtIntfContainerId();
    if ( containerId != TNodeId::NullId ( ) )
    	{
		RClientInterface::OpenPostMessageClose ( NodeId (), containerId, foMsg );
    	}
	}

TInt CHttpMUdpFlow::ValidateRequest ()
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ValidateRequest")));
	TInt err = KErrNone;
	TInt method = iUPnPRequest->Handle ().Method ().Index ( TUPnPTable::Table() );

	switch ( method )
		{
		case UPnP::ENotify:
			{
			LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ValidateRequest - Validating NOTIFY request")));
			err = ValidateNotifyRequest ();
			}
		break;

		case UPnP::EMSearch:
			{
			LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ValidateRequest - Validating MSEARCH request")));
			err = ValidateMSearchRequest ();
			}
		break;

		default:
		err = KErrCorrupt;
		break;
		}

	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ValidateRequest - Validating returned with error code = %d"), err));
	return err;
	}

TInt CHttpMUdpFlow::ValidateNotifyRequest ()
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ValidateNotifyRequest")));
	
	if ( IsHeaderPresent ( *iUPnPRequest, UPnP::ENTS, TUPnPTable::Table() ) != KErrNone )
		return KErrCorrupt;
	
	THTTPHdrVal value;
	GetHeaderValue ( *iUPnPRequest, UPnP::ENTS, value, TUPnPTable::Table() );
	if(value.StrF ().DesC().CompareF ( KSsdpBye )!= 0)
		{
		if ( IsValidCacheControlHeader ( *iUPnPRequest ) != KErrNone )
		return KErrCorrupt;

		if ( IsHeaderPresent ( *iUPnPRequest, HTTP::ELocation, THTTPTable::Table() ) != KErrNone )
		return KErrCorrupt;
		}

	if ( IsHeaderPresent ( *iUPnPRequest, UPnP::ENT, TUPnPTable::Table() ) != KErrNone )
		return KErrCorrupt;

	return KErrNone;
	}

TInt CHttpMUdpFlow::ValidateMSearchRequest ()
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::ValidateMSearchRequest")));
	if ( IsValidHostHeader ( *iUPnPRequest ) && IsValidManHeader ( *iUPnPRequest ) &&
		IsHeaderPresent ( *iUPnPRequest, UPnP::EMX, TUPnPTable::Table() ) == KErrNone && IsHeaderPresent ( *iUPnPRequest, UPnP::EST, TUPnPTable::Table() )  == KErrNone )
		return KErrNone;
	return KErrCorrupt;
	}

TInt CHttpMUdpFlow::GetHeaderValue ( const CRequest& aRequest, TInt aFieldIndex, THTTPHdrVal& aFieldVal, const TStringTable& aTable ) const
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::GetHeaderValue")));
	CRequest& request = const_cast < CRequest& > ( aRequest  );
	RRequest req = request.Handle ();
	RHTTPHeaders headers = req.GetHeaderCollection ();
	RStringF fieldStr = aRequest.StringPool ().StringF ( aFieldIndex, aTable );
	return headers.GetField ( fieldStr, 0, aFieldVal );
	}

TInt CHttpMUdpFlow::GetParamValue ( const CRequest& aRequest , TInt aFieldIndex, TInt aParamIndex, THTTPHdrVal& aParamVal, const TStringTable& aTable ) const
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::GetParamValue")));
	CRequest& request = const_cast < CRequest& > ( aRequest  );
	RRequest req = request.Handle ();
	RHTTPHeaders headers = req.GetHeaderCollection ();
	RStringF fieldStr = aRequest.StringPool ().StringF ( aFieldIndex, aTable );
	RStringF paramStr = aRequest.StringPool ().StringF ( aParamIndex, aTable );
	TInt err = KErrNone;
	TInt fieldCount = 0;
	TRAP ( err, fieldCount = headers.FieldPartsL(fieldStr) );
	while (fieldCount>0)
		{
		err = headers.GetParam ( fieldStr, paramStr, aParamVal, fieldCount-1 );
		if(err == KErrNone )
			break;
		fieldCount--;
		}
	return err;
	}

TBool CHttpMUdpFlow::IsHeaderPresent ( const CRequest& aRequest , TInt aFieldIndex, const TStringTable& aTable ) const
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::IsHeaderPresent")));
	THTTPHdrVal value;
	return GetHeaderValue ( aRequest, aFieldIndex, value, aTable );
	}

TBool CHttpMUdpFlow::IsValidCacheControlHeader ( const CRequest& aRequest ) const
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::IsValidCacheControlHeader")));
	THTTPHdrVal value;
	return GetParamValue ( aRequest, HTTP::ECacheControl, HTTP::EMaxAge, value, THTTPTable::Table());
	}

TBool CHttpMUdpFlow::IsValidManHeader ( const CRequest& aRequest ) const
	{
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::IsValidManHeader")));
	THTTPHdrVal value;
	TInt err = GetHeaderValue ( aRequest, UPnP::EMAN, value, TUPnPTable::Table() );
	if ( ( err == KErrNone ) && ( value.StrF ().DesC ().Compare ( KSsdpDiscover ) == 0))
		{
		LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::IsValidManHeader - MAN header has a valid value")));
		return ETrue;
		}
	LOG(ESockLogExternal::Printf(KSubsysMUDPFlow, KComponent, _L8("CHttpMUdpFlow::IsValidManHeader - MAN header does not have a valid value")));
	return EFalse;
	}

TBool CHttpMUdpFlow::IsValidHostHeader ( CRequest& aRequest ) const
	{
	// From UPnP Device Architecture 1.0
	// Must be 239.255.255.250:1900. If the port number(:1900) is omitted,the receiver
	// should assume the default SSDP port number of 1900.
	_LIT8 ( KSsdpHost, "239.255.255.250" );
	
	THTTPHdrVal value;
	TInt err = GetHeaderValue ( aRequest, HTTP::EHost, value, THTTPTable::Table () );
	if ( err != KErrNone )
		{
		return EFalse;
		}
	
	const TDesC8& host = value.StrF ().DesC ();
	TInt pos = host.Locate ( TChar (':') );
	
	if ( pos != KErrNotFound )
		{
		TPtrC8 ptrHost = host.Mid ( 0, pos );			
		if ( ptrHost.Compare ( KSsdpHost ) != 0 )
			{
			return EFalse;
			}
		
		TPtrC8 ptrPort = host.Mid ( pos + 1 );
		_LIT8 ( KSsdpPort, "1900" );
		
		if ( ptrPort.Length () > 0 && ptrPort.Compare ( KSsdpPort ) != 0 )
			{
			return EFalse;
			}
		
		return ETrue;
		}
	
	if ( host.Compare ( KSsdpHost ) != 0 )
		{
		return EFalse;
		}
		
	return ETrue;
	}
/* CreateTimerEntry : creates the deltatimer entry and sets it to iEntry
 @param aUri  The uri whose timer entry has to be created
 */
void CHttpMUdpFlow::CreateTimerEntryL()
	{
	const TInt KMXValue = 120;
	TInt maxAge = 0;
	RStringF location;
	ReadRequestValues ( maxAge, location );
	if ( maxAge > KMXValue )
		maxAge = KMXValue;
	const TDesC8& uri = location.DesC();
	for ( TInt i = 0; i < iSearchTargetArray.Count (); ++i )
		{
		const CSearchTarget& target = *(iSearchTargetArray[i]);
		if ( target.Match ( uri ) != KErrNotFound )
			{
			CCallbackArgument* callArgument =CCallbackArgument::NewL(this, iRecvElements[0]->Address () , iSearchTargetArray[i]);
			CleanupStack::PushL(callArgument);
			TCallBack callBack(HandleMSearchRequestL,callArgument);
			TDeltaTimerEntry* entry = new (ELeave) TDeltaTimerEntry ( callBack );
			CTimerMappedUri* timerMappedUri = CTimerMappedUri::NewL(location.DesC(),entry,maxAge);
			iTimerMappedArray.AppendL(timerMappedUri);
			callArgument->SetTimerEntry( entry );
			iCallbackArray.AppendL(callArgument);
			CleanupStack::Pop();
			// queue up in deltatimer
			TInt time = AdjustedDelay(maxAge);
			TTimeIntervalMicroSeconds32 delay(time);
			iDeltaTimer->Queue(delay,*entry);
			}	
		}
	}
/* Used to calculate the time in microseconds to set the timer entry.Also a little
 of time is used extra to account for Intra process communication
 @param aTime	The time for which the timer has to be set in seconds
 @return The actual time in microseconds
 */
TInt CHttpMUdpFlow::AdjustedDelay( TInt aTime )
	{
	TInt newNum;
 	TTime now;
	now.UniversalTime();
	TInt64 randSeed = now.Int64();
	newNum = static_cast<TInt>(Math::Rand(randSeed)%aTime);
  	const TUint KIntTime = 1000000 ;
	newNum =  newNum * KIntTime ;
	return newNum;
	}
CCallbackArgument* CCallbackArgument::NewL(TAny* aThis ,const TAppProtAddr& aAddress , const CSearchTarget* aStTarget)
	{
	CCallbackArgument* callArgument = new (ELeave )CCallbackArgument(aThis, aAddress , aStTarget);
	return callArgument;
	}

