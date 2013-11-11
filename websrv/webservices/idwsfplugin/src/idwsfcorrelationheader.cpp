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
#include <SenXmlUtils.h>

#include "idwsfcorrelationheader.h"
#include "senguidgen.h"
#include "idwsfmessage.h"


namespace
    {
    _LIT8(KAttrMessageId, "messageID");
    _LIT8(KAttrRefToMessageId, "refToMessageID");
    _LIT8(KAttrTimestamp, "timestamp");
    }

CIdWsfCorrelationHeader* CIdWsfCorrelationHeader::NewL(
                                                const TDesC8& aRefToMessageId)
    {
    CIdWsfCorrelationHeader* pNew = new (ELeave) CIdWsfCorrelationHeader;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aRefToMessageId);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CIdWsfCorrelationHeader* CIdWsfCorrelationHeader::NewL(
                                            const RAttributeArray& aAttributes)
    {
    CIdWsfCorrelationHeader* pNew = new (ELeave) CIdWsfCorrelationHeader;
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aAttributes);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CIdWsfCorrelationHeader::CIdWsfCorrelationHeader()
    {
    }

void CIdWsfCorrelationHeader::ConstructL(const TDesC8& aRefToMessageId)
    {
    BaseConstructL(KSBXmlNs, KCorrelationName, KCorrelationQName);

    CSenGuidGen* pGuidGenerator = CSenGuidGen::NewLC();
    ipMessageId = pGuidGenerator->GetRandomGuid8LC();
    CleanupStack::Pop();            // ipMessageId
    CleanupStack::PopAndDestroy(); // pGuidGenerator

    ipRefToMessageId = aRefToMessageId.AllocL();

    // Init timestamp
    TTime t;
    t.UniversalTime();
    TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts;
    SenDateUtils::ToXmlDateTimeUtf82L(ts, t);
    ipTimestamp = ts.AllocL();
    }

void CIdWsfCorrelationHeader::ConstructL(const RAttributeArray& aAttributes)
    {
    BaseConstructL(KSBXmlNs, KCorrelationName);

    ipMessageId = SenXmlUtils::AllocAttrValueL(aAttributes, KAttrMessageId);
    ipRefToMessageId = SenXmlUtils::AllocAttrValueL(aAttributes, KAttrRefToMessageId);
    ipTimestamp = SenXmlUtils::AllocAttrValueL(aAttributes, KAttrTimestamp);

    __ASSERT_ALWAYS(ipMessageId != NULL, User::Leave(KErrArgument));
    __ASSERT_ALWAYS(ipTimestamp != NULL, User::Leave(KErrArgument));
    }

CIdWsfCorrelationHeader::~CIdWsfCorrelationHeader()
    {
    delete ipMessageId;
    delete ipRefToMessageId;
    delete ipTimestamp;
    }

TPtrC8 CIdWsfCorrelationHeader::MessageId()
    {
    return *ipMessageId;
    }

TPtrC8 CIdWsfCorrelationHeader::RefToMessageId()
    {
    if (!ipRefToMessageId)
        {
        return KNullDesC8();
        }
    else
        {
        return *ipRefToMessageId;
        }
    }

TPtrC8 CIdWsfCorrelationHeader::Timestamp()
    {
    if(ipTimestamp)
        return *ipTimestamp;
    else
        {
        return KNullDesC8(); // should not happen(!)
        }
    }

void CIdWsfCorrelationHeader::SetCorrelationL(const TDesC8& aRefToMessageId)
    {
    HBufC8* pRefToMessageId = NULL;aRefToMessageId.AllocL();
    delete ipRefToMessageId;
    ipRefToMessageId = pRefToMessageId;
    }

void CIdWsfCorrelationHeader::WriteAttrsToL(RWriteStream& aWriteStream)
    {
    WriteAttrToL(aWriteStream, KAttrMessageId, *ipMessageId);

    // check that reftomessageid exists and is over zero-length
    // IOP: empty reftomessageid is not allowed by specs(!):
    if (ipRefToMessageId && ipRefToMessageId->Length()>0)
        {
        WriteAttrToL(aWriteStream, KAttrRefToMessageId, *ipRefToMessageId);
        }
    WriteAttrToL(aWriteStream, KAttrTimestamp, *ipTimestamp);
    }

// End of File


