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




// INCLUDE
#include "wsoviservicesession.h"
#include "sendebug.h"
#include "senlogger.h"
#include "senservicemanagerdefines.h"
#include "SenServiceConnection.h" // err code definitions
#include "senserviceinvocationframework.h"
#include "SenHttpTransportProperties.h"
#include "SenDateUtils.h"
#include "sendebug.h"
#include "senwspattern.h"
#include "SenXmlUtils.h"
#include <xml/dom/xmlengnodelist.h> 
#include <SenHttpTransportProperties.h>
#include <SenIdentityProvider.h>
#include "wsovicredentialobserver.h"
#include "sencryptoutils.h"

const TInt64 KClockSlipSeconds = 60*15;


CWSOviServiceSession* CWSOviServiceSession::NewL(MSIF& aFramework)
    {
    CWSOviServiceSession* self = CWSOviServiceSession::NewLC(aFramework);
    CleanupStack::Pop(self);
    return self;
    }

CWSOviServiceSession* CWSOviServiceSession::NewLC(MSIF& aFramework)
    {
    CWSOviServiceSession* self = new (ELeave) CWSOviServiceSession(
        MSenServiceDescription::EOviServiceSession, aFramework);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CWSOviServiceSession::CWSOviServiceSession(TDescriptionClassType aType,
                                           MSIF& aSIF)
    : CSenWebServiceSession(aType, aSIF)
    {
    }

CWSOviServiceSession::~CWSOviServiceSession()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::~CWSOviServiceSession()");
    delete iSessionContext;
    delete iProviderID;
    delete iTrustAnchor;
    delete iOutContext;
    delete iInContext;
    delete iTransProp;
    delete iOrginalEndpoint;
    
    TInt count(iConsumerList.Count());
    for(TInt i=0; i<count; i++)
        {
        // we can assume that every remote consumer
        // has been wrapped inside WSOviSessionConsumer
        // -wrapperclass. Cast and destroy:
        CWSOviSessionConsumer* pConsumer
            = (CWSOviSessionConsumer*) iConsumerList[i];

        // this destroys the wrapper, but the remote
        // consumer objects ownership remains in either
        // XMLDao or ClientSession (etc)
        delete pConsumer;
        }
    delete iTokenObs;
    }

void CWSOviServiceSession::ConstructL()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::ConstructL()");             
    CSenWebServiceSession::ConstructL();
    iTokenObs = CWSOviCredentialObserver::NewL(this,iFramework.Manager().Log());
    }

TInt CWSOviServiceSession::AddConsumerL(MSenRemoteServiceConsumer& aConsumer)
    {
    TLSLOG_L(KSenClientSessionLogChannelBase+aConsumer.ConnectionId() ,KMinLogLevel ,"CWSOviServiceSession::AddConsumerL");            
    const TInt consumerCount(iConsumerList.Count());
    for(TInt i=0; i<consumerCount; i++)
        {
        if(iConsumerList[i]->Id() == aConsumer.Id())
            {
            return KErrAlreadyExists; // already added, nothing to do
            }
        }

    CWSOviSessionConsumer* pSessionConsumer =
                                    CWSOviSessionConsumer::NewL(aConsumer);

    return iConsumerList.Append(pSessionConsumer);
    }

TBool CWSOviServiceSession::ExistConsumerL()
    {
    const TInt consumerCount(iConsumerList.Count());
    if( consumerCount == 0 )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }

TInt CWSOviServiceSession::RemoveConsumerL(MSenRemoteServiceConsumer& aConsumer)
    {
    TLSLOG_L(KSenClientSessionLogChannelBase+aConsumer.ConnectionId() ,KMinLogLevel ,"CWSOviServiceSession::RemoveConsumerL:");    
    const TInt consumerCount(iConsumerList.Count());
    for(TInt i=0; i<consumerCount; i++)
        {
        if(iConsumerList[i]->Id() == aConsumer.Id())
            {
            CWSOviSessionConsumer* pConsumer
                            = (CWSOviSessionConsumer*) iConsumerList[i];
            delete pConsumer;
            iConsumerList.Remove(i);
            if( (consumerCount-1) == 0 )
                {
                iTokenObs->Cancel();
                }
            break;
            }
        }
    // If this session has no consumers, we can hold our grip to the credential
    // (note: this does NOT mean that credential is removed, not at all(!), but
    //  that new search for the credential has to be performed).
    if( consumerCount == 0 )
        {
        TLSLOG_L(KSenClientSessionLogChannelBase+aConsumer.ConnectionId() ,KNormalLogLevel ,"- Consumer count == 0");
        TLSLOG_L(KSenClientSessionLogChannelBase+aConsumer.ConnectionId() ,KNormalLogLevel ,"-> Closing the handle to the credential owned by the Credential Manager.");
        SetSecurityL(KNullDesC8); 
        }           
    return CSenServiceSession::RemoveConsumerL(aConsumer);
    }

TInt CWSOviServiceSession::MessageForSendingL( CSenHttpTransportProperties*& aTP,
                                                const TDesC8& aSenderID)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::MessageForSendingL");    
    TInt error(KErrNone);
    if (AmIHostletSession())
        {
        return KErrNotSupported;
        }
    else
        {
        CWSOviSessionConsumer* pConsumer = NULL;
        //senderID is internal id of consumer
        error = SessionConsumerL(aSenderID, pConsumer);
        if (error)
            {
            return error; 
            }
        
        CWSOviSessionContext* ssCtx = (CWSOviSessionContext*)SessionContext();
        
        iOutContext->UpdateFromSessionContextL(*ssCtx);
        iOutContext->UpdateTP((CSenTransportProperties*&)aTP);
        AdaptEndpointL(EFalse);
        iOutContext->Update(OAuth::KEndpointKey,Endpoint());
        AdaptEndpointL(ETrue);

        error = ((CWSOviPlugin&)iFramework).ProcessOutboundMessageL(
                iOutContext);
        
        iSessionContext->UpdateFromMessageOutContextL(*iOutContext);
        error = pConsumer->MessageForSendingL(aTP, iOutContext);
        }
    return error;
    }




MSenRemoteServiceConsumer* CWSOviServiceSession::RemoteConsumerL( const TDesC8& aSenderID )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::RemoteConsumerL");        
    TInt consumersCount(iConsumerList.Count());

    for(TInt i=0; i<consumersCount; i++)
        {
        if(iConsumerList[i]->Id() == aSenderID)
            {
            return iConsumerList[i];
            }
        }
    return NULL; // not found
    }

//---------------------------------------------------------------------------
// To obtain consumer during processing INBOUND.
//---------------------------------------------------------------------------
//   

TInt CWSOviServiceSession::SessionConsumerL( CWSOviMessageContext*& /*aMsgCtx*/,
                                              CWSOviSessionConsumer*& aSessionConsumer )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::SessionConsumerL");                
    TInt consumersCount(iConsumerList.Count());
    TInt error(KErrNotFound);
    if (error && consumersCount)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"CWSOviServiceSession::SessionConsumerL - consumer was not found - using the 1st one (as default)");
        aSessionConsumer = (CWSOviSessionConsumer*)iConsumerList[0];
        error = KErrNone;
        }
    return error;
    }

//---------------------------------------------------------------------------
// To obtain consumer during processing OUTBOUND.
//---------------------------------------------------------------------------
//   
        
TInt CWSOviServiceSession::SessionConsumerL( const TDesC8& aSenderID,
                                              CWSOviSessionConsumer*& aSessionConsumer )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::SessionConsumerL");                        
    aSessionConsumer = (CWSOviSessionConsumer*) RemoteConsumerL(aSenderID);
    if(aSessionConsumer)
        {
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }

TBool CWSOviServiceSession::Matches(MSenServiceDescription& aPattern)
    {
    TBool matches = CSenWebServiceSession::Matches(aPattern);
    if (!matches) return EFalse;
    
    MSenElement& xmlPatternAsElement = ((CSenWSDescription&)aPattern).AsElement();
    MSenElement* pElement = xmlPatternAsElement.Element( KSenIdpProviderIdLocalname );
    if(pElement)
        {
        TPtrC8 patternProviderID = pElement->Content();
        TPtrC8 thisProviderID = ProviderID();
        if(patternProviderID.Length()>0)
            {
            if( !(thisProviderID.Length()>0 && patternProviderID == thisProviderID) )
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::Matches() - Provider Id not same returning EFalse");
                return EFalse;
                }
            }
        }
        
    if (aPattern.DescriptionClassType() == MSenServiceDescription::EWSDescription)
        {
        TPtrC8 patternTransportCue = ((CSenWSDescription&)aPattern).TransportCue();
        if (patternTransportCue.Length() > 0)
            {
            if(!iTransportCue || (patternTransportCue != *iTransportCue))
                {
                return EFalse;
                }
            }
        }
    return matches;
    }

TInt CWSOviServiceSession::ScoreMatchL(MSenServiceDescription& aPattern)
    {
    TInt score = CSenWebServiceSession::ScoreMatchL(aPattern);
    
    MSenElement& xmlPatternAsElement = ((CSenWSDescription&)aPattern).AsElement();
    MSenElement* pElement = xmlPatternAsElement.Element(KSenIdpProviderIdLocalname);
    if(pElement)
        {
        TPtrC8 patternProviderID = pElement->Content();
        TPtrC8 thisProviderID = ProviderID();
        if(patternProviderID.Length()>0)
            {
            if ( thisProviderID.Length()>0 && patternProviderID == thisProviderID )
                {
                score++;
                }
            }
        }
        
    if (aPattern.DescriptionClassType() == MSenServiceDescription::EWSDescription)
        {
        TPtrC8 patternTransportCue = ((CSenWSDescription&)aPattern).TransportCue();
        if (patternTransportCue.Length() > 0)
            {
            if ( iTransportCue && (patternTransportCue == *iTransportCue) )
                {
                score++;
                }
            }
        }
        
    return score;
    }

TInt CWSOviServiceSession::SetTransportPropertiesL(const TDesC8& aProperties,
                                             MSenRemoteServiceConsumer& aConsumer)
    {
        if(iTransProp)
            {
            delete iTransProp;
            iTransProp = NULL;
            }
        iTransProp = HBufC8::NewL(aProperties.Length());
        TPtr8 ptr = iTransProp->Des();
        ptr.Append(aProperties);
        CWSOviSessionConsumer* pConsumer = NULL;
        //senderID is internal id of consumer
        TInt error = SessionConsumerL(aConsumer.Id(), pConsumer);
        if (!error && pConsumer)
            {
            MSenTransport& tp = pConsumer->TransportL();
            CSenHttpTransportProperties* transProp = ApplyTransportPropertiesLC(aProperties);
            HBufC8* utfTP = transProp->AsUtf8L();
            CleanupStack::PushL(utfTP);
            tp.SetPropertiesL(*utfTP, MSenLayeredProperties::ESenTransportLayer, &aConsumer);
            CleanupStack::PopAndDestroy(utfTP);
            CleanupStack::PopAndDestroy(transProp);
            }
        return KErrNone;
    }

CSenHttpTransportProperties* CWSOviServiceSession::ApplyTransportPropertiesLC(const TDesC8& aTransportProperties)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::ApplyTransportPropertiesL():");
    TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(aTransportProperties));
    CSenXmlReader* reader = XmlReader();
   TPtrC8 value;
    CSenHttpTransportProperties* httpTransProp = NULL;
    if (aTransportProperties.Length() <= KSenXmlPropertiesLocalname().Length()*2 
                                                                    + KSenLessThanSlash().Length()
                                                                    + KSenLessThan().Length()
                                                                    + KSenGreaterThan().Length()*2)
        {
        if (iTransProp && iTransProp->Length())
            {
            httpTransProp = CSenHttpTransportProperties::NewLC(*iTransProp, *reader);
            }
        else
            {
            httpTransProp = CSenHttpTransportProperties::NewLC();
            }
        }
    else
        {
        httpTransProp = CSenHttpTransportProperties::NewLC(aTransportProperties, *reader);
        if (iTransProp) 
            {
            //1 merge session and message layer transport properties
            CSenHttpTransportProperties* conHttpTransProp = CSenHttpTransportProperties::NewLC(*iTransProp, *reader);
            
            if (httpTransProp->ProxyHostL(value) == KErrNotFound)
                {
                if (conHttpTransProp->ProxyHostL(value) != KErrNotFound)
                    {
                    httpTransProp->SetProxyHostL(value);
                    }
                }
            if (httpTransProp->PropertyL(KNCIMEndpoint, value) == KErrNotFound)
                {
                if (conHttpTransProp->PropertyL(KNCIMEndpoint, value) != KErrNotFound)
                    {
                    httpTransProp->SetPropertyL(KNCIMEndpoint, value);
                    }
                }
            if (httpTransProp->PropertyL(KNCIMConsumerKeyId, value) == KErrNotFound)
                {
                if (conHttpTransProp->PropertyL(KNCIMConsumerKeyId, value) != KErrNotFound)
                    {
                    httpTransProp->SetPropertyL(KNCIMConsumerKeyId, value);
                    }
                }
            if (httpTransProp->PropertyL(KNCIMConsumerSecret, value) == KErrNotFound)
                {
                if (conHttpTransProp->PropertyL(KNCIMConsumerSecret, value) != KErrNotFound)
                    {
                    httpTransProp->SetPropertyL(KNCIMConsumerSecret, value);
                    }
                }
            if (httpTransProp->PropertyL(KNCIMConsumerServiceId, value) == KErrNotFound)
                {
                if (conHttpTransProp->PropertyL(KNCIMConsumerServiceId, value) != KErrNotFound)
                    {
                    httpTransProp->SetPropertyL(KNCIMConsumerServiceId, value);
                    }
                }
            
            TBool boolValue;
            if (httpTransProp->OnewayMessageOnOffL(boolValue) == KErrNotFound)
                {
                if (conHttpTransProp->OnewayMessageOnOffL(boolValue) != KErrNotFound)
                    {
                    httpTransProp->SetOnewayMessageOnOffL(boolValue);
                    }
                }
            if (httpTransProp->BoolPropertyL(KSenIAPDoNotPrompt, boolValue) == KErrNotFound)
                {
                if (conHttpTransProp->BoolPropertyL(KSenIAPDoNotPrompt, boolValue) != KErrNotFound)
                    {
                    httpTransProp->SetBoolPropertyL(KSenIAPDoNotPrompt, boolValue);
                    }
                }
            if (httpTransProp->UserAgentL(value) == KErrNotFound)
                {
                if (conHttpTransProp->UserAgentL(value) != KErrNotFound)
                    {
                    httpTransProp->SetUserAgentL(value);
                    }
                }
            TUint32 uint32value;
            if (httpTransProp->IapIdL(uint32value) == KErrNotFound)
                {
                if (conHttpTransProp->IapIdL(uint32value) != KErrNotFound)
                    {
                    httpTransProp->SetIapIdL(uint32value);
                    httpTransProp->SetSnapIdL(0);
                    }
                }
            if (httpTransProp->SnapIdL(uint32value) == KErrNotFound)
                {
                if (conHttpTransProp->SnapIdL(uint32value) != KErrNotFound)
                    {
                    httpTransProp->SetSnapIdL(uint32value);
                    httpTransProp->SetIapIdL(0);
                    }
                }
            
            TInt intValue;
            if (httpTransProp->MaxTimeToLiveL(intValue) == KErrNotFound)
                {
                if (conHttpTransProp->MaxTimeToLiveL(intValue) != KErrNotFound)
                    {
                    httpTransProp->SetMaxTimeToLiveL(intValue);
                    }
                }

            CleanupStack::PopAndDestroy(conHttpTransProp);    
            }
        }
    
    return httpTransProp;
    }

TInt CWSOviServiceSession::SendL( const TDesC8& aMessage, 
                                   const TDesC8& aTransportProperties,
                                   MSenRemoteServiceConsumer& aConsumer,
                                   TInt& aTxnId,
                                   HBufC8*& aRevalidationError )
    {
    TLSLOG_L(KSenClientSessionLogChannelBase+aConsumer.ConnectionId() ,KMinLogLevel ,"CWSOviServiceSession::SendL(TDesC8&, ...)");
    TInt retVal(KErrNone);
    if ( AmIHostletSession() )
        {
        TLSLOG_L(KSenClientSessionLogChannelBase+aConsumer.ConnectionId() ,KMinLogLevel ,"- This is a session for a hostlet.");
        retVal  = CSenWebServiceSession::SendL( aMessage, aTransportProperties, aConsumer, aTxnId, aRevalidationError );
        TLSLOG_FORMAT((KSenClientSessionLogChannelBase+aConsumer.ConnectionId(), KNormalLogLevel , _L8("- Return value from CSenWebServiceSession::SendL: %d"), retVal ));
        TLSLOG_FORMAT((KSenClientSessionLogChannelBase+aConsumer.ConnectionId(), KNormalLogLevel , _L8("- Transaction ID from transport: %d"), aTxnId ));
        }
    else
        {
        TLSLOG_L(KSenClientSessionLogChannelBase+aConsumer.ConnectionId() ,KMinLogLevel ,"- This is a session for a consumer.");
        PrepareOutCtxL(aMessage);
        VerifyPermissionL();
        //we still have to check if validUntil is not expired.
        retVal = RevalidateMobileTicketIfExpiredL( aRevalidationError );
        iSessionContext->Update(WSOviContextKeys::KOnlySharing, ETrue);

        if( retVal == KErrNone )
            {
            delete aRevalidationError; 
            aRevalidationError = NULL;
            CSenHttpTransportProperties* transProp = ApplyTransportPropertiesLC(aTransportProperties);
            retVal = MessageForSendingL(transProp, aConsumer.Id());
            if (aMessage.Length())
                {
                transProp->SetContentTypeL(OAuth::KFormUrlEncoded());
                }
            
            HBufC8* utfTP = transProp->AsUtf8L();
            CleanupStack::PushL(utfTP);
            MSenTransport& transport = aConsumer.TransportL();
            AdaptEndpointL(EFalse);
            const TDesC8* message = iOutContext->GetDesC8L(WSOviContextKeys::KMessageBody);
            if (message)
                {
                retVal = transport.SendL(Endpoint(), *message, *utfTP, *this, aConsumer, aTxnId );
                }
            else
                {
                retVal = transport.SendL(Endpoint(), aMessage, *utfTP, *this, aConsumer, aTxnId );
                }
            AdaptEndpointL(ETrue);
            CleanupStack::PopAndDestroy(utfTP);    
            CleanupStack::PopAndDestroy(transProp);    
            DestroyOutCtx();
            }
        }
    return retVal;
    }


TInt CWSOviServiceSession::SubmitL( const TDesC8& aMessage, 
                                     const TDesC8& aTransportProperties,
                                     MSenRemoteServiceConsumer& aConsumer,
                                     HBufC8*& aResponse )
    {
    TLSLOG_L(KSenClientSessionLogChannelBase+aConsumer.ConnectionId() ,KMinLogLevel ,"CWSOviServiceSession::SubmitL(TDesC8&, ...)");
    TInt retVal(KErrNone);
    if (!AmIHostletSession())
        {
        PrepareOutCtxL(aMessage);
        CSenHttpTransportProperties* transProp = ApplyTransportPropertiesLC(aTransportProperties);
        retVal = MessageForSendingL(transProp, aConsumer.Id());
        if (aMessage.Length())
            {
            transProp->SetContentTypeL(OAuth::KFormUrlEncoded());
            }

        
        HBufC8* utfTP = transProp->AsUtf8L();
        CleanupStack::PushL(utfTP);
        MSenTransport& transport = aConsumer.TransportL();
        AdaptEndpointL(EFalse);

        const TDesC8* message = iOutContext->GetDesC8L(WSOviContextKeys::KMessageBody);
        if (message)
            {
	        TLSLOG(KSenClientSessionLogChannelBase+aConsumer.ConnectionId(), KMinLogLevel, _L8("CWSOviServiceSession::SubmitL() - Request"));
	        TLSLOG(KSenClientSessionLogChannelBase+aConsumer.ConnectionId(), KMinLogLevel, _L8("********************************************"));
	        TLSLOG_ALL(KSenClientSessionLogChannelBase+aConsumer.ConnectionId()  , KMaxLogLevel , *message);
	        TLSLOG(KSenClientSessionLogChannelBase+aConsumer.ConnectionId(), KMinLogLevel, _L8("********************************************"));
                    
            retVal = transport.SubmitL(Endpoint(), *message, *utfTP, aResponse, aConsumer);
            }
        else
            {
	        TLSLOG(KSenClientSessionLogChannelBase+aConsumer.ConnectionId(), KMinLogLevel, _L8("CWSOviServiceSession::SubmitL() - Request"));
	        TLSLOG(KSenClientSessionLogChannelBase+aConsumer.ConnectionId(), KMinLogLevel, _L8("********************************************"));
	        TLSLOG_ALL(KSenClientSessionLogChannelBase+aConsumer.ConnectionId()  , KMaxLogLevel , aMessage);
	        TLSLOG(KSenClientSessionLogChannelBase+aConsumer.ConnectionId(), KMinLogLevel, _L8("********************************************"));            	
            	
            retVal = transport.SubmitL(Endpoint(), aMessage, *utfTP, aResponse, aConsumer);
            }
               
        TLSLOG(KSenClientSessionLogChannelBase+aConsumer.ConnectionId(), KMinLogLevel, _L8("CWSOviServiceSession::SubmitL() - Response"));
        TLSLOG(KSenClientSessionLogChannelBase+aConsumer.ConnectionId(), KMinLogLevel, _L8("********************************************"));
        TLSLOG_ALL(KSenClientSessionLogChannelBase+aConsumer.ConnectionId()  , KMaxLogLevel , *aResponse);
        TLSLOG(KSenClientSessionLogChannelBase+aConsumer.ConnectionId(), KMinLogLevel, _L8("********************************************"));
        
        MSenProperties& prop = transport.PropertiesL();
        ((CWSOviPlugin&)iFramework).ProcessInboundDispatchL(this, retVal, aResponse, &prop );
        
        AdaptEndpointL(ETrue);
        CleanupStack::PopAndDestroy(utfTP);
        CleanupStack::PopAndDestroy(transProp);
        DestroyOutCtx();

        if (retVal == KErrNone)
            {
            iSubmitState = WSOviSession::KSubmitStateOK;
            }

    //becouse of lack phase (dispatch to proprr consumer). 
    //It is SUBMIT nature, only used internally between Client and NAUTH
    //we have to make validation here
        CWSOviSessionConsumer* consumer = NULL;
        if (iInContext)
            {
            SessionConsumerL(iInContext, consumer);    
            if (!(consumer && consumer->Id() == aConsumer.Id()))
                {
                retVal = KErrNotFound;
                }
            }
        }
    return retVal;
    }

void CWSOviServiceSession::AdaptEndpointL(TBool aToOrginal)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase,KMinLogLevel ,"CWSOviServiceSession::AdaptEndpointL");	
    TPtrC8 oldEndpoint = Endpoint();
    if (aToOrginal)
        {
        SetEndPointL(*iOrginalEndpoint);
        return;
        }
    
    delete iOrginalEndpoint;
    iOrginalEndpoint = NULL;
    iOrginalEndpoint = oldEndpoint.AllocL();
    
    HBufC8* newEp(NULL);
/*    CSenIdentityProvider* pIdentityProvider = IdentityProviderFromCoreL(ProviderID());
    if (pIdentityProvider)
        {
        TPtrC8 advAuth = pIdentityProvider->AdvisoryAuthnID();
        
        newEp = HBufC8::NewLC(oldEndpoint.Length()
                                + advAuth.Length());
        TPtr8 ptr = newEp->Des();
        _LIT8(KMask, "%S");
        TInt pos = oldEndpoint.Find(KMask());
        if (pos != KErrNotFound)
            {
            ptr.Append(oldEndpoint.Left(pos));
            ptr.Append(advAuth);
            ptr.Append(oldEndpoint.Right(oldEndpoint.Length() - pos - KMask().Length()));
            SetEndPointL(ptr);
            }
        
        CleanupStack::PopAndDestroy(newEp);    
        }
*/  
    CSenTransportProperties* tp = iOutContext->TP();
    if (tp)
        {
        TInt err(KErrNone);
        TPtrC8 suffixEndpoint;
        err = tp->EndpointResourceL(suffixEndpoint);
        if (!err)
            {
            newEp = HBufC8::NewLC(suffixEndpoint.Length()
                                  +oldEndpoint.Length());
            TPtr8 ptrNewEp = newEp->Des();
            ptrNewEp.Append(oldEndpoint);
            TInt retVal = oldEndpoint.Find(suffixEndpoint);
            if(retVal == KErrNotFound)
            	{
            	ptrNewEp.Append(suffixEndpoint);
            	}
            SetEndPointL(*newEp);
            CleanupStack::PopAndDestroy(newEp);
            }
        }
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase,KMinLogLevel ,"CWSOviServiceSession::AdaptEndpointL Completed");
    }
CSenIdentityProvider* CWSOviServiceSession::IdentityProviderFromCoreL(
                                            const TDesC8& aProviderID)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::IdentityProviderLFromCore");
    CSenIdentityProvider* pIdentityProvider = NULL;
    if(aProviderID.Length() > 0)
        {
        CDesC8ArrayFlat* array = new (ELeave) CDesC8ArrayFlat(1);
        CleanupStack::PushL(array);
        array->AppendL(aProviderID);
        pIdentityProvider = ((CWSOviPlugin&)iFramework).Manager().IdentityProviderL(*array, ETrue);
        CleanupStack::PopAndDestroy(array);
        }
    return pIdentityProvider;
    }
TInt CWSOviServiceSession::CanHandleErrorL()
    {
    TInt answer(KErrNone);
    const TInt* renewNeeded(NULL);
    const TInt* retry = iSessionContext->GetIntL(WSOviContextKeys::KRetryNeeded());

    
    
    
    if (iSessionContext)
        {
        renewNeeded = iSessionContext->GetIntL(WSOviContextKeys::KReAuthNeeded());
        }
    //retry in milisecunds
    if (retry && *retry > 0 &&
        iRetryCounter < WSOviSession::KCounterMax)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::CanHandleErrorL - RETRY request (new timestamp needed)");
        iRetryCounter++;
        answer = KErrSenResendRequired;  
        }
    else if((renewNeeded && *renewNeeded &&
           iRenewCounter < WSOviSession::KCounterMax)
           )
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::CanHandleErrorL - RENEW request (new ticket is needed)");
        iRenewCounter++;
        answer = KErrSenReinitRequired;                    
        }
    else
        {
        iRenewCounter = 0;
        iRetryCounter = 0;
        }
    return answer;
    }
TInt CWSOviServiceSession::SendToConsumerL(HBufC8* aMessage,
                              const TInt aTxnId,
                              MSenRemoteServiceConsumer& aConsumer,
                              MSenProperties* aResponseTransportProperties)
    {
    TLSLOG_L(KSenClientSessionLogChannelBase+aConsumer.ConnectionId()  , KMinLogLevel,"CWSOviServiceSession::SendToConsumerL()");
    ((CWSOviPlugin&)iFramework).ProcessInboundDispatchL(this, KErrNone, aMessage, aResponseTransportProperties);
    iRenewCounter = 0;
    iRetryCounter = 0;
    TInt retVal = aConsumer.HandleMessageL( aMessage, aTxnId, aResponseTransportProperties );
    return retVal;
    }
    
TInt CWSOviServiceSession::SendErrorToConsumerL( const TInt aErrorCode,
                                                     HBufC8* apError,
                                                     const TInt aTxnId,
                                                     MSenRemoteServiceConsumer& aConsumer,
                                                     MSenProperties* aResponseTransportProperties )
    {
    TLSLOG_L(KSenClientSessionLogChannelBase+aConsumer.ConnectionId()  , KMinLogLevel,"CWSOviServiceSession::SendErrorToConsumerL");
    TLSLOG_FORMAT((KSenClientSessionLogChannelBase+aConsumer.ConnectionId(), KMinLogLevel , _L8("- ErrorCode: %d"), aErrorCode ));
    ((CWSOviPlugin&)iFramework).ProcessInboundDispatchL(this, aErrorCode, apError, aResponseTransportProperties);
    TInt answer = CanHandleErrorL();
    if (answer)
        {
        delete apError;
        return aConsumer.HandleErrorL(NULL, answer, aTxnId, aResponseTransportProperties);
        }
    else
        {
        return HandleErrorL(aErrorCode, apError, aTxnId, aConsumer, aResponseTransportProperties);
        }
    }
    
    
    
TInt CWSOviServiceSession::SubmitSoapL( const TDesC8& /*aSoapMessage*/, 
                                         const TDesC8& /*aTransportProperties*/,
                                         MSenRemoteServiceConsumer& /*aConsumer*/,
                                         HBufC8*& /*aResponse*/ )
    {
    return KErrNotSupported;
    }

TInt CWSOviServiceSession::SendSoapL( const TDesC8& /*aSoapMessage*/, 
                                       const TDesC8& /*aTransportProperties*/,
                                       MSenRemoteServiceConsumer& /*aConsumer*/,
                                       TInt& /*aTxnId*/,
                                       HBufC8*& /*aRevalidationError*/ )
    {
    return KErrNotSupported;
    }

  
TBool CWSOviServiceSession::IsExpiredL()
    {
    CSenServiceSession::SetStatusL();
    return (CSenServiceSession::StatusL() == KSenConnectionStatusExpired);
    }
    
        
void CWSOviServiceSession::SetSessionContext(CWSOviSessionContext* aContext)
    {
    delete iSessionContext;
    iSessionContext = aContext;
    }
CWSOviSessionContext* CWSOviServiceSession::SessionContext()
    {
    return iSessionContext;
    }
    
void CWSOviServiceSession::ClearCredentialL()
    {
    // Before removing the credential, checkt that
    // iCredentialPtr.Credential() != NULL
    if ( HasSecurity() ) // this performs the NULL check for credential
    	{
	    // Remove Invalid Credential from Credentials DB
	    TInt credentialId = iCredentialPtr.Credential()->IdentifierL().IdL();
	    ((MSenServiceManager&)iFramework.Manager()).RemoveCredentialL(credentialId);
    	}

    // Remove Credential usage from Session
    SetSecurityL(KNullDesC8);
    iValidUntil.Set(_L("18000101:"));
    }
    
TInt CWSOviServiceSession::InitializeFromL( MSenServiceDescription& aDescription)
    {
    CSenWebServiceSession::SetSecurityL(KNullDesC8);
    iValidUntil.Set(_L("19000101:"));
    
    TDescriptionClassType classType = aDescription.DescriptionClassType();
    if( classType == MSenServiceDescription::EWSDescription
                                            ||
        classType == MSenServiceDescription::EWSPattern
                                            ||
        classType == MSenServiceDescription::EIdentityProvider
        )
        {
        MSenElement& xmlSdAsElement = (
            (CSenWSDescription*)&aDescription)->AsElement();


        MSenElement* pElement = xmlSdAsElement.Element(KSenIdpProviderIdLocalname);
        delete iProviderID;
        iProviderID = NULL;
        if(pElement)
            {
            iProviderID = pElement->Content().AllocL();
            }
        else
            {
            CSenIdentityProvider* provider = ((CWSOviPlugin&)iFramework).
                    Manager().IdentityProviderL(aDescription);
            if (provider)
                {
                iProviderID = provider->ProviderID().AllocL();
                }
            }
        pElement = xmlSdAsElement.Element(WSOviSession::KTrustAnchorElementLocalName);
        if(pElement)
            {
            delete iTrustAnchor;
            iTrustAnchor = NULL;
            iTrustAnchor = pElement->Content().AllocL();
            }
        
       // CSenXmlReader* reader = XmlReader();
        if (!iSessionContext) 
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::InitializeFromL");
            iSessionContext = CWSOviSessionContext::NewL();
            
            //calling higher class load such things like iClientServerInterval
            CSenWebServiceSession::InitializeFromL(aDescription);
            if ( StatusL() != KSenConnectionStatusReady )
                {
                // Try to search Credential directly from CredentialManager
                // if Connection/Credential is not ready
                if ( TryToSearchValidCredentialL() == KErrNone )	//codescannerwarnings
                    {
                    SetStatusL();
                    }
                }
            if (!iCredentialPtr.Credential())
                {
                pElement = xmlSdAsElement.Element(WSOviSession::KTokenCreationTimeLocalName);
                if(pElement)
                    {
                    User::LeaveIfError(iSessionContext->Add(WSOviContextKeys::KTokenCreationTime, pElement->Content()));    
                    }
                pElement = xmlSdAsElement.Element(WSOviSession::KTokenValidUntilTimeLocalName);
                if(pElement)
                    {
                    User::LeaveIfError(iSessionContext->Add(WSOviContextKeys::KTokenValidUntilTime, pElement->Content()));    
                    }                    
                pElement = xmlSdAsElement.Element(WSOviSession::KTTLLocalName);
                if(pElement)
                    {
                    User::LeaveIfError(iSessionContext->Add(WSOviContextKeys::KTTL, pElement->Content()));    
                    }
                pElement = xmlSdAsElement.Element(WSOviSession::KTokenSecretLocalName);
                if(pElement)
                    {
                    User::LeaveIfError(iSessionContext->Add(WSOviContextKeys::KTokenSecret, pElement->Content()));    
                    }
                }
            AddSecurityTokenToContextL();
            }
        }
    else
        {
        CSenWebServiceSession::InitializeFromL(aDescription);
        }
        
    return KErrNone;
    }
    
void CWSOviServiceSession::AddSecurityTokenToContextL()
    {
    if ( HasSecurity() )
        {
        CSenCredentialProperties& properties = iCredentialPtr.Credential()->PropertiesL();  //codescannerwarnings
        TInt retVal;
        TPtrC8 value;
        if ( iSessionContext )
            {
            retVal = properties.PropertyL(WSOviSession::KTokenCreationTimeLocalName, value);
            if ( retVal == KErrNone )
                {
                User::LeaveIfError(iSessionContext->Add(WSOviContextKeys::KTokenCreationTime, value));    
                }
            retVal = properties.PropertyL(WSOviSession::KTokenValidUntilTimeLocalName, value);
            if ( retVal == KErrNone )
                {
                User::LeaveIfError(iSessionContext->Add(WSOviContextKeys::KTokenValidUntilTime, value));    
                }               
            retVal = properties.PropertyL(WSOviSession::KTTLLocalName, value);
            if ( retVal == KErrNone )
                {
                User::LeaveIfError(iSessionContext->Add(WSOviContextKeys::KTTL, value));    
                }
            retVal = properties.PropertyL(WSOviSession::KTokenSecretLocalName, value);
            if ( retVal == KErrNone )
                {
                User::LeaveIfError(iSessionContext->Add(WSOviContextKeys::KTokenSecret, value));    
                }
        
	        HBufC8* pSecurity = SecurityL();
	        if (pSecurity)
	            {
	            CleanupStack::PushL(pSecurity);
	            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::InitializeFromL - adding token to ctx");

	            User::LeaveIfError(iSessionContext->Add(
	                    WSOviContextKeys::KToken,
	                    *pSecurity));  
	            CleanupStack::PopAndDestroy(pSecurity);
	            } 
			}		
        }
    }

void CWSOviServiceSession::AddPropertiesFromSessionContextToCredentialL()
    {
    if (iSessionContext)
        {
        if ( iCredentialPtr.Credential() )
            {
            CSenCredentialProperties& properties = iCredentialPtr.Credential()->PropertiesL();  //codescannerwarnings
            const TDesC8* pValue = iSessionContext->GetDesC8L(WSOviContextKeys::KTokenCreationTime);
            if ( pValue )
                {
                properties.SetPropertyL(WSOviSession::KTokenCreationTimeLocalName(), *pValue);
                }
            pValue = iSessionContext->GetDesC8L(WSOviContextKeys::KTokenValidUntilTime);
            if ( pValue )
                {
                properties.SetPropertyL(WSOviSession::KTokenValidUntilTimeLocalName(), *pValue);
                }            
            pValue = iSessionContext->GetDesC8L(WSOviContextKeys::KTTL);
            if ( pValue )
                {
                properties.SetPropertyL(WSOviSession::KTTLLocalName(), *pValue);
                }        
            pValue = iSessionContext->GetDesC8L(WSOviContextKeys::KTokenSecret);
            if ( pValue )
                {
                properties.SetPropertyL(WSOviSession::KTokenSecretLocalName(), *pValue);
                }        

            // Now, serialize this session (and all it's updated credentials) into database(s)
            iFramework.Manager().SaveL( *this ); // this will currently *also* serialize associated credential (new/updated MT)                
            }
        }
    }


void CWSOviServiceSession::WriteExtensionsAsXMLToL(RWriteStream& aWriteStream)
    {
    CSenWebServiceSession::WriteExtensionsAsXMLToL(aWriteStream);

    if(iProviderID)
        {
        aWriteStream.WriteL(WSOviSession::KProviderIDTag);
        aWriteStream.WriteL(*iProviderID);
        aWriteStream.WriteL(WSOviSession::KProviderIDEndTag);
        }

    if(iTrustAnchor)
        {
        aWriteStream.WriteL(WSOviSession::KTrustAnchorTag);
        aWriteStream.WriteL(*iTrustAnchor);
        aWriteStream.WriteL(WSOviSession::KTrustAnchorEndTag);
        }
    if (iSessionContext)
    	{
        if (!iCredentialPtr.Credential() )
            {
            const TDesC8* value = iSessionContext->GetDesC8L(WSOviContextKeys::KTokenCreationTime);
            if (value)
                {
                aWriteStream.WriteL(WSOviSession::KTokenCreationTimeTag);
                aWriteStream.WriteL(*value);
                aWriteStream.WriteL(WSOviSession::KTokenCreationTimeEndTag);
                }
            value = iSessionContext->GetDesC8L(WSOviContextKeys::KTokenValidUntilTime);
            if (value)
                {
                aWriteStream.WriteL(WSOviSession::KTokenValidUntilTimeTag);
                aWriteStream.WriteL(*value);
                aWriteStream.WriteL(WSOviSession::KTokenValidUntilTimeEndTag);
                }                
            value = iSessionContext->GetDesC8L(WSOviContextKeys::KTTL);
            if (value)
                {
                aWriteStream.WriteL(WSOviSession::KTTLTag);
                aWriteStream.WriteL(*value);
                aWriteStream.WriteL(WSOviSession::KTTLEndTag);
                }
            value = iSessionContext->GetDesC8L(WSOviContextKeys::KTokenSecret);
            if (value)
                {
                aWriteStream.WriteL(WSOviSession::KTokenSecretTag);
                aWriteStream.WriteL(*value);
                aWriteStream.WriteL(WSOviSession::KTokenSecretEndTag);
                }
            }
    	}
    }
    
TPtrC8 CWSOviServiceSession::ProviderID()
    {
    if(iProviderID)
        return *iProviderID;
    else
        return KNullDesC8();
   
    }

void CWSOviServiceSession::SetProviderIDL(const TDesC8& aProviderId)
    {
    if (iProviderID)
        {
        delete iProviderID;
        iProviderID = NULL;
        }
    iProviderID = aProviderId.AllocL();
    }


void CWSOviServiceSession::SetTrustAnchorL(const TDesC8& aURI)
    {
    delete iTrustAnchor;
    iTrustAnchor = NULL;
    iTrustAnchor = aURI.AllocL();
    }
TPtrC8 CWSOviServiceSession::TrustAnchor()
    {
    if(iTrustAnchor)
        return *iTrustAnchor;
    else
        return KNullDesC8();
    }

TInt CWSOviServiceSession::ComputeStatusL()
    {
    
    TInt retVal = CSenServiceSession::ComputeStatusL();
    //we copy logic from CSenWebServiceSession and add :1) IProviderID condition
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CWSOviServiceSession::ComputeStatusL() - retval: %d"), retVal));
    if ((retVal == KSenConnectionStatusReady) && iProviderID)
        {
        const TDesC8*  tokenCreatedTimeDes = NULL;
        const TDesC8*  ttlDes = NULL;
        TPtrC8 tokenCreationTimePtr;
        TPtrC8 ttlPtr;
        
        if ( HasSecurity() )
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::ComputeStatusL() - HasSecurity true");
            
            CSenCredentialProperties& properties = iCredentialPtr.Credential()->PropertiesL();  //codescannerwarnings
            TInt retVal(KErrNone);
            retVal = properties.PropertyL(WSOviSession::KTokenCreationTimeLocalName(), tokenCreationTimePtr);
            if (!retVal)
                {
                tokenCreatedTimeDes = &tokenCreationTimePtr;    
                }
            retVal = properties.PropertyL(WSOviSession::KTTLLocalName(), ttlPtr);
            if (!retVal)
                {
                ttlDes = &ttlPtr;
                }
            }
        else
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::ComputeStatusL() - HasSecurity false");
            if (iSessionContext)
                {
                tokenCreatedTimeDes = iSessionContext->GetDesC8L(WSOviContextKeys::KTokenCreationTime);
                ttlDes = iSessionContext->GetDesC8L(WSOviContextKeys::KTTL);
                }
            }
        if (tokenCreatedTimeDes && ttlDes)
	        {
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CWSOviServiceSession::ComputeStatusL() - ttlDes: %S & tokencreationDes: %S"), ttlDes, tokenCreatedTimeDes));
			TInt64 tokenCreatedTime(0);
			TInt64 ttl(0);
			TLex8 temp;
			  
			temp= *tokenCreatedTimeDes;
			temp.Val(tokenCreatedTime);
			  
			temp = *ttlDes;
			temp.Val(ttl);
               
			if (tokenCreatedTime && ttl) 
				{
				HBufC8* nowTimeDes = SenCryptoUtils::GetTimestampL();
				TInt64 nowTime(0);
				temp = *nowTimeDes;
				temp.Val(nowTime);
				delete nowTimeDes;
               	
				if (nowTime) 
					{
					TTimeIntervalSeconds margin(KClockSlipSeconds);
					TTimeIntervalSeconds diff = tokenCreatedTime + ttl -margin.Int() - nowTime;
	                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CWSOviServiceSession::ComputeStatusL() - diff: %d "), diff.Int()));
					if (diff.Int()<0) 
						{
						retVal = KSenConnectionStatusExpired;
						}
					}
               }
	        }
        else
            {
            retVal = KSenConnectionStatusExpired;
            }
        }
    return retVal;

    } 
    
void CWSOviServiceSession::PrepareOutCtxL(const TDesC8& aMessageBody)
    {
    iClientTime.UniversalTime();
    
    //CSenXmlReader* reader = XmlReader();
    if (iOutContext)
        {
        delete iOutContext;
        iOutContext = NULL;
        }
    iOutContext = CWSOviMessageContext::NewL(SenContext::EOutgoing);
    if (aMessageBody != KNullDesC8)
        {
        iOutContext->Update(WSOviContextKeys::KMessageBody, aMessageBody);
        }
    }

void CWSOviServiceSession::DestroyOutCtx()
    {
    delete iOutContext;
    iOutContext = NULL;
    }

TInt CWSOviServiceSession::RevalidateMobileTicketIfExpiredL( HBufC8*& aErrorMessage ) 
    {
    TInt result(KErrNone);
    if (IsExpiredL())
        {
        //we need revalidate
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviServiceSession::RevalidateMobileTicketIfExpiredL - revalidation is required.")));
        iTokenObs->Cancel();
        result = ((CWSOviPlugin&)iFramework).ProcessOutboundValidationL( *this, this, aErrorMessage );
        if ( result == KErrNone )
            {
            // validation succeeded => serialize this session (and its credentials immediately)
            iFramework.Manager().SaveL( *this ); // this will currently *also* serialize associated credential (new/updated MT)
            }
        if (result > KErrNone)
            {
            result = KErrSenAuthenticationFault;
            }
        return result;
        }
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviServiceSession::RevalidateMobileTicketIfExpiredL - ticket is still valid.")));        
    return KErrNone;
    }
    
TBool CWSOviServiceSession::HasSuperClass( TDescriptionClassType aType )
    {
    if( aType == MSenServiceDescription::EWebServiceSession ) // direct superclass!
        {
        // If asked type is the know *direct* father/mother, return true:
        return ETrue; 
        } 
    else
        {
        // Otherwise, ask from superclass (chain, recursively)
        return CSenWebServiceSession::HasSuperClass( aType ); 
        }
    }

TInt CWSOviServiceSession::ShareTokenWithL( CWSOviServiceSession* aWSOviSessionDst)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::ShareTokenWithL()");
    TInt error(KErrNone);
    if ( !HasSecurity() )
        {
        return KErrNotFound;
        }
    //share token (only not expired), otherway it is sensless
    //dont call SetStatus/IsExpired becouse it will infornm consumer/Core about chnge. 
    //Session will not be able to revalidate by itself in next sending
    if (ComputeStatusL() != KSenConnectionStatusExpired)
        {

        error = iSessionContext->ShareTokenWithL(
                aWSOviSessionDst->SessionContext());        
                
        aWSOviSessionDst->AddCredentialL(iCredentialPtr, iValidUntil);
        return error;
        }
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::ShareTokenWithL() returning KErrNotFound");
        return KErrNotFound;
        }
    }
    
void CWSOviServiceSession::AddCredentialL( const TDesC8& aSecurity, TTime aValidUntil )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::AddCredentialL(aSecurity, aValidUntil)");
    iValidUntil = aValidUntil;
    SetSecurityL(aSecurity);
    SetStatusL();
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::AddCredentialL(aSecurity, aValidUntil) Completed");
    }
    
void CWSOviServiceSession::AddCredentialL(RSenCredentialPtr aCredentialPtr, TTime aValidUntil)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::AddCredentialL(aCredentialPtr, aValidUntil)");
    iValidUntil = aValidUntil;
    SetCredentialPtrL(aCredentialPtr); // Share same Credential between multiple Sessions	//codescannerwarnings
    SetStatusL();
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::AddCredentialL(aCredentialPtr, aValidUntil) Completed");
    }
    
TBool CWSOviServiceSession::AmIHostletSession()
    {
    if(iTransportCue && (KSenTransportCueHostletConnection() == *iTransportCue))
        {
        return ETrue;
        }
    return EFalse;
    }
    

void CWSOviServiceSession::FillCredentialIdentifierL(CSenCredentialIdentifier& aIdentifier)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::FillCredentialIdentifierL:");
    CSenWebServiceSession::FillCredentialIdentifierL(aIdentifier);
    //_LIT8(KProviderIdElementLocalName,  "ProviderID");
    //aIdentifier.SetPropertyL(KProviderIdElementLocalName, ProviderID());

    TPtrC8 providerId = ProviderID();
    aIdentifier.SetPropertyL(KSenIdpProviderIdLocalname, providerId);
    
    // Add information about the account that is priviledged to consume this credential
    
    // (a) find matching IDP description (which represents the account / userinfo)
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- About to call LookupIdentityProviderL");
    CSenIdentityProvider* pIdentityProvider = LookupIdentityProviderL();
//    CSenIdentityProvider* pIdentityProvider = NULL;
//    if ( providerId != KNullDesC8 )
//        {
//        LOG_WRITEFORMAT((_L8("- Searching for IdentityProvider using ProviderId : %S"), &providerId));
//        CSenWSDescription* pIdpPattern = CSenWSDescription::NewLC();
//        pIdpPattern->SetEndPointL(providerId);
//        pIdentityProvider = iFramework.Manager().IdentityProviderL(*pIdpPattern);
//        CleanupStack::PopAndDestroy(pIdpPattern);
//        }

    if ( pIdentityProvider )
        {
        TPtrC8 username = pIdentityProvider->UserName();
        if( username.Length() > 0 )
            {
            aIdentifier.SetPropertyL(KSenIdpAuthzIDLocalname, username);
            }
        TPtrC8 password = pIdentityProvider->Password();
        if( password.Length() > 0 )
            {
            aIdentifier.SetPropertyL(KSenIdpPasswordLocalname, password);
            }
        }
    }
    
void CWSOviServiceSession::VerifyPermissionL()
    {
    if( HasSecurity() )
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::VerifyPermissionL:");
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- Making call to LookupIdentityProviderL");
        CSenIdentityProvider* pProvider = LookupIdentityProviderL();
        if ( pProvider )
            {
            CSenCredentialIdentifier& identifier = iCredentialPtr.Credential()->IdentifierL();
            TPtrC8 username; // that was used to acquire this credential
            TInt retVal = identifier.PropertyL(KSenIdpAuthzIDLocalname, username);
            
            TPtrC8 password; // that was used to acquire this credential
            retVal = identifier.PropertyL(KSenIdpPasswordLocalname, password);
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("=> Credential properties: username = '%S', password = '%S'"), &username, &password));

            TPtrC8 accountUsername = pProvider->UserName();
            TPtrC8 accountPassword = pProvider->Password();
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("=> Candidate IDP details: username = '%S', password = '%S'"), &accountUsername, &accountPassword));
            
            if( ( username.Length() > 0 && username != accountUsername ) ||
                ( password.Length() > 0 && password != accountPassword ) )
                {
                // either username or password DOES NOT MATCH (and the mismatching
                // one is not zero-length string)
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"-> Will not grant permission to consume current credential,");

                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"   clearing credential & it's properties from context.");
                SetSecurityL(KNullDesC8);
                ClearCredentialPropertiesFromContext();
                }
             else 
                { 
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"--> Permission to consume current credential is granted.");
                // => Permission to consume this credential is granted(!) 
                }
            }
        else
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- There is no IDP (account), clearing credential & it's properties from context.");
            // It is not possible to consumer WS-* service(s) without IDP (account)
            SetSecurityL(KNullDesC8);
            ClearCredentialPropertiesFromContext();
            }
        }
    else
        {
        // If session has no credential (neither MT or SCT), neither should it have any 
        // related data in context(!)
        //--> LOG_WRITE_L("- There is no credential, clearing credential properties from context.");
        //--> SetSecurityL(KNullDesC8);
        ClearCredentialPropertiesFromContext();
        }        
    }
    
CSenIdentityProvider* CWSOviServiceSession::LookupIdentityProviderL()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::LookupIdpL()");
    TPtrC8 providerId = ProviderID();
    CSenIdentityProvider* pIdentityProvider = NULL;
    if ( providerId != KNullDesC8 )
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("- Searching for IdentityProvider using ProviderId : %S"), &providerId));
        CSenWSDescription* pIdpPattern = CSenWSDescription::NewLC();
        pIdpPattern->SetEndPointL(providerId);
        pIdentityProvider = iFramework.Manager().IdentityProviderL(*pIdpPattern);
        CleanupStack::PopAndDestroy(pIdpPattern);
        }
    return pIdentityProvider;
    }
     
    
  
TBool CWSOviServiceSession::HasEqualPrimaryKeysL( MSenServiceDescription& aCandidate )
    {
    TBool retVal(EFalse);
    if ( CSenWSDescription::HasEqualPrimaryKeysL(aCandidate) )
        {
        // Endpoint, Contract and FrameworkID match, all of them.
        // Now, in WS-Ovi, also check ProviderID
        if (aCandidate.DescriptionClassType() == DescriptionClassType())
            {
            CWSOviServiceSession& sessionCandidate = (CWSOviServiceSession&)aCandidate;
            if (sessionCandidate.ProviderID() == ProviderID())
                {
                retVal = ETrue;
                }
            }
        }
    return retVal; 
    }
    
void CWSOviServiceSession::CredentialChanged( TSenCredentialChange aChange,
		                                       TAny* apPointer )
    {
    if ( aChange == MSenCredentialObserver::EDestroyed && iSessionContext )
        {
        ClearCredentialPropertiesFromContext();
        }
    else if ( aChange == MSenCredentialObserver::EAdded && iSessionContext)
    	{
    	AddSecurityTokenToContextL();
    	}
    CSenWebServiceSession::CredentialChanged( aChange, apPointer );
    }


void CWSOviServiceSession::ClearCredentialPropertiesFromContext()
    {
    if( iSessionContext )
        {
        iSessionContext->Remove(WSOviContextKeys::KToken);    
        iSessionContext->Remove(WSOviContextKeys::KTokenCreationTime);
        iSessionContext->Remove(WSOviContextKeys::KTokenValidUntilTime);
        iSessionContext->Remove(WSOviContextKeys::KTTL);
        iSessionContext->Remove(WSOviContextKeys::KTokenSecret);
        }
    }
void CWSOviServiceSession::SetMaxValidUntil()
    {
    iValidUntil = Time::MaxTTime();
    }

void CWSOviServiceSession::ActiveTicketObserverL()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::ActiveTicketObserverL()");
    if (iSessionContext)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::ActiveTicketObserverL()- SessionContext exists");
        const TDesC8*  tokenCreatedTimeDes = iSessionContext->GetDesC8L(WSOviContextKeys::KTokenCreationTime);
        const TDesC8*  ttlDes = iSessionContext->GetDesC8L(WSOviContextKeys::KTTL);
        if (tokenCreatedTimeDes && ttlDes)
        	{
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::ActiveTicketObserverL() - ttl & time creation exists");
            TInt64 tokenCreatedTime(0);
            TInt64 ttl(0);
            TLex8 temp;
            temp.Assign(*tokenCreatedTimeDes);
            temp.Val(tokenCreatedTime);
            
            temp = *ttlDes;
            temp.Val(ttl);
            
            if (tokenCreatedTime && ttl)
            	{
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::ActiveTicketObserverL() - ttl & time creation is valid");
            	HBufC8* nowTimeDes = SenCryptoUtils::GetTimestampL();
            	TInt64 nowTime(0);
            	temp = *nowTimeDes;
            	temp.Val(nowTime);
            	delete nowTimeDes;
            	if (nowTime)
            		{
            	    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::ActiveTicketObserverL() - now time exists");
            	    TInt marginForComputeStatus(10);
                	TTimeIntervalSeconds margin(KClockSlipSeconds-marginForComputeStatus);
                    TTimeIntervalSeconds diff = tokenCreatedTime + ttl -margin.Int() - nowTime;
            		if (diff.Int()>=0)
            			{
            		    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviServiceSession::ActiveTicketObserverL() - start observer");
            			iTokenObs->Start(diff);
            			}
            		}
            	}
        	}
        }
    }


// End of file

