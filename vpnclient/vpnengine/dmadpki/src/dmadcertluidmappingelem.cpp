/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CDmAdCertLuidMappingElem.
*
*/


//#include <utf.h>

#include "dmadcertluidmappingelem.h"
#include "utlcrypto.h"
#include "base64.h"
#include "vpnlogger.h"

CDmAdCertLuidMappingElem* CDmAdCertLuidMappingElem::NewL(const TDesC8& aIssuerName, const TDesC8& aSerialNumber)
    {
    TRACE("CDmAdCertLuidMappingElem::NewL");
    
    CDmAdCertLuidMappingElem* self = NewLC(aIssuerName, aSerialNumber);
    CleanupStack::Pop(self);
    return self;
    }

CDmAdCertLuidMappingElem* CDmAdCertLuidMappingElem::NewLC(const TDesC8& aIssuerName, const TDesC8& aSerialNumber)
    {
    CDmAdCertLuidMappingElem* self = new (ELeave) CDmAdCertLuidMappingElem();
    CleanupStack::PushL(self);
    self->ConstructL(aIssuerName, aSerialNumber);
    return self;
    }

void CDmAdCertLuidMappingElem::ConstructL(const TDesC8& aIssuerName, const TDesC8& aSerialNumber)
    {
    TRACE("CDmAdCertLuidMappingElem::ConstructL");
    
    iIssuerName = aIssuerName.AllocL();
    iSerialNumber = aSerialNumber.AllocL();
    iLuid = BuildLuidL(aIssuerName, aSerialNumber);
    }
    
CDmAdCertLuidMappingElem::CDmAdCertLuidMappingElem()
    {
    }
    
CDmAdCertLuidMappingElem::~CDmAdCertLuidMappingElem()
    {
    TRACE("CDmAdCertLuidMappingElem::~CDmAdCertLuidMappingElem");
    
    delete iLuid;
    delete iIssuerName;
    delete iSerialNumber;
    }

void CDmAdCertLuidMappingElem::CleanupOperationDeleteCArrayPtr(TAny* aPtr)
    {
    TRACE("CDmAdCertLuidMappingElem::CleanupOperationDeleteCArrayPtr");
    
    if (!aPtr)
        {
        return;
        }
    CArrayPtr<CDmAdCertLuidMappingElem>* array = reinterpret_cast<CArrayPtr<CDmAdCertLuidMappingElem>*>(aPtr);
    array->ResetAndDestroy();
    delete array;
    }

TPtrC8 CDmAdCertLuidMappingElem::Luid() const
    {
    TRACE("CDmAdCertLuidMappingElem::Luid");
    
    TPtrC8 ret(KNullDesC8);
    if (iLuid != 0)
        {
        ret.Set(*iLuid);
        }
    return ret;
    }

TPtrC8 CDmAdCertLuidMappingElem::IssuerName() const
    {
    TRACE("CDmAdCertLuidMappingElem::IssuerName");
    
    TPtrC8 ret(KNullDesC8);
    if (iIssuerName != 0)
        {
        ret.Set(*iIssuerName);
        }
    return ret;
    }

TPtrC8 CDmAdCertLuidMappingElem::SerialNumber() const
    {
    TRACE("CDmAdCertLuidMappingElem::SerialNumber");
    
    TPtrC8 ret(KNullDesC8);
    if (iSerialNumber != 0)
        {
        ret.Set(*iSerialNumber);
        }
    return ret;
    }

HBufC8* CDmAdCertLuidMappingElem::BuildLuidL(const TDesC8& aIssuerName, const TDesC8& aSerialNumber)
    {
    TRACE("CDmAdCertLuidMappingElem::BuildLuidL");
    
    CUtlMessageDigest* digester = TUtlCrypto::MakeMessageDigesterL(TUtlCrypto::EUtlMessageDigestSha1);
    CleanupStack::PushL(digester);

    HBufC8* hashBuf = HBufC8::NewLC(digester->HashSize());
    TPtr8 hashBufDesc(hashBuf->Des());

    digester->Update(aIssuerName);
    hashBufDesc.Copy(digester->Final(aSerialNumber));

    TBase64Codec base64Codec;
    HBufC8* luidB64;
    if ((luidB64 = base64Codec.Base64EncodeLC(*hashBuf)) == NULL)
        {
        DEBUG_LOG(_L("No memory"));        
        User::Leave(KErrNoMemory);
        }

    CleanupStack::Pop(luidB64);
    CleanupStack::PopAndDestroy(2); //hashBuf, digester
    return luidB64;
    }
