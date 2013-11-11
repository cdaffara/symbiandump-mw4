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
#include <badesca.h>
#include <SenXmlReader.h>
#include <s32mem.h>
#include <utf.h>

#include "SenWsSecurityHeader.h"
#include "SenSoapMessage.h"


EXPORT_C CSenSoapMessage* CSenSoapMessage::NewL()
    {
    CSenSoapMessage* pNew = new (ELeave) CSenSoapMessage;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL();
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenSoapMessage* CSenSoapMessage::NewL(TSOAPVersion aVersion)
    {
    CSenSoapMessage* pNew = new (ELeave) CSenSoapMessage;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aVersion);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenSoapMessage* CSenSoapMessage::NewL(TSOAPVersion aVersion, const TDesC8& aSecurityNs)
    {
    CSenSoapMessage* pNew = new (ELeave) CSenSoapMessage;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aVersion, aSecurityNs);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

EXPORT_C CSenSoapMessage::~CSenSoapMessage()
    {
    if (ipSecurityHeader)
        {
        // We do not own the element, so remove it from the fragment.
        ipSecurityHeader->ExtractElement();
        delete ipSecurityHeader;
        }
    }
EXPORT_C CSenSoapMessage::CSenSoapMessage()
: ipSecurityHeader(NULL)
    {
    }

EXPORT_C void CSenSoapMessage::BaseConstructL()
    {
    CSenSoapEnvelope::BaseConstructL();
    }
EXPORT_C void CSenSoapMessage::BaseConstructL(TSOAPVersion aVersion)
    {
    CSenSoapEnvelope::BaseConstructL(aVersion);
    }

EXPORT_C void CSenSoapMessage::BaseConstructL(TSOAPVersion aVersion, const TDesC8& aSecurityNs)
    {
    BaseConstructL(aVersion);
    if ((aSecurityNs == KSecuritySchemeXmlNs) ||(aSecurityNs == KSecurityXmlNs))
        {
        ipSecurityHeader = CSenWsSecurityHeader::NewL(KNullDesC8, aSecurityNs);
        CSenElement& soapHeader = HeaderL();
        soapHeader.AddElementL(ipSecurityHeader->AsElement());
        }
    }

EXPORT_C void CSenSoapMessage::SetSecurityHeaderL(const TDesC8& aData)
    {
    CSenWsSecurityHeader* pTemp = NULL;
    
    if(aData==KNullDesC8)
        {
        // we don't have credential
        pTemp = NewSecurityHeaderLC(NULL);
        }
    else
        {
        // there is a credential to add..
        pTemp = NewSecurityHeaderLC(&aData);
        }

    delete HeaderL().RemoveElement(pTemp->XmlNs(), KSecurityName);
    if (ipSecurityHeader)
        {
        // We do not own the element, so remove it from the fragment.
        ipSecurityHeader->ExtractElement();
        delete ipSecurityHeader;
        ipSecurityHeader = NULL;
        }
    ipSecurityHeader = pTemp;
    CleanupStack::Pop(); // pTemp
    this->AddHeaderL(ipSecurityHeader->AsElement());
    }

EXPORT_C void CSenSoapMessage::ParseHeaderL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes
    )
    {
    CSenWsSecurityHeader* pTemp = NewSecurityHeaderLC();

    if ((pTemp->XmlNs() == aNsUri) && (KSecurityName() == aLocalName))
        {
        // Remove existing <Security> header
        delete HeaderL().RemoveElement(pTemp->XmlNs(), KSecurityName);
        if (ipSecurityHeader)
            {
            // We do not own the element, so remove it from the fragment.
            ipSecurityHeader->ExtractElement();
            delete ipSecurityHeader;
            ipSecurityHeader = NULL;
            }
        ipSecurityHeader = pTemp;
        CleanupStack::Pop(); // pTemp
        DelegateParsingL(*ipSecurityHeader);
        this->AddHeaderL(ipSecurityHeader->AsElement());
        }
    else
        {
        CleanupStack::PopAndDestroy(); // pTemp
        CSenSoapEnvelope::ParseHeaderL(
            aNsUri, 
            aLocalName, 
            aQName, 
            aAttributes);
        }
    }

EXPORT_C TInt CSenSoapMessage::AddSecurityTokenL(const TDesC8& aNewToken)
    {
    CSenElement& soapHeader = HeaderL();

    CSenWsSecurityHeader* pHeader = NewSecurityHeaderLC();

    CSenElement* wsSecurityHeader = NULL;
    wsSecurityHeader = soapHeader.Element(pHeader->XmlNs(), KSecurityName);

    if(!wsSecurityHeader)
        {
        wsSecurityHeader = pHeader->ExtractElement(); // Let's take ownership
        soapHeader.AddElementL(*wsSecurityHeader);    // takes ownership
        }
    CleanupStack::PopAndDestroy(); // pHeader // safe to delete

    HBufC8* pContent = HBufC8::NewLC(wsSecurityHeader->Content().Length() +
                                aNewToken.Length());

    TPtr8 content = pContent->Des();
    content.Append(wsSecurityHeader->Content());
    content.Append(aNewToken);
    wsSecurityHeader->SetContentL(content);
    CleanupStack::PopAndDestroy(); // pContent

    return KErrNone;
    }

EXPORT_C CSenWsSecurityHeader* CSenSoapMessage::NewSecurityHeaderLC(
                                                const TDesC8* aData)
    {
    CSenWsSecurityHeader* pNew = NULL;
    if(aData)
        {
        if (ipSecurityHeader)
            {
            pNew = CSenWsSecurityHeader::NewL(*aData, ipSecurityHeader->XmlNs());    
            }
        else
            {
            pNew = CSenWsSecurityHeader::NewL(*aData);
            }
        
        }
    else
        {
        if (ipSecurityHeader)
            {
            pNew = CSenWsSecurityHeader::NewL(KNullDesC8, ipSecurityHeader->XmlNs());    
            }
        else
            {
            pNew = CSenWsSecurityHeader::NewL();    
            }
    }
    CleanupStack::PushL(pNew);
    return pNew;
    }

// End of File
