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




//co-branding CR
#include <e32std.h>
#include "wsstarcobrandinghandler.h"
#include "wsstarcobrandingclient.h"
#include "senpointermap.h"
#include "wspolicy.h"
#include "sendebug.h"
#include "wsstardictionary.h"
#include "MSenServicePolicy.h"
#include "wsstarpolicymanager.h"
#include "senlogger.h"

//---------------------------------------------------------------------------
// Create instance of concrete ECOM interface implementation
//---------------------------------------------------------------------------
//
CWSStarCoBrandingHandler* CWSStarCoBrandingHandler::NewL(TAny* aHandlerCtx)
    {    
    MSenHandlerContext* handlerCtx = reinterpret_cast<MSenHandlerContext*>(aHandlerCtx);
    CWSStarCoBrandingHandler* self   = new (ELeave) CWSStarCoBrandingHandler(*handlerCtx);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
//
CWSStarCoBrandingHandler::CWSStarCoBrandingHandler(MSenHandlerContext& aCtx):CSenSessionHandler(aCtx)
    {
    }

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
//
CWSStarCoBrandingHandler::~CWSStarCoBrandingHandler()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("~CWSStarCoBrandingHandler::~CWSStarCoBrandingHandler()")));
    }

//---------------------------------------------------------------------------
// Second phase construction.
//---------------------------------------------------------------------------
//
void CWSStarCoBrandingHandler::ConstructL()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarCoBrandingHandler::ConstructL()")));
    }

//---------------------------------------------------------------------------
// the most emergent method in message handler interface.
// Each sub-class must implement this method thus providing message processing routine.
//---------------------------------------------------------------------------
//
TInt CWSStarCoBrandingHandler::InvokeL(MSenSessionContext& aCtx)
    {
    TLSLOG_L( KSenCoreServiceManagerLogChannelBase, KMinLogLevel, "CWSStarCoBrandingHandler::InvokeL(MSenMessageContext& aCtx)" );

//getting data from input
    CWSStarServiceSession* pNewSession =
            (CWSStarServiceSession*)aCtx.GetSenRemoteServiceSessionL(
            WSStarContextKeys::KServiceSession());
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
        
    
    CWSStarTrustClient* pSTSClient = STSClientL(pIdentityProvider);
    if (pSTSClient)
            {
 
 
            CSIF* sif = (CSIF*)iHandlerContext.GetAnyL(HandlerContextKey::KSIF());
        
        
            CWSStarServiceSession* wsSTSSession =   
                                            pSTSClient->GetSTSSessionL();                           
 
            CWSStarCoBrandingClient* wsStarCoBrandingClient = 
                                CWSStarCoBrandingClient::NewL(*sif,*Log());
            
            if (wsStarCoBrandingClient)
                    {
                    
                    CleanupStack:: PushL(wsStarCoBrandingClient);
                    
                    TInt error(KErrNone);
                    wsStarCoBrandingClient->SetHandlerCtx(&iHandlerContext);
                    //wsStarCoBrandingClient->SetVersion(iVersion);
                    
                    wsStarCoBrandingClient->SetCoBrandingSessionL(wsSTSSession);

                    //validate session of concrete WebService
                    HBufC8* errorMessage = NULL;
                    TInt leaveCode( KErrNone );
                    TRAP( leaveCode, error = wsStarCoBrandingClient->FetchSCTL( *pNewSession, errorMessage ); )    
#ifdef _SENDEBUG
                    if( leaveCode )
                        {
                        TLSLOG_L( KSenCoreServiceManagerLogChannelBase, KMinLogLevel, "CWSStarCoBrandingHandler::InvokeL(MSenMessageContext& aCtx):" );
                        TLSLOG_L( KSenCoreServiceManagerLogChannelBase, KMinLogLevel, "- wsStarCoBrandingClient->FetchSCTL() leaved!" );
                        }
#endif // _SENDEBUG                    
                    leaveCode = 0; // not used in release builds
                    //((MSenContext&)aCtx).Update(WSStarContextKeys::KReAuthNeeded, EFalse);
                    if (errorMessage)
                        {
                       ((MSenContext&)aCtx).Update(WSStarContextKeys:: KErrMessage,  *errorMessage);
                        }
                    delete errorMessage;
                    errorMessage = NULL;
                    //if credential expired session will use validator to obtain new
                    
                    // Core / XML DAO takes ownership of new session (will keep in array of SD):
                    // - new session HAS
                    //      - credential
                    //      - context which keep for example key for signing message        
                    // - if duplicate (equal primary keys) exists, it is deleted
                    
                    CleanupStack:: PopAndDestroy(wsStarCoBrandingClient);
                     
                    return error;
                    }
               else
                    {
                    return KErrNotFound;
                    }
                     
            }
       else
            {
            return KErrNotFound;
            }
            
                                
     }

//---------------------------------------------------------------------------
// Obtain or construct STS client
//---------------------------------------------------------------------------
//
  
CWSStarTrustClient* CWSStarCoBrandingHandler::STSClientL(CSenIdentityProvider*& aIdentityProvider)
{
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,(_L("CWSStarPlugin::STSClientL")));

    CWSStarTrustClient* pSTSClient = NULL;
    //providerId from Identities.xml
    TPtrC8 providerId(KNullDesC8);
    if(aIdentityProvider)
        {
        providerId.Set(aIdentityProvider->ProviderID());
        }

    RPointerArray<CSenWSDescription> serviceDescriptions;
    CleanupClosePushL(serviceDescriptions);
        
    //try to find STS session in cache,some STS can be registered
    TInt retVal(KErrNone);
    
    TRAPD(error, retVal = iHandlerContext.GetSenCoreServiceManager()->ServiceDescriptionsL(serviceDescriptions, KWSStarSTSContract()));
    if(!retVal&&!error)
        {
        RPointerArray<CSenWSDescription> matches;
        CleanupClosePushL(matches);
        
        for(TInt i = 0; i < serviceDescriptions.Count(); i++)
            {
            //select XMLDescription from DAO where contract = STS, so we can cast
            pSTSClient = (CWSStarTrustClient*)serviceDescriptions[i];
            if(providerId.Length()>0)
                {
                TPtrC8 providerIDFromDescription = pSTSClient->ProviderID();
                if(providerIDFromDescription == providerId)
                    {
                    matches.AppendL(pSTSClient);
                    }
                }
            else
                {
                matches.AppendL(pSTSClient);
                }
            }
        if(matches.Count())
            {
            pSTSClient =
                reinterpret_cast<CWSStarTrustClient*>(matches[0]);
            pSTSClient->SetAccount(aIdentityProvider);
            }
        else
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,(_L("CWSStarValidateHandler::STSClient - No matching STS description available!")));
            pSTSClient = NULL;
            }
    
        //if we find multi sts client (different only with lifetime, probably Secure Context Token lifetime)
        if(matches.Count() > 1)
            {
            // search for a instance with longest validity
            for(TInt i = matches.Count()-1; i > 0 ; i--)
                {
                CWSStarTrustClient* pNewerSTSClient =
                  reinterpret_cast<CWSStarTrustClient*>(matches[i]);
                CWSStarServiceSession* newerSession = (CWSStarServiceSession*)pNewerSTSClient->ServiceSession();
                CWSStarServiceSession* session = (CWSStarServiceSession*)pSTSClient->ServiceSession();
                
				if(newerSession && session) // STSClient Session was not set, if not validated 
					{
					if(newerSession->ValidUntilL() > session->ValidUntilL())
					  	{
					    pSTSClient = pNewerSTSClient;
					    pSTSClient->SetAccount(aIdentityProvider);
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
    
    
    //no session, so we have create new STS client, temporary code 

    //- if (!pSTSClient)
    //-    {
    //-    CSIF* sif = (CSIF*)iHandlerContext.GetAnyL(HandlerContextKey::KSIF());
    //-    pSTSClient = CWSStarTrustClient::NewL(*sif,*Log());
    //-    }
    return pSTSClient;
}


//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//  
SenHandler::THandlerDirection CWSStarCoBrandingHandler::Direction() const
    {
    return SenHandler::EBoth;
    }

//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//    
SenHandler::THandlerPhase CWSStarCoBrandingHandler::Phase()
    {
    return SenHandler::EValidate;
    }
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//   
CSenIdentityProvider* CWSStarCoBrandingHandler::IdentityProviderFromCoreL(
                                            const TDesC8& aProviderID)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"CWSStarCoBrandingHandler::IdentityProviderLFromCore");
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
TInt CWSStarCoBrandingHandler::InitL(MSenHandlerContext& aCtx)
    {
    iHandlerContext = aCtx;
    return KErrNone;
    }

//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//    
RFileLogger* CWSStarCoBrandingHandler::Log() const
    {
    RFileLogger* pLog = NULL;
    TRAP_IGNORE( pLog = (RFileLogger*)iHandlerContext.GetAnyL(HandlerContextKey::KLogger); )
    return pLog;
    }
    
// END OF FILE


