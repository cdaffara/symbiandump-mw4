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


// System includes
#include <es_sock.h>


#include <http/thttptable.h>
#include <httpstringconstants.h>
#include <httperr.h>
#include <utf.h>
#include <inetprottextutils.h>
#include <upnp/tupnptable.h>

// Local includes
#include "upnpserverconstants.h"
#include "upnplog.h"
#include "chttpclienthandler.h"
#include "upnpmemoryutils.h"


const TUint KInactivityTimeOutVal = 60000000; // 60secs, DLNA Requirements [7.2.8.7]

__FLOG_STMT(_LIT8(KComponent,"Flow");)

//
CHTTPClientTransaction* CHTTPClientTransaction::NewL ( CHeaderCodec& aCodec, RStringPool& aStringPool, TNodeCtxId aNodeCtxId )
	{	
	CHTTPClientTransaction* self = new ( ELeave ) CHTTPClientTransaction ( aNodeCtxId );
    CleanupStack::PushL ( self );
    self->ConstructL ( aCodec, aStringPool );
    CleanupStack::Pop ( self );
	return self;
	}

CHTTPClientTransaction::CHTTPClientTransaction ( TNodeCtxId aNodeCtxId )
	:CTransaction ( ), iNodeCtxId ( aNodeCtxId ), iComposingStarted ( EFalse )
	{	
	}

CHTTPClientTransaction::~CHTTPClientTransaction ( )
	{	
	RemoveAllBodyParts ( );
	}


//
CHTTPClientHandler* CHTTPClientHandler::NewL ( MHttpEventObserver& aObserver, CChunkManager* aChunkMgr, TSockAddr* aAddr )
	{	
	CHTTPClientHandler* self = new ( ELeave ) CHTTPClientHandler ( aObserver, aChunkMgr, aAddr );
	CleanupStack::PushL ( self );
	self->ConstructL ( );
	CleanupStack::Pop ( self );
	return self;
	}

CHTTPClientHandler::CHTTPClientHandler ( MHttpEventObserver& aObserver, CChunkManager* aChunkMgr, TSockAddr* aAddr )
	: CProtocolHandler ( ), iObserver ( aObserver )
	{
	if ( aAddr )
		iRemoteAddr = *aAddr;
	iFlags = 0;
	iFirstTrans = ETrue;
	SetSocketIdle ( );
	iAllocator.SetChunkManager(aChunkMgr);
	SetComposerIdle ( );
	SetParserIdle ( );
	SetReq100Continue ( EFalse );
	}

void CHTTPClientHandler::ConstructL ( )
	{
	CProtocolHandler::ConstructL ();
	iComposer = CUpnpRequestComposer::NewL ( *this );
	iParser = CUpnpResponseParser::NewL ( *this );
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("Created CHTTPClientHandler")));
	}

CHTTPClientHandler::~CHTTPClientHandler ( )
	{	
	CloseLink ( );
	delete iComposer;
	delete iParser;
	if ( !iExcessData.IsEmpty() )
		iExcessData.Free ( );
	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("Deleted CHTTPClientHandler")));
	}

CTransactionWrapper* CHTTPClientHandler::GetTransactionWrapperL ( )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::GetTransactionWrapperL")));
	return new (ELeave ) CTransactionWrapper ( iTransactions );
	}

void CHTTPClientHandler::SetConnectionInfo ( TSockAddr& aAddr )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::SetConnectionInfo")));
	iRemoteAddr = aAddr;
	if ( !SocketIdle ( ) )
		{
		// socket is connected & there is no network traffic. This happens only when pipelining is enabled
		// Flow can call this API inorder to reuse. so connection is no longer useful, close it.
		CloseLink ( );
		}
	}

void CHTTPClientHandler::SubmitTransaction ( CTransaction* aTransaction )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::SubmitTransaction")));
	TInt pos = FindTransaction ( aTransaction );
	TInt err = KErrNone;

	if ( KErrNotFound == pos )
		{
		TRAP_IGNORE ( iTransactions.Append ( aTransaction ) );
		}

	// Set data supplier for body
	CRequest* request = aTransaction->Request ( );
	request->AddBody ( this );
	
	err = OpenLink ( );
	if ( err != KErrNone ) // memory failures
		{
		NotifyPendingTransactions ( err );
		CloseLink ( );
		}
	else
		{
		ProcessPendingTransactions ( iTransactions.Count () - 1 );
		iTimer->StopTimer ();
		}			
	}


void CHTTPClientHandler::NotifyNewRequestBodyData ( CTransaction* aTransaction, RMBufChain& aData )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::NotifyNewRequestBodyData")));
	
	// CheckToNotifyNewBodyPart
	CHTTPClientTransaction* ctrans = static_cast<CHTTPClientTransaction*> ( CurrentTransaction ( ) );
	TInt availableBufCount = ctrans->BodyParts( ).NumBufs ( );
	TInt currentPartId = ctrans->RequestPartIdx( );

	if ( currentPartId == availableBufCount )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("New Body part is added")));

		// all available parts are send, and a new one is just added
		// notify waiting upnp message composer
		iComposer->NotifyNewBodyData( );
		}
	
	aTransaction->AddBodyPart( aData );
	}

void CHTTPClientHandler::CancelTransaction ( CTransaction* aTransaction )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::CancelTransaction")));
	TInt reqPos = FindTransaction ( aTransaction );
	__ASSERT_DEBUG ( reqPos != KErrNotFound, User::Invariant ( ) );

	// Cleanup response headers
	aTransaction->Response( )->Handle( ).GetHeaderCollection ( ).RemoveAllFields ( );	
	RemoveTransaction ( reqPos );
		
	CHTTPClientTransaction* trans = static_cast<CHTTPClientTransaction*>( aTransaction );	
	if ( trans->ComposingStarted () )
		{	
		ResetTransactions (); // until request positions.
		iComposer->ResetComposer ();
		iParser->ResetParser ();
		
		ReOpenLink ();
		}
	}

CTransaction* CHTTPClientHandler::CurrentTransaction ( ) const
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::CurrentTransaction")));
	__ASSERT_DEBUG ( iTransactions.Count ( ) > 0, User::Invariant ( ) );
	return iTransactions[0];
	}

void CHTTPClientHandler::ProcessPendingTransactions ( TInt aNextTransIdx )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::ProcessPendingTransactions")));
	if ( HasPendingTransactions () && CanPipeLine () )
		{
		StartComposer ( iTransactions[aNextTransIdx] );
		}
	}

TInt CHTTPClientHandler::OpenLink ( )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::OpenLink")));
	TInt err = KErrNone;

	if ( HasPendingTransactions ( ) && SocketIdle ( ) )
		{
		TRAP ( err, OpenSocketL ( KAfInet, KSockStream, KProtocolInetTcp ) );
		if ( err != KErrNone )
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::OpenLink - Returned with error %d"), err));
			return err;
			}
		ResetSocketIdle ( );
		}
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::OpenLink - Returned with error %d"), err));
	return err;
	}

void CHTTPClientHandler::OpenSocketL ( TUint aAddrFamily, TUint aSockType, TUint aProtocol )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::OpenSocketL")));
	if ( NULL == iSocketOpener )
		{
		iSocketOpener = CSocketOpener::NewL ( *this );
		}
	iSocketOpener->MakeSocket ( aAddrFamily, aSockType, aProtocol );
	}

void CHTTPClientHandler::MakeConnection ( )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::MakeConnection")));
	iSocketHandler.Connect ( iRemoteAddr );
	}

void CHTTPClientHandler::StartComposer ( CTransaction* aTransaction )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::StartComposer")));
	if ( ComposerIdle ( ) )
		{ 
		ResetComposerIdle ( );
		
		CHTTPClientTransaction* ctrans = static_cast<CHTTPClientTransaction*>( aTransaction );		
				
		SetReq100Continue ( CheckFor100ContinueHeader ( * ( aTransaction->Request () ) ) );
		ctrans->SetComposingStarted ( ETrue );
 		iComposer->ComposeRequest ( aTransaction->Request () ); 		
		}
	}

void CHTTPClientHandler::StartParser ( CTransaction* aTransaction, RMemChunk& aData )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::StartParser")));
	
	ResetParserIdle ( );
	
	iParser->ParseResponse ( aData, aTransaction->Response ( ) );
	}

void CHTTPClientHandler::CloseLink ( )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::CloseLink")));
	if ( iSocketOpener )
 		{
 		delete iSocketOpener;
 		iSocketOpener = NULL;
 		}

 	// Cancel all socket operations
	iSocketHandler.CancelAll ( );
	iSocket.Close ( );

	iFlags = 0;
	iFirstTrans = ETrue;
	SetSocketIdle ( );
	SetComposerIdle ( );
	SetParserIdle ();
	SetReq100Continue ( EFalse );
	}

TBool CHTTPClientHandler::CheckForConnectionPersistence ( )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::CheckForConnectionPersistence")));
	CResponse* response = CurrentTransaction( )->Response ( );
	TVersion version = response->Version( );

	if ( version.iMajor == 1 && version.iMinor == 0 )
		{ // non-persistent
		LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::CheckForConnectionPersistence - HTTP v1.0 in use. Connection is non-persistant")));
		return EFalse;
		}

	const RStringPool& strP = response->StringPool ( );
    RStringF name = strP.StringF ( HTTP::EConnection, THTTPTable::Table() );
    RHTTPHeaders headers = response->Handle( ).GetHeaderCollection ( );
    THTTPHdrVal value;

	if ( version.iMajor == 1 && version.iMinor == 1 )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::CheckForConnectionPersistence - HTTP v1.1 in use")));
		// Check for Connection: close header
		if ( headers.GetField ( name, 0, value ) == KErrNone &&
				value.Type ( ) == THTTPHdrVal::KStrFVal &&
				value.StrF ( ).Index ( THTTPTable::Table() ) == HTTP::EClose )
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::CheckForConnectionPersistence - Connection: Close found")));
			LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::CheckForConnectionPersistence - Connection is non-persistant")));
           	// Connection header has "close" value, non-persistent
           	return EFalse;
           	}
		LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::CheckForConnectionPersistence - Connection is persistant")));
        // connection is non-persistent
		return ETrue;
		}
	// else HTTP 1.x
	if ( headers.GetField ( name, 0, value ) == KErrNone &&
			value.Type ( ) == THTTPHdrVal::KStrFVal &&
    		value.StrF ( ).Index ( THTTPTable::Table() ) == HTTP::EKeepAlive )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::CheckForConnectionPersistence - HTTP v1.x in use")));
		LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::CheckForConnectionPersistence - Connection:: Keep-Alive found")));
		LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::CheckForConnectionPersistence - Connection is persistant")));
       	// Connection header has "keep-alive" value, persistent
       	return ETrue;
        }

	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::CheckForConnectionPersistence - Connection is non-persistant")));
    // non-persistent
	return EFalse;
	}

TBool CHTTPClientHandler::CheckFor100ContinueHeader ( CRequest& aRequest )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::CheckFor100ContinueHeader")));
	
	TBool value = EFalse;
	if ( aRequest.Method ( ).Index ( THTTPTable::Table() ) == HTTP::EPOST
		|| aRequest.Method ( ).Index ( TUPnPTable::Table() ) == UPnP::EMPost )
		{
    	const RStringPool& strP = aRequest.StringPool ( );

    	// Check for Except: 100-continue header
    	RStringF name = strP.StringF ( HTTP::EExpect, THTTPTable::Table() );
    	THTTPHdrVal hdrValue;
		RHTTPHeaders headers = aRequest.Handle( ).GetHeaderCollection ( );

    	if ( headers.GetField ( name, 0, hdrValue ) == KErrNone  && hdrValue.Type ( ) == THTTPHdrVal::KStrFVal )
			{
        	// what's the value?
    		if( hdrValue.StrF ( ).Index ( THTTPTable::Table() ) == HTTP::E100Continue )
				{
            	// Expect header has a value of 100-continue
            	value = ETrue;
            	}
			}
		}
	
	return value;
	}

 void CHTTPClientHandler::NotifyPendingTransactions ( TInt aError )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::NotifyPendingTransactions")));
	TInt ii = iTransactions.Count ( );

	while ( ii > 0 )
		{
		// Notify waiting transaction of the error
		THTTPEvent event ( aError );
		iObserver.OnHttpEvent ( iTransactions[--ii], event );
		}

	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::NotifyPendingTransactions - Pending transaction queue has been reset")));
	// Reset the pending transaction queue.
	iTransactions.Reset ( );
	}


// MSocketHandler
void CHTTPClientHandler::OpenComplete ( RInternalSocket& aSocket )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::OpenComplete")));
	iSocket = aSocket;
	iSocketHandler.Attach ( aSocket );

	if ( HasPendingTransactions ( ) )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::OpenComplete - Pending transactions found")));
		MakeConnection ( );
		}
	else
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::OpenComplete - Found no pending transactions")));
		CloseLink ( );
		}
	}

void CHTTPClientHandler::ConnectComplete ( )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::ConnectComplete")));
	if ( HasPendingTransactions ( ) )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::ConnectComplete - Pending transactions found")));
		StartComposer ( CurrentTransaction ( ) );
		}
	else
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::ConnectComplete - Found no pending transactions")));
		CloseLink ( );
		}
	}

void CHTTPClientHandler::SendComplete ( TInt /*aLength*/ )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::SendComplete")));
	// Handle 100-continue
	if ( Request100Continue () )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::SendComplete - Expect: 100-Continue found")));
		
		// headers sent to server. before sending body, read socket for 100-continue response
		iSocketHandler.Recv ();
		SetReadTriggered ();
		
		SetReq100Continue ( EFalse );
		}
    else
    	{
    	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::SendComplete - Expect: 100-Continue not found")));
    	
		iComposer->RequestDataSent ( ); // enables composer to release send data chain
    	}
	}

void CHTTPClientHandler::RecvComplete ( RMBufChain& aData )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::RecvComplete")));
	if ( HasPendingTransactions () )
		{// Parser should be in Idle state, else something is happening seriously wrong.

		RMemChunk memChunk;
		TUPnPMemoryUtils::CreateMemChunk(memChunk, aData, iAllocator);

		StartParser ( CurrentTransaction ( ), memChunk );
		}

	//parser takes ownership of aData, so it can be cleaned-up
	aData.Free ( );

	ResetReadTriggered ( );
	}

void CHTTPClientHandler::Error ( TOperation aOperation, TInt aError )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::Error")));
	switch ( aOperation )
		{
		case EOpenByProtocol:
		case EConnect:
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::Error - TOperation::EConnect")));
			CloseLink ( );
			NotifyPendingTransactions ( aError );
			}
			break;

		case EMBufSend:
			iComposer->DataSentFailed ();
		case ERecv:
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpClntTrans, KComponent, _L8("CHTTPClientHandler::Error - TOperation::ERecv")));
			
			iComposer->ResetComposer ();
			iParser->ResetParser ();				
			CloseLink ( );
			
			if ( aError == KErrEof || aError == KErrCancel )
				{
				ResetTransactions ();
				ReOpenLink ();								
				}
			else
				{
				NotifyPendingTransactions ( aError );
				}
			}
			break;

		default:
			__ASSERT_DEBUG ( 0, User::Invariant ( ) );
			break;
		}
	}

void CHTTPClientHandler::ResetTransactions ()
	{	
	for ( TInt i = 0; i < iTransactions.Count (); i++ )
		{
		CHTTPClientTransaction* ctrans = static_cast<CHTTPClientTransaction*>( iTransactions[i] );
		ctrans->ResetRequestPart ();
		ctrans->SetComposingStarted ( EFalse );
		}
	}
	
// MComposerObserver
void CHTTPClientHandler::MessageDataReadyL ( RBuf8& aData )
	{
	RMBufChain data;	//Need to Check
	data.CreateL(aData);
	iSocketHandler.Send ( data );
	SetWriteTriggered ( );
	}

void CHTTPClientHandler::ComposingConcluded ( )
	{
	if ( !ReadTriggered ( ) )
		{
		iSocketHandler.Recv ( ); // should call this before compose conclude for 100-continue
		SetReadTriggered ( );
		}
	
	TInt nextTransId = FindTransaction ( & (iComposer->Request () ) ) + 1;
	
	ResetWriteTriggered ( );
	SetComposerIdle ( );		
	iComposer->ResetComposer ();
	
	if ( nextTransId < iTransactions.Count () )
		{
		ProcessPendingTransactions ( nextTransId );
		}
	}

void CHTTPClientHandler::ComposerError ( TInt aError )
	{	
	// Report there is an Error in the transaction(iCurrentRequest)
	// and do cleanup, process next transaction or close link
	TInt pos = FindTransaction ( &(iComposer->Request( )) );
	__ASSERT_DEBUG ( pos != KErrNotFound, User::Invariant ( ) );

	THTTPEvent event ( aError );
	iObserver.OnHttpEvent ( iTransactions[pos], event );
	RemoveTransaction ( pos );

	SetComposerIdle ();
	iComposer->ResetComposer ();
	
	if ( WriteTriggered ( ) ) // request data is half sent to server.
		{
		ReOpenLink ();
		return;
		}
	
	if ( iTransactions[pos] != NULL ) // no request data is sent, process next transaction
		{
		StartComposer ( iTransactions[pos] );
		}
	}


// MParserObserver
void CHTTPClientHandler::GotHeaders ( )
	{	
	// check for connection persistance
	if ( CheckForConnectionPersistence ( ) )
		{
		EnablePipeLining ();		
		}
	else
		{
		DisablePipeLining ( );
		}

	// 100-continue status is not notified to clients, but resume composer to send body
	CResponse* response = CurrentTransaction( )->Response( );
	if ( response->Status ( ) == HTTPStatus::EContinue )
		{
		iComposer->RequestDataSent ( ); // enables composer to send first body part.
    	}
    else
		{
		THTTPEvent event ( THTTPEvent::EGotResponseHeaders );
		iObserver.OnHttpEvent ( CurrentTransaction ( ), event);
		}
	}

void CHTTPClientHandler::GotBodyData ( )
	{
	THTTPEvent event ( THTTPEvent::EGotResponseBodyData );
	iObserver.OnHttpEvent ( CurrentTransaction ( ), event );
	}

void CHTTPClientHandler::DataParsed ( )
	{	
	if ( HasPendingTransactions ( ) && !ReadTriggered ( ) )
		{
		iSocketHandler.Recv ( );
		SetReadTriggered ( );
		}
	
	if ( ParserIdle () )
		{		
		iParser->ResetParser ();
		}
	
	if ( !iExcessData.IsEmpty ( ) )
		{
		StartParser ( CurrentTransaction ( ), iExcessData );
		iExcessData.Free ( );
		}
	}

void CHTTPClientHandler::ParsingComplete ( RMemChunk& aExcessData )
	{	
	// notify the observer response complete and cleanup it.
	CTransaction* ctrans = CurrentTransaction ( );
	RemoveTransaction ( 0 );	
	THTTPEvent event ( THTTPEvent::EResponseComplete );
	iObserver.OnHttpEvent ( ctrans, event );

	SetParserIdle ( );
	
	//check for pipelining
	if ( !CanPipeLine ( ) )
		{
		// Pipelining is diabled and discard the junk data received
		aExcessData.Free ();		
		ReOpenLink ();	
		}
	else
		{
		if ( iFirstTrans ) // Note, when pipelining is enabled and
						   // first transaction is completed, composer is waiting to start next transaction
			{
			iFirstTrans = EFalse;
			ProcessPendingTransactions ();
			}
		if ( HasPendingTransactions ( ) )
			{
			// PipeLining is enabled so next transaction response is received, start parser
			iExcessData.Assign ( aExcessData );			
			}
		else
			{
			aExcessData.Free ();
			iTimer->StartTimer ( KInactivityTimeOutVal );
			}
		}
	}

void CHTTPClientHandler::ParserError ( TInt aError )
	{	
	// Report there is an Error in the transaction(0)
	THTTPEvent event ( aError );
	iObserver.OnHttpEvent ( CurrentTransaction ( ), event );
	RemoveTransaction ( 0 );
	
	iParser->ResetParser ();
	ReOpenLink ();
	}

void CHTTPClientHandler::RemoveTransaction ( TInt aPos )
	{
	if ( aPos < iTransactions.Count () )
		{
		iTransactions.Remove ( aPos );
		}
	}

void CHTTPClientHandler::ReOpenLink ( )
	{	
	CloseLink ( );
	TInt err = OpenLink ( );
	if ( err != KErrNone )
		{
		NotifyPendingTransactions ( err );
		CloseLink ( );		
		}
	}

// From MUPnPTimerObserver
void CHTTPClientHandler::TimeOut ()
	{	
	CloseLink ();
	}

// From MHttpDataSupplier
TBool CHTTPClientHandler::GetNextDataPart ( TPtrC8& aDataPart )
	{	
	CHTTPClientTransaction* ctrans = static_cast<CHTTPClientTransaction*> ( CurrentTransaction ( ) );
	ctrans->GetBodyPart( aDataPart );
	return ctrans->IsLastBodyPart( );
	}

void CHTTPClientHandler::ReleaseData ( )
	{	
	CHTTPClientTransaction* ctrans = static_cast<CHTTPClientTransaction*> ( CurrentTransaction ( ) );
	ctrans->RemoveBodyPart( );

	// Note! No need for checking NumBufs will crash if Empty, since removeBodyPart
	// will just increment the requestPartIdx

	// Call NotifyNewBodyData only if currentPartIdx != available MBuf
	TInt availableBufCount = ctrans->BodyParts( ).NumBufs ( );
	TInt currentPartId = ctrans->RequestPartIdx( );

	if ( currentPartId != availableBufCount )
		{
		iComposer->NotifyNewBodyData( );
		}
	else
		{
		// ..nothing to do, http message composer should say composing is concluded
		// or it waits for more data, which should be triggered by clientflow
		}
	}

TInt CHTTPClientHandler::OverallDataSize ( )
	{	
	CHTTPClientTransaction* ctrans = static_cast<CHTTPClientTransaction*> ( CurrentTransaction ( ) );

	// field or Content-Length
	TInt dataSize = MHttpMessageParserObserver::ENoBody;
	// Check is there a Content-Length field?
	CRequest* request = ctrans->Request ( );
	RStringF contentLength = request->StringPool ( ).StringF ( HTTP::EContentLength, THTTPTable::Table() );
	THTTPHdrVal value;
	TInt err = request->Handle( ).GetHeaderCollection ( ).GetField ( contentLength, 0, value );
	if ( err == KErrNone )
		{
		dataSize = value.Int ( );
		}
	return dataSize;
	}
