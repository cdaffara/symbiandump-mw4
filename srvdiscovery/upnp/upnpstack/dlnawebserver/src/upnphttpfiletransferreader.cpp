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
* Description:  CUpnpHttpFileTransferReader is a class responsible for 
*                asynchronous reading data from socket and controlling it
*
*/



#include "upnphttpsession.h"
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"
#include "upnphttpfiletransferreader.h"
#include "upnphttpfileaccess.h"
#include "inet6err.h"




// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::NewL
// Factory method.
// ---------------------------------------------------------------------------
//
CUpnpHttpFileTransferReader* CUpnpHttpFileTransferReader::NewL( CUpnpTcpSession& aSession,
                                                RSocket& aSocket,
                                                TThreadPriority aPriority,
                                                TInt aReadPortion,
                                                TInt aBufferSize)
    {
    CUpnpHttpFileTransferReader* self = new ( ELeave ) 
                            CUpnpHttpFileTransferReader( aSession, aSocket,
                                              aPriority, aReadPortion, aBufferSize );
    CleanupStack::PushL( self );
    self->BaseConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::CUpnpHttpFileTransferReader
// C++ constructor.
// ---------------------------------------------------------------------------
//
CUpnpHttpFileTransferReader::CUpnpHttpFileTransferReader( CUpnpTcpSession& aSession,
                                          RSocket& aSocket,
                                          TThreadPriority aPriority,
                                          TInt aReadPortion,
                                          TInt aBufferSize)
    :CActive( aPriority ),
    iSocket( aSocket ),
    iSession( aSession ),
    iReceivePtr( NULL, 0, 0 ),
    iReadBufferSize(aReadPortion),
    iBufferSize( aBufferSize )
    {
    LOGS1( "%i, CUpnpHttpFileTransferReader::CUpnpHttpFileTransferReader", iSession.Id() ); 
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::BaseConstructL
// Two-phased constructor
// constructor that can leave. Used from derived classes.
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferReader::BaseConstructL()
    {
    CActiveScheduler::Add( this );              
                
    iRetryTimer = CUpnpNotifyTimer::NewL( this );
    iCancelTimer = CUpnpNotifyTimer::NewL( this );        
    
    InitiateBufferL();
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::CalculateBufferSize
// Finds nearest higher power of 2 for the given size
// ---------------------------------------------------------------------------
//
TInt CUpnpHttpFileTransferReader::CalculateBufferSize( TInt aReadPortion, 
                                                       TInt aBufferSize )
    {
    aBufferSize -= 1;
    aBufferSize = aBufferSize | (aBufferSize >> 1);
    aBufferSize = aBufferSize | (aBufferSize >> 2);
    aBufferSize = aBufferSize | (aBufferSize >> 4);
    aBufferSize = aBufferSize | (aBufferSize >> 8);
    aBufferSize = aBufferSize | (aBufferSize >> 16);    
    aBufferSize += 1;

    if (aBufferSize <aReadPortion)
        {        
        aBufferSize = aReadPortion;
        }
    return aBufferSize;
    }            

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::~CUpnpHttpFileTransferReader
// Destructor.
// ---------------------------------------------------------------------------
//
CUpnpHttpFileTransferReader::~CUpnpHttpFileTransferReader()
    {    
    Cancel();    
    iCacheBuffer.Close();        
    delete iRetryTimer;
    delete iCancelTimer;   
    }
    
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::Start
// Starts reading.
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferReader::StartL()
    {    
    LOGS1( "%i, CUpnpHttpFileTransferReader::Start", iSession.Id() );
    if ( IsActive() )
        {        
        return;
        }
    
    if ( RemainingBytes() <= 0 )
        {
        HandleOneBufferShortTransferL();
        }
    else 
        {
        IssueRead();
        }
    }    
    
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::HandleOneBufferShortTransferL
// ---------------------------------------------------------------------------
//    
void CUpnpHttpFileTransferReader::HandleOneBufferShortTransferL()
    {    
    TInt totalLength = iSession.FileAccess()->TransferTotal();
    if ( totalLength > 0 && totalLength < iCacheBuffer.Length() )
        {
        //cut off malicious bytes exceeding contenth-length
        iCacheBuffer.SetLength( totalLength );
        }
    TInt error = iSession.FileAccess()->SaveL( iCacheBuffer );
            
    if ( error != KErrNone )
        {                
        iSession.FileTransferReaderErrorL( error );
        LOGSH( error, "error in HandleOneBufferShortTransferL" );
        return;
        }
    iSession.FileTransferReaderDoneL();
    Finish();
    }     

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::DoCancel
// From class CActive.
// Cancels issued reading.
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferReader::DoCancel()
    {
    LOGS1( "%i, CUpnpHttpFileTransferReader::DoCancel", iSession.Id() );
    // Cancel asychronous read request
    iSocket.CancelRead();
    }
    
// ---------------------------------------------------------------------------
// CUpnpTcpSessionReader::CancelRetry
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferReader::CancelTimers()
    {
    iRetryTimer->Cancel();
    iCancelTimer->Cancel();
    }       
    
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::RunL
// From class CActive.
// Function is called as a callback when the issued reading is completed.
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferReader::RunL()
    {
    LOGS2( "%i, CUpnpHttpFileTransferReader::RunL(), iStatus %i",
            iSession.Id(), iStatus.Int() );

    // Active object request complete handler
    switch ( iStatus.Int() )
        {
        case KErrNone:
            iRetryErrorCount = 0;
            iCancelTimer->Cancel();

            iSession.TimerCancel();
            iCacheBuffer.SetLength( iCacheBuffer.Length() + iLen() );
            if ( ( iCacheBuffer.MaxLength() == iCacheBuffer.Length() ) 
                || !(RemainingBytes() > 0))
                {                
                iIsFinished = HandleL();                               
                }                                      
            if ( !iIsFinished )
                {
                IssueRead();
                iSession.StartTimeoutTimer( ETrue );
                }
            else
                {                
                Finish();
                }                                       
            break;
         case KErrCancel:
            if ( iCancelFromTimer ) 
                {          
                IssueRead();
                iCancelFromTimer = EFalse;
                }
            break;            
        case KErrEof:
            iSession.TimerCancel();
            iCacheBuffer.SetLength( iCacheBuffer.Length() + iLen() );
            iIsFinished = HandleL();                        
            if ( iIsFinished )
                {
                iSession.HandleErrorL( iStatus.Int() );
                break;
                }                       
        case KErrNoMemory:
        case KErrNotReady:
        case KErrInet6AddressExpired:
            if ( iRetryErrorCount < KMaxRetryErrors )
                {
                iRetryTimer->Cancel();
                iRetryTimer->After( KRetryWaitTime * ++iRetryErrorCount, EFalse );
                break;
                }
        default:            
            iIsFinished = ETrue;
            iIsActivated = EFalse;
            if ( iSession.FileAccess() ) 
                { 
                iSession.FileAccess()->DeleteFile();            
                }
            iSession.HandleErrorL( iStatus.Int() );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSessionWriter::RunError
// RunError is called when RunL leaves.
// -----------------------------------------------------------------------------
//

TInt CUpnpHttpFileTransferReader::RunError( TInt /*aError*/ )
    {  
    LOGS( "CUpnpHttpFileTransferReader::RunError"); 
    HandleError();      
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSessionReader::TimerEventL
// Retry read
// -----------------------------------------------------------------------------
//        
void CUpnpHttpFileTransferReader::TimerEventL( CUpnpNotifyTimer* aTimer )
    {
    if ( aTimer == iCancelTimer )
        {
        LOGS( "CUpnpHttpFileTransferReader::TimerEventL, canceltimer");
        iCancelFromTimer = ETrue;
        iSocket.CancelRecv();
        }
    else if ( aTimer == iRetryTimer )
        {
        IssueRead();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSessionReader::StartCancelTimer
// -----------------------------------------------------------------------------
//  
void CUpnpHttpFileTransferReader::StartCancelTimer()
    {
    LOGS( "CUpnpHttpFileTransferReader::StartCancelTimer");
    TInt timeout = iSession.TimeoutCurrentValue();
    iCancelTimer->Cancel();
       
    timeout = timeout/2 - KTwoSec;
    if ( timeout <= 0 ) 
        {
        timeout = KOneSec;
        }
    iCancelTimer->After( timeout, EFalse );
    iCancelFromTimer = EFalse;
    }
   
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::IssueRead
// Issues reading.
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferReader::IssueRead()
    {       
    if ( IsActive() )
        {
        LOGS1( "%i, PANIC: CUpnpHttpFileTransferReader::IssueRead!", 
                iSession.Id() );
        User::Panic( _L("CUpnpHttpFileTransferReader is active" ), KErrGeneral );    
        } 
                                               
    if ( iSession.IsConnected() )
        {          
        ReadFromSocket();
        }
    }
    
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::RemainingBytes
// Returns the remaining numbers of bytes to read
// ---------------------------------------------------------------------------
//
TInt CUpnpHttpFileTransferReader::RemainingBytes()
    {
    LOGS( "CUpnpHttpFileTransferReader::RemainingBytes()");
    if ( !iSession.FileAccess() )
        {        
        return 0;
        }    
    if ( iSession.FileAccess()->TransferTotal() == KErrNotFound ) 
        {        
        return ( TcpFinFoundRemainingBytes() );
        }    
    return ( iSession.FileAccess()->TransferTotal() - 
             iSession.FileAccess()->BytesWritten() - iCacheBuffer.Length() );
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::ReadBufferSize
// Returns the maximum size of bytes to read for once
// ---------------------------------------------------------------------------
//
TInt CUpnpHttpFileTransferReader::ReadBufferSize()
    {
    LOGS( "CUpnpHttpFileTransferReader::ReadBufferSize()");
    if ( !iSession.FileAccess() )
        {        
        return 0;
        }
    //checks how much space in the buffer
    TInt availableSpace = AvailableSpace();
    
    //content-length no specified            
    if( iSession.FileAccess()->TransferTotal() == KErrNotFound )
        {        
        return availableSpace;
        }
        
    TInt remainingBytes = RemainingBytes();     
    //chooses free space in the buffer or the num of remaining bytes to read        
    return ( availableSpace >= remainingBytes ) ? remainingBytes : availableSpace;    
    }               
    
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::HandleL
// Handles adata in buffer, returns if file transfer has finished
// ---------------------------------------------------------------------------
//
TBool CUpnpHttpFileTransferReader::HandleL()
    {
    LOGS( "CUpnpHttpFileTransferReader::HandleL");
    TInt error = KErrGeneral;    
    if ( iSession.FileAccess() )
        {
        LOGS( "calling iFile->SaveL()");
        TPtr8 ptr = iCacheBuffer.LeftTPtr( iCacheBuffer.Size() );
        error = iSession.FileAccess()->SaveL( ptr );
        iCacheBuffer.SetLength(0);
        }    
    if ( error != KErrNone )
        {                
        iSession.FileTransferReaderErrorL( error );
        LOGS( "returning ETrue after FileTransferReaderErrorL" );
        return ETrue;
        }
    else if ( !( RemainingBytes() > 0 ) )
        {
        iSession.FileTransferReaderDoneL();
        LOGS( "returning ETrue FileTransferReaderDoneL");
        return ETrue;
        }        
    LOGS( "returning EFalse"); 
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::Finish
// Finishes session
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferReader::Finish()
    {            
    iIsActivated = EFalse;
    iSession.StartClosingSession();        
    }    

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::Reset
// Makes reader ready to start reading new content
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferReader::Reset()
    {    
    iIsFinished = EFalse;
    iIsActivated = EFalse;
    iCacheBuffer.SetLength(0);
    }
    
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::Activated
// Checks if the filetransfer reader is started and in use
// ---------------------------------------------------------------------------
//
TBool CUpnpHttpFileTransferReader::Activated()
    {    
    return iIsActivated;
    } 

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::SetActivated
// Filetransfer reader taken in use
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferReader::SetActivated( TBool aValue )
    {    
    iIsActivated = aValue;
    }
        
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::ReadFromSocket
// Reads data from socket
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferReader::ReadFromSocket()
    {    
    Deque();
    CActiveScheduler::Add(this);
    
    TInt len = ReadBufferSize();
    TPtr8 ptr = iCacheBuffer.RightTPtr( 0 );
    iReceivePtr.Set( (TUint8*)ptr.Ptr(), len, len );
    StartCancelTimer();
    iSocket.RecvOneOrMore( iReceivePtr, 0, iStatus, iLen );   
    SetActive();
    }        

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::AppendL(const TDesC8& aBeginning)
// Appends beginning data to the buffer
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferReader::AppendL( const TDesC8& aBeginning )
    {    
    //if no space for this operation, abort it,it shouldn't occur
    if ( iCacheBuffer.MaxSize() - iCacheBuffer.Length() < aBeginning.Length() )
        {        
        User::Leave(KErrAbort);
        }
    
    iCacheBuffer.Append( aBeginning );
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::InitiateBufferL()
// Initiates the buffer
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferReader::InitiateBufferL()
    {    
    if ( !iSession.FileAccess() )
        {        
        User::Leave( KErrGeneral );  
        }
    
    iCacheBuffer.Close();    
    iCacheBuffer.CreateL( iBufferSize );
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::HandleError
// Handles error from RunL
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferReader::HandleError()
    {
    if ( iSession.FileAccess() )
        {
        iSession.FileAccess()->DeleteFile();        
        }
    iIsFinished = ETrue;
    Finish();        
    }    

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::TcpFinFoundRemainingBytes
// Detects end of body when no content length
// ---------------------------------------------------------------------------
//
TInt CUpnpHttpFileTransferReader::TcpFinFoundRemainingBytes()
    {
    if ( iStatus.Int() == KErrEof )
        {
        //the num of written bytes will be the transfer total
        iSession.FileAccess()->SetTransferTotal( 
                                   iSession.FileAccess()->BytesWritten() );
        return 0;
        }        
    else
        {        
        return AvailableSpace();        
        }
    }  
       
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::AvailableSpace
// Returns num of bytes that will still fit into the buffer
// ---------------------------------------------------------------------------
//
TInt CUpnpHttpFileTransferReader::AvailableSpace()
    {
    TInt space = (iBufferSize < iCacheBuffer.Length() + iReadBufferSize) ?
                iBufferSize - iCacheBuffer.Length(): iReadBufferSize;    
    return space;        
    }  
        
//  End of File
