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








#ifndef SEN_CREDENTIAL_MANAGER_H
#define SEN_CREDENTIAL_MANAGER_H

// INCLUDES
#include "msencredentialmanager.h"

#include <e32std.h>
#include <flogger.h>    // RFileLogger

#include <SenFragmentBase.h>
#include <MSenServiceDescription.h>

// FORWARD DECLARATIONS
class MSenCoreServiceManager;
class CSenCredentialIdentifier;
class CSenCredentialProperties;
class CSenParser;

// CLASS DECLARATION
class CSenCredentialManager : public CSenFragmentBase , public MSenCredentialManager
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CSenCredentialManager* NewL(MSenCoreServiceManager& aManager);

        IMPORT_C static CSenCredentialManager* NewLC(MSenCoreServiceManager& aManager);

        IMPORT_C virtual ~CSenCredentialManager();

        /**
        * Load and parse SenCredentialManager from the file.
        * Statuscodes:
        *   KErrNone                Ok
        *  Other codes are system error codes.
        * @return status code.
        */
        TInt LoadDB();

        /**
        * Save SenCredentialManager as an XML document into
        * the file.
        * Statuscodes:
        *   KErrNone                Ok
        *  Other codes are system error codes.
        * @return status code.
        */
        TInt SaveCredentialDB();

        // From CSenBaseFragment
        virtual void OnStartElementL(const RTagInfo& aElement,
        							 const RAttributeArray& aAttributes,
        						 	 TInt aErrorCode);

        virtual void OnEndElementL(const RTagInfo& aElement,
        						   TInt aErrorCode);

        // from MSenCredentialManager
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
		/** Improved heuristics from credential DB removal / cleanups
		*
		* It finds the matching CredentialContainer child elements of Credentials 
		* element using the service description and add the touch attribute to all 
		* matched credential container elements with current tick count value.
		* If touch attribute already there it will be replaced with new current 
		* tick count value.
		*
		* @param asd     is the new established session service description.
		* Leave codes: Symbian system-wide error codes.  
		*/                                                    
		IMPORT_C virtual void UpdateTouchCredDBL(MSenServiceDescription& asd);
        
	protected:
	    RFileLogger* Log();

    private:

        /**
        * C++ default constructor.
        */
        CSenCredentialManager(MSenCoreServiceManager& aManager);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // New functions

        /**
        * Initialize this SenCredentialManager by parsing
        * the given file with the given XMLReader.
        * The file is expected to contain valid XML that
        * follows the example given above.
        * @param file
        * @param reader
        */
        void LoadFromL(const TDesC& aFile);

        /**
        * Save the current state of the SenCredentialManager as an XML
        * document into a file.
        * Status codes:
        *   KErrNone                Ok
        *   Other codes are system error codes.
        * @return status code.
        */
        TInt SaveToL( const TDesC& aFile );

	    virtual TInt NextCredentialIdL();	//codescannerwarnings
	    
	    virtual void UpdateMaxCredentialIdL();	//codescannerwarnings
	    TBool IsApplicableOrShareableL(const CSenWSDescription& aPattern,CSenInternalCredential* aCredential);
	    TInt RefreshMTL(const CSenWSDescription& aPattern);
	    /** Improved heuristics from credential DB removal / cleanups
	    *
	    * Checks touch attribute value for all CredentialContainer child elements of 
	    * Credentials element if that exceeds the the tick counts for two weeks 
	    * (Max tick count) or if it is greater than current tick count it removes the 
	    * corresponding CredentialContainer child element from the Credentials
	    * element child list.
	    * 
	    * 1 millisec = 1 tick
		* 86400 sec = 1 day
		* 86400*1000 millisec = 1 day
		* 86400*14*1000 = 1209600000 millisec = 2 weeks
		* So if attr val >= 1209600000 or > current tick count.
		* Note: always attr val should be <= current tick count.
		*
	    * eg;
	    * Before call to CleanupUnusedCredDBL()
	    * <Credentials CredentialsMaxID="3">
		* <CredentialContainer>
		* <Identifier>
  		* <CredentialId>1</CredentialId>
		* ..........
		* ..........
		* </CredentialContainer>
		* <CredentialContainer touch="1000">
		* <Identifier>
  		* <CredentialId>2</CredentialId>
		* ..........
		* .........
		* </CredentialContainer touch="1309600000">
		* <CredentialContainer>
		* <Identifier>
  		* <CredentialId>3</CredentialId>
		* ...........
		* ...........
		* </CredentialContainer>
		* </Credentials>
		*
		* After call to CleanupUnusedCredDBL()
	    * <Credentials CredentialsMaxID="3">
		* <CredentialContainer>
		* <Identifier>
  		* <CredentialId>1</CredentialId>
		* .........
		*..........
		* </CredentialContainer>
		* <CredentialContainer touch="1000">
		* <Identifier>
  		* <CredentialId>2</CredentialId>
		* ..........
		* ..........
		* </CredentialContainer>
		* </Credentials>
		* 
		* Leave codes: Symbian system-wide error codes.
	    */
	    void CleanupUnusedCredDBL();
	    TBool IsAuto(CSenIdentityProvider& aIdp);
#ifdef __CRYPTO_HW__									
        void EncryptPasswordL(const TDesC8& aData, TDes8& aCipherText);
		
		void DecryptPasswordL(const TDesC8& aData, TDes8& aCipherText);

#endif // __CRYPTO_HW__
	    
    private: // Data
        MSenCoreServiceManager&     iManager;
        RFileLogger*                iLog;                   // not owned (no create/close)    
		CSenParser*                 ipParser;               // owned 
		RSenCredentialPtrArray      iCredentialArray;       // owned
		
		CSenFragmentBase*           ipCredentialContainer;  // owned
		CSenCredentialIdentifier*   ipCredentialIdentifier; // owned
		CSenCredentialProperties*   ipCredentialProperties; // owned
		CSenInternalCredential*     ipCredential;           // owned
		
		TInt                        iMaxCredentialID;
    };

#endif // SEN_BASE_CREDENTIAL_MANAGER_H

// End of File
