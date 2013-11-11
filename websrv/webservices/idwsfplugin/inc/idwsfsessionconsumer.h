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








#ifndef IDWSF_SESSION_CONSUMER_H
#define IDWSF_SESSION_CONSUMER_H

// INCLUDES
#include <e32base.h>
#include <e32des8.h>    // HBufC8

#include "msenremoteserviceconsumer.h"
#include "senpointermap.h"

#include "idwsfmessage.h"


// DATA TYPES
typedef RSenPointerMap<TDesC8, HBufC8> RSentMessagesPointerMap;

// FORWARD DECLARATIONS
class CIdWsfMessage;
class RFileLogger;

//class CSenPolicy; // deprecated, not in use

// CLASS DECLARATION
class CIdWsfSessionConsumer : public CBase, public MSenRemoteServiceConsumer
    {
    public: // Constructors and destructor
        
        static CIdWsfSessionConsumer* NewL(MSenRemoteServiceConsumer& aServiceConsumer,
                                           RFileLogger& aLogger);

        static CIdWsfSessionConsumer* NewLC(MSenRemoteServiceConsumer& aServiceConsumer,
                                            RFileLogger& aLogger);

        virtual ~CIdWsfSessionConsumer();

        // New functions

        /**
        * Expects() is called by CIdWsfServiceSession to check
        * whether or not this consumer expects certain response
        */
        virtual TBool Expects(const TDesC8& aRefToMessageID);

        const TDesC8& Id() const;
        TInt SetSessionL(MSenRemoteServiceSession& iServiceSession);

        TInt HandleIdWsfMessageL( CIdWsfMessage* apMessage,
                                  const TInt aTxnId,
                                  MSenProperties* aResponseTransportProperties,
                                  TBool aCompleteMessage );

        TInt HandleMessageL( HBufC8* aMessage,
                             const TInt aTxnId,
                             MSenProperties* aResponseTransportProperties );

        TInt HandleErrorL( HBufC8* aError,
                           const TInt aErrorCode,
                           const TInt aTxnId,
                           MSenProperties* aResponseTransportProperties );

        void SetStatusL(const TInt status);

        TInt MessageForSendingL(const TDesC8& aBody,
                                const TDesC8& aReceivedMessageId,
                                CIdWsfMessage*& aMessage);

        void SetSentMessageIdL(const TDesC8& aSentMessageId);
        TPtrC8 SentMessageId();
        void SetReceivedMessageIdL(const TDesC8& aReceivedMessageId);
        TPtrC8 ReceivedMessageId();
        CSenIdentifier& Identifier() const;
        TDesC8* BodyL(const TDesC8& aMessageID) const;
        MSenTransport& TransportL();
        virtual TInt ChunkByTxnIdL(TInt aTxnId, CSenChunk*& aChunk);    
        virtual void FileProgress(TInt aTxnId, TBool aIncoming, TBool aIsSoap,
                const TDesC8& aSoapOrCid, TInt aProgress);
		virtual void SetDataTrafficDetails( TSenDataTrafficDetails& aDetails) ; 	
    protected:
        RFileLogger* Log() const;

    
    private: 
    
        /**
        * C++ default constructor.
        */
        CIdWsfSessionConsumer(MSenRemoteServiceConsumer& aServiceConsumer,
                              RFileLogger& aLogger);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();



    private: // Data
        MSenRemoteServiceConsumer& iConsumer;
        RFileLogger& iLogger;
        HBufC8* iSentMessageId;     // owned
        HBufC8* iReceivedMessageId; // owned
        RSentMessagesPointerMap iSentMessages; // owned
    };
    
#endif // IDWSF_SESSION_CONSUMER_H

// End of File
