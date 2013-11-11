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
//

#ifndef __HTTPSERVER_H_
#define __HTTPSERVER_H_

// System Includes
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_protflow.h>
#include <uri8.h>
#include <rmemchunk.h>

#include "cupnprequestparser.h"
#include "cupnpresponsecomposer.h"
#include "httpserverhandler.h"

using namespace Messages;

class MHttpEventObserver;
class CTransaction;

NONSHARABLE_CLASS(CServiceInfo) : public CBase
	{
	public:
		static CServiceInfo* NewL ( const TDesC8& aUri, MHttpEventObserver& iObserver );

		virtual ~CServiceInfo ();
		TBool Match ( const TDesC8& aUri );
		TBool Match ( const TUriC8& aUri, const TDesC8& aHost ) const;
		MHttpEventObserver& Observer () const;
	private:
		CServiceInfo ( MHttpEventObserver& iObserver );
		void ConstructL ( const TDesC8& aUri );

	private:
		RBuf8 iUri;
		MHttpEventObserver& iObserver;
		TUriParser8 iParsedUri;
	};

// -----------------------------------------

NONSHARABLE_CLASS(CServiceInfoArray) : public CBase
	{
	public:
		CServiceInfoArray ();
		~CServiceInfoArray ();

		void AddServiceUriL ( const TDesC8& aUri, MHttpEventObserver& aObserver );
		void RemoveServiceUri ( const TDesC8& aUri );
		TInt FindServiceUri ( const TDesC8& aUri );
		TInt MatchServiceUri ( const TUriC8& aUri, const TDesC8& aHost );
		TInt CountServiceUri ();
		CServiceInfo& operator [] (TInt aPos)
			{
			return *( iServiceInfos [ aPos ] );
			}
		private:
		RPointerArray<CServiceInfo> iServiceInfos;
	};

// ---------------------------------------------

class HttpRequestUtils
 	{
	public:
	static TBool IsHostPresent ( const CRequest& aRequest );
	static const TDesC8& HostStr ( const CRequest& aRequest );
	static void ValidateL ( const CRequest& aRequest );
	static TInt  ValidateMethod ( const CRequest& aRequest );
	static TInt ValidateHost ( const CRequest& aRequest );
	static TInt IsConnectionRequired ( const CRequest& aRequest );
	};

// -----------------------------------------------


class TServiceControlProviderInfo
	{
public:
	TServiceControlProviderInfo ( TNodeCtxId aServiceId, TNodeCtxId aControlProviderId )
	: iServiceId ( aServiceId ), iControlProviderId ( aControlProviderId )
	{

	}
	TServiceControlProviderInfo ()
	{ }

public:
	TNodeCtxId	iServiceId;
	TNodeCtxId	iControlProviderId;
	};


class CServerTransaction;

NONSHARABLE_CLASS(CHttpServer) : public CBase,
							public ASimpleNodeIdBase,
							public MParserObserver,
							public MComposerObserver
	{
	public:
	static CHttpServer* NewL (ESock::CProtocolIntfBase* aProtIntf);
	void AddServiceUriL ( const TDesC8& aUri, MHttpEventObserver& aObserver, TNodeCtxId aServiceId, TNodeCtxId aControlProviderId );
	TBool RemoveServiceUri ( const TDesC8& aUri, TNodeCtxId aServiceId, TNodeCtxId aControlProviderId );

	private:
	void Start ( TUint aPort );
	void StopServer ();

	// From ASimpleNodeIdBase
	virtual void ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage );

	// From MParserObserver
	virtual void GotHeaders();
	inline virtual void GotBodyData();
	virtual void DataParsed();
	inline virtual void ParsingComplete(RMemChunk& aExcessData);
	virtual void ParserError(TInt aError);

	// From MComposerObserver
	virtual void MessageDataReadyL ( RBuf8& aData );
	inline virtual void ComposingConcluded ();
	virtual void ComposerError ( TInt aError );

	void CreateListener ();
	TInt FindServiceUri ( const TDesC8& aUri );
	TBool MatchRequestUri () const;
	void RejectConnection ( RMBufChain& aData );

	void TransferConnectionL ( RInternalSocket& aSocket, RMBufChain& aData );

	private:
	CHttpServer (ESock::CProtocolIntfBase* aProtIntf);
	void ConstructL ();
	virtual ~CHttpServer ();

	private:
	TNodeId					iAppProtIntfId;
	CServiceInfoArray*		iServiceInfos;
	CUpnpRequestParser* 	iParser;
	CUpnpResponseComposer*	iComposer;
	CTransaction*		 	iTransaction;
	CServerHandlerArray		iServerHandlerArray;
	CHeaderCodec*			iCodec;

	RArray<TServiceControlProviderInfo> iServiceControlProviderInfos;
	TBool								iStarted;

	TServiceControlProviderInfo	iLastServiceControlProviderInfo;
	TBool						iIsLeaving;
	ESock::CProtocolIntfBase* 			iProtIntf;
	RStringPool					iStringPool;
	RMemChunk					iMemChunk;
	};

void CHttpServer::GotBodyData ()
	{

	}

void CHttpServer::ParsingComplete ( RMemChunk& /* aExcessData*/ )
	{
	// We don't come here. We complete parsing once we identify our request URI
	}

void CHttpServer::ComposingConcluded ()
	{
	
	}


#endif // __HTTPSERVER_H__
