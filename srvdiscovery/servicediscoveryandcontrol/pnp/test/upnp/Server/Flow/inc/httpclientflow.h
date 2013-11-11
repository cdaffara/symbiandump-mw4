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

#ifndef __HTTPCLIENTFLOW_H_
#define __HTTPCLIENTFLOW_H_

// System Includes
#include <comms-infras/ss_subconnflow.h>
#include <comms-infras/ss_flowbinders.h>
#include <comms-infras/ss_protflow.h>

#include <http/framework/cheadercodec.h>
#include <http/mhttpdatasupplier.h>

// Local Includes
#include "upnpflowbase.h"
#include "upnp_cf_msgs.h"
#include "chttpclienthandler.h"
#include "csoapparser.h"

using namespace Messages;
using namespace ESock;


const TUint KMaxPipelineTransactions = 6;


class CHttpClientFlow: public CUPnPFlowBase, public MFlowBinderControl,
				public MSessionData, public MSessionControl, // Session Binders with Upper layer
				public MHttpEventObserver	// Incoming HTTP Response Events Observer
	{
public:
	static CHttpClientFlow* NewL ( CSubConnectionFlowFactoryBase& aFactory,
						  CProtocolIntfBase* aProtocolIntf,
						  const TNodeId& aSubConnId,
						  const TAppProtAddr& aSockAddr );
	~CHttpClientFlow ( );

	// From CSubConnectionFlowBase
	virtual MFlowBinderControl* DoGetBinderControlL ( );

	inline void RemName ( TSockAddr& anAddr ) const;
	inline TInt Status ( ) const;

private:
	CHttpClientFlow ( CSubConnectionFlowFactoryBase& aFactory, CProtocolIntfBase* aProtocolIntf, const Messages::TNodeId& aSubConnId, const TAppProtAddr& aSockAddr );
	void ConstructL ( );


	virtual void ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage );
	
	// From MFlowBinderControl
	MSessionControl* GetControlL ( TInt aSessionType, MSessionControlNotify& aSessionControlNotify );
	MSessionData* BindL ( MSessionDataNotify& aNotify );
	void Unbind ( );
	CSubConnectionFlowBase* Flow ( ); // returns its own flow

	// MSessionControl
	void ActiveOpen ( );
	inline void ActiveOpen ( const TDesC8& aConnectionData );
	inline TInt PassiveOpen ( TUint aQueueSize );
	inline TInt PassiveOpen ( TUint aQueueSize,const TDesC8& aConnectionData );
	inline void AutoBind ( );
	inline void LocalName ( TSockAddr& anAddr ) const;
	inline TInt SetLocalName ( TSockAddr& anAddr );
	TInt SetRemName ( TSockAddr& anAddr );
	void Shutdown ( MSessionControl::TCloseType option );
	inline void Shutdown ( MSessionControl::TCloseType option, const TDesC8& aDisconnectionData );
	inline TInt SecurityCheck ( MProvdSecurityChecker* aSecurityChecker );
	TInt GetOption ( TUint level, TUint name, TDes8 &anOption ) const;
	TInt SetOption ( TUint level, TUint name, const TDesC8 &anOption );
	inline void Ioctl ( TUint level, TUint name, TDes8 *anOption );
	inline void CancelIoctl ( TUint aLevel, TUint aName );
	inline void Start ( );

	// MSessionData
	inline TUint Write ( const TDesC8& aDesc, TUint aOptions, TSockAddr* anAddr = NULL );
	TInt Write ( RMBufChain& aData, TUint aOptions, TSockAddr* anAddr = NULL );
	inline void GetData ( TDes8& aDesc, TUint options, TSockAddr* anAddr = NULL );
	TInt GetData ( RMBufChain& aData, TUint aLength, TUint aOptions, TSockAddr* anAddr = NULL );

	// From MHttpEventObserver
	TInt OnHttpEvent ( CTransaction* aTransaction, THTTPEvent& aEvent );


	void PrepareAndSubmitTransactionL ( RMBufChain& aData );
	void PrepareAndSubmitTransactionL ( TUpnpMessage::TUPnPClientRequestInfo& aClntReqInfo, TNodeCtxId aChannelId );
	void ReSubmitTransactionL ( CTransaction* aTransaction );
	void CancelRequest ( TNodeCtxId aChannelId );
	TInt FindTransaction ( TNodeCtxId aChannelId );

	void ProcessControlPlaneResponseEvent ( CHTTPClientTransaction* aTransaction, THTTPEvent& aEvent );
	void NotifyError ( CHTTPClientTransaction* aTransaction, TInt aError );
	void DoReset ( );
	void SetTransactionComplete ( CTransaction* aTransaction );
	
	void SetRequestUriL ( CTransaction* aTrans, const TDesC8& aUri );
	void SetHostHeaderL ( CTransaction* aTrans, const TDesC8& aHost, TInt aPort );

	void BuildNotifyRequestL ( CRequest& aNewRequest, TUpnpMessage::TUPnPClientRequestInfo& aClntReqInfo );
	void BuildSubscribeRequestL ( CRequest& aNewRequest, TUpnpMessage::TUPnPClientRequestInfo& aClntReqInfo );
	void SetSidHeaderL ( CRequest& aNewRequest, TUpnpMessage::TUPnPClientRequestInfo& aClntReqInfo );
	void SetNtHeaderL ( RHTTPHeaders aHeaders );

	void SetContentTypeHeaderL ( RHTTPHeaders aHeaders );
	TBool SetActionHeaderL ( RMBufChain& aData );
	void ConstructSoapActionHeaderValueL ( RMBufChain& aData );
	void FillResponseChain ( CTransaction* aTransaction, TInt& aNewLen );


private:
	// upper flow
	MSessionControlNotify* 	iSessionControlNotify;
	MSessionDataNotify* 	iSessionDataNotify;
	TBool					iDCIdle;
	
	CHTTPClientHandler* 	iClientHandler;
	CTransactionWrapper*	iTransactions;

	RStringPool		iStringPool;
	CHeaderCodec*	iCodec;
	RMBufChain		iBufChain;

	// only for control channel usage
	CSoapParser* 	iActionParser;
	TInetAddr		iRemoteAddress;
	RBuf8			iRequestUri;
	RBuf8			iActionValue;
	TInt 			iError;
	TInt			iOverallDataSize;
	TInt 			iChannelIdentifier;
	TBool			iResponseCompleted;
	TBool			iReadStreamStarted;	
	};


inline TInt CHttpClientFlow::Status ( ) const
	{
	if ( iSessionControlNotify != NULL || iSessionDataNotify != NULL || iTransactions->Count ( ) == KMaxPipelineTransactions )
	// flow is attached to upper flow ( i.e.CSocket ) or pipeline transactions reached max
		return KErrNotReady;

	return KErrNone;
	}
#include "httpclientflow.inl"
#endif // HTTPCLIENTFLOW_H
