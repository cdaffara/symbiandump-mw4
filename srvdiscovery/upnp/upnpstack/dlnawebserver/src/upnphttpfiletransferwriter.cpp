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
* Description:  CUpnpHttpFileTransferWriter is a class responsible for 
*                   asynchronous writing to a socket owned by CUpnpTcpSession.
*
*/



#include "upnptcpsession.h"
#include "upnphttpfiletransferwriter.h"
#include "upnphttpfileaccess.h"
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::NewL
// Factory method.
// ---------------------------------------------------------------------------
//
CUpnpHttpFileTransferWriter* CUpnpHttpFileTransferWriter::NewL( CUpnpTcpSession& aSession,
                                                    RSocket& aSocket,
                                                    TThreadPriority aPriority,
                                                    TInt aWriteSize )
    {
    CUpnpHttpFileTransferWriter* self = new ( ELeave ) 
            CUpnpHttpFileTransferWriter( aSession, aSocket, aPriority, aWriteSize );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::CUpnpHttpFileTransferWriter
// C++ constructor.
// ---------------------------------------------------------------------------
//
CUpnpHttpFileTransferWriter::CUpnpHttpFileTransferWriter( CUpnpTcpSession& aSession,
                                              RSocket& aSocket, 
                                              TThreadPriority aPriority,
                                              TInt aWriteSize )
    :CActive( aPriority ),
    iSocket( aSocket ),
    iSession( aSession ),
    iState(EUnknown),
    iSendPointer(NULL,0),
    iWriteSize(aWriteSize)
    {
    LOGS1( "%i, CUpnpHttpFileTransferWriter::CUpnpHttpFileTransferWriter", iSession.Id() );
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CUpnpTcpSessionReader::ConstructL
// Two-phased constructor
// constructor that can leave. Used from derived classes.
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferWriter::ConstructL()
    {
    CActiveScheduler::Add( this );   
    iRetryWrite = CUpnpRetryWrite::NewL( iSession, iSocket, this, (TThreadPriority)Priority() );
    
    iSendBuffer.CreateL( iWriteSize );
    }
    
// ---------------------------------------------------------------------------
// CUpnpTcpSessionReader::~CUpnpTcpSessionReader
// Destructor.
// ---------------------------------------------------------------------------
//
CUpnpHttpFileTransferWriter::~CUpnpHttpFileTransferWriter()
    {    
    Cancel();    
    
    iSendBuffer.Close();
    
    delete iRetryWrite;
    }
 
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::DoCancel
// From class CActive
// Cancels issued writing request.
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferWriter::DoCancel()
    {
    LOGS1( "%i, CUpnpHttpFileTransferWriter::DoCancel", iSession.Id() );
    
	iState = ECancelled;
    }
    
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::RunL
// From class CActive
// Function called when issued writing is completed. Function checks if there
// are any write requests left to send and if there are then it issues writing.
// If queue is empty then function informs upper layer that writing is completed.
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferWriter::RunL()
    {
    LOGS2( "%i, CUpnpHttpFileTransferWriter::RunL(), iStatus %i", 
    		iSession.Id() , iStatus.Int() );
    
    // Active object request complete handler
    if ( iStatus == KErrNone )
        {
        switch( iState )
            {
            // Character has been written to socket
            case EHeaders:
            	//100-continue not expected
            	if(!iSession.FileAccess())
            		{
            		Cancel();
            		break;
            		}
            	else 
            		{
            		iState = EContent;            		
            		}
            case EContent:            	
            	SendNextPortionL();            
                
                break;
            case EFinished:                    	    	
            	iSession.FileTransferWriterDoneL();

            	iSession.StartTimeoutTimer(EFalse);            	                      	
				FinishL();
            	//export finished
            	break;    
            
            default:                
                break;
            };
        }
    else
        {
           if( (iState == EHeaders || iState == EContent || iState == EFinished) &&
              (iStatus.Int() == KErrNoMemory || iStatus.Int() == KErrNotReady ) )
           {
				iRetryWrite->IssueWriteRetry();
    	   }
		   else
	       {
				// Error: pass it up to session
				iSession.HandleErrorL( iStatus.Int() );
				iState = ECancelled;
			}
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::RunError
// RunError is called when RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpFileTransferWriter::RunError( TInt aError )
    {
    LOGS2( "%i, CUpnpHttpFileTransferWriter::RunError %d"
    		, iSession.Id() , aError);
    
    iState = EFinished;        
                
    TRAP_IGNORE( HandleErrorL() );
    iSession.StartClosingSession();
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::SendNextWriteRequestL
// Function checks if there are any write requests left and if there are then
// issues writing and removes this request from queue.
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferWriter::SendNextPortionL()
    {
    LOGS2( "%i, CUpnpHttpFileTransferWriter::SendNextPortionL(), iState %i",
    		iSession.Id() , iState );
	
	if( iSession.FileAccess() )
		{
		if( iState == EContent )    	
	    	if( !IsActive() )
	    		{      	
		    	iSendPointer.Set( (TUint8*)iSendBuffer.Ptr(),0,iSendBuffer.MaxLength() );
		        if( iSession.FileAccess()->GetL(iSendPointer,iSendBuffer.MaxLength()) )
		        	{
		        	iState = EFinished;     		
		        	}
		        
				
				iSession.TimerCancel();        
		        WriteToSocket();
		        iSession.StartTimeoutTimer(ETrue);	       
		        
	    		}
		}
	else
		{
		Cancel();
		}
  
    }  

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::StartL
// Passes control to the reader
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferWriter::StartL()
    {
    iWaiting = EFalse;    
	switch(iState)
		{
			case EUnknown:			
				
				iState = EHeaders;
				SendHeadersL();
				break;
			case EHeaders:
				
				if(iSession.FileAccess()->HeadersSent())
					{
					iState = EContent;				
					SendNextPortionL();
					}
				else
					SendHeadersL();
				break;
				
			default:
				break;
		}
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::StartL
// Sends the HTTP headers of the content
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferWriter::SendHeadersL()
    {
    if(iState == EHeaders)
    	if( iSession.FileAccess() )
    		if(!IsActive())
    			{ 
    			//in case session not connected,waiting...
    			if(!iSession.IsConnected())
					{
					iWaiting = ETrue;
					return;
					}   			
				iSendBuffer.Zero();
				
				if ( ( iSession.FileAccess()->GetHeaders().Length() -
				       iSession.FileAccess()->TransferredBytes()) <= 
		               iSendBuffer.MaxLength() )
					{
					TPtrC8 headerPointer;
					headerPointer.Set(iSession.FileAccess()->GetHeaders());
					headerPointer.Set( headerPointer.Right( 
		            iSession.FileAccess()->GetHeaders().Length()-
		                iSession.FileAccess()->TransferredBytes()) );
				
					iSendBuffer.Append( headerPointer );			
					iSession.FileAccess()->SetHeadersSent();

                    // Adding file content to the header
                    TInt length = iSendBuffer.MaxLength()-iSendBuffer.Length();
                    						
                    HBufC8* tmpBuffer = HBufC8::NewLC(length);
					
                    iSendPointer.Set(tmpBuffer->Des());
                    if( iSession.FileAccess()->GetL(iSendPointer,length) )
                        {
                        iState = EFinished;
                        }
                    iSendBuffer.Append(iSendPointer);
                    CleanupStack::PopAndDestroy( tmpBuffer );
					}
				else
					{
					TPtrC8 headerPointer;
					headerPointer.Set( iSession.FileAccess()->GetHeaders() );
					headerPointer.Set( headerPointer.Right( 
		            iSession.FileAccess()->GetHeaders().Length() -
		                iSession.FileAccess()->TransferredBytes() ) );
					headerPointer.Set( headerPointer.Left( 
		                iSendBuffer.Length() ) );
					
					iSendBuffer.Append( headerPointer );				
					iSession.FileAccess()->SetPosOfHeader( 
					    iSession.FileAccess()->TransferredBytes() + headerPointer.Length() );							
					}
		    	
		    	iSendPointer.Set((TUint8*)iSendBuffer.Ptr(),iSendBuffer.Length(),iSendBuffer.MaxLength());
    	   		
    	   		iSession.TimerCancel();        
	        	WriteToSocket();
	        	iSession.StartTimeoutTimer(ETrue);
	        	
	        	if(!iHttpPostStarted)
	        		{
	        		iSession.NotifyUpperLayersOnPostL();
	        		iHttpPostStarted = ETrue;
	        		}
	        		
    			}
    }          

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::Reset
// Resets the state of the writer
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferWriter::Reset()
    {
    iState = EUnknown;  
    iHttpPostStarted = ETrue;  
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::WriteToSocket
// Writes data to socket
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferWriter::WriteToSocket()
    {
	Deque();
	CActiveScheduler::Add(this);  

	iSocket.Write(iSendPointer,iStatus);
	SetActive();
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::IsWaiting
// Checks if file transfer writer is in use
// ---------------------------------------------------------------------------
//
TBool CUpnpHttpFileTransferWriter::IsWaiting()
    {
	return iWaiting;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::IsActivated
// Writes data to socket
// ---------------------------------------------------------------------------
//
TBool CUpnpHttpFileTransferWriter::IsActivated()
    {
	return (!iWaiting && iState!=EUnknown);
    } 

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::IsRetrying
// if waiting to retry writing to a socket
// ---------------------------------------------------------------------------
//
TBool CUpnpHttpFileTransferWriter::IsRetrying()
    {
	return (iRetryWrite->IsStarted());
    }
    
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::IsCancelled
// Checks if transfer is cancelled
// ---------------------------------------------------------------------------
//
TBool CUpnpHttpFileTransferWriter::IsCancelled()
    {
	return (iState== ECancelled);
    }        
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::FinishL
// Finishes download
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferWriter::FinishL()
    {
	iSession.FileTransferWriterHandleMoreL();
    }    
    
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::HandleErrorL
// Handles RunL error
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferWriter::HandleErrorL()
    {
    iSession.FileTransferWriterDoneL(); // 200ok to upper layers + delete iFileServe    
    FinishL();                          // serving uotstanding requests  
    }
        
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::SetPostNotify
// Post notify will be sent
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileTransferWriter::SetPostNotify()
    {
	iHttpPostStarted = EFalse;
    }    
        
// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::RetryWriteFailL
// 
// ---------------------------------------------------------------------------
//    
void CUpnpHttpFileTransferWriter::RetryWriteFailL( TInt aError )
	{
	// Error: pass it up to session
	iSession.HandleErrorL( aError );
    iState = ECancelled;
	}

// ---------------------------------------------------------------------------
// CUpnpHttpFileTransferWriter::RetryWriteSucceed
// 
// ---------------------------------------------------------------------------
// 
void CUpnpHttpFileTransferWriter::RetryWriteSucceed()
	{
	WriteToSocket();
	}
    
//  End of File
