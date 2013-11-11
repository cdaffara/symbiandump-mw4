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


#include <in_sock.h>
#include <eintsock.h>
#include "rsockethandler.h"
#include "csockethandler.h"



const TInt KDefaultFlags = 0;
/**
Constructor.
*/
CSocketHandler::CSocketHandler ( RInternalSocket& aSocket, MSocketHandlerObserver& aObserver )
: CActive ( EPriorityStandard ),
	iSocket ( aSocket ),
  iObserver ( aObserver )
	{
	CActiveScheduler::Add(this);	
	}

CSocketHandler::CSocketHandler ( MSocketHandlerObserver& aObserver )
: CActive ( EPriorityNormal ),
iObserver ( aObserver )
	{
	CActiveScheduler::Add(this);
	}

/** 
Destructor.
*/
CSocketHandler::~CSocketHandler ()
	{
	Cancel ();		
	}

/**
Starts a socket operation on RInternalSocket.

@param aParams. Socket operation parameters.
*/
void CSocketHandler::Activate ( const TSocketHandlerParams& aParams )
	{
	iOperation = static_cast < TOperation > ( aParams.iArgs[0] );
	TInt err = KErrNone;
	switch ( iOperation )
		{
		case EConnect:
			{
			TSockAddr* addr = reinterpret_cast < TSockAddr* > ( aParams.iArgs[1] );
			iSocket.Connect ( *addr, iStatus );
			}
		break;
		
		case EAccept:
			{
			err = iAcceptingSocket.Open ();
			iSocket.Accept ( iAcceptingSocket, iStatus );
			}
		break;
		
		case EMBufSend:
			{
			iBufChain.Init ( );
			RMBufChain* bufChain = reinterpret_cast < RMBufChain* > ( aParams.iArgs[1] );
			iLength = 0;
			iSocket.Send ( *bufChain, KDefaultFlags, iStatus, iLength );
			}
		break;
		
/*		case EOpenByName:
			{
			const TDesC* buf = reinterpret_cast < const TDesC* > ( aParams.iArgs[1] );
			iSocket.Open ( *buf, iStatus );
			}
		break;
				
		case EDescSend:
			{
			const TDesC8* buf = reinterpret_cast < const TDesC8* > ( aParams.iArgs[1] );
			iSocket.Send ( *buf, KDefaultFlags, iStatus );
			}
		break;
		
		case EDescSendTo:
			{
			const TDesC8* buf = reinterpret_cast < const TDesC8* > ( aParams.iArgs[1] );
			iSockAddr = *(reinterpret_cast < TSockAddr* > ( aParams.iArgs[2] ));
			iSocket.SendTo ( *buf, iSockAddr, KDefaultFlags, iStatus );
			}
		break; 
		
*/		case EMBufSendTo:
			{
			iBufChain.Init ( );
			iBufChain = *(reinterpret_cast < RMBufChain* > ( aParams.iArgs[1] ));
			iSockAddr = *(reinterpret_cast < TSockAddr* > ( aParams.iArgs[2] ));
			iLength = 0;
			iSocket.SendTo ( iBufChain, iSockAddr, KDefaultFlags, iStatus, iLength );
			}
		break;

		case ERecv:
			{
			iBufChain.Init ( );
			iSocket.Recv ( iBufChain, KDefaultFlags, iStatus );
			}
		break;

		case ERecvFrom:
			{
			iBufChain.Init ();
			iSocket.RecvFrom ( iBufChain, iSockAddr, KDefaultFlags, iStatus );
			}
		break;

		case EOpenByProtocol:
			{
			TUint addrFamily = static_cast < TUint> ( aParams.iArgs[1] );	
			TUint sockType = static_cast < TUint> ( aParams.iArgs[2] );				
			TUint prot = static_cast < TUint> ( aParams.iArgs[3] );
			iSocket.Open ( addrFamily, sockType, prot, iStatus );
			}
		break;

		default:
			User::Invariant ();
		};
		
	if ( KErrNone == err )
		{
		SetActive();
		}
	else
		{
		CompleteSelf ( err );
		}
	}

void CSocketHandler::CompleteSelf ( TInt aError )
	{
    TRequestStatus* pStat = &iStatus;
    User::RequestComplete ( pStat, aError );
	SetActive ( );
    }
	
/**
Handles this idle active object's request completion event.
*/
void CSocketHandler::RunL ()
	{
	TInt err = iStatus.Int ();
	if ( err == KErrNone )
		{
		NotifyCompletion ();
		}
	else
		{
		iObserver.Error ( iOperation, err );
		}
	}

/**
Cancellation of an outstanding request.
*/
void CSocketHandler::DoCancel ()
	{
	CancelCurrent ();
	}
	
void CSocketHandler::NotifyCompletion ( )
	{
	switch ( iOperation )
		{
		case EConnect:
			{
			iObserver.ConnectComplete();
			}
		break;
		
		case EAccept:
			{
			iObserver.AcceptComplete( iAcceptingSocket );
			RInternalSocket sock;
			iAcceptingSocket = sock;
			}
		break;
		
//		case EDescSend:
		case EMBufSend:	
			{
			iObserver.SendComplete( iLength() );
			}
		break;

//		case EDescSendTo:
		case EMBufSendTo:
			{
			iObserver.SendToComplete( iLength() );
			}
		break;
			
		case ERecv:		
			{
			iObserver.RecvComplete( iBufChain );
			}
		break;
		
		case ERecvFrom:
			{
			iObserver.RecvFromComplete( iBufChain, iSockAddr );
			}
		break;

		case EOpenByProtocol:
//		case EOpenByName:
			{
			iObserver.OpenComplete(iSocket);
			}
		break;
		
/*		case EIoctl:
			iObserver.IoctlComplete();
		break;				
*/		 
		};
	}

/**
Determines whether the socket handler has a request outstanding
*/
TBool CSocketHandler::IsRunning ( TOperation aOperation )
	{
	return ( IsActive() && ( aOperation == iOperation  ) );
	}

void CSocketHandler::CancelCurrent ()
	{
	switch ( iOperation )
		{
		case EConnect:
			iSocket.CancelConnect ();
		break;
		
		case EAccept:
			iSocket.CancelAccept ();
			iAcceptingSocket.Close ();
		break;
		
//		case EDescSend:
		case EMBufSend:	
//		case EDescSendTo:
		case EMBufSendTo:
			iSocket.CancelSend ();
		break;

		case ERecv:
		case ERecvFrom:
			iSocket.CancelRecv ();
		break;

		case EOpenByProtocol:
//		case EOpenByName:
			iSocket.Close (); 
		break;	
		};
	}
