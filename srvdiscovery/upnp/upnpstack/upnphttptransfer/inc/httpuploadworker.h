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
* Description:  Handles upload of single file
*
*/


#ifndef C_HTTPUPLOADWORKER_H
#define C_HTTPUPLOADWORKER_H

// system includes
#include <escapeutils.h> 
#include <http/mhttpdatasupplier.h>

// user includes
#include "httptransferworker.h"

// forward declarations
class MHttpWorkerObserver;

/**
 * Handles upload of single file. Used by CHttpUploader.
 *
 * @lib httptransfer.lib
 * @since S60 v3.2
 */
NONSHARABLE_CLASS ( CHttpUploadWorker ) : public CHttpTransferWorker,
                                          public MHTTPDataSupplier,
                                          MHttpNotifyTimerObserver
    {
public:
     
    /**
     * Constructor
     *
     * @since S60 v3.2
     * @param aObserver Transfer observer
     * @param aIAPId IAP id
     * @param aBufferSize Size of the buffer
     * @param aCallback Call back to workerobserver
     */
    static CHttpUploadWorker* NewL( MHttpTransferObserver& aObserver,
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
    static CHttpUploadWorker* NewLC( MHttpTransferObserver& aObserver,
                                     TUint aIAPId,
                                     TUint aBufferSize,
                                     MHttpWorkerObserver& aCallback );

    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CHttpUploadWorker();

// New functions

    /**
     * Sends a file to some URI through HTTP channel.
     * The POST HTTP method is used.
     *
     * @since S60 v.3.2
     */
    void SendL();
     
    /**
     * Invokes the http method.
     * This actually creates the transaction, sets the headers and body 
     * and then starts the transaction.
     *
     * @since S60 v3.2
     */
    void InvokeHttpMethodL();

// from base class CHttpTransferWorker

    /**
     * Stops the transfer of the file
     *
     * @since S60 v3.2
     */
    void CancelTransfer();
    
    /**
     * Starts the processing of a file. Asynchronous.
     *
     * @since S60 v3.2
     */
    virtual void StartProcessL();

// from base class MHTTPTransactionCallback

    /**
     * Called by framework to notify about transaction events.
     *
     * @since S60 v3.2
     * @param aTransaction Transaction, where the event occured.
     * @param aEvent Occured event.
     */
    virtual void MHFRunL( RHTTPTransaction aTransaction,
                          const THTTPEvent& aEvent );

    /**
     * Called by framework when leave occurs in handling of 
     * transaction event.
     *
     * @since S60 v3.2
     * @param aError The leave code that occured.
     * @param aTransaction The transaction that was being processed 
     * @param aEvent The event that was being processed
     * @return Error code
     */
    virtual TInt MHFRunError( TInt aError, 
                              RHTTPTransaction aTransaction,
                              const THTTPEvent& aEvent );             

// from base class MHttpDataSupplier

    /**
     * Obtain a data part from the supplier.  The data is guaranteed
     * to survive until a call is made to ReleaseData()
     *
     * @since S60 v3.2
     * @param aDataPart - the data part
     * @return ETrue if this is the last part. EFalse otherwise
     */   
    TBool GetNextDataPart( TPtrC8& aDataPart );
    
    /**
     * Release the current data part being held at the data
     * supplier.  This call indicates to the supplier that the part
     * is no longer needed, and another one can be supplied, if
     * appropriate.
     *
     * @since S60 v3.2.
     */   
    void ReleaseData();

    /**
     * Obtain the overall size of the data being supplied, if known
     * to the supplier.  Where a body of data is supplied in several
     * parts this size will be the sum of all the part sizes. If
     * the size is not known, KErrNotFound is returned; in this case
     * the client must use the return code of GetNextDataPart to find
     * out when the data is complete.
     *
     * @since S60 v3.2
     * @return Return code
     */       
    TInt OverallDataSize();
    
    /**
     * Reset the data supplier. This indicates to the data supplier that 
     * it should return to the first part of the data. This could be used 
     * in a situation where the data consumer has encountered an error
     * and needs the data to be supplied afresh. Even if the last part 
     * has been supplied (i.e. GetNextDataPart has returned ETrue), the 
     * data supplier should reset to the first part.
     *
     * If the supplier cannot reset it should return an error code; 
     * otherwise it should return KErrNone, where the reset will be assumed
     * to have succeeded
     *
     * @since S60 v3.2
     * @return Error code
     */       
    TInt Reset();
   

private:
   
    /**
     * Default constructor
     *
     * @since S60 v3.2
     * @param aObserver Transfer observer
     * @param aIAPId IAP id
     * @param aBufferSize Size of the buffer
     * @param aCallback Call back to workerobserver
     */
    CHttpUploadWorker( MHttpTransferObserver& aObserver,
                       TUint aIAPId,
                       TUint aBufferSize,
                       MHttpWorkerObserver& aCallback );

    /**
     * Second phase constructor
     *
     * @since S60 v3.2
     */
    void ConstructL();
    
    /**
     * Map HTTP transaction error to Symbian error (POST request)
     *
     * @since S60 v3.2
     * @param aError HTTP transaction error
     * @return symbian error code
     */
    TInt MapPostTransactionError( TInt aError ) const;

    /**
     * Complete the transaction and notify the client
     *
     * @since S60 v3.2
     * @param aError HTTP transaction error
     */
    void CompleteAndNotify( TInt aError );

    /**
    * Callback function for timer expirations.
    * @since Series60 5.0 HN
    * @param aTimer Timer that has expired.
    **/
    void TimerEventL( CHttpNotifyTimer* aTimer );
    
    /**
     * Cancels and destroys the 100-continue response wait timer if it exists.
     *
     * @since S60 v3.2.3
     */
    void Destroy100ContinueTimer();
    
    /**
     * Callback method for 100 continue timer. Cancels waiting for 
     * 100-continue response and proceeds with sending the body.
     *
     * @param TAny* the object who generated the event 
     *              (TAny pointer to this object)
     * @since S60 v3.2.3
     */
    static TInt Cancel100ContinueWaitL( TAny* aParam );

private: 
 
    /**
     * For performance reasons,
     * read data from the file in chunks of 640K
     * Own.
     */
    HBufC8* iRequestBodyBuffer;
 
    /**
     * How much body data has been sent to the HTTP stack
     */
    TUint iSendDataCount;
    
    /**
     * How much data will be sent
     */
    TInt iOverallDataSize;
    
    /**
     * Is there still data left
     */
    TBool iMoreToCome;
    
    /**
     * Was the process of the request already done
     * to avoid the repeated the process of the same part of the request
     */
    TBool iAlreadyDone;
      
    /**
     * Here in file to start to read, currently always 0 
     * (the beginning of the file)
     */
    TUint iBodyFileOffset;
    
    /**
     * Http status
     */ 
    TInt iHttpStatus;            
    
    /**
     * Timer to wait for 100-continue response. If no response is received 
     * in the given time, we will continue with sending the body.
     * Owned.
     */
    CPeriodic* i100ContinueTimer;
    
    /**
    * Timeout timer for current session
    */
    CHttpNotifyTimer* iSessionTimer;
    
    };

#endif // C_HTTPUPLOADWORKER_H