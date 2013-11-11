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
* Description:        The SenProviderPolicy is a non-public implementation of
*                a "ProviderPolicy", which offers functionality for classes
*                implementing MSenProviderPolicy interface. Those classes
*                typically delegate their function calls to ready methods
*                defined in this class.
*
*/








#ifndef SEN_PROVIDER_POLICY_H
#define SEN_PROVIDER_POLICY_H

//  INCLUDES
#include <e32base.h> // for CActive
#include <badesca.h>

#include "SenDomFragment.h"
#include "MSenProviderPolicy.h"
#include "senpolicy.h"

// FORWARD DECLARATIONS
class CSenIdentityProviderIdArray8;

// CLASS DECLARATION

/**
* Example structure of ProviderPolicy XML fragment:
*
*   <ProviderPolicy>
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
*   </ProviderPolicy>
*/
class CSenProviderPolicy : public CSenPolicy
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructors.
        * These parameterless constructors are ment to be used
        * for example before parsing XML stream.
        */
        IMPORT_C static CSenProviderPolicy* NewL();
        IMPORT_C static CSenProviderPolicy* NewLC();
        IMPORT_C static CSenProviderPolicy* NewL(TUint32 aIapId);
        IMPORT_C static CSenProviderPolicy* NewLC(TUint32 aIapId);
        IMPORT_C static CSenProviderPolicy* NewL(TUint32 aId, TBool aIsSnapId);
        IMPORT_C static CSenProviderPolicy* NewLC(TUint32 aId, TBool aIsSnapId);        

        virtual ~CSenProviderPolicy();
        
        // New functions
        
        /**
        * Checks if policies match.
        * @since    Series60 3.0
        * @param    aPolicyPattern is the policy being compared 
        *           with this instance.
        * @return   ETrue if policy candidate is accepted by current policy
        *           definition, EFalse if not.
        */
        IMPORT_C virtual TBool Accepts(MSenProviderPolicy& aPolicyPattern);

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
        IMPORT_C virtual TInt RebuildFrom(MSenProviderPolicy& aTemplate);
       
        
        /**
        * Sets transport specific properties.
        * @param aProperties contains the transport properties in serialized (UTF-8) form.
        * With HTTP, this descriptor is typically created by utilizing CSenHttpProperties class.
        * @return KErrNone if successful or otherwise some system-wide error code.
        */
        IMPORT_C TInt SetTransportPropertiesL(const TDesC8& aProperties);


        /*
        * Method returns currently effective transport properties as a serialized
        * descriptor. With HTTP, one may utilize CSenHttpProperities class in
        * order to read this data into properties object. Note that returned 
        * list of properties also includes properties, which are defined as default
        * for certain transport in the webservice framework.
        */
        IMPORT_C TInt TransportPropertiesL(HBufC8*& aProperties);
    private: 
        
        /**
        * C++ default constructor.
        */
        CSenProviderPolicy();
        
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
        TBool AcceptsL(MSenProviderPolicy& aPolicyPattern);
        TInt RebuildFromL(MSenProviderPolicy& aTemplate);
    };

#endif //SEN_PROVIDER_POLICY_H

// End of File
