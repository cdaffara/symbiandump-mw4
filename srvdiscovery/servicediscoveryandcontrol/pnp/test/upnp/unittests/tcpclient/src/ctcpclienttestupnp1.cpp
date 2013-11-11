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
#include "ctcpclienttestupnp1.h"

const TUint16 KActivityNull = 0;
const TUint KHttpDefaultPort = 80;


const TUint KTransactionCount = 1;
_LIT8 ( KTxtRawRequest, "GET / HTTP/1.1\r\nHost: [0:0:0:0:0:0:0:1]\r\nContent-Length: 0\r\n\r\n" );
_LIT8 ( KTxtRawResponse1, "HTTP/1.0 200 OK\r\nContent-Length: 0\r\n\r\n" );


CTcpClientTestUPnP1* CTcpClientTestUPnP1::NewL ( CProxyProvd* aProvd )
	{
	CTcpClientTestUPnP1* ret = new ( ELeave ) CTcpClientTestUPnP1 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpClientTestUPnP1::CTcpClientTestUPnP1 ( CProxyProvd* aProvd ) :
	CTcpClientTestBase(aProvd), iTestServer(NULL)
	{
	}

CTcpClientTestUPnP1::~CTcpClientTestUPnP1 ()
	{	
	iTimer.Close ();
	}

void CTcpClientTestUPnP1::ConstructL()
	{	
	iTimer.CreateLocal ();
	}

TVerdict CTcpClientTestUPnP1::RunTestL()
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
			
			THttpClientFlowQuery flowQuery ( TAppProtAddr ( KInetAddrLoop, KHttpDefaultPort ), Id (), EHttpClientFlow, THttpClientFlowQuery::ECreateNew, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
						
			RClientInterface::OpenPostMessageClose ( NodeId (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = ESendRequestData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		
		case ESendRequestData:
			{
			iLogger.WriteFormat(_L("<i>Client is Created</i>"));
			iLogger.WriteFormat(_L("<i>Send data..... </i>"));
			
			RMemChunk bodyBuf;
			RMemChunk uriBuf;
			//uriBuf.CreateL ( _L8 ("http://127.0.0.1:9999") );
			uriBuf.CreateL ( _L8 ("http://[0:0:0:0:0:0:0:1]"), iAllocator );
			TUpnpMessage::TUPnPClientRequestInfo msg ( HTTP::EGET, uriBuf, bodyBuf, NULL );
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg );
			
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}

		case ECleanup:
			{
			delete iTestServer;
			// cleanup tcp client flow
			delete reinterpret_cast<CHttpClientFlow*> ( iClientId.Ptr () );
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
	
void CTcpClientTestUPnP1::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}

MTcpClientTestCase* CTcpClientTestUPnP1::GetTestCase ()
	{
	return this;
	}
	
CTestExecuteLogger& CTcpClientTestUPnP1::GetLogger ( )
	{
	return iLogger;
	}


void CTcpClientTestUPnP1::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iClientId = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TResponse > () )
		{
		TUpnpMessage::TResponse& msg = message_cast< TUpnpMessage::TResponse >(aMessage);
		
		RBuf8 responseBodyData;
		responseBodyData.CreateMax ( msg.iBodyData.Length () );
		responseBodyData.FillZ ( msg.iBodyData.Length () );
		msg.iBodyData.CopyOut ( responseBodyData );	
		responseBodyData.Close ();
		msg.iBodyData.Free ();
		
		CompleteSelf(EPass);
		}
	else if ( aMessage.IsMessage<TEBase::TError > () )
		{
		iLogger.WriteFormat(_L("<i>TEBase::TError..... </i>"));
		CompleteSelf(EFail);
		}

	}
	
const TDesC& CTcpClientTestUPnP1::TestCaseName () const
	{
	_LIT ( KTxtTitle, "Defect Fix CINC077703More" );
	return KTxtTitle ();
	}
	
TInt CTcpClientTestUPnP1::TotalTransactionCount () const
	{
	return KTransactionCount;
	}

const TDesC8& CTcpClientTestUPnP1::GetRawRequest ( TInt aTransIndex )
	{
	__ASSERT_ALWAYS ( aTransIndex < KTransactionCount, User::Invariant () );
	return KTxtRawRequest ();
	}
	
const TDesC8& CTcpClientTestUPnP1::GetRawResponse ( TInt aTransIndex )
	{
	__ASSERT_ALWAYS( aTransIndex < KTransactionCount, User::Invariant () );
	return KTxtRawResponse1 ();
	}

TInt CTcpClientTestUPnP1::TransactionCount () const
	{ 
	return KTransactionCount; 
	}



