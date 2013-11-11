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
* Description: Header declaration
*
*/








#ifndef M_SEN_REMOTE_SERVICE_SESSION
#define M_SEN_REMOTE_SERVICE_SESSION


/*
 * MSenRemoteServiceSession is a non-public remote interface that is only 
 * implemented by the CSenServiceSession class. The methods of this interface 
 * are called upon by instances of CSenClientSession.
 *
 * @see RService in Java reference implementation (2005) - regardless of 
 * name-mismatch methods of this interface *equal* with RService. 
 * 
*/

// INCLUDES
#include "MSenServiceDescription.h"
#include "msentransport.h"
#include "MSenFragment.h"

// FORWARD DECLARE
class CSenTransportBase;
class MSenRemoteServiceConsumer;
class MSenRemoteHostlet;
class CSenChunk;

//enum TDescriptionClassType;


// CLASS DECLARATION
class MSenRemoteServiceSession  //: public MSenFragment
    {
    public:
    
        // New functions
        
        virtual MSenServiceDescription& AsServiceDescription() = 0;
        /*
         * 
         * 
         * 
         */
        virtual void WriteAsXMLToL(RWriteStream& aWriteStream) = 0;

    
        /*
         * StatusL() will compute and return the status code.
         *
         * @return current status of this service session. Basic
         * constants are defined in public MSenServiceConsumer.h:
         *
         * KSenConnectionStatusNew              Connection is being initialized, but not yet ready.
         * KSenConnectionStatusReady            Connection is ready to be used. For example, SubmitL()
         *                                      and SendL() methods (depending of framework) are in
         *                                      invocable state.
         * KSenConnectionStatusExpired          Connection is expired. Typically, a new connection
         *                                      needs next to be initialized in order to communicate
         *                                      with the underlying service behind this service
         *                                      connection.
         * If any error should occur in this method, it leaves.
         *  
         */
        virtual TInt StatusL() = 0;
    
        /*
         * Method synchronously submits a message to (remote) service 
         * via the transport of this session. Method will block until
         * the response is received.
         * 
         * @param aMessage is the actual request body to be sent to
         *        the remote service via the transport of this session.
         * @param aTransportProperties are the properties to be passed
         *        to the transport. Transport properties further define
         *        how the message should be carried out to the service
         *        provider. In HTTP transport's case, properties could
         *        dictate what HTTP method, header(s) and proxy params
         *        are to be used in the transport layer.
         * @param aConsumer is a reference to the actual caller of the
         *        service, which is consuming this session. Transport
         *        might be interested to resolve identifier, and other
         *        aspects of the consumer via this handle.
         * @param aResponse is reference to a pointer where transport
         *        is expected to reserve memory for tha actual response.
         * @return KErrNone if submit is successfull or otherwise some 
         *        system-wide error code.
         */
        virtual TInt SubmitL(const TDesC8& aMessage, 
                             const TDesC8& aTransportProperties,
                             MSenRemoteServiceConsumer& aConsumer,
                             HBufC8*& aResponse) = 0;
    
        /*
         * Method asynchronously sends a message to (remote) service
         * via the transport of this session. Function will typically
         * return almost immediately, since the service response will
         * be received via spesific callback methods.
         * 
         * @param aMessage is the actual request body to be sent to
         *        the remote service via the transport of this session.
         * @param aTransportProperties are the properties to be passed
         *        to the transport. Transport properties further define
         *        how the message should be carried out to the service
         *        provider. In HTTP transport's case, properties could
         *        dictate what HTTP method, header(s) and proxy params
         *        are to be used in the transport layer.
         * @param aConsumer is a reference to the actual caller of the
         *        service, which is consuming this session. Transport
         *        might be interested to resolve identifier, and other
         *        aspects of the consumer via this handle.
         * @param aTxnId is reference to transaction id,
         *        to be set by the transport to identify
         *        this request.
         * @param aRevalidationError contains error / SOAP Fault from
         *        authentication service (like WS-* STS or Liberty AS / IDP)
         * @return KErrNone if send is successfull or otherwise some 
         *        system-wide error code.
         */
        virtual TInt SendL( const TDesC8& aMessage, 
                            const TDesC8& aTransportProperties,
                            MSenRemoteServiceConsumer& aConsumer,
                            TInt& aTxnId,
                            HBufC8*& aRevalidationError ) = 0;
    
        /*
         * Method synchronously submits a message to (remote) service 
         * via the transport of this session.
         * 
         * @param aSoapMessage is the actual request body - a SOAP
         *        envelope, which is to be sent to remote service 
         *        via the transport of this session.
         * @param aTransportProperties are the properties to be passed
         *        to the transport. Transport properties further define
         *        how the message should be carried out to the service
         *        provider. In HTTP transport's case, properties could
         *        dictate what HTTP method, header(s) and proxy params
         *        are to be used in the transport layer.
         * @param aConsumer is a reference to the actual caller of the
         *        service, which is consuming this session. Transport
         *        might be interested to resolve identifier, and other
         *        aspects of the consumer via this handle.
         * @param aResponse is reference to a pointer where transport
         *        is expected to reserve memory for tha actual response.
         * @return KErrNone if submit is successfull or otherwise some 
         *        system-wide error code.
         */
        virtual TInt SubmitSoapL(const TDesC8& aSoapMessage,
                                 const TDesC8& aTransportProperties,
                                 MSenRemoteServiceConsumer& aConsumer,
                                 HBufC8*& aResponse) = 0;
        /*
         * Method asynchronously sends a message to (remote) service
         * via the transport of this session. Function will typically
         * return almost immediately, since the service response will
         * be received via spesific callback methods.
         * 
         * @param aSoapMessage is the actual request body - a SOAP
         *        envelope, which is to be sent to remote service 
         *        via the transport of this session.
         * @param aTransportProperties are the properties to be passed
         *        to the transport. Transport properties further define
         *        how the message should be carried out to the service
         *        provider. In HTTP transport's case, properties could
         *        dictate what HTTP method, header(s) and proxy params
         *        are to be used in the transport layer.
         * @param aConsumer is a reference to the actual caller of the
         *        service, which is consuming this session. Transport
         *        might be interested to resolve identifier, and other
         *        aspects of the consumer via this handle.
         * @param aTxnId is reference to transaction id,
         *        to be set by the transport to identify
         *        this request.
         * @param aRevalidationError contains error / SOAP Fault from
         *        authentication service (like WS-* STS or Liberty AS / IDP)
         * @return KErrNone if send is successfull or otherwise some 
         *        system-wide error code.
         */
        virtual TInt SendSoapL( const TDesC8& aSoapMessage,
                                const TDesC8& aTransportProperties,
                                MSenRemoteServiceConsumer& aConsumer,
                                TInt& aTxnId,
                                HBufC8*& aRevalidationError ) = 0;
    
    
        /**
        * Method checks if this session is ready to interact with (remote)
        * service. Typically, if service invocation framework requires 
        * credentials, they are checked inside this method.
<        * @return boolean indicating if this session is ready (ETrue) or
        *         not (EFalse) to interact with (possibly remote) service.
        */
        virtual TBool IsReadyL() = 0;

        virtual TInt HasFacetL(const TDesC8& aURI, TBool& aHasFacet) = 0;
        virtual TInt FacetValue(TDesC8& aURI, HBufC8*& aValueTo) = 0;
        virtual TInt CompleteServerMessagesOnOffL(const TBool& aCompleteOnOff) = 0;
        virtual TPtrC8 Endpoint() = 0;
        virtual TPtrC8 Contract() = 0;
        virtual TPtrC8 FrameworkId() = 0;
        virtual void StartTransaction() = 0;
        virtual void TransactionCompleted() = 0;
        virtual MSenServiceDescription::TDescriptionClassType DescriptionClassType() = 0;

        /**
        * Method is used to apply transport properties for this service session.
        * In basic implementation, the properties information is passed to 
        * transport of this session.
        * @param aProperties contains transport properties properties as descriptor
        * @param aConsumer is the "actor", typically the remote consumer which is 
        *        invoking this method.
        * @return KErrNone if successful, or otherwise some system-wide error code
        */
        virtual TInt SetTransportPropertiesL(const TDesC8& aProperties,
                                             MSenRemoteServiceConsumer& aConsumer) = 0;

        virtual TInt TransportPropertiesL(HBufC8*& aProperties,
                                             MSenRemoteServiceConsumer& aConsumer) = 0;



        /**
        * Sets the transport plug-in instance,
        * passing the ownership to this 
        * session.
        * @param aTransport is the new transport for this session.
        * Note that if a Transport instance is already owned, it 
        * will be deleted.
        */
        virtual void SetTransportL(CSenTransportBase* aTransport) = 0;

        /**
        * Getter for currently effective transport instance.
        * It is strongly recommended, that service sessions
        * themselves acquire transport via TransportL -variant,
        * which is capable of factoring a transport in case
        * that it does not exist already.
        * @return currently set transport, or NULL
        * 
        */
        virtual MSenTransport* Transport() const = 0;

        /**
        * Returns the transport of this session. If transport is not
        * yet known (owned), an attempt to create a transport for this
        * session will be performed, and this attempt may result this
        * method to leave.
        * @param aRequestor is the consumer of this session, which is
        * about to utilize the transport of this session.
        * Leaves with system-wide error code in case that a new transport
        * cannot be instantiated.
        */
        virtual MSenTransport& TransportL(MSenRemoteServiceConsumer& aRequestor) = 0;

        /**
        * Sets the hostlet for this session. HostletConnection server-side
        * sessions (CSenClientSessions) call this method in order to provide
        * their own reference to session, which can be utilized by HostletTransport
        * plug-ins, so that the messages from remote service consumers can be routed.
        * @param aHostlet is a reference to MSenRemoteHostlet implementation, typically
        * to CSenClientSession class. Note that it is perfectly legal to pass
        * NULL as aHostlet - that call just means, that any existing MSenRemoteHostlet
        * will be "unset" / detached from this session.
        * @return KErrNone if successful, or otherwise some system-wide error code.
        * Method may leave, if aHostlet is found invalid.
        */
        virtual TInt SetHostletL(MSenRemoteHostlet* aHostlet) = 0;

 
        virtual MSenRemoteHostlet* Hostlet() const = 0;        

        /**
        * Removes a service consumer from this session.
        *
        * Removed consumer will no longer receive any callbacks
        * from the service provider it used to communicate with.
        * Typically, MSenRemoteServiceConsumer implementations call
        * this method from their destructors. When the count of
        * session consumers is decreased to zero, and session
        * does not hold any reference to a MSenRemoteHostlet either
        * the default implementation provided in here will also
        * delete the associated transport plug-in instance, in order
        * to save (memory, messaging stack etc) resources .
        * @param aConsumer is the service consumer to be removed.
        * @return positive index of successfully removed consumer or
        *         KErrNotFound - if consumer was not found
        */
        virtual TInt RemoveConsumerL(MSenRemoteServiceConsumer& aConsumer) = 0;
        
        /**
        * This method is invoked by WS-stack Core on each call to
        * CSenHostletConnection::RespondL. The typical implementation
        * in service session class will use associated transport plug-in
        * to complete the pending transaction. If contents of the message
        * do not require any modification, then the session can rely that
        * the response set by the hostlet application (MSenHostlet) is
        * already in place. If there is need to modify the response in
        * SIF spesific way, then the message can be resolved using the
        * txn ID (before completing this transaction).
        * @param aProvider is the interface to the hostlet that is providing
        * this response -- via RespondL issued inside (or after) 
        * MSenHostlet::ServiceL.
        * @param aTxnId is the transaction ID of this request/response pair
        * @param aServiceCode is the code that was set to MSenHostletResponse
        * by the hostlet application
        * @param aRecipientsConsumerId may be used to resolve interested
        * service consumer, which issued the request to the hostlet
        * @param aMessageChunk contains the message as raw 8-bit data,
        * plus it's other properties
        * @return TInt KErrNone on success, or otherwise some of the system-wide
        * error codes.
        */
        virtual TInt ProvideHostletResponseL(MSenRemoteHostlet& aProvider,
                                             const TInt aTxnId,
                                             const TInt aServiceCode,
                                             const TDesC8& aRecipientsConsumerId,
                                             CSenChunk& aMessageChunk) = 0;
        

        //virtual HBufC8* AsXmlL() = 0;

        //virtual TInt SignMessages(const TDesC8& aAlgoritm,
        //                          const TDesC8& aSigningKey) = 0;
    };
#endif // M_SEN_REMOTE_SERVICE_SESSION


