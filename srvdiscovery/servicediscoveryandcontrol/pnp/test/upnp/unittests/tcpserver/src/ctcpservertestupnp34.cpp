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
#include "CTcpServerTestUPnP34.h"

using namespace Messages;

//------------------------------------------------------------//	
/////////////////// CTcpServerTestUPnP34 /////////////////////////
////// Testcase: Send the test data from the test client/////////
//------------------------------------------------------------//

_LIT8 ( KTestUPnPServiceUri,"http://127.0.0.1:80/testserviceuri1" );
_LIT8 ( KData, "UNSUBSCRIBE /testserviceuri1 HTTP/1.1\r\nHOST: 127.0.0.1:80\r\nSID: testserviceuri1\r\nNT: upnp:event\r\n\r\n");
_LIT8 ( KExpectedResponse, "HTTP/1.1 400 Bad Request");

CTcpServerTestUPnP34* CTcpServerTestUPnP34::NewL ( CProxyProvd* aProvd )
	{
	CTcpServerTestUPnP34* ret = new ( ELeave ) CTcpServerTestUPnP34 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpServerTestUPnP34::CTcpServerTestUPnP34 ( CProxyProvd* aProvd ) :
	CTcpServerTestCase ( aProvd )
	{
	}
	
CTcpServerTestUPnP34::~CTcpServerTestUPnP34 ()
	{

	}
	
void CTcpServerTestUPnP34::ConstructL ()
	{
	CTcpServerTestCase::ConstructL(KTestUPnPServiceUri);
	}

TVerdict CTcpServerTestUPnP34::RunTestL ()
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
	
	
void CTcpServerTestUPnP34::RecvComplete ( RMBufChain& aData )
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

//------------------------------------------------------------------------------------
_LIT8 ( KTestUPnPServiceUri1,"http://127.0.0.1:80/testserviceuri1" );
_LIT8 ( KPartialData, "POST /testserviceuri1 HTTP/1.1\r\nHOST: 127.0.0.1:80\r\nContent-Length: 10\r\n\r\n");

CTcpServerTestUPnP35* CTcpServerTestUPnP35::NewL ( CProxyProvd* aProvd )
	{
	CTcpServerTestUPnP35* ret = new ( ELeave ) CTcpServerTestUPnP35 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpServerTestUPnP35::CTcpServerTestUPnP35 ( CProxyProvd* aProvd ) :
	CTcpServerTestCase ( aProvd )
	{
	}
	
CTcpServerTestUPnP35::~CTcpServerTestUPnP35 ()
	{
	}
	
void CTcpServerTestUPnP35::ConstructL ()
	{
	CTcpServerTestCase::ConstructL(KTestUPnPServiceUri1);
	}

TVerdict CTcpServerTestUPnP35::RunTestL ()
	{
	switch ( iState )
		{			
		case ESendData:
			{			
			//iSendChain.CreateL ( KData1 );
			iSendChain.CreateL ( KPartialData );
			iSocketHandler.Send ( iSendChain );
			
			iState = EStartTimer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		case EStartTimer:
			{
			const TUint KTimeOutVal = 31000000; // 31secs
			iTimer.After ( iStatus, KTimeOutVal );
			iState = ETimerStopped;
			Reschedule ();
			return EPass;
			}
		case ETimerStopped:
			{
			iClientSocket.Close ();			
			iState = ESendStopServer;			
			CTcpServerTestCase::RunTestL ();
			return EPass;
			}
		default:
			{
			return CTcpServerTestCase::RunTestL();
			}
		}
	}

void CTcpServerTestUPnP35::SendComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();
	CompleteSelf ( KErrNone );
	}
	
void CTcpServerTestUPnP35::RecvComplete ( RMBufChain& /*aData*/ )
	{
	// will not reach here, since after sending partial data & socket is closed
	}
	
//------------------------------------------------------------------------------------
// KTestUPnPServiceUri1 -- same as above
// KPartialData -- same as above
_LIT8 ( KExpectedResponse1, "HTTP/1.1 408 Request Timeout");

CTcpServerTestUPnP36* CTcpServerTestUPnP36::NewL ( CProxyProvd* aProvd )
	{
	CTcpServerTestUPnP36* ret = new ( ELeave ) CTcpServerTestUPnP36 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpServerTestUPnP36::CTcpServerTestUPnP36 ( CProxyProvd* aProvd ) :
	CTcpServerTestCase ( aProvd )
	{
	}
	
CTcpServerTestUPnP36::~CTcpServerTestUPnP36 ()
	{
	}
	
void CTcpServerTestUPnP36::ConstructL ()
	{
	CTcpServerTestCase::ConstructL(KTestUPnPServiceUri1);
	}

TVerdict CTcpServerTestUPnP36::RunTestL ()
	{
	switch ( iState )
		{			
		case ESendData:
			{			
			iSendChain.CreateL ( KPartialData );
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

void CTcpServerTestUPnP36::RecvComplete ( RMBufChain& aData )
	{
	// will reach here, after sending partial, and should 408 status code
	RBuf8 responseBuf;
	responseBuf.CreateMax ( aData.Length () );
	aData.CopyOut ( responseBuf );
	aData.Free ();
	
	if ( responseBuf.FindF ( KExpectedResponse1 ) == KErrNotFound )
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
	
//------------------------------------------------------------------------------------
// KTestUPnPServiceUri1 -- same as above
_LIT8 ( KData1, "GET /testserviceuri1 HTTP/1.1\r\nHOST: 127.0.0.1:80\r\nContent-Length: 0\r\nConnection: close\r\n\r\n" );
// KExpectedResponse1,-- same above
_LIT8 ( KBodyData, "ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppvppppppppppppppppppppppppppppppppppppvppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppvppppppppppppppppppppppppppppppppppppvppppppppppppppppppppppppppppppppppppvvppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppvvppppppppppppppppppppppppppppppppppppvvppppppppppppppppppppppppppppppppppppvpppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp" );

CTcpServerTestUPnP37* CTcpServerTestUPnP37::NewL ( CProxyProvd* aProvd )
	{
	CTcpServerTestUPnP37* ret = new ( ELeave ) CTcpServerTestUPnP37 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpServerTestUPnP37::CTcpServerTestUPnP37 ( CProxyProvd* aProvd ) :
	CTcpServerTestCase ( aProvd )
	{
	}
	
CTcpServerTestUPnP37::~CTcpServerTestUPnP37 ()
	{
	iResponseBuf.Close ();
	}
	
void CTcpServerTestUPnP37::ConstructL ()
	{
	CTcpServerTestCase::ConstructL(KTestUPnPServiceUri1);
	iUPnPTimer = CUPnPTimer::NewL ( *this );
	}

TVerdict CTcpServerTestUPnP37::RunTestL ()
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

void CTcpServerTestUPnP37::RecvComplete ( RMBufChain& aData )
	{
	RBuf8 partialBuf;
	partialBuf.CreateMax ( aData.Length () );
	aData.CopyOut ( partialBuf );
	aData.Free ();
	
	_LIT8 ( KContentLength, "\r\n\r\n" );
	if ( partialBuf.Find ( KContentLength ) != KErrNotFound )
		{	
		TRAP_IGNORE(iResponseBuf.CreateL ( KBodyData ().Length () ));		
		}
	else
		{
		iResponseBuf.Append ( partialBuf );
		}
	partialBuf.Close ();
	
	if ( iResponseBuf.Length () != KBodyData ().Length () )
		{
		iSocketHandler.Recv ();
		}
	else
		{
		CompleteSelf ( KErrNone );
		}
	}
	
void CTcpServerTestUPnP37::ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TUpnpMessage::TUPnPRequestInfo> () )
		{
		TUpnpMessage::TUPnPRequestInfo& requestMsg = message_cast< TUpnpMessage::TUPnPRequestInfo > ( aMessage );
		iTransaction = static_cast<CTransaction*> ( requestMsg.iPtr );
		const TUint KTimeOutVal = 30000000; // 30secs
		iUPnPTimer->StartTimer ( KTimeOutVal );
		}
	else
		{
		CTcpServerTestCase::ReceivedL ( aSender, aRecipient,aMessage );
		}
	}

void CTcpServerTestUPnP37::TimeOut ()
	{
	SendResponseToServerFlow ();
	}

void CTcpServerTestUPnP37::SendResponseToServerFlow ()
	{
	RMBufChain dataChian;
	TRAP_IGNORE(dataChian.CreateL( KBodyData ));
	iTransaction->AddBodyPart( dataChian );
	RClientInterface::OpenPostMessageClose ( NodeId (), iTcpServer,
								TUpnpMessage::TUPnPResponseInfo ( iTransaction, HTTPStatus::EOk, NULL ).CRef () );
	}









