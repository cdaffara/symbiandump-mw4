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

#ifndef HTTPSERVERFLOW_H_
#define HTTPSERVERFLOW_H_

//System Includes
#include <comms-infras/ss_subconnflow.h>
#include <comms-infras/ss_protflow.h>
#include <comms-infras/eintsock.h>
#include <rmemchunk.h>

#include "upnpflowbase.h"
#include "rsockethandler.h"
#include "csocketopener.h"
#include "httpserverhandler.h"
#include "upnp_cf_msgs.h"
//Local Includes

using namespace ESock;

class CHttpServer;

class CHttpServerFlow: public CUPnPFlowBase,	public MFlowBinderControl,
				/*Session Binder with Upper layer */
				public MSessionData, public MSessionControl, public MHttpEventObserver
	{
	friend class CUPnPFlowFactory;
public:
	static CHttpServerFlow* NewL(CSubConnectionFlowFactoryBase& aFactory, CProtocolIntfBase* aProtocolIntf, const TDesC8& aUri, const Messages::TNodeId& aSubConnId );
	inline const TDesC8& Uri() const;

private:

	struct TControlTransaction
		{
		TControlTransaction ( TInt aId, CServerTransaction& aTrans )
			: iId ( aId ),
			iTransaction ( aTrans ),
			iReadComplete ( EFalse ),
			iReadTriggered ( EFalse ),
			iNotifyComplete ( EFalse )
			{
			}

		CServerTransaction& iTransaction;
		TInt iId;
		TBool iNotifyComplete : 1;
		TBool iReadTriggered : 1;
		TBool iReadComplete : 1;
		};


	// From CSubConnectionFlowBase
	virtual void ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage );

 	virtual MFlowBinderControl* DoGetBinderControlL();

	//From MFlowBinderControl
	virtual MSessionControl* GetControlL (TInt aSessionType, MSessionControlNotify& aSessionControlNotify);
	virtual MSessionData* BindL(MSessionDataNotify& aNotify);
	virtual void Unbind();
	virtual CSubConnectionFlowBase* Flow(); //returns its own flow

	// MSessionControl
	virtual void ActiveOpen();
	inline virtual void ActiveOpen(const TDesC8& aConnectionData);
	inline virtual TInt PassiveOpen(TUint aQueueSize);
	inline virtual TInt PassiveOpen(TUint aQueueSize,const TDesC8& aConnectionData);
	inline virtual void AutoBind();
	inline virtual void LocalName(TSockAddr& anAddr) const;
	inline virtual TInt SetLocalName(TSockAddr& anAddr);
	inline virtual void RemName(TSockAddr& anAddr) const;
	inline virtual TInt SetRemName(TSockAddr& anAddr);
	virtual void Shutdown(MSessionControl::TCloseType option);
	inline virtual void Shutdown(MSessionControl::TCloseType option, const TDesC8& aDisconnectionData);
	inline virtual TInt SecurityCheck(MProvdSecurityChecker* aSecurityChecker);
	virtual TInt GetOption(TUint level, TUint name, TDes8 &anOption) const;
	virtual TInt SetOption(TUint level, TUint name, const TDesC8 &anOption);
	inline virtual void Ioctl(TUint level, TUint name, TDes8 *anOption);
	inline virtual void CancelIoctl(TUint aLevel, TUint aName);
	inline virtual void Start();

	// MSessionData
	inline virtual TUint Write(const TDesC8& aDesc,TUint aOptions, TSockAddr* anAddr = NULL);
	virtual TInt Write(RMBufChain& aData, TUint aOptions, TSockAddr* anAddr = NULL );
	inline virtual void GetData(TDes8& aDesc,TUint options,TSockAddr* anAddr=NULL);
	virtual TInt GetData(RMBufChain& aData, TUint aLength,TUint aOptions,TSockAddr* anAddr=NULL);

	// From MHttpEventObserver
	virtual TInt OnHttpEvent ( CTransaction* aTransaction, THTTPEvent& aEvent );


	void StartFlow ( TNodeCtxId aSender );
	void StopFlow ( TNodeCtxId aSender );

	void HandleUPnPResponseL ( CServerTransaction* aTrans, TInt aStatus, SMetaDataNetCtorExt* aInfo );
	// ----------------------------------
	// The following should be moved to a separate utils class
	TInt GetHeaderValue ( const CRequest& aRequest, TInt aFieldIndex, THTTPHdrVal& aFieldVal, const TStringTable& aTable ) const;
	TBool MatchHeaderValue ( const CRequest& aRequest, TInt aField, const TDesC8& aFieldValue, const TStringTable& aTable );

	TBool IsValidNTHeader ( const CRequest& aRequest );
	TBool IsValidContentTypeHeader ( const CRequest& aRequest );
	TBool IsValidNTSHeader ( const CRequest& aRequest );
	inline TBool IsValidCharset ( const CRequest& aRequest );
	TBool IsValidManHeader ( const CRequest& aRequest);

	TBool IsHeaderPresent ( const CRequest& aRequest, TInt aField, const TStringTable& aTable );
	void SetHeaderL ( CResponse& aRequest, TInt aFieldIndex, RMemChunk& aVal, const TStringTable& aTable );
	void SetHeaderL ( CResponse& aRequest, TInt aFieldIndex, TInt aVal, const TStringTable& aTable );
	void IterateHeaders ( CRequest& aRequest );
	// ------------------------------------

	TInt ValidateUPnPRequest ( const CTransaction& aTrans );
	TInt ValidateSubscribeRequest ( const CTransaction& aTrans );
	TInt ValidateUnsubscribeRequest ( const CTransaction& aTrans );
	TInt ValidateNotifyRequest ( const CTransaction& aTrans );
	TInt ValidatePostRequest ( const CTransaction& aTrans );
	TInt ValidateMPostRequest ( const CTransaction& aTrans );

	TInt RouteTransaction ( CServerTransaction& aTrans, THTTPEvent& aEvent );

	TInt FindOrCreateControlTransaction ( CServerTransaction& aTrans );
	TInt FindControlTransaction ( TInt aId ) const;
	TInt FindControlTransaction ( CServerTransaction& aTrans ) const;
	void RemoveControlTransaction ( TInt aId );
	TInt AddControlTransaction ( TInt aId, CServerTransaction& aTrans );
	TControlTransaction FirstControlTransactionToNotify () const;


private:
	CHttpServerFlow(CSubConnectionFlowFactoryBase& aFactory, CProtocolIntfBase* aProtocolIntf, const Messages::TNodeId& aSubConnId );
	~CHttpServerFlow();

	void ConstructL ( const TDesC8& aUri );

private:
	MSessionControlNotify* 	iSessionControlNotify;
	MSessionDataNotify* 	iSessionDataNotify;
	RBuf8					iUri;
	RArray <TControlTransaction>	iControlTransactions;
	TInt 					iControlId;
};

const TDesC8& CHttpServerFlow::Uri() const
	{
	return iUri;
	}

#include "httpserverflow.inl"
#endif // HTTPCLIENTFLOW_H
