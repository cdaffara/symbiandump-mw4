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











#include "wsstarpassporthandler.h"
#include "SenXmlUtils.h"
#include "wsstarmessageutils.h"
#include "wsstartrustclient.h"
#include "SenSoapFault.h"
#include "senlogger.h"
#include "sensaxutils.h"

#ifdef RD_SEN_VTCP_SUPPORT
#include "wsstarpassportheaderinbound.h"
#endif//RD_SEN_VTCP_SUPPORT


using namespace WSPassport32;

// Create instance of concrete ECOM interface implementation
CWSStarPassportHandler* CWSStarPassportHandler::NewL(TAny* aHandlerCtx)
    {
    MSenHandlerContext* ctx =
        reinterpret_cast<MSenHandlerContext*>( aHandlerCtx);
    CWSStarPassportHandler* self = new (ELeave) CWSStarPassportHandler(*ctx);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
TInt CWSStarPassportHandler::InvokeL(MSenMessageContext& aCtx)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPassportHandler::InvokeL(MSenMessageContext& aCtx)")));
    CSenSoapEnvelope* message = 
        const_cast<CSenSoapEnvelope*>(aCtx.GetSenSoapEnvelopeL(
                                                SenContext::KSenCurrentSoapMessageCtxKey));
    const TBool* boolValue = NULL;
    if (message)
        {
        _LIT8(KId,"Id");
        boolValue = aCtx.GetIntL(WSStarContextKeys::KPassportEnabled);
        if ((boolValue && *boolValue == (TBool)ETrue))
            {
            if (aCtx.Direction() == SenContext::EOutgoing)
                {//------------------OUTGOING
                CSenElement& header = message->HeaderL();
                CSenElement& authInfo = header.AddElementL(KPassportXmlns,
                                                           KAuthInfoTag,
                                                           KAuthInfoQTag);
                SenXmlUtils::AddAttributeL(authInfo, KId ,KPPAuthInfoAttrValue);
                authInfo.AddElementL(KPassportXmlns, 
                                        KHostingAppTag, 
                                        KHostingAppQTag).SetContentL(KHostingAppTagValue);
                authInfo.AddElementL(KPassportXmlns, 
                                        KBinaryVersionTag, 
                                        KBinaryVersionQTag).SetContentL(KBinaryVersionTagValue);
                authInfo.AddElementL(KPassportXmlns, 
                                        KUIVersionTag, 
                                        KUIVersionQTag).SetContentL(KUIVersionTagValue);
                authInfo.AddElementL(KPassportXmlns, 
                                        KCookiesTag, 
                                        KCookiesQTag).SetContentL(KCookiesTagValue);
                authInfo.AddElementL(KPassportXmlns, 
                                        KRequestParamsTag, 
                                        KRequestParamsQTag).SetContentL(KRequestParamsTagValue);
                CSenElement& body = message->BodyL();
                CSenElement* multiEl = 
                    CWSStarMessageUtils::GetMarkedElementL(body, WSStarContextKeys::KMultiReqTagMark, 
                                                            EFalse);
                if (multiEl)
                    {
                    multiEl->Set(KPassportXmlns, KRequestMultipleSecurityTokensTag, 
                                    KRequestMultipleSecurityTokensQTag);
                    }
                        
                }
            else if(aCtx.Direction() == SenContext::EIncoming)
                {//------------------INCOMING
                TPtrC8 problem(KNullDesC8);
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
                                
                                if (nmSpace && 
                                    nmSpace->URI() == WSTrust::KTrustNamespace200502())
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
                                nmSpace->URI() == WSTrust::KTrustNamespace200502()))
                                {
                                problem.Set(code);
                                }    
                            }
    //------------------real logic                        
                        if (problem.Length()>0 && problem == WSTrust::KFailedAuthentication)
                            {
                            CSenElement* elem = NULL;
                            CSenElement& env = message->AsElement();
                            elem = CWSStarMessageUtils::FindElementL(KRedirectUrlTag, env);
                            if (elem)
                                {
                                const CSenNamespace* ns = elem->Namespace();
                                if (ns && ns->URI() == KPassportFaultNamespaceXmlns)
                                    {
                                    ((MSenContext&)aCtx).Update(WSStarSession::KErrorLayer, WSStarSession::ETrust);
                                    ((MSenContext&)aCtx).Update(WSStarContextKeys::KReAuthNeeded, EFalse);
                                    ((MSenContext&)aCtx).Update(WSStarContextKeys::KRedirect, elem->Content());        
                                    }
                                }
                            }
                        }
                    }
		#ifdef RD_SEN_VTCP_SUPPORT					
                else
                    {
                        TPtrC8 txtMessage = aCtx.GetMessageL();
                        if(message)
                        {
                            return HandleInboundL(aCtx, message);
                        }
                        else if(txtMessage.Length())
                        {
                            TInt ret(KErrNone);
                            TInt endTagStart;
                            TInt endTagEnd;
                            TInt startTagStart;
                            TInt startTagEnd;
                            TPtrC8 prefix;

                            ret = SenSaxUtils::SearchEndTagL(txtMessage,KSenSoapEnvelopeBodyName,
                                                             endTagStart, endTagEnd, prefix);
                            if ( ret != KErrNotFound )
                                {
                                ret = SenSaxUtils::SearchStartTagL(txtMessage,prefix,KSenSoapEnvelopeBodyName,
                                                                   startTagStart, startTagEnd);
                                }
                            if ( ret != KErrNotFound )
                                {
                                    TPtrC8 startPart(txtMessage.Ptr(),startTagEnd+1);
                                    TPtrC8 endPart(txtMessage.Ptr()+endTagStart,txtMessage.Size()-endTagStart);
                                    HBufC8* pXmlWithoutBody = HBufC8::NewLC(startPart.Length()+endPart.Length());
                                    pXmlWithoutBody->Des().Append(startPart);
                                    pXmlWithoutBody->Des().Append(endPart);

                                    CWSStarPassportHeaderInbound* inbound =
                                            CWSStarPassportHeaderInbound::NewLC();
                                    inbound->SetMessageContext(aCtx);
                                    CSenXmlReader& reader = const_cast<CSenXmlReader&>(*aCtx.GetParser());
                                    inbound->SetReader(reader);
                                    inbound->BuildFrom(*pXmlWithoutBody);
                                    CleanupStack::PopAndDestroy(inbound);


                                    CleanupStack::PopAndDestroy(pXmlWithoutBody);
        
                                
                                }
                            else//case with empty body - <S:Body/>, so we can parse all
                                {
                                CWSStarPassportHeaderInbound* inbound =
                                        CWSStarPassportHeaderInbound::NewLC();
                                inbound->SetMessageContext(aCtx);
                                CSenXmlReader& reader = const_cast<CSenXmlReader&>(*aCtx.GetParser());
                                inbound->SetReader(reader);
                                inbound->BuildFrom(txtMessage);
                                CleanupStack::PopAndDestroy(inbound);                        
                                }
                        }
					}                        
		#endif//RD_SEN_VTCP_SUPPORT
				
                }
            }
        }
    else
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPassportHandler::InvokeL(MSenMessageContext& aCtx) - \
                        Soap message in message context not exists")));
        return KErrNotFound;
        }
    return KErrNone;    
    }    

TInt CWSStarPassportHandler::HandleInboundL(MSenMessageContext& aCtx, CSenSoapEnvelope* aMessage)
    {

    CSenElement& header = aMessage->HeaderL();

    CWSStarMessageUtils::RArraySenElements elements;
    CWSStarMessageUtils::FindElementL(KCredProperty, header,elements);
    CSenElement* elem = NULL;
    TInt count(elements.Count());
    TInt i=0;
    while (i<count)
        {
        elem = elements[i];
        if (elem->LocalName() == WSPassport32::KCredProperty)
            {
            const TDesC8& nsUri = elem->NamespaceURI();
            if ((nsUri == WSPassport32::KPassportFaultNamespaceXmlns))
                {
                    const TDesC8* ptr= elem->AttrValue(WSPassport32::KName);
                    if(*ptr == WSPassport32::KMainBrandID)
                    {
                        ((MSenContext&)aCtx).Update(WSStarContextKeys::KMainBrandID, elem->Content());
                    }
                 }
            }
        i++;
        }
    elements.Reset();
    
    CWSStarMessageUtils::RArraySenElements credProperties;
    CWSStarMessageUtils::FindElementL(KCredProperties, header,credProperties);
    elem = NULL;
    count = 0;
    count = credProperties.Count();
	if(count == 1)
	{
		elem = credProperties[0];	
		HBufC8* xmlSnippet = elem->AsXmlL();
		CleanupStack::PushL(xmlSnippet);
		((MSenContext&)aCtx).Update(WSStarContextKeys::KBrandIDList,xmlSnippet->Des() );		
 		CleanupStack::PopAndDestroy(xmlSnippet);
     		
	}
    credProperties.Reset();
    
    return KErrNone;
            
    }
SenHandler::THandlerDirection CWSStarPassportHandler::Direction() const
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPassportHandler::Direction()")));
    return SenHandler::EOutgoing;
    }
SenHandler::THandlerPhase CWSStarPassportHandler::Phase()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPassportHandler::Phase()")));
    return SenHandler::EMessage;
    }

// Constructor
CWSStarPassportHandler::CWSStarPassportHandler(MSenHandlerContext& aCtx)
:CSenMessageHandler(aCtx)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPassportHandler::CWSStarPassportHandler(MSenHandlerContext& aCtx)")));
    }

// Destructor
CWSStarPassportHandler::~CWSStarPassportHandler()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPassportHandler::~CWSStarPassportHandler()")));
    }

// Second phase construction.
void CWSStarPassportHandler::ConstructL()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPassportHandler::ConstructL()")));
    }

TInt CWSStarPassportHandler::InitL(MSenHandlerContext& aCtx)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPassportHandler::InitL(MSenHandlerContext& aCtx)")));
    iHandlerContext = aCtx;
    return KErrNone;
    }

RFileLogger* CWSStarPassportHandler::Log() const
    {
    RFileLogger* pLog = NULL;
    TRAP_IGNORE( pLog = (RFileLogger*)iHandlerContext.GetAnyL(HandlerContextKey::KLogger); )
    return pLog;
    }

// END OF FILE

