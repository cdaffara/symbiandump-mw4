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
* Description:    Header declaration
*
*/











#ifndef SERVICE_POLICY_H
#define SERVICE_POLICY_H

//  INCLUDES
#include <e32base.h> // for CActive
#include <badesca.h>
// INCLUDES
#include <flogger.h>        
#include "SenDomFragment.h"
#include "MSenServicePolicy.h"
#include "SenBaseFragment.h"

// FORWARD DECLARATION
class CSenIdentityProviderIdArray8;

// CLASS DECLARATION

/**
/**
* Example structure of ServicePolicy XML fragment:
*
*   <ServicePolicy>
*   </ServicePolicy>
*/
class CServicePolicy : public CSenDomFragment, public MSenServicePolicy
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructors.
        * These parameterless constructors are ment to be used
        * for example before parsing XML stream.
        */
        static CServicePolicy* NewL();
        static CServicePolicy* NewLC();

        /**
        * Destructor
        */
        virtual ~CServicePolicy();
        
        // Functions from base classes
        
        // From MServicePolicy
        virtual TInt AddPolicyL(CSenElement* aPolicy);
        virtual TInt RemovePolicyL(TDesC8& aPolicyName);
        virtual TInt ServicePolicies(RPolicyArray& aPolicies);
        virtual TInt RebuildServicePolicyFrom(MSenServicePolicy& aTemplate);
        virtual TBool Accepts(MSenServicePolicy& aPolicyPattern);
        virtual CSenElement* ClientPolicy();
        virtual TBool ClientPolicy(const TDesC8& aKey);
        virtual HBufC8* ClientPolicyValue(const TDesC8& aKey);        
        TInt AddClientPolicyL(CSenElement* aTemplate);                    
        
    protected: 
        
        /**
        * C++ default constructor.
        */
        CServicePolicy();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void BaseConstructL(const TDesC8& aLocalName);
        //RFileLogger* Log() const;        
    
    private: // New functions
    
        /**
        * Leaving variants of the functions.
        */
        TBool AcceptsL(MSenServicePolicy& aPolicyPattern);
        TInt RebuildServicePolicyFromL(MSenServicePolicy& aTemplate);
        
        
        TInt AddWspPolicyL(CSenElement* aPolicy);
        TBool ValidatePolicy(CSenElement* aPolicy);
        TPtrC8 PolicyIdL(CSenElement* aPolicy);	//codescannerwarnings
        TPtrC8 PolicyNameL(CSenElement* aPolicy);	//codescannerwarnings
        TPtrC8 PolicyUriL(CSenElement* aPolicy);
        void GenerateDeviceIdL(CSenElement* aPolicy);	//codescannerwarnings
        
        void AddApplicationInfoElementsL(CSenElement* aApplicationInfoElement);
        void AddAppicationElementsL(CSenElement* aApplicationElement);
        void AddDeviceElementsL(CSenElement* aDeviceElement);
        void AddUserAgentElementsL(CSenElement* aUserAgentElement);
        void AddOSElementsL(CSenElement* aOSElement);

    private: // Data
//        CSenIdentityProviderIdArray8* iIdpIdList;
        //RFileLogger iLog;
    };

#endif //SEN_POLICY_H

// End of File
