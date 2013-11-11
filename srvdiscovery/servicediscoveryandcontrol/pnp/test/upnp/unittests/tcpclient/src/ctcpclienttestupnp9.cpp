/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include <ss_glob.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <httpstringconstants.h>

#include "ptcpclienttests.h"	
#include "upnp_cf_msgs.h"
#include "upnpflowbase.h"
#include "httpclientflow.h"
#include "upnpflowfactory.h"

#include <http/framework/cheadercodecplugin.h>
#include <upnp/tupnptable.h>

#include "CTcpClientTestUPnP9.h"
#include "upnpserverconstants.h"

//const TUint16 KActivityNull = 0;
//const TUint KHttpDefaultPort = 80;


const TUint KTransactionCount = 1;
_LIT8 ( KTxtRawRequest, "POST / HTTP/1.1\r\nHost: 127.0.0.1:80\r\nExpect: 100-Continue\r\nContent-Length: 330\r\n\r\n<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:QueryStateVariableResponse xmlns:u=\"urn:shemas-upnp-org:control-1-0\"><return>Query return value</return></u:QueryStateVariableResponse></s:Body></s:Envelope>" );
_LIT8 ( KTxtRawResponse, "HTTP/1.1 200 OK\r\nContent-Length: 330\r\n\r\n<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:QueryStateVariableResponse xmlns:u=\"urn:shemas-upnp-org:control-1-0\"><return>Query return value</return></u:QueryStateVariableResponse></s:Body></s:Envelope>\r\n\r\n" );
_LIT8 ( KBodyData, "<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:QueryStateVariableResponse xmlns:u=\"urn:shemas-upnp-org:control-1-0\"><return>Query return value</return></u:QueryStateVariableResponse></s:Body></s:Envelope>" );

CTcpClientTestUPnP9* CTcpClientTestUPnP9::NewL ( CProxyProvd* aProvd )
	{
	CTcpClientTestUPnP9* ret = new ( ELeave ) CTcpClientTestUPnP9 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpClientTestUPnP9::CTcpClientTestUPnP9 ( CProxyProvd* aProvd ) :
	CTcpClientTestBase(aProvd), iTestServer(NULL)
	{
	}

CTcpClientTestUPnP9::~CTcpClientTestUPnP9 ()
	{	
	iTimer.Close ();
	delete iCodec;
	iStringPool.Close();
	delete iClientHandler;
	delete iTransaction;
	}

void CTcpClientTestUPnP9::ConstructL()
	{	
	iTimer.CreateLocal ();
	iStringPool.OpenL( TUPnPTable::Table() );
	iStringPool.OpenL(THTTPTable::Table());
	iCodec = CHeaderCodecPlugin::NewL ( KUPnPCodecName, iStringPool );			
	}

TVerdict CTcpClientTestUPnP9::RunTestL()
	{
	switch ( iState )
		{
		case ECreateTestServer:
			{
			iLogger.WriteFormat(_L("<i>Creating TestServer..... </i>"));
			
			iTestServer = CTestTcpServer::NewL ( *this );
			
			iState  = ECreateTestClient;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		
		case ECreateTestClient:
			{
			iLogger.WriteFormat(_L("<i>TestServer is created..... </i>"));			
			iLogger.WriteFormat(_L("<i>Creating TestClient..... </i>"));
			
			
			
			TInetAddr	remoteAddress( KInetAddrLoop, 80 );
			iClientHandler = CHTTPClientHandler::NewL ( *this, iChunkManager, &remoteAddress );
			iTransaction = PrepareTransactionL ();
			iClientHandler->SubmitTransaction( iTransaction );
					
			iState  = ESendRequestData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		
		case ESendRequestData:
			{
			return EPass;
			}

		case ECleanup:
			{
			delete iTestServer;
			
			// cleanup tcp client flow
			//delete reinterpret_cast<CHttpClientFlow*> ( iClientId.Ptr () );
			iTimer.After ( iStatus, 60000000 ); //10 secs
			iState = EComplete;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		case EComplete:
			{
			iLogger.WriteFormat(_L("<i>TestCase: Complete..... </i>"));
			return EPass;
			}
			
		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
	
CHTTPClientTransaction* CTcpClientTestUPnP9::PrepareTransactionL ()
	{	
	TNodeCtxId channelId ( MeshMachine::KActivityNull, TNodeId::NullId() );

	CHTTPClientTransaction* ctrans = CHTTPClientTransaction::NewL ( *iCodec, iStringPool, channelId );
	
	CRequest* crequest = ctrans->Request( );
	RHTTPHeaders headers = crequest->Handle( ).GetHeaderCollection ( );
	RRequest rrequest = crequest->Handle( );

	// Set request method
	RStringF stringF = iStringPool.StringF ( HTTP::EPOST, THTTPTable::Table() );
	rrequest.SetMethod ( stringF );

	// Set request Uri	
	TUriParser8 uriPath;
	uriPath.Parse ( _L8 ("/") );
	rrequest.SetURIL ( uriPath );	
	
	RStringF hostValStr = iStringPool.OpenFStringL( _L8("127.0.0.1:80"));
	THTTPHdrVal hostVal ( hostValStr );
	headers.SetFieldL ( iStringPool.StringF ( HTTP::EHost, THTTPTable::Table() ), hostVal );
	hostValStr.Close ();
	
	//Set Content Length
	THTTPHdrVal hdrVal ( 330 );
	headers.SetFieldL ( iStringPool.StringF ( HTTP::EContentLength, THTTPTable::Table() ), hdrVal );

	stringF = iStringPool.StringF ( HTTP::E100Continue, THTTPTable::Table() );
	
	// 100-continue    
	THTTPHdrVal hdrStr ( stringF );
	headers.SetFieldL ( iStringPool.StringF ( HTTP::EExpect, THTTPTable::Table() ), hdrStr );
	
	// Set Body Data	
	RMBufChain buf;
	buf.CreateL( KBodyData);	
	ctrans->AddBodyPart( buf );
	ctrans->SetComplete ();

	stringF.Close ();
	return ctrans;	
	}
	
	
void CTcpClientTestUPnP9::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}

MTcpClientTestCase* CTcpClientTestUPnP9::GetTestCase ()
	{
	return this;
	}
	
CTestExecuteLogger& CTcpClientTestUPnP9::GetLogger ( )
	{
	return iLogger;
	}

	
const TDesC& CTcpClientTestUPnP9::TestCaseName () const
	{
	_LIT ( KTxtTitle, "Defect Fix CINC077703More" );
	return KTxtTitle ();
	}
	
TInt CTcpClientTestUPnP9::TotalTransactionCount () const
	{
	return KTransactionCount;
	}

const TDesC8& CTcpClientTestUPnP9::GetRawRequest ( TInt aTransIndex )
	{
	__ASSERT_ALWAYS ( aTransIndex < KTransactionCount, User::Invariant () );
	return KTxtRawRequest ();
	}
	
const TDesC8& CTcpClientTestUPnP9::GetRawResponse ( TInt aTransIndex )
	{
	__ASSERT_ALWAYS( aTransIndex < KTransactionCount, User::Invariant () );
	return KTxtRawResponse ();
	}

TInt CTcpClientTestUPnP9::TransactionCount () const
	{ 
	return KTransactionCount; 
	}


TInt CTcpClientTestUPnP9::OnHttpEvent ( CTransaction* /*aTransaction*/, THTTPEvent& /*aEvent*/ )
	{
	return KErrNone;
	}


/////////////////////////////////////////////////////////////////////////////////////////////////

_LIT8 ( KTxtRawRequest1, "GET / HTTP/1.1\r\nHost: 127.0.0.1:80\r\n\r\n" );
_LIT8 ( KTxtRawResponse1, "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n" );

CTcpClientTestUPnP16* CTcpClientTestUPnP16::NewL ( CProxyProvd* aProvd )
	{
	CTcpClientTestUPnP16* ret = new ( ELeave ) CTcpClientTestUPnP16 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpClientTestUPnP16::CTcpClientTestUPnP16 ( CProxyProvd* aProvd ) :
	CTcpClientTestBase(aProvd), iTestServer(NULL)
	{
	}

CTcpClientTestUPnP16::~CTcpClientTestUPnP16 ()
	{	
	iTimer.Close ();
	delete iCodec;
	iStringPool.Close();
	delete iTransaction;
	delete iTestServer;
	delete iClientHandler;
	}

void CTcpClientTestUPnP16::ConstructL()
	{		
	iTimer.CreateLocal ();
	iStringPool.OpenL( TUPnPTable::Table() );
	iStringPool.OpenL(THTTPTable::Table());
	iCodec = CHeaderCodecPlugin::NewL ( KUPnPCodecName, iStringPool );	
	}

TVerdict CTcpClientTestUPnP16::RunTestL()
	{
	switch ( iState )
		{
		case ECreateTestServer:
			{
			iLogger.WriteFormat(_L("<i>Creating TestServer..... </i>"));
			
			iTestServer = CTestTcpServer::NewL ( *this );
			
			iState  = ECreateTestClient;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		
		case ECreateTestClient:
			{
			iLogger.WriteFormat(_L("<i>TestServer is created..... </i>"));			
			iLogger.WriteFormat(_L("<i>Creating TestClient..... </i>"));
					
			
			TInetAddr	remoteAddress( KInetAddrLoop, 80 );
			iClientHandler = CHTTPClientHandler::NewL ( *this, iChunkManager, &remoteAddress );
			iClientHandler->SetConnectionInfo ( remoteAddress );
			iTransaction = PrepareTransactionL ();
			iClientHandler->SubmitTransaction( iTransaction );
								
			iState  = ESendRequestData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		
		case ESendRequestData:
			{
			iClientHandler->SubmitTransaction( iTransaction );
			iClientHandler->CancelTransaction( iTransaction );
			
			delete iClientHandler;
			
			TInetAddr	remoteAddress( KInetAddrLoop, 80 );
			iClientHandler = CHTTPClientHandler::NewL ( *this, iChunkManager, &remoteAddress );			
			iTransaction->SetComposingStarted ( EFalse );
			iClientHandler->SubmitTransaction( iTransaction );
			iClientHandler->CancelTransaction( iTransaction );
			
			iTimer.After ( iStatus, 60000000 ); //10 secs
			
			iState  = EComplete;			
			Reschedule();
			
			return EPass;
			}

		case EComplete:
			{
			iLogger.WriteFormat(_L("<i>TestCase: Complete..... </i>"));
			return EPass;
			}
			
		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
	
CHTTPClientTransaction* CTcpClientTestUPnP16::PrepareTransactionL ()
	{	
	TNodeCtxId channelId ( MeshMachine::KActivityNull, TNodeId::NullId() );

	CHTTPClientTransaction* ctrans = CHTTPClientTransaction::NewL ( *iCodec, iStringPool, channelId );
	
	CRequest* crequest = ctrans->Request( );
	RHTTPHeaders headers = crequest->Handle( ).GetHeaderCollection ( );
	RRequest rrequest = crequest->Handle( );

	// Set request method
	RStringF stringF = iStringPool.StringF ( HTTP::EGET, THTTPTable::Table() );
	rrequest.SetMethod ( stringF );

	// Set request Uri	
	TUriParser8 uriPath;
	uriPath.Parse ( _L8 ("/") );
	rrequest.SetURIL ( uriPath );	
	
	RStringF hostValStr = iStringPool.OpenFStringL( _L8("127.0.0.1:80"));
	THTTPHdrVal hostVal ( hostValStr );
	headers.SetFieldL ( iStringPool.StringF ( HTTP::EHost, THTTPTable::Table() ), hostVal );
	hostValStr.Close ();
	
	ctrans->SetComplete ();

	stringF.Close ();
	return ctrans;	
	}
	
	
void CTcpClientTestUPnP16::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}

MTcpClientTestCase* CTcpClientTestUPnP16::GetTestCase ()
	{
	return this;
	}
	
CTestExecuteLogger& CTcpClientTestUPnP16::GetLogger ( )
	{
	return iLogger;
	}

	
const TDesC& CTcpClientTestUPnP16::TestCaseName () const
	{
	_LIT ( KTxtTitle, "Defect Fix CINC077703More" );
	return KTxtTitle ();
	}
	
TInt CTcpClientTestUPnP16::TotalTransactionCount () const
	{
	return KTransactionCount;
	}

const TDesC8& CTcpClientTestUPnP16::GetRawRequest ( TInt aTransIndex )
	{
	__ASSERT_ALWAYS ( aTransIndex < KTransactionCount, User::Invariant () );
	return KTxtRawRequest1 ();
	}
	
const TDesC8& CTcpClientTestUPnP16::GetRawResponse ( TInt aTransIndex )
	{
	__ASSERT_ALWAYS( aTransIndex < KTransactionCount, User::Invariant () );
	return KTxtRawResponse1 ();
	}

TInt CTcpClientTestUPnP16::TransactionCount () const
	{ 
	return KTransactionCount; 
	}


TInt CTcpClientTestUPnP16::OnHttpEvent ( CTransaction* /*aTransaction*/, THTTPEvent& aEvent )
	{
	if ( aEvent.iStatus == THTTPEvent::EResponseComplete )		
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aEvent.iStatus < 0 )
		{
		CompleteSelf ( aEvent.iStatus );
		}
	return KErrNone;
	}


/////////////////////////////////////////////////////////////////////////////////////////////////

_LIT8 ( KBodyData1, "12345678901234567890" );

CTcpClientTestUPnP17* CTcpClientTestUPnP17::NewL ( CProxyProvd* aProvd )
	{
	CTcpClientTestUPnP17* ret = new ( ELeave ) CTcpClientTestUPnP17 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpClientTestUPnP17::CTcpClientTestUPnP17 ( CProxyProvd* aProvd ) :
	CTcpClientTestBase(aProvd), iTestServer(NULL)
	{
	}

CTcpClientTestUPnP17::~CTcpClientTestUPnP17 ()
	{	
	iTimer.Close ();
	delete iCodec;
	iStringPool.Close();
	delete iTransaction;
	delete iTestServer;
	delete iClientHandler;
	}

void CTcpClientTestUPnP17::ConstructL()
	{		
	iTimer.CreateLocal ();
	iStringPool.OpenL( TUPnPTable::Table() );
	iStringPool.OpenL(THTTPTable::Table());
	iCodec = CHeaderCodecPlugin::NewL ( KUPnPCodecName, iStringPool );	
	}

TVerdict CTcpClientTestUPnP17::RunTestL()
	{
	switch ( iState )
		{
		case ECreateTestServer:
			{
			iLogger.WriteFormat(_L("<i>Creating TestServer..... </i>"));
			
			iTestServer = CTestTcpServer::NewL ( *this );
			
			iState  = ECreateTestClient;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		
		case ECreateTestClient:
			{
			iLogger.WriteFormat(_L("<i>TestServer is created..... </i>"));			
			iLogger.WriteFormat(_L("<i>Creating TestClient..... </i>"));
					
			
			TInetAddr	remoteAddress( KInetAddrLoop, 80 );
			iClientHandler = CHTTPClientHandler::NewL ( *this, iChunkManager, &remoteAddress );
			iClientHandler->SetConnectionInfo ( remoteAddress );
			iTransaction = PrepareTransactionL ();
			iClientHandler->SubmitTransaction( iTransaction );
								
			iState  = EComplete;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		
		case EComplete:
			{
			iLogger.WriteFormat(_L("<i>TestCase: Complete..... </i>"));
			return EPass;
			}
			
		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
	
CHTTPClientTransaction* CTcpClientTestUPnP17::PrepareTransactionL ()
	{	
	TNodeCtxId channelId ( MeshMachine::KActivityNull, TNodeId::NullId() );

	CHTTPClientTransaction* ctrans = CHTTPClientTransaction::NewL ( *iCodec, iStringPool, channelId );
	
	CRequest* crequest = ctrans->Request( );
	RHTTPHeaders headers = crequest->Handle( ).GetHeaderCollection ( );
	RRequest rrequest = crequest->Handle( );

	// Set request method
	RStringF stringF = iStringPool.StringF ( HTTP::EGET, THTTPTable::Table() );
	rrequest.SetMethod ( stringF );

	// Set request Uri	
	TUriParser8 uriPath;
	uriPath.Parse ( _L8 ("/") );
	rrequest.SetURIL ( uriPath );	
	
	RStringF hostValStr = iStringPool.OpenFStringL( _L8("127.0.0.1:80"));
	THTTPHdrVal hostVal ( hostValStr );
	headers.SetFieldL ( iStringPool.StringF ( HTTP::EHost, THTTPTable::Table() ), hostVal );
	hostValStr.Close ();

	//Set Content Length
	THTTPHdrVal hdrVal ( 10 );
	headers.SetFieldL ( iStringPool.StringF ( HTTP::EContentLength, THTTPTable::Table() ), hdrVal );

	stringF = iStringPool.StringF ( HTTP::E100Continue, THTTPTable::Table() );

	// Set Body Data	
	RMBufChain buf;
	buf.CreateL( KBodyData1);	
	ctrans->AddBodyPart( buf );
	ctrans->SetComplete ();
		
	stringF.Close ();
	return ctrans;	
	}
	
	
void CTcpClientTestUPnP17::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}

MTcpClientTestCase* CTcpClientTestUPnP17::GetTestCase ()
	{
	return this;
	}
	
CTestExecuteLogger& CTcpClientTestUPnP17::GetLogger ( )
	{
	return iLogger;
	}

	
const TDesC& CTcpClientTestUPnP17::TestCaseName () const
	{
	_LIT ( KTxtTitle, "CTcpClientTestUPnP17" );
	return KTxtTitle ();
	}
	
TInt CTcpClientTestUPnP17::TotalTransactionCount () const
	{
	return KTransactionCount;
	}

const TDesC8& CTcpClientTestUPnP17::GetRawRequest ( TInt aTransIndex )
	{
	__ASSERT_ALWAYS ( aTransIndex < KTransactionCount, User::Invariant () );
	return KTxtRawRequest1 ();
	}
	
const TDesC8& CTcpClientTestUPnP17::GetRawResponse ( TInt aTransIndex )
	{
	__ASSERT_ALWAYS( aTransIndex < KTransactionCount, User::Invariant () );
	return KTxtRawResponse1 ();
	}

TInt CTcpClientTestUPnP17::TransactionCount () const
	{ 
	return KTransactionCount; 
	}


TInt CTcpClientTestUPnP17::OnHttpEvent ( CTransaction* /*aTransaction*/, THTTPEvent& aEvent )
	{
	if ( aEvent.iStatus == THTTPEvent::EResponseComplete )		
		{
		CompleteSelf ( KErrNone );		
		}
	else if ( aEvent.iStatus < 0 )
		{
		CompleteSelf ( aEvent.iStatus );
		}
	return KErrNone;
	}

