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











#include "senwspolicymanager.h"
#include "seninternalwspolicy.h"

#include <s32mem.h>
#include <s32file.h>
#include <sysutil.h>
#include <SenFacet.h>
#include <SenXmlUtils.h>
#include "sendebug.h"               // internal \Utils\inc - filelogging MACROs
#include "msencoreservicemanager.h" // internal \Framework\inc
#include "SenParser.h"
#include "SenDateUtils.h"
#include "senservicepolicy.h"
#include "wspolicyutils.h"
#include "wspolicy.h"
#include "senguidgen.h"
#include "senxmldebug.h"
#include "senlogger.h"


#include <SenServiceConnection.h> 
#include <xml/dom/xmlengnodelist.h> 
using namespace WSPolicy;
using namespace SenWSInternalPolicy;
namespace
    {
#if !defined( EKA2 ) && !defined( RD_SECURE_PRIV_DATA )
    _LIT(KWsPolicyFile,           "C:\\system\\data\\senwspolicies.xml");
#else
    _LIT(KWsPolicyFile,           "senwspolicies.xml");
#endif

    const TInt KStateParsingContainer   = 99;
    const TInt KStateParsingIdentifier  = 100;
    const TInt KStateParsingProperties  = 102;
    const TInt KStateParsingPolicy            = 104;
    const TInt KFlatBufSize                       = 128;
    
    const TUint KValidityTime = +7;
    
    }




EXPORT_C CSenWSPolicyManager* CSenWSPolicyManager::NewL(MSenCoreServiceManager& aManager)
    {
    CSenWSPolicyManager* pNew = NewLC(aManager);
    CleanupStack::Pop(pNew);
    return(pNew);
       
    }

EXPORT_C CSenWSPolicyManager* CSenWSPolicyManager::NewLC(MSenCoreServiceManager& aManager)
    {
    CSenWSPolicyManager* pNew = new (ELeave) CSenWSPolicyManager(aManager);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    
    }

EXPORT_C CSenWSPolicyManager::~CSenWSPolicyManager()
    {
    delete iPolicDataLoader;
    iPolicyArray.ResetAndDestroy();
    iConsumerArray.ResetAndDestroy();

    }
CSenWSPolicyManager::CSenWSPolicyManager(MSenCoreServiceManager& aManager) 
:iManager(aManager),
iLog((RFileLogger*)aManager.Log())
    {
    
    }
void CSenWSPolicyManager::ConstructL()
    {
        iPolicDataLoader  = CSenWSPolicyLoader::NewL(iManager);
        RefreshPolicyContainerL();	//codescannerwarnings
    }
RFileLogger* CSenWSPolicyManager::Log()
    {
    return iLog;
    }

EXPORT_C TInt CSenWSPolicyManager::RegisterWsPolicyL(CSenWSDescription* aPattern,
                                                    TInt& aErrorTo)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyManager::RegisterWsPolicyL");                
        TInt retVal(KErrGeneral);
        if(iPolicDataLoader == NULL)
        {
          iPolicDataLoader  = CSenWSPolicyLoader::NewL(iManager);
        }
        
        retVal = iPolicDataLoader->RegisterWsPolicyL(aPattern, aErrorTo);
        
        if(retVal == KErrNone)
            RefreshPolicyContainerL();	//codescannerwarnings
        
        if(iPolicDataLoader)
        {
            delete iPolicDataLoader;
            iPolicDataLoader = NULL;
        }
        return retVal;
    }

EXPORT_C TInt CSenWSPolicyManager::UnRegisterWsPolicyL(CSenWSDescription* aPattern, 
                                                        TInt& aErrorTo)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyManager::UnRegisterWsPolicyL");
        TInt retVal(KErrGeneral);
        if(iPolicDataLoader == NULL)
        {
          iPolicDataLoader  = CSenWSPolicyLoader::NewL(iManager);
        }
        
        retVal = iPolicDataLoader->UnRegisterWsPolicyL(aPattern, aErrorTo);
        
        if(retVal == KErrNone)
            RefreshPolicyContainerL();	//codescannerwarnings
        
        if(iPolicDataLoader)
        {
            delete iPolicDataLoader;
            iPolicDataLoader = NULL;
        }
        return retVal;
    
}

EXPORT_C TInt CSenWSPolicyManager::AddWsPolicyL(CSenWSDescription* aPattern, const TDesC8& aUri,
                                               const TDesC8& aPolicy, TInt& aErrorTo)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyManager::AddWsPolicyL");        
        TInt retVal(KErrGeneral);
        if(iPolicDataLoader == NULL)
        {
          iPolicDataLoader  = CSenWSPolicyLoader::NewL(iManager);
        }
        
        retVal = iPolicDataLoader->AddWsPolicyL(aPattern, aUri, aPolicy, aErrorTo);
        if(retVal == KErrNone)
            RefreshPolicyContainerL();	//codescannerwarnings
        
        if(iPolicDataLoader)
        {
            delete iPolicDataLoader;
            iPolicDataLoader = NULL;
        }
        return retVal;
    
    }

EXPORT_C TInt CSenWSPolicyManager::RemoveWsPolicyL( CSenWSDescription* aPattern, 
                                                const TDesC8& aUri, TInt& aErrorTo)
    {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyManager::RemoveWsPolicyL");
        TInt retVal(KErrGeneral);
        if(iPolicDataLoader == NULL)
        {
          iPolicDataLoader  = CSenWSPolicyLoader::NewL(iManager);
        }
        
        retVal = iPolicDataLoader->RemoveWsPolicyL(aPattern, aUri, aErrorTo);
        
        if(retVal == KErrNone)
            RefreshPolicyContainerL();  //codescannerwarnings
        
        if(iPolicDataLoader)
        {
            delete iPolicDataLoader;
            iPolicDataLoader = NULL;
        }
        return retVal;
    
    }
TInt CSenWSPolicyManager::IsConsumerExist(const TDesC8& aEndpoint, const TDesC8& aConsumerId)
{
    TInt count(iConsumerArray.Count());
    for(TInt i = 0; i< count; i++)
    {
        CConsumerContainer* consumer = iConsumerArray[i];
        if(consumer->Uri() == aEndpoint && consumer->ConsumerId() == aConsumerId)
            return i;
    }
    return KErrNotFound;
}
TInt CSenWSPolicyManager::PolicyConsumerCountL(CSenInternalWsPolicy* aInternalPolicy)	//codescannerwarnings
	{
    TInt retVal(KErrNone);
	if (aInternalPolicy)
		{
	    TPtrC8 metadata = aInternalPolicy->IdentifierL().MetadataEndpointL();	//codescannerwarnings
	    TInt count(iConsumerArray.Count());
	    for(TInt i = 0; i< count; i++)
			{
			CConsumerContainer* consumer = iConsumerArray[i];
			if(consumer->Uri() == metadata)
			    retVal++;
			}
		}    
    return retVal;
	}

EXPORT_C TInt CSenWSPolicyManager::AddPolicyConsumerL(const TDesC8& aUri, const TDesC8& aConsumerId)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyManager::AddPolicyConsumerL");
    
    TInt indx = IsConsumerExist(aUri,aConsumerId);
    if(indx == KErrNotFound)
    {
        CConsumerContainer* consumer = CConsumerContainer::NewL(aUri,aConsumerId);    
        if(consumer) 
        {
            iConsumerArray.Append(consumer);
            RefreshPolicyContainerL();	//codescannerwarnings
        }
        else
        {
            
        }
            
        
    }
    return KErrNone;
    }

EXPORT_C TInt CSenWSPolicyManager::RemovePolicyConsumerL(const TDesC8& aUri, const TDesC8& aConsumerId)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyManager::RemovePolicyConsumerL");
    TInt indx = IsConsumerExist(aUri,aConsumerId);
    if(indx > KErrNotFound)
        {
        CConsumerContainer* consumer = iConsumerArray[indx];
        if(consumer) 
            {
            TInt count (iPolicyArray.Count());
            for(TInt i = 0; i< count; i++)
                {
                CNormalizedPolicyConatiner* container =  iPolicyArray[i];
                if(container->IsSameContainer(aUri))
                   {
                    container->CountDec();
                    if(container->ConsumerCount() < 1)
                        { //we no longer ahve any consumer for this conatiner
                        iPolicyArray.Remove(i);
                        delete container;
                        container = NULL;
                        }
                    }
            }
            iConsumerArray.Remove(indx);
            delete consumer;
            consumer = NULL;
            return KErrNone;
            }
        }
    return KErrNotFound;
    }

EXPORT_C CSenElement*  CSenWSPolicyManager::WsPolicyL( CSenWSDescription* aPattern, 
                                                const TDesC8& aUri, TInt& aErrorTo)
    {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyManager::WsPolicyL");
        aErrorTo = KErrNotFound;
        CSenElement* ele = NULL;

        if(aPattern == NULL && aUri.Length() > 0) //special case for MS
            {
            TInt count (iPolicyArray.Count());
            for(TInt i = 0; i< count; i++)
                {
                CNormalizedPolicyConatiner* container =  iPolicyArray[i];
                if(container->URI() == aUri)
                    {
                    aErrorTo = KErrNone;
                    return container->Policy();
                    }
                }
                
            }
    
        if(aUri.Length() == 0)
            {
            ele = PolicyFromSDL(aPattern);	//codescannerwarnings
            }
        else
            {
            ele = PolicyFromUri(aPattern, aUri);   
            }
         
        
        if(ele != NULL)
            aErrorTo = KErrNone;
        
        return ele;
    }
    
CSenElement* CSenWSPolicyManager::PolicyByProviderId(const TDesC8& aProviderId, TInt&  aErrorTo)
{
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyManager::PolicyByProviderId");
    TInt count (iPolicyArray.Count());
    for(TInt i = 0; i< count; i++)
    {
       CNormalizedPolicyConatiner* container =  iPolicyArray[i];
       if(container->ProviderId() == aProviderId)
       {
       CSenElement* ele = container->Policy();
       if(ele)
           {
            aErrorTo = KErrNone;
            return ele;
           }
       }
    }
    aErrorTo = KErrNotFound;
    return NULL;
  
}

CSenElement* CSenWSPolicyManager::PolicyFromUri(CSenWSDescription* /*aPolicy*/, const TDesC8& aUri)
{
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyManager::PolicyFromUri");
    TInt count (iPolicyArray.Count());
    for(TInt i = 0; i< count; i++)
    {
       CNormalizedPolicyConatiner* container =  iPolicyArray[i];
       if(container->IsSameContainer(aUri))
       {
        return container->Policy();
       }
    }
    return NULL;
}

CSenElement* CSenWSPolicyManager::PolicyFromSDL(CSenWSDescription* aPolicy)	//codescannerwarnings
{
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyManager::PolicyFromSD");

    if( aPolicy )
        {
        CSenServicePolicy* pol = (CSenServicePolicy*)aPolicy->ServicePolicy();
        CSenElement& servicePolicy = pol->AsElement();
        CSenElement* clientPolicy = servicePolicy.Element( KClientPolicyLocalName );
        if(clientPolicy)
            {
            RPointerArray<CSenElement> metaDataPoliciesUris;
            CleanupClosePushL(metaDataPoliciesUris);
            clientPolicy->ElementsL(metaDataPoliciesUris, KMetadataEndpointLocalname);
            TInt count(metaDataPoliciesUris.Count());
            if(count> 0)
                {
                CSenElement* metaData = NULL;
                for(TInt i=0; i<count; i++)
                    {
                    metaData = metaDataPoliciesUris[i];
                    if(metaData && metaData->HasContent())
                        {
                         TPtrC8 uriName = metaData->Content();
                         CleanupStack::PopAndDestroy(&metaDataPoliciesUris);            
                         return PolicyFromUri(aPolicy, uriName);                   
                        }
                    }
                }
             CleanupStack::PopAndDestroy(&metaDataPoliciesUris);            
            }
        }
    return NULL;
    }


TInt CSenWSPolicyManager::UpdatePolicyContainerL(CSenInternalWsPolicy* aInternalPolicy) //codescannerwarnings
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyManager::UpdatePolicyContainer");
        TInt indx(IsContainerL(aInternalPolicy));
        CNormalizedPolicyConatiner* container =  iPolicyArray[indx];
        if(container && container->URI().Compare(aInternalPolicy->IdentifierL().MetadataEndpointL()) == 0)	//codescannerwarnings
        {
            TBool retval = container->UpdateL(aInternalPolicy);
            container->SetConsumerCount(PolicyConsumerCountL(aInternalPolicy));//codescannerwarnings
            return KErrNone;
        }
    return KErrNotFound;
    
    }
TInt CSenWSPolicyManager::IsContainerL(CSenInternalWsPolicy* aInternalPolicy) //codescannerwarnings
    {
    TInt count (iPolicyArray.Count());
    for(TInt i = 0; i< count && aInternalPolicy; i++)
    {
        CNormalizedPolicyConatiner* container =  iPolicyArray[i];
        TPtrC8 endpoint = aInternalPolicy->IdentifierL().MetadataEndpointL();	//codescannerwarnings
        if(container)
        {
            if(container->URI().Compare(endpoint) == 0)
                return i;
        }
    }
    return KErrNotFound;
    
    }    
    
TInt CSenWSPolicyManager::CreatePolicyContainerL(CSenInternalWsPolicy* aInternalPolicy)	//codescannerwarnings
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyManager::CreatePolicyContainer");
    TInt retVal(KErrNone);
    TInt indx = IsContainerL(aInternalPolicy);	//codescannerwarnings
    if(indx == KErrNotFound)
        {
        TPtrC8 metaendpoint = aInternalPolicy->IdentifierL().MetadataEndpointL();	//codescannerwarnings
        CSenElement* metadta = aInternalPolicy->IdentifierL().MetadataEndpointElementL();	//codescannerwarnings
        {
        
        if(metadta && metadta->HasContent())
            {
            TPtrC8 endpoint = metadta->Content();
            CNormalizedPolicyConatiner* container = 
            CNormalizedPolicyConatiner::NewL(aInternalPolicy, endpoint);
            TPtrC8 metaendpoint = aInternalPolicy->IdentifierL().MetadataEndpointL();	//codescannerwarnings
            
            if(container && container->IsSameContainer(metaendpoint))
                {
                container->UpdateL(aInternalPolicy);
                retVal = iPolicyArray.Append(container);
                }

            }
        }
        
        }
    return retVal;
    
    }    

TInt CSenWSPolicyManager::RefreshPolicyContainerL()	//codescannerwarnings
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyManager::RefreshPolicyContainer");
    TInt retVal(KErrGeneral);
    if(iPolicDataLoader == NULL)
        {
        iPolicDataLoader  = CSenWSPolicyLoader::NewL(iManager);
        }
        
    RSenWSPolicyArray policies;
    CleanupClosePushL(policies);
    retVal = iPolicDataLoader->InternalWsPoliciesL(policies);
    if(retVal == KErrNone)
        {
        TInt count(policies.Count());
        for(TInt i = 0; i< count; i++)
            {
            CSenInternalWsPolicy* policy =  policies[i];
            TInt consumerCount = PolicyConsumerCountL(policy);//codescannerwarnings
            if( consumerCount > 0)
            {
            //for every internal policy we should have a container here            
            if(policy && IsContainerL(policy) != KErrNotFound)	//codescannerwarnings
                {//We have alreadya  containere
                // we already have this Policy container
                // we just got a new consumer for this policy
                // so the count will be increased
                retVal = UpdatePolicyContainerL(policy); //codescannerwarnings
                if(retVal == KErrNone)
                { //we got proper update
                    
                }
                }
            else
                {//create a new container
				retVal = CreatePolicyContainerL(policy); //codescannerwarnings
                }
                
            }
            }
        }

    CleanupStack::PopAndDestroy(&policies);
    
    if(iPolicDataLoader)
        {
        delete iPolicDataLoader;
        iPolicDataLoader = NULL;
        }

    return retVal;
        
    }

EXPORT_C TInt CSenWSPolicyManager::ExpiredPoliciesL(RExpiredPolicyArray& aArray,  TInt& aErrorTo)
{
TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyManager::ExpiredPoliciesL");    
    TTime now;
    now.UniversalTime();
    
    TInt count (iPolicyArray.Count());
    
    for(TInt i = 0; i< count; i++)
    {
        CNormalizedPolicyConatiner* container =  iPolicyArray[i];
        if(container && container->ValidUntil() < now)
        {
            CExpiredPolicy* expir = CExpiredPolicy::NewL(container->Contract(),
                                                    container->Endpoint(),
                                                    container->URI(), container->ProviderId()); 
            if(expir)                                                    
            {
                aArray.Append(expir);
                aErrorTo = KErrNone;
            }
        }
    }
    
    if(aArray.Count() > 0)
        return KErrNone;
    else
        return KErrNotFound;
    
}

TInt CSenWSPolicyManager::AddWsPolicy1L(CSenWSDescription* aPattern, const TDesC8& aUri,
                                               const TDesC8& aPolicy, TInt& aErrorTo)
{
    return AddWsPolicyL(aPattern, aUri, aPolicy, aErrorTo);   
}

TInt CSenWSPolicyManager::RemoveWsPolicy1L( CSenWSDescription* aPattern, 
                                                const TDesC8& aUri, TInt& aErrorTo)
{
    return RemoveWsPolicyL(aPattern, aUri, aErrorTo);  
}
                                                                                                                                                                                               
TInt CSenWSPolicyManager::RegisterWsPolicy1L(CSenWSDescription* aPattern,
                                                    TInt& aErrorTo)
{
   return RegisterWsPolicyL(aPattern, aErrorTo);   
}

TInt CSenWSPolicyManager::UnRegisterWsPolicy1L(CSenWSDescription* aPattern,
                                                    TInt& aErrorTo)
{
    return UnRegisterWsPolicyL(aPattern, aErrorTo);  
}
                                                    
CSenElement*  CSenWSPolicyManager::WsPolicy1L( CSenWSDescription* aPattern, 
                                                const TDesC8& aUri, TInt& aErrorTo)
{
    return WsPolicyL(aPattern, aUri, aErrorTo);  
}

CSenElement*  CSenWSPolicyManager::WsPolicy1L( CSenWSDescription* aPattern, 
                                                TInt& aErrorTo)
{
    return WsPolicyL(aPattern, KNullDesC8(), aErrorTo);  
}
CSenElement*  CSenWSPolicyManager::WsPolicy1L( const TDesC8& aProviderId, TInt& aErrorTo)
{
    return PolicyByProviderId(aProviderId, aErrorTo);
}
TInt CSenWSPolicyManager::AddPolicyConsumer1L(const TDesC8& aUri, const TDesC8& aConsumerId)
{
    return AddPolicyConsumerL(aUri,aConsumerId);    
}

TInt CSenWSPolicyManager::RemovePolicyConsumer1L(const TDesC8& aUri, const TDesC8& aConsumerId)
{
    return RemovePolicyConsumerL(aUri,aConsumerId);    
}


CNormalizedPolicyConatiner* CNormalizedPolicyConatiner::NewL(CSenInternalWsPolicy* aPolicy, const TDesC8& aUri)
    {
    CNormalizedPolicyConatiner* pNew = CNormalizedPolicyConatiner::NewLC(aPolicy, aUri);
    CleanupStack::Pop(pNew);
    return(pNew);
        
    }
CNormalizedPolicyConatiner* CNormalizedPolicyConatiner::NewLC(CSenInternalWsPolicy* aPolicy, const TDesC8& aUri)
    {
    CNormalizedPolicyConatiner* pNew = new (ELeave) CNormalizedPolicyConatiner();
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aPolicy, aUri);
    return pNew;
 
    }
CNormalizedPolicyConatiner::~CNormalizedPolicyConatiner()
    {
    delete iContract; 
    delete iEndpoint; 
    delete iUri; 
    delete iProviderId;
    delete iPolicy;
    }
CNormalizedPolicyConatiner::CNormalizedPolicyConatiner()
    {
        
    }
void CNormalizedPolicyConatiner::ConstructL(CSenInternalWsPolicy* aPolicy, const TDesC8& aUri)
    {
    TPtrC8 contract = aPolicy->IdentifierL().ContractL();	//codescannerwarnings
    TPtrC8 endpoint = aPolicy->IdentifierL().EndpointL();	//codescannerwarnings
    TPtrC8 providerId = aPolicy->IdentifierL().ProviderIdL();	//codescannerwarnings
    TTime valid = aPolicy->ValidUntilL(aUri);
    
    iCount = 1;
    iContract = contract.AllocL();
    iEndpoint = endpoint.AllocL();
    iProviderId= providerId.AllocL();
    iUri = aUri.AllocL();
    iValidUntil = valid;
    }

TBool CNormalizedPolicyConatiner::UpdateL(CSenInternalWsPolicy* aInternalPolicy)
    {
    TTime valid = aInternalPolicy->ValidUntilL(URI());
    CSenElement* normPolicy = NULL;
    if(valid  > iValidUntil ) //we have new expiry time
    {
        CSenElement*  policy = aInternalPolicy->WsPolicyByUriL(URI());
        TInt length = 0;
        if(policy)
        {
        CBufFlat* pBuf = NULL;
        pBuf = CBufFlat::NewL(KFlatBufSize);
        CleanupStack::PushL(pBuf);
        RBufWriteStream bufWs(*pBuf);
        CleanupClosePushL(bufWs);
        policy->WriteAsXMLToL(bufWs);
        TPtr8 p8 = pBuf->Ptr(0);
        length = p8.Length();
        CleanupStack::PopAndDestroy(2); // bufWs (close), pBuf
        }

        if(length < 100000)        
        {
        CWSPolicy* wspolicy = CWSPolicy::NewLC();
        TRAPD(error, normPolicy = wspolicy->NormalizePolicy2L(*policy, NULL);)
        
        CleanupStack::PopAndDestroy(wspolicy);
        if(error == KErrNone && normPolicy)
        {
            if(iPolicy)
                delete iPolicy;
            
            iPolicy = normPolicy;
            iValidUntil   = valid;            
            return ETrue;            
        }
        //set this policy as new policy
    }
    }
    else if(valid == iValidUntil)
    {
        if(iPolicy == NULL)
        {
        CSenElement*  policy = aInternalPolicy->WsPolicyByUriL(URI());
        TInt length = 0;
        if(policy)
        {
        CBufFlat* pBuf = NULL;
        pBuf = CBufFlat::NewL(KFlatBufSize);
        CleanupStack::PushL(pBuf);
        RBufWriteStream bufWs(*pBuf);
        CleanupClosePushL(bufWs);
        policy->WriteAsXMLToL(bufWs);
        TPtr8 p8 = pBuf->Ptr(0);
        length = p8.Length();
        CleanupStack::PopAndDestroy(2); // bufWs (close), pBuf
        }

        if(length < 100000)        
        {
        CWSPolicy* wspolicy = CWSPolicy::NewLC();
        TRAPD(error, normPolicy = wspolicy->NormalizePolicy2L(*policy, NULL);)    
        CleanupStack::PopAndDestroy(wspolicy);
        if(error == KErrNone && normPolicy)
        {            
            iPolicy = normPolicy;
            iValidUntil   = valid;            
            return ETrue;            
        }
        //set this policy as new policy
            
        }
    }
    }
    return EFalse;
    }   
TBool CNormalizedPolicyConatiner::IsSameContainer(const TDesC8& aUri)
    {
    
    if(aUri.Compare(URI()) == 0)
        {
            return ETrue;    
        }
       return EFalse;
    }
void CNormalizedPolicyConatiner::CountInc()
{
    iCount++;    
}
void CNormalizedPolicyConatiner::CountDec()
{
    iCount--;
    if(iCount < 0)
        iCount = 0;
}
void CNormalizedPolicyConatiner::SetConsumerCount(TInt aCount)
{
    iCount = aCount;
}
TInt  CNormalizedPolicyConatiner::ConsumerCount()
    {
    return iCount;
    }
TPtrC8  CNormalizedPolicyConatiner::Contract()
    {
    return iContract->Des();
    }
TPtrC8  CNormalizedPolicyConatiner::Endpoint()
    {
    return iEndpoint->Des();
    }
TPtrC8  CNormalizedPolicyConatiner::ProviderId()
    {
    return iProviderId->Des();
    }
TPtrC8  CNormalizedPolicyConatiner::URI()
    {
    return iUri->Des();
    }
TTime  CNormalizedPolicyConatiner::ValidUntil()
    {
    return iValidUntil;
    }
CSenElement*  CNormalizedPolicyConatiner::Policy()
    {
    return iPolicy;
    }

    
CSenWSPolicyLoader* CSenWSPolicyLoader::NewL(MSenCoreServiceManager& aManager)
    {
    CSenWSPolicyLoader* pNew = NewLC(aManager);
    CleanupStack::Pop(pNew);
    return(pNew);
    }

CSenWSPolicyLoader* CSenWSPolicyLoader::NewLC(MSenCoreServiceManager& aManager)
    {
    CSenWSPolicyLoader* pNew = new (ELeave) CSenWSPolicyLoader(aManager);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

CSenWSPolicyLoader::CSenWSPolicyLoader(MSenCoreServiceManager& aManager)
: iManager(aManager),
  iLog((RFileLogger*)aManager.Log()),
  iMaxPolicyID(0)
    {
    }

CSenWSPolicyLoader::~CSenWSPolicyLoader()
    {
    SaveDB();
    
    TInt count = iPolicyArray.Count();
    for (TInt i=0; i < count; i++)
        {
        iPolicyArray[i].Close();
        }

    iPolicyArray.Close();
    
    delete ipPolicyContainer;
    delete ipPolicyIdentifier;
    delete ipWSpolicy;
   
    }

void CSenWSPolicyLoader::ConstructL()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyLoader::ConstructL");
    BaseConstructL(KSenWsPolicies);
    LoadDBL();	//codescannerwarnings
    }

RFileLogger* CSenWSPolicyLoader::Log()
    {
    return iLog;
    }
    
void CSenWSPolicyLoader::StartElementL(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
    {
    const TPtrC8 localName   = aLocalName;
    
    switch (iState)
        {
        case KSenStateSave:
            {

            if (aLocalName.Compare(KWsPolicyContainer) == 0)
                {
                if ( ipPolicyContainer )
                    {
                   // CSenElement* element  = ipPolicyContainer->ExtractElement();
                    delete ipPolicyContainer;
                    ipPolicyContainer = NULL;
                    }
                    

                CSenElement& element = AsElement();

                TPtrC8 prefix(KNullDesC8);
                if(aQName.Length()>0)
                    {
                    TInt colon = aQName.Locate(':');
                    if(colon>0) // Note: 0 also treated as no prefix
                        {
                        prefix.Set(aQName.Ptr(), colon);
                        }
                    }
                
				ipPolicyContainer = CSenBaseFragment::NewL(
					                         aNsUri, aLocalName,
					                         aQName, aAttributes,
					                         element);
				
				ipPolicyContainer->AsElement().SetNamespaceL(prefix, aNsUri);	                         
                iState = KStateParsingContainer;
                }

            break;
            }
        case KStateParsingContainer: 
            {
            if (aLocalName.Compare(KSenWsPolicyIdentifierLocalname) == 0)
                {
                if ( ipPolicyIdentifier )
                    {
                    //CSenElement* element  = ipPolicyIdentifier->ExtractElement();
                    delete ipPolicyIdentifier;
                    ipPolicyIdentifier = NULL;
                    }

                CSenElement& element  = ipPolicyContainer->AsElement();
                TPtrC8 prefix(KNullDesC8);
                if(aQName.Length()>0)
                    {
                    TInt colon = aQName.Locate(':');
                    if(colon>0) // Note: 0 also treated as no prefix
                        {
                        prefix.Set(aQName.Ptr(), colon);
                        }
                    }

				ipPolicyIdentifier = CSenWSPolicyIdentifier::NewL(
					                         aNsUri, aLocalName,
					                         aQName, aAttributes,
					                         element);

                iState = KStateParsingIdentifier;
                ipPolicyIdentifier->AsElement().SetNamespaceL(prefix, aNsUri);
                DelegateParsingL(*ipPolicyIdentifier);
                }                
            else if(aLocalName.Compare(KSenWSPolicyMetadata) == 0)
                {
                // Parsing Policy
                // 
                // <=> Container can have only two elements:
                //     1) Identifier,
                //     2) PolicyMetadata
                
                if ( ipWSpolicy )
                    {
                    CSenElement* element  = ipWSpolicy->ExtractElement();
                    delete ipWSpolicy;
                    ipWSpolicy = NULL;
                    }
                

                CSenElement& element  = ipPolicyContainer->AsElement();

				ipWSpolicy = CSenInternalWsPolicy::NewL(
					                         aNsUri, aLocalName,
					                         aQName, aAttributes,
					                         element
					                         );
		        ipWSpolicy->SetIdentifier(ipPolicyIdentifier);
		        ipPolicyIdentifier = NULL;

                iState = KStateParsingPolicy;

                DelegateParsingL(*ipWSpolicy);
                }
              
            break;
            }
        default:
            {

            if (aLocalName.Compare(KSenWsPolicies) == 0)
                {
                TInt count(aAttributes.Count());

                for (TInt i=0; i<count; i++)
                    {
                    TPtrC8 localName = aAttributes[i].Attribute().LocalName().DesC();
                    TPtrC8 value = aAttributes[i].Value().DesC();
                    
                    if ( localName == KWsPoliciesMaxID )
                        {
                        TLex8 lex;
                        lex.Assign(value);
                        lex.Val( iMaxPolicyID );                        
                        break;
                        }
                    }
                }
                
            CSenBaseFragment::StartElementL(aNsUri, aLocalName, aQName, aAttributes);
            break;
            }
        }
    }

void CSenWSPolicyLoader::EndElementL(const TDesC8& aNsUri,
                                      const TDesC8& aLocalName,
                                      const TDesC8& aQName)
    

    {
    switch(iState)
        {
        case KStateParsingIdentifier:
            {
            iState = KStateParsingContainer;
            break;
            }
        case KStateParsingProperties:
            {
            iState = KStateParsingContainer;
            break;
            }           
        case KStateParsingPolicy:
            {

            RSenWSPolicyPtr policyPtr;
            policyPtr.OpenL(ipWSpolicy);
            CleanupClosePushL(policyPtr);
            iPolicyArray.Append(policyPtr);
            CleanupStack::Pop(&policyPtr);    
            
            CSenElement& element = this->AsElement().AddElementL(KWsPolicyContainer);
            element.AddElementL(ipWSpolicy->IdentifierL().AsElement());	//codescannerwarnings
            ipWSpolicy->IdentifierL().AsElement().SetParent(&element);	//codescannerwarnings
            ipWSpolicy->IdentifierL().SetIdL(NextId());	//codescannerwarnings
            element.AddElementL(ipWSpolicy->AsElement());
            ipWSpolicy->AsElement().SetParent(&element);

                
            ipWSpolicy = NULL;

            iState = KStateParsingContainer;            
            break;
            }
        case KStateParsingContainer:
            {
            if ( ipPolicyContainer )
                {
                CSenElement& element  = ipPolicyContainer->AsElement();
                RPointerArray<CSenElement>& children = element.ElementsL();
                    
                // Element should always have 2 child elements:
                //         1) Identifier and
                //         2) PolicyMetadata
                if ( children.Count() < 2 )
                    {
                    if ( ipPolicyIdentifier )
                        {
                       // CSenElement* element  = ipPolicyIdentifier->ExtractElement();
                        delete ipPolicyIdentifier;
                        ipPolicyIdentifier = NULL;
                        }
                    if ( ipWSpolicy )
                        {
                       // CSenElement* element  = ipWSpolicy->ExtractElement();
                        delete ipWSpolicy;
                        ipWSpolicy = NULL;
                        }
                    }

                delete ipPolicyContainer;
                ipPolicyContainer = NULL;
                }
                
            iState = KSenStateSave;
            break;
            }
        default:
            {
            CSenBaseFragment::EndElementL(aNsUri, aLocalName, aQName);
            break;
            }
        }
    }    
    
TInt CSenWSPolicyLoader::LoadDBL()	//codescannerwarnings
    {

    TInt retVal(KErrNone);
    TRAP(retVal, LoadFromL(KWsPolicyFile));

    return retVal;
    }

void CSenWSPolicyLoader::LoadFromL(const TDesC& aFile)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyLoader::LoadFromL");            
    RFs fss;
    User::LeaveIfError(fss.Connect());
    CleanupClosePushL(fss);

    TInt leaveCode(KErrNone);

#if defined( EKA2 ) || defined( RD_SECURE_PRIV_DATA )
    TBuf<KMaxPath> file;
    fss.CreatePrivatePath(EDriveC);
    fss.PrivatePath(file);
    file.Append(aFile);

    RFile  xmlFile;
    leaveCode = xmlFile.Open(fss , file, EFileRead );
    if(leaveCode == KErrNone)
        {
        TInt size;
        xmlFile.Size(size);

        HBufC8 *pBuf=HBufC8::NewL(size);	//CodeScannerWarnings

        CleanupStack::PushL(pBuf);
        TPtr8 buf8ptr(pBuf->Des());
        xmlFile.Read(buf8ptr,size);
        iXmlReader = CSenXmlReader::NewL(KXmlParserMimeType); // use libxml2 sax parser
        TRAP(leaveCode, ParseL(buf8ptr));
        delete iXmlReader;
        iXmlReader = NULL;  //CodeScannerWarnings
        CleanupStack::PopAndDestroy(pBuf);
        }
    xmlFile.Close();        
#else
    TRAP(leaveCode, ParseL(fss, aFile, *this));
#endif

    iState = KSenStateSave;
    
#ifdef _SENDEBUG
    if(leaveCode == KErrNotFound)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"- The senwspolicies.xml does not exist!");
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"- New file will be generated.");
        }
    else if(leaveCode != KErrNone)
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel, _L8("- Parsing of senwspolicies.xml leaved: %d"), leaveCode));
        }
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"- Policy database (senwspolicies.xml) successfully read.");
        }
#else
    leaveCode=0; // this is a decision: we are not interested
                 // of invalid input in read/parse phase
#endif

    SaveDB(); // Always validatby saving
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyLoader::LoadL - End");        
    CleanupStack::PopAndDestroy(); // fss
    }

TInt CSenWSPolicyLoader::SaveDB()
    {
    TInt retVal(KErrNone);
    TRAP(retVal, SaveToL(KWsPolicyFile));
    return retVal;
    }
TInt CSenWSPolicyLoader::MergeToL()
{
//    CSenWSPolicyDBLoader* dbLoader = CSenWSPolicyDBLoader::NewL(iManager); 
return KErrNone;   
}
TInt CSenWSPolicyLoader::SaveToL(const TDesC& aFile)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyLoader::SaveToL");        
    TInt count = iPolicyArray.Count();
    TInt maxId = 0;
    for (TInt i=0; i<count; i++)
        {
        if ( iPolicyArray[i].Policy()->IdentifierL().IdL() > maxId )	//codescannerwarnings
            {
            maxId = iPolicyArray[i].Policy()->IdentifierL().IdL();	//codescannerwarnings
            }
        }
    iMaxPolicyID = maxId;
//    UpdateMaxId();

    // First, collect everything into MEMORY
    CBufFlat *pBuf = CBufFlat::NewL(KFlatBufSize);
    CleanupStack::PushL(pBuf);

    RBufWriteStream bufWs(*pBuf);
    CleanupClosePushL(bufWs);

    AsElement().WriteAsXMLToL(bufWs);

    TPtrC8 p8 = pBuf->Ptr(0);

    CleanupStack::PopAndDestroy(1); // bufWs

    // Everything in MEMORY ok, prepare to write into file
    RFs fss;
    User::LeaveIfError(fss.Connect());
    CleanupClosePushL(fss);

    RFileWriteStream fileOutStream;
    CleanupClosePushL(fileOutStream);

    if(!SysUtil::FFSSpaceBelowCriticalLevelL(&fss, p8.Length()) )
        {
        //Data caging 2 implementation
#if defined( EKA2 ) || defined( RD_SECURE_PRIV_DATA )
        TBuf<KMaxPath> file;
        fss.CreatePrivatePath(EDriveC);
        fss.PrivatePath(file);
        file.Append(aFile);
        fileOutStream.Replace(fss, file, EFileWrite);
#else
        fileOutStream.Replace(fss, aFile, EFileWrite);
#endif
        // finally write the UTF-8 into the file.
        fileOutStream.WriteL(p8);
        }

    CleanupStack::PopAndDestroy(3); // fileOutStream, fss, pBuf
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyLoader::SaveToL - End");        
    return KErrNone;
    }

TInt CSenWSPolicyLoader::InternalWsPoliciesL(RSenWSPolicyArray& aPolicies)
    {

    TInt retVal(KErrNotFound);

    TInt count = iPolicyArray.Count();
    
    for (TInt i=0; i<count; i++)
        {
        aPolicies.AppendL(iPolicyArray[i].Policy());                
        }

    if ( aPolicies.Count() == 0 )
        {
        return retVal;
        }

    return KErrNone;
    
    }
TInt CSenWSPolicyLoader::InternalWsPolicyL(CSenWSDescription& aPattern,
								         RSenWSPolicyArray& aPolicies)
    {
    
    TInt retVal(KErrNotFound);

    TInt count = iPolicyArray.Count();
    
    for (TInt i=0; i<count; i++)
        {
        if ( iPolicyArray[i].Policy()->IsApplicableL(aPattern) )	//codescannerwarnings
            {
            aPolicies.AppendL(iPolicyArray[i].Policy());                
            }
        }

    if ( aPolicies.Count() == 0 )
        {
        return retVal;
        }

    return KErrNone;
    }
TInt CSenWSPolicyLoader::InternalWsPolicyL(const TDesC8& aUri,
								         RSenWSPolicyArray& aPolicies)
    {
    
    TInt retVal(KErrNotFound);

    TInt count = iPolicyArray.Count();
    
    for (TInt i=0; i<count; i++)
        {
        if ( iPolicyArray[i].Policy()->IsApplicableL(aUri) )	//codescannerwarnings
            {
            aPolicies.AppendL(iPolicyArray[i].Policy());                
            }
        }

    if ( aPolicies.Count() == 0 )
        {
        return retVal;
        }

    return KErrNone;
    }

TInt CSenWSPolicyLoader::VerifyPolicyContentsL(CSenInternalWsPolicy* apInternalPolicy, TInt& aErrorTo)	//codescannerwarnings
{
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyLoader::VerifyPolicyContents Final");    
    TInt retVal(KErrNotFound);

    TInt count = iPolicyArray.Count();
    TPtrC8 uri = apInternalPolicy->IdentifierL().MetadataEndpointL();
    for (TInt i=0; i<count; i++)
        {
        CSenInternalWsPolicy* pIntPolicy = iPolicyArray[i].Policy();
        if(pIntPolicy)
        {
            TPtrC8 pUri = pIntPolicy->IdentifierL().MetadataEndpointL();
            if(uri == pUri)
            {
                TPtrC8 contract = apInternalPolicy->IdentifierL().ContractL();	//codescannerwarnings
                TPtrC8 endpoint = apInternalPolicy->IdentifierL().EndpointL();	//codescannerwarnings
                TPtrC8 pContract = pIntPolicy->IdentifierL().ContractL();	//codescannerwarnings
                TPtrC8 pEndpoint = pIntPolicy->IdentifierL().EndpointL();	//codescannerwarnings
                if(contract == pContract && endpoint== pEndpoint)
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"same policy");    
                    //same do nuthing
                }
                else
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"different policy");    
                aErrorTo = KErrNone;
                CSenElement*  policy = pIntPolicy->WsPolicyByUriL(pUri);
                CSenElement*  policyNew = apInternalPolicy->WsPolicyByUriL(pUri);
                if(policy)
                {
                    if(policyNew == NULL) //no existing policy
                    {
                        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"different policy, add");    
                        retVal = apInternalPolicy->AddMetadataPolicyL(policy);    
                    }
                    else //existing policy so verify it 
                    {
                        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"different policy, update");    
                        retVal = apInternalPolicy->UpdateMetadataPolicyL(policy);
                    // if the Metadata of Policy (aElement) is already euqal to 
                    }                
                }
                }
            }
            
        }
        
        }
        return retVal;
        
}
RSenWSPolicyPtr CSenWSPolicyLoader::AddWsPolicyL(CSenInternalWsPolicy* apInternalPolicy,
                                                        TInt& aErrorTo)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyLoader::AddWsPolicyL Final");    
    if ( !apInternalPolicy )
        {
        SENDEBUG_L("AddPolicyL(*) - Illegal argument: NULL");
        aErrorTo = KErrArgument;
        return RSenWSPolicyPtr();
        }
        
    CleanupStack::PushL(apInternalPolicy);
    RSenWSPolicyPtr policyPtr;
    policyPtr.OpenL(apInternalPolicy);
    CleanupStack::Pop(apInternalPolicy);
    CleanupClosePushL(policyPtr);
    iPolicyArray.AppendL(policyPtr);
    CleanupStack::Pop(&policyPtr);
    
    CSenElement& element = this->AsElement().AddElementL(KWsPolicyContainer);
    element.AddElementL(apInternalPolicy->IdentifierL().AsElement());	//codescannerwarnings
    apInternalPolicy->IdentifierL().AsElement().SetParent(&element);	//codescannerwarnings
    apInternalPolicy->IdentifierL().SetIdL(NextId());	//codescannerwarnings
    element.AddElementL(apInternalPolicy->AsElement());
    apInternalPolicy->AsElement().SetParent(&element);
    
    VerifyPolicyContentsL(apInternalPolicy,aErrorTo);	//codescannerwarnings
    SaveDB();
    
    aErrorTo = KErrNone;
    return policyPtr;
    }
TInt CSenWSPolicyLoader::AddWsPolicyL(const TDesC8& aUri, CSenElement* aPolicy, TInt& aErrorTo)
{
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyLoader::AddWsPolicyL as Element");
    TInt retVal(KErrNotFound);
    RSenWSPolicyArray array;
    CleanupClosePushL(array);
    aErrorTo = InternalWsPolicyL(aUri,array);
    
    //array should have only one policy
    TInt count(array.Count()); 
    TPtrC8 metaendpoint = CWSPolicyUtils::PolicyUriL(aPolicy);
    
    for (TInt i = 0; i< count; i++)   
    {
        CSenInternalWsPolicy* ptr = array[i];
        CSenWSPolicyIdentifier& identifier = ptr->IdentifierL();	//codescannerwarnings
        if(identifier.MetadataEndpointL() == metaendpoint)	//codescannerwarnings
        {

        CSenElement* policy = ptr->WsPolicyByUriL(metaendpoint);
        if(policy == NULL) //no existing policy
        {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyLoader::AddWsPolicyL Add");
            retVal = ptr->AddMetadataPolicyL(aPolicy);    
        }
        else //existing policy so verify it 
        {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyLoader::AddWsPolicyL Replace");
            retVal = ptr->UpdateMetadataPolicyL(aPolicy);
        // if the Metadata of Policy (aElement) is already euqal to 
        }
        SaveDB();            
            
        }

        
    }
    CleanupStack::PopAndDestroy(&array);
    
    return retVal; 
}
TInt CSenWSPolicyLoader::AddWsPolicyL(CSenWSDescription* /*aPattern*/, const TDesC8& aUri,
                                               const TDesC8& aPolicy, TInt& aErrorTo)
{
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyLoader::AddWsPolicyL");
    TInt retVal(KErrNotFound);
    CSenElement* policy = CreatePolicyElementL(aUri, aPolicy);
    if(policy)
    {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyLoader::AddWsPolicyL adding Policy as string");
        CleanupStack::PushL(policy);
        retVal = AddWsPolicyL(aUri, policy, aErrorTo);
        CleanupStack::PopAndDestroy(policy);
        policy = NULL;        
    }
    else
    {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"Not a Valid xml policy to add ");
    }
    return retVal; 
}
TInt CSenWSPolicyLoader::RemoveWsPolicyL( CSenWSDescription* aPattern, 
                                                    const TDesC8& aUri, TInt& aErrorTo)
{
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyLoader::RemoveWsPolicyL");
    TInt retVal(KErrNotFound);

    RSenWSPolicyArray array;
    CleanupClosePushL(array);
    aErrorTo = InternalWsPolicyL(*aPattern,array);
    
    //array should have only one Entry in array i.e. Matching Service
    TInt count(array.Count()); 
    for (TInt i = 0; i< count; i++)   
    {
        CSenInternalWsPolicy* ptr = array[i];
        if(aUri.Length() == 0)
            {
            // delete all policies
           //delete all metadatas
           
            }
        else
            {
            CSenElement* policy = ptr->WsPolicyByUriL(aUri);
            if(policy) //we have policy so remove it
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"Removing CSenWSPolicyLoader::RemoveWsPolicyL");
                aErrorTo = ptr->RemoveMetadataPolicyL(aUri);
                retVal = KErrNone;    
                }
                
            }
    }

    CleanupStack::PopAndDestroy(&array);
    SaveDB(); 
    return retVal;
}
    
TInt CSenWSPolicyLoader::NextId()
    {
    iMaxPolicyID++;

    UpdateMaxId();    
    
    return iMaxPolicyID;
    }
    
void CSenWSPolicyLoader::UpdateMaxId()
    {
/*    
    TBool found = EFalse;
    CSenElement& element = AsElement();
    RPointerArray<CSenBaseAttribute>& attrs = element.AttributesL();
    
    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for(TInt j=0; j < ele_count; j++)
        {
            
        bs = (attrs)[j];
        if(bs)
            {
            if(bs->Name().Compare(KWsPoliciesMaxID) == 0)
                {
                TBuf8<KFlatBufSize> buffer;
                buffer.Num(iMaxPolicyID);
                bs->SetValueL(buffer);
                found = ETrue;
                }
            }
        }
        
    if ( !found )
        {
        TBuf8<KFlatBufSize> buffer;
        buffer.Num(iMaxPolicyID);
        element.AddAttributeL(KWsPoliciesMaxID, buffer);
        }
*/
    }
    
TInt CSenWSPolicyLoader::RegisterWsPolicyL(CSenInternalWsPolicy* aInternalPolicy,
                                                    TInt& aErrorTo)
{
	TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyLoader::RegisterWsPolicyL2");
    if(aInternalPolicy == NULL)
    {
        aErrorTo = KErrArgument;
        return KErrGeneral;
    }
    
    TInt count = iPolicyArray.Count();
    TInt isPresent(KErrNotFound);

    
    for (TInt i=0; i<count; i++)
        {
        CSenInternalWsPolicy* internalPolicy = iPolicyArray[i].Policy();
        TBool retVal = internalPolicy->IsApplicableL(aInternalPolicy);	//codescannerwarnings
        if(retVal)
            {
            isPresent = KErrNone;
            break;
            }
        }

    if(isPresent == KErrNotFound)
    {
    	TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"Adding CSenWSPolicyLoader::RegisterWsPolicyL");
        AddWsPolicyL(aInternalPolicy, aErrorTo);
        return KErrNone;
    }
    
    return KErrAlreadyExists;
        
}

        
TInt CSenWSPolicyLoader::RegisterWsPolicyL(CSenWSDescription* aPattern,
                                                    TInt& aErrorTo)
{
//check if the policy is already in DB
//if yes then use that
//else
//create new one
    TInt retVal(KErrGeneral);
    TInt ret(KErrGeneral);
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyLoader::RegisterWsPolicyL");    

    TPtrC8 cont = aPattern->Contract();
    TPtrC8 endpo = aPattern->Endpoint();
    CSenElement* providerId = aPattern->AsElement().Element(KProviderIdLocalName);

    CSenServicePolicy* pol = (CSenServicePolicy*)aPattern->ServicePolicy();
    CSenElement& servicePolicy = pol->AsElement();
    CSenElement* clientPolicy = servicePolicy.Element( KClientPolicyLocalName );
    if(clientPolicy)
        {
        RPointerArray<CSenElement> metaDataPoliciesUris;
        CleanupClosePushL(metaDataPoliciesUris);
        clientPolicy->ElementsL(metaDataPoliciesUris, KMetadataEndpointLocalname);
        TInt count(metaDataPoliciesUris.Count());
        if(count> 0)
            {
            CSenElement* metaData = NULL;
            for(TInt i=0; i<count; i++)
                {
                metaData = metaDataPoliciesUris[i];
                if(metaData && metaData->HasContent())
                    {
                    TPtrC8 uriName = metaData->Content();
                    
                    CSenInternalWsPolicy* pInternalPolicy = CSenInternalWsPolicy::NewLC();
                    CSenWSPolicyIdentifier& identifier = pInternalPolicy->IdentifierL();	//codescannerwarnings
                    
                    identifier.AddMetadataEndpointL(metaData);

                    if(cont.Length() > 0)
                        identifier.SetPropertyL(KContractLocalname, cont);
                    if(endpo.Length() > 0)
                        identifier.SetPropertyL(KEndpointLocalname, endpo);
                    if(providerId && providerId->HasContent())     
                        {
                        TPtrC8 provider = providerId->Content();    
                        identifier.SetPropertyL(KProviderIdLocalName, provider);
                        }
                    TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,(uriName));
                    retVal = RegisterWsPolicyL(pInternalPolicy, aErrorTo);
                        
                    if(retVal != KErrNone )
                    {
                        CleanupStack::PopAndDestroy(pInternalPolicy);
                    }
                    else
                    {
                        CleanupStack::Pop(pInternalPolicy);
                        ret = KErrNone;
                    }
                    
                    }
                }
            }
        CleanupStack::PopAndDestroy(&metaDataPoliciesUris);
        }

    return ret;          
}

TInt CSenWSPolicyLoader::UnRegisterWsPolicyL(CSenWSDescription* aPattern,
                                                    TInt& aErrorTo)
{
	TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CSenWSPolicyLoader::UnRegisterWsPolicyL");
    TInt isPresent(KErrNotFound);
    TInt count = 0;
    for (TInt i=0; i<iPolicyArray.Count(); i++)
    {
        if(iPolicyArray[i].Policy()->IsApplicableL(*aPattern))	//codescannerwarnings
        {
            count++;
        }
    }

    for(TInt j=0; j<count; j++)
        {
        for (TInt i=0; i<iPolicyArray.Count(); i++)
            {
                if(iPolicyArray[i].Policy()->IsApplicableL(*aPattern))	//codescannerwarnings
                {
                    CSenElement& root = this->AsElement();
                    CSenElement& element = iPolicyArray[i].Policy()->AsElement();
                    CSenElement* parent = element.Parent();
                    iPolicyArray[i].Close();
                    iPolicyArray.Remove(i);

                    if(parent)
                    {
                        CSenElement* removed = root.RemoveElement(*parent);
                        delete removed;
                        removed = NULL;
                    }
                        
                    aErrorTo = KErrNone;
                    isPresent=  KErrNone; 
                }
            }
        }
    SaveDB();        
    return isPresent;
}

CSenElement*  CSenWSPolicyLoader::WsPolicyL( CSenWSDescription* /*aPattern*/, 
                                                const TDesC8& /*aUri*/, TInt& /*aErrorTo*/)
{
 return NULL;   
}

CSenElement* CSenWSPolicyLoader::GetExpiryTagL(CSenElement& aPolicy)
    {
//if there is no expiry tag in the policy the default expiry time is UTC +7 days    
    RPointerArray<CSenElement> timeStamps;
    CleanupClosePushL(timeStamps);       
    CSenElement* validity = NULL;
    const CSenNamespace* msNsUri = aPolicy.Namespace(MSWSPolicy::KMSNsPrefix);
    if(msNsUri)
        {
        if(aPolicy.ElementsL(timeStamps, msNsUri->URI(), MSWSPolicy::KMetaDataValidity) == KErrNone)    
            {
            TInt validityStampsCount = timeStamps.Count();
            
            if(validityStampsCount > 0)
                {
                validity = timeStamps[0];//mws:metadatavalidity 
                CleanupStack::PopAndDestroy();
                return validity->Element(msNsUri->URI(), MSWSPolicy::KExpiry);
                }
            }
        
        }
    else
        {
        if(aPolicy.ElementsL(timeStamps, MSWSPolicy::KMetaDataValidity) == KErrNone)
            {
                    
            TInt validityStampsCount = timeStamps.Count();
            
            if(validityStampsCount > 0)
                {
                validity = timeStamps[0];//mws:metadatavalidity 
                CleanupStack::PopAndDestroy();
                return validity->Element(MSWSPolicy::KExpiry);
                }
            }
        
        }
    CleanupStack::PopAndDestroy();        
    return NULL;
    }
CSenElement* CSenWSPolicyLoader::CreatePolicyElementL(const TDesC8& aUri, const TDesC8& aPolicy)
{
    TInt retVal(KErrGeneral);
    
    CMyXml* xml = CMyXml::NewLC();
    
    retVal = xml->ReadFromDescL(aPolicy);
  
    CSenElement* ele = NULL;

    if(retVal == KErrNone)
        {
         ele = xml->ExtractElement();
         TPtrC8 eleName = ele->LocalName();
         TPtrC8 eleNs = ele->NsPrefix();
        if(ele)
           if((eleName == KWsdlDocument && eleNs == KWsdlNsPrefix) || 
            (eleName == KPolicyDocument && eleNs == KPolicyNsPrefix))
            {//this is definitely a poliycy or wsdl file
            //Add Name, Id, Metaendpoint; validity
            ProcessNewPolicyL(aUri, *ele);
            }
        else            
            {
            delete ele; 
            ele = NULL;
            }
        }

        CleanupStack::PopAndDestroy();
        return ele;

}    
TInt CSenWSPolicyLoader::ProcessNewPolicyL( const TDesC8& aUri, CSenElement& aPolicy)
{
     TPtrC8 policyUri = CWSPolicyUtils::PolicyUriL(&aPolicy);
     if(policyUri.Length() == 0)
        SenXmlUtils::AddAttributeL(aPolicy, KMetadataEndpointLocalname, aUri);   
     
     TPtrC8 name = CWSPolicyUtils::PolicyNameL(&aPolicy);
     if(name.Length() == 0)
        SenXmlUtils::AddAttributeL(aPolicy, KName, aUri);   
     
     TPtrC8 id = CWSPolicyUtils::PolicyIdL(&aPolicy);
     if(id.Length() == 0)
        {

        CSenGuidGen* pGuidGenerator = CSenGuidGen::NewLC();
        HBufC8* iPolicyId = pGuidGenerator->GetRandomGuid8LC();
        TPtr8 policyIdDes = iPolicyId->Des();
        policyIdDes.LowerCase();

        aPolicy.AddAttrL(KWsuId, policyIdDes);
        aPolicy.AddNamespaceL(KWsuPrefix, KWsuNsUri);
                
        CleanupStack::PopAndDestroy(iPolicyId);
        CleanupStack::PopAndDestroy(pGuidGenerator);
        }

        
    CSenElement* expiry = GetExpiryTagL(aPolicy);
    TInt addtimeTag(KErrGeneral);
    TTime expiryTime = Time::NullTTime();
    if(expiry)
        {
        TInt updateTime = EvaluateExpiryL(expiry, expiryTime);
        if(updateTime == KErrNone)
            {
            addtimeTag = KErrNone;
            }
        }
    
    if(addtimeTag != KErrNone)
        {
        expiryTime.UniversalTime();
        TTimeIntervalDays interval(KValidityTime);
        expiryTime+=interval;
        }
    
    TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts;
    SenDateUtils::ToXmlDateTimeUtf82L(ts, expiryTime);
    SenXmlUtils::AddAttributeL(aPolicy, KSenWSPolicyValidUntil, ts); 
        
    return KErrNone;   
}    
TInt CSenWSPolicyLoader::EvaluateExpiryL(CSenElement* aExpiry, TTime& aExpiryTime)
{ 
    TInt updateTime(KErrNotFound);
    if(aExpiry && aExpiry->HasContent())
    {
        TPtrC8 timeTag = aExpiry->Content();
        _LIT8(KUtc,"UTC");
        _LIT8(KDays,"days");
        TInt utc = timeTag.Find(KUtc);  //CodeScannerWarnings      
        TInt days = timeTag.Find(KDays);
        if(utc != KErrNotFound && days != KErrNotFound)
        {   //We found UTC +x days timestamp
            //replace this time stamp with expiry time starting from now
        
            TInt plus = timeTag.Locate('+');
            if(plus != KErrNotFound)
            {
                TInt timeLength = days-plus-2;
                TPtrC8 timeValue =   timeTag.Mid(plus+2,timeLength-1);
                if(timeValue != KNullDesC8)
                {
                    TInt value = 0;        
                    TLex8 lex(timeValue);
                    TInt retVal = lex.Val(value);
                    
                    if(retVal != KErrNone)
                    {
                    value = KValidityTime;
                    }

                    if(value > 0)
                    {
                        TTime expiry;
                        expiry.UniversalTime();
                        TTimeIntervalDays interval(value);
                        expiry+=interval;
                        
                        TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts;
                        TRAP (retVal, SenDateUtils::ToXmlDateTimeUtf82L(ts, expiry));
                        if(retVal == KErrNone && ts.Length() > 0)
                        {
                            aExpiry->SetContentL(ts);
                            aExpiryTime = expiry;
                            updateTime = KErrNone;
                        }
                    }
                }
            }
        }
        
        if(updateTime != KErrNone)
        //the content is not in "UTC +7 days" format so try the XML time format
        {
            TTime now;
            now.UniversalTime();
     
            TTime expiry = Time::NullTTime();
            TRAPD(retval, expiry = SenDateUtils::FromXmlDateTimeL(timeTag);)
            if(retval == KErrNone)
                {   
                if(expiry > now)
                    {
                    aExpiryTime = expiry;
                    updateTime = KErrNone;
                    }
                    else //strange case. the policy provided to us is already expired
                    {
                    updateTime = KErrNone;                        
                    }
                }
            else
                {
             updateTime = KErrGeneral;
                }
            
        }
    }
    
    return updateTime;
}
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

EXPORT_C CExpiredPolicy* CExpiredPolicy::NewL(const TDesC8& aContract, const TDesC8& aEndpoint,
                                             const TDesC8& aUri, const TDesC8& aProviderId)
{
    CExpiredPolicy* pNew = NewLC(aContract, aEndpoint, aUri, aProviderId);
    CleanupStack::Pop(pNew);
    return(pNew);
}
EXPORT_C CExpiredPolicy* CExpiredPolicy::NewLC(const TDesC8& aContract, const TDesC8& aEndpoint,
                                             const TDesC8& aUri, const TDesC8& aProviderId)
{
    CExpiredPolicy* pNew = new (ELeave) CExpiredPolicy();
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aContract, aEndpoint, aUri, aProviderId);
    return pNew;
}
EXPORT_C CExpiredPolicy::~CExpiredPolicy()
{
    delete iContract; //service contract
    delete iEndpoint; //service contract
    delete iURI; //uri to download
    delete iProvideId;
    delete iContents; //contents

}

EXPORT_C TPtrC8  CExpiredPolicy::Contract()
{
    return  iContract->Des();  
}
EXPORT_C TPtrC8  CExpiredPolicy::Endpoint()
{
    return iEndpoint->Des();    
}
EXPORT_C TPtrC8  CExpiredPolicy::Uri()
{
    return iURI->Des();
}
TPtrC8  CExpiredPolicy::ProvideId()
{
    return iProvideId->Des();
}
EXPORT_C TPtrC8  CExpiredPolicy::Contents()
{
    if(iContents)
        return iContents->Des();
    else
        return KNullDesC8();
}
EXPORT_C TInt    CExpiredPolicy::SetContentsL(const TDesC8& aContent)
{
    if(aContent.Length() > 0)
        {
        if(iContents)
            {
                delete iContents;
                iContents = NULL;
            }
        iContents = aContent.AllocL();
        return KErrNone;
        }
    
    return KErrArgument;        
}
    

CExpiredPolicy::CExpiredPolicy() :iContents(NULL)
{
    
}
void CExpiredPolicy::ConstructL(const TDesC8& aContract, const TDesC8& aEndpoint, const TDesC8& aUri, const TDesC8& aProviderId)
{

    iContract = aContract.AllocL();
    iEndpoint = aEndpoint.AllocL();
    iProvideId = aProviderId.AllocL();
    iURI = aUri.AllocL();
 
}
    
CConsumerContainer* CConsumerContainer::NewL(const TDesC8& aUri, const TDesC8& aConsumerId)
{
    CConsumerContainer* pNew = NewLC(aUri, aConsumerId);
    CleanupStack::Pop(pNew);
    return(pNew);

}
CConsumerContainer* CConsumerContainer::NewLC(const TDesC8& aUri, const TDesC8& aConsumerId)
{
    CConsumerContainer* pNew = new (ELeave) CConsumerContainer();
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aUri, aConsumerId);
    return pNew;

}
CConsumerContainer::~CConsumerContainer()
{
    delete iConsumerId;
    delete iURI;

}
TPtrC8  CConsumerContainer::Uri()
{
    return iURI->Des();
}
TPtrC8  CConsumerContainer::ConsumerId()
{
    return iConsumerId->Des();
}
CConsumerContainer::CConsumerContainer()
{
    
}
void CConsumerContainer::ConstructL(const TDesC8& aUri, const TDesC8& aConsumerId)
{
    iConsumerId =  aConsumerId.AllocL();
    iURI = aUri.AllocL();
}



// End of file
