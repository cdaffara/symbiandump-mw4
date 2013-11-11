/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:           The class for submitting synchronous requests via call
*                to asynchronous SendL of a certain MSenTransport instance
*                and making the ActiveScheduler to wait until a response
*                callback is received from the transport (typically a server
*                for certain networking stack).
*
*/











#ifndef SEN_SYNC_REQUESTER_H
#define SEN_SYNC_REQUESTER_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>

#include "msenservicesession.h"
#include "msenremoteserviceconsumer.h"
#include "MSenProperties.h"

// FORWARD DECLARATIONS
class MSenServiceSession;
class MSenRemoteServiceConsumer;
class MSenCoreServiceManager;
class MSenTransport;
class CSenGuidGen;
class MSenRemoteHostlet;
class MSenProperties;
// CLASS DECLARATION

/**
 * The class for submitting requests in a synchronous
 * *manner* using the async SendL implementation offered
 * by given MSenTransport reference.
 *
 * Any transport plug-in developer may wish to use this
 * class to implement SubmitL of her transport, using
 * the SendL method implementation of that transport.
 * In most of the cases, that is the most beneficial
 * way to implement SubmitL, blocking the caller's 
 * ActiveScheduler execution until a response is 
 * received.
 *
 * One can take a look of LocalTransportPlugin implementation
 * and use it as a reference of how this synchronous requester
 * class can be implemented.
 */
class CSenSyncRequester : public CBase,
                          public MSenServiceSession,
                          public MSenRemoteServiceConsumer
    {
    public:
        /**
         * Two-phased constructor.
         */
        IMPORT_C static CSenSyncRequester* NewL(MSenTransport& aTransport,
                                                MSenCoreServiceManager& iServiceManager);

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CSenSyncRequester* NewLC(MSenTransport& aTransport,
                                                 MSenCoreServiceManager& iServiceManager);

        /**
         * Destructor.
         */
        IMPORT_C virtual ~CSenSyncRequester();


        /**
         * Submit a synchronous HTTP POST request to a given URI using
         * the - asynchronous - SendL method of the MSenTransport that
         * this class is "wrapping". Any transport plugin, which wishes
         * to implement SubmitL by calling and waiting SendL to complete,
         * can use this method to implement their synchronous SubmitL
         * function.
         *
         * @param aRequestUri service endpoint (URI) for the outgoing message.
         * @param aMessage outgoing message content.
         * @param aRequestTransportProperties outgoing message properties
         * @param aResponseContent incoming response message (content).
         * @param aStatusCode the status code of the response message,
         *        indicating whether or not an error occurred
         * @return a transaction id of the "wrapped" SendL, which is performed
         *         inside this method, or error code, negative integer, if
         *         sending of the message fails.
         */
        virtual TInt SubmitL(const TDesC8& aEndpointUri,
                     const TDesC8& aMessage,
                     const TDesC8& aRequestTransportProperties,
                     HBufC8*& apResponseTo);

        // Functions from base classes
    
        /**
        *   from MSenServiceSession
        */
    
        virtual TInt SendToConsumerL(HBufC8* apMessage,
                                     const TInt aTxnId,
                                     MSenRemoteServiceConsumer& aConsumer,
                                     MSenProperties* aResponseTransportProperties = NULL);

        virtual TInt SendErrorToConsumerL(const TInt aErrorCode,
                                          HBufC8* apError,
                                          const TInt aTxnId,
                                          MSenRemoteServiceConsumer& aConsumer,
                                          MSenProperties* aResponseTransportProperties = NULL);  

        virtual TInt HandleErrorL(const TInt aErrorCode,
                                  HBufC8* apError,
                                  const TInt aTxnId,
                                  MSenRemoteServiceConsumer& aConsumer,
                                  MSenProperties* aResponseTransportProperties = NULL);
        /**
        *   from MSenRemoteServiceConsumer
        */
        virtual const TDesC8& Id() const;
        virtual TInt SetSessionL(MSenRemoteServiceSession& aServiceSession);
        virtual TInt HandleMessageL(HBufC8* apMessage,
                                    const TInt aTxnId,
                                    MSenProperties* aResponseTransportProperties);

        virtual TInt HandleErrorL(HBufC8* aError,
                                  const TInt aErrorCode,
                                  const TInt aTxnId,
                                  MSenProperties* aResponseTransportProperties);

         virtual void SetStatusL(const TInt aStatus);
         virtual MSenTransport& TransportL();
         virtual CSenIdentifier& Identifier() const;  
         virtual TInt ChunkByTxnIdL(TInt aTxnId, CSenChunk*& aChunk);
         virtual MSenRemoteHostlet* CSenSyncRequester::Hostlet() const;

         virtual TInt SendToHostletL(MSenRemoteHostlet& aReceiver,
                                     const TDesC8& aMessage,
                                     const TInt aTxnId,
                                     MSenRemoteServiceConsumer& aFrom,
                                     MSenProperties* aProperties = NULL);
        virtual void FileProgress(TInt aTxnId, TBool aIncoming, TBool aIsSoap,
                const TDesC8& aSoapOrCid, TInt aProgress);
		virtual void SetDataTrafficDetails( TSenDataTrafficDetails& aDetails); 	
    protected:
        IMPORT_C CSenSyncRequester(MSenTransport& aTransport, 
                                   MSenCoreServiceManager& iServiceManager);
                          
        IMPORT_C void BaseConstructL();

    private:  // Data
        MSenTransport& iTransport;
        MSenCoreServiceManager& iServiceManager; // for GUID generation API access etc
        CActiveSchedulerWait iSchedulerWait;

        CSenIdentifier* ipIdentifier;   // owned
        HBufC8* ipConsumerId;           // owned
        HBufC8* ipResponse;             // owned
        HBufC8* ipResponseProperties;   // owned, contains properties of the response
        MSenProperties::TSenPropertiesClassType         iResponsePropertiesType;
        TInt iStatusCode;               // contains KErrNone or some error
    };

#endif //SEN_SYNC_REQUESTER_H

// End of File
