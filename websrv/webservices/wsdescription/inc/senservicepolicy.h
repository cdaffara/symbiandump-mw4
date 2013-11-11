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
* Description:        The SenServicePolicy is a non-public implementation of
*
*/








#ifndef SEN_SERVICE_POLICY_H
#define SEN_SERVICE_POLICY_H

//  INCLUDES
#include <e32base.h> // for CActive
#include <badesca.h>

#include "SenDomFragment.h"
#include "MSenServiceDescription.h"
#include "servicepolicy.h"
#include "MSenServicePolicy.h"
// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
* Example structure of ServicePolicy XML fragment:
*
*	<ServicePolicy>
*
*	</ServicePolicy>
*/
class CSenServicePolicy : public CServicePolicy
    {
	public: // Constructors and destructor

		/**
     	* Two-phased constructors.
	 	* These parameterless constructors are ment to be used
	 	* for example before parsing XML stream.
     	*/
    	static CSenServicePolicy* NewL();
    	static CSenServicePolicy* NewLC();

		virtual ~CSenServicePolicy();
        // From CServicePolicy
        virtual TInt RebuildServicePolicyFrom(MSenServicePolicy& aTemplate);
        virtual TBool Accepts(MSenServicePolicy& aPolicyPattern);
        virtual CSenElement* ClientPolicy();
 	private: 
		/**
        * C++ default constructor.
        */
		CSenServicePolicy();
        void ConstructL();		
		/**
        * By default Symbian 2nd phase constructors are private.
        */
		void BaseConstructL();
    };

#endif //SEN_SERVICE_POLICY_H

// End of File
