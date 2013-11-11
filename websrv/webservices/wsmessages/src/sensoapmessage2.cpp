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
#include <xml/dom/xmlengnodelist.h>
#include <badesca.h>
#include <SenXmlReader.h>
#include <s32mem.h>
#include <utf.h>

#include "SenWsSecurityHeader2.h"
#include "SenSoapMessage2.h"

#include "sendebug.h"
#include "senlogger.h"


EXPORT_C CSenSoapMessage2* CSenSoapMessage2::NewL()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::NewL()");
    CSenSoapMessage2* pNew = CSenSoapMessage2::NewLC();
    CleanupStack::Pop(pNew);
    return pNew;    
    }

EXPORT_C CSenSoapMessage2* CSenSoapMessage2::NewLC()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::NewLC()");
    CSenSoapMessage2* pNew = new (ELeave) CSenSoapMessage2();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    return pNew;
    }

EXPORT_C CSenSoapMessage2* CSenSoapMessage2::NewL(TSOAPVersion aVersion)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::NewL(aVersion)");
    CSenSoapMessage2* pNew = CSenSoapMessage2::NewLC(aVersion);
    CleanupStack::Pop(pNew);
    return pNew;    
    }

EXPORT_C CSenSoapMessage2* CSenSoapMessage2::NewLC(TSOAPVersion aVersion)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::NewLC(aVersion)");
    CSenSoapMessage2* pNew = new (ELeave) CSenSoapMessage2();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aVersion);
    return pNew;
    }

EXPORT_C CSenSoapMessage2* CSenSoapMessage2::NewL(TSOAPVersion aVersion,
                                                  const TDesC8& aSecurityNs)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::NewL(aVersion,aSecurityNs)");
    CSenSoapMessage2* pNew = CSenSoapMessage2::NewLC(aVersion, aSecurityNs);
    CleanupStack::Pop(pNew);
    return pNew;    
    }

EXPORT_C CSenSoapMessage2* CSenSoapMessage2::NewLC(TSOAPVersion aVersion,
                                                   const TDesC8& aSecurityNs)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::NewLC(aVersion,aSecurityNs)");
    CSenSoapMessage2* pNew = new (ELeave) CSenSoapMessage2();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aVersion, aSecurityNs);
    return pNew;
    }

EXPORT_C CSenSoapMessage2* CSenSoapMessage2::NewL(MSenMessageContext& aContext)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::NewL(aContext)");
    CSenSoapMessage2* pNew = CSenSoapMessage2::NewLC(aContext);
    CleanupStack::Pop(pNew);
    return pNew;    
    }

EXPORT_C CSenSoapMessage2* CSenSoapMessage2::NewLC(MSenMessageContext& aContext)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::NewLC(aContext)");
    CSenSoapMessage2* pNew = new (ELeave) CSenSoapMessage2();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aContext);
    return pNew;
    }

EXPORT_C CSenSoapMessage2* CSenSoapMessage2::NewL(MSenMessageContext& aContext,
                                                  TSOAPVersion aVersion)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::NewL(aContext,aVersion)");
    CSenSoapMessage2* pNew = CSenSoapMessage2::NewLC(aContext, aVersion);
    CleanupStack::Pop(pNew);
    return pNew;    
    }

EXPORT_C CSenSoapMessage2* CSenSoapMessage2::NewLC(MSenMessageContext& aContext,
                                                   TSOAPVersion aVersion)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::NewLC(aContext,aVersion)");
    CSenSoapMessage2* pNew = new (ELeave) CSenSoapMessage2();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aContext, aVersion);
    return pNew;
    }

EXPORT_C CSenSoapMessage2* CSenSoapMessage2::NewL(MSenMessageContext& aContext,
                                                  TSOAPVersion aVersion,
                                                  const TDesC8& aSecurityNs)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::NewL(aContext,aVersion,aSecurityNs)");
    CSenSoapMessage2* pNew = CSenSoapMessage2::NewLC(aContext,
                                                      aVersion,
                                                      aSecurityNs);
    CleanupStack::Pop(pNew);
    return pNew;    
    }

EXPORT_C CSenSoapMessage2* CSenSoapMessage2::NewLC(MSenMessageContext& aContext,
                                                  TSOAPVersion aVersion,
                                                  const TDesC8& aSecurityNs)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::NewLC(aContext,aVersion,aSecurityNs)");
    CSenSoapMessage2* pNew = new (ELeave) CSenSoapMessage2();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aContext, aVersion, aSecurityNs);
    return pNew;
    }
    
EXPORT_C CSenSoapMessage2* CSenSoapMessage2::NewL(CSenSoapMessage2& aMessage)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::NewL(aMessage)");
    CSenSoapMessage2* pNew = CSenSoapMessage2::NewLC(aMessage);
    CleanupStack::Pop(pNew); // pNew
    return pNew;    
    }

EXPORT_C CSenSoapMessage2* CSenSoapMessage2::NewLC(CSenSoapMessage2& aMessage)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::NewLC(aMessage)");
    CSenSoapMessage2* pNew = new (ELeave) CSenSoapMessage2();
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aMessage);
    return pNew;
    }
    
EXPORT_C CSenSoapMessage2::~CSenSoapMessage2()
    {
    if (ipSecurityHeader)
        {
        // We do not own the element, so remove it from the fragment.
        ipSecurityHeader->ExtractElement();
        delete ipSecurityHeader;
        }
    }
    
EXPORT_C CSenSoapMessage2::CSenSoapMessage2()
: ipSecurityHeader(NULL)
    {
    }

EXPORT_C void CSenSoapMessage2::BaseConstructL()
    {
    CSenSoapEnvelope2::BaseConstructL();
    }

EXPORT_C void CSenSoapMessage2::BaseConstructL(TSOAPVersion aVersion)
    {
    CSenSoapEnvelope2::BaseConstructL(aVersion);
    }

EXPORT_C void CSenSoapMessage2::BaseConstructL(TSOAPVersion aVersion, const TDesC8& aSecurityNs)
    {
    BaseConstructL(aVersion);
    if ( ( aSecurityNs == KSecuritySchemeXmlNs ) || ( aSecurityNs == KSecurityXmlNs ) )
        {
        TXmlEngElement element = AsElementL();
        RSenDocument document = AsDocumentL();
        ipSecurityHeader = CSenWsSecurityHeader2::NewL(KNullDesC8,
                                                       aSecurityNs,
                                                       document,
                                                       element);
        this->AddHeaderL(ipSecurityHeader->AsElementL());
        }
    }

EXPORT_C void CSenSoapMessage2::BaseConstructL(MSenMessageContext& aContext)
    {
    CSenSoapEnvelope2::BaseConstructL(aContext);
    }

EXPORT_C void CSenSoapMessage2::BaseConstructL(MSenMessageContext& aContext,
                                               TSOAPVersion aVersion)
    {
    CSenSoapEnvelope2::BaseConstructL(aContext, aVersion);
    }

EXPORT_C void CSenSoapMessage2::BaseConstructL(MSenMessageContext& aContext,
                                               TSOAPVersion aVersion,
                                               const TDesC8& aSecurityNs)
    {
    BaseConstructL(aContext, aVersion);
    if ( ( aSecurityNs == KSecuritySchemeXmlNs ) || ( aSecurityNs == KSecurityXmlNs ) )
        {
        TXmlEngElement element = AsElementL();
        RSenDocument document = AsDocumentL();
        ipSecurityHeader = CSenWsSecurityHeader2::NewL(KNullDesC8,
                                                       aSecurityNs,
                                                       document,
                                                       element);
        this->AddHeaderL(ipSecurityHeader->AsElementL());
        }
    }
    
EXPORT_C void CSenSoapMessage2::BaseConstructL(CSenSoapMessage2& aMessage)
    {
    CSenSoapEnvelope2::BaseConstructL(aMessage);
    }

EXPORT_C void CSenSoapMessage2::SetSecurityHeaderL(const TDesC8& aData)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::SetSecurityHeaderL(aData)");
    CSenWsSecurityHeader2* pTemp = NULL;
    TXmlEngElement element = AsElementL();
    RSenDocument document = AsDocumentL();

    if ( aData == KNullDesC8 )
        {
        // we don't have credential
        pTemp = NewSecurityHeaderLC(NULL, document, element);
        }
    else
        {
        // there is a credential to add..
        pTemp = NewSecurityHeaderLC(&aData, document, element);
        }

    TXmlEngElement header = HeaderL();
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);
    header.GetElementsByTagNameL(list, KSecurityName,
                                       pTemp->XmlNs());
    while ( list.HasNext() )
        {
        TXmlEngElement element = list.Next();
        element.Remove();
        }

    if ( ipSecurityHeader )
        {
        delete ipSecurityHeader;
        ipSecurityHeader = NULL;
        }
    
    CleanupStack::PopAndDestroy(&list);
        
    ipSecurityHeader = pTemp;
    CleanupStack::Pop(); // pTemp
    this->AddHeaderL(ipSecurityHeader->AsElementL());
    }

EXPORT_C void CSenSoapMessage2::ParseHeaderL( const RTagInfo& aElement,
                                              const RAttributeArray& aAttributes)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::ParseHeaderL");
    const TPtrC8 saxLocalName   = aElement.LocalName().DesC();
    const TPtrC8 saxNsUri       = aElement.Uri().DesC();
    const TPtrC8 saxPrefix      = aElement.Prefix().DesC();
    
    TXmlEngElement element = AsElementL();
    RSenDocument document = AsDocumentL();
    
    CSenWsSecurityHeader2* pTemp = NewSecurityHeaderLC(NULL, document, element);

    if ((pTemp->XmlNs() == saxNsUri) && (KSecurityName() == saxLocalName))
        {
        // Remove existing <Security> header
        TXmlEngElement header = HeaderL();
        RXmlEngNodeList<TXmlEngElement> list;
        CleanupClosePushL(list);
        header.GetElementsByTagNameL(list, KSecurityName, pTemp->XmlNs());
        while ( list.HasNext() )
            {
            TXmlEngElement element = list.Next();
            element.Remove();
            }
            
        CleanupStack::PopAndDestroy(&list);

        if ( ipSecurityHeader )
            {
            delete ipSecurityHeader;
            ipSecurityHeader = NULL;
            }
            
        ipSecurityHeader = pTemp;
        CleanupStack::Pop(); // pTemp
        
        TXmlEngElement wsSecurityHeader = ipSecurityHeader->AsElementL();
        wsSecurityHeader.MoveTo(header);
        OnDelegateParsingL(*ipSecurityHeader);        
        }
    else
        {
        TXmlEngElement rootElement = pTemp->AsElementL();
        rootElement.Remove();
        CleanupStack::PopAndDestroy(); // pTemp
        CSenSoapEnvelope2::ParseHeaderL(aElement, aAttributes);
        }
    }

EXPORT_C TInt CSenSoapMessage2::AddSecurityTokenL(const TDesC8& aNewToken)
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::AddSecurityTokenL(aNewToken)");
    TXmlEngElement element = AsElementL();
    RSenDocument document = AsDocumentL();
    TXmlEngElement soapHeader = HeaderL();
    TXmlEngElement wsSecurityHeader;

    // Create <wsse:Security> element into wrong place
    // <=> into a root element = Envelope.
    // <S:Envelope>
    //   ...
    //   <wsse:Security>
    // That's because we don't want following search to find
    // this new header inside <S:Header> element.
    CSenWsSecurityHeader2* pHeader = NewSecurityHeaderLC(NULL, document, element);
    RXmlEngNodeList<TXmlEngElement> list;
    CleanupClosePushL(list);
    soapHeader.GetElementsByTagNameL(list, KSecurityName,
                                           pHeader->XmlNs());
                                          
    if ( !list.HasNext() )
        {
        // <wsse:Security> element was not found from <S:Header> element.
        // => Add new header by moving header from (root) <S:Envelope>
        //    element into <S:Header> element.
        wsSecurityHeader = pHeader->ExtractElement();
        wsSecurityHeader.MoveTo(soapHeader);
        }
    else
        {
        // <wsse:Security> element was found from <S:Header> element.
        // => Delete new header element from SoapMessage DOM tree by
        //    removing header from (root) <S:Envelope> element.
        wsSecurityHeader = pHeader->ExtractElement();
        wsSecurityHeader.Remove();
        
        // Select found <wsse:Security> element to be edited
        wsSecurityHeader = list.Next();
        }
        
    CleanupStack::PopAndDestroy(&list);

    CleanupStack::PopAndDestroy(); // pHeader // safe to delete

    TPtrC8 content = wsSecurityHeader.Text();
    HBufC8* pContent = HBufC8::NewLC( content.Length() + aNewToken.Length() );
    TPtr8 ptrContent = pContent->Des();
    ptrContent.Append(content);
    ptrContent.Append(aNewToken);
    wsSecurityHeader.SetTextNoEncL(*pContent);
    CleanupStack::PopAndDestroy(pContent);    

    return KErrNone;
    }

EXPORT_C CSenWsSecurityHeader2* CSenSoapMessage2::NewSecurityHeaderLC( const TDesC8* aData,
                                                                       RSenDocument aDocument,
                                                                       TXmlEngElement element )
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::NewSecurityHeaderLC");
    CSenWsSecurityHeader2* pNew = NULL;

    if ( aData )
        {
        if ( ipSecurityHeader )
            {
            pNew = CSenWsSecurityHeader2::NewL(*aData, ipSecurityHeader->XmlNs(), aDocument, element);
            }
        else
            {
            pNew = CSenWsSecurityHeader2::NewL(*aData, aDocument, element);
            }
        }
    else
        {
        if ( ipSecurityHeader )
            {
            pNew = CSenWsSecurityHeader2::NewL(KNullDesC8, ipSecurityHeader->XmlNs(), aDocument, element);
            }
        else
            {
            pNew = CSenWsSecurityHeader2::NewL(aDocument, element);
            }
        }

    CleanupStack::PushL(pNew);
    return pNew;
    }
    
EXPORT_C MSenMessage::TClass CSenSoapMessage2::Type()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::Type");
    return MSenMessage::ESoapMessage2;
    }
    
EXPORT_C MSenMessage* CSenSoapMessage2::CloneL()
    {
    TLSLOG_L(KSenMessagesLogChannel, KMinLogLevel, "CSenSoapMessage2::CloneL");
    CSenSoapMessage2* pMessage = CSenSoapMessage2::NewL(*this);
    return pMessage;
    }
    
// End of File
