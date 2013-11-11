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








#ifndef IDWSF_SERVICE_SESSION_H
#define IDWSF_SERVICE_SESSION_H

// INCLUDES
#include "msenremoteserviceconsumer.h"
#include "msenserviceinvocationframework.h"
#include "midwsfsessionvalidator.h"
#include "senidwsfplugin.h"
#include "senwebservicesession.h"
#include "idwsfserviceprovider.h"
#include "idwsfserviceinstance.h"
#include "idwsfsessionconsumer.h"
#include "idwsfsiuheader.h"
#include "idwsfmessage.h"

// CONSTANTS
_LIT8(KSecNullNull,     "urn:liberty:security:2003-08:null:null"); // IOP
_LIT8(KSecNullBearer,   "urn:liberty:security:2004-04:null:Bearer");
_LIT8(KSecTlsNull,      "urn:liberty:security:2003-08:TLS:null"); // IOP
_LIT8(KSecTlsBearer,    "urn:liberty:security:2004-04:TLS:Bearer");

// ID-WSF 1.1 namespaces
_LIT8(KSecNullBearer11, "urn:liberty:security:2005-02:null:Bearer");
_LIT8(KSecTlsBearer11,  "urn:liberty:security:2005-02:TLS:Bearer");

// FORWARD DECLARATIONS
class CIdWsfSessionConsumer;
class CIdWsfServiceInstance;
class MIdWsfSessionValidator;

// CLASS DECLARATION
class CIdWsfServiceSession : public CSenWebServiceSession
    {
    public: // Constructors and destructor
    
        static CIdWsfServiceSession* NewL(MSIF& aFramework);
        static CIdWsfServiceSession* NewLC(MSIF& aFramework);

        virtual ~CIdWsfServiceSession();

        // New functions
        
        virtual TBool Matches(MSenServiceDescription& aPattern);
        virtual TInt ConstructSecurityTokenL(const TDesC& aToken,
                                             HBufC8*& aSecurityToken);

        virtual TInt InitializeFromL(CIdWsfServiceInstance& aServiceInstance);
        virtual TBool IsSAMLL(const TDesC8& aSecMechURI);
        virtual TBool IsBearerL(const TDesC8& aSecMechURI);
        TPtrC8 ProviderId();
        TPtrC8 ServiceType();
        TInt UpdateFromL(CIdWsfSiuHeader& aHeader);
        void SetProviderL(CIdWsfServiceProvider *aProvider);
        CIdWsfServiceProvider* Provider();
        void SetResourceIdL(const TDesC8& aURI);
        TPtrC8 ResourceId();
        void SetServiceTypeL(const TDesC8& aServiceType);
        void SetTrustAnchorL(const TDesC8& aProviderId);
        virtual TInt CanHandleErrorL(CIdWsfMessage& aSOAPMessage);
    
        /**
        * Read the facets of the given pattern ServiceDescription.
        * Take on those facets that we understand.
        * @param pattern
        */
        virtual void CopyFacetsFromL(MSenServiceDescription& aPattern);
    
        void SetValidator(MIdWsfSessionValidator* aValidator);
        MIdWsfSessionValidator* Validator();

        // Functions from base classes
        
        // From SenServiceSession
        virtual MSenRemoteServiceConsumer* RemoteConsumerL(
                                                    const TDesC8& aSenderID);
        virtual void StartTransaction();
        virtual void TransactionCompleted();
        virtual TInt SetStatusL();

        // From SenWebServiceSession
        virtual TInt RemoveConsumerL(MSenRemoteServiceConsumer& aConsumer);
        virtual TInt AddConsumerL(MSenRemoteServiceConsumer& aConsumer);
        virtual TPtrC8 FrameworkId();
        virtual TInt MessageForSendingL(const TDesC8& aBody,
                                        const TDesC8& aSenderID,
                                        CSenSoapMessage*& aMessage);

        virtual TInt NewMessageL(CSenSoapMessage*& aMessage);
        virtual TInt InitializeFromL(MSenServiceDescription& aServiceDescription);
        virtual TInt ParseMessageL(CSenSoapMessage& aSOAPMessage);

        virtual TInt SendSoapMessageToConsumerL( CSenSoapMessage* apMessage,
                                                 const TInt aTxnId,
                                                 MSenRemoteServiceConsumer& aConsumer,
                                                 MSenProperties* aResponseTransportProperties );

        virtual void SetEndPointL(const TDesC8& aURI);
        virtual void SetFrameworkHeadersL(CSenSoapMessage& aMsg);
        virtual TPtrC8 TrustAnchor();
        
        virtual TInt SendL( const TDesC8& aMessage, 
                            const TDesC8& aTransportProperties,
                            MSenRemoteServiceConsumer& aConsumer,
                            TInt& aTxnId,
                            HBufC8*& aRevalidationError );
        
        virtual TInt SubmitL( const TDesC8& aMessage, 
                              const TDesC8& aTransportProperties,
                              MSenRemoteServiceConsumer& aConsumer,
                              HBufC8*& aResponse );
                              
        TInt SendSoapL( const TDesC8& aSoapMessage, 
                        const TDesC8& aTransportProperties,
                        MSenRemoteServiceConsumer& aConsumer,
                        TInt& aTxnId,
                        HBufC8*& /*aRevalidationError*/ );
      
                              
        TInt SubmitSoapL( const TDesC8& aSoapMessage, 
                          const TDesC8& aTransportProperties,
                          MSenRemoteServiceConsumer& aConsumer,
                          HBufC8*& aResponse );

        /**
        * Method checks if ID-WSF framework can handle this fault, if not, then
        * error is delivered to session consumer
        *
        * @param apSoapMessage is the SOAP message representing fault.
        * @param aResponse is where either SOAP fault OR - if ID-WSF
        *        framework was able to handle this fault (recover)
        *        - the valid response from the service, achieved
        *        by re-submitted request (for example to new endpoint,
        *        if the handled fault was "endpoint moved" -type fault.
        *        
        * 
        * @return KErrSenSoapFault, if this error (fault) could not be handled.
        *         In this case, aResponse contains (extracted) SOAP fault
        *         element.
        *         KErrNone, if recovery was possible and successful, and
        *                   then aResponse contains valid response from 
        *                   the service. The SOAP fault, which was taken
        *                   care here - between (remote) service and ID-WSF 
        *                   framework - remains thus "hidden" from actual 
        *                   web service consumer.
        *         KErrSenSoapFault, if fault could not be handled. Now
        *                   aResponse will contain the extracted SOAP
        *                   fault element as UTF-8 descriptor (TDesC8)
        *         KErrSenInternal, the SOAP fault could not be found from
        *                   aSoapMessage.
        *         KErrNotFound - the consumer could not be found.
        */
        virtual TInt HandleSoapFaultL( CSenSoapMessage* apSOAPMessage,
                                       HBufC8*& aResponse );

        /**
        * Method checks if ID-WSF framework can handle this fault, if not, then
        * error is delivered to session consumer
        *
        * @param apSoapMessage is the SOAP message representing fault.
        * @param aErrorCode, is the error code received from transport
        * @param aTxnId is the transaaction id from transport, which 
        *        can be used to map this error response to consumer's 
        *        request.
        * @param aConsumer is the consumer which was calling the transport
        *        and originally sent the request.
        * @param aResponseTransportProperties includes response properties
        *        from transport.
        * @return KErrNone, if error was either succesfully handled or
        *         processed (delivered to consumer) The SOAP fault, which was taken care between (remote)
        *         service and ID-WSF framework is hereby "hidden" from
        *         actual web service consumer.
        */
        virtual TInt HandleSoapFaultL(CSenSoapMessage* apSOAPMessage,
                                      const TInt aErrorCode,
                                      const TInt aTxnId,
                                      MSenRemoteServiceConsumer& aConsumer,
                                      MSenProperties* aResponseTransportProperties);

        virtual void WriteExtensionsAsXMLToL(RWriteStream& aWriteStream);

        /**
        * @see CSenWSDescription
        */
        virtual TBool HasSuperClass( TDescriptionClassType aType );
        
        virtual void FillCredentialIdentifierL(CSenCredentialIdentifier& aIdentifier);

    private: 
        
        /**
        * C++ default constructor.
        */
        CIdWsfServiceSession(TDescriptionClassType aType, MSIF& aFramework);
    
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        // New functions
        
        TInt SessionConsumerL(  CIdWsfMessage& aMessage,
                                CIdWsfSessionConsumer*& aSessionConsumer);
        
        TInt SessionConsumerL(  const TDesC8& aSenderID,
                                CIdWsfSessionConsumer*& aSessionConsumer);
                                
        // NOTE: this is temporary method which MUST be removed when message context is made available in SendL/SubmitL                                
        TInt SetFrameworkPropertiesL(MSenTransport& aTransport);                                
    
        void SetSenSecurityMechanismL(const TDesC8& aSenSecurityMechanism);
        TPtrC8 SenSecurityMechanism();
        TInt SetOptionsFromL(MSenServiceDescription& aServiceDescription);

    private: // Data
        CIdWsfServiceProvider*  iProvider;
        HBufC8* ipResourceId;
        HBufC8* ipTrustAnchor;
        HBufC8* ipReceivedMessageId;
        HBufC8* ipSenSecurityMechanism;
        //no need to store received message IDs;
        TBool  iMessageThread; 
        MIdWsfSessionValidator* ipValidator; // not owned
        TInt iSubmitState;
    };
    
#endif // IDWSF_SERVICE_SESSION_H

// End of File

