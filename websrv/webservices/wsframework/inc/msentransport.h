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








#ifndef M_SEN_TRANSPORT_H
#define M_SEN_TRANSPORT_H

// INCLUDES
#include <e32std.h>

#include "msenservicesession.h"
#include "msenremoteserviceconsumer.h"
#include "msenlayeredproperties.h"

#include "MSenElement.h"

// CONSTANTS
const TInt KErrSenTransactionComplete = 20;

// FORWARD DECLARATIONS
class CSenFragment;
class CSenTransportProperties;

// DATA TYPES
typedef RPointerArray<CSenFragment> RTransportProperties;

// CLASS DECLARATION
class MSenTransport
    {
    public:
    
        // New functions
        
        /**
        * Method for sending synchronous  request to a known endpoint (URI). This method
        * will block the current ActiveScheduler until the response from service is received.
        *
        * @param aEndpoint is the endpoint, typically an URI of the service being invoked
        * @param aMessage is the actual message to the service behind known endpoint
        * @param aTransportProperties containts the transport related request properties
        * @param aResponse will contain the response after this synchronous call returns.
        * @param aConsumer is the underlying caller invoking this request.
        * @return KErrSenNoEndpoint if aEndpoint lenght is zero or negative.
        */
        virtual TInt SubmitL(const TDesC8& aEndpoint,
                             const TDesC8& aMessage,
                             const TDesC8& aTransportProperties,
                             HBufC8*& aResponse,
                             MSenRemoteServiceConsumer& aConsumer) = 0;  
    
        /**
        * Method for sending asynchronous request to a known endpoint (URI).
        * @param aEndpoint is the endpoint, typically an URI of the service being invoked
        * @param aMessage is the actual message to the service behind known endpoint
        * @param aTransportProperties containts the transport related request properties
        * @param aReplyTo is the session session performing this request for the caller 
        * @param aConsumer is the underlying caller invoking this request (addressee)
        * @param aTxnId will be set to refer to the new transaction id of this request
        * @return KErrNone or some error, if the method fails. Error code can be either some
        * system-wide constnat, or a WSF specific KErrSenNoEndpoint if aEndpoint lenght is zero
        * or negative.
        */
        virtual TInt SendL(const TDesC8& aEndpoint,
                           const TDesC8& aMessage,
                           const TDesC8& aTransportProperties,
                           MSenServiceSession& aReplyTo,
                           MSenRemoteServiceConsumer& aConsumer,
                           TInt& aTxnId) = 0;

        /**
        * Method parses and applies the given properties on top of current properties 
        * (state) of the transport. This means, that all spesified property entries 
        * override existing ones 
        */
        virtual TInt SetPropertiesL(const TDesC8& aProperties,
                                    MSenLayeredProperties::TSenPropertiesLayer aTargetLayer,
                                    MSenRemoteServiceConsumer* aConsumer) = 0;
        
        virtual TInt PropertiesL(HBufC8*& aProperties) = 0;

        /**
        * Cancels the transaction from the transport, removing interested pending
        * consumers from list of response receivers, preventing any callbacks
        * being performed even though transport itself may still receive response
        * from some (network stack) server.
        * @return KErrNone, if cancel was performed
        *         KErrNotFound if transaction was not found 
        */
        virtual TInt CancelTransaction(const TInt aTxnId) = 0;

        /**
        * Method requests the transport plug-in implementation to complete
        * a pending transaction. Typically, service provider sessions
        * (like MSenRemoteHostlet) implementations call this method to
        * signalize, that certain pending, asynchronous transaction
        * issued earlier by some consumer has been completed.
        * @param aTxnId is the id of the transaction to be completed
        * @param aStatus is an optional, additinal status code, which
        * transport plug-in should pass to the "addressee", the consumer,
        * who originated this transaction.
        * @return KErrNone if transaction was completed successfully
        *         KErrNotFound if pending transaction, matching with
        *         given aTxnId was not found.
        */
        virtual TInt CompleteTransaction(const TInt aTxnId,
                                         const TInt aStatus = KErrNone) = 0;

        /**
        * Getter for the URI scheme of this transport plug-in implementation,
        * Examples of such scheme are: http, tcp, udp and local.
        * @return the scheme of this transport
        */
        virtual TPtrC8 UriSchemeL() = 0;
        
        
        
        virtual MSenProperties& PropertiesL() = 0;
    
        inline virtual TInt SubscribeEventListenerL(MSenRemoteServiceConsumer& /*aConsumer*/) 
            {
            return KErrNotSupported;
            }
        inline virtual TInt UnsubscribeEventListenerL(MSenRemoteServiceConsumer& /*aConsumer*/)
            {
            return KErrNotSupported;
            }
    };

#endif // SEN_MTRANSPORT_H

// End of File
