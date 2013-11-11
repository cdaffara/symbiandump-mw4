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
// @internalTechnology
// 
//

#ifndef __CSOCKETHANDLER_H__
#define __CSOCKETHANDLER_H__

#include <e32base.h>
#include <e32cmn.h>
#include <es_mbuf.h>
#include <es_sock.h>
#include <sockhandlerdefs.h>
#include <eintsock.h>

// Forward class references.
class MSocketHandlerObserver;

using namespace SockHandler;
/**
This is an active object which performs all the asynchronous operations of RInternalSocket. 
On completion of the operation CSocketHandler will notify its observer.

@internalComponent
*/
class CSocketHandler : public CActive
	{
	public:
		CSocketHandler ( RInternalSocket& aSocket, MSocketHandlerObserver& aObserver );
		CSocketHandler ( MSocketHandlerObserver& aObserver );
		~CSocketHandler ();
		
		void Activate ( const TSocketHandlerParams& aParams );
		TBool IsRunning ( TOperation aOperation );
		
	protected:	
		// CActive overloads
		void RunL ();
		void DoCancel ();
	
	private:
		void NotifyCompletion ();
		void CancelCurrent ();
		void CompleteSelf ( TInt aError );

	private:
		RInternalSocket				iSocket;
		MSocketHandlerObserver&		iObserver;	// Observer for socket operational success completion & error conditons
		TOperation					iOperation;
		
		RMBufChain					iBufChain;
		TSockAddr					iSockAddr;
		RInternalSocket				iAcceptingSocket;
		TSockXfrLength 				iLength;
	};

#endif // __CSOCKETHANDLER_H__
