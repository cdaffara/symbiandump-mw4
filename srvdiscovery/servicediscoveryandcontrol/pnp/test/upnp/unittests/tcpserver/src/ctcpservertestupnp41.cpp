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
#include "CTcpServerTestUPnP41.h"
#include "upnpserverconstants.h"

_LIT8 ( KTestUPnPServiceUri,"http://127.0.0.1:80/testserviceuri1" );
_LIT8 ( KData, "POST /testserviceuri1 HTTP/1.1\r\nHost: 127.0.0.1\r\nContent-Length: 330\r\nContent-Type: text/xml; charset=utf-8\r\nSoapAction: :urn:shemas-upnp-org:control-1-0#QueryStateVariableResponse:\r\n\r\n");
_LIT8 ( KData1, "<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:QueryStateVariableResponse xmlns:u=\"urn:shemas-upnp-org:control-1-0\"><return>Query return value</return></u:QueryStateVariableResponse></s:Body></s:Envelope>" );
_LIT8 ( KExpectedResponse, "HTTP/1.1 200 OK");

CTcpServerTestUPnP41* CTcpServerTestUPnP41::NewL ( CProxyProvd* aProvd )
	{
	CTcpServerTestUPnP41* ret = new ( ELeave ) CTcpServerTestUPnP41 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpServerTestUPnP41::CTcpServerTestUPnP41 ( CProxyProvd* aProvd ) :
	CTcpServerTestCase ( aProvd ),iSentHeaders(EFalse),iIsWriteDone(EFalse)
	{
	}
	
CTcpServerTestUPnP41::~CTcpServerTestUPnP41 ()
	{

	}
	
void CTcpServerTestUPnP41::ConstructL ()
	{
	CTcpServerTestCase::ConstructL(KTestUPnPServiceUri);
	}

TVerdict CTcpServerTestUPnP41::RunTestL ()
	{
	switch ( iState )
		{
			
		case ESendData:
			{
			
			if(!iIsWriteDone)
				{
				iSendChain.CreateL ( KData );
				iSocketHandler.Send ( iSendChain );
				iSentHeaders = ETrue;
				}
			else
				{
				
				TCHMessageOption option ( 0, 330 );
				TPckg < TCHMessageOption > optionBuf ( option );
				
				iSSP->SetOption(KCHOptionLevel, KCHMaxLength, optionBuf);
				
				RBuf8 resData;
				iSSP->GetOption(23, KCHMaxLength, resData);
				//iSSP->GetOption(KCHOptionLevel, KCHMaxLength, resData);
				
				RMBufChain bodyBuf;
								
				iSSPData->Write(bodyBuf, 0, NULL);
				
				TCHMessageOption option1 ( 1, 330 );
				TPckg < TCHMessageOption > optionBuf1 ( option1 );
				
				iSSP->SetOption(KCHOptionLevel, KCHMaxLength, optionBuf1);
			
				//iSSP->SetOption(KCHOptionLevel, KCHLastMessage, optionBuf1);
				bodyBuf.CreateL(KData1);
				iSSPData->Write(bodyBuf, 1, NULL);
				
				iSSP->Shutdown(MSessionControl::EStopInput);
				
				iState = ESendStopServer;
				
				}
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
	
	
void CTcpServerTestUPnP41::RecvComplete ( RMBufChain& aData )
	{
	RBuf8 responseBuf;
		
	responseBuf.CreateMax ( aData.Length () );
	aData.CopyOut ( responseBuf );
	aData.Free ();
	
	if ( responseBuf.FindF ( KExpectedResponse ) != KErrNotFound )
		{
		iResponse = EPass;	// test case Passed
		iSocketHandler.Recv ();
		}
	else if ( responseBuf.FindF ( KData1 ) != KErrNotFound )
		{
		CompleteSelf ( KErrNone );
		}
	
	responseBuf.Close ();
	
	}

void CTcpServerTestUPnP41::SendComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();
	if(iSentHeaders)
		{
		TRAP_IGNORE(iSendChain.CreateL ( KData1 ));
		iSocketHandler.Send ( iSendChain );
		iSentHeaders = EFalse;
		}
	else
		iSocketHandler.Recv ();
	}
	



void CTcpServerTestUPnP41::NewData(TUint aCount)
	{
	
	
	iResponseLength = iResponseLength + aCount;
	
	if(!aCount && (iResponseLength == 334))
		{
		iIsWriteDone = ETrue;
		CompleteSelf ( KErrNone );
		}
	else
		{
		RBuf8 responseBuf;
		RBuf8 resData;
		RMBufChain	dataReceived;
		
		iSSPData->GetData(dataReceived, aCount, 0, NULL);
			
		responseBuf.CreateMax ( dataReceived.Length () );
		dataReceived.CopyOut ( responseBuf );
		dataReceived.Free ();
		responseBuf.Close ();
		}
	/*
	iSSP->GetOption(KCHOptionLevel, KCHLastMessage, resData);
	TBool isLastMsg = *(TUint*) resData.Ptr();

	if(isLastMsg)
		{
		iIsWriteDone = ETrue;
		CompleteSelf ( KErrNone );
		}
	*/
	
	}

void CTcpServerTestUPnP41::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iTcpServer = msg.iNodeId;
		
		iFlow = mcfnode_cast<CSubConnectionFlowBase>(
		reinterpret_cast<Messages::ANode*>(	iTcpServer.Ptr()));
		
		
		// Perform the binding to the flow below
		iFlowBinder = iFlow->GetBinderControlL();
		iSSP = iFlowBinder->GetControlL(KSockStream, *this);
		iSSPData = iFlowBinder->BindL(*this);
		
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPRequestInfo> ())
		{

		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////

_LIT8 ( KMPostData, "M-POST /testserviceuri1 HTTP/1.1\r\nHost: 127.0.0.1\r\nMAN: http://schemas.xmlsoap.org/soap/envelope/\r\nContent-Length: 330\r\nContent-Type: text/xml; charset=utf-8\r\nSoapAction: :urn:shemas-upnp-org:control-1-0#QueryStateVariableResponse:\r\n\r\n");


CTcpServerTestUPnP42* CTcpServerTestUPnP42::NewL ( CProxyProvd* aProvd )
	{
	CTcpServerTestUPnP42* ret = new ( ELeave ) CTcpServerTestUPnP42 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpServerTestUPnP42::CTcpServerTestUPnP42 ( CProxyProvd* aProvd ) :
	CTcpServerTestCase ( aProvd ),iSentHeaders(EFalse),iIsWriteDone(EFalse)
	{
	}
	
CTcpServerTestUPnP42::~CTcpServerTestUPnP42 ()
	{

	}
	
void CTcpServerTestUPnP42::ConstructL ()
	{
	CTcpServerTestCase::ConstructL(KTestUPnPServiceUri);
	}

TVerdict CTcpServerTestUPnP42::RunTestL ()
	{
	switch ( iState )
		{
			
		case ESendData:
			{
			
			if(!iIsWriteDone)
				{
				iSendChain.CreateL ( KMPostData );
				iSocketHandler.Send ( iSendChain );
				iSentHeaders = ETrue;
				}
			else
				{
				
				TCHMessageOption option ( 0, 330 );
				TPckg < TCHMessageOption > optionBuf ( option );
				
				iSSP->SetOption(KCHOptionLevel, KCHMaxLength, optionBuf);
				
				RBuf8 resData;
				iSSP->GetOption(23, KCHMaxLength, resData);
				//iSSP->GetOption(KCHOptionLevel, KCHMaxLength, resData);
				
				RMBufChain bodyBuf;
								
				iSSPData->Write(bodyBuf, 0, NULL);
				
				TCHMessageOption option1 ( 1, 330 );
				TPckg < TCHMessageOption > optionBuf1 ( option1 );
				
				iSSP->SetOption(KCHOptionLevel, KCHMaxLength, optionBuf1);
			
				//iSSP->SetOption(KCHOptionLevel, KCHLastMessage, optionBuf1);
				bodyBuf.CreateL(KData1);
				iSSPData->Write(bodyBuf, 1, NULL);
				
				iSSP->Shutdown(MSessionControl::EStopInput);
				
				iState = ESendStopServer;
				
				}
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
	
	
void CTcpServerTestUPnP42::RecvComplete ( RMBufChain& aData )
	{
	RBuf8 responseBuf;
		
	responseBuf.CreateMax ( aData.Length () );
	aData.CopyOut ( responseBuf );
	aData.Free ();
	
	if ( responseBuf.FindF ( KExpectedResponse ) != KErrNotFound )
		{
		iResponse = EPass;	// test case Passed
		iSocketHandler.Recv ();
		}
	else if ( responseBuf.FindF ( KData1 ) != KErrNotFound )
		{
		CompleteSelf ( KErrNone );
		}
	
	responseBuf.Close ();
	
	}

void CTcpServerTestUPnP42::SendComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();
	if(iSentHeaders)
		{
		TRAP_IGNORE(iSendChain.CreateL ( KData1 ));
		iSocketHandler.Send ( iSendChain );
		iSentHeaders = EFalse;
		}
	else
		iSocketHandler.Recv ();
	}
	



void CTcpServerTestUPnP42::NewData(TUint aCount)
	{
	
	
	iResponseLength = iResponseLength + aCount;
	
	if(!aCount && (iResponseLength == 334))
		{
		iIsWriteDone = ETrue;
		CompleteSelf ( KErrNone );
		}
	else
		{
		RBuf8 responseBuf;
		RBuf8 resData;
		RMBufChain	dataReceived;
		
		iSSPData->GetData(dataReceived, aCount, 0, NULL);
			
		responseBuf.CreateMax ( dataReceived.Length () );
		dataReceived.CopyOut ( responseBuf );
		dataReceived.Free ();
		responseBuf.Close ();
		}
	/*
	iSSP->GetOption(KCHOptionLevel, KCHLastMessage, resData);
	TBool isLastMsg = *(TUint*) resData.Ptr();

	if(isLastMsg)
		{
		iIsWriteDone = ETrue;
		CompleteSelf ( KErrNone );
		}
	*/
	
	}

void CTcpServerTestUPnP42::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iTcpServer = msg.iNodeId;
		
		iFlow = mcfnode_cast<CSubConnectionFlowBase>(
		reinterpret_cast<Messages::ANode*>(	iTcpServer.Ptr()));
		
		
		// Perform the binding to the flow below
		iFlowBinder = iFlow->GetBinderControlL();
		iSSP = iFlowBinder->GetControlL(KSockStream, *this);
		iSSPData = iFlowBinder->BindL(*this);
		
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPRequestInfo> ())
		{

		}
	}







