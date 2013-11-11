/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef SEN_BASIC_WEB_SERVICE_SESSION_H
#define SEN_BASIC_WEB_SERVICE_SESSION_H

//  INCLUDES
#include "senwebservicesession.h"
#include "SenHttpTransportProperties.h"

// FORWARD DECLARATIONS
class MSenServiceConsumer;
class MServiceDescription;
class CSenCredential;
class CSenSoapMessage;

// CONSTANTS
_LIT8(KConsumerIdLocalname, "ConsumerID");

// CLASS DECLARATION
class CSenBasicWebServiceSession : public CSenWebServiceSession
    {
    public: // Constructors and destructor
        static CSenBasicWebServiceSession* NewL(MSIF& aFramework);
        static CSenBasicWebServiceSession* NewLC(MSIF& aFramework);

        virtual ~CSenBasicWebServiceSession();

        virtual TInt InitializeFromL(MSenServiceDescription& aServiceDescription);
        
        virtual TInt SubmitL(const TDesC8& aMessage, 
                             const TDesC8& aTransportProperties,
                             MSenRemoteServiceConsumer& aConsumer,
                             HBufC8*& aResponse);

        virtual TInt SendL( const TDesC8& aMessage, 
                            const TDesC8& aTransportProperties,
                            MSenRemoteServiceConsumer& aConsumer,
                            TInt& aTxnId,
                            HBufC8*& aRevalidationError );
        
        virtual TInt SubmitSoapL(const TDesC8& aMessage, 
                                 const TDesC8& aTransportProperties,
                                 MSenRemoteServiceConsumer& aConsumer,
                                 HBufC8*& aResponse);

        virtual TInt SendSoapL( const TDesC8& aMessage, 
                                const TDesC8& aTransportProperties,
                                MSenRemoteServiceConsumer& aConsumer,
                                TInt& aTxnId,
                                HBufC8*& /*aRevalidationError */ );
		virtual TInt HandleSoapFaultL(CSenSoapMessage* apSOAPMessage,
                                            const TInt aErrorCode,
                                            const TInt aTxnId,
                                            MSenRemoteServiceConsumer& aConsumer,
                                            MSenProperties* aResponseTransportProperties) ;
	 	virtual TInt HandleSoapFaultL(CSenSoapMessage* apSOAPMessage,
                                            HBufC8*& aResponse);

        virtual TInt ComputeStatusL();
        /**
        * @see CSenWSDescription
        */
        virtual TBool HasSuperClass( TDescriptionClassType aType );

    protected:
        CSenBasicWebServiceSession(TDescriptionClassType aType, MSIF& aFramework);
        void ConstructL();
        
    private:
        // NOTE: this is temporary method which MUST be removed when message context is made available in SendL/SubmitL                                
        TInt SetFrameworkPropertiesL(MSenTransport& aTransport);                                
        
        // Helper method, which allows sending of "unwrapped" descriptor (UTF-8 data)
        // in case of HTTP GET or DELETE. Otherwise, for example http query string
        // would be illegally "wrapped" inside SOAP body(!).
        TInt HttpMethodFromPropertiesL( const TDesC8& aProperties, CSenHttpTransportProperties::TSenHttpMethod& aMethod );
    };

#endif // SEN_BASIC_WEB_SERVICE_SESSION_H

// End of File
