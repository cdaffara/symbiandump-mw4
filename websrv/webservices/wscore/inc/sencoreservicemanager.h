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











#ifndef SEN_CORE_SERVICE_MANAGER_H
#define SEN_CORE_SERVICE_MANAGER_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <f32file.h>
#include <flogger.h>
#include <etelmm.h>
#include <tconvbase64.h>
#include <SenXmlReader.h>

#include "senservicemanagerdefines.h"
#include "msencoreservicemanager.h"
#include "senxmldao.h"
#include "msentransport.h"

// CONSTANTS
#ifdef EKA2 

// Server's policy here

// Total number of ranges
const TUint KCoreServiceManagerRangeCount = 11;	//CodeScannerWarnings

// Definition of the ranges of IPC numbers
// NOTE: ranges MUST start from request number 0  (!)
const TInt coreServiceManagerRanges[KCoreServiceManagerRangeCount] = 
        {
        0,  // ESenServInitialize,
        1,  // ESenServTransaction,
        2,  // ESenServInstallFramework,
        3,  // ESenServAssociateService,
            // ESenServDissociateService, 
            // ESenServRegisterIdentityProvider,
            // ESenServUnregisterIdentityProvider,
            // ESenServRegisterServiceDescription,
            // ESenServUnregisterServiceDescription,
        9,  // ESenServGetServiceDescriptionsByPattern, 
            // ESenServGetServiceDescriptionsByUri,
        11, // ESenServIsReady,
            // ESenServHasFacet,
            // ESenServServerMessagesOnOff,
        14, // ESenServGetLengthOfServiceDescriptionByUri,
            // ESenServGetLengthOfServiceDescriptionByPattern,
        16, // ESenServRequestServiceDescription,
            // ESenServReceiveServiceDescription,
        18, // ESenServSendMsgAndGetTxnId,
            // ESenServSendSoapMsgAndGetTxnId,
        20, // ESenServStartTransaction,
            // ESenServTransactionCompleted,
            // ESenServCancelSession,
            // ESenServSetTransportProperties,
            // ESenServTransportProperties
            // ESenServCreateHostletConnection, 
            // ESenServWaitForHostletRequest    
            // ESenServProvideHostletResponse   
            // ESenServMoveChunk
            // ESenServMoveFile
            // ESenServSendFileHandle
            // ESenServGetGredentials
            // ESenServAddCredential
            // ESenServRemoveCredential
            // ESenServCancelRequest
            // ESenServConnectionID
            // ESenServConnectionIdentityProvider
            // ESenObserveTransfer
            // ESenObserveAuthCallback
            // ESenTransferProgress
        	// ESenServSearchIdentityProvider
            // ESenServTrafficDetails
	    // ESenObserveCoBranding	
        	// ESenServGetIdentityProviders
#ifdef __ENABLE_ALR__
			// ESenServRegisterMobiltyObserver
			//ESenServMigrateToPreffredCarrier
			//ESenServNewCarrierAccepted
        47
#else
		44
#endif
        }; 

// Policy to implement for each of the above ranges        
const TUint8 coreServiceManagerElementsIndex[KCoreServiceManagerRangeCount] = 
        {
        2,                           //applies to 1st range  <=>  0
        CPolicyServer::EAlwaysPass,  //applies to 2nd range  <=>  1
        1,                           //applies to 3rd range  <=>  2
        3,                           //applies to 4th range  <=>  3 - 8
        0,                           //applies to 5th range  <=>  9 - 10
        CPolicyServer::EAlwaysPass,  //applies to 6th range  <=> 11 - 13
        0,                           //applies to 7th range  <=> 14 - 15
        CPolicyServer::EAlwaysPass,  //applies to 8th range  <=> 16 - 17
        2,                           //applies to 9th range  <=> 18 - 19
        CPolicyServer::EAlwaysPass,  //applies to 10th range <=> 20 - 35
        CPolicyServer::ENotSupported //applies to 11th range (out of range IPC; >35)
        };

// Specific capability checks
const CPolicyServer::TPolicyElement coreServiceManagerElements[] = 
        {
        //policy "0"; fail call if ReadUserData not present
        {_INIT_SECURITY_POLICY_C1(ECapabilityReadUserData), CPolicyServer::EFailClient},  
        //policy "1"; fail call if WriteUserData not present
        {_INIT_SECURITY_POLICY_C1(ECapabilityWriteUserData), CPolicyServer::EFailClient}, 
        //policy "2"; fail call if NetworkServices not present
        {_INIT_SECURITY_POLICY_C1(ECapabilityNetworkServices), CPolicyServer::EFailClient}, 
        //policy "3"; fail call if ReadUserData, WriteUserData not present
        {_INIT_SECURITY_POLICY_C2(ECapabilityReadUserData, ECapabilityWriteUserData), CPolicyServer::EFailClient}
        };

// Package all the above together into a policy
const CPolicyServer::TPolicy coreServiceManagerPolicy =
        {
        CPolicyServer::EAlwaysPass,         // specifies all connect attempts should pass 
        KCoreServiceManagerRangeCount,       // number of ranges                                   
        coreServiceManagerRanges,           // ranges array
        coreServiceManagerElementsIndex,    // elements<->ranges index
        coreServiceManagerElements,         // array of elements
        };

#endif //EKA2

// FORWARD DECLARATIONS
class CSenGuidGen;
class CSenBaseIdentityManager;
class CSenCredentialManager;
class CSenServerContext;
class CSenCoreShutdownTimer;

// CLASS DECLARATION

#ifdef EKA2  
class CSenCoreServiceManager : public CPolicyServer,
                               public MSenCoreServiceManager
#else
class CSenCoreServiceManager : public CServer2,
                               public MSenCoreServiceManager
#endif
    {
    public: 
        
        // Constructors
        static CSenCoreServiceManager* NewL();
        static CSenCoreServiceManager* NewLC();

        // C++ destructor
        virtual ~CSenCoreServiceManager();


        // From MSenIdentityManager:

        virtual MSenIdentity& IdentityL();
        virtual TInt UserNameL(HBufC8*& aUserName);
        virtual CSenIdentityProvider* IdentityProviderL();
        virtual CSenIdentityProvider* IdentityProviderL(const TDesC8& aURI);
        virtual CSenIdentityProvider* IdentityProviderL(const CDesC8Array& aIdpList,
                                                        TBool aStrict);

        virtual CSenIdentityProvider* IdentityProviderL(MSenServiceDescription& aSD);

        virtual CSenIdentityProvider* IdentityProviderL(MSenServiceDescription& aSD,
                                                        const CDesC8Array& aIdpList,
                                                        TBool aStrict);
        virtual const RPointerArray<CSenIdentityProvider>& IdentityProvidersL();

        virtual TInt RegisterIdentityProviderL(CSenIdentityProvider* apIdp);
        virtual TInt UnregisterIdentityProviderL(CSenIdentityProvider& aIdp);
        virtual TBool AssociateServiceL(const TDesC8& aServiceID, const TDesC8& aProviderID);
        virtual TBool DissociateServiceL(const TDesC8& aServiceID, const TDesC8& aProviderID);
        virtual TInt IdentitiesL(CDesC8Array& aIdentitiesList);
        virtual TInt AuthenticationForL(CSenIdentityProvider& aAccount, TPckgBuf<TSenAuthentication>& aResponse);
        virtual TPtrC8 SenSecurityMechanismNames8L();
        virtual void SetShowPasswordDialog(const TBool aState);
        virtual CSenSecurityMechanism* MechanismNamedL(const TDesC8& aName);

        // From MSenServiceManager
        virtual TInt InitServiceConnectionL(MSenRemoteServiceConsumer& aServiceConsumer,
                                            CSenWSDescription& aPattern,
                                            HBufC8*& aErrorMsg);

        virtual TInt ServiceDescriptionsL(RWSDescriptionArray& aMatches,
                                          const TDesC8& aContract);

        virtual TInt ServiceDescriptionsL(RWSDescriptionArray& aMatches,
                                          MSenServiceDescription& aPattern);

        virtual TInt RegisterServiceDescriptionL(CSenWSDescription* apSD);
        virtual TInt UnregisterServiceDescriptionL(MSenServiceDescription& aSD);
        virtual CSenBaseFragment* InstallFrameworkL(const TDesC8& aFrameworkId);
        
        virtual TPtrC8 IMSI();
        
        virtual TPtrC8 IMEI();

        virtual CSenXmlReader* XMLReader();
        virtual HBufC8* RandomGuidL(); 
        virtual RFileLogger* Log() const;
        virtual void IncrementConnections();
        virtual void DecrementConnections();


        virtual TInt ContainsServiceDescriptionL(TBool& aContains,
                                                 CSenWSDescription& aPattern);

        virtual TInt ContainsIdentityProviderL(TBool& aContains,
                                               CSenIdentityProvider& aIDP);
                                               
        virtual TInt FindMatchingIdentityProviderL(CSenIdentityProvider &aIdp,
                                                   CSenIdentityProvider*& apMatch);
                                                   
        virtual TInt UpdateIdentityProviderL(CSenIdentityProvider& aIdp);

        virtual TInt NextTransactionId();

        /**
        * Notifies *ALL* framework plug-ins about certain event
        * @return some system-wide error code, if an error occurred
        */
        virtual TInt NotifyFrameworksL(const TInt aEvent);

        /**
        * Notifies *ALL* framework plug-ins about certain event,
        * an additional object pointer may be passed
        * NULL is also accepted
        * @return some system-wide error code, if an error occurred
        */
        virtual TInt NotifyFrameworksL(const TInt aEvent, TAny* aArgument);

        /**
        * Notifies only those frameworks plug-ins, which match
        * with given framework ID (aFrameworkID)
        * If aFrameworkID == KNullDesC8 (zero-length),
        * then *all* known framework plug-ins are notified about the event.
        * @return some system-wide error code, if an error occurred
        */
        virtual TInt NotifyFrameworksL(const TDesC8& aFrameworkID, 
                                       const TInt aEvent);

        /**
        * Notifies only those frameworks plug-ins, which match
        * with given framework ID (aFrameworkID)
        * If aFrameworkID == KNullDesC8 (zero-length),
        * then *all* known framework plug-ins are notified about the event,
        * an additional object pointer may be passed
        * NULL is also accepted
        * @return some system-wide error code, if an error occurred
        */
        virtual TInt NotifyFrameworksL(const TDesC8& aFrameworkID,
                                       const TInt aEvent,
                                       TAny* aArgument);


        virtual TInt SizeOfServiceDescriptionsL(RWSDescriptionArray& aArray);
        
        virtual TInt SizeOfCredentialsL(RSenCredentialArray& aArray);
        virtual TInt SizeOfIdentityProvidersL(const RPointerArray<CSenIdentityProvider>& aArray);

        // From MSenCoreServiceManager
    
        /**
        * Takes the ownership of apServiceDescription
        * @return KErrNone on success
        *         KErrArgument, if aServiceDescription is NULL
        *         KErrSenNoContractNoEndPoint, if both endpoint and
        *            contract are of zero-lenght, making SD invalid
        *         KErrGeneral if duplicate (blocking), service description
        *         with equal primary keys could not be removed.
        */
        virtual TInt AddServiceDescriptionL(CSenWSDescription* apServiceDescription);

        virtual TInt RemoveServiceDescriptionL(CSenWSDescription& aServiceDescription);

        virtual TInt SaveL(const CSenServiceSession& aServiceSession);
        virtual TInt SaveL(const CSIF& aServiceInvocationFramework);

        virtual CDesC8Array& SupportedFrameworksL();

        virtual MSenProvider& LookupHostletForL(const TDesC8& aHostletEndpoint,
                                                const TDesC& aReqThreadId,
                                                const TDesC8& aReqConsumerId);

        virtual TInt ReleaseHostletL(const MSenProvider* aHostlet,
                                     const TDesC& aReqThreadId,
                                     const TDesC8& aReqConsumerId);


        // From MSenTransportFactory:
        virtual CSenTransportBase* CreateL(CSenWSDescription& aInitializer,
                                           CSenServiceSession* apSession);
        
        // from CServer
        
        CSession2* NewSessionL(const TVersion &aVersion,
                               const RMessage2& aMessage) const; 

        static TInt ThreadFunction(TAny* sStarted);

        
        virtual HBufC8* EncodeToBase64LC(const TDesC8& aSource);
        virtual HBufC8* DecodeFromBase64LC(const TDesC8& aSource);

        // New functions
        virtual RStringPool& StringPool();

/*
        // New functions
        virtual TInt AddActiveHostletConnectionEndpointL(const TDesC8& aEndpoint);
        virtual TInt RemoveActiveHostletConnectionEndpoint(const TDesC8& aEndpoint);
*/

        // From MSenCredentialManager
        
        virtual TInt SaveCredentialDB();
        
		virtual TInt CredentialsL(const CSenWSDescription& aPattern,
                                  RSenCredentialArray& aCredentials);

		virtual TInt CredentialsL(const CSenWSDescription& aPattern,
                                  const CSenIdentityProvider& aIdP,
                                  RSenCredentialArray& aCredentials);

		virtual TInt CredentialsL(const CSenWSDescription& aPattern,
                                  RSenCredentialPtrArray& aCredentials);

		virtual TInt CredentialsL(const CSenWSDescription& aPattern,
                                  const CSenIdentityProvider& aIdP,
                                  RSenCredentialPtrArray& aCredentials);

		virtual RSenCredentialPtr AddCredentialL(CSenInternalCredential* apCredential,
                                                 TInt& aErrorTo);

		virtual RSenCredentialPtr AddCredentialL(CSenIdentityProvider* apIdP,
                                                 CSenInternalCredential* apCredential,
	                                             TInt& aErrorTo);

		virtual RSenCredentialPtr AddCredentialL(const TDesC8& aCredential,
                                                 TInt& aErrorTo);

		virtual RSenCredentialPtr AddCredentialL(CSenIdentityProvider* apIdP,
                                                 const TDesC8& aCredential,
                                                 TInt& aErrorTo);

		virtual TInt RemoveCredentialsL(const CSenWSDescription& aPattern);

		virtual TInt RemoveCredentialsL(const CSenWSDescription& aPattern,
										const CSenIdentityProvider& aIdP);
										
        virtual TInt RemoveCredentialsL(const TDesC8& aProviderId);

        virtual TInt RemoveCredentialL(TInt aInternalCredentialId);	//codescannerwarnings
        
        virtual RSenCredentialPtr CredentialL(TInt aInternalCredentialId,
                                              TInt& aErrorTo);

        virtual RSenCredentialPtr UpdateCredentialL(TInt aInternalCredentialId,
                                                    CSenInternalCredential* apCredential,
                                                    TInt& aErrorTo);
        
        virtual RSenCredentialPtr UpdateCredentialL(TInt aInternalCredentialId,
                                                    const TDesC8& aCredential,
                                                    TInt& aErrorTo);

        virtual TInt NextConnectionID();

    protected:  
        
        // From CActive
        TInt RunError(TInt aError);

    private: 
        
        /**
        * C++ default constructor.
        */
        CSenCoreServiceManager(TInt aPriority);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // New functions
        
        MSIF* Framework(const TDesC8& aFrameworkID);
        TBase64& Base64Codec();

//        RPointerArray<HBufC8>& ActiveHostletEndpointsL();

    private: 
        
        // CS framework:
        static void PanicClient(const RMessage2& aMessage,
                                TWsfServPanic aReason);

        static void PanicServer(TWsfServPanic aPanic);
        static void ThreadFunctionL();
        void UpdateTouchL(CSenServiceSession* aSession);
        
        void GetImsiImeiL();


    private: // Data
        CSenXMLDAO*              iDAO;                      // owned
        CSenBaseIdentityManager* iIdentityManager;          // owned
        RFileLogger              iLog;                      // owned
        TInt                     iConnectionCount;
        CSenGuidGen*             iGuidGenerator;            // owned
        CSenXmlReader*           iReader;                   // owned
        TBase64 iBase64Codec; 
        RStringPool iStringPool;                            // owned
//        RPointerArray<HBufC8>*   ipActiveHostletEndpoints;  // owned

        TInt iNextTransactionId;

        CSenCredentialManager*   iCredentialManager;        // owned
        
        CSenTransportBase* ipVtcpTransport; // not owned
//        HBufC8*        ipVtcpEndpoint;  // owned

        TInt                     iConnectionID;
        
        CSenServerContext* ipServerContext; // owned
        CSenCoreShutdownTimer* iShutdownTimer;
        TBuf8<15> iIMSI;
        TBuf8<50> iIMEI;
    };

#endif // SEN_CORE_SERVICE_MANAGER_H

// End of File
