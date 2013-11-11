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




// INCLUDE FILES
#include "wsstarcobrandingclient.h"

#include <s32mem.h>
#include <f32file.h>
#include <s32file.h>
#include <SenXmlUtils.h>
#include "sencryptoutils.h"

#include "wsstarservicesession.h"
#include "SenXmlUtils.h"
#include "msencoreservicemanager.h"
#include "sendebug.h"
#include "senlogger.h"
#include "MSenServiceDescription.h"
#include "senlogger.h"
#include "SenServiceConnection.h"
#include "seninternalserviceconsumer.h"
#include "wsstarrstresponse.h"
#include <SenCredential.h>
#include "wsstarpolicy.h"
#include "wsstarplugin.h"
#include <SenTransportProperties.h>
#include "senwspattern.h"

using namespace WSTrust;
using namespace WSPolicy;

//namespace
//    {
//    _LIT8(KRemoteConsumer,       "RemoteConsumer");
//    }

//---------------------------------------------------------------------------
// Creates and returns a new instace of this class.
//---------------------------------------------------------------------------
//
CWSStarCoBrandingClient* CWSStarCoBrandingClient::NewL(
                                                        CSIF& aSIF,
                                                        RFileLogger& aLogger)
    {
    CWSStarCoBrandingClient* pNew = NewLC(aSIF, aLogger);
    CleanupStack::Pop(pNew);
    return pNew;
    }

//---------------------------------------------------------------------------
// Creates and returns a new instace of this class and leaves the object on the cleanup stack
//---------------------------------------------------------------------------
//
CWSStarCoBrandingClient* CWSStarCoBrandingClient::NewLC(
                                                        CSIF& aSIF,
                                                        RFileLogger& aLogger)
    {
    CWSStarCoBrandingClient* pNew =
        new (ELeave) CWSStarCoBrandingClient(
                    aSIF,
                    MSenServiceDescription::EWSStarSTSClient,
                    aLogger);

    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }

//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
//
CWSStarCoBrandingClient::CWSStarCoBrandingClient(
                                                CSIF& aSIF,
                                                TDescriptionClassType aType,
                                                RFileLogger& aLogger)
    : CSenInternalServiceConsumer(aType, aSIF),
    iLog(aLogger)
    {
    }

void CWSStarCoBrandingClient::BaseConstructL()
    {
    CSenInternalServiceConsumer::ConstructL();
    iTpCounter = 0;
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarCoBrandingClient::ConstructL()")));
    }

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
//
CWSStarCoBrandingClient::~CWSStarCoBrandingClient()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarCoBrandingClient::~CWSStarCoBrandingClient");
    delete ipTransport;
    delete iCoBrandingOrginalEndpoint;
    }


//---------------------------------------------------------------------------
// SetStatus
//---------------------------------------------------------------------------
//

void CWSStarCoBrandingClient::SetStatus(TInt /*aStatus */)
    {
    // No implementation in Java
     }

//---------------------------------------------------------------------------
// SetSTSSessionL
//---------------------------------------------------------------------------
//
void CWSStarCoBrandingClient::SetCoBrandingSessionL(
                                                CWSStarServiceSession* aSession)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarCoBrandingClient::SetCoBrandingSessionL");
    _LIT8(KTouch, "touch");
    const TDesC8* attrValue = aSession->AsElement().AttrValue(KTouch);
    if(attrValue != NULL)
    	{
    	AsElement().AddAttrL(KTouch, *attrValue);
    	}
    iCoBrandingSession = aSession;
    }
    
//---------------------------------------------------------------------------
// Fetch SCT request
//---------------------------------------------------------------------------
//Co-Branding CR
TInt CWSStarCoBrandingClient::FetchSCTL(
                                    CWSStarServiceSession& aSession,
                                    HBufC8*& aErrorMessage)
{
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarCoBrandingClient::FetchSCTL(..)");
    TInt retVal(KErrNone);            // for returning error codes
    iWSStarServiceSession = &aSession;            // save session
    iWSStarServiceSession->AddConsumerL(*this);
            
    CWSStarSessionContext* coBrandingSsnContext = iCoBrandingSession->SessionContext();
    CWSStarSessionContext* valSsnContext = iWSStarServiceSession->SessionContext();

    //PART 2 ::
    
    HBufC8* pBodyXML = NULL;
    HBufC8* coBrandResponseXML = NULL;

    const TBool* brandID = coBrandingSsnContext->GetIntL(WSStarContextKeys::KBrandIDEnabled);
    if(brandID && *brandID)
        {
            
        //Create a Fetch SCt body and update
       // CWSStarPlugin& frmWSStar = (CWSStarPlugin&)Framework();
        CSenElement* pBody = CreateFSTL();
        
        if(pBody)
	    {
	       
	        //Update application info attribute
	        if (CWSStarPolicy::CheckPolicyValueL(WSStarContextKeys::KMwsNamespace, iCoBrandingSession))
	            {
	                HBufC8* mwsNameSapce = CWSStarPolicy::GetPolicyValueL(WSStarContextKeys::KMwsNamespace,iCoBrandingSession);
	                CleanupStack::PushL(mwsNameSapce);
	                valSsnContext->Update(WSStarContextKeys::KMwsNamespace, mwsNameSapce->Des());    
	                CleanupStack::PopAndDestroy(mwsNameSapce);

	             }

	        AddAppInfoHeaderL();
	        
	        //Update action
	        valSsnContext->Update(WSStarContextKeys::KAction, KActionIssueFST200610);
	        
	        HBufC8* address = NULL;

	        address = CWSStarPolicy::GetPolicyValueL(WSStarContextKeys::KWAPGW,iCoBrandingSession);    

	        if (address)
	            {
	            valSsnContext->Update(WSStarContextKeys::KTo, *address);
	            
	            delete address;
	            address = NULL;        
	            }


	        valSsnContext->Update(WSStarContextKeys::KBodyElement, pBody);
	        pBodyXML = pBody->AsXmlL();
	        CleanupStack::PushL(pBodyXML);
	         

	        //Set transport properties    
	        CSenTransportProperties* transProp = CSenTransportProperties::NewLC();
	        TPtrC8 value;
	        value.Set(*valSsnContext->GetDesC8L(WSStarContextKeys::KAction));
	        transProp->SetSoapActionL(value);
	        
	        // Get the IAP setting and accordingly set the transport properties
	    	TUint32 id = 0;
	    	TInt getIapRetCode = IapId(id);
	    	
	    	    ;
	    	// Check whether IAP ID is pre-defined, and copy it only if such property value is found
	    	//if ( getIapRetCode == KErrNone && id < 4294967295) // unsigned 4294967295 == -1 signed
	    	if ( getIapRetCode == KErrNone && id < (TUint32)KErrNotFound) // unsigned 4294967295 == -1 signed
	    	    {
	    	    transProp->SetIapIdL(id); 
	    	    }
	    	// ETMI-6WAGDB end
	    	
	    	//limit http timeout, 
	    	//  if sts hangs and can not send any response, then we block every async client method.
	    	//  Possibel revalidation during sendL can take too long time therefore we create new limit
	    	transProp->SetMaxTimeToLiveL(WSStarSession::KMaxHttpTimeOut);
	    	 	    
	    	const TDesC8* pMwsNamespace = valSsnContext->GetDesC8L(WSStarContextKeys::KMwsNamespace);
	    	
	    	if(pMwsNamespace)
	    	    transProp->SetMwsNamespaceL(*pMwsNamespace);

	        HBufC8* transPropAsXml = transProp->AsUtf8LC();

	        //overwrite passport endpoint (provided by GUI)  by endpoint provided by policy
	        AdaptCoBrandEndpointL();

	        iTpCounter++;
	        retVal = iWSStarServiceSession->SubmitL(*pBodyXML, *transPropAsXml, *this, coBrandResponseXML);
	        iTpCounter--;
	        CleanupStack::PushL(coBrandResponseXML);
	        AdaptCoBrandEndpointL(ETrue);
	        valSsnContext->Remove(WSStarContextKeys::KTo);    
	        valSsnContext->Remove(WSStarContextKeys::KAppInfoPresent);    
	        
	        
	        if(coBrandResponseXML)
	            {
	            //Parse the response
	            CSenParser* pParser = CSenParser::NewLC();
	            CSenSoapEnvelope2* pResponseFragment = CSenSoapEnvelope2::NewL();
	            CleanupStack::PushL(pResponseFragment);
		        TRAPD(errorL,pParser->ParseL(*coBrandResponseXML, *pResponseFragment));

	            if ( retVal!= KErrNone || errorL!= KErrNone)
	                {
	                    aErrorMessage = coBrandResponseXML;
	                }
	            
	            CleanupStack::PopAndDestroy(2);  // pParser, pResponseFragment            
	                
	            }
	            
	        if (!aErrorMessage)
	            {
	            CleanupStack::PopAndDestroy(coBrandResponseXML);               
	            }
	        else
	            {
	            CleanupStack::Pop(coBrandResponseXML);
	            }
	        
	        CleanupStack::PopAndDestroy(transPropAsXml);
	        CleanupStack::PopAndDestroy(transProp);
	        CleanupStack::PopAndDestroy(pBodyXML); 
	        
	    }
    }

    if (! iTpCounter)
        {
        delete ipTransport;
        ipTransport = NULL;
        }
        
    
    iWSStarServiceSession->RemoveConsumerL(*this);

    return retVal;
    
}
//End Of Co-Branding
void CWSStarCoBrandingClient::AddAppInfoHeaderL()
{
    CWSStarSessionContext* valSsnContext = iWSStarServiceSession->SessionContext();

    //Update application info attribute
    if (CWSStarPolicy::CheckPolicyValueL(WSStarContextKeys::KAppInfo, iCoBrandingSession))
        {
            valSsnContext->Update(WSStarContextKeys::KAppInfoPresent, ETrue);

            RPointerArray<CSenElement> aAppElements;
            CWSStarPolicy::GetPolicyValueL(WSStarContextKeys::KAppInfo,WSStarContextKeys::KApplication,iCoBrandingSession,aAppElements);
            
            for(TInt i=0;i<aAppElements.Count();i++)
                {
                  CSenElement* element = aAppElements[i];
                  if(element->LocalName() == KName)
                  {
                    valSsnContext->Update(WSStarContextKeys::KApplicationName, element->Content());    
                  }
                  if(element->LocalName() == WSStarContextKeys::KVendor)
                  {
                    valSsnContext->Update(WSStarContextKeys::KApplicationVendor, element->Content());    
                  }
                  if(element->LocalName() == WSStarContextKeys::KVersion)
                  {
                    valSsnContext->Update(WSStarContextKeys::KApplicationVersion, element->Content());    
                  }
                }
           
            RPointerArray<CSenElement> aDeOpElements;
            CWSStarPolicy::GetPolicyValueL(WSStarContextKeys::KAppInfo,WSStarContextKeys::KDeviceOptions,iCoBrandingSession,aDeOpElements);
            for(TInt i=0;i<aDeOpElements.Count();i++)
                {
                  CSenElement* element = aDeOpElements[i];
                  if(element->LocalName() == WSStarContextKeys::KLocale)
                  {
                    valSsnContext->Update(WSStarContextKeys::KLocale, element->Content());    
                  }
                  if(element->LocalName() == WSStarContextKeys::KUserAgent)
                  {
                    RPointerArray<CSenElement> childElements1 = element->ElementsL();
                    for(TInt j=0;j<childElements1.Count();j++)
                    {
                      CSenElement* childElement1 = childElements1[j];
                      if(childElement1->LocalName() == WSStarContextKeys::KManufacturer)
                          {
                            valSsnContext->Update(WSStarContextKeys::KManufacturer, childElement1->Content());    
                          }
                      if(childElement1->LocalName() == WSStarContextKeys::KModel)
                          {
                            valSsnContext->Update(WSStarContextKeys::KModel, childElement1->Content());    
                          }
                      if(childElement1->LocalName() == WSStarContextKeys::KIMSI)
                          {
                            valSsnContext->Update(WSStarContextKeys::KIMSI, childElement1->Content());    
                          }
                      if(childElement1->LocalName() == WSStarContextKeys::KOS)
                          {
                          
                                RPointerArray<CSenElement> childElements2 = childElement1->ElementsL();
                                for(TInt k=0;k<childElements2.Count();k++)
                                {
                                  CSenElement* childElement2 = childElements2[k];
                                  if(childElement2->LocalName() == WSStarContextKeys::KName)
                                      {
                                        valSsnContext->Update(WSStarContextKeys::KDeviceName, childElement2->Content());    
                                      }
                                  if(childElement2->LocalName() == WSStarContextKeys::KVersion)
                                      {
                                        valSsnContext->Update(WSStarContextKeys::KDeviceVersion, childElement2->Content());    
                                      }
                                }
                          }
                      }
                  }
               }
        }
}
CSenServiceSession* CWSStarCoBrandingClient::ServiceSession()
    {
    return iWSStarServiceSession;
    }

//---------------------------------------------------------------------------
// Create body for Fetch SCT request
//---------------------------------------------------------------------------
//Co-Branding CR
CSenElement* CWSStarCoBrandingClient::CreateFSTL()
{

    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::CreateFSTL");

//-----------------
//1 Proxy
//-----------------

    if (CWSStarPolicy::CheckPolicyValueL(WSStarContextKeys::KWAPGW, iCoBrandingSession))
        {
        
        HBufC8* wapGW = CWSStarPolicy::GetPolicyValueL(WSStarContextKeys::KWAPGW, iCoBrandingSession);
        if (wapGW)
            {
             CleanupStack::PushL(wapGW);
            }  
        else
            {
                //Hard Coded Value    
            }
        
        CSenDomFragment* domFrg = CSenDomFragment::NewL(KFSTTag);
        CSenElement* elementRst = domFrg->ExtractElement();
        
        CleanupStack::PushL(elementRst);
        
        delete domFrg;
        domFrg = NULL;
                
        elementRst->AddNamespaceL(KNullDesC8,*wapGW);
        

        CleanupStack::Pop(elementRst);
        
        if (wapGW)
            {
             CleanupStack::PopAndDestroy(wapGW);
            }  
                    
        return elementRst;
        }
    
    return NULL;        

}

//End Of Co-Branding
MSenTransport& CWSStarCoBrandingClient::TransportL()
    {
    if(!ipTransport)
        {
        // Core is capable of working as transport factory, too.
        // The returned transport instancies may or may not be
        // ECOM plugins:
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::Instantiating new transport");
        ipTransport = Framework().Manager().CreateL(*iWSStarServiceSession, iWSStarServiceSession); 
        }
    return *ipTransport;
    }    
//---------------------------------------------------------------------------
// Getter for endpoint
//---------------------------------------------------------------------------
//
TPtrC8 CWSStarCoBrandingClient::Endpoint()
    {
    return iWSStarServiceSession->Endpoint();
    }
//---------------------------------------------------------------------------
// Getter for contract
//---------------------------------------------------------------------------
//
TPtrC8 CWSStarCoBrandingClient::Contract()
    {
    return iWSStarServiceSession->Contract();
    }
//---------------------------------------------------------------------------
// Getter for providerId
//---------------------------------------------------------------------------
//
TPtrC8 CWSStarCoBrandingClient::ProviderID()
    {
    return iWSStarServiceSession->ProviderID();
    }

//---------------------------------------------------------------------------
// Getter for framework
//---------------------------------------------------------------------------
//
TPtrC8 CWSStarCoBrandingClient::FrameworkId()
    {
    return iWSStarServiceSession->FrameworkId();
    }
//---------------------------------------------------------------------------
// serialie to xml
//---------------------------------------------------------------------------
//
void CWSStarCoBrandingClient::WriteAsXMLToL(RWriteStream& aWriteStream)
    {
    iWSStarServiceSession->WriteAsXMLToL(aWriteStream);
    }
MSenServicePolicy* CWSStarCoBrandingClient::ServicePolicy()
    {
    return iWSStarServiceSession->ServicePolicy();
    }
TInt CWSStarCoBrandingClient::IapId(TUint32& aCurrentIapId)
    {
    return iWSStarServiceSession->IapId(aCurrentIapId);
    }

//---------------------------------------------------------------------------
// setter for ws-trust version
//---------------------------------------------------------------------------
//
void CWSStarCoBrandingClient::SetHandlerCtx(MSenHandlerContext* aHndCtx)
    {
    iHndCtx = aHndCtx;
    }

//Co-branding CR    
TInt CWSStarCoBrandingClient::AdaptCoBrandEndpointL(TBool aToOrginal)	//codescannerwarnings
    {

    TInt returnValue( KErrNone );
    if (aToOrginal)
        {
        iWSStarServiceSession->SetEndPointL(*iCoBrandingOrginalEndpoint);
        return returnValue;
        }
    delete iCoBrandingOrginalEndpoint;
    iCoBrandingOrginalEndpoint = NULL;
    iCoBrandingOrginalEndpoint = iWSStarServiceSession->Endpoint().AllocL();        
    HBufC8* address = NULL;
    
    address = CWSStarPolicy::GetPolicyValueL(WSStarContextKeys::KWAPGW,iCoBrandingSession);    
    
    if (address)
        {
        iWSStarServiceSession->SetEndPointL(*address);
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::AdaptCoBrandEndpointL() - endpoint replaced by value from policy");
        
        delete address;
        address = NULL;        
        }
    else
        {
        returnValue = KErrNotFound;
        }

    return returnValue;
    }    
    
TBool CWSStarCoBrandingClient::Matches(MSenServiceDescription& aOtherServiceDescription)
    {
    return iWSStarServiceSession->Matches(aOtherServiceDescription);

    }
    
// End of file


