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








#ifndef M_SEN_IDENTITY_H
#define M_SEN_IDENTITY_H


#include <e32base.h>
#include <e32des8.h>    // HBufC8

#include <SenIdentityProvider.h>
//TYPEDEFS
typedef RPointerArray<CSenIdentityProvider> RIdentityProviderList;

// CLASS DECLARATION
class MSenIdentity
    {
    public: // New functions
    
        /**
        * Getter for name.
        * @return name of the identity
        */
        virtual const TDesC8& Name() = 0;

        /**
        * Getter for user name.
        * @return user name of the identity
        */
        virtual const TDesC8& UserName() = 0;

        /**
        * Getter for type.
        * @return type of the identity
        */
        virtual const TDesC8& Type() = 0;

        /**
        * @return the default IdentityProvider of the Identity.
        */
        virtual CSenIdentityProvider* IdentityProviderL() = 0;

        /**
        * @param aService
        * @return the IdentityProvider that has the ServiceID
        * that best mathces the endpoint or providerID
        * of the given ServiceDescription.
        */
        virtual CSenIdentityProvider* IdentityProviderL(
                                    MSenServiceDescription& aService) = 0;
        /**
        * @param aURI
        * @return the IdentityProvider that has the ServiceID
        * that best mathces the given URI.
        */
        virtual CSenIdentityProvider* IdentityProviderL( const TDesC8& aURI ) = 0;

        /**
        * @param aIdpList an array of ProviderIDs
        * @param strict if set to true only an IdentityProvider from the List
        * will be returned, and null if none of the list entries is known to the
        * current active Identity.
        * @return the currently preferred IdentityProvider
        * from within the given List with ProviderIDs.
        */
        virtual CSenIdentityProvider* IdentityProviderL(
                                                const CDesC8Array& aIdpList,
                                                TBool aStrict) = 0;
        /**
        * @param aService
        * @param aIdpList an array of ProviderIDs
        * @param strict if set to true only an IdentityProvider from the List
        * will be returned, and null if none of the list entries is known to the
        * current active Identity.
        * @return the currently preferred IdentityProvider
        * from within the given List with ProviderIDs.
        */
        virtual CSenIdentityProvider* IdentityProviderL(
                                            MSenServiceDescription& aService,
                                            const CDesC8Array& aIdpList,
                                            TBool aStrict) = 0;

        /**
        * Checks if Identity already has an identity provider with given data.
        * Existing identity provider can still have more data than the pattern.
        * @param    aContains   will contain the boolean value on return. ETrue if
        *           contains, EFalse otherwise.
        * @param    aIDP    the pattern identity provider.
        * @return KErrNone or system-wide error code.
        */
        virtual TInt ContainsIdentityProviderL( TBool& aContains,
                                                CSenIdentityProvider& aIDP) = 0;
    
        /**
        * Register an IdentityProvider for the current active Identity.
        * @param aIdp a registered IdentityProvider. Ownership of the
        * argument is transferred into this class from the caller.
        * @return KErrNone or system-wide error code.
        */
        virtual TInt RegisterIdentityProviderL( CSenIdentityProvider* aIdp ) = 0;
        
        virtual TInt UpdateIdentityProviderL(CSenIdentityProvider& aIdp) = 0;

        /**
        * Unregister an IdentityProvider for the current active Identity.
        * @param aIdp an unregistered IdentityProvider
        * @return   KErrNone        if everything goes OK.
        *           KErrNotFound    if correct identity provider was not found
        *           Other error codes are system-wide Symbian error codes.
        */
        virtual TInt UnregisterIdentityProviderL( CSenIdentityProvider& aIdp) = 0;

        /**
        * Associate a service to an IdentityProvider.
        * @param aServiceID the contract or endpoint of a service
        * @param aProviderID the id of an IdentityProvider
        * @return true if successful. Failure may be caused by non-existence
        * of the IdentityProvider for the current user.
        */
        virtual TBool AssociateServiceL( const TDesC8& aServiceID,
                                         const TDesC8& aProviderID ) = 0;

        /**
        * Dissociate a service from an IdentityProvider.
        * @param aServiceID the contract or endpoint of a service
        * @param aProviderID the id of an IdentityProvider
        * @return true if successful. Failure may be caused by non-existence
        * of the IdentityProvider for the current user.
        */
        virtual TBool DissociateServiceL(const TDesC8& aServiceID,
                                        const TDesC8& aProviderID ) = 0;
                                        
        virtual TInt FindMatchingIdentityProviderL(CSenIdentityProvider &aIdp,
                                                   CSenIdentityProvider*& apMatch) = 0;

        /**
        * Write the Identity as XML into a write stream.
        * This method is invoked by the IdentityManager when it saves
        * its state and/or configuration.
        * Identities should serialize sufficient information
        * to enabled re-instantiation from the same XML content.
        */
        virtual void WriteAsXMLToL(RWriteStream& aWriteStream) = 0;

        /**
        * Write the Identity as XML into 8-bit (UTF-8) buffer
        */
        virtual HBufC8* AsXmlL() = 0;

        /**
        * Write the Identity as XML into 8-bit (Unicode) buffer
        */
        virtual HBufC* AsXmlUnicodeL() = 0;
        
        virtual const RIdentityProviderList& IdentityProvidersL() const = 0;
    };

#endif // M_SEN_IDENTITY_H

// End of File
