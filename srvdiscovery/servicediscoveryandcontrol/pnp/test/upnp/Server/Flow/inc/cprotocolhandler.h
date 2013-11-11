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

#ifndef __CPROTOCOLHANDLER_H_
#define __CPROTOCOLHANDLER_H_


// System Includes
#include "httpevent.h"

// Local Includes
#include "ctransaction.h"
#include "rsockethandler.h"
#include "mcomposerobserver.h"
#include "mparserobserver.h"
#include "cupnptimer.h"

class MHttpEventObserver
	{
public:
	virtual TInt OnHttpEvent ( CTransaction* aTransaction, THTTPEvent& aEvent ) = 0;
	};
 	
 	
class CProtocolHandler : public CBase,						 
						 public MSocketHandlerObserver,
						 public MComposerObserver,
						 public MParserObserver,
						 public MHTTPDataSupplier,										
						 public MUPnPTimerObserver
	{
public:
	virtual ~CProtocolHandler ( );
	
protected:
	CProtocolHandler ( RInternalSocket& aSocket );
	CProtocolHandler ( );	
	void ConstructL ();
	
	TInt FindTransaction ( CTransaction* aTransaction ) const;
	TInt FindTransaction ( const CRequest* aRequest ) const;
	
	// From MSocketHandlerObserver
	inline void AcceptComplete ( RInternalSocket& /*aSocket*/ );
	inline void SendToComplete ( TInt /*aLength*/ );
	inline void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ );
	
	// From MHTTPDataSupplier
	inline TInt Reset ();
	
protected:
	RSocketHandler 			iSocketHandler;
	RInternalSocket 		iSocket;	
	RArray<CTransaction*>	iTransactions;	
	CUPnPTimer*				iTimer;
	};


void CProtocolHandler::AcceptComplete ( RInternalSocket& /*aSocket*/ )
	{
	}

void CProtocolHandler::SendToComplete ( TInt /*aLength*/ )
	{
	}

void CProtocolHandler::RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{
	}

TInt CProtocolHandler::Reset ()
	{
	return KErrNone;
	}


#endif // CPROTOCOLHANDLER_H
