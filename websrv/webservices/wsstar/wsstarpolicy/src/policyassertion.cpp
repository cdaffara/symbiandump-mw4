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







#include "policyassertion.h"
#include "xorcompositeassertion.h"
#include "andcompositeassertion.h"
#include "wspolicywriter.h"
#include "sendebug.h"
CPolicyAssertion* CPolicyAssertion::NewL()
{
    CPolicyAssertion* pSelf = CPolicyAssertion::NewLC();
    CleanupStack::Pop(pSelf);
    return pSelf;

}
CPolicyAssertion* CPolicyAssertion::NewLC()
{
    CPolicyAssertion* pSelf = new (ELeave) CPolicyAssertion();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;

}
CPolicyAssertion* CPolicyAssertion::NewL(CPolicyAssertion* aValue)
{
    CPolicyAssertion* pSelf = CPolicyAssertion::NewLC(aValue);
    CleanupStack::Pop(pSelf);
    return pSelf;
    
}
CPolicyAssertion* CPolicyAssertion::NewLC(CPolicyAssertion* aValue)
{
    CPolicyAssertion* pSelf = new (ELeave) CPolicyAssertion();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aValue);
    return pSelf;    
}

CPolicyAssertion::~CPolicyAssertion()
{
    delete iName;
    delete iId;
    delete iMetadataEndpoint;
}
CPolicyAssertion::CPolicyAssertion():
iName(NULL),
iId(NULL),
iMetadataEndpoint(NULL)
{

}
void CPolicyAssertion::ConstructL()
{

}

void CPolicyAssertion::ConstructL(CPolicyAssertion* aValue)
{
    ConstructL();

    RPolicyTerms terms = aValue->GetTerms();
    
    if(terms.Count() > 0)
        AddTermsCopyL(terms);
    
}
TAssertionType CPolicyAssertion::Type()
{
    return ECompositePolicyType;
}
MAssertion* CPolicyAssertion::NormalizeL()
{
    return NormalizeL(NULL);
}
MAssertion*	CPolicyAssertion::NormalizeL(CPolicyRegistry* aRegistry)
{
    if (IsNormalized()) 
    {
    	return this;
    }
//	TPtrC8 xmlBase = Name();
//	TPtrC8 id = Id();
	CPolicyAssertion* policy = CPolicyAssertion::NewL(); //new Policy(xmlBase, id);

	CXorCompositeAssertion* XOR = CXorCompositeAssertion::NewL();
	CAndCompositeAssertion* AND = CAndCompositeAssertion::NewL();	
	
    RPolicyTerms childXorTermList;
    
	RPointerArray<MAssertion> terms = GetTerms();
	MAssertion* term = NULL;
	MAssertion* result = NULL;
    TInt termCount = terms.Count();
	for (TInt i = 0; i< termCount; i++)
	{
		term = terms[i];
		result = term->NormalizeL(aRegistry);

		if(dynamic_cast<CPolicyAssertion*>(result))
		{
			CXorCompositeAssertion* tempXor = (CXorCompositeAssertion*) result->GetTerms()[0];

			if (tempXor->Size() != 1)
			{
			//result is Policy and have an XOR
			//delete the policy and put XOR as result
//				result = Xor;
                
			    CPolicyAssertion* ele = (CPolicyAssertion*)result;
			    result = NULL;
			    result = CXorCompositeAssertion::NewL();
			    
			    RPolicyTerms xorTerms = tempXor->GetTerms();
			    result->AddTermsCopyL(xorTerms);
			    
			    delete ele;
			    ele = NULL;
            } 
			else 
			{
			    MAssertion* tempTerm = tempXor->GetTerms()[0];
			    if(tempTerm)
			    {
                    RPolicyTerms terms2 = tempTerm->GetTerms();
    				AND->AddTermsCopyL(terms2);
//    				terms2.Close();
			    }
    				CPolicyAssertion* eleToDel = (CPolicyAssertion*)result;
    				delete eleToDel;
    				eleToDel = NULL;
    				result = NULL;
		
				continue;
			}
		}

		if(dynamic_cast<CXorCompositeAssertion*>(result))
		{

			if (((CXorCompositeAssertion*) result)->IsEmpty())
			{
//				CXorCompositeAssertion* emptyXor = CXorCompositeAssertion::NewL();
//				emptyXor->SetNormalized(ETrue);
//                result->AddTermL(AND);
				policy->AddTerm(result);
				policy->SetNormalized(ETrue);
                delete AND;
                delete XOR;
                childXorTermList.Close();
				return policy;
			}

			childXorTermList.Append(result);
			continue;
		}

		if(dynamic_cast<CAndCompositeAssertion*>(result))
		{

			if (result->IsEmpty()) 
			{
				CAndCompositeAssertion* emptyAnd = CAndCompositeAssertion::NewL();
				XOR->AddTerm(emptyAnd);

			} 
			else 
			{
			    RPolicyTerms terms3 = result->GetTerms();
				AND->AddTermsCopyL(terms3);
			}
                CAndCompositeAssertion* eletoDel = (CAndCompositeAssertion*)result;
				delete eletoDel;			
			
			continue;
		}
		
		AND->AddTerm(result);
	}

////////////////////////////////////////////////////	
////////////////////////////////////////////////////
	// processing child-XORCompositeAssertions
	if (childXorTermList.Count() > 1) {

		for (TInt i = 0; i < childXorTermList.Count(); i++) {

			for (TInt j = i; j < childXorTermList.Count(); j++) {

				if (i != j) {
					CXorCompositeAssertion* xorTermA = (CXorCompositeAssertion*) childXorTermList[i];
					CXorCompositeAssertion* xorTermB = (CXorCompositeAssertion*) childXorTermList[j];

					RPolicyTerms iterA = xorTermA->GetTerms();

					for (TInt k = 0; k< iterA.Count(); k++)
					{
						MAssertion* andTermA = iterA[k];
                        
                        RPolicyTerms iterB = xorTermB->GetTerms();

    					for (TInt l = 0; l< iterB.Count(); l++)
    					{
							MAssertion* andTermB = iterB[l];
							CAndCompositeAssertion* anAndTerm = CAndCompositeAssertion::NewL();
							RPolicyTerms termsAA = andTermA->GetTerms();
							RPolicyTerms termsBB = andTermB->GetTerms();
							anAndTerm->AddTermsCopyL(termsAA);
							anAndTerm->AddTermsCopyL(termsBB);
//							termsBB.Close();
//							termsAA.Close();
							XOR->AddTerm(anAndTerm);
						}
//    						iterB.Close();
					}
//    					iterA.Close();
				}
			}
		}
		for (TInt x = 0; x < childXorTermList.Count(); x++) 
	    {
	        CXorCompositeAssertion* xorTermTemp = (CXorCompositeAssertion*)childXorTermList[x];
	        if(xorTermTemp)
	            delete xorTermTemp;	        
	    }
	}
	else if (childXorTermList.Count() == 1) 
	{
		CXorCompositeAssertion* xorTerm = (CXorCompositeAssertion*)childXorTermList[0];
		XOR->AddTermsCopyL(xorTerm->GetTerms());
        delete xorTerm;
	}

	else if (childXorTermList.Count() == 0)
	{
        if(AND->Size() > 0)
            XOR->AddTerm(AND);
        else
            delete AND;
        policy->AddTerm(XOR);
		policy->SetNormalized(ETrue);
		
        childXorTermList.Close();
    	return policy;
	}

    childXorTermList.Close();
	
////////////////////////////////////////////////////	
////////////////////////////////////////////////////
	RPolicyTerms primTerms = AND->GetTerms();
	RPolicyTerms andTerms = XOR->GetTerms();
    for(TInt p = 0; p < andTerms.Count(); p++)
    {
		MAssertion* anAndTerm = andTerms[p];
		anAndTerm->AddTermsCopyL(primTerms);
	}
	
    delete AND;
	policy->AddTerm(XOR);
	policy->SetNormalized(ETrue);
	
	return policy;

}         
MAssertion* CPolicyAssertion::IntersectL(MAssertion* aAssertion, CPolicyRegistry* aRegistry)
{
	MAssertion* normalizedMe = (IsNormalized()) ? this : NormalizeL(aRegistry);
	if (!(dynamic_cast<CPolicyAssertion*>(normalizedMe))) 
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
		CXorCompositeAssertion* norm1 = (CXorCompositeAssertion*) normalizedMe->GetTerms()[0];
		nPOLICY->AddTerm(norm1->IntersectL((CXorCompositeAssertion*) target->GetTerms()[0], NULL));
		return nPOLICY;
	}
	case ECompositeXorType:
	case ECompositeAndType:
	case EPrimitiveType: 
	{
		CPolicyAssertion* nPOLICY = CPolicyAssertion::NewL();
		nPOLICY->AddTerm((normalizedMe->GetTerms()[0])->IntersectL(target, NULL));
		return nPOLICY;
	}

	default: 
	{
	}

	}
return NULL;	
}
MAssertion* CPolicyAssertion::MergeL(MAssertion* aAssertion, CPolicyRegistry* aRegistry)
{
    MAssertion* normalizedMe = (IsNormalized()) ? this : NormalizeL(aRegistry);

	if (!(dynamic_cast<CPolicyAssertion*>(normalizedMe))) 
	{
		return normalizedMe->MergeL(aAssertion, aRegistry);
	}

	CPolicyAssertion* nPOLICY = CPolicyAssertion::NewL();

	MAssertion* target = (aAssertion->IsNormalized()) ? aAssertion : aAssertion->NormalizeL(aRegistry);
	short type = target->Type();

    RPolicyTerms terms = normalizedMe->GetTerms();
    if(terms.Count() > 0)
    {
        CXorCompositeAssertion* xorTerm = (CXorCompositeAssertion*)terms[0];
            
    	switch (type) 
    	{

        	case ECompositePolicyType: 
        	{
        	    RPolicyTerms tgtTerms = target->GetTerms();
        	    if(tgtTerms.Count() > 0)
        	    {
                    CXorCompositeAssertion* tgtTerm = (CXorCompositeAssertion*)tgtTerms[0];
                    MAssertion* result = xorTerm->MergeL(tgtTerm, NULL);
                    nPOLICY->AddTerm(result);
        	    }
        		return nPOLICY;
        	}
        	case ECompositeXorType:
        	case ECompositeAndType:
        	case EPrimitiveType:
            {
                MAssertion* result = xorTerm->MergeL(target, NULL);            
        		nPOLICY->AddTerm(result);
        		return nPOLICY;
        	}

        	default: 
        	{
        	}

    	}
    }
    return NULL;    
}

void CPolicyAssertion::SetNameL(TDesC8& aName)
{
    iName = aName.AllocL();
}          
void CPolicyAssertion::SetIdL(TDesC8& aId)
{
    iId = aId.AllocL();
}
TPtrC8 	CPolicyAssertion::Name()
{
    return iName->Des();
}     
TPtrC8 	CPolicyAssertion::Id()
{
    return iId->Des();
}
TPtrC8 CPolicyAssertion::PolicyURI()
{
    return iName->Des();
}
