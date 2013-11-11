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








// INCLUDE FILES
#include "seninternalwspolicy.h"
#include "SenFacet.h"
#include <SenXmlElement.h>
#include <SenXmlUtils.h>
#include "senwsdescription.h"
#include "SenDateUtils.h"
#include "senservicepolicy.h"
#include "wspolicyutils.h"


#ifdef SYMBIAN_SECURE_ECOM
    // for S60 FP3 (2.8) platform or newer (3.0 and above)
    #include <xml/attribute.h> // needed for RAttributeArray
//#else  // for S60 FP2 platform (2.6) or older
    //#include "Attribute.h"
#endif

#include <xml/dom/xmlengnodelist.h> 
using namespace Xml;
using namespace SenWSInternalPolicy;
namespace
    {
    const TInt KFlatBufSize                       = 128;
    }
CSenInternalWsPolicy* CSenInternalWsPolicy::NewL()
    {
    CSenInternalWsPolicy* pNew = NewLC();
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CSenInternalWsPolicy* CSenInternalWsPolicy::NewLC()
    {
    CSenInternalWsPolicy* pNew = new (ELeave) CSenInternalWsPolicy;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(KSenWSPolicyMetadata);
    return pNew;
    }

CSenInternalWsPolicy* CSenInternalWsPolicy::NewL(
                                           	const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
    {
    CSenInternalWsPolicy* pNew = NewLC(aNsUri, aLocalName, aQName, aAttributes);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CSenInternalWsPolicy* CSenInternalWsPolicy::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
    {
    CSenInternalWsPolicy* pNew = new (ELeave) CSenInternalWsPolicy;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes);
    return pNew;
    }

CSenInternalWsPolicy* CSenInternalWsPolicy::NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                             CSenElement& aParent)
    {
    CSenInternalWsPolicy* pNew = NewLC(aNsUri, aLocalName,
                                         aQName, aAttributes, aParent);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CSenInternalWsPolicy* CSenInternalWsPolicy::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            CSenElement& aParent)
    {
    CSenInternalWsPolicy* pNew = new (ELeave) CSenInternalWsPolicy;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent);
    return pNew;
    }
void CSenInternalWsPolicy::ConstructL(const TDesC8&  aNsUri,
                                          const TDesC8&  aLocalName,
                                          const TDesC8&  aQName,
                                          const RAttributeArray& aAttributes,
                                           CSenElement& aParent)
    {
    BaseConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent);
    }
void CSenInternalWsPolicy::ConstructL(const TDesC8&  aNsUri,
                                          const TDesC8&  aLocalName,
                                          const TDesC8&  aQName,
                                          const RAttributeArray& aAttributes)
    {
    BaseConstructL(aNsUri, aLocalName, aQName, aAttributes);
    }
  
void CSenInternalWsPolicy::ConstructL(const TDesC8& aLocalName)
    {
    CSenBaseFragment::BaseConstructL(aLocalName);
    }

CSenInternalWsPolicy::CSenInternalWsPolicy()
    {
    }

CSenInternalWsPolicy::~CSenInternalWsPolicy()
    {
    delete ipIdentifier;
    }

TBool CSenInternalWsPolicy::IsApplicableL(CSenWSDescription& aPattern)
    {
    TPtrC8 value;
    CSenElement& patternElement = ((CSenWSDescription&)aPattern).AsElement();
    CSenWSPolicyIdentifier& identifier = IdentifierL();

    TPtrC8 contract = aPattern.Contract();
    TPtrC8 endpoint = aPattern.Endpoint();
    CSenElement* providerId = aPattern.AsElement().Element(KProviderIdLocalName);
    if(providerId && providerId->HasContent())     
        {
        TPtrC8 provider = providerId->Content();    
        TPtrC8 valProviderId = identifier.ProviderIdL();
        TPtrC8 valEndpoint = identifier.EndpointL();
        TPtrC8 valContract = identifier.ContractL();
    
        if(contract == valContract && 
            endpoint == valEndpoint && 
            provider == valProviderId)
            {
            return ETrue;     
            }
        }
    return EFalse;    
    }
    
TBool CSenInternalWsPolicy::IsApplicableL(const TDesC8& aPolicyEndpoint)
    {
    CSenWSPolicyIdentifier& identifier = IdentifierL();

        TPtrC8 valEndpoint = identifier.MetadataEndpointL();
        if(valEndpoint == aPolicyEndpoint)
        {
            return ETrue;     
        }
    return EFalse;    
    }

TBool CSenInternalWsPolicy::IsApplicableL(CSenInternalWsPolicy* aInternalPolicy)
{
    CSenWSPolicyIdentifier& identifier = IdentifierL();
    CSenWSPolicyIdentifier& id = aInternalPolicy->IdentifierL();

        TPtrC8 valmetaEndpoint = identifier.MetadataEndpointL();
        TPtrC8 metaendpoint = id.MetadataEndpointL();
        TPtrC8 valEndpoint = identifier.EndpointL();
        TPtrC8 endpoint = id.EndpointL();
        TPtrC8 valcontract = identifier.ContractL();
        TPtrC8 contract = id.ContractL();

        if(valmetaEndpoint == metaendpoint &&
        valEndpoint == endpoint &&
        valcontract == contract)
        {
            return ETrue;     
        }
    return EFalse;    
    
}
TInt CSenInternalWsPolicy::PolicyCountL()
{
    TInt count = 0;
    CSenElement& element = AsElement();
    RPointerArray<CSenElement> list = element.ElementsL();
    CleanupClosePushL(list);
    count = list.Count();
    CleanupStack::PopAndDestroy(&list); // Close()
    return count;
    
}
TBool CSenInternalWsPolicy::UpdateMetadataEndpointL(CSenInternalWsPolicy* aInternalPolicy, TInt& aExist)
    {
//see if the New policy is in database
//&aPolicy is new Policy
//if the new policy is present and newer then the current -- we have to  replace  it
//if the new policy is present and same then the current -- check expiry - if same then no replace
//if the new policy dont have a policy then get the policy from here
//if the new policy is same (from expiry time)

    CSenWSPolicyIdentifier& newId = aInternalPolicy->IdentifierL();
    CSenWSPolicyIdentifier& id = IdentifierL();
    
    if (IsApplicableL(aInternalPolicy))
        {
            aExist = KErrNone;
            id.UpdateMetadataEndpointL(newId);
            return ETrue;
        }
        return EFalse;
    
    }

void CSenInternalWsPolicy::SetIdentifier(CSenWSPolicyIdentifier* aIdentifier)
    {
    if ( ipIdentifier )
        {
        delete ipIdentifier;
        }

    ipIdentifier = aIdentifier;
    }

CSenWSPolicyIdentifier& CSenInternalWsPolicy::IdentifierL()
    {
    if ( !ipIdentifier )
        {
        ipIdentifier = CSenWSPolicyIdentifier::NewL();
        }

    return *ipIdentifier;
    }

    
void CSenInternalWsPolicy::SetSession(CSenServiceSession& aSession)
    {
    ipSession = &aSession;
    }

CSenServiceSession* CSenInternalWsPolicy::Session()
    {
    return ipSession;
    }

TPtrC8 CSenInternalWsPolicy::ExpiryTimeL(CSenElement* aElement)
{
    if(CSenWSPolicyIdentifier::HasAttributeL(aElement, KSenWSPolicyValidUntil))
        {
        return IdentifierL().ElementAttribValueL(aElement, KSenWSPolicyValidUntil);
        }
    
    return KNullDesC8();        

}
TInt CSenInternalWsPolicy::AddMetadataPolicyL(CSenElement* aElement)
{
    if(aElement == NULL)
        return KErrArgument;
    
    TInt retVal(KErrNotFound);
    TPtrC8 metaendpoint = CWSPolicyUtils::PolicyUriL(aElement);
    TPtrC8 expiry = ExpiryTimeL(aElement);
    
    CSenElement& ele = AsElement();
    CSenElement* metaEndPointEle= IdentifierL().MetadataEndpointElementL();
    if (metaEndPointEle)
    {
        //confirm if the expiry time is greater then current time
        TTime now;
        now.UniversalTime();
        TTime validuntil = SenDateUtils::FromXmlDateTimeL(expiry);
        if(validuntil > now)
            {
            SenXmlUtils::AddAttributeL(*metaEndPointEle, KSenWSPolicyValidUntil,expiry); 
//Have to copy incase for multiple SDs have same policy
//we cannot give ownership of Policy here so create a copy and dont take ownership
            
            CSenElement* copy = CSenXmlElement::NewL(aElement->LocalName());
            copy->CopyFromL(*aElement);
            ele.AddElementL(*copy);
            retVal = KErrNone;
            }
    }

    return retVal;
}
TInt CSenInternalWsPolicy::UpdateMetadataPolicyL(CSenElement* aElement)
{

    TInt retVal(KErrNotFound);
    TPtrC8 metaendpoint = CWSPolicyUtils::PolicyUriL(aElement);
    TPtrC8 newExpiry = ExpiryTimeL(aElement);
    
    CSenElement* metaEndPointEle= IdentifierL().MetadataEndpointElementL();
             
    CSenElement& ele = AsElement();
    RPointerArray<CSenElement>& policyList = ele.ElementsL();
   
    TInt count2(policyList.Count());    //Now check for WSDL or wsp:Policy 
    for(TInt i = 0; i < count2; i++)
        {
        CSenElement* docElement = policyList[i];
        TPtrC8 currentExpiry = ExpiryTimeL(docElement);
        TPtrC8 eleName = docElement->LocalName();
        TPtrC8 eleNs = docElement->NsPrefix();
        
        if(eleName == KWsdlDocument && eleNs == KWsdlNsPrefix)
            {
            }
        else if (eleName == KPolicyDocument && eleNs == KPolicyNsPrefix)
            {
            TPtrC8 doc_metaendpoint = CWSPolicyUtils::PolicyUriL(docElement);
            
            TTime now;
            now.UniversalTime();
            TBuf8<SenDateUtils::KXmlDateTimeMaxLength> nowtime;
            TRAPD(error, SenDateUtils::ToXmlDateTimeUtf82L(nowtime, now);)
            
            if(error == KErrNone && metaEndPointEle && doc_metaendpoint == metaendpoint && 
                        newExpiry > currentExpiry &&
                        newExpiry > nowtime)
                {
                CSenElement* removed = ele.RemoveElement(*docElement);
                delete removed;
                removed = NULL;
                
                SenXmlUtils::AddAttributeL(*metaEndPointEle, KSenWSPolicyValidUntil,newExpiry); 
//Have to copy incase for multiple SDs have same policy
//we cannot give ownership of Policy here so create a copy and dont take ownership
                CSenElement* copy = CSenXmlElement::NewL(aElement->LocalName());
                copy->CopyFromL(*aElement);
                ele.AddElementL(*copy);

                retVal = KErrNone;
                return retVal;
                }
            
            }            
        }



    return retVal;
}
TInt CSenInternalWsPolicy::RemoveMetadataPolicyL(const TDesC8& aUri)
{
    TInt retVal(KErrNotFound);
    CSenElement& ele = AsElement();
    TPtrC8 endpoint = IdentifierL().MetadataEndpointL();
    if(endpoint != aUri)
    {
        return retVal;
    }
    
    CSenElement* metadataendpoint = IdentifierL().MetadataEndpointElementL();
    if (metadataendpoint)
    {
        RPointerArray<CSenElement>& policyList = ele.ElementsL();
        TInt policyCount(policyList.Count());
        for(TInt i = 0; i< policyCount; i++)
        {
            CSenElement* policy = policyList[i];
            TPtrC8 metaendpoint = CWSPolicyUtils::PolicyUriL(policy);
            if(metaendpoint == aUri)
            {
                CSenElement* removed = ele.RemoveElement(*policy);
                delete removed;
                removed = NULL;
                
                retVal = KErrNone;
                return retVal;
            }
        }
    }
    retVal = KErrNotFound;
    return retVal;
    
}
CSenElement* CSenInternalWsPolicy::WsPolicyByUriL(const TDesC8& aUri)
{
    CSenElement& ele = AsElement();
    TPtrC8 uri = IdentifierL().MetadataEndpointL();
    if(uri != aUri)
    {
        return NULL;
    }
    
    RPointerArray<CSenElement>& policyList = ele.ElementsL();
    TInt policyCount(policyList.Count());
    for(TInt i = 0; i< policyCount; i++)
    {
        CSenElement* policy = policyList[i];
        TPtrC8 metaendpoint = CWSPolicyUtils::PolicyUriL(policy);
        if(metaendpoint == aUri)
        {

            return policy;
        }
    }
    return NULL;
    
}
CSenElement* CSenInternalWsPolicy::WsPolicyByName(const TDesC8& /*aName*/)
{
/*
    CSenElement& ele = AsElement();
    RPointerArray<CSenElement> list;
    CleanupClosePushL(list);
    TInt retVal = Identifier().MetadataEndpoint(list, aUri);
    if (retVal == KErrNone)
    {
        TInt count(list.Count()); //we should al3ways have count "1"
        if(count ==1) 
        {
            RPointerArray<CSenElement>& policyList = ele.ElementsL();
            TInt policyCount(policyList.Count());
            for(TInt i = 0; i< policyCount; i++)
            {
                CSenElement* policy = policyList[i];
                TPtrC8 name = CWSPolicyUtils::PolicyNameL(policy);
                if(name == aName)
                {
                    CleanupStack::PopAndDestroy(&list);
                    return policy;
                }
                    
            }
                
        }
    }
    CleanupStack::PopAndDestroy(&list);
*/    
    return NULL;
    
}
TTime CSenInternalWsPolicy::ValidUntilL(const TDesC8& aUri)
    {
    TTime retTime = Time::NullTTime();
    TInt retVal(KErrGeneral);

    CSenElement& ele = AsElement();
    TPtrC8 uri = IdentifierL().MetadataEndpointL();
    if(uri != aUri)
    {
        return retTime;
    }
    
    CSenElement* metaEndPointEle = IdentifierL().MetadataEndpointElementL();
    if(metaEndPointEle)
    {
        TPtrC8 expiry = ExpiryTimeL(metaEndPointEle);
        if(expiry.Length() > 0)
            TRAP (retVal, retTime = SenDateUtils::FromXmlDateTimeL(expiry));
    }
    

    if(retVal != KErrNone)
        retTime = Time::NullTTime();
        
    return retTime;
    
    }


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



CSenWSPolicyProperties* CSenWSPolicyProperties::NewL()
    {
    CSenWSPolicyProperties* pNew = NewLC();
    CleanupStack::Pop(pNew);
    return pNew;
    }

CSenWSPolicyProperties* CSenWSPolicyProperties::NewLC()
    {
    CSenWSPolicyProperties* pNew = new (ELeave) CSenWSPolicyProperties;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(KSenWsPolicyProperteisLocalname);
    return pNew;
    }

CSenWSPolicyProperties* CSenWSPolicyProperties::NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
    {
    CSenWSPolicyProperties* pNew = NewLC(aNsUri, aLocalName, aQName, aAttributes);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CSenWSPolicyProperties* CSenWSPolicyProperties::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
    {
    CSenWSPolicyProperties* pNew = new (ELeave) CSenWSPolicyProperties;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes);
    return pNew;
    }

CSenWSPolicyProperties* CSenWSPolicyProperties::NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            CSenElement& aParent)
    {
    CSenWSPolicyProperties* pNew = NewLC(aNsUri, aLocalName,
                                           aQName, aAttributes, aParent);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CSenWSPolicyProperties* CSenWSPolicyProperties::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            CSenElement& aParent)
    {
    CSenWSPolicyProperties* pNew = new (ELeave) CSenWSPolicyProperties;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent);
    return pNew;
    }

CSenWSPolicyProperties::~CSenWSPolicyProperties()
    {
    }

CSenWSPolicyProperties::CSenWSPolicyProperties()
    {
    }

TInt CSenWSPolicyProperties::SetPropertyL(const TDesC8& aName,
                                                         const TDesC8& aValue)
    {
    CSenElement& element = AsElement();
    RPointerArray<CSenElement> list;
    CleanupClosePushL(list);

    element.ElementsL(list, aName);

    if ( list.Count() > 0 )
        {
        CSenElement* firstElement = list[0];
        firstElement->SetContentL(aValue);
        CleanupStack::PopAndDestroy(&list); 
        
        return KErrNone;
        }
    CleanupStack::PopAndDestroy(&list);

    CSenElement& newElement = element.AddElementL(aName);
    newElement.SetContentL(aValue);
    return KErrNone;
    }

TInt CSenWSPolicyProperties::PropertyL(const TDesC8& aName, TPtrC8& aValue)
    {
    TInt retVal(KErrNone);

    CSenElement& element = AsElement();
    RPointerArray<CSenElement> list;
    CleanupClosePushL(list);

    element.ElementsL(list, aName);

    if ( list.Count() > 0 )
        {
        CSenElement* firstElement = list[0];
        if(firstElement->HasContent())
            aValue.Set(firstElement->Content());
        }
    else
        {
        retVal = KErrNotFound;
        }

    CleanupStack::PopAndDestroy(&list);
    return retVal;
    }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CSenWSPolicyIdentifier* CSenWSPolicyIdentifier::NewL()
    {
    CSenWSPolicyIdentifier* pNew = NewLC();
    CleanupStack::Pop(pNew);
    return pNew;
    }

CSenWSPolicyIdentifier* CSenWSPolicyIdentifier::NewLC()
    {
    CSenWSPolicyIdentifier* pNew = new (ELeave) CSenWSPolicyIdentifier;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(KSenWsPolicyIdentifierLocalname);
    return pNew;
    }

CSenWSPolicyIdentifier* CSenWSPolicyIdentifier::NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
    {
    CSenWSPolicyIdentifier* pNew = NewLC(aNsUri, aLocalName, aQName, aAttributes);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CSenWSPolicyIdentifier* CSenWSPolicyIdentifier::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
    {
    CSenWSPolicyIdentifier* pNew = new (ELeave) CSenWSPolicyIdentifier;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes);
    return pNew;
    }

CSenWSPolicyIdentifier* CSenWSPolicyIdentifier::NewL(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            CSenElement& aParent)
    {
    CSenWSPolicyIdentifier* pNew = NewLC(aNsUri, aLocalName,
                                           aQName, aAttributes, aParent);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CSenWSPolicyIdentifier* CSenWSPolicyIdentifier::NewLC(
                                            const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes,
                                            CSenElement& aParent)
    {
    CSenWSPolicyIdentifier* pNew = new (ELeave) CSenWSPolicyIdentifier;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent);
    return pNew;
    }

CSenWSPolicyIdentifier::~CSenWSPolicyIdentifier()
    {
    }

CSenWSPolicyIdentifier::CSenWSPolicyIdentifier()
    {
    }

void CSenWSPolicyIdentifier::SetIdL(TInt aId)
    {
    CSenElement& element = AsElement();
    RPointerArray<CSenElement> list;
    CleanupClosePushL(list);

    element.ElementsL(list, KSenWSPolicyId);

    CSenElement* idElement;
    if ( list.Count() > 0 )
        {
        idElement = list[0];
        }
    else
        {
        idElement = &element.AddElementL(KSenWSPolicyId);
        }

    TBuf8<KFlatBufSize> buffer;
    buffer.Num(aId);
    idElement->SetContentL(buffer);

    CleanupStack::PopAndDestroy(&list); 
    }

TInt CSenWSPolicyIdentifier::IdL()
    {
    TInt retVal(KErrNotFound);

    CSenElement& element = AsElement();
    RPointerArray<CSenElement> list;
    CleanupClosePushL(list);

    element.ElementsL(list, KSenWSPolicyId);

    if ( list.Count() > 0 )
        {
        CSenElement* idElement = list[0];
        if(idElement->HasContent())
            {
            TLex8 lex;
            lex.Assign(idElement->Content());
            lex.Val( retVal );
            }
        }

    CleanupStack::PopAndDestroy(&list); 

    return retVal;
    }
TPtrC8 CSenWSPolicyIdentifier::ContractL()
{
    CSenElement& element = AsElement();
    RPointerArray<CSenElement> list;
    CleanupClosePushL(list);

    element.ElementsL(list, KContractLocalname);
    
    if ( list.Count() > 0 )
        {
        CSenElement* idElement = list[0];
        CleanupStack::PopAndDestroy(&list); 
        if(idElement->HasContent())
            return idElement->Content();
       else
            return KNullDesC8();           
        
        }

    CleanupStack::PopAndDestroy(&list); 

    return KNullDesC8();
    
}
TPtrC8 CSenWSPolicyIdentifier::EndpointL()
{
    
    CSenElement& element = AsElement();
    RPointerArray<CSenElement> list;
    CleanupClosePushL(list);

    element.ElementsL(list, KEndpointLocalname);
    
    if ( list.Count() > 0 )
        {
        CSenElement* idElement = list[0];
        CleanupStack::PopAndDestroy(&list); 
       if(idElement->HasContent())
            return idElement->Content();
       else
            return KNullDesC8();           

        }

    CleanupStack::PopAndDestroy(&list); 

    return KNullDesC8();
    
}
TPtrC8 CSenWSPolicyIdentifier::MetadataEndpointL()
{

    CSenElement& element = AsElement();
    RPointerArray<CSenElement> list;
    CleanupClosePushL(list);

    element.ElementsL(list, KMetadataEndpointLocalname);
    
    if ( list.Count() > 0 )
        {
        CSenElement* idElement = list[0];
        CleanupStack::PopAndDestroy(&list); 
       if(idElement->HasContent())
            return idElement->Content();
       else
            return KNullDesC8();           

        }

    CleanupStack::PopAndDestroy(&list); 

    return KNullDesC8();    
    
}
CSenElement* CSenWSPolicyIdentifier::MetadataEndpointElementL()
{

    CSenElement& element = AsElement();
    RPointerArray<CSenElement> list;
    CleanupClosePushL(list);

    element.ElementsL(list, KMetadataEndpointLocalname);
    
    if ( list.Count() > 0 )
        {
        CSenElement* idElement = list[0];
        CleanupStack::PopAndDestroy(&list); 
       if(idElement->HasContent())
            return idElement;
       else
            return NULL;

        }

    CleanupStack::PopAndDestroy(&list); 

    return NULL;    
    
}

TPtrC8 CSenWSPolicyIdentifier::ProviderIdL()
{

    CSenElement& element = AsElement();
    RPointerArray<CSenElement> list;
    CleanupClosePushL(list);

    element.ElementsL(list, KProviderIdLocalName);
    
    if ( list.Count() > 0 )
        {
        CSenElement* idElement = list[0];
        CleanupStack::PopAndDestroy(&list); 
       if(idElement->HasContent())
            return idElement->Content();
       else
            return KNullDesC8();           

        }

    CleanupStack::PopAndDestroy(&list); 

    return KNullDesC8();    
}
TPtrC8 CSenWSPolicyIdentifier::ElementAttribValueL(CSenElement* aElement, const TDesC8& aName)
{

    if(aElement == NULL || aName.Length() == NULL) { return KNullDesC8(); }
    
    RPointerArray<CSenBaseAttribute>& attrs = aElement->AttributesL();
    
    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
        bs = (attrs)[j];
        if(bs)
        {
            if(bs->Name().Compare(aName) == 0)
            {
                 return bs->Value();
            }
         }

            
        }
    return KNullDesC8();
}
TBool CSenWSPolicyIdentifier::HasAttributeL(CSenElement* aElement, const TDesC8& aName)
{
     if(aElement == NULL || aName.Length() == NULL) { return EFalse; }
    
    RPointerArray<CSenBaseAttribute>& attrs = aElement->AttributesL();
    
    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
        bs = (attrs)[j];
        if(bs)
        {
            if(bs->Name().Compare(aName) == 0)
            {
                 return ETrue;
            }
         }

            
        }
    return EFalse;
    
}

TInt CSenWSPolicyIdentifier::AddMetadataEndpointL(CSenElement* aElement)
{

    if(aElement == NULL || aElement->HasContent() == EFalse)
        return KErrArgument;

//get the method    
    TPtrC8 ptrMethod = ElementAttribValueL(aElement, KMetadataMethodLocalname);
//now compare the two Metadata endpoints
    CSenElement& element = AsElement();
    RPointerArray<CSenElement> list;
    CleanupClosePushL(list);
    element.ElementsL(list, aElement->LocalName());
    TInt count = list.Count();
    
    if(count == 0)    
        {
        CSenElement& newElement = element.AddElementL(aElement->LocalName());
        newElement.SetContentL(aElement->Content());
        if(ptrMethod.Length() > 0)
            SenXmlUtils::AddAttributeL(newElement, KMetadataMethodLocalname, ptrMethod);
        CleanupStack::PopAndDestroy(&list);
        return KErrNone;
        }
    
    for(TInt i = 0; i < count; i++)
        {
        CSenElement* firstElement = list[i];
        if(firstElement->Content() == aElement->Content())
            {
            
            TPtrC8 firstMethod = ElementAttribValueL(firstElement, KMetadataMethodLocalname);            
            if (ptrMethod.Compare(firstMethod) == 0 )
                {
                CleanupStack::PopAndDestroy(&list); // Close()
                //exactly Same so no need to add again                        
                return KErrNone;
                }
            }
            
        }

    CleanupStack::PopAndDestroy(&list);
    
    CSenElement& newElement = element.AddElementL(aElement->LocalName());
    newElement.SetContentL(aElement->Content());
    if(ptrMethod.Length() > 0)
        SenXmlUtils::AddAttributeL(newElement, KMetadataMethodLocalname, ptrMethod);

    return KErrNone;
    
    
}

TInt CSenWSPolicyIdentifier::UpdateMetadataEndpointL(CSenWSPolicyIdentifier& aIdentifier)
{

//now compare contents and then compare attributes
    TInt retVal = KErrNotFound;
    TPtrC8 endpoint = MetadataEndpointL();
    TPtrC8 canEndPoint = aIdentifier.MetadataEndpointL();
    if(endpoint == canEndPoint)
        {
        CSenElement* ele1 = MetadataEndpointElementL();        
        CSenElement* ele2 = aIdentifier.MetadataEndpointElementL();

        if(ele1 && ele2 && ele1->HasContent()) //&& ele2->HasContent())
        //no need to verify this as important thing is that new content has Contents
        {
            //if Endpoint URi is same
            if(ele1->Content() == ele2->Content())
                {
                if(HasAttributeL(ele1, KMetadataMethodLocalname))
                    {
                    TPtrC8 val1 =  ElementAttribValueL(ele1, KMetadataMethodLocalname);
                      
                    if(HasAttributeL(ele2, KMetadataMethodLocalname))
                        {
                        TPtrC8 val2 =  ElementAttribValueL(ele2, KMetadataMethodLocalname) ; 
                        if(val1.Compare(val2) != 0)
                            {
                            //replace the Attribute with a newer Value
                             ele2->AddAttrL(KMetadataMethodLocalname,val1);
                             retVal = KErrNone;
                            }
                        }
                    }                
                }        
        }
        }
    return retVal;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



void RSenWSPolicyPtr::OpenL(CSenInternalWsPolicy* apPolicy)
    {
    ipPolicyData = new (ELeave) TSenWsPolicyData;
    ipPolicyData->iCounter = 1;
    ipPolicyData->ipPolicy = apPolicy;
    }
    
CSenInternalWsPolicy* RSenWSPolicyPtr::Policy()
    {
    if ( ipPolicyData )
        {
        return ipPolicyData->ipPolicy;
        }
    else
        {
        return NULL;
        }
    }
    
void RSenWSPolicyPtr::SetPolicy(CSenInternalWsPolicy* apPolicy)
    {
    if ( ipPolicyData )
        {
        delete ipPolicyData->ipPolicy;
        ipPolicyData->ipPolicy = apPolicy;
        }
    }
    
RSenWSPolicyPtr RSenWSPolicyPtr::Clone()
    {
    if ( ipPolicyData )
        {
        (ipPolicyData->iCounter)++;
        }
    return *this;
    }
    
void RSenWSPolicyPtr::Close()
    {
    if ( ipPolicyData )
        {

        (ipPolicyData->iCounter)--;
        if ( ipPolicyData->iCounter == 0 )
            {
            TRAP_IGNORE( ipPolicyData->ipPolicy->IdentifierL().ExtractElement(); )
            ipPolicyData->ipPolicy->ExtractElement();
            delete ipPolicyData->ipPolicy;
            delete ipPolicyData;
            }

        ipPolicyData = NULL;
        }
    }

void RSenWSPolicyPtr::CloseAndDestroyPolicy()
    {
    if ( ipPolicyData )
        {
        TRAP_IGNORE( ipPolicyData->ipPolicy->IdentifierL().ExtractElement(); )
        ipPolicyData->ipPolicy->ExtractElement();
        delete ipPolicyData->ipPolicy;
        ipPolicyData->ipPolicy = NULL;
        
        (ipPolicyData->iCounter)--;
        if ( ipPolicyData->iCounter == 0 )
            {
            delete ipPolicyData;
            }

        ipPolicyData = NULL;
        }
    }
    
RSenWSPolicyPtr::RSenWSPolicyPtr()
: ipPolicyData(NULL)
    {
    }

// END OF FILE
