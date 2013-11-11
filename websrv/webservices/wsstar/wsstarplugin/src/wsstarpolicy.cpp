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
#include "wsstarpolicy.h"
#include "wsstarpolicyhandler.h" 
#include "wsstarcons.h"

namespace
{   
    _LIT8(KServicePolicyLocalName, "ServicePolicy");
    _LIT8(KClientServicePolicyLocalName, "ClientPolicy");
    _LIT8(KSenNsUri, "urn:com.nokia.Sen.config.1.0");
    
    _LIT8(KHeader, "Header");
    _LIT8(KBody, "Body");
    _LIT8(KNamespace, "Namespace");
}
/*

CWSStarPolicy* CWSStarPolicy::NewL()
{
    CWSStarPolicy* pSelf =
    CWSStarPolicy::NewLC();
    CleanupStack::Pop(pSelf);
    return pSelf;   
}
CWSStarPolicy* CWSStarPolicy::NewLC()
{
    CWSStarPolicy* pSelf =
    new (ELeave) CWSStarPolicy();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf; 
}
CWSStarPolicy::~CWSStarPolicy()
{
    
}
CWSStarPolicy::CWSStarPolicy()
{
    
}
void CWSStarPolicy::ConstructL()
{
}


void CWSStarPolicy::SetPolicy(CSenElement* aPolicy)
{
}

*/
CSenElement* CWSStarPolicy::GetPolicyL(MSenHandlerContext& aCtx, MSenServiceDescription* aSD)	//codescannerwarnings
{

    CWSStarPlugin* SIF = (CWSStarPlugin*)aCtx.GetAnyL(HandlerContextKey::KSIF());//codescannerwarnings
    if(SIF)
    {
        CSenSessionHandler* pPolicyHandler =  (CSenSessionHandler*)SIF->Handler(WSStarConfig::KPolicyValue);
        if(pPolicyHandler)
        {
            CWSStarPolicyHandler* pHandler = (CWSStarPolicyHandler*)pPolicyHandler;
            if( pHandler )
            {
                TPtrC8 contract(KWSStarSTSContract());
                return pHandler->PolicyL(aSD);	//codescannerwarnings
            }
        }
    }
    return NULL;
}

TPtrC8 CWSStarPolicy::GetAttributeValueL(const TDesC8& aAttribute, CSenElement* aElement)
{
    RPointerArray<CSenBaseAttribute>& attrs = aElement->AttributesL();
    
    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
        bs = (attrs)[j];
        if(bs)

        if(bs->Name().Compare(aAttribute) == 0)
            {
             return bs->Value();
            }
         }
    return KNullDesC8();
}

TPtrC8 CWSStarPolicy::GetNSPrefixL(const TDesC8& aNamespaceUri, CSenElement* aElement)
{
    RPointerArray<CSenNamespace> sourceNamespaces = aElement->NamespacesL();
    if (sourceNamespaces.Count() > 0)
    {
        for (TInt i=0;i<sourceNamespaces.Count(); i++)
        {
            CSenNamespace* pNamespace = sourceNamespaces[i];
            if(pNamespace->URI().Compare(aNamespaceUri) == 0)
            {
                return pNamespace->Prefix();
            }
        }
    }
    
    return KNullDesC8();        
}
TInt CWSStarPolicy::ChildCountL(CSenElement* aTerm)	//codescannerwarnings
{
    RPointerArray<CSenElement>& children = aTerm->ElementsL();
    
    TInt childCount = children.Count();
    return childCount;
    
}
TBool CWSStarPolicy::CheckPolicyValueL(const TDesC8& aKey, MSenServiceDescription* aSD)
    {

    CSenWSDescription* pSD = (CSenWSDescription*)aSD;
//    CSenElement* pServicePolicy = pSD->AsElement().Element(KServicePolicyLocalName);
    MSenServicePolicy* pServicePolicy = pSD->ServicePolicy(); 
    // Cant be possible :p
    if(!pServicePolicy)
        return EFalse;
   
//    CSenElement* pClientPolicy = pServicePolicy->Element(KClientServicePolicyLocalName);
    CSenElement* pClientPolicy = pServicePolicy->ClientPolicy();
/*    
    if(!pClientPolicy)
        {
        pClientPolicy = pServicePolicy->Element(KSenNsUri, KClientServicePolicyLocalName);
        }
  */	
    //now start worrying
    if(!pClientPolicy)
    return EFalse;
    
    if(pClientPolicy)
    {
        CSenElement* result = NULL;
        result = pClientPolicy->Element(aKey);
        if(result)
            return ETrue;
        else
            return EFalse;
    }                    

    return EFalse;
    }
HBufC8* CWSStarPolicy::GetPolicyValueL(const TDesC8& aKey, MSenServiceDescription* aSD)
    {

    CSenWSDescription* pSD = (CSenWSDescription*)aSD;
    CSenElement* pServicePolicy = pSD->AsElement().Element(KServicePolicyLocalName);
    HBufC8* value = NULL;
    // Cant be possible :p
    if(!pServicePolicy)
        return value;
    CSenElement* pClientPolicy = pServicePolicy->Element(KClientServicePolicyLocalName);
    if(!pClientPolicy)
        {
        pClientPolicy = pServicePolicy->Element(KSenNsUri, KClientServicePolicyLocalName);
        }
    //now start worrying
    if(!pClientPolicy)
        return value;
    
    if(pClientPolicy)
    {
        CSenElement* result = NULL;
        result = pClientPolicy->Element(aKey);
        if(result && result->HasContent())
        {
            value = result->Content().AllocL();
        }
    }
      return value;
    }
TInt CWSStarPolicy::SearchForElementsL(CSenElement* aPolicy, TPtrC8 aKey, RPointerArray<CSenElement>& aElements)
{
    RPointerArray<CSenElement>& children = aPolicy->ElementsL();
    TInt count(children.Count());
    CSenElement* req = NULL;
    CSenElement* result = NULL;
    for (TInt i = 0; i < count; i++)
     {
        req = children[i];
        TPtrC8 nimi = req->LocalName();
        if(nimi.Compare(aKey) == 0)
        {
                result =  req;
                aElements.Append(result);
        }

        if(!result)
            SearchForElementsL(req, aKey, aElements);
//        if (result)
//           break;
     }
     
    return aElements.Count();
    
}
CSenElement* CWSStarPolicy::SearchForElementL(CSenElement* aPolicy, TPtrC8 aKey)	//codescannerwarnings
{

    RPointerArray<CSenElement>& children = aPolicy->ElementsL();
    TInt count(children.Count());
    CSenElement* req = NULL;
    CSenElement* result = NULL;
    for (TInt i = 0; i < count; i++)
     {
        req = children[i];
        TPtrC8 nimi = req->LocalName();
        if(nimi.Compare(aKey) == 0)
        {
                result =  req;
        }
        if(!result)
            result = SearchForElementL(req, aKey);	//codescannerwarnings
        if (result)
           break;
     }
     
    return result;
}

HBufC8* CWSStarPolicy::GetPolicyValueL(const TDesC8& aKey, const TDesC8& aSubKey, CWSStarPlugin& aWSStarPlugin, MSenServiceDescription* aSD)
    {
    HBufC8* result = NULL;
    CSenElement* policy = NULL;
    
    CSenSessionHandler* pPolicyHandler =  (CSenSessionHandler*)(aWSStarPlugin.Handler(WSStarConfig::KPolicyValue()));
    if(pPolicyHandler)
    {
        CWSStarPolicyHandler* phand = (CWSStarPolicyHandler*)pPolicyHandler;
        if(phand)
        {
                TPtrC8 contract(KWSStarSTSContract());
                policy =  phand->PolicyL(aSD); //Policy(aSD);	//codescannerwarnings
  
        }
           
    }
    if(policy)
    {
        CSenElement* token = SearchForElementL(policy, aKey);	//codescannerwarnings
        CSenElement* subValue = NULL;
        if(token)                
        {
            if(aSubKey.Length() > 0)
                subValue =  SearchForElementL(token, aSubKey);   	//codescannerwarnings
            else
                subValue = token;
            
                if(subValue)
                {
                    if(ChildCountL(subValue) > 0)	//codescannerwarnings
                        {
                        //not supprted now
                        }
                    if(subValue->HasContent())
                        {
                        result = subValue->Content().AllocL();
                        }
                }
            }
        }
    return result;
    }
HBufC8* CWSStarPolicy::GetPolicyValueL(const TDesC8& aKey, const TDesC8& aSubKey, MSenHandlerContext& aCtx, MSenServiceDescription* aSD)
    {
    RPointerArray<CSenElement> aElements;
    return GetPolicyValueL(aKey, aSubKey, aCtx, aSD, aElements);
    }
HBufC8* CWSStarPolicy::GetPolicyValueL(const TDesC8& aTokenType, const TDesC8& aKey, const TDesC8& aSubKey, MSenHandlerContext& aCtx, MSenServiceDescription* aSD)
    {
    RPointerArray<CSenElement> aElements;
    return GetPolicyValueL(aTokenType, aKey, aSubKey, aCtx, aSD, aElements);
    }
HBufC8* CWSStarPolicy::GetPolicyValueL(const TDesC8& aTokenType, const TDesC8& aKey, const TDesC8& aSubKey, MSenHandlerContext& aCtx, MSenServiceDescription* aSD, RPointerArray<CSenElement>& aElements)
{
    HBufC8* result = NULL;
    CSenElement* policy = GetPolicyL(aCtx, aSD);	//codescannerwarnings
    if(policy)
    {
        RPointerArray<CSenElement> elements;
        CleanupClosePushL(elements);
        
        TInt count = SearchForElementsL(policy, aKey, elements);

        for (TInt i = 0; i< count; i++)
            {
            
            CSenElement* token = elements[i];
            CSenElement* ele = SearchForElementL(token, _L8("TokenType"));	//codescannerwarnings
            if(ele)            
            {
                if(ele->HasContent())
                {
                    TPtrC8 contents = ele->Content();
                    if(contents == aTokenType)
                        {
                        CSenElement* subValue = NULL;
                        if(token)
                            {
                                if(aSubKey.Length() > 0)
                                    subValue =  SearchForElementL(token, aSubKey);   	//codescannerwarnings
                                else
                                    subValue = token;
                                
                                    if(subValue)
                                    {
                                        if(ChildCountL(subValue) > 0)	//codescannerwarnings
                                        {
                                            //                    All of the elements
                                            aElements =  subValue->ElementsL();
                                            //OuterXML
                                            HBufC8* data = subValue->AsXmlL();
                                            if(data && data->Length() > 0)
                                            {//parse the tag name and return the child tags only
                                                TInt start = data->Locate('>');
                                                TInt end = data->LocateReverse('<');
                                                if(start+1 < end-1 && start != KErrNotFound && end != KErrNotFound)
                                                {
                                                    TInt length = end-1-start;
                                                    TPtrC8 dataq(data->Mid(start+1,length));
                                                    result = HBufC8::NewL(dataq.Length());
                                                    TPtr8 id16 = result->Des();
                                                    id16.Copy(dataq); 
                                                }
                                                
                                            }
                                            delete data;
                                        }
                                        else if(subValue->HasContent())
                                        {
                                            result = subValue->Content().AllocL();
                                        }
                                        
                                    }
                            }
                        
                        
                        
                        }
                }
            }
            
            }
        CleanupStack::Pop(&elements);
        elements.Close();
    }   
            
    return result;
    
}
HBufC8* CWSStarPolicy::GetPolicyValueL(const TDesC8& aKey, const TDesC8& aSubKey, MSenHandlerContext& aCtx, MSenServiceDescription* aSD, RPointerArray<CSenElement>& aElements)
{
    HBufC8* result = NULL;
    CSenElement* policy = GetPolicyL(aCtx, aSD);	//codescannerwarnings
    if(policy)
    {
        CSenElement* token = SearchForElementL(policy, aKey);	//codescannerwarnings
        CSenElement* subValue = NULL;
        if(token)                
        {
            if(aSubKey.Length() > 0)
                subValue =  SearchForElementL(token, aSubKey);   	//codescannerwarnings
            else
                subValue = token;
            
                if(subValue)
                {
                    if(ChildCountL(subValue) > 0)	//codescannerwarnings
                    {
                        //                    All of the elements
                        aElements =  subValue->ElementsL();
                        //OuterXML
                        HBufC8* data = subValue->AsXmlL();
                        if(data && data->Length() > 0)
                        {//parse the tag name and return the child tags only
                            TInt start = data->Locate('>');
                            TInt end = data->LocateReverse('<');
                            if(start+1 < end-1 && start != KErrNotFound && end != KErrNotFound)
                            {
                                TInt length = end-1-start;
                                TPtrC8 dataq(data->Mid(start+1,length));
                                result = HBufC8::NewL(dataq.Length());
                                TPtr8 id16 = result->Des();
                                id16.Copy(dataq); 
                            }
                            
                        }
                        delete data;
                    }
                    else if(subValue->HasContent())
                    {
                        result = subValue->Content().AllocL();
                    }
                    
                }
        }
        
    }   
            
    return result;
}

void CWSStarPolicy::GetPolicyValueL(const TDesC8& aKey, const TDesC8& aSubKey,MSenServiceDescription* aSD,RPointerArray<CSenElement>& aElements)
    {

    CSenWSDescription* pSD = (CSenWSDescription*)aSD;
    CSenElement* pServicePolicy = pSD->AsElement().Element(KServicePolicyLocalName);

    // Cant be possible :p
    if(!pServicePolicy)
        return;
    CSenElement* pClientPolicy = pServicePolicy->Element(KClientServicePolicyLocalName);
    if(!pClientPolicy)
        {
        pClientPolicy = pServicePolicy->Element(KSenNsUri, KClientServicePolicyLocalName);
        }
    //now start worrying
    if(!pClientPolicy)
        return;
    
    if(pClientPolicy)
        {
        CSenElement* resultKey = NULL;
        resultKey = pClientPolicy->Element(aKey);
        if(resultKey)
            {
            CSenElement* resultSubKey = NULL;
            resultSubKey = resultKey->Element(aSubKey);
            
            if(resultSubKey)
                {
                 aElements = resultSubKey->ElementsL();
                
                     }
            
                 } 
         
            }
      return;
    }
void CWSStarPolicy::GetPolicyValueL(const TDesC8& aKey, const TDesC8& aSubKey, MSenHandlerContext& aCtx, MSenServiceDescription* aSD, RPointerArray<HBufC8>& aList)
    {
    CSenElement* policy = GetPolicyL(aCtx, aSD);	//codescannerwarnings
    if (policy)
        {
        GetValueL(aKey, aSubKey, policy, aList);
        }
    }
void CWSStarPolicy::GetPolicyValueL(const TDesC8& aKey, const TDesC8& aSubKey, CWSStarPolicyHandler* aPolicyHandler, MSenServiceDescription* aSD, RPointerArray<HBufC8>& aList)
    {
    CSenElement* policy = aPolicyHandler->PolicyL(aSD);	//codescannerwarnings
    if (policy)
        {
        GetValueL(aKey, aSubKey, policy, aList);
        }
    /*if (aList.Count() == 0 && aKey == WSPolicy::KSignedParts)
        {
        aList.ResetAndDestroy();
        aList.Append(_L8("MessageID").AllocL());
        aList.Append(_L8("http://schemas.xmlsoap.org/ws/2004/08/addressing").AllocL());
        aList.Append(_L8("ReplyTo").AllocL());
        aList.Append(_L8("http://schemas.xmlsoap.org/ws/2004/08/addressing").AllocL());
        aList.Append(_L8("To").AllocL());
        aList.Append(_L8("http://schemas.xmlsoap.org/ws/2004/08/addressing").AllocL());
        aList.Append(_L8("Action").AllocL());
        aList.Append(_L8("http://schemas.xmlsoap.org/ws/2004/08/addressing").AllocL());
        aList.Append(_L8("Timestamp").AllocL());
        aList.Append(_L8("http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd").AllocL());
        }*/
    }

void CWSStarPolicy::GetValueL(const TDesC8& aKey, const TDesC8& aSubKey, CSenElement* aPolicy, RPointerArray<HBufC8>& aList)
    {
    CSenElement* token = SearchForElementL(aPolicy, aKey);	//codescannerwarnings
    CSenElement* subValue = NULL;
    if(token)
    {
        if(aSubKey.Length() > 0)
            subValue =  SearchForElementL(token, aSubKey);   	//codescannerwarnings
        else
            subValue = token;
        
        if(subValue)
        {
            if(aKey == WSPolicy::KSignedParts || aSubKey == WSPolicy::KSignedParts)
            {
                CEncryptedSignedPartsI* signedParts = CEncryptedSignedPartsI::NewL(subValue);    
                if(signedParts->Header() || signedParts->Body())
                {
                //return the list of elements to be signed or Enrypted
                //If Body element is found in list then whole body should be signed/encrypted
                //If Header Element is found then All headers should be signed/encrypted
                //If individual elements are found then only those elements 
                // in Header should be signed/encrypted
                    signedParts->ElementsL(aList);	//codescannerwarnings
                    
                }
                delete  signedParts;                   
            }
        }
    }
    }
CSenElement* CWSStarPolicy::GetElementL(const TDesC8& aKey, const TDesC8& aSubKey, MSenHandlerContext& aCtx, MSenServiceDescription* aSD)
{
    CSenElement* policy = GetPolicyL(aCtx, aSD);	//codescannerwarnings
    if(policy)
    {
        CSenElement* token = SearchForElementL(policy, aKey);	//codescannerwarnings
        CSenElement* subValue = NULL;
        if(token)
        {
            if(aSubKey.Length() > 0)
                subValue =  SearchForElementL(token, aSubKey);   	//codescannerwarnings
            else
                subValue = token;
            
            if(subValue)
            {
                if(aKey == WSPolicy::KDictionaries || aSubKey == WSPolicy::KDictionary)
                {
                return subValue;
                }
            }
        }
        
    }   
    return NULL;
}

    
///////////////////////////////////////////////////////////////////////
//////    CEncryptedSignedPartsI                                              
///////////////////////////////////////////////////////////////////////
CEncryptedSignedPartsI* CEncryptedSignedPartsI::NewL(CSenElement* aAssertion)
{
     CEncryptedSignedPartsI* pSelf = CEncryptedSignedPartsI::NewLC(aAssertion);
    CleanupStack::Pop(pSelf);
    return pSelf;  
}  

CEncryptedSignedPartsI* CEncryptedSignedPartsI::NewLC(CSenElement* aAssertion)
{
  
    CEncryptedSignedPartsI* pSelf = new (ELeave) CEncryptedSignedPartsI();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aAssertion);
    return pSelf;  
}
CEncryptedSignedPartsI::~CEncryptedSignedPartsI()
{
    iHeaders.Close();
}

TBool CEncryptedSignedPartsI::IsEqualEncryptedSignedParts(CEncryptedSignedPartsI* aAssertion)
{
    if(Body() == aAssertion->Body() && Header() == aAssertion->Header())
    {
        return ETrue;    
    }
    
    return EFalse;
}

TBool CEncryptedSignedPartsI::Header()
{
    return iHeader;    
}
TBool CEncryptedSignedPartsI::Body()
{
    return iBody;
}
void CEncryptedSignedPartsI::ElementsL(RPointerArray<HBufC8>& aElements)	//codescannerwarnings
{
    if(iBody)
        {
        aElements.Append(KBody().AllocL());
        aElements.Append(KNullDesC8().AllocL());
        }
        
    if(iHeader && iHeaders.Count() == 0)
    {
        aElements.Append(KHeader().AllocL());
    }
    else if(iHeader && iHeaders.Count() > 0)
    {
        for(TInt i = 0; i< iHeaders.Count(); i++)
        {
            SoapHeader header(iHeaders[i]);
            TPtrC8 name(header.iName);
            if(name != KNullDesC8())
            {
               aElements.Append(name.AllocL());
               aElements.Append(header.iNsUri.AllocL());
            }
            
        }
    }
}
void CEncryptedSignedPartsI::ConstructL(CSenElement* aAssertion)
{
//No need to rip off caz there wont be any policy elements here

    RPointerArray<CSenElement>& tokenChildren = aAssertion->ElementsL(); //find the policy element
    
    if(tokenChildren.Count() < 0)
    {
        iHeader = ETrue;
        iBody = ETrue;
        return;
    }
   
    CSenElement* child = NULL;
    SoapHeader header;
    for (TInt i = 0; i< tokenChildren.Count(); i++) //while (tokenChildren.HasNext()) // Get all the elements
    { 
        child = tokenChildren[i];

        TPtrC8 localName = child->LocalName();
                
        if(localName.Compare(KBody) == 0)
        {
           iBody = ETrue;
        }
        else if(localName.Compare(KHeader) == 0)
        {
            iHeader = ETrue;
            //get header attributes
            
           TPtrC8 headerName = CWSStarPolicy::GetAttributeValueL(WSPolicy::KName, child);            
           if(headerName != KNullDesC8())
           {

               header.iName.Set(headerName);
           }
           
           TPtrC8 nsUri = CWSStarPolicy::GetAttributeValueL(KNamespace, child);            
           //if(nsUri != KNullDesC8())
           //{
           header.iNsUri.Set(nsUri);             
           //               header.iNsPrefix.Set(CWSStarPolicy::GetNSPrefixL(nsUri, child));
           //}           
           iHeaders.Append(header);

        }
    }
}
CEncryptedSignedPartsI::CEncryptedSignedPartsI()
:
    iHeader(EFalse),
    iBody(EFalse)
{
    
}
// End of file
