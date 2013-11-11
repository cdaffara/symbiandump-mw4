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

#ifndef __CHTTPCLIENTHANDLER_H_
#define __CHTTPCLIENTHANDLER_H_


// System Includes
#include <comms-infras/ss_nodemessages.h>
#include <rmemchunk.h>

// Local Includes
#include "cprotocolhandler.h"
#include "csocketopener.h"
#include "cupnprequestcomposer.h"
#include "cupnpresponseparser.h"

using namespace Messages;


//
class CHTTPClientTransaction: public CTransaction
	{
public:
	static CHTTPClientTransaction* NewL ( CHeaderCodec& aCodec, RStringPool& aStringPool, TNodeCtxId aNodeCtxId );
	~CHTTPClientTransaction ( );

	inline TNodeCtxId NodeCtxId ( ) const;
	inline TUint ActivityId ( ) const;


	inline void GetBodyPart ( TPtrC8& aData )
		{
		RMBuf* mBuf = iBodyParts.First ( );
		TUint i = 0;
		while ( i != iRequestPartIdx )
			{
			mBuf = mBuf->Next ( );
			++i;
			}
		aData.Set ( mBuf->Ptr( ), mBuf->Length() );
		}

	inline void RemoveBodyPart ( )
		{
		iRequestPartIdx++;
		}

	inline TBool IsLastBodyPart ( ) const
		{
		return ( iBodyParts.NumBufs ( ) == iRequestPartIdx + 1 ) && Complete ();
		}

	inline void ResetRequestPart ( )
		{
		iRequestPartIdx = 0;
		}

	inline TInt RequestPartIdx ()
		{
		return iRequestPartIdx;
		}
		
	inline void SetComposingStarted ( TBool aValue )
		{
		iComposingStarted = aValue;
		}
		
	inline TBool ComposingStarted ()
		{
		return iComposingStarted;
		}
			
private:
	CHTTPClientTransaction ( TNodeCtxId aNodeCtxId );

private:
	TNodeCtxId	iNodeCtxId;
	TUint		iRequestPartIdx;
	TBool		iComposingStarted;	
	};

TNodeCtxId CHTTPClientTransaction::NodeCtxId ( ) const
	{
	return iNodeCtxId; // this itself will carry activityId as well
	}

TUint CHTTPClientTransaction::ActivityId ( ) const
	{
	return iNodeCtxId.NodeCtx ( );
	}


//
class CTransactionWrapper: public CBase
	{
public:
	CTransactionWrapper ( RArray<CTransaction*>& aTransactionArray )
		: iTransactions ( aTransactionArray )
		{
		}
	TInt Count ( ) const
		{
		return iTransactions.Count ( );
		}
	void AppendL ( CTransaction* aTransaction )
		{
		iTransactions.AppendL ( aTransaction );
		}
	CHTTPClientTransaction* Get ( TInt anIndex )
		{
		__ASSERT_DEBUG ( (anIndex >= 0 && anIndex < iTransactions.Count()), User::Invariant() );
		return static_cast<CHTTPClientTransaction*> ( iTransactions[anIndex] );
		}
	void Remove ( TInt anIndex)
		{
		__ASSERT_DEBUG ( (anIndex >= 0 && anIndex < iTransactions.Count()), User::Invariant() );
		iTransactions.Remove ( anIndex );
		}
private:
	RArray<CTransaction*>& iTransactions;
	};



//
class CHTTPClientHandler : public CProtocolHandler
	{
public:
	static CHTTPClientHandler* NewL ( MHttpEventObserver& aObserver, CChunkManager* aChunkMgr, TSockAddr* aAddr = NULL );
	~CHTTPClientHandler ( );

	CTransactionWrapper* GetTransactionWrapperL ( );
	void SetConnectionInfo ( TSockAddr& aAddr );
	void SubmitTransaction ( CTransaction* aTransaction );
	void CancelTransaction ( CTransaction* aTrans );
	// More Data is available for flow Transaction
	void NotifyNewRequestBodyData ( CTransaction* aTrans, RMBufChain& aData );
	
private:
	CHTTPClientHandler ( MHttpEventObserver& aObserver, CChunkManager* aChunkMgr, TSockAddr* aAddr );
	void ConstructL ( );

	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket );
	void ConnectComplete ( );
	void SendComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& aData );
	void Error ( TOperation aOperation, TInt aError  );

	// From MComposerObserver
	void MessageDataReadyL ( RBuf8& aData );
	void ComposingConcluded ( );
	void ComposerError ( TInt aError );

	// From MParserObserver
	void GotHeaders ( );
	void GotBodyData ( );
	void DataParsed ( );
	void ParsingComplete ( RMemChunk& aExcessData );
	void ParserError ( TInt aError );

	// From MHTTPDataSupplier
	TBool GetNextDataPart ( TPtrC8& aDataPart );
	void ReleaseData ( );
	TInt OverallDataSize ( );
	// From MUPnPTimerObserver	
	virtual void TimeOut ();
	
	TInt OpenLink ( );
	void ReOpenLink ();
	void OpenSocketL ( TUint aAddrFamily, TUint aSockType, TUint aProtocol );
	void MakeConnection ( );
	void CloseLink ( );

	void StartComposer ( CTransaction* aTransaction );
	void ProcessPendingTransactions ( TInt aNextTransIdx = 0 );
	void NotifyPendingTransactions ( TInt aError );
	void StartParser ( CTransaction* aTransaction, RMemChunk& aData );
	void RemoveTransaction ( TInt aPos );
		
	TBool CheckForConnectionPersistence ( );
	TBool CheckFor100ContinueHeader ( CRequest& aRequest );

	CTransaction* CurrentTransaction ( ) const;
 	void ResetTransactions ();
	// utility API's
	inline void SetSocketIdle ( );
	inline void ResetSocketIdle ( );
	inline TBool SocketIdle ( );
	inline TBool HasPendingTransactions ( );
	inline TBool CanPipeLine ( );
	inline void EnablePipeLining ( );
	inline void DisablePipeLining ( );
	inline void SetComposerIdle ( );
	inline void ResetComposerIdle ( );
	inline TBool ComposerIdle ( );
	inline void SetWriteTriggered ( );
	inline void ResetWriteTriggered ( );
	inline TBool WriteTriggered ( );
	inline void SetReadTriggered ( );
	inline void ResetReadTriggered ( );
	inline TBool ReadTriggered ( );
	inline void SetParserIdle ( );
	inline void ResetParserIdle ( );
	inline TBool ParserIdle ( );
	inline void SetReq100Continue ( TBool aValue );
	inline TBool Request100Continue ();
	
private:
	enum TFlags
		{
		ESocketIdle		=0x01,
		ECanPipeLine	=0x02,
		EComposerIdle	=0x04,
		EWriteTriggered =0x08,
		EReadTriggered  =0x10,
		EParserIdle		=0x20,
		};

	MHttpEventObserver&		iObserver;
	CSocketOpener*			iSocketOpener;
	TSockAddr 				iRemoteAddr;
	CUpnpRequestComposer* 	iComposer;
	CUpnpResponseParser*	iParser;
	RMemChunk				iExcessData;
	RMemoryAllocator		iAllocator;
	TUint32 				iFlags; 	// Bydefault pipelining is DISABLED & socket is idle	
	TBool					iFirstTrans;
	TBool					iReq100Continue;
	};

void CHTTPClientHandler::SetSocketIdle ( )
	{
	iFlags |= ESocketIdle;
	}

void CHTTPClientHandler::ResetSocketIdle ( )
	{
	iFlags &= ~ESocketIdle;
	}

TBool CHTTPClientHandler::SocketIdle ( )
	{
	return iFlags & ESocketIdle;
	}

TBool CHTTPClientHandler::HasPendingTransactions ( )
	{
	return iTransactions.Count ( ) > 0 ? ETrue: EFalse;
	}

TBool CHTTPClientHandler::CanPipeLine ( )
	{
	return iFlags & ECanPipeLine;
	}

void CHTTPClientHandler::EnablePipeLining ( )
	{
	iFlags |= ECanPipeLine;
	}

void CHTTPClientHandler::DisablePipeLining ( )
	{
	iFlags &= ~ECanPipeLine;
	}

void CHTTPClientHandler::SetComposerIdle ( )
	{
	iFlags |= EComposerIdle;
	}

void CHTTPClientHandler::ResetComposerIdle ( )
	{
	iFlags &= ~EComposerIdle;
	}

TBool CHTTPClientHandler::ComposerIdle ( )
	{
	return iFlags & EComposerIdle;
	}

void CHTTPClientHandler::SetWriteTriggered ( )
	{
	iFlags |= EWriteTriggered;
	}

void CHTTPClientHandler::ResetWriteTriggered ( )
	{
	iFlags &= ~EWriteTriggered;
	}

TBool CHTTPClientHandler::WriteTriggered ( )
	{
	return iFlags & EWriteTriggered;
	}

void CHTTPClientHandler::SetReadTriggered ( )
	{
	iFlags |= EReadTriggered;
	}

void CHTTPClientHandler::ResetReadTriggered ( )
	{
	iFlags &= ~EReadTriggered;
	}

TBool CHTTPClientHandler::ReadTriggered ( )
	{
	return iFlags & EReadTriggered;
	}
	
void CHTTPClientHandler::SetParserIdle ( )
	{
	iFlags |= EParserIdle;
	}
	
void CHTTPClientHandler::ResetParserIdle ( )
	{
	iFlags &= ~EParserIdle;
	}

TBool CHTTPClientHandler::ParserIdle ( )
	{
	return iFlags & EParserIdle;
	}

void CHTTPClientHandler::SetReq100Continue ( TBool aValue )
	{
	iReq100Continue = aValue;
	}
	
TBool CHTTPClientHandler::Request100Continue ()
	{
	return iReq100Continue;
	}


#endif // CHTTPCLIENTHANDLER_H

