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
* Description:  Handles download of a file
*
*/


#ifndef C_CHTTPDOWNLOADWORKER_H
#define C_CHTTPDOWNLOADWORKER_H


// User include files
#include "httptransferworker.h"
#include "httptransferobserver.h"
#include "httptransferbase.h"

// forward declarations
class MHttpWorkerObserver;

/**
 * This class downloads on single file.
 *
 * @lib httptransfer.lib
 * @since S60 v3.2
 */
NONSHARABLE_CLASS ( CHttpDownloadWorker ) : public CHttpTransferWorker, MHttpNotifyTimerObserver
    {
public:

    /**
     * Constructor
     *
     * @since S60 v3.2
     * @param aObserver Transfer observer
     * @param aIAPId IAP id
     * @param aBuffersize Size of the buffer
     * @param aCallback Call back to workerobserver
     */
    static CHttpDownloadWorker* NewL( MHttpTransferObserver& aObserver,
                                      TUint aIAPId,
                                      TUint aBufferSize,
                                      MHttpWorkerObserver& aCallback );

    /**
     * Constructor
     *
     * @since S60 v3.2
     * @param aObserver Transfer observer
     * @param aIAPId IAP id
     * @param aBufferSize Size of the buffer
     * @param aCallback Call back to workerobserver
     */
    static CHttpDownloadWorker* NewLC( MHttpTransferObserver& aObserver,
                                       TUint aIAPId,
                                       TUint aBufferSize,
                                       MHttpWorkerObserver& aCallback );

    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CHttpDownloadWorker();

// from base class CHttpTransferWorker
    
    /**
     * From base class CHttpTransferWorker
     * Stops the transfer of the file
     *
     * @since S60 v3.2
     */
    void CancelTransfer();
    
    /**
     * From base class CHttpTransferWorker
     * Starts the processing of a file.
     * Asynchronous.
     *
     * @since S60 v3.2
     */
    virtual void StartProcessL();

    /**
     * From base class CHttpTransferWorker
     * Called by framework to notify about transaction events.
     *
     * @since S60 v3.2
     * @param aTransaction Transaction, where the event occured.
     * @param aEvent Occured event.
     */
    void MHFRunL( RHTTPTransaction aTransaction,
                  const THTTPEvent& aEvent );

    /**
     * From base class CHttpTransferWorker
     * Called by framework when leave occurs in handling of 
     * transaction event.
     *
     * @since S60 v3.2
     * @param aError The leave code that occured.
     * @param aTransaction The transaction that was being processed 
     * @param aEvent The event that was being processed
     * @return Error code
     */
    TInt MHFRunError( TInt aError, 
                      RHTTPTransaction aTransaction,
                      const THTTPEvent& aEvent );

private:

    /**
     * Deletes partially downloaded file from file system.
     *
     * @since S60 v3.2
     */
    void DeleteFileL();

    /**
     * Checks disk space
     * @param aSize to be written to file
     * @since S60 v3.2
     */
    void CheckDiskSpaceL(TInt aSize);
    
    /**
     * Writes the last part of body into a file
     *
     * @since S60 v3.2
     */
    void WriteTailIntoFileL();

    /**
     * Handles the HTTP response data
     *
     * @since S60 v3.2
     * @param aTransaction HTTP transaction
     */
    void HandleResponseDataL( RHTTPTransaction aTransaction );

    /**
     * Sends HTTP GET request
     *
     * @since S60 v3.2
     */
    void HttpGetL();

    /**
     * Performs the actions needed when download is finished
     *
     * @since @S60 v3.2
     * @param aTransaction HTTP transaction
     */
    void FinishDownload( RHTTPTransaction aTransaction );
    
    /**
     * Default constructor
     *
     * @since S60 v3.2
     * @param aIAPId IAP id
     * @param aBufferSize Size of the buffer
     * @param aObserver Transfer observer
     * @param aCallback Call back for workerobserver
     */
    CHttpDownloadWorker( TUint aIAPId,
                         TUint aBufferSize,
                         MHttpTransferObserver& aObserver,
                         MHttpWorkerObserver& aCallback );

    /**
     * Second phase constructor
     *
     * @since S60 v3.2
     */
    void ConstructL();
    
    /**
     * Terminates the transfer and informs the client through
     * TransferComplete.
     *
     * @since S60 v3.2
     * @param aError Error code
     */
    void CompleteAndNotifyL( TInt aError );
    
    /**
    * Callback function for timer expirations.
    * @since Series60 5.0 HN
    * @param aTimer Timer that has expired.
    **/
    void TimerEventL( CHttpNotifyTimer* aTimer );
    
private: 

    /**
     * Size of the downloaded file
     */ 
    TInt iFileSizeFromHeader;
    
    /**
     * Amount of transferred bytes
     */     
    TInt iBytesTransferred;
    
    /**
     * Response body
     * Own.
     */
    HBufC8* iResponseBody;
    
    /**
    * Timeout timer for current session
    */
    CHttpNotifyTimer* iSessionTimer;
    
    };

#endif // C_CHTTPDOWNLOADWORKER_H
