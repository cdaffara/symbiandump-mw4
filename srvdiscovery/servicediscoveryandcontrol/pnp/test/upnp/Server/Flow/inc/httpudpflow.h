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

#ifndef __HTTPUDPFLOW_H_
#define __HTTPUDPFLOW_H_

//System Includes
#include <comms-infras/ss_subconnflow.h>
#include <comms-infras/ss_flowbinders.h>
#include <comms-infras/ss_protflow.h>
#include<in_sock.h>
#include<es_sock.h>
#include <rmemchunk.h>
#include <rmemcell.h>

#include "upnpflowbase.h"
#include "rsockethandler.h"
#include "csocketopener.h"
#include "cupnpresponseparser.h"
#include "cupnprequestcomposer.h"
#include "cupnpresponsecomposer.h"
#include "upnp_cf_msgs.h"
#include "upnpctrlscprstates.h"
#include "upnpstatemachine.h"
#include "cupnptimer.h"

NONSHARABLE_CLASS(CSearchTarget) : public CBase
	{
	public:

	CSearchTarget ( Messages::TNodeCtxId& aChannelId )
	: iOriginator ( aChannelId )
	{
	}

	~CSearchTarget ()
		{
		iSearchTarget.Close ();
		}

	void AddTargetL ( RMemChunk& aSt )
		{
		iSearchTarget.CreateMaxL(aSt.Length());
		aSt.CopyOut(iSearchTarget);
		aSt.Free();
		}

	TInt Match ( const TDesC8& aSt ) const
		{
		_LIT8 ( KSsdpAll, "ssdp:all" );
		
		if ( iSearchTarget.CompareF ( KSsdpAll ) == 0 || aSt.CompareF ( KSsdpAll ) == 0 )
			{
				return ETrue;
			}
		
		TInt requestedUriIndex = 0;
		TInt registeredUriIndex = 0;
		
		TInt registeredVersion = UPnPStateMachine::CUPnPUtils::ExtractVersionNumber ( iSearchTarget, registeredUriIndex );
		TInt requestedVersion = UPnPStateMachine::CUPnPUtils::ExtractVersionNumber ( aSt, requestedUriIndex );
		
		TPtrC8 registeredUri = iSearchTarget.Left(registeredUriIndex - 1);
		TPtrC8 requestedUri = aSt.Left(requestedUriIndex - 1);
		
		if ( (registeredUri.CompareF(requestedUri) == 0) && (requestedVersion <= registeredVersion))
			{
				return ETrue;
			}
		return KErrNotFound;
		}

	const Messages::TNodeCtxId& Originator () const
		{
		return iOriginator;
		}
	const TDesC8& SearchTarget() const
		{
		return iSearchTarget;
		}
	private:
	RBuf8					iSearchTarget;
	Messages::TNodeCtxId	iOriginator;
	};

typedef RArray < CSearchTarget* > CSearchTargetArray;
class CUPnPProtocolIntfBase;

NONSHARABLE_CLASS ( CSendElement ) : public CBase
	{
	public:
	enum TSendType
		{
		EMSearchRequest =0,
		ENotifyAliveRequest,
		ENotifyByeRequest,
		EMSearchResponse
		};
	CSendElement ( TSendType aType, CMessage& aMessage, const TAppProtAddr& aAddr )
	: iMessage ( &aMessage ),
	iType ( aType )
	{
	iSockAddr.SetAddress ( aAddr.iAddr );
	iSockAddr.SetPort ( aAddr.iPort );
	}

	~CSendElement ()
	{
	delete iMessage;
	}

	CMessage* Message ()
	{
	return iMessage;
	}
	TSendType Type () const
	{
	return iType;
	}


	public:
	CMessage* 	iMessage;
	TSendType 	iType;
	TInetAddr	iSockAddr;
	};


typedef RArray < CSendElement* > CSendElements;

NONSHARABLE_CLASS ( CHttpUdpFlow ) : public CUPnPFlowBase,
									 public MSocketHandlerObserver,
									 public MComposerObserver,
						 			 public MParserObserver,
						 			 public MUPnPTimerObserver
	{
	friend class CUPnPFlowFactory;

	public:
	static CHttpUdpFlow* NewL ( CSubConnectionFlowFactoryBase& aFactory,
								CUPnPProtocolIntfBase* aProtocolIntf,
						  		const Messages::TNodeId& aSubConnId );
	~CHttpUdpFlow ();

	private:

	CHttpUdpFlow ( CSubConnectionFlowFactoryBase& aFactory,
					CUPnPProtocolIntfBase* aProtocolIntf,
					const TNodeId& aSubConnId );

	void ConstructL ();

	protected:
	// From CSubConnectionFlowBase
	virtual void ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage );


	// From MSocketHandlerObserver
	virtual void OpenComplete ( RInternalSocket& aSocket );
	inline virtual void ConnectComplete ();
	inline virtual void AcceptComplete ( RInternalSocket& aSocket );
	inline virtual void SendComplete ( TInt aLength );
	virtual void SendToComplete ( TInt aLength );
	inline virtual void RecvComplete ( RMBufChain& aData );
	virtual void RecvFromComplete ( RMBufChain& aData, const TSockAddr& aAddr );
//	virtual void IoctlComplete ();
	inline virtual void Error ( TOperation aOperation, TInt aError  );

	// From MComposerObserver
	virtual void MessageDataReadyL ( RBuf8& aData );
	virtual void ComposingConcluded ();
	virtual void ComposerError ( TInt aError );

	// From MUPnPTimerObserver
	virtual void TimeOut ();
	
	// From MParserObserver
	virtual void GotHeaders ();
	inline virtual void GotBodyData ();
	inline virtual void DataParsed();
	virtual void ParsingComplete ( RMemChunk& aExcessData );
	virtual void ParserError ( TInt aError );

	// ------------------------------------
	// The below functions should be moved to a common class. There are some in
	// HTTP server/flow as well
	TInt GetHeaderValue ( const CResponse& aResponse, TInt aFieldIndex, THTTPHdrVal& aFieldVal, const TStringTable& aTable );
	TInt GetParamValue ( const CResponse& aResponse, TInt aFieldIndex, TInt aParamIndex, THTTPHdrVal& aParamVal, const TStringTable& aTable );
	TInt IsHeaderPresent ( const CResponse& aResponse, TInt aFieldIndex, const TStringTable& aTable );
	TInt IsValidCacheControlHeader ( const CResponse& aResponse );
	void SetHeaderValueL ( RHTTPHeaders& aHdr, TInt aFieldIndex, const TDesC8& aFieldVal, const TStringTable& aTable );
	void SetMaxAgeHeaderValueL ( TInt aMaxAgeVal, RHTTPHeaders aHeaders );
	TBool IsContentLengthZero ( const CResponse& aResponse );

	// ------------------------------------

	TInt ValidateSearchResponse ();
	void StartRecv ();
	void NotifyClientsL ();
	void ReadResponseValues ( TInt& aMaxAge, RStringF& aLocation, RStringF& aUsn, RStringF& aST );
	void HandleSearchRequestL ( TNodeCtxId aChannelId, TUpnpMessage::TUPnPSearchRequest& aMessage );
	void HandleStopSearch ( TNodeCtxId aChannelId );

	void HandleSearchResponseL ( TUpnpMessage::TUPnPSearchResponse& aMessage );
	void HandlePublishAliveRequestL ( TUpnpMessage::TUPnPPublishAliveRequest& aMessage );
	void HandlePublishByeRequestL ( TUpnpMessage::TUPnPPublishByeRequest& aMessage );
	CRequest& CreatePublishRequestLC ();

	void SendIfOneElement ();
	void HandleSendElement ();
	void IncrementSendCount();
	void StartSendTimer();
	inline TInt IsMSearchOrAliveRequest(); 
	
	void CreateSendTimerL();
	void CreateSocketL ();
	CSendElement::TSendType RemoveFirstSendElement ();

	TInt FindSearchTarget ( TNodeCtxId& aId );

	TBool IsSocketCreated () const
		{
		return iSocketCreated;
		}
private:
	RInternalSocket			iSocket;
	RSocketHandler			iSocketHandler;
	CSocketOpener* 			iSocketOpener;
	CSearchTargetArray		iSearchTargetArray;
	CUpnpResponseParser* 	iResponseParser;
	CUpnpRequestComposer* 	iRequestComposer;
	CUpnpResponseComposer* 	iResponseComposer;

	CHeaderCodec*			iCodec;
	CResponse*				iUPnPResponse; // Used for parsing the search response
	CSendElements			iSendElements;
	TBool					iSocketCreated;
	
	RMBufChain				iBackupData;
	TInt 					iSendCount;

	TNodeCtxId				iLastLeavingClient;
	TBool					iIsLeaving;
	TBool					iSendFlag;
	TBool					iIsComposeCompleted;
	
	RStringPool				iStringPool;
	
	CUPnPTimer*			    iSendTimer;
	
	};


void CHttpUdpFlow::ConnectComplete ()
	{
	// We don't handle this
	}

void CHttpUdpFlow::AcceptComplete ( RInternalSocket& /* aSocket */ )
	{
	// We don't handle this
	}

void CHttpUdpFlow::SendComplete ( TInt /* aLength */ )
	{
	}

void CHttpUdpFlow::RecvComplete ( RMBufChain& /* aData */ )
	{
	// We don't handle this
	}

void CHttpUdpFlow::Error ( TOperation /*aOperation*/, TInt /*aError*/  )
	{

	}

void CHttpUdpFlow::GotBodyData ()
	{
	ASSERT (0);
	}

void CHttpUdpFlow::DataParsed()
	{
	// We shouldn't come here. Corrupt message
	StartRecv ();
	}

TInt CHttpUdpFlow::IsMSearchOrAliveRequest()
	{
	return ((iSendElements[0]->iType == CSendElement::EMSearchRequest) || (iSendElements[0]->iType == CSendElement::ENotifyAliveRequest));
	}


#endif // __HTTPUDPFLOW_H
