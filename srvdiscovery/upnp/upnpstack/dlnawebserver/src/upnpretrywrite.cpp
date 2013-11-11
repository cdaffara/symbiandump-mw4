/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/



#include "upnptcpsession.h"
#include "upnpretrywrite.h"

#ifdef _DEBUG
#define KLogFile _L("DLNAWebServer.txt")
#endif
#include "upnpcustomlog.h"

// -----------------------------------------------------------------------------
// CUpnpRetryWrite::NewL
//
// -----------------------------------------------------------------------------
//
CUpnpRetryWrite* CUpnpRetryWrite::NewL( CUpnpTcpSession& aSession, RSocket& aSocket, MUpnpRetryWriteObserver* aObserver, TThreadPriority aPriority )
    {
    CUpnpRetryWrite* self = new ( ELeave ) CUpnpRetryWrite( aSession, aSocket, aObserver, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpRetryWrite::CUpnpRetryWrite
//
// -----------------------------------------------------------------------------
//
CUpnpRetryWrite::CUpnpRetryWrite( CUpnpTcpSession& aSession, RSocket& aSocket, MUpnpRetryWriteObserver* aObserver, TThreadPriority aPriority ):CActive( aPriority ),iSession( aSession ),iSocket( aSocket ),iObserver( aObserver ),iInternalState(EUnknown),iWriteErrorsCount(0)
    {
    }

// -----------------------------------------------------------------------------
// CUpnpRetryWrite::ConstructL
//
// -----------------------------------------------------------------------------
//	
void CUpnpRetryWrite::ConstructL()
    {
    CActiveScheduler::Add( this );
    User::LeaveIfError( iWriteTimer.CreateLocal() );   
    }

// -----------------------------------------------------------------------------
// CUpnpRetryWrite::~CUpnpRetryWrite
//
// -----------------------------------------------------------------------------
//
CUpnpRetryWrite::~CUpnpRetryWrite()
    {
    Cancel();
    iWriteTimer.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpRetryWrite::IssueWriteRetry
//
// -----------------------------------------------------------------------------
//
void CUpnpRetryWrite::IssueWriteRetry()
    {
	iSession.WaitRetryError(iStatus.Int());
	iWriteTimer.After( iStatus, KWaitWithSocketWrite * ++iWriteErrorsCount );
	SetActive();
	iInternalState = EWaiting;
    }

// -----------------------------------------------------------------------------
// CUpnpRetryWrite::WriteToSocket
//
// -----------------------------------------------------------------------------
//
void CUpnpRetryWrite::WriteToSocket()
    {
	iSession.WaitRetryError(iStatus.Int(), ETrue);
	iSocket.Write( KNullDesC8, iStatus );
	SetActive();
	iInternalState = EWriting;
    }

// -----------------------------------------------------------------------------
// CUpnpRetryWrite::PassErrorToObserverL
//
// -----------------------------------------------------------------------------
//
void CUpnpRetryWrite::PassErrorToObserverL( TInt aError )
	{
	iInternalState = EUnknown;	
	iWriteErrorsCount = 0;
	iObserver->RetryWriteFailL( aError );
	}

// -----------------------------------------------------------------------------
// CUpnpRetryWrite::RunL
//
// -----------------------------------------------------------------------------
//	
void CUpnpRetryWrite::RunL()
    {
	if ( iStatus == KErrNone )
		{
		switch( iInternalState )
			{
			case EWaiting:
				WriteToSocket();
				break;
			case EWriting:
				iInternalState = EUnknown;	
				iWriteErrorsCount = 0;
				iObserver->RetryWriteSucceed();
				break;
			}
		}
	else
		{
		if( iInternalState == EWriting )
			{
			switch( iStatus.Int() )
				{
				case KErrNoMemory:
				case KErrNotReady:							
					if( iWriteErrorsCount < KWriteErrorsMax )
						{
						IssueWriteRetry();
						break;
						}							
				default:
					PassErrorToObserverL( iStatus.Int() );
					break;
				}
			}
        else
	    	{
    		if( iInternalState == EWaiting && iStatus.Int() == KErrAbort )
	    		{        		
        		iWriteTimer.After(iStatus,1);
        		iWriteErrorsCount++;
        		SetActive();
	    		}	
    		else
	    		PassErrorToObserverL( iStatus.Int() );
	    	}			
		}
    }

// -----------------------------------------------------------------------------
// CUpnpRetryWrite::DoCancel
//
// -----------------------------------------------------------------------------
//
void CUpnpRetryWrite::DoCancel()
    {
    iWriteErrorsCount = 0;
	iInternalState = EUnknown;
    }

// -----------------------------------------------------------------------------
// CUpnpRetryWrite::RunError
//
// -----------------------------------------------------------------------------
//
TInt CUpnpRetryWrite::RunError( TInt aError )
    {
    LOGS1("CUpnpRetryWrite::RunError(%d)", aError);
   	return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpRetryWrite::IsStarted
//
// -----------------------------------------------------------------------------
//
TBool CUpnpRetryWrite::IsStarted()
	{
	return (iInternalState != EUnknown);
	}

