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








#include "wsovivalidatehandler.h"
#include "wsovioauthclient.h"
#include "sendebug.h"
#include "senlogger.h"

class CWSOviHandlerContext;

//---------------------------------------------------------------------------
// Create instance of concrete ECOM interface implementation
//---------------------------------------------------------------------------
//
CWSOviValidateHandler* CWSOviValidateHandler::NewL(TAny* aHandlerCtx)
    {
    
    MSenHandlerContext* handlerCtx =
        reinterpret_cast<MSenHandlerContext*>(aHandlerCtx);
    CWSOviValidateHandler* self   = new (ELeave) CWSOviValidateHandler(*handlerCtx);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
//
CWSOviValidateHandler::CWSOviValidateHandler(MSenHandlerContext& aCtx):CSenSessionHandler(aCtx)
    {
    }

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
//
CWSOviValidateHandler::~CWSOviValidateHandler()
    {
    }

//---------------------------------------------------------------------------
// Second phase construction.
//---------------------------------------------------------------------------
//
void CWSOviValidateHandler::ConstructL()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,(_L("CWSOviValidateHandler::ConstructL()")));
    }
    

//---------------------------------------------------------------------------
// the most emergent method in message handler interface.
// Each sub-class must implement this method thus providing message processing routine.
//---------------------------------------------------------------------------
//
TInt CWSOviValidateHandler::InvokeL(MSenSessionContext& aCtx)
    {

//getting data from input
    CWSOviServiceSession* pNewSession =
            (CWSOviServiceSession*)aCtx.GetSenRemoteServiceSessionL(
            WSOviContextKeys::KServiceSession());
    if (pNewSession->ProviderID() == KNullDesC8)
        {
        return KErrNone;
        }
//try to find identity provider for curennt webService using its ProviderId
    CSenIdentityProvider* pIdentityProvider = IdentityProviderFromCoreL(
            pNewSession->ProviderID());
    if (!pIdentityProvider)
        {
        return KErrNotFound;
        }
    
    const TBool* onlySharingPtr = ((MSenContext&)aCtx).GetIntL(WSOviContextKeys::KOnlySharing);
    TBool onlySharing;
    if (onlySharingPtr)
        {
        onlySharing = *onlySharingPtr;
        }
    else
        {
        onlySharing = ETrue;
        }
    
    CWSOviOAuthClient* pAuthClient = AuthClientL(pIdentityProvider);
    if (pAuthClient)
            {
            TInt error(KErrNone);
            pAuthClient->SetHandlerCtx(&iHandlerContext);
            //validate session of concrete WebService
            HBufC8* errorMessage = NULL;
            TRAPD(lerror, error = pAuthClient->ValidateL(*pNewSession, errorMessage, onlySharing);)    
            ((MSenContext&)aCtx).Update(WSOviContextKeys::KReAuthNeeded, EFalse);
            if (errorMessage)
                {
                ((MSenContext&)aCtx).Update(WSOviContextKeys::KErrMessage,  *errorMessage);
                }
            delete errorMessage;
            errorMessage = NULL;
            if (lerror) return lerror;
            //if credential expired session will use validator to obtain new
            
            // Core / XML DAO takes ownership of new session (will keep in array of SD):
            // - new session HAS
            //      - credential
            // - if duplicate (equal primary keys) exiAuth, it is deleted
            return error;
            }
       else
            {
            return KErrNotFound;
            }
    
    }
    
SenHandler::THandlerDirection CWSOviValidateHandler::Direction() const
    {
        return SenHandler::EBoth;
    };
SenHandler::THandlerPhase CWSOviValidateHandler::Phase()
    {
        return SenHandler::EValidate;
    };
    
//---------------------------------------------------------------------------
// Obtain or construct Auth client
//---------------------------------------------------------------------------
//
  
CWSOviOAuthClient* CWSOviValidateHandler::AuthClientL(CSenIdentityProvider*& aIdentityProvider)
{
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,(_L("CWSOviValidateHandler::AuthClientL")));

    CWSOviOAuthClient* pAuthClient = NULL;
    //providerId from Identities.xml
    TPtrC8 providerId(KNullDesC8);
    if(aIdentityProvider)
        {
        providerId.Set(aIdentityProvider->ProviderID());
        }

    RPointerArray<CSenWSDescription> serviceDescriptions;
    CleanupClosePushL(serviceDescriptions);
        
    //try to find Auth session in cache,some Auth can be registered
    TInt retVal(KErrNone);
    
    TRAPD(error, retVal = iHandlerContext.GetSenCoreServiceManager()->ServiceDescriptionsL(serviceDescriptions, KWSOviAuthenticationServiceContract()));
    if(!retVal&&!error)
        {
        RPointerArray<CSenWSDescription> matches;
        CleanupClosePushL(matches);
        
        for(TInt i = 0; i < serviceDescriptions.Count(); i++)
            {
            //select XMLDescription from DAO where contract = Auth, so we can cast
            if (serviceDescriptions[i]->DescriptionClassType() == MSenServiceDescription::EOviOAuthClient)
                {
                pAuthClient = (CWSOviOAuthClient*)serviceDescriptions[i];
                if(providerId.Length()>0)
                    {
                    TPtrC8 providerIDFromDescription = pAuthClient->ProviderID();
                    if(providerIDFromDescription == providerId)
                        {
                        matches.AppendL(pAuthClient);
                        }
                    }
                else
                    {
                    matches.AppendL(pAuthClient);
                    }                
                }
            }
        if(matches.Count())
            {
            pAuthClient =
                reinterpret_cast<CWSOviOAuthClient*>(matches[0]);
            pAuthClient->SetAccount(aIdentityProvider);
            }
        else
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,(_L("CWSOviValidateHandler::AuthClient - No matching AuthClient  description available!")));
            pAuthClient = NULL;
            }
    
        //if we find multi Auth client (different only with lifetime, probably Secure Context Token lifetime)
        if(matches.Count() > 1)
            {
            // search for a instance with longest validity
            for(TInt i = matches.Count()-1; i > 0 ; i--)
                {
                CWSOviOAuthClient* pNewerAuthClient =
                  reinterpret_cast<CWSOviOAuthClient*>(matches[i]);
                CWSOviServiceSession* newerSession = (CWSOviServiceSession*)pNewerAuthClient->ServiceSession();
                CWSOviServiceSession* session = (CWSOviServiceSession*)pAuthClient->ServiceSession();
                
				if(newerSession && session) // AuthClient Session was not set, if not validated 
					{
					if(newerSession->ValidUntilL() > session->ValidUntilL())
					  	{
					    pAuthClient = pNewerAuthClient;
					    pAuthClient->SetAccount(aIdentityProvider);
					    }
					}
                }
            }
            CleanupStack::PopAndDestroy(2, &serviceDescriptions);
        }
    else
        {
          CleanupStack::PopAndDestroy(&serviceDescriptions);
        }
    return pAuthClient;
}

//---------------------------------------------------------------------------
// Create IdentityProvider from CoreManager
//---------------------------------------------------------------------------
//
CSenIdentityProvider* CWSOviValidateHandler::IdentityProviderFromCoreL(
                                            const TDesC8& aProviderID)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"CWSOviValidateHandler::IdentityProviderLFromCore");
    CSenIdentityProvider* pIdentityProvider = NULL;
    if(aProviderID.Length() > 0)
        {
        CDesC8ArrayFlat* array = new (ELeave) CDesC8ArrayFlat(1);
        CleanupStack::PushL(array);
        array->AppendL(aProviderID);
        pIdentityProvider = iHandlerContext.GetSenCoreServiceManager()->
                IdentityProviderL(*array, ETrue);
        CleanupStack::PopAndDestroy(array);
        }
    return pIdentityProvider;
    }

//---------------------------------------------------------------------------
// Init implementation
//---------------------------------------------------------------------------
//
TInt CWSOviValidateHandler::InitL(MSenHandlerContext& aCtx)
    {
    iHandlerContext = aCtx;
    return KErrNone;
    }
RFileLogger* CWSOviValidateHandler::Log() const
    {
    RFileLogger* pLog = NULL;
    TRAP_IGNORE( pLog = (RFileLogger*)iHandlerContext.GetAnyL(HandlerContextKey::KLogger); )
    return pLog;
    }
MSenHandlerContext& CWSOviValidateHandler::HandlerContext()
	{
	return iHandlerContext;
	}
// END OF FILE

