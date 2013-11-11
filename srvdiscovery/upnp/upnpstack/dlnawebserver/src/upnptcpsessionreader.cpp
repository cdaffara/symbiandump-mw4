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
* Description:  CUpnpTcpSessionReader is a class responsible for 
*                   asynchronous reading from socket owned by CUpnpTcpSession.
*
*/



#include "upnptcpsession.h"
#include "upnptcpsessionreader.h"
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"
#include "inet6err.h"

static const TInt KMaxRetryErrors = 5;
static const TInt KRetryWaitTime = 1000000;
static const TInt KTwoSec = 2000000;
static const TInt KOneSec = 1000000;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CUpnpTcpSessionReader::NewL
// Factory method.
// ---------------------------------------------------------------------------
//
CUpnpTcpSessionReader* CUpnpTcpSessionReader::NewL( CUpnpTcpSession& aSession,
                                                    RSocket& aSocket,
                                                    TThreadPriority aPriority,
                                                    TInt aBufferSize )
    {
    CUpnpTcpSessionReader* self = new ( ELeave ) 
                CUpnpTcpSessionReader( aSession, aSocket, aPriority, aBufferSize );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CUpnpTcpSessionReader::CUpnpTcpSessionReader
// C++ constructor.
// ---------------------------------------------------------------------------
//
CUpnpTcpSessionReader::CUpnpTcpSessionReader( CUpnpTcpSession& aSession,
                                              RSocket& aSocket, 
                                              TThreadPriority aPriority,
                                              TInt aBufferSize )
    :CActive( aPriority ),
    iSocket( aSocket ),
    iSession( aSession ),
    iBufferSize( aBufferSize )
    {
    LOGS1( "%i, CUpnpTcpSessionReader::CUpnpTcpSessionReader", iSession.Id() );
    
    }

// ---------------------------------------------------------------------------
// CUpnpTcpSessionReader::TcpConstructL
// Two-phased constructor
// constructor that can leave. Used from derived classes.
// ---------------------------------------------------------------------------
//
void CUpnpTcpSessionReader::ConstructL()
    {
    CActiveScheduler::Add( this );
    
    iReceiveBuffer.CreateL( iBufferSize );
        
    iRetryTimer = CUpnpNotifyTimer::NewL( this );
    iCancelTimer = CUpnpNotifyTimer::NewL( this );        
    }
    
// ---------------------------------------------------------------------------
// CUpnpTcpSessionReader::~CUpnpTcpSessionReader
// Destructor.
// ---------------------------------------------------------------------------
//
CUpnpTcpSessionReader::~CUpnpTcpSessionReader()
    {
    LOGS1( "%i, CUpnpTcpSessionReader::~CUpnpTcpSessionReader", iSession.Id() );
    Cancel();
    
    iReceiveBuffer.Close();  
    
    delete iRetryTimer;
    delete iCancelTimer;   
    }
    
// ---------------------------------------------------------------------------
// CUpnpTcpSessionReader::Start
// Starts reading.
// ---------------------------------------------------------------------------
//
void CUpnpTcpSessionReader::Start()
    {
    LOGS1( "%i, CUpnpTcpSessionReader::Start", iSession.Id() );
    // Initiate a new read from socket into iBuffer
    if ( !IsActive() )
        {
        IssueRead();
        }
    }    
    
// ---------------------------------------------------------------------------
// CUpnpTcpSessionReader::DoCancel
// From class CActive.
// Cancels issued reading.
// ---------------------------------------------------------------------------
//
void CUpnpTcpSessionReader::DoCancel()
    {
    LOGS1( "%i, CUpnpTcpSessionReader::DoCancel", iSession.Id() );
    // Cancel asychronous read request
    iSocket.CancelRead();
    }

// ---------------------------------------------------------------------------
// CUpnpTcpSessionReader::CancelRetry
// ---------------------------------------------------------------------------
//
void CUpnpTcpSessionReader::CancelTimers()
    {
	iRetryTimer->Cancel();
	iCancelTimer->Cancel();
    }       
    
// ---------------------------------------------------------------------------
// CUpnpTcpSessionReader::RunL
// From class CActive.
// Function is called as a callback when the issued reading is completed.
// ---------------------------------------------------------------------------
//
void CUpnpTcpSessionReader::RunL()
    {
    LOGS2( "%i, CUpnpTcpSessionReader::RunL(), iStatus %i",
    		iSession.Id(), iStatus.Int() );

    // Active object request complete handler
    switch ( iStatus.Int() )
        {
        case KErrNone:
           	iRetryErrorCount = 0;
           	iCancelTimer->Cancel();

            // Forward read data to HTTPSession 
            iSession.ReceivingCompletedL( iReceiveBuffer );
            break;
            
         case KErrCancel:
            if(iCancelFromTimer) 
                {          
                IssueRead();
                iCancelFromTimer = EFalse;
                }
            break;
       
        case KErrNoMemory:
        case KErrNotReady:
        case KErrInet6AddressExpired:
            if( iRetryErrorCount < KMaxRetryErrors )
	            {
	        	iRetryTimer->Cancel();
				iRetryTimer->After( KRetryWaitTime * ++iRetryErrorCount, EFalse );
				break;
	            }
            
        default:
            iSession.HandleErrorL( iStatus.Int() );
            break;
        }
    }
// -----------------------------------------------------------------------------
// CUpnpTcpSessionWriter::RunError
// RunError is called when RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CUpnpTcpSessionReader::RunError( TInt aError )
    {
    LOGS2( "%i, CUpnpTcpSessionReader::RunError - %d"
    		, iSession.Id(), aError);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSessionReader::TimerEventL
// Retry read
// -----------------------------------------------------------------------------
//        
void CUpnpTcpSessionReader::TimerEventL( CUpnpNotifyTimer* aTimer )
    {
    if( aTimer == iCancelTimer )
        {
        iCancelFromTimer = ETrue;
        iSocket.CancelRecv();
        }
    else if( aTimer == iRetryTimer )
        {
        IssueRead();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSessionReader::StartCancelTimer
// -----------------------------------------------------------------------------
//  
void CUpnpTcpSessionReader::StartCancelTimer()
    {
    TInt timeout = iSession.TimeoutCurrentValue();
   	iCancelTimer->Cancel();
   	
   	timeout = timeout/2 - KTwoSec;
   	if( timeout <= 0 ) timeout = KOneSec;
   	
	iCancelTimer->After( timeout, EFalse );
    iCancelFromTimer = EFalse;
    }
   
// ---------------------------------------------------------------------------
// CUpnpTcpSessionReader::IssueRead
// Issues reading.
// ---------------------------------------------------------------------------
//
void CUpnpTcpSessionReader::IssueRead()
    {
    LOGS1( "%i, CUpnpTcpSessionReader::IssueRead", iSession.Id());
    // Initiate a new read from socket into iBuffer
    if( IsActive() )
        {
        LOGS1( "%i, PANIC: CUpnpTcpSessionReader::IssueRead!", iSession.Id() );
        User::Panic( _L("CUpnpTcpSessionReader is active" ), KErrGeneral );    
        } 
                                               
    if( iSession.IsConnected() )
        {
      	LOGS1( "%i, CUpnpTcpSessionReader::IssueRead"
          	, iSession.Id());
          	
        Deque();
        CActiveScheduler::Add(this);
          	
        iReceiveBuffer.Zero();
        
        StartCancelTimer();

        iSocket.RecvOneOrMore( iReceiveBuffer, 0, iStatus, iLen );   
        SetActive();
        }
    }

    
//  End of File
