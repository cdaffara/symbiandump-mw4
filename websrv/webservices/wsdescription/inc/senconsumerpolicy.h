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
* Description:        The SenConsumerPolicy is a non-public implementation of
*                a ConsumerPolicy, which offers functionality for classes
*                implementing MSenConsumerPolicy interface. Those classes
*                typically delegate their function calls to ready mehthods
*                defined in this class.
*
*/








#ifndef SEN_CONSUMER_POLICY_H
#define SEN_CONSUMER_POLICY_H

//  INCLUDES
#include <e32base.h> // for CActive
#include <badesca.h>

#include "SenDomFragment.h"
#include "MSenConsumerPolicy.h"
#include "senpolicy.h"

// FORWARD DECLARATION
class CSenIdentityProviderIdArray8;

// CLASS DECLARATION

/**
* Example structure of ConsumerPolicy> XML fragment:
*
*   <ConsumerPolicy>
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
*   </ConsumerPolicy>
*/
class CSenConsumerPolicy : public CSenPolicy
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructors.
        * These parameterless constructors are ment to be used
        * for example before parsing XML stream.
        */
        IMPORT_C static CSenConsumerPolicy* NewL();
        IMPORT_C static CSenConsumerPolicy* NewLC();

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CSenConsumerPolicy* NewL(TUint32 aIapId);
        IMPORT_C static CSenConsumerPolicy* NewLC(TUint32 aIapId);
        
        IMPORT_C static CSenConsumerPolicy* NewL(TUint32 aId, TBool aIsSnapId);
        IMPORT_C static CSenConsumerPolicy* NewLC(TUint32 aId, TBool aIsSnapId);        

        // Functions from base classes
    
        // From MSenConsumerPolicy 
    
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
        IMPORT_C virtual TInt RebuildFrom(MSenConsumerPolicy& aTemplate);

        /**
        * Checks if service pattern accepts given policy.
        * @param aPolicyPattern the consumer policy to check.
        * @return ETrue if accepts, EFalse if not
        */
        IMPORT_C virtual TBool Accepts(MSenConsumerPolicy& aPolicyPattern);

        /**
        * Destructor
        */
        virtual ~CSenConsumerPolicy();

    private:
    
        /**
        * C++ default constructor.
        */
        CSenConsumerPolicy();

        /**
        * By default Symbian 2nd phase constructors are private.
        */
        IMPORT_C void BaseConstructL();
        IMPORT_C void BaseConstructL(TUint32 aIapId);
        IMPORT_C void BaseConstructL(TUint32 aId, TBool aIsSnapId);
        
        
        // New functions
        
        /**
        * Leaving variants of the functions.
        */
        TBool AcceptsL(MSenConsumerPolicy& aPolicyPattern);
        TInt RebuildFromL(MSenConsumerPolicy& aTemplate);
    };

#endif //SEN_CONSUMER_POLICY_H

// End of File
