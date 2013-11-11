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








#include "wsoviregisterhandler.h"
#include "wsovioauthclient.h"
#include "sendebug.h"
#include "senlogger.h"


class CWSOviHandlerContext;
// Create instance of concrete ECOM interface implementation
CWSOviRegisterHandler* CWSOviRegisterHandler::NewL(TAny* aHandlerCtx)
    {
    
    MSenHandlerContext* handlerCtx =
        reinterpret_cast<MSenHandlerContext*>(aHandlerCtx);
    CWSOviRegisterHandler* self   = new (ELeave) CWSOviRegisterHandler(*handlerCtx);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Constructor
CWSOviRegisterHandler::CWSOviRegisterHandler(MSenHandlerContext& aCtx):CSenSessionHandler(aCtx)
    {
    
    }

// Destructor
CWSOviRegisterHandler::~CWSOviRegisterHandler()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviRegisterHandler::~CWSOviRegisterHandler()");
    }

// Second phase construction.
void CWSOviRegisterHandler::ConstructL()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviRegisterHandler::ConstructL()");
    }
    
TInt CWSOviRegisterHandler::InvokeL(MSenSessionContext& aCtx)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviRegisterHandler::InvokeL()");
    MSenServiceDescription& pServiceDescription = *(MSenServiceDescription*)aCtx.GetSenWSDescriptionL(WSOviContextKeys::KServiceDescription());
    const TDesC8* action = aCtx.GetDesC8L(WSOviContextKeys::KRegisterAction());
    TInt result(KErrNone);
    if (*action == WSOviContextValues::KActionRegister())
        {
        result = RegisterServiceDescriptionL(pServiceDescription);
        }
    else if (*action == WSOviContextValues::KActionUnregister())
        {
        result = UnregisterServiceDescriptionL(pServiceDescription);
        };
    return result;  
    }
    
SenHandler::THandlerDirection CWSOviRegisterHandler::Direction() const
    {
        return SenHandler::EBoth;
    };
SenHandler::THandlerPhase CWSOviRegisterHandler::Phase()
    {
        return SenHandler::EDiscovery;
    };

//---------------------------------------------------------------------------
// Attempt to register the ServiceDescription to the ServiceManager 
//---------------------------------------------------------------------------
//
TInt CWSOviRegisterHandler::RegisterServiceDescriptionL(MSenServiceDescription& aServiceDescription )
    {
    TInt retval(KErrNone);
    CWSOviServiceSession* pSession = NULL;
    TPtrC8 contract = aServiceDescription.Contract();
    TPtrC8 endpoint = aServiceDescription.Endpoint();

    if (contract == KNullDesC8) 
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviRegisterHandler::RegisterServiceDescriptionL failed - KErrSenNoContract");
        retval = KErrSenNoContract;
        }
    else if(endpoint == KNullDesC8)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviRegisterHandler::RegisterServiceDescriptionL failed - KErrSenNoEndpoint");
        retval = KErrSenNoEndpoint;
        }
    else
        {
        if(contract == KWSOviAuthenticationServiceContract)
            {
            retval = RegisterAuthClientL(&aServiceDescription);
            }
        else if (aServiceDescription.DescriptionClassType() ==
                        MSenServiceDescription::EOviServiceSession)
            {
            //  description already is a session so we just add to core-DAO
            //  usecase:    when connection has been made, and service is registered 
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviRegisterHandler::RegisterServiceDescriptionL - session is already so just adding")));
                pSession = (CWSOviServiceSession*) &aServiceDescription;
                retval = iHandlerContext.GetSenCoreServiceManager()->AddServiceDescriptionL(pSession);
            }
        else
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviRegisterHandler::RegisterServiceDescriptionL - Creating session from description...")));
            pSession = CWSOviServiceSession::NewLC(*(CSIF*)iHandlerContext.GetAnyL(HandlerContextKey::KSIF()));	//codescannerwarnings
            //usecase:  when description has been read from db during bootup
            //          session has to be initialized (set validity time for example)
            retval = pSession->InitializeFromL(aServiceDescription);
            if (retval == KErrNone)
                {
                retval = iHandlerContext.GetSenCoreServiceManager()->AddServiceDescriptionL(pSession);
                CleanupStack::Pop(pSession);
                }
            else
                {
                CleanupStack::PopAndDestroy(pSession);
                }
            }
        }
    return retval;
    }
    
    
//---------------------------------------------------------------------------
// Attempt to unregister the ServiceDescription from the ServiceManager 
//---------------------------------------------------------------------------
//
TInt CWSOviRegisterHandler::UnregisterServiceDescriptionL(
    MSenServiceDescription& aServiceDescription)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviRegisterHandler::UnregisterServiceDescriptionL()");
    TInt retval(KErrNone);
    CWSOviServiceSession *pSession = NULL;
    TPtrC8 contract = aServiceDescription.Contract();

    if(contract == KWSOviAuthenticationServiceContract)
        {
        retval = UnRegisterAuthClientL(&aServiceDescription);
        }
    else
        {
        if(aServiceDescription.DescriptionClassType() ==
                MSenServiceDescription::EOviServiceSession)
            {
            pSession = (CWSOviServiceSession*) &aServiceDescription;
            retval = iHandlerContext.GetSenCoreServiceManager()->RemoveServiceDescriptionL(*pSession);
            }
        else
            {

            pSession = CWSOviServiceSession::NewLC(*(CSIF*)iHandlerContext.GetAnyL(HandlerContextKey::KSIF()));	//codescannerwarnings
            retval = pSession->InitializeFromL(aServiceDescription);
            if (retval == KErrNone)
                {
                RWSDescriptionArray sessions;
                retval = iHandlerContext.GetSenCoreServiceManager()->ServiceDescriptionsL(sessions,aServiceDescription);
                if (!retval && sessions.Count())
                    {
                    RServiceConsumerArray consumers;
                    ((CWSOviServiceSession*)sessions[0])->Consumers(consumers);
                    TInt count = consumers.Count();
                    consumers.Reset();
                    consumers.Close();
                    sessions.Reset();
                    sessions.Close();
                    if (count)
                        {
                        CleanupStack::PopAndDestroy(pSession);
                        return KErrInUse;
                        }
                    }
                retval = iHandlerContext.GetSenCoreServiceManager()->RemoveServiceDescriptionL(*pSession);
                }
            CleanupStack::PopAndDestroy(pSession);
            }
        }

    return retval;
    }    

//---------------------------------------------------------------------------
// Register specific description (Auth contract)
//---------------------------------------------------------------------------
//    
TInt CWSOviRegisterHandler::RegisterAuthClientL(
                                MSenServiceDescription *aServiceDescription)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviRegisterHandler::RegisterNAUTHClientL");
    
    TPtrC8 endpoint = aServiceDescription->Endpoint();
    TInt retval = KErrNone;
    CWSOviServiceSession* pSession = NULL;
    CWSOviOAuthClient* pAuthClient = NULL;

    if(aServiceDescription->DescriptionClassType() ==
                MSenServiceDescription::EOviOAuthClient)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviRegisterHandler::RegisterNAUTHClient - This ServiceDescription is already an NAUTH Client")));
        pAuthClient = (CWSOviOAuthClient*)aServiceDescription;
        retval = iHandlerContext.GetSenCoreServiceManager()->AddServiceDescriptionL(pAuthClient);
        }
    else
        {
        // Create new auth client and initialize
        // it from given description
        pSession = CWSOviServiceSession::NewLC(*(CSIF*)iHandlerContext.GetAnyL(HandlerContextKey::KSIF()));
        retval = pSession->InitializeFromL(*aServiceDescription);

        if(retval != KErrNone)
            {
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CWSOviRegisterHandler::RegisterNAUTHClientL failed %d"), retval));
            CleanupStack::PopAndDestroy(pSession);
            return retval;
            }
        TPtrC8 sessionEndpoint = pSession->Endpoint();
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CWSOviRegisterHandler::RegisterNAUTHClientL created NAUTH using endpoint:'%S'"), &sessionEndpoint));

        pAuthClient = CWSOviOAuthClient::NewLC(*(CSIF*)iHandlerContext.GetAnyL(HandlerContextKey::KSIF()));	//codescannerwarnings
        pAuthClient->SetAuthSessionL(pSession);    // pSession will be owned by Auth Client
        CleanupStack::Pop(pAuthClient);
        CleanupStack::Pop(pSession);
        CleanupStack::PushL(pAuthClient);

        retval = iHandlerContext.GetSenCoreServiceManager()->AddServiceDescriptionL(pAuthClient);
        if(retval != KErrNone)
            {
            CleanupStack::PopAndDestroy(1);//pAuthClient
            }
        else
        	CleanupStack::Pop(pAuthClient);
        pAuthClient = NULL;
        }
    return retval;
    }

//---------------------------------------------------------------------------
// Unregister specific description (Auth contract)
//---------------------------------------------------------------------------
//    

TInt CWSOviRegisterHandler::UnRegisterAuthClientL(
                                MSenServiceDescription *aServiceDescription)
    {
    TInt retval = KErrNone;

    CWSOviServiceSession* pSession = NULL;
    CWSOviOAuthClient* pAuthClient = NULL;

    if(aServiceDescription->DescriptionClassType() ==
                MSenServiceDescription::EOviOAuthClient)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviRegisterHandler::UnRegisterAuthClientL - This ServiceDescription is already an NAUTH Client")));
        pAuthClient = (CWSOviOAuthClient*)aServiceDescription;
        retval = iHandlerContext.GetSenCoreServiceManager()->RemoveServiceDescriptionL(*pAuthClient);
        }
    else
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviRegisterHandler::UnRegisterAuthClientL - This ServiceDescription is not NAUTH Client")));
        pSession = CWSOviServiceSession::NewLC(*(CSIF*)iHandlerContext.GetAnyL(HandlerContextKey::KSIF()));
        retval = pSession->InitializeFromL(*aServiceDescription);
        if(retval != KErrNone)
            {
            CleanupStack::PopAndDestroy(pSession);
            return retval;
            }
        pAuthClient = CWSOviOAuthClient::NewLC(*(CSIF*)iHandlerContext.GetAnyL(HandlerContextKey::KSIF));	
        pAuthClient->SetAuthSessionL(pSession);    // pSession will be owned by Auth Client
        CleanupStack::Pop(pAuthClient);
        CleanupStack::Pop(pSession);
        CleanupStack::PushL(pAuthClient);

        retval = iHandlerContext.GetSenCoreServiceManager()->RemoveServiceDescriptionL(*pAuthClient);
        CleanupStack::PopAndDestroy(pAuthClient);
        }

    return retval;
    }    
    
//---------------------------------------------------------------------------
// Logger using during DEBUG mode
//---------------------------------------------------------------------------
//    

TInt CWSOviRegisterHandler::InitL(MSenHandlerContext& aCtx)
    {
    iHandlerContext = aCtx;
    return KErrNone;
    }
    
// END OF FILE

