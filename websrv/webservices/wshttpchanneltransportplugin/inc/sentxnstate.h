/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Header declaration
*
*/











#ifndef WSF_TXN_STATE_H
#define WSF_TXN_STATE_H

//  INCLUDES
#include <e32base.h>
#include <http/mhttpdatasupplier.h>
#include <flogger.h>
#include "senresponseobserver.h"
#include "senmultipartutils.h"

// CONSTANTS
namespace
    {
    // Panic texts
    _LIT(KRequestUriNullPanicText,   "Req URI is NULL");
    _LIT(KPostContTypeNullPanicText, "PostCntType NULL");
    _LIT8(KDownloadFolder, "SenBlobResponses"); // folder where should be created Blobs from Response
    }

// FORWARD DECLARATIONS
class CSenAsyncTimeOut;
// CLASS DECLARATION
/**
 * Class for handling the transaction state in the http client class.
 * Implements the MHTTPDataSupplier interface.
 *
 */
class CSenTxnState : public CBase,
                     public MHTTPDataSupplier
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     */
    static CSenTxnState* NewLC(MSenResponseObserver& aObserver,
                               //RFileLogger* aLog,
                               const TDesC8* aUri,
                               const TDesC8& aContentType,
                               const TDesC8* aBody = NULL);

    /**
     * Two-phased constructor.
     *
     */
    static CSenTxnState* NewL(MSenResponseObserver& aObserver,
                              //RFileLogger* aLog,
                              const TDesC8* aUri,
                              const TDesC8& aContentType,
                              const TDesC8* aBody = NULL);

    /**
     * Virtual destructor.
     *
     */
    virtual ~CSenTxnState();

        // New functions

    /**
     * Transaction state's identifier.
     * @return Id of the transaction handled in this transaction state object.
     */
    TInt Id() const;
    
    RHTTPTransaction Transaction() const;

    /**
     * Set transaction state's identifier.
     */
    void SetId(TInt aId);
    void SetTransaction(RHTTPTransaction aTransaction);

    /**
     * Collects incoming response's body.
     * @param aDataPart data chunk from response body's GetNextDataPart
     */
    void CollectResponseBodyL(const TDesC8& aDataPart);

    /**
     * @return URI for the message to be posted.
     */
    const TDesC8& RequestUri() const;

    /**
     * @return content type for the message to be posted.
     */
    const TDesC8& PostContentType() const;

    inline TBool HasRequestBody() { return iHasRequestBody; }
    void ResponseReceivedL(const TDesC8& aContentType);
    void ResponseErrorL(TInt aError);
	void StateChanged(TInt aState); // In order to propagate http status codes

    TInt Cancel();

    /**
    * This transformation method appends the current
    * contents of body (if not NULL) into a tailing
    * postfix of current Uri. Method is typically 
    * called prior sending a GET transaction. Method
    * appends the "body" string (GET query string, 
    * DELETE argument) into the base URI, as it is.
    * The responsibility of the correctness for this
    * newly formed URI is on the caller SendL/SubmitL,
    * who might also has set the transport properties, too.
    * Since method temporarily allocates memory in order
    * to make a copy of the body, it may leave if system
    * runs out of heap.
    *
    * Note: this method deletes the current body owned
    * by this class instance, and sets iHasRequestBody
    * to EFalse.
    */
    void TransformBodyToUriL();

    /**
    * Returns the body (content) of this transaction
    */
    TPtrC8 Body();
    
    /**
    * Sets Content-Type in HTTP header
    * @param aSession
    * @param aHeaders
    */
    virtual void SetContentTypeHeaderL(const RHTTPSession aSession, RHTTPHeaders aHeaders);

    // Functions from base classes

    /**
     * From MHTTPDataSupplier::GetNextDataPart.
     * Obtain a data part from the supplier.  The data is guaranteed
     * to survive until a call is made to ReleaseData().
     * @param aDataPart The data part.
     * @return ETrue if this is the last part, EFalse otherwise.
     */
    virtual TBool GetNextDataPart(TPtrC8& aDataPart);

    /**
     * From MHTTPDataSupplier::ReleaseData.
     * Release the current data part being held at the data
     * supplier.  This call indicates to the supplier that the part
     * is no longer needed, and another one can be supplied, if
     * appropriate.
     */
    virtual void ReleaseData();

    /**
     * From MHTTPDataSupplier::OverallDataSize.
     * Obtain the overall size of the data being supplied, if known
     * to the supplier.  Where a body of data is supplied in several
     * parts this size will be the sum of all the part sizes. If
     * the size is not known, KErrNotFound is returned; in this case
     * the client must use the return code of GetNextDataPart to find
     * out when the data is complete.
     *
     * @return A size in bytes, or KErrNotFound if the size is not known.
     */
    virtual TInt OverallDataSize();

    /**
     * From MHTTPDataSupplier::Reset.
     * Reset the data supplier.  This indicates to the data supplier
     * that it should return to the first part of the data.
     * This could be used in a situation where the data consumer has
     * encountered an error and needs the data to be supplied afresh.
     * Even if the last part has been supplied (i.e. GetNextDataPart
     * has returned ETrue), the data supplier should reset to the
     * first part. If the supplier cannot reset it should return an
     * error code; otherwise it should return KErrNone, where the
     * reset will be assumed to have succeeded.
     *
     * @return KErrNone if successful, otherwise another error code.
     */
    virtual TInt Reset();
	void SetSession(RHTTPSession aSession); 	 
    TInt CalculateHeadersSizeL(RHTTPHeaders aHeaders); 

    void SetTP(CSenHttpTransportProperties* tp);
    MSenProperties& HttpChannelPropertiesL();
    void EnableTimeOutL(TInt aTimeOutSec);
    void DisableTimeOutL();
protected:
    /**
     * C++ default constructor.
     *
     */
    CSenTxnState(MSenResponseObserver& aObserver);//,
                 //RFileLogger* aLog);
   
    /**
     * File logger.
     *
     */
    //RFileLogger* Log() const;

private:

    /**
     * 2nd phase constructor.
     *
     */
    void ConstructL(const TDesC8* aUri,
                    const TDesC8& aContentType,
                    const TDesC8* aBody);

protected: // Data
    /**
     * Pointer to the requester's observer. Not owned.
     */
    MSenResponseObserver* iObserver;
    /**
     * Pointer to the logger. Not owned.
     */
    //RFileLogger* iLog; // Not owned
    /**
     * Request has body.
     */
    TBool iHasRequestBody;
	
	TBool iPostBodyReleased;
	/**
     * URI of the request to be posted.
     */
    HBufC8* iRequestUri;

    RHTTPTransaction ipTransaction; // NOT OWNED
    /**
     * Id of the transaction in question.
     */
    TInt iId;
	/**
     * Body of the incoming response.
     */
    HBufC8* iResponseBody;
    /**
     * Body of the request to be posted.
     */
    HBufC8* iPostBody;
    /**
     * Content type of the request to be posted.
     */
    HBufC8* iPostContentType;
    
    /**
     * Pointer to the Session. Not owned.
     */    
    RHTTPSession iSession; // Not owned	
    /**
     * Contains DataTraffic Details.
     */    
    TSenDataTrafficDetails iDetails; 
    CSenHttpTransportProperties* iTP;//owned
    
    CSenAsyncTimeOut* iTimeOut;
    };

#endif // WSF_TXN_STATE_H

// End of File
