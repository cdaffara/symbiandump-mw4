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








#ifndef SEN_BASE_IDENTITY_MANAGER_H
#define SEN_BASE_IDENTITY_MANAGER_H

// INCLUDES
#include <e32std.h>
#include <ecom.h>
#include <flogger.h>    // RFileLogger
#include <badesca.h>    // CDesC8Array

#include <SenIdentityProvider.h>
#include <SenBaseFragment.h>

#include "senbaseidentity.h"
#include "msenidentitymanager.h" 




// CONSTANTS
#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES
const TUid KUidSechMech = {0x101F96FC};
#else
const TUid KUidSechMech = {0x101F9733};
#endif
// FORWARD DECLARATIONS
class CSenSenSecurityMechanismObserver;
class MSenCoreServiceManager;

// CLASS DECLARATION
class CSenBaseIdentityManager : public CSenBaseFragment,
                                public MSenIdentityManager
    {
    public: // Constructors and destructor
    
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CSenBaseIdentityManager* NewL(
                                            MSenCoreServiceManager& aManager);
        IMPORT_C static CSenBaseIdentityManager* NewLC(
                                            MSenCoreServiceManager& aManager);

        IMPORT_C virtual ~CSenBaseIdentityManager();
        
        // New functions
        
        void ReloadSenSecurityMechanismsL();

        // Functions from base classes
        
        // from MSenIdentityManager
        TPtrC8 SenSecurityMechanismNames8L();
        CSenSecurityMechanism* MechanismNamedL(const TDesC8& aName);

        /**
        * Initialize this SenBaseIdentityManager by parsing
        * the given file with the given XMLReader.
        * The file is expected to contain valid XML that
        * follows the example given above.
        * @param file
        * @param reader
        */
        void LoadFromL(TDesC& aFile, CSenXmlReader& aReader);


        /**
        * Save the current state of the SenBaseIdentityManager as an XML
        * document into a file.
        * Statuscodes:
        *   KErrNone                Ok
        *  Other codes are system error codes.
        * @return status code.
        */
        IMPORT_C TInt WriteDatabase();

        /**
        * Creates the element. Overrides CSenBaseFragment
        * StartElementL-function
        * @param aNsUri name space URI
        * @param aLocalName local name
        * @param aQName cue name
        * @param apAttrs XML attributes
        */
        virtual void StartElementL( const TDesC8& aNsUri,
                                    const TDesC8& aLocalName,
                                    const TDesC8& aQName,
                                    const RAttributeArray& aAttributes);

        /**
        * Implements MSenIdentityManager
        * @return the currently active default identity.
        */
        virtual MSenIdentity& IdentityL();

        /**
        * Implements MSenIdentityManager
        * @param On the return, contains the (System) userName of the
        *        currently active user.
        * @return KErrNone if no error, or some of the system wide error codes
        */
        virtual TInt UserNameL(HBufC8*& aUserName);

        /**
        * Implements MSenIdentityManager
        * @return the default IdentityProvider of the currently active
        *         Identity.
        */
        virtual CSenIdentityProvider* IdentityProviderL();

        /**
        * Implements MSenIdentityManager
        * @param aServiceDescription
        * @return the IdentityProvider of the currently active Identity.
        */
        virtual CSenIdentityProvider* IdentityProviderL(
                                MSenServiceDescription& aServiceDescription );

        /**
        * Implements MSenIdentityManager
        * @param aURI
        * @return the IdentityProvider of the currently active Identity.
        */
        virtual CSenIdentityProvider* IdentityProviderL(const TDesC8& aURI );

        /**
        * Implements MSenIdentityManager
        * @param aIdpList an array of ProviderIDs
        * @param aStrict if set to true only an IdentityProvider from the List
        * will be returned, and null if none of the list entries is known to
        * the current active Identity.
        * @return the currently preferred IdentityProvider.
        */
        virtual CSenIdentityProvider* IdentityProviderL(
                                const CDesC8Array& aIdpList, TBool aStrict);

        /**
        * Implements MSenIdentityManager
        * @param aServiceDescription
        * @param aIdpList an array of ProviderIDs
        * @param aStrict if set to true only an IdentityProvider from the List
        * will be returned, and null if none of the list entries is known to
        * the current active Identity.
        * @return the currently preferred IdentityProvider.
        */
        virtual CSenIdentityProvider* IdentityProviderL(
                                MSenServiceDescription& aServiceDescription,
                                const CDesC8Array& aIdpList,
                                TBool aStrict);

        /**
        * Checks if there already is an identity provider with given data.
        * Existing identity provider can still have more data than the pattern.
        * @param    aContains   will contain the boolean value on return. ETrue if
        *           contains, EFalse otherwise.
        * @param    aIDP    the pattern identity provider.
        * @return KErrNone or system-wide error code.
        */
        virtual TInt ContainsIdentityProviderL(
                                       TBool& aContains,
                                       CSenIdentityProvider& aIDP);

        /**
        * Implements MSenIdentityManager
        * Ownerships is transferred
        * @param aIdp a registered IdentityProvider
        * @return KErrNone if successful.
        */
        virtual TInt RegisterIdentityProviderL(CSenIdentityProvider* apIdp );

        /**
        * Implements MSenIdentityManager
        * @param aIdp an unregistered IdentityProvider
        * @return KErrNone if successful.
        *         KErrNotFound if IdentityProvider was not found.
        *         Other return values are symbian system-wide error codes.
        */
        virtual TInt UnregisterIdentityProviderL(CSenIdentityProvider& aIdp );

        /**
        * Implements MSenIdentityManager
        * @param aServiceID the contract or endpoint of a service
        * @param aProviderID the id of an IdentityProvider
        * @return true if successful. Failure may be caused by non-existence
        * of the IdentityProvider for the current user.
        */
        virtual TBool AssociateServiceL(const TDesC8& aServiceID,
                                        const TDesC8& aProviderID );

        /**
        * Implements MSenIdentityManager
        * @param aServiceID the contract or endpoint of a service
        * @param aProviderID the id of an IdentityProvider
        * @return true if successful. Failure may be caused by non-existence
        * of the IdentityProvider for the current user.
        */
        virtual TBool DissociateServiceL(const TDesC8& aServiceID,
                                         const TDesC8& aProviderID );

        /**
        * Implements MSenIdentityManager
        * Status codes:
        *   KErrNone                        ok
        *   KErrNotFound                    no identities
        *   Other codes are system error codes
        * @param aIdentitiesList an array where the identities can be inserted
        * @return status/error code.
        */
        virtual TInt IdentitiesL( CDesC8Array& aIdentitiesList );

        /**
        * NOTE! This code does not do as documentation states.
        * Only second line text is shown and
        * "Ok" button on the left side.
        * @return the value of button pressed (OK or CANCEL)
        */
        virtual TInt YesNoQueryL(const TDesC& aLine1,
                                 const TDesC& aLine2,
                                 const TDesC& aButton1,
                                 const TDesC& aButton2);

        virtual TInt AuthenticationForL(CSenIdentityProvider& aAccount, TPckgBuf<TSenAuthentication>& aResponse);

        /**
        * For encoding and decoding base 64:
        */
        virtual HBufC8* EncodeToBase64LC(const TDesC8& aSource);
        virtual HBufC8* DecodeFromBase64LC(const TDesC8& aSource);
        
        virtual TInt FindMatchingIdentityProviderL(CSenIdentityProvider &aIdp,
                                                   CSenIdentityProvider*& apMatch);
        
        virtual TInt UpdateIdentityProviderL(CSenIdentityProvider& aIdp);

        /**
        * "Enables/disables" -flag which determines if UI interaction with end-user is allowed or not
        */
        virtual void SetShowPasswordDialog(const TBool aState);

        RFileLogger* Log();
        /** Improved heuristics from Identity DB removal / cleanups
		*
		* It finds the matching IdentityProvider child elements of current Identity 
		* element using the service description and add the touch attribute to all 
		* matched IdentityProvider elements with current tick count value.
		* If touch attribute already there it will be replaced with new current 
		* tick count value.
		*
		* @param asd     is the new established session service description.
		* Leave codes: Symbian system-wide error codes.  
		*/
        IMPORT_C void UpdateTouchIdentityDBL(MSenServiceDescription& asd);
        
        
        /**
        * Implements MSenIdentityManager
        * @return the default IdentityProvider of the currently active
        *         Identity.
        */
        const virtual RPointerArray<CSenIdentityProvider>& IdentityProvidersL();

    protected: 
    
        /**
        * C++ default constructor.
        */
        CSenBaseIdentityManager(MSenCoreServiceManager& aManager);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
        // New functions
        
        /**
        * Save the current state of the SenBaseIdentityManager as an XML
        * document into a file.
        * Status codes:
        *   KErrNone                Ok
        *   Other codes are system error codes.
        * @return status code.
        */
        virtual TInt WriteConfigurationToL( const TDesC& aFile );
        TBool AllowSavePasswordL();
        /** Improved heuristics from Identity DB removal / cleanups : Naga
	    *
	    * Checks touch attribute value for all IdentityProvider child elements of 
	    * current Identity element if that exceeds the the tick counts for two weeks 
	    * (Max tick count) or if it is greater than current tick count value it removes 
	    * the corresponding IdentityProvider child element from the Identity element 
	    * child list.    
	    * 
	    * 1 millisec = 1 tick
		* 86400 sec = 1 day
		* 86400*1000 millisec = 1 day
		* 86400*14*1000 = 1209600000 millisec = 2 weeks
		* So if attr val >= 1209600000 or > current tick count.
		* Note: always attr val should be <= current tick count.
		*
	    * eg;
	    * Before call to CleanupUnusedIdentityDBL()
	    * <Users>
		* <User>
		* <Identity>
		* <IdentityProvider>
  		* <Endpoint>http://10.21.32.20/WSStar/CredMan/STS.aspx</Endpoint> 
  		* .........
  		* ..........
  		* </IdentityProvider>
		* <IdentityProvider touch="1000">
  		* <Endpoint>http://10.21.32.20/WSStar/CredMan/STS_001.aspx</Endpoint> 
  		* ..........
  		* ..........
  		* </IdentityProvider>
  		* <IdentityProvider touch="1309600000">
  		* <Endpoint>http://10.21.32.20/WSStar/CredMan/Webservises.aspx</Endpoint> 
  		* ..........
  		* ............
  		* </IdentityProvider>
		* </Identity>
  		* </User>
  		* </Users>
		* 
		* After call to CleanupUnusedIdentityDBL()
	    * <Users>
		* <User>
		* <Identity>
		* <IdentityProvider>
  		* <Endpoint>http://10.21.32.20/WSStar/CredMan/STS.aspx</Endpoint> 
  		* .........
  		* ..........
  		* </IdentityProvider>
		* <IdentityProvider touch="1000">
  		* <Endpoint>http://10.21.32.20/WSStar/CredMan/STS_001.aspx</Endpoint> 
  		* ..........
  		* ..........
  		* </IdentityProvider>
  		* </Identity>
  		* </User>
  		* </Users>
  		* 
		* Leave codes: Symbian system-wide error codes.
	    */
        void CleanupUnusedIdentityDBL();

    protected: // Data
        CSenBaseIdentity* iIdentity;
        CSenIdentityProvider* iProvider;
        MSenCoreServiceManager& iManager;
        TBool iConfigReadonly;
        RFileLogger* iLog;   // not owned (no create/close)
        CSenSenSecurityMechanismObserver* iObserver;
        CBufFlat* ipMechanicsNames;
        HBufC8* ipDeviceID;     // Owned
        RImplInfoPtrArray       iEcomInfoArray;
        RPointerArray<HBufC8>   iSechMechNames;
        RPointerArray<HBufC8>   iSechMechCues;
        TBool iShowPasswordDialog;
    };

#endif // SEN_BASE_IDENTITY_MANAGER_H

// End of File
