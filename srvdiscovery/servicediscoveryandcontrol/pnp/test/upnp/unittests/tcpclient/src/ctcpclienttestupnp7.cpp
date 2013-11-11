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

#include "CTcpClientTestUPnP7.h"
#include "upnpserverconstants.h"

const TUint16 KActivityNull = 0;
const TUint KHttpDefaultPort = 80;


const TUint KTransactionCount = 1;
_LIT8 ( KTxtRawRequest, "POST / HTTP/1.1\r\nHost: 127.0.0.1\r\nContent-Length: 330\r\nContent-Type: text/xml; charset=utf-8\r\nSoapAction: :urn:shemas-upnp-org:control-1-0#QueryStateVariableResponse:\r\n\r\n" );
_LIT8 ( KTxtRawResponse1, "HTTP/1.1 200 OK\r\nContent-Length: 330\r\n\r\n<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:QueryStateVariableResponse xmlns:u=\"urn:shemas-upnp-org:control-1-0\"><return>Query return value</return></u:QueryStateVariableResponse></s:Body></s:Envelope>\r\n\r\n" );
_LIT8 ( KData1, "<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:QueryStateVariableResponse xmlns:u=\"urn:shemas-upnp-org:control-1-0\"><return>Query return value</return></u:QueryStateVariableResponse></s:Body1></s:Envelope>" );

CTcpClientTestUPnP7* CTcpClientTestUPnP7::NewL ( CProxyProvd* aProvd )
	{
	CTcpClientTestUPnP7* ret = new ( ELeave ) CTcpClientTestUPnP7 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpClientTestUPnP7::CTcpClientTestUPnP7 ( CProxyProvd* aProvd ) :
	CTcpClientTestBase(aProvd), iTestServer(NULL)
	{
	}

CTcpClientTestUPnP7::~CTcpClientTestUPnP7 ()
	{	
	iTimer.Close ();
	}

void CTcpClientTestUPnP7::ConstructL()
	{	
	iTimer.CreateLocal ();
	}

TVerdict CTcpClientTestUPnP7::RunTestL()
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
			
			TPtrC8 data(KData1);
			
			iSSP->SetOption(KCHOptionLevel, KCHAbsoluteUri, _L8 ("http://127.0.0.1"));
			
			TCHMessageOption option ( 1, data.Length() );
			TPckg < TCHMessageOption > optionBuf ( option );
			
			iSSP->SetOption(KCHOptionLevel, KCHMaxLength, optionBuf);
			
			RMBufChain bodyBuf;
			bodyBuf.CreateL(KData1);
			iSSPData->Write(bodyBuf, 0, NULL);
			
			//////Panic in this case--Defect
			/*
			RMBufChain bodyBuf1;
			bodyBuf1.CreateL(KNullDesC8());
			iSSPData->Write(bodyBuf1, 0, NULL);
			*/

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
	
void CTcpClientTestUPnP7::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}

MTcpClientTestCase* CTcpClientTestUPnP7::GetTestCase ()
	{
	return this;
	}
	
CTestExecuteLogger& CTcpClientTestUPnP7::GetLogger ( )
	{
	return iLogger;
	}


void CTcpClientTestUPnP7::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iClientId = msg.iNodeId;
		
		
		iFlow = mcfnode_cast<CSubConnectionFlowBase>(
		reinterpret_cast<Messages::ANode*>(	iClientId.Ptr()));
		
		
		// Perform the binding to the flow below
		iFlowBinder = iFlow->GetBinderControlL();
		iSSP = iFlowBinder->GetControlL(KSockStream, *this);
		iSSPData = iFlowBinder->BindL(*this);
		
		
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TResponse > () )
		{
		
		CompleteSelf(EPass);
		}
	else if ( aMessage.IsMessage<TEBase::TError > () )
		{
		iLogger.WriteFormat(_L("<i>TEBase::TError..... </i>"));
		CompleteSelf(EFail);
		}

	}
	
const TDesC& CTcpClientTestUPnP7::TestCaseName () const
	{
	_LIT ( KTxtTitle, "Defect Fix CINC077703More" );
	return KTxtTitle ();
	}
	
TInt CTcpClientTestUPnP7::TotalTransactionCount () const
	{
	return KTransactionCount;
	}

const TDesC8& CTcpClientTestUPnP7::GetRawRequest ( TInt aTransIndex )
	{
	__ASSERT_ALWAYS ( aTransIndex < KTransactionCount, User::Invariant () );
	return KTxtRawRequest ();
	}
	
const TDesC8& CTcpClientTestUPnP7::GetRawResponse ( TInt aTransIndex )
	{
	__ASSERT_ALWAYS( aTransIndex < KTransactionCount, User::Invariant () );
	return KTxtRawResponse1 ();
	}

TInt CTcpClientTestUPnP7::TransactionCount () const
	{ 
	return KTransactionCount; 
	}









