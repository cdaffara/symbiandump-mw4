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




#include "wssecuritypolicy.h"
#include "wspolicy.h"
#include "wssecuritypolicyconstants.h"

using namespace WsSecurityPolicy;

CWsSecurityPolicy* CWsSecurityPolicy::NewL()
{
    CWsSecurityPolicy* pSelf =
        CWsSecurityPolicy::NewLC();
    CleanupStack::Pop(pSelf);
    return pSelf;
}
CWsSecurityPolicy* CWsSecurityPolicy::NewLC()
{
    CWsSecurityPolicy* pSelf = new (ELeave) CWsSecurityPolicy();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;  
}
void CWsSecurityPolicy::ConstructL()
{

}
CWsSecurityPolicy::~CWsSecurityPolicy()
{
    
}
CWsSecurityPolicy::CWsSecurityPolicy()
{
    
}

CSenElement* CWsSecurityPolicy::IntersectL(TDesC8& aKey, CSenElement* aFirst, CSenElement* aSecond)
    {
    TBool reply(EFalse);

    if(aKey.Compare(KAsymmetricBinding) == 0)
        {
        CAsymmetricBinding* firstBinding = CAsymmetricBinding::NewL(aFirst);
        CleanupStack::PushL( firstBinding );
        CAsymmetricBinding* secondBinding = CAsymmetricBinding::NewL(aSecond);
        CleanupStack::PushL( secondBinding );
        reply = firstBinding->IsEqualAsymmtericAssertion( secondBinding );        
        CleanupStack::PopAndDestroy( secondBinding ); // delete secondBinding;
        CleanupStack::PopAndDestroy( firstBinding ); // delete firstBinding;
        }
    else if(aKey.Compare(KSymmetricBinding) == 0)
        {
        CSymmetricBinding* firstBinding = CSymmetricBinding::NewL(aFirst);
        CleanupStack::PushL( firstBinding );
        CSymmetricBinding* secondBinding = CSymmetricBinding::NewL(aSecond);
        CleanupStack::PushL( secondBinding );
        reply = firstBinding->IsEqualSymmtericAssertion(secondBinding);
        CleanupStack::PopAndDestroy( secondBinding ); // delete secondBinding;
        CleanupStack::PopAndDestroy( firstBinding ); // delete firstBinding;
        }
    else if(aKey.Compare(KWss10) == 0)
        {
        CWss10* pFirstAssertion = CWss10::NewL(aFirst);
        CleanupStack::PushL( pFirstAssertion );
        CWss10* pSecondAssertion = CWss10::NewL(aSecond);
        CleanupStack::PushL( pSecondAssertion );
        reply = pFirstAssertion->IsEqualWss10Assertion(pSecondAssertion);
        CleanupStack::PopAndDestroy( pSecondAssertion ); // delete pSecondAssertion;
        CleanupStack::PopAndDestroy( pFirstAssertion ); // delete pFirstAssertion;
        }
    else if(aKey.Compare(KWss11) == 0)
        {
        CWss11* pFirstAssertion = CWss11::NewL(aFirst);
        CleanupStack::PushL( pFirstAssertion );
        CWss11* pSecondAssertion = CWss11::NewL(aSecond);
        CleanupStack::PushL( pSecondAssertion );
        reply = pFirstAssertion->IsEqualWss11Assertion(pSecondAssertion);
        CleanupStack::PopAndDestroy( pSecondAssertion ); // delete pSecondAssertion;
        CleanupStack::PopAndDestroy( pFirstAssertion ); // delete pFirstAssertion;
        }
    else if(aKey.Compare(KTrust10) == 0)
        {
        CWsTrust10* pFirstAssertion = CWsTrust10::NewL(aFirst);
        CleanupStack::PushL( pFirstAssertion );
        CWsTrust10* pSecondAssertion = CWsTrust10::NewL(aSecond);
        CleanupStack::PushL( pSecondAssertion );
        reply = pFirstAssertion->IsEqualWsTrust10Assertion(pSecondAssertion);
        CleanupStack::PopAndDestroy( pSecondAssertion ); // delete pSecondAssertion;
        CleanupStack::PopAndDestroy( pFirstAssertion ); // delete pFirstAssertion;
        }
    else if(aKey.Compare(KSignedParts) == 0 || aKey.Compare(KEncryptedParts) == 0 )
        {
        CEncryptedSignedParts* pFirstAssertion = CEncryptedSignedParts::NewL(aFirst);
        CleanupStack::PushL( pFirstAssertion );
        CEncryptedSignedParts* pSecondAssertion = CEncryptedSignedParts::NewL(aSecond);        
        CleanupStack::PushL( pSecondAssertion );
        reply = pFirstAssertion->IsEqualEncryptedSignedParts(pSecondAssertion);
        CleanupStack::PopAndDestroy( pSecondAssertion ); // delete pSecondAssertion;
        CleanupStack::PopAndDestroy( pFirstAssertion ); // delete pFirstAssertion;
        }   
    else if(aKey.Compare(KSignedElements) == 0 || aKey.Compare(KEncryptedElements) == 0 )
        {
        CEncryptedSignedElements* pFirstAssertion = CEncryptedSignedElements::NewL(aFirst);
        CleanupStack::PushL( pFirstAssertion );
        CEncryptedSignedElements* pSecondAssertion = CEncryptedSignedElements::NewL(aSecond);
        CleanupStack::PushL( pSecondAssertion );
        reply = pFirstAssertion->IsEqualEncryptedSignedElements(pSecondAssertion);
        CleanupStack::PopAndDestroy( pSecondAssertion ); // delete pSecondAssertion;
        CleanupStack::PopAndDestroy( pFirstAssertion ); // delete pFirstAssertion;
        }   

    // return the first, if comparison equals
    if (reply)
        {
        return aFirst;
        }
    else
        {
        return NULL;
        }
    }

CSenElement* CWsSecurityPolicy::MergeL(TDesC8& aKey, CSenElement* aFirst, CSenElement* aSecond)
    {
    TBool reply(EFalse);
    if(aKey.Compare(KAsymmetricBinding) == 0)
    {
        CAsymmetricBinding* firstBinding = CAsymmetricBinding::NewL(aFirst);
        CleanupStack::PushL( firstBinding );
        CAsymmetricBinding* secondBinding = CAsymmetricBinding::NewL(aSecond);
        CleanupStack::PushL( secondBinding );
        reply = firstBinding->IsEqualAsymmtericAssertion(secondBinding);
        CleanupStack::PopAndDestroy( secondBinding ); // delete secondBinding;
        CleanupStack::PopAndDestroy( firstBinding ); // delete firstBinding;
        }
    else if(aKey.Compare(KSymmetricBinding) == 0)
        {
        CSymmetricBinding* firstBinding = CSymmetricBinding::NewL(aFirst);
        CleanupStack::PushL( firstBinding );
        CSymmetricBinding* secondBinding = CSymmetricBinding::NewL(aSecond);
        CleanupStack::PushL( secondBinding );
        reply = firstBinding->IsEqualSymmtericAssertion(secondBinding);
        CleanupStack::PopAndDestroy( secondBinding ); // delete secondBinding;
        CleanupStack::PopAndDestroy( firstBinding ); // delete firstBinding;
        }
    else if(aKey.Compare(KWss10) == 0)
        {
        CWss10* pFirstAssertion = CWss10::NewL(aFirst);
        CleanupStack::PushL( pFirstAssertion );
        CWss10* pSecondAssertion = CWss10::NewL(aSecond);
        CleanupStack::PushL( pSecondAssertion );
        reply = pFirstAssertion->IsEqualWss10Assertion(pFirstAssertion);
        CleanupStack::PopAndDestroy( pSecondAssertion ); // delete pSecondAssertion;
        CleanupStack::PopAndDestroy( pFirstAssertion ); // delete pFirstAssertion;
        }
    else if(aKey.Compare(KWss11) == 0)
        {
        CWss11* pFirstAssertion = CWss11::NewL(aFirst);
        CleanupStack::PushL( pFirstAssertion );
        CWss11* pSecondAssertion = CWss11::NewL(aSecond);
        CleanupStack::PushL( pSecondAssertion );
        reply = pFirstAssertion->IsEqualWss11Assertion(pSecondAssertion);
        CleanupStack::PopAndDestroy( pSecondAssertion ); // delete pSecondAssertion;
        CleanupStack::PopAndDestroy( pFirstAssertion ); // delete pFirstAssertion;
        }
     else if(aKey.Compare(KTrust10) == 0)
        {
        CWsTrust10* pFirstAssertion = CWsTrust10::NewL(aFirst);
        CleanupStack::PushL( pFirstAssertion );
        CWsTrust10* pSecondAssertion = CWsTrust10::NewL(aSecond);
        CleanupStack::PushL( pSecondAssertion );
        reply = pFirstAssertion->IsEqualWsTrust10Assertion(pSecondAssertion);
        CleanupStack::PopAndDestroy( pSecondAssertion ); // delete pSecondAssertion;
        CleanupStack::PopAndDestroy( pFirstAssertion ); // delete pFirstAssertion;
        }   
    else if(aKey.Compare(KSignedParts) == 0 || aKey.Compare(KEncryptedParts) == 0 )
        {
        CEncryptedSignedParts* pFirstAssertion = CEncryptedSignedParts::NewL(aFirst);
        CleanupStack::PushL( pFirstAssertion );
        CEncryptedSignedParts* pSecondAssertion = CEncryptedSignedParts::NewL(aSecond);        
        CleanupStack::PushL( pSecondAssertion );
        reply = pFirstAssertion->IsEqualEncryptedSignedParts(pSecondAssertion);
        CleanupStack::PopAndDestroy( pSecondAssertion ); // delete pSecondAssertion;
        CleanupStack::PopAndDestroy( pFirstAssertion ); // delete pFirstAssertion;
        }   
    else if(aKey.Compare(KSignedElements) == 0 || aKey.Compare(KEncryptedElements) == 0 )
        {
        CEncryptedSignedElements* pFirstAssertion = CEncryptedSignedElements::NewL(aFirst);
        CleanupStack::PushL( pFirstAssertion );
        CEncryptedSignedElements* pSecondAssertion = CEncryptedSignedElements::NewL(aSecond);
        CleanupStack::PushL( pSecondAssertion );
        reply = pFirstAssertion->IsEqualEncryptedSignedElements(pSecondAssertion);
        CleanupStack::PopAndDestroy( pSecondAssertion ); // delete pSecondAssertion;
        CleanupStack::PopAndDestroy( pFirstAssertion ); // delete pFirstAssertion;

//        RArray<TPtrC8> result;
//        TBool signAll = firstAssertion->MergedElements(SecondAssertion, result);
//        CSenElement* mergedEelement = aFirst.CopyL();
//        mergedEelement.RemoveChildren();
//        
//        if(!signAll)
//        {
//
//            //reply value depends on it
//            if(//reply && 
//            result.Count() > 0)
//            {
////                TString argument;
////                argument.SetL(KXPath);
//                for(TInt i = 0; i< result.Count(); i++)
//                {//Copy original element, delete all children and add new children
//                    
//                    TPtrC8 te= result[i];
////                    TString content;
////                    content.SetL(te);
//                    CSenElement* xpath = mergedEelement.AddNewElementSameNsL(KXPath);
//                    xpath.AddTextL(te);
////                    content.Close();
//                }
////                argument.Close();
//
//              }
//        
//        }
//
        }   
    if (reply)
        {
        return aFirst;
        }
    else
        {
        return NULL;
        }
    }


///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////

CToken::~CToken()
    {
    }

CToken::CToken():
iTokenInclusion(KIncludeAlways),
iDerivedKeys(EFalse)
    {
    }

void CToken::ConstructL(CSenElement* aToken)
    {
    const TDesC8* attr = aToken->AttrValue(KIncludeToken);
    if(attr)
        {
        HBufC8* attrib = attr->AllocL();
        TPtrC8 includeToken = attrib->Des();
        if(includeToken.Compare(KIncludeNever) == 0)
            iTokenInclusion.Set(KIncludeNever);
        else if(includeToken.Compare(KIncludeOnce) == 0)
            iTokenInclusion.Set(KIncludeOnce);
        else if(includeToken.Compare(KIncludeAlwaysToReceipent) == 0)
            iTokenInclusion.Set(KIncludeAlwaysToReceipent);
        else if(includeToken.Compare(KIncludeAlways) == 0)
            iTokenInclusion.Set(KIncludeAlways);
        delete attrib;
        }
   }

void CToken::BaseConstructL(CSenElement* aToken)
    {
    ConstructL(aToken);
    }


CHttpsToken* CHttpsToken::NewL(CSenElement* aToken)
    {
    CHttpsToken* pSelf = CHttpsToken::NewLC(aToken);
    CleanupStack::Pop(pSelf);
    return pSelf;
    }

CHttpsToken* CHttpsToken::NewLC(CSenElement* aToken)
    {
    CHttpsToken* pSelf = new (ELeave) CHttpsToken();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aToken);
    return pSelf;  
    }

CHttpsToken::~CHttpsToken()
    {
    }

CHttpsToken::CHttpsToken():
iRequireClientCertificate(ETrue)
{
 
}

void CHttpsToken::ConstructL(CSenElement* aToken)
    {
    const TDesC8* attr = aToken->AttrValue(KRequireClientCertificate); 
    if(attr)
        {
        HBufC8* attrib = attr->AllocL();
        TPtrC8 requireCertificateToken = attrib->Des();
        if(requireCertificateToken.Compare(KTrue) == 0)
            iRequireClientCertificate = ETrue;
        else if(requireCertificateToken.Compare(KFalse) == 0)
            iRequireClientCertificate = EFalse;
        }
    RPointerArray<CSenElement>& tokenChildren = aToken->ElementsL();
    CSenElement* child = NULL;
    for (TInt i = 0; i < tokenChildren.Count(); i++)
        { 
        //There should be only one element here the TokenType
        child = tokenChildren[i];
        TPtrC8 localName = child->LocalName();
        if(localName.Compare(WSPolicy::KWsPolicy) == 0)
            {
            iPolicy = child;
            }
        }
    }

CSamlToken* CSamlToken::NewL(CSenElement* aToken)
{
    CSamlToken* pSelf = CSamlToken::NewLC(aToken);
    CleanupStack::Pop(pSelf);
    return pSelf;
  
}
CSamlToken* CSamlToken::NewLC(CSenElement* aToken)
{
    CSamlToken* pSelf = new (ELeave) CSamlToken();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aToken);
    return pSelf; 

}
CSamlToken::~CSamlToken()
{
    
}
CSamlToken::CSamlToken()
{
    
}
void CSamlToken::ConstructL(CSenElement* aToken)
{
    BaseConstructL(aToken);
   
    CSenElement* AND = CXmlHelper::RipPolicyXORElementL(aToken);

    RPointerArray<CSenElement>& tokenChildren =   AND->ElementsL();

    CSenElement* child;

    for (TInt i = 0; i < tokenChildren.Count(); i++)
    { //There should be only one element here the TokenType
        child = tokenChildren[i];

        TPtrC8 localName = child->LocalName();
        
        if(localName.Compare(KRequireKeyIdentifierReference) == 0)
        {
            iRequireKeyIdentifierReference = ETrue;
        }
        else if(localName.Compare(KWssSamlV10Token10) == 0)
        {
            iSamlTokenProfile.Set(KWssSamlV10Token10);
        }
        else if(localName.Compare(KWssSamlV11Token10) == 0)
        {
            iSamlTokenProfile.Set(KWssSamlV11Token10);
        }
        
        else if(localName.Compare(KWssSamlV10Token11) == 0)
        {
            iSamlTokenProfile.Set(KWssSamlV10Token11);
        }
        else if(localName.Compare(KWssSamlV11Token11) == 0)
        {
            iSamlTokenProfile.Set(KWssSamlV11Token11);
        }
        else if(localName.Compare(KWssSamlV20Token11) == 0)
        {
            iSamlTokenProfile.Set(KWssSamlV20Token11);
        }
    }

}

CUsernameToken* CUsernameToken::NewL(CSenElement* aToken)
{
    CUsernameToken* pSelf = CUsernameToken::NewLC(aToken);
    CleanupStack::Pop(pSelf);
    return pSelf;
}
CUsernameToken* CUsernameToken::NewLC(CSenElement* aToken)
{
    CUsernameToken* pSelf = new (ELeave) CUsernameToken();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aToken);
    return pSelf; 
}
CUsernameToken::~CUsernameToken()
{

}

CUsernameToken::CUsernameToken()
:iUsernameTokenProfile(KWssUsernameToken10)
{

}
void CUsernameToken::ConstructL(CSenElement* aToken)
{
    BaseConstructL(aToken);
 
     
    CSenElement* AND = CXmlHelper::RipPolicyXORElementL(aToken);
     

    RPointerArray<CSenElement>& tokenChildren =  AND->ElementsL();
    
    CSenElement* child;
  
    for (TInt i = 0; i < tokenChildren.Count(); i++)
    { //There should be only one element here the TokenType
        child = tokenChildren[i];

        TPtrC8 localName = child->LocalName();
        
        if(localName.Compare(KWssUsernameToken10) == 0)
        {
            iUsernameTokenProfile.Set(KWssUsernameToken10);
        }
        else if(localName.Compare(KWssUsernameToken11) == 0)
        {
            iUsernameTokenProfile.Set(KWssUsernameToken11);
        }
    }

}


CRequestSecurityTokenTemplate* CRequestSecurityTokenTemplate::NewL(CSenElement* aToken)
{
    CRequestSecurityTokenTemplate* pSelf = CRequestSecurityTokenTemplate::NewLC(aToken);
    CleanupStack::Pop(pSelf);
    return pSelf;
    
}
CRequestSecurityTokenTemplate* CRequestSecurityTokenTemplate::NewLC(CSenElement* aToken)
{
    CRequestSecurityTokenTemplate* pSelf = new (ELeave) CRequestSecurityTokenTemplate();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aToken);
    return pSelf; 
   
}
CRequestSecurityTokenTemplate::~CRequestSecurityTokenTemplate()
{
  delete iToken;
  delete iTrustVersion;  
}
CRequestSecurityTokenTemplate::CRequestSecurityTokenTemplate()
:iToken(KNullDesC8().AllocL()),
iTrustVersion(KNullDesC8().AllocL())
{
    
}
void CRequestSecurityTokenTemplate::ConstructL(CSenElement* aToken)
{ //parse the token and then save it as HBufC8 

    const TDesC8* attr = aToken->AttrValue(KTrustVersion);    
    if(attr)
    {
        iTrustVersion = attr->AllocL();
    }
    
    RPointerArray<CSenElement>& tokenChildren =   aToken->ElementsL();

    CSenElement* child = NULL;
    RBuf8 buffer;
    CleanupClosePushL(buffer);
    RBuf8 request;
    CleanupClosePushL(request);

    for (TInt i = 0; i < tokenChildren.Count(); i++)  
    { 
        child = tokenChildren[i];
        
        TPtrC8 localName = child->LocalName();
        
//        child.OuterXmlL(buffer);
        request.Append(buffer);
    }

    iToken = request.AllocL();
    
    CleanupStack::PopAndDestroy(&request);
    CleanupStack::PopAndDestroy(&buffer);
        
}



CIssuedToken* CIssuedToken::NewL(CSenElement* aToken)
{
     CIssuedToken* pSelf = CIssuedToken::NewLC(aToken);
    CleanupStack::Pop(pSelf);
    return pSelf;
   
}
CIssuedToken* CIssuedToken::NewLC(CSenElement* aToken)
{
      CIssuedToken* pSelf = new (ELeave) CIssuedToken();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aToken);
    return pSelf; 
  
}
CIssuedToken::~CIssuedToken()
{
}

CIssuedToken::CIssuedToken()
:
iIssuer(NULL),
iRequireDerivedKeys(EFalse),
iRequestSecurityTokenTemplate(NULL),
iRequireExternalReference(EFalse),
iRequireInternalReference(EFalse)
{
}
void CIssuedToken::ConstructL(CSenElement* aToken)
{ //can have multiple child elements including poliy and assertions
   //loop for multiple children 
   
    BaseConstructL(aToken);

    RPointerArray<CSenElement>& tokenChildren =   aToken->ElementsL();

    CSenElement* child;
    for (TInt i = 0; i < tokenChildren.Count(); i++)  
    { 
        child = tokenChildren[i];

        TPtrC8 localName = child->LocalName();
        
        if(localName.Compare(KIssuer) == 0)
        { //get the issuer element and save it, this is endpoint of STS
            iIssuer = child; //request.AllocL();
        }
        else if(localName.Compare(KRequestSecurityTokenTemplate) == 0)
        { 
        //we are just using the origianl element o get all the 
        //information from RequestSecurityTokenTemplate
            iRequestSecurityTokenTemplate = child; 
            // CRequestSecurityTokenTemplate::NewL(child);  
        }
        else if(localName.Compare(WSPolicy::KWsPolicy) == 0)
        {
             
            CSenElement* AND = CXmlHelper::RipPolicyXORElementL(aToken);
             

            RPointerArray<CSenElement>& children = AND->ElementsL();
            
            CSenElement* child2;
            for (TInt j = 0; j < children.Count(); j++)  
            { 
                child2 = children[j];

                TPtrC8 localName = child2->LocalName();
                
                if(localName.Compare(KRequireDerivedKeys) == 0)
                { 
                    iRequireDerivedKeys = ETrue;
                }
                if(localName.Compare(KRequireExternalReference) == 0)
                { 
                    iRequireExternalReference = ETrue;
                }
                else if(localName.Compare(KRequireInternalReference) == 0)
                { 
                    iRequireInternalReference = ETrue; 
                }

            }

        }
    
    }

}


CSecureConversationToken* CSecureConversationToken::NewL(CSenElement* aToken)
{
    CSecureConversationToken* pSelf = CSecureConversationToken::NewLC(aToken);
    CleanupStack::Pop(pSelf);
    return pSelf;
    
}
CSecureConversationToken* CSecureConversationToken::NewLC(CSenElement* aToken)
{
    CSecureConversationToken* pSelf = new (ELeave) CSecureConversationToken();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aToken);
    return pSelf; 
  
}
CSecureConversationToken::~CSecureConversationToken()
{
 delete iIssuer;   
}

TPtrC8 CSecureConversationToken::Issuer()
{
  return iIssuer->Des();  
}
TBool CSecureConversationToken::RequireExternalUriReference()
{
    return iRequireExternalUriReference;
}
TBool CSecureConversationToken::SC10SecurityContextToken()
{
   return  iSC10SecurityContextToken;
}
TBool CSecureConversationToken::IsBootstrapPolicy()
{
    return iBootstrap;
}
CSenElement* CSecureConversationToken::BootstrapPolicy()
{
    return iBootstrapPolicy;
}
CSecureConversationToken::CSecureConversationToken()
:
    iIssuer(KNullDesC8().AllocL()),
    iRequireExternalUriReference(EFalse),
    iSC10SecurityContextToken(EFalse),
    iRequireDerivedKeys(EFalse),
    iBootstrap(EFalse)
{
    
}
void CSecureConversationToken::ConstructL(CSenElement* aToken)
{
   BaseConstructL(aToken);
   
   TPtrC8 localName = aToken->LocalName();
   
    RPointerArray<CSenElement>& tokenChildren = aToken->ElementsL();
    CSenElement* child;

    for (TInt j = 0; j < tokenChildren.Count(); j++)  
    { 
        child = tokenChildren[j];

        TPtrC8 childName = child->LocalName();
        
        if(childName.Compare(KIssuer) == 0)
        { //get the issuer element and save it, this is endpoint of STS
            RBuf8 buffer;
            CleanupClosePushL(buffer);
//            child.OuterXmlL(buffer);
            iIssuer = buffer.AllocL();
            CleanupStack::PopAndDestroy(&buffer);
        }
        else if(childName.Compare(WSPolicy::KWsPolicy) == 0)
        {
             
            CSenElement* AND = CXmlHelper::RipPolicyXORElementL(aToken);
             
            
            RPointerArray<CSenElement>& ANDChildren =  AND->ElementsL();
            TPtrC8 policyChildName = child->LocalName();
            
            CSenElement* child2;

            for (TInt i = 0; i < ANDChildren.Count(); i++)  
            { 
                child2 = ANDChildren[i];

                TPtrC8 localName = child2->LocalName();
  
                if(localName.Compare(KRequireDerivedKeys) == 0)
                { 
                     iRequireDerivedKeys = ETrue;
                }
                else if(localName.Compare(KRequireExternalUriReference) == 0)
                { 
                    iRequireExternalUriReference = ETrue; 
                }
                else if(localName.Compare(KSC10SecurityContextToken) == 0)
                { 
                    iSC10SecurityContextToken = ETrue; 
                }
                else if(localName.Compare(KBootstrapPolicy) == 0)
                { 
                    iBootstrap = ETrue;
                    iBootstrapPolicy = child2; 
                }

            }
        }

    }
 }


//TOKEN HOLDERS  
  ////////////////////////////////////////////////////////

CTokenHolder* CTokenHolder::NewL(CSenElement* aToken)
{
     CTokenHolder* pSelf = CTokenHolder::NewLC(aToken);
    CleanupStack::Pop(pSelf);
    return pSelf;   
}
CTokenHolder* CTokenHolder::NewLC(CSenElement* aToken)
{
           CTokenHolder* pSelf = new (ELeave) CTokenHolder();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aToken);
    return pSelf; 
}
CTokenHolder::~CTokenHolder()
{
   if(iToken)
        delete iToken;
}

//check here what kind of token the element have and then  
//instantiate that TokenType
CTokenHolder::CTokenHolder()
:iToken(NULL)
{
    
}
void CTokenHolder::ConstructL(CSenElement* aToken)
{
     
    CSenElement* AND = CXmlHelper::RipPolicyXORElementL(aToken);
     

    RPointerArray<CSenElement>& tokenChildren = AND->ElementsL();

    CSenElement* child;
    for (TInt i = 0; i < tokenChildren.Count(); i++)  
    { //There should be only one element here the TokenType
        child = tokenChildren[i];

        TPtrC8 localName = child->LocalName();
        
        if(localName.Compare(KUsernameToken) == 0)
        {
            iTokenType.Set(KUsernameToken);
            iToken = CUsernameToken::NewL(child);
        }
        else if(localName.Compare(KIssuedToken) == 0)
        {
            iTokenType.Set(KIssuedToken);
            iToken = CIssuedToken::NewL(child);
        }
        else if(localName.Compare(KSecureConversationToken) == 0)
        {
            iTokenType.Set(KSecureConversationToken);
            iToken = CSecureConversationToken::NewL(child);
        }
        else if(localName.Compare(KSamlToken) == 0)
        {
            iTokenType.Set(KSamlToken);
            iToken = CSamlToken::NewL(child);
        }
        else if(localName.Compare(KHttpsToken) == 0)
        {
            iTokenType.Set(KHttpsToken);
            iToken = CHttpsToken::NewL(child);
        }
        else
        {
            //errror here
        }

    }
}

CWss10* CWss10::NewL(CSenElement* aAssertion)
{
    CWss10* pSelf = CWss10::NewLC(aAssertion);
    CleanupStack::Pop(pSelf);
    return pSelf;
   
}
CWss10* CWss10::NewLC(CSenElement* aAssertion)
{
    CWss10* pSelf = new (ELeave) CWss10();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aAssertion);
    return pSelf;
}
CWss10::~CWss10()
{
    
}
void CWss10::ConstructL(CSenElement* aAssertion)
{

    BaseConstructL(aAssertion);
}

CWss10::CWss10()
:iMustSupportRefKeyIdentifier(EFalse),
iMustSupportRefIssuerSerial(EFalse),
iMustSupportRefExternalURI(EFalse),
iMustSupportRefEmbeddedToken(EFalse)
{

}

TBool CWss10::IsEqualWss10Assertion(CWss10* aAssertion)
{
   if 
    (
    (MustSupportRefKeyIdentifier() == aAssertion->MustSupportRefKeyIdentifier()) &&
    (MustSupportRefIssuerSerial() == aAssertion->MustSupportRefIssuerSerial()) &&
    (MustSupportRefExternalURI() == aAssertion->MustSupportRefExternalURI()) &&
    (MustSupportRefEmbeddedToken() == aAssertion->MustSupportRefEmbeddedToken())
    )    
        return ETrue;
    else
        return EFalse;
  
}

TBool CWss10::MustSupportRefKeyIdentifier()
{
    return iMustSupportRefKeyIdentifier;    
}
TBool CWss10::MustSupportRefIssuerSerial()
{
    return iMustSupportRefIssuerSerial;
}
TBool CWss10::MustSupportRefExternalURI()
{
    return iMustSupportRefExternalURI;
}
TBool CWss10::MustSupportRefEmbeddedToken()
{
    return iMustSupportRefEmbeddedToken;
}

void CWss10::BaseConstructL(CSenElement* aAssertion)
{
     
    CSenElement* AND = CXmlHelper::RipPolicyXORElementL(aAssertion);
     

    RPointerArray<CSenElement>& wss10Children = AND->ElementsL();
    CSenElement* child = NULL;

    for (TInt i = 0; i<wss10Children.Count(); i++)
    {
        child = wss10Children[i];
        
        TPtrC8 localName = child->LocalName();

       if(localName.Compare(KMustSupportRefKeyIdentifier) == 0)
        {
            iMustSupportRefKeyIdentifier = ETrue;
        }
        else if(localName.Compare(KMustSupportRefIssuerSerial) == 0)
        {
            iMustSupportRefIssuerSerial = ETrue;
        }
        else if(localName.Compare(KMustSupportRefExternalURI) == 0)
        {
            iMustSupportRefExternalURI = ETrue;
        }
        else if(localName.Compare(KMustSupportRefEmbeddedToken) == 0)
        {
            iMustSupportRefEmbeddedToken = ETrue;
        }
        else
        {
            //do something    
        }
    }
}


CWss11* CWss11::NewL(CSenElement* aAssertion)
{
    CWss11* pSelf = CWss11::NewLC(aAssertion);
    CleanupStack::Pop(pSelf);
    return pSelf;
    
}
CWss11* CWss11::NewLC(CSenElement* aAssertion)
{
    CWss11* pSelf = new (ELeave) CWss11();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aAssertion);
    return pSelf;
   
}
CWss11::~CWss11()
{
    
}

TBool CWss11::IsEqualWss11Assertion(CWss11* aAssertion)
{
    if 
    ((MustSupportRefThumbprint() == aAssertion->MustSupportRefThumbprint()) &&
    (MustSupportRefEncryptedKey() == aAssertion->MustSupportRefEncryptedKey()) &&
    (RequireSignatureConfirmation() == aAssertion->RequireSignatureConfirmation()) &&
    (MustSupportRefKeyIdentifier() == aAssertion->MustSupportRefKeyIdentifier()) &&
    (MustSupportRefIssuerSerial() == aAssertion->MustSupportRefIssuerSerial()) &&
    (MustSupportRefExternalURI() == aAssertion->MustSupportRefExternalURI()) &&
    (MustSupportRefEmbeddedToken() == aAssertion->MustSupportRefEmbeddedToken())
    
    )    
        return ETrue;
    else
        return EFalse;
    
}

TBool CWss11::MustSupportRefThumbprint()
{
    return  iMustSupportRefThumbprint;    
}
TBool CWss11::MustSupportRefEncryptedKey()
{
    return  iMustSupportRefEncryptedKey;
}
TBool CWss11::RequireSignatureConfirmation()
{
    return  iRequireSignatureConfirmation;
}
void CWss11::ConstructL(CSenElement* aAssertion)
{   
    BaseConstructL(aAssertion);
    
     
    CSenElement* AND = CXmlHelper::RipPolicyXORElementL(aAssertion);
     

    RPointerArray<CSenElement>& wss11Children = AND->ElementsL();
    CSenElement* child;
    TInt count = wss11Children.Count();

    for (TInt i = 0; i<count; i++)
    {
        child = wss11Children[i];
        
        TPtrC8 localName = child->LocalName();
        
       if(localName.Compare(KMustSupportRefThumbprint) == 0)
        {
            iMustSupportRefThumbprint = ETrue;
        }
        else if(localName.Compare(KMustSupportRefEncryptedKey) == 0)
        {
            iMustSupportRefEncryptedKey = ETrue;
        }
        else if(localName.Compare(KRequireSignatureConfirmation) == 0)
        {
            iRequireSignatureConfirmation = ETrue;
        }
        else
        {
            //do something    
        }
    }
}
CWss11::CWss11()
:iMustSupportRefThumbprint(EFalse),
iMustSupportRefEncryptedKey(EFalse),
iRequireSignatureConfirmation(EFalse)
{
    
}



CWsTrust10* CWsTrust10::NewL(CSenElement* aAssertion)
{
    CWsTrust10* pSelf = CWsTrust10::NewLC(aAssertion);
    CleanupStack::Pop(pSelf);
    return pSelf;
   
}
CWsTrust10* CWsTrust10::NewLC(CSenElement* aAssertion)
{
    CWsTrust10* pSelf = new (ELeave) CWsTrust10();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aAssertion);
    return pSelf;
}
CWsTrust10::~CWsTrust10()
{
    
}

TBool CWsTrust10::IsEqualWsTrust10Assertion(CWsTrust10* aAssertion)
{
    if  (
        (MustSupportClientChallenge() == aAssertion->MustSupportClientChallenge()) &&
        (MustSupportServerChallenge() == aAssertion->MustSupportServerChallenge()) &&
        (RequireClientEntropy() == aAssertion->RequireClientEntropy()) &&
        (RequireServerEntropy() == aAssertion->RequireServerEntropy()) &&
        (MustSupportIssuedTokens() == aAssertion->MustSupportIssuedTokens())
        )
        return ETrue;
    else
        return EFalse;
}
/*
_LIT8(KTrust10, "Trust10");
*/    
TBool CWsTrust10::MustSupportClientChallenge()
{
    return   iMustSupportClientChallenge;  
}
TBool CWsTrust10::MustSupportServerChallenge()
{
    return iMustSupportServerChallenge;
}
TBool CWsTrust10::RequireClientEntropy()
{
    return iRequireClientEntropy;
}
TBool CWsTrust10::RequireServerEntropy()
{
    return iRequireServerEntropy;
}
TBool CWsTrust10::MustSupportIssuedTokens()
{
    return iMustSupportIssuedTokens;
}



void CWsTrust10::ConstructL(CSenElement* aAssertion)
{
     
    CSenElement* AND = CXmlHelper::RipPolicyXORElementL(aAssertion);
     

    RPointerArray<CSenElement>& wsTrustChildren = AND->ElementsL();
    
    CSenElement* child;
    TInt count = wsTrustChildren.Count();

    for (TInt i = 0; i<count; i++)
    {
        child = wsTrustChildren[i];
        
        TPtrC8 localName = child->LocalName();

        if(localName.Compare(KMustSupportClientChallenge) == 0)
        {
            iMustSupportClientChallenge = ETrue;
        }
        else if(localName.Compare(KMustSupportServerChallenge) == 0)
        {
            iMustSupportServerChallenge = ETrue;
        }
        else if(localName.Compare(KRequireClientEntropy) == 0)
        {
            iRequireClientEntropy = ETrue;
        }
        else if(localName.Compare(KRequireServerEntropy) == 0)
        {
            iRequireServerEntropy = ETrue;
        }
        else if(localName.Compare(KMustSupportIssuedTokens) == 0)
        {
            iMustSupportIssuedTokens  = ETrue;  
        }
        else
        {
            //errror here
        }
    }
}
CWsTrust10::CWsTrust10()
:iMustSupportClientChallenge(EFalse),
iMustSupportServerChallenge(EFalse),
iRequireClientEntropy(EFalse),
iRequireServerEntropy(EFalse),
iMustSupportIssuedTokens(EFalse)
{
    
}


///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
/*
CXmlHelper::CXmlHelper()
    {
    }
    
CXmlHelper::~CXmlHelper()
    {
    }
*/

TPtrC8 CXmlHelper::AttributeValueL(CSenElement* aAssertion, const TDesC8& aAttribName)
{
    if(aAssertion == NULL) { User::Leave(KErrArgument); }
    
    RPointerArray<CSenBaseAttribute>& attrs = aAssertion->AttributesL();
    
    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
        bs = (attrs)[j];
        if(bs)
        {
            if(bs->Name().Compare(aAttribName) == 0)
            {
                 return bs->Value();
            }
         }

            
        }
    return KNullDesC8();
    
}
TBool CXmlHelper::HasAttributeL(CSenElement* aAssertion, const TDesC8& aAttribName)
{
    if(aAssertion == NULL) { User::Leave(KErrArgument); }
    
    RPointerArray<CSenBaseAttribute>& attrs = aAssertion->AttributesL();
    
    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
        bs = (attrs)[j];
        if(bs)
        {
            if(bs->Name().Compare(aAttribName) == 0)
            {
                 return ETrue;
            }
         }

            
        }
    return EFalse;
    
}
CSenElement* CXmlHelper::RipPolicyXORElementL(CSenElement* aElement)
{

    RPointerArray<CSenElement>& pPolicyList = aElement->ElementsL();

    CSenElement* XOR = NULL;
    CSenElement* AND = NULL;
    if(pPolicyList.Count() > 0)
    {
        CSenElement* policy = pPolicyList[0];
        TPtrC8 policyName = policy->LocalName();
        
        RPointerArray<CSenElement>& policyXORChild = policy->ElementsL(); //find the policy/XOR element
        
        if(policyXORChild.Count() > 0)
        {
            XOR = policyXORChild[0];
            
            TPtrC8 xorName = XOR->LocalName();

            RPointerArray<CSenElement>& policyANDChild = XOR->ElementsL(); //find the policy/XOR/AND element
            
            if(policyANDChild.Count() > 0)
            {
                AND = policyANDChild[0];
                TPtrC8 andName = AND->LocalName();
                return AND;

            }
        }

    }

    return NULL;
}


///////////////////////////////////////////////////////////////////
// CLayout
///////////////////////////////////////////////////////////////////

CLayout* CLayout::NewL(CSenElement* aLayout)
{
    CLayout* pSelf = CLayout::NewLC(aLayout);
    CleanupStack::Pop(pSelf);
    return pSelf;
}
CLayout* CLayout::NewLC(CSenElement* aLayout)
{
        CLayout* pSelf = new (ELeave) CLayout();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aLayout);
    return pSelf;
}
CLayout::~CLayout()
{

}
void CLayout::ConstructL(CSenElement* aLayout)
{ 
     
    CSenElement* AND = CXmlHelper::RipPolicyXORElementL(aLayout);
     

    RPointerArray<CSenElement>& layoutChildren = AND->ElementsL();
    CleanupClosePushL(layoutChildren);
    CSenElement* child;
    TInt count = layoutChildren.Count();
    for (TInt i = 0; i<count; i++)
    {
        child = layoutChildren[i];
        
        TPtrC8 localName = child->LocalName();

        if(localName.Compare(KStrict) == 0)
        {
            iLayout.Set(KStrict);
        }
        else if(localName.Compare(KLax) == 0)
        {
            iLayout.Set(KLax);   
        }
        else if(localName.Compare(KLaxTsFirst) == 0)
        {
            iLayout.Set(KLaxTsFirst);
        }
        else if(localName.Compare(KLaxTsLast) == 0)
        {
            iLayout.Set(KLaxTsLast);
        }
        else
        {
            //errror here
        }
    }
    CleanupStack::PopAndDestroy(&layoutChildren);
}

CLayout::CLayout()
:iLayout(KLax)
{

}

TBool CLayout::IsEqualLayout(CLayout* aLayout)
{
    if (iLayout.Compare(aLayout->Layout()) == 0)
        return ETrue;
    else
        return EFalse;
}
///////////////////////////////////////////////////////////////////
//BINDING, SYMASYMMBINDING, ASYMMETRICBINDING, SYMMETRICBINDING
///////////////////////////////////////////////////////////////////

CBinding* CBinding::NewL(CSenElement* aBinding)
{
    CBinding* pSelf = CBinding::NewLC(aBinding);
    CleanupStack::Pop(pSelf);
    return pSelf;  
}
CBinding* CBinding::NewLC(CSenElement* aBinding)
{
    CBinding* pSelf = new (ELeave) CBinding();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aBinding);
    return pSelf;
}
CBinding::~CBinding()
{
    delete iAlgorithmSuite;
    delete iLayout; 
}

void CBinding::SetbaseElementL(CSenElement* aBinding)
{
    RPointerArray<CSenElement>& symmBindingChildren = aBinding->ElementsL();
    TInt count = symmBindingChildren.Count();
    CSenElement* child = NULL;
    for (TInt i = 0; i<count; i++)
    {
        child = symmBindingChildren[i];
        
        TPtrC8 localName = child->LocalName();
        
        if(localName.Compare(KAlgorithmSuite) == 0)
        {
            iAlgorithmSuite = CAlgorithmSuite::NewL(child);   
        }
        else if(localName.Compare(KLayout) == 0)
        {
            iLayout = CLayout::NewL(child);
        }
        else if(localName.Compare(KIncludeTimestamp) == 0)
        {
            iIncludeTimestamp = ETrue;
        }
        else
        {
            //do something    
        }
    }
}

CBinding::CBinding():
iAlgorithmSuite(NULL),
iLayout(NULL),
iIncludeTimestamp(EFalse)
{
    
}
void CBinding::ConstructL(CSenElement* /*aBinding*/)
{
    
}
/////////////////////////////////////////////////////////////
CSymmAsymmBinding* CSymmAsymmBinding::NewL(CSenElement* aBinding)
{
    CSymmAsymmBinding* pSelf = CSymmAsymmBinding::NewLC(aBinding);
    CleanupStack::Pop(pSelf);
    return pSelf;  

}
CSymmAsymmBinding* CSymmAsymmBinding::NewLC(CSenElement* aBinding)
{
    CSymmAsymmBinding* pSelf = new (ELeave) CSymmAsymmBinding();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aBinding);
    return pSelf;

}

CSymmAsymmBinding::~CSymmAsymmBinding()
{

}
void CSymmAsymmBinding::BaseConstructL(CSenElement* aBinding)
{

    SetbaseElementL(aBinding);

    RPointerArray<CSenElement>& symmBindingChildren = aBinding->ElementsL();

    CSenElement* child = NULL;

    for (TInt i = 0; i < symmBindingChildren.Count(); i++)
    {
        child = symmBindingChildren[i];
        
        TPtrC8 localName = child->LocalName();
        
        if(localName.Compare(KEncryptBeforeSigning) == 0)
        {
            iProtectionOrder.Set(KEncryptBeforeSigning);
        }
        else if(localName.Compare(KSignBeforeEncrypting) == 0 )
        {
            iProtectionOrder.Set(KSignBeforeEncrypting);
        }
        else if(localName.Compare(KEncryptSignature) == 0)
        {
             iEncryptSignature = ETrue;
        }
        else if(localName.Compare(KProtectTokens) == 0)
        {
            iProtectTokens = ETrue;
        }
        else if (localName.Compare(KOnlySignEntireHeadersAndBody) == 0)
        {
            iOnlySignEntireHeadersAndBody = ETrue;
        }
        else
        {
            //do something    
        }
    }

}

CSymmAsymmBinding::CSymmAsymmBinding()
:
iProtectionOrder(KSignBeforeEncrypting),
iEncryptSignature(EFalse),
iProtectTokens(EFalse),
iOnlySignEntireHeadersAndBody(EFalse)

{

}
void CSymmAsymmBinding::ConstructL(CSenElement* /*aBinding*/)
{
    
}
///////////////////////////////////////////////////////////
//CAsymmetricBinding
///////////////////////////////////////////////////////////
CAsymmetricBinding* CAsymmetricBinding::NewL(CSenElement* aAsymmBinding)
{
    CAsymmetricBinding* pSelf = CAsymmetricBinding::NewLC(aAsymmBinding);
    CleanupStack::Pop(pSelf);
    return pSelf;  
}
CAsymmetricBinding* CAsymmetricBinding::NewLC(CSenElement* aAsymmBinding)
{
    CAsymmetricBinding* pSelf = new (ELeave) CAsymmetricBinding();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aAsymmBinding);
    return pSelf;
}
CAsymmetricBinding::~CAsymmetricBinding()
{
    delete iInitiatorToken;
    delete iRecipientToken;
}
void CAsymmetricBinding::ConstructL(CSenElement* aAsymmBinding)
{//parse AsymmBinding and get AlgoSuite Element

    TPtrC8 localName = aAsymmBinding->LocalName();
   
     
    CSenElement* AND = CXmlHelper::RipPolicyXORElementL(aAsymmBinding);
     

    BaseConstructL(AND);        
    RPointerArray<CSenElement>& symmBindingChildren =  AND->ElementsL();
    
    CSenElement* child = NULL;

    for (TInt i = 0; i < symmBindingChildren.Count(); i++)
    {
        child = symmBindingChildren[i];
        
        TPtrC8 localName = child->LocalName();
        
       if(localName.Compare(KInitiatorToken) == 0)
        {
         iInitiatorToken = CTokenHolder::NewL(child);

        }
        else if(localName.Compare(KRecipientToken) == 0)
        {
         iRecipientToken = CTokenHolder::NewL(child);
        }
        else
        {
            //do something    
        }
    }
    CleanupStack::PopAndDestroy(&symmBindingChildren);

}

CAsymmetricBinding::CAsymmetricBinding()
{
/*
<sp:InitiatorToken> </sp:InitiatorToken>
<sp:RecipientToken> </sp:RecipientToken>
<sp:AlgorithmSuite ... > ... </sp:AlgorithmSuite>
<sp:Layout ... > ... </sp:Layout> ?
<sp:IncludeTimestamp ... /> ?
<sp:EncryptBeforeSigning ... /> ?
<sp:EncryptSignature ... /> ?
<sp:ProtectTokens ... /> ?
<sp:OnlySignEntireHeadersAndBody ... /> ?
*/
}

TBool CAsymmetricBinding::IsEqualAsymmtericAssertion(CAsymmetricBinding* aAsymmetricBinding)
{

   if(
    (GetAlgoSuite()->IsEqualAlgoSuite(aAsymmetricBinding->GetAlgoSuite())) && 
    (GetLayout()->IsEqualLayout(aAsymmetricBinding->GetLayout())) &&
    (IncludetimeStamp() == aAsymmetricBinding->IncludetimeStamp()) &&
    (EncryptSignature() == aAsymmetricBinding->EncryptSignature()) &&
    (EncryptBeforeSigning().Compare(aAsymmetricBinding->EncryptBeforeSigning()) == 0) &&
    (ProtectTokens() == aAsymmetricBinding->ProtectTokens()) &&
    (OnlySignEntireHeadersAndBody() == aAsymmetricBinding->OnlySignEntireHeadersAndBody()))
    {
        return ETrue;    
    }
        
    return EFalse;
        
}

///////////////////////////////////////////////////////////
//CSymmetricBinding
///////////////////////////////////////////////////////////
CSymmetricBinding* CSymmetricBinding::NewL(CSenElement* aSymmBinding)
{
    CSymmetricBinding* pSelf = CSymmetricBinding::NewLC(aSymmBinding);
    CleanupStack::Pop(pSelf);
    return pSelf;  
}
CSymmetricBinding* CSymmetricBinding::NewLC(CSenElement* aSymmBinding)
{
    CSymmetricBinding* pSelf = new (ELeave) CSymmetricBinding();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aSymmBinding);
    return pSelf;
}
CSymmetricBinding::~CSymmetricBinding()
{

   delete iEncriptionToken;
   delete iSignatureToken;
   delete iProtectionToken;
   
}
void CSymmetricBinding::ConstructL(CSenElement* aAsymmBinding)
{//parse AsymmBinding and get AlgoSuite Element

    TPtrC8 localName = aAsymmBinding->LocalName();
    CSenElement* AND = CXmlHelper::RipPolicyXORElementL(aAsymmBinding);
    RPointerArray<CSenElement>& symmBindingChildren = AND->ElementsL();
    BaseConstructL(AND);    
    CSenElement* child = NULL;

    for (TInt i = 0; i < symmBindingChildren.Count(); i++)
    {
        child = symmBindingChildren[i];
        
        TPtrC8 localName = child->LocalName();
        
       if(localName.Compare(KEncryptionToken) == 0)
        {
         iEncriptionToken = CTokenHolder::NewL(child);
        }
        else if(localName.Compare(KSignatureToken) == 0)
        {
         iSignatureToken = CTokenHolder::NewL(child);
        }
        else if(localName.Compare(KProtectionToken) == 0)
        {
         iProtectionToken = CTokenHolder::NewL(child);
        }
        else
        {
            //do something    
        }
    }
    

}

CSymmetricBinding::CSymmetricBinding()
{
/*
<sp:EncryptionToken> </sp:EncryptionToken>
<sp:SignatureToken> </sp:SignatureToken>
or
<sp:ProtectionToken> </sp:ProtectionToken>

<sp:AlgorithmSuite ... > ... </sp:AlgorithmSuite>
<sp:Layout ... > ... </sp:Layout> ?
<sp:IncludeTimestamp ... /> ?
<sp:EncryptBeforeSigning ... /> ?
<sp:EncryptSignature ... /> ?
<sp:ProtectTokens ... /> ?
<sp:OnlySignEntireHeadersAndBody ... /> ?
*/
}

TBool CSymmetricBinding::IsEqualSymmtericAssertion(CSymmetricBinding* aSymmetricBinding)
{

   if(
    (GetAlgoSuite()->IsEqualAlgoSuite(aSymmetricBinding->GetAlgoSuite())) && 
    (GetLayout()->IsEqualLayout(aSymmetricBinding->GetLayout())) &&
    (IncludetimeStamp() == aSymmetricBinding->IncludetimeStamp()) &&
    (EncryptSignature() == aSymmetricBinding->EncryptSignature()) &&
    (EncryptBeforeSigning().Compare(aSymmetricBinding->EncryptBeforeSigning()) == 0) &&
    (ProtectTokens() == aSymmetricBinding->ProtectTokens()) &&
    (OnlySignEntireHeadersAndBody() == aSymmetricBinding->OnlySignEntireHeadersAndBody()))
    {
        return ETrue;    
    }
        
    return EFalse;
        
}

///////////////////////////////////////////////////////////
//CTranportBinding
///////////////////////////////////////////////////////////

CTranportBinding* CTranportBinding::NewL(CSenElement* aTransportBinding)
{
    CTranportBinding* pSelf = CTranportBinding::NewLC(aTransportBinding);
    CleanupStack::Pop(pSelf);
    return pSelf;  
    
}
CTranportBinding* CTranportBinding::NewLC(CSenElement* aTransportBinding)
{
    CTranportBinding* pSelf = new (ELeave) CTranportBinding();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aTransportBinding);
    return pSelf;
    
}
CTranportBinding::~CTranportBinding()
{
  delete iTransportToken;  
}
  
TBool CTranportBinding::IsEqualTransportBindingAssertion(CTranportBinding* aTransportBinding)
{
    if(
    (GetAlgoSuite()->IsEqualAlgoSuite(aTransportBinding->GetAlgoSuite())) && 
    (GetLayout()->IsEqualLayout(aTransportBinding->GetLayout())) &&
    (IncludetimeStamp() == aTransportBinding->IncludetimeStamp()))
    {
        return ETrue;    
    }
        
    return EFalse;
  
}
void CTranportBinding::ConstructL(CSenElement* aTransportBinding)
{
    TPtrC8 localName = aTransportBinding->LocalName();
   
     
    CSenElement* AND = CXmlHelper::RipPolicyXORElementL(aTransportBinding);
     
    
    SetbaseElementL(AND);    

    RPointerArray<CSenElement>& bindingChildren = AND->ElementsL();
    
    CSenElement* child = NULL;

    for (TInt i = 0; i < bindingChildren.Count(); i++)
    {
        child = bindingChildren[i];
        
        TPtrC8 localName = child->LocalName();
        
       if(localName.Compare(KTransportToken) == 0)
        {
         iTransportToken = CTokenHolder::NewL(child);
        }
        else
        {
            //do something    
        }
    }

}
CTranportBinding::CTranportBinding()
{
    
}

//   CTokenHolder* iTransportToken;
   

///////////////////////////////////////////////////////////
//CAlgorithmSuite
///////////////////////////////////////////////////////////

CAlgorithmSuite* CAlgorithmSuite::NewL(CSenElement* aAlgoSuite)
{
    CAlgorithmSuite* pSelf = CAlgorithmSuite::NewLC(aAlgoSuite);
    CleanupStack::Pop(pSelf);
    return pSelf;  
}  

CAlgorithmSuite* CAlgorithmSuite::NewLC(CSenElement* aAlgoSuite)
{
    CAlgorithmSuite* pSelf = new (ELeave) CAlgorithmSuite();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aAlgoSuite);
    return pSelf;
}

CAlgorithmSuite::~CAlgorithmSuite()
{
    delete iAlgoSuite;
    delete iSymmetricSignature;
    delete iAsymmetricSignature;
    delete iComputedKey;
    delete iDigest;
    delete iEncryption;
    delete iSymmetricKeyWrap;
    delete iAsymmetricKeyWrap;
    delete iEncryptionKeyDerivation;
    delete iSignatureKeyDerivation;
    delete iC14n; 
    delete iSoapNormalization;
    delete iStrTransform;
    delete iXPath;
    delete iXPathFilter20;
      
}
void CAlgorithmSuite::ConstructL(CSenElement* aAlgoSuite)
{
     
    CSenElement* AND = CXmlHelper::RipPolicyXORElementL(aAlgoSuite);
     

    RPointerArray<CSenElement>& algoChildren = AND->ElementsL();; //find the policy element
    CSenElement* child = NULL;
    for (TInt i = 0; i < algoChildren.Count(); i++)
    {   //should be only one element
        child = algoChildren[i];
        
        TPtrC8 localName = child->LocalName();
        iAlgoSuite = localName.AllocL();
        SetAlgoSuiteL(localName);
        
    }
  
}
CAlgorithmSuite::CAlgorithmSuite()
:
iAlgoSuite(KNullDesC8().AllocL()),
iSymmetricSignature(KAlgoHMAC_SHA1().AllocL()),
iAsymmetricSignature(KAlgoRSA_SHA1().AllocL()),
iComputedKey(KAlgoP_SHA1().AllocL()),
iMaximumSymmetricKeyLength(256),
iMinimumAsymmetricKeyLength(1024),
iMaximumAsymmetricKeyLength(4096),
iDigest(KNullDesC8().AllocL()),
iEncryption(KNullDesC8().AllocL()),
iSymmetricKeyWrap(KNullDesC8().AllocL()),
iAsymmetricKeyWrap(KNullDesC8().AllocL()),
iEncryptionKeyDerivation(KNullDesC8().AllocL()),
iSignatureKeyDerivation(KNullDesC8().AllocL()),
iMinimumSymmetricKeyLength(0),
iC14n(KAlgoEX_C14N().AllocL()),
iSoapNormalization(KNullDesC8().AllocL()),
iStrTransform(KNullDesC8().AllocL()),
iXPath(KNullDesC8().AllocL()),
iXPathFilter20(KNullDesC8().AllocL())

{
    
}
TBool CAlgorithmSuite::IsEqualAlgoSuite(CAlgorithmSuite* aAlgoSuite)
    {
    if (AlgoSuite().Compare(aAlgoSuite->AlgoSuite()) == 0)
        return ETrue;
    else
        return EFalse;
    }

void CAlgorithmSuite::SetAlgoSuiteL(TDesC8& aSuiteName)
    {
    if(aSuiteName.Compare(KBasic256) == 0) 
        {
        iDigest = KAlgoSHA1().AllocL();
        iEncryption = KAlgoAES256().AllocL();
        iSymmetricKeyWrap = KAlgoKW_AES256().AllocL();
        iAsymmetricKeyWrap = KAlgoKW_RSA_OAEP().AllocL();
        iEncryptionKeyDerivation = KAlgoP_SHA1_L256().AllocL();
        iSignatureKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iMinimumSymmetricKeyLength = 256;
        } 
    else if (aSuiteName.Compare(KBasic192) == 0) 
        {
        iDigest = KAlgoSHA1().AllocL();
        iEncryption = KAlgoAES192().AllocL();
        iSymmetricKeyWrap = KAlgoKW_AES192().AllocL();
        iAsymmetricKeyWrap = KAlgoKW_RSA_OAEP().AllocL();
        iEncryptionKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iSignatureKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iMinimumSymmetricKeyLength = 192;
        }
    else if (aSuiteName.Compare(KBasic128) == 0)
        {
        iDigest = KAlgoSHA1().AllocL();
        iEncryption = KAlgoAES128().AllocL();
        iSymmetricKeyWrap = KAlgoKW_AES128().AllocL();
        iAsymmetricKeyWrap = KAlgoKW_RSA_OAEP().AllocL();
        iEncryptionKeyDerivation = KAlgoP_SHA1_L128().AllocL();
        iSignatureKeyDerivation = KAlgoP_SHA1_L128().AllocL();
        iMinimumSymmetricKeyLength = 128;
        } 
    else if (aSuiteName.Compare(KTripleDes) == 0) 
        {
        iDigest = KAlgoSHA1().AllocL();
        iEncryption = KAlgoTRIPLE_DES().AllocL();
        iSymmetricKeyWrap = KAlgoKW_TRIPLE_DES().AllocL();
        iAsymmetricKeyWrap = KAlgoKW_RSA_OAEP().AllocL();
        iEncryptionKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iSignatureKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iMinimumSymmetricKeyLength = 192;
        } 
    else if (aSuiteName.Compare(KBasic256Rsa15) == 0)
        {
        iDigest = KAlgoSHA1().AllocL();
        iEncryption = KAlgoAES256().AllocL();
        iSymmetricKeyWrap = KAlgoKW_AES256().AllocL();
        iAsymmetricKeyWrap = KAlgoKW_RSA15().AllocL();
        iEncryptionKeyDerivation = KAlgoP_SHA1_L256().AllocL();
        iSignatureKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iMinimumSymmetricKeyLength = 256;
        } 
    else if (aSuiteName.Compare(KBasic192Rsa15) == 0) 
        {
        iDigest = KAlgoSHA1().AllocL();
        iEncryption = KAlgoAES192().AllocL();
        iSymmetricKeyWrap = KAlgoKW_AES192().AllocL();
        iAsymmetricKeyWrap = KAlgoKW_RSA15().AllocL();
        iEncryptionKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iSignatureKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iMinimumSymmetricKeyLength = 192;
        } 
    else if (aSuiteName.Compare(KBasic128Rsa15) == 0) 
        {
        iDigest = KAlgoSHA1().AllocL();
        iEncryption = KAlgoAES128().AllocL();
        iSymmetricKeyWrap = KAlgoKW_AES128().AllocL();
        iAsymmetricKeyWrap = KAlgoKW_RSA15().AllocL();
        iEncryptionKeyDerivation = KAlgoP_SHA1_L128().AllocL();
        iSignatureKeyDerivation = KAlgoP_SHA1_L128().AllocL();
        iMinimumSymmetricKeyLength = 128;
        } 
    else if (aSuiteName.Compare(KTripleDesRsa15) == 0)
        {
        iDigest = KAlgoSHA1().AllocL();
        iEncryption = KAlgoTRIPLE_DES().AllocL();
        iSymmetricKeyWrap = KAlgoKW_TRIPLE_DES().AllocL();
        iAsymmetricKeyWrap = KAlgoKW_RSA15().AllocL();
        iEncryptionKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iSignatureKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iMinimumSymmetricKeyLength = 192;
        }
    else if(aSuiteName.Compare(KBasic256Sha256) == 0) 
        {
        iDigest = KAlgoSHA256().AllocL();
        iEncryption = KAlgoAES256().AllocL();
        iSymmetricKeyWrap = KAlgoKW_AES256().AllocL();
        iAsymmetricKeyWrap = KAlgoKW_RSA_OAEP().AllocL();
        iEncryptionKeyDerivation = KAlgoP_SHA1_L256().AllocL();
        iSignatureKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iMinimumSymmetricKeyLength = 256;
        }
    else if (aSuiteName.Compare(KBasic192Sha256) == 0)
        {
        iDigest = KAlgoSHA256().AllocL();
        iEncryption = KAlgoAES192().AllocL();
        iSymmetricKeyWrap = KAlgoKW_AES192().AllocL();
        iAsymmetricKeyWrap = KAlgoKW_RSA_OAEP().AllocL();
        iEncryptionKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iSignatureKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iMinimumSymmetricKeyLength = 192;
        } 
    else if (aSuiteName.Compare(KBasic128Sha256) == 0)
        {
        iDigest = KAlgoSHA256().AllocL();
        iEncryption = KAlgoAES128().AllocL();
        iSymmetricKeyWrap = KAlgoKW_AES128().AllocL();
        iAsymmetricKeyWrap = KAlgoKW_RSA_OAEP().AllocL();
        iEncryptionKeyDerivation = KAlgoP_SHA1_L128().AllocL();
        iSignatureKeyDerivation = KAlgoP_SHA1_L128().AllocL();
        iMinimumSymmetricKeyLength = 128;
        } 
    else if(aSuiteName.Compare(KTripleDesSha256) == 0) 
        {
        iDigest = KAlgoSHA256().AllocL();
        iEncryption = KAlgoTRIPLE_DES().AllocL();
        iSymmetricKeyWrap = KAlgoKW_TRIPLE_DES().AllocL();
        iAsymmetricKeyWrap = KAlgoKW_RSA_OAEP().AllocL();
        iEncryptionKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iSignatureKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iMinimumSymmetricKeyLength = 192;
        }  
    else if (aSuiteName.Compare(KBasic256Sha256Rsa15) == 0) 
        {
        iDigest = KAlgoSHA256().AllocL();
        iEncryption = KAlgoAES256().AllocL();
        iSymmetricKeyWrap = KAlgoKW_AES256().AllocL();
        iAsymmetricKeyWrap = KAlgoKW_RSA15().AllocL();
        iEncryptionKeyDerivation = KAlgoP_SHA1_L256().AllocL();
        iSignatureKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iMinimumSymmetricKeyLength = 256;
        } 
    else if (aSuiteName.Compare(KBasic192Sha256Rsa15) == 0) 
        {
        iDigest = KAlgoSHA256().AllocL();
        iEncryption = KAlgoAES192().AllocL();
        iSymmetricKeyWrap = KAlgoKW_AES192().AllocL();
        iAsymmetricKeyWrap = KAlgoKW_RSA15().AllocL();
        iEncryptionKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iSignatureKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iMinimumSymmetricKeyLength = 192;
        } 
    else if (aSuiteName.Compare(KBasic128Sha256Rsa15) == 0) 
        {
        iDigest = KAlgoSHA256().AllocL();
        iEncryption = KAlgoAES128().AllocL();
        iSymmetricKeyWrap = KAlgoKW_AES128().AllocL();
        iAsymmetricKeyWrap = KAlgoKW_RSA15().AllocL();
        iEncryptionKeyDerivation = KAlgoP_SHA1_L128().AllocL();
        iSignatureKeyDerivation = KAlgoP_SHA1_L128().AllocL();
        iMinimumSymmetricKeyLength = 128;
        } 
    else if (aSuiteName.Compare(KTripleDesSha256Rsa15) == 0) 
        {
        iDigest = KAlgoSHA256().AllocL();
        iEncryption = KAlgoTRIPLE_DES().AllocL();
        iSymmetricKeyWrap = KAlgoKW_TRIPLE_DES().AllocL();
        iAsymmetricKeyWrap = KAlgoKW_RSA15().AllocL();
        iEncryptionKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iSignatureKeyDerivation = KAlgoP_SHA1_L192().AllocL();
        iMinimumSymmetricKeyLength = 192;
        } 
    //else {}
    }

TPtrC8 CAlgorithmSuite::AlgoSuite()
    {
    return iAlgoSuite->Des();    
    }

TPtrC8 CAlgorithmSuite::SymmetricSignature()
    {
    return iSymmetricSignature->Des();
    }

TPtrC8 CAlgorithmSuite::AsymmetricSignature()
    {
    return iAsymmetricSignature->Des();
    }

TPtrC8 CAlgorithmSuite::ComputedKey()
    {
    return iComputedKey->Des();
    }

TInt CAlgorithmSuite::MaximumSymmetricKeyLength()
    {
    return iMaximumSymmetricKeyLength;
    }

TInt CAlgorithmSuite::MinimumAsymmetricKeyLength()
    {
    return iMinimumAsymmetricKeyLength;
    }

TInt CAlgorithmSuite::MaximumAsymmetricKeyLength()
    {
    return iMaximumAsymmetricKeyLength;
    }

TPtrC8 CAlgorithmSuite::Digest()
    {
    return iDigest->Des();
    }

TPtrC8 CAlgorithmSuite::Encryption()
    {
    return iEncryption->Des();
    }

TPtrC8 CAlgorithmSuite::SymmetricKeyWrap()
    {
    return iSymmetricKeyWrap->Des();
    }

TPtrC8 CAlgorithmSuite::AsymmetricKeyWrap()
    {
    return iAsymmetricKeyWrap->Des();    
    }

TPtrC8 CAlgorithmSuite::EncryptionKeyDerivation()
    {
    return iEncryptionKeyDerivation->Des();
    }

TPtrC8 CAlgorithmSuite::SignatureKeyDerivation()
    {
    return iSignatureKeyDerivation->Des();
    }
    
TInt CAlgorithmSuite::MinimumSymmetricKeyLength()
    {
    return iMinimumSymmetricKeyLength;
    }

TPtrC8 CAlgorithmSuite::C14n()
    {
    return iC14n->Des();
    }
TPtrC8 CAlgorithmSuite::SoapNormalization()
{
    return iSoapNormalization->Des();
}
TPtrC8 CAlgorithmSuite::StrTransform()
    {
    return iStrTransform->Des();
    }
    
TPtrC8 CAlgorithmSuite::XPath()
    {
    return iXPath->Des();
    }
    
TPtrC8 CAlgorithmSuite::XPathFilter20()
    {
    return iXPathFilter20->Des();
    }

void CAlgorithmSuite::SetC14nL(TDesC8& aValue)
    {
    if(iC14n)
        {
        delete iC14n;
        iC14n = NULL;
        }
    iC14n = aValue.AllocL();
    }

void CAlgorithmSuite::SetSoapNormalizationL(TDesC8& aValue)
    {
    if(iSoapNormalization)
        delete iSoapNormalization;
    iSoapNormalization = NULL;
    iSoapNormalization = aValue.AllocL();
    }
    
void CAlgorithmSuite::SetStrTransformL(TDesC8& aValue)
    {
    if(iStrTransform)
        delete iStrTransform;
    iStrTransform = NULL;
    iStrTransform = aValue.AllocL();
    }
    
void CAlgorithmSuite::SetXPathL(TDesC8& aValue)
    {
    if(iXPath)
        delete iXPath;
    iXPath = NULL;
    iXPath = aValue.AllocL();
    }
    
void CAlgorithmSuite::SetXPathFilter20L(TDesC8& aValue)
    {
    if(iXPathFilter20)
        delete iXPathFilter20;
    iXPathFilter20 = NULL;
    iXPathFilter20 = aValue.AllocL();
    }

///////////////////////////////////////////////////////////////////////
//////    CEncryptedSignedParts                                              
///////////////////////////////////////////////////////////////////////
CEncryptedSignedParts* CEncryptedSignedParts::NewL(CSenElement* aAssertion)
    {
    CEncryptedSignedParts* pSelf = CEncryptedSignedParts::NewLC(aAssertion);
    CleanupStack::Pop(pSelf);
    return pSelf;  
    }

CEncryptedSignedParts* CEncryptedSignedParts::NewLC(CSenElement* aAssertion)
    {
    CEncryptedSignedParts* pSelf = new (ELeave) CEncryptedSignedParts();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aAssertion);
    return pSelf;  
    }
    
CEncryptedSignedParts::~CEncryptedSignedParts()
    {
    iHeaders.Close();
    }

TBool CEncryptedSignedParts::IsEqualEncryptedSignedParts(CEncryptedSignedParts* aAssertion)
    {
    
    if(Body() == aAssertion->Body() && Header() == aAssertion->Header())
        {
        return ETrue;    
        }
    return EFalse;
    }

TBool CEncryptedSignedParts::Header()
    {
    return iHeader;    
    }
    
TBool CEncryptedSignedParts::Body()
    {
    return iBody;
    }
    
void CEncryptedSignedParts::ConstructL(CSenElement* aAssertion)
    {
    //No need to rip off caz there wont be any policy elements here
    RPointerArray<CSenElement>& tokenChildren =   aAssertion->ElementsL();
    if(tokenChildren.Count() < 0)
        {
        iHeader = ETrue;
        iBody = ETrue;
        return;
        }
    CSenElement* child;
    for (TInt i = 0; i < tokenChildren.Count(); i++)
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
            SoapHeader header;
            if(CXmlHelper::HasAttributeL(aAssertion, KName()))
                {
                TPtrC8 headerName  = CXmlHelper::AttributeValueL(aAssertion, KName);
                header.iName.Set(headerName);
                }
            if(CXmlHelper::HasAttributeL(aAssertion, KNamespace()))
                {
                TPtrC8 nsUri  = CXmlHelper::AttributeValueL(aAssertion, KNamespace());
                header.iNsUri.Set(nsUri);
                }           
           iHeaders.Append(header);
            }
        } 
    }
    
CEncryptedSignedParts::CEncryptedSignedParts()
:   iHeader(EFalse),
    iBody(EFalse)
    {
    }
    
void CEncryptedSignedParts::ElementsL( RPointerArray<HBufC8>& aElements )
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
    
///////////////////////////////////////////////////////////////////////
//////    CEncryptedSignedElements                                              
///////////////////////////////////////////////////////////////////////
CEncryptedSignedElements* CEncryptedSignedElements::NewL(CSenElement* aAssertion)
    {
    CEncryptedSignedElements* pSelf = CEncryptedSignedElements::NewLC(aAssertion);
    CleanupStack::Pop(pSelf);
    return pSelf;  
    } 

CEncryptedSignedElements* CEncryptedSignedElements::NewLC(CSenElement* aAssertion)
    {
    CEncryptedSignedElements* pSelf = new (ELeave) CEncryptedSignedElements();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL(aAssertion);
    return pSelf;  
    }

CEncryptedSignedElements::~CEncryptedSignedElements()
    {
    iXpaths.Close(); 
    delete iXPathVersion; 
    }

TBool CEncryptedSignedElements::IsEqualEncryptedSignedElements(CEncryptedSignedElements* aAssertion)
    {
    
    if( XPathVersion().Compare(aAssertion->XPathVersion()) == 0 )
        {
        return ETrue;         
        }
    return EFalse;        
    }

void CEncryptedSignedElements::ConstructL(CSenElement* aAssertion)
    {
    //No need to rip off caz there wont be any policy elements here
    if(CXmlHelper::HasAttributeL(aAssertion, KXPathVersion()))
        {
        TPtrC8 xpathVersion  = CXmlHelper::AttributeValueL(aAssertion,KXPathVersion());
        iXPathVersion = xpathVersion.AllocL();
        }
    RPointerArray<CSenElement> tokenChildren;
    CleanupClosePushL(tokenChildren);
    aAssertion->ElementsL(tokenChildren, KWsSecurityPolicyNsUri, KXPath);
    CSenElement* child;
    TInt count = tokenChildren.Count();
    for (TInt i = 0; i < count; i++)
        { 
        child = tokenChildren[i];
        TPtrC8 localName = child->LocalName();
        if(child->HasContent())
            {
            TPtrC8 content = child->Content();
            //check if xpath is ns qualified
            TBool result = IfNsQualified(aAssertion, content);
            
            if(result)
                {
                iXpaths.Append(content);
                }
            }
        } 
    CleanupStack::PopAndDestroy(&tokenChildren);
    }
    
CEncryptedSignedElements::CEncryptedSignedElements()
:iXPathVersion(KNullDesC8().AllocL())
    {
    }
    
TInt CEncryptedSignedElements::GetXPaths(RArray<TPtrC8>& aXpaths)
    {
    RArray<TPtrC8> xpaths(iXpaths);
    aXpaths =  xpaths;
    return KErrNone;
    }
    
TBool CEncryptedSignedElements::MergedElements(CEncryptedSignedElements* aAssertion, RArray<TPtrC8>& aResult)
    {
    if( XPathVersion().Compare(aAssertion->XPathVersion()) == 0 )
        {
        RArray<TPtrC8> xpath1;
        GetXPaths(xpath1);
        RArray<TPtrC8> xpath2;
        aAssertion->GetXPaths(xpath2);
        
        TInt count = xpath1.Count(); 
        TInt count2 = xpath2.Count(); 
        
        if(count == 0 || count2 == 0) //we have to sign/encrypt ALL Headers+Body
            {
            return ETrue;
            }
        
        for (TInt i=0; i< count; i++)
            {
            aResult.Append(xpath1[i]);
            }

        for ( TInt j=0; j< count2; j++ )
            {
            if(aResult.Find(xpath2[j]) == KErrNotFound)
                {
                aResult.Append(xpath1[j]);  
                }
            } 
       }
    return EFalse;           
    }

TBool CEncryptedSignedElements::IfNsQualified(CSenElement* aAssertion, TDesC8& aXpath)
    {
    // the xpath is /pp:nimi/qq:foo/rr:bah/ 
    //all the namespacce defined in Xpath should be in the Document namespaces
    TPtrC8 xpath(aXpath);
    while ( true )
        {
        TInt slash = xpath.Find(_L8("/"));
        xpath.Set(xpath.Right(xpath.Length()-slash-1)); 
        if( slash >= 0 )   
            {
            TInt position = xpath.Find(KColon);
            if(position)
                {
                TPtrC8 nsString = xpath.Left(position);
                const CSenNamespace* nameSpace = aAssertion->Namespace(nsString, ETrue);
                if(nameSpace != NULL)
                    {
                    continue;
                    }
                else
                    {
                    return EFalse;
                    }
                }
            else 
                break;
            }
        else
            {
            break;
            }
        }
    return ETrue;
    }

// END OF FILE


