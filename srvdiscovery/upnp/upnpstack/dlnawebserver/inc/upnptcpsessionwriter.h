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



#ifndef C_UPNPTCPSESSIONWRITER_H
#define C_UPNPTCPSESSIONWRITER_H


#include <e32base.h>
#include <in_sock.h>
#include "upnpretrywrite.h"

class CUpnpTcpSession;
class CUpnpTcpWriteRequest;

    
/**
 *  CUpnpTcpSessionWriter is responsible for asynchronous writing.
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
 *  @since Series60 2.6
 */
NONSHARABLE_CLASS (CUpnpTcpSessionWriter) : public CActive, MUpnpRetryWriteObserver
    {
public:

    /**
    * CUpnpTcpSessionWriter factory method.
    * @since Series60 2.6
    * @param aSession session that runs and supervises reader.
    * @param aSocket socket from which data will be read.
    * @param aPriority priority with which the reader will be working.
    * @return ?description
    */
    static CUpnpTcpSessionWriter* NewL( CUpnpTcpSession& aSession,
                                        RSocket& aSocket, 
                                        TThreadPriority aPriority );

    /**
    * Virtual destructor.
    * @since Series60 2.6
    */   
    virtual ~CUpnpTcpSessionWriter();

public: // New functions
    
    /**
    * Issues writing in asynchronous mode.
    * @since Series60 2.6
    * @param aData buffer with data to write.
    */
    void IssueWriteL( const TDesC8& aData );
    
    
    /**
    * Function used by CUpnpTcpSession that owns the writer
    * to notify if socket is connected and if writer can write data.
    * @since Series60 2.6
    * @param aConnected
    */
    void SessionConnected( TBool aConnected );
    
    /**
    * Returnes true if there are some write request queued and
    *   false if all data has been sent already.
    * @since Series60 2.6
    * @return true if there are some write request queued and
    *   false if all data has been sent already.
    */
    TBool HasWriteRequestsToSend();
    
    /**
    * Returns if retrying of writng occurs
    * @since series60 3.2
    * @return ETrue/EFalse
    */  
    TBool IsRetrying();

    /**
    * Returns if writer is writing some data at the moment.
    * @since series60 2.6
    * @return true if writer writes data at the moment.
    */  
    TBool IsWriting();

protected: // Functions from base CActive
    
    /**
    * Cancels issued writing request.
    * Standard active object function
    * @since Series60 2.6
    */
    void DoCancel();
    
    /**
    * Function called when the write request is finished.
    * Standard active object function
    * @since Series60 2.6
    */
    void RunL();
    
    /**
    * Trap RunL leaves
    * Standard active object function
    * @since Series60 2.0
    */
    TInt RunError( TInt aError );
    
private: // Constructors and destructors
    
    /**
    * CUpnpTcpSessionWriter constructor.
    * @since Series60 2.6
    * @param aSession session that runs and supervises reader.
    * @param aSocket socket from which data will be read.
    * @param aPriority priority with which the reader will be working.
    */
    CUpnpTcpSessionWriter( CUpnpTcpSession& aSession,
                           RSocket& aSocket,
                           TThreadPriority aPriority );

    /**
    * By default Symbian 2nd phase constructor is private.
    * @since Series60 2.6
    */
    void ConstructL();


private: // New functions

    /**
    * Sends next queued write request if such exists.
    * @since Series60 2.6
    */
    void SendNextWriteRequestL();
    
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

private: // Enumerations

    /**
    * TWriteState, records whether a write request is pending.
    *   - EnotConnected Socket is not yet connected so writer cannot write.
    *   - EWriting  A write request is pending with the socket server.
    *   - EWaiting  The idle state for this object. Socket is connected
    *               and the writer waits for write requests form session.
    *   - EDisconnected Session is already disconnected so don't write and don't 
    *               queue requests.
    */
    enum TWriterState
        {
        ENotConnected,
        EWriting,
        EWaiting,
        EDisconnected
        };

private: 
    
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
    TWriterState iState;

    /**
    * Buffer that is used to send data to session.
    * Own.
    */
    RBuf8 iSendBuffer;

    /**
    * List of write requests. If session is unable to send data to session,
    * it is put into write request list.
    * Own.
    */
    RPointerArray<CUpnpTcpWriteRequest> iWriteRequestList;
    
    /**
    * Initial priority of the active object.
    */
    TThreadPriority iInitialPriority;
    
    /**
    * Implementation of retrying write to the socket
    */
    CUpnpRetryWrite* iRetryWrite;

    };

#endif // C_UPNPTCPSESSIONWRITER_H

// End Of File
