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

//System Includes
#include <comms-infras/ss_nodeinterfaces.h>
//#include <comms-infras/ss_nodemessages_controlprovider.h>
#include <inetprottextutils.h>
#include <uriutils.h>
#include <http/thttptable.h>
#include <httperr.h>
#include <httpstringconstants.h>
#include <stringpool.h>
#include <http/framework/cheadercodecplugin.h>
#include <upnp/tupnptable.h>
#include <rmemchunk.h>

//Local Includes
#include "httpclientflow.h"
#include "upnpserverconstants.h"
#include "upnplog.h"
#include "upnppint.h"
#include "upnpmemoryutils.h"


using namespace ESock;
using namespace Messages;


__FLOG_STMT(_LIT8(KComponent,"Flow");)
const TUint KHttpDefaultPort = 80;
_LIT8(KSlash, "/");

CHttpClientFlow* CHttpClientFlow::NewL ( CSubConnectionFlowFactoryBase& aFactory,
										CProtocolIntfBase* aProtocolIntf,
										const TNodeId& aSubConnId,
										const TAppProtAddr& aSockAddr )
	{	
	CHttpClientFlow* self = new ( ELeave ) CHttpClientFlow ( aFactory, aProtocolIntf, aSubConnId, aSockAddr );
	CleanupStack::PushL ( self );
	self->ConstructL( );
	CleanupStack::Pop ( self );
	return self;
	}


CHttpClientFlow::CHttpClientFlow ( CSubConnectionFlowFactoryBase& aFactory, CProtocolIntfBase* aProtocolIntf, const TNodeId& aSubConnId, const TAppProtAddr& aSockAddr )
	: CUPnPFlowBase ( aFactory, aProtocolIntf, EHttpClientFlow, aSubConnId ),
	iDCIdle ( EFalse ), iRemoteAddress ( aSockAddr.iAddr, aSockAddr.iPort ), iResponseCompleted ( EFalse ), iReadStreamStarted ( EFalse )
	{
	LOG_NODE_CREATE(KESockFlowTag, CHttpClientFlow);
	}


CHttpClientFlow::~CHttpClientFlow ( )
	{	
	iRemoteAddress.FillZ ( );
	iRequestUri.Close ( );
	iActionValue.Close ( );
	for ( TInt idx = 0; idx < iTransactions->Count( ); idx++ )
		{
		delete iTransactions->Get( idx );
		}
	if ( !iBufChain.IsEmpty ( ) )
		iBufChain.Free ( );

	delete iCodec;
	iStringPool.Close();
	// Note! delete client handler after cleanp of transactions, or else it will panic
	delete iClientHandler;
	delete iActionParser;

	iSubConnectionProvider.Close ( );
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("Destroyed CHttpClientFlow")));
	LOG_NODE_DESTROY(KESockFlowTag, CHttpClientFlow);
	}

void CHttpClientFlow::ConstructL ( )
	{	
	iClientHandler = CHTTPClientHandler::NewL ( *this, static_cast<CUPnPProtocolIntfBase *>( ProtocolIntf () )->GetMemoryChunkManager(), &iRemoteAddress );
	iTransactions = iClientHandler->GetTransactionWrapperL( );

	iStringPool.OpenL( TUPnPTable::Table() );
	iStringPool.OpenL(THTTPTable::Table());
	iCodec = CHeaderCodecPlugin::NewL ( KUPnPCodecName, iStringPool );
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("Created CHttpClientFlow")));
	}



//From CSubConnectionFlowBase MNode
void CHttpClientFlow::ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{	
 	if ( aMessage.IsMessage<TEChild::TDestroy> () )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::ReceivedL - Received message TCFMessage::TDestroy")));
  		if ( RemoveControlClient ( FindControlClient ( address_cast<TNodeId> ( aSender ) ) ) == 0 )
   			{
   			DeleteThisFlow ( );
   			}
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStart> () )
		{
        LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::ReceivedL - Received message TCFMessage::TDataClientStart")));
		RClientInterface::OpenPostMessageClose ( NodeId (), address_cast<TNodeCtxId> ( aSender ), TCFDataClient::TStarted ().CRef () );
    	}
    else if ( aMessage.IsMessage<TCFDataClient::TStop> () )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::ReceivedL - Received message TCFMessage::TDataClientStop")));
		CancelRequest ( address_cast<TNodeCtxId> ( aSender ) );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPClientRequestInfo> () )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::ReceivedL - Received message TUpnpMessage::TUPnPClientRequestInfo")));
		TUpnpMessage::TUPnPClientRequestInfo& clntReqInfo = message_cast<TUpnpMessage::TUPnPClientRequestInfo> ( aMessage );
		PrepareAndSubmitTransactionL ( clntReqInfo, address_cast<TNodeCtxId> ( aSender ) );
		}
	}

void CHttpClientFlow::CancelRequest ( TNodeCtxId aChannelId )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::CancelRequest")));
	TInt pos = FindTransaction ( aChannelId );
	if ( pos != KErrNotFound ) // transaction is not found
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::CancelRequest - Cancelling transaction")));
		CTransaction* ctrans = iTransactions->Get( pos );
		iClientHandler->CancelTransaction ( ctrans );

		THTTPEvent event ( THTTPEvent::ECancel );
		OnHttpEvent ( ctrans, event );
		}
	else
		{
		if ( !iDCIdle ) // This will make sure when ASocket attached as upperflow
						// and unbind is received from it, No more communication will 
						// happen to SCPR.
			{
			RClientInterface::OpenPostMessageClose ( NodeId (), aChannelId, TCFDataClient::TStopped ( 0 ).CRef () );
			}
		}
	}


TInt CHttpClientFlow::FindTransaction ( TNodeCtxId aChannelId )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::FindTransaction")));
	TUint count = iTransactions->Count( );
	for ( TInt idx = 0; idx < count; idx++ )
		{
		CHTTPClientTransaction* ctrans = iTransactions->Get( idx );

		if ( ctrans->NodeCtxId ( ) == aChannelId )
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::FindTransaction - Transaction found at index %d"), idx));
			return idx;
			}
		}
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::FindTransaction - Transaction not found")));
	return KErrNotFound;
	}

void CHttpClientFlow::PrepareAndSubmitTransactionL ( RMBufChain& aData )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::PrepareAndSubmitTransactionL")));

	TNodeCtxId channelId ( MeshMachine::KActivityNull, NodeId () );

	CHTTPClientTransaction* ctrans = CHTTPClientTransaction::NewL ( *iCodec, iStringPool, channelId );
	iTransactions->AppendL ( ctrans );

	CRequest* crequest = ctrans->Request( );
	RHTTPHeaders headers = crequest->Handle( ).GetHeaderCollection ( );
	RRequest rrequest = crequest->Handle( );

	// Set request method
	RStringF stringF = iStringPool.StringF ( HTTP::EPOST, THTTPTable::Table() );
	rrequest.SetMethod ( stringF );

	// Set request Uri
	SetRequestUriL ( ctrans, iRequestUri );
	SetContentTypeHeaderL ( headers );

	//Set Content Length
	THTTPHdrVal hdrVal ( iOverallDataSize );
	headers.SetFieldL ( iStringPool.StringF ( HTTP::EContentLength, THTTPTable::Table() ), hdrVal );

	// Note! 100-continue, UPnP is not having any usecase to use this

    //Set Soap Action
	if ( SetActionHeaderL ( aData ) )
		{
		// Complete headers are available, so Transaction is ready for submission
		LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::PrepareAndSubmitTransactionL - Submitting transaction")));
		iClientHandler->SubmitTransaction( ctrans );
		}

	// Set Body Data
	ctrans->AddBodyPart( aData );
	SetTransactionComplete ( ctrans );
	}

TBool CHttpClientFlow::SetActionHeaderL ( RMBufChain& aData )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::SetActionHeaderL")));

	CTransaction* ctrans = iTransactions->Get( 0 );

	CRequest* crequest = ctrans->Request( );
	RHTTPHeaders headers = crequest->Handle( ).GetHeaderCollection ( );
	RRequest rrequest = crequest->Handle( );

	// get service name & action name
	ConstructSoapActionHeaderValueL ( aData );

	if ( iActionValue.Length ( ) != 0 )
		{
		RStringF stringF =  iStringPool.StringF ( UPnP::ESoapAction, TUPnPTable::Table() );
		headers.SetRawFieldL ( stringF, iActionValue, KSoapFieldSeparator );
		}

	return iActionValue.Length ( ) > 0;
	}

void CHttpClientFlow::ConstructSoapActionHeaderValueL ( RMBufChain& aData )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::ConstructSoapActionHeaderValueL")));

	if ( iActionParser == NULL )
		{
		iActionParser =  CSoapParser::NewL ( );
		}

	RBuf8 xmlData;
	xmlData.CreateMaxL ( aData.Length ( ) );
	aData.CopyOut ( xmlData );
	CleanupClosePushL ( xmlData );

	if ( iActionParser->ParseSoapL( xmlData ) )
		{
		TInt len = iActionParser->SoapAction( ).Length ( ) + KColon ( ).Length ( ) * 2;

		iActionValue.CreateL ( len );

		iActionValue.Copy ( KColon );
		iActionValue.Append ( iActionParser->SoapAction( ) );
		iActionValue.Append ( KColon );

		delete iActionParser;
		iActionParser = NULL;
		}

	CleanupStack::PopAndDestroy ( &xmlData );
	}

void CHttpClientFlow::SetContentTypeHeaderL ( RHTTPHeaders aHeaders )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::SetContentTypeHeaderL")));
	// Set content-type
	RStringF stringF = iStringPool.StringF ( HTTP::EContentType, THTTPTable::Table() );
	THTTPHdrVal contentTypeHdrVal;
	RStringF contentTypeHdr = iStringPool.StringF ( UPnP::ETextXml, TUPnPTable::Table() );
	contentTypeHdrVal.SetStrF ( contentTypeHdr );

	RStringF paramStringF = iStringPool.StringF ( HTTP::ECharset, THTTPTable::Table() );
	THTTPHdrVal contentTypeParamVal;
	RStringF contentTypeParam = iStringPool.StringF ( UPnP::EUtf8, TUPnPTable::Table() );
	contentTypeParamVal.SetStrF ( contentTypeParam );
	aHeaders.SetFieldL ( stringF, contentTypeHdrVal, paramStringF, contentTypeParamVal );
	}

void CHttpClientFlow::ReSubmitTransactionL ( CTransaction* aTransaction )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::ReSubmitTransactionL")));
	CRequest* crequest = aTransaction->Request( );
	RHTTPHeaders headers = crequest->Handle( ).GetHeaderCollection ( );
	RRequest rrequest = crequest->Handle ( );

	// Set request method
	RStringF stringF = iStringPool.StringF ( UPnP::EMPost, TUPnPTable::Table() );
	rrequest.SetMethod ( stringF );

	// Set MAN Header
	stringF = iStringPool.StringF ( UPnP::EMAN, TUPnPTable::Table() );
	THTTPHdrVal manHdrVal;
	RStringF manHdr = iStringPool.StringF ( UPnP::ESoapUrl, TUPnPTable::Table() );
	manHdrVal.SetStrF ( manHdr );

	RStringF paramStringF = iStringPool.StringF ( UPnP::ENS, TUPnPTable::Table() );
	THTTPHdrVal nsParamVal;
	RStringF nsParam = iStringPool.StringF ( UPnP::ENsParam, TUPnPTable::Table() );
	headers.SetFieldL ( stringF, manHdrVal, paramStringF, nsParamVal );

	// recompose by adding '01' for SOAPACTION
	// remove existing soap header
	headers.RemoveField ( iStringPool.StringF ( UPnP::ESoapAction, TUPnPTable::Table() ) );

	RStringF soapHdr = iStringPool.StringF ( UPnP::ESoapHeader, TUPnPTable::Table() );
	
	// get service name & action name
	headers.SetRawFieldL ( soapHdr, iActionValue, KSoapFieldSeparator );

	// Transaction is ready for submission.
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::ReSubmitTransactionL - Submitting transaction")));
	iClientHandler->SubmitTransaction( aTransaction );
	}

void CHttpClientFlow::PrepareAndSubmitTransactionL ( TUpnpMessage::TUPnPClientRequestInfo& aClntReqInfo, TNodeCtxId aChannelId )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::PrepareAndSubmitTransactionL")));
	CHTTPClientTransaction* ctrans = CHTTPClientTransaction::NewL ( *iCodec, iStringPool, aChannelId );
	CleanupStack::PushL ( ctrans );

	RBuf8 uriBuf;
	uriBuf.CreateMax( aClntReqInfo.iUri.Length( ) );
	CleanupClosePushL( uriBuf );
	uriBuf.FillZ( aClntReqInfo.iUri.Length( ) );
	aClntReqInfo.iUri.CopyOut( uriBuf );
	aClntReqInfo.iUri.Free( );
	// Extract the uri path from the absolute uri and set the request uri
	SetRequestUriL ( ctrans, uriBuf );
	CleanupStack::PopAndDestroy( &uriBuf );

	RRequest rrequest = ctrans->Request( )->Handle( );
	RStringF method;
	RStringF contentLenStrF = iStringPool.StringF ( HTTP::EContentLength, THTTPTable::Table() );
	THTTPHdrVal contentLenVal;
	TInt contentLength = 0;
	contentLenVal.SetInt( contentLength );
	switch ( aClntReqInfo.iRequestMethod )
		{
		case HTTP::EGET:
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::PrepareAndSubmitTransactionL - Handling describe(GET) request")));
			method = iStringPool.StringF ( HTTP::EGET, THTTPTable::Table() );
			rrequest.SetMethod ( method );
			}
		break;

		case UPnP::ESubscribe:
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::PrepareAndSubmitTransactionL - Handling SUBSCRIBE request")));
			method = iStringPool.StringF ( UPnP::ESubscribe, TUPnPTable::Table() );
			rrequest.SetMethod ( method );
			BuildSubscribeRequestL ( *(ctrans->Request()), aClntReqInfo );
			}
		break;

		case UPnP::EUnsubscribe:
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::PrepareAndSubmitTransactionL - Handling UNSUBSCRIBE request")));
			method = iStringPool.StringF ( UPnP::EUnsubscribe, TUPnPTable::Table() );
			rrequest.SetMethod ( method );
			SetSidHeaderL ( *(ctrans->Request()), aClntReqInfo );
			}
		break;

		case UPnP::ENotify:
			{
			RMBufChain bufChain;
			TUPnPMemoryUtils::CreateMBuf(bufChain, aClntReqInfo.iBody );
			LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::PrepareAndSubmitTransactionL - Handling NOTIFY request")));
			method = iStringPool.StringF ( UPnP::ENotify, TUPnPTable::Table() );
			rrequest.SetMethod ( method );
			BuildNotifyRequestL ( *(ctrans->Request()), aClntReqInfo );
			// set body
			contentLength = aClntReqInfo.iBody.Length ( );
			contentLenVal.SetInt( contentLength );
			ctrans->AddBodyPart( bufChain );
			aClntReqInfo.iBody.Free();
			}
		break;

		default:
		break;
		}

	CleanupStack::Pop ( );

	rrequest.GetHeaderCollection ( ).SetFieldL ( contentLenStrF, contentLenVal );
	// Transaction is ready for submission.
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::PrepareAndSubmitTransactionL - Submitting transaction")));
	ctrans->SetComplete ();
	iClientHandler->SubmitTransaction( ctrans );
	}

void CHttpClientFlow::SetRequestUriL ( CTransaction* aTrans, const TDesC8& aUri )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::SetRequestUriL")));
	RRequest rrequest = aTrans->Request( )->Handle( );
	TUriParser8 uri;
	uri.Parse ( aUri );
	TPtrC8 ptr;
	ptr.Set(uri.UriDes());
	ptr.Set(ptr.Mid(ptr.Find(_L8("//")) + 2));
	TInt pos = ptr.Locate('/');
	if(pos == KErrNotFound)
		{
		ptr.Set(KSlash());
		}
	else
		{
		ptr.Set(ptr.Mid(pos));
		}
	
	TUriParser8 uriPath;
	uriPath.Parse(ptr);
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::SetRequestUriL - Setting the URI")));
	rrequest.SetURIL ( uriPath );
	
	
	// Set Host Header
	TInt port;
	if ( uri.IsPresent ( EUriPort ) )
		{
		const TDesC8& portStr = uri.Extract ( EUriPort );
		InetProtTextUtils::ConvertDescriptorToInt ( portStr, port );
		LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::SetRequestUriL - Got the port from URI %d"), port));
		}
	else
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::SetRequestUriL - Using default HTTP port 80")));
		port = KHttpDefaultPort;
		}

	SetHostHeaderL ( aTrans, uri.Extract ( EUriHost ), port );
	}

void CHttpClientFlow::SetHostHeaderL ( CTransaction* aTrans, const TDesC8& aHost, TInt aPort )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::SetHostHeaderL")));
	RStringF hostStr = iStringPool.StringF ( HTTP::EHost, THTTPTable::Table() );
	RRequest request = aTrans->Request( )->Handle( );
    RHTTPHeaders headers = request.GetHeaderCollection();

	//If the Host header does not already exist, add it!
	THTTPHdrVal hostValue;
	if ( headers.GetField ( hostStr, 0, hostValue ) == KErrNotFound )
		{
		// Set the Host header...
		RStringF hostValStr;
		// Check if its a literal IPV6 address
		UriUtils::TUriHostType aHostsType = UriUtils::HostType ( aHost );
		if ( ( aHostsType != UriUtils::ETextHost ) && ( aHostsType != UriUtils::EIPv4Host ) ) // is an IPv6 or other future protocol address
			{
			HBufC8* ipv6LiteralHost = HBufC8::NewLC ( aHost.Length() + 2 ); // add 2 for the braces

			TPtr8 ipv6LiteralHostPtr = ipv6LiteralHost->Des();

			ipv6LiteralHostPtr.Append ( KIPv6HostOpenBrace );
			ipv6LiteralHostPtr.Append ( aHost );
			ipv6LiteralHostPtr.Append ( KIPv6HostCloseBrace );

			hostValStr = iStringPool.OpenFStringL( ipv6LiteralHostPtr );
			CleanupStack::PopAndDestroy( ipv6LiteralHost );
			}
		else
			{
			hostValStr = iStringPool.OpenFStringL( aHost );
			}

		CleanupClosePushL ( hostValStr );

		THTTPHdrVal hostVal ( hostValStr );
		headers.SetFieldL ( hostStr, hostVal );
		CleanupStack::PopAndDestroy ( &hostValStr );

		// Also set the port number if Host header is not empty and a port
		// number is not the default.
		if ( aPort != KHttpDefaultPort )
			{
			THTTPHdrVal portVal ( aPort );
			RStringF portStr = iStringPool.StringF ( HTTP::EPort,THTTPTable::Table() );
			headers.SetParamL ( hostStr, portStr, portVal, 0 );
			}
		}
	}

void CHttpClientFlow::BuildSubscribeRequestL ( CRequest& aNewRequest, TUpnpMessage::TUPnPClientRequestInfo& aClntReqInfo )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::BuildSubscribeRequestL")));
	TSubscribeRequestInfo& info = *( reinterpret_cast < TSubscribeRequestInfo* > ( aClntReqInfo.iInfo ) );
	RStringF strF = iStringPool.StringF ( UPnP::ETimeout, TUPnPTable::Table() );
	if(info.iTimeOut != 0)
		{
		THTTPHdrVal hTimeOutVal(info.iTimeOut);
		aNewRequest.Handle().GetHeaderCollection().SetFieldL ( strF, hTimeOutVal );
		}

	strF = iStringPool.StringF ( UPnP::ECallback, TUPnPTable::Table() );
	THTTPHdrVal hCallbackVal;

	if ( info.iCallback.Length() > 0 )
		{
		RBuf8 callbackBuf;
		callbackBuf.CreateMax ( info.iCallback.Length( ) );
		CleanupClosePushL ( callbackBuf );
		callbackBuf.FillZ ( info.iCallback.Length( ) );
		info.iCallback.CopyOut ( callbackBuf );
		info.iCallback.Free ( );

		RStringF callbackStrF = iStringPool.OpenFStringL ( callbackBuf );
		CleanupClosePushL ( callbackStrF );
		hCallbackVal.SetStrF (callbackStrF );
		aNewRequest.Handle ( ).GetHeaderCollection ( ).SetFieldL ( strF, hCallbackVal );
		CleanupStack::PopAndDestroy ( &callbackStrF );
		CleanupStack::PopAndDestroy ( &callbackBuf );
		SetNtHeaderL ( aNewRequest.Handle ( ).GetHeaderCollection ( ) );
		}
	else
		{
		SetSidHeaderL ( aNewRequest, aClntReqInfo );
		}
	}

void CHttpClientFlow::SetSidHeaderL ( CRequest& aNewRequest, TUpnpMessage::TUPnPClientRequestInfo& aClntReqInfo )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::SetSidHeaderL")));
	if( aClntReqInfo.iRequestMethod == UPnP::ESubscribe || aClntReqInfo.iRequestMethod == UPnP::EUnsubscribe )
		{
		TSubscribeRequestInfo& info = *( reinterpret_cast < TSubscribeRequestInfo* > ( aClntReqInfo.iInfo ) );
		RStringF strF = iStringPool.StringF ( UPnP::ESID, TUPnPTable::Table() );
		THTTPHdrVal hSidVal;

		RBuf8 sidBuf;
		sidBuf.CreateMax ( info.iSid.Length( ) );
		CleanupClosePushL ( sidBuf );
		sidBuf.FillZ ( info.iSid.Length( ) );
		info.iSid.CopyOut( sidBuf );
		//info.iSid.Free ( );
		info.iSid.Init();
		TPtrC8 asdf(sidBuf);
		RStringF sidValStrF = iStringPool.OpenFStringL( sidBuf );
		CleanupClosePushL ( sidValStrF );
		hSidVal.SetStrF ( sidValStrF );
		aNewRequest.Handle ( ).GetHeaderCollection ( ).SetFieldL ( strF, hSidVal );
		CleanupStack::PopAndDestroy ( &sidValStrF );
		CleanupStack::PopAndDestroy ( &sidBuf );
		}
	else if ( aClntReqInfo.iRequestMethod == UPnP::ENotify )
		{
		TEventNotifyRequestInfo& info = *( reinterpret_cast < TEventNotifyRequestInfo* > ( aClntReqInfo.iInfo ) );
		RStringF strF = iStringPool.StringF ( UPnP::ESID, TUPnPTable::Table() );
		THTTPHdrVal hSidVal;

		RBuf8 sidBuf;
		sidBuf.CreateMax ( info.iSid.Length( ) );
		CleanupClosePushL ( sidBuf );
		sidBuf.FillZ ( info.iSid.Length( ) );
		info.iSid.CopyOut( sidBuf );
		//info.iSid.Free ( );
		info.iSid.Init();
		RStringF sidValStrF = iStringPool.OpenFStringL( sidBuf );
		CleanupClosePushL ( sidValStrF );
		hSidVal.SetStrF ( sidValStrF );
		aNewRequest.Handle ( ).GetHeaderCollection ( ).SetFieldL ( strF, hSidVal );
		CleanupStack::PopAndDestroy ( &sidValStrF );
		CleanupStack::PopAndDestroy ( &sidBuf );
		}
	}

void CHttpClientFlow::BuildNotifyRequestL ( CRequest& aNewRequest, TUpnpMessage::TUPnPClientRequestInfo& aClntReqInfo )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::BuildNotifyRequestL")));
	TEventNotifyRequestInfo& info = *( reinterpret_cast < TEventNotifyRequestInfo* > ( aClntReqInfo.iInfo ) );
	SetSidHeaderL ( aNewRequest, aClntReqInfo );
	SetNtHeaderL ( aNewRequest.Handle ( ).GetHeaderCollection ( ) );

	RStringF strF = iStringPool.StringF ( UPnP::ESEQ, TUPnPTable::Table() );
	THTTPHdrVal hSeqVal( info.iSeqKey );
	aNewRequest.Handle ( ).GetHeaderCollection ( ).SetFieldL ( strF, hSeqVal );

	strF = iStringPool.StringF ( UPnP::ENTS, TUPnPTable::Table() );
	RStringF ntsValStrF = iStringPool.StringF ( UPnP::EUpnpPropChange, TUPnPTable::Table() );
	THTTPHdrVal hNtsVal;
	hNtsVal.SetStrF ( ntsValStrF );
	aNewRequest.Handle ( ).GetHeaderCollection ( ).SetFieldL ( strF, hNtsVal );

	// Set content-type
	strF = iStringPool.StringF ( HTTP::EContentType, THTTPTable::Table() );
	RStringF contentTypeStrF = iStringPool.StringF( UPnP::ETextXml, TUPnPTable::Table() );
	THTTPHdrVal hContentTypeVal;
	hContentTypeVal.SetStrF ( contentTypeStrF );
	aNewRequest.Handle ( ).GetHeaderCollection ( ).SetFieldL ( strF, hContentTypeVal );
	}

void CHttpClientFlow::SetNtHeaderL ( RHTTPHeaders aHeaders )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::SetNtHeaderL")));
	RStringF strF = iStringPool.StringF ( UPnP::ENT, TUPnPTable::Table() );
	RStringF ntValStrF = iStringPool.StringF ( UPnP::EUpnpEvent, TUPnPTable::Table() );
	CleanupClosePushL ( ntValStrF );
	THTTPHdrVal hNtVal;
	hNtVal.SetStrF ( ntValStrF );
	aHeaders.SetFieldL ( strF, hNtVal );
	CleanupStack::PopAndDestroy ( &ntValStrF );
	}


MFlowBinderControl* CHttpClientFlow::DoGetBinderControlL ( )
	{	
    return this;
	}

// MFlowBinderControl
CSubConnectionFlowBase* CHttpClientFlow::Flow ( )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::Flow")));
	return this;
	}

MSessionControl* CHttpClientFlow::GetControlL ( TInt /*aSessionType*/, MSessionControlNotify& aSessionControlNotify )
    {	
    LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::GetControlL")));
    ASSERT ( iSessionControlNotify == NULL );
	iSessionControlNotify = &aSessionControlNotify;

	return this;
    }

MSessionData* CHttpClientFlow::BindL ( MSessionDataNotify& aNotify )
    {	
    LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::BindL")));
	ASSERT ( iSessionDataNotify == NULL );
	iSessionDataNotify = &aNotify;
	iSubConnectionProvider.PostMessage ( Id (), TCFControlProvider::TActive ().CRef () );
	return this;
    }

void CHttpClientFlow::Unbind ( )
    {	
    LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::Unbind")));
   	iSessionControlNotify = NULL;
	iSessionDataNotify = NULL;
	iDCIdle = ETrue;
	if ( iSubConnectionProvider.IsOpen ( ) )
		{
	    iSubConnectionProvider.PostMessage ( Id (), TCFControlProvider::TIdle ().CRef () );
		}
    }

// MSessionControl
void CHttpClientFlow::Shutdown ( MSessionControl::TCloseType aOption )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::Shutdown")));
	if ( MSessionControl::ENormal == aOption)
		iSessionControlNotify->CanClose( MSessionControlNotify::EDelete);
	}

void CHttpClientFlow::ActiveOpen ( )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::ActiveOpen")));
	iSessionControlNotify->ConnectComplete ( );
	}

TInt CHttpClientFlow::SetRemName ( TSockAddr& anAddr )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::SetRemName")));
	iRemoteAddress = anAddr;
	iClientHandler->SetConnectionInfo ( iRemoteAddress );
	return KErrNone;
	}

TInt CHttpClientFlow::GetOption ( TUint aLevel, TUint aName, TDes8& anOption ) const
	{	
	if ( aLevel == KCHOptionLevel )
		{
		switch ( aName )
			{
			case KCHMaxLength:
				{
				*(TUint*) anOption.Ptr() = iOverallDataSize;
				}
			break;

			case KCHLastMessage:
				{
				*(TUint*) anOption.Ptr() = ( iResponseCompleted && iBufChain.Length () == 0 );
				}
			break;

			default:
				{
				ASSERT ( 0 );
				}
			break;
			}
		}

	return KErrNone;
	}

TInt CHttpClientFlow::SetOption (TUint aLevel, TUint aName, const TDesC8& anOption)
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::SetOption")));

	TInt err = KErrNone;
	if ( aLevel == KCHOptionLevel )
		{
		switch ( aName )
			{
			case KCHAbsoluteUri:
				{
				err = iRequestUri.Create ( anOption );
				}
			break;

			case KCHMaxLength:
				{
				TPckgBuf <TCHMessageOption> option;
				option.Copy ( anOption );

				iChannelIdentifier = option ( ).iId;
				iOverallDataSize = option ( ).iValue;
				}
			break;

			default:
				{
				ASSERT ( 0 );
				}
			break;
			}
		}

	return err;
	}

TInt CHttpClientFlow::Write ( RMBufChain& aData, TUint /*aOptions*/, TSockAddr* /*anAddr*/ )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::Write")));

	// Note!..http client flow of upnp stack doesn't
	// supports HTTP chunked encoding for sending request body

	__ASSERT_DEBUG ( iOverallDataSize != KErrUnknown, User::Invariant () );

	TInt len = aData.Length ( );

	if ( iTransactions->Count( ) == 0 )
		{
		TRAP ( iError, PrepareAndSubmitTransactionL ( aData ) );
		if ( iError != KErrNone )
			{
			if ( !aData.IsEmpty() )
				aData.Free ( );
			DoReset ( );
			return iError;
			}
		}
	else
		{
		CTransaction* ctrans = iTransactions->Get( 0 );

		if ( iActionValue.Length ( ) == 0 )
			{			
			TBool doSubmit = EFalse;
			TRAP ( iError, doSubmit = SetActionHeaderL ( aData ) );
			
			// Append body parts
			ctrans->AddBodyPart ( aData );

			if ( iError == KErrNone )
				{
				// transaction is waiting for more data to build soap action header
				if ( doSubmit )
					{
					// Complete headers are available, so Transaction is ready for submission
					LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::Write - Submitting transaction")));
					iClientHandler->SubmitTransaction( ctrans );					
					}
				}
			else
				{
				DoReset ( );
				return iError;
				}
			}
		else
			{
			// transaction is waiting for more request body data
			LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::Write - Notfiy new request body parts")));
			iClientHandler->NotifyNewRequestBodyData ( ctrans, aData );		
			}
		SetTransactionComplete ( ctrans );
		}	
	
	aData.Init ( );
	return len;
	}

TInt CHttpClientFlow::GetData ( RMBufChain& aData, TUint aLength, TUint /*aOptions*/, TSockAddr* /*anAddr*/ )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::GetData")));
	
	if ( iError < 0 )
		{
		DoReset ();
		return iError;
		}
		
	if ( !iReadStreamStarted )
		{
		TBuf8<sizeof ( TInt )> identifiersBuf;
		identifiersBuf.AppendNumFixedWidth ( iChannelIdentifier, EDecimal, sizeof ( TInt ) );
		aData.Append ( identifiersBuf.Length () );
		aData.CopyIn ( identifiersBuf );
		iReadStreamStarted = ETrue;
		}
	else
		{
		RMBufChain newChain;
		iError = iBufChain.Split ( aLength, newChain );
		if ( iError != KErrNone )
			{
			DoReset ();
			return iError;
			}
		
		aData.Assign ( iBufChain );
		iBufChain = newChain;
		}

	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::GetData--Data Read by Upper flow--%d"), aData.Length ( )));
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::GetData--Data left over--%d"), iBufChain.Length ( )));
	
	return aData.Length ( );
	}

void CHttpClientFlow::SetTransactionComplete ( CTransaction* aTransaction )
	{
	if ( iOverallDataSize == aTransaction->BodyParts ().Length () )
		{
		aTransaction->SetComplete ();
		}
	}

// MInputStreamObserver
TInt CHttpClientFlow::OnHttpEvent ( CTransaction* aTransaction, THTTPEvent& aEvent )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::OnHttpEvent")));
	CHTTPClientTransaction* clientTrans = static_cast<CHTTPClientTransaction*>( aTransaction );

	// Process event for Control plane transaction
	TNodeCtxId selfCtx ( MeshMachine::KActivityNull, NodeId () );
	if ( clientTrans->NodeCtxId () != selfCtx )
		{
		ProcessControlPlaneResponseEvent ( clientTrans, aEvent );
		return KErrNone;
		}

	// Process event for Socket Transaction
	TBool cleanupNeeded = EFalse;
	switch ( aEvent.iStatus )
		{
		case THTTPEvent::EGotResponseHeaders:
			{
			RResponse response = clientTrans->Response( )->Handle( );
			RHTTPHeaders headers = response.GetHeaderCollection ( );

			RStringF fieldStr = iStringPool.StringF ( HTTP::EContentLength, THTTPTable::Table ( ) );
			THTTPHdrVal value;

			if ( headers.GetField ( fieldStr, 0, value ) == KErrNotFound )
				{
				iOverallDataSize = KErrUnknown;
				}
			else
				{
				iOverallDataSize = value.Int ( );
				}			
			}
			break;

		case THTTPEvent::EGotResponseBodyData:
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::OnHttpEvent - THTTPEvent::EGotResponseBodyData")));
			
			TInt notifyLen = 0;
			FillResponseChain ( aTransaction, notifyLen );			
			iSessionDataNotify->NewData( notifyLen );
			}
			break;
		
		case THTTPEvent::EResponseComplete:
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::OnHttpEvent - THTTPEvent::EResponseComplete")));
			if ( clientTrans->Response( )->Status( ) == HTTPStatus::EMethodNotAllowed )
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::OnHttpEvent - THTTPEvent::EResponseComplete - Response status = %d"), clientTrans->Response( )->Status( )));
				
				clientTrans->SetComposingStarted ( EFalse );
				clientTrans->ResetRequestPart ();
				TRAPD ( err, ReSubmitTransactionL ( clientTrans ) );

				if ( err != KErrNone )
					{
					LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::OnHttpEvent - THTTPEvent::EResponseComplete - Error %d while re-submitting trans"), err));
					cleanupNeeded = ETrue;
					NotifyError ( clientTrans, err );
					}
				}
			else
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::OnHttpEvent - THTTPEvent::EResponseComplete - Response status = %d"), clientTrans->Response( )->Status( )));
								
				iResponseCompleted = ETrue;
				cleanupNeeded = ETrue;
				iSessionDataNotify->NewData( sizeof ( TInt ) );
				}
			}
			break;

		default:
			{
			if ( aEvent.iStatus < 0 )
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::OnHttpEvent - Error status = %d"), aEvent.iStatus));
				NotifyError ( clientTrans, aEvent.iStatus );
				}
			}
			break;
		}
	
	if ( cleanupNeeded )
		{
		// body is processed completely, and clienthandler removed it from its array
		delete clientTrans;
		}
	return KErrNone;
	}

void CHttpClientFlow::ProcessControlPlaneResponseEvent ( CHTTPClientTransaction* aTransaction, THTTPEvent& aEvent )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::ProcessControlPlaneResponseEvent")));
	TBool iCleanupNeeded = EFalse;

	switch ( aEvent.iStatus )
		{
		case THTTPEvent::ECancel:
			{
			// post complete message with KErrCancel
			LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::ProcessControlPlaneResponseEvent - THTTPEvent::ECancel")));		
			RClientInterface::OpenPostMessageClose ( NodeId (), aTransaction->NodeCtxId (), TCFDataClient::TStopped ( 0 ).CRef () );
			iCleanupNeeded = ETrue;
			}
		break;

		case THTTPEvent::EGotResponseBodyData:
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::ProcessControlPlaneResponseEvent - THTTPEvent::EGotResponseBodyData")));

			TInt notifyLen = 0;
			FillResponseChain ( aTransaction, notifyLen );
			// Note! no need to process any body parts for control plane transaction
			}
		break;

		case THTTPEvent::EResponseComplete:
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::ProcessControlPlaneResponseEvent - THTTPEvent::EResponseComplete")));
			CResponse* cresponse = aTransaction->Response( );
			RMemoryAllocator allocator(static_cast<CUPnPProtocolIntfBase *>( ProtocolIntf () )->GetMemoryChunkManager());

			RMemChunk memChunk;
 			TUPnPMemoryUtils::CreateMemChunk(memChunk, iBufChain, allocator );

			// request method is subscribe
			if ( aTransaction->Request( )->Method( ).Index ( TUPnPTable::Table() )
									== UPnP::ESubscribe )
				{
				if(cresponse->Status() == HTTPStatus::EOk)
					{
					RHTTPHeaders headers = cresponse->Handle( ).GetHeaderCollection ( );

		  			// Timeout field value
		  			RStringF fieldNameStr = iStringPool.StringF ( UPnP::ETimeout, TUPnPTable::Table() );
		  			THTTPHdrVal timeOutVal;
		 			headers.GetField ( fieldNameStr, 0, timeOutVal );
					TInt timeOut;					
					if ( timeOutVal.Type ( ) == THTTPHdrVal::KNoType )
						{
						RHTTPHeaders reqHds = aTransaction->Request( )->Handle( ).GetHeaderCollection ( );
						reqHds.GetField ( fieldNameStr, 0, timeOutVal );
						timeOut = timeOutVal.Int ( );
						}
					else
						{
						timeOut = timeOutVal.Int ( );
						}
						
		 			// SID field value
		 			fieldNameStr = iStringPool.StringF ( UPnP::ESID, TUPnPTable::Table() );
		 			TPtrC8 rawValue;
		 			headers.GetRawField ( fieldNameStr, rawValue );
		 			
		 			RMemChunk sid;
		 			sid.Create ( rawValue, allocator );
		 			
		 			
					TUpnpMessage::TSubscribeResponse msg ( cresponse->Status( ),
															cresponse->Version( ),
															memChunk,
															timeOut,
															sid );					
					RClientInterface::OpenPostMessageClose ( NodeId (), aTransaction->NodeCtxId (), msg );			
					}
				else
					{
					TEBase::TError msg ( TUpnpMessage::TUPnPClientRequestInfo::Id(), cresponse->Status() );
					RClientInterface::OpenPostMessageClose ( NodeId (), aTransaction->NodeCtxId (), msg );
					}
				}
			else
				{
				TUpnpMessage::TResponse msg ( cresponse->Status( ), cresponse->Version( ), memChunk );
				RClientInterface::OpenPostMessageClose ( NodeId (), aTransaction->NodeCtxId (), msg );					
				}
			iBufChain.Free ( );
			iCleanupNeeded = ETrue;
			}
		break;

		default:
			{
			if ( aEvent.iStatus < 0 )
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::ProcessControlPlaneResponseEvent - Error status = %d"), aEvent.iStatus));
				NotifyError ( aTransaction, aEvent.iStatus );
				iCleanupNeeded = ETrue;
				}
			}
		break;
		}

	if ( iCleanupNeeded )
		{
		// Cleanup
		delete aTransaction;
		}
	}

void CHttpClientFlow::FillResponseChain ( CTransaction* aTransaction, TInt& aNewLen )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::FillResponseChain")));
	MHTTPDataSupplier* bodySupplier = aTransaction->Response( )->Body( );
	TPtrC8 bodyPtr;
	RMBufChain bodyChain;

	bodySupplier->GetNextDataPart ( bodyPtr );
	bodyChain.Create ( bodyPtr );
	iBufChain.Append ( bodyChain );

	bodySupplier->ReleaseData( );

	aNewLen = bodyPtr.Length ( );

	LOG(ESockLogExternal::Printf(KSubsysHttpClntFlow, KComponent, _L8("CHttpClientFlow::FillResponseChain--chunk length from parser = %d"), aNewLen));
	}

void CHttpClientFlow::NotifyError ( CHTTPClientTransaction* aTransaction, TInt aError )
	{		
	TNodeCtxId selfCtx ( MeshMachine::KActivityNull, NodeId () );
	if ( aTransaction->NodeCtxId () != selfCtx )
		{
		// For SCPR Transactions post error response and cleanup
		TEBase::TError msg ( TUpnpMessage::TUPnPClientRequestInfo::Id ( ), aError );
		RClientInterface::OpenPostMessageClose ( NodeId (), aTransaction->NodeCtxId (), msg );		
		}
	else
		{
		// Handle error for socket transactions
		iError = aError;
		iResponseCompleted = ETrue;
		iOverallDataSize = sizeof ( TInt );
		iSessionDataNotify->NewData( iOverallDataSize );
		}		
	}

void CHttpClientFlow::DoReset ( )
	{	
	iResponseCompleted = EFalse;
	iOverallDataSize = 0;

	iRequestUri.Close ( );
	iActionValue.Close ( );
	iRemoteAddress.FillZ ( );
	iBufChain.Free ( );

	if ( iTransactions->Count() )
		{
		CHTTPClientTransaction* ctrans = iTransactions->Get( 0 );
		delete ctrans;
		iTransactions->Remove( 0 );
		}
	}
