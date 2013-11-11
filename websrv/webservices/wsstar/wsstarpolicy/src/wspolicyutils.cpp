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
* Description:        
*
*/








// INCLUDE FILES
#include "wspolicyutils.h"
#include "SenDateUtils.h"
#include "SenXmlElement.h"
#include "senattributes.h"
#include "policyassertion.h"
#include "xorcompositeassertion.h"
#include "andcompositeassertion.h"
#include "wspolicyreader.h"
using namespace WSPolicy;
using namespace MSWSPolicy;


CPolicyAssertion* CWSPolicyUtils::GetSinglePolicyL(RPolicyTerms& policyList, CPolicyRegistry* reg)
{
		CPolicyAssertion* policyTerm = NULL;
		CPolicyAssertion* policyTerm2 = NULL;

		for(TInt i = 0; i < policyList.Count() ; i++)

		{
			policyTerm2 = (CPolicyAssertion*) policyList[i];
			if(policyTerm == NULL)
			{
			   policyTerm = policyTerm2; 
			}
			else
			{
			    policyTerm = (CPolicyAssertion*)policyTerm->MergeL(policyTerm2, reg);
			}
		}

		if (policyTerm && (!policyTerm->IsNormalized())) 
		{
			policyTerm = (CPolicyAssertion*) policyTerm->NormalizeL();
		}
		
		return policyTerm;
	}

void CWSPolicyUtils::GetPrimTermsListL(CPolicyAssertion* policy, RPolicyTerms& aTerms) 
{
	MAssertion* norm = policy;
	if (!policy->IsNormalized())
	{
		norm = (CPolicyAssertion*) policy->NormalizeL();
	}

	CXorCompositeAssertion* xorTerm = (CXorCompositeAssertion*) norm->GetTerms()[0];
	CAndCompositeAssertion* andTerm = (CAndCompositeAssertion*) xorTerm->GetTerms()[0];

    RPolicyTerms terms = andTerm->GetTerms();
    TInt count = terms.Count();
    for(TInt i = 0; i<count ; i++)
    {
        MAssertion* asser = terms[i];
        aTerms.Append(asser);
    }
}
	
CPolicyAssertion* CWSPolicyUtils::GetPolicy(RPolicyTerms& /*terms*/)
{
//implemented in PrimitiveAssertion
return KErrNone;
}


EXPORT_C TPtrC8 CWSPolicyUtils::PolicyIdL(CSenElement* aPolicy)
{

    if(aPolicy == NULL) { return KNullDesC8(); }

    RPointerArray<CSenBaseAttribute>& attrs = aPolicy->AttributesL();

    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
        bs = (attrs)[j];
        if(bs)
            {
            if(bs->Name().Compare(KWsuId) == 0)
                {
                return bs->Value();
                }
            }
         }    
    return KNullDesC8();
}

EXPORT_C TPtrC8 CWSPolicyUtils::PolicyNameL(CSenElement* aPolicy)
{

    if(aPolicy == NULL) { return KNullDesC8(); }
    
    RPointerArray<CSenBaseAttribute>& attrs = aPolicy->AttributesL();
    
    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
        bs = (attrs)[j];
        if(bs)
        {
            if(bs->Name().Compare(KName) == 0)
            {
                 return bs->Value();
            }
         }

            
        }
    return KNullDesC8();
}

EXPORT_C TPtrC8 CWSPolicyUtils::PolicyUriL(CSenElement* aPolicy)
{

    if(aPolicy == NULL) { return KNullDesC8(); }
    
    RPointerArray<CSenBaseAttribute>& attrs = aPolicy->AttributesL();
    
    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
        bs = (attrs)[j];
        if(bs)
        {
            if(bs->Name().Compare(MSWSPolicy::KMetaDataLocalName) == 0)
            {
                 return bs->Value();
            }
         }

            
        }
    return KNullDesC8();
}
EXPORT_C TBool CWSPolicyUtils::IsExpiredPolicyL(CSenElement* aPolicy)
{
    if(aPolicy == NULL) { return ETrue; }
    
    CSenElement* expiry = GetExpiryTagL(aPolicy);
    if(expiry and expiry->HasContent())
    {
        TPtrC8 expiryTime = expiry->Content();
     
        TTime now;
        now.UniversalTime();
     
        TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts;
        SenDateUtils::ToXmlDateTimeUtf82L(ts, now);
        
        if(ts > expiryTime)
            return ETrue;
        else
            return EFalse;
    }
    return EFalse; 
}
CSenElement* CWSPolicyUtils::GetExpiryTagL(CSenElement* aPolicy)
{
    CSenElement* documentRoot = (CSenElement*)&aPolicy->Root();
    if(documentRoot)     
    {
        RPointerArray<CSenElement> timeStamps;
        CleanupClosePushL(timeStamps);       
        CSenElement* validity = NULL;
        const CSenNamespace* msNsUri = documentRoot->Namespace(MSWSPolicy::KMSNsPrefix);
        if(msNsUri)
            {
            if(documentRoot->ElementsL(timeStamps, msNsUri->URI(), MSWSPolicy::KMetaDataValidity) == KErrNone)    
                {
                TInt validityStampsCount = timeStamps.Count();
                
                if(validityStampsCount > 0)
                    {
                    validity = timeStamps[0];//mws:metadatavalidity 
                    CleanupStack::PopAndDestroy();
                    return validity->Element(msNsUri->URI(), MSWSPolicy::KExpiry);
                    }
                }
            
            }
        else
            {
            if(documentRoot->ElementsL(timeStamps, MSWSPolicy::KMetaDataValidity) == KErrNone)
                {
                        
                TInt validityStampsCount = timeStamps.Count();
                
                if(validityStampsCount > 0)
                    {
                    validity = timeStamps[0];//mws:metadatavalidity 
                    CleanupStack::PopAndDestroy();
                    return validity->Element(MSWSPolicy::KExpiry);
                    }
                }
            
            }

        CleanupStack::PopAndDestroy();
    }
    return NULL;
}
	
// End of File

