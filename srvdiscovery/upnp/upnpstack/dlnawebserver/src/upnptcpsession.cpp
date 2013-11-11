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
* Description:  Declares ControlPoint class.
*
*/


// INCLUDE FILES

#include "upnptcpsessionreader.h"
#include "upnphttpfiletransferreader.h"
#include "upnphttpchunkfiletransferreader.h"
#include "upnphttpfiletransferwriter.h"
#include "upnptcpsessionwriter.h"
#include "upnphttpsession.h"
#include "upnphttpfileaccess.h"
#include "upnphttpmessagefactory.h"
#include "upnphttpbuffer.h"
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"

//CONSTANTS
const TInt KMaxExportRetry = 0;
 
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpTcpSession::CUpnpTcpSession
// C++ constructor used when remote host issued connection to server
// -----------------------------------------------------------------------------
//
CUpnpTcpSession::CUpnpTcpSession( RSocket& aSocket,
                                  TThreadPriority aPriority )
    : iSocket( aSocket ),
      iTimeoutCurrentValue( KSessionTimeout ), iTimeoutSessionValue(KSessionTimeout)
    {
    iInitialPriority = aPriority;

    iSessionNeedsErrorReply = ETrue;
    iDisconnectNotification=ETrue;
    iContinuedMessageDownload = EFalse;
    iExportRetryCounter = 0;
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSession::TcpConstructL
// Two-phased constructor
// constructor that can leave. Used from derived classes.
// -----------------------------------------------------------------------------
//
void CUpnpTcpSession::TcpConstructL( RSocket aSocket,
                                     TInt aReadBufferSize,
                                     TInt aFileReadBufferSize )
    { 
    
    LOGS1( "%i, CUpnpTcpSession::TcpConstructL(RSocket)", iId );

    aSocket.RemoteName( iAddr );
    iAddr.ConvertToV4();
          
    // Create timeout timer and start it.
    iTimeoutTimer = CUpnpNotifyTimer::NewL( this );
    iSocketShutdown = CUpnpSocketShutdown::NewL( *this, iSocket, iInitialPriority );

    // Create socket read and write active objects
    iTcpReader = CUpnpTcpSessionReader::NewL( *this, iSocket,
                                     iInitialPriority, aReadBufferSize );    
    iTcpWriter = CUpnpTcpSessionWriter::NewL( *this, iSocket, iInitialPriority );

    iDisconnectNotification = ETrue;
    iIsPersistent = ETrue;
    iFileReadBufferSize = aFileReadBufferSize;

    SetState( EConnected );

    InitialReceiveL();
    
    StartTimeoutTimer( iSessionNeedsErrorReply );
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSession::FileTransferReaderConstructL
// Creates and activates the File Transfer reder object
// constructor that can leave. Used from derived classes.
// -----------------------------------------------------------------------------
//
void CUpnpTcpSession::FileTransferReaderConstructL()
    {
    LOGS1( "%i, CUpnpTcpSession::FileTransferConstructL", iId );
        
    // Create filetransfer reader, active object
   if(!iFTReader)
       {       
       if(this->FileAccess() && this->FileAccess()->EncodingMode())
           {
           iFTReader = CUpnpHttpChunkFileTransferReader::NewL( *this, iSocket,
                                      iInitialPriority, KFileTransferReaderBufSizeDefault, 
                   CUpnpHttpChunkFileTransferReader::CalculateBufferSize(
                            KFileTransferReaderBufSizeDefault,iFileReadBufferSize));
           }
       else
           {
           iFTReader = CUpnpHttpFileTransferReader::NewL( *this, iSocket,
                                 iInitialPriority, KFileTransferReaderBufSizeDefault, 
                       CUpnpHttpChunkFileTransferReader::CalculateBufferSize(
                            KFileTransferReaderBufSizeDefault,iFileReadBufferSize));      
           }
       }       
   else
       iFTReader->Reset();
   
   iFTReader->SetActivated(ETrue);
    
    }
    
// -----------------------------------------------------------------------------
// CUpnpTcpSession::FileTransferWriterConstructL
// Creates and activates the File Transfer writer object
// constructor that can leave. Used from derived classes.
// -----------------------------------------------------------------------------
//
void CUpnpTcpSession::FileTransferWriterConstructL(TInt aWriteSize)
    {
    LOGS1( "%i, CUpnpTcpSession::FileTransferWriterConstructL", iId );
        
    // Create filetransfer writer, active object
   if (!iFTWriter)
       {        
     iFTWriter = CUpnpHttpFileTransferWriter::NewL( *this, iSocket,
                                                  iInitialPriority, aWriteSize);  
       }       
   else
       iFTWriter->Reset();
   
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSession::~CUpnpTcpSession
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CUpnpTcpSession::~CUpnpTcpSession()
    {
    delete iTcpReader;
    delete iTcpWriter;   
    delete iFTReader;
    if(iFTWriter)
        {
        if(iState != ENotConnected)
            {
            iSocket.CancelWrite();
            }            
        delete iFTWriter;
        }    

    delete iTimeoutTimer;
    delete iReceivedMessage;
    delete iSocketShutdown;
    LOGS2( "%i, CUpnpTcpSession::~CUpnpTcpSession() - iState: %i",iId, iState );
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSession::ShutdownStop
// Disconnect connection
// -----------------------------------------------------------------------------
//
void CUpnpTcpSession::ShutdownStop()
    {
    LOGS2( "%i, CUpnpTcpSession::ShutdownStop, state %i", iId, iState );
    iState = EShuttingDownWriteWait;
    if( !iTcpWriter->IsWriting() && !iTcpWriter->IsRetrying() ) 
        {
        iSocketShutdown->ShutdownStop();
           SetState( EShuttingDown);
        }

    }

// -----------------------------------------------------------------------------
// CUpnpTcpSession::WriteL
// Write
// -----------------------------------------------------------------------------
//
void CUpnpTcpSession::WriteL( const TDesC8& aBuffer )
    {
    LOGS1( "%i, CUpnpTcpSession::WriteL(TDesC8&)", iId );

    iTcpWriter->IssueWriteL( aBuffer );

    StartTimeoutTimer( iContinuedMessageDownload );

    LOGS2( "%i, CUpnpTcpSession::WriteL - HTTP (Serverport %i) *** Writing...",
            iId, ServerPort() );
    }


// -----------------------------------------------------------------------------
// CUpnpTcpSession::InitialReceiveL
// Receive
// -----------------------------------------------------------------------------
//
void CUpnpTcpSession::InitialReceiveL()
    {
    LOGS1( "%i, CUpnpTcpSession::ReceiveL()", iId );
    if ( ( iState == EConnected ) && ( !iTcpReader->IsActive() ) )
        {
        iTcpReader->Start();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSession::ReceiveL
// Receive
// -----------------------------------------------------------------------------
//
void CUpnpTcpSession::ReceiveL()
    {      
    // If session is already close or it will be closed when reading is finisihed
    if ( IsConnected() && ( iSessionKeepAlive || DownloadOngoing() ) )
        {
        if (iFTReader && iFTReader->Activated())
            {
            iFTReader->StartL();
            }
        else
            {
            iTcpReader->Start();
            }
        StartTimeoutTimer( iSessionNeedsErrorReply );
        }             
    else if ( OutstandingWritings() ) //outstanding TCP writing operations
        {
        StartTimeoutTimer( iSessionNeedsErrorReply );
        }
    else
        {
        StartDisconnect();
        }
    }    

  
// -----------------------------------------------------------------------------
// CUpnpTcpSession::ReceivingCompletedL
// 
// -----------------------------------------------------------------------------
//
void CUpnpTcpSession::ReceivingCompletedL( TDesC8& aBuffer )
    {
    LOGS1( "%i, CUpnpTcpSession::ReceivingCompletedL(TDesC8& aBuffer)", iId);

    iTimeoutTimer->Cancel();
    BufferReceivedL( aBuffer );
    ReceiveL();            
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSession::WritingCompletedL
// 
// -----------------------------------------------------------------------------
//    
void CUpnpTcpSession::WritingCompletedL()
    {
    LOGS2( "%i, CUpnpTcpSession::WritingCompletedL() iState %i", iId, iState );

    iTimeoutTimer->Cancel();              
    
    switch( iState )
        {
        case EConnected:
            {
            TInetAddr local;
            iSocket.LocalName( local );
            local.SetFamily( KAfInet );

            TInetAddr remote;
            iSocket.RemoteName( remote );
            remote.SetFamily( KAfInet );    
            
            if( ( iSessionKeepAlive || remote == local ) && !iContinuedMessageDownload )
                {
                // 20s timeout without HTTP 408 message
                StartTimeoutTimer( EFalse );
                }
            else if( iContinuedMessageDownload )
                {
                // If there is expected content of the POST message after sending
                // there should be error response after timeout.
                // 20s timeout with HTTP 408 message
                StartTimeoutTimer( ETrue );
                }
            else
                {
                //if no POSTs left in a pipeline
                if (!iSessionKeepAlive)                   
                    {
                    iSessionNeedsErrorReply = EFalse;
                    StartDisconnect();
                    }
                }
            }           
                
            break;
        case EShuttingDownWriteWait:
                //after successful writing continue to shutdown
                ShutdownStop();
                break;
        case EDisconnecting:
            if ( iDisconnectNotification )
                {
                NotifyDisconnectL( RemoteHost() );
                DeleteThisSessionL( this );
                }
            else
                {
                iSessionNeedsErrorReply = EFalse;
                StartDisconnect();
                }
            break;
           case EServerBusy:
               DeleteThisSessionL( this );
              break;
        default:
            LOGS2( "%i, PANIC: CUpnpTcpSession::WritingCompletedL, iState: %i", iId, iState );
            break;

        }
    }
        
// -----------------------------------------------------------------------------
// CUpnpTcpSession::ServerBusyL
// Send server busy.
// -----------------------------------------------------------------------------
//
void CUpnpTcpSession::ServerBusyL()
    {
    LOGS1( "%i, CUpnpTcpSession::ServerBusyL()", iId );
    // no reason for reader to read data
    iTcpReader->Cancel();
    if(iFTReader)
        iFTReader->Cancel();
    
    CUpnpHttpMessage* msg = RUpnpHttpMessageFactory::HttpResponseErrorL( 
                        RemoteHost(), 
                        EHttpServiceUnavailable );
    CleanupStack::PushL( msg );
    HBufC8* msgbody = NULL;
    msgbody = msg->ToStringL();

    CleanupStack::PopAndDestroy(msg);
    CleanupStack::PushL( msgbody );

    WriteL( *msgbody );
    CleanupStack::PopAndDestroy(msgbody);

    iState = EServerBusy;
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSession::Id
// Returns session ID
// -----------------------------------------------------------------------------
//
TInt CUpnpTcpSession::Id()
    {
    return iId;
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSession::StartTimeoutTimerL
// Start timer.
// -----------------------------------------------------------------------------
//
void CUpnpTcpSession::StartTimeoutTimer( TBool aSessionErrorReplyNeeded )
    {
    LOGS2( "%i, CUpnpTcpSession::StartTimeoutTimer, state %i", iId, iState );

        if( !ShuttingDown())
            {
            iTimeoutTimer->Cancel();          
            iTimeoutTimer->After( iTimeoutCurrentValue, EFalse );
            iSessionNeedsErrorReply = aSessionErrorReplyNeeded;    
            LOGS2( "%i, CUpnpTcpSession::StartTimeoutTimer session:%i"
                             , iId, iTimeoutCurrentValue );
            }
            else 
                {
                // If already session is switched to disconnected state then just delete it.
                LOGS1( "%i, CUpnpTcpSession::StartTimeoutTimer session disconnected"
                                , iId );
                iSessionNeedsErrorReply = EFalse;
                StartDisconnect();
                }
    }
  
// -----------------------------------------------------------------------------
// CUpnpTcpSession::StartDisconnectingTimer
// Start timer.
// -----------------------------------------------------------------------------
//
void CUpnpTcpSession::StartDisconnectingTimer(TInt aTime)
    {
    LOGS2( "%i, CUpnpTcpSession::StartDisconnectingTimer, state %i",
            iId, iState );
    StartDisconnect();         
    iTimeoutTimer->After( aTime, EFalse );
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSession::StartDisconnect
// Start disconnect session.
// -----------------------------------------------------------------------------
//
void CUpnpTcpSession::StartDisconnect(TBool aDisconnectNotification)
    {
    LOGS2( "%i, CUpnpTcpSession::StartDisconnect, state %i",
            iId, iState ); 

    iDisconnectNotification = aDisconnectNotification;
    if( iState == EConnected )
        SetState( EDisconnecting );
    iTimeoutTimer->Cancel();
    TRAPD( err, DisconnectRequestL() );
    if( err < 0 ) 
        {
        ShutdownStop();
        }
    }
    
// -----------------------------------------------------------------------------
// CUpnpTcpSession::TimerEventL
// Timer event.
// -----------------------------------------------------------------------------
//
void CUpnpTcpSession::TimerEventL( CUpnpNotifyTimer* /*aTimer*/ )
    {
    if( iState == EConnected ) iSocketShutdown->MarksTimeout();
    this->DisconnectRequestL();
    }
    
// -----------------------------------------------------------------------------
// CUpnpTcpSession::DisconnectRequestL
// Handles discnnect request.
// -----------------------------------------------------------------------------
//
void CUpnpTcpSession::DisconnectRequestL()
    {
    LOGS2( "%i, CUpnpTcpSession::DisconnectRequestL -  HTTP (Serverport %i)"
            , iId, ServerPort() );
    if (DownloadOngoing() && (iState != EDisconnecting))
        {
        if(iExportRetryCounter < KMaxExportRetry)
            {
            //restart timer - download ongoing
            StartTimeoutTimer(ETrue);
            iExportRetryCounter++;                
            }
        else
            {
            iSocket.CancelWrite();
            SetState( ENotConnected );
                       NotifyDisconnectL( RemoteHost() );
                       iSessionNeedsErrorReply=EFalse;
                       DeleteThisSessionL( this );
            }
            return;
        }
    
    if ( iSessionNeedsErrorReply && !ShuttingDown() )
        {
        LOGS1( "%i, CUpnpTcpSession::DisconnectRequestL - reply 408!", iId );

        CUpnpHttpMessage* msg = RUpnpHttpMessageFactory::HttpResponseErrorL( 
                                                        RemoteHost(), 
                                                        EHttpRequestTimeout );
        CleanupStack::PushL( msg );
        HBufC8* tempBuf = NULL;
        tempBuf = msg->ToStringL();
        CleanupStack::PopAndDestroy( msg );
        CleanupStack::PushL( tempBuf );
        WriteL( *tempBuf );
        CleanupStack::PopAndDestroy( tempBuf );
        SetState( EDisconnecting );
        NotifyTimeoutL();
        }
    else
        {
        LOGS1( "%i, CUpnpTcpSession::DisconnectRequestL - no reply!", iId );
        DeleteThisSessionL( this );
        }
    
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSession::SetState
// Set state to connected.
// -----------------------------------------------------------------------------
//
void CUpnpTcpSession::SetState( TUpnpTcpSessionState aState )
    {
    LOGS1( "%i, CUpnpTcpSession::SetState()", iId );
    iState = aState;
    switch(iState)
        {
            case EConnected:
            
                iTcpWriter->SessionConnected( ETrue );                                    
                break;
            case EDisconnecting:
                //outstandings writings
                iTcpWriter->SessionConnected( ETrue );
                        
                break;
            default:
                iTcpWriter->SessionConnected( EFalse );
                if(iFTWriter)
                    iFTWriter->Cancel();
                break;        
                
        }
    
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSession::IsConnected
// Retrun connection state.
// -----------------------------------------------------------------------------
//
TBool CUpnpTcpSession::IsConnected()
    {
    return ( iState == EConnected );
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSession::RemoteHost
// Return remote host address.
// -----------------------------------------------------------------------------
//
TInetAddr& CUpnpTcpSession::RemoteHost()
    {
    return iAddr;
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSession::HandleErrorL
// 
// -----------------------------------------------------------------------------
//
void CUpnpTcpSession::HandleErrorL( TInt aError  )
    {
    LOGS2( "%i, CUpnpTcpSession::HandleErrorL( %i )", iId , aError);
    
    if ( aError == KErrCancel )
        {
        return;    
        }
         

    if(!ShuttingDown())
        {
        if( iState ==  EShuttingDownWriteWait )
            {
            ShutdownStop();
            return;
            }

    switch( aError )
        {
        case KErrNoMemory:
            NotifyTimeoutL();
            CheckConnectionCloseL();
            iSessionNeedsErrorReply = EFalse;
            SetState( EDisconnecting );
            NotifyDisconnectL( RemoteHost() );
            iDisconnectNotification = EFalse;
            StartDisconnectingTimer( KErrorTimeout );
            break;            

        case KErrEof:
            NotifyTimeoutL();
            CheckConnectionCloseL();
            iSessionNeedsErrorReply = EFalse;
            SetState( EDisconnecting );
            NotifyDisconnectL( RemoteHost() );
            DeleteThisSessionL( this );
            break;
        
        case KErrServerBusy:
            iDisconnectNotification = ETrue;
            iSessionNeedsErrorReply=EFalse;
            SocketServerBusyL( RemoteHost() );           
            StartDisconnect();    
            break;

        case KErrDisconnected:
            CheckConnectionCloseL();
            SetState( EDisconnecting );            
			iSessionNeedsErrorReply = EFalse;
			iDisconnectNotification = EFalse;
			StartDisconnect();
			break;            
        case KErrCouldNotConnect:      
        case KErrHostUnreach:    
        default:
            iSessionError = aError;            
            ShutdownStop();
            break;
            }
        }
    else
        {
        iSocketShutdown->HandleError( aError );
        }

    }
 
// -----------------------------------------------------------------------------
// CUpnpTcpSession::WaitWriteError
// 
// -----------------------------------------------------------------------------
//    
void CUpnpTcpSession::WaitRetryError(TInt aError, TBool aStartTimer)
    {
    LOGS2( "%i, CUpnpTcpSession::WaitRetryError - %i",
            iId, aError );
    iTimeoutTimer->Cancel();
    if (aStartTimer)
        StartTimeoutTimer( iContinuedMessageDownload );
    }
    
// -----------------------------------------------------------------------------
// CUpnpTcpSession::StartClosingSession
// 
// -----------------------------------------------------------------------------
//    
void CUpnpTcpSession::StartClosingSession()    
{    
    iSessionNeedsErrorReply=EFalse;
    StartDisconnect( ETrue );    
}

// -----------------------------------------------------------------------------
// CUpnpTcpSession::IsPersistentConnection
// 
// -----------------------------------------------------------------------------
//    
TBool CUpnpTcpSession::IsPersistentConnection()    
{    
    return iIsPersistent;
}

// -----------------------------------------------------------------------------
// CUpnpTcpSession::IsPersistentConnection
// 
// -----------------------------------------------------------------------------
//    
CUpnpHttpFileTransferReader* CUpnpTcpSession::FileTransferReader()
{    
    return iFTReader;
}

// -----------------------------------------------------------------------------
// CUpnpTcpSession::FileTransferWriter
// 
// -----------------------------------------------------------------------------
//    
CUpnpHttpFileTransferWriter* CUpnpTcpSession::FileTransferWriter()
{    
    return iFTWriter;
}

// -----------------------------------------------------------------------------
// CUpnpTcpSession::IsPersistentConnection
// 
// -----------------------------------------------------------------------------
//    
TBool CUpnpTcpSession::OutstandingWritings()    
{    
    //whether content being served or outstanding messages to be written to the socket
    return (FileServeExists() || iTcpWriter->HasWriteRequestsToSend());
}

// -----------------------------------------------------------------------------
// CUpnpTcpSession::DownloadOngoing
// 
// -----------------------------------------------------------------------------
//    
TBool CUpnpTcpSession::DownloadOngoing() 
{    
    //whether content being served or outstanding messages to be written to the socket
    return (FileTransferWriter() && FileTransferWriter()->IsActivated() &&
                                    !FileTransferWriter()->IsCancelled());
}

// -----------------------------------------------------------------------------
// CUpnpTcpSession::TimerCancel
// 
// -----------------------------------------------------------------------------
//    
void CUpnpTcpSession::TimerCancel()    
    {
    iTimeoutTimer->Cancel();    
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSession::ShuttingDown
// 
// -----------------------------------------------------------------------------
//    
TBool CUpnpTcpSession::ShuttingDown() 
    {    
    return (iState == ENotConnected || iState == EShuttingDown );
    }
    
// -----------------------------------------------------------------------------
// CUpnpTcpSession::CloseSocketL
// 
// -----------------------------------------------------------------------------
//    
void CUpnpTcpSession::CloseSocketL()
    {
    if( iDisconnectNotification )
        {
        NotifyDisconnectL( RemoteHost() );
        }
    else
        {
        NotifyTimeoutL();
        }
    iState = ENotConnected;
    iSocket.Close();
    DestroyIt( this );        
    }

// -----------------------------------------------------------------------------
// CUpnpTcpSession::TimeoutCurrentValue
// 
// -----------------------------------------------------------------------------
//    
TInt CUpnpTcpSession::TimeoutCurrentValue()
    {
    return iTimeoutCurrentValue;
    }
    
// -----------------------------------------------------------------------------
// CUpnpTcpSession::CancelTcpReader()
// 
// -----------------------------------------------------------------------------
//    
void CUpnpTcpSession::CancelTcpReader()
    {
    iTcpReader->Cancel();    
    }    

//  End of File
