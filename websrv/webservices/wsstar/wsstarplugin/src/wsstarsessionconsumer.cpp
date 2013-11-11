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
#include <SenXmlUtils.h>

#include "wsstarsessionconsumer.h"
#include "sendebug.h"
#include <SenServiceConnection.h>
#include "senlogger.h"
#include "wsstarcons.h"

CWSStarSessionConsumer* CWSStarSessionConsumer::NewL( MSenRemoteServiceConsumer& aServiceConsumer,
                                                      RFileLogger& aLogger )
    {
    CWSStarSessionConsumer* pSelf =
        CWSStarSessionConsumer::NewLC(aServiceConsumer, aLogger);
    CleanupStack::Pop(pSelf);
    return pSelf;
    }

CWSStarSessionConsumer* CWSStarSessionConsumer::NewLC( MSenRemoteServiceConsumer& aServiceConsumer,
                                                       RFileLogger& aLogger )
    {
    CWSStarSessionConsumer* pSelf =
                        new (ELeave) CWSStarSessionConsumer( aServiceConsumer, aLogger );
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;
    }

CWSStarSessionConsumer::~CWSStarSessionConsumer()
    {
    //delete iSentMessageId;
    //delete iReceivedMessageId;
    iSentMessages.Reset();
    iSendReplyTo.Reset();
    iTranspProp.Reset();
    }

CWSStarSessionConsumer::CWSStarSessionConsumer( MSenRemoteServiceConsumer& aServiceConsumer,
                                                RFileLogger& aLogger )
:   iConsumer(aServiceConsumer),
    iLogger(aLogger),
    //iSentMessageId(NULL),
    //iReceivedMessageId(NULL),
    iSentMessages(ETrue, ETrue),
    iSendReplyTo(ETrue, ETrue),
    iTranspProp(ETrue, ETrue)
    {
    }


const TDesC8& CWSStarSessionConsumer::Id() const
    {
    return iConsumer.Id();
    }

TInt CWSStarSessionConsumer::SetSessionL(MSenRemoteServiceSession& aServiceSession)
    {
    iConsumer.SetSessionL(aServiceSession);
    return KErrNone;
    }

void CWSStarSessionConsumer::HandleSoapMessageL( CSenSoapMessage* apMessage,
                                                 const TInt aTxnId,
                                                 MSenProperties* aResponseTransportProperties,
                                                 TBool aCompleteMessage )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarSessionConsumer::HandleSoapMessageL(CSenSoapMessage*)");
    
    if ( apMessage )
        {
        CleanupStack::PushL( apMessage ); // this method has the ownership, until passed forward(!)
        
        // fix for the error HTUR-6XDABN : Start
        
    	// After sending the message to Remote consumer, Corresponding message is not 
    	// removed from the message queue. Due to this the message body sits in the 
    	// queue (memory) till the session consumer object gets destroyed. 
    	CSenElement& header = apMessage->HeaderL();
    	// Response RelatesTo content=MessageId of the request
        RPointerArray<CSenElement>& msgIdArray = header.ElementsL();
    
    	for(TInt i = 0; i < msgIdArray.Count(); i++)
    		{
    		CSenElement& idElement = *msgIdArray[i];
    		
    		if( idElement.LocalName() == WSStarContextKeys::KRelatesTo() )
    			{
            	// Remove the consumed message from the message queue 
            	iSentMessages.RemoveByKey( idElement.Content() );
            	iTranspProp.RemoveByKey( idElement.Content() ); 		
            	break;
    			}
    		}
    		
    	// msgIdArray.Close(); // Must be removed (now commented out) OR otherwise regression 
    	                       // (MMAA-6YFQ5V) will occur! The msgIdArray is a *reference*, 
    	                       // and any modification to this array will MODIFY the actual
    	                       // contents of that DOM tree(!). 
    	                         
    	// fix for the error HTUR-6XDABN : End
    	
        HBufC8* pMessageAsString = NULL;
        if (aCompleteMessage)
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- Returning whole SOAP-ENV");
            pMessageAsString = apMessage->AsXmlL();
            }
        else
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- Detaching SOAP body (calling BodyAsStringL).");
            pMessageAsString = apMessage->BodyAsStringL();
            }
        CleanupStack::PopAndDestroy( apMessage ); // object has been serialized into a descriptor => de-alloc it
        
        if (pMessageAsString)
            {
            iConsumer.HandleMessageL(pMessageAsString, aTxnId, aResponseTransportProperties);
            }
        else
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"- Fatal(!): pMessageAsString == NULL, invoking remote consumer's HandleErrorL.");
            iConsumer.HandleErrorL(NULL, KErrSenNoSoapBody, aTxnId, aResponseTransportProperties);
            }
        }
    else // apMessage == NULL
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel,"- Fatal(!): WSStarSessionConsumer::HandleSoapMessageL, apMessage == NULL, sending KErrSenInternal to remote consumer via HandleErrorL.");
        iConsumer.HandleErrorL(NULL, KErrSenInternal, aTxnId, aResponseTransportProperties);
        }           
    }

TInt CWSStarSessionConsumer::HandleMessageL( HBufC8* apMessage,
                                             const TInt aTxnId,
                                             MSenProperties* aResponseTransportProperties )
    {
#ifdef _SENDEBUG
    CleanupStack::PushL(apMessage);
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarSessionConsumer::HandleMessageL");
    CleanupStack::Pop(apMessage);
#endif // _SENDEBUG    
    return iConsumer.HandleMessageL(apMessage, aTxnId, aResponseTransportProperties);
    }

TInt CWSStarSessionConsumer::HandleErrorL( HBufC8* apError,
                                           const TInt aErrorCode,
                                           const TInt aTxnId,
                                           MSenProperties* aResponseTransportProperties )
    {
#ifdef _SENDEBUG
    CleanupStack::PushL(apError);
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarSessionConsumer::HandleErrorL");
    CleanupStack::Pop(apError);
#endif // _SENDEBUG    
    return iConsumer.HandleErrorL(apError, aErrorCode, aTxnId, aResponseTransportProperties);
    }

void CWSStarSessionConsumer::SetStatusL(const TInt status)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarSessionConsumer::SetStatusL");
    //we will not inform consumers about  fact that credential is expired, 
    // ws* has to obtain new ticket and keep connection valid
    if (status == KSenConnectionStatusExpired) 
        {
        iConsumer.SetStatusL(KSenConnectionStatusReady);    
        }
    else
        {
        iConsumer.SetStatusL(status);
        }
    }

TInt CWSStarSessionConsumer::MessageForSendingL(const TDesC8& aBody,
                                           CWSStarMessageContext*& aCtx)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarSessionConsumer::MessageForSendingL");
    
    //do nothing with message. Message is preapred by session and handlers
    //we need only set correlation id using context which travel with message
    //------------SCHarF ------- glue with addressing to get messageId. We will expect some answer
    const TDesC8* msgId = aCtx->GetDesC8L(WSStarContextKeys::KMessageID);
    const TDesC8* replyTo = aCtx->GetDesC8L(WSStarContextKeys::KReplyTo);
    if (msgId)
        {
        iSentMessages.Append(msgId->AllocL(), aBody.AllocL());    
        if(replyTo)
            {
            iSendReplyTo.Append(msgId->AllocL(), replyTo->AllocL());
            }
        const TDesC8* transpProp =
                aCtx->GetDesC8L(WSStarContextKeys::KWSStarTranspProp);
        if (transpProp)
            {
            iTranspProp.Append(msgId->AllocL(), transpProp->AllocL());
            }
        }
    //------------end SCHarF ------- 
    
    return KErrNone;
    }

void CWSStarSessionConsumer::ConstructL()
    {
    }

/*void CWSStarSessionConsumer::SetSentMessageIdL(const TDesC8& aSentMessageId)
    {
    HBufC8* pNew = NULL;
    if(aSentMessageId.Length()>0)
        pNew = aSentMessageId.AllocL();
    delete iSentMessageId;
    iSentMessageId = pNew;
    }

TPtrC8 CWSStarSessionConsumer::SentMessageId()
    {
    if(iSentMessageId)
        {
        return *iSentMessageId;
        }
    else
        {
        return KNullDesC8();
        }
    }

void CWSStarSessionConsumer::SetReceivedMessageIdL(
                                            const TDesC8& aReceivedMessageId)
    {
    HBufC8* pNew =NULL;
    if(aReceivedMessageId.Length()>0)
        pNew = aReceivedMessageId.AllocL();
    delete iReceivedMessageId;
    iReceivedMessageId = pNew;
    }

TPtrC8 CWSStarSessionConsumer::ReceivedMessageId()
    {
    if(iReceivedMessageId)
        return *iReceivedMessageId;
    else
        return KNullDesC8();
    }
*/
TBool CWSStarSessionConsumer::Expects(const TDesC8& aRelatesTo, const TDesC8* aTo)
    {
    TInt index = iSentMessages.Find(aRelatesTo);
    TBool result(EFalse);
    TInt index2 = index; 
    if (aTo)
        index2 = iSendReplyTo.Find(aRelatesTo);
    
    if (index == index2 && index2 != KErrNotFound)
        {
        if (aTo)
            {
            const TDesC8* replyTo = iSendReplyTo.ValueAt(index);
            if (*replyTo == *aTo)
                {
                result = ETrue;
                }
            else
                {
                TLSLOG(KSenFaultsLogChannel, KSenFaultsLogLevel, WSStarSessionConsumer::KFaultInvalidToTag);   
                }
            }
        else
            {
            result = ETrue;
            }
        }
    else
        {
        TLSLOG(KSenFaultsLogChannel, KSenFaultsLogLevel, WSStarSessionConsumer::KFaultInvalidRelatesToTag);   
        }

    return result;
    }

TDesC8* CWSStarSessionConsumer::BodyL(const TDesC8& aMessageID) const
    {
    HBufC8* pBody = NULL;

    TInt index = iSentMessages.Find(aMessageID);
    if (index != KErrNotFound)
        {
        pBody = (HBufC8*)iSentMessages.ValueAt(index);
        }
    return pBody;
    }

const TDesC8& CWSStarSessionConsumer::TransportProperties(
        const TDesC8& aMessageID) const
    {
    TInt index = iTranspProp.Find(aMessageID);
    if (index != KErrNotFound)
        {
        const TDesC8* tp = iTranspProp.ValueAt(index);
        if (tp)
            {
            return *tp;
            }
        }
    return KNullDesC8;
    }

MSenTransport& CWSStarSessionConsumer::TransportL()
    {
    return iConsumer.TransportL();
    }
    
TInt CWSStarSessionConsumer::ChunkByTxnIdL(TInt aTxnId, CSenChunk*& aChunk)
    {
    return iConsumer.ChunkByTxnIdL(aTxnId, aChunk);
    }

void CWSStarSessionConsumer::FileProgress(TInt /*aTxnId*/, TBool /*aIncoming*/, TBool /*aIsSoap*/,
                const TDesC8& /*aSoapOrCid*/, TInt /*aProgress*/)
    {
    }

CSenIdentifier& CWSStarSessionConsumer::Identifier() const
    {
    return iConsumer.Identifier();
    }


// enable file logging (macros)
RFileLogger* CWSStarSessionConsumer::Log() const
    {
    return (RFileLogger*) &iLogger;
    }
    
TBool CWSStarSessionConsumer::HasAuthenticationCallback()
    {
    return iConsumer.HasAuthenticationCallback();
    }

TInt CWSStarSessionConsumer::ReauthenticationNeededL(CSenIdentityProvider*& aIdp)
	{
	return iConsumer.ReauthenticationNeededL(aIdp);
	}
void CWSStarSessionConsumer::SetDataTrafficDetails( TSenDataTrafficDetails&  aDetails)  
	{
	iConsumer.SetDataTrafficDetails(aDetails);
	}

TBool CWSStarSessionConsumer::HasCoBrandingCallback()
    {
    return iConsumer.HasCoBrandingCallback();
    }

TBool CWSStarSessionConsumer::OnGetBrandIdL(  const TDesC8& aBrandIdListAsXmlSnippet, RBuf8& aSelectedBrandId)
    {
    return iConsumer.OnGetBrandIdL(aBrandIdListAsXmlSnippet,aSelectedBrandId);
    }
// End of file
