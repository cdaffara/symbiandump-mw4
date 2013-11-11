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

#ifndef __HTTPSERVERHANDLER_H_
#define __HTTPSERVERHANDLER_H_

#include "ctransaction.h"
#include "cprotocolhandler.h"
#include "cupnprequestparser.h"
#include "cupnpresponsecomposer.h"
#include "cupnptimer.h"

#include <rmemchunk.h>

NONSHARABLE_CLASS(CServerTransaction) : public CTransaction
	{
	public:
	static CServerTransaction* NewL ( CHeaderCodec& aCodec, RStringPool& aStringPool, MHttpEventObserver& aObserver )
		{
		CServerTransaction* self = new ( ELeave )CServerTransaction	( aObserver );
		CleanupStack::PushL ( self );
		self->ConstructL ( aCodec, aStringPool );
		CleanupStack::Pop (); // self
		return self;
		}

	inline MHttpEventObserver& ServerObserver () const
		{
		return *iServerObserver;
		}

	inline void SetReadyToSend ()
		{
		iReadyToSend = ETrue;
		}

	inline TBool ReadyToSend () const
		{
		return iReadyToSend;
		}

	inline void SetClosing ()
		{
		iCloseConnection = ETrue;
		}

	inline TBool IsClosing () const
		{
		return iCloseConnection;
		}

	inline void SetParsing  ( TBool aValue )
		{
		iParsing = aValue;
		}

	inline TBool Parsing () const
		{
		return iParsing;
		}

	inline void SetComposing ( TBool aValue )
		{
		iComposing = aValue;
		}

	inline TBool Composing () const
		{
		return iComposing;
		}

	private:
	CServerTransaction ( MHttpEventObserver& aObserver )
	: iServerObserver ( &aObserver )
		{
		}

	private:
		TBool					iReadyToSend : 1;
		TBool					iCloseConnection : 1;
		TBool					iComposing : 1;
		TBool					iParsing : 1;
		MHttpEventObserver*		iServerObserver;
	};

class CServiceInfoArray;
class CHttpServerHandler;

typedef RPointerArray < CHttpServerHandler > CServerHandlerArray;

NONSHARABLE_CLASS(CHttpServerHandler) : public CProtocolHandler,
										public MHttpEventObserver
	{
	public:

	static CHttpServerHandler* NewL ( RInternalSocket& aSocket, RMemChunk& aData, CServiceInfoArray& aServiceInfos, CServerHandlerArray& aServerHandlers,
									CHeaderCodec& aCodec, RStringPool& aStringPool, CChunkManager* aChunkMgr );
	virtual ~CHttpServerHandler ();


	static void CreateResponse ( CResponse& aResponse, TInt aStatusCode, MHttpEventObserver* aServerHandler = NULL );
	static void CreateResponseL ( CResponse& aResponse, TInt aStatusCode );

	private:
	CHttpServerHandler ( RInternalSocket& aSocket, RMemChunk& aData, CServiceInfoArray& aServiceInfos, CServerHandlerArray& aServerHandlers,
	CHeaderCodec& aCodec, RStringPool& aStringPool, CChunkManager* aChunkMgr );
	void ConstructL ();

	// From MParserObserver
	virtual void GotHeaders ();
	virtual void GotBodyData ();
	virtual void DataParsed ();
	virtual void ParsingComplete ( RMemChunk& aExcessData );
	virtual void ParserError ( TInt aError );

	// From MComposerObserver
	virtual void MessageDataReadyL ( RBuf8& aData );
	virtual void ComposingConcluded ();
	virtual void ComposerError ( TInt aError );

	// From MSocketHandlerObserver
	inline virtual void OpenComplete ( RInternalSocket& aSocket );
	inline virtual void ConnectComplete ();
	virtual void SendComplete ( TInt aLength );
	virtual void RecvComplete ( RMBufChain& aData );
	virtual void Error ( TOperation aOperation, TInt aError  );

	// From MHttpEventObserver
	virtual TInt OnHttpEvent ( CTransaction* aTransaction, THTTPEvent& aEvent );

	// methods from MHTTPDataSupplier
	TBool GetNextDataPart ( TPtrC8& aDataPart );
	void ReleaseData ();
	TInt OverallDataSize ();

	// From MUPnPTimerObserver	
	virtual void TimeOut ();

	CServerTransaction& CurrentTransaction () const;
	CServerTransaction& LastTransaction () const;
	void NotifyEvent ( THTTPEvent& aEvent );
	TBool CanDestroy () const;
	void DestroySelf ();

	void StartNewTransactionL ();
	void StartNewTransaction ();
	void DeleteTransaction ( CTransaction* aTrans );

	void SetError ( TInt aError );
	TBool Error () const;

	private:
		RMemChunk 				iReceivedData;
		RMemoryAllocator		iAllocator;
		CServiceInfoArray&		iServiceInfos;   // Not owned.
		CServerHandlerArray&	iServerHandlers; // Not owned
		CUpnpRequestParser* 	iRequestParser;
		CUpnpResponseComposer* 	iResponseComposer;		
		TInt 					iError;

		CHeaderCodec& 			iServerCodec; // Not owned
		RStringPool& 			iStringPool; // Not owned
	};

// From MSocketHandlerObserver
void CHttpServerHandler::OpenComplete ( RInternalSocket& /* aSocket */ )
{
// We don't handle this. Assert here
}

void CHttpServerHandler::ConnectComplete ()
{
// We don't handle this. Assert here

}


#endif // __HTTPSERVERHANDLER_H__
