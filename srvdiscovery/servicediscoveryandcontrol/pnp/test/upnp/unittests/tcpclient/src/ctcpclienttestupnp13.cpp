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

#include "CTcpClientTestUPnP13.h"
#include "upnpserverconstants.h"

const TUint16 KActivityNull = 0;
const TUint KHttpDefaultPort = 80;


const TUint KTransactionCount = 1;
_LIT8 ( KTxtRawRequest, "POST / HTTP/1.1\r\nHost: 127.0.0.1\r\nContent-Length: 330\r\nContent-Type: text/xml; charset=utf-8\r\nSoapAction: :urn:shemas-upnp-org:control-1-0#QueryStateVariableResponse:\r\n\r\n" );
_LIT8 ( KTxtRawResponse1, "HTTP/1.1 200 OK\r\nContent-Length: 330\r\n\r\n<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:QueryStateVariableResponse xmlns:u=\"urn:shemas-upnp-org:control-1-0\"><return>Query return value</return></u:QueryStateVariableResponse></s:Body></s:Envelope>\r\n\r\n" );
_LIT8 ( KData1, "<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:QueryStateVariableResponse " );
_LIT8 ( KData2, "xmlns:u=\"urn:shemas-upnp-org:control-1-0\">");
_LIT8 ( KData3, "<return>Query return value</return></u:QueryStateVariableResponse></s:Body></s:Envelope>");

CTcpClientTestUPnP13* CTcpClientTestUPnP13::NewL ( CProxyProvd* aProvd )
	{
	CTcpClientTestUPnP13* ret = new ( ELeave ) CTcpClientTestUPnP13 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpClientTestUPnP13::CTcpClientTestUPnP13 ( CProxyProvd* aProvd ) :
	CTcpClientTestBase(aProvd), iTestServer(NULL)
	{
	}

CTcpClientTestUPnP13::~CTcpClientTestUPnP13 ()
	{	
	iTimer.Close ();
	}

void CTcpClientTestUPnP13::ConstructL()
	{	
	iTimer.CreateLocal ();
	}

TVerdict CTcpClientTestUPnP13::RunTestL()
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
			TPtrC8 data2(KData2);
			TPtrC8 data3(KData3);
			
			iSSP->SetOption(KCHOptionLevel, KCHAbsoluteUri, _L8 ("http://127.0.0.1"));
			
			TCHMessageOption option ( 1, data.Length()+data2.Length()+data3.Length() );
			
			TPckg < TCHMessageOption > optionBuf ( option );
			
			iSSP->SetOption(KCHOptionLevel, KCHMaxLength, optionBuf);
			
			RMBufChain bodyBuf;
			bodyBuf.CreateL(KData1);
			iSSPData->Write(bodyBuf, 0, NULL);
			
			/*
			
			TCHMessageOption option2 ( 1, data2.Length() );
			TPckg < TCHMessageOption > optionBuf2 ( option2 );
			
			iSSP->SetOption(KCHOptionLevel, KCHMaxLength, optionBuf2);
			*/
			
			RMBufChain bodyBuf1;
			bodyBuf1.CreateL(KData2);
			iSSPData->Write(bodyBuf1, 0, NULL);

			/*
			
			TCHMessageOption option3 ( 1, data3.Length() );
			TPckg < TCHMessageOption > optionBuf3 ( option3 );
			
			iSSP->SetOption(KCHOptionLevel, KCHMaxLength, optionBuf3);
			*/
			
			RMBufChain bodyBuf2;
			bodyBuf2.CreateL(KData3);
			iSSPData->Write(bodyBuf2, 0, NULL);

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
	
void CTcpClientTestUPnP13::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}

MTcpClientTestCase* CTcpClientTestUPnP13::GetTestCase ()
	{
	return this;
	}

CTestExecuteLogger& CTcpClientTestUPnP13::GetLogger ( )
	{
	return iLogger;
	}
	

void CTcpClientTestUPnP13::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
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
	
const TDesC& CTcpClientTestUPnP13::TestCaseName () const
	{
	_LIT ( KTxtTitle, "CTcpClientTestMWrites" );
	return KTxtTitle ();
	}
	
TInt CTcpClientTestUPnP13::TotalTransactionCount () const
	{
	return KTransactionCount;
	}

const TDesC8& CTcpClientTestUPnP13::GetRawRequest ( TInt /*aTransIndex*/ )
	{
	//__ASSERT_ALWAYS ( aTransIndex < KTransactionCount, User::Invariant () );
	return KTxtRawRequest ();
	}
	
const TDesC8& CTcpClientTestUPnP13::GetRawResponse ( TInt /*aTransIndex*/ )
	{
	//__ASSERT_ALWAYS( aTransIndex < KTransactionCount, User::Invariant () );
	return KTxtRawResponse1 ();
	}

TInt CTcpClientTestUPnP13::TransactionCount () const
	{ 
	return KTransactionCount; 
	}

void CTcpClientTestUPnP13::NewData(TUint aCount)
	{
	RMBufChain data;
	RBuf8 resData;
	
	iLogger.WriteFormat(_L("<i>NewData..... </i>"));
	iSSP->GetOption(1,KCHMaxLength,resData);
	resData.Create( aCount );
	iSSP->GetOption(KCHOptionLevel,KCHMaxLength,resData);
	TInt len = *(TUint*) resData.Ptr();
	resData.Close();
	
	iSSPData->GetData(data,aCount,0);
	resData.Create( aCount );
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







