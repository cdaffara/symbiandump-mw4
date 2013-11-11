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
#include "wsstartrustclient.h"

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

_LIT8(KRemoteConsumer,       "RemoteConsumer");

//---------------------------------------------------------------------------
// Creates and returns a new instace of this class.
//---------------------------------------------------------------------------
//
CWSStarTrustClient* CWSStarTrustClient::NewL(
                                                        CSIF& aSIF,
                                                        RFileLogger& aLogger)
    {
    CWSStarTrustClient* pNew = NewLC(aSIF, aLogger);
    CleanupStack::Pop(pNew);
    return pNew;
    }

//---------------------------------------------------------------------------
// Creates and returns a new instace of this class and leaves the object on the cleanup stack
//---------------------------------------------------------------------------
//
CWSStarTrustClient* CWSStarTrustClient::NewLC(
                                                        CSIF& aSIF,
                                                        RFileLogger& aLogger)
    {
    CWSStarTrustClient* pNew =
        new (ELeave) CWSStarTrustClient(
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
CWSStarTrustClient::CWSStarTrustClient(
                                                CSIF& aSIF,
                                                TDescriptionClassType aType,
                                                RFileLogger& aLogger)
    : CSenInternalServiceConsumer(aType, aSIF),
    iLog(aLogger)
    {
    }

void CWSStarTrustClient::BaseConstructL()
    {
    CSenInternalServiceConsumer::ConstructL();
    iTpCounter = 0;
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarTrustClient::ConstructL()")));
    }

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
//
CWSStarTrustClient::~CWSStarTrustClient()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::~CWSStarTrustClient");
    delete iSTSSession;
    delete iEntropyClient;
    delete iAppliesTo;
    delete ipTransport;
    delete iTokenType;
    delete iSTSOrginalEndpoint;
    }


//---------------------------------------------------------------------------
// The most siginificant method of this class. Make validation of session, 
//    credentails obtained if needed.
//    Sequential logic, synchronized, step bye SOAP exchange scenrio.
//---------------------------------------------------------------------------
//
TInt CWSStarTrustClient::ValidateL( CWSStarServiceSession& aSession,
                                    TBool aRenewing,
                                    HBufC8*& aErrorMessage,
                                    TBool aOnlySharing )
    {
    iSTSSession->CompleteServerMessagesOnOffL(ETrue);
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::ValidateL(..)");
    TInt retVal(KErrNone);            // for returning error codes
    iWSStarServiceSession = &aSession;            // save session
    
//----------------------
//0  token sharing
//----------------------
    TBool sharingSucceed = EFalse;
    HBufC8* clusterUsed = CWSStarPolicy::GetPolicyValueL(KIssuedToken, KRPSCluster, *iHndCtx, iSTSSession);
    CleanupStack::PushL(clusterUsed);
    
    //its mean - if our provider uses cluster template, 
    //so we can try copy MT and SCT form other msn services which use same provider ID
    if (clusterUsed)
        {
        RPointerArray<CSenWSDescription> msnSDs;
        CleanupClosePushL(msnSDs);
        CSenWSPattern* pattern = CSenWSPattern::NewLC();
     
        //select * from session_of_SD where session is (ws* framework, and has same provideId)
        //here we assume that provider use cluster template
        
        pattern->SetFrameworkIdL(KDefaultWSStarFrameworkID);
        pattern->AsElement().AddElementL(KSenIdpProviderIdLocalname).SetContentL(iIdentityProvider->ProviderID());
        //pattern->AsElement().AddElementL(WSStarSession::KProviderIdElementLocalName).SetContentL(iIdentityProvider->ProviderID());
        //seeking
        CWSStarServiceSession* msnSession = NULL;
        TRAPD(leaveError, retVal = Framework().Manager().ServiceDescriptionsL(msnSDs, *pattern));
        if(!retVal && !leaveError)
            {
            TInt count = msnSDs.Count();
            TBool pMTwasReplaceBySCT = EFalse;
            TBool seekSCT = EFalse;
            for(TInt i = 0; i < msnSDs.Count(); i++)
                {
                //all sessions (so in practice we have also access to SCT, not only MT),
                // to eliminate client, just for case we check type
                TDescriptionClassType dt = msnSDs[i]->DescriptionClassType();
                if (dt == MSenServiceDescription::EWSStarServiceSession)
                    {
                    msnSession = (CWSStarServiceSession*)msnSDs[i];
                    //dont share to itself
                    if (msnSession != iWSStarServiceSession)
                        {
                        TRAPD(err, retVal = msnSession->ShareTokenWithL(
                                iWSStarServiceSession, pMTwasReplaceBySCT,
                                seekSCT));
                        if ((retVal == KErrNone) && (err == KErrNone))
                            {
                            sharingSucceed = ETrue;
                            if (pMTwasReplaceBySCT)
                                {
                                break;    
                                }
                            else
                                {
                                seekSCT = ETrue;//in order to find other session which can have additioanly SCT
                                }
                            }    
                        }
                    }
                }
            if ( !pMTwasReplaceBySCT && !sharingSucceed )
                {
                // Did not find valid MT      (<=> sharingSucceed     == EFalse)
                // AND did not find valid SCT (<=> pMTwasReplaceBySCT == EFalse) 
                // 
                // Try to search valid MT directly from CredentialManager
                // (This is needed especially in case when MT is imported
                //  through Public API
                //  => There is not yet Session which holds imported MT)
                if ( iWSStarServiceSession->TryToSearchValidCredentialL() == KErrNone )	//codescannerwarnings
                    {
                    iWSStarServiceSession->SetStatusL();
                    iWSStarServiceSession->AddSecurityTokenToContextL();
                    sharingSucceed = ETrue;
                    }
                }
            }
        CleanupStack::PopAndDestroy(2, &msnSDs);
        }
    CleanupStack::PopAndDestroy(clusterUsed);
    
    if (sharingSucceed)
        {
        iWSStarServiceSession->SetTrustAnchorL(iIdentityProvider->ProviderID());
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::ValidateL(..) - sharing succed, we don't need more validation");
        return KErrNone;
        }
    if (aOnlySharing)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::ValidateL(..) - only sharing was allowed so NO continuation");
        return KErrNone;
        }
    
    
//-----------------
//1 Init session
//-----------------

    if (!iSTSSession)// if trustClient has NOT been registered earlier
        {
        iSTSSession = CWSStarServiceSession::NewL(Framework());
        iSTSSession->InitializeFromL(*this);
        iSTSSession->SetEndPointL(iIdentityProvider->Endpoint());
        iSTSSession->SetContractL(KWSStarSTSContract);
        iSTSSession->AddConsumerL(*this);
        }
    //CSenXmlReader* reader = Framework().Manager().XMLReader();
    CWSStarSessionContext* stsSsnContext = iSTSSession->SessionContext();
    iPassportMode = CWSStarPolicy::CheckPolicyValueL(KPassportExtensions, iSTSSession);
    if (iPassportMode)
        {
        stsSsnContext->Update(WSStarContextKeys::KPassportEnabled, ETrue);
        aRenewing = FALSE; //MS passport doesn't support renewing binding. So we force here issue binding
        }
    iRenewingMode = aRenewing;

//-----------------
//2.preparing context    
//-----------------------WS-TRUST 2005-02 chapter 6 -  RST issue-------------------------    
    stsSsnContext->Update(WSStarContextKeys::KAction, GetAction());
    
    TInt retryCounter(0);
    TBool retry(ETrue);
    TBool authenticationInfoPromptedFromUser(EFalse);
    TPckgBuf<TSenAuthentication> authInfo;
    HBufC8* responseXML = NULL;
    HBufC8* pBodyXML = NULL;
    MSenRemoteServiceConsumer* pConsumer = NULL;
            
    while ( retry ) // Authentication loop starts
        {
        retry = EFalse;
        
        //setting secure profile of authentication with sts, can X509, so in future some factory calling
    if (CWSStarPolicy::CheckPolicyValueL(KUsernameTokenOverTLS, iSTSSession))
            {
            HBufC8* token = NULL;
            if ( authenticationInfoPromptedFromUser )
                {
                CSenWsSecurityHeader::UsernameTokenL(authInfo().iUsername,
                                                     authInfo().iPassword,
                                                     token);
                }
            else
                {
                CSenWsSecurityHeader::UsernameTokenL(iIdentityProvider->UserName(),
                                                     iIdentityProvider->Password(),
                                                     token);
                }
            stsSsnContext->Update(WSStarContextKeys::KSecurityToken, *token);    
        delete token;    
        token = NULL;
        }
    
    //building body
    //CWSStarPlugin& frmWSStar = (CWSStarPlugin&)Framework();
    CSenElement* pBody = CreateBodyL();
    stsSsnContext->Update(WSStarContextKeys::KBodyElement, pBody);
        pBodyXML = pBody->AsXmlL();
        CleanupStack::PushL(pBodyXML);
    //-----------------    
    //_____________________________________SYNC Sending____________________________________________

        CSenTransportProperties* transProp = CSenTransportProperties::NewLC();
		const TDesC8* value =stsSsnContext->GetDesC8L(WSStarContextKeys::KAction);
		if(value)
        transProp->SetSoapActionL(*value);
        
        // IAP dialog used to appear though IAP id is provided as a part of Service Description
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
    	 	    
        HBufC8* transPropAsXml = transProp->AsUtf8LC();
    /*
        if( iWSStarServiceSession )
            {
            iWSStarServiceSession->SendStatusToConsumerL( KSenStatusAuthentication );
            }
    */        
        //overwrite passport endpoint (provided by GUI)  by endpoint provided by policy
        AdaptStsEndpointL();	//codescannerwarnings
        iTpCounter++;
        retVal = iSTSSession->SubmitL(*pBodyXML, *transPropAsXml, *this, responseXML);
        iTpCounter--;
        CWSStarSessionContext* pSessionCtxt = iWSStarServiceSession->SessionContext();// redundant code 
        TAny* ptr = pSessionCtxt->GetAnyL(KRemoteConsumer());	//codescannerwarnings
        if ( ptr )
            {
            pConsumer = REINTERPRET_CAST(MSenRemoteServiceConsumer*, ptr);
            }
        else
            {
            pConsumer = iWSStarServiceSession->Consumer();
            }

        pConsumer->SetDataTrafficDetails(iTrustDetails);
        AdaptStsEndpointL(ETrue);
        CleanupStack::PopAndDestroy(transPropAsXml);
        CleanupStack::PopAndDestroy(transProp);
        CleanupStack::PopAndDestroy(pBodyXML);
        
        CleanupStack::PushL(responseXML);

        if ( retVal == KErrSenSoapFault )
            {
            //CREDLOG_L(KSenCredsLogLevelNormal,".......failed (SOAP Fault)");
            CWSStarSessionContext* pSessionContext = iWSStarServiceSession->SessionContext();
            TAny* ptr = pSessionContext->GetAnyL(KRemoteConsumer());	//codescannerwarnings
            if ( ptr )
                {
                pConsumer = REINTERPRET_CAST(MSenRemoteServiceConsumer*, ptr);
                }
            else
                {
                pConsumer = iWSStarServiceSession->Consumer();
                }
            
            if ( pConsumer && !pConsumer->HasAuthenticationCallback() &&
                 retryCounter < 3 )
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::ValidateL - Consumer does not have AuthenticationCallback");
                
                const TDesC8* pValue = iIdentityProvider->AsElement().AttrValue( KSenAttrPromptUserInfo );
                
#ifdef _RD_SEN_WS_STAR_DO_NOT_PROMPT_AUTHINFO_BY_DEFAULT
                if ( pValue && *pValue != _L8("false") )
                // attribute has to exist, otherwise logic defaults to: DO NOT PROMPT
#else
                if ( !pValue || *pValue != _L8("false") )
                // even when attribute does NOT exist, or when it does and has any other value but "false", default to: PROMPT
#endif                
                    {
                    const TBool* renewingPtr = stsSsnContext->GetIntL(WSStarContextKeys::KReAuthNeeded);
					if ( renewingPtr )
                     	{
                    if ( *renewingPtr )
                        {
                            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"- Prompt is used to get AuthenticationInfo");
                        TInt ret = (Framework().Manager()).AuthenticationForL(*iIdentityProvider,
                                                                              authInfo);
                        if ( ret == KErrNone )
                            {
                            retryCounter++;
                            retry = ETrue;
                            authenticationInfoPromptedFromUser = ETrue;
                            }
                        }
                     	}
                    }
                else
                    {
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"CWSStarTrustClient::ValidateL - promptUserInfo = false");
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel," => prompt is not used.");
                    }
                }
            if ( pConsumer && pConsumer->HasAuthenticationCallback() &&
                             retryCounter < 3 )
            	{
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"CWSStarTrustClient::ValidateL - Consumer has AuthenticationCallback");
                
                const TDesC8* pValue = iIdentityProvider->AsElement().AttrValue( KSenAttrPromptUserInfo );
                
                if ( !pValue || *pValue != _L8("false") )
                    {
                    const TBool* renewingPtr = stsSsnContext->GetIntL(WSStarContextKeys::KReAuthNeeded);
                    if ( renewingPtr && *renewingPtr )
                        {
                        const TBool* renewingByCredObserv = iWSStarServiceSession->SessionContext()->GetIntL(WSStarContextKeys::KReAuthNeededFromCredObserv);
                        if( renewingByCredObserv != NULL && *renewingByCredObserv )
                        	{
                        	// Credential Observer -> call reauthenticatino through connection agent
                        	CSenIdentityProvider* newIdP = iIdentityProvider;
                        	TRAPD(error, retVal = pConsumer->ReauthenticationNeededL(newIdP));
    	                    if(error == KErrNone && retVal == KErrNone)
    	                    	{
    	                    	// Update identity provider in case old one got corrupted
    	                    	iIdentityProvider = newIdP;
    	                    	retryCounter++;
    	                    	retry = ETrue;
    	                    	}
    	                    else
    	                    	{
    	                    	retVal = KErrSenFailedAuthentication;
    	                    	}
                        	}
                        else
                        	{
                        	retVal = KErrSenFailedAuthentication;
                        	}
                        }
                    }
                else
                    {
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"CWSStarTrustClient::ValidateL - UserInfoPrompt = false");
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel," => AuthenticationCallback is not used.");
                    }
                
            	}
            }
            
        if ( retry )
            {
            CleanupStack::PopAndDestroy(responseXML);
            }
        }  // Authentication loop ends
   
    if (!retVal)
        {
        const TDesC8* responseAction = stsSsnContext->GetDesC8L(WSStarContextKeys::KResponseAction);
        if  (responseAction &&
                responseAction->Length())
            {
            if (*responseAction != GetActionResponse())
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"CWSStarTrustClient::ValidateL - wrong action in response");

                retVal = KErrNotFound;
                //no ws-trust specifiv fault defined for invalid RESPONSE. I can only answer iwth addressign fault
                //but no access to addressing handler directly
                }
            }
        }
    if (retVal != KErrNone)
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CWSStarTrustClient::ValidateL - error when submit to STS %d"), retVal));

        if (responseXML)
            {
            aErrorMessage = responseXML;
            }
        
        CleanupStack::Pop(responseXML);
        if (! iTpCounter)
            {
            //we don't have to keep transport. Important expecially during night test, 
            //  when transport and its RConnection exceed idle timeout
            delete ipTransport;
            ipTransport = NULL;
            }
        return retVal;
        }
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::ValidateL - response from STS is a valid SOAP");
	#ifdef RD_SEN_VTCP_SUPPORT
	//#ifdef RD_SEN_CO_BRAND_SUPPORT
	//stsSsnContext->Update(WSStarContextKeys::KBrandIDEnabled, EFalse);
    const TDesC8* mainBrandID = stsSsnContext->GetDesC8L(WSStarContextKeys::KMainBrandID);
    if  (mainBrandID && mainBrandID->Length())
        {
        if(iPassportMode)
            {
            //Call Back Consumer
            CWSStarSessionContext* pSessionContext = iWSStarServiceSession->SessionContext();
            TAny* ptr = pSessionContext->GetAnyL(KRemoteConsumer());	//codescannerwarnings
            if ( ptr )
                {
                pConsumer = REINTERPRET_CAST(MSenRemoteServiceConsumer*, ptr);
                }
            else
                {
                pConsumer = iWSStarServiceSession->Consumer();
                }
          
            if ( pConsumer && pConsumer->HasCoBrandingCallback() )
                {
                	TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"CWSStarTrustClient::HasCoBrandingCallback");
                    const TDesC8* brandIDList = stsSsnContext->GetDesC8L(WSStarContextKeys::KBrandIDList);

                    RBuf8 selectedBrandId;
                    //User will reply with go/no go optoin
                    if(pConsumer->OnGetBrandIdL(*brandIDList,selectedBrandId))
                    {
                		TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"CWSStarTrustClient::Brand ID Is Enabled");
                        // co-branding - Add co branding details in context
                        stsSsnContext->Update(WSStarContextKeys::KBrandIDEnabled, ETrue);
                    }
                    else
                    {
                		TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"CWSStarTrustClient::Brand ID Is Disabled");
                        // co-branding - Add co branding details in context
                        stsSsnContext->Update(WSStarContextKeys::KBrandIDEnabled, EFalse);
                    }

                }

            }
        }
    
    TBool cacheToken = ETrue;
    const TBool* brandID = stsSsnContext->GetIntL(WSStarContextKeys::KBrandIDEnabled);
    if(brandID && !(*brandID))
    {
    	cacheToken = EFalse;
    	retVal = KErrSenBrandNotSupported;
    }
    
    if(cacheToken)
	{
	//#endif//RD_SEN_CO_BRAND_SUPPORT	
   #endif//RD_SEN_VTCP_SUPPORT
//-----------------
//4 building response    
//-----------------
    CWSStarRSTResponse* rstResponse = CWSStarRSTResponse::NewLC(FALSE, *iAppliesTo, iVersion);
    rstResponse->SetPolicyVersion(&GetPolicyNs());
    rstResponse->UpdateAddressingHoldersL(*stsSsnContext);
    rstResponse->SetReader(*Framework().Manager().XMLReader());
    //shouldnt be error, parsing is done earlier by core so we can be sure, that it is valid SOAP XML
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::ValidateL - building RSTResponse");
    rstResponse->BuildFrom(*responseXML);
    
    if (! iWSStarServiceSession->ExistConsumerL())
        {
        retVal = KErrNotFound;
        }
   else
        {
    iWSStarServiceSession->SetTrustAnchorL(iIdentityProvider->ProviderID());
    if (rstResponse->IsResponse())
        {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::ValidateL - set ctx basing on RSTR");
	            //const TBool* brandID = stsSsnContext->GetIntL(WSStarContextKeys::KBrandIDEnabled);
	            TBool isWAPExists = CWSStarPolicy::CheckPolicyValueL(WSStarContextKeys::KWAPGW, iSTSSession);
			#ifdef RD_SEN_VTCP_SUPPORT
			//#ifdef RD_SEN_CO_BRAND_SUPPORT
				if(isWAPExists && (brandID && *brandID))
			#else
        if(isWAPExists)
      //#endif//RD_SEN_CO_BRAND_SUPPORT   
			#endif//RD_SEN_VTCP_SUPPORT				
	            {
	                retVal = iWSStarServiceSession->SessionContext()->SetTokenKeysL(
	                        *rstResponse, iEntropyClient,EFalse);                        
	            }
	            else
	            {
	                retVal = iWSStarServiceSession->SessionContext()->SetTokenKeysL(
	                        *rstResponse, iEntropyClient,ETrue);
	            }
            iWSStarServiceSession->ActiveTicketObserverL();
        //SetTokenKeys call indirectly SetStatus in order to signal session's consumers that connection is ready for sending                
        if (retVal)
            {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::ValidateL - response from STS is wrong RSTR, maybe some nested Fault");
                //CREDLOG_L(KSenCredsLogLevelNormal,".......failed, problem with RSTR analizing");
            aErrorMessage = responseXML;
					if (retVal == KErrNotFound)
						{
						retVal = KErrSenSoapFault;
		                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel," ------- returning SOAP Fault for MS and clearing the credential if any");
						iWSStarServiceSession->DetachCredentialsL();
						}
	                }
            /*else
                {
                #ifdef _SENDEBUG
                    _LIT8(KCredsLogLine, "...Token has been issued (period = %S - %S)");
                    CREDLOG_FORMAT((KSenCredsLogChannel, KSenCredsLogLevelNormal, KCredsLogLine,  &(rstResponse->CredentialCreated()),  &(rstResponse->CredentialExpires())));
                 #endif           
                }*/
        }
    else
        {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::ValidateL - response from STS is not a proper RSTR");
        aErrorMessage = responseXML;
        retVal = KErrNotFound;
        }
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CWSStarTrustClient::ValidateL - building session Ctx: %d"), retVal));
       }
  
    CleanupStack::PopAndDestroy(rstResponse);
#ifdef RD_SEN_VTCP_SUPPORT
//#ifdef RD_SEN_CO_BRAND_SUPPORT
	}
//#endif//RD_SEN_CO_BRAND_SUPPORT	
#endif//RD_SEN_VTCP_SUPPORT	
    if (!aErrorMessage)
        {
        CleanupStack::PopAndDestroy(responseXML);
        }
    else
        {
        CleanupStack::Pop(responseXML);
        }
    if (! iTpCounter)
        {
        delete ipTransport;
        ipTransport = NULL;
        }
    return retVal;
    }



//---------------------------------------------------------------------------
// SetStatus
//---------------------------------------------------------------------------
//

void CWSStarTrustClient::SetStatus(TInt /*aStatus */)
    {
    // No implementation in Java
     }

//---------------------------------------------------------------------------
// SetAccount
//---------------------------------------------------------------------------
//
void CWSStarTrustClient::SetAccount(
                                    CSenIdentityProvider* aIdentityProvicer)
    {
    iIdentityProvider = aIdentityProvicer;
    }
/*CSenIdentityProvider* CWSStarTrustClient::Account()
    {
    return iIdentityProvider;
    }*/
/*TInt CWSStarTrustClient::SetProviderIdL(const TDesC8& aProviderID)
    {
    HBufC8* pNew = NULL;
    if(aProviderID.Length()>0)
        {
        pNew = aProviderID.AllocL();
        }
    delete ipProviderId;
    ipProviderId = pNew;
    return KErrNone;
    }
TPtrC8 CWSStarTrustClient::ProviderId()
    {
    if(ipProviderId)
        return *ipProviderId;
    else
        return KNullDesC8();
    }*/

//---------------------------------------------------------------------------
// ServiceSession
//---------------------------------------------------------------------------
//
CSenServiceSession* CWSStarTrustClient::ServiceSession()
    {
    return iWSStarServiceSession;
    }

//---------------------------------------------------------------------------
// Obtain Transport using a factory from Core Manager
//---------------------------------------------------------------------------
//
MSenTransport& CWSStarTrustClient::TransportL()
    {
    if(!ipTransport)
        {
        // Core is capable of working as transport factory, too.
        // The returned transport instancies may or may not be
        // ECOM plugins:
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::Instantiating new transport");
        ipTransport = Framework().Manager().CreateL(*iSTSSession, iSTSSession); 
        }
    return *ipTransport;
    }
//---------------------------------------------------------------------------
// SetSTSSessionL
//---------------------------------------------------------------------------
//
void CWSStarTrustClient::SetSTSSessionL(
                                                CWSStarServiceSession* aSession)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::SetServiceSessionL");
    _LIT8(KTouch, "touch");
    // free memory
    //delete iSTSSession;
    const TDesC8* attrValue = aSession->AsElement().AttrValue(KTouch);
    if(attrValue != NULL)
    	{
    	AsElement().AddAttrL(KTouch, *attrValue);
    	}
    iSTSSession = aSession;
    if(iSTSSession)
        {
        iSTSSession->AddConsumerL(*this);
        }
    }

CWSStarServiceSession* CWSStarTrustClient::GetSTSSessionL()
{
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::SetServiceSessionL");
    if(iSTSSession)
        {
        return iSTSSession;
        }
    else
        {
        return NULL;            
        }
}
//---------------------------------------------------------------------------
// Create body for STS request
//---------------------------------------------------------------------------
//
CSenElement* CWSStarTrustClient::CreateBodyL()
    {
    HBufC8* value = const_cast<HBufC8*>(CWSStarPolicy::GetPolicyValueL(KIssuedToken, KRPSCluster, *iHndCtx, iSTSSession));
    CleanupStack::PushL(value);    

//-----------------
//1 RST part (1st otken, proper one)
//-----------------
    CSenElement* specificRST = NULL;
    if (value && iPassportMode)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::CreateBodyL - found cluster");
        delete iAppliesTo;
        iAppliesTo = NULL;
        iAppliesTo = value;
        specificRST = CreateRSTL(iRenewingMode, value->Des());
        CleanupStack::Pop(value);        
        value = NULL;
        }
    else
        {
        delete iAppliesTo;
        iAppliesTo = NULL;
        iAppliesTo = iWSStarServiceSession->Endpoint().AllocL();
        specificRST = CreateRSTL(iRenewingMode, *iAppliesTo);
        CleanupStack::PopAndDestroy(value);        
        //Renewing - check from policy
        //@Allow not specified, the default value is true.
        //@OK not specified, the default value is false.
        specificRST->AddElementL(*iVersion, KRenewingTag);    
        }
    CleanupStack::PushL(specificRST);

//-----------------
//2 entropy
//-----------------
    
    if (CWSStarPolicy::CheckPolicyValueL(KClientEntropy, iSTSSession))
        {
        TRAPD(error, RandomEntropyClientL());
        if (!error)
            {
            CSenElement& entrElement = specificRST->AddElementL(*iVersion, KEntropyTag);    
            entrElement.AddElementL(*iVersion, KBinarySecretTag).SetContentL(*iEntropyClient);    
            }
        }

//-----------------
//3 excluding dynamic RST by RST template 
//-----------------
    delete iTokenType;
    iTokenType = NULL;
    iTokenType = CWSStarPolicy::GetPolicyValueL(WSPolicy::KTokenType, iSTSSession);
    if (!iTokenType && iPassportMode)
        {
        iTokenType = WSPassport32::KDefaultTokenType().AllocL();
        }
        
        
    RPointerArray<CSenElement> elements;
    if (iTokenType)
        {
/*#ifdef _RD_SEN_ENABLE_CREDLOG
        _LIT8(KCredsLogLine, "Sending RST to STS to get new token (type %S)....");
        CREDLOG_FORMAT((KSenCredsLogChannel, KSenCredsLogLevelNormal, KCredsLogLine,  tokenType));
#endif        */
        value  = CWSStarPolicy::GetPolicyValueL(*iTokenType, KIssuedToken, KRequestSecurityTokenTemplate, *iHndCtx, iSTSSession, elements);
        }
    else
        {
        value  = CWSStarPolicy::GetPolicyValueL(KIssuedToken, KRequestSecurityTokenTemplate, *iHndCtx, iSTSSession, elements);
        }
    if (!value)
        {
       	if (iTokenType)
       	    {
            specificRST->AddElementL(*iVersion, 
                                KTokenTypeTag).SetContentL(*iTokenType);
      	    }
        }
    else
        {
            TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(*value));//in debug we dont have to push to cleanup stack
            delete value;
            value = NULL;
            CleanupStack::PopAndDestroy(specificRST);
            CSenDomFragment* domFrg = CSenDomFragment::NewL(*iVersion, 
                                                    KRSTTag, 
                                                    KRSTQTag);
            CleanupStack::PushL(domFrg);
            specificRST = domFrg->ExtractElement();
            CleanupStack::PopAndDestroy(domFrg);
            CleanupStack::PushL(specificRST);
            
            for (TInt i = 0; i<elements.Count(); i++ )
                {
                CSenDomFragment* rstTemplate = CSenDomFragment::NewL(*elements[i]);
                CSenElement* rstTemplateEl = rstTemplate->ExtractElement();
                delete rstTemplate;
                rstTemplate = NULL;
                CleanupStack::PushL(rstTemplateEl);
                specificRST->AddElementL(*rstTemplateEl);
                CleanupStack::Pop(rstTemplateEl);
                }
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::CreateBodyL - added template");
            }
       
     
//-----------------
//4 add passport RST (redundant info, but passport requires that)
//      and enable multirequest
//-----------------
    if (!iPassportMode)
        {
        CleanupStack::Pop(specificRST);
        return specificRST;
        }
    else
        {
        CSenElement* passportRST = CreateRSTL(EFalse, WSPassport32::KPassportAppliesTo());
        CleanupStack::PushL(passportRST);
        CSenDomFragment* domFrg = CSenDomFragment::NewL(WSStarContextKeys::KMultiReqTagMark);
        CSenElement* reqMultiple = domFrg->ExtractElement();
        delete domFrg;
        CleanupStack::PushL(reqMultiple);
        reqMultiple->AddElementL(*specificRST);
        reqMultiple->AddElementL(*passportRST);
        CleanupStack::Pop(3, specificRST);
        return reqMultiple;
        }
    }
//---------------------------------------------------------------------------
// Create RST skeleton.
//---------------------------------------------------------------------------
//
CSenElement* CWSStarTrustClient::CreateRSTL(TBool aRenewing, TPtrC8 aAppliesTo)
    {
    //whole logic for RST factory
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CWSStarTrustClient::CreateRST - appliesTo: %S"), &aAppliesTo));

    CSenDomFragment* domFrg = CSenDomFragment::NewL(*iVersion, 
                                                    KRSTTag, 
                                                    KRSTQTag);
    CSenElement* elementRst = domFrg->ExtractElement();
    CleanupStack::PushL(elementRst);
    delete domFrg;
    domFrg = NULL;
    elementRst->AddElementL(*iVersion, KRequestTypeTag).SetContentL(GetRequestType());
    
//__________ISSUE
    if (!aRenewing)
        {
            CSenElement& applies = elementRst->AddElementL(GetPolicyNs(),
                                                            KAppliesToTag,
                                                            KAppliesToQTag);
            SenXmlUtils::AddAttributeL(applies, WSStarContextKeys::KAddressAttrMark, aAppliesTo);
        
        }
//__________RENEW
    //in 7. Renewal Binding
    //Other extensions MAY be specified in the request (and the response), but the key
    //semantics (size, type, algorithms, scope, etc.) MUST NOT be altered during renewal.
    else
        {
        CSenElement& el = elementRst->AddElementL(*iVersion, KRenewTargetTag);
        const TDesC8* oldToken = iWSStarServiceSession->SessionContext()->GetDesC8L(WSStarContextKeys::KSecurityTokenBackup);
        if (oldToken)
            {
            el.SetContentL(*oldToken);
            }
        }
    CleanupStack::Pop(elementRst);
    return elementRst;
    }
// End of file

//---------------------------------------------------------------------------
// Getter for endpoint
//---------------------------------------------------------------------------
//
TPtrC8 CWSStarTrustClient::Endpoint()
    {
    return iSTSSession->Endpoint();
    }
//---------------------------------------------------------------------------
// Getter for contract
//---------------------------------------------------------------------------
//
TPtrC8 CWSStarTrustClient::Contract()
    {
    return iSTSSession->Contract();
    }
//---------------------------------------------------------------------------
// Getter for providerId
//---------------------------------------------------------------------------
//
TPtrC8 CWSStarTrustClient::ProviderID()
    {
    return iSTSSession->ProviderID();
    }

//---------------------------------------------------------------------------
// Getter for framework
//---------------------------------------------------------------------------
//
TPtrC8 CWSStarTrustClient::FrameworkId()
    {
    return iSTSSession->FrameworkId();
    }
//---------------------------------------------------------------------------
// serialie to xml
//---------------------------------------------------------------------------
//
void CWSStarTrustClient::WriteAsXMLToL(RWriteStream& aWriteStream)
    {
    iSTSSession->WriteAsXMLToL(aWriteStream);
    }
TInt CWSStarTrustClient::UpdatePolicyL(CWSStarPolicyHandler* aPolicyHandler, MSenServiceDescription* aSD)
    {
    return iSTSSession->UpdatePolicyL(aPolicyHandler, aSD);
    }

MSenServicePolicy* CWSStarTrustClient::ServicePolicy()
    {
    return iSTSSession->ServicePolicy();
    }
TInt CWSStarTrustClient::IapId(TUint32& aCurrentIapId)
    {
    return iSTSSession->IapId(aCurrentIapId);
    }
//---------------------------------------------------------------------------
// randomize entropy for client
//---------------------------------------------------------------------------
//
void CWSStarTrustClient::RandomEntropyClientL()
    {
    HBufC8* hashedRandom = SenCryptoUtils::RandomAndHashMd5LC();
    iEntropyClient = SenCryptoUtils::EncodeBase64L(*hashedRandom);
    CleanupStack::PopAndDestroy(hashedRandom);
    if (!iEntropyClient)
        {
        User::Leave(KErrNotFound);
        }
    }
//---------------------------------------------------------------------------
// setter for ws-trust version
//---------------------------------------------------------------------------
//

void CWSStarTrustClient::SetVersion(const TDesC8* aVersion)
    {
    iVersion = aVersion;
    }
    
//---------------------------------------------------------------------------
// getter for action (depends on version)
//---------------------------------------------------------------------------
//
const TDesC8& CWSStarTrustClient::GetAction()
    {
    if (!iRenewingMode)
        {
        if (*iVersion == KTrustNamespace200502)
            {
            return KActionIssueRST200502;
            }
        else
            {
            return KActionIssueRST200404;   
            }
        }
    else
        {
        if (*iVersion == KTrustNamespace200502)
            {
            return KActionRenewRST200502;      
            }
        else
            {
            return KActionRenewRST200404;
            }
        }
    }
//---------------------------------------------------------------------------
// getter for action response (depends on version)
//---------------------------------------------------------------------------
//
const TDesC8& CWSStarTrustClient::GetActionResponse()
    {
    if (!iRenewingMode)
        {
        if (*iVersion == KTrustNamespace200502)
            {
            return KActionIssueRSTR200502;
            }
        else
            {
            return KActionIssueRSTR200404;   
            }
        }
    else
        {
        if (*iVersion == KTrustNamespace200502)
            {
            return KActionRenewRSTR200502;      
            }
        else
            {
            return KActionRenewRSTR200404;
            }
        }
    }
//---------------------------------------------------------------------------
// getter for request type (depends on version)
//---------------------------------------------------------------------------
//
    
const TDesC8& CWSStarTrustClient::GetRequestType()
    {
    if (!iRenewingMode)
        {
        if (*iVersion == KTrustNamespace200502)
            {
            return KRequestTypeIssueRST200502;
            }
        else
            {
            return KRequestTypeIssueRST200404;   
            }
        }
    else
        {
        if (*iVersion == KTrustNamespace200502)
            {
            return KRequestTypeRenewRST200502;      
            }
        else
            {
            return KRequestTypeRenewRST200404;
            }
        }
    }
//---------------------------------------------------------------------------
// getter for policy namespace(depends on version)
//---------------------------------------------------------------------------
//

const TDesC8& CWSStarTrustClient::GetPolicyNs()
    {
    if (*iVersion == KTrustNamespace200502)
        {
        return KPolicyNamespace200409;
        }
    else
        {
        return KPolicyNamespace200212 ;   
        }
    }

TBool CWSStarTrustClient::HasSuperClass( TDescriptionClassType aType )
    {
    if( aType == MSenServiceDescription::ESenInternalServiceConsumer ) // direct superclass!
        {
        // If asked type is the know *direct* father/mother, return true:
        return ETrue; 
        } 
    else
        {
        // Otherwise, ask from superclass (chain, recursively)
        return CSenInternalServiceConsumer::HasSuperClass( aType ); 
        }
    }

void CWSStarTrustClient::SetHandlerCtx(MSenHandlerContext* aHndCtx)
    {
    iHndCtx = aHndCtx;
    }
TBool CWSStarTrustClient::HasEqualPrimaryKeysL(MSenServiceDescription& aCandidate)
    {
    TBool retVal(EFalse);
    if ( CSenWSDescription::HasEqualPrimaryKeysL(aCandidate) )
        {
        // Endpoint, Contract and FrameworkID match, all of them.
        // Now, in WS-*, also check ProviderID
        if (aCandidate.DescriptionClassType() == DescriptionClassType())
            {
            CWSStarTrustClient& trustCandidate = (CWSStarTrustClient&)aCandidate;
            if (trustCandidate.ProviderID() == ProviderID())
                {
                retVal = ETrue;
                }
            }
        }
    return retVal; 
    }
/*
TBool CWSStarTrustClient::HasEqualPrimaryKeysL(MSenServiceDescription& aCandidate)
    {
    TBool retVal(EFalse);
    //if ( aCandidate.Endpoint() == Endpoint() &&  aCandidate.Contract() == Contract() && aCandidate.FrameworkId() == FrameworkId() )
    if ( CSenWSDescription::HasEqualPrimaryKeysL(aCandidate) )
        {
        // Endpoint, Contract and FrameworkID match, all of them.
        // Now, in WS-*, also check ProviderID
        TPtrC8 providerID = ContentOf( WSStarSession::KProviderIdElementLocalName );
        CSenWSDescription& xmlSD = (CSenWSDescription&)aCandidate;
        CSenElement* idElement = xmlSD.AsElement().Element( WSStarSession::KProviderIdElementLocalName );
        if ( idElement && idElement->Content() == providerID ) 
            {
            // candidate has ProviderID element, which is equal with this sessions's provider ID
            retVal = ETrue;
            }
        }
    return retVal;    
    }
*/

void CWSStarTrustClient::AdaptStsEndpointL(TBool aToOrginal)	//codescannerwarnings
    {
    if (aToOrginal)
        {
        iSTSSession->SetEndPointL(*iSTSOrginalEndpoint);
        return;
        }
    
    delete iSTSOrginalEndpoint;
    iSTSOrginalEndpoint = NULL;
    iSTSOrginalEndpoint = iSTSSession->Endpoint().AllocL();        
    HBufC8* address = NULL;
    if (iTokenType)
        {
        address = CWSStarPolicy::GetPolicyValueL(*iTokenType, KIssuedToken, KAddress, *iHndCtx, iSTSSession);    
        }
    else
        {
        address = CWSStarPolicy::GetPolicyValueL(KIssuedToken, KAddress, *iHndCtx, iSTSSession);    
        }
    
    if (address)
        {
        iSTSSession->SetEndPointL(*address);
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarTrustClient::AdaptStsEndpoint() - endpoint replaced by value from policy");
        }
    delete address;
    address = NULL;
    }
TBool CWSStarTrustClient::Matches(MSenServiceDescription& aOtherServiceDescription)
    {
    return iSTSSession->Matches(aOtherServiceDescription);
    }
void CWSStarTrustClient::SetDataTrafficDetails( TSenDataTrafficDetails& aDetails) 		
	{
	iTrustDetails = aDetails;
	}
// End of file

