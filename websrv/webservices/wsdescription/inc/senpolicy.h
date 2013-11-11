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








#ifndef SEN_POLICY_H
#define SEN_POLICY_H

//  INCLUDES
#include <e32base.h> // for CActive
#include <badesca.h>

#include "SenDomFragment.h"
#include "msenpolicy.h"

// FORWARD DECLARATION
class CSenIdentityProviderIdArray8;

// CLASS DECLARATION

/**
* Example structure of Policy XML fragment:
*   <Policy>
*
*       <Transport>
*           <IapId>
*               1234567890
*           </IapId>
*       </Transport>
*
*       <IdentityProviderIDs strict="true">
*           <IdentityProviderID>
*             urn:first.id.example.com
*           </IdentityProviderID>
*           <IdentityProviderID>
*             urn:second.id.example.com
*           </IdentityProviderID>
*       </IdentityProviderIDs>
*
*   </Policy>
*/
class CSenPolicy : public CSenDomFragment, public MSenPolicy
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructors.
        * These parameterless constructors are ment to be used
        * for example before parsing XML stream.
        */
        IMPORT_C static CSenPolicy* NewL();
        IMPORT_C static CSenPolicy* NewLC();
        IMPORT_C static CSenPolicy* NewL(TUint32 aIapId);
        IMPORT_C static CSenPolicy* NewLC(TUint32 aIapId);
        IMPORT_C static CSenPolicy* NewL(TUint32 aId, TBool aIsSnapId);
        IMPORT_C static CSenPolicy* NewLC(TUint32 aId, TBool aIsSnapId);        

        /**
        * Destructor
        */
        virtual ~CSenPolicy();
        
        // Functions from base classes
        
        // From MSenPolicy
    
        /**
        * Setter for Internet Access Point (IAP) ID.
        * Method is used to define default access point
        * to avoid IAP selection dialog prompt.
        * @since Series60 3.0
        * @param aIapId is the ID found from CommDB.
        */ 
        IMPORT_C virtual void SetIapIdL(TUint32 aIapId);

        /**
        * Getter for IAP ID.
        * @param aCurrentIapId will contain the current Internet Access Point ID
        * Max value for IAP ID is 2147483647
        * @return
        *  - KErrNone if <IapId> element is found and has valid content
        *  - KErrNotFound if no <IapId> element has been found or it has
        *       no content
        *  - TLex error codes, if <IapId> element has non-numeric value,
        *    which cannot be resolved
        */ 
        IMPORT_C virtual TInt IapId(TUint32& aCurrentIapId);

        /**
        * Setter for IdentityProvider IDs. Overrides 
        * current values with the given values from the array.
        * When initiating a new service connection, this list of
        * ProviderIDs define, which IdentityProviders are preferred
        * OR which are strictly acceptable ones.
        * @since Series60 3.0
        * @param aList  list of IdentityProvider IDs.
        */ 
        IMPORT_C virtual void SetIdentityProviderIdsL(
                                        CSenIdentityProviderIdArray8& aList);

        /**
        * Adds a new Identity provider ID value at the end of the current list 
        * values
        * Checks for duplicate (does not insert new ID elements with equal
        * content)
        * @return   KErrAlreadyExists, if a duplicate is tried to add
        *           KErrArgument if a zero-length descriptor is tried to add
        *           (aProviderId.Length() == 0)
        */
        IMPORT_C virtual TInt AddIdentityProviderIdL(const TDesC8& aProviderId);

        /**
        * Method reads provider policy class instance values from given
        * template. Values may include an IAP ID, preferred IdentityProvider
        * IDs etc.
        * @since Series60 3.0
        * @param aTemplate from which the values are read.
        * @return       KErrNone            Everything went ok.
        *               KErrAlreadyExists   Duplicates were found
        *               KErrArgument        Invalid providerIds were found
        *               Other error codes are system-wide Symbian error codes.
        */ 
        IMPORT_C virtual TInt RebuildFrom(MSenPolicy& aTemplate);

        /**
        * Getter for IdentityProvider IDs.
        * @since Series60 3.0
        * @return a list of IDP arrays if such value(s) have been set
        *           or an empty array if no IDP:s have been specified.
        */ 
        IMPORT_C virtual const CSenIdentityProviderIdArray8&
                                                    IdentityProviderIds8L();
        /**
        * Checks if policies match.
        * @param    aPolicyPattern  the policy to match
        * @return ETrue if policies match, EFalse if not.
        */
        IMPORT_C virtual TBool Accepts(MSenPolicy& aPolicyPattern);
        
        /**
        * Sets the SnapId
        * @param    aSnapId snap id
        */
        IMPORT_C virtual void SetSnapIdL(TUint32 aSnapId) ;
        
        /**
        * Gets the SnapId
        * @param    aCurrentSnapId snap id
        * @return KErrNone if snap id is get properly
        */
        IMPORT_C virtual TInt SnapId(TUint32& aCurrentSnapId) ;         

    protected: 
        
        /**
        * C++ default constructor.
        */
        CSenPolicy();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        IMPORT_C void BaseConstructL(const TDesC8& aLocalName);
        IMPORT_C void BaseConstructL(const TDesC8& aLocalName,
                                     TUint32 aIapId);
        IMPORT_C void BaseConstructL(const TDesC8& aLocalName,
                                     TUint32 aId, TBool aIsSnapId);
    
    private: // New functions
    
        /**
        * Leaving variants of the functions.
        */
        TBool AcceptsL(MSenPolicy& aPolicyPattern);
        TInt RebuildFromL(MSenPolicy& aTemplate);

    private: // Data
        CSenIdentityProviderIdArray8* iIdpIdList;
    };

#endif //SEN_POLICY_H

// End of File
