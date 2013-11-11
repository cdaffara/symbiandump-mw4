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
#include <e32std.h>
#include <s32strm.h>
#include "sendebug.h"                 // internal Utils\inc - logging MACROs

#include <SenElement.h>
#include <SenXmlUtils.h>
#include "SenXmlElement.h"
#include "servicepolicy.h"
#include "SenIdentityProviderIdArray8.h"
#include "senguidgen.h"
#include "senlogger.h"
	
namespace
    {
    _LIT8(KServicePolicyLocalName, "ServicePolicy");
    _LIT8(KClientServicePolicyLocalName, "ClientPolicy");
    _LIT8(KPolicyArrayLocalName, "PolicyArray");
    _LIT8(KMetaDataLocalName, "MetadataEndpoint");
    _LIT8(KName, "Name");
    _LIT8(KSenNsUri, "urn:com.nokia.Sen.config.1.0");
    _LIT8(KPolicyLocalName, "Policy");
    _LIT8(KWsPolicyNsUri , "http://schemas.xmlsoap.org/ws/2004/09/policy"); // Namespace of WS-Policy
    _LIT8(KId, "wsu:Id"); 
	_LIT8(KWsuPrefix, "wsu"); 
	_LIT8(KWsuNsUri, "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wsswssecurity-utility-1.0.xsd");
    _LIT8(KAppInfoLocalName, "AppInfo");
    _LIT8(KApplicationLocalName, "Application");
    _LIT8(KDeviceOptionsLocalName, "DeviceOptions");
    _LIT8(KUserAgentLocalName, "UserAgent");
    _LIT8(KOSLocalName, "OS");
    //_LIT8(KAppInfoNsUri , "http://schemas.live.com/mws/2006/10/core");
    }


CServicePolicy* CServicePolicy::NewL()
    {
    CServicePolicy* pNew = NewLC();
    CleanupStack::Pop();
    return(pNew) ;
    }

CServicePolicy* CServicePolicy::NewLC()
    {
    CServicePolicy* pNew = new (ELeave) CServicePolicy();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(KServicePolicyLocalName());
    return pNew;
    }

CServicePolicy::CServicePolicy()
    {
    }

CServicePolicy::~CServicePolicy()
    {
    TLSLOG_CLOSE(KSenServConnLogChannelBase);
    //LOG_CLOSE;
    }

void CServicePolicy::BaseConstructL(const TDesC8& aPolicyLocalName)
    {
    CSenBaseFragment::BaseConstructL(aPolicyLocalName);
#ifdef _SENDEBUG    
    _LIT(KSenServConn, "SenServicePolicy");
    _LIT(KSenServConnLog, "SenServicePolicy.log");
    
    
    // Open connection to the file logger server
    TLSLOG_OPEN(KSenServConnLogChannelBase,KSenServConnLogLevel ,KSenServConn,KSenServConnLog);
    //LOG_CREATELOG((KSenServConn, KSenServConnLog, EFileLoggingModeOverwrite));
#endif    
    }

//Do nothing
TBool CServicePolicy::Accepts(MSenServicePolicy& aPolicyPattern)
    {
    TBool retVal(ETrue);
    TRAPD(err, retVal = AcceptsL(aPolicyPattern));
    if (err) return EFalse;
    return retVal;
    }

//Do nothing
TBool CServicePolicy::AcceptsL(MSenServicePolicy& /*aPolicyPattern*/)
    {
    return ETrue;
    }

//Return all of the Services polices from SD only <wsp:Policy> tags
TInt CServicePolicy::ServicePolicies(RPolicyArray& aPolicies)
    {
    TInt err( KErrNone );
    CSenElement* pPolicyArrayElement = AsElement().Element(KPolicyArrayLocalName);
    if(pPolicyArrayElement)
        {
        TRAP( err, aPolicies = pPolicyArrayElement->ElementsL(); ) //, KWsPolicyNsUri, KPolicyLocalName);
        if(!err && aPolicies.Count() > 0)
           {
           return KErrNone;
           }
        return KErrNotFound;
        }
    return KErrNotFound;        
    }

//Adding client policy to SD
TInt CServicePolicy::AddClientPolicyL(CSenElement* aTemplate)
    {
    /*CSenElement* pServicePolicyElement = AsElement().Element(KServicePolicyLocalName);*/
    CSenElement* pClientPolicyElement = AsElement().Element(KClientServicePolicyLocalName);
    
    if(!pClientPolicyElement)
        {
        pClientPolicyElement = AsElement().Element(KSenNsUri, KClientServicePolicyLocalName);
        if(!pClientPolicyElement)
            {
            pClientPolicyElement = &(AsElement().AddElementL(KClientServicePolicyLocalName));  
            }
        }
    
    if(!pClientPolicyElement->ConsistsOfL(*aTemplate))
        {
        RPointerArray<CSenElement> children = aTemplate->ElementsL();
        for(TInt i = 0; i< children.Count(); i++)
            {
                CSenElement* ele = children[i];
                
                if(!ele->LocalName().Compare(KAppInfoLocalName))
                {
                    AddApplicationInfoElementsL(ele);
                }
                else
                {
                     CSenElement& added = pClientPolicyElement->AddElementL(ele->LocalName());
                     
                    TPtrC8 sourceContent = ele->Content();
                    if(sourceContent.Length() > 0)
                        {
                        added.SetContentL(sourceContent);
                        }
                    RPointerArray<CSenBaseAttribute> sourceAttributes = ele->AttributesL();
                    if (sourceAttributes.Count() > 0)
                        {
                        for (TInt i=0;i<sourceAttributes.Count(); i++)
                            {
                            CSenBaseAttribute* pBaseAttribute = sourceAttributes[i];
                            SenXmlUtils::AddAttributeL(added, pBaseAttribute->Name(), pBaseAttribute->Value());
                            }
                        }
                            
                }
                
            }
        }
    return KErrNone;
    }
void CServicePolicy::AddApplicationInfoElementsL(CSenElement* aApplicationInfoElement)
    {
    CSenElement* pClientPolicyElement = AsElement().Element(KClientServicePolicyLocalName);
    CSenElement* pAppPolicyElement = pClientPolicyElement->Element(KAppInfoLocalName);
    
    if(!pAppPolicyElement)
        {
        pAppPolicyElement = pClientPolicyElement->Element(KSenNsUri, KAppInfoLocalName);
        if(!pAppPolicyElement)
            {
            pAppPolicyElement = &(pClientPolicyElement->AddElementL(KAppInfoLocalName));  
            }
        }
             
    TPtrC8 sourceContent = aApplicationInfoElement->Content();
    if(sourceContent.Length() > 0)
        {
        pAppPolicyElement->SetContentL(sourceContent);
        }
        
    RPointerArray<CSenBaseAttribute> sourceAttributes = aApplicationInfoElement->AttributesL();
    if (sourceAttributes.Count() > 0)
        {
        for (TInt i=0;i<sourceAttributes.Count(); i++)
            {
            CSenBaseAttribute* pBaseAttribute = sourceAttributes[i];
            SenXmlUtils::AddAttributeL(*pAppPolicyElement, pBaseAttribute->Name(), pBaseAttribute->Value());
            }
        }
        
    RPointerArray<CSenElement> children = aApplicationInfoElement->ElementsL();
    for(TInt i = 0; i< children.Count(); i++)
        {
            CSenElement* ele = children[i];
            
            if(!ele->LocalName().Compare(KApplicationLocalName))
            {
                AddAppicationElementsL(ele);
            }
            else if(!ele->LocalName().Compare(KDeviceOptionsLocalName))
            {
                AddDeviceElementsL(ele);
            }
            else
            {
                CSenElement& added = pAppPolicyElement->AddElementL(ele->LocalName());
                TPtrC8 sourceContent = ele->Content();
                if(sourceContent.Length() > 0)
                    {
                    added.SetContentL(sourceContent);
                    }
                RPointerArray<CSenBaseAttribute> sourceAttributes = ele->AttributesL();
                if (sourceAttributes.Count() > 0)
                    {
                    for (TInt i=0;i<sourceAttributes.Count(); i++)
                        {
                        CSenBaseAttribute* pBaseAttribute = sourceAttributes[i];
                        SenXmlUtils::AddAttributeL(added, pBaseAttribute->Name(), pBaseAttribute->Value());
                        }
                    }
            }
         }

     }
void CServicePolicy::AddAppicationElementsL(CSenElement* aApplicationElement)
    {
    CSenElement* pClientPolicyElement = AsElement().Element(KClientServicePolicyLocalName);
    CSenElement* pAppInfoPolicyElement = pClientPolicyElement->Element(KAppInfoLocalName);
    CSenElement* pAppPolicyElement = pAppInfoPolicyElement->Element(KApplicationLocalName);
    
    if(!pAppPolicyElement)
        {
        pAppPolicyElement = pAppInfoPolicyElement->Element(KSenNsUri, KApplicationLocalName);
        if(!pAppPolicyElement)
            {
            pAppPolicyElement = &(pAppInfoPolicyElement->AddElementL(KApplicationLocalName));  
            }
        }
    TPtrC8 sourceContent = aApplicationElement->Content();
    if(sourceContent.Length() > 0)
        {
        pAppPolicyElement->SetContentL(sourceContent);
        }
        
    RPointerArray<CSenBaseAttribute> sourceAttributes = aApplicationElement->AttributesL();
    if (sourceAttributes.Count() > 0)
        {
        for (TInt i=0;i<sourceAttributes.Count(); i++)
            {
            CSenBaseAttribute* pBaseAttribute = sourceAttributes[i];
            SenXmlUtils::AddAttributeL(*pAppPolicyElement, pBaseAttribute->Name(), pBaseAttribute->Value());
            }
        }
        
    RPointerArray<CSenNamespace> sourceNamespaces = aApplicationElement->NamespacesL();
    if (sourceNamespaces.Count() > 0)
        {
        for (TInt i=0;i<sourceNamespaces.Count(); i++)
            {
            CSenNamespace* pNameSapce = sourceNamespaces[i];
            pAppPolicyElement->AddNamespaceL(*pNameSapce,ETrue);
            }
        }
        
    RPointerArray<CSenElement> children = aApplicationElement->ElementsL();
    for(TInt i = 0; i< children.Count(); i++)
        {
            CSenElement* ele = children[i];
            CSenElement& added = pAppPolicyElement->AddElementL(ele->LocalName());
             
          TPtrC8 sourceContent = ele->Content();
            if(sourceContent.Length() > 0)
                {
                added.SetContentL(sourceContent);
                }
            RPointerArray<CSenBaseAttribute> sourceAttributes = ele->AttributesL();
            if (sourceAttributes.Count() > 0)
                {
                for (TInt i=0;i<sourceAttributes.Count(); i++)
                    {
                    CSenBaseAttribute* pBaseAttribute = sourceAttributes[i];
                    SenXmlUtils::AddAttributeL(added, pBaseAttribute->Name(), pBaseAttribute->Value());
                    }
                }
             
        }

     }
void CServicePolicy::AddDeviceElementsL(CSenElement* aDeviceElement)
    {
    CSenElement* pClientPolicyElement = AsElement().Element(KClientServicePolicyLocalName);
    CSenElement* pAppPolicyElement = pClientPolicyElement->Element(KAppInfoLocalName);
    CSenElement* pDevicePolicyElement = pAppPolicyElement->Element(KDeviceOptionsLocalName);
    
    if(!pDevicePolicyElement)
        {
        pDevicePolicyElement = pAppPolicyElement->Element(KSenNsUri, KDeviceOptionsLocalName);
        if(!pDevicePolicyElement)
            {
            pDevicePolicyElement = &(pAppPolicyElement->AddElementL(KDeviceOptionsLocalName));  
            }
        }
            
    TPtrC8 sourceContent = aDeviceElement->Content();
    if(sourceContent.Length() > 0)
        {
        pDevicePolicyElement->SetContentL(sourceContent);
        }
        
    RPointerArray<CSenBaseAttribute> sourceAttributes = aDeviceElement->AttributesL();
    if (sourceAttributes.Count() > 0)
        {
        for (TInt i=0;i<sourceAttributes.Count(); i++)
            {
            CSenBaseAttribute* pBaseAttribute = sourceAttributes[i];
            SenXmlUtils::AddAttributeL(*pDevicePolicyElement, pBaseAttribute->Name(), pBaseAttribute->Value());
            }
        }
        
    RPointerArray<CSenNamespace> sourceNamespaces = aDeviceElement->NamespacesL();
    if (sourceNamespaces.Count() > 0)
        {
        for (TInt i=0;i<sourceNamespaces.Count(); i++)
            {
            CSenNamespace* pNameSapce = sourceNamespaces[i];
            pDevicePolicyElement->AddNamespaceL(*pNameSapce,ETrue);
            }
        }
            
    RPointerArray<CSenElement> children = aDeviceElement->ElementsL();
    for(TInt i = 0; i< children.Count(); i++)
        {
            CSenElement* ele = children[i];
                    
            if(!ele->LocalName().Compare(KUserAgentLocalName))
            {
                AddUserAgentElementsL(ele);
            }
            else
            {

            CSenElement& added = pDevicePolicyElement->AddElementL(ele->LocalName());
             
          TPtrC8 sourceContent = ele->Content();
            if(sourceContent.Length() > 0)
                {
                added.SetContentL(sourceContent);
                }
            RPointerArray<CSenBaseAttribute> sourceAttributes = ele->AttributesL();
            if (sourceAttributes.Count() > 0)
                {
                for (TInt i=0;i<sourceAttributes.Count(); i++)
                    {
                    CSenBaseAttribute* pBaseAttribute = sourceAttributes[i];
                    SenXmlUtils::AddAttributeL(added, pBaseAttribute->Name(), pBaseAttribute->Value());
                    }
                }
                
            } 
             
        }

     }
void CServicePolicy::AddUserAgentElementsL(CSenElement* aUserAgentElement)
    {
    CSenElement* pClientPolicyElement = AsElement().Element(KClientServicePolicyLocalName);
    CSenElement* pAppPolicyElement = pClientPolicyElement->Element(KAppInfoLocalName);
    CSenElement* pDevicePolicyElement = pAppPolicyElement->Element(KDeviceOptionsLocalName);
    CSenElement* pUserAgentElement = pDevicePolicyElement->Element(KUserAgentLocalName);
    
    if(!pUserAgentElement)
        {
        pUserAgentElement = pDevicePolicyElement->Element(KSenNsUri, KUserAgentLocalName);
        if(!pUserAgentElement)
            {
            pUserAgentElement = &(pDevicePolicyElement->AddElementL(KUserAgentLocalName));  
            }
        }
            
    TPtrC8 sourceContent = aUserAgentElement->Content();
    if(sourceContent.Length() > 0)
        {
        pUserAgentElement->SetContentL(sourceContent);
        }
        
    RPointerArray<CSenBaseAttribute> sourceAttributes = aUserAgentElement->AttributesL();
    if (sourceAttributes.Count() > 0)
        {
        for (TInt i=0;i<sourceAttributes.Count(); i++)
            {
            CSenBaseAttribute* pBaseAttribute = sourceAttributes[i];
            SenXmlUtils::AddAttributeL(*pUserAgentElement, pBaseAttribute->Name(), pBaseAttribute->Value());
            }
        }
        
    RPointerArray<CSenElement> children = aUserAgentElement->ElementsL();
    for(TInt i = 0; i< children.Count(); i++)
        {
            CSenElement* ele = children[i];
            
            if(!ele->LocalName().Compare(KOSLocalName))
            {
                AddOSElementsL(ele);
            }
            else
            {

            CSenElement& added = pUserAgentElement->AddElementL(ele->LocalName());
             
          TPtrC8 sourceContent = ele->Content();
            if(sourceContent.Length() > 0)
                {
                added.SetContentL(sourceContent);
                }
            RPointerArray<CSenBaseAttribute> sourceAttributes = ele->AttributesL();
            if (sourceAttributes.Count() > 0)
                {
                for (TInt i=0;i<sourceAttributes.Count(); i++)
                    {
                    CSenBaseAttribute* pBaseAttribute = sourceAttributes[i];
                    SenXmlUtils::AddAttributeL(added, pBaseAttribute->Name(), pBaseAttribute->Value());
                    }
                }
             
            } 
        }

     }
void CServicePolicy::AddOSElementsL(CSenElement* aOSElement)
    {
    CSenElement* pClientPolicyElement = AsElement().Element(KClientServicePolicyLocalName);
    CSenElement* pAppPolicyElement = pClientPolicyElement->Element(KAppInfoLocalName);
    CSenElement* pDevicePolicyElement = pAppPolicyElement->Element(KDeviceOptionsLocalName);
    CSenElement* pUserAgentElement = pDevicePolicyElement->Element(KUserAgentLocalName);
    CSenElement* pOSElement = pUserAgentElement->Element(KOSLocalName);
    
    if(!pOSElement)
        {
        pOSElement = pUserAgentElement->Element(KSenNsUri, KOSLocalName);
        if(!pOSElement)
            {
            pOSElement = &(pUserAgentElement->AddElementL(KOSLocalName));  
            }
        }
            
    TPtrC8 sourceContent = aOSElement->Content();
    if(sourceContent.Length() > 0)
        {
        pOSElement->SetContentL(sourceContent);
        }
        
    RPointerArray<CSenBaseAttribute> sourceAttributes = aOSElement->AttributesL();
    if (sourceAttributes.Count() > 0)
        {
        for (TInt i=0;i<sourceAttributes.Count(); i++)
            {
            CSenBaseAttribute* pBaseAttribute = sourceAttributes[i];
            SenXmlUtils::AddAttributeL(*pOSElement, pBaseAttribute->Name(), pBaseAttribute->Value());
            }
        }
        
    RPointerArray<CSenElement> children = aOSElement->ElementsL();
    for(TInt i = 0; i< children.Count(); i++)
        {
            CSenElement* ele = children[i];
            CSenElement& added = pOSElement->AddElementL(ele->LocalName());
             
          TPtrC8 sourceContent = ele->Content();
            if(sourceContent.Length() > 0)
                {
                added.SetContentL(sourceContent);
                }
            RPointerArray<CSenBaseAttribute> sourceAttributes = ele->AttributesL();
            if (sourceAttributes.Count() > 0)
                {
                for (TInt i=0;i<sourceAttributes.Count(); i++)
                    {
                    CSenBaseAttribute* pBaseAttribute = sourceAttributes[i];
                    SenXmlUtils::AddAttributeL(added, pBaseAttribute->Name(), pBaseAttribute->Value());
                    }
                }
             
        }

     }

// Public function for adding a policy
TInt CServicePolicy::AddPolicyL(CSenElement* aPolicy)
    {
    TInt retVal(KErrNotFound);
    retVal = ValidatePolicy(aPolicy);
    
#ifdef _SENDEBUG
    {
    TLSLOG(KSenServConnLogChannelBase,KMaxLogLevel,(_L("TInt CServicePolicy::AddPolicyL  POLICY")));
    TLSLOG(KSenServConnLogChannelBase,KMaxLogLevel,(_L("-----------DIRECT ADDING---------------")));
    TLSLOG(KSenServConnLogChannelBase,KMaxLogLevel,(_L("---------------------------------------")));
    TLSLOG(KSenServConnLogChannelBase,KMaxLogLevel,(_L("---------------------------------------")));
    HBufC8* xml = aPolicy->AsXmlL();
    if(xml)
        {
        TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,(*xml));

        }
        delete xml;
    }    
#endif // _SENDEBUG

    if(retVal)
        {
        retVal =  AddWspPolicyL(aPolicy); 
        }
    return retVal;
    }
//Add a policy to SD
TInt CServicePolicy::AddWspPolicyL(CSenElement* aPolicy)
    {
    TInt retVal(KErrNotFound);
    CSenElement& pServicePolicy = AsElement();
    CSenElement* pPolicyArray = pServicePolicy.Element(KPolicyArrayLocalName);
    //get the PolicyArray which is holding all the <wsp:policy> elements
    if(!pPolicyArray)
        {
        pPolicyArray = pServicePolicy.Element(KSenNsUri, KPolicyArrayLocalName);
        if(!pPolicyArray)
            pPolicyArray = &(pServicePolicy.AddElementL(KPolicyArrayLocalName));
        }
    //get all the policies from <PolicyArray>        
    RPointerArray<CSenElement> policies;
    pPolicyArray->ElementsL(policies,KWsPolicyNsUri,KPolicyLocalName);
    
    CSenElement* pPolicy = NULL;
   
    TInt count(policies.Count());
    //If we have existing polcies then verify that this new policy doesnt exist there
    if(count > 0)
        {    
        for(TInt i=0; i<count; i++)
            {
            pPolicy = policies[i];
            if(pPolicy)
                {
                    if(pPolicy->LocalName().Compare(aPolicy->LocalName()) == 0 &&
                    (PolicyIdL(pPolicy).Compare(PolicyIdL(aPolicy)) != 0))	//codescannerwarnings
                        {
                        //same policy with same wsu:Id then 
                        // We are trying to add a policy with endpoint same
                        // it means that our current policy is expired and we have 
                        // fetched new policy
                        // we have to delete here old policy and then add new policy
                            if(PolicyUriL(pPolicy).Compare(PolicyUriL(aPolicy)) == 0)
                            {
                                CSenElement* deletedPolicy = pPolicyArray->RemoveElement(*pPolicy);
                                delete deletedPolicy;
                            }
                        //then add fresh policy here
                        CSenElement& apol= pPolicyArray->AddElementL(KPolicyLocalName);
                        apol.CopyFromL(*aPolicy);                        
                        retVal = KErrNone;
                        policies.Close();
                        return retVal;
                        }
                }

            }
        }
    else //Just add this policy in the <PolicyArray>
        {
        
        CSenElement& apol= pPolicyArray->AddElementL(KPolicyLocalName);
        apol.CopyFromL(*aPolicy);                                
        PolicyIdL(&apol);	//codescannerwarnings
        retVal = KErrNone;
        }
     
     policies.Close();
    return retVal;
    }

//Remove a policy from current SD
TInt CServicePolicy::RemovePolicyL(TDesC8& aPolicyUri)
    {
    TInt retVal(KErrNone);
    CSenElement& pServicePolicy = AsElement();
    CSenElement* pPolicyArray = pServicePolicy.Element(KPolicyArrayLocalName);
//get the PolicyArray which is holding all the <wsp:policy> elements
    if(!pPolicyArray)
        {
        pPolicyArray = pServicePolicy.Element(KSenNsUri, KPolicyArrayLocalName);
        if(!pPolicyArray)
               retVal = KErrNotFound; 
        }
    if(retVal == KErrNone)
		{
		//get all the policies from <PolicyArray>        
		RPointerArray<CSenElement> policies;
		CleanupClosePushL(policies);
		pPolicyArray->ElementsL(policies,KWsPolicyNsUri,KPolicyLocalName);
    
		CSenElement* pPolicy = NULL;
   
		TInt count(policies.Count());
//If we have existing polcies then verify that this new policy doesnt exist there
		if(count > 0)
			{    
			for(TInt i=0; i<count; i++)
				{
				pPolicy = policies[i];
				if(pPolicy)
					{
						if(PolicyNameL(pPolicy).Compare(aPolicyUri) == 0)	//codescannerwarnings
							{
                        //same policy with same NAME 
                        CSenElement* removed = pPolicyArray->RemoveElement(*pPolicy);
                        delete removed;
                        retVal = KErrNone; 
							}
					}

				}
			}
		CleanupStack::PopAndDestroy(); // policies
		}
    return retVal;
    }


// sets (rebuilds) the Service Policy in SD from template
// @return  - a error, if at least one addition of new contents has
// failed OR KerrNone if every property was successfully reset
TInt CServicePolicy::RebuildServicePolicyFrom(MSenServicePolicy& aTemplate)
    {
    TInt retVal(KErrNone);
    TRAPD(err, retVal = RebuildServicePolicyFromL(aTemplate));
    if (err != KErrNone)
        {
        retVal = err;
        }
    return retVal;
    }

TInt CServicePolicy::RebuildServicePolicyFromL(MSenServicePolicy& /*aTemplate*/)
    {
    TInt retVal(KErrNone);
    // check for possible provider policy definition
    return retVal;
    }
    
// Returns a Policy ID value. 
// If the value not found then generate a random 
// value and assign it to the policy
TPtrC8 CServicePolicy::PolicyIdL(CSenElement* aPolicy)	//codescannerwarnings
    {
    RPointerArray<CSenBaseAttribute>& attrs = aPolicy->AttributesL();
    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
    for (TInt j=0; j < ele_count; j++)
        {
        bs = (attrs)[j];
        if(bs && bs->Name().Compare(KId) == 0)
            {
            return bs->Value();
            }
         }
    // If there is no ID for a policy, then add a random
    // Policy ID to the policy and put it into Service Description
    GenerateDeviceIdL(aPolicy);
    return PolicyIdL(aPolicy);
    }
    
//Generate a random ID
void CServicePolicy::GenerateDeviceIdL(CSenElement* aPolicy)	//codescannerwarnings
    {

    CSenGuidGen* pGuidGenerator = CSenGuidGen::NewLC();
    HBufC8* iPolicyId = pGuidGenerator->GetRandomGuid8LC();
    TPtr8 policyIdDes = iPolicyId->Des();
    policyIdDes.LowerCase();
    
    aPolicy->AddAttrL(KId, policyIdDes);
    aPolicy->AddNamespaceL(KWsuPrefix, KWsuNsUri);
    
    CleanupStack::PopAndDestroy(iPolicyId);
    CleanupStack::PopAndDestroy(pGuidGenerator);
    }

//return Policy atribute Name
TPtrC8 CServicePolicy::PolicyNameL(CSenElement* aPolicy)		//codescannerwarnings
    {
    RPointerArray<CSenBaseAttribute>& attrs = aPolicy->AttributesL();

    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
        bs = (attrs)[j];
        if(bs)

        if(bs->Name() == KName)
            {
             return bs->Value();
            }
         }
         return NULL;
    }

TPtrC8 CServicePolicy::PolicyUriL(CSenElement* aPolicy)
{

    if(aPolicy == NULL) { User::Leave(KErrGeneral); }
    
    RPointerArray<CSenBaseAttribute>& attrs = aPolicy->AttributesL();
    
    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
        bs = (attrs)[j];
        if(bs)
        {
            if(bs->Name().Compare(KMetaDataLocalName) == 0)
            {
                 return bs->Value();
            }
         }

            
        }
    return KNullDesC8();
}

TBool CServicePolicy::ValidatePolicy(CSenElement* /*aPolicy*/)
    {
    return ETrue;  
    }
CSenElement* CServicePolicy::ClientPolicy()
{
    CSenElement* pClientPolicyElement = AsElement().Element(KClientServicePolicyLocalName);
    
    if(!pClientPolicyElement)
        {
        return AsElement().Element(KSenNsUri, KClientServicePolicyLocalName);
        }
        
    return NULL;
}    
TBool CServicePolicy::ClientPolicy(const TDesC8& aKey)
{

    CSenElement* pClientPolicy = ClientPolicy();

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

}
HBufC8* CServicePolicy::ClientPolicyValue(const TDesC8& aKey)
    {
    HBufC8* retVal = NULL;        
    TInt err(KErrNone);
    CSenElement* pClientPolicy = ClientPolicy();
    if( pClientPolicy )
        {
        CSenElement* result = NULL;
        result = pClientPolicy->Element(aKey);
        if( result && result->HasContent() )
            {
            TRAP( err, retVal = result->Content().AllocL(); )
            if( err ) 
                {
                retVal = NULL;
                }
            }
        }                    
    return retVal;
    }
/*    
RFileLogger* CServicePolicy::Log() const 
    {
    return (RFileLogger*) &iLog;
    }    
*/
// End of file
