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

#include <ss_glob.h>
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <elements/nm_interfaces.h>

#include <httpstringconstants.h>
#include <httperr.h>
#include <http/thttptable.h>
#include <http/framework/cheadercodecplugin.h>
#include <upnp/tupnptable.h>
#include <uriutils.h>

#include "app_protintf_msgs.h"
#include "httpserver.h"
#include "ctransaction.h"
#include "httpserverhandler.h"
#include "httpevent.h"
#include "upnpserverconstants.h"
#include "upnplog.h"
#include "upnppint.h"
#include "upnpmemoryutils.h"

__FLOG_STMT(_LIT8(KComponent,"Flow");)

using namespace Messages;
using namespace ESock;

CServiceInfo* CServiceInfo::NewL ( const TDesC8& aUri, MHttpEventObserver& aObserver )
	{
	CServiceInfo* self = new (ELeave) CServiceInfo ( aObserver );
	CleanupStack::PushL ( self );
	self->ConstructL ( aUri );
	CleanupStack::Pop (); // self
	return self;
	}

TBool CServiceInfo::Match ( const TDesC8& aUri )
	{
	return ( iUri.CompareF ( aUri ) == 0 );
	}

TBool CServiceInfo::Match ( const TUriC8& aUri, const TDesC8& aHost ) const
	{
	// Check host matches
	if ( aHost.Length () != 0 )
		{
		// Search if aHost has the port also.
		TInt pos = aHost.Find(_L8(":"));
		if((pos != KErrNotFound) && (UriUtils::HostType(aHost.Left(pos)) == UriUtils::EIPv4Host))
			{
			// Port number is present in aHost
			if(iParsedUri.Extract(EUriHost).Compare(aHost.Left(pos)) == 0)
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("UriHost Matched")));
				
				// Check if the port in aHost matches with the port in iParsedUri
				if(iParsedUri.Extract(EUriPort).Compare(aHost.Right(aHost.Length()-(pos+1))) !=0 )
					{
					// Port numbers did not match
					LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("UriPort Not Matched")));	
					return EFalse;	
					}
				}
			else
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("UriHost Not Matched")));	
				return EFalse;
				}
			}
			
		else if ( iParsedUri.Extract ( EUriHost ).CompareF ( aHost ) )
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("UriHost Not Matched")));
			return EFalse;
			}
		}
	else
		{
		// Check if its matches in the Uri
		if ( iParsedUri.Compare ( aUri, EUriHost ) )
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("UriHost Not Matched")));
			return EFalse;
			}
		}
	// Check path component matches
	if ( iParsedUri.Compare ( aUri, EUriPath ) )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("UriPath Not Matched")));
		return EFalse;
		}

	LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("UriPath Matched")));
	return ETrue;
	}

CServiceInfo::CServiceInfo ( MHttpEventObserver& aObserver )
: iObserver ( aObserver )
	{

	}

void CServiceInfo::ConstructL ( const TDesC8& aUri )
	{
	User::LeaveIfError ( iParsedUri.Parse ( aUri ) );
	iUri.CreateL ( aUri );
	}

CServiceInfo::~CServiceInfo ()
	{
	iUri.Close ();
	}

MHttpEventObserver& CServiceInfo::Observer () const
	{
	return iObserver;
	}

// -------------------------------------------------

CServiceInfoArray::CServiceInfoArray ()
	{
	}

CServiceInfoArray::~CServiceInfoArray ()
	{
	iServiceInfos.ResetAndDestroy ();
	}

void CServiceInfoArray::AddServiceUriL ( const TDesC8& aUri, MHttpEventObserver& aObserver )
	{
	TInt position = FindServiceUri ( aUri );
	if( position != KErrNotFound )
		{
		User::Leave ( KErrAlreadyExists );
		}

	CServiceInfo* info = CServiceInfo::NewL ( aUri, aObserver );
	CleanupStack::PushL ( info );
	iServiceInfos.AppendL ( info );
	CleanupStack::Pop (); // info
	}

void CServiceInfoArray::RemoveServiceUri ( const TDesC8& aUri )
   	{
   	TInt pos = FindServiceUri ( aUri );
   	if ( pos != KErrNotFound  )
   		{
   		CServiceInfo* info = iServiceInfos [ pos ];
   		iServiceInfos.Remove ( pos );
   		delete info;
   		}
   	}

TInt CServiceInfoArray::FindServiceUri ( const TDesC8& aUri )
	{
	for ( TInt i = 0; i < iServiceInfos.Count(); ++i )
		{
		if ( iServiceInfos[i]->Match( aUri ) )
			return i;
		}
	return KErrNotFound;
	}

TInt CServiceInfoArray::MatchServiceUri ( const TUriC8& aUri, const TDesC8& aHost )
	{
	for ( TInt i = 0; i < iServiceInfos.Count(); ++i )
		{
		if ( iServiceInfos[i]->Match( aUri, aHost ) )
			return i;
		}
	return KErrNotFound;
	}

TInt CServiceInfoArray::CountServiceUri ()
	{
	return iServiceInfos.Count();
	}

// -------------------------------------------------------------

void HttpRequestUtils::ValidateL ( const CRequest& aRequest )
	{
	User::LeaveIfError ( ValidateMethod ( aRequest ) );
	User::LeaveIfError ( ValidateHost ( aRequest ) );
	}

TInt  HttpRequestUtils::ValidateMethod ( const CRequest& aRequest )
	{
	// We validate the RFC2616 methods that we support. Currrent we support only
	// GET and POST method. Other methods are validated in the protocol flow.

	RStringF method = aRequest.Method ();
	RStringPool strP = aRequest.StringPool();
	
	RStringF connect = strP.StringF(HTTP::ECONNECT, THTTPTable::Table());
	RStringF del = strP.StringF(HTTP::EDELETE, THTTPTable::Table());
	RStringF head = strP.StringF(HTTP::EHEAD, THTTPTable::Table());
	RStringF options = strP.StringF(HTTP::EOPTIONS, THTTPTable::Table());
	RStringF put = strP.StringF(HTTP::EPUT, THTTPTable::Table());
	RStringF trace = strP.StringF(HTTP::ETRACE, THTTPTable::Table());
	
	if(aRequest.Method() == connect||aRequest.Method() == del||aRequest.Method() == head||
	   aRequest.Method() == options||aRequest.Method() == put||aRequest.Method() == trace 
	   )
		{
		return KErrNotSupported;
		}
	else
		{
		return KErrNone;
		}
	}

TInt HttpRequestUtils::ValidateHost ( const CRequest& aRequest )
	{
	if ( !aRequest.IsHTTP10 () )
		{
		// HTTP/1.1 Host header must be present.
		// Relevant information from RFC2616 Sec 14.23
		// 1. A client MUST include a Host header field in all HTTP/1.1 request messages .
		// 2. If the requested URI does not include an Internet host name for the service being requested, then the Host header
		// field MUST be given with an empty value.
		// Also from sec 19.6.1.1 - Servers MUST report a 400 (Bad Request) error if an HTTP/1.1 request does not include a Host
		// request-header.
		// For more information refer the above sections in RFC2616

		// Note: ( seee above 2 ) - RFC2616 explicitly is not saying what we should do when we are having Host header field with
		// emtpy value. Only it mandates the Host header field MUST be present if it is a HTTP/1.1 request. So we just check
		// the presence of Host header field and we will leave rest to URI matching.
		if ( !HttpRequestUtils::IsHostPresent ( aRequest ) )
			{
			return KErrCorrupt;
			}
		}
	return KErrNone;
	}

TBool HttpRequestUtils::IsHostPresent ( const CRequest& aRequest )
	{
	CRequest& req = const_cast < CRequest& > ( aRequest );
	RRequest request = req.Handle ();
	RHTTPHeaders headers = request.GetHeaderCollection ();
	RStringF hostStr = aRequest.StringPool ().StringF ( HTTP::EHost, THTTPTable::Table() );
	THTTPHdrVal hostValue;
	return headers.GetField(hostStr, 0, hostValue) == KErrNone;
	}

const TDesC8& HttpRequestUtils::HostStr ( const CRequest& aRequest )
	{
	CRequest& req = const_cast < CRequest& > ( aRequest );
	RStringF hostStr = aRequest.StringPool ().StringF ( HTTP::EHost, THTTPTable::Table() );
	RHTTPHeaders headers = req.Handle ().GetHeaderCollection ();
	THTTPHdrVal hostValue;
	if ( headers.GetField(hostStr, 0, hostValue) != KErrNotFound )
		{
		return hostValue.StrF ().DesC ();
		}
	return KNullDesC8 ();
	}

TBool HttpRequestUtils::IsConnectionRequired ( const CRequest& aRequest )
	{
	TBool bConnRequired = EFalse;
	CRequest& req = const_cast < CRequest& > ( aRequest );
	RRequest request = req.Handle ();
	RHTTPHeaders headers = request.GetHeaderCollection ();
	RStringF name = aRequest.StringPool ().StringF ( HTTP::EConnection, THTTPTable::Table() );
	THTTPHdrVal value;

	if ( request.IsHTTP10 () ) // HTTP 1.0 request,  check for keep alive header value.
		{
		// Check for Connection: Keep Alive header
		if ( headers.GetField ( name, 0, value ) == KErrNone &&
				value.Type ( ) == THTTPHdrVal::KStrFVal &&
				value.StrF ( ).Index ( THTTPTable::Table() ) == HTTP::EKeepAlive )
			{
			bConnRequired = ETrue;
			}
		}
	else // HTTP 1.1
		{
		// Check for Connection: Keep Alive header
		if ( headers.GetField ( name, 0, value ) == KErrNone &&
				value.Type ( ) == THTTPHdrVal::KStrFVal &&
				value.StrF ( ).Index ( THTTPTable::Table() ) == HTTP::EClose )
			{
			bConnRequired = EFalse;
			}
		else
			{
			bConnRequired = ETrue;
			}
		}
	return bConnRequired;
	}

// --------------------------------------------------------------

CHttpServer* CHttpServer::NewL (ESock::CProtocolIntfBase* aProtIntf)
	{
	CHttpServer* self = new (ELeave)CHttpServer(aProtIntf);
	CleanupStack::PushL ( self );
	self->ConstructL ();
	CleanupStack::Pop ();
	return self;
	}

CHttpServer::CHttpServer (ESock::CProtocolIntfBase* aProtIntf) : 
		iStarted ( EFalse ), iIsLeaving ( EFalse ), iProtIntf ( aProtIntf )
	{
	LOG_NODE_CREATE ( KESockFlowTag, CHttpServer );
	}

CHttpServer::~CHttpServer ()
	{
	delete iServiceInfos;
	delete iParser;
	delete iComposer;
	delete iTransaction;
	if ( !iMemChunk.IsEmpty() )
		iMemChunk.Free ();
	iServerHandlerArray.ResetAndDestroy ();
	iServiceControlProviderInfos.Reset ();
	iServiceControlProviderInfos.Close ();
	//delete iCodec;
	iStringPool.Close();
	LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("Destroyed CHttpServer")));
	LOG_NODE_DESTROY(KESockFlowTag, CHttpServer);
	}

void CHttpServer::ConstructL ()
	{
	iStringPool.OpenL(TUPnPTable::Table());
	iStringPool.OpenL(THTTPTable::Table());
	iCodec = CHeaderCodecPlugin::NewL ( KUPnPCodecName, iStringPool );

	iParser = CUpnpRequestParser::NewL ( *this );
	iComposer = CUpnpResponseComposer::NewL ( *this );
	iTransaction = CTransaction::NewL ( *iCodec, iStringPool );
	iServiceInfos = new ( ELeave ) CServiceInfoArray;
	LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("Created CHttpServer")));
	}

void CHttpServer::AddServiceUriL ( const TDesC8& aUri, MHttpEventObserver& aObserver, TNodeCtxId aServiceId, TNodeCtxId aControlProviderId )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("CHttpServer::AddServiceUriL")));
	iServiceInfos->AddServiceUriL ( aUri, aObserver );

	if ( iAppProtIntfId == TNodeId::NullId () && iServiceControlProviderInfos.Count () == 0 )
		{
		CreateListener ();
		}

	if ( !iStarted )
		{
		TServiceControlProviderInfo newInfo ( aServiceId, aControlProviderId );
		iServiceControlProviderInfos.Append ( newInfo );
		}
	else
		{
		RClientInterface::OpenPostMessageClose ( aServiceId, aControlProviderId, ESock::TCFDataClient::TStarted ().CRef () );
		}

	}

TBool CHttpServer::RemoveServiceUri ( const TDesC8& aUri, TNodeCtxId aServiceId, TNodeCtxId aControlProviderId )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("CHttpServer::RemoveServiceUri")));
	iServiceInfos->RemoveServiceUri ( aUri );

	if ( iServiceInfos->CountServiceUri () == 0 ) // last service is stopped, so stop listener
		{
		iLastServiceControlProviderInfo.iServiceId = aServiceId;
		iLastServiceControlProviderInfo.iControlProviderId = aControlProviderId;

		iIsLeaving = ETrue;
		StopServer ();
		return ETrue;
		}

	if ( TNodeId::NullId () != aControlProviderId )
		{		
		RClientInterface::OpenPostMessageClose ( aServiceId, aControlProviderId, TCFDataClient::TStopped ( 0 ).CRef () );		
		}
	return EFalse;
	}

TBool CHttpServer::MatchRequestUri () const
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("CHttpServer::MatchRequestUri")));
	CRequest* request = iTransaction->Request ();
	return iServiceInfos->MatchServiceUri ( request->Handle ().URI (), HttpRequestUtils::HostStr ( *(iTransaction->Request ()) ) ) != KErrNotFound ? ETrue : EFalse;
	}

void CHttpServer::CreateListener ()
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("CHttpServer::CreateListener")));
	const TInt KAppProtIntfFactoryUid = 0x2000D05B; // Put in common place
//	const TInt KGenericContainerIndex = 1;
	const TInt KListenPort = 80; // Maybe we should make it as configurable and multiple ports
	TAppProtIntfQuery query ( KProtocolInetTcp, KListenPort );
	LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("CHttpServer::CreateListener - Posting message TCFMessage::TFindOrCreateFactoryObject")));
	
	TCFFactory::TFindOrCreatePeer foMsg ( TCFPlayerRole::EDataPlane, TUid::Uid( KAppProtIntfFactoryUid ), &query );
//    foMsg.PostTo ( SockManGlobals::Get()->GetPlaneFC ( EDataPlane, KGenericContainerIndex ) );
    TNodeId containerId = static_cast<CUPnPProtocolIntfBase*>(iProtIntf)->GetAppProtIntfContainerId();
    if(containerId != TNodeId::NullId ( ))
    	{    	
    	RClientInterface::OpenPostMessageClose ( NodeId (), containerId, foMsg );
    	}
	}



void CHttpServer::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("CHttpServer::ReceivedL")));

	if ( aMessage.MessageId ().Realm () == TCFFactory::ERealmId )
		{
		if ( aMessage.MessageId().MessageId()  == TCFFactory::TPeerFoundOrCreated::EId )
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("CHttpServer::ReceivedL - TCFMessage::TFOCreated")));
			const TCFFactory::TPeerFoundOrCreated & msg = message_cast < const TCFFactory::TPeerFoundOrCreated  > ( aMessage );
			iAppProtIntfId = msg.iNodeId;			
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( MeshMachine::KActivityNull, NodeId () ), iAppProtIntfId, TAppProtIntfMessage::TJoin ().CRef () );
			}
		}
	else
		{
		switch ( aMessage.MessageId().MessageId() )
			{
			case TAppProtIntfMessage::TJoinComplete::EId:
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("CHttpServer::ReceivedL - TAppProtIntfMessage::TJoinComplete")));

				// We joined. Now we started listening for incomming connections
				iStarted = ETrue;

				for ( TInt i = 0; i < iServiceControlProviderInfos.Count (); i++ )
					{
					RClientInterface::OpenPostMessageClose ( iServiceControlProviderInfos[i].iServiceId, iServiceControlProviderInfos[i].iControlProviderId, TCFDataClient::TStarted ().CRef () );			
					}
				iServiceControlProviderInfos.Reset ();
				}
			break;

			case TAppProtIntfMessage::TNewConnection::EId:
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("CHttpServer::ReceivedL - TAppProtIntfMessage::TNewConnection")));

				TAppProtIntfMessage::TNewConnection& currentMsg = message_cast < TAppProtIntfMessage::TNewConnection > ( aMessage );

				if ( !iIsLeaving )
					{
		        	RMemoryAllocator allocator(static_cast<CUPnPProtocolIntfBase *>( iProtIntf )->GetMemoryChunkManager());
		        	if ( iMemChunk.IsEmpty() )
		        		{
		        		TUPnPMemoryUtils::CreateMemChunk(iMemChunk, currentMsg.iData, allocator);
						RMemChunk memChunk = iMemChunk;
						iParser->ParseRequest ( memChunk, iTransaction->Request() );
		        		}
		        	else
		        		{
		        		//In case of More data parse only new data and then append to iMemChunk.
						RMemChunk mChunk;
						TUPnPMemoryUtils::CreateMemChunk(mChunk, currentMsg.iData, allocator);
						RMemChunk memChunk = mChunk;
						iParser->ParseRequest ( memChunk, iTransaction->Request() );
						iMemChunk.Append(mChunk);
			        	}
					}
				}
			break;

			case TAppProtIntfMessage::TTransferConnection::EId:
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("CHttpServer::ReceivedL - TAppProtIntfMessage::TTransferConnection")));
				// Now we are going to accept the connection. Create a new server handler and reset the parser
				TAppProtIntfMessage::TTransferConnection& msg = message_cast < TAppProtIntfMessage::TTransferConnection > ( aMessage );

				TransferConnectionL ( msg.iSocket, msg.iData );
				}
			break;

			case TAppProtIntfMessage::TLeaveComplete::EId:
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("CHttpServer::ReceivedL - TAppProtIntfMessage::TLeaveComplete")));

				// post dataclient stopped to controlprovider
				if ( TNodeId::NullId () != iLastServiceControlProviderInfo.iControlProviderId )
					{
					RClientInterface::OpenPostMessageClose ( iLastServiceControlProviderInfo.iServiceId, iLastServiceControlProviderInfo.iControlProviderId, TCFDataClient::TStopped ( 0 ).CRef () );
					}

				// post destroy to service provider				
				RClientInterface::OpenPostMessageClose ( NodeId (), iAppProtIntfId, TAppProtIntfMessage::TDestroy ().CRef () );
					
				// make self destruction
				delete this;
				}
			break;

			case TAppProtIntfMessage::TError::EId:
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("CHttpServer::ReceivedL - TAppProtIntfMessage::TError")));
				//Note! Nothing to do if already leaving state,
				// else need to send error to control clients and do listener cleanup
				}
			break;

			default:
			break;
			}
		}
	}

void CHttpServer::StopServer ()
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("CHttpServer::StopServer - Posting message TAppProtIntfMessage::TLeave")));
	RClientInterface::OpenPostMessageClose ( TNodeCtxId ( MeshMachine::KActivityNull, NodeId () ), iAppProtIntfId, TAppProtIntfMessage::TClientLeavingRequest ().CRef () );				
	}

void CHttpServer::GotHeaders()
	{
	TRAPD(err, HttpRequestUtils::ValidateL(*(iTransaction->Request())));
	if(err == KErrCorrupt)
		{
		// Generate an error response. 400 Bad Request
		CHttpServerHandler::CreateResponse ( *(iTransaction->Response ()), HTTPStatus::EBadRequest );
		iComposer->ComposeResponse ( iTransaction->Response () );
		}

	// If we are accepting the connection application protocol interface will
	// send a TTransferConnection msg. In other case we will send a 404 Not Found response
	else if ( MatchRequestUri () )
		{
		RClientInterface::OpenPostMessageClose ( TNodeCtxId ( MeshMachine::KActivityNull, NodeId () ), iAppProtIntfId, TAppProtIntfMessage::TAcceptConnection ().CRef () );	
		}
	else
		{
		// Generate an error response. 404 Not Found
		CHttpServerHandler::CreateResponse ( *(iTransaction->Response ()), HTTPStatus::ENotFound );
		iComposer->ComposeResponse ( iTransaction->Response () );
		}
	// We can reset the parser now as we completed the URI matching.
	// Reset the parser as we don't need to parse anymore
	iParser->ResetParser ();
	}

void CHttpServer::DataParsed ()
	{
	RClientInterface::OpenPostMessageClose ( NodeId (), iAppProtIntfId, TAppProtIntfMessage::TMoreData ().CRef () );	
	}

void CHttpServer::ParserError ( TInt /*aError*/ )
	{
	// Reset the parser as we don't need to parse anymore
	iParser->ResetParser ();
	// Generate an error response. 400 Bad request
	CHttpServerHandler::CreateResponse ( *(iTransaction->Response ()), HTTPStatus::EBadRequest );
	iComposer->ComposeResponse ( iTransaction->Response () );
	}

void CHttpServer::MessageDataReadyL ( RBuf8& aData )
	{
	RMBufChain data;	//Need to Check
	data.CreateL(aData);
	RejectConnection ( data );
	iComposer->ResponseDataSent ();
 	iComposer->ResetComposer (); // composer's DataState is left in WaitingForRelease, so Reset it
	}

void CHttpServer::ComposerError ( TInt /*aError*/ )
	{
	RMBufChain data;
	RejectConnection ( data );
	iComposer->ResetComposer ();
	}

void CHttpServer::RejectConnection ( RMBufChain& aData )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("CHttpServer::RejectConnection")));
	
	// Sending an eror response.
	TAppProtIntfMessage::TRejectConnection msg ( aData );
	RClientInterface::OpenPostMessageClose ( NodeId (), iAppProtIntfId, msg );
	
	// Reset our current transaction
	iTransaction->Reset ();
	}


void CHttpServer::TransferConnectionL ( RInternalSocket& aSocket, RMBufChain& aData )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpServer, KComponent, _L8("CHttpServer::TransferConnectionL")));

	if ( iIsLeaving )
		{
		aSocket.Close ();
		}
	else
		{
		CChunkManager* chunkMgr = static_cast<CUPnPProtocolIntfBase *>( iProtIntf )->GetMemoryChunkManager();
		CHttpServerHandler* serverHandler = CHttpServerHandler::NewL ( aSocket, iMemChunk, *iServiceInfos, iServerHandlerArray, *iCodec, iStringPool, chunkMgr );
		CleanupStack::PushL ( serverHandler );
		iServerHandlerArray.AppendL ( serverHandler );
		CleanupStack::Pop (); // serverHandler

		iTransaction->Reset ();
		}
	aData.Free ();
	
	}
