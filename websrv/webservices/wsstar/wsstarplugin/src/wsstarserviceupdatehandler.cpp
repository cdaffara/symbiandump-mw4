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











#include "wsstarserviceupdatehandler.h"
#include "wsstartrustclient.h"
#include "sendebug.h"
#include "senlogger.h"
#include "SenSoapFault.h"
#include "SenSoapMessage.h"
#include "wsstarservicesession.h"
#include "SenXmlUtils.h"
#include "wsstarrstresponse.h"

namespace WSSecureConversation
{ 
    //---------Faults data
    _LIT8( KBadContextToken, "BadContextToken");
}
namespace WSSecurity
{ 
    //---------Faults data
    _LIT8( KMessageExpired, "MessageExpired");
    _LIT8( KInvalidSecurity, "InvalidSecurity");
}

class CWSStarHandlerContext;
// Create instance of concrete ECOM interface implementation
CWSStarServiceUpdateHandler* CWSStarServiceUpdateHandler::NewL(TAny* aHandlerCtx)
    {
    
    MSenHandlerContext* handlerCtx =
        reinterpret_cast<MSenHandlerContext*>(aHandlerCtx);
    CWSStarServiceUpdateHandler* self   = new (ELeave) CWSStarServiceUpdateHandler(*handlerCtx);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Constructor
CWSStarServiceUpdateHandler::CWSStarServiceUpdateHandler(MSenHandlerContext& aCtx):CSenSessionHandler(aCtx)
    {
    
    }

// Destructor
CWSStarServiceUpdateHandler::~CWSStarServiceUpdateHandler()
    {
    }

// Second phase construction.
void CWSStarServiceUpdateHandler::ConstructL()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarServiceUpdateHandler::ConstructL()")));
    const TDesC8* version = iHandlerContext.GetDesC8L(HandlerContextKey::KVersion);
    if (version && 
        (*version == WSTrust::KTrustNamespace200502 || *version == WSTrust::KTrustNamespace200404))
        {
        iVersion = version;
        }
    else
        {
        iVersion = &WSTrust::KTrustNamespace200502;
        }
    }
    
TInt CWSStarServiceUpdateHandler::InvokeL(MSenSessionContext& aCtx)
    {
    CSenSoapEnvelope* message = (CSenSoapEnvelope*)(aCtx.GetAnyL(SenContext::KSenCurrentSoapMessageCtxKey));//codescannerwarnings
    CWSStarServiceSession* pNewSession = (CWSStarServiceSession*)aCtx.GetSenRemoteServiceSessionL(WSStarContextKeys::KServiceSession());
    
    //abstract variable which represent probelm (it is code or subcode depends of soap version)
    TPtrC8 problem(KNullDesC8);
    if (message)
        {
        if ((aCtx.Direction()==SenContext::EIncoming) ||
            (aCtx.Direction()==SenContext::EBoth))
            {  
            if (message->IsFault())
                {
                CSenSoapFault* faultElement = message->FaultL();
                if (faultElement)
                    {
                    if (message->NsUri() == KSenSoap12EnvelopeXmlns)
                        {
//----------------SOAP1.2
                        TPtrC8 code = faultElement->FaultCode();
                        const CSenNamespace* nmSpaceCode = faultElement->Namespace(SenXmlUtils::NsPrefix(code), ETrue);
                        code.Set(SenXmlUtils::LocalName(code));
                        
                        if ((
                            (nmSpaceCode && 
                            nmSpaceCode->URI() == KSenSoap12EnvelopeXmlns) 
                            ||
                            !nmSpaceCode)
                            &&
                            code == KSender)
                            {
                            TPtrC8 subcode = faultElement->FaultSubcode();
                            const CSenNamespace* nmSpace = faultElement->Namespace(SenXmlUtils::NsPrefix(subcode), ETrue);
                            subcode.Set(SenXmlUtils::LocalName(subcode));
                            
                            if ((nmSpace && 
                                nmSpace->URI() == *iVersion)
                                ||
                                !nmSpace) 
                                {
                                problem.Set(subcode);
                                }    
                            }
                        else if (nmSpaceCode && nmSpaceCode->URI() == KSecurityXmlNs)
                            {
                            problem.Set(code);
                            }
                        }
                    else if (message->NsUri() == KSenSoapEnvelopeXmlns)
                        {
//----------------SOAP1.1
                        TPtrC8 code = faultElement->FaultCode();
                        const CSenNamespace* nmSpace = faultElement->Namespace(SenXmlUtils::NsPrefix(code), ETrue);
                        code.Set(SenXmlUtils::LocalName(code));
                        if ((nmSpace && 
                            nmSpace->URI() == *iVersion))
                            {
                            problem.Set(code);
                            }    
                        }
//------------------real logic                        
                    if (problem.Length()>0)
                        {
                        if( problem == WSTrust::KInvalidRequest ||
                            problem == WSTrust::KRequestFailed  ||
                            problem == WSTrust::KInvalidSecurityToken  ||
                            problem == WSTrust::KAuthenticationBadElements  ||
                                   
                            problem == WSTrust::KBadRequest  ||
                            problem == WSTrust::KExpiredData  ||
                            problem == WSTrust::KInvalidTimeRange  ||
                            problem == WSTrust::KInvalidScope  ||
                            problem == WSTrust::KUnableToRenew
                           )
                           {
                           ((MSenContext&)aCtx).Update(WSStarSession::KErrorLayer, WSStarSession::ETrust);
                           }
                        else if (problem == WSTrust::KRenewNeeded ||
                        
                                 problem == WSTrust::KFailedAuthentication ||
                                 problem == WSTrust::KInvalidSecurityToken ||
                                 problem == WSSecurity::KInvalidSecurity ||
                                 problem == WSSecureConversation::KBadContextToken ||
                                 problem == WSSecurity::KMessageExpired)
                            {
				TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarServiceUpdateHandler::InvokeL")));
				TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,problem);
				TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarServiceUpdateHandler::InvokeL clear credential")));
                            ((MSenContext&)aCtx).Update(WSStarSession::KErrorLayer, WSStarSession::ETrust);
                            ((MSenContext&)aCtx).Update(WSStarContextKeys::KReAuthNeeded, ETrue);
                            pNewSession->ClearCredentialL();
                            pNewSession->SetStatusL();//in order to compute state
                            }
                        }
                    }
                }
            else if (message->HasHeader())
                {//checking for SCT (SecureContextToken)
                CSenElement& el = message->HeaderL();
                //HBufC8* msg = message->HeaderL().AsXmlL();
                                
                CSenElement* detached = el.DetachL();
                CleanupStack::PushL(detached);
                CSenElement& body = message->BodyL();
                HBufC8* msg = detached->AsXmlL();
                CleanupStack::PushL(msg);
                message->AsElement().InsertElementL(*detached, body);
                CleanupStack::Pop(2, detached);
                CleanupStack::PushL(msg);
                
                //in case MSP Guide v3.0, seems that SCT comes in Body (not in header)
                CWSStarRSTResponse* rstResponse =
                        CWSStarRSTResponse::NewLC(FALSE, KNullDesC8, iVersion);
                //policy namespace has not be set, is not neccessary for SCT
                CSenXmlReader* reader = const_cast<CSenXmlReader*>(aCtx.GetParser());
                rstResponse->SetReader(*reader);
                rstResponse->BuildFrom(*msg);
                if (rstResponse->IsResponse())
                    {
                    if (rstResponse->IsContextToken())
                        {
                        CWSStarSessionContext* ssCtx = pNewSession->SessionContext();
                        ssCtx->SetTokenKeysL(*rstResponse, NULL, ETrue,ETrue);
                        #ifdef _SENDEBUG
                                TPtrC8 endpoint = pNewSession->Endpoint();
                                _LIT8(KCredsLogLine, "SCT has been delivered by backend TO service (%S), so present token will be replaced");
                                CREDLOG_FORMAT((KSenCredsLogChannel, KSenCredsLogLevelNormal, KCredsLogLine,  &endpoint));
                        #endif           
                        //----update expires
                        /*
                        timestamp is calcualted
                        
                        if (rstResponse->CredentialExpires().Length())
                            {
                            ssCtx->Update(WSStarContextKeys::KTimestampExpires, rstResponse->CredentialExpires());
                            }
                        else
                            {
                            //ssCtx->Remove(WSStarContextKeys::KTimestampExpires);
                            }
                        //----update created
                        if (rstResponse->CredentialCreated().Length())
                            {
                            ssCtx->Update(WSStarContextKeys::KTimestampCreated, rstResponse->CredentialCreated());
                            }
                        else
                            {
                            ssCtx->Remove(WSStarContextKeys::KTimestampCreated);
                            }
                            */
                        //---remove SCT from BODY, it is not a bisness logic
                        //namespace if in SOAP element still will be not visible for user 
                        // becouse only HandleMessage(Body) is supported
                        
                        //after wave11 increment SCT is only in header so below logic is just for case
                        /*CSenElement& body = message->BodyL();
                        CSenElement* sct = GetSCTElementL(body);
                        if (sct)
                            {
                            sct = sct->DetachL();
                            delete sct;
                            sct = NULL;
                            }*/
                        //----end removing SCT from BODY
                        }
                    }
                CleanupStack::PopAndDestroy(2, msg);
                }
            }
        }
    return KErrNone;       
    }
    
SenHandler::THandlerDirection CWSStarServiceUpdateHandler::Direction() const
    {
    return SenHandler::EIncoming;
    };
SenHandler::THandlerPhase CWSStarServiceUpdateHandler::Phase()
    {
    return SenHandler::EMessage;
    };

//---------------------------------------------------------------------------
// Logger using during DEBUG mode
//---------------------------------------------------------------------------
//    
RFileLogger* CWSStarServiceUpdateHandler::Log() const
    {
    RFileLogger* pLog = NULL;
    TRAP_IGNORE( pLog = (RFileLogger*)iHandlerContext.GetAnyL(HandlerContextKey::KLogger); )
    return pLog;
    }

TInt CWSStarServiceUpdateHandler::InitL(MSenHandlerContext& aCtx)
    {
    iHandlerContext = aCtx;
    return KErrNone;
    }

CSenElement* CWSStarServiceUpdateHandler::GetSCTElementL(CSenElement& aParent)
    {
    CSenElement* sct = NULL;
    sct = aParent.Element(*iVersion, WSTrust::KRSTResponseTag);
    if (!sct)
        {
        RPointerArray<CSenElement>& elementsBody = aParent.ElementsL();
        TInt count(elementsBody.Count());
        TInt i=0;
        while (i<count && !sct)
            {
            sct = GetSCTElementL(*elementsBody[i]);
            i++;
            }
        }
    return sct;
    }
// END OF FILE

