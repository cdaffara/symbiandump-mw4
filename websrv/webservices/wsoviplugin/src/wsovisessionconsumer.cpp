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











// INCLUDE FILES
#include <SenXmlUtils.h>

#include "wsovisessionconsumer.h"
#include "sendebug.h"
#include <SenServiceConnection.h>
#include "senlogger.h"
#include "wsovicons.h"
#include "SenHttpTransportProperties.h"

CWSOviSessionConsumer* CWSOviSessionConsumer::NewL( MSenRemoteServiceConsumer& aServiceConsumer)
    {
    CWSOviSessionConsumer* pSelf =
        CWSOviSessionConsumer::NewLC(aServiceConsumer);
    CleanupStack::Pop(pSelf);
    return pSelf;
    }

CWSOviSessionConsumer* CWSOviSessionConsumer::NewLC( MSenRemoteServiceConsumer& aServiceConsumer)
    {
    CWSOviSessionConsumer* pSelf =
                        new (ELeave) CWSOviSessionConsumer( aServiceConsumer);
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;
    }

CWSOviSessionConsumer::~CWSOviSessionConsumer()
    {
    iSentMessages.Reset();
    iSendReplyTo.Reset();
    iTranspProp.Reset();
    }

CWSOviSessionConsumer::CWSOviSessionConsumer( MSenRemoteServiceConsumer& aServiceConsumer)
:   iConsumer(aServiceConsumer),
    iSentMessages(ETrue, ETrue),
    iSendReplyTo(ETrue, ETrue),
    iTranspProp(ETrue, ETrue)
    {
    }


const TDesC8& CWSOviSessionConsumer::Id() const
    {
    return iConsumer.Id();
    }

TInt CWSOviSessionConsumer::SetSessionL(MSenRemoteServiceSession& aServiceSession)
    {
    iConsumer.SetSessionL(aServiceSession);
    return KErrNone;
    }


TInt CWSOviSessionConsumer::HandleMessageL( HBufC8* apMessage,
                                             const TInt aTxnId,
                                             MSenProperties* aResponseTransportProperties )
    {
    return iConsumer.HandleMessageL(apMessage, aTxnId, aResponseTransportProperties);
    }

TInt CWSOviSessionConsumer::HandleErrorL( HBufC8* apError,
                                           const TInt aErrorCode,
                                           const TInt aTxnId,
                                           MSenProperties* aResponseTransportProperties )
    {
    return iConsumer.HandleErrorL(apError, aErrorCode, aTxnId, aResponseTransportProperties);
    }

void CWSOviSessionConsumer::SetStatusL(const TInt status)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviSessionConsumer::SetStatusL");
    iConsumer.SetStatusL(status);
    }

TInt CWSOviSessionConsumer::MessageForSendingL(CSenHttpTransportProperties*& /*aTP*/,
                                           CWSOviMessageContext*& /*aCtx*/)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviSessionConsumer::MessageForSendingL");
    return KErrNone;
    }

void CWSOviSessionConsumer::ConstructL()
    {
    }


const TDesC8& CWSOviSessionConsumer::TransportProperties(
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

MSenTransport& CWSOviSessionConsumer::TransportL()
    {
    return iConsumer.TransportL();
    }
    
TInt CWSOviSessionConsumer::ChunkByTxnIdL(TInt aTxnId, CSenChunk*& aChunk)
    {
    return iConsumer.ChunkByTxnIdL(aTxnId, aChunk);
    }

void CWSOviSessionConsumer::FileProgress(TInt /*aTxnId*/, TBool /*aIncoming*/, TBool /*aIsSoap*/,
                const TDesC8& /*aSoapOrCid*/, TInt /*aProgress*/)
    {
    }

CSenIdentifier& CWSOviSessionConsumer::Identifier() const
    {
    return iConsumer.Identifier();
    }

    
TBool CWSOviSessionConsumer::HasAuthenticationCallback()
    {
    return iConsumer.HasAuthenticationCallback();
    }

TInt CWSOviSessionConsumer::ReauthenticationNeededL(CSenIdentityProvider*& aIdp)
	{
	return iConsumer.ReauthenticationNeededL(aIdp);
	}

void CWSOviSessionConsumer::SetDataTrafficDetails( TSenDataTrafficDetails&  aDetails)  
	{
	iConsumer.SetDataTrafficDetails(aDetails);
	}
// End of file

