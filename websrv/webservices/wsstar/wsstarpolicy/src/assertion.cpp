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








#include "assertion.h"
#include "andcompositeassertion.h"
#include "xorcompositeassertion.h"
#include "wspolicywriter.h"
#include "policyreferenceassertion.h"
#include "sendebug.h"                 // internal Utils\inc - logging MACROs

CAssertion* CAssertion::NewL()
{
    CAssertion* pSelf = CAssertion::NewLC();
    CleanupStack::Pop(pSelf);
    return pSelf;

}
CAssertion* CAssertion::NewLC()
{
    CAssertion* pSelf = new (ELeave) CAssertion();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;
}
CAssertion::~CAssertion()
{
    if(iChildterms.Count() > 0)
        {
        for(TInt i=0; i< iChildterms.Count(); i++) 
            {
            MAssertion* assert = iChildterms[i];
            DeleteAssertion(assert);
            }
        }
    iChildterms.Reset();
}
    
CAssertion::CAssertion() :
iParent(NULL),
iNormalized(EFalse)
{    
}

void CAssertion::ConstructL()
{
}

void CAssertion::AddTerm(MAssertion* aAssertion)
{
    if(aAssertion)
    {
        iChildterms.Append(aAssertion);    
    }
}

void 	CAssertion::AddTerms(RPolicyTerms& aAssertions)
{
  for(TInt i = 0; i<aAssertions.Count(); i++ )  
  {
    MAssertion* assertion  = aAssertions[i];
    AddTerm(assertion);
  }
}

void CAssertion::AddTermCopyL(MAssertion* aAssertion)
{
    MAssertion* eleToAdd = NULL;
    if (aAssertion->Type() == EPrimitiveType)
    {
    
		eleToAdd = CPrimitiveAssertion::NewL((CPrimitiveAssertion*)aAssertion);

	}
	else if (aAssertion->Type() == ECompositeXorType)
	{
		eleToAdd = CXorCompositeAssertion::NewL((CXorCompositeAssertion*)aAssertion);
	}
    else if (aAssertion->Type() == EPolicyReferenceType) 
    {
	    eleToAdd = CPolicyReferenceAssertion::NewL((CPolicyReferenceAssertion*)aAssertion);
    }
	else if (aAssertion->Type() == ECompositePolicyType)
	{
        eleToAdd = CPolicyAssertion::NewL((CPolicyAssertion*)aAssertion);
	
	} 
	else if (aAssertion->Type() == ECompositeAndType)
	{
		eleToAdd = CAndCompositeAssertion::NewL((CAndCompositeAssertion*)aAssertion);

	} 
	else
	{
	}
	
	
    if(eleToAdd)
    {
        iChildterms.Append(eleToAdd);    	
    }
    
}
void CAssertion::AddTermsCopyL(RPolicyTerms& aAssertions)
{
  for(TInt i = 0; i<aAssertions.Count(); i++ )  
  {
    MAssertion* assertion  = aAssertions[i];
    AddTermCopyL(assertion);
  }
    
}
void CAssertion::MoveTermL(RPolicyTerms& /*aAssertions*/)
{
    
}
void CAssertion::MoveTermsL(RPolicyTerms& aAssertions)
{

  for(TInt i = 0; i<aAssertions.Count(); i++ )  
  {
    MAssertion* assertion  = aAssertions[i];
    
    if(assertion)
    {
        iChildterms.Append(assertion);
    }
        
  }
    aAssertions.Reset();
}

MAssertion* CAssertion::GetParent()
{
    if(iParent)
        return  iParent;
    else
        return NULL;
   
}
RPolicyTerms& CAssertion::GetTerms()
{
    return iChildterms;
}
TBool CAssertion::HasParent()
{
    if(iParent)
        return  ETrue;
    else
        return EFalse;
}
MAssertion*	CAssertion::IntersectL(MAssertion* aAssertion)
{
    return IntersectL(aAssertion, NULL);    
}
TBool CAssertion::IsEmpty()
{
    if(iChildterms.Count() < 1)
        return ETrue;
    else
        return EFalse;
}
TBool CAssertion::IsNormalized()
{
    return iNormalized;
}
MAssertion* CAssertion::MergeL(MAssertion* aAssertion)
{
    return MergeL(aAssertion, NULL);
}
MAssertion* CAssertion::NormalizeL()
{
    return NormalizeL(NULL);
}
TBool CAssertion::Remove(MAssertion* aAssertion)
{
    if(Size() > 0)
    {
        TInt indx = iChildterms.Find(aAssertion);
        if(indx != KErrNotFound)
        {
            iChildterms.Remove(indx);
            return ETrue;
        }
    }
    
    return EFalse;
}

void CAssertion::SetNormalized(TBool aFlag)
{
    iNormalized = aFlag;
    for (TInt i = 0; i < iChildterms.Count(); i++)
	{
    	((MAssertion*)iChildterms[i])->SetNormalized(aFlag);
	}
}
void CAssertion::SetParent(MAssertion* aParent)
{
    iParent = aParent;
}
TInt CAssertion::Size()
{
    return iChildterms.Count();    
}

MAssertion* CAssertion::IntersectL(MAssertion* /*aAssertion*/, CPolicyRegistry* /*aRegistry*/)
{
    return NULL;   
}
MAssertion* CAssertion::MergeL(MAssertion* /*aAssertion*/, CPolicyRegistry* /*aRegistry*/)
{
    return NULL;   
}
MAssertion* CAssertion::NormalizeL(CPolicyRegistry* /*aRegistry*/)
{
    return NULL;    
}

TAssertionType CAssertion::Type()
{
    return EInvalidType;   
}
TInt CAssertion::DeleteAssertion(MAssertion* aAssertion)
{
    if(aAssertion == NULL)
        return KErrNone;
    
    
    if (dynamic_cast<CPrimitiveAssertion*>(aAssertion))
    {
		CPrimitiveAssertion* ele = (CPrimitiveAssertion*)aAssertion;
        delete ele;
        ele = NULL;    
	} 
	else if (dynamic_cast<CXorCompositeAssertion*>(aAssertion))
	{
		CAssertion* ele = (CAssertion*)aAssertion;
        delete ele;
        ele = NULL;    
	} 
    else if (dynamic_cast<CPolicyReferenceAssertion*>(aAssertion)) 
    {
		CPolicyReferenceAssertion* ele = (CPolicyReferenceAssertion*)aAssertion;
        delete ele;
        ele = NULL;    
	} 
	else if (dynamic_cast<CPolicyAssertion*>(aAssertion))
	{
        CPolicyAssertion* ele = (CPolicyAssertion*)aAssertion;
        delete ele;
        ele = NULL;    
	}
	else if (dynamic_cast<CAndCompositeAssertion*>(aAssertion))
	{
		CAssertion* ele = (CAssertion*)aAssertion;
        delete ele;
        ele = NULL;    
	}
    return KErrNone;
}

// End of File
