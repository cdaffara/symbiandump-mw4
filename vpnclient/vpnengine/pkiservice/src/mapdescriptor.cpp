/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* A data object for CPKIMapper class thar holds the information 
* required to map API set to use the storage model which is not 
* native for that API.
*
*/
#include <x509cert.h>
#include <asymmetric.h>

#include "mapdescriptor.h"
#include "pkiserviceclientservercommon.h"
#include "pkiserviceassert.h"
#include "pkcs10.h"

_LIT8(KEmptyDescriptor, "");

CMapDescriptor* CMapDescriptor::NewL(const TDesC& aLabel,
                                     const CX509Certificate& aCertificate,
                                     TPKICertificateOwnerType aOwnerType,
                                     TPkiServiceStoreType aCertStoreType)
    {
    CMapDescriptor* self = new (ELeave) CMapDescriptor;
    CleanupStack::PushL(self);
    self->ConstructL(aLabel, aCertificate,
                     aOwnerType, aCertStoreType);
    CleanupStack::Pop(self);
    
    return self;
    }


void CMapDescriptor::ConstructL(const TDesC& aLabel,
                                const CX509Certificate& aCertificate,
                                TPKICertificateOwnerType aOwnerType,
                                TPkiServiceStoreType aCertStoreType)
    {
    iLabel = aLabel;
    iCertStoreType = aCertStoreType;
    iOwnerType = aOwnerType;


    iIssuerName = aCertificate.DataElementEncoding(CX509Certificate::EIssuerName)->AllocL();
    iSubjectName = aCertificate.DataElementEncoding(CX509Certificate::ESubjectName)->AllocL();    

    // Copy rfc822 name from subjectAlt name    
    const CX509CertExtension* subjAltName = aCertificate.Extension(KSubjectAltName);
    if(subjAltName != NULL)
        {
        CX509AltNameExt* subjectAlt = CX509AltNameExt::NewLC(subjAltName->Data());
        if(subjectAlt != NULL)
            {
            const CArrayPtrFlat<CX509GeneralName> *nameArray; 
            nameArray = &subjectAlt->AltName();
            // Search rfc822
            for(TInt i = 0; i < nameArray->Count(); i++)
                {
                if(nameArray->At(i)->Tag() == EX509RFC822Name)
                    {
                    TPtrC8 data = nameArray->At(i)->Data();
                    iRfc822Name = data.Right(data.Length() - 2).AllocL();
                    break;
                    }
                }                            
            }
        CleanupStack::PopAndDestroy(subjectAlt);
        }
    
    
    
    
    // Serial number
    const TPtrC8* serial = aCertificate.DataElementEncoding(CX509Certificate::ESerialNumber);
    if(serial != NULL)
        {
        iSerialNumber = serial->AllocL();               
        }
    else
        {
        iSerialNumber = KEmptyDescriptor().AllocL();
        }        
    
    // Validity period
    iStartTime = aCertificate.ValidityPeriod().Start();
    iEndTime = aCertificate.ValidityPeriod().Finish();    
        
    if (EPKICACertificate == iOwnerType)
        {
        iKeyId = aCertificate.SubjectKeyIdentifierL();
        }
    else
        {
        iKeyId = aCertificate.KeyIdentifierL();
        }                
    
    const CSubjectPublicKeyInfo& publicKeyInfo = aCertificate.PublicKey();            
    const TPtrC8 keyData = publicKeyInfo.KeyData();

    TX509KeyFactory keyFactory;
    switch(publicKeyInfo.AlgorithmId())
        {
        case ERSA:
            {                        
            iKeyAlgorithm = EPKIRSA;
            const CRSAPublicKey* keyRSA = keyFactory.RSAPublicKeyL( keyData );
            const TInteger&  n = keyRSA->N();
            iKeySize = n.BitCount();
            delete keyRSA;                    
            }
            break;          
        case EDSA:
            {                        
            iKeyAlgorithm = EPKIDSA;
            TPtrC8 params = publicKeyInfo.EncodedParams();
            const CDSAPublicKey* keyDSA = keyFactory.DSAPublicKeyL( params, keyData );
            const TInteger& y = keyDSA->Y();
            iKeySize = y.BitCount();
            delete keyDSA;
            }
            break;
        default:
            User::Leave(KErrNotSupported);
            break;                    
        }                    
    
    
    }


CMapDescriptor::~CMapDescriptor()
    {
    delete iIssuerName;
    delete iSubjectName;
    delete iRfc822Name;
    delete iSerialNumber;
    iApplUids.Close();
    }

TBool CMapDescriptor::IsMatchingL(TSecurityObjectDescriptor &aDesc, 
                                  const TBool aInfoOnly, 
                                  TPkiServiceStoreType aCertStoreType) const
    {
    TBool match(EFalse);

    LOG(Log::Printf(_L("Matching")));
    LOG_1(" Pure informational: %d", aInfoOnly);

    LOG(Log::Printf(_L("Matching: certificate %S"), &iLabel));
    for(;;)
        {
        if (aDesc.iOwnerType != EPKICACertificate &&
            aCertStoreType != EPkiStoreTypeAny) 
            {            
            if (iCertStoreType != aCertStoreType)
                {
                LOG(Log::Printf(_L("    Store doesn't match, aborting")));
                match = EFalse;
                break;                
                }
            }
        else 
            {
            LOG(Log::Printf(_L("Skipping store check, not relevant")));
            }
                    
            
        if (aDesc.iSubjectKeyIdUsed)
            {            
            if(iKeyId == aDesc.iSubjectKeyId)
                {
                match = ETrue;
                }
            else
                {
                match = EFalse;
                break;
                }
            }
            
        if(aDesc.iTrustedAuthorityUsed)
            {
            if(iIssuerName == NULL)
                {
                match = EFalse;
                break;
                }
            else
                {
                CX500DistinguishedName* dnSuffix1 = CX500DistinguishedName::NewLC(*iIssuerName);
                CX500DistinguishedName* dnSuffix2;
                TInt popCount = 3;

                // ASN1 or plain text
                if((aDesc.iTrustedAuthority[0] != 0x30)
                   || ((aDesc.iTrustedAuthority[1] != 0x81)
                   && (aDesc.iTrustedAuthority[1] != 0x82)
                   && ((aDesc.iTrustedAuthority[1] + 2) != aDesc.iTrustedAuthority.Length())))
                    {
                    HBufC8* name2Der;
                    CPkcs10Req::BuildDistinguishedNameDerFromTextL(name2Der,
                                                                   aDesc.iTrustedAuthority,
                                                                   EFalse, KNullDesC8);
                    CleanupStack::PushL(name2Der);

                    dnSuffix2 = CX500DistinguishedName::NewLC(*name2Der);
                    }
                else
                    {
                    dnSuffix2 = CX500DistinguishedName::NewLC(aDesc.iTrustedAuthority);
                    popCount = 2;
                    }


                if(MatchL(*dnSuffix1, *dnSuffix2))
                    {
                    match = ETrue;
                    CleanupStack::PopAndDestroy(popCount);
                    }
                else
                    {
                    match = EFalse;
                    CleanupStack::PopAndDestroy(popCount);
                    break;
                    }
                }
            }
        if(aDesc.iOwnerTypeUsed)
            {
            if(iOwnerType == aDesc.iOwnerType)
                {
                match = ETrue;
                }
            else
                {
                match = EFalse;
                break;
                }
            }
        if(aDesc.iSerialNumberUsed)
            {
            if ((iSerialNumber != NULL) && ((*iSerialNumber).Compare(aDesc.iSerialNumber) == 0))
                {
                match = ETrue;
                }
            else
                {
                match = EFalse;
                break;
                }
            }

        if(aDesc.iIdentitySubjectNameUsed)
            {
            if(iSubjectName == NULL)
                {
                match = EFalse;
                break;
                }
            else
                {
                CX500DistinguishedName* dnSuffix1 = CX500DistinguishedName::NewLC(*iSubjectName);
                CX500DistinguishedName* dnSuffix2;
                TInt popCount = 3;
                // ASN1 or plain text
                if((aDesc.iIdentitySubjectName[0] != 0x30)
                   || ((aDesc.iIdentitySubjectName[1] != 0x81)
                   && (aDesc.iIdentitySubjectName[1] != 0x82)
                   && ((aDesc.iIdentitySubjectName[1] + 2) != aDesc.iIdentitySubjectName.Length())))
                    {
                    HBufC8* name2Der;
                    CPkcs10Req::BuildDistinguishedNameDerFromTextL(name2Der,
                                                                   aDesc.iIdentitySubjectName,
                                                                   EFalse, KNullDesC8);
                    CleanupStack::PushL(name2Der);

                    dnSuffix2 = CX500DistinguishedName::NewLC(*name2Der);
                    }
                else
                    {
                    dnSuffix2 = CX500DistinguishedName::NewLC(aDesc.iIdentitySubjectName);
                    popCount = 2;
                    }

                if(MatchL(*dnSuffix1, *dnSuffix2))
                    {
                    CleanupStack::PopAndDestroy(popCount);
                    match = ETrue;
                    }
                else
                    {
                    CleanupStack::PopAndDestroy(popCount);
                    match = EFalse;
                    break;
                    }
                }
            }

        if(aDesc.iIdentityRfc822NameUsed)
            {
            if(iRfc822Name == NULL)
                {
                match = EFalse;
                break;
                }
            else
                {
                TInt bytes = aDesc.iIdentityRfc822Name.Length();
                TPtrC8 tail = (*iRfc822Name).Right(bytes);
                if (tail.CompareF(aDesc.iIdentityRfc822Name) == 0)
                    {
                    match = ETrue;
                    }
                else
                    {
                    match = EFalse;
                    break;
                    }
                }
            }
            
        if(aDesc.iKeySizeUsed)
            {
            if(iKeySize == aDesc.iKeySize)
                {
                match = ETrue;
                }
            else
                {
                match = EFalse;
                break;
                }
            }
            
        if (match && !aInfoOnly)
            {            
            TValidity  val = CertValidity();
            // Treat future certificates as valid
            if((val == EValid) || (val == ENotValidYet))
                {
                match = ETrue;
                }
            else
                {
                LOG_("Matching: Expired, and not an informational request");
                match = EFalse;
                break;
                }
            }
            
        break;
        }
        
    return match;
    }

TBool CMapDescriptor::IsEqual(CMapDescriptor &aDesc)
    {
    TBool match = EFalse;

    for(;;)
        {
        if((iIssuerName != NULL) && 
           (aDesc.iIssuerName != NULL) && 
           (iIssuerName->Des().Compare(*aDesc.iIssuerName) == 0))
            {
            match = ETrue;
            }
        else
            {
            match = EFalse;
            break;
            }
        
        if((iSerialNumber != NULL) && (aDesc.iSerialNumber != NULL) && (iSerialNumber->Des().Compare(*aDesc.iSerialNumber) == 0))
            {
                match = ETrue;
                }
            else
                {
                match = EFalse;
                break;
                }
            
        break;
        }

    return match;
    }
    

void CMapDescriptor::SetMapTrusted(const TBool &aTrusted) 
    {
    iIsTrusted = aTrusted;
    }

void CMapDescriptor::SetMapDeletable(TBool aDeletable)
    {
    iIsDeletable = aDeletable;
    }

void CMapDescriptor::SetMapApplications(const RArray<TUid> &aApplications) 
    {
    iApplUids.Close();
    for(TInt i=0; i<aApplications.Count();i++)
        { 
        iApplUids.Append(aApplications[i]);
        }
    }

TPtrC CMapDescriptor::Label() const
    {
    return iLabel;
    }

const TPKIKeyIdentifier& CMapDescriptor::CertificateKeyId() const
    {    
    return iKeyId;
    }

TUint CMapDescriptor::KeySize() const
    {
    return iKeySize;
    }

TPKICertificateOwnerType CMapDescriptor::OwnerType() const
    {
    return iOwnerType;
    }

TPtrC8 CMapDescriptor::TrustedAuthority() const
    {
    if (iIssuerName == NULL)
        {
        return KEmptyDescriptor();
        }
    else
        {
        return *iIssuerName;
        }
    }

TPtrC8 CMapDescriptor::IdentitySubjectName() const
    {
    if (iSubjectName == NULL)
        {
        return KEmptyDescriptor();
        }
    else
        {
        return *iSubjectName;
        }
    }

TPtrC8 CMapDescriptor::SerialNumber() const
    {
    if (iSerialNumber == NULL)
        {
        return KEmptyDescriptor();
        }
    else
        {
        return *iSerialNumber;
        }
    }

TPKIKeyAlgorithm CMapDescriptor::KeyAlgorithm() const
    {
    return iKeyAlgorithm;
    }

TBool CMapDescriptor::Deletable() const
    {
    return iIsDeletable;
    }

TTime CMapDescriptor::EndTime() const
    {
    return iEndTime;
    }

CMapDescriptor::TValidity CMapDescriptor::CertValidity() const 
    {
    TTimeIntervalSeconds tolerance(120); 
    TTime currentTime;
    currentTime.UniversalTime();

    if (iEndTime <= currentTime)
        {
        return EExpired;
        }

    if (iStartTime >= currentTime + tolerance)
        {
        return ENotValidYet;
        }

    return EValid;
    }

TBool CMapDescriptor::IsApplicable(TUid aApplUid) const
    {
    TBool isApplicable = EFalse;
    for (TInt i = 0; i < iApplUids.Count(); ++i)
        {
        if (aApplUid == iApplUids[i])
            {
            isApplicable = ETrue;
            break;
            }
        }
    return isApplicable;
    }

TBool CMapDescriptor::MatchL(const CX500DistinguishedName& aDn1, const CX500DistinguishedName& aDn2) const
{
    if((aDn1.Count() == 0) || (aDn2.Count() == 0))
        {
        return EFalse;
        }
    
    if (aDn1.Count() < aDn2.Count())
        {
        return EFalse;
        }
    else
        {
        // For each field in aDn2, aDn1 must contain a field with the same value
        for (TInt i = 0; i < aDn2.Count(); i++)
            {
            if (!HasElementL(aDn1, aDn2.Element(i)))
                {
                return EFalse;
                }
            }
        }

    return ETrue;
}

TBool CMapDescriptor::HasElementL(const CX500DistinguishedName& aDn, const CX520AttributeTypeAndValue& aElement) const
{
    for (TInt i = 0; i < aDn.Count(); i++)
        {
        if (aElement.ExactMatchL(aDn.Element(i)))
            {
            return ETrue;
            }
        }
    return EFalse;
}
