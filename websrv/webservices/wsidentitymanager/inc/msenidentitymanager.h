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








#ifndef M_SEN_IDENTITY_MANAGER_H
#define M_SEN_IDENTITY_MANAGER_H

// INCLUDES
#include <e32base.h>
#include <e32des8.h>    // HBufC8
#include <badesca.h>    // CDesC8Array

#include <SenIdentityProvider.h>
#include <MSenServiceDescription.h>

#include "msenidentity.h" 

// FORWARD DECLARATIONS
class CSenSecurityMechanism;

/**
* An MSenIdentityManager knows about  the various Identitities
* of the user(s) of the system. Components use this interface
* to register identities and query for identities in various
* situations.
*
* An Identity can contain all kinds of information and is not
* further specified here. For maximum flexibility an identity
* is modelled as an XML Element that can have arbitrary children
* elements.
*
* The IdentityManager can have a rather complex list of Identities,
* the Identity that it returns is based upon:
* - the currently active user identity (set)
* - a URL (Endpoint) or ProviderID given in the request
*
* An IdentityManager can return a full Identity (which can contain a
* lot of information); but also offer methods to only return an
* IdentityProvider that can be used to obtain access to a particular service.
*/
class MSenIdentityManager
    {
    public: // New functions

        /**
        * This method is not intended for the developer API and is for internal
        * use only. It may be exposed the developers of
        * ServiceInvocationFrameworks.
        * @return the currently active default identity.
        */
        virtual MSenIdentity& IdentityL() = 0;

        /**
        * This method is not intended for the developer API and is for internal
        * use only. It may be exposed the developers of
        * ServiceInvocationFrameworks.
        * @param the (System) userName of the currently active user.
        * @return KErrNone if no error, or some of the system wide error codes
        */
        virtual TInt UserNameL(HBufC8*& aUserName) = 0;

        /**
        * This method is not intended for the developer API and is for internal
        * use only. It may be exposed the developers of
        * ServiceInvocationFrameworks.
        * @return the default IdentityProvider of the currently active Identity
        *         or NULL if not found(!)
        */
        virtual CSenIdentityProvider* IdentityProviderL() = 0;

        /**
        * This method is not intended for the developer API and is for internal
        * use only. It may be exposed the developers of
        * ServiceInvocationFrameworks.
        * @param aServiceDescription
        * @return the IdentityProvider of the currently active
        * Identity that has the ServiceID that best mathces the
        * endpoint or providerID of the given ServiceDescription.
        */
        virtual CSenIdentityProvider* IdentityProviderL(
                            MSenServiceDescription& aServiceDescription ) = 0;

        /**
        * This method is not intended for the developer API and is for internal
        * use only. It may be exposed the developers of
        * ServiceInvocationFrameworks.
        * @param aURI
        * @return the IdentityProvider of the currently active
        * Identity that has the ServiceID that best mathces the
        * given URI or NULL if not found(!).
        */
        virtual CSenIdentityProvider* IdentityProviderL( const TDesC8& aURI ) = 0;

        /**
        * This method is not intended for the developer API and is for internal
        * use only. It may be exposed the developers of
        * ServiceInvocationFrameworks.
        * @param aIdpList an array of ProviderIDs
        * @param aStrict if set to true only an IdentityProvider from the List
        * will be returned, and null if none of the list entries is known to
        * the current active Identity.
        * @return the currently preferred IdentityProvider
        * from within the given List with ProviderIDs.
        */
        virtual CSenIdentityProvider* IdentityProviderL(
                                                const CDesC8Array& aIdpList,
                                                TBool aStrict) = 0;

        /**
        * This method is not intended for the developer API and is for internal
        * use only. It may be exposed the developers of
        * ServiceInvocationFrameworks.
        * @param aServiceDescription
        * @param aIdpList an array of ProviderIDs
        * @param aStrict if set to true only an IdentityProvider from the List
        * will be returned, and null if none of the list entries is known to the
        * current active Identity.
        * @return the currently preferred IdentityProvider
        * from within the given List with ProviderIDs.
        */
        virtual CSenIdentityProvider* IdentityProviderL(
                                MSenServiceDescription& aServiceDescription,
                                const CDesC8Array& aIdpList,
                                TBool aStrict) = 0;

        /**
        * Register an IdentityProvider for the current active Identity.
        * @param aIdp a registered IdentityProvider
        * @return true if succesful.
        */
        virtual TInt RegisterIdentityProviderL( CSenIdentityProvider* aIdp ) = 0;

        /**
        * Unregister an IdentityProvider for the current active Identity.
        * @param aIdp an unregistered IdentityProvider
        * @return true if succesful.
        */
        virtual TInt UnregisterIdentityProviderL( CSenIdentityProvider& aIdp) = 0;

        /**
        * Associate a service to an IdentityProvider.
        * @param aServiceID the contract or endpoint of a service
        * @param aProviderID the id of an IdentityProvider
        * @return true if successful. Failure may be caused by non-existence
        * of the IdentityProvider for the current user.
        */
        virtual TBool AssociateServiceL(const TDesC8& aServiceID,
                                        const TDesC8& providerID ) = 0;

        /**
        * Dissociate a service from an IdentityProvider.
        * @param aServiceID the contract or endpoint of a service
        * @param aProviderID the id of an IdentityProvider
        * @return true if successful. Failure may be caused by non-existence
        * of the IdentityProvider for the current user.
        */
        virtual TBool DissociateServiceL(const TDesC8& aServiceID,
                                         const TDesC8& aProviderID ) = 0;

        /**
        * This method is not intended for the developer API and is for internal
        * use only. It may be exposed the developers of
        * ServiceInvocationFrameworks.
        * Status codes:
        *   KErrNone                        ok
        *   KErrNotFound                    no identities
        *   Other codes are system error codes
        * @param aIdentitiesList an array where the identities can be inserted
        * @return status/error code.
        */
        virtual TInt IdentitiesL( CDesC8Array& aIdentitiesList ) = 0;
        
        virtual TInt AuthenticationForL(CSenIdentityProvider& aAccount, TPckgBuf<TSenAuthentication>& aResponse) = 0;
        virtual TPtrC8 SenSecurityMechanismNames8L() = 0;
        virtual CSenSecurityMechanism* MechanismNamedL(const TDesC8& aName) = 0;

        /**
        * For encoding and decoding base 64:
        */
        virtual HBufC8* EncodeToBase64LC(const TDesC8& aSource) = 0;
        virtual HBufC8* DecodeFromBase64LC(const TDesC8& aSource) = 0;

        virtual void SetShowPasswordDialog(const TBool aState) = 0;
        
        virtual TInt FindMatchingIdentityProviderL(CSenIdentityProvider &aIdp,
                                                   CSenIdentityProvider*& apMatch) = 0;
                                                   
        virtual TInt UpdateIdentityProviderL(CSenIdentityProvider& aIdp) = 0;
        virtual const RPointerArray<CSenIdentityProvider>& IdentityProvidersL() = 0;
    };

#endif // M_SEN_IDENTITY_MANAGER_H

// End of File
