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

#include <e32base.h>
#include "rsockethandler.h"
#include "csockethandler.h"

/**
Constructor.
*/
EXPORT_C RSocketHandler::RSocketHandler ( MSocketHandlerObserver& aObserver )
: iObserver ( aObserver )
	{
	}

/**
Attaches to the open socket.

@param aSocket. The open RInternalSocket.
*/
EXPORT_C void RSocketHandler::Attach ( RInternalSocket& aSocket )
	{
	CancelAll ();
	iSocket = aSocket;
	}

/**
Connects to a remote host.

@param aAddr. A remote destination address.
*/
EXPORT_C void RSocketHandler::Connect ( const TSockAddr& aAddr )
	{
	TSocketHandlerParams params ( EConnect, &aAddr );
	ActivateSocketHandler ( params );
	}

/**
Facilitates a client/server connection from a remote socket.

On successful completion a new connected socket is returned to the socket handler observer 
and it may then be used to transfer data.
@param: void.
@return: void 
*/
EXPORT_C void RSocketHandler::Accept ()
	{
	TSocketHandlerParams params( EAccept );	
	ActivateSocketHandler ( params );
	}

/**
Sends data to remote host on a connected socket.

@param aData. The data to be sent.
*/
EXPORT_C void RSocketHandler::Send (  RMBufChain& aData )
	{
	TSocketHandlerParams params ( EMBufSend, &aData );
	ActivateSocketHandler ( params );	
	}

/**
Sends data to remote host on a unconnected socket.

@param aData. The data to be sent.
@param aAddr. A remote destination address
*/
EXPORT_C void RSocketHandler::SendTo ( const RMBufChain& aData, TSockAddr& aAddr )
	{
	TSocketHandlerParams params ( EMBufSendTo, &aData, &aAddr );
	ActivateSocketHandler ( params );	
	}

/**
Receives data from a remote host.
*/
EXPORT_C void RSocketHandler::Recv ()
	{
	TSocketHandlerParams params ( ERecv );
	ActivateSocketHandler ( params );
	
	}

/**
Receives data from a remote host through a unconnected socket and returns a source address.
*/
EXPORT_C void RSocketHandler::RecvFrom ()
	{
	TSocketHandlerParams params ( ERecvFrom );
	ActivateSocketHandler ( params );

	}

/**
Sends data to remote host on a connected socket.

@param aData. The data to be sent.

EXPORT_C void RSocketHandler::Send ( const TDesC8& aData )
	{
	TSocketHandlerParams params ( EDescSend, &aData );
	ActivateSocketHandler ( params );

	}

Sends data to remote host on a unconnected socket.

@param aData. The data to be sent.
@param aAddr. A remote destination address

EXPORT_C void RSocketHandler::SendTo ( const TDesC8& aData, TSockAddr& aAddr )
	{
	TSocketHandlerParams params ( EDescSendTo, &aData, &aAddr );
	ActivateSocketHandler ( params );
	}


Applies an asynchronous I/O control operation on a socket.

@param aCommand. Ioctl command.

EXPORT_C void RSocketHandler::Ioctl ( TUint aCommand, TDes8* aData  = 0 , TUint aLevel  = KLevelUnspecified  )
	{
	TSocketHandlerParams params ( EIoctl, aCommand, aData, aLevel );	
	ActivateSocketHandler ( params );					
	}


Cancels an outstanding socket operation.

@param aOperation. Operation to be cancelled.

EXPORT_C void RSocketHandler::Cancel ( TOperation aOperation )
	{
	CSocketHandler* handler = ActiveSocketHandler ( aOperation );
	if ( handler )
		handler->Cancel ();
	}
*/

/**
Cancels all outstanding operations.
*/
EXPORT_C void RSocketHandler::CancelAll ()
	{
	// The descrtrutor of CSocketHandler will take care of cancel
	iSocketHandlers.ResetAndDestroy ();	
	}

/**
Finds the first free socket handler to perform the socket operation and if none
available it creates a new socket handler.

@param aOperation. Socket operation to be performed.
@return A free socket handler to perform the socket operation.
*/
CSocketHandler* RSocketHandler::FreeSocketHandler ( TOperation aOperation )
	{
	CSocketHandler* freeHandler = ActiveSocketHandler ( aOperation );
	if ( freeHandler == NULL )
		{
		// No active socket handlers for this operation			
		// Find the first free one
		for ( TInt i = 0; i < iSocketHandlers.Count(); ++i )
			{
			if ( ( iSocketHandlers[i]->IsRunning ( aOperation ) ) )
				{
				return iSocketHandlers[i];	
				}
			}
		// None available. Create one, append to the array and return
		CSocketHandler* handler = new CSocketHandler ( iSocket, iObserver );
		if ( handler != NULL )
			{
			iSocketHandlers.Append ( handler );
			return handler;
			}	
		}
	else if ( freeHandler->IsRunning ( aOperation ) )
		{
		__ASSERT_DEBUG ( 1, User::Invariant () );
		}
	
	return freeHandler;
	}

/**
Finds an active socket handler which matches the socket operation.

@param aOperation. Socket operation to be performed.
@return A free socket handler to perform the socket operation.
*/
CSocketHandler* RSocketHandler::ActiveSocketHandler ( TOperation aOperation )
	{
	// Find the active socket handler.
	for ( TInt i = 0; i < iSocketHandlers.Count(); ++i )
		{
		if ( (iSocketHandlers[i]->IsRunning ( aOperation )) )
			{
			return iSocketHandlers[i];	
			}
		}
	return NULL;	
	}

/**
Finds or creates a socket handler which matches the socket operation and
submits the request to socket handler.

@param aParams. Socket operation parameters.
*/
void RSocketHandler::ActivateSocketHandler ( TSocketHandlerParams& aParams )
	{
	TOperation op = static_cast < TOperation > ( aParams.iArgs[0] );
	CSocketHandler* handler = FreeSocketHandler ( op );
	if ( handler == NULL )
		{
		// Possibly failed due to nomemory.
		iObserver.Error ( op, KErrNoMemory ); 
		return;			
		}
	handler->Activate ( aParams );
	}
