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








#ifndef M_SEN_REMOTE_HOSTLET
#define M_SEN_REMOTE_HOSTLET


/**
 * MSenRemoteHostlet is non-public interface, which is typically implemented by
 * Symbian server-side session implementations (like CSenClientSession) and
 * called by Transport plug-ins (like HostletTransportPlugin) classes.
 * Interface is used send requests (transactions from service consumers)
 * to hostlet applications (residing behind process boundary) - in this sense
 * the "remote" here stands for for that hostlet application process, which
 * is remotely manipulated. Any methods introduced in this interface, are
 * typically turned into IPC operations by the implementing classes, so
 * that corresponding real communication (data transfer) may take place.
 */

 // CLASS DECLARATION
class MSenRemoteHostlet
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
        * @param aMessage is the request message from service consumer
        * @param aConsumer is the consumer sending the request. 
        * @param aTxnId is the transaction id of this async request. 
        * This id is most likely used by MSenRemoteHostlet implementations
        * to query CSenChunk (RChunk) from RSenRemoteServiceConsumer, so
        * that copying of that payload can be avoided altogether.
        * @return KErrNone if message was successfully delivered to hostlet
        *         KErrSen
        *         Otherwise, some system-wide error code.
        * Method leaves with some system-wide code, if message cannot be
        * processed by the hostlet implementation without some major fault.
        */
        virtual TInt ProcessRequestFromConsumerL(const TDesC8& aMessage,
                                                 const TInt aTxnId,
                                                 MSenRemoteServiceConsumer& aConsumer) = 0; 


        // Call-back which tells when HostletConnection's Respond (ASYNC OP) has been
        // completed and with what code (error code or KErrNone)
        virtual TInt OnServiceCompleteL(const TInt aTxnId, TInt aCompletionCode, const TDesC8& aDesc = KNullDesC8) = 0; 
        virtual void TransferProgressForHostlet(TInt aTxnId, TBool aIncoming,
                TBool aIsSoap, const TDesC8& aSoapOrCid, TInt aProgress) = 0;
    };
#endif // M_SEN_SERVICE_SESSION
