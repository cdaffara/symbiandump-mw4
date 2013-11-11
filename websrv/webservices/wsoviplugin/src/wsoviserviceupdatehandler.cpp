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








#include "wsoviserviceupdatehandler.h"
#include "sendebug.h"
#include "senlogger.h"
#include "wsovitokencreationresponse.h"
#include "sencryptoutils.h"
#include "wsovicons.h"
#include "wsovierrorresponse.h"
#include <httpstd.h>
#include <SenHttpTransportProperties.h>
#include "senlayeredhttptransportproperties.h"
#include <SenDateUtils.h>

using namespace OAuth;



class CWSOviHandlerContext;
// Create instance of concrete ECOM interface implementation
CWSOviServiceUpdateHandler* CWSOviServiceUpdateHandler::NewL(TAny* aHandlerCtx)
    {
    
    MSenHandlerContext* handlerCtx =
        reinterpret_cast<MSenHandlerContext*>(aHandlerCtx);
    CWSOviServiceUpdateHandler* self   = new (ELeave) CWSOviServiceUpdateHandler(*handlerCtx);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Constructor
CWSOviServiceUpdateHandler::CWSOviServiceUpdateHandler(MSenHandlerContext& aCtx):CSenSessionHandler(aCtx)
    {
    
    }

// Destructor
CWSOviServiceUpdateHandler::~CWSOviServiceUpdateHandler()
    {
    }

// Second phase construction.
void CWSOviServiceUpdateHandler::ConstructL()
    {
    }
    
TInt CWSOviServiceUpdateHandler::InvokeL(MSenSessionContext& aCtx)
    {
    CWSOviServiceSession* remoteServiceSession =
                               (CWSOviServiceSession*)aCtx.GetSenRemoteServiceSessionL(
                                                           WSOviContextKeys::KServiceSession());                       
    const TDesC8* message = ((CWSOviSessionContext&)aCtx).GetDesC8L(WSOviContextKeys::KMessageBody);
    const TInt* errorCode = ((CWSOviSessionContext&)aCtx).GetIntL(WSOviContextKeys::KErrCode);
    
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::ProcessInboundDispatch")));
    CWSOviServiceSession* oviServiceSession = ((CWSOviServiceSession*)remoteServiceSession);
    CWSOviSessionContext& pCtx = (CWSOviSessionContext&)aCtx;
    //fault scenarios
    if (message && errorCode)
    	{
	    if (*errorCode == EHttp_401_Unauthorized-KHttpPluginStatusBase)
	        {
		 	TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("*errorCode == EHttp_401_Unauthorized-KHttpPluginStatusBase")));
	        CWSOviErrorResponse* errFragment = CWSOviErrorResponse::NewLC();
	        errFragment->SetReader(*iHandlerContext.GetSenCoreServiceManager()->XMLReader());
	        errFragment->BuildFrom(*message);
	        if (! errFragment->Code().Compare(WSOviResponse::KErrorCodeInvalidAccessToken()))
	            {
				 TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L(" ---- WSOviResponse::KErrorCodeInvalidAccessToken()")));
	            if (!pCtx.GetSenRemoteServiceSessionL(WSOviContextKeys::KServiceSession))
	                {
	                pCtx.Add(WSOviContextKeys::KServiceSession, *(MSenRemoteServiceSession*)remoteServiceSession);
	                }
	            pCtx.Update(WSOviContextKeys::KReAuthNeeded, ETrue);
	            oviServiceSession->ClearCredentialL();
	            oviServiceSession->SetStatusL();//in order to compute state
	            }
	        else if(! errFragment->Code().Compare(WSOviResponse::KErrorCodeUnauthorized()))
	            {
			 	TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("----- WSOviResponse::KErrorCodeUnauthorized()")));
	            MSenProperties* tp = (MSenProperties*)pCtx.GetAnyL(WSOviContextKeys::KTP());
	            if (tp) 
	                {
					TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("----- WSOviContextKeys::KTP() Found")));
	                switch (tp->PropertiesClassType())
	                    {
	                    case MSenProperties::ESenHttpTransportProperties:
	                        {
	                        TInt minutes;
	                        ((CSenHttpTransportProperties*)tp)->IntPropertyL(WSOviSession::KDateHttpHeader(), minutes);
	                        pCtx.Update(WSOviContextKeys::KRetryNeeded, minutes);
	                        }    
	                    break;
	                    case MSenProperties::ESenLayeredHttpTransportProperties:
	                        {
	                        TInt minutes;
	                        ((CSenLayeredHttpTransportProperties*)tp)->IntPropertyL(WSOviSession::KDateHttpHeader(), minutes);
	                        pCtx.Update(WSOviContextKeys::KRetryNeeded, minutes);
	                        }    
	                    default:
	                    break;
	                    }
	                }
	            }
	        CleanupStack::PopAndDestroy(errFragment);
	        }
		else if(*errorCode == 403) //Handling 403 resource not found error
			{
			TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("*errorCode == 403 ")));
			CWSOviErrorResponse* errFragment = CWSOviErrorResponse::NewLC();
			errFragment->SetReader(*iHandlerContext.GetSenCoreServiceManager()->XMLReader());
			errFragment->BuildFrom(*message);
			if(! errFragment->Code().Compare(WSOviResponse::KErrorCodeUnauthorized()))
				{			
                    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("----- if WSOviResponse::KErrorCodeUnauthorized()")));
					if (!pCtx.GetSenRemoteServiceSessionL(WSOviContextKeys::KServiceSession))
						{
						pCtx.Add(WSOviContextKeys::KServiceSession, *(MSenRemoteServiceSession*)remoteServiceSession);
						}
					if( ! errFragment->Text().Compare(WSOviResponse::KEmailUsed()) ||
					  (errFragment->Text().Compare(WSOviResponse::KMobileUsed1)!= KErrNotFound &&
					   errFragment->Text().Compare(WSOviResponse::KMobileUsed2) != KErrNotFound )
					  )
					    {
					    pCtx.Update(WSOviContextKeys::KRetryNeeded, EFalse);
					    }
					else
					    {
					    pCtx.Update(WSOviContextKeys::KReAuthNeeded, ETrue);
					    }
					oviServiceSession->ClearCredentialL();
					oviServiceSession->SetStatusL();//in order to compute state
				}
			else
			    {
                TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("-----else  WSOviResponse::KErrorCodeUnauthorized()")));
				if (!pCtx.GetSenRemoteServiceSessionL(WSOviContextKeys::KServiceSession))
				    {
				    pCtx.Add(WSOviContextKeys::KServiceSession, *(MSenRemoteServiceSession*)remoteServiceSession);
				    }
		            pCtx.Update(WSOviContextKeys::KReAuthNeeded, ETrue);
		            oviServiceSession->ClearCredentialL();
		            oviServiceSession->SetStatusL();//in order to compute state                
			    }
			CleanupStack::PopAndDestroy(errFragment);
			}
	    //positive scenario, for example: signup proces already returns token
	    else if (*errorCode == KErrNone)
	        {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviPlugin::ProcessInboundDispatch - token exists");
            CWSOviTokenCreationResponse* responseFragment = CWSOviTokenCreationResponse::NewLC();
            responseFragment->SetReader(*iHandlerContext.GetSenCoreServiceManager()->XMLReader());
            responseFragment->BuildFrom(*message);
            if (responseFragment->IsTokenUpdate())
                {
                TPtrC8 ttl = responseFragment->TTL();
                TBool ttlInfinitive = EFalse;
                if (ttl.Length())
                    {
                    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,ttl);
                    User::LeaveIfError(pCtx.Update(WSOviContextKeys::KTTL,ttl));
                    }
                else
                    {
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviPlugin::ProcessInboundDispatch - ttl is missing");
                    const TInt KMaxTTLLength = 20;
                    TBuf8<KMaxTTLLength> ttlMaxInt;
                    ttlMaxInt.AppendNum(KMaxTInt);
                    User::LeaveIfError(pCtx.Update(WSOviContextKeys::KTTL, ttlMaxInt));
                    ttlInfinitive = ETrue;
                    }
                TPtrC8 tokenSecret = responseFragment->TokenSecret();
                if (tokenSecret.Length())
                    {
                    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,tokenSecret);
                    User::LeaveIfError(pCtx.Update(WSOviContextKeys::KTokenSecret,tokenSecret));
                    }
                HBufC8* created = SenCryptoUtils::GetTimestampL();
                CleanupStack::PushL(created);
                User::LeaveIfError(pCtx.Update(WSOviContextKeys::KTokenCreationTime,*created));        
                CleanupStack::PopAndDestroy(created);
                
                TPtrC8 token = responseFragment->Token();
                if (!token.Length())
                    {
                    pCtx.Remove(WSOviContextKeys::KTokenCreationTime);
                    }
                else
                    {    
                    TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,token);
                    if (!oviServiceSession->ExistConsumerL())
                        {
                        pCtx.Remove(WSOviContextKeys::KTokenCreationTime);
                        }
                    else
                        {
					    TPtrC8 validUntil = responseFragment->ValidUntil();
					    if (validUntil.Length())
					        {
					        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,validUntil);
					        User::LeaveIfError(pCtx.Update(WSOviContextKeys::KTokenValidUntilTime,validUntil));
					        }
						else
							{
							TTime tmpValidUntil = Time::MaxTTime();
							TBuf8<SenDateUtils::KXmlDateTimeMaxLength> pValidUntil;
							SenDateUtils::ToXmlDateTimeUtf8L(pValidUntil, tmpValidUntil);
					        User::LeaveIfError(pCtx.Update(WSOviContextKeys::KTokenValidUntilTime,pValidUntil));
							}
                       
                        pCtx.SetTokenKeysL(token);
                        }
                    }
                if (!ttlInfinitive)
                    {
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviPlugin::ProcessInboundDispatch - Call ActiveTicketObserverL()");
                    oviServiceSession->ActiveTicketObserverL();
                    }

                TPtrC8 username = responseFragment->Username();
                if (username.Length())
                    {
                    oviServiceSession->SetTrustAnchorL(username);
                    oviServiceSession->SetProviderIDL(username);
                    }
                }
            CleanupStack::PopAndDestroy(responseFragment);
            pCtx.Update(WSOviContextKeys::KRetryNeeded, 0);
	        }
		else
			{
	        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"---- Neither 401 nor KErrNone !!");
			}
    	}
    return KErrNone;
    }
    
SenHandler::THandlerDirection CWSOviServiceUpdateHandler::Direction() const
    {
        return SenHandler::EIncoming;
    };
SenHandler::THandlerPhase CWSOviServiceUpdateHandler::Phase()
    {
        return SenHandler::EMessage;
    };


    
//---------------------------------------------------------------------------
// Logger using during DEBUG mode
//---------------------------------------------------------------------------
//    

TInt CWSOviServiceUpdateHandler::InitL(MSenHandlerContext& aCtx)
    {
    iHandlerContext = aCtx;
    return KErrNone;
    }
    
// END OF FILE

