/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
#include <SenServiceConnection.h>


#include "idwsfsessionconsumer.h"
#include "idwsfmessage.h"
#include "sendebug.h"
#include "senlogger.h"

#include <flogger.h>

CIdWsfSessionConsumer* CIdWsfSessionConsumer::NewL(MSenRemoteServiceConsumer& aServiceConsumer,
                                                   RFileLogger& aLogger)
    {
    CIdWsfSessionConsumer* pSelf =
        CIdWsfSessionConsumer::NewLC(aServiceConsumer, aLogger);
    CleanupStack::Pop(pSelf);
    return pSelf;
    }

CIdWsfSessionConsumer* CIdWsfSessionConsumer::NewLC(MSenRemoteServiceConsumer& aServiceConsumer,
                                                    RFileLogger& aLogger)
    {
    CIdWsfSessionConsumer* pSelf =
                        new (ELeave) CIdWsfSessionConsumer(aServiceConsumer, aLogger);
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;
    }

CIdWsfSessionConsumer::~CIdWsfSessionConsumer()
    {
    delete iSentMessageId;
    delete iReceivedMessageId;
    iSentMessages.Reset();
    }

CIdWsfSessionConsumer::CIdWsfSessionConsumer(MSenRemoteServiceConsumer& aServiceConsumer,
                                             RFileLogger& aLogger)
:   iConsumer(aServiceConsumer),
    iLogger(aLogger),
    iSentMessageId(NULL),
    iReceivedMessageId(NULL),
    iSentMessages(ETrue, ETrue)
    {
    }


const TDesC8& CIdWsfSessionConsumer::Id() const
    {
    return iConsumer.Id();
    }

TInt CIdWsfSessionConsumer::SetSessionL(MSenRemoteServiceSession& aServiceSession)
    {
    iConsumer.SetSessionL(aServiceSession);
    return KErrNone;
    }

TInt CIdWsfSessionConsumer::HandleIdWsfMessageL( CIdWsfMessage* apMessage,
                                                 const TInt aTxnId,
                                                 MSenProperties* aResponseTransportProperties,
                                                 TBool aCompleteMessage )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfSessionConsumer::HandleIdWsfMessageL");
    
    TInt retVal(KErrNone);
    if ( apMessage )
        {
        CleanupStack::PushL( apMessage ); // ownership is here
    
        iSentMessages.RemoveByKey( apMessage->MessageId() );
    
        HBufC8* pMessageAsString = NULL;
        
    	//TSW ID: EMKY-6S4CGS & ID:EMKY-6N3AGA
    	// Check if aCompleteMessage in ON then get entire envelope or else only body
        if ( aCompleteMessage )
            {
            pMessageAsString = apMessage->AsXmlL();
            }
        else
            {
            pMessageAsString = apMessage->BodyAsStringL();
            }
        CleanupStack::PopAndDestroy( apMessage ); // object serialized to descriptor, de-alloc!
    
        if( pMessageAsString )
            {
            retVal = iConsumer.HandleMessageL(pMessageAsString, aTxnId, aResponseTransportProperties);
            }
        else
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfSessionConsumer::HandleIdWsfMessageL:");
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"- Fatal(!): pMessageAsString == NULL, calling remote consumer's HandleErrorL with KErrSenNoSoapBody");
            retVal = iConsumer.HandleErrorL(NULL, KErrSenNoSoapBody, aTxnId, aResponseTransportProperties);
            }
        }
    else // apMessage == NULL
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfSessionConsumer::HandleIdWsfMessageL:");
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"- Fatal(!): apMessage == NULL, calling remote consumer's HandleErrorL with KErrSenInternalError");
        retVal = iConsumer.HandleErrorL( NULL, KErrSenInternal, aTxnId, aResponseTransportProperties);
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,_L("CIdWsfSessionConsumer::HandleIdWsfMessageL - HandleErrorL returned: %d. Returning KErrSenInternalError in ANY case."), retVal));
        retVal = KErrSenInternal;            
        }        
    return retVal;
    }

TInt CIdWsfSessionConsumer::HandleMessageL( HBufC8* apMessage,
                                            const TInt aTxnId,
                                            MSenProperties* aResponseTransportProperties ) 
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfSessionConsumer::HandleMessageL - passing message to consumer");
    return iConsumer.HandleMessageL( apMessage, aTxnId, aResponseTransportProperties );
    }

TInt CIdWsfSessionConsumer::HandleErrorL( HBufC8* apError,
                                          const TInt aErrorCode,
                                          const TInt aTxnId,
                                          MSenProperties* aResponseTransportProperties )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfSessionConsumer::HandleErrorL - passing error to consumer");
    return iConsumer.HandleErrorL( apError, aErrorCode, aTxnId, aResponseTransportProperties );
    }

void CIdWsfSessionConsumer::SetStatusL(const TInt status)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfSessionConsumer::SetStatusL");
    iConsumer.SetStatusL(status);
    }

TInt CIdWsfSessionConsumer::MessageForSendingL( const TDesC8& aBody,
                                                const TDesC8& aReceivedMessageId,
                                                CIdWsfMessage*& apMessage )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfSessionConsumer::MessageForSendingL");
    if(aReceivedMessageId.Length()>0)
        {
        apMessage = CIdWsfMessage::NewL(aReceivedMessageId);
        }
    else
        {
        apMessage = CIdWsfMessage::NewL();
        }

    apMessage->SetBodyL(aBody);
    iSentMessages.Append(apMessage->MessageId().AllocL(), aBody.AllocL());
    return KErrNone;
    }

void CIdWsfSessionConsumer::ConstructL()
    {
    }

void CIdWsfSessionConsumer::SetSentMessageIdL(const TDesC8& aSentMessageId)
    {
    HBufC8* pNew = NULL;
    if(aSentMessageId.Length()>0)
    pNew = aSentMessageId.AllocL();    
    delete iSentMessageId;
    iSentMessageId = pNew;
    }

TPtrC8 CIdWsfSessionConsumer::SentMessageId()
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

void CIdWsfSessionConsumer::SetReceivedMessageIdL(
                                            const TDesC8& aReceivedMessageId)
    {
    HBufC8* pNew =NULL;
    if(aReceivedMessageId.Length()>0)
        pNew = aReceivedMessageId.AllocL();
    delete iReceivedMessageId;
    iReceivedMessageId = pNew;
    }

TPtrC8 CIdWsfSessionConsumer::ReceivedMessageId()
    {
    if(iReceivedMessageId)
        return *iReceivedMessageId;
    else
        return KNullDesC8();
    }



TBool CIdWsfSessionConsumer::Expects(const TDesC8& aRefToMessageID)
    {
    TInt index = iSentMessages.Find(aRefToMessageID);
    if (index != KErrNotFound)
        {
        return ETrue;
        }

    return EFalse;
    }

TDesC8* CIdWsfSessionConsumer::BodyL(const TDesC8& aMessageID) const
    {
    HBufC8* pBody = NULL;

    TInt index = iSentMessages.Find(aMessageID);
    if (index != KErrNotFound)
        {
        pBody = (HBufC8*)iSentMessages.ValueAt(index);
        }

    return pBody;
    }

MSenTransport& CIdWsfSessionConsumer::TransportL()
    {
    return iConsumer.TransportL();
    }

CSenIdentifier& CIdWsfSessionConsumer::Identifier() const
    {
    return iConsumer.Identifier();
    }


// enable file logging (macros)
RFileLogger* CIdWsfSessionConsumer::Log() const
    {
    return (RFileLogger*) &iLogger;
    }


TInt CIdWsfSessionConsumer::ChunkByTxnIdL(TInt aTxnId, CSenChunk*& aChunk)
    {
    return iConsumer.ChunkByTxnIdL(aTxnId, aChunk);
    }

void CIdWsfSessionConsumer::FileProgress(TInt /*aTxnId*/, TBool /*aIncoming*/, TBool /*aIsSoap*/,
                const TDesC8& /*aSoapOrCid*/, TInt /*aProgress*/)
    {
    }
void CIdWsfSessionConsumer::SetDataTrafficDetails( TSenDataTrafficDetails& /* aDetails*/)  
	{
	}
// End of file


