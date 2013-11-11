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











#include "wsstaraddressinghandler.h"
#include "wsstarmessageutils.h"
#include "SenXmlUtils.h"
#include "sensaxutils.h"
#include "senlogger.h" // private Utils\inc

using namespace WSStarAddressing;
using namespace WSStarAddressingFault;

// Create instance of concrete ECOM interface implementation
CWSStarAddressingHandler* CWSStarAddressingHandler::NewL(TAny* aHandlerCtx)
    {
    MSenHandlerContext* ctx =
        reinterpret_cast<MSenHandlerContext*>( aHandlerCtx);
    CWSStarAddressingHandler* self = new (ELeave) CWSStarAddressingHandler(*ctx);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

TInt CWSStarAddressingHandler::InvokeL(MSenMessageContext& aCtx)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarAddressingHandler::InvokeL(MSenMessageContext& aCtx)")));
    CSenSoapEnvelope* message = 
        const_cast<CSenSoapEnvelope*>(aCtx.GetSenSoapEnvelopeL(
                                    SenContext::KSenCurrentSoapMessageCtxKey)
                                    );
    TPtrC8 txtMessage = aCtx.GetMessageL();
    InitVersionL(aCtx.GetDesC8L(WSStarContextKeys::KAddressingVersion));
    if (message)
        {
        const CSenNamespace* nmSpace = message->Namespace(KAddressingPrefix);
        if (nmSpace)
            {
            InitVersionL(&nmSpace->URI());
            }
        if (aCtx.Direction() == SenContext::EOutgoing)
            {
            return HandleOutboundL(aCtx, message);
            }
        else if (aCtx.Direction()==SenContext::EIncoming)
            {
            return HandleInboundL(aCtx, message);
            }
        else
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarAddressingHandler::InvokeL(MSenMessageContext& aCtx) - \
                                strange direction")));
            return KErrNotFound;
            }
        }
    else if (txtMessage.Length())
        {
//---------------idea based on method
/*    
TInt CSenWebServiceSession::HandleBodyWithoutParsingL(CSenSoapMessage& aMessage,
                                                      const TDesC8& aResponse)
*/
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
            if ( ret != KErrNotFound )
                {
                TPtrC8 startPart(txtMessage.Ptr(),startTagEnd+1);
                TPtrC8 endPart(txtMessage.Ptr()+endTagStart,txtMessage.Size()-endTagStart);
                HBufC8* pXmlWithoutBody = HBufC8::NewLC(startPart.Length()+endPart.Length());
                pXmlWithoutBody->Des().Append(startPart);
                pXmlWithoutBody->Des().Append(endPart);
        
                CWSStarAddressingHeaderInbound* inbound =
                        CWSStarAddressingHeaderInbound::NewLC(iVersion);
                inbound->SetMessageContext(aCtx);
                CSenXmlReader& reader = const_cast<CSenXmlReader&>(*aCtx.GetParser());
                inbound->SetReader(reader);
                inbound->BuildFrom(*pXmlWithoutBody);
                CleanupStack::PopAndDestroy(inbound);
                
                
                CleanupStack::PopAndDestroy(pXmlWithoutBody);
                
                }
            }
        else//case with empty body - <S:Body/>, so we can parse all
            {
             CWSStarAddressingHeaderInbound* inbound =
                    CWSStarAddressingHeaderInbound::NewLC(iVersion);
            inbound->SetMessageContext(aCtx);
            CSenXmlReader& reader = const_cast<CSenXmlReader&>(*aCtx.GetParser());
            inbound->SetReader(reader);
            inbound->BuildFrom(txtMessage);
            CleanupStack::PopAndDestroy(inbound);
            }
        
        return ret;
        }
    else
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarAddressingHandler::InvokeL(MSenMessageContext& aCtx) - \
                            Soap message in message context not exists")));
        return KErrNotFound;
        }
    }    
TInt CWSStarAddressingHandler::HandleInboundL(MSenMessageContext& aCtx, CSenSoapEnvelope* aMessage)
    {
/*
    CWSStarAddressingHeaderInbound* inbound =  CWSStarAddressingHeaderInbound::NewLC(iVersion);
    inbound->SetMessageContext(aCtx);
    CSenXmlReader& reader = const_cast<CSenXmlReader&>(*aCtx.GetParser());
    inbound->SetReader(reader);
    HBufC8* buf = aMessage->AsXmlL();
    CleanupStack::PushL(buf);
    inbound->BuildFrom(*buf);
*/
    CSenElement& header = aMessage->HeaderL();

    CSenElement* elem = NULL;
    CSenElement* addressElem = NULL;    

    elem = CWSStarMessageUtils::FindElementL(KToName, header);
    if (elem)
        {
        const TDesC8& nsUri = elem->NamespaceURI();
        if (!iVersion && (nsUri == KAddressingXmlns200408 ||
            nsUri == KAddressingXmlns200403 ||
            nsUri == KAddressingXmlns200412 ||
            nsUri == KAddressingXmlns200502 ||
            nsUri == KAddressingXmlns200503 ||
            nsUri == KAddressingXmlns200508))
            {
            iVersion = nsUri.AllocL();
            }
        ((MSenContext&)aCtx).Update(WSStarContextKeys::KTo, elem->Content());
        }
    elem = CWSStarMessageUtils::FindElementL(KRelatesToName, header);
    if (elem)
        {
        ((MSenContext&)aCtx).Update(WSStarContextKeys::KRelatesTo, elem->Content());
        }
    elem = CWSStarMessageUtils::FindElementL(KMessageIDName, header);
    if (elem)
        {
        ((MSenContext&)aCtx).Update(WSStarContextKeys::KMessageID, elem->Content());
        }
    elem = CWSStarMessageUtils::FindElementL(KActionName, header);
    if (elem)
        {
        ((MSenContext&)aCtx).Update(WSStarContextKeys::KAction, elem->Content());
        }
    elem = CWSStarMessageUtils::FindElementL(WSStarAddressingEPR::KDeviceAddressName, header);
    if (elem)
        {
        ((MSenContext&)aCtx).Update(WSStarContextKeys::KReplyToDeviceAddress, elem->Content());
        }
    
    elem = CWSStarMessageUtils::FindElementL(KReplyToName, header);
    if (elem)
        {
        addressElem = CWSStarMessageUtils::FindElementL(KAddressName, *elem);
        if (addressElem)
            {
            ((MSenContext&)aCtx).Update(WSStarContextKeys::KReplyTo, addressElem->Content());
            }
        }
    elem = CWSStarMessageUtils::FindElementL(KFaultToName, header);
    if (elem)
        {
        addressElem = CWSStarMessageUtils::FindElementL(KAddressName, *elem);
        if (addressElem)
            {
            ((MSenContext&)aCtx).Update(WSStarContextKeys::KFaultTo, addressElem->Content());
            }
        }
    elem = CWSStarMessageUtils::FindElementL(KFromName, header);
    if (elem)
        {
        addressElem = CWSStarMessageUtils::FindElementL(KAddressName, *elem);
        if (addressElem)
            {
            ((MSenContext&)aCtx).Update(WSStarContextKeys::KFrom, addressElem->Content());
            }
        }
    _LIT8(Kpp, "pp");
    _LIT8(KCredProperties, "credProperties");
    _LIT8(KCredProperty, "credProperty");
    //_LIT8(KName, "Name");
    CWSStarMessageUtils::RArraySenElements credPropertiesElements;
    CSenElement* ppElem = CWSStarMessageUtils::FindElementL(Kpp, header);
    if(ppElem)
        {
        CSenElement* credPropertiesElem = CWSStarMessageUtils::FindElementL(KCredProperties, *ppElem);//Naga
        if(credPropertiesElem)
            {
            CWSStarMessageUtils::FindElementL(KCredProperty, *credPropertiesElem, credPropertiesElements);
            TInt count = credPropertiesElements.Count();
            for(TInt i=0; i<count; i++)
                {
                elem = credPropertiesElements[i];
                const TDesC8* ptr = elem->AttrValue(WSStarContextKeys::KName);
                if ( *ptr == WSStarContextKeys::KMainBrandID )
                    {
                    ((MSenContext&)aCtx).Update(WSStarContextKeys::KMainBrandID, elem->Content());
                    }
                if (*ptr == WSStarContextKeys::KBrandIDList)
                    {
                    ((MSenContext&)aCtx).Update(WSStarContextKeys::KBrandIDList, elem->Content());
                    }            
                }
            }        
        }

/*
    if (!iVersion && inbound->Version())
        {
        iVersion = inbound->Version()->AllocL();
        }
*/    
    if (!aCtx.GetDesC8L(WSStarContextKeys::KRelatesTo))
        {
        TLSLOG_FORMAT((KSenFaultsLogChannel, KSenFaultsLogLevel, 
                        WSStarAddressingFault::KInvalidHeaderFormatString8,
                        &KRelatesToQName(),&WSStarAddressingFault::KMissingTag()));   
        
        }
    if (!aCtx.GetDesC8L(WSStarContextKeys::KTo))
        {
        TLSLOG_FORMAT((KSenFaultsLogChannel, KSenFaultsLogLevel, KRequiredHeaderFormatString8,
                        &KToQName()));   
        
        }
    if (!aCtx.GetDesC8L(WSStarContextKeys::KAction))
        {
        TLSLOG_FORMAT((KSenFaultsLogChannel, KSenFaultsLogLevel, KRequiredHeaderFormatString8,
                        &KActionQName()));   
        
        }
    if (aCtx.GetDesC8L(WSStarContextKeys::KReplyTo) || aCtx.GetDesC8L(WSStarContextKeys::KFaultTo))
        {
        if (!aCtx.GetDesC8L(WSStarContextKeys::KMessageID))
            {
            TLSLOG_FORMAT((KSenFaultsLogChannel, KSenFaultsLogLevel, KRequiredHeaderFormatString8,
                            &KMessageIDQName()));   
            
            }
        }

    //    CleanupStack::PopAndDestroy(buf);
//    CleanupStack::PopAndDestroy(inbound);
    //CSenElement& body = aMessage->BodyL();
    ShareMyLiterals(aCtx);
    if (aMessage->IsFault())
        {
        TPtrC8 errorCode(KNullDesC8);
        CSenSoapFault* faultElement = aMessage->FaultL();
        if (faultElement)
            {
            TPtrC8 code;
            if (aMessage->NsUri() == KSenSoapEnvelopeXmlns)
                {
                TPtrC8 code = faultElement->FaultCode();                            
                const CSenNamespace* nmSpace = faultElement->Namespace(SenXmlUtils::NsPrefix(code),
                                                                       ETrue);
                code.Set(SenXmlUtils::LocalName(code));
                if (nmSpace)
                    {
                    if (!iVersion)
                        {
                        InitVersionL(&nmSpace->URI());
                        }
                    if (iVersion && 
                        nmSpace->URI() == *iVersion)
                        {
                        errorCode.Set(code);
                        }    
                    }
                }
            else if (aMessage->NsUri() == KSenSoap12EnvelopeXmlns)
                {
                code.Set(faultElement->FaultCode());
                const CSenNamespace* nmSpaceCode = 
                        faultElement->Namespace(SenXmlUtils::NsPrefix(code), ETrue);
                code.Set(SenXmlUtils::LocalName(code));
                if ((nmSpaceCode && 
                    nmSpaceCode->URI() == KSenSoap12EnvelopeXmlns) &&
                    (code == KSender || code == KReceiver))
                    {
                    TPtrC8 subcode = faultElement->FaultSubcode();
                    const CSenNamespace* nmSpace = 
                        faultElement->Namespace(SenXmlUtils::NsPrefix(subcode), ETrue);
                    subcode.Set(SenXmlUtils::LocalName(subcode));
                if (nmSpace)
                    {
                    if (!iVersion)
                        {
                        InitVersionL(&nmSpace->URI());
                        }
                    if (iVersion && 
                        nmSpace->URI() == *iVersion)
                        {
                        errorCode.Set(subcode);
                        }    
                    }
                    }
                }
            if (errorCode.Length()>0)
                {
                if (errorCode == WSStarAddressingFault::KInvalidHeader ||
                    errorCode == WSStarAddressingFault::KRequiredHeader ||
                    errorCode == WSStarAddressingFault::KDestinationUnreachable ||
                    errorCode == WSStarAddressingFault::KActionNotSupported )
                    {
                    ((MSenContext&)aCtx).Update(WSStarSession::KErrorLayer,
                                                WSStarSession::EAddressing);
                    }
                if ((errorCode == WSStarAddressingFault::KEndpointUnavailable)
                    && (code == KReceiver))
                    {
                    ((MSenContext&)aCtx).Update(WSStarSession::KErrorLayer,
                                                WSStarSession::EAddressing);
                    CSenElement* detailElement = faultElement->Element(KFault12DetailLocalname);
                    if (detailElement)
                        {
                        CSenElement* retryElement = detailElement->Element(*iVersion,
                                                                           KRetryAfterName); 
                        if (retryElement)
                            {
                            TPtrC8 retryAfter = retryElement->Content();
                            TLex8 num;
                            TInt retryInt;
                            num.Assign(retryAfter);
                            TInt ret = num.Val(retryInt);
                            if (ret == KErrNone)
                                ((MSenContext&)aCtx).Update(WSStarContextKeys::KRetryAfter,
                                                            retryInt);
                            }
                        }
                    }
                }
            }
        else
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarAddressingHandler::InvokeL(MSenMessageContext& aCtx) - \
                            Fault element not exists")));
            return KErrNotFound;
            }
        }
    return KErrNone;
    }
TInt CWSStarAddressingHandler::HandleOutboundL(MSenMessageContext& aCtx, 
                                                CSenSoapEnvelope* aMessage)
    {
    if (!iVersion)
        {
        iVersion = KAddressingXmlns200508().AllocL();
        }
    TPtrC8 anonymous(KNullDesC8);
    if (!iVersion->Compare(KAddressingXmlns200403()))
        {
        anonymous.Set(KReplyToAnonymous200403());
        }
     else if(!iVersion->Compare(KAddressingXmlns200408()))
        {
        anonymous.Set(KReplyToAnonymous200408());
        }
     else if(!iVersion->Compare(KAddressingXmlns200412()))
        {
        anonymous.Set(KReplyToAnonymous200412());
        }
     else if(!iVersion->Compare(KAddressingXmlns200502()))
        {
        anonymous.Set(KReplyToAnonymous200502());
        }
     else if(!iVersion->Compare(KAddressingXmlns200503()))
        {
        anonymous.Set(KReplyToAnonymous200503());
        }
     else if(!iVersion->Compare(KAddressingXmlns200508()))
        {
        anonymous.Set(KReplyToAnonymous200508());
        }
    CSenElement& element = aMessage->AsElement();
    element.AddNamespaceL(KAddressingPrefix, *iVersion);
    ((MSenContext&)aCtx).Update(WSStarContextKeys::KAddressingNsHolder, *iVersion);
///////////////    
    const TDesC8* value = NULL;
    const TInt* isPassportEnabled = aCtx.GetIntL(WSStarContextKeys::KPassportEnabled);
    value = aCtx.GetDesC8L(WSStarContextKeys::KMessageID);
    if (!value)
        {
        SetMessageIDL(aCtx);
        }
        
    if (!isPassportEnabled || (isPassportEnabled && *isPassportEnabled == FALSE))
        {
        CSenElement& header = aMessage->HeaderL();
        value = aCtx.GetDesC8L(WSStarContextKeys::KMessageID);
        if (value)
            {
            CreateElementAddressingL(header, KMessageIDName, *value, EFalse);
            }
        else
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarAddressingHandler::InvokeL(MSenMessageContext& aCtx) - \
                            MessageId in message context not exists")));
            return KErrNotFound;
            }
        value = aCtx.GetDesC8L(WSStarContextKeys::KTo);
        if (value)
            {
            CreateElementAddressingL(header, KToName, *value, EFalse);
            }
        else
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarAddressingHandler::InvokeL(MSenMessageContext& aCtx) - \
                            Endpoint in message context not exists")));
            return KErrNotFound;
            }
        value = aCtx.GetDesC8L(WSStarContextKeys::KAction);
        if (value)
            {
            CreateElementAddressingL(header, KActionName, *value, EFalse);
            }
        else
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarAddressingHandler::InvokeL(MSenMessageContext& aCtx) - \
                            Action in message context not exists")));
            return KErrNotFound;
            }
        value = aCtx.GetDesC8L(WSStarContextKeys::KRelatesTo);
        if (value)
            {
            CreateElementAddressingL(header, KRelatesToName, *value, EFalse);
            }
        value = aCtx.GetDesC8L(WSStarContextKeys::KFrom);
        if (value)
            {
            CreateElementAddressingL(header, KFromName, *value, ETrue);
            }
        value = aCtx.GetDesC8L(WSStarContextKeys::KReplyTo);
        if (value)
            {
            CSenElement* element = CreateElementAddressingL(header, KReplyToName, *value, ETrue);
            const TDesC8* mwsNs = NULL;
            mwsNs = aCtx.GetDesC8L(WSStarContextKeys::KMwsNamespace);
            if (mwsNs)
                {
                const TDesC8* deviceId = NULL;
                deviceId = aCtx.GetDesC8L(WSStarContextKeys::KReplyToDeviceAddress);
                if (deviceId && element)
                    {
                    ((CWSStarAddressingEPR*)element)->AddDeviceAddressL(*mwsNs,*deviceId);
                    }
                }
            }
        else
            {
            CSenElement* element = CreateElementAddressingL(header, KReplyToName, 
                                                            anonymous, ETrue);
            ((MSenContext&)aCtx).Update(WSStarContextKeys::KReplyTo, anonymous);
            const TDesC8* mwsNs = NULL;
            mwsNs = aCtx.GetDesC8L(WSStarContextKeys::KMwsNamespace);
            if (mwsNs)
                {
                const TDesC8* deviceId = NULL;
                deviceId = aCtx.GetDesC8L(WSStarContextKeys::KReplyToDeviceAddress);
                if (deviceId && element)
                    {
                    ((CWSStarAddressingEPR*)element)->AddDeviceAddressL(*mwsNs,*deviceId);
                    }
                }
            }
            
        value = aCtx.GetDesC8L(WSStarContextKeys::KFaultTo);
        if (value)
            {
            CreateElementAddressingL(header, KFaultToName, *value, ETrue);
            }    
        if(aCtx.GetIntL(WSStarContextKeys::KAppInfoPresent))
            {
            AddAppInfoHeaderL(aCtx,header);
            }
        }
    
    CSenElement& body = aMessage->BodyL();
    CSenElement* addressHolderEl = NULL;
    do
        {
        addressHolderEl = 
            CWSStarMessageUtils::GetMarkedElementL(body,  WSStarContextKeys::KAddressAttrMark);
        if (addressHolderEl)
            {
            value = addressHolderEl->AttrValue(WSStarContextKeys::KAddressAttrMark);
            CreateElementAddressingL(*addressHolderEl, KEndpointReferenceName,
                                     *value, ETrue);
            //removing marker
            CSenBaseAttribute* marker = 
                SenXmlUtils::RemoveAttributeL(*addressHolderEl, WSStarContextKeys::KAddressAttrMark);
            delete marker;
            marker = NULL;
            }
            
        }while(addressHolderEl);
    HBufC8* bodyString = body.AsXmlL();
    ((MSenContext&)aCtx).Update(WSStarContextKeys::KBody, *bodyString);
    ((MSenContext&)aCtx).Remove(WSStarContextKeys::KBodyElement);
    delete bodyString;
    bodyString = NULL;
    return KErrNone;    
    }
void CWSStarAddressingHandler::AddAppInfoHeaderL(MSenMessageContext& aCtx, 
                                                CSenElement& aHeader)
{
    const TDesC8* value = NULL;

    const TDesC8* mwsNs = NULL;
    mwsNs = aCtx.GetDesC8L(WSStarContextKeys::KMwsNamespace);
    if (mwsNs)
        {
            //CreateElementAddressingL(header, WSStarContextKeys::KApplication, *value,EFalse);
            CSenElement& aaplication = aHeader.AddElementL(*mwsNs,
                                   WSStarContextKeys::KApplication,
                                   WSStarContextKeys::KNsPApplication);

            value = aCtx.GetDesC8L(WSStarContextKeys::KApplicationName);
            if(value)
            {
                CSenElement& name = aaplication.AddElementL(*mwsNs,
                                            WSStarContextKeys::KName,
                                            WSStarContextKeys::KNsPName);                       
                 name.SetContentL(*value);
            }

            value = aCtx.GetDesC8L(WSStarContextKeys::KApplicationVendor);
            if(value)
            {
                CSenElement& vendor = aaplication.AddElementL(*mwsNs,
                                        WSStarContextKeys::KVendor,
                                        WSStarContextKeys::KNsPVendor);                       
                 vendor.SetContentL(*value);
            }
            value = aCtx.GetDesC8L(WSStarContextKeys::KApplicationVersion);
            if(value)
            {
                CSenElement& version = aaplication.AddElementL(*mwsNs,
                                       WSStarContextKeys::KVersion,
                                       WSStarContextKeys::KNsPVersion);                       
                 version.SetContentL(*value);
            }
            
            CSenElement& deviceoption = aHeader.AddElementL(*mwsNs,
                                   WSStarContextKeys::KDeviceOptions,
                                   WSStarContextKeys::KNsPDeviceOptions);
            
            value = aCtx.GetDesC8L(WSStarContextKeys::KLocale);
            if(value)
            {
                CSenElement& local = deviceoption.AddElementL(*mwsNs,
                                       WSStarContextKeys::KLocale,
                                       WSStarContextKeys::KNsPLocale);                       
                 local.SetContentL(*value);
            }
            
            CSenElement& useragent = deviceoption.AddElementL(*mwsNs,
                                     WSStarContextKeys::KUserAgent,
                                     WSStarContextKeys::KNsPUserAgent);
            
            value = aCtx.GetDesC8L(WSStarContextKeys::KManufacturer);
            if(value)
            {
                CSenElement& manufacturer = useragent.AddElementL(*mwsNs,
                                             WSStarContextKeys::KManufacturer,
                                             WSStarContextKeys::KNsPManufacturer);                       
                 manufacturer.SetContentL(*value);
            }
            value = aCtx.GetDesC8L(WSStarContextKeys::KModel);
            if(value)
            {
                CSenElement& model = useragent.AddElementL(*mwsNs,
                                       WSStarContextKeys::KModel,
                                       WSStarContextKeys::KNsPModel);                       
                 model.SetContentL(*value);
            }
            
            CSenElement& os = useragent.AddElementL(*mwsNs,
                                     WSStarContextKeys::KOS,
                                     WSStarContextKeys::KNsPOS);
         
           value = aCtx.GetDesC8L(WSStarContextKeys::KDeviceName);
            if(value)
            {
                CSenElement& name = os.AddElementL(*mwsNs,
                                    WSStarContextKeys::KName,
                                    WSStarContextKeys::KNsPName);                       
                 name.SetContentL(*value);
            }
           value = aCtx.GetDesC8L(WSStarContextKeys::KDeviceVersion);
            if(value)
            {
                CSenElement& version = os.AddElementL(*mwsNs,
                                        WSStarContextKeys::KVersion,
                                        WSStarContextKeys::KNsPVersion);                       
                version.SetContentL(*value);
            }

                                                 
            value = aCtx.GetDesC8L(WSStarContextKeys::KIMSI);
            if(value)
            {
                CSenElement& imsi = useragent.AddElementL(*mwsNs,
                                        WSStarContextKeys::KIMSI,
                                        WSStarContextKeys::KNsPIMSI);                       
                 imsi.SetContentL(*value);
            }
        }    
    return;
    }
    
SenHandler::THandlerDirection CWSStarAddressingHandler::Direction() const
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarAddressingHandler::Direction()")));
    return SenHandler::EBoth;
    }
SenHandler::THandlerPhase CWSStarAddressingHandler::Phase()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarAddressingHandler::Phase()")));
    return SenHandler::EMessage;
    }

// Constructor
CWSStarAddressingHandler::CWSStarAddressingHandler(MSenHandlerContext& aCtx)
:CSenMessageHandler(aCtx)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarAddressingHandler::\
                        CWSStarAddressingHandler(MSenHandlerContext& aCtx)")));
    }

// Destructor
CWSStarAddressingHandler::~CWSStarAddressingHandler()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarAddressingHandler::~CWSStarAddressingHandler()")));
    delete iVersion;
    }

// Second phase construction.
void CWSStarAddressingHandler::ConstructL()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarAddressingHandler::ConstructL()")));
    
    InitVersionL(NULL);
    }
void CWSStarAddressingHandler::InitVersionL(const TDesC8* aVersion)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarAddressingHandler::InitVersion()")));
    const TDesC8* version = iHandlerContext.GetDesC8L(HandlerContextKey::KVersion);
    if (aVersion && 
        (*aVersion == KAddressingXmlns200408 || 
         *aVersion == KAddressingXmlns200403 ||
         *aVersion == KAddressingXmlns200412 ||
         *aVersion == KAddressingXmlns200502 ||
         *aVersion == KAddressingXmlns200503 ||
         *aVersion == KAddressingXmlns200508))
        {
        delete iVersion;
        iVersion = NULL;
        iVersion = aVersion->AllocL();
        }
    else if(version && 
        (*version == KAddressingXmlns200408 || 
         *version == KAddressingXmlns200403 ||
         *version == KAddressingXmlns200412 ||
         *version == KAddressingXmlns200502 ||
         *version == KAddressingXmlns200503 ||
         *version == KAddressingXmlns200508))
        {
        delete iVersion;
        iVersion = NULL;
        iVersion = version->AllocL();
        }
    else
        {//no version from handler config
        //no version from user
        //so it means some default behavior (new addressing), 
        //only allocate if differen (to safe time)
        if ((!iVersion) || (*iVersion != KAddressingXmlns200508()))
            {
            delete iVersion;
            iVersion = NULL;
            //iVersion = KAddressingXmlns200508().AllocL();
            }
        //else - just leave ai it is
        }
    }
CSenElement* CWSStarAddressingHandler::CreateElementAddressingL(CSenElement& aParentElement, const TDesC8& aLocalName,
                                                        const TDesC8& aContent, TBool aUseAddress)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarAddressingHandler::CreateElementAddressing(CSenElement& aParentElement,\
                     const TDesC8& aLocalName, const TDesC8& aContent, TBool aUseAddress)")));

    CSenElement* newelement = NULL;
    if (!aUseAddress)
        {
        newelement = aParentElement.CreateElementL(KAddressingPrefix(),
                                                                aLocalName);
        CleanupStack::PushL(newelement);
        newelement->SetContentL(aContent);
        }
    else
        {
        newelement = CWSStarAddressingEPR::NewL(aParentElement, aLocalName, aContent, *iVersion);
        CleanupStack::PushL(newelement);
        }
    if (newelement)
        {
        aParentElement.AddElementL(*newelement);
        CleanupStack::Pop(newelement);
        }
/*
    if (aUseAddress)
        {
        CSenElement& element = *newelement;
        CreateElementAddressingL(element,KAddressName,aContent,EFalse);
        }
    else
        {
        newelement->SetContentL(aContent);                
        }*/
    return newelement;    
    }

void CWSStarAddressingHandler::SetMessageIDL(MSenMessageContext& aCtx)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarAddressingHandler::SetMessageID(MSenMessageContext& aCtx)")));
    CSenGuidGen* pGuidGenerator = CSenGuidGen::NewLC();
    HBufC8* messageId = pGuidGenerator->GetRandomGuid8LC();
    TPtr8 messageIdDes = messageId->Des();
    messageIdDes.LowerCase();
    ((MSenContext&)aCtx).Update(WSStarContextKeys::KMessageID,*messageId);
    CleanupStack::PopAndDestroy(messageId);
    CleanupStack::PopAndDestroy(pGuidGenerator);
    }
void CWSStarAddressingHandler::ShareMyLiterals(MSenMessageContext& aCtx)
    {
    if (iVersion)
        {
        ((MSenContext&)aCtx).Update(WSStarContextKeys::KAddressingNsHolder,
                *iVersion);
        ((MSenContext&)aCtx).Update(WSStarContextKeys::KAddressingEndpointTagHolder, KAddressName);
        }
    }
TInt CWSStarAddressingHandler::InitL(MSenHandlerContext& aCtx)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarAddressingHandler::InitL(MSenHandlerContext& aCtx)")));
    iHandlerContext = aCtx;
    return KErrNone;
    }

RFileLogger* CWSStarAddressingHandler::Log() const
    {
    RFileLogger* pLog = NULL;
    TRAP_IGNORE( pLog = (RFileLogger*)iHandlerContext.GetAnyL(HandlerContextKey::KLogger); )
    return pLog;
    }
    
// END OF FILE

