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








#ifndef SEN_SERVICE_SESSION_H
#define SEN_SERVICE_SESSION_H

//  INCLUDES
#include <e32base.h>
#include <e32des8.h>
#include <e32std.h>

#include "senwsdescription.h"
#include <SenFacet.h>

#include "msenremoteservicesession.h"
#include "msenservicesession.h"

#include "msenserviceinvocationframework.h"
#include "MSenServiceConsumer.h"
#include "msenremoteserviceconsumer.h"
#include "msencoreservicemanager.h"

// CONSTANTS
_LIT8(KMessageThread, "urn:com.nokia.serene.facet.message-thread");

// DATA TYPES
typedef RPointerArray<MSenRemoteServiceConsumer> RServiceConsumerArray;

// FORWARD DECLARATIONS
class CSIF;
class CSenTransportBase;
class MSenRemoteHostlet;

// CLASS DECLARATION
class CSenServiceSession :  public CSenWSDescription,
                            public MSenRemoteServiceSession,
                            public MSenServiceSession
    {
    public:
    
        // New functions
        
        IMPORT_C virtual TInt InitializeFromL(MSenServiceDescription& aServiceDescription);

        /**
        * Parse the service session related information out of a message
        * sent by the service. Default is to do nothing.
        * Subclasses should override this.
        * @param    aString containing a message received from a service.
        * @param    String with parsed data
        * @return TInt - a KErrNone or error
        */
        IMPORT_C virtual TInt ParseResponseL(const TDesC8& aInput,
                                             HBufC8*& aOutput);

        /**
         * Ask the core service manager to save this service session into
         * XML DAO database.
         *
         * This convenience method can be called from within other methods of
         * this class or subclass, or by some service invocation framework
         * implementations.
         * Leave codes: Symbian system-wide error codes.
         */
        IMPORT_C virtual void SaveL();

        /**
        * This getter method does not trigger any execution, it just returns
        * the current status of this session.
        * @return  TInt the status of this service session.
        */
        IMPORT_C virtual TInt StatusL();

        /**
        * Compute the status of the ServiceSession. If the status
        * changes to (or from) of "KSenConnectionStatusReady"
        * the service consumers are notified. Internally, method
        * calls ComputeStatusL().
        * @return TInt status code indicating the current status,
        *  which may have changed when this method was executed.
        *  
        */
        IMPORT_C virtual TInt SetStatusL();

        /**
        * Method computes the current status of this service session.
        * Subclasses typically override this. Method does NOT set
        * the internal status variable of this class and so it NEITHER
        * notifies consumer(s) about the computed result.
        * @return integer (TInt), which indicates current, but only 
        * computed status of this service session.
        */
        IMPORT_C virtual TInt ComputeStatusL();
        
        /**
        * Adds a service consumer for this session.
        *
        * This service consumer will be notified of service responses
        * (to it's own queries) and of all status changes.
        * This method needs to be implemented -- often some 
        * service invocation framework specific subclass.
        * @param aConsumer is the service consumer object that exposes 
        * the MSenRemoteServiceConsumer interface. Typically some 
        * Symbian server-side session, which represents "remote"
        * consumer application, separate process behind IPC boundary.
        * @return KErrNone or some system-wide error code
        */
        virtual TInt AddConsumerL(MSenRemoteServiceConsumer& aConsumer) = 0;


        // from MSenRemoteServiceSession
        IMPORT_C TInt RemoveConsumerL(MSenRemoteServiceConsumer& aConsumer);
        
        /**
        * Return a iterator over a list with the consumers (clients) of this
        * ServiceSession.
        * This method is called from within several other methods of the abstract
        * ServiceSession class.
        * This method should be implemented by a subclass
        * specific to a ServiceInvocationFramework implementation.
        * Ownership of the aConsumers is NOT transfered to the caller.
        * @param    aConsumers  List of the current consumers.
        *           Owned by ServiceSession.
        * @return  TInt - a KErrNone or error
        */
        virtual TInt Consumers(RServiceConsumerArray& aConsumers) const = 0;

        // Functions from base classes

        // from MSenRemoteServiceSession

        IMPORT_C virtual TInt SetTransportPropertiesL(const TDesC8& aProperties,
                                                      MSenRemoteServiceConsumer& aConsumer);

        IMPORT_C virtual TInt TransportPropertiesL(HBufC8*& aProperties,
                                                   MSenRemoteServiceConsumer& aConsumer);
                                                      
        IMPORT_C virtual TInt SubmitL(const TDesC8& aMessage, 
                                      const TDesC8& aTransportProperties,
                                      MSenRemoteServiceConsumer& aConsumer,
                                      HBufC8*& aResponse);

        IMPORT_C virtual TInt SendL( const TDesC8& aMessage, 
                                     const TDesC8& aTransportProperties,
                                     MSenRemoteServiceConsumer& aConsumer,
                                     TInt& aTxnId,
                                     HBufC8*& aRevalidationError );

        virtual TInt SubmitSoapL(const TDesC8& aSoapMessage, 
                                 const TDesC8& aTransportProperties,
                                 MSenRemoteServiceConsumer& aConsumer,
                                 HBufC8*& aResponse) = 0;

        virtual TInt SendSoapL( const TDesC8& aSoapMessage, 
                                const TDesC8& aTransportProperties,
                                MSenRemoteServiceConsumer& aConsumer,
                                TInt& aTxnId,
                                HBufC8*& aRevalidationError ) = 0;

        // from MSenServiceDescription
        IMPORT_C virtual TBool Matches(MSenServiceDescription& aPattern);
        IMPORT_C virtual TInt HasFacetL(const TDesC8& aURI, TBool& aHasFacet);
        IMPORT_C virtual TInt FacetValue(TDesC8& aURI, HBufC8*& aValueTo);
        IMPORT_C virtual TInt AddFacetL(const CSenFacet& aFacet);
        IMPORT_C virtual TInt SetFacetL(const CSenFacet& aFacet);
        IMPORT_C virtual TInt RemoveFacet(const TDesC8& aURI);
        IMPORT_C virtual TInt FacetsL(RFacetArray& aFacetArray);
        IMPORT_C virtual TInt ScoreMatchL(MSenServiceDescription& aPattern);

        // from MSenRemoteServiceSession
        IMPORT_C virtual TBool IsReadyL(); // This method equals with isValid() in Java ref-impl
        IMPORT_C virtual TInt CompleteServerMessagesOnOffL(const TBool& aCompleteOnOff);

        // from CSenWSDescription (overrides)
        virtual void SetContractL(const TDesC8& aContract) = 0;
        virtual void SetEndPointL(const TDesC8& aEndPoint) = 0;
        virtual TPtrC8 FrameworkVersion() = 0;

        // implement the MSenRemoteServiceSession:
        IMPORT_C virtual TPtrC8 Endpoint();
        IMPORT_C virtual TPtrC8 Contract();
        IMPORT_C TPtrC8 FrameworkId();

        IMPORT_C virtual MSenRemoteServiceConsumer* RemoteConsumerL(const TDesC8& aSenderID);
        IMPORT_C virtual void WriteExtensionsAsXMLToL(RWriteStream& aWriteStream);

        IMPORT_C MSenServiceDescription::TDescriptionClassType DescriptionClassType();
        IMPORT_C MSenServiceDescription& AsServiceDescription();
        
        IMPORT_C void SetTransportL(CSenTransportBase* aTransport);
        IMPORT_C MSenTransport* Transport() const;
        IMPORT_C MSenTransport& TransportL(MSenRemoteServiceConsumer& aRequestor);

        // Destructor
        IMPORT_C virtual ~CSenServiceSession();


        // From MSenServiceSession:
        virtual TInt SendToConsumerL(HBufC8* apMessage,
                                     const TInt aTxnId,
                                     MSenRemoteServiceConsumer& aConsumer,
                                     MSenProperties* aResponseTransportProperties = NULL) = 0; 

        virtual TInt SendErrorToConsumerL(const TInt aErrorCode,
                                          HBufC8* apError,
                                          const TInt aTxnId,
                                          MSenRemoteServiceConsumer& aConsumer,
                                          MSenProperties* aResponseTransportProperties = NULL) = 0; 


        /**
        * In addition of meeting the MSenServiceSession interface, service session
        * extends this method to be available for the sessions (frameworks) themselves:
        * any session class implementation should override this method in order to 
        * provide a possibility for the service invocation framework to recover from 
        * particular (SOAP and other) errors.
        *
        * @param aErrorCode is the error code identifying this error
        * @param aError is the error message - actual response from service
        * @param aConsumer is the consumer of the service. It could
        * be CSenClientSession or some subclass of CSenInternalConsumer
        * (like in Liberty ID-WSF it could be authentication service client
        * or discovery service client).
        * @param aTxnId is the transaction id of this async responsed error.
        * @param aResponseTransportProperties includes the response properties
        * from transport
        * @return KErrNone if error message was successfully delivered to 
        *         consumer(s). Otherwise, some system-wide error code.
        */
        
        virtual TInt HandleErrorL(const TInt aErrorCode,
                                  HBufC8* apError,
                                  const TInt aTxnId,
                                  MSenRemoteServiceConsumer& aConsumer,
                                  MSenProperties* aResponseTransportProperties = NULL) = 0; 



        // from MSenRemoteHostlet:

        IMPORT_C TInt SetHostletL(MSenRemoteHostlet* aHostlet);
        IMPORT_C MSenRemoteHostlet* Hostlet() const;
        
        // From MSenServiceSession:
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
                                                      
        // From CSenWSDescription:
        IMPORT_C virtual TBool HasSuperClass( TDescriptionClassType aType );
        IMPORT_C virtual TInt SetTransportCueL(const TDesC8& aCue);
        IMPORT_C virtual TPtrC8 TransportCue();
//new
        IMPORT_C virtual TInt RefreshMTL(HBufC8*& aRevalidationError);
    protected: 
    
        /**
        * C++ default constructor.
        */
        IMPORT_C CSenServiceSession(TDescriptionClassType aType, MSIF& aFramework);
        
        /**
        * BaseConstructL is made available for subclasses, since they should call
        * it in their own ConstructL or BaseConstructL method.
        */
        IMPORT_C void BaseConstructL();

        // Filelogging method:
        IMPORT_C RFileLogger* Log() const;
        
        /**
        * Note that this non-virtual method allows subclasses to notify about following
        * *pre-state* kind of status codes ONLY (from SenServiceConnection.h):
        *
        * const TInt KSenConnectionStatusAuthentication       =   3; // pre-state
        * const TInt KSenConnectionStatusDiscovery            =   4; // pre-state
        * const TInt KSenConnectionStatusMetadataExchange     =   5; // pre-state
        *
        * In other words, this method cannot be used in credential state notfications (valid / expired).
        */
        //void SendStatusToConsumersL(const TInt aStatus);
        TInt                    iStatus;

    protected: // Data
        MSIF&                   iFramework;     // Not owned
        RServiceConsumerArray   iConsumerList;  
        MSenRemoteHostlet*      ipHostlet;      // Not owned

    private: // Data
        //TInt                    iStatus;
        RFacetArray             iFacets;        // Owned 
        CSenTransportBase*      ipTransport;     // Owned
        TBool                   iTransportOwned;
    };

#endif // SEN_SERVICE_SESSION_H

// End of File