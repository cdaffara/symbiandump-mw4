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
* Description:           The class for submitting synchronous HTTP requests.
*                Create and initialize instance of CSenHttpChannel
*                first and pass it as a parameter to this wrapper
*                class in a constructor.
*
*/











#ifndef WSF_HTTP_SYNC_REQUESTER_H
#define WSF_HTTP_SYNC_REQUESTER_H

//  INCLUDES
#include <badesca.h>

#include "senresponseobserver.h"
#include "MSenElement.h"

// FORWARD DECLARATIONS
class CSenHttpChannel;
class CSenLayeredHttpTransportProperties;

// CLASS DECLARATION

/**
 * The class for submitting synchronous HTTP requests.
 * Create and initialize instance of CSenHttpChannel
 * first and pass it as a parameter to this wrapper
 * class in a constructor.
 *
 */
class CSenHttpSyncRequester : public CTimer,
                              public MSenResponseObserver
    {
public:
    /**
     * Two-phased constructor.
     */
    static CSenHttpSyncRequester* NewL(CSenHttpChannel* aHttpChannel);

    /**
     * Two-phased constructor.
     */
    static CSenHttpSyncRequester* NewLC(CSenHttpChannel* aHttpChannel);

    /**
     * Two-phased constructor.
     */
    static CSenHttpSyncRequester* NewL(CSenHttpChannel* aHttpChannel,
                                        MSenResponseObserver* aParentObserver);

    /**
     * Two-phased constructor.
     */
    static CSenHttpSyncRequester* NewLC(CSenHttpChannel* aHttpChannel,
                                        MSenResponseObserver* aParentObserver);

    /**
     * Destructor.
     */
    virtual ~CSenHttpSyncRequester();

    /**
     * Submit a synchronous HTTP GET request to a given URI.
     * The method leaves if an error occurs. This method
     * takes the ownership of the heap buffers aResponseContentType
     * and aResponseContent.
     *
     * @param aRequestUri outgoing request URI.
     * @param aResponseContentType incoming response content type.
     * @param aResponseContent incoming response content.
     */
    // HTTP GET
    //TInt SubmitL(const TDesC8& aRequestUri,
    //                      HBufC8*& aResponseContentType,
    //                      HBufC8*& aResponseContent,
    //                      TInt& aErrorCode);

    /**
     * Submit a synchronous HTTP POST request to a given URI.
     * The method leaves if an error occurs. This method
     * takes the ownership of the heap buffers aResponseContentType
     * and aResponseContent.
     *
     * @param aRequestUri outgoing request URI.
     * @param aRequestContentType outgoing request content type.
     * @param aRequestContent outgoing request content.
     * @param aResponseContentType incoming response content type.
     * @param aResponseContent incoming response content.
     */
    TInt SubmitL(const TDesC8& aRequestUri,
                 CSenLayeredHttpTransportProperties& aProps,
                 const TDesC8& aRequestContent,
                 HBufC8*& aResponseContentType,
                 HBufC8*& aResponseContent,
                 TInt& aErrorCode);

    /*
    // SOAP envelope version of HTTP POST, IOP
    TInt SubmitL(const TDesC8& aRequestUri,
                    const TDesC8* aRequestContentType,
                    MSenElement& aMessage, // SOAP envelope
                    HBufC8*& aResponseContentType,
                    HBufC8*& aResponseContent,
                    TInt& aErrorCode);
    */

        // Functions from base classes
        
    /**
     * Callback method from MSenResponseObserver.
     */
    virtual void ResponseReceivedL( TInt aRequestId,
                                    const TAny* aContentType, 
                                    HBufC8* apContent,
                                    CSenHttpTransportProperties* aHttpPrperties = NULL );

    /**
     * Callback method from MSenResponseObserver.
     */
    virtual void ResponseErrorL(TInt aId, TInt aError, HBufC8* aErrorBody, CSenHttpTransportProperties* aHttpProperties  );
    virtual void FileProgress(TInt /*aTxnId*/, TBool /*aIncoming*/,
            TBool /*aIsSoap*/, const TDesC8& /*aSoapOrCid*/, TInt /*aProgress*/)
        {
        }

    virtual void StateChanged(TInt aRequestId, TInt aState); // In order to propagate http status codes
	virtual void SetTrafficDetails(TSenDataTrafficDetails& aDetails) ; 
    virtual MSenProperties& PropertiesL();
    
//from activeObject
    void DoCancel();
    void RunL();
    TInt RunError(TInt /*aError*/);

private:
    CSenHttpSyncRequester(CSenHttpChannel* aHttpChannel,
                          MSenResponseObserver* aParentObserver);
    void ConstructL();

private:  // Data
    CSenHttpChannel* iHttpChannel;          // not owned
    CActiveSchedulerWait iSchedulerWait;
    TBool isStopped;
    HBufC8* iContentType;
    HBufC8* iContent;
    TInt iStatusCode; // containts KErrNone or some error
    MSenResponseObserver* iParentObserver;  // not owned
    };

#endif //WSF_HTTP_SYNC_REQUESTER_H

// End of File
