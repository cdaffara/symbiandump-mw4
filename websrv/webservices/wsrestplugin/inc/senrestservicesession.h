/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef SEN_REST_SERVICE_SESSION_H
#define SEN_REST_SERVICE_SESSION_H

//  INCLUDES
#include "senwebservicesession.h"

// FORWARD DECLARATIONS
class MSenServiceConsumer;
class MServiceDescription;
class CSenCredential;
class CSenSoapMessage;

// CONSTANTS
_LIT8(KConsumerIdLocalname, "ConsumerID");

// CLASS DECLARATION
class CSenRestServiceSession : public CSenWebServiceSession
    {
    public:
        /**
        * Constructors and destructor
        */
        static CSenRestServiceSession* NewL(MSIF& aFramework);
        static CSenRestServiceSession* NewLC(MSIF& aFramework);
        virtual ~CSenRestServiceSession();


        // Functions from base classes:

        // From WebServiceSession
        TInt ComputeStatusL();         // In REST service session: nothing to do

        virtual TInt CompleteServerMessagesOnOffL(const TBool& aCompleteOnOff);

        // Complete superclass implementations:
        virtual TInt SubmitL(const TDesC8& aMessage, 
                             const TDesC8& aTransportProperties,
                             MSenRemoteServiceConsumer& aConsumer,
                             HBufC8*& aResponse);

        virtual TInt SendL(const TDesC8& aMessage, 
                           const TDesC8& aTransportProperties,
                           MSenRemoteServiceConsumer& aConsumer,
                           TInt& aTxnId,
                           HBufC8*& aRevalidationError);
        
        virtual TInt SubmitSoapL(const TDesC8& aMessage, 
                                 const TDesC8& aTransportProperties,
                                 MSenRemoteServiceConsumer& aConsumer,
                                 HBufC8*& aResponse);

        virtual TInt SendSoapL( const TDesC8& aMessage, 
                                const TDesC8& aTransportProperties,
                                MSenRemoteServiceConsumer& aConsumer,
                                TInt& aTxnId,
                                HBufC8*& aRevalidationError );

        TInt SendToConsumerL(HBufC8* apMessage,
                             const TInt aTxnId,
                             MSenRemoteServiceConsumer& aConsumer,
                             MSenProperties* aResponseTransportProperties = NULL); 

        TInt SendErrorToConsumerL(const TInt aErrorCode,
                                  HBufC8* apError,
                                  const TInt aTxnId,
                                  MSenRemoteServiceConsumer& aConsumer,
                                  MSenProperties* aResponseTransportProperties = NULL); 

        TInt HandleErrorL(const TInt aErrorCode,
                          HBufC8* apError,
                          const TInt aTxnId,
                          MSenRemoteServiceConsumer& aConsumer,
                          MSenProperties* aResponseTransportProperties = NULL); 


        /**
        * @see CSenWSDescription
        */
        virtual TBool HasSuperClass( TDescriptionClassType aType );

    protected:
        CSenRestServiceSession(TDescriptionClassType aType, MSIF& aFramework);
        void ConstructL();
        
    private:
        // NOTE: this is temporary method which MUST be removed when message context is made available in SendL/SubmitL                                
        TInt SetFrameworkPropertiesL(MSenTransport& aTransport); // Temporary
    };



#endif // SEN_REST_SERVICE_SESSION_H

// End of File
