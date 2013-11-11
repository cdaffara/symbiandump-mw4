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
#include <f32file.h>
#include "PolicyNormalizer.h"
#include "SenXmlElement.h"
#include "WSPolicyRegistry.h"

using namespace WSPolicy;

namespace
{
_LIT( KOutPath, "c:\\logs\\out.xml");
_LIT8(KColon, ":");
//_LIT8( KServicePolicyLocalName, "ServicePolicy");
}

    
    
CPolicyNormalizer* CPolicyNormalizer::NewL()
    {
    CPolicyNormalizer* pSelf =
        CPolicyNormalizer::NewLC();
    CleanupStack::Pop(pSelf);
    return pSelf;
    }

CPolicyNormalizer* CPolicyNormalizer::NewLC()
    {
    CPolicyNormalizer* pSelf =
                        new (ELeave) CPolicyNormalizer();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;
    }

CPolicyNormalizer::~CPolicyNormalizer()
{
    delete iXmlReader;
    delete iPath;
    delete iNewPolicy;
    iOrphanElements.ResetAndDestroy();
}

CPolicyNormalizer::CPolicyNormalizer()
    {
    }
void CPolicyNormalizer::ConstructL()
    {
//    BaseConstructL(KServicePolicyLocalName());
    iXmlReader = CSenXmlReader::NewL();
    iNewPolicy = CSenXmlElement::NewL(KWsPolicy);
    count = 0;
    }
    


CSenElement* CPolicyNormalizer::NormalizeL(CPolicyRegistry* aRegistry)
{

    CSenElement& element = AsElement();
    TPtrC8 name = element.LocalName();

    if (name.Compare(KWsPolicy) == 0)
    {
        iNewPolicy->CopyFromL(element);
        
        RPointerArray<CSenElement>& children = iNewPolicy->ElementsL();
        CleanupClosePushL(children);
        TInt childCount = children.Count();
        
        if(childCount > 0)
        {
            CSenElement* pNextChild;   
            TInt i = 0;
            
            while (i < childCount)
            {   
                
                pNextChild = children[childCount-1];
                CSenElement* newElement = iNewPolicy->RemoveElement(*pNextChild);
                delete newElement;
                childCount--;
            }
                
        }
        CleanupStack::PopAndDestroy(); //children
       ProcessPolicyReferenceL(&element, aRegistry);
       ProcessPolicyTermL(&element, iNewPolicy);
       ProcessLogicL(iNewPolicy);
       ProcessAssertionBranchingL(iNewPolicy);
       
//#ifdef _SENDEBUG       
       WriteL(iNewPolicy);
//#endif
       return iNewPolicy;
    }

    return NULL; //&element;   

}

TPtrC8 CPolicyNormalizer::GetReferenceUriL(CSenElement* aReference)
    {
    RPointerArray<CSenBaseAttribute>& attrs = aReference->AttributesL();
    
    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
         bs = (attrs)[j];
         if(bs->Name().Compare(KUri) == 0)
         {
            return bs->Value();
         }
            
              
         }
    return KNullDesC8();
    }
    
TInt CPolicyNormalizer::ReplacePolicyReferenceL(CSenElement* aReferenceElement, CSenElement* aReferredPolicy)
    {
    //get the parent of policy, remove referenceElement and add referred policy there 
    
    CSenElement* parent = aReferenceElement->Parent();
    
    if(!parent)
        return KErrNotFound;
    //add all policy child elements here and then copy those element's contents 
    //from referred policy,
    //the referred policy element is owned by SD so do not use it
    
     RPointerArray<CSenElement>& children = aReferredPolicy->ElementsL();
        TInt childCount = children.Count();
        
        if(childCount > 0)
        {
            CSenElement* pNextChild;   
            TInt i = 0;
            
            while (i < childCount)
            {   
                
                pNextChild = children[i];
                TPtrC8 localName = pNextChild->LocalName();
                TPtrC8 nsUri = pNextChild->NamespaceURI();
                CSenElement& newElement = parent->AddElementL(nsUri, localName);
                newElement.CopyFromL(*pNextChild);                
                i++;
            }
        }

//now we have resolved the reference so we dont need the PolicyReference element    
    CSenElement* removed = parent->RemoveElement(*aReferenceElement);
    
    if(removed)
        {
        delete removed;
        return KErrNone;
        }
    return KErrNotFound;
    }

CSenElement* CPolicyNormalizer::ProcessPolicyReferenceL(CSenElement* aPolicy, CPolicyRegistry* aRegistry)
{    //Loop for wsp:PolicyReference element and if found then replace
    // it with wsp:All and copy everything from found policy to here
    if(aRegistry == NULL)
        return NULL;
    //Check if there is a reference if yes then resolve it
    RPointerArray<CSenElement> referenceChildren;
    
    if(aPolicy->ElementsL(referenceChildren, KWsPolicyReference) == KErrNone)
        {
        TInt childCount = referenceChildren.Count();
        TInt i = 0;
        CSenElement* pNextChild;

        while (i < childCount)
            {
            pNextChild = referenceChildren[i];
            TPtrC8 localName = pNextChild->LocalName();
            TPtrC8 uri = GetReferenceUriL(pNextChild);
            _LIT8(KHash, "#");
            
            HBufC8* aRippedUri = NULL;
             
            if(uri.Left(1).Compare(KHash) == 0)
               aRippedUri = uri.Right(uri.Length()-1).AllocL();
            
            if(aRippedUri->Length() > 0)
                {
                CSenElement* referedPolicy = aRegistry->LookupPolicy(aRippedUri->Des());
                if(referedPolicy)
                    {
                    ReplacePolicyReferenceL(pNextChild,referedPolicy);
                    }
                }
                delete aRippedUri;
                
            i++;
            }
        }
        
    //Check in all children recursively PolicyReferences and resolve them
    RPointerArray<CSenElement>& children = aPolicy->ElementsL();
    TInt childCount = children.Count();    

    CSenElement* pNextChild;
    TInt i=0;

    while (i < childCount)
    {
        pNextChild = children[i];
        TPtrC8 localName = pNextChild->LocalName();
        ProcessPolicyReferenceL(pNextChild, aRegistry);
        i++;
    }
    
  return NULL;  
}

TBool CPolicyNormalizer::IsOptionalL(CSenElement* aAssertion)
{
    // 7, 21, 23, 24
    RPointerArray<CSenBaseAttribute>& attrs = aAssertion->AttributesL();
    
    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
         bs = (attrs)[j];
         if(bs->Name().Compare(KWspOptinal) == 0)
         {
            if(bs->Value() == KWspOptinalTrue)
                return ETrue;
         }
            
              
         }
    return EFalse;
}

CSenElement* CPolicyNormalizer::ProcessOptionalElement(CSenElement* /*aAssertion*/, CSenElement* aNormalAssertion)
    {
    return aNormalAssertion;
    }

CSenElement* CPolicyNormalizer::ProcessAssertionTermL(CSenElement* aAssertion, CSenElement* aNormalAssertion)
{
    TPtrC8 assertionName = aAssertion->LocalName();
    TPtrC8 assertionNsUri = aAssertion->NamespaceURI();
    TPtrC8 nsPrefix = aAssertion->NsPrefix();
    TBuf8<255> qname(nsPrefix);
    qname.Append(':');
    qname.Append(assertionName);
    CSenElement& newNormalForm = aNormalAssertion->AddElementL(assertionNsUri,assertionName, qname);
  
    if(IsOptionalL(aAssertion))
    {
        if (CopySenElementWithoutOptionL(aAssertion, &newNormalForm))
        {
            CSenElement* parent = aNormalAssertion->Parent();
            AddAndElementL(parent);
        }
    }
    else
    {
        CopySenElementL(aAssertion, &newNormalForm);    
    }
    
    
    

    RPointerArray<CSenElement>& children = aAssertion->ElementsL();
    TInt childCount = children.Count();  
    
    if (childCount > 0)
    {
        CSenElement* pNextChild;
        TInt i =0;
        while (i < childCount)
        {
        
            pNextChild = children[i]; 
            TPtrC8 localName = pNextChild->LocalName();
            
            
            if(localName.Compare(KXorCompositeAssertion) == 0)
            {

            }
            else if (localName.Compare(KAndCompositeAssertion) == 0)
            {

            }
            else if (localName.Compare(KWsPolicy) == 0 )
            {
                CSenElement* aNewElement = AddPolicyElementL(&newNormalForm);
                aNewElement = ProcessPolicyTermL(pNextChild,aNewElement);

            }
            else
            {//if asserion is not well defined and dont have  apolicy object here 
               //then it should add one and then add XOR and AND
//                 CSenElement* aNewElement = AddPolicyElementL(&newNormalForm);
//                 CSenElement* pChildXor = AddXorElementL(aNewElement);
//                 CSenElement* pChildAnd = AddAndElementL(pChildXor);
                 aNormalAssertion = ProcessAssertionTermL(pNextChild, &newNormalForm);
            }
            
            i++;    
        }
    }
    
  	return aNormalAssertion;
}

CSenElement* CPolicyNormalizer::ProcessPolicyTermL(CSenElement* aAssertion, CSenElement* aNormalAssertion)
{
     
    RPointerArray<CSenElement>& children = aAssertion->ElementsL();
    TInt childCount = children.Count();    
    CSenElement* pNextChild;
    
    if(childCount == 0)
    {
      CSenElement* pXor = AddXorElementL(aNormalAssertion);
      CSenElement* pAnd = AddAndElementL(pXor);
      return aNormalAssertion;
    }
    TInt i=0;
    
    CSenElement* assertionParent = aNormalAssertion;
   

    while (i < childCount)
    {
        pNextChild = children[i];
         
        
        TPtrC8 localName = pNextChild->LocalName();

        if(localName.Compare(WSPolicy::KXorCompositeAssertion) == 0)
        {
            TPtrC8 nt = aNormalAssertion->LocalName();

            if(nt.Compare(WSPolicy::KWsPolicy)== 0) //PERFECT we need it
            {
            //        aNormalAssertion = 
                ProcessXORTermL(pNextChild,aNormalAssertion);
            }

            else if(nt.Compare(WSPolicy::KXorCompositeAssertion)== 0)
            {
               CSenElement* NewXorParent =  aNormalAssertion->Parent();
               ProcessXORTermL(pNextChild,NewXorParent); 
            }
            else if(nt.Compare(WSPolicy::KAndCompositeAssertion)== 0)
            {
               CSenElement* NewXorParent =  aNormalAssertion->Parent()->Parent();
               ProcessXORTermL(pNextChild,NewXorParent);
            }
            else
            {
                //SHOULDNT BE  A CASE CAZ then we have XOR in some assertion
                //It should always be in a POLICY tag
            }
        }
        
    
        else if (localName.Compare(WSPolicy::KAndCompositeAssertion) == 0)
        {
            CSenElement* pNormChild = AddXorElementL(aNormalAssertion);
            
    //        aNormalAssertion = 
            ProcessANDTermL(pNextChild, pNormChild);
        }
            
        
        else if (localName.Compare(WSPolicy::KWsPolicy) == 0 )
        {
            TPtrC8 parentName = aNormalAssertion->LocalName();
            if(parentName.Compare(WSPolicy::KXorCompositeAssertion) == 0)
            {
                CSenElement* pChildXor = AddXorElementL(aNormalAssertion);
                CSenElement* pChildAnd = AddAndElementL(pChildXor);
                
            }
            else if(parentName.Compare(WSPolicy::KAndCompositeAssertion) == 0)
            {
            }
            else if(parentName.Compare(WSPolicy::KWsPolicy) == 0)
            {
                CSenElement* pChildXor = AddXorElementL(aNormalAssertion);
                CSenElement* pChildAnd = AddAndElementL(pChildXor);
               
            }
            else
            {
                CSenElement* pChildXor = AddXorElementL(aNormalAssertion);
                CSenElement* pChildAnd = AddAndElementL(pChildXor);                
            }
        }
        else    //its an Assertion
        {
            TPtrC8 parentName = assertionParent->LocalName();
            //if parent is Policy -> add XOR and AND and then process 
            //assertion- >new parent for same level is AND
            
            //if parent is XOR -> Add AND and then process assertion. at this level the parent will be AND
            //if parent is AND just process assertion
            if(IsOptionalL(pNextChild))
            {
                if(parentName.Compare(WSPolicy::KWsPolicy) == 0)
                {
                    CSenElement* pXor = AddXorElementL(assertionParent);
                    CSenElement* pAnd = AddAndElementL(pXor);
    //                aNormalAssertion = 
                    ProcessAssertionTermL(pNextChild, pAnd);
                }
                else if(parentName.Compare(WSPolicy::KAndCompositeAssertion) == 0)
                {
                    CSenElement* parentNow = assertionParent;
                    assertionParent = assertionParent->Parent();
                    CSenElement* policyParent= assertionParent->Parent();
                    CSenElement* pXor = AddXorElementL(policyParent);
                    CSenElement* pAnd = AddAndElementL(pXor);
    //              aNormalAssertion = 
                    ProcessAssertionTermL(pNextChild, pAnd);   
                    assertionParent = parentNow;  

                }   
                else if(parentName.Compare(WSPolicy::KXorCompositeAssertion) == 0)
                {
                    
                }
                else
                {
                    //probably error   
                }
                
 
            }
            else
            {
                if(parentName.Compare(WSPolicy::KWsPolicy) == 0)
                {
                    CSenElement* pXor = AddXorElementL(assertionParent);
                    CSenElement* pAnd = AddAndElementL(pXor);
                    assertionParent = pAnd;
    //                assertionParent =  ProcessAssertionTermL(pNextChild, pAnd);
                    
                }
                else if(parentName.Compare(WSPolicy::KXorCompositeAssertion) == 0)
                {
    
                    CSenElement* pXor = AddXorElementL(assertionParent);
                    CSenElement* pAnd = AddAndElementL(pXor);
    //            aNormalAssertion = ProcessAssertionTermL(pNextChild, pAnd);

                    
                }
                else if(parentName.Compare(WSPolicy::KAndCompositeAssertion) == 0)
                {
    
    //            aNormalAssertion = ProcessAssertionTermL(pNextChild, assertionParent);
                    
                }
                else 
                {
                    CSenElement* pXor = AddXorElementL(assertionParent);
                    CSenElement* pAnd = AddAndElementL(pXor);
                    assertionParent = pAnd;
      //            aNormalAssertion =  ProcessAssertionTermL(pNextChild, pAnd);
                }
                
                ProcessAssertionTermL(pNextChild, assertionParent);
            
            }
        }
        
    

     i++;   
    }

    return assertionParent;

}
CSenElement* CPolicyNormalizer::ProcessXORTermL(CSenElement* aAssertion, CSenElement* aNormalAssertion)
{
    
    CSenElement* pNormChild = AddXorElementL(aNormalAssertion);
    
    RPointerArray<CSenElement>& children = aAssertion->ElementsL();
    TInt childCount = children.Count();
    TInt i = 0;
    
     while (i < childCount)
    {
        CSenElement* pChild = children[i];
        
        CSenElement* pNewChild = NULL;
//        CSenElement* newChild = NULL;
        
        TPtrC8 childName = pChild->LocalName();
        
        if(childName.Compare(WSPolicy::KAndCompositeAssertion) == 0)
        {
            //If AND element then take child of AND
//            CSenElement* newChild = 
            ProcessANDTermL(pChild, pNormChild);
            
        }
        else if (childName.Compare(WSPolicy::KXorCompositeAssertion) == 0)
        {

        }
        else if(childName.Compare(WSPolicy::KWsPolicy) == 0)
        {
            pNewChild = AddAndElementL(pNormChild);
            if(HasChildL(pChild) > 0)
                ProcessPolicyTermL(pChild,pNewChild);
        }
        else
        {
            pNewChild = AddAndElementL(pNormChild);
//            newChild = 
            ProcessAssertionTermL(pChild, pNewChild);
            //if Assertion then first we haveto add AND and then add Assertion
            //(assertion will be done by AND too)
        }
          
          i++;  
    }
    
    		
	return pNormChild;	
	
}
CSenElement* CPolicyNormalizer::ProcessANDTermL(CSenElement* aAssertion, CSenElement* aNormalAssertion)
{
    CSenElement* pNormChild = AddAndElementL(aNormalAssertion);
    
    RPointerArray<CSenElement>& children = aAssertion->ElementsL();
    TInt childCount = children.Count();
//    CSenElement* normalizedFinalChild = NULL;
    TInt i=0;
    while (i < childCount)
    {
        CSenElement* pChild = children[i];
//        CSenElement* newChild = NULL;
        
        TPtrC8 childName = pChild->LocalName();
     
    if(childName.Compare(WSPolicy::KAndCompositeAssertion) == 0)
        {
        
        }
    else if (childName.Compare(WSPolicy::KXorCompositeAssertion) == 0)
        {
            //will be solved during collapsing
        
        }
    else if (childName.Compare(WSPolicy::KWsPolicy) == 0)
        {
            ProcessPolicyTermL(pChild,pNormChild);
            
        }
    else
        {
//              normalizedFinalChild = 
              ProcessAssertionTermL(pChild, pNormChild);
        }
 
        i++; 
    }
       
	return aNormalAssertion;
}





CSenElement* CPolicyNormalizer::ProcessLogicL(CSenElement* aTerm)
{
    
     //if sibling names are same as <ExactlyOne>
    RPointerArray<CSenElement> matchingSiblings;
    TInt matchingSiblingCount = 0;
    if(aTerm->ElementsL(matchingSiblings, WSPolicy::KXorCompositeAssertion) == KErrNone)
        matchingSiblingCount= matchingSiblings.Count();
    if(matchingSiblingCount>1)
        {  //XOR elements should have AND elements in them else normalization error
        //Find elements
        CSenElement* firstSibling = matchingSiblings[0];
        
        TInt i = 1;
            while ( i < matchingSiblingCount)
            {
                CSenElement* nextSibling = matchingSiblings[i];
                firstSibling = ProcessAndAndLogicL(firstSibling, nextSibling);

                i++;
            }
            TInt j = 0;
            while ( j < matchingSiblingCount)
            {
                CSenElement* removed = aTerm->RemoveElement(*matchingSiblings[j]);
                if(removed)
                    delete removed;
                j++;                
            }
                        
/****        aTerm->AddElementL(*firstSibling); */
         }
    
    RPointerArray<CSenElement>& children = aTerm->ElementsL();
    TInt childCount = children.Count();
    
    TInt i=0;
        

    while (i < childCount)
    {
        CSenElement* pChild = children[i];
        
        if(childCount == 1)
            ProcessLogicL(pChild);
        
        TPtrC8 childName = pChild->LocalName();
        TPtrC8 parentName = aTerm->LocalName();
    
        
        //if parent name and child name are same
        if(childName.Compare(parentName) == 0)
        {
                if(childName.Compare(WSPolicy::KWsPolicy) == 0)
                {
                    
                }
                else if(childName.Compare(WSPolicy::KAndCompositeAssertion) == 0)
                {
                    
                }
                else if(childName.Compare(WSPolicy::KXorCompositeAssertion) == 0)
                {
                  ProcessXorCollapseLogicL(aTerm,pChild);
                  continue;  
                }   
          
        }
        

        i++;
    }
    
    matchingSiblings.Close();
    return aTerm;
}

CSenElement* CPolicyNormalizer::ProcessXorCollapseLogicL(CSenElement* aParent, CSenElement* aChild)
{
        RPointerArray<CSenElement>& siblings = aParent->ElementsL();
        RPointerArray<CSenElement>& collapsingChildren = aChild->ElementsL();
        
        TInt childCount = collapsingChildren.Count();
        TInt i=0;
     
        RPointerArray<CSenElement> matchingSiblings;
        TInt matchingSiblingCount = 0;
        if(aParent->ElementsL(matchingSiblings, WSPolicy::KAndCompositeAssertion) == KErrNone)
            matchingSiblingCount= matchingSiblings.Count();
                
     
        while (i < childCount)
        {
            CSenElement* child = collapsingChildren[i];
           
         
              for (TInt j = 0; j<matchingSiblingCount; j++)                    
                {
                
                    RPointerArray<CSenElement>& siblingsChildren = matchingSiblings[j]->ElementsL();
                    TInt matchingSiblingChildrenCount= siblingsChildren.Count();
                    
                    for (TInt k = 0; k<matchingSiblingChildrenCount; k++)
                    {
                        CSenElement* element = siblingsChildren[k];
                        child->AddElementL(*element);
 //                       TPtrC8 name = detachedSibling->LocalName();
                        
                    }
                    
                    
                }
                child->SetParent(aParent);
                aParent->AddElementL(*child);

      
        i++;
        }
        
         CSenElement* thisOne = aParent->RemoveElement(*aChild);
         if(thisOne)
            delete thisOne;
        
        for (TInt j = 0; j<matchingSiblingCount; j++)  
        {
            CSenElement* thisOneagain = aParent->RemoveElement(*matchingSiblings[j]);    
            if(thisOneagain)
                delete thisOneagain;
            
        }
        
        
        return aParent;
    

}
CSenElement* CPolicyNormalizer::ProcessAssertionBranchingL(CSenElement* aPolicy)
{
  
  RPointerArray<CSenElement>& children = aPolicy->ElementsL();
  TInt childCount = children.Count();
  
  if(childCount == 0)
    return aPolicy;
  
  TInt i = 0;
  while (i < childCount)
    {
    CSenElement* pChild = children[i];
    TPtrC8 childName = pChild->LocalName();
    
    if( childName.Compare(WSPolicy::KWsPolicy) == 0 
        || childName.Compare(WSPolicy::KAndCompositeAssertion) == 0
        || childName.Compare(WSPolicy::KXorCompositeAssertion) == 0)
        {
        ProcessAssertionBranchingL(pChild);
        }   
    else //Assertion
        {
        DuplicateAssertionBranchL(aPolicy , pChild);
        }
    i++;
    }
  return aPolicy;
  }

CSenElement* CPolicyNormalizer::DuplicateAssertionBranchL(CSenElement* /*aParent*/, CSenElement* aDuplicatingPolicy)
{  //aDuplicatingPolicy is always an assertion 
//check if aDuplicatingPolicy have two assertions, if yes then divide the assertion in 
  // T W O separate assertions
  //first check that if any of the two assertion themselve have 
  
  // if XOR have multiple ANDs then check if XOR parent is an Assertion. 
  // if yes then divide them in Two assertions.
  TPtrC8 asseryName = aDuplicatingPolicy->LocalName();
  TInt childCount = HasChildL(aDuplicatingPolicy);
  
  if( childCount== 0) //There is NO CHILD NESTING in assertion
        return aDuplicatingPolicy;  
  else if (childCount == 1) ////Assertion should always have only one <wsp:policy> element. 
  {
    TBool deleted = EFalse;
        CSenElement* assertionPolicyElement = aDuplicatingPolicy->Child(0); //policy element
        TPtrC8 policyName = assertionPolicyElement->LocalName();
        if(policyName.Compare(WSPolicy::KWsPolicy)== 0)
        { //now we should have XOR and AND elements
        
        CSenElement* assertionXORElement = assertionPolicyElement->Child(0); //XOR element
        //there should be only one XOR element (in normalize mode)
        TPtrC8 name1 = assertionXORElement->LocalName();
        TInt AndElementCount = HasChildL(assertionXORElement);
        //If it has only one AND child element then nuthing to do here but if more then one element
        // then it means that assertion have nesting and should have a different branch.
        if(AndElementCount == 0)
            return aDuplicatingPolicy;
        else if(AndElementCount == 1)
        {
             ProcessAssertionBranchingL(assertionXORElement->Child(0));   
        }
        else if(AndElementCount > 1)
        {
             RPointerArray<CSenElement>& assertionXorChildren = assertionXORElement->ElementsL(); //AND elements
             TInt i = 0;
             while (i < AndElementCount)
             {
                //copy the root assertion to new tag
                // Take out the replacing tag
                // create two separate tag and replace them in the original and root copy tag
                
                CSenXmlElement* AND = (CSenXmlElement*)assertionXorChildren[i];
//                ProcessAssertionBranchingL(AND);
                
                CSenXmlElement* rootAssertion = (CSenXmlElement*)FindRootAssertionL(AND);
                if (!IsChildOfElement(rootAssertion, AND))
                {
                    i++;
                    continue;
                }
                
//create a duplicate tag,
                //duplicating assertion                
                TPtrC8 assertionCopyName = rootAssertion->LocalName();
                TPtrC8 assertionCopyNsUri = rootAssertion->NamespaceURI();
                CSenXmlElement* XORParent = (CSenXmlElement*)rootAssertion->Parent();
                CSenElement* parentsBrandNewRootAssertion =AddAndElementL(XORParent->Parent());
                
                CSenElement& newDuplicateAssertion = parentsBrandNewRootAssertion->AddElementL(assertionCopyNsUri,assertionCopyName);

//cut and paste the second AND to duplicate
                
                newDuplicateAssertion.CopyFromL(*rootAssertion);
                
                DeleteAsertionIndexL(aDuplicatingPolicy, &newDuplicateAssertion, i);
                deleted = ETrue;
//                newDuplicateAssertion.SetParent(parentsBrandNewRootAssertion);
                //parentsBrandNewRootAssertion->ReplaceElementL(newDuplicateAssertion);
                
                //find from this new element the required assertion and then delete
                // all of the optins from it except the current i+1 ALL element
             
                i++;
             }

            if(deleted)                    
            {
             CSenElement* rootAssertion = FindRootAssertionL(aDuplicatingPolicy);
             CSenElement* AND = rootAssertion->Parent();
             CSenElement* XOR = AND->Parent();
             CSenElement* thisOne = XOR->RemoveElement(*AND);
             if(thisOne)             
                 iOrphanElements.Append(thisOne);
//                delete thisOne; //P DuplicateAssertionBranch A T H E T I C   H A C K 
             //now we have solved the first level nesting
             //our original policy is changed so now use the original policy and solve it again
             CSenElement* documentRoot = (CSenElement*)&XOR->Root(); 
             ProcessAssertionBranchingL(documentRoot);
 
        }
        
        
        }
        }
  }
  else if(childCount > 1)
  {
  //this count is always one because its a child wsp:policy element, policy element 
  //could have multiple XOR elements . If it comes here something is W R O N G
    
  }
    //here is the nesting problem. the assertion policy should have only one 
   // Alternative = <XOR><AND>< --assertion-- ></AND></XOR> 
   // NO TWO XORs are allowed here
   return aDuplicatingPolicy;
}
CSenElement* CPolicyNormalizer::DeleteAsertionIndexL(CSenElement* aAssertion, CSenElement* anewAssertion, TInt aIndx)
{
    TPtrC8 name = aAssertion->LocalName();
    TPtrC8 nsUri = aAssertion->NamespaceURI();
    TInt i = 0;

    RPointerArray<CSenElement>& assertionChildren = anewAssertion->ElementsL();
    TInt newAssertionChildCount = assertionChildren.Count();
    
    if(newAssertionChildCount == 0)
        return anewAssertion;
    
    TPtrC8 namee = anewAssertion->LocalName();
    if (name.Compare(anewAssertion->LocalName())== 0)
    { //delete children from this assertion except index child
        CSenXmlElement* PolicyElement = (CSenXmlElement*)anewAssertion->Child(0);
        CSenXmlElement* XORElement = (CSenXmlElement*)PolicyElement->Child(0);
        RPointerArray<CSenElement>& XorChildren = XORElement->ElementsL();
        TInt XORChildrenCount = XorChildren.Count();
         
        for (TInt k = 0; k <XORChildrenCount; k++)
        {
            if(k == aIndx)
            {
                
            }
            else
            {
                CSenElement* newElement = XORElement->RemoveElement(*XorChildren[XORChildrenCount-1-k]);
                if(newElement)
                {
                    delete newElement;               
                //    break;    
                }
                
            }
                
        }
        return anewAssertion;
    }

    while (i < newAssertionChildCount)
    {
    
        DeleteAsertionIndexL(aAssertion, assertionChildren[i], aIndx);
        i++;
    }
    
    return anewAssertion;
}
CSenElement* CPolicyNormalizer::FindRootAssertionL(CSenElement* aElement)
{

    CSenElement* rootAssertionTag = NULL;
    
    CSenElement* documentRoot = (CSenElement*)&aElement->Root();
    CSenElement* doumentXOR = documentRoot->Child(0); //cant be more then one :p
    
    //find first assertion here  
    RPointerArray<CSenElement>& children = doumentXOR->ElementsL(); //AND elements
    TInt childCount = children.Count();
    
    if(childCount == 0) //something invalid with document
        return aElement;
  
    else if (childCount > 0) //onyl one AND element, Next child should be root Assertion
    {
        TInt i = 0;
        while (i < childCount)
        {            
            CSenElement* andTag = children[i];
        TInt andAssertions = HasChildL(andTag);
        
        if(andAssertions == 0)
            return aElement;
        else if(andAssertions == 1)
            {
                TBool isChild = IsChildOfElement(andTag->Child(0), aElement);
                if(isChild)
                return andTag->Child(0);
            }
        else if (andAssertions > 1)
        {
            RPointerArray<CSenElement>& rootAssertions = andTag->ElementsL(); //AND elements
            TInt j = 0;
            while (j < andAssertions)
            {
                
                TBool isChild = IsChildOfElement(rootAssertions[j], aElement);
                
                if(isChild)
                {
                     rootAssertionTag = rootAssertions[j];
                     break;
                }
                
                j++;
                }
            }
        i++;
        }
        
        
    }
    else if (childCount > 1)
    {
    
        
    }
    
    return rootAssertionTag;

}
TBool CPolicyNormalizer::IsChildOfElement(CSenElement* aParent, CSenElement* aChild)
{ //aParent and child botth are assertions
    
    CSenElement* rootChildAssertionTag = aChild; //->Parent();
    TPtrC8 nqame1 = aParent->LocalName();
    while (true)
    {
        
        if(rootChildAssertionTag)
        {
            
            TPtrC8 nqame2 = rootChildAssertionTag->LocalName();
            
            if(rootChildAssertionTag == aParent)
            {
                return ETrue;
        
            }
        }
        else
        {
            return EFalse;    
        }   
        rootChildAssertionTag = rootChildAssertionTag->Parent();
    }
}
CSenElement* CPolicyNormalizer::ProcessAndCollapseLogic(CSenElement* aParent, CSenElement* /*aChild*/)
{
 return  aParent;  
}

CSenElement* CPolicyNormalizer::ProcessAndAndLogicL(CSenElement* aFirstTerm, CSenElement* aSecondTerm )
{
    //Two XOR XOR parameters are given which have same parent and have AND child elements,
    //The AND child elements will be resolved according to Ditributive law 
    //takes all child Assertion elements from SecondTerm and put them in first term
    

    RPointerArray<CSenElement> matchingSiblingsFirst;
    CleanupClosePushL(matchingSiblingsFirst);
    
    if(aFirstTerm->ElementsL(matchingSiblingsFirst, WSPolicy::KAndCompositeAssertion) == KErrNone
       || aFirstTerm->ElementsL(matchingSiblingsFirst, WSPolicy::KXorCompositeAssertion) == KErrNone
       || aFirstTerm->ElementsL(matchingSiblingsFirst, WSPolicy::KWsPolicy) == KErrNone
       || aSecondTerm->ElementsL(matchingSiblingsFirst, WSPolicy::KAndCompositeAssertion) == KErrNone
       || aSecondTerm->ElementsL(matchingSiblingsFirst, WSPolicy::KXorCompositeAssertion) == KErrNone
       || aSecondTerm->ElementsL(matchingSiblingsFirst, WSPolicy::KWsPolicy) == KErrNone)
       
       
       
    { // they are NOT the last children so first go deep to last assertion and then process that assertion
            ProcessLogicL(aFirstTerm);
            ProcessLogicL(aSecondTerm);
    }
    CleanupStack::PopAndDestroy();//matchingSiblingsFirst
    
//    else // they are the last children
   
     
        RPointerArray<CSenElement>& siblings = aFirstTerm->ElementsL();
        RPointerArray<CSenElement>& collapsingChildren = aSecondTerm->ElementsL();
         
        
        TInt siblingsCount = siblings.Count();
        TInt childCount = collapsingChildren.Count();
        
        if(siblingsCount == 0)
            return aFirstTerm;
        else if (childCount == 0)
            return aSecondTerm;
        
        TInt i=0;
        TInt j=0;
        
        CSenElement* collpasingElement = NULL;
        CSenElement* returningElement = NULL;
        if(siblingsCount <  childCount)
        {
            CSenElement& distributiveElement = aFirstTerm->Parent()->AddElementL(WSPolicy::KWsPolicyNsUri,WSPolicy::KXorCompositeAssertion);;
//            distributiveElement = CSenXmlElement::NewL(WSPolicy::KWsPolicyNsUri,WSPolicy::KXorCompositeAssertion);   
//            aFirstTerm->Parent()->AddElementL(*distributiveElement);
//            distributiveElement->SetParent(aFirstTerm->Parent());
            
            while(j < childCount)
            { //chnage logic for <ALL/>
/*

<XOR>
    <AND>
        <!--assertion1-->
    </AND>
    <AND/>
</XOR>
<XOR>
    <AND>
        <!--assertion3-->
    </AND>
    <AND>
        <!--assertion4-->
    </AND>

</XOR>

1.make assertion1 assertion 3
2. make assertion 2 assertion 3
3. Replace <AND/> with all the assertions from second <XOR> i.e. 
with 
    <AND>
        <!--assertion3-->
    </AND>
    <AND>
        <!--assertion4-->
    </AND>
*/                
                CSenElement* newParentElement = collapsingChildren[j];
                
                TBool EmptyAndElement = EFalse;
                if (HasChildL(newParentElement) < 1)
                {
                    EmptyAndElement = ETrue;
                }
                
                while(i < siblingsCount)
                {
                    collpasingElement = siblings[i];

                    RPointerArray<CSenElement>& collpasEleAssertion = collpasingElement->ElementsL();
                    TInt countt = collpasEleAssertion.Count();
                    
                    if(EmptyAndElement) //copy all elements from here to new parent
                    {

                           CSenElement& ele = distributiveElement.AddElementL(collpasingElement->NamespaceURI(), collpasingElement->LocalName());
                           ele.CopyFromL(*collpasingElement);
                    }
//                    else if(collpasingElement->Child(0))
//                        newParentElement->AddElementL(*collpasingElement->Child(0));
                    else
                    { //This is an empty <ALL /> element so use distributive law to resolve it
                       
                       CSenElement* AndElement = AddAndElementL(&distributiveElement);
                        //copy here all elements from first AND element and 2nd AND element
                        
                        RPointerArray<CSenElement>& ParentChildren = newParentElement->ElementsL();
                        TInt countHere = ParentChildren.Count();
                          for (TInt k = 0; k < countHere; k++)
                            {
                                CSenElement* pkChild = ParentChildren[k];
                                CSenElement& here = AndElement->AddElementL(pkChild->NamespaceURI(), pkChild->LocalName());
                                here.CopyFromL(*pkChild);
                            }
                          for(TInt l = 0; l < countt; l++)
                            {
                                CSenElement* paChild = collpasEleAssertion[l];
                                CSenElement& hereAgain = AndElement->AddElementL(paChild->NamespaceURI(), paChild->LocalName());
                                hereAgain.CopyFromL(*paChild);
                                
                            }
                          
                    }
                    i++;
                }
                i = 0;
                j++;
            }
            returningElement = &distributiveElement;
            CSenElement* parentElement = aFirstTerm->Parent();
//            parentElement->RemoveElement(*aFirstTerm);
            
        }
        else
        {
            CSenElement& distributiveElement = aFirstTerm->Parent()->AddElementL(WSPolicy::KWsPolicyNsUri,WSPolicy::KXorCompositeAssertion);;
//            distributiveElement = CSenXmlElement::NewL(WSPolicy::KWsPolicyNsUri,WSPolicy::KXorCompositeAssertion);   
//            aFirstTerm->Parent()->AddElementL(*distributiveElement);
//            distributiveElement->SetParent(aFirstTerm->Parent());
            while(j < siblingsCount)
            {
                CSenElement* newParentElement = siblings[j]; //AND element

                TBool EmptyAndElement = EFalse;
                if (HasChildL(newParentElement) < 1)
                {
                    EmptyAndElement = ETrue;
                }

        
                while(i < childCount)
                {
                    collpasingElement = collapsingChildren[i]; //AND element

                    RPointerArray<CSenElement>& collpasEleAssertion = collpasingElement->ElementsL(); // AND element Assertions
                    TInt countt = collpasEleAssertion.Count();
                    
                    if(EmptyAndElement) //copy all elements from here to new parent
                    {
                           CSenElement& ele = distributiveElement.AddElementL(collpasingElement->NamespaceURI(), collpasingElement->LocalName());
                           ele.CopyFromL(*collpasingElement);
                    }   
//                    else if(countt == 1)    //If AND has only one assertion
//                        newParentElement->AddElementL(*collpasingElement->Child(0));
                    else //if(countt > 1)
                    { //Distributive Law
                    
                        CSenElement* AndElement = AddAndElementL(&distributiveElement);
                        //copy here all elements from first AND element and 2nd AND element
                        
                        RPointerArray<CSenElement>& ParentChildren = newParentElement->ElementsL();
                        TInt countHere = ParentChildren.Count();
                          for (TInt k = 0; k < countHere; k++)
                            {
                                CSenElement* pkChild = ParentChildren[k];
                                CSenElement& here = AndElement->AddElementL(pkChild->NamespaceURI(), pkChild->LocalName());
                                here.CopyFromL(*pkChild);
                            }
                          for(TInt l = 0; l < countt; l++)
                            {
                                CSenElement* paChild = collpasEleAssertion[l];
                                CSenElement& hereAgain = AndElement->AddElementL(paChild->NamespaceURI(), paChild->LocalName());
                                hereAgain.CopyFromL(*paChild);
                            }
                         
                    }
                    i++;
                }
                i = 0;
                j++;
        }
            returningElement = &distributiveElement; //; aFirstTerm;
            CSenElement* parentElement = aSecondTerm->Parent();
        }

    return  returningElement;
    
}

TInt CPolicyNormalizer::HasChildL(CSenElement* aTerm)
{
    RPointerArray<CSenElement>& children = aTerm->ElementsL();
    
    TInt childCount = children.Count();
    return childCount;
    
}

CSenElement* CPolicyNormalizer::AddAndElementL(CSenElement* aAssertion)
{
   
    CSenElement& newElement = aAssertion->AddElementL(WSPolicy::KWsPolicyNsUri,
                                                 WSPolicy::KAndCompositeAssertion);
    return &newElement;
                                                                                                  
}

CSenElement* CPolicyNormalizer::AddXorElementL(CSenElement* aAssertion)
{
     CSenElement& newElement = aAssertion->AddElementL(WSPolicy::KWsPolicyNsUri,
                                                 WSPolicy::KXorCompositeAssertion);
       return &newElement;
                                                                                                  
}

CSenElement* CPolicyNormalizer::AddPolicyElementL(CSenElement* aAssertion)
{
     CSenElement& newElement = aAssertion->AddElementL(WSPolicy::KWsPolicyNsUri,
                                                 WSPolicy::KWsPolicy);
       return &newElement;                                                                                                
}



TBool CPolicyNormalizer::ValidateElementNsL(CSenElement* aAssertion)
{
  
  CSenElement& element = AsElement();
  RPointerArray<CSenNamespace>& namespaces = element.NamespacesL();
  
  if(aAssertion->NamespaceURI().Length() < 1 )
  {
  
   TPtrC8 elementName = aAssertion->LocalName();
   TInt location = elementName.Find(KColon);
   
   if (location)
       {
        TPtrC8 nsPrefix = elementName.Left(location);
          
        CSenNamespace* ns = NULL;
        TInt count = namespaces.Count();
        for (TInt i=0; i < count; i++)
            {
            ns = (namespaces)[i];
            if(ns->Prefix().Compare(nsPrefix) == 0)
                {
                aAssertion->SetNamespaceL(ns->Prefix(),ns->URI());
                return ETrue;
                }
            }
        }
    }
    return EFalse;
}


TBool CPolicyNormalizer::CopySenElementWithoutOptionL(CSenElement* aSource,CSenElement* aDestination)
{
  
  TPtrC8 sourceContent = aSource->Content();
  TBool isTrue = EFalse;
  if (sourceContent.Length() > 0)
      {
        aDestination->SetContentL(sourceContent);
        
      }

    RPointerArray<CSenBaseAttribute> sourceAttributes = aSource->AttributesL();
    if (sourceAttributes.Count() > 0)
        {
        for (TInt i=0;i<sourceAttributes.Count(); i++)
            {
            CSenBaseAttribute* pBaseAttribute = sourceAttributes[i];
            if(pBaseAttribute->Name().Compare(WSPolicy::KWspOptinal) == 0)
            {
                if(pBaseAttribute->Value().Compare(WSPolicy::KWspOptinalTrue) == 0)
                    isTrue = ETrue;                    
            }
                
            else
                aDestination->AddAttributeL(pBaseAttribute->Name(),pBaseAttribute->Value());
            
            }
        }
//        sourceAttributes.Close();
        return isTrue;  
}

CSenElement* CPolicyNormalizer::CopySenElementL(CSenElement* aSource, CSenElement* aDestination)
{
  TPtrC8 sourceContent = aSource->Content();
  if (sourceContent.Length() > 0)
      {
        aDestination->SetContentL(sourceContent);
        
      }
    aDestination->SetNamespaceL(aSource->NsPrefix(), aSource->NamespaceURI());

    RPointerArray<CSenBaseAttribute> sourceAttributes = aSource->AttributesL();
    if (sourceAttributes.Count() > 0)
        {
        for (TInt i=0;i<sourceAttributes.Count(); i++)
            {
            CSenBaseAttribute* pBaseAttribute = sourceAttributes[i];

            aDestination->AddAttributeL(pBaseAttribute->Name(),
                                        pBaseAttribute->Value());
            
            }
        }
//        sourceAttributes.Close();
        return aDestination;
}

void CPolicyNormalizer::ReadFileL( TFileName aPath )
    {
    RFs fss;
    User::LeaveIfError(fss.Connect());
    CleanupClosePushL(fss);
     
    if(iPath)
        {
        delete iPath;
        iPath = NULL;
        }
        
    iPath = aPath.AllocL();
    
    TFileName iFileName(aPath);
    RFile  xmlFile;
    TInt err = xmlFile.Open(fss , iFileName, EFileRead );
    
    if(err == KErrNone)
        {
        TInt size;
        xmlFile.Size(size);

        HBufC8 *iBuf=HBufC8::NewL(size);

        TPtr8 buf8ptr(iBuf->Des());
        xmlFile.Read(buf8ptr,size);
        xmlFile.Close();

        TInt leaveCode(KErrNone);
        TRAP leaveCode, ParseL(buf8ptr) );
        leaveCode = 0; // not used currently
        delete iBuf;
        iBuf = NULL;
        }
    CleanupStack::PopAndDestroy(1); // fss
    }

TInt CPolicyNormalizer::ReadL(TDesC8& aXmlDesc)
    {
    TInt err (KErrNone);
    TInt size = aXmlDesc.Length();
    if(size > 0)
        {
        HBufC8 *pBuf= aXmlDesc.AllocL();
        CleanupStack::PushL(pBuf);
        TPtr8 buf8ptr(pBuf->Des());
        
        TRAP (err, ParseL(buf8ptr));
        
        CleanupStack::PopAndDestroy(pBuf);
        }
        return err;
    }

void CPolicyNormalizer::WriteAllL(CSenElement* /*aXml*/)
{
  RFs fss;
       User::LeaveIfError(fss.Connect());
        CleanupClosePushL(fss);
        
    TBuf<255> file;
    _LIT( KFileName, "c:\\logs\\normalizer%d.xml");
    file.Format(KFileName, count);
    count++;
    TPtrC OutputFile(file);
    
   TBuf<256> tempp(KOutPath);
//   tempp.Insert(tempp.Length() , iPath->Des());
   
//   TFileName iFileName(tempp);
   RFile  xmlFile;
   xmlFile.Replace(fss , OutputFile, EFileWrite );
   HBufC8* temp = iNewPolicy->AsXmlL();
   xmlFile.Write(temp->Des());
   xmlFile.Close();
   delete temp;
    
   CleanupStack::PopAndDestroy(1); // fss
  
}
void CPolicyNormalizer::WriteL(CSenElement* aXml)
{
    RFs fss;
        User::LeaveIfError(fss.Connect());
        CleanupClosePushL(fss);
        
    TBuf<255> file;
    _LIT( KFileName, "c:\\logs\\normalizer%d.xml");
    file.Format(KFileName, count);
    count++;
    TPtrC OutputFile(file);
    
   TBuf<256> tempp(KOutPath);
//   tempp.Insert(tempp.Length() , iPath->Des());
   
//   TFileName iFileName(tempp);
   RFile  xmlFile;
   xmlFile.Replace(fss , OutputFile, EFileWrite );
   HBufC8* temp = aXml->AsXmlL();
   xmlFile.Write(temp->Des());
   xmlFile.Close();
   delete temp;
    
   CleanupStack::PopAndDestroy(1); // fss
   
}

// End of file
