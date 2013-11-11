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










#include "wsstarcredentialcollectorhandler.h"
#include "senlogger.h"
#include <SenDateUtils.h>

// Create instance of concrete ECOM interface implementation
CWSStarCredentialCollectorHandler* CWSStarCredentialCollectorHandler::NewL(TAny* aHandlerCtx)
    {
    
    MSenHandlerContext* handlerCtx =
        reinterpret_cast<MSenHandlerContext*>(aHandlerCtx);
    CWSStarCredentialCollectorHandler* self = 
            new(ELeave)CWSStarCredentialCollectorHandler(*handlerCtx);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Constructor
CWSStarCredentialCollectorHandler::CWSStarCredentialCollectorHandler(MSenHandlerContext& aCtx)
:CSenMessageHandler(aCtx)
    {
    }

// Destructor
CWSStarCredentialCollectorHandler::~CWSStarCredentialCollectorHandler()
    {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarCredentialCollectorHandler::\
                         ~CWSStarCredentialCollectorHandler()")));
    }

// Second phase construction.
void CWSStarCredentialCollectorHandler::ConstructL()
    {
    }
    
TInt CWSStarCredentialCollectorHandler::InvokeL(MSenMessageContext& aCtx)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarCredentialCollectorHandler::InvokeL(MSenMessageContext& aCtx)")));
    TInt result = KErrNone;
    CSenSoapMessage* message = 
        (CSenSoapMessage*)(aCtx.GetSenSoapEnvelopeL(SenContext::KSenCurrentSoapMessageCtxKey));
    if (message)
        {   
        if (aCtx.Direction()==SenContext::EOutgoing)
            {
            //1. first put timestamp in security section. 
            //Propably 1st tag processed by servers


            HBufC8* timestamp = NULL;
            const TDesC8*  createdValue = aCtx.GetDesC8L(WSStarContextKeys::KTimestampCreated);
            const TDesC8*  expiredValue = aCtx.GetDesC8L(WSStarContextKeys::KTimestampExpires);
                 
            if (createdValue)
                {
                TTime clientTime;
                TTime mtTime;
                const TDesC8* mtTimetd = aCtx.GetDesC8L(WSStarContextKeys::KPhoneTimeWhenMTResolved);
                mtTime = SenDateUtils::FromXmlDateTimeL(*mtTimetd);
                clientTime.UniversalTime();
                if (clientTime != Time::NullTTime())
                    {
                    TTimeIntervalMicroSeconds  diff  = clientTime.MicroSecondsFrom(mtTime);
                    if (diff > 0)
                        {
                        TTime createTime = SenDateUtils::FromXmlDateTimeL(*createdValue);
                        TBuf8<SenDateUtils::KXmlDateTimeMaxLength> newCreateTime;
                        TInt leaveCode(KErrNone);
                        TRAP(leaveCode, SenDateUtils::ToXmlDateTimeUtf82L(newCreateTime, createTime + diff);)
                        if (!leaveCode)
                            {
                            User::LeaveIfError(
                                        ((MSenContext&)aCtx).Update(WSStarContextKeys::KTimestampCreated, 
                                        newCreateTime));
                            createdValue = &newCreateTime;
                            }
                        }
                 }
            //problem with signing if "expired" tag exists. 
            //Presently wssecurity is turned-off, so we can include this tag.
            if (expiredValue)  
                {
                CSenWsSecurityHeader::TimestampL(*createdValue, *expiredValue, timestamp);
                CleanupStack::PushL(timestamp);
                }
            else
                {
                CSenWsSecurityHeader::TimestampL(*createdValue, timestamp);
                CleanupStack::PushL(timestamp);
                }
            message->AddSecurityTokenL(*timestamp);
            CleanupStack::PopAndDestroy(timestamp);
                }
//2.after timestamp put else stuff like MT            
            const TDesC8* securityToken = aCtx.GetDesC8L(WSStarContextKeys::KSecurityToken);
            if (securityToken)
                {
                message->AddSecurityTokenL(*securityToken);
                }
            else
                {
                TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarCredentialCollectorHandler::InvokeL\
                                (MSenMessageContext& aCtx) -  \
                                Security token in message context not exists")));
                return KErrNone;
                }            
            }
        }
    else
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarCredentialCollectorHandler::InvokeL(MSenMessageContext& aCtx)\
                         - SOAP message in message context not exists")));
        result = KErrNotFound;
        }
    return result;
    }

TInt CWSStarCredentialCollectorHandler::InitL(MSenHandlerContext& aCtx)
    {
    iHandlerContext = aCtx;
    return KErrNone;
    }
    
SenHandler::THandlerDirection CWSStarCredentialCollectorHandler::Direction() const
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarCredentialCollectorHandler::Direction()")));
    return SenHandler::EOutgoing;
    };
SenHandler::THandlerPhase CWSStarCredentialCollectorHandler::Phase()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarCredentialCollectorHandler::Phase()")));
    return SenHandler::EMessage;
    };    
RFileLogger* CWSStarCredentialCollectorHandler::Log() const
    {
    RFileLogger* pLog = NULL;
    TRAP_IGNORE( pLog = (RFileLogger*)iHandlerContext.GetAnyL(HandlerContextKey::KLogger); )
    return pLog;
    }
// END OF FILE

