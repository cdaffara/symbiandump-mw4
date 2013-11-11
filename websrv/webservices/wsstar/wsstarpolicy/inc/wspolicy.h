/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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






 

#ifndef WS_POLICY_H
#define WS_POLICY_H

// INCLUDES
#include <e32std.h>

#include "SenDomFragment.h"
#include <SenDomFragmentBase.h>
#include "policyassertion.h"
#include "xorcompositeassertion.h"
#include "andcompositeassertion.h"
#include "wspolicyreader.h"
#include "wspolicyutils.h"
// FORWARD DECLARATIONS
class CWSPolicyReader;
class CWSPolicyWriter;
class CMyXml;
/**
 * Class: CWSPolicy
 * Description: Public interface to a policy. A policy should be created by this class. 
 * The policy will be stored as XML-DOM tree in memory and can be used to do different 
 * kind of processings on the policy. i.e. Normalizing, intersecting and merging
 */
class CWSPolicy : public CBase
    {

    public:
        //public two phase constructors
        //still there is no purpose to create a policy with empty arguments ??
        IMPORT_C    static CWSPolicy* NewL();
        IMPORT_C    static CWSPolicy* NewLC();
        //creates a policy from a descriptor
        IMPORT_C    static CWSPolicy* NewL(TDesC8& aPolicyUri, TDesC8& aPolicy);
        IMPORT_C    static CWSPolicy* NewLC(TDesC8& aPolicyUri, TDesC8& aPolicy);
        //creates a policy from CSenElement    
        IMPORT_C    static CWSPolicy* NewL(CSenElement* aPolicy);
        IMPORT_C    static CWSPolicy* NewLC(CSenElement* aPolicy);
        //creates a policy from TElement//libxml2
        //IMPORT_C    static CWSPolicy* NewL(TElement aPolicy);
        //IMPORT_C    static CWSPolicy* NewLC(TElement aPolicy);

        IMPORT_C    TInt SetPolicyL(TDesC8& aPolicyUri, TDesC8& aPolicy);
        //Normalize a policy using a registry to solve references
        //returns ETrue if normalization is successfult else EFalse
        IMPORT_C    TBool NormalizePolicyL(CPolicyRegistry* aRegistry);
        CSenElement*  NormalizePolicy2L(CSenElement& aPolicy, CPolicyRegistry* aRegistry);
        //Intersect a policy with second policy provided as a parameter
        //return intersected policy
        IMPORT_C    CWSPolicy* IntersectPolicyL(CWSPolicy* aPolicy);
        //Merge a policy with second policy provided as a parameter
        //return merged policy
        IMPORT_C    CWSPolicy* MergePolicyL(CWSPolicy* aPolicy);
            
        //returns ETrue if the policy is normalized else EFalse
        IMPORT_C    TBool Normalized();

        IMPORT_C    HBufC8* PolicyAsXmlL();
        IMPORT_C    CSenElement* PolicyL();	//codescannerwarnings
        IMPORT_C    HBufC8* NormalizedAsXmlL();
        IMPORT_C    CSenElement* NormalizedPolicy();
        IMPORT_C    TBool IsExpiredPolicyL();	//codescannerwarnings
        IMPORT_C    CSenElement* NonNormalizedPolicyL();	//codescannerwarnings
        ~CWSPolicy();   
        
    private:
        CWSPolicy();
        void ConstructL();
        TInt NormalizePL(CPolicyRegistry* aRegistry);
        CWSPolicy* MergePL(CWSPolicy* aPolicy);
        CWSPolicy* IntersectPL(CWSPolicy* aPolicy);	//codescannerwarnings  

        void GeneratePolicyIdL(CSenElement* aPolicy);	//codescannerwarnings 
        TPtrC8 PolicyIdL(CSenElement* aPolicy);
        TPtrC8 PolicyNameL(CSenElement* aPolicy);
        TInt AddPolicyNameL(CSenElement* aPolicy);
        TInt AddPolicyMetadatEndpointL(CSenElement* aPolicy);
        TInt AddPolicyIdL(CSenElement* aPolicy);
        TInt AddPolicyValidityL(CSenElement* aPolicy);
        TInt EvaluateExpiryL(CSenElement* aPolicy);	//codescannerwarnings 
        CSenElement* GetExpiryTagL();	//codescannerwarnings 
        void BaseConstructL(TDesC8& aPolicyUri, TDesC8& aPolicy);
        void BaseConstructL(CSenElement* aPolicy);

        MAssertion* NormaAssertion();

    private: // Data

        CWSPolicyReader* iReader;
        CWSPolicyWriter* iWriter;
        CMyXml* iInDoc;
        CMyXml* iOutDoc;

        CSenElement* iNormPolicy; //owned           
        CPolicyAssertion* iNormaPolicyAssert; //owned
        TBool iNormalized;
        TBool iExpired;
        HBufC8* iPolicyId;
        HBufC8* iPolicyName;
        HBufC8* iPolicyUri;
        TInt count;
        TTime iExpiry;
    };

class CMyXml :public CSenDomFragment
{
public:    
    static CMyXml* NewL();
    static CMyXml* NewLC();
    ~CMyXml();
    
    TInt ReadFromDescL(const TDesC8& aData);
    CSenElement* XMLDocL();
private:

    CMyXml();
    void ConstructL();
    TBool iParsed;

};   
#endif // WS_POLICY_H
// END OF FILE
