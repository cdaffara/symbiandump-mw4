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
* Description:  CUpnpHttpChunkFileTransferReader is a class responsible for 
*                asynchronous TCP reading and controlling data
*
*/



#include "upnphttpsession.h"
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"
#include "upnphttpfiletransferreader.h"
#include "upnphttpchunkfiletransferreader.h"
#include "upnphttpchunkparser.h"
#include "upnphttpfileaccess.h"
#include "inet6err.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CUpnpHttpChunkFileTransferReader::NewL
// Factory method.
// ---------------------------------------------------------------------------
//
CUpnpHttpChunkFileTransferReader* CUpnpHttpChunkFileTransferReader::NewL( 
                                                    CUpnpTcpSession& aSession,
                                                    RSocket& aSocket,
                                                    TThreadPriority aPriority,
                                                    TInt aReadPortion,
                                                    TInt aBufferSize)
    {
    CUpnpHttpChunkFileTransferReader* self = new ( ELeave ) 
                    CUpnpHttpChunkFileTransferReader( aSession, aSocket,
                                             aPriority, aReadPortion, aBufferSize);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CUpnpHttpChunkFileTransferReader::CUpnpHttpChunkFileTransferReader
// C++ constructor.
// ---------------------------------------------------------------------------
//
CUpnpHttpChunkFileTransferReader::CUpnpHttpChunkFileTransferReader( CUpnpTcpSession& aSession,
                                              RSocket& aSocket,
                                              TThreadPriority aPriority,
                                              TInt aReadPortion,
                                              TInt aBufferSize)
    :CUpnpHttpFileTransferReader( aSession, aSocket,
               aPriority, aReadPortion, aBufferSize + aReadPortion),
    iDecoded(0)
    {        
    }

// ---------------------------------------------------------------------------
// CUpnpHttpChunkFileTransferReader::ConstructL
// Two-phased constructor
// constructor that can leave. Used from derived classes.
// ---------------------------------------------------------------------------
//
void CUpnpHttpChunkFileTransferReader::ConstructL()
    {   
    BaseConstructL();
    
    iParser = CUpnpHttpChunkParser::NewL();
    }    
// ---------------------------------------------------------------------------
// CUpnpHttpChunkFileTransferReader::~CUpnpHttpChunkFileTransferReader
// Destructor.
// ---------------------------------------------------------------------------
//
CUpnpHttpChunkFileTransferReader::~CUpnpHttpChunkFileTransferReader()
    {        
    delete iParser;    
    }

// ---------------------------------------------------------------------------
// CUpnpHttpChunkFileTransferReader::RemainingBytes
// Returns the remaining numbers of bytes to read
// ---------------------------------------------------------------------------
//
TInt CUpnpHttpChunkFileTransferReader::RemainingBytes()
    {
    if( !iSession.FileAccess() )
        return KErrNone;

    return ( iSession.FileAccess()->TransferTotal()-iSession.FileAccess()->BytesWritten() );
    }
    
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferReader::Start
// Starts reading.
// ---------------------------------------------------------------------------
//    
void CUpnpHttpChunkFileTransferReader::StartL()
    {    
    LOGS1( "%i, CUpnpHttpFileTransferReader::Start", iSession.Id() );
    
    // Initiate a new read from socket into iBuffer
    if ( !IsActive() )
    {
          
          //checks buffer buffer is not empty   
         if(iCacheBuffer.Length() > 0) 
            {
            //check buffer session should be finished
            iIsFinished = HandleL();
            if(iIsFinished)
                {
                Finish();

                if( iSession.FileAccess() )
                    iSession.FileAccess()->SetChunkCompleted( iIsFinished );
                }
            //if not issue reading
            else
                {
                IssueRead();
                iSession.StartTimeoutTimer(ETrue);
                }
        
            }
        else
            {
            IssueRead();
            }
        
    }
    } 

// ---------------------------------------------------------------------------
// CUpnpHttpChunkFileTransferReader::ReadBufferSize
// Returns the maximum size of bytes to read for once
// ---------------------------------------------------------------------------
//
TInt CUpnpHttpChunkFileTransferReader::ReadBufferSize()
    {
    return(AvailableSpace());
    }
        
// -----------------------------------------------------------------------------
// CUpnpHttpChunkParser::Parse
// Decoding the chunked-encoded buffer
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpChunkFileTransferReader::Parse( TDes8& aBuffer, TInt& aPos )
    {    
    if( aPos >= aBuffer.Length() )
        {
        iParser->SetErrorState();
        return ETrue;
        }
    return iParser->Parse( aBuffer, aPos );
    }
    
// ---------------------------------------------------------------------------
// CUpnpHttpChunkFileTransferReader::HandleL
// Handles data in buffer, returns if file transfer has finished
// ---------------------------------------------------------------------------
//
TBool CUpnpHttpChunkFileTransferReader::HandleL()
    {
    TInt error = KErrGeneral;
        
    error = Parse(iCacheBuffer, iDecoded);
            
    if( error == KErrNone && iSession.FileAccess() )
        {
        if ( iSession.FileAccess()->TransferTotal() == KErrNotFound )
            {
            iSession.FileAccess()->SetTransferTotal( 0 );
            }
        //parser finished decoding - writing all bytes
        if ( iParser->IsFinished() )
            {
            error = HandleReadDataL( iDecoded );
            }
        //buffer almost full - writing a fixed block of decoded data
        else if ( iBufferSize - iDecoded <= iReadBufferSize )
            {
            error = HandleReadDataL( iBufferSize - iReadBufferSize );
            }
    }
    iCacheBuffer.ReAllocL( iBufferSize );
    
    if( error != KErrNone )
        {        
        iSession.FileTransferReaderErrorL( error );
        return ETrue;
        }
    else if( iParser->IsFinished() )
        {
        if ( iSession.FileAccess() )
            {
            iSession.FileAccess()->SetChunkCompleted( ETrue );
            }
        iSession.FileTransferReaderDoneL();
        return ETrue;
        }
            
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpChunkFileTransferReader::HandleReadDataL
// Handles read data - trims the buffer and save data to file
// ---------------------------------------------------------------------------
//
TInt CUpnpHttpChunkFileTransferReader::HandleReadDataL( TInt aPieceToRead )
    {        
    //reads decoded data
    TPtr8 ptr = iCacheBuffer.LeftTPtr( aPieceToRead );
    
    //saves decoded data
    TInt error = iSession.FileAccess()->SaveL( ptr );
    iSession.FileAccess()->SetTransferTotal( 
        iSession.FileAccess()->TransferTotal() + aPieceToRead );
    
    //saved data are cut from buffer
    iCacheBuffer.Delete( 0, aPieceToRead );
  
    //position adjusted to the number of still encoded bytes        
    iDecoded -= aPieceToRead;			

    return error;
    }    

// ---------------------------------------------------------------------------
// CUpnpHttpChunkFileTransferReader::Finish
// On finishing the file transfer
// ---------------------------------------------------------------------------
//
void CUpnpHttpChunkFileTransferReader::Finish()
    {        
    CUpnpHttpFileTransferReader::Finish();    
    }    

// ---------------------------------------------------------------------------
// CUpnpHttpChunkFileTransferReader::RunL
// From class CActive.
// Function is called as a callback when the issued reading is completed.
// ---------------------------------------------------------------------------
//
void CUpnpHttpChunkFileTransferReader::RunL()
    {
    LOGS2( "%i, CUpnpHttpChunkFileTransferReader::RunL(), iStatus %i",
            iSession.Id(), iStatus.Int() );

    // Active object request complete handler
    switch ( iStatus.Int() )
        {
        case KErrNone:
            iRetryErrorCount = 0;
            iCancelTimer->Cancel();
 
           iSession.TimerCancel();
           iCacheBuffer.SetLength( iCacheBuffer.Length() + iLen() );            
           iIsFinished = HandleL();                                           
            
            if(!iIsFinished)
                {
                IssueRead();
                iSession.StartTimeoutTimer(ETrue);
                }
            else
                 Finish();                                     
            break;
        
        case KErrCancel:
            if(iCancelFromTimer) 
                {          
                IssueRead();
                iCancelFromTimer = EFalse;
                }
            break;
        case KErrEof:
            iSession.TimerCancel();
            iCacheBuffer.SetLength( iLen() );                    
            iIsFinished = HandleL();            
            
            if(iIsFinished)
                {
                iSession.HandleErrorL( iStatus.Int() );
                break;
                }
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
            iIsFinished = ETrue;
            iIsActivated = EFalse;            
            iSession.HandleErrorL( iStatus.Int() );
            break;
        }
        if( iSession.FileAccess() )
            iSession.FileAccess()->SetChunkCompleted( iIsFinished );
    }

// -----------------------------------------------------------------------------
// CUpnpHttpChunkFileTransferReader::RunError
// From CActive.
// Invoked when RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpChunkFileTransferReader::RunError( TInt aErr )
    {
    LOGS1("CUpnpHttpChunkFileTransferReader::RunError(%d)", aErr);
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpChunkFileTransferReader::Reset
// Makes reader ready to start reading new content
// ---------------------------------------------------------------------------
//
void CUpnpHttpChunkFileTransferReader::Reset()
    {    
    iCacheBuffer.SetLength(0);
    iDecoded = 0;
    iParser->Reset();
    }

// ---------------------------------------------------------------------------
// CUpnpHttpChunkFileTransferReader::ReadFromSocket
// Reads from the socket
// ---------------------------------------------------------------------------
//
void CUpnpHttpChunkFileTransferReader::ReadFromSocket()
    {    
    Deque();
    CActiveScheduler::Add(this);
    
    iReceivePtr.Set( (TUint8*)iCacheBuffer.RightTPtr( 0 ).Ptr(), ReadBufferSize(), ReadBufferSize() );
    StartCancelTimer();
    iSocket.RecvOneOrMore( iReceivePtr,0,iStatus,iLen);       
    SetActive();
    }        
       
//  End of File
