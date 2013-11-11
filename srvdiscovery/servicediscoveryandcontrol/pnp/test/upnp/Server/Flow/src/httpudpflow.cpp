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
// upnpflow.cpp
// @internalComponent
// 
//

//System Includes
#include <comms-infras/ss_nodeinterfaces.h>


#include <http/thttptable.h>
#include <http/framework/cheadercodecplugin.h>
#include <httpstringconstants.h>
#include <inetprottextutils.h>
#include <upnp/tupnptable.h>

//Local Includes
#include "httpudpflow.h"
#include "upnppint.h"
#include "upnp_cf_msgs.h"
#include "upnpctrlscprstates.h"
#include "upnpserverconstants.h"
#include "upnplog.h"
#include "upnpmemoryutils.h"

const TUint KDeftMaxAgeVal = 1800;
const TUint KDeftSendCount = 9;
const TUint KDeftTimeOutVal = 200000;	//200ms,Requirements [7.2.4.2] & [7.2.4.7] of DLNA

__FLOG_STMT(_LIT8(KComponent,"Flow");)

CHttpUdpFlow* CHttpUdpFlow::NewL ( CSubConnectionFlowFactoryBase& aFactory,
						  			CUPnPProtocolIntfBase* aProtocolIntf,
					  			const TNodeId& aSubConnId )
	{
	CHttpUdpFlow* self = new ( ELeave )CHttpUdpFlow ( aFactory, aProtocolIntf, aSubConnId );
	CleanupStack::PushL ( self );
	self->ConstructL ();
	CleanupStack::Pop (); // self
	return self;
	}

CHttpUdpFlow::CHttpUdpFlow ( CSubConnectionFlowFactoryBase& aFactory,
					CUPnPProtocolIntfBase* aProtocolIntf,
					const TNodeId& aSubConnId )
	: CUPnPFlowBase ( aFactory, aProtocolIntf, EHttpUdpFlow, aSubConnId ),
	iSocketHandler ( *this ), iSocketCreated ( EFalse ),iSendCount ( 0 ),  
	iIsLeaving ( EFalse ),iSendFlag ( EFalse ),iIsComposeCompleted ( EFalse )
	{
	LOG_NODE_CREATE(KESockFlowTag, CHttpUdpFlow);
	}

CHttpUdpFlow::~CHttpUdpFlow ()
	{
	delete iRequestComposer;
	delete iResponseParser;
	delete iResponseComposer;
	delete iSendTimer;

	delete iSocketOpener;
	iSocketHandler.CancelAll ();

	iSocket.Close ();
	iSearchTargetArray.Close ();
	delete iUPnPResponse;
	//delete iCodec;
	iSendElements.Close ();
	iStringPool.Close();
	iSubConnectionProvider.Close ();
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("Destroyed CHttpUdpFlow")));
	LOG_NODE_DESTROY(KESockFlowTag, CHttpUdpFlow);
	}


void CHttpUdpFlow::ConstructL ()
	{
	iStringPool.OpenL ( TUPnPTable::Table() );
	iStringPool.OpenL(THTTPTable::Table());
	iCodec = CHeaderCodecPlugin::NewL ( KUPnPCodecName, iStringPool );
	iResponseParser = CUpnpResponseParser::NewL ( *this );
	iRequestComposer = CUpnpRequestComposer::NewL ( *this );
	iResponseComposer = CUpnpResponseComposer::NewL ( *this );
	iUPnPResponse = CResponse::NewL ( *iCodec, iStringPool );
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("Created CHttpUdpFlow")));
	}

void CHttpUdpFlow::ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::ReceivedL")));
	
	if ( aMessage.IsMessage<TEChild::TDestroy> () )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::ReceivedL - TCFMessage::TDestroy")));
    	if ( RemoveControlClient ( FindControlClient ( address_cast<TNodeId> ( aSender ) ) ) == 0 )
    		{
    		LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::ReceivedL - TCFMessage::TDestroy - Deleting current flow")));
    		DeleteThisFlow ();
    		}
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStart> () )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::ReceivedL - TCFMessage::TDataClientStart")));
        RClientInterface::OpenPostMessageClose ( NodeId (), address_cast<TNodeCtxId> ( aSender ), TCFDataClient::TStarted ().CRef () );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStop> () )
		{
       	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::ReceivedL - TUpnpMessage::TUPnPStopSearch")));

    	SetClientLeaving ( address_cast<TNodeId> ( aSender ) ); // set the control client leaving

		if ( CheckIfAllClientsLeaving ( ) && iSendElements.Count () != 0 )
		   	{
		   	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::ReceivedL - TUpnpMessage::TUPnPStopSearch -- Marked for Deletion")));

		   	MarkMeForDeletion ();
		   	iIsLeaving = ETrue;
			iLastLeavingClient = address_cast<TNodeCtxId> ( aSender );
		   	}
		else
			{
		    PostDataClientStopped ( address_cast<TNodeCtxId> ( aSender ) );
			}
		}
	else if ( TUpnpMessage::ERealmId == aMessage.MessageId( ).Realm( ) )		
    	{
	   	switch ( aMessage.MessageId( ).MessageId( ) )
			{
			case TUpnpMessage::TUPnPSearchRequest::EId:
            	{
            	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::ReceivedL - TUpnpMessage::TUPnPSearchRequest")));
            	HandleSearchRequestL ( address_cast<TNodeCtxId> ( aSender ), static_cast < TUpnpMessage::TUPnPSearchRequest& > ( aMessage ) );
            	CreateSendTimerL();
				SendIfOneElement ();
            	}
            break;

			case TUpnpMessage::TCancelRequest::EId:
				{
            	HandleStopSearch ( address_cast<TNodeCtxId> ( aSender ) );
            	}
			break;

            case TUpnpMessage::TUPnPSearchResponse::EId:
            	{
            	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::ReceivedL - TUpnpMessage::TUPnPSearchResponse")));
            	HandleSearchResponseL ( static_cast < TUpnpMessage::TUPnPSearchResponse& > ( aMessage ) );
				SendIfOneElement ();
            	}
            break;

            case TUpnpMessage::TUPnPPublishAliveRequest::EId:
            	{
            	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::ReceivedL - TUpnpMessage::TUPnPPublishAliveRequest")));
            	HandlePublishAliveRequestL ( static_cast < TUpnpMessage::TUPnPPublishAliveRequest& > ( aMessage ) );
            	CreateSendTimerL();
            	SendIfOneElement ();
            	}
            break;

            case TUpnpMessage::TUPnPPublishByeRequest::EId:
            	{
            	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::ReceivedL - TUpnpMessage::TUPnPPublishByeRequest")));
            	HandlePublishByeRequestL ( static_cast < TUpnpMessage::TUPnPPublishByeRequest& > ( aMessage ) );
            	SendIfOneElement ();
            	}
            break;
	        }
		}
	}

// From MSocketHandlerObserver
void CHttpUdpFlow::OpenComplete ( RInternalSocket& aSocket )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::OpenComplete")));
	iSocket = aSocket;
	TInt err = KErrNone;
	TUint port = KUpnpUdpPort;
	do
		{
		// [REF] DLNA Requirement-7.2.3.4
		if ( port == KUpnpMCPort )
			++port;

		TInetAddr addr ( port++ );
		addr.SetAddress ( KInetAddrAny );

		err = aSocket.Bind( addr );

		}while ( err == KErrInUse && port != KMaxTUint );

	if ( err == KErrInUse || port == KMaxTUint )
		{
		return; // bind error is ignored, needs to be handled
		}

	iSocketHandler.Attach ( aSocket );
	iSocketCreated = ETrue;
	HandleSendElement (); // This call will happen only once.
	StartRecv(); //keep Socket in receive mode
	}

void CHttpUdpFlow::SendToComplete ( TInt /*aLength*/ )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::SendToComplete")));

	if ( !iSendFlag && iSendElements[0]->iType != CSendElement::EMSearchResponse )
		{
		IncrementSendCount();
		iSendFlag = ETrue;
		iSocketHandler.SendTo ( iBackupData, iSendElements[0]->iSockAddr );
		}
	else
		{
		iBackupData.Init ();
		iSendFlag = EFalse;

		iIsComposeCompleted = ETrue;
		CSendElement::TSendType currentSendType = RemoveFirstSendElement ();
		if ( CSendElement::EMSearchResponse == currentSendType )
			{
			//M-Serach Response
			iResponseComposer->ResponseDataSent ();
			iResponseComposer->ResetComposer ();
			}
		else
			{
			//Publish
			iRequestComposer->RequestDataSent();
			iRequestComposer->ResetComposer ();
			}

		HandleSendElement ();
		}
	}

void CHttpUdpFlow::RecvFromComplete ( RMBufChain& aData, const TSockAddr& /*aAddr*/ )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::RecvFromComplete")));

	RMemoryAllocator allocator(static_cast<CUPnPProtocolIntfBase *>( ProtocolIntf () )->GetMemoryChunkManager());
	RMemChunk memChunk;
	TUPnPMemoryUtils::CreateMemChunk(memChunk, aData, allocator);

	iResponseParser->ParseResponse ( memChunk, iUPnPResponse );
	
	aData.Free();
	}

// From MComposerObserver
void CHttpUdpFlow::MessageDataReadyL ( RBuf8& aData )
	{
	ASSERT ( iSendElements.Count() > 0 );

	if ( !iSendFlag && iSendElements[0]->iType != CSendElement::EMSearchResponse)
		{
		iBackupData.CreateL(aData);
		}
	RMBufChain sendData;
	sendData.CreateL(aData);
	iSocketHandler.SendTo ( sendData, iSendElements[0]->iSockAddr );
	}

void CHttpUdpFlow::ComposingConcluded ()
	{
	if ( iIsLeaving && iSendElements.Count () == 0 )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::ComposingConcluded - Deleting current flow")));

		PostDataClientStopped ( iLastLeavingClient );
		}
	}

void CHttpUdpFlow::ComposerError ( TInt /*aError*/ )
	{
	RemoveFirstSendElement ();
	HandleSendElement ();
	}

// From MParserObserver
void CHttpUdpFlow::GotHeaders ()
	{
	if ( ValidateSearchResponse () != KErrNone )
		{
		// Corrupt message. Reset the parser and restart the receive
		StartRecv ();
		}
	// Else we will notify the SCPR from ::ParsingComplete
	}

void CHttpUdpFlow::ParsingComplete ( RMemChunk& /*aExcessData*/ )
	{
	TRAP_IGNORE ( NotifyClientsL () );
	StartRecv ();
	iUPnPResponse->Reset();
	}

void CHttpUdpFlow::ParserError ( TInt /*aError*/ )
	{
	StartRecv ();
	}

TInt CHttpUdpFlow::GetHeaderValue ( const CResponse& aResponse, TInt aFieldIndex, THTTPHdrVal& aFieldVal, const TStringTable& aTable )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::GetHeaderValue")));
	CResponse& response = const_cast < CResponse& > ( aResponse );
	RResponse res = response.Handle ();
	RHTTPHeaders headers = res.GetHeaderCollection ();
	RStringF fieldStr = aResponse.StringPool ().StringF ( aFieldIndex, aTable );
	return headers.GetField ( fieldStr, 0, aFieldVal );
	}

TInt CHttpUdpFlow::GetParamValue ( const CResponse& aResponse, TInt aFieldIndex, TInt aParamIndex, THTTPHdrVal& aParamVal,  const TStringTable& aTable )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::GetParamValue")));
	CResponse& response = const_cast < CResponse& > ( aResponse );
	RResponse res = response.Handle ();
	RHTTPHeaders headers = res.GetHeaderCollection ();
	RStringF fieldStr = aResponse.StringPool ().StringF ( aFieldIndex, aTable );
	RStringF paramStr = aResponse.StringPool ().StringF ( aParamIndex, aTable );
	TInt err = KErrNotFound;
	TInt fieldCount =  0;
	TRAP ( err, fieldCount = headers.FieldPartsL(fieldStr) );
	while (fieldCount>0)
		{
		err = headers.GetParam ( fieldStr, paramStr, aParamVal, fieldCount-1 );
		if(err == KErrNone )
			break;
		fieldCount--;
		}
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::GetParamValue - Returned with error code %d"), err));
	return err;
	}

TInt CHttpUdpFlow::IsHeaderPresent ( const CResponse& aResponse, TInt aFieldIndex, const TStringTable& aTable )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::IsHeaderPresent")));
	THTTPHdrVal value;
	return GetHeaderValue ( aResponse, aFieldIndex, value, aTable );
	}

TInt CHttpUdpFlow::IsValidCacheControlHeader ( const CResponse& aResponse )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::IsValidCacheControlHeader")));
	THTTPHdrVal value;
	return GetParamValue ( aResponse, HTTP::ECacheControl, HTTP::EMaxAge, value, THTTPTable::Table() );
	}


void CHttpUdpFlow::SetHeaderValueL ( RHTTPHeaders& aHdr, TInt aFieldIndex, const TDesC8& aFieldVal, const TStringTable& aTable )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::SetHeaderValueL")));
	RStringF hdrValStr = iStringPool.OpenFStringL ( aFieldVal );
	THTTPHdrVal hdrVal ( hdrValStr );
	aHdr.SetFieldL ( iStringPool.StringF ( aFieldIndex, aTable ), hdrVal );
	hdrValStr.Close();
	}

TInt CHttpUdpFlow::ValidateSearchResponse ()
	{
	// 1. MUST be a HTTP/1.1 version
	// 2. MUST contain cache-control header with max-age directive
	// 3. EXT header MUST be present
	// 4. Location header MUST be present
	// 5. ST header MUST be present
	// 6. USN header MUST be present
	// 7. No body
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::ValidateSearchResponse")));

	if ( !iUPnPResponse->IsHTTP10() && IsValidCacheControlHeader ( *iUPnPResponse ) == KErrNone &&
		IsHeaderPresent ( *iUPnPResponse, UPnP::EExt, TUPnPTable::Table() ) == KErrNone &&
		IsHeaderPresent ( *iUPnPResponse, HTTP::ELocation, THTTPTable::Table() ) == KErrNone &&
		IsHeaderPresent ( *iUPnPResponse, UPnP::EST, TUPnPTable::Table() ) == KErrNone &&
		IsHeaderPresent ( *iUPnPResponse, UPnP::EUSN, TUPnPTable::Table() ) == KErrNone &&
		IsContentLengthZero ( *iUPnPResponse ) )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::ValidateSearchResponse - Returned with KErrNone")));
		return KErrNone;
		}
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::ValidateSearchResponse - Returned with KErrCorrupt")));
	return KErrCorrupt;
	}

void CHttpUdpFlow::ReadResponseValues ( TInt& aMaxAge, RStringF& aLocation, RStringF& aUsn, RStringF& aST )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::ReadResponseValues")));
	// We already checked the presence of these headers. We can just read those here
	// Read maxAge, Location & USN values from the response
	THTTPHdrVal value(KDeftMaxAgeVal);
	GetParamValue ( *iUPnPResponse, HTTP::ECacheControl, HTTP::EMaxAge, value, THTTPTable::Table() );
	aMaxAge = value.Int ();

	GetHeaderValue ( *iUPnPResponse, HTTP::ELocation, value, THTTPTable::Table() );
	aLocation = value.StrF ();

	GetHeaderValue ( *iUPnPResponse, UPnP::EUSN, value, TUPnPTable::Table() );
	aUsn = value.StrF ();

	GetHeaderValue ( *iUPnPResponse, UPnP::EST, value, TUPnPTable::Table() );
	aST = value.StrF ();
	}

void CHttpUdpFlow::NotifyClientsL ()
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::NotifyClients")));
	TInt maxAge;
	RStringF location;
	RStringF usn;
	RStringF st;
	ReadResponseValues ( maxAge, location, usn, st );
	for ( TInt i = 0; i < iSearchTargetArray.Count (); ++i )
		{
		const CSearchTarget& target = *(iSearchTargetArray[i]);
		if ( target.Match ( st.DesC() ) != KErrNotFound
							&& !IsClientLeaving ( target.Originator () ) )
			{
			// We have a match. Post the information to our control client
			RMemoryAllocator allocator(static_cast<CUPnPProtocolIntfBase *>( ProtocolIntf () )->GetMemoryChunkManager());
			
			RMemChunk locBuf;
			RMemChunk usnBuf;
			RMemChunk stBuf;
			locBuf.CreateL ( location.DesC (), allocator );
			TCleanupItem item1 ( &UPnPStateMachine::CUPnPUtils::CleanupMBufChain, &locBuf );
			CleanupStack::PushL ( item1 );
			usnBuf.CreateL ( usn.DesC (), allocator );
			TCleanupItem item2 ( &UPnPStateMachine::CUPnPUtils::CleanupMBufChain, &usnBuf );
			CleanupStack::PushL ( item2 );
			stBuf.CreateL ( st.DesC (), allocator );
			TCleanupItem item3 ( &UPnPStateMachine::CUPnPUtils::CleanupMBufChain, &stBuf );
			CleanupStack::PushL ( item3 );

			TSsdpInfo info ( maxAge, locBuf, usnBuf, stBuf );
			
			LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::NotifyClients - Posting message TUpnpMessage::TUPnPSearchResponse")));
			TUpnpMessage::TUPnPSearchResponse msg ( info );
	        RClientInterface::OpenPostMessageClose ( NodeId (), target.Originator (), msg );

			CleanupStack::Pop (); // item1
			CleanupStack::Pop (); // item2
			CleanupStack::Pop (); // item3
			}
		}
	}

void CHttpUdpFlow::StartRecv ()
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::StartRecv")));
	// Reset the parser and recv
	iResponseParser->ResetParser ();
	iSocketHandler.RecvFrom ();
	}

// -----------------------
// The below functions to be genralised
void CHttpUdpFlow::HandleSearchRequestL ( TNodeCtxId aChannelId, TUpnpMessage::TUPnPSearchRequest& aMessage )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::HandleSearchRequestL")));
	_LIT8 ( KMultiCastAddr, "239.255.255.250:1900" );
	_LIT8 ( KManHeaderValue, "ssdp:discover" );
	_LIT8 ( KSearchUri, "*" );
	CRequest* searchRequest = CRequest::NewL ( *iCodec, iStringPool );
	CleanupStack::PushL ( searchRequest );

	RStringF method = iStringPool.StringF ( UPnP::EMSearch, TUPnPTable::Table() );
	// Set the method and URI
	RRequest req = searchRequest->Handle ();
	req.SetMethod ( method );
	TUriParser8 parser;
	parser.Parse ( KSearchUri () );
	req.SetURIL ( parser );

	RHTTPHeaders hdr = searchRequest->Handle ().GetHeaderCollection ();

	// Set host header
	SetHeaderValueL ( hdr, HTTP::EHost, KMultiCastAddr(), THTTPTable::Table() );

	// Set MAN header
	SetHeaderValueL ( hdr, UPnP::EMAN, KManHeaderValue(), TUPnPTable::Table() );


	// Set MX header
	THTTPHdrVal mxVal ( aMessage.iSsdpInfo.iMx );
	hdr.SetFieldL ( iStringPool.StringF ( UPnP::EMX, TUPnPTable::Table() ), mxVal );

	// Set ST header
	TPtrC8 stPtr ( aMessage.iSsdpInfo.iSearchTarget.First ()->Ptr (), aMessage.iSsdpInfo.iSearchTarget.First ()->Length () );
	SetHeaderValueL ( hdr, UPnP::EST, stPtr, TUPnPTable::Table() );

	CSearchTarget* searchTarget = new ( ELeave ) CSearchTarget ( aChannelId );
	CleanupStack::PushL ( searchTarget );
	searchTarget->AddTargetL ( aMessage.iSsdpInfo.iSearchTarget );
	iSearchTargetArray.AppendL ( searchTarget );
	CleanupStack::Pop (); // searchTarget
	TAppProtAddr addr ( KSsdpMulticastAddr, KUpnpMCPort );

	CSendElement* sendElement = new ( ELeave ) CSendElement ( CSendElement::EMSearchRequest, *searchRequest, addr );
	CleanupStack::PushL ( sendElement );
	iSendElements.AppendL ( sendElement );
	CleanupStack::Pop (); // sendElement

	CleanupStack::Pop (); // searchRequest
	}

void CHttpUdpFlow::HandleStopSearch ( TNodeCtxId aChannelId )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::HandleStopSearch")));

	TInt pos = FindSearchTarget ( aChannelId );

	if ( pos != KErrNotFound )
		{
		CSearchTarget *target = iSearchTargetArray[pos];
		iSearchTargetArray.Remove ( pos );
		delete target;

		RClientInterface::OpenPostMessageClose ( NodeId (), aChannelId, TUpnpMessage::TCancelled ().CRef () );
		}
	}

void CHttpUdpFlow::HandleSearchResponseL ( TUpnpMessage::TUPnPSearchResponse& aMessage )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::HandleSearchResponseL")));
	CResponse* searchResponse = CResponse::NewL ( *iCodec, iStringPool );

	RResponse response = searchResponse->Handle ();
	RHTTPHeaders hdr = response.GetHeaderCollection ();

	const TInt KStatusCode = 200;
	_LIT8 ( KStatusText, "OK" );
	// Set status code & text
	response.SetStatusCode ( KStatusCode );

	RStringF statusTextStr = iStringPool.OpenFStringL ( KStatusText() );
	CleanupClosePushL ( statusTextStr );
	response.SetStatusText ( statusTextStr );
	CleanupStack::PopAndDestroy (); // statusTextStr

	// Set CACHE-CONTROL: max-age=""
	SetMaxAgeHeaderValueL ( aMessage.iSsdpInfo.iMx, hdr );

	// Set Date header
	TTime time;
	time.UniversalTime();
	THTTPHdrVal timeHdrVal ( time.DateTime() );
	hdr.SetFieldL ( iStringPool.StringF(HTTP::EDate, THTTPTable::Table() ), timeHdrVal );

	// set EXT header
	SetHeaderValueL ( hdr, UPnP::EExt, KNullDesC8 (), TUPnPTable::Table() );

	// Set location str
	TPtrC8 locationPtr ( aMessage.iSsdpInfo.iLocation.First ()->Ptr (), aMessage.iSsdpInfo.iLocation.First ()->Length () );
	SetHeaderValueL ( hdr, HTTP::ELocation, locationPtr, THTTPTable::Table() );

	// Set ST str
	TPtrC8 stPtr ( aMessage.iSsdpInfo.iSearchTarget.First ()->Ptr (), aMessage.iSsdpInfo.iSearchTarget.First ()->Length () );
	SetHeaderValueL ( hdr, UPnP::EST, stPtr, TUPnPTable::Table() );

	// Set USN str
	TPtrC8 usnPtr ( aMessage.iSsdpInfo.iUsn.First ()->Ptr (), aMessage.iSsdpInfo.iUsn.First ()->Length () );
	SetHeaderValueL ( hdr, UPnP::EUSN, usnPtr, TUPnPTable::Table() );

	//Set Content Length
	hdr.SetFieldL ( iStringPool.StringF ( HTTP::EContentLength, THTTPTable::Table() ), THTTPHdrVal ( 0 ) );

	CSendElement* sendElement = new ( ELeave ) CSendElement ( CSendElement::EMSearchResponse, *searchResponse, aMessage.iSsdpInfo.iAddr );
	iSendElements.AppendL ( sendElement );
	}



void CHttpUdpFlow::HandlePublishAliveRequestL ( TUpnpMessage::TUPnPPublishAliveRequest& aMessage )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::HandlePublishAliveRequestL")));
	CRequest& request = CreatePublishRequestLC ();

	RHTTPHeaders hdr = request.Handle ().GetHeaderCollection ();
	// Set CACHE-CONTROL: max-age=""
	SetMaxAgeHeaderValueL ( aMessage.iSsdpInfo.iMx, hdr );

	// Set location str
	TPtrC8 locationPtr ( aMessage.iSsdpInfo.iLocation.First ()->Ptr (), aMessage.iSsdpInfo.iLocation.First ()->Length () );
	SetHeaderValueL ( hdr, HTTP::ELocation, locationPtr, THTTPTable::Table() );

	// Set NT str
	TPtrC8 ntPtr ( aMessage.iSsdpInfo.iSearchTarget.First ()->Ptr (), aMessage.iSsdpInfo.iSearchTarget.First ()->Length () );
	SetHeaderValueL ( hdr, UPnP::ENT, ntPtr, TUPnPTable::Table() );

	SetHeaderValueL ( hdr, UPnP::ENTS, KSsdpAlive(), TUPnPTable::Table() );
	TPtrC8 usnPtr ( aMessage.iSsdpInfo.iUsn.First ()->Ptr (), aMessage.iSsdpInfo.iUsn.First ()->Length () );
	SetHeaderValueL ( hdr, UPnP::EUSN, usnPtr, TUPnPTable::Table() );

	//Set Server
	_LIT8 ( KServerName, "Symbian UPnP/1.0 server" );
	RStringF serverStr = iStringPool.OpenFStringL ( KServerName () );
	THTTPHdrVal serverHdrVal ( serverStr );
	hdr.SetFieldL ( iStringPool.StringF ( HTTP::EServer, THTTPTable::Table () ), serverHdrVal );
	serverStr.Close ();

	//Set Content Length
	hdr.SetFieldL ( iStringPool.StringF ( HTTP::EContentLength, THTTPTable::Table() ), THTTPHdrVal ( 0 ) );

	TAppProtAddr addr ( KSsdpMulticastAddr, KUpnpMCPort  );
	CSendElement* sendElement = new ( ELeave ) CSendElement ( CSendElement::ENotifyAliveRequest, request, addr );
	iSendElements.AppendL ( sendElement );

	CleanupStack::Pop (); //request
	}

void CHttpUdpFlow::SetMaxAgeHeaderValueL ( TInt aMaxAgeVal, RHTTPHeaders aHeaders )
	{
	_LIT8 ( KMaxAge, "max-age=" );

	RStringF cacheControl = iStringPool.StringF( HTTP::ECacheControl, THTTPTable::Table() );
	HBufC8* buf;
	InetProtTextUtils::ConvertIntToDescriptorL ( aMaxAgeVal, buf );
	CleanupStack::PushL ( buf );
	RBuf8 rBuf;
	rBuf.CreateMaxL ( KMaxAge ().Length () + buf->Length () );
	rBuf.Copy ( KMaxAge );
	rBuf.Append ( buf->Des () );

	RStringF cacheControlValStrF = iStringPool.OpenFStringL( rBuf );
	CleanupClosePushL ( cacheControlValStrF );
	THTTPHdrVal cacheControlVal;
	cacheControlVal.SetStrF ( cacheControlValStrF );

	aHeaders.SetFieldL ( cacheControl, cacheControlVal );

	CleanupStack::PopAndDestroy ( &cacheControlValStrF );
	CleanupStack::PopAndDestroy ( );
	}

void CHttpUdpFlow::HandlePublishByeRequestL ( TUpnpMessage::TUPnPPublishByeRequest& aMessage )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::HandlePublishByeRequestL")));
	CRequest& request = CreatePublishRequestLC ();

	RHTTPHeaders hdr = request.Handle ().GetHeaderCollection ();

	// Set NT str
	TPtrC8 ntPtr ( aMessage.iSsdpInfo.iSearchTarget.First ()->Ptr (), aMessage.iSsdpInfo.iSearchTarget.First ()->Length () );
	SetHeaderValueL ( hdr, UPnP::ENT, ntPtr, TUPnPTable::Table() );

	SetHeaderValueL ( hdr, UPnP::ENTS, KSsdpBye(), TUPnPTable::Table() );
	TPtrC8 usnPtr ( aMessage.iSsdpInfo.iUsn.First ()->Ptr (), aMessage.iSsdpInfo.iUsn.First ()->Length () );
	SetHeaderValueL ( hdr, UPnP::EUSN, usnPtr, TUPnPTable::Table() );

	//Set Content Length
	hdr.SetFieldL ( iStringPool.StringF ( HTTP::EContentLength, THTTPTable::Table() ), THTTPHdrVal ( 0 ) );

	TAppProtAddr addr ( KSsdpMulticastAddr, KUpnpMCPort  );
	CSendElement* sendElement = new ( ELeave ) CSendElement ( CSendElement::ENotifyByeRequest, request, addr );
	iSendElements.AppendL ( sendElement );

	CleanupStack::Pop (); //request
	}

CRequest& CHttpUdpFlow::CreatePublishRequestLC ()
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::CreatePublishRequestLC")));
	CRequest* publishRequest = CRequest::NewL ( *iCodec, iStringPool );
	CleanupStack::PushL ( publishRequest );

	RStringF method = iStringPool.StringF ( UPnP::ENotify, TUPnPTable::Table() );
	// Set the method and URI
	RRequest req = publishRequest->Handle ();
	req.SetMethod ( method );
	TUriParser8 parser;
	parser.Parse ( KSearchUri () );
	req.SetURIL ( parser );

	RHTTPHeaders hdr = publishRequest->Handle ().GetHeaderCollection ();

	// Set host header
	SetHeaderValueL ( hdr, HTTP::EHost, KMultiCastAddr(), THTTPTable::Table() );

	return *publishRequest;
	}

// --------------------------------------------

void CHttpUdpFlow::SendIfOneElement ()
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::SendIfOneElement")));
	if ( iSendElements.Count () == 1 )
		{
		HandleSendElement ();
		}
	}

void CHttpUdpFlow::HandleSendElement ()
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::HandleSendElement")));

	if ( !IsSocketCreated () )
		{
		TRAP_IGNORE ( CreateSocketL () );
		return;
		}
	// Sent the first element
	if ( iSendElements.Count() > 0 )
		{
		CSendElement* element = iSendElements[0];
		
		if(iSendCount > KDeftSendCount)
			{
			if(iSendTimer && iSendTimer->IsActive())
				return;
			}
		iIsComposeCompleted = EFalse; 
		if ( element->Type() == CSendElement::EMSearchResponse )
			{
			iSendCount = 0;
			iResponseComposer->ComposeResponse ( static_cast < CResponse* > ( element->Message () ) );
			}
		else
			{
			StartSendTimer();
			IncrementSendCount();
			iRequestComposer->ComposeRequest ( static_cast <CRequest*> ( (element->Message ()) ) );
			}		
		}
	}

void CHttpUdpFlow::CreateSocketL ()
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::CreateSocketL")));
	if ( !iSocketOpener )
		{
		iSocketOpener = CSocketOpener::NewL ( *this );
		iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
		}
	}

CSendElement::TSendType CHttpUdpFlow::RemoveFirstSendElement ()
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::RemoveFirstSendElement")));
	ASSERT ( iSendElements.Count () > 0 );
	CSendElement* element = iSendElements[0];
	iSendElements.Remove (0);
	CSendElement::TSendType sendType = element->Type ();
	delete element;
	return sendType;
	}

TInt CHttpUdpFlow::FindSearchTarget ( TNodeCtxId& aChannelId )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::FindSearchTarget")));
	for ( TInt i = 0; i < iSearchTargetArray.Count (); ++i )
	{
	if ( iSearchTargetArray[i]->Originator () == aChannelId )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::FindSearchTarget - Search target found")));
		return i;
		}
	}
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::FindSearchTarget - Search target not found")));
	return KErrNotFound;
	}


TBool CHttpUdpFlow::IsContentLengthZero ( const CResponse& aResponse )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::IsContentLengthZero")));
	THTTPHdrVal contentLenVal;
	if ( GetHeaderValue ( aResponse, HTTP::EContentLength, contentLenVal, THTTPTable::Table() ) == KErrNone )
		{
		if ( contentLenVal.Int( ) == 0 )
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::IsContentLengthZero - Returned ETrue")));
			return ETrue;
			}
		LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::IsContentLengthZero - Returned EFalse")));
		return EFalse;
		}
	LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::IsContentLengthZero - Content-Length header was not found")));
	return EFalse;
	}

void CHttpUdpFlow::CreateSendTimerL()
	{
	if(!iSendTimer)
		{
		iSendTimer = CUPnPTimer::NewL(*this);
		}
	}

void CHttpUdpFlow::TimeOut()
	{
	iSendCount = 0;
	if(iIsComposeCompleted)
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::TimeOut[HandleSendElement]")));
		HandleSendElement();
		}
	return;
	}

void CHttpUdpFlow::IncrementSendCount()
	{
	if(IsMSearchOrAliveRequest())
		{
		++iSendCount;
		}
	else
		{
		iSendCount = 0;
		}
	}

void CHttpUdpFlow::StartSendTimer()
	{
	if( IsMSearchOrAliveRequest() )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpUDPFlow, KComponent, _L8("CHttpUdpFlow::StartSendTimer[Timer Started]")));
		iSendTimer->StartTimer(KDeftTimeOutVal);
		}
	}







