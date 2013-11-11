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

#include <httpstringconstants.h>
#include <httperr.h>
#include <http/thttptable.h>
#include <upnp/tupnptable.h>

#include "httpserverhandler.h"
#include "httpserver.h"
#include "httpevent.h"

#include "upnpserverconstants.h"
#include "upnplog.h"
#include "upnpmemoryutils.h"

const TUint KRecvTimeOutVal = 30000000; // 30secs


__FLOG_STMT(_LIT8(KComponent,"Flow");)


CHttpServerHandler* CHttpServerHandler::NewL ( RInternalSocket& aSocket, RMemChunk& aData, CServiceInfoArray& aServiceInfos, CServerHandlerArray& aServerHandlers, CHeaderCodec& aCodec, RStringPool& aStringPool, CChunkManager* aChunkMgr )
	{
	CHttpServerHandler* self = new ( ELeave ) CHttpServerHandler ( aSocket, aData, aServiceInfos, aServerHandlers, aCodec, aStringPool, aChunkMgr );
	CleanupStack::PushL ( self );
	self->ConstructL ();
	CleanupStack::Pop (); // self
	return self;
	}

void CHttpServerHandler::ConstructL ()
	{
	CProtocolHandler::ConstructL ();
	iRequestParser = CUpnpRequestParser::NewL ( *this );
	iResponseComposer = CUpnpResponseComposer::NewL ( *this );
	StartNewTransactionL ();
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("Created CHttpServerHandler")));
	}

CHttpServerHandler::~CHttpServerHandler ()
	{
	if ( !iReceivedData.IsEmpty() )
		iReceivedData.Free ();
	delete	iRequestParser;
	delete	iResponseComposer;	
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("Destroyed CHttpServerHandler")));
	}

CHttpServerHandler::CHttpServerHandler ( RInternalSocket& aSocket, RMemChunk& aData, CServiceInfoArray& aServiceInfos, CServerHandlerArray& aServerHandlers, CHeaderCodec& aCodec, RStringPool& aStringPool, CChunkManager* aChunkMgr )
: CProtocolHandler (aSocket ),
iServiceInfos ( aServiceInfos ),
iServerHandlers ( aServerHandlers ),
iServerCodec ( aCodec ),
iStringPool ( aStringPool )
	{
	iSocketHandler.Attach ( iSocket );
	iAllocator.SetChunkManager(aChunkMgr);
	iReceivedData.Assign( aData );
	}

// MParserObserver
void CHttpServerHandler::GotHeaders ()
	{
	CServerTransaction& trans = LastTransaction ();
	RRequest request = trans.Request ()->Handle ();
	RStringF hostStr = trans.Request ()->StringPool().StringF(HTTP::EHost, THTTPTable::Table());

	if(!HttpRequestUtils::IsConnectionRequired( *( trans.Request () ) ))
		trans.SetClosing();

	TRAPD ( err, HttpRequestUtils::ValidateL ( *( trans.Request () ) ) );

	if ( err == KErrNone )
		{
		// Notify the flow that the headers are received
		THTTPEvent evt (THTTPEvent::EGotRequestHeaders);
		NotifyEvent ( evt );
		}
	else
		{
		TInt statusCode = KErrNone;;
		switch ( err )
			{
			case KErrNotSupported:
			statusCode = HTTPStatus::ENotImplemented;
			break;

			case KErrCorrupt:
			statusCode = HTTPStatus::EBadRequest;
			break;

			default:
			// Internal server error
			statusCode = HTTPStatus::EInternalServerError;
			break;
			}
		CHttpServerHandler::CreateResponse ( *(trans.Response()), statusCode, this );
		trans.SetReadyToSend ();
		trans.SetClosing ();
		// We have an error and we no longer parse
		iRequestParser->ResetParser ();
		// If this is the only transaction we can compose the response.
		if ( &LastTransaction () == &CurrentTransaction () )
			{
			CurrentTransaction ().SetComposing ( ETrue );
			iResponseComposer->ComposeResponse ( trans.Response () );
			}
		}
	}


void CHttpServerHandler::GotBodyData ()
	{
	THTTPEvent evt( THTTPEvent::EGotRequestBodyData );
	NotifyEvent ( evt );
	}

void CHttpServerHandler::DataParsed ()
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::DataParsed")));
	
	// Reset parser on completion one parsing successfully.
	if ( !LastTransaction ().Parsing () )
		{
		iRequestParser->ResetParser ();
		}

	// Handle excess data & start new transaction
	if ( !iReceivedData.IsEmpty() )
		{
		// Start a new transaction with the pending data. We are getting pipelined request
		StartNewTransaction ();
		// Free up the data that we received
		iReceivedData.Free ();
		}
	else
		{
		iSocketHandler.Recv ();
		iTimer->StartTimer ( KRecvTimeOutVal );
		}
	}

void CHttpServerHandler::ParsingComplete ( RMemChunk& aExcessData )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::ParsingComplete")));
	
	// Notify the flow that our request is complete
	THTTPEvent evt( THTTPEvent::ERequestComplete );
	NotifyEvent ( evt );

	LastTransaction ().SetParsing ( EFalse );
	
	if ( !aExcessData.IsEmpty () )
		iReceivedData.Append ( aExcessData );
	
	if ( Error () == KErrNone )
		{
		// Check do we have to send the response for our first transaction. This typically happens
		// in the case of URI not found
		CServerTransaction& currentTrans = CurrentTransaction ();
		if ( currentTrans.ReadyToSend ()
				&& !currentTrans.Composing () )
			{
			CurrentTransaction ().SetComposing ( ETrue );
			iResponseComposer->ComposeResponse ( currentTrans.Response () );
			}
		}
	else if ( Error () != KErrNone && iTransactions.Count () == 0 )
		{
		// We give a chance to send any pending responses
		DestroySelf ();
		}

	}

void CHttpServerHandler::ParserError ( TInt aError )
	{
	// If we got a parser error we should see whether we had more than one transaction.
	// Yes? Then we should wait for the previous transactions completion then
	// close the connection.
	// No? we just cleanup ourself. We will not send any response as we got an invalid request
	// and we have no previous transactions.

	// Note: The above is what we have to do... But for now Cleanup ourself by doing nothing
	SetError ( aError );
	DestroySelf ();	 // ... more
	}

// MComposerObserver
void CHttpServerHandler::MessageDataReadyL ( RBuf8& aData )
	{
	RMBufChain sendData;
	sendData.CreateL(aData);
	iSocketHandler.Send ( sendData );
	}

void CHttpServerHandler::ComposingConcluded ()
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::ComposingConcluded")));
	iResponseComposer->ResetComposer ();

	TBool needToClose = CurrentTransaction ().IsClosing ();

	// Delete the first transaction
	DeleteTransaction ( iTransactions[0] );

	// Check we should close the connection once we sent response to this transaction
	if ( needToClose )
		{
		DestroySelf ();
		return;
		}

	if ( iTransactions.Count () > 0 )
		{
		// Check the next transaction is ready to send
		CServerTransaction& trans = CurrentTransaction ();
		if ( trans.ReadyToSend () )
			{
			CurrentTransaction ().SetComposing ( ETrue );
			iResponseComposer->ComposeResponse ( trans.Response () );
			}
		}
	}

void CHttpServerHandler::ComposerError ( TInt aError )
	{
	// Now we have an error. We just cleanup ourself
	SetError ( aError );
	DestroySelf ();
	}

void CHttpServerHandler::SendComplete ( TInt /* aLength */ )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::SendComplete")));
	CurrentTransaction ().SetComposing ( EFalse );
	iResponseComposer->ResponseDataSent	();
	}

void CHttpServerHandler::RecvComplete ( RMBufChain& aData )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::RecvComplete")));
	
	iTimer->StopTimer ();
	
	RMemChunk memChunk;
	TUPnPMemoryUtils::CreateMemChunk(memChunk, aData, iAllocator);
	iReceivedData.Append ( memChunk );	
	// Check we have some transactions to give the received data or should we create a new one
	if ( iTransactions.Count () > 0 && LastTransaction ().Parsing () )
		{
		iRequestParser->ParseRequest ( iReceivedData, LastTransaction ().Request () );
		// parser takes ownership of aData, so it can be cleaned-up
		iReceivedData.Free ();
		return;
		}
	StartNewTransaction ();

	if ( Error () != KErrNone && iTransactions.Count () == 0 )
		{
		// We give a chance to send any pending responses
		DestroySelf ();
		}
	aData.Free();
	}

void CHttpServerHandler::TimeOut ()
	{
	if ( iTransactions.Count () > 0 )		
		{
		CServerTransaction& trans = CurrentTransaction ();
		if ( trans.Parsing () )
			{
			// reset the request parser that is waiting for more data
			iRequestParser->ResetParser ();
			trans.SetParsing ( EFalse );
			//create reponse with 408 status code
			CHttpServerHandler::CreateResponse ( *( trans.Response () ), KRequestTimeoutStatusCode, this );
			trans.SetReadyToSend ();
			trans.SetComplete ();
			// trigger compose self start event for the timed-out transaction
			THTTPEvent evt ( THTTPEvent::ECompleteResponse );
			OnHttpEvent ( iTransactions[0], evt );
			}
		}
	else
		{
		// no transactions are pending,simply make self destroy.
		DestroySelf ();
		}
	}

void CHttpServerHandler::Error ( TOperation /*aOperation*/, TInt aError  )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::Error - %D"), aError));
	// This is a socket error. We cannot do anything here.
	// We just cleanup ourself
	SetError ( aError );
	
	iResponseComposer->DataSentFailed ();		
	iResponseComposer->ResetComposer ();
	iRequestParser->ResetParser ();
	iSocketHandler.CancelAll ();	
	
	TInt i = iTransactions.Count () - 1;
	while ( i >= 0 )
		{
		CTransaction* trans = iTransactions[i];
		if ( trans->Complete () )
			{
			iTransactions.Remove ( i );
			delete trans;
			}
		--i;
		}
		
	DestroySelf ();
	}

CServerTransaction& CHttpServerHandler::CurrentTransaction () const
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::CurrentTransaction")));
	__ASSERT_DEBUG ( iTransactions.Count () > 0, User::Invariant () );
	return static_cast < CServerTransaction& > ( *( iTransactions[0] ) );
	}

CServerTransaction& CHttpServerHandler::LastTransaction () const
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::LastTransaction")));
	TInt count = iTransactions.Count ();
	__ASSERT_DEBUG ( count > 0, User::Invariant () );
	return static_cast < CServerTransaction& > ( *( iTransactions[ count - 1 ]  ) );
	}

void CHttpServerHandler::NotifyEvent ( THTTPEvent& aEvent )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::NotifyEvent")));
	CServerTransaction& trans = LastTransaction ();
	// Ensure that the flow still exists
	CRequest* request = trans.Request ();
	TInt pos = iServiceInfos.MatchServiceUri ( request->Handle ().URI (), HttpRequestUtils::HostStr ( *request ) );
	if ( pos == KErrNotFound )
		{
		if ( !trans.Composing () )
			{
			CHttpServerHandler::CreateResponse ( *( trans.Response () ), HTTPStatus::ENotFound, this );
			trans.SetReadyToSend ();
			}
		}
	else if ( !trans.Complete () ) // Server flow on validation failure, 
								   // completes the transaction after creating error response
								   // so no need to push ERequestComplete Event to ServerFlow.
								   // start composing error response
		{
		CServiceInfo& info = iServiceInfos [ pos ];
		info.Observer ().OnHttpEvent ( &trans, aEvent );
		}
	}

void CHttpServerHandler::CreateResponse ( CResponse& aResponse, TInt aStatusCode, MHttpEventObserver* aServerHandler )
	{
	TRAPD ( err, CHttpServerHandler::CreateResponseL ( aResponse, aStatusCode ) );

	if ( err != KErrNone )
		{
		RResponse response = aResponse.Handle ();
		response.SetStatusCode ( HTTPStatus::EInternalServerError );

		// Should be changed once the status text us moved to te string table
		RStringF responseTextString;
		responseTextString = aResponse.StringPool().StringF ( UPnP::EInternalServerError, TUPnPTable::Table() );
		response.SetStatusText ( responseTextString );
		
		if ( aServerHandler != NULL )
			{
			CHttpServerHandler& handler = static_cast < CHttpServerHandler& > ( *aServerHandler );
			handler.SetError ( err );
			}
		}
	}

void CHttpServerHandler::CreateResponseL ( CResponse& aResponse, TInt aStatusCode )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::CreateResponse")));
	TVersion version ( KMajorVersion, KMinorVersion, 0 );
	RResponse response = aResponse.Handle ();
	RStringPool sp = aResponse.StringPool ();

	response.SetStatusCode ( aStatusCode );
	response.SetVersion ( version );

	// Add current date header
	RHTTPHeaders hdrs = response.GetHeaderCollection ();
	TTime time;
	time.UniversalTime();
	THTTPHdrVal timeHdrVal ( time.DateTime() );
	hdrs.SetFieldL ( aResponse.StringPool().StringF(HTTP::EDate, THTTPTable::Table() ), timeHdrVal );

	_LIT8 ( KServerName, "Symbian UPnP/1.0 server" );
	RStringF serverStr = sp.OpenFStringL ( KServerName () );
	THTTPHdrVal serverHdrVal ( serverStr );
	hdrs.SetFieldL ( sp.StringF ( HTTP::EServer, THTTPTable::Table() ), serverHdrVal );
	serverStr.Close ();

	if ( aStatusCode != HTTPStatus::EOk )
		{// Content Length
		hdrs.SetFieldL ( sp.StringF( HTTP::EContentLength, THTTPTable::Table() ), THTTPHdrVal ( 0 ) );
		}

	RStringF responseTextString;
	response.SetStatusText ( responseTextString );

	switch ( aStatusCode )
		{
		case HTTPStatus::EOk:
		responseTextString = sp.StringF ( UPnP::EOk, TUPnPTable::Table() );
		break;

		case HTTPStatus::ENotFound:
		responseTextString = sp.StringF ( UPnP::ENotFound, TUPnPTable::Table() );
		break;

		case HTTPStatus::EBadRequest:
		responseTextString = sp.StringF ( UPnP::EBadRequest, TUPnPTable::Table() );
		break;

		case HTTPStatus::EInternalServerError:
		responseTextString = sp.StringF ( UPnP::EInternalServerError, TUPnPTable::Table() );
		break;

		case HTTPStatus::ENotImplemented:
		responseTextString = sp.StringF ( UPnP::ENotImplemented, TUPnPTable::Table() );
		break;

		case HTTPStatus::EPreconditionFailed:
		responseTextString = sp.StringF ( UPnP::EPreconditionFailed, TUPnPTable::Table() );
		break;

		case HTTPStatus::EMethodNotAllowed:
		responseTextString = sp.StringF ( UPnP::EMethodNotAllowed, TUPnPTable::Table() );
		break;
	
		case HTTPStatus::EUnsupportedMediaType:
		responseTextString = sp.StringF ( UPnP::EUnsupportedMediaType, TUPnPTable::Table() );
		break;

		case KRequestTimeoutStatusCode:
		responseTextString = sp.StringF ( UPnP::ERequestTimeout, TUPnPTable::Table() );
		break;
		
		case KInvalidSeqStatusCode:
		responseTextString = sp.StringF ( UPnP::EMethodNotAllowed, TUPnPTable::Table() );
		break;

		default:
		ASSERT ( 0 );
		break;
		}
	response.SetStatusText ( responseTextString );
	}

void CHttpServerHandler::StartNewTransactionL ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::StartNewTransaction")));
	CServerTransaction* trans = CServerTransaction::NewL ( iServerCodec, iStringPool, *this );
	CleanupStack::PushL ( trans );
	User::LeaveIfError ( iTransactions.Append ( trans ) );
	CleanupStack::Pop (); // trans
	iRequestParser->ParseRequest ( iReceivedData, trans->Request () ); // Needs change
	iReceivedData.Free (); // Free up the data that we received, parser takes ownership
	trans->SetParsing ( ETrue );
	trans->Response()->Handle().SetBody ( *this ); // Set self as the response body supplier
	}

void CHttpServerHandler::StartNewTransaction ()
	{
	TRAPD ( err, StartNewTransactionL () );
	if ( err != KErrNone )
		{
		SetError ( err );
		}
	}

void CHttpServerHandler::DeleteTransaction ( CTransaction* aTrans )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::DeleteTransaction")));
	for ( TInt i = 0; i < iTransactions.Count (); ++i )
		{
		CTransaction* trans = iTransactions[i];
		if ( trans == aTrans )
			{
			iTransactions.Remove ( i );
			delete trans;
			return;
			}
		}
	}

TInt CHttpServerHandler::OnHttpEvent ( CTransaction* aTransaction, THTTPEvent& aEvent )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::OnHttpEvent")));
	CServerTransaction& trans = static_cast< CServerTransaction& > (*aTransaction);
	
	if ( Error ()
		|| aEvent == THTTPEvent::EFailed ) // ControlChannel transaction response is half send, so close it
		{
		if ( &trans == &CurrentTransaction () )
			{
			iResponseComposer->ResetComposer ();
			}
		DeleteTransaction ( aTransaction );
		TInt err = iError;
		DestroySelf ();
		return err;
		}
		
	// Check if this is the first transaction that we have to respond. Otherwise just move
	// this into a ready to send state.	
	if ( &trans != &CurrentTransaction() )
		{
		// THTTPEvent::EFailed is used for control channel pending transactions
		if ( aEvent == THTTPEvent::EFailed )
			{
			DeleteTransaction ( aTransaction );
			return KErrNone;
			}
		else
			{
			trans.SetReadyToSend ();
			return KErrNone;
			}
		}	

	// The flow can Notify us in 2 ways.
	// 1. Complete response information is available including any body data. This happens when
	// the flow receives the response from the control plane
	// In this case we will receives a THTTPEvent::ECompleteResponse event from the flow
	// in which case we start composing the response
	// 2. The flow receives the data via RControlChannel send from the application
	// Here, we have to start composing when we receive the THTTPEvent::EGotResponseHeaders &
	// THTTPEvent::EGotResponseBodyData in which case we notify the composer about the body data.
	// 1. We have to start composing when we get THTTPEvent::EGotResponseHeaders
	// 2. We have to inform the composer that we have new response body data available
	// 3. Response completion --- we don't do anything here, just an indication that we are concluded the response.
	//  keep going ComposingConcluded will reset the composer
	switch ( aEvent.iStatus )
		{
		case THTTPEvent::EGotResponseHeaders: // for control channel
		case THTTPEvent::ECompleteResponse:	  // for control plane
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::OnHttpEvent - THTTPEvent::EGotResponseHeaders")));
			CurrentTransaction ().SetComposing ( ETrue );
			iResponseComposer->ComposeResponse ( CurrentTransaction ().Response() );
			}
		break;

		case THTTPEvent::EGotResponseBodyData:
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::OnHttpEvent - THTTPEvent::EGotResponseBodyData")));

			if ( !CurrentTransaction ().Composing () )
				iResponseComposer->NotifyNewBodyData ();
			}
		break;

		case THTTPEvent::EResponseComplete:
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::OnHttpEvent - THTTPEvent::EResponseComplete")));
			// We don't do anything here. We completed the whole response.
			iResponseComposer->NotifyNewBodyData (); // This is the last chunk. This happens when we send via chunked encoding
			}
		break;

		default:
			ASSERT(0);
			break;
		}
	return KErrNone;
	}

TBool CHttpServerHandler::CanDestroy () const
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::CanDestroy")));
	return ( iTransactions.Count () == 0 );
	}

// We will destroy ourself if we are not waiting for any response.
void CHttpServerHandler::DestroySelf ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::DestroySelf")));
	if ( CanDestroy () )
		{
		// Remove from the server handler array	 ...
		for ( TInt i = 0; i < iServerHandlers.Count (); ++i )
			{
			if ( iServerHandlers[i] == this )
				{
				iServerHandlers.Remove ( i );
				break;
				}
			}

		// We don't have any more transactions. We can delete ourself
		delete this;
		}

	}

void CHttpServerHandler::SetError ( TInt aError )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::SetError - %d"), aError));
	iError = aError;
	}

TBool CHttpServerHandler::Error () const
	{
	return ( iError != KErrNone );
	}

// -------------------------------------

TBool CHttpServerHandler::GetNextDataPart ( TPtrC8& aDataPart )
	{
	CServerTransaction& trans = CurrentTransaction ();
	trans.GetBodyPart ( aDataPart );
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::GetNextDataPart - %d, %d"), trans.IsLastBodyPart (), aDataPart.Length () ));
	return trans.IsLastBodyPart ();
	}

void CHttpServerHandler::ReleaseData ()
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::ReleaseData" )));
	CServerTransaction& trans = CurrentTransaction ();
	trans.RemoveBodyPart ();
	if ( !trans.BodyParts ().IsEmpty () )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler::ReleaseData body part present" )));
		TInt count = trans.BodyParts().NumBufs();
		// We will call NotifyNewBodyData only when we are sending it as a chunked-encoding &
		// the number of MBuf available is > 1. Last MBuf we will notify via a EResponseComplete from OnHttpEvent
		if ( ( trans.DataLeft() == KErrUnknown && count > 1 )
				|| ( count > 0 ) ) // Normal send by knowing the length. we will notify if we have > 0 MBufs
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpSrvrHndlr, KComponent, _L8("CHttpServerHandler - Calling NotifyNewBodyData on composer" )));
			trans.SetComposing ( ETrue );
			iResponseComposer->NotifyNewBodyData ();
			}
		}
	}

TInt CHttpServerHandler::OverallDataSize ()
	{
	CServerTransaction& trans = CurrentTransaction ();
	// For now we calculate the total data size based on the Content-Length field
	// Note: In the futre, ( for full HTTP server ) this should be based on the chunked encoding
	// field or Content-Length
	TInt dataSize = MHttpMessageParserObserver::ENoBody;
	// Check is there a Content-Length field?
	CResponse* response = trans.Response ();
	RStringF contentLength = response->StringPool().StringF( HTTP::EContentLength, THTTPTable::Table() );
	THTTPHdrVal value;
	TInt err = response->Handle().GetHeaderCollection().GetField( contentLength, 0, value );
	if ( err == KErrNone )
		{
		dataSize = value.Int ();
		}
	return dataSize;
	}
