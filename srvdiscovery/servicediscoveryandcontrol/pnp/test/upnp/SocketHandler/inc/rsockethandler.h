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
// @internalAll
// 
//

#ifndef __RSOCKETHANDLER_H__
#define __RSOCKETHANDLER_H__

#include <es_sock.h>
#include <comms-infras/eintsock.h>

#include "sockhandlerdefs.h"

using namespace SockHandler;

class CSocketHandler;


/** 
A SocketHandler observer.

Implementor of this class will be notified on completion of socket operations from a socket handler.

@internalAll
*/
class MSocketHandlerObserver
	{
public:
	virtual void OpenComplete ( RInternalSocket& aSocket ) =0;
	virtual void ConnectComplete () =0;
	virtual void AcceptComplete ( RInternalSocket& aSocket ) =0;
	virtual void SendComplete ( TInt aLength ) =0;
	virtual void SendToComplete ( TInt aLength ) =0;
	virtual void RecvComplete ( RMBufChain& aData ) =0;
	virtual void RecvFromComplete ( RMBufChain& aData, const TSockAddr& aAddr ) =0;
//	virtual void IoctlComplete () =0;
	virtual void Error ( TOperation aOperation, TInt aError  ) =0;
	};


/**

@internalAll
*/
class RSocketHandler
	{
	public:

	IMPORT_C RSocketHandler ( MSocketHandlerObserver& aObserver );
	IMPORT_C void Attach ( RInternalSocket& aSocket );

	IMPORT_C void Connect ( const TSockAddr& aAddr );
	IMPORT_C void Accept ();
	
	IMPORT_C void Send ( RMBufChain& aData );
	IMPORT_C void SendTo ( const RMBufChain& aData, TSockAddr& aAddr );
	IMPORT_C void Recv ();
	IMPORT_C void RecvFrom ();

//	IMPORT_C void Send ( const TDesC8& aData );
//	IMPORT_C void SendTo ( const TDesC8& aData, TSockAddr& aAddr );
	
//	IMPORT_C void Ioctl ( TUint aCommand, TDes8* aData = 0, TUint aLevel = KLevelUnspecified );

//	IMPORT_C void Cancel ( TOperation aOperation );
	IMPORT_C void CancelAll ();
	
	private:
	CSocketHandler* FreeSocketHandler ( TOperation aOperation );
	CSocketHandler* ActiveSocketHandler ( TOperation aOperation );
	void ActivateSocketHandler ( TSocketHandlerParams& aParams );
	
	private:
	RInternalSocket					iSocket;			// Socket to perform operations. Not owned.
	MSocketHandlerObserver&			iObserver;			// Observer for socket operational success completion & error conditons
	
	RPointerArray < CSocketHandler > iSocketHandlers;	// Number of socket handlers
	TUint32							iReserved [ 2 ];	// reserved
	};

#endif // __RSOCKETHANDLER_H__
