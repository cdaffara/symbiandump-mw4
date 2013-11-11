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








#ifndef M_SEN_SERVICE_MANAGER_H
#define M_SEN_SERVICE_MANAGER_H

//  INCLUDES
#include <e32std.h>
#include <flogger.h> // RFileLogger
#include <badesca.h> // CDesCArray etc

#include "msentransportfactory.h"

// FORWARD DECLARATIONS
class CSenXmlReader;
class MSenServiceDescription;
class MSenIdentityManager;
class MSenRemoteServiceConsumer;
class CSenWSDescription;
class CSenServiceSession;
class CSenIdentityProvider;
class CSenBaseFragment;
class MSenProviderPolicy;
class MSenConsumerPolicy;
class RStringPool;
class CSenInternalCredential;
class RSenCredentialPtr;

// DATA TYPES
typedef RPointerArray<CSenWSDescription> RWSDescriptionArray;
typedef RPointerArray<CSenInternalCredential> RSenCredentialArray;
typedef RArray<RSenCredentialPtr> RSenCredentialPtrArray;

// CLASS DECLARATION

/**
* MSenServiceManager represents the client side interface of a
* CoreServiceManager
* The methods defined in this interface may executed from a client
* (ServiceConnection) via the server-side session (SenClientSession).
* Note that the singleton CoreServiceManager also has a number of public
* methods (defined in MSenCoreServiceManager interface) that are used by
* e.g. plug-ins, that are expected to operate in the same process as the
* CoreServiceManager.
*/
class MSenServiceManager : public MSenTransportFactory
    {
    public:
    
        // New functions
        
        /**
        * @param aServiceConnection is the ServiceConnection to 
        * be initialized. Method attempts to initialize connection
        * according to a pattern, which this service manager receives.
        * Pattern is spesified by some service consumer (WSC app's), 
        * which requests a new connection to be initialized.
        * Therefore, the new connection should be to a service that
        * has a service description matching with the pattern.
        * @param aPattern is a - typically partial - service description
        * that is used as a search pattern.
        * @param aErrorMsg may contain the error message (SOAP-ENV or SOAP 
        * fault) in case that this function is returning an error from service
        * @return KErrNone or a system-wide error code, if error occurred.
        */
        virtual TInt InitServiceConnectionL( MSenRemoteServiceConsumer& aServiceConsumer,
                                             CSenWSDescription& aPattern,
                                             HBufC8*& aErrorMsg) = 0;
        /**
        * Search for services whose service descriptions using a contract URI.
        * @param   aMatches is the list into which 0 or more ServiceDescriptions
        * will be appended, if one or more matches are found. Onership of the
        * appended pointers is not transferred to the caller.
        * @param   aContract is the URI which specifies the searhed contract.
        * @return KErrNone if successful, or otherwise a system-wide error code.
        */
        virtual TInt ServiceDescriptionsL( RWSDescriptionArray& aMatches,
                                           const TDesC8& aContract ) = 0;
        
        /**
        * Search for services with a matching ServiceDescription,
        * which can contain any combination of contract, endpoint and facets.
        * @param   aMatches is the list into which 0 or more service descriptions
        * will be appended. Ownership of appended pointers is not transferred to
        * the caller.
        * @param aPattern is the service description used as "search pattern", 
        * as a kind of "search mask". This means, that it is typically a "partial"
        * service description, which fields include only those fields that need
        * to match - the content of other fields (elements) is not relevant to
        * the caller.
        * @return KErrNone if successful, or otherwise a system-wide error code.
        */
        virtual TInt ServiceDescriptionsL( RWSDescriptionArray& aMatches,
                                           MSenServiceDescription& aPattern ) = 0;

        /**
        * Used by applications to inform the Service Manager about a service,
        * for example about an Authentication Service, which typically cannot
        * be discovered through other means.
        * @param aServiceDescription which is being registered.
        * @return KErrNone if registration was successful or otherwise:
        *  a system error
        */
        virtual TInt RegisterServiceDescriptionL( CSenWSDescription* apServiceDescription ) = 0;

        /**
        * Used by application to to inform the Service Manager about a service
        * which no longer is available. The ServiceManager will remove the
        * service from its database as soon as it is no longer used.
        * @param aServiceDescription
        * @return KErrNone if registration succeeded or otherwise some system-wide
        * error code.
        */
        virtual TInt UnregisterServiceDescriptionL( MSenServiceDescription& aServiceDescription ) = 0;

        /**
        * Method attempts to find new framework using given FrameworkId. If framework ECOM
        * is found, the a pointer to it's configuration reader (fragment) is returned, or
        * NULL otherwise.
        */
        virtual CSenBaseFragment* InstallFrameworkL( const TDesC8& aFrameworkId ) = 0;

        /**
        * Return the XML reader instance used and owned by this Core Service Manager
        */
        virtual CSenXmlReader* XMLReader() = 0;

        /**
        * @return totally randomly generated (but not cryptographically strong)
        * GUID. The GUID is based on the MD5 hash of a random number and the 
        * current time. 
        */
        virtual HBufC8* RandomGuidL() = 0;

        /*
        * @return pointer to the file logger owned by this Core Service Manager
        */
        virtual RFileLogger* Log() const = 0;

        virtual void IncrementConnections() = 0;
        virtual void DecrementConnections() = 0;

        /**
        * Register an IdentityProvider for the current active Identity.
        * Ownerships is transferred. Method is typically implemented
        * by some Identity Manager which this Core Service Manager knows.
        * @param aIdp a registered IdentityProvider
        * @return KErrNone or system-wide error code.
        */
        virtual TInt RegisterIdentityProviderL( CSenIdentityProvider* aIdp ) = 0;

        /**
        * Unregister an IdentityProvider for the current active Identity.
        * Method is typically implemented by some Identity Manager which 
        * this Core Service Manager knows.
        * @param aIdp an unregistered IdentityProvider
        * @return true if successful.
        */
        virtual TBool UnregisterIdentityProviderL( CSenIdentityProvider& aIdp ) = 0;

        /**
        * Associate a service to an IdentityProvider.
        * @param aServiceID the contract or endpoint of a service
        * @param aProviderID the id of an IdentityProvider
        * @return true if successful. Failure may be caused by non-existence
        * of the IdentityProvider for the current user.
        */
        virtual TBool AssociateServiceL( const TDesC8& aServiceID,
                                         const TDesC8& providerID ) = 0;

        /**
         * Dissociate a service from an IdentityProvider.
         * @param aServiceID the contract or endpoint of a service
         * @param aProviderID the id of an IdentityProvider
         * @return true if successful. Failure may be caused by non-existence
         * of the IdentityProvider for the current user.
         */
        virtual TBool DissociateServiceL( const TDesC8& aServiceID,
                                          const TDesC8& aProviderID ) = 0;

        virtual void SetShowPasswordDialog( const TBool aState ) = 0;

        /**
         *  Check if there is an existing service description with identical data
         *  already in the database. The service description in the database can
         *  contain more information than the pattern, but all the pattern 
         *  information MUST match exactly for a match.
         *  @param  aContains contains the boolean value in return, ETrue if found,
         *          EFalse if not
         *  @param  aPattern the service description data to be searched.
         *  @return KErrNone or other system-wide Symbian error codes.
         */
        virtual TInt ContainsServiceDescriptionL( TBool& aContains,
                                                  CSenWSDescription& aPattern ) = 0;

        /**
         *  Check if there is an existing identity provider with identical data
         *  already in the database. The identity provider in the database can
         *  contain more information than the pattern identity provider, 
         *  but all the pattern information MUST match exactly for a match.
         *  @param  aContains contains the boolean value in return, ETrue if found,
         *          EFalse if not
         *  @param  aPattern the identity provider data to be searched.
         *  @return KErrNone or other system-wide Symbian error codes.
         */
        virtual TInt ContainsIdentityProviderL( TBool& aContains,
                                                CSenIdentityProvider& aIDP) = 0;

        /**
        * Notifies all framework plug-ins about certain event
        * @param aEvent is the event code passed to all plug-ins
        * @return some system-wide error code, if an error occurred
        */
        virtual TInt NotifyFrameworksL( const TInt aEvent ) = 0;

        /**
        * Notifies all framework plug-ins about certain event and
        * object.
        * @param aEvent is the event code passed to all plug-ins
        * @param aArgument is an additional object pointer may be
        * passed. NULL is also accepted.
        * @return some system-wide error code, if an error occurred
        */
        virtual TInt NotifyFrameworksL( const TInt aEvent, 
                                        TAny* aArgument ) = 0;


        /**
        * Notifies only those frameworks plug-ins, which match
        * with given framework ID (aFrameworkID)
        * @param aEvent is the event code passed to certain plug-ins
        * @param aFrameworkID spesifies which frameworks are need to
        * be notified. Note, that if aFrameworkID is KNullDesC8 
        * (zero-length),then all known framework plug-ins are notified
        * about the event.
        * @return some system-wide error code, if an error occurred
        */
        virtual TInt NotifyFrameworksL( const TDesC8& aFrameworkID,
                                        const TInt aEvent ) = 0;

        /**
        * Notifies only those frameworks plug-ins, which match
        * with given framework ID (aFrameworkID) and 
        * an additional object pointer may be passed
        * NULL is also accepted
        * @param aEvent is the event code passed to certain plug-ins
        * @param aFrameworkID spesifies which frameworks are need to
        * be notified. Note, that if aFrameworkID is KNullDesC8 
        * (zero-length),then all known framework plug-ins are notified
        * about the event.
        * @param aArgument is an additional object pointer may be
        * passed. NULL is also accepted.
        * @return some system-wide error code, if an error occurred
        */
        virtual TInt NotifyFrameworksL( const TDesC8& aFrameworkID,
                                        const TInt aEvent,
                                        TAny* aArgument ) = 0;

        /**
        * Calculates the size of service descriptions, in bytes.
        * Internally, this method calls WriteAsXMLToL() from each
        * service description instance found inside aArray.
        *
        * @param aArray contains the service descriptions, which
        * size in XML is requested.
        * @return total amount of bytes that XML representation of
        *         service descriptions included in aArray take.
        *
        * In case of error, this method will leaves and passes on
        * the system-wide error code in question.
        */
        virtual TInt SizeOfServiceDescriptionsL( RWSDescriptionArray& aArray ) = 0;
        
        /**
        * Calculates the size of credentials, in bytes.
        * Internally, this method calls WriteAsXMLToL() from each
        * credential instance found inside aArray.
        *
        * @param aArray contains the credentials, which size in
        *               XML is requested.
        * @return       total amount of bytes that XML representation
        *               of credentials included in aArray take.
        *
        * In case of error, this method will leaves and passes on
        * the system-wide error code in question.
        */
        virtual TInt SizeOfCredentialsL( RSenCredentialArray& aArray ) = 0;
        virtual TInt SizeOfIdentityProvidersL(const RPointerArray<CSenIdentityProvider>& aArray) = 0;


        /**
        * Getter for WSF-wide string pool. Any subcomponent utilizing
        * a string pool based API should use this instance, which is
        * owned by service manager implementation behind this interface.
        * @return RStringPool reference to string pool owned by the manager
        *
        */
        virtual RStringPool& StringPool() = 0;

        /**
        * Method attempts to add an entry to the list of active hostlet connection
        * endpoints.
        * @param aEndpoint is the identifier, typically some URN, representing the
        *        endpoint. It is expected to be locally unique. MSenRemoteHostlet
        *        implementations typically call this method to figure out whether
        *        the endpoint proposed by actual Hostlet Connection is available
        *        or not. In case of error, that error is then normally propagated
        *        all the way back to the public API, so that constructor -- 
        *        CSenHostletConnection::NewL will leave.
        * @return KErrNone if add succeeded
        *         KErrSenEndpointReserved, if the endpoint is already allocated 
        *         for some other hostlet connection.
        * Note: KErrSenEndpointReserved is defined in SenHostletConnection.h
        */
//        virtual TInt AddActiveHostletConnectionEndpointL(const TDesC8& aEndpoint) = 0;

        /**
        * Removes entry from the list of active hostlet connection endpoints.
        * @param aEndpoint is the identifier, typically some URN, representing the
        *        endpoint. MSenRemoteHostlet impelemtations typically call this method
        *        when the hostlet connection (server-side session) is being de-allocated.
        * @return KErrNone if add succeeded
        *         KErrSenEndpointReserved, if the endpoint is already allocated 
        *         for some other hostlet connection.
        * Note: KErrSenEndpointReserved is defined in SenHostletConnection.h
        */
//        virtual TInt RemoveActiveHostletConnectionEndpoint(const TDesC8& aEndpoint) = 0;

        virtual TInt NextTransactionId() = 0;
        
		virtual TInt CredentialsL( const CSenWSDescription& aPattern,
								   RSenCredentialArray& aCredentials ) = 0;

		virtual TInt CredentialsL( const CSenWSDescription& aPattern,
								   const CSenIdentityProvider& aIdP,
								   RSenCredentialArray& aCredentials ) = 0;

		virtual TInt CredentialsL( const CSenWSDescription& aPattern,
								   RSenCredentialPtrArray& aCredentials ) = 0;

		virtual TInt CredentialsL( const CSenWSDescription& aPattern,
								   const CSenIdentityProvider& aIdP,
								   RSenCredentialPtrArray& aCredentials ) = 0;

		virtual RSenCredentialPtr AddCredentialL( CSenInternalCredential* apCredential,
		                                           TInt& aErrorTo ) = 0;

		virtual RSenCredentialPtr AddCredentialL( CSenIdentityProvider* apIdP,
									               CSenInternalCredential* apCredential,
									               TInt& aErrorTo ) = 0;

		virtual RSenCredentialPtr AddCredentialL( const TDesC8& aCredential,
		                                           TInt& aErrorTo ) = 0;

		virtual RSenCredentialPtr AddCredentialL( CSenIdentityProvider* apIdP,
									               const TDesC8& aCredential,
									               TInt& aErrorTo ) = 0;

		virtual TInt RemoveCredentialsL( const CSenWSDescription& aPattern ) = 0;

		virtual TInt RemoveCredentialsL( const CSenWSDescription& aPattern,
										 const CSenIdentityProvider& aIdP ) = 0;
        
        virtual TInt RemoveCredentialsL(const TDesC8& aProviderId) = 0;
										
        virtual TInt RemoveCredentialL( TInt aInternalCredentialId ) = 0; //codescannerwarnings
        
        virtual RSenCredentialPtr CredentialL( TInt aInternalCredentialId,
                                               TInt& aErrorTo ) = 0;

        virtual RSenCredentialPtr UpdateCredentialL( TInt aInternalCredentialId,
                                                     CSenInternalCredential* apCredential,
                                                     TInt& aErrorTo ) = 0;
        
        virtual RSenCredentialPtr UpdateCredentialL( TInt aInternalCredentialId,
                                                     const TDesC8& aCredential,
                                                     TInt& aErrorTo ) = 0;
										
        virtual TInt FindMatchingIdentityProviderL( CSenIdentityProvider &aIdp,
                                                    CSenIdentityProvider*& apMatch ) = 0;
                                                   
        virtual TInt UpdateIdentityProviderL( CSenIdentityProvider& aIdp ) = 0;
        
        virtual TInt NextConnectionID() = 0;
    };

#endif // M_SEN_SERVICE_MANAGER_H

// End of File
