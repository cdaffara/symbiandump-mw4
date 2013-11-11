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










#include "wsovicredentialcollectorhandler.h"
#include "senlogger.h"
#include <SenDateUtils.h>
using namespace OAuth;
using namespace WSOviResponse;
// Create instance of concrete ECOM interface implementation
CWSOviCredentialCollectorHandler* CWSOviCredentialCollectorHandler::NewL(TAny* aHandlerCtx)
    {
    
    MSenHandlerContext* handlerCtx =
        reinterpret_cast<MSenHandlerContext*>(aHandlerCtx);
    CWSOviCredentialCollectorHandler* self = 
            new(ELeave)CWSOviCredentialCollectorHandler(*handlerCtx);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Constructor
CWSOviCredentialCollectorHandler::CWSOviCredentialCollectorHandler(MSenHandlerContext& aCtx)
:CSenMessageHandler(aCtx)
    {
    }

// Destructor
CWSOviCredentialCollectorHandler::~CWSOviCredentialCollectorHandler()
    {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviCredentialCollectorHandler::\
                         ~CWSOviCredentialCollectorHandler()")));
    }

// Second phase construction.
void CWSOviCredentialCollectorHandler::ConstructL()
    {
    }
    
TInt CWSOviCredentialCollectorHandler::InvokeL(MSenMessageContext& aCtx)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviCredentialCollectorHandler::InvokeL(MSenMessageContext& aCtx)")));
    
    const TDesC8* token = aCtx.GetDesC8L(WSOviContextKeys::KToken());
    if (token && aCtx.Direction()==SenContext::EOutgoing)
        {
        TPtrC8 tokenPtr(token->Mid(KTokenTag().Length()));
        tokenPtr.Set(tokenPtr.Left(tokenPtr.Length() - KTokenTagEnd().Length()));
        
        RMapDescriptors& headerProperties = ((CWSOviMessageContext&)aCtx).OAuthProperies();
        headerProperties.Insert(&KParToken, tokenPtr.AllocL());    
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviCredentialCollectorHandler::InvokeL\
                                (MSenMessageContext& aCtx) -  \
                                Token added to OAuth parameters")));
        ((CWSOviMessageContext&)aCtx).Update(WSOviContextKeys::KToken(), tokenPtr);
        return KErrNone;                                
        }
    else
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviCredentialCollectorHandler::InvokeL\
                                (MSenMessageContext& aCtx) -  \
                                Token in message context not exists")));
        return KErrNotFound;
        }
    }

TInt CWSOviCredentialCollectorHandler::InitL(MSenHandlerContext& aCtx)
    {
    iHandlerContext = aCtx;
    return KErrNone;
    }
    
SenHandler::THandlerDirection CWSOviCredentialCollectorHandler::Direction() const
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviCredentialCollectorHandler::Direction()")));
    return SenHandler::EOutgoing;
    };
SenHandler::THandlerPhase CWSOviCredentialCollectorHandler::Phase()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviCredentialCollectorHandler::Phase()")));
    return SenHandler::EMessage;
    };    

// END OF FILE

