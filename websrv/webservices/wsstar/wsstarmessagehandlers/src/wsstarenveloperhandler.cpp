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













#include "wsstarenveloperhandler.h"
#include "senlogger.h"

// Create instance of concrete ECOM interface implementation
CWSStarEnveloperHandler* CWSStarEnveloperHandler::NewL(TAny* aHandlerCtx)
    {
    
    MSenHandlerContext* handlerCtx =
        reinterpret_cast<MSenHandlerContext*>(aHandlerCtx);
    CWSStarEnveloperHandler* self   = new (ELeave) CWSStarEnveloperHandler(*handlerCtx);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Constructor
CWSStarEnveloperHandler::CWSStarEnveloperHandler(MSenHandlerContext& aCtx):CSenMessageHandler(aCtx)
    {
    }

// Destructor
CWSStarEnveloperHandler::~CWSStarEnveloperHandler()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarEnveloperHandler::~CWSStarEnveloperHandler()")));
    }

// Second phase construction.
void CWSStarEnveloperHandler::ConstructL()
    {
    }
    
TInt CWSStarEnveloperHandler::InvokeL(MSenMessageContext& aCtx)
    {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarEnveloperHandler::InvokeL(MSenMessageContext& aCtx)")));
        const TInt* soapVersion = aCtx.GetIntL(WSStarContextKeys::KSoapVersion);
        const TDesC8* securityVersion = iHandlerContext.GetDesC8L(HandlerContextKey::KVersion);
        CSenSoapMessage* message;
        TPtrC8 secVersion(KSecurityXmlNs);
        if (securityVersion
            && (*securityVersion == KSecuritySchemeXmlNs || *securityVersion == KSecurityXmlNs))
            {
            secVersion.Set(*securityVersion);
            }
        if (soapVersion)
            {
            //old SOAP version force to use old sec spec
            if ((TSOAPVersion)*soapVersion == ESOAP11)
                {
                secVersion.Set(KSecuritySchemeXmlNs);
                }
            message = CSenSoapMessage::NewL((TSOAPVersion)*soapVersion, secVersion);
            }
        else
            {
            message = CSenSoapMessage::NewL(ESOAP12, secVersion);
            ((MSenContext&)aCtx).Add(WSStarContextKeys::KSoapVersion,ESOAP12);        
            }
        CleanupStack::PushL(message);    
        CSenElement* elemBody = 
            const_cast<CSenElement*>(aCtx.GetSenElementL(WSStarContextKeys::KBodyElement));
        if (elemBody)
            {
            message->BodyL().AddElementL(*elemBody);
            aCtx.Remove(WSStarContextKeys::KBodyElement);
            }
        else
            {
            const TDesC8* content = aCtx.GetDesC8L(WSStarContextKeys::KBody);
            if (content)
                {
                message->SetBodyL(*content);
                }
            }
        CleanupStack::Pop(message);
        aCtx.Update(SenContext::KSenCurrentSoapMessageCtxKey, message);
        return KErrNone;
    }
    
SenHandler::THandlerDirection CWSStarEnveloperHandler::Direction() const
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarEnveloperHandler::Direction()")));
    return SenHandler::EOutgoing;
    };
SenHandler::THandlerPhase CWSStarEnveloperHandler::Phase()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarEnveloperHandler::Phase()")));
    return SenHandler::EMessage;
    };    
    
TInt CWSStarEnveloperHandler::InitL(MSenHandlerContext& aCtx)
    {
    iHandlerContext = aCtx;
    return KErrNone;
    }
RFileLogger* CWSStarEnveloperHandler::Log() const
    {
    RFileLogger* pLog = NULL;
    TRAP_IGNORE( pLog = (RFileLogger*)iHandlerContext.GetAnyL(HandlerContextKey::KLogger); )
    return pLog;
    }    
// END OF FILE

