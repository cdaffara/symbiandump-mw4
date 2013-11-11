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








#ifndef M_SEN_POLICY_H
#define M_SEN_POLICY_H

//  INCLUDES
#include <e32base.h> // for CActive
#include <badesca.h>

#include "SenDomFragment.h"

// FORWARD DECLARATION
class CSenIdentityProviderIdArray8;

// CLASS DECLARATION

/**
* Example structure of Policy XML fragment:
*
*   <Policy>
*       <Transport>
*           <IapId>
*               1234567890
*           </IapId>
*       </Transport>
*       <IdentityProviderIDs strict="true">
*           <IdentityProviderID>
*             urn:first.id.example.com
*           </IdentityProviderID>
*           <IdentityProviderID>
*             urn:second.id.example.com
*           </IdentityProviderID>
*       </IdentityProviderIDs>
*   </Policy>
*/
class MSenPolicy
    {
    public: // New functions

        /**
        * Setter for Internet Access Point (IAP) ID.
        * Method is used to define default access point
        * to avoid IAP selection dialog prompt.
        * @since Series60 3.0
        * @param aIapId is the ID found from CommDB.
        */ 
        virtual void SetIapIdL(TUint32 aIapId) = 0;

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
        virtual TInt IapId(TUint32& aCurrentIapId) = 0;

        /**
        * Setter for IdentityProvider IDs. Overrides 
        * current values with the given values from the array.
        * When initiating a new service connection, this list of
        * ProviderIDs define, which IdentityProviders are preferred
        * OR which are strictly acceptable ones.
        * @since Series60 3.0
        * @param aList  list of IdentityProvider IDs.
        */ 
        virtual void SetIdentityProviderIdsL(
                                    CSenIdentityProviderIdArray8& aList) = 0;

        /**
        * Adds a new Identity provider ID value at the end of the current list 
        * values
        * Checks for duplicate (does not insert new ID elements with equal
        * content)
        * @return   KErrAlreadyExists, if a duplicate is tried to add
        *           KErrArgument if a zero-length descriptor is tried to add
        *           (aProviderId.Length() == 0)
        */
        virtual TInt AddIdentityProviderIdL(const TDesC8& aProviderId) = 0;

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
        virtual TInt RebuildFrom(MSenPolicy& aTemplate) = 0;

        /**
        * Getter for IdentityProvider IDs.
        * @since Series60 3.0
        * @return a list of IDP arrays if such value(s) have been set
        *           or an empty array if no IDP:s have been specified.
        */ 
        virtual const CSenIdentityProviderIdArray8& IdentityProviderIds8L() = 0;

        /**
        * Checks if policies match.
        * @return ETrue if policies match, EFalse if not.
        */
        virtual TBool Accepts(MSenPolicy& aPolicyPattern) = 0;
        
        virtual void SetSnapIdL(TUint32 aSnapId) = 0;
        
        virtual TInt SnapId(TUint32& aCurrentSnapId) = 0;
    };

#endif //M_SEN_POLICY_H

// End of File
