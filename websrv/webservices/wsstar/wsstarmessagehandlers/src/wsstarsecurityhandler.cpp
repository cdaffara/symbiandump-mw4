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













#include <e32std.h>
#include <xmlsecwinit.h>
#include <xmlsecwsign.h>
#include "sencryptoutils.h"
#include "sendebug.h"
#include "SenSoapMessage.h"
#include "SenParser.h"
#include "wsstarsecurityhandler.h"
#include "wsstarcons.h"
#include "wsstarhandlercontext.h"
#include "wsstarmessagehandlerscons.h"
#include "senlogger.h" // private Utils\inc
#include "wsstarmessageutils.h"
using namespace WSStarSecurityFault;

//_LIT8(KSecurityIdPrefix, "SEC_");
_LIT8(KSecurityPrefix, "ds");

// Create instance of concrete ECOM interface implementation
CWSStarSecurityHandler* CWSStarSecurityHandler::NewL(
        MSenHandlerContext* aHandlerCtx)
    {
    
    CWSStarSecurityHandler* self =
            new (ELeave) CWSStarSecurityHandler(*aHandlerCtx);
    return self;
    }

// Constructor
CWSStarSecurityHandler::CWSStarSecurityHandler(MSenHandlerContext& aCtx):
        CSenMessageHandler(aCtx)
    {
    }

// Destructor
CWSStarSecurityHandler::~CWSStarSecurityHandler()
    {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarSecurityHandler::~CWSStarSecurityHandler()")));
    }

TInt CWSStarSecurityHandler::InvokeL(MSenMessageContext& aCtx)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarSecurityHandler::InvokeL(MSenMessageContext& aCtx)")));

    if (aCtx.Direction() == SenContext::EOutgoing)
        {
        const TDesC8* keyB64 = aCtx.GetDesC8L(WSStarContextKeys::KPOPBase64);        
        const TDesC8* keyType = aCtx.GetDesC8L(WSStarContextKeys::KBinaryType);
        if (!keyB64)
            {
            return KErrNone;
            }
        HBufC8* key = SenCryptoUtils::DecodeBase64L(*keyB64);
        if (!key)
            {
            return KErrArgument;
            }
        CleanupStack::PushL(key);

        RArrayElements elements;
        CleanupClosePushL(elements);
        
        CSenDomFragmentBase* base = GetElementsToSignL(aCtx, &elements);
        if (!base)
            {
            CleanupStack::PopAndDestroy(2, key);
            return KErrNone;
            }
        CleanupStack::PushL(base);
        
        base->AsDocumentL().RegisterXmlIdL(KSecurityUtilityId, KSecurityUtilityXmlNs);
        
        if (elements.Count())
            {
            XmlSecPushL();
            //TUint id = 0;
            for (TInt i = 0; i < elements.Count(); i++)
                {
                TXmlEngElement& el = elements[i];
                TXmlEngNamespace ns = el.LookupNamespaceByPrefixL(KSecurityUtilityXmlNsPrefix);
                if (ns.IsUndefined())
                    {
                        ns = el.AddNamespaceDeclarationL(KSecurityUtilityXmlNs, 
                                                            KSecurityUtilityXmlNsPrefix);    
                    }
                else if (ns.Uri() != KSecurityUtilityXmlNs)
                    {
                    User::Leave(KErrCorrupt);
                    }
                    
                 //!!!atention, current id value is absed upn tag name. Below commented code 
                 //generate more uniq name (similar to sequencer)
                //while (!el.HasAttributeL(KSecurityUtilityId, KSecurityUtilityXmlNs))
                    {
                    //TBuf8<12> idStr = KSecurityIdPrefix();
                    //idStr.AppendNumFixedWidth(++id, EHex, 8);
                    //el.AddXmlIdL(KSecurityUtilityId, idStr, ns);
                    el.AddXmlIdL(KSecurityUtilityId, elements[i].Name(), ns);
                    }
                }
            
            CXmlSecSign* sign = CXmlSecSign::NewLC();
            TXmlEngElement signature;
            if (!keyType || //only 4 possible case null, nonce, asymmetric, symmetric.
                (keyType && 
                *keyType == WSStarContextValues::KPOPTypeAsimmetric))
                {
                TLSLOG(KSenFaultsLogChannel, KSenFaultsLogLevel, KUnsupportedAlgorithm);  
                CleanupStack::PopAndDestroy(sign); 
                XmlSecPopAndDestroy();
                CleanupStack::PopAndDestroy(base); 
                CleanupStack::PopAndDestroy(); //elements, it is R class
                CleanupStack::PopAndDestroy(key); 
                return KErrNone;
                }
            else //if nonce , then key is computed and also is simettric
                {
                sign->CreateTemplateL(CXmlSecSign::EHMAC, 
                                        elements, 
                                        KSecurityUtilityWsuId,
                                        EFalse,
                                        CXmlSecSign::KExclusiveC14N,
                                        KSecurityPrefix,
                                        EFalse
                                        );
                sign->SetKeyFromBufferL(*key, KNullDesC8, CXmlSecSign::EHMAC);
                //const TDesC8* str = aCtx.GetDesC8L(WSStarContextKeys::KSTR);
                _LIT8(KSTRstr,"<wsse:SecurityTokenReference \
xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/\
oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
<wsse:Reference wsse:URI=\"token\" />\
</wsse:SecurityTokenReference>");
                const TDesC8* str = &KSTRstr();
                if (str)
                    {
                    CSenDomFragmentBase* strFr = CSenDomFragmentBase::NewL();
                    CleanupStack::PushL(strFr);
                    CSenParser* parser = CSenParser::NewLC();
                    parser->ParseL(*str, *strFr); 
                    sign->SetKeyInfoL(strFr->AsElementL().CopyL());
                    CleanupStack::PopAndDestroy(2, strFr);
                    }
                signature = sign->SignXmlNodesL(elements);
                }
        
            HBufC8* messageXml = base->AsXmlL();
            CleanupStack::PushL(messageXml);
            
            CSenSoapMessage* message=NULL;
            CSenSoapMessage* msg = 
                (CSenSoapMessage*)aCtx.GetSenSoapEnvelopeL(
                                            SenContext::KSenCurrentSoapMessageCtxKey);
            if(msg->SoapVersion() == ESOAP12)
            {
                message = CSenSoapMessage::NewL( ESOAP12, KSecurityXmlNs);
            }
            else
            {
                message = CSenSoapMessage::NewL(ESOAP11, KSecuritySchemeXmlNs );
            }
            CleanupStack::PushL(message);
            CSenXmlReader& reader =   const_cast<CSenXmlReader&>(*aCtx.GetParser());

            message->ParseWithL( reader );
            message->ParseL(*messageXml);
            aCtx.Update(SenContext::KSenCurrentSoapMessageCtxKey, message);
            CleanupStack::Pop(message);
            AttachElementL(aCtx, signature);
            CleanupStack::PopAndDestroy(2, sign);//messagexml, sign
            
            
            XmlSecPopAndDestroy();
            }

        CleanupStack::PopAndDestroy(3, key);//base, elements, key
        }
       return KErrNone;
    }

    
SenHandler::THandlerDirection CWSStarSecurityHandler::Direction() const
    {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarSecurityHandler::Direction()")));
        return SenHandler::EBoth;
    };
SenHandler::THandlerPhase CWSStarSecurityHandler::Phase()
    {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarSecurityHandler::Phase()")));
        return SenHandler::EMessage;
    };    
TInt CWSStarSecurityHandler::InitL(MSenHandlerContext& /*aCtx*/)
    {
    return KErrNone;
    }
    
    
RFileLogger* CWSStarSecurityHandler::Log() const
    {
    RFileLogger* pLog = NULL;
    TRAP_IGNORE( pLog = (RFileLogger*)iHandlerContext.GetAnyL(HandlerContextKey::KLogger); )
    return pLog;
    }
    
//------------------------------------------------------------------------------
// Add signature element to security header
//------------------------------------------------------------------------------
//
void CWSStarSecurityHandler::AttachElementL(MSenMessageContext& aCtx,
        TXmlEngElement& aSignature)
    {
    CSenSoapMessage* message = (CSenSoapMessage*)
            (aCtx.GetSenSoapEnvelopeL(
            SenContext::KSenCurrentSoapMessageCtxKey));
    RBuf8 buffer;
    aSignature.OuterXmlL(buffer);
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(_L("Siganture from buffer")));
    TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(buffer));
    message->AddSecurityTokenL(buffer);
    buffer.Close();
    }
    
//------------------------------------------------------------------------------
// Returns collection of elements which has to be signed. Depenend on policy 
// which determine what to sign
//------------------------------------------------------------------------------
//

CSenDomFragmentBase* CWSStarSecurityHandler::GetElementsToSignL( MSenMessageContext& aCtx, RArrayElements* aElements )
    {
    CSenSoapMessage* msg = (CSenSoapMessage*)aCtx.GetSenSoapEnvelopeL(
            SenContext::KSenCurrentSoapMessageCtxKey);
    if (!msg)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("There is no message in message context")));
        return NULL;
        }
    
    CSenDomFragmentBase* base = CSenDomFragmentBase::NewL();
    CleanupStack::PushL(base);

    HBufC8* xmlMsg = msg->AsXmlL();
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(_L("____parse message in order to find elements____")));
    TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel,(*xmlMsg));
    
    CleanupStack::PushL(xmlMsg);
    
    
    CSenParser* parser = CSenParser::NewLC();
    parser->ParseL(*xmlMsg, *base); 
    CleanupStack::PopAndDestroy(2, xmlMsg);//parser, xmlMsg
    
    TXmlEngElement parent = base->AsElementL();
    
       
    TPtrC8 verDes = KSenSoapEnvelopeXmlns();
    const TInt* ver = aCtx.GetIntL(WSStarContextKeys::KSoapVersion);
    if (!ver || (*ver != ESOAP11))
        {
        verDes.Set(KSenSoap12EnvelopeXmlns());
        }
        
    //determine using global policy        
    RPointerArray<HBufC8>* pTags = (RPointerArray<HBufC8>*)aCtx.GetAnyL(WSStarContextKeys::KSignedPartsArray);
    
    if( pTags )
        {
        TInt count(pTags->Count());
        for (TInt i = 0; i<count; i+=2 )
            {
            TLSLOG_L( KSenCoreServiceManagerLogChannelBase, KMaxLogLevel, "____looking for elements____" );
            HBufC8* tagg = (*pTags)[i];
            HBufC8* taggNs = (*pTags)[i+1];
            TPtrC8 name = tagg->Des();
            TPtrC8 pNs = taggNs->Des();
            TLSLOG( KSenCoreServiceManagerLogChannelBase, KMaxLogLevel, ( *((*pTags)[i]) ) );
            TLSLOG( KSenCoreServiceManagerLogChannelBase, KMaxLogLevel, ( *((*pTags)[i+1]) ) );
            if ( pNs == KNullDesC8() ) // assumption is that this is some soap namespace (like Body)
                {
                pNs.Set( verDes );
                }
            CWSStarMessageUtils::FindElementL( name, pNs, parent, aElements );    
            }
        }
#ifdef _SENDEBUG        
    else // pTags == NULL
        {
        TLSLOG_L( KSenCoreServiceManagerLogChannelBase, KMinLogLevel, "CWSStarSecurityHandler::GetElementsToSignL(MSenMessageContext):" );
        TLSLOG_L( KSenCoreServiceManagerLogChannelBase, KMinLogLevel, "- aCtx.GetAnyL(WSStarContextKeys::KSignedPartsArray) returned NULL(!)" );
        }
#endif // _SENDEBUG        
    CleanupStack::Pop( base );
    return base;
    }


// END OF FILE

