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




#include "PolicyMerger.h"

#include <XmlEngDdom.h>
#include "wssecuritypolicy.h"
#include "WSPolicy.h"

using namespace WSPolicy;

CPolicyMerger* CPolicyMerger::NewL()
{
    CPolicyMerger* pSelf =
        CPolicyMerger::NewLC();
    CleanupStack::Pop(pSelf);
    return pSelf;
}
CPolicyMerger* CPolicyMerger::NewLC()
{
    CPolicyMerger* pSelf = new (ELeave) CPolicyMerger();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;  
}
void CPolicyMerger::ConstructL()
{
   iSecurityPolicyDomain =    CWsSecurityPolicy::NewL();   
}
CPolicyMerger::~CPolicyMerger()
{
   iPolicy1.Close(); 
   iPolicy2.Close();
    
   delete iSecurityPolicyDomain;
   
}
CPolicyMerger::CPolicyMerger()
{
    
}

TXmlEngElement CPolicyMerger::MergeL(TDesC& /*aFirstPolicy*/, TDesC& /*aSecondPolicy*/)
{
/*
    TDOMImplementation domImpl = XmlEngine::DOM::DOMImplementationSource::DOMImplementationL (XmlEngine::DOM::KImplementationID);  
    RDOMParser parser = domImpl.CreateDOMParser();

    iPolicy1 = parser.ParseFileL( aFirstPolicy );
    iPolicy2 = parser.ParseFileL( aSecondPolicy );

    TXmlEngElement& pPolicy1 = iPolicy1.AsElement();
    TXmlEngElement& pPolicy2 = iPolicy2.AsElement();
    
    TXmlEngElement result = MergeL(pPolicy1,pPolicy2);

    return result;
      */
      return NULL;
}


TXmlEngElement CPolicyMerger::MergeL(TXmlEngElement aFirstPolicy, TXmlEngElement aSecondPolicy)
{
    RXmlEngDocument mergedPolicyDoc = aFirstPolicy.OwnerDocument().CloneDocumentL();
    TXmlEngElement& newIntersectedPolicy = mergedPolicyDoc.AsElement();

    RXmlEngNodeList<TXmlEngElement> listPolicynew;
    newIntersectedPolicy.GetChildElements(listPolicynew); //get the children of policy  

    if(listPolicynew.HasNext()) 
    {
        iMergedPolicyElement = listPolicynew.Next();
        iMergedPolicyElement.RemoveChildren();
    }

    RXmlEngNodeList<TXmlEngElement> listPolicyFirst;
    aFirstPolicy.GetChildElements(listPolicyFirst); //get the children of policy

    RXmlEngNodeList<TXmlEngElement> listPolicySecond;
    aSecondPolicy.GetChildElements(listPolicySecond); //get the children of policy
    
    TXmlEngElement XOR = iMergedPolicyElement.AddNewElementSameNsL(WSPolicy::KXorCompositeAssertion);
    
    
    if(listPolicyFirst.HasNext() && listPolicySecond.HasNext()) 
    {
        TXmlEngElement policyElement = listPolicyFirst.Next();
        TPtrC8 localName = policyElement.TagName();
                
        RXmlEngNodeList<TXmlEngElement> xorList; //get the only xor element
        policyElement.GetElementsByTagNameL(xorList, WSPolicy::KXorCompositeAssertion, WSPolicy::KWsPolicyNsUri);
        TXmlEngElement pXORFirst = NULL;
        
        if(xorList.HasNext())
            pXORFirst = xorList.Next();
        //get all AND elements from the above xor. 
        
        RXmlEngNodeList<TXmlEngElement> andListFirst; //get the only xor element from First Policy
        pXORFirst.GetElementsByTagNameL(andListFirst, WSPolicy::KAndCompositeAssertion, WSPolicy::KWsPolicyNsUri);



    //second policy
        TXmlEngElement policyElementSec = listPolicySecond.Next();
        
        TPtrC8 localName2 = policyElementSec.TagName();

                
        RXmlEngNodeList<TXmlEngElement> xorListSec; //get the only xor element
        policyElementSec.GetElementsByTagNameL(xorListSec, WSPolicy::KXorCompositeAssertion, WSPolicy::KWsPolicyNsUri);
        TXmlEngElement pXORSec = NULL;
        
        if(xorListSec.HasNext())
            pXORSec = xorListSec.Next();
        //we have to get all AND elements from the above xor. we will do it in loop
        
        
        while (andListFirst.HasNext())
        {
            TXmlEngElement AND = XOR.AddNewElementSameNsL(WSPolicy::KAndCompositeAssertion);
            
            TXmlEngElement andEleF = andListFirst.Next();
            
            TPtrC8 localName = andEleF.TagName();
            //find all assertions in this AND element
            
            
            MergeCompareSecondAssertionsL(andEleF, pXORSec, AND);
        }
  
    }

    return XOR;
}


TXmlEngElement CPolicyMerger::MergeCompareSecondAssertionsL(TXmlEngElement aFirstElement, TXmlEngElement aSecondElement, TXmlEngElement aIntersectedAND)
{

    RXmlEngNodeList<TXmlEngElement> assertionListF; //get all of first assrtions
    aFirstElement.GetChildElements(assertionListF);

    RXmlEngNodeList<TXmlEngElement> andListSec; //get all of AND elements
    aSecondElement.GetElementsByTagNameL(andListSec, WSPolicy::KAndCompositeAssertion, WSPolicy::KWsPolicyNsUri);
    
    RArray<TXmlEngElement> result;
    while(andListSec.HasNext())
    {
    //for every of AND element, loop for every AND element in first policy
        TXmlEngElement andEleS = andListSec.Next();
        
        TPtrC8 localNameS = andEleS.TagName();
        //find all assertions in this AND element
        
        RXmlEngNodeList<TXmlEngElement> assertionListS; //get the only xor element
        andEleS.GetChildElements(assertionListS);
        //RESOLVE all assertion from same AND element  

         MergeAllANDAssertionsL(aFirstElement, andEleS, result);
         if(result.Count() > 0)             
         {
           for (TInt i = 0; i<result.Count(); i++) 
           {
                TXmlEngElement resultEle = result[i].CopyL();
                aIntersectedAND.AppendChildL(resultEle);
           }
           result.Reset();
         }
        //if result then we have merged policy else we have NULL here
         
    }
    return NULL;
}

void CPolicyMerger::MergeAllANDAssertionsL(TXmlEngElement aFirstElement, TXmlEngElement aSecondElement, RArray<TXmlEngElement>& aResult)
{
    TInt outcount = 0;
    TInt inCount = 0;

    RXmlEngNodeList<TXmlEngElement> assertionListF; //get all of first assrtions
    aFirstElement.GetChildElements(assertionListF);
    
    while (assertionListF.HasNext())
    {
        TXmlEngElement assertionFirst = assertionListF.Next();

        TPtrC8 localName1 = assertionFirst.TagName();
        outcount++;

        RXmlEngNodeList<TXmlEngElement> assertionListS; //get all of AND elements
        aSecondElement.GetChildElements(assertionListS);

        while(assertionListS.HasNext())
        {
            TXmlEngElement assertionSecond = assertionListS.Next();

            TPtrC8 localName2 = assertionSecond.TagName();
            inCount++;
            if((localName1.Compare(localName2) == 0) && 
            VerifyNamespace(assertionFirst, assertionSecond))            
            {
                //call domain specific policy to resolve
                //check the namespaces here to find which domain we need to process
                //both namespaces should be checked and if they are same then we can intersect them
                TPtrC8 namespaceUri1 = assertionFirst.NamespaceUri();
                //if its ws security policy
                if(namespaceUri1.Compare(KWsSecurityPolicyNsUri) == 0)
                {
/*                
                    TXmlEngElement result = iSecurityPolicyDomain->Merge(localName1, assertionFirst, assertionSecond);
                    if(result.NotNull())
                    {
                        aResult.Append(result);
                    }
                    else
                    {
                        aResult.Append(assertionFirst);
                        aResult.Append(assertionSecond);
                    }
*/                    
                }                
            }
        }
        
    }
    
}

TInt CPolicyMerger::VerifyNamespace(TXmlEngElement aFirstElement, TXmlEngElement aSecondElement)
{
    TPtrC8 namespaceUri1 = aFirstElement.NamespaceUri();
    TPtrC8 namespaceUri2 = aSecondElement.NamespaceUri();
    
    if(namespaceUri1 != KNullDesC8() && namespaceUri2 != KNullDesC8())
    {
      if(namespaceUri1.Compare(namespaceUri2) == 0)
        return ETrue;  
    }

  return KErrNotFound;  
}
