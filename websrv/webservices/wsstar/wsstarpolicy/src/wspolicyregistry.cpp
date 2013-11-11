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
* Description:        
*
*/








// INCLUDE FILES

#include "wspolicyregistry.h"
#include <SenXmlElement.h> // check if this include is needed(?)
#include "wspolicy.h"

using namespace WSPolicy;


EXPORT_C CPolicyRegistry* CPolicyRegistry::NewL()
{
     CPolicyRegistry* pSelf =
        CPolicyRegistry::NewLC();
    CleanupStack::Pop(pSelf);
    return pSelf;
   
}
EXPORT_C CPolicyRegistry* CPolicyRegistry::NewLC()
{
    CPolicyRegistry* pSelf =
                        new (ELeave) CPolicyRegistry();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;
  
}
CPolicyRegistry::~CPolicyRegistry()
{
   iRegistry.Reset(); // destroys
   delete iParent;
}
EXPORT_C void CPolicyRegistry::PopulateRegistryL(CSenElement* aServicePolicy)	//codescannerwarnings
{
    RPointerArray<CSenElement> policies;
    CleanupClosePushL(policies);
    aServicePolicy->ElementsL(policies,WSPolicy::KWsPolicyNsUri,WSPolicy::KWsPolicy);
    
    CSenElement* pPolicy = NULL;
   
    TInt count(policies.Count());

    if(count > 0)
    {
        
    for(TInt i=0; i<count; i++)
        {
        pPolicy = policies[i];
        if(pPolicy)
            {
                TPtrC8 id = PolicyIdL(pPolicy);
                TPtrC8 name = PolicyNameL(pPolicy);
                if(id != KNullDesC8() && name != KNullDesC8())
                RegisterPolicy(id, pPolicy);
            }
        }
    }
    
    CleanupStack::PopAndDestroy(); // policies
}
EXPORT_C void CPolicyRegistry::ResetRegistry()
{
    iRegistry.Reset();
}

TPtrC8 CPolicyRegistry::PolicyIdL(CSenElement* aPolicy)
{
    RPointerArray<CSenBaseAttribute>& attrs = aPolicy->AttributesL();

    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
        bs = (attrs)[j];
        if(bs)

        if(bs->Name().Compare(WSPolicy::KWsuId) == 0)
            {
             return bs->Value();
            }
         }

    return KNullDesC8();
}
TPtrC8 CPolicyRegistry::PolicyNameL(CSenElement* aPolicy)
{
    RPointerArray<CSenBaseAttribute>& attrs = aPolicy->AttributesL();

    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
        bs = (attrs)[j];
        if(bs)

        if(bs->Name().Compare(WSPolicy::KName) == 0)
            {
             return bs->Value();
            }
         }

    return KNullDesC8();
}


EXPORT_C void CPolicyRegistry::SetParent(CPolicyRegistry* aParent)
{
    iParent = aParent;
}
EXPORT_C CPolicyRegistry* CPolicyRegistry::Parent()
{
    return iParent;
}

EXPORT_C CSenElement* CPolicyRegistry::LookupPolicyByNameL(const TPtrC8& aPolicyName)	//codescannerwarnings
{
    if(aPolicyName.Length() <1)
        return NULL;
    
    TInt count = iRegistry.Count();
    
    for (TInt i = 0; i< count; i++)
    {
      CSenElement* policy = (CSenElement*)iRegistry.ValueAt(i);
      
      if(policy)
      {
        TPtrC8 name = PolicyNameL(policy);
        TPtrC8 policyUri = CWSPolicyUtils::PolicyUriL(policy);
        if(name != KNullDesC8() && (name.Compare(aPolicyName) == 0 
                                || policyUri.Compare(aPolicyName) == 0))
        {
            return  policy;  
        }
      }
        
    }

    return NULL;
}
EXPORT_C CSenElement* CPolicyRegistry::LookupPolicy(const TPtrC8& aPolicyID)
{

    TInt index = iRegistry.Find(aPolicyID);
    if(index!=KErrNotFound)
    {
        CSenElement* policy = (CSenElement*)iRegistry.ValueAt(index);
        TDesC8* policyId(iRegistry.KeyAt(index));

        if (policy == NULL && iParent != NULL) 
        {//No recursive calls as it is a flat array of policies
//            policy = iParent->LookupPolicy(aPolicyURI);
        }
        
        return policy;
    }
    else
    return NULL;
}
EXPORT_C void CPolicyRegistry::RegisterPolicy(TDesC8& aPolicyID, CSenElement* aPolicy)
{
    
    TInt index = iRegistry.Find(aPolicyID);
    if(index == KErrNotFound)
    {
        TInt  append = iRegistry.Append(aPolicyID.Alloc(), aPolicy);    
//        if(append==KErrNone) { }
    }
    
}
EXPORT_C void CPolicyRegistry::UnregisterPolicy(const TPtrC8& aPolicyID)
{
    TInt  retVal = iRegistry.RemoveByKey(aPolicyID);
    if(retVal == KErrNone)
    {
        
    }
                
}
CPolicyRegistry::CPolicyRegistry():
iRegistry(ETrue,EFalse),
iParent(NULL)
{
    
}
void CPolicyRegistry::ConstructL()
{
//    if(aParent != NULL)
//     iParent = aParent;       
}
