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








#ifndef SEN_SERVICE_CONNECTION_IMPL_H
#define SEN_SERVICE_CONNECTION_IMPL_H

// INCLUDES
#include <flogger.h>

#include <SenXmlReader.h>
#include <MSenServiceDescription.h>

#include "SenServiceManager.h"
#include "rsenservicemanager.h"
#include "senservicemanagerinterfaces.h"

// FORWARD DECLARATIONS
class MSenIdentityManager;
class CSenGuidGen;

// CLASS DECLARATION
class CSenServiceManagerImpl : public CSenServiceManager, MSenInternalServiceManager
    {
    public: // Constructors and destructor

        static CSenServiceManagerImpl* NewL();
        static CSenServiceManagerImpl* NewLC();
        static CSenServiceManagerImpl* NewL(MSenAuthenticationProvider& aAuthProvider);
        static CSenServiceManagerImpl* NewLC(MSenAuthenticationProvider& aAuthProvider);

        virtual ~CSenServiceManagerImpl();

        // New functions

        /**
        * Public API to Core Service Manager
        */
        virtual TInt ServiceDescriptionsL(  const TDesC8& aUri,
                                            RServiceDescriptionArray& aList);

        virtual TInt ServiceDescriptionsL(  MSenServiceDescription& aSD,
                                            RServiceDescriptionArray& aList);
    
        virtual TInt RegisterServiceDescriptionL(MSenServiceDescription& aSD);

        virtual TInt UnregisterServiceDescriptionL(MSenServiceDescription& aSD);

        /**
        * Public API to Identity Manager
        */
        virtual TInt RegisterIdentityProviderL(CSenIdentityProvider& aProvider);
        virtual TInt UnregisterIdentityProviderL(CSenIdentityProvider& aProvider);
        virtual TInt AssociateServiceL( const TDesC8& aServiceID,
                                        const TDesC8& aProviderID);

        virtual TInt DissociateServiceL(const TDesC8& aServiceID,
                                        const TDesC8& aProviderID);
                                        
        /**
        * Currently: available through private header only
        * Install a ServiceInvocationFramework. The WSF checks if some
        * custom ECOM exists and adds an entry (Framework element) into
        * sensessions.xml database.
        * @param aFrameworkId is the "cue" of the ECOM plug-in being installed
        * (existing in ECOM default dir)
        * @return status/error code.
        * Status codes:
        * KErrNone                      ok
        * Error codes:
        * KErrArgument                  The given framework ID was zero length or
        *                               an invalid descriptor.
        * KErrNotFound                  Framework with the given ID was not found
        *                               and couldn't be installed.
        */
        virtual TInt InstallFrameworkL(const TDesC8& aFrameworkId);


        TInt ExtractCredentialsL(TPtrC8& aCredentials, RCredentialArray& aList,
                                 RCredentialPropertiesArray& aCredentialPropertiesList);
                                        
        virtual TInt CredentialsL(const TDesC8& aEndpoint,
                                  RCredentialArray& aCredentials,
                                  RCredentialPropertiesArray& aCredentialProperties);
        
        virtual TInt CredentialsL(const TDesC8& aEndpoint,
                                  const CSenIdentityProvider& aIdP,
                                  RCredentialArray& aCredentials,
                                  RCredentialPropertiesArray& aCredentialProperties);
        
        virtual TInt CredentialsL(const TDesC8& aEndpoint,
                                  const TDesC8& aUserName,
                                  const TDesC8& aPassword,
                                  RCredentialArray& aCredentials,
                                  RCredentialPropertiesArray& aCredentialProperties);

        virtual TInt CredentialsL(const MSenServiceDescription& aPattern,
                                  RCredentialArray& aCredentials,
                                  RCredentialPropertiesArray& aCredentialProperties);
        
        virtual TInt CredentialsL(const MSenServiceDescription& aPattern,
                                  const CSenIdentityProvider& aIdP,
                                  RCredentialArray& aCredentials,
                                  RCredentialPropertiesArray& aCredentialProperties);
                                  
        virtual TInt CredentialsL(const MSenServiceDescription& aPattern,
                                  const TDesC8& aUserName,
                                  const TDesC8& aPassword,
                                  RCredentialArray& aCredentials,
                                  RCredentialPropertiesArray& aCredentialProperties);
                                  
        virtual TInt AddCredentialL(const TDesC8& aEndpoint,
                                    const CSenIdentityProvider& aIdP,
                                    const CSenCredential2& aCredential);

        virtual TInt AddCredentialL(const MSenServiceDescription& aPattern,
                                    const CSenIdentityProvider& aIdP,
                                    const CSenCredential2& aCredential);
                                    
        virtual TInt AddCredentialL(const TDesC8& aEndpoint,
                                    const CSenIdentityProvider& aIdP,
                                    const CSenCredential2& aCredential,
                                    const CSenXmlProperties& aCredentialProperties);
                                        
        virtual TInt AddCredentialL(const MSenServiceDescription& aPattern,
                                    const CSenIdentityProvider& aIdP,
                                    const CSenCredential2& aCredential,
                                    const CSenXmlProperties& aCredentialProperties);

        virtual TInt RemoveCredentialsL(const MSenServiceDescription& aPattern,
                                        const CSenIdentityProvider& aIdP);

        virtual TInt RemoveCredentialsL(const MSenServiceDescription& aPattern,
                                        const TDesC8& aUserName,
                                        const TDesC8& aPassword);

        virtual TInt RemoveCredentialsL(const CSenIdentityProvider& aIdP);                                        
                                        
        virtual TInt Identifier();      
        
        virtual TInt IdentityProvidersL( RIdentityProviderArray& aList );
        
        virtual TAny* InterfaceByUid( TUid aUID );

                                        
    protected: // Functions from base classes
        
        // from CActive
        void RunL();
        void DoCancel();

    private:
    
        /**
        * C++ default constructor.
        */
        CSenServiceManagerImpl(MSenAuthenticationProvider* apAuthProvider);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // New functions
            
        //RFileLogger* Log() const;

        /**
        * @return KErrNone if successful, and at least one service
        *         description was extracted. 
        *         KErrNotFound, if aSDs is a zero-length descriptor,
        *         or not a single service description could be parsed.
        */
        TInt ExtractServiceDescriptionsL(TPtrC8& aSDs,
                                          RServiceDescriptionArray& aList);
        TInt ExtractIdentiyProvidersL(TPtrC8& aIdPs, 
                                          RIdentityProviderArray& aList);
        TPtrC NextChunkNameL();	//Codescannerwarnings

    private: // Data
        //RFileLogger iLog;
        RSenServiceManager iConnection;
        CSenXmlReader* iReader;
        TPtr8 iInBuffer;
        HBufC8* iOutBuf;
        TPtr8 iOutBuffer;
        TBool iInitialized;
        CSenGuidGen* ipGenerator;
        HBufC* ipChunkName;
        TInt iChunkNameNumber;
        MSenAuthenticationProvider* ipAuthProvider;
        TInt iConnectionID;
        //TInt iTlsLogChannel;
        
    };

#endif //SEN_SERVICE_CONNECTION_IMPL_H

// End of File



