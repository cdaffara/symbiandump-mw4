/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of TDmAdCertUtil.
*
*/



#include <x509cert.h>
#include <x509keys.h>
#include <x500dn.h>

#include "dmadcertutil.h"
#include "pkcs10.h"
#include "utlcrypto.h"


void TDmAdCertUtil::ParseAvaL(const CX520AttributeTypeAndValue& aAva, TDes8& aOut)
    {
    TPtrC8 attr = CPkcs10Req::AttributeTypeOidToText(aAva.Type());
    TBuf8<32> tempOidBuf;
    HBufC* val = NULL;
    TRAPD(ret, val = aAva.ValueL());
    if(ret == KErrNone)
        {
        CleanupStack::PushL(val);
        if(attr.Length() == 0)
            {
            // Dotted oid representation
            tempOidBuf.Copy(aAva.Type());
            aOut.Append(tempOidBuf);
            }
        else
            {
            aOut.Append(attr);
            }
        aOut.Append(_L("="));
        aOut.Append(val->Des());
        CleanupStack::PopAndDestroy(val);
        }
    }

void TDmAdCertUtil::CertDnL(const CX500DistinguishedName& aName, TDes8& aOut)
    {
    TInt count = aName.Count();
    for (TInt i = 0; i < count; i++)
        {
        if(i > 0)
            aOut.Append(_L(","));
        const CX520AttributeTypeAndValue& ava = aName.Element(i);
        ParseAvaL(ava, aOut);
        }
    }

HBufC8* TDmAdCertUtil::Sha1DigestL(const TDesC8& aData)
    {
    CUtlMessageDigest* digester = TUtlCrypto::MakeMessageDigesterL(TUtlCrypto::EUtlMessageDigestSha1);
    CleanupStack::PushL(digester);

    HBufC8* hashBuf = HBufC8::NewLC(digester->HashSize());
    TPtr8 hashBufDesc(hashBuf->Des());
    
    hashBufDesc.Copy(digester->Final(aData));
    CleanupStack::Pop(hashBuf);
    CleanupStack::PopAndDestroy(digester);
    return hashBuf;
    }

HBufC8* TDmAdCertUtil::RSAKeyIdentifierLC(const TDesC8& aKeyData)
    {
    TX509KeyFactory keyFactory;
    CRSAPublicKey* rsaKey = keyFactory.RSAPublicKeyL(aKeyData);
    CleanupStack::PushL(rsaKey);
    HBufC8* modulusBuffer = rsaKey->N().BufferLC();
    HBufC8* hash = TDmAdCertUtil::Sha1DigestL(*modulusBuffer);
    CleanupStack::PopAndDestroy(2); // modulusBuffer, rsaKey
    CleanupStack::PushL(hash);
    return hash;
    }
