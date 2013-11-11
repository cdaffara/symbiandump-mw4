/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "wsovioauthclient.h"

#include <s32mem.h>
#include <f32file.h>
#include <s32file.h>
#include <SenXmlUtils.h>
#include "sencryptoutils.h"

#include "wsoviservicesession.h"
#include "SenXmlUtils.h"
#include "msencoreservicemanager.h"
#include "sendebug.h"
#include "senlogger.h"
#include "MSenServiceDescription.h"
#include "SenServiceConnection.h"
#include "seninternalserviceconsumer.h"
#include <SenCredential.h>
#include "wsoviplugin.h"
#include "SenHttpTransportProperties.h"
#include "senwspattern.h"
#include "wsovicons.h"
#include "wsovitokencreationresponse.h"
#include "sencryptoutils.h"
#include "wsoviutils.h"
#include <SenDateUtils.h>

namespace 
    {
    _LIT8(KRemoteConsumer,       "RemoteConsumer");
    }


using namespace OAuth;
//---------------------------------------------------------------------------
// Creates and returns a new instace of this class.
//---------------------------------------------------------------------------
//
CWSOviOAuthClient* CWSOviOAuthClient::NewL(
                                                        CSIF& aSIF)
    {
    CWSOviOAuthClient* pNew = NewLC(aSIF);
    CleanupStack::Pop(pNew);
    return pNew;
    }

//---------------------------------------------------------------------------
// Creates and returns a new instace of this class and leaves the object on the cleanup stack
//---------------------------------------------------------------------------
//
CWSOviOAuthClient* CWSOviOAuthClient::NewLC(
                                                        CSIF& aSIF)
    {
    CWSOviOAuthClient* pNew =
        new (ELeave) CWSOviOAuthClient(
                    aSIF,
                    MSenServiceDescription::EOviOAuthClient);

    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }

//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
//
CWSOviOAuthClient::CWSOviOAuthClient(
                                                CSIF& aSIF,
                                                TDescriptionClassType aType)
    : CSenInternalServiceConsumer(aType, aSIF)
    {
    }

void CWSOviOAuthClient::BaseConstructL()
    {
    CSenInternalServiceConsumer::ConstructL();
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviOAuthClient::ConstructL()")));
    }

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
//
CWSOviOAuthClient::~CWSOviOAuthClient()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviOAuthClient::~CWSOviOAuthClient");
    delete iAuthSession;
    delete iBody;
    }


//---------------------------------------------------------------------------
// The most siginificant method of this class. Make validation of session, 
//    credentails obtained if needed.
//    Sequential logic, synchronized, 
//---------------------------------------------------------------------------
//
TInt CWSOviOAuthClient::ValidateL( CWSOviServiceSession& aSession,
                                    HBufC8*& aErrorMessage,
                                    TBool aOnlySharing )

    {
//    iAuthSession->CompleteServerMessagesOnOffL(ETrue);
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviOAuthClient::ValidateL(..)");
    TInt retVal(KErrNone);            // for returning error codes
    iWSOviServiceSession = &aSession;            // save session
    
//----------------------
//0  token sharing, only if scope of token is not applied
//----------------------
    TBool sharingSucceed = EFalse;
    RPointerArray<CSenWSDescription> oviSDs;
    CleanupClosePushL(oviSDs);
    CSenWSPattern* pattern = CSenWSPattern::NewLC();
 
    //select * from session_of_SD where session is (ovi framework, and has same provideId && No scope of token)
    
    pattern->SetFrameworkIdL(KDefaultOviFrameworkID);
    pattern->AsElement().AddElementL(KSenIdpProviderIdLocalname).SetContentL(iIdentityProvider->ProviderID());
    //if (iWSOviServiceSession->ScopeToken() == EFalse) then pattern->SetScopeToken(EFalse) 
    CWSOviServiceSession* oviSession = NULL;
    TRAP(retVal, retVal = Framework().Manager().ServiceDescriptionsL(oviSDs, *pattern));

    if(!retVal)
        {
        TInt count = oviSDs.Count();
        for(TInt i = 0; i < oviSDs.Count(); i++)
            {
            //all sessions, to eliminate client, just for case we check type
            TDescriptionClassType dt = oviSDs[i]->DescriptionClassType();
            if (dt == MSenServiceDescription::EOviServiceSession)
                {
                oviSession = (CWSOviServiceSession*)oviSDs[i];
                //dont share to itself
                if (oviSession != iWSOviServiceSession)
                    {
                    TRAPD(err, retVal = oviSession->ShareTokenWithL(
                            iWSOviServiceSession));
                    if ((retVal == KErrNone) && (err == KErrNone))
                        {
                        sharingSucceed = ETrue;
                        break;
                        }    
                    }
                }
            }
        if ( !sharingSucceed )
            {
            // Did not find valid token      (<=> sharingSucceed     == EFalse)
            // 
            // Try to search valid toeken directly from CredentialManager
            // (This is needed especially in case when token is imported
            //  through Public API
            //  => There is not yet Session which holds imported token)
            if ( iWSOviServiceSession->TryToSearchValidCredentialL() == KErrNone )	//codescannerwarnings
                {
                iWSOviServiceSession->SetStatusL();
                iWSOviServiceSession->AddSecurityTokenToContextL();
                sharingSucceed = ETrue;
                }
            }
        }
    CleanupStack::PopAndDestroy(2, &oviSDs);
        
    
    if (sharingSucceed)
        {
        iWSOviServiceSession->SetTrustAnchorL(iIdentityProvider->ProviderID());
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviOAuthClient::ValidateL(..) - sharing succed, we don't need more validation");
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

    if (!iAuthSession)// if AuthenticationClient has NOT been registered earlier
        {
        iAuthSession = CWSOviServiceSession::NewL(Framework());
        iAuthSession->InitializeFromL(*this);
        iAuthSession->SetEndPointL(iIdentityProvider->Endpoint());
        iAuthSession->SetContractL(KWSOviAuthenticationServiceContract);
        iAuthSession->AddConsumerL(*this);
        }
    CSenXmlReader* reader = Framework().Manager().XMLReader();
    CWSOviSessionContext* NAUTHSsnContext = iAuthSession->SessionContext();
    
    

//-----------------
//2.preparing context    
//-----------------------
    TInt retryCounter(0);
    TBool retry(ETrue);
    TBool authenticationInfoPromptedFromUser(EFalse);
    TPckgBuf<TSenAuthentication> authInfo;
    HBufC8* response = NULL;
    MSenRemoteServiceConsumer* pConsumer = NULL;
                
    while ( retry ) // Authentication loop starts
        {
        retry = EFalse;
        //building body
        //CWSOviPlugin& frmWSOvi = (CWSOviPlugin&)Framework();
        const TDesC8* version  = iHndCtx->GetDesC8L(HandlerContextKey::KVersion);
        TTime time;
        time.UniversalTime();
        
        TPtrC8 userName(KNullDesC8);
        TPtrC8 password(KNullDesC8);
        if ( authenticationInfoPromptedFromUser )
            {
            userName.Set(authInfo().iUsername);
            password.Set(authInfo().iPassword);
            }
        else
            {
            userName.Set(iIdentityProvider->UserName());
            password.Set(iIdentityProvider->Password());
            }
        HBufC8* created(NULL);
        //CWSOviSessionContext* sessioncontext = iWSOviServiceSession->SessionContext();
        const TInt* diffHandlerContext = iHndCtx->GetIntL(HandlerContextKey::KDiff);
        if( diffHandlerContext && *diffHandlerContext > 0 )
            {
            TTimeIntervalMinutes minutes(*diffHandlerContext);
            TTime diff = time - minutes;
            time -= TTimeIntervalMicroSeconds(diff.Int64());
            created = SenCryptoUtils::GetTimestampL(time);
            }
        else
            {
            created = SenCryptoUtils::GetTimestampL();
            }
        CleanupStack::PushL(created);
        CreateBodyL(*version,
                    userName,
                    password,
					*created);
        CleanupStack::PopAndDestroy(created);

        
        //-----------------    
        //_____________________________________SYNC Sending____________________________________________

        CSenHttpTransportProperties* transProp = CSenHttpTransportProperties::NewLC();
        
        // IAP dialog used to appear though IAP id is provided as a part of Service Description
        // Get the IAP setting and accordingly set the transport properties
    	TUint32 id = 0;
    	TInt getIapRetCode = IapId(id);
    	
    	// Check whether IAP ID is pre-defined, and copy it only if such property value is found
    	//if ( getIapRetCode == KErrNone && id < 4294967295) // unsigned 4294967295 == -1 signed
    	if ( getIapRetCode == KErrNone && id < (TUint32)KErrNotFound) // unsigned 4294967295 == -1 signed
    	    {
    	    transProp->SetIapIdL(id); 
    	    }
    	// ETMI-6WAGDB end
    	
    	//limit http timeout, 
    	//  if NAUTH hangs and can not send any response, then we block every async client method.
    	//  Possibel revalidation during sendL can take too long time therefore we create new limit
    	transProp->SetMaxTimeToLiveL(WSOviSession::KMaxHttpTimeOut);
		transProp->SetBoolPropertyL(OAuth::KNeedSecurity(), EFalse);
		transProp->SetEndpointResourceL(WSOviSession::KTokenEndpointSuffix());
//    	transProp->SetContentTypeL(KFormUrlEncoded());
		
//set consumer keys for proper DB		
		HBufC8* providerTP(NULL); 
		iIdentityProvider->TransportPropertiesL(providerTP);
		if (providerTP)
		    {
        	CleanupStack::PushL(providerTP);
        	CSenHttpTransportProperties* httpProviderTP = CSenHttpTransportProperties::NewLC(*providerTP, *reader);
        	if (httpProviderTP)
        		{
        		TPtrC8 content(KNullDesC8);
        		httpProviderTP->PropertyL(KNCIMConsumerKeyId , content);
        		if (content.Length())
        			{
        			transProp->SetPropertyL(KNCIMConsumerKeyId, content);
        			}
        		httpProviderTP->PropertyL(KNCIMConsumerSecret , content);
        		if (content.Length())
        			{
        			transProp->SetPropertyL(KNCIMConsumerSecret, content);
        			}
        		httpProviderTP->PropertyL(KNCIMConsumerServiceId , content);
        		if (content.Length())
        			{
        			transProp->SetPropertyL(KNCIMConsumerServiceId, content);
        			}
        		/*TUint32 id = 0;
                if (! httpProviderTP->SnapIdL(id))
                      {
                      transProp->SetSnapIdL(id);
                      }*/
                CleanupStack::PopAndDestroy(httpProviderTP);
        		}
            CleanupStack::PopAndDestroy(providerTP);
		    }
		
		// setting SNAP/IAP 
            if(!iAuthSession->IapId(id) )
                {
                transProp->SetIapIdL(id);
                transProp->SetSnapIdL(0);
                }
            else if(!iAuthSession->SnapId(id) )
                {
                transProp->SetSnapIdL(id);
                transProp->SetIapIdL(0);
                }
	      

        HBufC8* transPropAsXml = transProp->AsUtf8LC();
//        HBufC8* bodyEncodedUrl = CWSOviUtils::FormEncodeCharsLC(*pBody);
//        _LIT8(KXmlContent, "XMLContent=");
//        HBufC8* allBody = HBufC8::NewL(KXmlContent().Length()+bodyEncodedUrl->Length());
//        allBody->Des().Append(KXmlContent());
//        allBody->Des().Append(*bodyEncodedUrl);
//        CleanupStack::PushL(allBody);
//-----------------
//3.submitting
//-------------------        
        retVal = iAuthSession->SubmitL(*iBody, *transPropAsXml, *this, response);

        aSession.SetTransportL(ipTransport);//ownership moved
        ipTransport = NULL;
        
        if (!response)
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviOAuthClient::ValidateL - Response is empty - 1");
            CleanupStack::PopAndDestroy(transPropAsXml);
            CleanupStack::PopAndDestroy(transProp);
            return retVal;
            }

        const TInt* diffContext = iHndCtx->GetIntL(HandlerContextKey::KDiff);
        if (diffContext && *diffContext > 0)
            {
            delete response;
            response = NULL;
            TTime time;
            time.UniversalTime();
            TTimeIntervalMinutes minutes(*diffContext);
            TTime diff = time-minutes;
            time -= TTimeIntervalMicroSeconds(diff.Int64());
            HBufC8* created = SenCryptoUtils::GetTimestampL(time);
            CleanupStack::PushL(created);
            CreateBodyL(*version,
                        userName,
                        password,
                        *created);
            CleanupStack::PopAndDestroy(created);
            retVal = iAuthSession->SubmitL(*iBody, *transPropAsXml, *this, response);
            aSession.SetTransportL(ipTransport);//ownership moved
            ipTransport = NULL;
            }
        
        CleanupStack::PopAndDestroy(transPropAsXml);
        CleanupStack::PopAndDestroy(transProp);
        CleanupStack::PushL(response);

        if ( retVal )
            {
            CWSOviSessionContext* pSessionContext = iWSOviServiceSession->SessionContext();
            TAny* ptr = pSessionContext->GetAnyL(KRemoteConsumer());	//codescannerwarnings
            if ( ptr )
                {
                pConsumer = REINTERPRET_CAST(MSenRemoteServiceConsumer*, ptr);
                }
            else
                {
                pConsumer = iWSOviServiceSession->Consumer();
                }
            
            if ( pConsumer && !pConsumer->HasAuthenticationCallback() &&
                 retryCounter < 3 )
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviOAuthClient::ValidateL - Consumer does not have AuthenticationCallback");
                
                const TDesC8* pValue = iIdentityProvider->AsElement().AttrValue( KSenAttrPromptUserInfo );                
/* #ifdef _RD_SEN_WS_STAR_DO_NOT_PROMPT_AUTHINFO_BY_DEFAULT */ // For OVI no Notifier Plugin is popped up
                if ( pValue && *pValue != _L8("false") )
                // attribute has to exist, otherwise logic defaults to: DO NOT PROMPT
//#else
//                if ( !pValue || *pValue != _L8("false") )
                // even when attribute does NOT exist, or when it does and has any other value but "false", default to: PROMPT
//#endif                
                    {
                    const TBool* renewingPtr = NAUTHSsnContext->GetIntL(WSOviContextKeys::KReAuthNeeded);
					if ( renewingPtr && *renewingPtr )
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
                else
                    { 
                    //Special Case handled for OVI Frameworks
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"------- Notifier plugin will not be used for OVI What about UI Less SignIn !!!");
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel," => prompt is not used.");
                    }
                }
            if ( pConsumer && pConsumer->HasAuthenticationCallback() &&
                             retryCounter < 3 )
            	{
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"CWSOviOAuthClient::ValidateL - Consumer has AuthenticationCallback");
                
                const TDesC8* pValue = iIdentityProvider->AsElement().AttrValue( KSenAttrPromptUserInfo );
                
                if ( !pValue || *pValue != _L8("false") )
                    {
                    const TBool* renewingPtr = NAUTHSsnContext->GetIntL(WSOviContextKeys::KReAuthNeeded);
                    if ( renewingPtr && *renewingPtr )
                        {
                        retVal = KErrSenFailedAuthentication;
                        }
                    }
                else
                    {
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"CWSOviOAuthClient::ValidateL - UserInfoPrompt = false");
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel," => AuthenticationCallback is not used.");
                    }
                
            	}
            }
            
        if ( retry )
            {
            CleanupStack::PopAndDestroy(response);
            }
        }  // Authentication loop ends

    HBufC8* ttlCreated = SenCryptoUtils::GetTimestampL();
    CleanupStack::PushL(ttlCreated);
    User::LeaveIfError(iWSOviServiceSession->SessionContext()->Update(WSOviContextKeys::KTokenCreationTime,*ttlCreated));
    CleanupStack::PopAndDestroy(ttlCreated);
    
    
    if (retVal != KErrNone)
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CWSOviOAuthClient::ValidateL - error when submit to NAUTH %d"), retVal));
        iWSOviServiceSession->SessionContext()->Remove(WSOviContextKeys::KTokenCreationTime);

        if (response)
            {
            aErrorMessage = response;
            }
        
        CleanupStack::Pop(response);
        return retVal;
        }
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviOAuthClient::ValidateL - response from NAUTH is a valid SOAP");
//-----------------
//4 building response    
//-----------------
    CWSOviTokenCreationResponse* responseFragment = CWSOviTokenCreationResponse::NewLC();
    responseFragment->SetReader(*Framework().Manager().XMLReader());
    responseFragment->BuildFrom(*response);

    TPtrC8 ttl = responseFragment->TTL();
    TBool ttlInfinitive = EFalse;
//        _LIT8(KTempTTL,"1000");
    if (ttl.Length())
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,ttl);
        User::LeaveIfError(iWSOviServiceSession->SessionContext()->Update(WSOviContextKeys::KTTL,ttl));
        }
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviOAuthClient::ValidateL - ttl is missing");
        const TInt KMaxTTLLength = 20;
        TBuf8<KMaxTTLLength> ttlMaxInt;
        ttlMaxInt.AppendNum(KMaxTInt);
        User::LeaveIfError(iWSOviServiceSession->SessionContext()->Update(WSOviContextKeys::KTTL,ttlMaxInt));
        ttlInfinitive = ETrue;
        }
    TPtrC8 tokenSecret = responseFragment->TokenSecret();
    if (tokenSecret.Length())
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,tokenSecret);
        User::LeaveIfError(iWSOviServiceSession->SessionContext()->Update(WSOviContextKeys::KTokenSecret,tokenSecret));
        }
    TPtrC8 token = responseFragment->Token();
    if (!token.Length())
        {
        iWSOviServiceSession->SessionContext()->Remove(WSOviContextKeys::KTokenCreationTime);
        retVal = KErrNotFound;
        }
    else
    	{    
        TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,token);
	    if (!iWSOviServiceSession->ExistConsumerL())
	        {
	        iWSOviServiceSession->SessionContext()->Remove(WSOviContextKeys::KTokenCreationTime);
	        retVal = KErrNotFound;
	        }
	    else
	        {
	        iWSOviServiceSession->SetTrustAnchorL(iIdentityProvider->ProviderID());
	        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviOAuthClient::ValidateL - set ctx basing on response");
	        
		    TPtrC8 validUntil = responseFragment->ValidUntil();
		    if (validUntil.Length())
		        {
		        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,validUntil);
		        User::LeaveIfError(iWSOviServiceSession->SessionContext()->Update(WSOviContextKeys::KTokenValidUntilTime,validUntil));
		        }
			else
				{
				TTime tmpValidUntil = Time::MaxTTime();
				TBuf8<SenDateUtils::KXmlDateTimeMaxLength> pValidUntil;
				SenDateUtils::ToXmlDateTimeUtf8L(pValidUntil, tmpValidUntil);
		        User::LeaveIfError(iWSOviServiceSession->SessionContext()->Update(WSOviContextKeys::KTokenValidUntilTime,pValidUntil));				
				}	
	        
	        retVal = iWSOviServiceSession->SessionContext()->SetTokenKeysL(token);
	        if (retVal)
	            {
	            aErrorMessage = response;
	            }
	        }
    	}
    CleanupStack::PopAndDestroy(responseFragment);
    
    if (!aErrorMessage)
        {
        if (!ttlInfinitive)
            {
            iWSOviServiceSession->ActiveTicketObserverL();
            }
        CleanupStack::PopAndDestroy(response);
        }
    else
        {
        CleanupStack::Pop(response);
        }
    return retVal;
    }



//---------------------------------------------------------------------------
// SetStatus
//---------------------------------------------------------------------------
//

void CWSOviOAuthClient::SetStatus(TInt /*aStatus */)
    {
    // No implementation in Java
     }

//---------------------------------------------------------------------------
// SetAccount
//---------------------------------------------------------------------------
//
void CWSOviOAuthClient::SetAccount(
                                    CSenIdentityProvider* aIdentityProvicer)
    {
    iIdentityProvider = aIdentityProvicer;
    }


//---------------------------------------------------------------------------
// ServiceSession
//---------------------------------------------------------------------------
//
CSenServiceSession* CWSOviOAuthClient::ServiceSession()
    {
    return iWSOviServiceSession;
    }

//---------------------------------------------------------------------------
// Obtain Transport using a factory from Core Manager
//---------------------------------------------------------------------------
//
MSenTransport& CWSOviOAuthClient::TransportL()
    {
    if(!ipTransport)
        {
        // Core is capable of working as transport factory, too.
        // The returned transport instancies may or may not be
        // ECOM plugins:
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviOAuthClient::Instantiating new transport");
        ipTransport = Framework().Manager().CreateL(*iAuthSession, iAuthSession); 
        }
    return *ipTransport;
    }
//---------------------------------------------------------------------------
// SetNAUTHSessionL
//---------------------------------------------------------------------------
//
void CWSOviOAuthClient::SetAuthSessionL(
                                                CWSOviServiceSession* aSession)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviOAuthClient::SetServiceSessionL");
    _LIT8(KTouch, "touch");
    // free memory
    //delete iAuthSession;
    const TDesC8* attrValue = aSession->AsElement().AttrValue(KTouch);
    if(attrValue != NULL)
    	{
    	AsElement().AddAttrL(KTouch, *attrValue);
    	}
    iAuthSession = aSession;
    if(iAuthSession)
        {
        iAuthSession->AddConsumerL(*this);
        }
    }


//---------------------------------------------------------------------------
// Getter for endpoint
//---------------------------------------------------------------------------
//
TPtrC8 CWSOviOAuthClient::Endpoint()
    {
    return iAuthSession->Endpoint();
    }
//---------------------------------------------------------------------------
// Getter for contract
//---------------------------------------------------------------------------
//
TPtrC8 CWSOviOAuthClient::Contract()
    {
    return iAuthSession->Contract();
    }
//---------------------------------------------------------------------------
// Getter for providerId
//---------------------------------------------------------------------------
//
TPtrC8 CWSOviOAuthClient::ProviderID()
    {
    return iAuthSession->ProviderID();
    }

//---------------------------------------------------------------------------
// Getter for framework
//---------------------------------------------------------------------------
//
TPtrC8 CWSOviOAuthClient::FrameworkId()
    {
    return iAuthSession->FrameworkId();
    }
//---------------------------------------------------------------------------
// serialie to xml
//---------------------------------------------------------------------------
//
void CWSOviOAuthClient::WriteAsXMLToL(RWriteStream& aWriteStream)
    {
    iAuthSession->WriteAsXMLToL(aWriteStream);
    }

MSenServicePolicy* CWSOviOAuthClient::ServicePolicy()
    {
    return iAuthSession->ServicePolicy();
    }
TInt CWSOviOAuthClient::IapId(TUint32& aCurrentIapId)
    {
    return iAuthSession->IapId(aCurrentIapId);
    }
    


TBool CWSOviOAuthClient::HasSuperClass( TDescriptionClassType aType )
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

void CWSOviOAuthClient::SetHandlerCtx(MSenHandlerContext* aHndCtx)
    {
    iHndCtx = aHndCtx;
    }
TBool CWSOviOAuthClient::HasEqualPrimaryKeysL(MSenServiceDescription& aCandidate)
    {
    TBool retVal(EFalse);
    if ( CSenWSDescription::HasEqualPrimaryKeysL(aCandidate) )
        {
        // Endpoint, Contract and FrameworkID match, all of them.
        // Now, in WS-Ovi, also check ProviderID
        if (aCandidate.DescriptionClassType() == DescriptionClassType())
            {
            CWSOviOAuthClient& AuthenticationCandidate = (CWSOviOAuthClient&)aCandidate;
            if (AuthenticationCandidate.ProviderID() == ProviderID())
                {
                retVal = ETrue;
                }
            }
        }
    return retVal; 
    }

void CWSOviOAuthClient::CreateBodyL(const TDesC8& aXmlNs, const TDesC8& aUsername, const TDesC8& aPassword, const TDesC8& aCreated)
    {
    if(aPassword == KNullDesC8())
    	{
    	TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviOAuthClient::CreateBodyL() - aPassword == KNullDesC8")));
    	}
    delete iBody;
    iBody = NULL;
    HBufC8* nonce = SenCryptoUtils::GetRandomNonceL();
    CleanupStack::PushL(nonce);
    HBufC8* passDec = SenXmlUtils::DecodeHttpCharactersLC( aPassword );
    HBufC8* digest = CWSOviUtils::DigestAuthStringL(*nonce, aCreated, aUsername, *passDec);
    CleanupStack::PopAndDestroy(passDec);
    CleanupStack::PushL(digest);
                    
    iBody = HBufC8::NewL(KCreationRequestFormat8().Length()
    						+ aXmlNs.Length()
                            + aUsername.Length()
                            + nonce->Length()
                            + aCreated.Length()
                            + digest->Length());
    TPtr8 ptr = iBody->Des();
    ptr.Format(KCreationRequestFormat8,
    		   &aXmlNs,
               &aUsername,
               &(nonce->Des()),
               &aCreated,
               &(digest->Des()));
    CleanupStack::PopAndDestroy(digest);
    CleanupStack::PopAndDestroy(nonce);
    }

TBool CWSOviOAuthClient::Matches(MSenServiceDescription& aOtherServiceDescription)
    {
    return iAuthSession->Matches(aOtherServiceDescription);
    }
// End of file

