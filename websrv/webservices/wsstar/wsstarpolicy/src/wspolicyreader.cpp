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
* Description:      
*
*/






#include "wspolicyreader.h"

#include "policyassertion.h"
#include "andcompositeassertion.h"
#include "xorcompositeassertion.h"
#include "assertion.h"
#include "primitiveassertion.h"
#include "policyreferenceassertion.h"

using namespace WSPolicy;

CWSPolicyReader* CWSPolicyReader::NewL()
{
     CWSPolicyReader* pSelf = CWSPolicyReader::NewLC();
    CleanupStack::Pop(pSelf);
    return pSelf;
    
}
CWSPolicyReader* CWSPolicyReader::NewLC()
{
       CWSPolicyReader* pSelf = new (ELeave) CWSPolicyReader();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;
     
}
CWSPolicyReader::~CWSPolicyReader()
{
}
CWSPolicyReader::CWSPolicyReader()
{
}
void CWSPolicyReader::ConstructL()
{
}

CPolicyAssertion* CWSPolicyReader::GetPolicyL(CSenElement* aPolicy)
{
    CPolicyAssertion* policy = ReadPolicyL(aPolicy);
    if(policy)
        return policy;
    else
        return NULL;
}


//PRIVATE METHODS
CPolicyAssertion* CWSPolicyReader::ReadPolicyL(CSenElement* aEle)
{
    if(!aEle)
    return NULL;
    
    CPolicyAssertion* policy = CPolicyAssertion::NewL();
    
    TPtrC8 kwsuid(KWsuId);
    TPtrC8 id = GetAttribL(kwsuid, aEle);
    if (id != KNullDesC8())
    {
        policy->SetIdL(id);
    }

    RPointerArray<MAssertion> terms;
    ReadTermsL(aEle, terms);
    policy->AddTerms(terms);
    terms.Reset();

    return policy;

}
CAndCompositeAssertion* CWSPolicyReader::ReadAndAssertionL(CSenElement* aEle)
{
	CAndCompositeAssertion* andAssert = CAndCompositeAssertion::NewL();

    RPointerArray<MAssertion> terms;
    ReadTermsL(aEle, terms);
    andAssert->AddTerms(terms);
    terms.Reset();
    return andAssert;
    
}
CXorCompositeAssertion* CWSPolicyReader::ReadXorAssertionL(CSenElement* aEle)
{
	CXorCompositeAssertion* xorAssert = CXorCompositeAssertion::NewL();

    RPointerArray<MAssertion> terms;
    ReadTermsL(aEle, terms);
    xorAssert->AddTerms(terms);
    terms.Reset();
    return xorAssert;
    
}
CAssertion* CWSPolicyReader::ReadAssertionL(CSenElement* aEle)
{
	TPtrC8 nsUri = aEle->NamespaceURI();
	TPtrC8 localName = aEle->LocalName();
	
	if(nsUri.Length() == 0 || localName.Length() == 0)
	    return NULL;

		if (!(nsUri == KWsPolicyNsUri)) 
		{
			return ReadPrimitiveAssertionL(aEle);
		}

		if (localName == KWsPolicy)
		{
			return ReadPolicyL(aEle);

		}
		else if (localName == KAndCompositeAssertion)
		{
          return ReadAndAssertionL(aEle);

		}
		else if (localName == KXorCompositeAssertion)
		{
			return ReadXorAssertionL(aEle);

		}
		else if (localName == KWsPolicyReference ) 
		{
			return ReadPolicyReferenceL(aEle);

		} 
		else 
		{
		    return NULL;
		}
}
CPolicyReferenceAssertion* CWSPolicyReader::ReadPolicyReferenceL(CSenElement* aEle)
{
    
    TPtrC8 id = GetAttribL(KUri(), aEle);
    if(id.Length() > 0)
    {
        return CPolicyReferenceAssertion::NewL(id);    
    }
    return NULL;    
}
CPrimitiveAssertion* CWSPolicyReader::ReadPrimitiveAssertionL(CSenElement* aEle)
{
    TPtrC8 localName = aEle->LocalName();//GetQname
    TPtrC8 nsprefix = aEle->NsPrefix();
    TPtrC8 nsuri= aEle->NamespaceURI();
    
    if(localName.Length() == 0 || nsprefix.Length() == 0 || nsuri.Length() == 0)    
    return NULL;
    
    CPrimitiveAssertion* result = CPrimitiveAssertion::NewL(localName,nsprefix,nsuri);

    RAttributeMap attribs(ETrue,ETrue);
    GetAttributesL(aEle, attribs);
    result->SetAttributesL(attribs);
    attribs.Reset();

    
    TPtrC8 isOptional = result->GetAttributeValue(KWspOptinal);
    
    if(isOptional == KWspOptinalTrue)
    {
        result->SetOptional(ETrue);
    }
    else if(isOptional == KWspOptinalFalse)
    {
        result->SetOptional(EFalse);
        result->RemoveAttribute(KWspOptinal);
    }

    // setting the text value ..
    if(aEle->HasContent())
    {
        TPtrC8 strValue = aEle->Content();    
        if(strValue != KNullDesC8())
        
    	result->SetStrValueL(strValue);
    }
    

    RPointerArray<CSenElement> childElements = aEle->ElementsL();
    for (TInt i=0; i<childElements.Count(); i++ )
    {
        CSenElement* childElement = childElements[i];
        if(childElement && childElement->LocalName() == KWsPolicy
            && childElement->NamespaceURI() == KWsPolicyNsUri)
        {
            CPolicyAssertion* policy = ReadPolicyL(childElement);
    		result->AddTerm(policy);

    	} 
    	else
    	{
    		CPrimitiveAssertion* pa = ReadPrimitiveAssertionL(childElement);
    		result->AddTerm(pa);
    	}
    }
    
    return result;

}
void CWSPolicyReader::ReadTermsL(CSenElement* aEle, RPointerArray<MAssertion>& aTerms)
{
	RPointerArray<CSenElement> childElements = aEle->ElementsL();
    for (TInt i=0; i<childElements.Count(); i++ )
    {
        CSenElement* obj = childElements[i];
        if(obj)
        {
            MAssertion* ass = ReadAssertionL(obj);
            if(ass)
            {
                aTerms.Append(ass);
            }
        }
    }
}

void CWSPolicyReader::GetAttributesL(CSenElement* aEle, RAttributeMap& aAttributes)
{
	RPointerArray<CSenBaseAttribute> attribs = aEle->AttributesL();
    for (TInt i=0; i<attribs.Count(); i++ )
    {
        CSenBaseAttribute* obj = attribs[i];
        if(obj)
        {
            TPtrC8 name = obj->Name();
            TPtrC8 value = obj->Value();
            
            aAttributes.Append(name.AllocL(), value.AllocL()); 
        }
    }
}

TPtrC8 CWSPolicyReader::GetAttribL(TPtrC8 aReference, CSenElement* aEle)
{

    RPointerArray<CSenBaseAttribute>& attrs = aEle->AttributesL();
    
    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
         bs = (attrs)[j];
         if(bs->Name().Compare(aReference) == 0)
         {
            return bs->Value();
         }
            
              
         }
    return KNullDesC8();
}
//EOF
