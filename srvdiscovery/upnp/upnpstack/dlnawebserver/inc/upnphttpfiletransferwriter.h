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



#ifndef C_UPNPHTTPFILETRANSFERWRITER_H
#define C_UPNPHTTPFILETRANSFERWRITER_H


#include <e32base.h>
#include <in_sock.h>
#include "upnpretrywrite.h"

class CUpnpTcpSession;
class CUpnpHttpFileAccess;
    
/**
 *  CUpnpHttpFileTransferWriter is responsible for asynchronous writing.
 *  Writer class is owned by CUpnpTcpSession which request asynchronous 
 *  writing from it and is notified by writer when the write request 
 *  is completed or an error occured.
 *  When writer receives data to send it checks if it can be sent to the socket
 *  right away and if session is not yet connected or writer already sends data
 *  then it queues incoming data. When writing to the socket ends, writer checks 
 *  if there is data in queue waiting for sending. If thereis then it issues writing it 
 *  until the queue is empty. After writing all data, the writer notifies session about
 *  completion of the writing taska and switches to waiting state until it receives 
 *  another write request.
 *  @lib DLNAWebServer.lib
 *  @since Series60 3.2
 */
NONSHARABLE_CLASS (CUpnpHttpFileTransferWriter) : public CActive, MUpnpRetryWriteObserver
    {
public:

    /**
    * CUpnpHttpFileTransferWriter factory method.
    * @since Series60 3.2
    * @param aSession session that runs and supervises reader.
    * @param aSocket socket from which data will be read.
    * @param aPriority priority with which the reader will be working.
    */
    static CUpnpHttpFileTransferWriter* NewL( CUpnpTcpSession& aSession,
                                        RSocket& aSocket,
                                        TThreadPriority aPriority,
                                        TInt aWriteSize);

    /**
    * Virtual destructor.
    * @since Series60 3.2
    */   
    virtual ~CUpnpHttpFileTransferWriter();

public: // New functions
    
       
    /**
    * Passes control to the reader
    * @since series60 3.2    
    */  
    void StartL();
    
    /**
    * Resets state of the writer
    * @since series60 3.2   
    */
    void Reset();
    
    /**
    * Indicates it writer is waiting for connection
    * @since series60 3.2   
    */
    TBool IsWaiting();
    
    /**
    * Indicates it writer is in use
    * @since series60 3.2   
    */
    TBool IsActivated();
    
    /**
    * Indicates it iRetryWrite object is waiting to rewrite
    * @since series60 3.2   
    */
    TBool IsRetrying();
    
    /**
    * Indicates it file download is cancelled
    * @since series60 3.2   
    */
    TBool IsCancelled();
    
    /**
    * Sets that post notification is need
    * @since series60 3.2   
    */
    void SetPostNotify();
    
private: // from MUpnpRetryWriteObserver
	
    /**
    * Called when retrying of writing was succesed.
    * @since Series60 3.2
    */
	void RetryWriteSucceed();

    /**
    * Called when retrying of writing fail.
    * @since Series60 3.2
    */
	void RetryWriteFailL( TInt aError );


protected: // Functions from base CActive
    
    /**
    * Cancels issued writing request.
    * Standard active object function
    * @since Series60 3.2
    */
    void DoCancel();
    
    /**
    * Function called when the write request is finished.
    * Standard active object function
    * @since Series60 3.2
    */
    void RunL();
    
    /**
    * Trap RunL leaves
    * Standard active object function
    * @since Series60 3.2
    */
    TInt RunError( TInt aError );
    
private: // Constructors and destructors
    
    /**
    * CUpnpHttpFileTransferWriter constructor.
    * @since Series60 3.2
    * @param aSession session that runs and supervises reader.
    * @param aSocket socket from which data will be read.
    * @param aPriority priority with which the reader will be working.
    * @param aWriteSize
    */
    CUpnpHttpFileTransferWriter( CUpnpTcpSession& aSession,
                           RSocket& aSocket,
                           TThreadPriority aPriority,
                           TInt aWriteSize);

    /**
    * By default Symbian 2nd phase constructor is private.    
    * @since Series60 3.2
    */
    void ConstructL();

    /**
    * Sends next queued write request if such exists.
    * @since Series60 3.2
    */
    void SendNextPortionL();
    
    /**
    * Sends HTTP headers
    * @since Series60 3.2
    */
    void SendHeadersL();
    
    
    /**
    * Writes to the socket
    * @since Series60 3.2
    */
    void WriteToSocket();
    
    /**
    * Finishes downloading
    * @since Series60 3.2
    */
    void FinishL();
    
    /**
    * Handles RunL error
    * @since Series60 3.2
    */
    void HandleErrorL();

private: // Enumerations

    /**
    * TWriteState, records whether a write request is pending.
    *   - EUnknown Initial state
    *   - EHeaders Sending headers
    *   - EContent Sending content    
    *   - ECancelled Writing to the socket is cancelled
    */

   enum TFileTransferWriterState
    {
    EUnknown,
    EHeaders,
    EContent,
    EFinished,
    ECancelled
    };        

private: // Data
    
    /**
    * Socket used for writing data, not owned.
    */
    RSocket& iSocket;

    /**
    * Reference to session that owns the writer.
    * Session is request writing and is notified when it's finished or
    * errors occure, not owned.
    */
    CUpnpTcpSession& iSession;
    
    /**
    * State of the writer.
    */
    TFileTransferWriterState iState;

    /**
    * Buffer that is used to send data.    
    */
    RBuf8 iSendBuffer;
    
    /**
    * Pointer to data to be sent    
    */
    TPtr8 iSendPointer;
        
    
    /**
    * Size of the write buffer
    */
    TInt iWriteSize;
    
    /**
    * Indicates when writed needs to wait for connecting to the remote host
    */
    TBool iWaiting;
    
    /**
    * Indicates when POST is being sent
    */
    TBool iHttpPostStarted;    

    /**
    * Implementation of retrying write to the socket
    */
    CUpnpRetryWrite* iRetryWrite;

    };

#endif // C_UPNPHTTPFILETRANSFERWRITER_H

// End Of File
