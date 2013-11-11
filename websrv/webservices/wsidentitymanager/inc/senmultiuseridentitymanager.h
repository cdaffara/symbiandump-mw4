/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef SEN_MULTIUSER_IDENTITY_MANAGER_H
#define SEN_MULTIUSER_IDENTITY_MANAGER_H

// INCLUDES
#include <e32std.h>
#include <ecom.h>
#include <flogger.h>    // RFileLogger
#include <badesca.h>    // CDesC8Array

#include <SenIdentityProvider.h>
#include <SenBaseFragment.h>

#include "senbaseidentitymanager.h"
#include "msenidentitymanager.h" 
#include "senuser.h" 
#include "senpointermap.h" 





// FORWARD DECLARATIONS
class MSenCoreServiceManager;

// CLASS DECLARATION
class CSenMultiUserIdentityManager : public CSenBaseIdentityManager
    {
    public: // Constructors and destructor
    
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CSenMultiUserIdentityManager* NewL(
                                            MSenCoreServiceManager& aManager);
        IMPORT_C static CSenMultiUserIdentityManager* NewLC(
                                            MSenCoreServiceManager& aManager);

        IMPORT_C virtual ~CSenMultiUserIdentityManager();
        


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
                                    
        virtual void EndElementL(const TDesC8& aNsUri,
                                 const TDesC8& aLocalName,
                                 const TDesC8& aQName);

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
   
     
    private: 
    
        /**
        * C++ default constructor.
        */
        CSenMultiUserIdentityManager(MSenCoreServiceManager& aManager);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
        // New functions
        
        /**
        * Save the current state of the SenMultiUserIdentityManager as an XML
        * document into a file.
        * Status codes:
        *   KErrNone                Ok
        *   Other codes are system error codes.
        * @return status code.
        */
        TInt WriteConfigurationToL( const TDesC& aFile );
        
        
        CSenBaseIdentity* iActiveIdentity;
        CSenUser* iParsedUser;//used only while parsing
        CSenUser* iActiveUser;
        RPointerArray<CSenUser> iUsers;
    };

#endif // SEN_BASE_IDENTITY_MANAGER_H

// End of File
