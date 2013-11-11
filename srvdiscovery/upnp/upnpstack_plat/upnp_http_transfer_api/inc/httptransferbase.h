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
* Description:  Base class for downloader / uploader classes
*
*/


#ifndef C_CHTTPTRANSFERBASE_H
#define C_CHTTPTRANSFERBASE_H


// System include files
#include <e32base.h>
#include <f32file.h>

// user include files
#include "httpworkerobserver.h"

// Forward declarations
class CHttpTransferWorker;
class MHttpTransferObserver;
class CHttpFile;

// Constants
const TUint KDefaultParallelTransfers = 1;

/**
 * Base class of the downloader and uploader classes.
 * Handles filequeue.
 *
 * @lib httptransfer.lib
 * @since S60 v3.2
 * @deprecated
 */
class CHttpTransferBase : public CActive,
                          public MHttpWorkerObserver
    {
public:

    /**
     * This enum is used in SetProperty to recognize the action
     *
     * @since S60 v3.2
     * @deprecated
     */
    enum THttpPropertyType 
        {
        ETargetURI,
        ETargetPath
        };

public:
    /**
     * Virtual destructor
     *
     * @since S60 v3.2
     * @deprecated
     */   
    virtual ~CHttpTransferBase();

    /**
     * Deletes all the instances of CHttpFiles in iFileQueue. Also
     * cancels all the on-going transfers.
     *
     * @since S60 v3.2
     * @deprecated
     */
    IMPORT_C void CancelAll();

    /**
     * Deletes the corresponding instance of CHttpFile in iFileQueue
     * or stops the corresponding transfer.
     *
     * @since S60 v3.2
     * @param aKey Key corresponding the process
     * @deprecated
     */
    IMPORT_C TInt CancelTransfer( TAny* aKey );

    /**
     * Switch the tracking of the duration on/off.
     *
     * @since S60 v3.2
     * @param aKey Key corresponding the process
     * @param aValue Value of the flag
     * @deprecated
     */
    IMPORT_C TInt TrackProgress( TAny* aKey, TBool aValue );

    /**
     * Sets a header
     *
     * @since S60 v3.2
     * @param aKey the file identifier
     * @param aFieldName The field name, e.g. 'Accept'
     * @param aFieldValue The field value. e.g. 'text/plain'
     * @deprecated
     */
    IMPORT_C void SetHeaderL( TAny* aKey,
                              const TDesC8& aFieldName, 
                              const TDesC8& aFieldValue );

    /**
     * Starts the transfer of the file.
     * Leaves if aKey is invalid
     *
     * @since S60 v3.2
     * @param aKey Identifier of the downloaded file
     * @deprecated
     */
    IMPORT_C void StartTransferL( TAny* aKey );

    /**
     * Sets property
     *
     * @since S60 v3.2
     * @param aKey Key of the file property concerns
     * @param aProperty Id of the property to be changed
     * @param aValue Value of the property
     * @deprecated
     */
    IMPORT_C void SetPropertyL( TAny* aKey, 
                                THttpPropertyType aProperty, 
                                const TDesC& aValue );

    /**
     * Gets a property
     *
     * @since S60 v3.2
     * @param aKey Key of the file property concerns
     * @param aProperty Id of the property
     * @return Pointer to the property value
     * @deprecated
     */
    IMPORT_C const HBufC* GetPropertyL( TAny* aKey, 
                                        THttpPropertyType aProperty );

    /**
     * Adds file into wait queue
     *
     * @since S60 v3.2
     * @param aKey Key of the file
     * @param aPath Path of the file
     * @param aUri Uri of the file
     * @deprecated
     */
    IMPORT_C void InsertFileIntoWaitQueueL( TAny* aKey, 
                                            const TDesC& aTargetPath,
                                            const TDesC8& aUri );

    /**
     * Adds file into wait queue
     *
     * @since S60 v3.2
     * @param aKey Key of the file
     * @param aFileHandle Handle to the file
     * @param aUri Uri of the file
     * @deprecated
     */
    IMPORT_C void InsertFileIntoWaitQueueL( TAny* aKey, 
                                            const RFile& aFileHandle,
                                            const TDesC8& aUri );

    /**
     * Moves the file to file queue from wait queue. Starts also the
     * transfer.
     *
     * @since S60 v3.2
     * @param aKey Key of the file
     * @deprecated
     */
    IMPORT_C void MoveToTransferQueueL( TAny* aKey );

    /**
     * Activates new request
     *
     * @since S60 v3.2
     * @deprecated
     */
    void ActivateNewTransferRequest();

    /**
     * Processes the next file in the queue
     *
     * @since S60 v3.2
     * @deprecated
     */
    void ProcessNextFileL();

// from base class CActive

    /**
     * Cancels outstanding request.
     *
     * @since S60 v3.2
     * @deprecated
     */
    void DoCancel();

    /**
     * Handles request completion event.
     *
     * @since S60 v3.2
     * @deprecated
     */
    void RunL();

    /**
     * Handles leave which occurs in RunL
     *
     * @since S60 v3.2
     * @param aError Error code
     * @return Error code or KErrNone if handle successful
     * @deprecated
     */
    TInt RunError( TInt aError );

// from base class MHttpWorkerObserver

    /**
     * Call back for workers to inform when they are ready
     *
     * @since S60 v3.2
     * @deprecated
     */
    void WorkerCompleted();

protected:

    /**
     * Creates new CHttpFile object and adds it to the iFileQueue
     *
     * @since S60 v3.2
     * @param aUri Uri of the downloaded object
     * @param aTargetPath Local path of the object
     * @param aKey Identifier of the object
     * @deprecated
     */
    void CreateAndQueueHttpFileL( const TDesC8& aUri, 
                                  const TDesC& aTargetPath, 
                                  TAny* aKey );

    /**
     * Creates new CHttpFile object and adds it to the iFileQueue
     *
     * @since S60 v3.2
     * @param aUri Uri of the downloaded object
     * @param aFileHandle Handle to the file on disk
     * @param aKey Identifier of the object
     * @deprecated
     */
    void CreateAndQueueHttpFileL( const TDesC8& aUri, 
                                  const RFile& aFileHandle,
                                  TAny* aKey );

    /**
     * Returns and removes the last item from iFileQueue and sets it to
     * iProcessedFile.
     *
     * @since S60 v3.2
     * @return Pointer to next file in the queue. Null if empty queue.
     * @deprecated
     */
    CHttpFile* NextFile();

    /**
     * Default constructor
     *
     * @since S60 v3.2
     * @deprecated
     */
    CHttpTransferBase();
    
protected: 

    /**
     * Pointerarray which contains the files
     */
    RPointerArray<CHttpFile> iFileQueue;

    /**
     * Queue for the waiting files
     */
    RPointerArray<CHttpFile> iWaitQueue;

    /**
     * Array of worker classes.
     */
    RPointerArray<CHttpTransferWorker> iWorkerArray;

    /**
     * Pointer to the transfer observer
     * Not own.
     */
    MHttpTransferObserver* iObserver;

    /**
     * IAP id
     */
    TUint iIAPId;

    /**
     * buffer size, configurable
     */
    TUint iBufferSize;

    /**
     * Number of parallel transfers
     */
    TUint iParallelTransfers;

    /**
     * Worker which is being processed
     */
    CHttpTransferWorker* iActiveWorker;

    };

#endif // C_CHTTPTRANSFERBASE_H