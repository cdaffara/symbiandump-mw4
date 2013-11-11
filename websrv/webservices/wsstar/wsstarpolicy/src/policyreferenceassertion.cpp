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









#include "policyreferenceassertion.h"
#include "wspolicyregistry.h"
#include "policyassertion.h"
#include "wspolicyreader.h"
CPolicyReferenceAssertion* CPolicyReferenceAssertion::NewL(TDesC8& aUri)
{
    CPolicyReferenceAssertion* pSelf = CPolicyReferenceAssertion::NewLC(aUri);
    CleanupStack::Pop(pSelf);
    return pSelf;
    
}
CPolicyReferenceAssertion* CPolicyReferenceAssertion::NewLC(TDesC8& aUri)
{
    CPolicyReferenceAssertion* pSelf = new (ELeave) CPolicyReferenceAssertion();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aUri);
    return pSelf;
    
}
CPolicyReferenceAssertion* CPolicyReferenceAssertion::NewL(CPolicyReferenceAssertion* aValue)
{
    CPolicyReferenceAssertion* pSelf = CPolicyReferenceAssertion::NewLC(aValue);
    CleanupStack::Pop(pSelf);
    return pSelf;
    
}
CPolicyReferenceAssertion* CPolicyReferenceAssertion::NewLC(CPolicyReferenceAssertion* aValue)
{
    CPolicyReferenceAssertion* pSelf = new (ELeave) CPolicyReferenceAssertion();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aValue);
    return pSelf;
    
}
CPolicyReferenceAssertion::~CPolicyReferenceAssertion()
{
    delete iUri;    
}
CPolicyReferenceAssertion::CPolicyReferenceAssertion()
:iUri(NULL)
{
    
}
void CPolicyReferenceAssertion::ConstructL(TDesC8& aUri)
{
    if(aUri.Length() > 0)
    {
        iUri = aUri.AllocL();
    }
}
TPtrC8 CPolicyReferenceAssertion::PolicyUri()
{
    if(iUri && iUri->Length() > 0)
    {
        TPtrC8 p = iUri->Des();
        return p;
    }
    else
    {
        return KNullDesC8();
    }
}

void CPolicyReferenceAssertion::ConstructL(CPolicyReferenceAssertion* aValue)
{
    if(aValue->PolicyUri().Length() > 0)
    {
        TPtrC8 uri = aValue->PolicyUri();
        ConstructL(uri);
    }
}
void CPolicyReferenceAssertion::AddTerm(MAssertion* /*aAssertion*/)
{
    
}
void CPolicyReferenceAssertion::AddTerms(RPolicyTerms& /*aAssertions*/)
{
    
}
void CPolicyReferenceAssertion::AddTermCopyL(MAssertion* /*aAssertion*/)
{
    
}
void CPolicyReferenceAssertion::AddTermsCopyL(RPolicyTerms& /*aAssertions*/)
{
    
}
void CPolicyReferenceAssertion::MoveTermL(RPolicyTerms& /*aAssertions*/)
{
    
}
void CPolicyReferenceAssertion::MoveTermsL(RPolicyTerms& /*aAssertions*/)
{
    
}
MAssertion* CPolicyReferenceAssertion::GetParent()
{
  return NULL;  
}
TBool CPolicyReferenceAssertion::HasParent()
{
    return EFalse;
}
TBool CPolicyReferenceAssertion::IsEmpty()
{
    return ETrue;
}
TBool CPolicyReferenceAssertion::IsNormalized()
{
    return EFalse;
}
TBool CPolicyReferenceAssertion::Remove(MAssertion* /*aAssertion*/)
{
    return EFalse;
}
void CPolicyReferenceAssertion::SetNormalized(TBool /*aFlag*/)
{
    
}
void CPolicyReferenceAssertion::SetParent(MAssertion* /*aParent*/)
{
    
}
TInt CPolicyReferenceAssertion::Size()
{
    return KErrNone;
}
TAssertionType  CPolicyReferenceAssertion::Type()
{
    return EPolicyReferenceType;    
}

MAssertion* CPolicyReferenceAssertion::IntersectL(MAssertion* /*aAssertion*/, CPolicyRegistry* /*aRegistry*/)
{
    return NULL;
}
MAssertion* CPolicyReferenceAssertion::IntersectL(MAssertion* /*aAssertion*/)
{
    return NULL;
}
MAssertion* CPolicyReferenceAssertion::MergeL(MAssertion* /*aAssertion*/, CPolicyRegistry* /*aRegistry*/)
{
    return NULL;
}
MAssertion* CPolicyReferenceAssertion::MergeL(MAssertion* /*aAssertion*/)
{
    return NULL;
}
MAssertion* CPolicyReferenceAssertion::NormalizeL(CPolicyRegistry* aRegistry)
{
    if (aRegistry == NULL) 
    {
        return NULL;
    }

    _LIT8(KHash, "#");
    HBufC8* pRippedUri = NULL;	//CodeScannerWarnings
    MAssertion* result = NULL;
    
    TPtrC8 uri = PolicyUri();
    
    if(uri.Left(1).Compare(KHash) == 0)
    {
        pRippedUri = uri.Right(uri.Length()-1).AllocL(); 
        CleanupStack::PushL(pRippedUri);
        if(pRippedUri->Length() > 0)
        {
            
            CSenElement* targetPolicy = aRegistry->LookupPolicy(pRippedUri->Des());
            if (targetPolicy == NULL)
            {
                result = NULL;;
            }
            else
            {
                CWSPolicyReader* reader = CWSPolicyReader::NewLC();
                CPolicyAssertion* policy = reader->GetPolicyL(targetPolicy);
                CleanupStack::PopAndDestroy();
                CleanupStack::PushL( policy );
                result = policy->NormalizeL( aRegistry );
                CleanupStack::PopAndDestroy( policy );
            }
            
        }
        CleanupStack::PopAndDestroy(pRippedUri);
    }
    return result;

}
MAssertion* CPolicyReferenceAssertion::NormalizeL()
{
    return NULL;
}
