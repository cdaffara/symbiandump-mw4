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
#include <comms-infras/ss_thread.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include "ptcpservertests.h"
#include "upnp_cf_msgs.h"
#include "upnpflowbase.h"
#include "upnpflowfactory.h"
#include "CTcpServerTestCase.h"

#include <httperr.h>
#include "CTcpServerTestUPnP30.h"

using namespace Messages;

//------------------------------------------------------------//	
/////////////////// CTcpServerTestUPnP30 /////////////////////////
////// Testcase: Send the test data from the test client/////////
//------------------------------------------------------------//

_LIT8 ( KTestUPnPServiceUri,"http://127.0.0.1:80/testserviceuri1" );
_LIT8 ( KData, "M-POST /testserviceuri1 HTTP/1.1\r\nHOST: 127.0.0.1:80\r\nMAN: http://xmlsoap.org/soap/envelope/\r\n\r\n");
_LIT8 ( KExpectedResponse, "HTTP/1.1 400 Bad Request");

CTcpServerTestUPnP30* CTcpServerTestUPnP30::NewL ( CProxyProvd* aProvd )
	{
	CTcpServerTestUPnP30* ret = new ( ELeave ) CTcpServerTestUPnP30 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpServerTestUPnP30::CTcpServerTestUPnP30 ( CProxyProvd* aProvd ) :
	CTcpServerTestCase ( aProvd )
	{
	}
	
CTcpServerTestUPnP30::~CTcpServerTestUPnP30 ()
	{

	}
	
void CTcpServerTestUPnP30::ConstructL ()
	{
	CTcpServerTestCase::ConstructL(KTestUPnPServiceUri);
	}

TVerdict CTcpServerTestUPnP30::RunTestL ()
	{
	switch ( iState )
		{
			
		case ESendData:
			{			
			iSendChain.CreateL ( KData );
			iSocketHandler.Send ( iSendChain );
			
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
					
		default:
			{
			return CTcpServerTestCase::RunTestL();
			}
		}
	}
	
	
void CTcpServerTestUPnP30::RecvComplete ( RMBufChain& aData )
	{
	RBuf8 responseBuf;
	responseBuf.CreateMax ( aData.Length () );
	aData.CopyOut ( responseBuf );
	aData.Free ();
	
	if ( responseBuf.FindF ( KExpectedResponse ) == KErrNotFound )
		{
		iResponse = EFail; // test case failed
		}
	else
		{
		iResponse = EPass;	// test case passed
		}
	
	responseBuf.Close ();
	CompleteSelf ( KErrNone );
	}


/////////////////////////////////////////////////////////////////////////////////////////

_LIT8 ( KData1, "GET1 /testserviceuri1 HTTP/1.1\r\nHOST: 127.0");
_LIT8 ( KData2,".0.1:80\r\n\r\n");
_LIT8 ( KResponse, "HTTP/1.1 501 Not Implemented");

CTcpServerTestUPnP38* CTcpServerTestUPnP38::NewL ( CProxyProvd* aProvd )
	{
	CTcpServerTestUPnP38* ret = new ( ELeave ) CTcpServerTestUPnP38 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpServerTestUPnP38::CTcpServerTestUPnP38 ( CProxyProvd* aProvd ) :
	CTcpServerTestCase ( aProvd )
	{
	}
	
CTcpServerTestUPnP38::~CTcpServerTestUPnP38 ()
	{

	}
	
void CTcpServerTestUPnP38::ConstructL ()
	{
	CTcpServerTestCase::ConstructL(KTestUPnPServiceUri);
	}

TVerdict CTcpServerTestUPnP38::RunTestL ()
	{
	switch ( iState )
		{
			
		case ESendData:
			{			
			iSendChain.CreateL ( KData1 );
			iSocketHandler.Send ( iSendChain );
			
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
					
		default:
			{
			return CTcpServerTestCase::RunTestL();
			}
		}
	}
	
	
void CTcpServerTestUPnP38::RecvComplete ( RMBufChain& aData )
	{
	RBuf8 responseBuf;
	responseBuf.CreateMax ( aData.Length () );
	aData.CopyOut ( responseBuf );
	aData.Free ();
	
	if ( responseBuf.FindF ( KResponse ) == KErrNotFound )
		{
		iResponse = EFail; // test case failed
		}
	else
		{
		iResponse = EPass;	// test case passed
		}
	
	responseBuf.Close ();
	CompleteSelf ( KErrNone );
	}

void CTcpServerTestUPnP38::SendComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();
	
	if(!iSentOnce)
		{
		TRAP_IGNORE(iSendChain.CreateL ( KData2 ));
		iSocketHandler.Send ( iSendChain );
		iSentOnce = ETrue;
		}
	else
		{
		iSocketHandler.Recv ();
		//iTimer.After ( iStatus, 1000000 ); //1secs
		}
	}



//------------------------------------------------------------//	
/////////////////// CTcpServerTestUPnP39 /////////////////////////
////// Testcase: Send the test data from the test client/////////
//------------------------------------------------------------//

_LIT8 ( KData3, "GET /testserviceuri1 HTTP/1.1\r\nHOST: 127.0.0.1:80\r\nContent-Length: 0\r\n\r\n");
_LIT8 ( KData31, "CONNECT /testserviceuri1 HTTP/1.1\r\nHOST: 127.0.0.1:80\r\nContent-Length: 0\r\n\r\n");
_LIT8 ( KExpectedResponse3, "HTTP/1.1 200 OK");
_LIT8 ( KExpectedResponse31, "HTTP/1.1 501 Not Implemented");
_LIT8 ( KResponseData, "Test Server");

CTcpServerTestUPnP39* CTcpServerTestUPnP39::NewL ( CProxyProvd* aProvd )
	{
	CTcpServerTestUPnP39* ret = new ( ELeave ) CTcpServerTestUPnP39 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpServerTestUPnP39::CTcpServerTestUPnP39 ( CProxyProvd* aProvd ) :
	CTcpServerTestCase ( aProvd )
	{
	}
	
CTcpServerTestUPnP39::~CTcpServerTestUPnP39 ()
	{

	}
	
void CTcpServerTestUPnP39::ConstructL ()
	{
	CTcpServerTestCase::ConstructL(KTestUPnPServiceUri);
	}

TVerdict CTcpServerTestUPnP39::RunTestL ()
	{
	switch ( iState )
		{
			
		case ESendData:
			{			
			iSendChain.CreateL ( KData3 );
			iSocketHandler.Send ( iSendChain );
			
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
					
		default:
			{
			return CTcpServerTestCase::RunTestL();
			}
		}
	}
	
	
void CTcpServerTestUPnP39::RecvComplete ( RMBufChain& aData )
	{
	RBuf8 responseBuf;
	responseBuf.CreateMax ( aData.Length () );
	aData.CopyOut ( responseBuf );
	aData.Free ();
	
	if ( responseBuf.FindF ( KExpectedResponse3 ) != KErrNotFound )
		{
		iSocketHandler.Recv ();
		responseBuf.Close ();
		return;
		}
	if ( responseBuf.FindF ( KResponseData ) != KErrNotFound )
		{
		TRAP_IGNORE(iSendChain.CreateL ( KData31 ));
		iSocketHandler.Send ( iSendChain );
		responseBuf.Close ();
		return;
		}
	if ( responseBuf.FindF ( KExpectedResponse31 ) != KErrNotFound )
		{
		iResponse = EPass;	// test case Passed
		}
	else
		{
		iResponse = EFail;	// test case failed
		}
	
	responseBuf.Close ();
	CompleteSelf ( KErrNone );
	}

void CTcpServerTestUPnP39::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iTcpServer = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPRequestInfo> ())
		{
		TUpnpMessage::TUPnPRequestInfo& requestMsg = message_cast< TUpnpMessage::TUPnPRequestInfo > ( aMessage );
		CTransaction* trans = static_cast<CTransaction*> ( requestMsg.iPtr );
		RMBufChain bufChain;
		bufChain.CreateL ( KResponseData );	
		trans->AddBodyPart ( bufChain );
		
		TUpnpMessage::TUPnPResponseInfo msg ( trans, HTTPStatus::EOk, NULL );
		RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iTcpServer ), msg );
		}
	}


//------------------------------------------------------------//	
/////////////////// CTcpServerTestUPnP40 /////////////////////////
////// Testcase: Send the test data from the test client/////////
//------------------------------------------------------------//

_LIT8 ( KData32, "GET /testserviceuri1 HTTP/1.1\r\nContent-Length: 0\r\n\r\n");
_LIT8 ( KExpectedResponse32, "HTTP/1.1 400 Bad Request");

CTcpServerTestUPnP40* CTcpServerTestUPnP40::NewL ( CProxyProvd* aProvd )
	{
	CTcpServerTestUPnP40* ret = new ( ELeave ) CTcpServerTestUPnP40 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpServerTestUPnP40::CTcpServerTestUPnP40 ( CProxyProvd* aProvd ) :
	CTcpServerTestCase ( aProvd )
	{
	}
	
CTcpServerTestUPnP40::~CTcpServerTestUPnP40 ()
	{

	}
	
void CTcpServerTestUPnP40::ConstructL ()
	{
	CTcpServerTestCase::ConstructL(KTestUPnPServiceUri);
	}

TVerdict CTcpServerTestUPnP40::RunTestL ()
	{
	switch ( iState )
		{
			
		case ESendData:
			{			
			iSendChain.CreateL ( KData3 );
			iSocketHandler.Send ( iSendChain );
			
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
					
		default:
			{
			return CTcpServerTestCase::RunTestL();
			}
		}
	}
	
	
void CTcpServerTestUPnP40::RecvComplete ( RMBufChain& aData )
	{
	RBuf8 responseBuf;
	responseBuf.CreateMax ( aData.Length () );
	aData.CopyOut ( responseBuf );
	aData.Free ();
	
	if ( responseBuf.FindF ( KExpectedResponse3 ) != KErrNotFound )
		{
		iSocketHandler.Recv ();
		responseBuf.Close ();
		return;
		}
	if ( responseBuf.FindF ( KResponseData ) != KErrNotFound )
		{
		TRAP_IGNORE(iSendChain.CreateL ( KData32 ));
		iSocketHandler.Send ( iSendChain );
		responseBuf.Close ();
		return;
		}
	if ( responseBuf.FindF ( KExpectedResponse32 ) != KErrNotFound )
		{
		iResponse = EPass;	// test case Passed
		}
	else
		{
		iResponse = EFail;	// test case failed
		}
	
	responseBuf.Close ();
	CompleteSelf ( KErrNone );
	}

void CTcpServerTestUPnP40::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iTcpServer = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPRequestInfo> ())
		{
		TUpnpMessage::TUPnPRequestInfo& requestMsg = message_cast< TUpnpMessage::TUPnPRequestInfo > ( aMessage );
		CTransaction* trans = static_cast<CTransaction*> ( requestMsg.iPtr );
		RMBufChain bufChain;
		bufChain.CreateL ( KResponseData );	
		trans->AddBodyPart ( bufChain );
		
		TUpnpMessage::TUPnPResponseInfo msg ( trans, HTTPStatus::EOk, NULL );
		RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iTcpServer ), msg );
		}
	}










