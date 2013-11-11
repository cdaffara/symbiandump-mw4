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








#include "wsstarvalidatehandler.h"
#include "wsstartrustclient.h"
#include "sendebug.h"
#include "senlogger.h"

class CWSStarHandlerContext;

//---------------------------------------------------------------------------
// Create instance of concrete ECOM interface implementation
//---------------------------------------------------------------------------
//
CWSStarValidateHandler* CWSStarValidateHandler::NewL(TAny* aHandlerCtx)
    {
    
    MSenHandlerContext* handlerCtx =
        reinterpret_cast<MSenHandlerContext*>(aHandlerCtx);
    CWSStarValidateHandler* self   = new (ELeave) CWSStarValidateHandler(*handlerCtx);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
//
CWSStarValidateHandler::CWSStarValidateHandler(MSenHandlerContext& aCtx):CSenSessionHandler(aCtx)
    {
    }

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
//
CWSStarValidateHandler::~CWSStarValidateHandler()
    {
    }

//---------------------------------------------------------------------------
// Second phase construction.
//---------------------------------------------------------------------------
//
void CWSStarValidateHandler::ConstructL()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,(_L("CWSStarValidateHandler::ConstructL()")));
    const TDesC8* version = iHandlerContext.GetDesC8L(HandlerContextKey::KVersion);
    if (version && 
        (*version == WSTrust::KTrustNamespace200502 || *version == WSTrust::KTrustNamespace200404))
        {
        iVersion = version;
        }
    else
        {
        iVersion = &WSTrust::KTrustNamespace200502; //passport interface 2
        //&WSTrust::KTrustNamespace200404;
        }
    }
    

//---------------------------------------------------------------------------
// the most emergent method in message handler interface.
// Each sub-class must implement this method thus providing message processing routine.
//---------------------------------------------------------------------------
//
TInt CWSStarValidateHandler::InvokeL(MSenSessionContext& aCtx)
    {

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
    
    //in session context we have info
    const TBool* renewingPtr = ((MSenContext&)aCtx).GetIntL(WSStarContextKeys::KReAuthNeeded);
    TBool renewing;
    if (renewingPtr)
        {
        renewing = *renewingPtr;
        }
    else
        {
        renewing = EFalse;
        }
    const TBool* onlySharingPtr = ((MSenContext&)aCtx).GetIntL(WSStarContextKeys::KOnlySharing);
    TBool onlySharing;
    if (onlySharingPtr)
        {
        onlySharing = *onlySharingPtr;
        }
    else
        {
        onlySharing = ETrue;
        }
    
    CWSStarTrustClient* pSTSClient = STSClientL(pIdentityProvider);
    if (pSTSClient)
            {
			TInt lerror(KErrNone);
            TInt error(KErrNone);
            pSTSClient->SetHandlerCtx(&iHandlerContext);
            pSTSClient->SetVersion(iVersion);
            //validate session of concrete WebService
            HBufC8* errorMessage = NULL;
             // fix for compiler warning #550-D reverted back
            TRAP(lerror,error = pSTSClient->ValidateL(*pNewSession, renewing, errorMessage, onlySharing);)    
            lerror = 0; // not used in release builds
            ((MSenContext&)aCtx).Update(WSStarContextKeys::KReAuthNeeded, EFalse);
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
            return error;
            }
       else
            {
            return KErrNotFound;
            }
    
    }
    
SenHandler::THandlerDirection CWSStarValidateHandler::Direction() const
    {
        return SenHandler::EBoth;
    };
SenHandler::THandlerPhase CWSStarValidateHandler::Phase()
    {
        return SenHandler::EValidate;
    };
    
//---------------------------------------------------------------------------
// Obtain or construct STS client
//---------------------------------------------------------------------------
//
  
CWSStarTrustClient* CWSStarValidateHandler::STSClientL(CSenIdentityProvider*& aIdentityProvider)
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
    // architects says:- Yes... You must have AS registered into DAO (DB) sessions db
    /*if (!pSTSClient)
        {
        CSIF* sif = (CSIF*)iHandlerContext.GetAny(HandlerContextKey::KSIF());
        pSTSClient = CWSStarTrustClient::NewL(*sif,*Log());
        }*/
    return pSTSClient;
}

//---------------------------------------------------------------------------
// Create IdentityProvider from CoreManager
//---------------------------------------------------------------------------
//
CSenIdentityProvider* CWSStarValidateHandler::IdentityProviderFromCoreL(
                                            const TDesC8& aProviderID)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"CWSStarValidateHandler::IdentityProviderLFromCore");
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
TInt CWSStarValidateHandler::InitL(MSenHandlerContext& aCtx)
    {
    iHandlerContext = aCtx;
    return KErrNone;
    }
RFileLogger* CWSStarValidateHandler::Log() const
    {
    RFileLogger* pLog = NULL;
    TRAP_IGNORE( pLog = (RFileLogger*)iHandlerContext.GetAnyL(HandlerContextKey::KLogger); )
    return pLog;
    }
    
// END OF FILE

