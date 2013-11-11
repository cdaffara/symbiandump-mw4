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








#ifndef M_SERVICE_POLICY_H
#define M_SERVICE_POLICY_H

//  INCLUDES
#include <e32base.h> // for CActive
#include <badesca.h>

#include "SenDomFragment.h"
typedef RPointerArray<CSenElement> RPolicyArray;

// FORWARD DECLARATION

// CLASS DECLARATION

/**
* Example structure of Policy XML fragment:
*
*   <ServicePolicy>
*       <ClientPolicy>
*			<MetadataEndpoint>uri.1</MetadataEndpoint>
*			<MetadataEndpoint>uri.2</MetadataEndpoint>
*			<MetadataEndpoint>uri.3</MetadataEndpoint>
*           <PolicyValue1/>				
*           <PolicyValue2/>				
*       </ClientPolicy>
*       <PolicyArrray>
*			<wsp:Policy1/>
*			<wsp:Policy2/>
*       </PolicyArrray>
*   </ServicePolicy>
*/
class MServicePolicy
    {
    public: // New functions

        /**
        * Adds a policy in SD
        * Method is used to add a RAW policy obtained 
        * by Network or some dynamic means
        * @since Series60 4.0
        * @param aPolicy is the Policy Element
        */ 
        virtual TInt AddPolicyL(CSenElement* aPolicy) = 0;

        /**
        * Remove  a Policy from SD
        * @param aPolicyName , Policy with this name will be removed from SD
        * @return
        *  - KErrNone 
        
        */ 
        virtual TInt RemovePolicyL(TDesC8& aPolicyName) = 0;

        /**
        * Method reads provider policy class instance values from given
        * template. 
        * @since Series60 4.0
        * @param aTemplate from which the values are read.
        * @return   
        *           
        */ 
        virtual TInt RebuildServicePolicyFrom(MServicePolicy& aTemplate) = 0;
        
        /**
        * Checks if policies match.
        * @return ETrue if policies match, EFalse if not.
        */
        virtual TBool Accepts(MServicePolicy& aPolicyPattern) = 0;
       
        /**
        * Getter for ServicePolicies
        * @since Series60 4.0
        * @return 
        *           
        */ 

        virtual TInt ServicePolicies(RPolicyArray& aPolicies) = 0;
    };

#endif //M_SEN_POLICY_H

// End of File
