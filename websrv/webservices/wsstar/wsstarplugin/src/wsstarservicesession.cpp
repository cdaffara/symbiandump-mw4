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



// INCLUDE
#include "wsstarservicesession.h"
#include "sendebug.h"
#include "senlogger.h"
#include "SenSoapFault.h"
#include "senservicemanagerdefines.h"
#include "SenServiceConnection.h" // err code definitions
#include "senserviceinvocationframework.h"
#include "SenHttpTransportProperties.h"
#include "senvtcptransportproperties.h"
#include "SenDateUtils.h"
#include "wsstarpolicy.h"
#include "sendebug.h"
#include "sencryptoutils.h"
#include "senwspattern.h"
#include "SenXmlUtils.h"
#include <xml/dom/xmlengnodelist.h> 

#include <SenIdentityProvider.h>
#include "wsstarcredentialobserver.h"
using namespace WSPolicy;

/* Constant margin. Value is copied from framework internal class.
 * We check if MT is expired (now < validUntil - skew time - margin)
 * Margin is 15 minutes.
 */
const TInt64 KClockSlipMinutes = 15;


CWSStarServiceSession* CWSStarServiceSession::NewL(MSIF& aFramework)
    {
    CWSStarServiceSession* self = CWSStarServiceSession::NewLC(aFramework);
    CleanupStack::Pop(self);
    return self;
    }

CWSStarServiceSession* CWSStarServiceSession::NewLC(MSIF& aFramework)
    {
    CWSStarServiceSession* self = new (ELeave) CWSStarServiceSession(
        MSenServiceDescription::EWSStarServiceSession, aFramework);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CWSStarServiceSession::CWSStarServiceSession(TDescriptionClassType aType,
                                           MSIF& aSIF)
    : CSenWebServiceSession(aType, aSIF),
        iMessageThread(EFalse),
        iSubmitState(WSStarSession::KSubmitStateOK)
    {
    }

CWSStarServiceSession::~CWSStarServiceSession()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::~CWSStarServiceSession");            
    delete iSessionContext;
    delete iProviderID;
    delete iTrustAnchor;
    delete iOutContext;
    delete iInContext;
    delete ipReceivedMessageIdInTrans;
    delete iTransProp;
    
    TInt count(iConsumerList.Count());
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CWSStarServiceSession::~CWSStarServiceSession leaved ConsumerL now=  %d"), count));
    for(TInt i=0; i<count; i++)
        {
        // we can assume that every remote consumer
        // has been wrapped inside WSStarSessionConsumer
        // -wrapperclass. Cast and destroy:
        CWSStarSessionConsumer* pConsumer
            = (CWSStarSessionConsumer*) iConsumerList[i];

        // this destroys the wrapper, but the remote
        // consumer objects ownership remains in either
        // XMLDao or ClientSession (etc)
        delete pConsumer;
        }
    delete iTicketObs;
    }

void CWSStarServiceSession::ConstructL()
    {
    CSenWebServiceSession::ConstructL();    
    iTicketObs = CWSStarCredentialObserver::NewL(this, iFramework.Manager().Log());
    }

TInt CWSStarServiceSession::AddConsumerL(MSenRemoteServiceConsumer& aConsumer)
    {
    CSLOG_L(aConsumer.ConnectionId() ,KMinLogLevel ,"CWSStarServiceSession::AddConsumerL");            
    
    const TInt consumerCount(iConsumerList.Count());
    for(TInt i=0; i<consumerCount; i++)
        {
        if(iConsumerList[i]->Id() == aConsumer.Id())
            {
            return KErrAlreadyExists; // already added, nothing to do
            }
        }

    CWSStarSessionConsumer* pSessionConsumer =
                                    CWSStarSessionConsumer::NewL(aConsumer, *Log());

    TInt error = iConsumerList.Append(pSessionConsumer);
    CWSStarPlugin* fmw = (CWSStarPlugin*)&iFramework;
    fmw->PolicyConsumerAddedL(this,*pSessionConsumer);
    return KErrNone;
    }

/**
 * DetachCredentialsL
 *
 * This function  invalidates the session by deleting the credential
 * This also call setStatusL to recompute the status
*/
void CWSStarServiceSession::DetachCredentialsL()
	{
		ClearCredentialL() ;
		SetStatusL() ;
	}
TBool CWSStarServiceSession::ExistConsumerL()
    {
    const TInt consumerCount(iConsumerList.Count());
    if( consumerCount == 0 )
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"   - Consumer count = 0");
        return EFalse;
        }
    else
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("- Consumer count =  %d"), consumerCount));
        return ETrue;
        }
    }

TInt CWSStarServiceSession::RemoveConsumerL(MSenRemoteServiceConsumer& aConsumer)
    {
    CSLOG_L(aConsumer.ConnectionId() ,KMinLogLevel ,"CWSStarServiceSession::RemoveConsumerL:");    
    
    const TInt consumerCount(iConsumerList.Count());
    for(TInt i=0; i<consumerCount; i++)
        {
        if(iConsumerList[i]->Id() == aConsumer.Id())
            {
            CWSStarSessionConsumer* pConsumer
                            = (CWSStarSessionConsumer*) iConsumerList[i];
            delete pConsumer;
            iConsumerList.Remove(i);
            CSLOG_FORMAT((aConsumer.ConnectionId() , KMinLogLevel , _L8("CWSStarServiceSession::Removed ConsumerL now=  %d"), consumerCount-1));
            
            if( (consumerCount-1) == 0 )
                {
                iTicketObs->Cancel();
                }
            break;
            }
        }
    // If this session has no consumers, we can hold our grip to the credential
    // (note: this does NOT mean that credential is removed, not at all(!), but
    //  that new search for the credential has to be performed).
    if( consumerCount == 0 )
        {
        CSLOG_L(aConsumer.ConnectionId() ,KNormalLogLevel ,"- Consumer count == 0");
        
        CSLOG_L(aConsumer.ConnectionId() ,KNormalLogLevel ,"-> Closing the handle to the credential owned by the Credential Manager.");
        
        SetSecurityL(KNullDesC8); // this removes the reference to credential, but
                                  // it can be regained on next lookup
                                  // This needs to implemet. so that userinfo (password)
                                  // that grants the permission to use the credential
                                  // will be checked on that (credential sharing) lookup.
        }           
                
    return CSenServiceSession::RemoveConsumerL(aConsumer);
    }

TInt CWSStarServiceSession::MessageForSendingL( const TDesC8& aBody,
                                                const TDesC8& aSenderID,
                                                CSenSoapMessage*& aMessage)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::MessageForSendingL");    
    TInt error(KErrNone);
    if (AmIHostletSession())
        {
        return CSenWebServiceSession::MessageForSendingL(aBody, aSenderID, aMessage);
        }
    else
        {
        CleanupStack::PushL(aMessage);
        CWSStarSessionConsumer* pConsumer = NULL;
        //senderID is internal id of consumer
        error = SessionConsumerL(aSenderID, pConsumer);
        if (error)
            {
            CleanupStack::Pop(aMessage);
            return error; 
            }
        
        CWSStarSessionContext* ssCtx = (CWSStarSessionContext*)SessionContext();
        
        //----adding addressing info
        //messageId - set by handler
        User::LeaveIfError(iOutContext->Add(WSStarContextKeys::KTo, Contract()));
        if(ipReceivedMessageIdInTrans)
            {
            User::LeaveIfError(iOutContext->Add(WSStarContextKeys::KRelatesTo,
                    *ipReceivedMessageIdInTrans));
            }
        else
            {
            User::LeaveIfError(iOutContext->Add(WSStarContextKeys::KRelatesTo, NULL));
            }
        
        iOutContext->SetOwnedEnvelope();
        iOutContext->UpdateFromSessionContextL(*ssCtx);
        CleanupStack::Pop(aMessage);
        error = ((CWSStarPlugin&)iFramework).ProcessOutboundMessageL(
                iOutContext, &aBody, aMessage);
        
        iSessionContext->UpdateFromMessageOutContextL(*iOutContext);
        const TDesC8* resolvedBody = iSessionContext->GetDesC8L(WSStarContextKeys::KBody);
       
        if (resolvedBody && (!error))
            {
            error = pConsumer->MessageForSendingL(*resolvedBody , iOutContext);
            }
            
        aMessage = iOutContext->GetCurrentSoapMessage();
        iOutContext->SetOwnedEnvelope(EFalse);
        delete iOutContext;
        iOutContext = NULL;
        }
    return error;
    }

TInt CWSStarServiceSession::NewMessageL(CSenSoapMessage*& aMessage)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::NewMessageL( CSenSoapMessage*& )");                    
    //method used during ibound processing
    const TInt* soapVer = iSessionContext->GetIntL(WSStarContextKeys::KSoapVersion);
    
    if (soapVer)
        {
        aMessage = CSenSoapMessage::NewL((TSOAPVersion)*soapVer);    
        }
    else
        {
        aMessage = CSenSoapMessage::NewL(ESOAP12);    
        }
    CSenXmlReader* reader = XmlReader();
    if (iInContext)
        {
        delete iInContext;
        iInContext = NULL;
        }
    iInContext = CWSStarMessageContext::NewL(SenContext::EIncoming, reader);
    CWSStarSessionContext* ssCtx = (CWSStarSessionContext*)SessionContext();
    iInContext->UpdateFromSessionContextL(*ssCtx);
        
    return KErrNone;
    }

TInt CWSStarServiceSession::ParseMessageL(CSenSoapMessage& aSOAPMessage)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::ParseMessageL(CSenSoapMessage& )");

//-----SCHarF ------- 7.5.3.3 Inbound messageflow    
    
    //CSenXmlReader* reader = XmlReader();
    CSenSoapMessage* soapMessage = &aSOAPMessage;
    ((CWSStarPlugin&)iFramework).ProcessInboundDispatchL(this, soapMessage);
    ((CWSStarPlugin&)iFramework).ProcessInboundMessageL(iInContext, soapMessage);
    
    iServerTime.UniversalTime();

    //keep transaction chain
    if (iMessageThread)
        {
        const TDesC8* msgId = iInContext->GetDesC8L(WSStarContextKeys::KMessageID());
        if (msgId)
            {
            delete ipReceivedMessageIdInTrans;
            ipReceivedMessageIdInTrans = NULL;
            ipReceivedMessageIdInTrans = msgId->AllocL();
            }
        else
            {
            //transaction mode is enabled, but no messageId in response so what to do.
            //anyway we will continue without signalizing error
            }
        }
    
    iSessionContext->UpdateFromMessageInContextL(*iInContext);
    
    return CSenWebServiceSession::ParseMessageL(aSOAPMessage);

    }

TInt CWSStarServiceSession::SendSoapMessageToConsumerL( CSenSoapMessage* apMessage,
                                                        const TInt aTxnId,
                                                        MSenRemoteServiceConsumer& aConsumer,
                                                        MSenProperties* aResponseTransportProperties )
                                            
    {
    //-----SCHarF ------- glue with dispather for handlers 7.5.3.2 inbound
    CSLOG_L(aConsumer.ConnectionId() ,KMinLogLevel ,"CWSStarServiceSession::SendSoapMessageToConsumerL");
    

    if ( apMessage )
        {
        CleanupStack::PushL( apMessage );
        //dispatch to proper consumer
        CWSStarSessionConsumer* pConsumer = NULL;
        SessionConsumerL( iInContext, pConsumer );
        if (!pConsumer) 
            {
            CleanupStack::PopAndDestroy( apMessage ); // de-alloc immediately! Change this, if WS-stack one day gives back faults to Service Connections
            // No related consumer, signalize fault error 
            aConsumer.HandleErrorL( NULL, KErrSenBrokenSoapEnvelope, aTxnId, aResponseTransportProperties );
            }
        else
            {
            // handle message destroys this
            TBool completeServerMessages;
            HasFacetL(KCompleteMessagesFacet, completeServerMessages);
            CleanupStack::Pop( apMessage ); // REQUIREMENT for next method => it MUST take ownership IMMEDIATELY
            pConsumer->HandleSoapMessageL( apMessage, aTxnId, aResponseTransportProperties, completeServerMessages );
            }
        iRetryCounter = 0;     //reset counter for retryAfter faults. If proper message incomes, we can reset counter
        iRenewCounter = 0;     //
        iRedirectCounter =0 ;
        delete iInContext;
        iInContext = NULL;        
        return KErrNone;
        }
    else
        {
        CSLOG_L(aConsumer.ConnectionId() ,KMinLogLevel ,"CWSStarServiceSession::SendSoapMessageToConsumerL - apMessage == NULL => invoking remote consumer's HandleErrorL with KErrSenBrokenSoapEnvelope");
        
        /*TInt retVal = */ aConsumer.HandleErrorL( NULL, KErrSenBrokenSoapEnvelope, aTxnId, aResponseTransportProperties );
        return KErrSenInternal;
        }
    }



MSenRemoteServiceConsumer* CWSStarServiceSession::RemoteConsumerL( const TDesC8& aSenderID )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::RemoteConsumerL");        
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

TInt CWSStarServiceSession::SessionConsumerL( CWSStarMessageContext*& aMsgCtx,
                                              CWSStarSessionConsumer*& aSessionConsumer )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::SessionConsumerL");                
    TInt consumersCount(iConsumerList.Count());
    TInt error(KErrNotFound);
    const TDesC8* relatesTo = aMsgCtx->GetDesC8L(WSStarContextKeys::KRelatesTo);
    const TDesC8* endpoint = aMsgCtx->GetDesC8L(WSStarContextKeys::KTo);
    for(TInt i=0; i<consumersCount; i++)
        {
        CWSStarSessionConsumer* pConsumer = (CWSStarSessionConsumer*)iConsumerList[i];
        if ((relatesTo) && (pConsumer->Expects(*relatesTo, endpoint)))
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::SessionConsumerL - Found consumer");
            aSessionConsumer = pConsumer;
            error = KErrNone;
            break;
            }
        }
    if (error && consumersCount)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"CWSStarServiceSession::SessionConsumerL - consumer was not found - using the 1st one (as default)");
        aSessionConsumer = (CWSStarSessionConsumer*)iConsumerList[0];
        error = KErrNone;
        }
    return error;
    }

//---------------------------------------------------------------------------
// To obtain consumer during processing OUTBOUND.
//---------------------------------------------------------------------------
//   
        
TInt CWSStarServiceSession::SessionConsumerL( const TDesC8& aSenderID,
                                              CWSStarSessionConsumer*& aSessionConsumer )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::SessionConsumerL");                        
    aSessionConsumer = (CWSStarSessionConsumer*) RemoteConsumerL(aSenderID);
    if(aSessionConsumer)
        {
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }

// SYNC IMPLEMENTATION(!) - only available internally.
//  It means, that all SYNC operation for 3rd developer from public API are in fact ASYNC. 
//  @see SenServiceConnectionImpl.cpp
//  aSOAPMessage.IsFault() should always be TRUE.
// 
TInt CWSStarServiceSession::HandleSoapFaultL( CSenSoapMessage* apSOAPMessage,
                                              HBufC8*& aResponse )
    { 
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::HandleErrorL(CSenSoapMessage&, HBufC8*&)");

    TInt retVal(KErrNone);
    
    if ( apSOAPMessage )
        {
        CleanupStack::PushL( apSOAPMessage ); // ownerhip is here
        TInt answer = CanHandleErrorL();
        
        if (answer == KErrSenResendRequired)
            {
            CleanupStack::PopAndDestroy( apSOAPMessage ); // not needed atm
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CanHandleErrorL == TRUE");
    
            CWSStarSessionConsumer* pConsumer = NULL;
    
            // this takes care of messageIDs
            retVal = SessionConsumerL(iInContext, pConsumer);
            if(!pConsumer)
                {
                CleanupStack::PopAndDestroy( apSOAPMessage );
                return KErrNotFound;
                }
            const TDesC8* relatesTo = iInContext->GetDesC8L(WSStarContextKeys::KRelatesTo);
            if (!relatesTo)
                {
                //if response from backend doesn't include any relatesTo we can try assume
                //      that response is for last outgoing message. This trick is used by passport phase
                const TInt* isPassportEnabled = iInContext->GetIntL(WSStarContextKeys::KPassportEnabled);
                if (isPassportEnabled && *isPassportEnabled == TRUE)
                    {
                    relatesTo = iSessionContext->GetDesC8L(WSStarContextKeys::KMessageID);    
                    }
                }
            if (relatesTo)
                {
                TDesC8* pBody = pConsumer->BodyL(*relatesTo);
                if (pBody)
                    {
                    HBufC8* pTemp = HBufC8::NewLC(pBody->Length());
                    TPtr8 temp = pTemp->Des();
                    temp.Append(*pBody);
    
                    HBufC8* startBody = HBufC8::NewLC(KSenSoapEnvelopeBodyQName().Length()+
                                                    KSenLessThan().Length()+
                                                    KSenGreaterThan().Length());
                                                    
                    TPtr8 startBodyPtr = startBody->Des();
                    startBodyPtr.Append(KSenLessThan());                                
                    startBodyPtr.Append(KSenSoapEnvelopeBodyQName());                                
                    startBodyPtr.Append(KSenGreaterThan());                                
                    HBufC8* endBody = HBufC8::NewLC(KSenSoapEnvelopeBodyQName().Length()+
                                                    KSenLessThanSlash().Length()+
                                                    KSenGreaterThan().Length());
                                                    
                    TPtr8 endBodyPtr = endBody->Des();
                    endBodyPtr.Append(KSenLessThanSlash());                                
                    endBodyPtr.Append(KSenSoapEnvelopeBodyQName());                                
                    endBodyPtr.Append(KSenGreaterThan());                                
    
                    if( SenXmlUtils::StartsWith(*pBody, *startBody) )
                        {
                        temp.Delete( 0,startBody->Length() );
                        }
    
                    if( SenXmlUtils::EndsWith(*pTemp, *endBody) )
                        {
                        temp.Delete( pTemp->Length()-endBody->Length(),endBody->Length() );
                        }
    
                    CleanupStack::PopAndDestroy(endBody);                
                    CleanupStack::PopAndDestroy(startBody);                
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"Re-submitting the request");
                    //clear Id
                    retVal = SubmitL(*pTemp,
                           pConsumer->TransportProperties(*relatesTo),
                            *pConsumer, aResponse);
                    //retVal = SubmitL(*pTemp, KNullDesC8, *pConsumer, aResponse);        
                    CleanupStack::PopAndDestroy(pTemp);                
                    return retVal;
                    }    
                }
            }
        // In any other case, it is mean that we can not handle error, we can not
        // replace with new response.
                    
        // This is a fault which WS* cannot handle
        
        // or ESenReAuthAndResendNeeded which is not supported in SYNC internal mode.
        // In other words, wst:RenewNeeded is not supported when it coems from STS. 
        //     It is only supported when it comes from WebService, bit such scenario 
        //     use only ASYNC HandleError
        
        CSenSoapFault* pDetached = apSOAPMessage->DetachFaultL();
        CleanupStack::PopAndDestroy( apSOAPMessage ); // de-alloc msg after detach!
        TBool completeServerMessages(EFalse);
        HasFacetL(KCompleteMessagesFacet, completeServerMessages);

        if( pDetached )
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"Detached a SOAP fault");
            CleanupStack::PushL(pDetached);
            // Response contains only soapfault in case no completeServerMessages facet 
            // otherwise it should be left empty
            // aResponse is updated only when detached soap fault is required.
            if(completeServerMessages == EFalse) 
            	{
            	aResponse = pDetached->AsXmlL();	
            	}
            CleanupStack::PopAndDestroy(pDetached); 
            return KErrSenSoapFault; // this is 2nd OK return code!
            }
        else
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- No SOAP fault was received. Returning KErrNotFound");
            return KErrNotFound;
            }
        }
    else // apSOAPMessage == NULL
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- Fatal(!): CWSStarServiceSession::HandleErrorL - apSOAPMessage == NULL!. Returning KErrNotFound.");
        return KErrNotFound;
        }
    }

  void CWSStarServiceSession::ReattachCredentialsL()
	{
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("  -Entering ReattachCredentialsL iStatus : %d"), iStatus));
	
	TInt retValue = TryToSearchValidCredentialL();
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("  -ReattachCredentialsL retValue : %d"), retValue));
    if(retValue != KErrNone)
    {
    iValidUntil.Set(_L("19000101:")); // way back in history: January 1st 1900
    iStatus = ComputeStatusL();			
    }
	TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("  -Leaving ReattachCredentialsL iStatus : %d"), iStatus));
	}
// ASYNC
TInt CWSStarServiceSession::HandleSoapFaultL( CSenSoapMessage* apSOAPMessage,
                                              const TInt aErrorCode, // should be KErrSenSoapFault..
                                              const TInt aTxnId,
                                              MSenRemoteServiceConsumer& aConsumer,
                                              MSenProperties* aResponseTransportProperties )

    {
    TInt err(KErrNone);
    CSLOG_L(aConsumer.ConnectionId() ,KMinLogLevel ,"CWSStarServiceSession::HandleErrorL(CSenSoapMessage&)");
    

    if( apSOAPMessage )
        {
        CleanupStack::PushL(apSOAPMessage);
        TInt answer = CanHandleErrorL();
        
        if (answer)
            {
            aConsumer.HandleErrorL(NULL, answer, aTxnId, aResponseTransportProperties);
            }
        else
            {
            TBool hasFacet = EFalse;
            TInt err1 = HasFacetL(KCompleteMessagesFacet, hasFacet);
            HBufC8* pAsXml = NULL;
            if (hasFacet && (err1 == KErrNone))
                {
                CSLOG_L(aConsumer.ConnectionId() ,KNormalLogLevel ,"Full a SOAP fault");
                
                pAsXml = apSOAPMessage->AsXmlL();
                }
            else
                {
                CSenSoapFault* pDetached = apSOAPMessage->DetachFaultL();
                if(pDetached)
                    {
                    CSLOG_L(aConsumer.ConnectionId() ,KMinLogLevel ,"Detached a SOAP fault");
                    
                    CleanupStack::PushL(pDetached);
                    // pass KErrSenSoapFault // which is 2nd "OK" return code!
                    pAsXml = pDetached->AsXmlL();
                    CleanupStack::PopAndDestroy(pDetached);
                    }
                }
            if(pAsXml)
                {
                aConsumer.HandleErrorL(pAsXml, KErrSenSoapFault, aTxnId, aResponseTransportProperties);
                err = KErrNone;
                }
            else
                {
                CSLOG_L(aConsumer.ConnectionId() ,KMinLogLevel ,"No SOAP fault was received.");
                
                aConsumer.HandleErrorL(NULL, aErrorCode, aTxnId, aResponseTransportProperties);
                err = KErrSenInternal; // pass on the orig error code(?)
                }    
            }
        CleanupStack::PopAndDestroy(apSOAPMessage);
        }
    else
        {
        CSLOG_L(aConsumer.ConnectionId() ,KMinLogLevel ,"Fatal(!): CWSStarServiceSession::HandleSoapFaultL - apSoapMessage arg is NULL.");
        
        /*TInt retVal =*/ aConsumer.HandleErrorL(NULL, aErrorCode, aTxnId, aResponseTransportProperties);
        err = KErrSenInternal;
        }        
    return err;
    }

TInt CWSStarServiceSession::CanHandleErrorL()
    {
    TInt answer(KErrNone);
    
    //if failedAuthentication (SCT is now switched to MT, therefore we have to resend
    const TInt* retryAfter = iSessionContext->GetIntL(WSStarContextKeys::KRetryAfter());
    //othar case is simple retry after from service
    if (!retryAfter)
        retryAfter = iInContext->GetIntL(WSStarContextKeys::KRetryAfter());
    const TDesC8* redirect = iInContext->GetDesC8L(WSStarContextKeys::KRedirect());
    
    const TBool* renewNeeded = NULL;
    if (iSessionContext)
        {
        renewNeeded = iSessionContext->GetIntL(WSStarContextKeys::KReAuthNeeded());
        }
     //temporary code, becouse its server proces, we have to leave immediatlly

    //retry in milisecunds
    if (retryAfter && 
        *retryAfter <= WSStarSession::KMaxTimeForRetry*1000 &&
        *retryAfter >= KErrNone &&
        iRetryCounter < WSStarSession::KCounterMax)
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CWSStarServiceSession::CanHandleErrorL - RETRY request with delay = %d"), *retryAfter));        
        RTimer timer;  // The synchronous timer
        TRequestStatus timerStatus; // associated with timer
        timer.CreateLocal();   // Always created for this thread.
        timer.After(timerStatus,*retryAfter*1000);//in microseconds
        User::WaitForRequest(timerStatus);
        timer.Close();
       
        iRetryCounter++;
        answer = KErrSenResendRequired;  
        }
    else if((renewNeeded && *renewNeeded &&
           iRenewCounter < WSStarSession::KCounterMax)
           )
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CWSStarServiceSession::CanHandleErrorL - RENEW request (new ticket is needed) iRenewCounter = [%d]"), iRenewCounter)) ;
        iRenewCounter++;
        answer = KErrSenReinitRequired;                    
        }
    else if (redirect &&
           iRedirectCounter< WSStarSession::KCounterMax)
        {
        iRedirectCounter++;
        this->SetEndPointL(*redirect);
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("CWSStarServiceSession::CanHandleErrorL - REDIRECT request iRenewCounter = [%d]"), iRedirectCounter) );
        answer = KErrSenResendRequired;
        }
    else
        {
        iRetryCounter = 0;
        iRenewCounter = 0;
        iRedirectCounter =0;
        answer = KErrNone;
        }
    return answer;
    }

TBool CWSStarServiceSession::Matches(MSenServiceDescription& aPattern)
    {
    TBool matches = CSenWebServiceSession::Matches(aPattern);
    if (!matches) return EFalse;

    /*HBufC8* patternClusterValue = NULL;
    TPtrC8 clusterFacetName(WSStarSession::KClusterLocalName);
    aPattern.FacetValue(clusterFacetName, patternClusterValue);
    CleanupStack::PushL(patternClusterValue);
    HBufC8* thisClusterValue = NULL;
    this->FacetValue(clusterFacetName, thisClusterValue);
    CleanupStack::PushL(thisClusterValue);
    if (!(thisClusterValue &&
        patternClusterValue &&
        *thisClusterValue == *patternClusterValue))
        {
        matches = EFalse;
        }
        
    CleanupStack::PopAndDestroy(thisClusterValue);
    CleanupStack::PopAndDestroy(patternClusterValue);*/

    MSenElement& xmlPatternAsElement = ((CSenWSDescription&)aPattern).AsElement();

//    MSenElement* pElement = xmlPatternAsElement.Element(WSStarSession::KProviderIdElementLocalName);
    MSenElement* pElement = xmlPatternAsElement.Element( KSenIdpProviderIdLocalname );
    //MSenElement* pElement =
     //                   xmlPatternAsElement.Element(WSStarSession::KProviderIdElementLocalName);
    if(pElement)
        {
        TPtrC8 patternProviderID = pElement->Content();
        TPtrC8 thisProviderID = ProviderID();
        if(patternProviderID.Length()>0)
            {
            if(!(thisProviderID.Length()>0 && patternProviderID == thisProviderID))
                {
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
    // for now the only interesting facet is the messageThread facet
    // if this session uses messageThreads it should not match any
    // description, as it essentially makes the session unique
    // (unless nobody is using this session).
    if (iMessageThread && HasConsumer())
        {
        // a new session is needed in any case
        matches = EFalse;
        }
    else
        {
        TBool hasFacet;
        TRAPD(leaveCode, aPattern.HasFacetL(KMessageThread, hasFacet));
        if (leaveCode != KErrNone) return EFalse;
        if (hasFacet)
            {
            if (matches && !HasConsumer()) matches = ETrue;
            else matches = EFalse;
            }
        }
    return matches;
    }

TInt CWSStarServiceSession::ScoreMatchL(MSenServiceDescription& aPattern)
    {
    TInt score = CSenWebServiceSession::ScoreMatchL(aPattern);
    
    MSenElement& xmlPatternAsElement = ((CSenWSDescription&)aPattern).AsElement();
//    MSenElement* pElement = xmlPatternAsElement.Element(WSStarSession::KProviderIdElementLocalName);
    MSenElement* pElement = xmlPatternAsElement.Element(KSenIdpProviderIdLocalname);
   // MSenElement* pElement =
     //                   xmlPatternAsElement.Element(WSStarSession::KProviderIdElementLocalName);

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

void CWSStarServiceSession::StartTransaction()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::StartTransaction()");                                            
    iMessageThread = ETrue;
    }

void CWSStarServiceSession::TransactionCompleted()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::TransactionCompleted()");                                    
    delete ipReceivedMessageIdInTrans;
    ipReceivedMessageIdInTrans = NULL;
    iMessageThread = EFalse;    
    }

TInt CWSStarServiceSession::SetTransportPropertiesL(const TDesC8& aProperties,
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
        CWSStarSessionConsumer* pConsumer = NULL;
        //senderID is internal id of consumer
        TInt error = SessionConsumerL(aConsumer.Id(), pConsumer);
        if (!error && pConsumer)
            {
            MSenTransport& tp = pConsumer->TransportL();
            HBufC8* transProp = ApplyTransportPropertiesL(aProperties);
            CleanupStack::PushL(transProp);
            tp.SetPropertiesL(*transProp, MSenLayeredProperties::ESenTransportLayer, &aConsumer);
            CleanupStack::PopAndDestroy(transProp);
            }
        return KErrNone;
    }

HBufC8* CWSStarServiceSession::ApplyTransportPropertiesL(const TDesC8& aTransportProperties)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::ApplyTransportPropertiesL():");
    TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(aTransportProperties));
    HBufC8* result = NULL;
    CSenXmlReader* reader = XmlReader();
    TInt error(KErrNone);
   TPtrC8 value;
    CSenVtcpTransportProperties* vtcpTransProp = NULL;
    if (aTransportProperties.Length() <= KSenXmlPropertiesLocalname().Length()*2 
                                                                    + KSenLessThanSlash().Length()
                                                                    + KSenLessThan().Length()
                                                                    + KSenGreaterThan().Length()*2)
        {
        if (iTransProp && iTransProp->Length())
            {
            vtcpTransProp = CSenVtcpTransportProperties::NewLC(*iTransProp, *reader);
            }
        else
            {
            vtcpTransProp = CSenVtcpTransportProperties::NewLC();
            }
        }
    else
        {
        vtcpTransProp = CSenVtcpTransportProperties::NewLC(aTransportProperties, *reader);
        if (iTransProp) 
            {
            //1 merge session and message layer transport properties
            CSenVtcpTransportProperties* conVtcpTransProp = CSenVtcpTransportProperties::NewLC(*iTransProp, *reader);
            
            //download folder
            if (vtcpTransProp->DownloadFolderL(value) == KErrNotFound)
                {
                if (conVtcpTransProp->DownloadFolderL(value) != KErrNotFound)
                    {
                    vtcpTransProp->SetDownloadFolderL(value);
                    }
                }
            //device id
            if (vtcpTransProp->DeviceIDL(value) == KErrNotFound)
                {
                if (conVtcpTransProp->DeviceIDL(value) != KErrNotFound)
                    {
                    vtcpTransProp->SetDeviceIDL(value);
                    }
                }

            if (vtcpTransProp->ProxyHostL(value) == KErrNotFound)
                {
                if (conVtcpTransProp->ProxyHostL(value) != KErrNotFound)
                    {
                    vtcpTransProp->SetProxyHostL(value);
                    }
                }

            if (vtcpTransProp->MwsNamespaceL(value) == KErrNotFound)
                {
                if (conVtcpTransProp->MwsNamespaceL(value) != KErrNotFound)
                    {
                    vtcpTransProp->SetMwsNamespaceL(value);
                    }
                }

            TBool boolValue;
            if (vtcpTransProp->ConnectionBoundL(boolValue) == KErrNotFound)
                {
                if (conVtcpTransProp->ConnectionBoundL(boolValue) != KErrNotFound)
                    {
                    vtcpTransProp->SetConnectionBoundL(boolValue);
                    }
                }
            if (vtcpTransProp->OnewayMessageOnOffL(boolValue) == KErrNotFound)
                {
                if (conVtcpTransProp->OnewayMessageOnOffL(boolValue) != KErrNotFound)
                    {
                    vtcpTransProp->SetOnewayMessageOnOffL(boolValue);
                    }
                }
            if (vtcpTransProp->BoolPropertyL(KSenIAPDoNotPrompt, boolValue) == KErrNotFound)
                {
                if (conVtcpTransProp->BoolPropertyL(KSenIAPDoNotPrompt, boolValue) != KErrNotFound)
                    {
                    vtcpTransProp->SetBoolPropertyL(KSenIAPDoNotPrompt, boolValue);
                    }
                }
            TInt intvalue;
            if (vtcpTransProp->ConnectionTimeOutL(intvalue) == KErrNotFound)
                {
                if (conVtcpTransProp->ConnectionTimeOutL(intvalue) != KErrNotFound)
                    {
                    vtcpTransProp->SetConnectionTimeOutL(intvalue);
                    }
                }
            if (vtcpTransProp->DeviceLCIDL(value) == KErrNotFound)
                {
                if (conVtcpTransProp->DeviceLCIDL(value) != KErrNotFound)
                    {
                    vtcpTransProp->SetDeviceLCIDL(value);
                    }
                }
            if (vtcpTransProp->DeviceLCIDL(value) == KErrNotFound)
                {
                if (conVtcpTransProp->DeviceLCIDL(value) != KErrNotFound)
                    {
                    vtcpTransProp->SetDeviceLCIDL(value);
                    }
                }
            if (vtcpTransProp->UserAgentL(value) == KErrNotFound)
                {
                if (conVtcpTransProp->UserAgentL(value) != KErrNotFound)
                    {
                    vtcpTransProp->SetUserAgentL(value);
                    }
                }
            TUint32 uint32value;
            if (vtcpTransProp->IapIdL(uint32value) == KErrNotFound)
                {
                if (conVtcpTransProp->IapIdL(uint32value) != KErrNotFound)
                    {
                    vtcpTransProp->SetIapIdL(uint32value);
                    }
                }
            if (vtcpTransProp->HeartbeatL(intvalue) == KErrNotFound)
                {
                if (conVtcpTransProp->HeartbeatL(intvalue) != KErrNotFound)
                    {
                    vtcpTransProp->SetHeartbeatL(intvalue);
                    }
                }
            if (vtcpTransProp->MaxTimeToLiveL(intvalue) == KErrNotFound)
                {
                if (conVtcpTransProp->MaxTimeToLiveL(intvalue) != KErrNotFound)
                    {
                    vtcpTransProp->SetMaxTimeToLiveL(intvalue);
                    }
                }
            if (vtcpTransProp->MinTimeToLiveL(intvalue) == KErrNotFound)
                {
                if (conVtcpTransProp->MinTimeToLiveL(intvalue) != KErrNotFound)
                    {
                    vtcpTransProp->SetMinTimeToLiveL(intvalue);
                    }
                }
            CleanupStack::PopAndDestroy(conVtcpTransProp);    
            }
        }
    
    


    error = vtcpTransProp->DeviceIDL(value);
    if (error || !value.Length())
        {
        value.Set(KNullDesC8());
        error = ((CWSStarPlugin&)iFramework).DeviceId(value);   
        if (!error)
            {
            vtcpTransProp->SetDeviceIDL(value);  			
            }
        }
    if (iOutContext)
        {
        if (value.Length())//value of device Id
            {
            TPtrC8 endpoint = Contract();
            TInt pos = endpoint.LocateReverse(TChar('/'));
            if (pos!=KErrNotFound)
                {
                TPtrC8 serviceName = endpoint.Mid(pos);
                HBufC8* deviceId = HBufC8::NewLC(value.Length()+serviceName.Length());
                TPtr8 deviceIdDes = deviceId->Des();
                deviceIdDes.Append(value);
                deviceIdDes.Append(serviceName);
                iOutContext->Update(WSStarContextKeys::KReplyToDeviceAddress, *deviceId);
                CleanupStack::PopAndDestroy(deviceId);
                }
            else
                {
                iOutContext->Update(WSStarContextKeys::KReplyToDeviceAddress, value);
                }
            }
        error = vtcpTransProp->ProxyHostL(value);
        if (!error)
            {
            iOutContext->Update(WSStarContextKeys::KReplyTo, value);
            }
	#ifdef RD_SEN_VTCP_SUPPORT
        //Proxy url CR
        error = vtcpTransProp->PropertyL(KSenConnectionProxyUrl,value);
        if (!error)
            {
            iOutContext->Update(WSStarContextKeys::KReplyTo, value);
            }	
	#endif//RD_SEN_VTCP_SUPPORT

        error = vtcpTransProp->MwsNamespaceL(value);
        if (!error)
            {
            iOutContext->Update(WSStarContextKeys::KMwsNamespace, value);
            }
        error = vtcpTransProp->SoapActionL(value);
        if (!error)
            {
            _LIT8(KQuote, "\"");

            HBufC8* pTemp = HBufC8::NewLC(value.Length());

            TPtr8 temp = pTemp->Des();

            temp.Append(value);

            if(SenXmlUtils::StartsWith(value, KQuote))
                {
                temp.Delete(0,KQuote().Length());
                }

            if(SenXmlUtils::EndsWith(*pTemp, KQuote))
                {
                temp.Delete(pTemp->Length()-1,KQuote().Length());
                }

            iOutContext->Update(WSStarContextKeys::KAction, *pTemp);
            CleanupStack::PopAndDestroy(pTemp);
            }
        //some GUI clients need messageId in GUI level :( and has control logic realted to messageId
        error = vtcpTransProp->PropertyL( KSenClientGeneratedMessageId, value );
        if (!error && iOutContext)
            {
                iOutContext->Update(WSStarContextKeys::KMessageID, value);
            }           
        }
    HBufC8* updatedTP = vtcpTransProp->AsUtf8LC();
    CSenHttpTransportProperties* httpTransProp = NULL;
    if (!updatedTP->Length())
        {
        httpTransProp = CSenHttpTransportProperties::NewLC();
        }
    else
        {
        httpTransProp = CSenHttpTransportProperties::NewLC(*updatedTP, *reader);
        }
    const TInt* soapVer = iSessionContext->GetIntL(WSStarContextKeys::KSoapVersion);
    error = httpTransProp->SoapActionL(value);
    if (!error && soapVer)
        {
        httpTransProp->ApplyBindingL((TSOAPVersion)*soapVer);
        }

    result = httpTransProp->AsUtf8L();
    CleanupStack::PopAndDestroy(httpTransProp);
    CleanupStack::PopAndDestroy(updatedTP);
    CleanupStack::PopAndDestroy(vtcpTransProp);
    return result;
    }

TInt CWSStarServiceSession::SendL( const TDesC8& aMessage, 
                                   const TDesC8& aTransportProperties,
                                   MSenRemoteServiceConsumer& aConsumer,
                                   TInt& aTxnId,
                                   HBufC8*& aRevalidationError )
    {
    CSLOG_L(aConsumer.ConnectionId() ,KMinLogLevel ,"CWSStarServiceSession::SendL(TDesC8&, ...)");
    TInt retVal(KErrNone);
    if ( AmIHostletSession() )
        {
        CSLOG_L(aConsumer.ConnectionId() ,KMinLogLevel ,"- This is a session for a hostlet.");
        
        retVal  = CSenWebServiceSession::SendL( aMessage, aTransportProperties, aConsumer, aTxnId, aRevalidationError );
        CSLOG_FORMAT((aConsumer.ConnectionId(), KNormalLogLevel , _L8("- Return value from CSenWebServiceSession::SendL: %d"), retVal ));
        
        CSLOG_FORMAT((aConsumer.ConnectionId(), KNormalLogLevel , _L8("- Transaction ID from transport: %d"), aTxnId ));
        
        }
    else
        {
        CSLOG_L(aConsumer.ConnectionId() ,KMinLogLevel ,"- This is a session for a consumer.");
        
        PrepareOutCtxL(aTransportProperties);
        FindAndShareSCTL();
        
        VerifyPermissionL();
        //we still have to check if validUntil is not expired.
        retVal = RevalidateMobileTicketIfExpiredL( aRevalidationError ); // RefreshMTL
        iSessionContext->Update(WSStarContextKeys::KOnlySharing, ETrue);
        //retVal = RefreshMTL( aRevalidationError );

#ifdef _SENDEBUG
        if( aRevalidationError )
            {
            CSLOG_L(aConsumer.ConnectionId() ,KMinLogLevel ,"- SOAP Fault / error from re-validation:");
        
            CSLOG_ALL(aConsumer.ConnectionId() ,KMinLogLevel ,( *aRevalidationError ));
        
            }
#endif // _SENDEBUG            
        if( retVal == KErrNone )
            {
            delete aRevalidationError; // should ALWAYS be null in here (as retval from RevalidateMobileTicketIfExpiredL was KErrNone!) //(as retval from RefreshMT was KErrNone!)
            aRevalidationError = NULL;
            HBufC8* transProp = ApplyTransportPropertiesL(aTransportProperties);
            CleanupStack::PushL(transProp);
            retVal  = CSenWebServiceSession::SendL( aMessage, *transProp, aConsumer, aTxnId, aRevalidationError );
            CleanupStack::PopAndDestroy(transProp);    
            }
        //else { // if RevalidateMobileTicketIfExpiredL method provided such // } // return error code and aRevalidationError, if RefreshMT method provided such // }
        }
    return retVal;
    }


TInt CWSStarServiceSession::SubmitL( const TDesC8& aMessage, 
                                     const TDesC8& aTransportProperties,
                                     MSenRemoteServiceConsumer& aConsumer,
                                     HBufC8*& aResponse )
    {
    CSLOG_L(aConsumer.ConnectionId() ,KMinLogLevel ,"CWSStarServiceSession::SubmitL(TDesC8&, ...)");
    
    TInt retVal(KErrNone);
    if (!AmIHostletSession())
        {
        PrepareOutCtxL(aTransportProperties);
        HBufC8* transProp = ApplyTransportPropertiesL(aTransportProperties);
        CleanupStack::PushL(transProp);
        retVal = CSenWebServiceSession::SubmitL(aMessage, *transProp, aConsumer, aResponse);
        CleanupStack::PopAndDestroy(transProp);
        if (retVal == KErrNone)
            {
            iSubmitState = WSStarSession::KSubmitStateOK;
            }

    //becouse of lack phase (dispatch to proprr consumer). 
    //It is SUBMIT nature, only used internally between Client and STS
    //we have to make validation here
        CWSStarSessionConsumer* consumer = NULL;
        if (iInContext)
            {
            SessionConsumerL(iInContext, consumer);    
            if (!(consumer && consumer->Id() == aConsumer.Id()))
                {
                retVal = KErrSenBrokenSoapEnvelope;
                }
            }
        }
    return retVal;
    }

TInt CWSStarServiceSession::SubmitSoapL( const TDesC8& aSoapMessage, 
                                         const TDesC8& aTransportProperties,
                                         MSenRemoteServiceConsumer& aConsumer,
                                         HBufC8*& aResponse )
    {
    CSLOG_L(aConsumer.ConnectionId() ,KMinLogLevel ,"CWSStarServiceSession::SubmitSoapL(TDesC8& ...)");
    
    TInt retVal(KErrNone);
    if (!AmIHostletSession())
        {
        PrepareOutCtxL(aTransportProperties);
        CSenSoapMessage* message = NULL;
        HBufC8* body = NULL;    
        CreateAndParseSoapMessageL(aSoapMessage, message, body);
        CleanupStack::PushL(body);
        // message ownership transfered to MessageForSendingL
        HBufC8* transProp = ApplyTransportPropertiesL(aTransportProperties);
        CleanupStack::PushL(transProp);
            
        retVal = MessageForSendingL(*body,aConsumer.Id(),message);
        CleanupStack::PushL(message);    
        HBufC8* pMsg = message->AsXmlL();
        CleanupStack::PushL(pMsg);    
        if (retVal == KErrNone)
            {
            retVal = CSenWebServiceSession::SubmitSoapL(*pMsg, *transProp, aConsumer, aResponse);
            }
        CleanupStack::PopAndDestroy(pMsg);
        CleanupStack::PopAndDestroy(message);
        CleanupStack::PopAndDestroy(transProp);
        CleanupStack::PopAndDestroy(body);
        if (retVal == KErrNone)
            {
            iSubmitState = WSStarSession::KSubmitStateOK;
            }

    //becouse of lack phase (dispatch to proprr consumer). 
    //It is SUBMIT nature, only used internally between Client and STS
    //we have to make validation here
        CWSStarSessionConsumer* consumer = NULL;
        if (iInContext)
            {
            SessionConsumerL(iInContext, consumer);    
            }
        if (!(consumer && consumer->Id() == aConsumer.Id()))
            {
            retVal = KErrSenBrokenSoapEnvelope;
            }
        }
    return retVal;
            
    }

TInt CWSStarServiceSession::SendSoapL( const TDesC8& aSoapMessage, 
                                       const TDesC8& aTransportProperties,
                                       MSenRemoteServiceConsumer& aConsumer,
                                       TInt& aTxnId,
                                       HBufC8*& aRevalidationError )
    {
    CSLOG_L(aConsumer.ConnectionId() ,KMinLogLevel ,"CWSStarServiceSession::SubmitSoapL(TDesC8& ...)");
    
    TInt retVal(KErrNone);
    if ( !AmIHostletSession() ) // no support for SendSoapL in hostlet sessions
        {
        PrepareOutCtxL(aTransportProperties);
        FindAndShareSCTL();
        VerifyPermissionL();
        // We still have to check if validUntil is not expired.
        retVal = RevalidateMobileTicketIfExpiredL( aRevalidationError ); // RefreshMTL
        iSessionContext->Update(WSStarContextKeys::KOnlySharing, ETrue);
        //retVal = RefreshMTL( aRevalidationError );

#ifdef _SENDEBUG
        if( aRevalidationError )
            {
            CSLOG_L(aConsumer.ConnectionId() ,KMinLogLevel ,"- SOAP Fault / error from re-validation:");
    
            CSLOG_ALL(aConsumer.ConnectionId() ,KMinLogLevel ,( *aRevalidationError ));
           
            }
#endif // _SENDEBUG            

        if (retVal == KErrNone) 
            {
            CSenSoapMessage* message = NULL;
            HBufC8* body = NULL;    
            CreateAndParseSoapMessageL(aSoapMessage, message, body);
            CleanupStack::PushL(body);
            // message ownership transfered to MessageForSendingL
            HBufC8* transProp = ApplyTransportPropertiesL(aTransportProperties);
            CleanupStack::PushL(transProp);
            retVal = MessageForSendingL(*body,aConsumer.Id(),message);
            CleanupStack::PushL(message);    
            HBufC8* pMsg = message->AsXmlL();
            CleanupStack::PushL(pMsg);
            ParseToSoapMessage2L( *pMsg,aConsumer,aTxnId );

//wslog            FILELOGALL(_L("WsLog"), _L("last_req.xml"), pMsg->Des());

            if (retVal == KErrNone)
                {
                delete aRevalidationError; // should ALWAYS be null in here (as retval from RevalidateMobileTicketIfExpiredL was KErrNone!)// RefreshMT was KErrNone!)
                aRevalidationError = NULL;
                retVal = CSenWebServiceSession::SendSoapL( *pMsg, *transProp, aConsumer, aTxnId, aRevalidationError );
                }
            CleanupStack::PopAndDestroy(pMsg);
            CleanupStack::PopAndDestroy(message);
            CleanupStack::PopAndDestroy(transProp);
            CleanupStack::PopAndDestroy(body);
            }
        //else { // re-validation failed, return an error and aRevalidationError descriptor }
        }
    return retVal;
    }

void CWSStarServiceSession::CreateAndParseSoapMessageL(const TDesC8& aSoapMessage, CSenSoapMessage*& aMessage, HBufC8*& aBody)
    {
    CSenXmlReader* reader = XmlReader();   

    aMessage = CSenSoapMessage::NewL();
    aMessage->SetReader(*reader);
    aMessage->BuildFrom(aSoapMessage);
    const TDesC8& uri = aMessage->NsUri();
    TInt version;
    if (uri == KSenSoapEnvelopeXmlns)
        {
        version = ESOAP11;
        }
    else
        {
        version  = ESOAP12;
        }

    if (iOutContext)
        {
        iSessionContext->Update(WSStarContextKeys::KSoapVersion,version);
        }

    CSenElement& bodyEl = aMessage->BodyL();
    HBufC8* bodyXml = bodyEl.AsXmlL();
    CleanupStack::PushL(bodyXml);
    TInt length = bodyXml->Length();
    CleanupStack::PopAndDestroy(bodyXml);
    aBody = HBufC8::NewLC(length);
    RPointerArray<CSenElement>& elements = bodyEl.ElementsL();
    TPtr8 body = aBody->Des();
    for (TInt i=0;i<elements.Count();i++)
        {
        CSenElement* element = elements[i];
        HBufC8* elXml = element->AsXmlL();
        CleanupStack::PushL(elXml);
        body.Append(*elXml);
        CleanupStack::PopAndDestroy(elXml);
        }   
    CleanupStack::Pop(aBody);
    }
    
TBool CWSStarServiceSession::IsExpiredL()
    {
    CSenServiceSession::SetStatusL();
    return (CSenServiceSession::StatusL() == KSenConnectionStatusExpired);
    }
    
        
void CWSStarServiceSession::SetSessionContext(CWSStarSessionContext* aContext)
    {
    delete iSessionContext;
    iSessionContext = aContext;
    }
CWSStarSessionContext* CWSStarServiceSession::SessionContext()
    {
    return iSessionContext;
    }
    
void CWSStarServiceSession::ClearCredentialL()
    {
    // Before removing the credential, checkt that
    // iCredentialPtr.Credential() != NULL
    if ( HasSecurity() ) // this performs the NULL check for credential
    	{
	    // Remove Invalid Credential from Credentials DB
	    TInt credentialId = iCredentialPtr.Credential()->IdentifierL().IdL();	//codescannerwarnings
	    ((MSenServiceManager&)iFramework.Manager()).RemoveCredentialL(credentialId);	//codescannerwarnings
    	}

    // Remove Credential usage from Session
    SetSecurityL(KNullDesC8);
    iValidUntil.Set(_L("18000101:"));
    }
    
TInt CWSStarServiceSession::InitializeFromL( MSenServiceDescription& aDescription, 
                                             CWSStarPolicyHandler* aPolicyHandler )
    {
    CSenWebServiceSession::SetSecurityL(KNullDesC8);
    iValidUntil.Set(_L("19000101:"));//from SetSecurityL()moved to here, because ID-WSF doesn't need
    aDescription.HasFacetL(KMessageThread,iMessageThread);
    
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
        //MSenElement* pElement =
         //               xmlSdAsElement.Element(WSStarSession::KProviderIdElementLocalName);
        delete iProviderID;
        iProviderID = NULL;
        if(pElement)
            {
            iProviderID = pElement->Content().AllocL();
            }
        else
            {
            CSenIdentityProvider* provider = ((CWSStarPlugin&)iFramework).
                    Manager().IdentityProviderL(aDescription);
            if (provider)
                {
                iProviderID = provider->ProviderID().AllocL();
                }
            }
        pElement = xmlSdAsElement.Element(WSStarSession::KTrustAnchorElementLocalName);
        if(pElement)
            {
            delete iTrustAnchor;
            iTrustAnchor = NULL;
            iTrustAnchor = pElement->Content().AllocL();
            }
        
        CSenXmlReader* reader = XmlReader();
        if (!iSessionContext) 
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::InitializeFromL");
            iSessionContext = CWSStarSessionContext::NewL(reader, &aDescription, aPolicyHandler);
            
            pElement = xmlSdAsElement.Element(WSStarSession::KSTRLocalName);
            if(pElement)
                {
                CSenElement* pNestedElement = pElement->Child(0);//assumption STR always has wsse:SecurityTokenReference
                if (pNestedElement)
                    {
                    HBufC8* content = pNestedElement->AsXmlL();
                    CleanupStack::PushL(content);
                    if (content)
                        {
                        User::LeaveIfError(iSessionContext->Add(WSStarContextKeys::KSTR, *content));
                        }
                    CleanupStack::PopAndDestroy(content);    
                    }
                }
                
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

            if ( !iCredentialPtr.Credential() )
                {
                pElement = xmlSdAsElement.Element(WSStarSession::KPOPBase64LocalName);
                if(pElement)
                    {
                    User::LeaveIfError(iSessionContext->Add(WSStarContextKeys::KPOPBase64, pElement->Content()));    
                    }
                pElement = xmlSdAsElement.Element(WSStarSession::KTokenType);
                if(pElement)
                    {
                    User::LeaveIfError(iSessionContext->Add(WSStarContextKeys::KTokenType, pElement->Content()));    
                    }
                pElement = xmlSdAsElement.Element(WSStarSession::KBinaryTypeLocalName);
                if(pElement)
                    {
                    User::LeaveIfError(iSessionContext->Add(WSStarContextKeys::KBinaryType, pElement->Content()));    
                    }
                pElement = xmlSdAsElement.Element(WSStarSession::KCreatedLocalName);
                if(pElement)
                    {
                    User::LeaveIfError(iSessionContext->Add(WSStarContextKeys::KTimestampCreated, pElement->Content()));    
                    }
                pElement = xmlSdAsElement.Element(WSStarSession::KPhoneTimeWhenMTResolvedLocalName);
                if(pElement)
                    {
                    User::LeaveIfError(iSessionContext->Add(WSStarContextKeys::KPhoneTimeWhenMTResolved, pElement->Content()));    
                    }
                }
                
            AddSecurityTokenToContextL();                
            ActiveTicketObserverL();           
            }
        }
    else
        {
        CSenWebServiceSession::InitializeFromL(aDescription);
        }
        
    return KErrNone;
    }
    
void CWSStarServiceSession::AddSecurityTokenToContextL()
    {
    if ( HasSecurity() )
        {
        CSenCredentialProperties& properties = iCredentialPtr.Credential()->PropertiesL();	//codescannerwarnings
        TInt retVal;
        TPtrC8 value;
        if ( iSessionContext )
            {
            retVal = properties.PropertyL(WSStarSession::KPOPBase64LocalName, value);
            if ( retVal == KErrNone )
                {
                User::LeaveIfError(iSessionContext->Add(WSStarContextKeys::KPOPBase64, value));    
                }
            retVal = properties.PropertyL(WSStarSession::KTokenType, value);
            if ( retVal == KErrNone )
                {
                User::LeaveIfError(iSessionContext->Add(WSStarContextKeys::KTokenType, value));    
                }
            retVal = properties.PropertyL(WSStarSession::KBinaryTypeLocalName, value);
            if ( retVal == KErrNone )
                {
                User::LeaveIfError(iSessionContext->Add(WSStarContextKeys::KBinaryType, value));    
                }
            retVal = properties.PropertyL(WSStarSession::KCreatedLocalName, value);
            if ( retVal == KErrNone )
                {
                User::LeaveIfError(iSessionContext->Add(WSStarContextKeys::KTimestampCreated, value));    
                }
            retVal = properties.PropertyL(WSStarSession::KPhoneTimeWhenMTResolvedLocalName, value);
            if ( retVal == KErrNone )
                {
                User::LeaveIfError(iSessionContext->Add(WSStarContextKeys::KPhoneTimeWhenMTResolved, value));    
                }

	        HBufC8* pSecurity = SecurityL();
	        if (pSecurity)
	            {
	            CleanupStack::PushL(pSecurity);
	            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::InitializeFromL - adding token to ctx");

	            retVal = properties.PropertyL(WSStarSession::KTokenType, value);
	            if ( retVal != KErrNone)
	                {
	                value.Set(KNullDesC8);
	                }
	            HBufC8* binarySecurityToken =
	                    SenCryptoUtils::CreateEncodedBinarySecretL(*pSecurity, value);
	            CleanupStack::PopAndDestroy(pSecurity);
	            CleanupStack::PushL(binarySecurityToken);
	            User::LeaveIfError(iSessionContext->Add(
	                    WSStarContextKeys::KSecurityToken,
	                    *binarySecurityToken));  
	            User::LeaveIfError(iSessionContext->Add(
	                    WSStarContextKeys::KSecurityTokenBackup,
	                    *binarySecurityToken));  
	            CleanupStack::PopAndDestroy(binarySecurityToken);  
	            TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts;
	            SenDateUtils::ToXmlDateTimeUtf82L(ts, iValidUntil);
	            User::LeaveIfError(iSessionContext->Add(WSStarContextKeys::KTimestampExpires, ts));    
	            }    
	        }
		}
    }

void CWSStarServiceSession::WriteExtensionsAsXMLToL(RWriteStream& aWriteStream)
    {
    CSenWebServiceSession::WriteExtensionsAsXMLToL(aWriteStream);

    if(iProviderID)
        {
        aWriteStream.WriteL(WSStarSession::KProviderIDTag);
        aWriteStream.WriteL(*iProviderID);
        aWriteStream.WriteL(WSStarSession::KProviderIDEndTag);
        }

    if(iTrustAnchor)
        {
        aWriteStream.WriteL(WSStarSession::KTrustAnchorTag);
        aWriteStream.WriteL(*iTrustAnchor);
        aWriteStream.WriteL(WSStarSession::KTrustAnchorEndTag);
        }
    
    if(iClientServerInterval.Int64() != 0)
        {
        aWriteStream.WriteL(WSStarSession::KServiceInterval);
        TBuf8<64> buf;
        buf.AppendNum(iClientServerInterval.Int64());
        aWriteStream.WriteL(buf);
        aWriteStream.WriteL(WSStarSession::KServiceIntervalEnd);
        }
        
    
    if (iSessionContext)
        {
        if ( iCredentialPtr.Credential() )
            {/*
            CSenCredentialProperties& properties = iCredentialPtr.Credential()->Properties();
            const TDesC8* pValue = iSessionContext->GetDesC8L(WSStarContextKeys::KPOPBase64);
            if ( pValue )
                {
                properties.SetPropertyL(WSStarSession::KPOPBase64LocalName(), *pValue);
                }        
            pValue = iSessionContext->GetDesC8L(WSStarContextKeys::KTokenType);
            if ( pValue )
                {
                properties.SetPropertyL(WSStarSession::KTokenType(), *pValue);
                }        
            pValue = iSessionContext->GetDesC8L(WSStarContextKeys::KBinaryType);
            if ( pValue )
                {
                properties.SetPropertyL(WSStarSession::KBinaryTypeLocalName(), *pValue);
                }        
            pValue = iSessionContext->GetDesC8L(WSStarContextKeys::KSTR);
            if ( pValue )
                {
                properties.SetPropertyL(WSStarSession::KSTRLocalName(), *pValue);
                }                    
            pValue = iSessionContext->GetDesC8L(WSStarContextKeys::KTimestampCreated);
            if ( pValue )
                {
                properties.SetPropertyL(WSStarSession::KCreatedLocalName(), *pValue);
                }                                
            pValue = iSessionContext->GetDesC8L(WSStarContextKeys::KPhoneTimeWhenMTResolved);
            if ( pValue )
                {
                properties.SetPropertyL(WSStarSession::KPhoneTimeWhenMTResolvedLocalName(), *pValue);
                }*/            
            }
        else
            {
            const TDesC8* value = iSessionContext->GetDesC8L(WSStarContextKeys::KPOPBase64);
            if(value)
                {
                aWriteStream.WriteL(WSStarSession::KPOPBase64Tag);
                aWriteStream.WriteL(*value);
                aWriteStream.WriteL(WSStarSession::KPOPBase64EndTag);
                }        
            value = iSessionContext->GetDesC8L(WSStarContextKeys::KTokenType);
            if(value)
                {
                aWriteStream.WriteL(WSStarSession::KTokenTypeTag);
                aWriteStream.WriteL(*value);
                aWriteStream.WriteL(WSStarSession::KTokenTypeEndTag);
                }        
            value = iSessionContext->GetDesC8L(WSStarContextKeys::KBinaryType);
            if(value)
                {
                aWriteStream.WriteL(WSStarSession::KBinaryTypeTag);
                aWriteStream.WriteL(*value);
                aWriteStream.WriteL(WSStarSession::KBinaryTypeEndTag);
                }        
            value = iSessionContext->GetDesC8L(WSStarContextKeys::KSTR);
            if(value)
                {
                aWriteStream.WriteL(WSStarSession::KSTRTag);
                aWriteStream.WriteL(*value);
                aWriteStream.WriteL(WSStarSession::KSTREndTag);
                }                    
            value = iSessionContext->GetDesC8L(WSStarContextKeys::KTimestampCreated);
            if(value)
                {
                aWriteStream.WriteL(WSStarSession::KCreatedTag);
                aWriteStream.WriteL(*value);
                aWriteStream.WriteL(WSStarSession::KCreatedEndTag);
                }                                
            value = iSessionContext->GetDesC8L(WSStarContextKeys::KPhoneTimeWhenMTResolved);
            if(value)
                {
                aWriteStream.WriteL(WSStarSession::KPhoneTimeWhenMTResolvedTag);
                aWriteStream.WriteL(*value);
                aWriteStream.WriteL(WSStarSession::KPhoneTimeWhenMTResolvedEndTag);
                }            
            }
        }
    }
    
TPtrC8 CWSStarServiceSession::ProviderID()
    {
    if(iProviderID)
        return *iProviderID;
    else
        return KNullDesC8();
   
    }
 
void CWSStarServiceSession::SetTrustAnchorL(const TDesC8& aURI)
    {
    delete iTrustAnchor;
    iTrustAnchor = NULL;
    iTrustAnchor = aURI.AllocL();
    }
TPtrC8 CWSStarServiceSession::TrustAnchor()
    {
    if(iTrustAnchor)
        return *iTrustAnchor;
    else
        return KNullDesC8();
    }

TInt CWSStarServiceSession::ComputeStatusL()
    {
    
    TInt retVal = CSenServiceSession::ComputeStatusL();
    TTime createTime;
    //we copy logic from CSenWebServiceSession and add :1) IProviderID condition
    if ((retVal == KSenConnectionStatusReady) && iProviderID)
        {
        TTime now;
        //we dont base of Mobile Time
        //now.UniversalTime(); 
        // now for us is passportTime + X (x = interval time = now - MTObtainedTime)
        
        // Following check is needed, so that *primary* search for credential properties 
        // is performed from the credential (first), context search is secondary 
        // (in practice, compute status call is always performed when we have 
        // credential so this code is always executed). Requirement for this code:
        // => This code is needed, when MT of this session has been expired, but a valid Module Test exists in DB
        // In WebServiceSession layer, there is a search (find) logic, looking for valid Module Tests, after which
        // the super class will call ComputeStatusL() [since WSS cannot know SIF spesific rules for validity].
        if ( HasSecurity() )
            {
            CSenCredentialProperties& properties = iCredentialPtr.Credential()->PropertiesL();	//codescannerwarnings
            
            TPtrC8 createdValue;
            TInt retVal = properties.PropertyL(WSStarSession::KCreatedLocalName, createdValue);
            if ( retVal == KErrNone )
                {
                TTime clientTime;
                TTime mtTime;
                TPtrC8 mtTimetd;
                retVal = properties.PropertyL(WSStarSession::KPhoneTimeWhenMTResolvedLocalName, mtTimetd);
                if ( retVal == KErrNone )
                    {
                    mtTime = SenDateUtils::FromXmlDateTimeL(mtTimetd);
                    clientTime.UniversalTime();
                    if (clientTime != Time::NullTTime())
                        {
                        TTimeIntervalMicroSeconds  diff  = clientTime.MicroSecondsFrom(mtTime);
                        createTime = SenDateUtils::FromXmlDateTimeL(createdValue);
                        now = createTime;
                        if (diff > 0)
                            {
                            now += diff;
                            }
                        }           
                    }
                }
            else
                {
                now.UniversalTime();
                }
            }
        else if ( iSessionContext )
            {
            const TDesC8*  createdValue = iSessionContext->GetDesC8L(WSStarContextKeys::KTimestampCreated);
                     
            if (createdValue)
                {
                TTime clientTime;
                TTime mtTime;
                const TDesC8* mtTimetd = iSessionContext->GetDesC8L(WSStarContextKeys::KPhoneTimeWhenMTResolved);
                if (mtTimetd)
                    {
                    mtTime = SenDateUtils::FromXmlDateTimeL(*mtTimetd);
                    clientTime.UniversalTime();
                    if (clientTime != Time::NullTTime())
                        {
                        TTimeIntervalMicroSeconds  diff  = clientTime.MicroSecondsFrom(mtTime);
                        createTime = SenDateUtils::FromXmlDateTimeL(*createdValue);
                        now = createTime;
                        if (diff > 0)
                            {
                            now += diff;
                            }
                        }           
                    }
                }
            else
                {
                now.UniversalTime();
                }
            }
        else
            {
            //for passport session, (internal session)
            //We wil base on Device time
            now.UniversalTime();
            }
        
#ifdef _SENDEBUG
    TBuf8<SenDateUtils::KXmlDateTimeMaxLength> timeStr;
    TRAPD(leaveCode, SenDateUtils::ToXmlDateTimeUtf82L(timeStr, now);)
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CWSStarServiceSession::computeStatus - now =: %S"), &timeStr));

    TRAP(leaveCode, SenDateUtils::ToXmlDateTimeUtf82L(timeStr, createTime);)
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CWSStarServiceSession::computeStatus - created =: %S"), &timeStr));

    TRAP(leaveCode, SenDateUtils::ToXmlDateTimeUtf82L(timeStr, iValidUntil);)
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CWSStarServiceSession::computeStatus - iValidUntil =: %S"), &timeStr));
#endif
        //HC !!!!!!!!!!!! very tricky things. Microsoft ticket always expires after 10hours event timestamps says sth different
        ///   therefore we simulate 14 hours (so one day ticket 24H -14 gives 10 magic hours)
        /*const TInt KMSUndocumentedMinutes = 60*14;  //nice for testing is 60*23+56
        TTimeIntervalMinutes ticketWindow;
        iValidUntil.MinutesFrom(createTime, ticketWindow);
        if (ticketWindow > TTimeIntervalMinutes(KMSUndocumentedMinutes)) 
            {
            LOG_WRITE((_L("CWSStarServiceSession::computeStatus ticket window is ok")));
            now = now + TTimeIntervalMinutes(KMSUndocumentedMinutes);
            }
        else
            {
            
             LOG_WRITE((_L("CWSStarServiceSession::computeStatus ticket window is too short to cut sth")));
            }
        

#ifdef _SENDEBUG
    TRAP(leaveCode, SenDateUtils::ToXmlDateTimeUtf8L(timeStr, now);)
    if (!leaveCode)
        {
            LOG_WRITEFORMAT((_L8("CWSStarServiceSession::computeStatus + fake MS 10Hours- now =: %S"), &timeStr));
        }
#endif
    */
        //include also margin (3 minutes)
        if (iValidUntil != Time::NullTTime() && 
            now > (iValidUntil - TTimeIntervalMinutes(KClockSlipMinutes)))
            //||
            //(iValidUntil == Time::NullTTime() && 
            //!HasSecurity()))
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarServiceSession::Credential is expired.")));
            retVal = KSenConnectionStatusExpired;
            }
           else
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarServiceSession::Credential is ok.")));
            }
        }
    return retVal;
    } 
    
void CWSStarServiceSession::PrepareOutCtxL(const TDesC8& aTransportProperties)
    {
    iClientTime.UniversalTime();
    
    CSenXmlReader* reader = XmlReader();
    if (iOutContext)
        {
        delete iOutContext;
        iOutContext = NULL;
        }
    iOutContext = CWSStarMessageContext::NewL(SenContext::EOutgoing, reader);
    if (aTransportProperties != KNullDesC8)
        {
        iOutContext->Add(WSStarContextKeys::KWSStarTranspProp,
                aTransportProperties);
        }
    }
  
// This method IS USED only by ASYNC methods (serving session SendL invokations 
// from 3rd party developer via SC) but NOT  by sync methods (like internal consumers)
TInt CWSStarServiceSession::RevalidateMobileTicketIfExpiredL( HBufC8*& aErrorMessage ) // RefreshMTL
//TInt CWSStarServiceSession::RefreshMTL( HBufC8*& aErrorMessage )
    {
    TInt result(KErrNone);
    if (IsExpiredL())
        {
        //we need revalidate
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarServiceSession::RevalidateMobileTicketIfExpiredL - revalidation is required.")));
        //LOG_WRITE((_L("CWSStarServiceSession::RefreshMT - we have to refresh")));
        result = ((CWSStarPlugin&)iFramework).ProcessOutboundValidationL( *this, this, aErrorMessage );
        if ( result == KErrNone )
            {
            // validation succeeded => serialize this session (and its credentials immediately)
            iFramework.Manager().SaveL( *this ); // this will currently *also* serialize associated credential (new/updated MT)
            }
        return result;
        }
    //LOG_WRITE((_L("CWSStarServiceSession::RefreshMT - ticket still valid")));        
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarServiceSession::RevalidateMobileTicketIfExpiredL - ticket is still valid.")));        
    return KErrNone;
    }
    
TBool CWSStarServiceSession::HasSuperClass( TDescriptionClassType aType )
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

TInt CWSStarServiceSession::ShareTokenWithL( CWSStarServiceSession* aWSStarSessionDst, 
                                             TBool& aMTwasReplaceBySCT,
                                             TBool aSeekSCT )
    {
    TInt error(KErrNone);
    if (aSeekSCT)
        {
        const TInt* isR = NULL;
        isR = iSessionContext->GetIntL(WSStarContextKeys::KMTIsReplacedBySCT);
        if (!isR || (isR && *isR == FALSE)) return KErrNotFound;
        }
    if ( !HasSecurity() )//if not MT than SCT for sure doesnt exist
        {
        return KErrNotFound;
        }
    //share MT (only not expired), otherway it is sensless
    //dont call SetStatus/IsExpired becouse it will infornm consumer/Core about chnge. 
    //Session will not be able to revalidate by itself in next sending
    if (ComputeStatusL() != KSenConnectionStatusExpired)
        {
/*#ifdef _SENDEBUG
        _LIT8(KCredsLogLine, "Token sharing FROM (%S) TO (%S)");
        CREDLOG_FORMAT((KSenCredsLogChannel, KSenCredsLogLevelNormal, KCredsLogLine,  &Endpoint(), &(aWSStarSessionDst->Endpoint())));
#endif  */
        //share possible SCT are already in ctx
        error = iSessionContext->ShareTokenWithL(
                aWSStarSessionDst->SessionContext(), aMTwasReplaceBySCT);        
                
        //HBufC8* pSecurity = SecurityL();
        //CleanupStack::PushL(pSecurity);
        //aWSStarSessionDst->AddCredentialL(*pSecurity, iValidUntil);//this also recalculate state
        aWSStarSessionDst->AddCredentialL(iCredentialPtr, iValidUntil);//this also recalculate state
        //CleanupStack::PopAndDestroy(pSecurity);
        return error;
        }
    else
        {
        return KErrNotFound;
        }
    }
    
void CWSStarServiceSession::AddCredentialL( const TDesC8& aSecurity, TTime aValidUntil )
    {
    iValidUntil = aValidUntil;
    SetSecurityL(aSecurity);
    SetStatusL();
    }
    
void CWSStarServiceSession::AddCredentialL(RSenCredentialPtr aCredentialPtr, TTime aValidUntil)
    {
    iValidUntil = aValidUntil;
    SetCredentialPtrL(aCredentialPtr); // Share same Credential between multiple Sessions	//codescannerwarnings
    SetStatusL();
    }
    
TBool CWSStarServiceSession::AmIHostletSession()
    {
    if(iTransportCue && (KSenTransportCueHostletConnection() == *iTransportCue))
        {
        return ETrue;
        }
    return EFalse;
    }
    
void CWSStarServiceSession::FindAndShareSCTL()
    {
    
    if (!iProviderID)
        {
        return;
        }

    //1)  check if we have already SCT
    const TInt* isR = iSessionContext->GetIntL(WSStarContextKeys::KMTIsReplacedBySCT);
    if (isR && *isR == TRUE) return;//SCT already exist so we dont have to share    
    
    //2) real sharing
    HBufC8* clusterUsed = CWSStarPolicy::GetPolicyValueL(WSPolicy::KIssuedToken, WSPolicy::KRPSCluster, (CWSStarPlugin&)iFramework, this);
    TInt retVal(KErrNone);
    //its mean - if our provider uses cluster template, 
    //we can SCT form other msn services which use same provider ID
    if (clusterUsed)
        {
        RPointerArray<CSenWSDescription> msnSDs;
        CleanupClosePushL(msnSDs);
        CSenWSPattern* pattern = CSenWSPattern::NewLC();
     
        //select * from session_of_SD where session is (ws* framework, and has same provideId)
        //here we assume that provider use cluster template
        
        pattern->SetFrameworkIdL(KDefaultWSStarFrameworkID);
        pattern->AsElement().AddElementL(KSenIdpProviderIdLocalname).SetContentL(*iProviderID);
        //pattern->AsElement().AddElementL(WSStarSession::KProviderIdElementLocalName).SetContentL(*iProviderID);
        //seeking
        CWSStarServiceSession* msnSession = NULL;
        TRAPD(leaveError, retVal = ((CWSStarPlugin&)iFramework).Manager().ServiceDescriptionsL(msnSDs, *pattern));
        //      Becouse they are registered without framework Id (DAO keep sessions, but in client case only SD -> see registering STSClient, DSClient..)
        //      SD matches only by enpoint and contract
        if(!retVal && !leaveError)
            {
            TInt count = msnSDs.Count();
            TBool pMTwasReplaceBySCT = EFalse;
            //if MT has been shared already than it happened during validation
            // now we just looking for SCT.
            //usecase:
            //      -----SC1::new
            //      -----SC2:new  (MT sharing inside trusat client
            //      -----SC1::send  (SCT is only inside SC1)
            //      -----SC2::send  (now we can share SCT)
            
            TBool seekOnlySCT = ETrue;
            for(TInt i = 0; i < msnSDs.Count(); i++)
                {
                //all sessions (so in practice we have also access to SCT, not only MT),
                // to eliminate client, just for case we check type
                TDescriptionClassType dt = msnSDs[i]->DescriptionClassType();
                if (dt == MSenServiceDescription::EWSStarServiceSession)
                    {
                    msnSession = (CWSStarServiceSession*)msnSDs[i];
                    TRAPD(err, retVal = msnSession->ShareTokenWithL(
                            this, pMTwasReplaceBySCT,
                            seekOnlySCT));
                    if ((retVal == KErrNone) && (err == KErrNone))
                        {
                        if (pMTwasReplaceBySCT)
                            {
                            break;    
                            }
                        }
                    }
                }
            }
        CleanupStack::PopAndDestroy(2, &msnSDs);
        }
    delete clusterUsed;
    }

TInt CWSStarServiceSession::UpdatePolicyL( CWSStarPolicyHandler* aPolicyHandler, MSenServiceDescription* aSD )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::UpdatePolicyL");
    CWSStarSessionContext* pCtx = SessionContext();
    if( !pCtx )
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"CWSStarServiceSession::UpdatePolicyL: - Attempting to create new session context");
        // In the future => session context exists BEFORE the session even has been created(!)
        CSenXmlReader* reader = XmlReader();
        iSessionContext = CWSStarSessionContext::NewL(reader, aSD, aPolicyHandler);        
        pCtx = SessionContext();
        
        if( !pCtx )
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::UpdatePolicyL: - FATAL: unable to create new session context(!).");
            return KErrSenInternal; // very unlikely oom case (that did not leave!) / construction of ctx oddly failed
            }
        }
    return pCtx->UpdatePolicyL(aPolicyHandler, aSD); 
    }
    
void CWSStarServiceSession::ParseToSoapMessage2L( const TDesC8& aSoapMessage, MSenRemoteServiceConsumer& aConsumer, TInt& aTxnId )
    {
    TInt ctxLookupErr(KErrNone);
    CSenSoapMessageDom2* soapMessage2 = NULL;
    MSenMessageContext* pCtx = aConsumer.MessageContextByTxnIdL( aTxnId, ctxLookupErr );
    if( ctxLookupErr == KErrNone && pCtx )
        {
        soapMessage2 = (CSenSoapMessageDom2*)pCtx->MessageL();		//codescannerwarnigs
        }

    if (!soapMessage2)
        {
        return;
        }
    CSenSoapMessageDom2* newMessage2 = CSenSoapMessageDom2::NewLC();
    CSenParser* parser = CSenParser::NewLC();
    parser->EnableFeature(EReportNamespaceMapping);
    parser->ParseL(aSoapMessage, *newMessage2); 
    CleanupStack::PopAndDestroy(parser);

    _LIT8(KCidPrefix, "cid:*");
    RArray<TXmlEngElement> binaryElements;
   	RXmlEngNodeList<TXmlEngAttr> attrArray;
   	CleanupClosePushL(binaryElements);
    TXmlEngElement rootElement = newMessage2->AsElementL();
    TInt countEl = ListBinaryElementsL(binaryElements,rootElement);        
    if (countEl == 0)
        {
        CleanupStack::PopAndDestroy(&binaryElements);
        CleanupStack::Pop(newMessage2);
        pCtx->SetMessage(newMessage2,ETrue);
        return;
        }
    CSLOG_FORMAT((aConsumer.ConnectionId(), KMinLogLevel , _L8("- Count of <xop:Include elements> in DOM tree: %d"), countEl));
    

    TXmlEngAttr attr;
    TPtrC8 cid;

    RSenDocument document = newMessage2->AsDocumentL();
    RSenDocument doc = soapMessage2->AsDocumentL();
    RArray<TXmlEngDataContainer> dataContainerArray;
    CleanupClosePushL(dataContainerArray);
    doc.GetDataContainerList(dataContainerArray);
    TInt countDC(dataContainerArray.Count());

 	
  	TXmlEngElement element;
  	if ( countEl > 0 && countDC > 0 )
	    {
		for ( TInt i = 0; i < countEl; i++ )
		    {
			element = binaryElements[i];
			element.GetAttributes(attrArray);
			while ( attrArray.HasNext() )
				{
				attr = attrArray.Next();
				TPtrC8 value = attr.Value();
            	TInt position = value.Match(KCidPrefix);
            	if ( position < 0 )
            		{
            		position = 0;
            		}
            	cid.Set(value.Mid( position + KCidPrefix().Size() - 1)); // minus star character (*)
            	if ( cid.Length() > 0 )
            	    {
            	    break;
            	    }
				}
		    attrArray.Close();
			
			for ( TInt j = 0; j < countDC; j++ )
			    {
			    TXmlEngDataContainer dataContainer = dataContainerArray[j];
                if ( dataContainer.Cid() == cid )
                    {
            		if ( dataContainer.NodeType() == TXmlEngNode::EChunkContainer)
            			{
                	    TXmlEngChunkContainer binData =
                	            document.CreateChunkContainerL(
                	                        dataContainer.Cid(),
                	                        dataContainer.AsChunkContainer().Chunk(),
                	                        dataContainer.AsChunkContainer().ChunkOffset(),
                	                        dataContainer.AsChunkContainer().Size() );
                        element.ReplaceWith(binData);
            			}
            		else if ( dataContainer.NodeType() == TXmlEngNode::EFileContainer)
            			{
                	    TXmlEngFileContainer binData = 
                	            document.CreateFileContainerL(
                	                        dataContainer.Cid(),
    	                                    dataContainer.AsFileContainer().File() );
            			
                        element.ReplaceWith(binData);
            			}

                    break;
                    }
			    }
		    }
		}        
    CleanupStack::PopAndDestroy(2, &binaryElements);
    CleanupStack::Pop(newMessage2);
    pCtx->SetMessage(newMessage2,ETrue);
    }
    
TInt CWSStarServiceSession::ListBinaryElementsL( RArray<TXmlEngElement>& aElementArray,
                                                 TXmlEngElement& aElement )
    {
//    TLSLOG_L(KSenCoreServiceManagerLogChannelBase, KMinLogLevel,"CWSStarServiceSession::ListBinaryElementsL"); // logged *way* TOO often! => commented out
	_LIT8(KInclude,         "Include");
	_LIT8(KXopIncludeUri,   "http://www.w3.org/2004/08/xop/include");
	
	TInt count(0);
	RXmlEngNodeList<TXmlEngElement> list;
	CleanupClosePushL(list);
	aElement.GetChildElements(list);
	TInt c = list.Count();

#ifdef _SENDEBUG
    TPtrC8 root = aElement.Name();
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("- Processing element: '%S', num of child elements: %d"), &root, c));
    TPtrC8 content = aElement.Text();
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"----  Content START ----");
    TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(content));
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"----  Content END ----");
#endif // _SENDEBUG

	

    while ( list.HasNext() )
        {
        TXmlEngElement element = list.Next();
        
        TPtrC8 tag = element.Name();
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel, _L8("- child: '%S'"), &tag));
        if ( tag == KInclude )
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"- <xop:Include> element found.");
            if ( element.NamespaceUri() == KXopIncludeUri )
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,"- namespace == http://www.w3.org/2004/08/xop/include");
                aElementArray.AppendL(element);
                count = count + 1;
                }
            }
        count = count + ListBinaryElementsL(aElementArray, element);
        }

    CleanupStack::PopAndDestroy(&list);
    return count;
    }

void CWSStarServiceSession::FillCredentialIdentifierL(CSenCredentialIdentifier& aIdentifier)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::FillCredentialIdentifierL:");
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
    
void CWSStarServiceSession::VerifyPermissionL()
    {
    if( HasSecurity() )
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::VerifyPermissionL:");
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
    
CSenIdentityProvider* CWSStarServiceSession::LookupIdentityProviderL()
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarServiceSession::LookupIdpL()");
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
     
    
void CWSStarServiceSession::AddPropertiesFromSessionContextToCredentialL()
    {
    if (iSessionContext)
        {
        if ( iCredentialPtr.Credential() )
            {
            CSenCredentialProperties& properties = iCredentialPtr.Credential()->PropertiesL();	//codescannerwarnings
            const TDesC8* pValue = iSessionContext->GetDesC8L(WSStarContextKeys::KPOPBase64);
            if ( pValue )
                {
                properties.SetPropertyL(WSStarSession::KPOPBase64LocalName(), *pValue);
                }        
            pValue = iSessionContext->GetDesC8L(WSStarContextKeys::KTokenType);
            if ( pValue )
                {
                properties.SetPropertyL(WSStarSession::KTokenType(), *pValue);
                }        
            pValue = iSessionContext->GetDesC8L(WSStarContextKeys::KBinaryType);
            if ( pValue )
                {
                properties.SetPropertyL(WSStarSession::KBinaryTypeLocalName(), *pValue);
                }        
            pValue = iSessionContext->GetDesC8L(WSStarContextKeys::KSTR);
            if ( pValue )
                {
                properties.SetPropertyL(WSStarSession::KSTRLocalName(), *pValue);
                }                    
            pValue = iSessionContext->GetDesC8L(WSStarContextKeys::KTimestampCreated);
            if ( pValue )
                {
                properties.SetPropertyL(WSStarSession::KCreatedLocalName(), *pValue);
                }                                
            pValue = iSessionContext->GetDesC8L(WSStarContextKeys::KPhoneTimeWhenMTResolved);
            if ( pValue )
                {
                properties.SetPropertyL(WSStarSession::KPhoneTimeWhenMTResolvedLocalName(), *pValue);
                }            

            // Now, serialize this session (and all it's updated credentials) into database(s)
            iFramework.Manager().SaveL( *this ); // this will currently *also* serialize associated credential (new/updated MT)                
            }
        }
    }
    
TBool CWSStarServiceSession::HasEqualPrimaryKeysL( MSenServiceDescription& aCandidate )
    {
    TBool retVal(EFalse);
    if ( CSenWSDescription::HasEqualPrimaryKeysL(aCandidate) )
        {
        // Endpoint, Contract and FrameworkID match, all of them.
        // Now, in WS-*, also check ProviderID
        if (aCandidate.DescriptionClassType() == DescriptionClassType())
            {
            CWSStarServiceSession& sessionCandidate = (CWSStarServiceSession&)aCandidate;
            if (sessionCandidate.ProviderID() == ProviderID())
                {
                retVal = ETrue;
                }
            }
        }
    return retVal; 
    }
    
void CWSStarServiceSession::CredentialChanged( TSenCredentialChange aChange,
		                                       TAny* apPointer )
    {
    if ( aChange == MSenCredentialObserver::EDestroyed && iSessionContext )
        {
        ClearCredentialPropertiesFromContext();
        }
    CSenWebServiceSession::CredentialChanged( aChange, apPointer );
    }


void CWSStarServiceSession::ClearCredentialPropertiesFromContext()
    {
    if( iSessionContext )
        {
        iSessionContext->Remove(WSStarContextKeys::KPOPBase64);
        iSessionContext->Remove(WSStarContextKeys::KTokenType);
        iSessionContext->Remove(WSStarContextKeys::KBinaryType);
        iSessionContext->Remove(WSStarContextKeys::KSTR);
        iSessionContext->Remove(WSStarContextKeys::KTimestampCreated);
        iSessionContext->Remove(WSStarContextKeys::KPhoneTimeWhenMTResolved);
        iSessionContext->Remove(WSStarContextKeys::KMTIsReplacedBySCT);    
        iSessionContext->Remove(WSStarContextKeys::KSecurityToken);    
        }
    //CSenWebServiceSession::CredentialChanged(aChange, apPointer);
    }

void CWSStarServiceSession::ActiveTicketObserverL()
    {
    const TDesC8*  createdValue = iSessionContext->GetDesC8L(WSStarContextKeys::KTimestampCreated);
    const TDesC8*  expiresValue = iSessionContext->GetDesC8L(WSStarContextKeys::KTimestampExpires);
    const TDesC8*  phoneTimeValue = iSessionContext->GetDesC8L(WSStarContextKeys::KPhoneTimeWhenMTResolved);

    TTimeIntervalMinutes margin(KClockSlipMinutes - 1);              
    if (createdValue && expiresValue  && phoneTimeValue)
        {
        TTime expTime, creTime, phTime, nowTime;
        creTime = SenDateUtils::FromXmlDateTimeL(*createdValue);
        expTime = SenDateUtils::FromXmlDateTimeL(*expiresValue);
        phTime = SenDateUtils::FromXmlDateTimeL(*phoneTimeValue);
        
        creTime += margin;
        TTimeIntervalMicroSeconds  diff = expTime.MicroSecondsFrom(creTime);   
        
#ifdef _SENDEBUG        
        TBuf8<64> buf;                              
        buf.Num( diff.Int64() );
        TLSLOG_FORMAT(( KSenCoreServiceManagerLogChannelBase, KNormalLogLevel, _L8("CWSStarServiceSession::ActiveTicketObserverL - range %S microsec"), &buf ));
#endif // _SENDEBUG        
        
        nowTime.UniversalTime();
        if (nowTime != Time::NullTTime())
            {
            TTimeIntervalMicroSeconds  diff2 = nowTime.MicroSecondsFrom( phTime ); // already missed time
#ifdef _SENDEBUG        
            buf.Num( diff2.Int64() );
            TLSLOG_FORMAT(( KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CWSStarServiceSession::ActiveTicketObserverL - already pased %S microsec"), &buf ));
#endif // _SENDEBUG        
            diff = TTimeIntervalMicroSeconds( diff.Int64() - diff2.Int64() ); // Counting the real difference
            }
        
        if (diff > 0) 
            {
            iTicketObs->Start( diff );//nowTime + diff);
#ifdef _SENDEBUG        
            buf.Num(diff.Int64());
            TLSLOG_FORMAT(( KSenCoreServiceManagerLogChannelBase, KNormalLogLevel, _L8("CWSStarServiceSession::ActiveTicketObserverL - iTicketObs->Start(%S) microsec"), &buf ));
            TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts;
            SenDateUtils::ToXmlDateTimeUtf8L(ts, nowTime + diff);
            TLSLOG_FORMAT(( KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CWSStarServiceSession::ActiveTicketObserverL - START time %S "), &ts ));
#endif            
            }
#ifdef _SENDEBUG        
        else
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase, KMinLogLevel, "CWSStarServiceSession::ActiveTicketObserverL - SKIP - ticket expired already we don't need to wait");
            // ==> Validation phase will soon executed and new timer WILL BE activated
            }
#endif            
        }
        
    }
// End of file
