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







#include "andcompositeassertion.h"
#include "xorcompositeassertion.h"
#include "primitiveassertion.h"
#include "policyassertion.h"

CAndCompositeAssertion* CAndCompositeAssertion::NewL()
{
    CAndCompositeAssertion* pSelf = CAndCompositeAssertion::NewLC();
    CleanupStack::Pop(pSelf);
    return pSelf;
   
}
CAndCompositeAssertion* CAndCompositeAssertion::NewLC()
{
    CAndCompositeAssertion* pSelf = new (ELeave) CAndCompositeAssertion();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;
 
}
CAndCompositeAssertion* CAndCompositeAssertion::NewL(CAndCompositeAssertion* aValue)
{
    CAndCompositeAssertion* pSelf = CAndCompositeAssertion::NewLC(aValue);
    CleanupStack::Pop(pSelf);
    return pSelf;
    
}
CAndCompositeAssertion* CAndCompositeAssertion::NewLC(CAndCompositeAssertion* aValue)
{
    CAndCompositeAssertion* pSelf = new (ELeave) CAndCompositeAssertion();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aValue);
    return pSelf;
    
}

CAndCompositeAssertion::~CAndCompositeAssertion()
{
    
}
CAndCompositeAssertion::CAndCompositeAssertion()
{
    
}
void CAndCompositeAssertion::ConstructL()
{
    
} 
void CAndCompositeAssertion::ConstructL(CAndCompositeAssertion* aValue)
{
    ConstructL();

    RPolicyTerms terms = aValue->GetTerms();
    
    if(terms.Count() > 0)
        AddTermsCopyL(terms);
    
 //   terms.Close();    
}

TAssertionType CAndCompositeAssertion::Type()
{
  return ECompositeAndType;  
}
void  CAndCompositeAssertion::AddTerm(MAssertion* aAssertion)
{
	if ((IsNormalized() && (aAssertion->Type() == EPrimitiveType))) 
	{
			SetNormalized(EFalse);
	}
	
	CAssertion::AddTerm(aAssertion);    
}
void CAndCompositeAssertion::AddTermCopyL(MAssertion* aAssertion)
{
 	if ((IsNormalized() && (aAssertion->Type() == EPrimitiveType))) 
	{
			SetNormalized(EFalse);
	}
	
	CAssertion::AddTermCopyL(aAssertion);    
}
MAssertion*	CAndCompositeAssertion::NormalizeL(CPolicyRegistry* aRegistry){

	if (IsNormalized()) 
	{
		return this;
	}

	CAndCompositeAssertion* AND = CAndCompositeAssertion::NewL();

	if (IsEmpty()) 
	{
		AND->SetNormalized(ETrue);
		return AND;
	}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
	RPolicyTerms XORs;

	RPointerArray<MAssertion> terms1 = GetTerms();
	MAssertion* term = NULL;
    TInt termCount = terms1.Count();

    for (TInt i = 0; i< termCount; i++)
    {
        
		term = terms1[i];
		MAssertion* result = term;
		if(!dynamic_cast<CPolicyAssertion*>(term))
		{
		    //term has new value object, no need to copy
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
			//term has new value object, no need to copy
			result = wrapper->NormalizeL(aRegistry);
			delete wrapper;
//			termsPol1.Close(); //not needed any more
		}

		if (dynamic_cast<CXorCompositeAssertion*>(result)) 
		{

			if (result->IsEmpty()) 
			{
                result->SetNormalized(true);
				delete AND;
                return result;
			}
			XORs.Append(result);
			continue;

		}

		if (dynamic_cast<CAndCompositeAssertion*>(result)) 
		{
  			if (result->IsEmpty()) 
			{
			    CAndCompositeAssertion* eleToDel = (CAndCompositeAssertion*)result;
			    delete eleToDel;
			}
			else
			{
    		    RPolicyTerms termsPol2 = result->GetTerms();
    			AND->AddTermsCopyL(termsPol2);

			    CAndCompositeAssertion* eleToDel = (CAndCompositeAssertion*)result;
			    delete eleToDel;
    			
//    			termsPol2.Close();
			}
			continue;
		}

		AND->AddTerm(result);
    }
    
	// processing child-XORCompositeAssertions

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
    
    CXorCompositeAssertion* XOR = CXorCompositeAssertion::NewL();
	
	if (XORs.Count() > 1) 
	{
		for (int i = 0; i < XORs.Count(); i++) 
		{

			for (int j = i; j < XORs.Count(); j++) 
			{

				if (i != j) 
				{
					CXorCompositeAssertion* XOR_A = (CXorCompositeAssertion*) XORs[i];
					CXorCompositeAssertion* XOR_B = (CXorCompositeAssertion*) XORs[j];

					RPolicyTerms iterA = XOR_A->GetTerms();
					for (TInt k = 0; k< iterA.Count(); k++) 
					{
					    MAssertion* anAND_A = iterA[k];
						RPolicyTerms iterB = XOR_B->GetTerms();

						for (TInt l = 0; l< iterB.Count(); l++)
						{

							MAssertion* anAND_B = iterB[l];
							CAndCompositeAssertion* nAND = CAndCompositeAssertion::NewL();
							RPolicyTerms termsAA = anAND_A->GetTerms();
							RPolicyTerms termsBB = anAND_B->GetTerms();
							nAND->AddTermsCopyL(termsAA);
							nAND->AddTermsCopyL(termsBB);
//                            termsAA.Close();
  //                          termsBB.Close();
							XOR->AddTerm(nAND);
						}
//						iterB.Close();
					}
//					iterA.Close();
				}
			}
		}

	} 
	else if (XORs.Count() == 1) 
	{	
		CXorCompositeAssertion* XORterm = (CXorCompositeAssertion*)XORs[0];
	    RPolicyTerms termsPol3 = XORterm->GetTerms();
		XOR->AddTermsCopyL(termsPol3);
		delete XORterm;
//		termsPol3.Close();
	}

	XORs.Close();

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
	
	if (XOR->IsEmpty())
	{
	    delete XOR;
		AND->SetNormalized(true);
		return AND;
	}

	if (AND->IsEmpty()) 
	{
	    delete AND;
		XOR->SetNormalized(true);
		return XOR;
	}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

	RPolicyTerms primTerms = AND->GetTerms();
	RPolicyTerms xTerms = XOR->GetTerms();

	for (TInt x = 0; x< xTerms.Count(); x++) 
	{
		MAssertion* rAND = xTerms[x];
		rAND->AddTermsCopyL(primTerms);
	}

    delete AND; //we have copied everythign from AND so now we dont need it
    
	XOR->SetNormalized(ETrue);
	return XOR;
    
}         
MAssertion* CAndCompositeAssertion::IntersectL(MAssertion* aAssertion, CPolicyRegistry* aRegistry)
{
    MAssertion* normalizedMe = ((IsNormalized()) ? this : NormalizeL(aRegistry));

    if (!(dynamic_cast<CAndCompositeAssertion*>(normalizedMe))) 
    {
    	return normalizedMe->IntersectL(aAssertion, aRegistry);
    }

    MAssertion* target = (aAssertion->IsNormalized()) ? aAssertion : aAssertion->NormalizeL(aRegistry);
    short type = target->Type();

    switch (type) 
    {

        case ECompositePolicyType: 
        {
        	CPolicyAssertion* nPOLICY = CPolicyAssertion::NewL();
        	nPOLICY->AddTerm(normalizedMe->IntersectL(target->GetTerms()[0], NULL));
        	return nPOLICY;
        }

        case ECompositeXorType: 
        {
        	CXorCompositeAssertion* nXOR = CXorCompositeAssertion::NewL();
        	RPointerArray<MAssertion> terms = target->GetTerms();

            TInt termCount = terms.Count();

           	for (TInt i = 0; i< termCount; i++)
        	{
        		MAssertion* asser = normalizedMe->IntersectL(terms[i]);

        		if (asser->Type() == ECompositeAndType) 
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

        case ECompositeAndType: 
        {
        	RPolicyTerms PRIMITIVES_A = ((normalizedMe->Size() > target->Size()) ? normalizedMe->GetTerms() : target->GetTerms());
        	RPolicyTerms PRIMITIVES_B = ((normalizedMe->Size() > target->Size()) ? target->GetTerms() : normalizedMe->GetTerms());

        	CPrimitiveAssertion* PRIMITIVE_A = NULL;
        	CPrimitiveAssertion* PRIMITIVE_B = NULL;

        	for (int i = 0; i < PRIMITIVES_A.Count(); i++) 
        	{
        		PRIMITIVE_A = (CPrimitiveAssertion*) PRIMITIVES_A[i];

        		TBool flag = false;

        		for (int j = 0; j < PRIMITIVES_B.Count(); j++) 
        		{
        			PRIMITIVE_B = (CPrimitiveAssertion*) PRIMITIVES_B[j];

        			if (PRIMITIVE_A->Name().Compare(PRIMITIVE_B->Name()) == 0) 
        			{
        				flag = true;
        				break;
        			}

        		}

        		if (!flag) 
        		{
        			return CXorCompositeAssertion::NewL();
        		}

        		MAssertion* a = PRIMITIVE_A->IntersectL(PRIMITIVE_B);

        		if (dynamic_cast<CXorCompositeAssertion*>(a)) 
        		{
        		    DeleteAssertion(a);
        			return CXorCompositeAssertion::NewL();
        		}
                else
                {
                    DeleteAssertion(a);
                }
        		
        	}
        	CAndCompositeAssertion* result = CAndCompositeAssertion::NewL();
        	result->AddTermsCopyL(PRIMITIVES_A);
        	result->AddTermsCopyL(PRIMITIVES_B);
        	return result;
        }

        case EPrimitiveType: 
        {
        	CQName* name = ((CPrimitiveAssertion*) target)->QName();
        	TBool isMatch = false;

        	CQName* targetName = NULL;
        	RPolicyTerms terms = normalizedMe->GetTerms();
            for (TInt j = 0; j < terms.Count(); j++) 
        	{
        		targetName = ((CPrimitiveAssertion*) terms[j])->QName();

        		if (name->Uri().Compare(targetName->Uri()) == 0) 
        		{
        			isMatch = true;
        			break;
        		}
        	}

        	if (isMatch) {
        		CAndCompositeAssertion* nAND = CAndCompositeAssertion::NewL();
        	    RPolicyTerms tgtTerm = normalizedMe->GetTerms();
        		nAND->AddTerms(tgtTerm);
        		nAND->AddTerm(target);
        		return nAND;
        	}

        	return CXorCompositeAssertion::NewL();
        }

        default: 
        {
        }

    }
    return NULL;    
}
MAssertion*	CAndCompositeAssertion::MergeL(MAssertion* aAssertion, CPolicyRegistry* aRegistry)
{
	MAssertion* normalizedMe = (IsNormalized()) ? this : NormalizeL(aRegistry);

	if (!(dynamic_cast<CAndCompositeAssertion*>(normalizedMe))) 
	{
		return normalizedMe->MergeL(aAssertion, aRegistry);
	}

	MAssertion* target = (aAssertion->IsNormalized()) ? aAssertion : aAssertion->NormalizeL(aRegistry);

	switch (target->Type()) 
	{

    	case ECompositePolicyType: 
    	{
    		CPolicyAssertion* nPOLICY = CPolicyAssertion::NewL();
    		RPolicyTerms xTerms = target->GetTerms();
    		if(xTerms.Count() > 0)
    		{
    		    CXorCompositeAssertion* term = (CXorCompositeAssertion*)xTerms[0];		    
    		    nPOLICY->AddTerm(normalizedMe->MergeL(term));		   
    		}		
    		return nPOLICY;
    	}

    	case ECompositeXorType: 
    	{

    		CXorCompositeAssertion* nXOR = CXorCompositeAssertion::NewL();
            RPolicyTerms xTerms = target->GetTerms();
    		for (TInt i=0; i< xTerms.Count(); i++)
    		{
    			CAndCompositeAssertion* AND = (CAndCompositeAssertion*) xTerms[i];
    			nXOR->AddTerm(normalizedMe->MergeL(AND));
    		}

    		return nXOR;
    	}

    	case ECompositeAndType: 
    	{
    		CAndCompositeAssertion* nAND = CAndCompositeAssertion::NewL();
            RPolicyTerms terms = normalizedMe->GetTerms();
    		nAND->AddTermsCopyL(terms);
    		RPolicyTerms targeterms = target->GetTerms();
    		nAND->AddTermsCopyL(targeterms);

    		return nAND;
    	}

    	case EPrimitiveType: 
    	{
    		CAndCompositeAssertion* nAND = CAndCompositeAssertion::NewL();
            RPolicyTerms terms = normalizedMe->GetTerms();
    		nAND->AddTermsCopyL(terms);
    		nAND->AddTermCopyL(target);

    		return nAND;
    	}

	}
    
    return NULL;	
}

