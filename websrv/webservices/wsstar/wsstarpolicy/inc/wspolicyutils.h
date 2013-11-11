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
* Description:        This utility class provides static methods for conversions*
*/








#ifndef WS_POLICYC_UTILS_H
#define WS_POLICYC_UTILS_H

//  INCLUDES
#include <e32base.h>
#include "massertion.h"
// CLASS DECLARATION
class CPolicyRegistry;
class CPolicyAssertion;
/**
*  @lib WSPolicy.lib
*  @since Series60 3.0
*/
class CWSPolicyUtils : public CBase
    {
    public: 

        static CPolicyAssertion* GetPolicy(RPolicyTerms& terms);
        static CPolicyAssertion* GetSinglePolicyL(RPolicyTerms& policyList, CPolicyRegistry* reg);
        static void GetPrimTermsListL(CPolicyAssertion* policy, RPolicyTerms& aTerms); 
        
        IMPORT_C static TPtrC8 PolicyNameL(CSenElement* aPolicy);

        /**
        */
        IMPORT_C static TPtrC8 PolicyIdL(CSenElement* aPolicy);
        
        IMPORT_C static TPtrC8 PolicyUriL(CSenElement* aPolicy);
                /**
        */
        IMPORT_C static TBool IsExpiredPolicyL(CSenElement* aPolicy);


    private:
        /**
        * Hide default C++ constructor.
        */
        CWSPolicyUtils() { }

        /**
        * Prohibit copy constructor if not deriving from CBase.
        */
        CWSPolicyUtils( const CWSPolicyUtils& );

        /**
        * Prohibit assignment operator if not deriving from CBase.
        */
        CWSPolicyUtils& operator=( const CWSPolicyUtils& );
        
        static CSenElement* GetExpiryTagL(CSenElement* aPolicy);
    };

#endif // WS_POLICYC_UTILS_H
// End of File
