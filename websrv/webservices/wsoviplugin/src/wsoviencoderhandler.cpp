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










#include "wsoviencoderhandler.h"
#include "senlogger.h"
#include "wsoviutils.h"
#include <SenHttpTransportProperties.h>
using namespace OAuth;
using namespace WSOviResponse;
// Create instance of concrete ECOM interface implementation
CWSOviEncoderHandler* CWSOviEncoderHandler::NewL(TAny* aHandlerCtx)
    {
    
    MSenHandlerContext* handlerCtx =
        reinterpret_cast<MSenHandlerContext*>(aHandlerCtx);
    CWSOviEncoderHandler* self = 
            new(ELeave)CWSOviEncoderHandler(*handlerCtx);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Constructor
CWSOviEncoderHandler::CWSOviEncoderHandler(MSenHandlerContext& aCtx)
:CSenMessageHandler(aCtx)
    {
    }

// Destructor
CWSOviEncoderHandler::~CWSOviEncoderHandler()
    {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviEncoderHandler::\
                         ~CWSOviEncoderHandler()")));
    }

// Second phase construction.
void CWSOviEncoderHandler::ConstructL()
    {
    }
    
TInt CWSOviEncoderHandler::InvokeL(MSenMessageContext& aCtx)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviEncoderHandler::InvokeL(MSenMessageContext& aCtx)")));
    
    const TDesC8* bodyMessage = ((CWSOviMessageContext&)aCtx).GetDesC8L(WSOviContextKeys::KMessageBody);

    if (bodyMessage)
        {
        CSenTransportProperties* transProp = ((CWSOviMessageContext&)aCtx).TP();
        CSenHttpTransportProperties::TSenHttpMethod httpMethod = CSenHttpTransportProperties::ESenHttpPost;
        TInt retVal = ((CSenHttpTransportProperties*)transProp)->HttpMethodL(httpMethod);
        if (httpMethod == CSenHttpTransportProperties::ESenHttpGet)
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviEncoderHandler::InvokeL\
                                    (MSenMessageContext& aCtx) -  \
                                    body is not encoding with GET method for http")));
            return KErrNone;
            }

        HBufC8* bodyEncodedUrl = CWSOviUtils::FormEncodeCharsLC(*bodyMessage);
        HBufC8* allBody = HBufC8::NewL(KXmlContentEqual().Length()+bodyEncodedUrl->Length());
        allBody->Des().Append(KXmlContentEqual());
        allBody->Des().Append(*bodyEncodedUrl);
        CleanupStack::PushL(allBody);
        
        ((CWSOviMessageContext&)aCtx).Update(WSOviContextKeys::KMessageBody, *allBody);
        
        CleanupStack::PopAndDestroy(allBody);
        CleanupStack::PopAndDestroy(bodyEncodedUrl);
        return KErrNone;
        }
    else
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviEncoderHandler::InvokeL\
                                (MSenMessageContext& aCtx) -  \
                                body message context not exists")));
        return KErrNotFound;
        }
    }

TInt CWSOviEncoderHandler::InitL(MSenHandlerContext& aCtx)
    {
    iHandlerContext = aCtx;
    return KErrNone;
    }
    
SenHandler::THandlerDirection CWSOviEncoderHandler::Direction() const
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviEncoderHandler::Direction()")));
    return SenHandler::EOutgoing;
    };
SenHandler::THandlerPhase CWSOviEncoderHandler::Phase()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviEncoderHandler::Phase()")));
    return SenHandler::EMessage;
    };    

// END OF FILE

