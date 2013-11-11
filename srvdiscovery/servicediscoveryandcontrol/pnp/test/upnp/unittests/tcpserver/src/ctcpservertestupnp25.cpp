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
#include "CTcpServerTestUPnP25.h"

using namespace Messages;

//------------------------------------------------------------//	
/////////////////// CTcpServerTestUPnP25 /////////////////////////
////// Testcase: Send the test data from the test client/////////
//------------------------------------------------------------//

_LIT8 ( KTestUPnPServiceUri,"http://127.0.0.1:80/testserviceuri1" );
_LIT8 ( KData, "GET /testserviceuri1 HTTP/1.0\r\nHOST: 127.0.0.1:80\r\n\r\n");
_LIT8 ( KExpectedResponse, "HTTP/1.1 200 OK");
_LIT8 ( KResponseData, "Test Server");

CTcpServerTestUPnP25* CTcpServerTestUPnP25::NewL ( CProxyProvd* aProvd )
	{
	CTcpServerTestUPnP25* ret = new ( ELeave ) CTcpServerTestUPnP25 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpServerTestUPnP25::CTcpServerTestUPnP25 ( CProxyProvd* aProvd ) :
	CTcpServerTestCase ( aProvd )
	{
	}
	
CTcpServerTestUPnP25::~CTcpServerTestUPnP25 ()
	{

	}
	
void CTcpServerTestUPnP25::ConstructL ()
	{
	CTcpServerTestCase::ConstructL(KTestUPnPServiceUri);
	}

TVerdict CTcpServerTestUPnP25::RunTestL ()
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
	
	
void CTcpServerTestUPnP25::RecvComplete ( RMBufChain& aData )
	{
	RBuf8 responseBuf;
	responseBuf.CreateMax ( aData.Length () );
	aData.CopyOut ( responseBuf );
	aData.Free ();
	
	if ( responseBuf.FindF ( KExpectedResponse ) != KErrNotFound )
		{
		iSocketHandler.Recv ();
		responseBuf.Close ();
		return;
		}
		
	if ( responseBuf.FindF ( KResponseData ) == KErrNotFound )
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

void CTcpServerTestUPnP25::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
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






