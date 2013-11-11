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
* Description:  Class that is derived by CUpnpHttpSession. Implements TCP-level
*                functionalities
*
*/


#ifndef C_CUPNPTCPSESSION_H
#define C_CUPNPTCPSESSION_H

// INCLUDES
#include <e32base.h>
#include "upnphttpmessage.h"
#include "upnplist.h" 
#include "upnpnotifytimer.h"
#include "upnpsocketshutdown.h"

// FORWARD DECLARATIONS

class CUpnpHttpBuffer;
class CUpnpHttpServer;
class CUpnpSocketShutdown;
class RSocket;
class CUpnpTcpSessionWriter;
class CUpnpHttpFileTransferReader;
class CUpnpHttpFileTransferWriter;
class CUpnpTcpSessionReader;
class CUpnpHttpFileAccess;

// CONSTANTS
static const TInt KErrorTimeout = 3000000;
//when reading from a file
static const TInt KServeFileSizeDefault = 10000;
// Based on 7.2.9.4 (DLNA 1.5 rev 0.90)
// We need wait until 30second for response
static const TInt KSessionTimeout = 35000000; 

static const TInt KConnectTimeout = 7000000;

//default buffer size for reading from socket
static const TInt KRcvBufSizeDefault = 10000;
//default size for fiel content reading from a socket, must be a power of 2
static const TInt KFileTransferReaderBufSizeDefault = 16384;
//timeout value for awaiting 100 continue
static const TInt K100ContinueTimeout = 5000000;

// CLASS DECLARATION

/**
*  @brief CUpnpTcpSession is a class handling TCP functionality of the session.
*   Class is responsible for managing connection and its timeout as well as
*   for reading from and writing to socket. Those two last are directly 
*   handled by CUpnpTcpSessionReader and CUpnpTcpSessionWriter but CUpnpTcpSession 
*   supervises those processes.
*  @lib DLNAWebServer.lib
*  @since Series60 2.6
*/  
NONSHARABLE_CLASS (CUpnpTcpSession) : public CBase, public MUpnpNotifyTimerObserver
    {
       friend class CUpnpSocketShutdown;        
public: // Constructors and destructor

    /** 
    * Constructor for building CUpnpTcpSession. 
    * This constructor is used when connection is iniatiated from network.
    * Second-phase construction is made by TcpConstructL().
    * @param aSocket Pointer to new Socket.
    * @param aPriority Priority of this session
    * @return A new CUpnpTcpServer instance.
    **/
    CUpnpTcpSession( RSocket& aSocket, TThreadPriority aPriority );

    /** 
    * Virtual destructor.
    **/
    virtual ~CUpnpTcpSession();

   
public: // Session functionality
    
    void ShutdownStop();

    /**
    * Starts to receive messages to this session
    * @since Series60 3.2
    * @return None.
    **/
    void InitialReceiveL();
    
    /**
    * Receive messages to this session
    * @since Series60 3.2
    * @return None.
    **/
    void ReceiveL();

    /**
    * Writes data thru this session's socket
    * @since Series60 2.6
    * @param aBuffer Data to write.
    * @return None.
    **/
    void WriteL( const TDesC8& aBuffer );

    /**
    * Checks is session is connected.
    * @since Series60 2.6
    * @return ETrue, if session is connected; EFalse othewise.
    **/
    TBool IsConnected();

    /**
    * Returns the address that session is connected to.
    * @since Series60 2.6
    * @return Address of remote endpoint of session's socket.
    **/
    TInetAddr& RemoteHost();

    /**
    * Starts the timeoutTimer for session. If timer expires, no reply has 
    * been sent to remote host.
    * @since Series60 2.6
    * @param aSessionErrorReplyNeeded ETrue, if in timeout there should be
    * sent HTTP error 408 Server timeout.
    **/
    void StartTimeoutTimer( TBool aSessionErrorReplyNeeded );

    /**
    * Starts the timeoutTimer for session. Session is deleted after 0.5ms by default.
    * @since Series60 2.6
    **/
    void StartDisconnectingTimer(TInt aTime = 500);
    
    /**
    * Starts disconnect session.
    * @since Series60 3.2
    **/
    
    void StartDisconnect( TBool aDisconnectNotification = EFalse );
    
    /**
    * Function that is used to send notification to client and disconnect 
    * the session.
    * @since Series60 2.6
    **/
    void ServerBusyL();
    
    /**
    * Returns the SessionId of this session.
    * @since Series60 3.1
    * @return SessionId.
    **/
    TInt Id();

public: // Callback functions for reader and writer

    /**
    * Handles possible error that can occure in the session as well as
    * in reader and writer active objects.
    * @since Series60 3.0
    * @param aError error code
    **/
    void HandleErrorL( TInt aError  );
    
    /**
    * Callback function for reader object. When the reader gets some 
    * data, it forwards it to the session with this callback.
    * @since Series60 3.0
    * @param aBuffer buffer with received data.
    **/
    void ReceivingCompletedL( TDesC8& aBuffer );
    
    /**
    * Callback function for writer object. When the request writing is
    * completed then writer calls this function to notify session about it.
    * @since Series60 3.0
    **/
    void WritingCompletedL();
    
    /**
    * Callback function for writer object. When the request writing is
    * not completed due to an error, a new attempt to write is being made...
    * @since Series60 3.2
    **/
    void WaitRetryError(TInt aError, TBool aStartTimer = EFalse);    
     /** 
    * Second phase constructor for building CUpnpTcpSession. 
    * This constructor is used when connection is iniatiated from network.
    * @param aSocket Pointer to new Socket which is already connected 
    *  to remote host.
    * @return None
    **/
    void TcpConstructL( RSocket aSocket, TInt aReadBufferSize, TInt aFileReadBufferSize );

   
    /** 
    * Cancels TcpReader
    * @return None.
    **/
    void CancelTcpReader();
    

public: // virtual callback functions

    /**
    * Callback function. Called when data has been received.
    * @since Series60 2.6
    * @param aBuffer Data that has been received thru session.
    **/
    virtual void BufferReceivedL( TDesC8& aBuffer ) = 0;
    
    /**
    * Callback function. Deletes given session.
    * @since Series60 2.6
    * @param aSession Session to delete.
    **/
    virtual void DeleteThisSessionL( CUpnpTcpSession* aSession ) = 0;

    /**
    * Checks if FileServe Exists.
    * @since Series60 2.6
    * @return ETrue, if fileServe exists; EFalse otherwise.
    **/
    virtual TBool FileServeExists() = 0;

    /**
    * Callback function. Notifies upper layers that session has been disconnected.
    * @since Series60 2.6
    * @param aAddr Address of remote host.
    **/
    virtual void NotifyDisconnectL( TInetAddr aAddr ) = 0;
    virtual void NotifyTimeoutL() = 0;
    
    //file transfer reader callbacks
    
    /**
    * Callback function. Notifies http session when upload finished
    * @since Series60 3.2    
    **/
    virtual void FileTransferReaderDoneL() = 0;
    
    /**
    * Callback function. Notifies http session when error in upload
    * @since Series60 3.2    
    **/
    virtual void FileTransferReaderErrorL(TInt aError) = 0;
    
    /**
    * Callback function. Notifies http session when download finished
    * @since Series60 3.2    
    **/
    virtual void FileTransferWriterDoneL() = 0;
    
    /**
    * Callback function. Checks for outstanding downloads
    * @since Series60 3.2    
    **/
    virtual void FileTransferWriterHandleMoreL() = 0;
    
    /**
    * Callback function. Notifies upper layers that SocketServer is busy.
    * @since Series60 2.6
    * @param aAddr Address of remote host.
    **/
    virtual void SocketServerBusyL( const TInetAddr& aAddr ) = 0;

    /**
    * Callback function. Deletes fileserve of this session.
    * @since Series60 2.6
    **/
    virtual void DeleteServeFileL() = 0;
    
    /**
    * Returns a pointer to FileAccess object of this session.
    * @since Series60 3.2
    * @return Pointer to FileAccess object.
    **/
    virtual CUpnpHttpFileAccess* FileAccess()=0;

    /**
    * Callback function. Returns the port number of parent HttpServer.
    * @since Series60 2.6
    * @return Portnumber of http server.
    **/
    virtual TInt ServerPort() = 0;

    /**
    * Callback function. Checks if connection uses Connection: Close 
    * header, and if so, tries to parse message and send it to upper layers.
    * @since Series60 2.6
    **/
    virtual void CheckConnectionCloseL() = 0;

    /**
    * Callback function. Informs upper layers that HTTP POST has been started 
    * by sending HTTP error code 801 message to upper layers.
    * @since Series60 2.6
    **/
    virtual void NotifyUpperLayersOnPostL() = 0;

    /**
    * Callback function. Informs upper layers that HTTP POST has been finished 
    * @since Series60 2.6
    **/
    virtual void PostingFileCompleteL() = 0;
    
public: // From MNotifyTimeObserver

    /**
    * Callback function for timer expirations.
    * @since Series60 2.6
    * @param aTimer Timer that has expired.
    **/
    void TimerEventL( CUpnpNotifyTimer* aTimer );   
    
    /**
    * Cancel timer
    * @since Series60 3.2
    **/
    void TimerCancel();
 
    /**
    * Handles disconnect request
    * @since Series60 3.2
    **/
    void DisconnectRequestL();      
    
    /**
    * Starts closing the session, no more message to be sent to the client
    * @since Series60 3.2
    **/
    void StartClosingSession();
    
     /**
    * Marked if a persistent connection
    * @since Series60 3.2
    **/
    TBool IsPersistentConnection();
    
     /**
    * Returns pointer to CUpnpHttpFileTransferReader
    * @since Series60 3.2
    * @return CUpnpHttpFileTransferReader*
    **/
    CUpnpHttpFileTransferReader* FileTransferReader();
    
     /**
    * Returns pointer to CUpnpHttpFileTransferWriter
    * @since Series60 3.2
    * @return CUpnpHttpFileTransferWriter*
    **/
    CUpnpHttpFileTransferWriter* FileTransferWriter();
            
    /**
    * Checks if there are outstanding sending operations.
    * @since Series60 3.2
    * @return TBool
    **/
    TBool DownloadOngoing();
    
    /**
    * Checks if the session is being shutdown
    * @since Series60 3.2
    * @return TBool
    **/
    TBool ShuttingDown() ;
      /**
    * Returns current value of the timeout
    * @since Series60 3.2
    * @return TInt
    */
    
     /**
    * Closes the socket
    * @since Series60 3.2
    */
    void CloseSocketL();

    TInt TimeoutCurrentValue();  
private: // private enumerations

    /**
    * State of session.
    *   - EnotConnected - Connection is not established yet.
    *   - EConnecting - TcpSession requested connection in asynchronous mode.
    *   - EConnected  - Connection is established, TcpReader is started and TcpWriter 
    *                 can write to the socket.
    *   - EDisconnecting - Session is about to be disconnected but socket is not closed
    *                 so TcpWriter can still write a response.
    *   - EServerBusy - Server is already occupied with processing requests so no other
    *                 session can be established.
    **/
    enum TUpnpTcpSessionState 
      {
      ENotConnected,
      EConnecting,
      EConnected,
      EDisconnecting,
      EShuttingDownWriteWait,
      EShuttingDownStop,
      EShuttingDown,
      EServerBusy
      };
      
private: // New functions
  
    /**
    * Sets session statel.
    * @since Series60 2.6
    * @param aState new state of the session.
    **/
    void SetState( TUpnpTcpSessionState aState );
    
    /**
    * Checks if there are outstanding writing operations.
    * @since Series60 3.2
    * @return TBool
    **/
    TBool OutstandingWritings();

protected:
    
     /**
    * Contructs file transfer reader
    * @since Series60 3.2
    **/
    void FileTransferReaderConstructL();
    
    virtual void DestroyIt(CUpnpTcpSession* aSession ) = 0;


    /**
    * Contructs file transfer writer
    * @since Series60 3.2
    **/
    void FileTransferWriterConstructL(TInt aWriteSize);    
    
protected:  
    
    TInt iFileReadBufferSize;
    // Session's socket, owned
    RSocket iSocket;

    // Address of remote host, owned
    TInetAddr iAddr;

    // iTcpReader, active object to control reads from the socket.
    // Owned by CUpnpTcpSession object.
    CUpnpTcpSessionReader* iTcpReader;
    
    // iFileTransferReader, active object to control reads from the socket.
    // Owned by CUpnpTcpSession object.
    CUpnpHttpFileTransferReader* iFTReader;
    
    // iFileTransferWriter, active object to control writes from the socket.
    // Owned by CUpnpTcpSession object.
    CUpnpHttpFileTransferWriter* iFTWriter;

    // iTcpWriter, active object to control writes to the socket.
    // Owned by CUpnpTcpSession object.
    CUpnpTcpSessionWriter* iTcpWriter;
    
    // Timer that is used to disconnect session when timeout expires, owned
    CUpnpNotifyTimer* iTimeoutTimer;
    
    // Buffer that is used to receive messages. See CUpnpHttpBuffer.
    CUpnpHttpBuffer* iReceivedMessage;

    // Session id of this session, owned
    TInt iId;

    // State of session, owned
    TUpnpTcpSessionState iState;

    // ETrue, if session is meant to be kept alive; EFalse otherwise, owned
    TBool iSessionKeepAlive;
    
    // ETrue, if session needs 408 HTTP error on timeout, owned
    TBool iSessionNeedsErrorReply;
    
    // Used with session disconnects. If ETrue, session send notification 
    // of disconnect to upper layers, owned
    TBool iDisconnectNotification;
    
    //Flag set true if any pending requests are left in a pipeline awating to be proccessed
    TBool iPendingPostRequests;

    // value that is used to check if downloaded content should be saved 
    // into a file.
    TBool iSaveToFile;

    // The current value of timeout, changes dependent on a operation-connecting, sending,...
    TInt iTimeoutCurrentValue;
    
    // Timeout that defines the session length
    TInt iTimeoutSessionValue;
    
    // Flag indicating if POST message with header "Expect: 100-continue"
    // was received and server should wait for download of body after
    // sending permission to continue download.
    TBool iContinuedMessageDownload;
    
    // Initial priority, priority set for message that initialize this session
    TThreadPriority iInitialPriority;
    
    //flag for indicating a persistant connection
    TBool iIsPersistent;
    
    // Implementation of the socket shutdown algorithm
    CUpnpSocketShutdown *iSocketShutdown;
    
    // Retry counter during export action
    TInt iExportRetryCounter;
    
    TInt iSessionError;
    };

#endif // C_CUPNPTCPSESSION_H

// End Of File
