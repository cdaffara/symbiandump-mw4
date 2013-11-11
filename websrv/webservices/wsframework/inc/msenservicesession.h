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
* Description:        Non-public callback interface, which transports typically call
*
*/








#ifndef M_SEN_SERVICE_SESSION
#define M_SEN_SERVICE_SESSION


/**
 * RServiceSession is a non-public remote interface
 * only implemented by the CSenServiceSession class. 
 * These methods are typically invoked by transport
 * instancies.
 *
 * Interface equals with com.nokia.serene.framework.RServiceSession
 * 
 * @see CSenServiceSession and Jave ref-implementation
 * in com.nokia.serene.framework.ServiceSession (2005)
 */

 

// FORWARD DECLARES
class MSenRemoteServiceConsumer;
class MSenRemoteHostlet;
class MSenProperties;

// CLASS DECLARATION
class MSenServiceSession
    {
    public:
        /**
        * Send a message with transaction ID to the service consumer.
        * This method allows session implementation to perform internal
        * mechanisms prior handing the response to the actual, already
        * known consumer (MSenRemoteServiceConsumer).
        *
        * Session implementation might decide, that certain messages are
        * not delivered to consumer(s). Or, the session may validate the
        * received message and actually decide sending an error to the
        * consumer.
        * 
        * @see sendToConsumer in com.nokia.serene.framework.RServiceSession
        *
        * @param aMessage is the response message from service
        * @param aTxnId is the transaction id of this async responsed message
        * @param aConsumer is the consumer of the service. It could
        * be CSenClientSession or some subclass of CSenInternalConsumer
        * (like in Liberty ID-WSF it could be authentication service client
        * or discovery service client).
        * @param aResponseTransportProperties contains the properties related
        * to this response (from the transport plug-in).
        * @return KErrNone if message was successfully delivered to consumer(s).
        *         Otherwise, some system-wide error code.
        */
        virtual TInt SendToConsumerL(HBufC8* apMessage,
                                     const TInt aTxnId,
                                     MSenRemoteServiceConsumer& aConsumer,
                                     MSenProperties* aResponseTransportProperties = NULL) = 0; 

        /**
        * Send an error code and message with transaction ID to the service 
        * consumer. This method allows session implementation to perform 
        * internal mechanisms prior handing the response to the actual, 
        * already known consumer (MSenRemoteServiceConsumer).
        *
        * Session implementation might decide, that certain errors are handled
        * without notifying the consumer(s).
        *
        * @see sendFaultToConsumer in com.nokia.serene.framework.RServiceSession
        *
        * @param aErrorCode is the error code identifying this error
        * @param aError is the error message - a response from service
        * @param aConsumer is the consumer of the service. It could
        * be CSenClientSession or some subclass of CSenInternalConsumer
        * (like in Liberty ID-WSF it could be authentication service client
        * or discovery service client).
        * @param aTxnId is the transaction id of this async responsed error.
        * @return KErrNone if error message was successfully delivered to 
        *         consumer(s). Otherwise, some system-wide error code.
        */
        virtual TInt SendErrorToConsumerL(const TInt aErrorCode,
                                          HBufC8* apError,
                                          const TInt aTxnId,
                                          MSenRemoteServiceConsumer& aConsumer,
                                          MSenProperties* aResponseTransportProperties = NULL) = 0; 


        /**
        * Method allows sessions of service invocation framework to recover
        * from certain errors (like certain SOAP faults) which can be recognized
        * at (low) transport layer. This way these faults could be  processed 
        * silently between service and its session, without disturbing the 
        * actual service consumer.
        *  
        * Since this type of errors (or faults) usually consists of XML,
        * a spesific error class can also be created to parse received
        * error.
        *
        * @see Java ref-implementation: handleFault in 
        *  com.nokia.serene.framework.RServiceSession
        *
        * @param aErrorCode is the error code identifying this error
        * @param aError is the error message - actual response from service
        * @param aTxnId is the transaction id of this async responsed error.
        * @param aConsumer is the consumer of the service. It could
        * be CSenClientSession or some subclass of CSenInternalConsumer
        * (like in Liberty ID-WSF it could be authentication service client
        * or discovery service client).
        * @param aResponseTransportProperties contains the properties related
        * to this response (from the transport plug-in).
        * @return KErrNone if error message was successfully delivered to 
        *         consumer(s). Otherwise, some system-wide error code.
        */
        virtual TInt HandleErrorL(const TInt aErrorCode,
                                  HBufC8* aError,
                                  const TInt aTxnId,
                                  MSenRemoteServiceConsumer& aConsumer,
                                  MSenProperties* aResponseTransportProperties = NULL) = 0; 

        /**
        * Returns the currently set hostlet 
        * @return MSenRemoteHostlet pointer or NULL, if this session
        * does not interact with hostlet (local service provider 
        * application).
        */
        virtual MSenRemoteHostlet* Hostlet() const = 0;

                      
       /*
        * @param aReceiver is the hostlet to which the message should be delivered
        * @param aMessage is the request from service a service consumer. 
        * SIF spesific service session implementation(s) may read or manipulate this
        * message by before handling it over to the hostlet (application).
        * @param aFrom is the consumer of the service, which is sending
        * the message.
        * @param aTxnId is the transaction id of this async message
        * @return KErrNone if message was successfully delivered to consumer(s).
        *         Otherwise, some system-wide error code.
        */             
        virtual TInt SendToHostletL(MSenRemoteHostlet& aReceiver,
                                    const TDesC8& aMessage,
                                    const TInt aTxnId,
                                    MSenRemoteServiceConsumer& aFrom,
                                    MSenProperties* aProperties = NULL) = 0; 
                                                                       
    };
#endif // M_SEN_SERVICE_SESSION
