/** @file
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Base class for download / upload worker classes
*
*/


#ifndef C_CHTTPTRANSFERWORKER_H
#define C_CHTTPTRANSFERWORKER_H


// System include files
#include <http/mhttptransactioncallback.h>
#include <e32def.h>
#include <e32cmn.h>
#include <http.h>
#include <e32base.h>
#include <es_sock.h>
#include <stringpool.h>

// User include files
#include "httptransferobserver.h"
#include "httpfile.h"
#include "httptransferbase.h"
#include "httptransfertimerobserver.h"
#include "upnpconnectionmanagerproxy.h"
#include "httpnotifytimer.h"

// Forward declarations
class MHttpTransactionObserver;
class CHttpTransferTimer;

const TInt KSessionTimeout = 35; //35 sec

/**
 * Virtual base class for CHttpDowloadWorker and CHttpUploadWorker.
 *
 * @lib httptransfer.lib
 * @since S60 v3.2
 */
class CHttpTransferWorker : public CBase,
                            public MHTTPTransactionCallback,
                            public MHttpTransferTimerObserver
    {
public:
    /**
     * State machine
     *
     * @since S60 v3.2
     */
    enum THttpWorkerState
        {
        // Waiting for start of the process
        EHttpWaitingForStart,
        
        // HTTP GET sent
        EHttpGetSent,
        
        // HTTP POST sent
        EHttpPostSent,

        // HTTP GET response received
        EHttpGetResponseReceived,

        // HTTP transaction finished
        EHttpTransactionIdle,
        
        // HTTP transaction canceled
        EHttpTransactionCanceled
        };

public:

    /**
     * Virtual destructor
     *
     * @since S60 v3.2
     */
    virtual ~CHttpTransferWorker();

    /**
     * Connects the HTTP session
     *
     * @since S60 v3.2
     */
    void ConnectL();
    
    /**
     * Sets file which should be transfered.
     *
     * @since S60 v3.2
     * @param aFile New file
     */
    void SetFileL( CHttpFile& aFile );
    
    /**
     * Stops the transfer of the file
     *
     * @since S60 v3.2
     */
    virtual void CancelTransfer()=0;
    
    /**
     * Starts the processing of a file.
     * Asynchronous.
     *
     * @since S60 v3.2
     */
    virtual void StartProcessL();
    
    /**
     * Sets tracking flag of the processed file to the value 
     * of the aValue attribute.
     *
     * @since S60 v3.2
     * @param aValue New value
     */
    void TrackProgress( TBool aValue );
    
    /**
     * Getter for key value of the processed file.
     *
     * @since S60 v3.2
     * @return Value of the aKey attribute of the processed file.
     */
    TAny* Key() const;
    
    /**
     * Getter for Uri
     *
     * @since S60 v3.2
     * @return Value of the Uri
     */
    const HBufC8* Uri() const;
    
    /**
     * Getter for Path
     *
     * @since S60 v3.2
     * @return Value of the path
     */
    const HBufC* Path() const;
    
    /**
     * Returns information if the process is on-going
     *
     * @since S60 v3.2
     * @return True if process is on-going
     */
    TBool ProcessOnGoing() const;
    
    /**
     * Returns information if the worker is waiting for start
     *
     * @since S60 v3.2
     * @return True if is waiting
     */
    TBool IsWaiting() const;
    
    /**
     * Sets headers
     *
     * @since S60 v3.2
     * @param aHeaders Header
     * @param aHdrField The field of the header
     * @param aHdrValue Value of the header
     */
    void SetHeaderL( RHTTPHeaders aHeaders, TInt aHdrField, 
                     const TDesC8& aHdrValue );

    /**
     * Sets headers
     *
     * @since S60 v3.2
     * @param aHeaders Header
     * @param aHdrField The field of the header
     * @param aHdrValue Value of the header
     */
    void SetHeaderL( RHTTPHeaders aHeaders,
                     const TDesC8& aHdrField,
                     const TDesC8& aHdrValue );
    
    /**
     * Returns a reference to the processed file. Returns NULL if 
     * the processed file is not set.
     *
     * @since S60 v3.2
     * @return A pointer to the processed file
     */
    CHttpFile* ProcessedFile();

// from base class MHTTPTransactionCallback

    /**
     * Called by framework to notify about transaction events.
     *
     * @since S60 v3.2
     * @param aTransaction Transaction, where the event occured.
     * @param aEvent Occured event.
     */
    virtual void MHFRunL( RHTTPTransaction aTransaction,
                          const THTTPEvent& aEvent )=0;

    /**
     * Called by framework when leave occurs in handling of transaction 
     * event.
     *
     * @since S60 v3.2
     * @param aError The leave code that occured.
     * @param aTransaction The transaction that was being processed 
     * @param aEvent The event that was being processed
     * @return Error code
     */
    virtual TInt MHFRunError( TInt aError,
                              RHTTPTransaction aTransaction,
                              const THTTPEvent& aEvent )=0;

// from base class MHttpTransferTimerObserver    
    
    /**
     * Call back for the timer
     *
     * @since S60 v3.2
     */
    void TimerCallback();
    
protected:
    /**
     * Second phase constructor
     *
     * @since S60 v3.2
     */
    void ConstructL();

protected: 

    /**
     * Pointer to the transfer observer
     * Not own.
     */
    MHttpTransferObserver* iObserver;

    /**
     * HTTP session
     */
    RHTTPSession iSession;
    
    /**
     * String pool
     */
    RStringPool iStringPool;
    
    /**
     * Connection
     */
    CUpnpConnectionManagerProxy *iConnectionManagerProxy;
    
    /**
     * Filename
     * Own.
     */
    HBufC* iFileName;
    
    /**
     * Handle to the file server session
     */
    RFs iFsSession;
    
    /**
     * File
     */
    RFile iFile;

    /**
     * File which is being processed.
     * Own.
     */
    CHttpFile* iProcessedFile;
    
    
    /**
     * State machine
     */
    THttpWorkerState iProcessState;

    /**
     * Buffer size
     */ 
    TInt iBufferSize;            
    
    /**
     * Socket server
     */
    RSocketServ iSocketServer;
    
    /**
     * A HTTP Transaction. This encapsulates 1 HTTP request and
     * response. A Transaction is associated with a session, and must be
     * created using the session's CreateTransactionL method.
     */
    RHTTPTransaction iHttpTransaction;
    
    /**
     * Worker observer
     * Not own.
     */
    MHttpWorkerObserver* iCallback;

    /**
     * Timer.
     * Own.
     */
    CHttpTransferTimer* iTimer;

    };

#endif // C_CHTTPTRANSFERWORKER_H
