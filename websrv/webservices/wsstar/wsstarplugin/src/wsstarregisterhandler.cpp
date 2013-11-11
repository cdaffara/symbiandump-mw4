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








#include "wsstarregisterhandler.h"
#include "wsstartrustclient.h"
#include "sendebug.h"
#include "senlogger.h"


class CWSStarHandlerContext;
// Create instance of concrete ECOM interface implementation
CWSStarRegisterHandler* CWSStarRegisterHandler::NewL(TAny* aHandlerCtx)
    {
    
    MSenHandlerContext* handlerCtx =
        reinterpret_cast<MSenHandlerContext*>(aHandlerCtx);
    CWSStarRegisterHandler* self   = new (ELeave) CWSStarRegisterHandler(*handlerCtx);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Constructor
CWSStarRegisterHandler::CWSStarRegisterHandler(MSenHandlerContext& aCtx):CSenSessionHandler(aCtx)
    {
    
    }

// Destructor
CWSStarRegisterHandler::~CWSStarRegisterHandler()
    {
    }

// Second phase construction.
void CWSStarRegisterHandler::ConstructL()
    {
    }
    
TInt CWSStarRegisterHandler::InvokeL(MSenSessionContext& aCtx)
    {
    MSenServiceDescription& pServiceDescription = *(MSenServiceDescription*)aCtx.GetSenWSDescriptionL(WSStarContextKeys::KServiceDescription());
    const TDesC8* action = aCtx.GetDesC8L(WSStarContextKeys::KRegisterAction());
    TInt result(KErrNone);
    if (*action == WSStarContextValues::KActionRegister())
        {
        result = RegisterServiceDescriptionL(pServiceDescription);
        }
    else if (*action == WSStarContextValues::KActionUnregister())
        {
        result = UnregisterServiceDescriptionL(pServiceDescription);
        };
    return result;  
    }
    
SenHandler::THandlerDirection CWSStarRegisterHandler::Direction() const
    {
        return SenHandler::EBoth;
    };
SenHandler::THandlerPhase CWSStarRegisterHandler::Phase()
    {
        return SenHandler::EDiscovery;
    };

//---------------------------------------------------------------------------
// Attempt to register the ServiceDescription to the ServiceManager 
//---------------------------------------------------------------------------
//
TInt CWSStarRegisterHandler::RegisterServiceDescriptionL(MSenServiceDescription& aServiceDescription )
    {
    TInt retval(KErrNone);
    CWSStarServiceSession* pSession = NULL;
    TPtrC8 contract = aServiceDescription.Contract();
    TPtrC8 endpoint = aServiceDescription.Endpoint();

    if (contract == KNullDesC8) 
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarRegisterHandler::RegisterServiceDescriptionL failed - KErrSenNoContract");
        retval = KErrSenNoContract;
        }
    else if(endpoint == KNullDesC8)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarRegisterHandler::RegisterServiceDescriptionL failed - KErrSenNoEndpoint");
        retval = KErrSenNoEndpoint;
        }
    else
        {
        if(contract == KWSStarSTSContract)
            {
            retval = RegisterSTSClientL(&aServiceDescription);
            }
        else if (aServiceDescription.DescriptionClassType() ==
                        MSenServiceDescription::EWSStarServiceSession)
            {
            //  description already is a session so we just add to core-DAO
            //  example:    when connection has been made, and service is registered 
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarRegisterHandler::RegisterServiceDescriptionL - session is already so just adding")));
                pSession = (CWSStarServiceSession*) &aServiceDescription;
                retval = iHandlerContext.GetSenCoreServiceManager()->AddServiceDescriptionL(pSession);
            }
        else
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarRegisterHandler::RegisterServiceDescriptionL - Creating session from description...")));
            pSession = CWSStarServiceSession::NewLC(*(CSIF*)iHandlerContext.GetAnyL(HandlerContextKey::KSIF()));	//codescannerwarnings
            //example:  when description has been read from db during bootup
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
TInt CWSStarRegisterHandler::UnregisterServiceDescriptionL(
    MSenServiceDescription& aServiceDescription)
    {
    TInt retval(KErrNone);
    CWSStarServiceSession *pSession = NULL;
    TPtrC8 contract = aServiceDescription.Contract();

    if(contract == KWSStarSTSContract)
        {
        retval = UnRegisterSTSClientL(&aServiceDescription);
        }
    else
        {
        if(aServiceDescription.DescriptionClassType() ==
                MSenServiceDescription::EWSStarServiceSession)
            {
            pSession = (CWSStarServiceSession*) &aServiceDescription;
            retval = iHandlerContext.GetSenCoreServiceManager()->RemoveServiceDescriptionL(*pSession);
            }
        else
            {

            pSession = CWSStarServiceSession::NewLC(*(CSIF*)iHandlerContext.GetAnyL(HandlerContextKey::KSIF()));	//codescannerwarnings
            retval = pSession->InitializeFromL(aServiceDescription);
            if (retval == KErrNone)
                {
                retval = iHandlerContext.GetSenCoreServiceManager()->RemoveServiceDescriptionL(*pSession);
                }
            CleanupStack::PopAndDestroy(pSession);
            }
        }

    return retval;
    }    

//---------------------------------------------------------------------------
// Register specific description (STS contract)
//---------------------------------------------------------------------------
//    
TInt CWSStarRegisterHandler::RegisterSTSClientL(
                                MSenServiceDescription *aServiceDescription)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarRegisterHandler::RegisterSTSClientL");
    
    TPtrC8 endpoint = aServiceDescription->Endpoint();
    TInt retval = KErrNone;
    CWSStarServiceSession* pSession = NULL;
    CWSStarTrustClient* pSTSClient = NULL;

    if(aServiceDescription->DescriptionClassType() ==
                MSenServiceDescription::EWSStarSTSClient)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarRegisterHandler::RegisterSTSClient - This ServiceDescription is already an STS Client")));
        pSTSClient = (CWSStarTrustClient*)aServiceDescription;
        retval = iHandlerContext.GetSenCoreServiceManager()->AddServiceDescriptionL(pSTSClient);
        }
    else
        {
        // Create new STS client and initialize
        // it from given description
        pSession = CWSStarServiceSession::NewLC(*(CSIF*)iHandlerContext.GetAnyL(HandlerContextKey::KSIF()));	//codescannerwarnings
        retval = pSession->InitializeFromL(*aServiceDescription);

        if(retval != KErrNone)
            {
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CWSStarRegisterHandler::RegisterSTSClientL failed %d"), retval));
            CleanupStack::PopAndDestroy(pSession);
            return retval;
            }
        TPtrC8 sessionEndpoint = pSession->Endpoint();
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CWSStarRegisterHandler::RegisterSTSClientL created STS using endpoint:'%S'"), &sessionEndpoint));

        pSTSClient = CWSStarTrustClient::NewLC(*(CSIF*)iHandlerContext.GetAnyL(HandlerContextKey::KSIF()), *Log());	//codescannerwarnings
        pSTSClient->SetSTSSessionL(pSession);    // pSession will be owned by STSClient
        CleanupStack::Pop(pSTSClient);
        CleanupStack::Pop(pSession);
        CleanupStack::PushL(pSTSClient);

        retval = iHandlerContext.GetSenCoreServiceManager()->AddServiceDescriptionL(pSTSClient);
        if(retval != KErrNone)
            {
            CleanupStack::PopAndDestroy(1);//pSTSClient
            }
        else
        CleanupStack::Pop(pSTSClient); 
        pSTSClient = NULL;
        }
    return retval;
    }

//---------------------------------------------------------------------------
// Unregister specific description (STS contract)
//---------------------------------------------------------------------------
//    

TInt CWSStarRegisterHandler::UnRegisterSTSClientL(
                                MSenServiceDescription *aServiceDescription)
    {
    TInt retval = KErrNone;

    CWSStarServiceSession *pSession = NULL;
    CWSStarTrustClient *pSTSClient = NULL;

    if(aServiceDescription->DescriptionClassType() ==
                MSenServiceDescription::EWSStarSTSClient)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarRegisterHandler::RegisterSTSClient - This ServiceDescription is already an STS Client")));
        pSTSClient = (CWSStarTrustClient*)aServiceDescription;
        retval = iHandlerContext.GetSenCoreServiceManager()->RemoveServiceDescriptionL(*pSTSClient);
        }
    else
        {
        
        pSession = CWSStarServiceSession::NewLC(*(CSIF*)iHandlerContext.GetAnyL(HandlerContextKey::KSIF()));	//codescannerwarnings
        retval = pSession->InitializeFromL(*aServiceDescription);
        if(retval != KErrNone)
            {
            CleanupStack::PopAndDestroy(pSession);
            return retval;
            }
        pSTSClient = CWSStarTrustClient::NewLC(*(CSIF*)iHandlerContext.GetAnyL(HandlerContextKey::KSIF()), *Log());	//codescannerwarnings
        pSTSClient->SetSTSSessionL(pSession);    // pSession will be owned by STSClient
        CleanupStack::Pop(pSTSClient);
        CleanupStack::Pop(pSession);
        CleanupStack::PushL(pSTSClient);

        retval = iHandlerContext.GetSenCoreServiceManager()->RemoveServiceDescriptionL(*pSTSClient);
        CleanupStack::PopAndDestroy(pSTSClient);
        }

    return retval;
    }    
    
//---------------------------------------------------------------------------
// Logger using during DEBUG mode
//---------------------------------------------------------------------------
//    
RFileLogger* CWSStarRegisterHandler::Log() const
    {
    RFileLogger* pLog = NULL;
    TRAP_IGNORE( pLog = (RFileLogger*)iHandlerContext.GetAnyL(HandlerContextKey::KLogger); )
    return pLog;
    }

TInt CWSStarRegisterHandler::InitL(MSenHandlerContext& aCtx)
    {
    iHandlerContext = aCtx;
    return KErrNone;
    }
    
// END OF FILE

