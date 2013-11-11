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

#include "CTcpClientTestUPnP8.h"
#include "upnpserverconstants.h"

const TUint16 KActivityNull = 0;
const TUint KHttpDefaultPort = 80;


const TUint KTransactionCount = 2;
_LIT8 ( KTxtRawRequest, "POST / HTTP/1.1\r\nHost: 127.0.0.1\r\nContent-Length: 330\r\nContent-Type: text/xml; charset=utf-8\r\nSoapAction: :urn:shemas-upnp-org:control-1-0#QueryStateVariableResponse:\r\n\r\n" );
_LIT8 ( KTxtRawRequest1, "M-POST / HTTP/1.1\r\nHost: 127.0.0.1\r\nContent-Length: 330\r\nContent-Type: text/xml; charset=utf-8\r\nMAN: \r\n01-SOAPACTION: :urn:shemas-upnp-org:control-1-0#QueryStateVariableResponse:\r\n\r\n" );
_LIT8 ( KTxtRawResponse1, "HTTP/1.1 405 Method Not Allowed\r\nContent-Length: 0\r\n\r\n" );
_LIT8 ( KData1, "<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:QueryStateVariableResponse xmlns:u=\"urn:shemas-upnp-org:control-1-0\"><return>Query return value</return></u:QueryStateVariableResponse></s:Body></s:Envelope>" );
_LIT8 ( KTxtRawResponse2, "HTTP/1.1 200 OK\r\nContent-Length: 330\r\n\r\n<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:QueryStateVariableResponse xmlns:u=\"urn:shemas-upnp-org:control-1-0\"><return>Query return value</return></u:QueryStateVariableResponse></s:Body></s:Envelope>\r\n\r\n" );

CTcpClientTestUPnP8* CTcpClientTestUPnP8::NewL ( CProxyProvd* aProvd )
	{
	CTcpClientTestUPnP8* ret = new ( ELeave ) CTcpClientTestUPnP8 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpClientTestUPnP8::CTcpClientTestUPnP8 ( CProxyProvd* aProvd ) :
	CTcpClientTestBase(aProvd), iTestServer(NULL)
	{
	}

CTcpClientTestUPnP8::~CTcpClientTestUPnP8 ()
	{	
	iTimer.Close ();
	}

void CTcpClientTestUPnP8::ConstructL()
	{	
	iTimer.CreateLocal ();
	}

TVerdict CTcpClientTestUPnP8::RunTestL()
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
			/*
			RMBufChain bodyBuf1;
			bodyBuf1.CreateL(KData1);
			iSSPData->Write(bodyBuf1, 0, NULL);
			*/
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
			iLogger.WriteFormat(_L("<i>TestCase: Complete..... </i>"));
			return EPass;
			}
			
		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
void CTcpClientTestUPnP8::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}

MTcpClientTestCase* CTcpClientTestUPnP8::GetTestCase ()
	{
	return this;
	}

CTestExecuteLogger& CTcpClientTestUPnP8::GetLogger ( )
	{
	return iLogger;
	}

	
void CTcpClientTestUPnP8::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
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
	
const TDesC& CTcpClientTestUPnP8::TestCaseName () const
	{
	_LIT ( KTxtTitle, "CTcpClientTestUPnP8" );
	return KTxtTitle ();
	}
	
TInt CTcpClientTestUPnP8::TotalTransactionCount () const
	{
	return KTransactionCount;
	}

const TDesC8& CTcpClientTestUPnP8::GetRawRequest ( TInt aTransIndex )
	{
	//__ASSERT_ALWAYS ( aTransIndex < KTransactionCount, User::Invariant () );
	if(aTransIndex == 0)
		return KTxtRawRequest ();
	else if(aTransIndex == 1)
		return KTxtRawRequest1 ();
	else
		return KNullDesC8 ();
		
	}
	
const TDesC8& CTcpClientTestUPnP8::GetRawResponse ( TInt aTransIndex )
	{
	//__ASSERT_ALWAYS( aTransIndex < KTransactionCount, User::Invariant () );
	if(aTransIndex == 0)
		return KTxtRawResponse1 ();
	else if(aTransIndex == 1)
		return KTxtRawResponse2 ();
	else
		return KNullDesC8 ();
	}

TInt CTcpClientTestUPnP8::TransactionCount () const
	{ 
	return KTransactionCount; 
	}

void CTcpClientTestUPnP8::NewData(TUint aCount)
	{
	RMBufChain data;
	RBuf8 resData;
	
	iLogger.WriteFormat(_L("<i>NewData..... </i>"));
	iSSP->GetOption(1,KCHMaxLength,resData);
	resData.Create( aCount );
	iSSP->GetOption(KCHOptionLevel,KCHMaxLength,resData);
	TInt len = *(TUint*) resData.Ptr();
	resData.Close();
	
	iSSPData->GetData(data,len,0);
	resData.Create( len );
	data.CopyOut(resData);
	resData.Close();
	resData.Create( aCount );
	iSSP->GetOption(KCHOptionLevel,KCHLastMessage,resData);
	TBool isLastMsg = *(TUint*) resData.Ptr();
	resData.Close();
	data.Init();
	
	if(isLastMsg)
		{
		iLogger.WriteFormat(_L("<i>NewData.LastMsg.... </i>"));
		CompleteSelf ( KErrNone );
		}
	}







