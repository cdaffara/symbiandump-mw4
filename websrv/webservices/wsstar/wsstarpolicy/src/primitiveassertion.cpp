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






 

#include "primitiveassertion.h"
#include "policyassertion.h"
#include "xorcompositeassertion.h"
#include "andcompositeassertion.h"
#include "wspolicyreader.h"
#include "wspolicyutils.h"
using namespace WSPolicy;

CPrimitiveAssertion* CPrimitiveAssertion::NewL()
{
    CPrimitiveAssertion* pSelf = CPrimitiveAssertion::NewLC();
    CleanupStack::Pop(pSelf);
    return pSelf;
    
}
CPrimitiveAssertion* CPrimitiveAssertion::NewLC()
{
     CPrimitiveAssertion* pSelf = new (ELeave) CPrimitiveAssertion();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;
   
}
CPrimitiveAssertion* CPrimitiveAssertion::NewL(CPrimitiveAssertion* aValue)
{
    CPrimitiveAssertion* pSelf = CPrimitiveAssertion::NewLC(aValue);
    CleanupStack::Pop(pSelf);
    return pSelf;
    
}
CPrimitiveAssertion* CPrimitiveAssertion::NewLC(CPrimitiveAssertion* aValue)
{
    CPrimitiveAssertion* pSelf = new (ELeave) CPrimitiveAssertion();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aValue);
    return pSelf;
}

CPrimitiveAssertion* CPrimitiveAssertion::NewL(TDesC8& aName, TDesC8& aPrefix, TDesC8& aUri)
{
    CPrimitiveAssertion* pSelf = CPrimitiveAssertion::NewLC(aName,aPrefix, aUri);
    CleanupStack::Pop(pSelf);
    return pSelf;
    
}
CPrimitiveAssertion* CPrimitiveAssertion::NewLC(TDesC8& aName, TDesC8& aPrefix, TDesC8& aUri)
{
     CPrimitiveAssertion* pSelf = new (ELeave) CPrimitiveAssertion();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aName, aPrefix, aUri);
    return pSelf;
   
}

CPrimitiveAssertion::~CPrimitiveAssertion()
    {
    delete iQname;//	QName qname;
    if(iAttr != NULL)
    	{
        iAttr->Reset(); //Hashtable attributes = new Hashtable();
        delete iAttr;
    	}
	delete iStrValue;
    }

CPrimitiveAssertion::CPrimitiveAssertion():
iOwner(NULL), iIsOptional(EFalse),iStrValue(NULL)
{
    
}
void CPrimitiveAssertion::ConstructL()
{
    iAttr = new (ELeave) RAttributeMap(ETrue, ETrue);    
}
void CPrimitiveAssertion::ConstructL(CPrimitiveAssertion* aValue)
{
    CQName* temp = aValue->QName();
    if(temp)
    {
        TPtrC8 name = temp->Name();
        TPtrC8 prefix = temp->Prefix();
        TPtrC8 uri = temp->Uri();
        ConstructL(name, prefix,uri);
    
        SetAttributesL(aValue->Attributes());

        TPtrC8 strval = aValue->StrValue();
        iStrValue = strval.AllocL();
        
        RPolicyTerms terms = aValue->GetTerms();
        
        if(terms.Count() > 0)
            AddTermsCopyL(terms);
    }
        
}

void CPrimitiveAssertion::ConstructL(TDesC8& aName, TDesC8& aPrefix, TDesC8& aUri)
{
    iQname = CQName::NewL(aName, aPrefix, aUri);
    ConstructL();
}
MAssertion*	CPrimitiveAssertion::GetParent()
{
    return iOwner;   
}
RPolicyTerms& CPrimitiveAssertion::GetTerms()
{  
    return CAssertion::GetTerms();
}
TAssertionType CPrimitiveAssertion::Type()
{
  return EPrimitiveType;    
}
TBool CPrimitiveAssertion::HasParent()
{
    if(iOwner)    
    return ETrue;
    else
    return EFalse;
}
TBool CPrimitiveAssertion::IsEmpty()
{
    if(Size() > 0)
        return EFalse;
    else
        return ETrue;
}
void AppendAll(RPolicyTerms& aSource, RPolicyTerms& aDest)
{
    TInt count = aSource.Count();
    if(count > 0)
    {
        for (TInt i = 0; i < count; i++)
        {
            MAssertion* term = aSource[i];
            aDest.Append(term);
        }
    }
}
MAssertion* CPrimitiveAssertion::NormalizeL()
{
    return NormalizeL(NULL);
}
MAssertion* CPrimitiveAssertion::NormalizeL(CPolicyRegistry* aRegistry)
{
    if (IsNormalized()) 
    {
    	return this;
    }

    if (IsOptional()) 
    {

    	CXorCompositeAssertion* XOR = CXorCompositeAssertion::NewL();
    	CAndCompositeAssertion* AND = CAndCompositeAssertion::NewL();

    	CPrimitiveAssertion* PRIM = GetSelfWithoutTermsL();
    	PRIM->RemoveAttribute(KWspOptinal);
    	PRIM->SetOptional(false);
    	RPolicyTerms terms1 = GetTerms();
    	PRIM->SetTermsL(terms1);
        
    	AND->AddTerm(PRIM);
    	XOR->AddTerm(AND);
    	XOR->AddTerm(CAndCompositeAssertion::NewL());
        
        MAssertion* result = XOR->NormalizeL(aRegistry);
        delete XOR;
    	return result;

    }
    RPolicyTerms terms2 = GetTerms();
    if (terms2.Count() == 0) 
    {
    	CPrimitiveAssertion* PRIM = GetSelfWithoutTermsL();
    	PRIM->SetNormalized(true);
    	return PRIM;
    }

    RPolicyTerms policyTerms;
    RPolicyTerms nonPolicyTerms;
    for(TInt i =0 ; i < terms2.Count(); i++) 
    {
    	MAssertion* term = terms2[i];

    	if (dynamic_cast<CPolicyAssertion*>(term)) 
    	{
    		policyTerms.Append(term);

    	} 
    	else if (dynamic_cast<CPrimitiveAssertion*>(term)) 
    	{
    		nonPolicyTerms.Append(term);

    	} 
    	else 
    	{
    	}
    }

    if (policyTerms.Count() == 0) {
    	CPrimitiveAssertion* PRIM = CPrimitiveAssertion::NewL(this);
    	PRIM->SetNormalized(true);
        policyTerms.Close();
        nonPolicyTerms.Close();
    	return PRIM;
    }

    CPolicyAssertion* policyTerm = CWSPolicyUtils::GetSinglePolicyL(policyTerms, aRegistry);
    RPolicyTerms polTerms = policyTerm->GetTerms();
    if(polTerms.Count() > 0)
    {
        MAssertion* xorTerm = polTerms[0];
        RPolicyTerms ANDs = xorTerm->GetTerms();

        if (ANDs.Count() == 0) 
        {
            delete policyTerm;
            policyTerms.Close();
            nonPolicyTerms.Close();
        	return CXorCompositeAssertion::NewL();
        }

        if (ANDs.Count() == 1) 
        {
        //DONE
            CAndCompositeAssertion* term = (CAndCompositeAssertion*) ANDs[0];
        	term->AddTermsCopyL(nonPolicyTerms);
        	CPrimitiveAssertion* PRIM = GetSelfWithoutTermsL();
        	PRIM->AddTerm(policyTerm);
            policyTerms.Close();
            nonPolicyTerms.Close();
        	return PRIM;
        }

        CPolicyAssertion* nPOLICY = CPolicyAssertion::NewL();
        CXorCompositeAssertion* nXOR = CXorCompositeAssertion::NewL();
        nPOLICY->AddTerm(nXOR);

        RPolicyTerms list;

        for (TInt j = 0; j < ANDs.Count(); j++) 
        {
        	CPrimitiveAssertion* nPRIM = GetSelfWithoutTermsL();
        	
            CAndCompositeAssertion* andTerm = (CAndCompositeAssertion*)ANDs[j];
            RPolicyTerms terms3 = andTerm->GetTerms();
        	AppendAll(terms3, list);

        	if (nonPolicyTerms.Count() > 0) 
        	{
        		AppendAll(nonPolicyTerms, list);
        	}
        	CPolicyAssertion* pol = GetSinglePolicyL(list);
        	nPRIM->AddTerm(pol);
        	
        	CAndCompositeAssertion* AND = CAndCompositeAssertion::NewL();
        	AND->AddTerm(nPRIM);
        	nXOR->AddTerm(AND);
        	list.Close();
        }

    	delete policyTerm;
        policyTerms.Close();
        nonPolicyTerms.Close();
        
        nPOLICY->SetNormalized(ETrue);
        return nPOLICY;    
    }
    
    return NULL;    
}
MAssertion* CPrimitiveAssertion::IntersectL(MAssertion* aAssertion)
{
    return IntersectL(aAssertion, NULL); 
}
MAssertion* CPrimitiveAssertion::IntersectL(MAssertion* aAssertion, CPolicyRegistry* aRegistry)
{
	MAssertion* normalizedMe = (IsNormalized()) ? this : NormalizeL(aRegistry);

	if (!(dynamic_cast<CPrimitiveAssertion*>(normalizedMe))) 
	{
		return normalizedMe->IntersectL(aAssertion, aRegistry);
	}

	MAssertion* target = (aAssertion->IsNormalized()) ? aAssertion : aAssertion->NormalizeL(aRegistry);

	// Am not a primitive assertion anymore ..
	if (!(dynamic_cast<CPrimitiveAssertion*>(aAssertion))) 
	{
		return normalizedMe->IntersectL(aAssertion, aRegistry);
	}

	// argument is not primitive type ..
	if (!(dynamic_cast<CPrimitiveAssertion*>(target))) 
	{
		return target->IntersectL(normalizedMe, aRegistry);
	}

	CPrimitiveAssertion* arg = (CPrimitiveAssertion*) target;
	CPrimitiveAssertion* self = (CPrimitiveAssertion*) normalizedMe;

	if (!self->Name().Compare(arg->Name()) == 0) 
	{
		return CXorCompositeAssertion::NewL(); // no bahaviour is admisible
	}

	if (self->GetTerms().Count() == 0 && arg->GetTerms().Count() == 0) 
	{
		CAndCompositeAssertion* assertion2 = CAndCompositeAssertion::NewL();
		assertion2->AddTermCopyL(self);
		assertion2->AddTermCopyL(arg);
		return assertion2;
	}

	if (self->GetTerms().Count() == 0 || arg->GetTerms().Count() == 0) 
	{
		return CXorCompositeAssertion::NewL(); // no
	}

	RPolicyTerms argChildTerms;
	TBool closeArrayarg = EFalse;
	if (dynamic_cast<CPolicyAssertion*>(arg->GetTerms()[0])) 
	{
		CWSPolicyUtils::GetPrimTermsListL((CPolicyAssertion*) arg->GetTerms()[0], argChildTerms);
		closeArrayarg = ETrue;
	}
	else 
	{
		argChildTerms = arg->GetTerms();
	}

	RPolicyTerms selfChildTerms;
	TBool closeArrayself = EFalse;
	if (dynamic_cast<CPolicyAssertion*>(self->GetTerms()[0])) 
	{
		CWSPolicyUtils::GetPrimTermsListL((CPolicyAssertion*) self->GetTerms()[0], selfChildTerms);
		closeArrayself = ETrue;
	} 
	else 
	{
		selfChildTerms = self->GetTerms();
	}

	///////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	CPrimitiveAssertion* PRIMITIVE_A = NULL;
	CPrimitiveAssertion* PRIMITIVE_B = NULL;
	RPolicyTerms primListA, primListB;

	if (selfChildTerms.Count() > argChildTerms.Count()) {
		primListA = selfChildTerms;
		primListB = argChildTerms;
	} else {
		primListA = argChildTerms;
		primListB = selfChildTerms;
	}

    for (TInt i = 0 ; i < primListA.Count(); i++)
    {
		PRIMITIVE_A = (CPrimitiveAssertion*) primListA[i];

		TBool found = EFalse;
        for (TInt j = 0 ; j < primListB.Count(); j++)
        {
			PRIMITIVE_B = (CPrimitiveAssertion*) primListB[j];
			if (PRIMITIVE_A->Name().Compare(PRIMITIVE_B->Name()) == 0)
			{
				found = ETrue;
				break;
			}
		}

		if (!found) 
		{
		    if(closeArrayarg)
		        argChildTerms.Close();
		    if(closeArrayself)
		        selfChildTerms.Close();
		    
			return CXorCompositeAssertion::NewL();
		}

        MAssertion* assert = PRIMITIVE_A->IntersectL(PRIMITIVE_B);
		if (dynamic_cast<CXorCompositeAssertion*>(assert)) 
		{
		    if(closeArrayarg)
		        argChildTerms.Close();
		    if(closeArrayself)
		        selfChildTerms.Close();
		
		    DeleteAssertion(assert);
			return CXorCompositeAssertion::NewL();
		}
		else
		{
		    DeleteAssertion(assert);
		}
	}

	CAndCompositeAssertion* andCompositeAssertion = CAndCompositeAssertion::NewL();
	andCompositeAssertion->AddTermCopyL(arg);
	andCompositeAssertion->AddTermCopyL(self);

		    if(closeArrayarg)
		        argChildTerms.Close();
		    if(closeArrayself)
		        selfChildTerms.Close();
		    
	return andCompositeAssertion;
    
}
MAssertion* CPrimitiveAssertion::MergeL(MAssertion* aAssertion)
{
    return MergeL(aAssertion, NULL);
}
MAssertion* CPrimitiveAssertion::MergeL(MAssertion* aAssertion, CPolicyRegistry* aRegistry)
{
	MAssertion* normalizedMe = (IsNormalized()) ? this : NormalizeL(aRegistry);

	if (!(dynamic_cast<CPrimitiveAssertion*>(normalizedMe))) 
	{
		return normalizedMe->MergeL(aAssertion, aRegistry);
	}

	MAssertion* target = (aAssertion->IsNormalized()) ? aAssertion : aAssertion->NormalizeL(aRegistry);

	if (!(dynamic_cast<CPrimitiveAssertion*>(target))) 
	{
		return target->MergeL(normalizedMe, aRegistry);
	}

	/*
	 * both self and the argument are primitive assertions. Hence both
	 * should be wrapped in an AndCompositeType
	 */
	CAndCompositeAssertion* AND = CAndCompositeAssertion::NewL();
	AND->AddTermCopyL(target);
	AND->AddTermCopyL(normalizedMe);
	return AND;
    
}
TBool CPrimitiveAssertion::Remove(MAssertion* aAssertion)
{
    if(aAssertion)
    {
        CAssertion::Remove(aAssertion);
    }
    return ETrue;
    
}
TInt CPrimitiveAssertion::Size()
{
    return CAssertion::Size();
}


TInt CPrimitiveAssertion::SetTermsL(RPolicyTerms& aTerms)
{
		this->AddTermsCopyL(aTerms);
		return KErrNone;    
}
void  CPrimitiveAssertion::AddAttributeL(TDesC8& aQname, TDesC8& aValue)
{
    TInt indx = iAttr->Find(aQname);
    if(indx == KErrNotFound)
    {
        iAttr->Append(aQname.AllocL(),aValue.AllocL());        
    }
    else
    {
        iAttr->UpdateValue(aQname.AllocL(),aValue.AllocL());    
    }
}
TPtrC8 CPrimitiveAssertion::GetAttributeValue(const TDesC8& aQname)
{
    TInt indx = iAttr->Find(aQname);
    if(indx != KErrNotFound)
    {
        const TDesC8* value =  iAttr->ValueAt(indx);
        return *value; //->Ptr();
    }
    return KNullDesC8();
}
RAttributeMap& CPrimitiveAssertion::Attributes()
{
//    aAttributes.Copy(iAttr);
return *iAttr;
}
TInt CPrimitiveAssertion::SetAttributesL(RAttributeMap& aAttributes)
{
 
 TInt count = aAttributes.Count();
 for (TInt i=0; i< count; i++)   
 {
    const TDesC8* name = aAttributes.KeyAt(i);
    const TDesC8* value = aAttributes.ValueAt(i);
    HBufC8* tt = name->AllocL();
    HBufC8* tt2 = value->AllocL();
    TPtrC8 namePtr = tt->Des();
    TPtrC8 valPtr = tt2->Des();
    AddAttributeL(namePtr, valPtr);
    delete tt;
    delete tt2;
 }
    return KErrNone;
}
TInt  CPrimitiveAssertion::RemoveAttribute(const TDesC8& aQname)
{
    TInt indx = iAttr->Find(aQname);
    if(indx != KErrNotFound)
    {
        iAttr->RemoveByKey(aQname);
        return KErrNone;
    }
    return KErrNotFound;
}

CQName* CPrimitiveAssertion::QName()
{   if(iQname)
        return iQname;
    else
        return NULL;
}
TPtrC8 CPrimitiveAssertion::Name()
{   if(iQname)
    return iQname->QName();
else
    return KNullDesC8();
}
TPtrC8 CPrimitiveAssertion::StrValue()
{
    if(iStrValue)
    return iStrValue->Des();
    else
    return KNullDesC8();
}
void CPrimitiveAssertion::SetStrValueL(TDesC8& aStrValue)
{
    if(aStrValue.Length() > 0)
        iStrValue = aStrValue.AllocL();     
}
TAny* CPrimitiveAssertion::Value()
{
  return NULL;
}

TBool CPrimitiveAssertion::IsOptional()
{
    return iIsOptional;    
}
TInt CPrimitiveAssertion::SetOptional(TBool aOptional)
{
    iIsOptional = aOptional; 
    return KErrNone; 
}

CPolicyAssertion* CPrimitiveAssertion::GetSinglePolicyL(RPolicyTerms aChildTerms)
{

		CPolicyAssertion* policy = CPolicyAssertion::NewL();
		CXorCompositeAssertion* pXor = CXorCompositeAssertion::NewL();
		CAndCompositeAssertion* pAnd = CAndCompositeAssertion::NewL();
		pAnd->AddTermsCopyL(aChildTerms);
		pXor->AddTerm(pAnd);
		policy->AddTerm(pXor);
		return policy;

}

TBool CPrimitiveAssertion::IsEmptyPolicy(CPolicyAssertion* aPolicy) 
{

        RPolicyTerms terms = aPolicy->GetTerms();
        if(terms.Count() > 0)
        {
   	    	CXorCompositeAssertion* xorTerm = (CXorCompositeAssertion* )terms[0];
    		return xorTerm->IsEmpty();
        }
        else
            return EFalse;

}

RPolicyTerms CPrimitiveAssertion::GetTerms(CPolicyAssertion* aPolicy) 
{

    RPolicyTerms terms1 = aPolicy->GetTerms();
    if(terms1.Count() > 0)
    {
        CXorCompositeAssertion* xorTerm = (CXorCompositeAssertion*)terms1[0];
        if(xorTerm)
        {
            RPolicyTerms terms2 = xorTerm->GetTerms();
            if(terms2.Count() > 0)
            {
                CAndCompositeAssertion* andTerm= (CAndCompositeAssertion*) terms2[0];
                if(andTerm)
                    return andTerm->GetTerms();
            }
        }
    }
    RPolicyTerms empty_terms;
    return empty_terms;
}

CPrimitiveAssertion* CPrimitiveAssertion::GetSelfWithoutTermsL() 
{
    TPtrC8 name = iQname->Name();
    TPtrC8 prefix = iQname->Prefix();
    TPtrC8 uri = iQname->Uri();
    
	CPrimitiveAssertion* prim = CPrimitiveAssertion::NewL(name, prefix, uri);
	prim->SetAttributesL(*iAttr);
	TPtrC8 strval = StrValue();
	prim->SetStrValueL(strval);
	return prim;
}


CQName* CQName::NewL(TDesC8& aName, TDesC8& aPrefix, TDesC8& aUri)
{
    CQName* pSelf = CQName::NewLC(aName, aPrefix, aUri);
    CleanupStack::Pop(pSelf);
    return pSelf;
    
}
CQName* CQName::NewLC(TDesC8& aName, TDesC8& aPrefix, TDesC8& aUri)
{
    CQName* pSelf = new (ELeave) CQName();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aName, aPrefix, aUri);
    return pSelf;
   
}
CQName::~CQName()
{
    delete iName;
    delete iQName;
    delete iPrefix;
    delete iUri;	
}
TPtrC8 CQName::Name()
{
    return iName->Des();
}
TPtrC8 CQName::QName()
{
    return iQName->Des();
}
TPtrC8 CQName::Prefix()
{
    return iPrefix->Des();
}
TPtrC8 CQName::Uri()
{
    return iUri->Des();
}
CQName::CQName()
{
    
}

void CQName::ConstructL(TDesC8& aName, TDesC8& aPrefix, TDesC8& aUri)
    {
	_LIT8(KColon, ":"); 
    iName = aName.AllocL();
    iPrefix = aPrefix.AllocL();
    iUri = aUri.AllocL();
    iQName = HBufC8::NewL( aPrefix.Length() + KColon().Length() + aName.Length() );
    TPtr8 qname = iQName->Des();
    qname.Append( aPrefix );
    qname.Append( KColon );
    qname.Append( aName );
    }
// End of File


