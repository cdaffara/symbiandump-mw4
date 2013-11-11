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
// @internalComponent
// 
//

//System Includes
#include <comms-infras/ss_nodeinterfaces.h>

#include <httpstringconstants.h>
#include <http/thttphdrval.h>
#include <httperr.h>
#include <http/thttptable.h>
#include <upnp/tupnptable.h>

//Local Includes
#include "httpserverflow.h"
#include "upnppint.h"
#include "httpserver.h"
#include "upnpserverconstants.h"
#include "upnplog.h"
#include "upnpmemoryutils.h"

__FLOG_STMT(_LIT8(KComponent,"Flow");)
using namespace ESock;

CHttpServerFlow* CHttpServerFlow::NewL ( CSubConnectionFlowFactoryBase& aFactory, CProtocolIntfBase* aProtocolIntf, const TDesC8& aUri, const TNodeId& aSubConnId )
	{
	CHttpServerFlow* self = new (ELeave) CHttpServerFlow( aFactory, aProtocolIntf, aSubConnId);
	CleanupStack::PushL ( self );
	self->ConstructL ( aUri );
	CleanupStack::Pop (); // self
	return self;
	}

CHttpServerFlow::CHttpServerFlow(CSubConnectionFlowFactoryBase& aFactory, ESock::CProtocolIntfBase* aProtocolIntf, const TNodeId& aSubConnId )
	: CUPnPFlowBase( aFactory, aProtocolIntf, EHttpServerFlow, aSubConnId ),
	iControlId ( 1 )
	{
	LOG_NODE_CREATE ( KESockFlowTag, CHttpServerFlow );
	}

void CHttpServerFlow::ConstructL ( const TDesC8& aUri )
	{
	iUri.CreateL ( aUri );
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("Created CHttpServerFlow")));
	}

CHttpServerFlow::~CHttpServerFlow ()
	{
	iUri.Close ();
	iSubConnectionProvider.Close ();
	iControlTransactions.Reset();
	iControlTransactions.Close ();

	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("Destroyed CHttpServerFlow")));
	LOG_NODE_DESTROY(KESockFlowTag, CHttpServerFlow);
	}

//From CSubConnectionFlowBase MNode
void CHttpServerFlow::ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{	
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ReceivedL")));
    
   	if ( aMessage.IsMessage<TEChild::TDestroy> () )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ReceivedL - Deleting current flow")));
        if ( iSessionControlNotify )
	       	{
           	iSessionControlNotify->CanClose( MSessionControlNotify::EDelete );
	       	}
       	DeleteThisFlow ();
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStart> () )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ReceivedL - TCFMessage::TDataClientStart")));
        StartFlow ( address_cast < TNodeCtxId > ( aSender ) );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStop> () )
		{
       	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ReceivedL - TCFMessage::TDataClientStop")));
       	StopFlow ( address_cast < TNodeCtxId > ( aSender ) );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPResponseInfo> () )
    	{
    	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ReceivedL - TUpnpMessage::TUPnPResponseInfo")));
    	const TUpnpMessage::TUPnPResponseInfo& msg = message_cast < TUpnpMessage::TUPnPResponseInfo > ( aMessage );
    	CServerTransaction* trans = static_cast < CServerTransaction* > ( msg.iTrans );
    	RMBufChain dataChain;
    	TUPnPMemoryUtils::CreateMBuf(dataChain, trans->MemParts());
    	trans->FreeMemChunk();
    	trans->AddBodyPart(dataChain);
    	HandleUPnPResponseL ( trans, msg.iStatus, msg.iInfo );
    	}
	}
	

MFlowBinderControl* CHttpServerFlow::DoGetBinderControlL()
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::DoGetBinderControlL")));
    return this;
	}

// MFlowBinderControl
CSubConnectionFlowBase* CHttpServerFlow::Flow()
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::Flow")));
	return	this;
	}

MSessionControl* CHttpServerFlow::GetControlL (TInt /*aSessionType*/, MSessionControlNotify& aSessionControlNotify)
    {
    LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::GetControlL")));
    ASSERT(iSessionControlNotify == NULL);
	iSessionControlNotify = &aSessionControlNotify;

    //As of now UPnP Flow doesn't care more abt SessionType, since i'm caring abt only CSocket Binding,
    //May be for RInternalSocket binding this needs to be iterated.

	return this;
    }

MSessionData* CHttpServerFlow::BindL(MSessionDataNotify& aNotify)
    {
    LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::BindL")));
	ASSERT(iSessionDataNotify == NULL);
	iSessionDataNotify = &aNotify;
	iSubConnectionProvider.PostMessage ( Id (), TCFControlProvider::TActive ().CRef () );
	return this;
    }

void CHttpServerFlow::Unbind()
    {
    LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::Unbind")));
    iSessionControlNotify = NULL;
	iSessionDataNotify = NULL;
	}

// MSessionControl
void CHttpServerFlow::Shutdown(MSessionControl::TCloseType aOption)
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::Shutdown")));

	if ( MSessionControl::ENormal == aOption )
		{
		// error the pending control transactions
		TUint count = iControlTransactions.Count ();

		for ( TInt pos = 0; pos < count; pos++ )
			{
			CServerTransaction& trans = iControlTransactions[pos].iTransaction;
			if ( !trans.ReadyToSend () )
				{
				// If we are not in ready to send state, it means that, we haven't created the response yet.
				CHttpServerHandler::CreateResponse ( *( trans.Response() ), HTTPStatus::EInternalServerError, &trans.ServerObserver() );
				trans.SetReadyToSend ();
				THTTPEvent evt ( THTTPEvent::EGotResponseHeaders );
				trans.ServerObserver ().OnHttpEvent ( &trans, evt );
				}
			else if ( trans.CloseNeeded () )
				{
				THTTPEvent evt ( THTTPEvent::EFailed );
				trans.ServerObserver ().OnHttpEvent ( &trans, evt );
				}
			}

		// remove control uri from http server, in order not to service further requests
		TNodeCtxId serviceId ( MeshMachine::KActivityNull, NodeId () );
		TNodeCtxId controlProviderId ( MeshMachine::KActivityNull, TNodeId::NullId () );
		static_cast<CUPnPProtocolIntfBase *>( ProtocolIntf () )->RemoveServiceUri ( iUri, serviceId, controlProviderId );

		// send data client idle to scpr
		iSubConnectionProvider.PostMessage ( Id (), TCFControlProvider::TIdle ().CRef () );
		}
	}

void CHttpServerFlow::ActiveOpen()
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ActiveOpen")));
	iSessionControlNotify->ConnectComplete();
	}

TInt CHttpServerFlow::GetOption(TUint aLevel, TUint aName, TDes8& anOption) const
	{
	if ( aLevel == KCHOptionLevel )
		{

		TControlTransaction trans ( FirstControlTransactionToNotify () );
		switch ( aName )
			{
			case KCHMaxLength:
				{
				THTTPHdrVal value;
				if ( GetHeaderValue ( *(trans.iTransaction.Request()), HTTP::EContentLength, value, THTTPTable::Table() ) == KErrNotFound )
					{
					*(TInt*) anOption.Ptr () = KErrUnknown;
					}
				else
					{
					*(TInt*) anOption.Ptr () = value.Int ();
					}
				}
			break;

			case KCHLastMessage:
				{
				*(TUint*) anOption.Ptr() = ( trans.iReadComplete && trans.iTransaction.BodyParts ().Length () == 0 );
				}
			break;

			default:
			ASSERT(0);
			}
		}
	return 0;
	}

TInt CHttpServerFlow::SetOption(TUint level, TUint name, const TDesC8& anOption)
	{
	TInt err = KErrNone;
	if ( level == KCHOptionLevel )
		{
		TPckgBuf <TCHMessageOption> option;
		option.Copy ( anOption );
		TInt pos = FindControlTransaction ( option().iId );
		if ( pos == KErrNotFound )
			return pos;
		TControlTransaction trans ( iControlTransactions[pos] );
		switch ( name )
			{
			case KCHMaxLength:
				{
				TRAP ( err, SetHeaderL ( *( trans.iTransaction.Response() ), HTTP::EContentLength, option().iValue, THTTPTable::Table() ) );

				if ( err == KErrNone )
					{
					// This will set the basic headers
					CHttpServerHandler::CreateResponse( *( trans.iTransaction.Response() ), HTTPStatus::EOk, &trans.iTransaction.ServerObserver() );
					}
				else
					{
					CHttpServerHandler::CreateResponse( *( trans.iTransaction.Response() ), HTTPStatus::EInternalServerError, &trans.iTransaction.ServerObserver() );
					}

				// Set the content length.
				trans.iTransaction.SetDataLeft ( option().iValue );
				// commented to solve race issue...
				//trans.iTransaction.SetReadyToSend ();   // Move to the ready to send state. When the next write happens
														// we just say body data is available
				//THTTPEvent evt ( THTTPEvent::EGotResponseHeaders );
				//trans.iTransaction.ServerObserver().OnHttpEvent ( &trans.iTransaction, evt );
				}
			break;

			case KCHLastMessage:
				{
				trans.iTransaction.SetComplete();
				THTTPEvent evt ( THTTPEvent::EResponseComplete );
				trans.iTransaction.ServerObserver().OnHttpEvent ( &trans.iTransaction, evt );
				}
			break;

			default:
			ASSERT(0);
			}
		}
	return err;
	}

// MSessionData

TInt CHttpServerFlow::Write ( RMBufChain& aData, TUint aOptions, TSockAddr* /* anAddr */ )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::Write")));
	TInt pos = FindControlTransaction ( aOptions );
	TInt ret = KErrNone;
	if ( pos == KErrNotFound )
		return pos;

	TInt len = aData.Length ();
	CServerTransaction& trans = iControlTransactions[pos].iTransaction;
	trans.AddBodyPart( aData );

	if ( !trans.ReadyToSend() )
		{
		// If we are not in ready to send state, it means that, we haven't created the response yet.
		trans.SetReadyToSend ();
		THTTPEvent evt ( THTTPEvent::EGotResponseHeaders );
		ret = trans.ServerObserver().OnHttpEvent ( &trans, evt );
		}
	else
		{
		THTTPEvent evt ( THTTPEvent::EGotResponseBodyData );
		ret = trans.ServerObserver().OnHttpEvent ( &trans, evt );
		}
	if(ret != KErrNone)
		{
		RemoveControlTransaction ( iControlTransactions[pos].iId );
		//Ownership is transferred to trans Object.
		aData.Init ();
		return ret;
		}
	
	// remove control transaction object if complete data is received from socket
	if ( !trans.CloseNeeded () ) // closed needed informs that complete data is not yet received
		{
		RemoveControlTransaction ( iControlTransactions[pos].iId );
		}
	
	aData.Init ();
	return len;
	}

TInt CHttpServerFlow::GetData ( RMBufChain& aData, TUint aLength, TUint /*aOptions*/, TSockAddr* /*anAddr*/ )
	{
	__ASSERT_DEBUG ( iControlTransactions.Count(), User::Invariant() );
	CServerTransaction& trans = FirstControlTransactionToNotify ().iTransaction;

	RMBufChain newChain;
	RMBufChain& bodyChain = trans.BodyParts ();
	TInt err = bodyChain.Split ( aLength, newChain );
	if ( err != KErrNone )
		{
		// Our request validation failed. Generate the response
		CHttpServerHandler::CreateResponse ( *( trans.Response () ), err, &trans.ServerObserver() );
		THTTPEvent evt ( THTTPEvent::ECompleteResponse );
		trans.ServerObserver().OnHttpEvent ( &trans, evt );
		}

	aData.Assign ( bodyChain );
	bodyChain = newChain;

	return aData.Length();
	}

// From MHttpEventObserver
TInt CHttpServerFlow::OnHttpEvent ( CTransaction* aTransaction, THTTPEvent& aEvent )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::OnHttpEvent")));
	TInt ret = KErrNone;

	switch ( aEvent.iStatus  )
		{
		case THTTPEvent::EGotRequestHeaders:
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::OnHttpEvent - THTTPEvent::EGotRequestHeaders")));
			ret = ValidateUPnPRequest( *aTransaction );
			}
			break;

		default:
			ret = RouteTransaction ( static_cast < CServerTransaction& > (*aTransaction), aEvent );
			break;
		}

	if ( ret != KErrNone )
		{
		LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::OnHttpEvent - THTTPEvent::EGotRequestHeaders - Creating response")));
		// Our request validation failed. Generate the response
		CHttpServerHandler::CreateResponse ( *( aTransaction->Response () ), ret, &(static_cast < CServerTransaction& > (*aTransaction).ServerObserver()) );
		( static_cast < CServerTransaction* > ( aTransaction ) )->SetComplete ();
		( static_cast < CServerTransaction* > ( aTransaction ) )->SetReadyToSend ();
		}
	return KErrNone;
	}

void CHttpServerFlow::StartFlow ( TNodeCtxId aSender )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::StartFlow")));

	TNodeCtxId serviceId ( MeshMachine::KActivityNull, NodeId () );	
	TRAPD ( err, static_cast<CUPnPProtocolIntfBase *>( ProtocolIntf() )->AddServiceUriL( iUri, *this, serviceId, aSender ) );
	
	if ( err != KErrNone )
		iSubConnectionProvider.PostMessage ( Id (), TEBase::TError ( TCFDataClient::TStart::Id (), err ).CRef() );
	}

void CHttpServerFlow::StopFlow ( TNodeCtxId aSender )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::StopFlow")));

	TNodeCtxId serviceId ( MeshMachine::KActivityNull, NodeId () );	
	__ASSERT_DEBUG  ( iSubConnectionProvider.RecipientId () == aSender, User::Invariant () );

	static_cast<CUPnPProtocolIntfBase *>( ProtocolIntf () )->RemoveServiceUri ( iUri, serviceId, aSender );
	}

TInt CHttpServerFlow::RouteTransaction ( CServerTransaction& aTrans, THTTPEvent& aEvent )
	{
	//  ... Support chunked-encoding here for the request body data incase of POST & MPOST.
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::RouteTransaction")));
	TBool dataUp = EFalse;

	RStringF post = aTrans.Request()->Method().Pool().StringF(HTTP::EPOST, THTTPTable::Table());
	RStringF mPost = aTrans.Request()->Method().Pool().StringF(UPnP::EMPost, TUPnPTable::Table());
	
	if((aTrans.Request()->Method() == post) || (aTrans.Request()->Method() == mPost))
		{
		dataUp = ETrue;
		}

	switch ( aEvent.iStatus )
		{
		case THTTPEvent::EGotRequestBodyData:
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::RouteTransaction - THTTPEvent::EGotRequestBodyData")));
			MHTTPDataSupplier* bodySupplier = aTrans.Request( )->Body( );
			ASSERT(bodySupplier);

			TPtrC8 bodyPtr;
			RMBufChain bodyChain;
			bodySupplier->GetNextDataPart ( bodyPtr );
			bodyChain.Create ( bodyPtr );
			aTrans.AddBodyPart( bodyChain );
			bodySupplier->ReleaseData( );

			if ( dataUp )
				{
				TInt index = FindOrCreateControlTransaction ( aTrans );
				if ( index < KErrNotFound )
					{
					return HTTPStatus::EInternalServerError;
					}

				TInt notifyDataLen = 0;
				if ( !iControlTransactions[index].iReadTriggered )
					{
					notifyDataLen = sizeof ( TInt );
					iControlTransactions[index].iReadTriggered = ETrue;
					}

				iSessionDataNotify->NewData ( notifyDataLen + bodyPtr.Length () );
				}
			}
			break;
		case THTTPEvent::ERequestComplete:
			{
			if ( !dataUp )
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::RouteTransaction - THTTPEvent::ERequestComplete")));
				LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::RouteTransaction - THTTPEvent::ERequestComplete - Posting message TUpnpMessage::TUPnPRequestInfo")));

				IterateHeaders ( *aTrans.Request() );		
				// Post the response to the SCPR
				iSubConnectionProvider.PostMessage ( Id (), TUpnpMessage::TUPnPRequestInfo ( &aTrans ).CRef () );
				}
			else
				{
				TInt index = FindOrCreateControlTransaction ( aTrans );
				iControlTransactions[index].iReadComplete = ETrue;
				// make sure if receive is pending, ASocket will complete it
				iSessionDataNotify->NewData ( 0 );
				}
			}
		break;

		default:
		ASSERT(0);
		break;
		}
	return KErrNone;
	}

TInt CHttpServerFlow::GetHeaderValue ( const CRequest& aRequest, TInt aFieldIndex, THTTPHdrVal& aFieldVal, const TStringTable& aTable ) const
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::GetHeaderValue")));
	CRequest& req = const_cast < CRequest& > ( aRequest );
	RRequest request = req.Handle ();
	RHTTPHeaders headers = request.GetHeaderCollection ();
	RStringF fieldStr = aRequest.StringPool ().StringF ( aFieldIndex, aTable );
	return headers.GetField ( fieldStr, 0, aFieldVal );
	}

TBool CHttpServerFlow::MatchHeaderValue ( const CRequest& aRequest, TInt aFieldIndex, const TDesC8& aFieldValue, const TStringTable& aTable )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::MatchHeaderValue")));
	THTTPHdrVal value;
	if (  GetHeaderValue ( aRequest, aFieldIndex, value, aTable ) == KErrNone )
		{
		RStringF valStr = value.StrF();
		if ( valStr.DesC ().Compare ( aFieldValue ) == 0 )
			{
			LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::MatchHeaderValue - Returned ETrue")));
			return ETrue;
			}
		}
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::MatchHeaderValue - Returned EFalse")));
	return EFalse;
	}

TBool CHttpServerFlow::IsHeaderPresent ( const CRequest& aRequest, TInt aFieldIndex, const TStringTable& aTable )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::IsHeaderPresent")));
	THTTPHdrVal value;
	return GetHeaderValue ( aRequest, aFieldIndex, value, aTable ) == KErrNone;
	}

TBool CHttpServerFlow::IsValidNTHeader ( const CRequest& aRequest )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::IsValidNTHeader")));
	_LIT8 ( KNTValue, "upnp:event" );
	return MatchHeaderValue ( aRequest, UPnP::ENT, KNTValue(), TUPnPTable::Table() );
	}

TBool CHttpServerFlow::IsValidContentTypeHeader ( const CRequest& aRequest )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::IsValidContentTypeHeader")));
	_LIT8 ( KContentTypeValue, "text/xml" );
	return MatchHeaderValue ( aRequest, HTTP::EContentType, KContentTypeValue(), THTTPTable::Table() );
	}

TBool CHttpServerFlow::IsValidNTSHeader ( const CRequest& aRequest )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::IsValidNTSHeader")));
	_LIT8 ( KNTSValue, "upnp:propchange" );
	return MatchHeaderValue ( aRequest, UPnP::ENTS, KNTSValue(), TUPnPTable::Table() );
	}

TBool CHttpServerFlow::IsValidManHeader ( const CRequest& aRequest )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::IsValidManHeader")));
	_LIT8 ( KManValue, "http://schemas.xmlsoap.org/soap/envelope/");
	return MatchHeaderValue ( aRequest, UPnP::EMAN, KManValue(), TUPnPTable::Table() );
	}

void CHttpServerFlow::SetHeaderL ( CResponse& aRequest, TInt aFieldIndex, RMemChunk& aVal, const TStringTable& aTable )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::SetHeaderL")));
	RStringPool sp = aRequest.StringPool ();
	RHTTPHeaders hdr = aRequest.Handle ().GetHeaderCollection ();
	TPtrC8 value;
	value.Set ( aVal.First()->Ptr (), aVal.Length () );
	RStringF valStr = sp.OpenFStringL ( value );
	THTTPHdrVal hdrVal ( valStr );
	hdr.SetFieldL ( sp.StringF(aFieldIndex, aTable ), hdrVal );
	valStr.Close ();
	}

void CHttpServerFlow::SetHeaderL ( CResponse& aRequest, TInt aFieldIndex, TInt aVal, const TStringTable& aTable )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::SetHeaderL")));
	RStringPool sp = aRequest.StringPool ();
	RHTTPHeaders hdr = aRequest.Handle ().GetHeaderCollection ();
	THTTPHdrVal hdrVal ( aVal );
	hdr.SetFieldL ( sp.StringF(aFieldIndex, aTable ), hdrVal );
	}


// Returns KErrNone if validation is suceeded otherwise one of the HTTP error status code will be returned
TInt CHttpServerFlow::ValidateUPnPRequest ( const CTransaction& aTrans )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ValidateUPnPRequest")));
	
	RStringF get = aTrans.Request()->Method().Pool().StringF(HTTP::EGET, THTTPTable::Table());
	RStringF post = aTrans.Request()->Method().Pool().StringF(HTTP::EPOST, THTTPTable::Table());
	TInt method = KErrNotFound;
	TInt ret = KErrNone;
	
	if((aTrans.Request()->Method() == get)||(aTrans.Request()->Method() == post))
		{
		method = aTrans.Request()->Method().Index(THTTPTable::Table());
		switch ( method )
			{
			case HTTP::EGET:
			break;

			case HTTP::EPOST:
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ValidateUPnPRequest - Validating POST request")));
				ret = ValidatePostRequest ( aTrans );
				}
			break;

			default:
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ValidateUPnPRequest - HTTPStatus::ENotImplemented")));
				ret = HTTPStatus::ENotImplemented;
				}
			break;
			}
		}
	
	else
		{
		method = aTrans.Request()->Method().Index(TUPnPTable::Table());
		switch ( method )
			{
			case UPnP::EMPost:
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ValidateUPnPRequest - Validating MPOST request")));
				ret = ValidateMPostRequest ( aTrans );
				}
			break;

			case UPnP::ESubscribe:
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ValidateUPnPRequest - Validating SUBSCRIBE request")));
				ret = ValidateSubscribeRequest ( aTrans );
				}
			break;

			case UPnP::EUnsubscribe:
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ValidateUPnPRequest - Validating UNSUBSCRIBE request")));
				ret = ValidateUnsubscribeRequest ( aTrans );
				}
			break;

			case UPnP::ENotify:
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ValidateUPnPRequest - Validating NOTIFY request")));
				ret = ValidateNotifyRequest ( aTrans );
				}
			break;

			default:
				{
				LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ValidateUPnPRequest - HTTPStatus::ENotImplemented")));
				ret = HTTPStatus::ENotImplemented;
				}
			break;
			}
		}
	
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ValidateUPnPRequest - Returned with error code = %d"), ret));
	return ret;
	}

// Returns KErrNone if validation is suceeded otherwise one of the HTTP error status code will be returned
TInt CHttpServerFlow::ValidateSubscribeRequest ( const CTransaction& aTrans )
	{
	// General: SUBSCRIBE request MUST not contain a body
	// 1. Subscribing: SUBSCRIBE request MUST contain a CALLBACK, NT headers. The NT header valus MUST be
	// "upnp:event".
	// Response: 412 status code ( pre-condition failed) - If NT or CALLBACK request header is not present
	//	400 Bad request - If SID header and one of NT or CALLBACK headers are present
	// 2. Re-subscribing: SUBSCRIBE request MUST contain a SID header and MUST not contain
	// a CALLBACK or NT header.
	// Response: Same as 1 except, if the SID is not valid we respond with 412 ( pre-condition failed )
	// The 412 error condition is handled in this case by the SCPR. Here we will not check whether the
	// SID is valid or not
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ValidateSubscribeRequest")));
	CRequest& request = *( aTrans.Request () );

	if ( request.Handle ().HasBody () )
		return HTTPStatus::EBadRequest; // MUST not contain a body

	TBool callback = IsHeaderPresent ( request, UPnP::ECallback, TUPnPTable::Table() );
	TBool nt = 	IsHeaderPresent ( request, UPnP::ENT, TUPnPTable::Table() );
	TBool sid = IsHeaderPresent ( request, UPnP::ESID, TUPnPTable::Table() );

	if ( callback && nt )
		{
		// Subscription request
		// Check the NT header value == MUST be "upnp:event"
		if ( IsValidNTHeader ( request ) )
			return sid ? HTTPStatus::EBadRequest : KErrNone;
		}
	if ( sid )
		{
		// Re-subscribe request
		return ( callback || nt ) ? HTTPStatus::EBadRequest : KErrNone;
		}

	// Otherwise pre-condition failed
	return HTTPStatus::EPreconditionFailed;
	}

// Returns KErrNone if validation is suceeded otherwise one of the HTTP error status code will be returned
TInt CHttpServerFlow::ValidateUnsubscribeRequest ( const CTransaction& aTrans )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ValidateUnsubscribeRequest")));
	// General: UNSUBSCRIBE request MUST not contain a body
	CRequest& request = *( aTrans.Request () );

	if ( request.Handle ().HasBody () )
		return HTTPStatus::EBadRequest; // MUST not contain a body

	TBool callback = IsHeaderPresent ( request, UPnP::ECallback, TUPnPTable::Table() );
	TBool nt = 	IsHeaderPresent ( request, UPnP::ENT, TUPnPTable::Table() );
	TBool sid = IsHeaderPresent ( request, UPnP::ESID, TUPnPTable::Table() );
	if ( sid )
		{
		return ( callback || nt ) ? HTTPStatus::EBadRequest : KErrNone;
		}
	// Otherwise pre-condition failed
	return HTTPStatus::EPreconditionFailed;
	}

// Returns KErrNone if validation is suceeded otherwise one of the HTTP error status code will be returned
TInt CHttpServerFlow::ValidateNotifyRequest ( const CTransaction& aTrans )
	{
	// NOTIFY request
	// 1. Content-Type header value MUST be "text/xml"
	// 2. NT header value MUST be "upnp:event"
	// 3. NTS header value MUST be "upnp:propchange"
	// 4. SID header value MUST be present. ( we check only the presence here )
	// 5. SEQ header value MUST be present ( we check only the presence here )
	// 6. MUST contain a body
	// Errors:
	// Bad request - If NT or NTS header is missing
	// Pre-condition failed - If the above 1-5 condition is failed

	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::ValidateNotifyRequest")));
	CRequest& request = *( aTrans.Request () );

	if ( IsValidContentTypeHeader ( request ) )
		{
		if ( IsValidNTHeader ( request ) && IsValidNTSHeader ( request ) )
			{
			if ( IsHeaderPresent ( request, UPnP::ESID, TUPnPTable::Table() ) && IsHeaderPresent ( request, UPnP::ESEQ, TUPnPTable::Table() ) )
				{
				return KErrNone;
				}
			}
		else if ( !IsHeaderPresent ( request, UPnP::ENT, TUPnPTable::Table() ) || !IsHeaderPresent ( request, UPnP::ENTS, TUPnPTable::Table() ) )
			{
			return HTTPStatus::EBadRequest;
			}
		}
	// Otherwise pre-condition failed
	return HTTPStatus::EPreconditionFailed;
	}

// Returns KErrNone if validation is suceeded otherwise one of the HTTP error status code will be returned
TInt CHttpServerFlow::ValidatePostRequest ( const CTransaction& aTrans )
	{
	// 1. Content-Type header MUST contain text/xml value  -- On failure responds with EUnsupportedMediaType status code
	// 2. Content-Type header MUST contain charset as utf-8 --- On failure responds with EBadRequest status code
	// 3. SOAPACTION header field MUST be present -- On failure responds with EPreconditionFailed status code
	// 4. POST request MUST contain a body	-- On failure responds with EBadRequest
	// 5. Currently we support only the POST method with Content-Length header on the request. If it is not present
	// we return Not Implemented ( 501 ) error response. In the future it may need implementation.

	CRequest& request = *( aTrans.Request () );

	if ( !IsHeaderPresent ( request, HTTP::EContentLength, THTTPTable::Table() ) )
		return HTTPStatus::ENotImplemented;

	if ( IsValidContentTypeHeader ( request ) && IsValidCharset ( request ) )
		{
		if ( IsHeaderPresent ( request , UPnP::ESoapAction, TUPnPTable::Table() ) )
			{
			return KErrNone;
			}
		else
			{
			return HTTPStatus::EPreconditionFailed;
			}
		}
	return HTTPStatus::EUnsupportedMediaType;
	}

// Returns KErrNone if validation is suceeded otherwise one of the HTTP error status code will be returned
TInt CHttpServerFlow::ValidateMPostRequest ( const CTransaction& aTrans )
	{
	// 1. MAN header must be present with value "http://schemas.xmlsoap,org/soap/envelope/"
	// On failure responds with KEBadRequest
	// 2. Checks the namespace and that matches with ns-SOAPACTION... ??
	// 3. Validate like the normal POST request
	CRequest& request = *( aTrans.Request () );

	if ( IsHeaderPresent ( request, UPnP::EMAN, TUPnPTable::Table() ) && IsValidManHeader( request ) )
		{
		return ValidatePostRequest ( aTrans );
		}
	return HTTPStatus::EBadRequest;
	}

void CHttpServerFlow::HandleUPnPResponseL ( CServerTransaction* aTrans, TInt aStatus, SMetaDataNetCtorExt* aInfo )
	{
	LOG(ESockLogExternal::Printf(KSubsysHttpSrvrFlow, KComponent, _L8("CHttpServerFlow::HandleUPnPResponseL")));
	CResponse* response = aTrans->Response ();

	CHttpServerHandler::CreateResponse ( *response, aStatus, &aTrans->ServerObserver ()  );

	if ( aTrans->Request ()->Handle ().Method ().Index ( TUPnPTable::Table() ) ==  UPnP::ESubscribe && aStatus == HTTPStatus::EOk )
		{
		TSubsribeResponseInfo& info = *( reinterpret_cast < TSubsribeResponseInfo* > ( aInfo ) );
		SetHeaderL ( *response, UPnP::ESID, info.iSid, TUPnPTable::Table());
		SetHeaderL ( *response, UPnP::ETimeout, info.iTimeout, TUPnPTable::Table() );
		info.iSid.Free (); // Free the RMBufChain
		}

	// Set the Content-Length header
	SetHeaderL ( *response, HTTP::EContentLength, aTrans->BodyParts ().Length (), THTTPTable::Table() );
	aTrans->SetDataLeft ( aTrans->BodyParts ().Length () );
	aTrans->SetComplete ();
	
	THTTPEvent evt ( THTTPEvent::ECompleteResponse );	
	aTrans->ServerObserver().OnHttpEvent ( aTrans, evt );
	}

void CHttpServerFlow::IterateHeaders ( CRequest& aRequest )
	{
	RHTTPHeaders hdr = aRequest.Handle ().GetHeaderCollection ();
	RStringPool sp = aRequest.StringPool ();
	THTTPHdrFieldIter it = hdr.Fields ();

	while ( it.AtEnd () == EFalse )
		{
		RStringTokenF fieldName = it ();
		RStringF fieldNameStr = sp.StringF ( fieldName );

		THTTPHdrVal fieldVal;
		hdr.GetField ( fieldNameStr, 0, fieldVal );

		++it;
		}
	}

TInt CHttpServerFlow::FindOrCreateControlTransaction ( CServerTransaction& aTrans )
	{
	TInt index = FindControlTransaction ( aTrans );
	if ( index == KErrNotFound )
		{
		return AddControlTransaction ( iControlId++, aTrans );
		}
	return index;
	}


TInt CHttpServerFlow::FindControlTransaction ( TInt aId ) const
	{
	for ( TInt i = 0; i < iControlTransactions.Count (); ++i )
		{
		if ( iControlTransactions[i].iId == aId )
			{
			return i;
			}
		}
	return KErrNotFound;
	}

TInt CHttpServerFlow::FindControlTransaction ( CServerTransaction& aTrans ) const
	{
	for ( TInt i = 0; i < iControlTransactions.Count (); ++i )
		{
		if ( &(iControlTransactions[i].iTransaction) == &aTrans )
			{
			return i;
			}
		}
	return KErrNotFound;
	}

void CHttpServerFlow::RemoveControlTransaction ( TInt aId )
	{
	TInt pos = FindControlTransaction ( aId );
	if ( pos != KErrNotFound )
		{
		iControlTransactions.Remove ( pos );
		}
	}

TInt CHttpServerFlow::AddControlTransaction ( TInt aId, CServerTransaction& aTrans )
	{
	TControlTransaction trans ( aId, aTrans );
	TInt err = iControlTransactions.Append ( trans );
	if ( err == KErrNone )
		{
		const TUint KIdentifiersLen = sizeof ( TInt );
		TBuf8<KIdentifiersLen> controlBuf;
		controlBuf.AppendNumFixedWidth ( aId, EDecimal, KIdentifiersLen );

		RMBufChain bufChain;
		err = bufChain.Create ( controlBuf );
		if ( err != KErrNone )
			return err;
		aTrans.BodyParts().Prepend ( bufChain );
		}

	return err == KErrNone ? iControlTransactions.Count() - 1 : err;
	}

CHttpServerFlow::TControlTransaction CHttpServerFlow::FirstControlTransactionToNotify () const
	{
	TInt i = 0;
	for ( i = 0; i < iControlTransactions.Count (); ++i )
		{
		if ( iControlTransactions[i].iNotifyComplete == EFalse )
			{
			break;
			}
		}
	return iControlTransactions[i];
	}


