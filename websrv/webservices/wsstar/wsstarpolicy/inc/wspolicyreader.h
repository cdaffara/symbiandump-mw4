/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef C_WS_POLICY_READER_H
#define C_WS_POLICY_READER_H


#include <e32std.h>
#include <f32file.h>

#include "SenDomFragment.h"
#include "policyassertion.h"
#include "primitiveassertion.h"

class CPolicyAssertion;
class CAndCompositeAssertion;
class CXorCompositeAssertion;
class CAssertion;
class CPrimitiveAssertion;
class CPolicyReferenceAssertion;
namespace WSPolicy
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
 	_LIT8( KMSNsPrefix, "mws");
 	_LIT8( KMetaDataValidity, "metadatavalidity");
	_LIT8( KExpiry, "Expiry");
	_LIT8(KMetaDataLocalName, "MetadataEndpoint");    
}

class CWSPolicyReader : public CBase
{
public:
    static  CWSPolicyReader* NewL();
    static  CWSPolicyReader* NewLC();
    ~CWSPolicyReader();

    CPolicyAssertion* GetPolicyL(CSenElement* aPolicy);
private:
     CPolicyAssertion* ReadPolicyL(CSenElement* aEle);
     CAndCompositeAssertion* ReadAndAssertionL(CSenElement* aEle);
     CXorCompositeAssertion* ReadXorAssertionL(CSenElement* aEle);
     CAssertion* ReadAssertionL(CSenElement* aEle);
     CPolicyReferenceAssertion* ReadPolicyReferenceL(CSenElement* aEle);
     CPrimitiveAssertion* ReadPrimitiveAssertionL(CSenElement* aEle);
     void ReadTermsL(CSenElement* aEle, RPointerArray<MAssertion>& aTerms);
     void GetAttributesL(CSenElement* aEle, RAttributeMap& aAttributes);
     
     TPtrC8 GetAttribL(TPtrC8 aReference, CSenElement* aEle);
     
     CWSPolicyReader();
     void ConstructL();
private:
    TInt count;
};
#endif //C_WS_POLICY_READER_H