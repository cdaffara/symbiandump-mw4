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








#ifndef SEN_WEB_SERVICE_SESSION_H
#define SEN_WEB_SERVICE_SESSION_H

//  INCLUDES
#include "senservicesession.h"
#include "seninternalcredential.h"

// FORWARD DECLARATIONS
class MSenServiceConsumer;
class MServiceDescription;
class CSenCredential;
class CSenSoapMessage;

// CLASS DECLARATION
class CSenWebServiceSession : public CSenServiceSession,
                              public MSenCredentialObserver
    {
    public: // Destructor
        IMPORT_C virtual ~CSenWebServiceSession();

        // New functions
        IMPORT_C virtual TInt InitializeFromL(MSenServiceDescription& aServiceDescription);

        /**
        * Computes the status of this session (checks the credentials).
        * @return the status code, one of the following:
        * 
        */
        IMPORT_C virtual TInt ComputeStatusL();

        /*
        * Computes the status of this session and returns the status.
        * @return ETrue, if this session is expired and has invalid credentials
        *         EFalse, if this session is is ready and has valid credentials
        */
        IMPORT_C virtual TBool IsExpiredL(); // Remove, not called from anywhere

        IMPORT_C virtual TInt AddCredentialL(CSenCredential& aCredential);
        IMPORT_C virtual TInt ConstructSecurityTokenL(CSenCredential& aCredential,
                                                      HBufC8*& aToken);

        IMPORT_C virtual TInt NewMessageL(CSenSoapMessage*& aMessage);
        IMPORT_C virtual TInt MessageForSendingL(const TDesC8& aBody,
                                                 const TDesC8& aSenderID,
                                                 CSenSoapMessage*& aMessage);

        IMPORT_C virtual TInt ParseResponseL(const TDesC8& aResponse,
                                             HBufC8*& aParsed);


        IMPORT_C virtual CSenXmlReader* XmlReader();
        IMPORT_C virtual TInt ParseResponseL(const TDesC8& aResponse,
                                         CSenSoapMessage*& aMessage);

        IMPORT_C virtual TInt ParseMessageL(CSenSoapMessage& aSOAPMessage);

        /**
        * Adds a consumer. Takes ownership!
        * @return KErrNone if no errors, otherwise some system-wide errorcode
        */
        IMPORT_C virtual TInt AddConsumerL(MSenRemoteServiceConsumer& aConsumer);
        IMPORT_C virtual MSenRemoteServiceConsumer* Consumer();

        /**
        * Return an array in which the consumers (clients) of this ServiceSession
        * reside. This method is called from within several other methods of the
        * abstract ServiceSession class.
        * This method should be implemented by a subclass
        * specific to a ServiceInvocationFramework implementation.
        * @param    aConsumers  List of the current consumers. Owned by
        *           ServiceSession.
        * Ownership of the aConsumers is NOT transfered to the caller
        * @return TInt - a KErrNone or error
        */
        IMPORT_C virtual TInt Consumers(RServiceConsumerArray& aConsumers) const;

        IMPORT_C virtual TInt SubmitL(const TDesC8& aMessage,
                                      const TDesC8& aTransportProperties,
                                      MSenRemoteServiceConsumer& aConsumer,
                                      HBufC8*& aResponse);

        IMPORT_C virtual TInt SendL( const TDesC8& aMessage, 
                                     const TDesC8& aTransportProperties,
                                     MSenRemoteServiceConsumer& aConsumer,
                                     TInt& aTxnId,
                                     HBufC8*& aRevalidationError );


        IMPORT_C virtual TInt SubmitSoapL(const TDesC8& aSoapMessage, 
                                          const TDesC8& aTransportProperties,
                                          MSenRemoteServiceConsumer& aConsumer,
                                          HBufC8*& aResponse);

        IMPORT_C virtual TInt SendSoapL( const TDesC8& aSoapMessage, 
                                         const TDesC8& aTransportProperties,
                                         MSenRemoteServiceConsumer& aConsumer,
                                         TInt& aTxnId,
                                         HBufC8*& aRevalidationError );

        /**
        * Method checks if this session is ready to interact with (remote)
        * service. Typically, if service invocation framework requires 
        * credentials, they are checked inside this method.
        * @return boolean indicating if this session is ready (ETrue) or
        *         not (EFalse) to interact with (possibly remote) service.
        */
        IMPORT_C virtual TBool IsReadyL();

        // SOAP processing callback functions:
        IMPORT_C virtual TInt SendSoapMessageToConsumerL( CSenSoapMessage* apMessage,
                                                          const TInt aTxnId,
                                                          MSenRemoteServiceConsumer& aConsumer,
                                                          MSenProperties* aResponseTransportProperties );

        /**
        * This synchronous method (pair for SubmitL) checks if the service 
        * invocation framework (this session) can handle this fault, and 
        * if not, then an attempt is made to delivered this fault to session
        * consumer.
        *
        * @param aSoapMessage is the SOAP message representing fault.
        * @param aResponse is where either SOAP fault OR - if some
        * framework (like ID-WSF) was able to handle this fault (recover)
        *        - the valid response from the service, achieved
        *        by re-submitted request (for example to new endpoint,
        *        if the handled fault was "endpoint moved" -type fault.
        * @param aResponseTransportProperties will contain the properties of
        * handled error (either for fault OR valid response -transaction)
        *        
        * 
        * @return KErrSenSoapFault, if this error (fault) could not be handled.
        *         In this case, aResponse contains (extracted) SOAP fault
        *         element.
        *         KErrNone, if recovery was possible and successful, and
        *                   then aResponse contains valid response from 
        *                   the service. The SOAP fault, which was taken
        *                   care here - between (remote) service and
        *                   framework - remains thus "hidden" from actual 
        *                   web service consumer.
        *         KErrSenSoapFault, if fault could not be handled. Now
        *                   aResponse will contain the extracted SOAP
        *                   fault element as UTF-8 descriptor (TDesC8)
        *         KErrSenInternal, the SOAP fault could not be found from
        *                   aSoapMessage.
        *         KErrNotFound - the consumer could not be found.
        */
        IMPORT_C virtual TInt HandleSoapFaultL(CSenSoapMessage* apSoapMessage,
                                               HBufC8*& aResponse /*,
                                               CSenTransportProperties*& aResponseTransportProperties */);
       
        /**
        * This asynchronous method (pair for SendL) checks if the service 
        * invocation framework (this session) can handle this fault, and 
        * if not, then an attempt is made to delivered this fault to 
        * session consumer.
        *
        * @param aSoapMessage is the SOAP message representing fault.
        * @param aErrorCode, is the error code received from transport
        * @param aTxnId is the transaaction id from transport, which 
        *        can be used to map this error response to consumer's 
        *        request.
        * @param aConsumer is the consumer which was calling the transport
        *        and originally sent the request.
        * @param aResponseTransportProperties includes response properties
        * for the handled error
        * @return KErrNone, if error was either succesfully handled or
        *         processed (delivered to consumer) The SOAP fault, which 
        *         was taken care between (remote) service and ID-WSF framework
        *         is hereby "hidden" from actual web service consumer.
        */
        IMPORT_C virtual TInt HandleSoapFaultL(CSenSoapMessage* apSoapMessage,
                                           const TInt aErrorCode,
                                           const TInt aTxnID,
                                           MSenRemoteServiceConsumer& aConsumer,
                                           MSenProperties* aResponseTransportProperties);


        // From MSenServiceSession:
        IMPORT_C TInt SendToConsumerL(HBufC8* aMessage,
                                      const TInt aTxnId,
                                      MSenRemoteServiceConsumer& aConsumer,
                                      MSenProperties* aResponseTransportProperties = NULL); 

        IMPORT_C TInt SendErrorToConsumerL(const TInt aErrorCode,
                                           HBufC8* apError,
                                           const TInt aTxnId,
                                           MSenRemoteServiceConsumer& aConsumer,
                                           MSenProperties* aResponseTransportProperties = NULL); 

        IMPORT_C TInt HandleErrorL(const TInt aErrorCode,
                                   HBufC8* apError,
                                   const TInt aTxnId,
                                   MSenRemoteServiceConsumer& aConsumer,
                                   MSenProperties* aResponseTransportProperties = NULL); 

        IMPORT_C virtual TInt SendToHostletL(MSenRemoteHostlet& aReceiver,
                                             const TDesC8& aMessage,
                                             const TInt aTxnId,
                                             MSenRemoteServiceConsumer& aFrom,
                                             MSenProperties* aProperties);

        IMPORT_C virtual TInt ProvideHostletResponseL( MSenRemoteHostlet& aProvider,
                                                       const TInt aTxnId,
                                                       const TInt aServiceCode,
                                                       const TDesC8& aRecipientsConsumerId,
                                                       CSenChunk& aMessageChunk );

        IMPORT_C virtual void SetFrameworkHeadersL(CSenSoapMessage& aMsg);
        IMPORT_C virtual TInt SetUserNameL(TDesC8& aUsername);

        IMPORT_C virtual TPtrC8 Endpoint();
        IMPORT_C virtual TPtrC8 Contract();
        IMPORT_C virtual TPtrC8 FrameworkId();
        IMPORT_C virtual TPtrC8 FrameworkVersion();
        IMPORT_C virtual const TTime& ValidUntilL();
        IMPORT_C virtual void SetContractL(const TDesC8& aContract);
        IMPORT_C virtual void SetEndPointL(const TDesC8& aURI);
        IMPORT_C virtual void SetSecurityL(const TDesC8& aSecurity);
        IMPORT_C virtual void SetCredentialPtrL(RSenCredentialPtr aCredentialPtr);	//codescannerwarnings
        IMPORT_C virtual void FillCredentialIdentifierL(CSenCredentialIdentifier& aIdentifier);
        IMPORT_C virtual void StartTransaction();
        IMPORT_C virtual void TransactionCompleted();
        IMPORT_C virtual void WriteAsXMLToL(RWriteStream& aWriteStream);
        IMPORT_C virtual TTimeIntervalMicroSeconds ClientServerInterval();
        IMPORT_C virtual void SetClientServerIntervalL(TTimeIntervalMicroSeconds aInterval);	//codescannerwarnings
        IMPORT_C TBool HasConsumer() const;

        
        // From CSenWSDescription:
        IMPORT_C virtual TBool HasSuperClass( TDescriptionClassType aType );
        IMPORT_C virtual TInt SetTransportCueL(const TDesC8& aCue);
        IMPORT_C virtual TPtrC8 TransportCue();

        IMPORT_C TInt TryToSearchValidCredentialL();	//codescannerwarnings
        
        IMPORT_C virtual void CredentialChanged(TSenCredentialChange aChange, TAny* aPointer);
        
        // from MSenRemoteServiceSession
//        IMPORT_C TInt RemoveConsumerL(MSenRemoteServiceConsumer& aConsumer);

        IMPORT_C TInt GetCredentialIdL();
        
        IMPORT_C TInt AddCredentialObserverL(CSenInternalCredential& aCredential);

    protected:
        
        /**
        * C++ default constructor.
        */
        IMPORT_C CSenWebServiceSession( TDescriptionClassType aType,
                                        MSIF& aFramework);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */                              
        IMPORT_C void ConstructL();
    
        IMPORT_C HBufC8* SecurityL();
        
        IMPORT_C TBool HasSecurity();
        
        void SetCredentialIdL(TInt aCredentialId);	//codescannerwarnings
        
    private:
        TInt HandleBodyWithoutParsingL(CSenSoapMessage& aMessage, const TDesC8& aResponse);
    
    protected: // Data
        TTimeIntervalMicroSeconds iClientServerInterval;

         // Data, which also subclasses can access
        TTime iValidUntil;
        TTime iMaxTime;
        
        RSenCredentialPtr  iCredentialPtr;
        HBufC8* iTransportCue; 

    private: // Data
        HBufC8* iEndpoint;
        HBufC8* iContract;
        HBufC8* iFrameworkId;
       

        // credentials as xml
        HBufC8* iSecurity;
    };

#endif // SEN_WEB_SERVICE_SESSION_H

// End of File
