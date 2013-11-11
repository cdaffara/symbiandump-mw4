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











#ifndef SEN_BASE_IDENTITY_H
#define SEN_BASE_IDENTITY_H

//INCLUDES
#include <SenBaseFragment.h>

#include "msenidentity.h"


// CONSTANTS
_LIT8( KIdentityElementLocalName, "Identity" );
_LIT8( KServiceIDElementLocalName, "ServiceID" );

// FORWARD DECLARATIONS
class CSenIdentityProvider;

//TYPEDEFS
typedef RPointerArray<CSenIdentityProvider> RIdentityProviderList;

// CLASS DECLARATION
class CSenBaseIdentity : public CSenBaseFragment,
                         public MSenIdentity
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CSenBaseIdentity* NewL(TPtrC8 aIMEI);
        IMPORT_C static CSenBaseIdentity* NewLC(TPtrC8 aIMEI);

        /**
        * Destructor
        */
        IMPORT_C ~CSenBaseIdentity();   
        
        // New functions
        
        /**
        * A getter.
        * @param aService the service description
        * @return a pointer to IdentityProvider
        */
        virtual CSenIdentityProvider* IdentityProviderL(
                                            MSenServiceDescription& aService );

        /**
        * A getter.
        * @param aIdentityProviderId an URI of the service (endpoint or contract)
        * @return a pointer to IdentityProvider
        */
        virtual CSenIdentityProvider* IdentityProviderL( const TDesC8& aUri );

        /**
        * A getter.
        * @param aIdpList an array of ProviderIDs
        * @param strict if set to true only an IdentityProvider from the List
        * will be returned, and null if none of the list entries is known to the
        * current active Identity.
        * @return a pointer to IdentityProvider
        */
        virtual CSenIdentityProvider* IdentityProviderL(
                                                const CDesC8Array& aIdpList,
                                                TBool aStrict);

        /**
        * A getter.
        * @param aService the service description
        * @param aIdpList an array of ProviderIDs
        * @param strict if set to true only an IdentityProvider from the List
        * will be returned, and null if none of the list entries is known to the
        * current active Identity.
        * @return a pointer to IdentityProvider
        */
        virtual CSenIdentityProvider* IdentityProviderL(
                                        MSenServiceDescription& aService,
                                        const CDesC8Array& aIdpList,
                                        TBool aStrict);

        /**
        * A getter.
        * @return a pointer to IdentityProvider
        */
        virtual CSenIdentityProvider* IdentityProviderL();

        /**
        * Register an IdentityProvider for the current active Identity.
        * @param aIdp a registered IdentityProvider
        * @return KErrNone or system-wide error code.
        */
        virtual TInt RegisterIdentityProviderL(CSenIdentityProvider* aIdp );
        
        virtual TInt UpdateIdentityProviderL(CSenIdentityProvider& aIdp);

        /**
        * Unregister an IdentityProvider for the current active Identity.
        * @param aIdp an unregistered IdentityProvider
        * @return true if successful.
        */
        virtual TInt UnregisterIdentityProviderL(CSenIdentityProvider& aIdp );

        /**
        * Associate a service to an IdentityProvider.
        * @param aServiceID the contract or endpoint of a service
        * @param aProviderID the id of an IdentityProvider
        * @return true if successful. Failure may be caused by non-existence
        * of the IdentityProvider for the current user.
        */
        virtual TBool AssociateServiceL( const TDesC8& aServiceID,
                                         const TDesC8& aProviderID );

        /**
        * Dissociate a service from an IdentityProvider.
        * @param aServiceID the contract or endpoint of a service
        * @param aProviderID the id of an IdentityProvider
        * @return true if successful. Failure may be caused by non-existence
        * of the IdentityProvider for the current user.
        */
        virtual TBool DissociateServiceL(const TDesC8& aServiceID,
                                         const TDesC8& aProviderID );

        /**
        * Creates the element. Overrides CSenBaseFragment
        * StartElementL-function
        * @param aNsUri name space URI
        * @param aLocalName local name
        * @param aQName cue name
        * @param apAttrs XML attributes
        */
        virtual void StartElementL(const TDesC8& aNsUri,
                                   const TDesC8& aLocalName,
                                   const TDesC8& aQName,
                                   const RAttributeArray& aAttributes);

        void EndElementL(const TDesC8& aNsUri,
                         const TDesC8& aLocalName,
                         const TDesC8& aQName);

        virtual HBufC8* AsXmlL();
        virtual HBufC* AsXmlUnicodeL();
        virtual void WriteAsXMLToL(RWriteStream& aWriteStream);

        /**
        * A Type getter.
        * @return a pointer to the type
        */
        virtual const TDesC8& Type();

        /**
        * A Name getter.
        * @return a pointer to the name
        */
        virtual const TDesC8& Name();

        /**
        * A UserName getter.
        * @return a pointer to the user name
        */
        virtual const TDesC8& UserName();

        /**
        * Checks if Identity already has an identity provider with given data.
        * Existing identity provider can still have more data than the pattern.
        * @param    aContains   will contain the boolean value on return. ETrue if
        *           contains, EFalse otherwise.
        * @param    aIDP    the pattern identity provider.
        * @return KErrNone or system-wide error code.
        */
        virtual TInt ContainsIdentityProviderL( TBool& aContains,
                                                CSenIdentityProvider& aIDP);


        /**
        * Method for finding an identity provider.
        * @param aIdp       Identity provider to be searched
        * @param apMatch    ref-to-pointer to the found identity provider or 
        *                   NULL if not found. Ownership is NOT transferred.
        * @return   KErrNotFound if no match was found.
        *           or otherwise the iProviderList index of found IdP.
        */
        virtual TInt FindMatchingIdentityProviderL(CSenIdentityProvider &aIdp,
                                                   CSenIdentityProvider*& apMatch);

        virtual const RIdentityProviderList& IdentityProvidersL() const;
                                                  
    private: 

        /**
        * C++ default constructor.
        */
        CSenBaseIdentity(TPtrC8 aIMEI);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */void ConstructL();

        // New functions
        
        /**
        * A getter.
        * @param aProviderID the providers ID
        * @return a pointer to IdentityProvider
        */
        CSenIdentityProvider* IdentityProviderNamed( const TDesC8& aProviderID );
        TInt IdentityProviderNamed( const TDesC8& aProviderID,
                                    CSenIdentityProvider*&  apMatch);

#ifdef __CRYPTO_HW__									
        void EncryptPasswordL(const TDesC8& aData, TDes8& aCipherText);
		
		void DecryptPasswordL(const TDesC8& aData, TDes8& aCipherText);

#endif // __CRYPTO_HW__
    private: //Data
        RIdentityProviderList iProviderList;
        CSenIdentityProvider* iProvider;
        TPtrC8 iIMEI;
    };

#endif // SEN_BASE_IDENTITY_H

// End of File
