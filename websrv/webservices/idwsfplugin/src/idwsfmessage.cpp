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
#include <SenDateUtils.h>

#include "idwsfmessage.h"

namespace
    {
    _LIT8(KSB11XmlNs,           "urn:liberty:sb:2004-04");
    _LIT8(KStatus,              "Status");
    _LIT8(KStatusCodeAttr,      "code");
    //_LIT8(KStatusOK,          "ok");
    _LIT8(KDetail,              "detail");
    }

CIdWsfMessage* CIdWsfMessage::NewL(const TDesC8& aRefToMessageId)
    {
    CIdWsfMessage* pNew = new (ELeave) CIdWsfMessage;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aRefToMessageId);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CIdWsfMessage::~CIdWsfMessage()
    {
    TInt leaveCode(KErrNone);
    TRAP(leaveCode, delete CSenSoapEnvelope::HeaderL().RemoveElement(KSiuXmlns, KSiuName);)
    leaveCode=0; // not used
    if(ipSiuHeader)
        {
        // We do not own the element, so remove it from the fragment.
        ipSiuHeader->ExtractElement();
        delete ipSiuHeader;
        }
    }

CIdWsfMessage::CIdWsfMessage()
    {
    }

void CIdWsfMessage::BaseConstructL(const TDesC8& aRefToMessageId)
    {
    CIdWsfSoapMessage::BaseConstructL();

    // Add correlation header
    ipCorrelationHeader = CIdWsfCorrelationHeader::NewL(aRefToMessageId);
    AddHeaderL(*ipCorrelationHeader);
    }

void CIdWsfMessage::ParseHeaderL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes
    )
    {
    if ((aNsUri == KSBXmlNs) && (aLocalName == KCorrelationName))
        {
        // Remove existing stuff
        delete HeaderL().RemoveElement(KSBXmlNs, KCorrelationName);

        // Add new correlation header
        ipCorrelationHeader = NULL;
        ipCorrelationHeader = CIdWsfCorrelationHeader::NewL(aAttributes);
        AddHeaderL(*ipCorrelationHeader);
        }
    else if ((aNsUri == KSB11XmlNs) && (aLocalName == KSiuName))
        {
        // Remove existing stuff
        delete HeaderL().RemoveElement(KSB11XmlNs, KSiuName);
        if (ipSiuHeader)
            {
            // We do not own the element, so remove it from the fragment.
            ipSiuHeader->ExtractElement();
            delete ipSiuHeader;
            }
        ipSiuHeader = NULL;
        ipSiuHeader = CIdWsfSiuHeader::NewL();
        DelegateParsingL(*ipSiuHeader);
        AddHeaderL(ipSiuHeader->AsElement());
        }
    else
        {
        CIdWsfSoapMessage::ParseHeaderL(aNsUri, aLocalName, aQName, aAttributes);
        }
    }

void CIdWsfMessage::SetCorrelationL(const TDesC8& aRefToMessageId)
    {
    ipCorrelationHeader->SetCorrelationL(aRefToMessageId);
    }

TPtrC8 CIdWsfMessage::MessageId()
    {
    return ipCorrelationHeader->MessageId();
    }


TPtrC8 CIdWsfMessage::RefToMessageId()
    {
    return ipCorrelationHeader->RefToMessageId();
    }

TPtrC8 CIdWsfMessage::Timestamp()
    {
    return ipCorrelationHeader->Timestamp();
    }

CIdWsfSiuHeader* CIdWsfMessage::SiuHeader()
    {
    return ipSiuHeader;
    }

const TDesC8* CIdWsfMessage::StatusCodeL()
    {
    const TDesC8* pCode = NULL;
    CSenElement* pStatus = NULL;
    // this fetched first child of Body,not yet the status
    CSenElement* pBodyChild = CIdWsfSoapMessage::BodyL().Child(0);

    if (pBodyChild)
        {
        //now we should have the S:Fault/detail element
        pStatus = pBodyChild->Element(KSBXmlNs,KStatus);
        }

    if (!pStatus && pBodyChild)
        {
        //now we should have the S:Fault/detail element
        pBodyChild = pBodyChild->Element(KNullDesC8, KDetail);
        if (pBodyChild)
            {
            //now we should have the S:Fault/detail element
            pStatus = pBodyChild->Element(KSBXmlNs,KStatus);
            }
        }

    if (pStatus)
        {
        pCode = pStatus->AttrValue(KStatusCodeAttr);
        }

    return pCode;
    }

// End of File
