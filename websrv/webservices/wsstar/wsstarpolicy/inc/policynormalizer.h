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






 

#ifndef POLICY_NORMALIZER_H
#define POLICY_NORMALIZER_H

#include <e32std.h>
#include "SenDomFragment.h"
#include "SenPointerMap.h"

// FORWARD DECLARATIONS
class CPolicyNormalizer;
class CPolicyRegistry;
class CWSPolicy;
typedef RSenPointerMap<TDesC8, CSenElement> RPolicyMap;
// CONSTANTS


// INCLUDES
namespace  WSPolicy
    {
    
	_LIT8( KAndCompositeAssertion , "All");             // Tag name of AndComposteAssertion
	_LIT8( KXorCompositeAssertion , "ExactlyOne");      // Tag name of XorCompositeAssertion
	_LIT8( KWsPolicy , "Policy");                       // Tag name of Policy
	_LIT8( KWsPolicyPrefix , "wsp");                    // Prefix of WS-Policy namespace
	_LIT8( KWsuNsPrefix , "wsu");                       // Prefix of WSU namespace
	_LIT8( KWsPolicyReference , "PolicyReference");     // Tag name of PolicyReference
	_LIT8( KWspOptinal , "wsp:Optional");             // Tag name of Optional Attribute
	_LIT8( KWspOptinalTrue , "true");             // Tag name of Optional Attribute TRUE
	_LIT8( KWspOptinalFalse , "false");             // Tag name of Optional Attribute FALSE
	_LIT8( KWsPolicyNsUri , "http://schemas.xmlsoap.org/ws/2004/09/policy"); // Namespace of WS-Policy
    
    _LIT8( KWsuPrefix , "wsu"); // Namespace of WSU	
	_LIT8( KWsuNsUri , "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd"); // Namespace of WSU
	_LIT8( KXmlNsUri , "http://www.w3.org/XML/1998/namespace"); // XML namespace
	
	_LIT8( KWsuId , "wsu:Id");                       // Prefix ID
	_LIT8( KId , "Id");                       // Prefix ID
	_LIT8( KName, "Name"); 
	_LIT8( KWsSecuritypolicyPrefix , "sp");                    // Prefix of WS-Security Policy namespace
	_LIT8( KWsSecurityPolicyNsUri , "http://schemas.xmlsoap.org/ws/2005/07/securitypolicy"); // Namespace of WS-Security Policy
	
	_LIT8( KUri , "URI");                       // PolicyReference URI
    }

namespace  MSWSPolicy
{
 	_LIT8( KMetaDataValidity, "metadatavalidity");
	_LIT8( KMetaDataNsUri, "http://schemas.live.com/mws/2005/08/core");
	_LIT8( KExpiry, "Expiry");
	_LIT8(KMetaDataLocalName, "MetadataEndpoint");    
}


/**
 * Class:   CPolicyNormalizer
 * Description: This class creates an instance of PolicyNormalizer. The DOM of policy is created by calling ReadL(TDesC8& aXmlDesc) method of the 
 * class. Then The DOM tree can be normalized as described in WS-Policy specification.
 */
class CPolicyNormalizer : public CSenDomFragment
    {

public:
    static CPolicyNormalizer* NewL();
    static CPolicyNormalizer* NewLC();
    ~CPolicyNormalizer();
    
    void ReadFileL(TFileName aPath);
    TInt ReadL(TDesC8& aXmlDesc);
    
    CSenElement* NormalizeL(CPolicyRegistry* aRegistry);
    CSenElement* NormalizedPolicy() {return  iNewPolicy; }
private:       

    CPolicyNormalizer();
    void ConstructL();
    void WriteL(CSenElement* aXml);
    void WriteAllL(CSenElement* aXml);

    TBool ValidateElementNsL(CSenElement* aAssertion);
    TBool IsOptionalL(CSenElement* aAssertion);               //codescannerwarnings

    CSenElement* ProcessPolicyReferenceL(CSenElement* aPolicy, CPolicyRegistry* aRegistry);
    TInt ReplacePolicyReferenceL(CSenElement* aReferenceElement, CSenElement* aReferredPolicy);	//codescannerwarnings
    TPtrC8 GetReferenceUriL(CSenElement* aReference);          //codescannerwarnings
    
    CSenElement* ProcessXORTermL(CSenElement* aAssertion, CSenElement* aNormalAssertion);
    CSenElement* ProcessANDTermL(CSenElement* aAssertion, CSenElement* aNormalAssertion);
    CSenElement* ProcessPolicyTermL(CSenElement* aAssertion, CSenElement* aNormalAssertion);
    CSenElement* ProcessAssertionTermL(CSenElement* aAssertion, CSenElement* aNormalAssertion);
    CSenElement* ProcessOptionalElement(CSenElement* aAssertion, CSenElement* aNormalAssertion);
    
    
    CSenElement* AddAndElementL(CSenElement* aAssertion);
    CSenElement* AddXorElementL(CSenElement* aAssertion);
    CSenElement* AddPolicyElementL(CSenElement* aAssertion);
    
    CSenElement* CopySenElementL(CSenElement* aSource,CSenElement* aDestination);
    TBool CopySenElementWithoutOptionL(CSenElement* aSource,CSenElement* aDestination);	//codescannerwarnings
    
    CSenElement* ProcessLogicL(CSenElement* aTerm);     //codescannerwarnings
    CSenElement* ProcessXorCollapseLogicL(CSenElement* aParent, CSenElement* aChild);  //codescannerwarnings
    CSenElement* ProcessAndCollapseLogic(CSenElement* aParent, CSenElement* aChild);
    CSenElement* ProcessAndAndLogicL(CSenElement* aFirstTerm, CSenElement* aSecondTerm ); //codescannerwarnings
    
    CSenElement* ProcessAssertionBranchingL(CSenElement* aPolicy);    //codescannerwarnings
    CSenElement* DuplicateAssertionBranchL(CSenElement* aParent, CSenElement* aDuplicatingPolicy);   //codescannerwarnings
    
    TInt HasChildL(CSenElement* aTerm);  //codescannerwarnings
    CSenElement* FindRootAssertionL(CSenElement* aElement);  //codescannerwarnings
    TBool IsChildOfElement(CSenElement* aParent, CSenElement* aChild);
    CSenElement* DeleteAsertionIndexL(CSenElement* aAssertion, CSenElement* anewAssertion, TInt aIndx);  //codescannerwarnings

    HBufC*        iPath;
    CSenElement* iNewPolicy;
    RPointerArray<CSenElement> iOrphanElements;
    TInt count;

    };
#endif // POLICY_NORMALIZER_H
// END OF FILE
