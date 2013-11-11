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
* Description:  CUpnpTcpSessionWriter is a class responsible for 
*                   asynchronous writing to a socket owned by CUpnpTcpSession.
*
*/



#include "upnptcpsession.h"
#include "upnptcpsessionwriter.h" 
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"


class CUpnpTcpWriteRequest : public CBase
  	{
public:
  	static CUpnpTcpWriteRequest* NewL( const TDesC8& aBuffer )
		    {
		    CUpnpTcpWriteRequest* self = new (ELeave) CUpnpTcpWriteRequest();
		    CleanupStack::PushL( self );
		    self->ConstructL( aBuffer );
		    CleanupStack::Pop();
		
		    return self;
		    }
    
private:
	  CUpnpTcpWriteRequest() {}
	  void ConstructL( const TDesC8& aBuffer )
		    {
		    iBuffer.CreateL( aBuffer );
		    }
    
public:
  	TDesC8& Buffer()
		    {
		    return iBuffer;
		    }
  
	  ~CUpnpTcpWriteRequest()
		    {
		    iBuffer.Close();
		    }
    
private:
  	RBuf8 iBuffer;
  	};
  
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CUpnpTcpSessionWriter::NewL
// Factory method.
// ---------------------------------------------------------------------------
//
CUpnpTcpSessionWriter* CUpnpTcpSessionWriter::NewL( CUpnpTcpSession& aSession,
                                                    RSocket& aSocket,
                                                    TThreadPriority aPriority )
    {
    CUpnpTcpSessionWriter* self = new ( ELeave ) 
            CUpnpTcpSessionWriter( aSession, aSocket, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CUpnpTcpSessionWriter::CUpnpTcpSessionWriter
// C++ constructor.
// ---------------------------------------------------------------------------
//
CUpnpTcpSessionWriter::CUpnpTcpSessionWriter( CUpnpTcpSession& aSession,
                                              RSocket& aSocket, 
                                              TThreadPriority aPriority )
    :CActive( aPriority ),
    iSocket( aSocket ),
    iSession( aSession ),
    iInitialPriority( aPriority )
    {
    LOGS1( "%i, CUpnpTcpSessionWriter::CUpnpTcpSessionWriter", iSession.Id() );
    }

// ---------------------------------------------------------------------------
// CUpnpTcpSessionReader::TcpConstructL
// Two-phased constructor
// constructor that can leave. Used from derived classes.
// ---------------------------------------------------------------------------
//
void CUpnpTcpSessionWriter::ConstructL()
    {
    CActiveScheduler::Add( this );
    iState = ENotConnected;
    iRetryWrite = CUpnpRetryWrite::NewL( iSession, iSocket, this, (TThreadPriority)Priority() );
    }
    
// ---------------------------------------------------------------------------
// CUpnpTcpSessionReader::~CUpnpTcpSessionReader
// Destructor.
// ---------------------------------------------------------------------------
//
CUpnpTcpSessionWriter::~CUpnpTcpSessionWriter()
    {
    LOGS1( "%i, CUpnpTcpSessionWriter::~CUpnpTcpSessionWriter", iSession.Id() );
    
    Cancel();
    iState = EDisconnected;
    
    iWriteRequestList.ResetAndDestroy();
    iWriteRequestList.Close();
  
    iSendBuffer.Close();

    delete iRetryWrite;
    }

// ---------------------------------------------------------------------------
// CUpnpTcpSessionWriter::IssueWriteL
// Issues writing request if session is connected and the socket is not 
// already sending. If metioned conditions are not met then it queues the request.
// ---------------------------------------------------------------------------
//
void CUpnpTcpSessionWriter::IssueWriteL( const TDesC8& aBuffer )
    {
    LOGS1( "%i, CUpnpTcpSessionWriter::IssueWriteL(TDesC8&)", iSession.Id() );
    
    // Don't do anything because socket is already closed or we are writing already.
    if( iState == EDisconnected || (iSession.DownloadOngoing() ) 
        	|| (iSession.ShuttingDown() && iState != ENotConnected) )
        return;
    
    // If session not connected yet or is already sending then queue the write request
    if ( iState == ENotConnected || iState == EWriting )
        {
        CUpnpTcpWriteRequest* req = CUpnpTcpWriteRequest::NewL( aBuffer );
        CleanupStack::PushL(req);
        User::LeaveIfError(iWriteRequestList.Append( req ));
        CleanupStack::Pop();
        
        LOGS2(  "%i, CUpnpTcpSessionWriter::IssueWriteL - queue size=%i,"
                          , iSession.Id(), iWriteRequestList.Count() );
        return;
        }
    
    LOGS2( "%i, CUpnpTcpSessionWriter::IssueWriteL queue size=%i"
                     , iSession.Id() , iWriteRequestList.Count() );
                     
    // If session already connected then just prepare sending buffer and request 
    // send from socket

    iSendBuffer.Close();
    iSendBuffer.CreateL( aBuffer );
    
    /*if (Priority() == iInitialPriority)       
        SetPriority( iInitialPriority - 5);       
    else 
        SetPriority( iInitialPriority); */
        
    Deque();
    CActiveScheduler::Add(this);  
    
    iSocket.Write( iSendBuffer, iStatus );
    iState = EWriting;  
    
    SetActive();
    }
    
// ---------------------------------------------------------------------------
// CUpnpTcpSessionWriter::SessionConnected
// Informs writer if session is connected. If it is then writer checks if there
// are queued write requests. If there are any then it issues writing.
// ---------------------------------------------------------------------------
//
void CUpnpTcpSessionWriter::SessionConnected( TBool aConnected )
    {
    LOGS2( "%i, CUpnpTcpSessionWriter::SessionConnected(),aConnected %i"
    		, iSession.Id() , aConnected );
    if( aConnected && iState == ENotConnected )
        {
        iState = EWaiting;
        TRAP_IGNORE( SendNextWriteRequestL() );
        }
    else if( !aConnected && iState != ENotConnected )
        {
        Cancel();
        iState = EDisconnected;
        }
    } 
    
// ---------------------------------------------------------------------------
// CUpnpTcpSessionWriter::HasWriteRequestsToSend
// Returns true if there are any write requests queued.
// ---------------------------------------------------------------------------
//
TBool CUpnpTcpSessionWriter::HasWriteRequestsToSend()
    {
    return ( iWriteRequestList.Count() > 0 );
    }

// ---------------------------------------------------------------------------
// CUpnpTcpSessionWriter::IsWriting
// Returns true if writer is writing data to the socket at the moment.
// ---------------------------------------------------------------------------
//          
TBool CUpnpTcpSessionWriter::IsWriting()
  {
  return iState == EWriting;    
  }
  
// ---------------------------------------------------------------------------
// CUpnpTcpSessionWriter::DoCancel
// From class CActive
// Cancels issued writing request.
// ---------------------------------------------------------------------------
//
void CUpnpTcpSessionWriter::DoCancel()
    {
    LOGS1( "%i, CUpnpTcpSessionWriter::DoCancel", iSession.Id() );
    
    // Cancel asychronous write request
    iState = EWaiting;
    }
    
// ---------------------------------------------------------------------------
// CUpnpTcpSessionWriter::RunL
// From class CActive
// Function called when issued writing is completed. Function checks if there
// are any write requests left to send and if there are then it issues writing.
// If queue is empty then function informs upper layer that writing is completed.
// ---------------------------------------------------------------------------
//
void CUpnpTcpSessionWriter::RunL()
    {
    LOGS2( "%i, CUpnpTcpSessionWriter::RunL(), iStatus %i", 
    		iSession.Id() , iStatus.Int() );
    
    // Active object request complete handler
    if ( iStatus == KErrNone )
        {
        switch( iState )
            {
            // Character has been written to socket
            case EWriting:
                
                if (Priority() == iInitialPriority)       
                    SetPriority( iInitialPriority + 5);       
                else 
                    SetPriority( iInitialPriority); 
                
                // Change state because write request is finished already
                iState = EWaiting;
                if( HasWriteRequestsToSend() )
                    SendNextWriteRequestL();
                else
                    iSession.WritingCompletedL();
                break;
            
            default:
                LOGS2( "%i, PANIC: CUpnpTcpSessionWriter::RunL, %i",
                		iSession.Id() , iState );
                User::Panic( _L("Writer::RunL"), iState );
                break;
            };
        }
    else
        {
			if( iState == EWriting && (iStatus.Int() == KErrNoMemory || iStatus.Int() == KErrNotReady ) )
			{
				iRetryWrite->IssueWriteRetry();
       		}
        else
	        {

				// Error: pass it up to session
				iState = EWaiting;
				iSession.HandleErrorL( iStatus.Int() );
			}
        }
    }
// -----------------------------------------------------------------------------
// CUpnpTcpSessionWriter::RunError
// RunError is called when RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CUpnpTcpSessionWriter::RunError( TInt aError )
    {
    LOGS2( "%i, CUpnpTcpSessionWriter::RunError %d"
    		, iSession.Id() , aError);
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CUpnpTcpSessionWriter::SendNextWriteRequestL
// Function checks if there are any write requests left and if there are then
// issues writing and removes this request from queue.
// ---------------------------------------------------------------------------
//
void CUpnpTcpSessionWriter::SendNextWriteRequestL()
    {
    LOGS2( "%i, CUpnpTcpSessionWriter::SendNextWriteRequest(), iState %i",
    		iSession.Id() , iState );
    switch( iState )
        {
        case EWaiting:
            // Write queued request
            if ( iWriteRequestList.Count() > 0 )
            		{
                CUpnpTcpWriteRequest* req = iWriteRequestList[0];
                if ( req )
                    {
                    iWriteRequestList.Remove(0);
                    iWriteRequestList.Compress();
                    CleanupStack::PushL( req );
                    IssueWriteL( req->Buffer() );
                    CleanupStack::PopAndDestroy( req );
                    }
            }
            break;

        case ENotConnected:
        case EDisconnected:
            LOGS2( "%i, PANIC: CUpnpTcpSessionWriter::SendNextWriteRequest, %i",
            		iSession.Id() , iState );
            User::Panic( _L("SendNext::ENotConnected or EDisconnected"), iState );
            break;
        }
    }  

// ---------------------------------------------------------------------------
// CUpnpTcpSessionWriter::RetryWriteFailL
//    
// ---------------------------------------------------------------------------
//    
void CUpnpTcpSessionWriter::RetryWriteFailL( TInt aError )
	{
	// Error: pass it up to session
	iSession.HandleErrorL( aError );
	iState = EWaiting;
	}

// ---------------------------------------------------------------------------
// CUpnpTcpSessionWriter::RetryWriteSucceed
//    
// ---------------------------------------------------------------------------
//    
void CUpnpTcpSessionWriter::RetryWriteSucceed()
	{
    if ( iSendBuffer.Length() == 0 ) return;
    
    Deque();
    CActiveScheduler::Add(this);  
    
    iSocket.Write( iSendBuffer, iStatus );
    iState = EWriting;         
    
    SetActive();
	}

// ---------------------------------------------------------------------------
// CUpnpTcpSessionWriter::IsRetrying
//    
// ---------------------------------------------------------------------------
//    
TBool CUpnpTcpSessionWriter::IsRetrying()
	{
	return (iRetryWrite->IsStarted());
	}

//  End of File
