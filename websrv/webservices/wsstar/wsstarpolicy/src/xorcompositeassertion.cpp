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







#include "xorcompositeassertion.h"
#include "andcompositeassertion.h"
#include "primitiveassertion.h"
#include "policyassertion.h"


CXorCompositeAssertion* CXorCompositeAssertion::NewL()
{
    CXorCompositeAssertion* pSelf = CXorCompositeAssertion::NewLC();
    CleanupStack::Pop(pSelf);
    return pSelf;
    
}
CXorCompositeAssertion* CXorCompositeAssertion::NewLC()
{
     CXorCompositeAssertion* pSelf = new (ELeave) CXorCompositeAssertion();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;
   
}
CXorCompositeAssertion* CXorCompositeAssertion::NewL(CXorCompositeAssertion* aValue)
{
    CXorCompositeAssertion* pSelf = CXorCompositeAssertion::NewLC(aValue);
    CleanupStack::Pop(pSelf);
    return pSelf;
    
}
CXorCompositeAssertion* CXorCompositeAssertion::NewLC(CXorCompositeAssertion* aValue)
{
     CXorCompositeAssertion* pSelf = new (ELeave) CXorCompositeAssertion();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aValue);
    return pSelf;
    
}

CXorCompositeAssertion::~CXorCompositeAssertion()
{
    
}
CXorCompositeAssertion::CXorCompositeAssertion()
{
    
}
void CXorCompositeAssertion::ConstructL(CXorCompositeAssertion* aValue)
{
    ConstructL();

    RPolicyTerms terms = aValue->GetTerms();
    
    if(terms.Count() > 0)
        AddTermsCopyL(terms);
    
//    terms.Close();

}
void CXorCompositeAssertion::ConstructL()
{
    
}

//from CAssertion
TAssertionType CXorCompositeAssertion::Type()
{
  return ECompositeXorType;    
}
void CXorCompositeAssertion::AddTerm(MAssertion* aAssertion)
{
	if ( IsNormalized() 
	    && (aAssertion->Type() == ECompositeAndType) 
	    && (aAssertion->IsNormalized()
	    ))
	{
			SetNormalized(EFalse);
	}
	CAssertion::AddTerm(aAssertion);
}
void CXorCompositeAssertion::AddTermCopyL(MAssertion* aAssertion)
{
	if ( IsNormalized() 
	    && (aAssertion->Type() == ECompositeAndType) 
	    && (aAssertion->IsNormalized()
	    ))
	{
			SetNormalized(EFalse);
	}
	CAssertion::AddTermCopyL(aAssertion);
    
}
MAssertion*	CXorCompositeAssertion::NormalizeL(CPolicyRegistry* aRegistry)
{
	if (IsNormalized()) {
			return this;
		}

		CXorCompositeAssertion* XOR = CXorCompositeAssertion::NewL();

		if (IsEmpty()) {
			XOR->SetNormalized(true);
			return XOR;
		}

		RPointerArray<MAssertion> terms1 = GetTerms();
        TInt termCount = terms1.Count();
        for (TInt i = 0; i< termCount; i++)
        {
        
			MAssertion* term = terms1[i];
			MAssertion* result = term;
			if(!dynamic_cast<CPolicyAssertion*>(term))
			{
			    result = term->NormalizeL(aRegistry);
			}

			if (dynamic_cast<CPolicyAssertion*>(result))
			{
				CAndCompositeAssertion* wrapper = CAndCompositeAssertion::NewL();
				
				RPolicyTerms termsPol1 = result->GetTerms();
				wrapper->AddTermsCopyL(termsPol1);

    			if(result != term)
    			{
                    CPolicyAssertion* eleToDel = (CPolicyAssertion*)result;
                    delete eleToDel;
    			}
				result = NULL;
				
				result = wrapper->NormalizeL(aRegistry);
				delete wrapper;

				if (dynamic_cast<CAndCompositeAssertion*>(result))
				{
					XOR->AddTerm(result);
				}
				else
				{
				    RPolicyTerms termsPol2 = result->GetTerms();
					XOR->AddTermsCopyL(termsPol2);
					
					CPolicyAssertion* eleToDel = (CPolicyAssertion*)result;
					delete eleToDel;
//					termsPol2.Close();
				}
				continue;
			}
            
            if (dynamic_cast<CPrimitiveAssertion*>(result)) 
			{
				CAndCompositeAssertion* wrapper = CAndCompositeAssertion::NewL();
				wrapper->AddTermCopyL(result);
				XOR->AddTerm(wrapper);
				CPrimitiveAssertion* eleToDel = (CPrimitiveAssertion*)result;
				delete eleToDel;
				result = NULL;
				continue;
			}
			
			if (dynamic_cast<CXorCompositeAssertion*>(result)) 
			{
			    RPolicyTerms termsPol3 = result->GetTerms();
				XOR->AddTermsCopyL(termsPol3);
				CXorCompositeAssertion* eleToDel = (CXorCompositeAssertion*)result;				
                delete eleToDel;				
				continue;
			}
			
			if (dynamic_cast<CAndCompositeAssertion*>(result)) 
			{
				XOR->AddTerm(result);
			}
		}
//        terms1.Close();
		XOR->SetNormalized(ETrue);
		return XOR;    
}
MAssertion* CXorCompositeAssertion::IntersectL(MAssertion* aAssertion, CPolicyRegistry* aRegistry)
{
    MAssertion* normalizedMe = (IsNormalized()) ? this : NormalizeL(aRegistry);

    if (!(dynamic_cast<CXorCompositeAssertion*>(normalizedMe))) 
    {
    	return normalizedMe->IntersectL(aAssertion, aRegistry);
    }

    MAssertion* target = (aAssertion->IsNormalized()) ? aAssertion : aAssertion->NormalizeL(aRegistry);
    short type = target->Type();
    
    RPolicyTerms terms = normalizedMe->GetTerms();
    
    switch (type) {

    case ECompositePolicyType: {
    	CPolicyAssertion* nPOLICY = CPolicyAssertion::NewL();
        CXorCompositeAssertion* term = (CXorCompositeAssertion*)terms[0];
    	nPOLICY->AddTerm(term->IntersectL(target, NULL));
    	return nPOLICY;
    }

    case ECompositeXorType: {
    	CXorCompositeAssertion* nXOR = CXorCompositeAssertion::NewL();

    	MAssertion* asser = NULL;
    	CAndCompositeAssertion* AND;

    	for (TInt i=0; i< terms.Count(); i++) 
    	{
    		AND = (CAndCompositeAssertion*)terms[i];
            RPolicyTerms tgtTerms = target->GetTerms();
    		for (TInt j=0; j< tgtTerms.Count(); j++) 
    		{
    		    asser = NULL;
    			asser = AND->IntersectL((CAndCompositeAssertion*)tgtTerms[j], NULL);

    			if (dynamic_cast<CAndCompositeAssertion*>(asser)) 
    			{
    				nXOR->AddTerm(asser);
    			}
    			else
    			{
    			    DeleteAssertion(asser);
    			}

    			
    			
    		}
    	}

    	return nXOR;
    }

    case ECompositeAndType: {
    	CXorCompositeAssertion* nXOR = CXorCompositeAssertion::NewL();
    	MAssertion* asser;
    	
    	for (TInt i=0; i< terms.Count(); i++) 
    	{
    		asser = ((CAndCompositeAssertion*) terms[i])->IntersectL(target, NULL);

    		if (dynamic_cast<CAndCompositeAssertion*>(asser)) 
    		{
    			nXOR->AddTerm(asser);
    		}
            else
            {
                DeleteAssertion(asser);
            }
    	}
    	return nXOR;
    }

    case EPrimitiveType: {
    	CXorCompositeAssertion* nXOR = CXorCompositeAssertion::NewL();

    	MAssertion* asser;
        for (TInt i=0; i< terms.Count(); i++) 
    	{
    		asser = ((CAndCompositeAssertion*)terms[i])->IntersectL(target, NULL);

    		if (dynamic_cast<CAndCompositeAssertion*>(asser)) 
    		{
    			nXOR->AddTerm(asser);
    		}
            else
            {
                DeleteAssertion(asser);
            }
    		
    	}
    	return nXOR;
    }

    default: {
    }

    }
return NULL;    
}
MAssertion* CXorCompositeAssertion::MergeL(MAssertion* aAssertion, CPolicyRegistry* aRegistry)
{
	MAssertion* normalizedMe = (IsNormalized()) ? this : NormalizeL(aRegistry);

	if (!(dynamic_cast<CXorCompositeAssertion*>(normalizedMe)))
	{
		return normalizedMe->MergeL(aAssertion, aRegistry);
	}

	MAssertion* target = (aAssertion->IsNormalized()) ? aAssertion : aAssertion->NormalizeL(aRegistry);

	short type = target->Type();

	switch (type) 
	{

    	case ECompositePolicyType: 
    	{

    		CPolicyAssertion* nPOLICY = CPolicyAssertion::NewL();
    		nPOLICY->AddTerm(normalizedMe->MergeL(target->GetTerms()[0]));
    		return nPOLICY;
    	}

    	case ECompositeXorType: 
    	{

    		CXorCompositeAssertion* nXOR = CXorCompositeAssertion::NewL();
            RPolicyTerms xTerms = normalizedMe->GetTerms();
    		for (TInt i=0; i< xTerms.Count(); i++)
    		{
    			CAndCompositeAssertion* AND = (CAndCompositeAssertion*) xTerms[i];

                RPolicyTerms targetTerms = target->GetTerms();
    	    	for (TInt j=0; j< targetTerms.Count(); j++)
    			{
                    MAssertion* tgtTerm = targetTerms[j];
    				nXOR->AddTerm(AND->MergeL(tgtTerm, NULL)); 
    			}

    			if (target->IsEmpty() && AND->IsEmpty()) { 
    				/*
    				 * " <wsp:ExactlyOne> <wsp:All/>
    				 * </wsp:ExactlyOne>".intersect(" <wsp:ExactlyOne/>")
    				 */
    				nXOR->AddTermCopyL(AND);
    			}
    		}

    		return nXOR;
    	}

    	case ECompositeAndType: 
    	{

    		CXorCompositeAssertion* nXOR = CXorCompositeAssertion::NewL();
            RPolicyTerms xTerms = normalizedMe->GetTerms();
    		for (TInt i=0; i< xTerms.Count(); i++)
    		{
    			CAndCompositeAssertion* term = (CAndCompositeAssertion*)xTerms[i];
    			nXOR->AddTerm(term->MergeL(target, NULL)); 
    		}
    		return nXOR;
    	}

    	case EPrimitiveType: 
    	{
    		CXorCompositeAssertion* nXOR = CXorCompositeAssertion::NewL();

            RPolicyTerms xTerms = normalizedMe->GetTerms();
    		for (TInt i=0; i< xTerms.Count(); i++)
    		{
    		    CAndCompositeAssertion*  tgtTerm =  (CAndCompositeAssertion*)xTerms[i];
    		    nXOR->AddTerm(tgtTerm->MergeL(target, NULL)); 
    		}

    		return nXOR;
    	}

    	default: 
    	{
    	}

	}
return NULL;	    
}
