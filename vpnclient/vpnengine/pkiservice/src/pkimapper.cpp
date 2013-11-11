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
* CPKIMapper class holds the information required to map API set to 
* use the storage model which is not native for that API.
*
*/

#include <x509cert.h>

#include "PKIMapper.h"
#include "mapdescriptor.h"
#include "log_r6.h"
#include "pkiserviceassert.h"


CPKIMapper* CPKIMapper::NewL()
    {
    CPKIMapper* self = new (ELeave) CPKIMapper();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPKIMapper::~CPKIMapper()
    {
    if ( iMapping )
        {
        iMapping->ResetAndDestroy();
        delete iMapping;
        }
    }


CPKIMapper::CPKIMapper()
    {
    }

void CPKIMapper::ConstructL()
    {
    LOG_("CPKIMapper::ConstructL");
    // Allocate mapping vector. Approx granularity 20 
    iMapping = new (ELeave) RPointerArray<CMapDescriptor>(20);
    }

void CPKIMapper::SetCacheCreated()
    {
    iCacheCreated = ETrue;
    }
TBool CPKIMapper::CacheCreated()
    {
    return iCacheCreated;
    }
    
TInt CPKIMapper::AddMapping(CMapDescriptor* aMap)
    {
    // Insert as first element, so that newest object will be found first
    TInt err = iMapping->Insert(aMap, 0);
    if (err != KErrNone)
        {
        delete aMap;
        }
    return err;
    }

void CPKIMapper::LogMap(CMapDescriptor &aDescriptor) const 
    {
    TPtrC objectName = aDescriptor.Label();
    const TPKIKeyIdentifier& subjectKeyId = aDescriptor.CertificateKeyId();
    
    LOG(Log::Printf(_L("====CertificateObject====\n")));
    LOG(Log::Printf(_L("ObjectName: %S\n"), &objectName));
    LOG(Log::Printf(_L("KeyId:")));
    LOG(Log::HexDump(NULL, NULL, subjectKeyId.Ptr(), subjectKeyId.Length()));
    }

    
void CPKIMapper::DeleteMapping(TInt aIndex)
    {
    // Delete mapping at index
    LOG(Log::Printf(_L("Delete mapping entry")));
    LogMap(*(*iMapping)[aIndex]);
    delete (*iMapping)[aIndex]; 
    iMapping->Remove(aIndex);
    iMapping->Compress();
    }

TInt CPKIMapper::CertCount(TBool aInfoOnly)
    {
    TInt count(0);    
    
    for(TInt i(0); i < iMapping->Count(); i++)
        {
        CMapDescriptor* mapping = (*iMapping)[i];
        count++;
        if (!aInfoOnly && mapping->CertValidity() == CMapDescriptor::EExpired)
            {
            // do not count expider certificates
            count--;
            }                
        }
    iCount = count;
    return count;            
    }

TInt CPKIMapper::ApplicableCertCount(const RArray<TUid>& aUidArray)
{
    iCount = 0;
    
    for(TInt i = 0; i < iMapping->Count(); i++)
        {
        const CMapDescriptor* mapping = (*iMapping)[i];
        if(mapping->OwnerType() == EPKICACertificate)
            {
            if(mapping->CertValidity() != CMapDescriptor::EExpired)
                {
                for (TInt j = 0; j < aUidArray.Count(); ++j)
                    {
                    if (mapping->IsApplicable(aUidArray[j]))
                        {
                        iCount++;
                        break;
                        }
                    }
                }
            }
        }
                    
    return iCount;
}

TInt CPKIMapper::GetCertDetailsL(TSecurityObjectDescriptor& aSecDesc, 
                                 TPkiServiceStoreType aStoreType, 
                                 TBool aInfoOnly,
                                 TCertificateListEntry& aResultCertInfo)
    {
    TInt index;
    TInt err = KErrNone;
    
    TRAPD(leave, err = ResolveCertMappingL(aSecDesc, iObjectName, 
                                           index, aInfoOnly, 
                                           aStoreType));
    if (leave != KErrNone)
        {
        return leave; 
        }
    
    if (err == KErrNone)
        {        
        const CMapDescriptor* mapping = (*iMapping)[index];        
        aResultCertInfo.iObjectName = mapping->Label();
        aResultCertInfo.iOwnerType = mapping->OwnerType();
        aResultCertInfo.iTrustedAuthority = mapping->TrustedAuthority();
        aResultCertInfo.iIdentitySubjectName = mapping->IdentitySubjectName();
        aResultCertInfo.iSerialNumber = mapping->SerialNumber();
        aResultCertInfo.iSubjectKeyId = mapping->CertificateKeyId();
        aResultCertInfo.iKeySize = mapping->KeySize();                 
        aResultCertInfo.iKeyAlgorithm = mapping->KeyAlgorithm();       
        aResultCertInfo.iIsDeletable = mapping->Deletable();           
        }
    return err;    
    }


TInt CPKIMapper::GetCertListL(const RMessage2 &aMessage, TBool aInfoOnly)
{
    TInt pos = 0;
    TInt iLast = 0;
    TInt iFirst = 0;
    
    iLast = iMapping->Count();
    CBufFlat* list = CBufFlat::NewL(sizeof(TCertificateListEntry));
    CleanupStack::PushL(list);
    list->ResizeL(iCount * sizeof(TCertificateListEntry));
    TCertificateListEntry certInfo;
    
    for(TInt i = iFirst; i < iLast; i++)
        {
        CMapDescriptor* mapping = (*iMapping)[i];
        if(aInfoOnly || mapping->CertValidity() != CMapDescriptor::EExpired)
            {
            certInfo.iObjectName = mapping->Label();
            certInfo.iOwnerType = mapping->OwnerType();
            certInfo.iTrustedAuthority = mapping->TrustedAuthority();
            certInfo.iIdentitySubjectName = mapping->IdentitySubjectName();
            certInfo.iSerialNumber = mapping->SerialNumber();
            certInfo.iSubjectKeyId = mapping->CertificateKeyId();
            certInfo.iKeySize = mapping->KeySize();                   // Key size
            certInfo.iKeyAlgorithm = mapping->KeyAlgorithm();         // RSA, DSA
            certInfo.iIsDeletable = mapping->Deletable();           // IsDeletable

            list->Write(pos * sizeof(TCertificateListEntry),
                        (TAny*)&certInfo,
                        sizeof(TCertificateListEntry));
            pos++;
            if(pos >= iCount)
                {
                break;
                }
            }            
        }
    TPtr8 ptrList = list->Ptr(0);
    aMessage.WriteL(0, ptrList);

    CleanupStack::PopAndDestroy(1); // list
    return KErrNone;
}


void CPKIMapper::GetApplicableCertListL(const RMessage2& aMessage, const RArray<TUid>& aUidArray)
{
    TInt pos = 0;
    
    CBufFlat* list = CBufFlat::NewL(sizeof(TCertificateListEntry));
    CleanupStack::PushL(list);
    list->ResizeL(iCount * sizeof(TCertificateListEntry));
    TCertificateListEntry certInfo;

    for(TInt i = 0; (i < iMapping->Count()) && (pos < iCount); i++)
        {
        CMapDescriptor* mapping = (*iMapping)[i];
        if(mapping->OwnerType() == EPKICACertificate)
            {
            if(mapping->CertValidity() != CMapDescriptor::EExpired)
                {
                for(TInt j = 0; j < aUidArray.Count(); j++)
                    {
                    if(mapping->IsApplicable(aUidArray[j]))
                        {
                        certInfo.iObjectName = mapping->Label();
                        certInfo.iOwnerType = mapping->OwnerType();
                        certInfo.iTrustedAuthority = mapping->TrustedAuthority();
                        certInfo.iIdentitySubjectName = mapping->IdentitySubjectName();
                        certInfo.iSerialNumber = mapping->SerialNumber();
                        certInfo.iSubjectKeyId = mapping->CertificateKeyId();
                        certInfo.iKeySize = mapping->KeySize();                   // Key size
                        certInfo.iKeyAlgorithm = mapping->KeyAlgorithm();         // RSA, DSA
                        certInfo.iIsDeletable = mapping->Deletable();           // IsDeletable

                        list->Write(pos * sizeof(TCertificateListEntry),
                                    (TAny*)&certInfo,
                                    sizeof(TCertificateListEntry));
                        pos++;
                        break;
                        }
                    }
                }
            }
        }
    TPtr8 ptrList = list->Ptr(0);
    aMessage.WriteL(0, ptrList);

    CleanupStack::PopAndDestroy(1); // list
}



CMapDescriptor& CPKIMapper::GetMapDescriptorAtIndex(TInt aIndex)
{
    PKISERVICE_ASSERT(aIndex < iMapping->Count());
    return *(*iMapping)[aIndex];
}


void CPKIMapper::GetCertificateKeyIdL(TSecurityObjectDescriptor &aDescriptor, TPKIKeyIdentifier &aKeyId,
                                      TPkiServiceStoreType aStoreType) const
    {
    LOG(Log::Printf(_L("-> CPKIMapper::GetCertificateKeyIdL"), aStoreType));
    aKeyId.Zero();
    
    TInt index;    
    
    TFileName* fileName = new (ELeave) TFileName;
    CleanupDeletePushL(fileName);
    fileName->Zero();
    
    TInt status = ResolveCertMappingL(aDescriptor, *fileName, index, EFalse, aStoreType);    

    CleanupStack::PopAndDestroy(fileName);
    
    
    if(status == KErrNone)
        {
        aKeyId.Copy((*iMapping)[index]->CertificateKeyId());
        }
    else
        {
        LOG(Log::Printf(_L("ResolveKeyMapping: key NOT found\n")));
        LogSearchArguments(aDescriptor);
        }    
    LOG(Log::Printf(_L("<- CPKIMapper::GetCertificateKeyIdL"), aStoreType));        
    }

/**
    Check whether the given label is unique among all VPN certs.
    @return True only if the given label doesn't exist
*/
TBool CPKIMapper::LabelIsUnique(const TDesC& aLabel) const
    {
    for (TInt i(0); i < iMapping->Count(); i++) 
        {
        if ((*iMapping)[i]->Label().Compare(aLabel) == 0) 
            {
            return EFalse;
            }
        }
    return ETrue;
    }

/**
    Check whether the given certificate already exists among VPN certs.
    @return True only if the given certificate does not exist in VPN's cert list.
*/
TBool CPKIMapper::CertificateIsUniqueL(const TDesC8& aCertData)
    {
    TBool ret(ETrue);
    
    LOG(Log::Printf(_L("Verifying the uniqueness of certificate:")));

    CX509Certificate* certificate = CX509Certificate::NewLC(aCertData);
    const TPtrC8* issuername = certificate->DataElementEncoding(CX509Certificate::EIssuerName);
    const TPtrC8* subjectname = certificate->DataElementEncoding(CX509Certificate::ESubjectName);
    
    HBufC* issuerdispname = certificate->IssuerName().DisplayNameL();
    CleanupStack::PushL(issuerdispname);
    HBufC* subjectdispname = certificate->SubjectName().DisplayNameL();
    CleanupStack::PushL(subjectdispname);
    
    TInt count(iMapping->Count());

    for (TInt i(0); i < count; i++) 
        {
        CMapDescriptor* mapitem = (*iMapping)[i];

        //  Use subject name for uniqueness criterion
        if (mapitem->IdentitySubjectName().Length() > 0)
            {
            if (subjectname->Compare(mapitem->IdentitySubjectName()) == 0) 
                {
                LOG(Log::Printf(_L("Found an existing cert that matches subject")));
                if (issuername->Compare(mapitem->TrustedAuthority()) == 0) 
                    {
                    ret = EFalse;
                    break;
                    }
                }
            }
        else if (subjectname->Compare(*issuername) == 0)
            {
            if (subjectname->Compare(mapitem->TrustedAuthority()) == 0) 
                {
                LOG(Log::Printf(_L("Found an existing cert that matches subject (CA)")));
                ret = EFalse;
                break;
                }
            }
        }

    CleanupStack::PopAndDestroy(3, certificate); // issuerdispname, subjectdispname

    return ret;
    }
    
/**
    Generates an unique label name for a certificate, based on its subject name.
    @param1 The binary data describing the certificate
    @param2 Return value for the generated unique name
    @param3 Certificate owner type -- currently not supported.
    @return none
*/
void CPKIMapper::GenerateUniqueNameL(
    const TDesC8 &aCertData, TDes& aName, 
    TCertificateOwnerType /*aOwnerType*/ )
    {
    LOG(Log::Printf(_L("CPKIMapper::GenerateUniqueNameL() entry")));

    CX509Certificate* certificate = CX509Certificate::NewLC(aCertData);

    HBufC* baseline = certificate->SubjectName().DisplayNameL();
    CleanupStack::PushL(baseline);
    // +5 for (999) suffix
    HBufC* variation = HBufC::NewLC(baseline->Length() + 5);
    variation->Des().Append(*baseline);
    
    if (baseline->Length() == 0)
        {
        TPtr ptr = variation->Des();
        ptr.Format(_L("(1)"));
        }

    // See whether the initial label is already unique    
    TInt iter(2);
    while (!LabelIsUnique(*variation)) 
        {
        // Iterate a new version of the label
        if (iter > 999) 
            {
            // too long name, just go with the previous.
            break;
            }
        TPtr ptr = variation->Des();
        ptr.Format(_L("%S(%d)"), &(*baseline), iter);
        iter++;
        }

    // Sanity check for string lengths
    aName = variation->Left(MAX_FILENAME_LENGTH);

    CleanupStack::PopAndDestroy(3); //variation, baseline, certificate

    LOG(Log::Printf(_L("CPKIMapper::GenerateUniqueNameL() exit")));
    }

TInt CPKIMapper::ResolveCertMappingL(TSecurityObjectDescriptor &aDescriptor, TDes16 &aFilename, 
                                     TInt &aIndex, const TBool aInfoOnly, 
                                     TPkiServiceStoreType aStoreType) const   
    {
    TInt i;
    TInt status = KErrNotFound;
    TInt foundIndex = -1;
    TTime furthestEndTime = TTime(0);

    LOG(Log::Printf(_L("Resolving cert mapping, STORETYPE: %d\n"), aStoreType));

    LOG(LogSearchArguments(aDescriptor));

    // Scan available mappings
    for(i = 0; i < iMapping->Count(); i++)
        {
        // Bypass entry in case that function leaves
        // (issuer/subject name can be invalid)
        CMapDescriptor* mapping = (*iMapping)[i];
        if(mapping->IsMatchingL(aDescriptor, aInfoOnly, aStoreType))
            {
            // If we found a match, process it further
            _LIT(KMidp2Label, "MIDP2");
            TBuf<12> buf;
            buf.Append(KMidp2Label);
            // Discard all MIDP2 certificates to avoid label-mapping problem
            if (buf.Compare(mapping->Label()) != 0)
                {
                if(mapping->EndTime() > furthestEndTime)
                    {
                    furthestEndTime = mapping->EndTime();
                    foundIndex = i;
                    LOG(Log::Printf(_L("Resolve cert mapping: Tentatively found a suitable one")));
                    // Continue to search the longest lasting certificate
                    }
                }
            else 
                {
                LOG(Log::Printf(_L("Found a cert, but it was a MIDP2 one - continuing search")));
                }
            }
        }
    
    if(foundIndex == -1)
        {
        LOG(Log::Printf(_L("Resolve cert mapping: No matching certificate found")));       
        status = KErrNotFound;
        }
    else
        {
        aFilename.Zero();
        status = KErrNone;
        if(status == KErrNone)
            {
            aFilename.Append((*iMapping)[foundIndex]->Label());
            status = KErrNone;
            aIndex = foundIndex;
            }
        LOG(Log::Printf(_L("ResolveCertMapping: certificate found\n")));
        LogSearchArguments(aDescriptor);
        LOG(Log::Printf(_L("ResolveCertMapping: Object info\n")));
        LogMap(*(*iMapping)[aIndex]);
        }
    return status;
    }    
  

void CPKIMapper::LogSearchArguments(TSecurityObjectDescriptor &aDescriptor) const
    {
    TBuf<256> temp;
    LOG(Log::Printf(_L("====Object Search arguments====\n")));
    if(aDescriptor.iTrustedAuthorityUsed)
        {
        temp.Copy(aDescriptor.iTrustedAuthority);
        if((aDescriptor.iTrustedAuthority[0] != 0x30)
           || ((aDescriptor.iTrustedAuthority[1] != 0x81)
               && (aDescriptor.iTrustedAuthority[1] != 0x82)
               && ((aDescriptor.iTrustedAuthority[1] + 2) != aDescriptor.iTrustedAuthority.Length())))
            {
            LOG(Log::Printf(_L("Trusted authority: %S\n"), &temp));
            }
        else
            {
            LOG(Log::Printf(_L("Trusted authority:")));
            LOG(Log::HexDump(NULL, NULL, aDescriptor.iTrustedAuthority.Ptr(), aDescriptor.iTrustedAuthority.Length()));
            }
        }
    if(aDescriptor.iIdentitySubjectNameUsed)
        {
        temp.Copy(aDescriptor.iIdentitySubjectName);
        if((aDescriptor.iIdentitySubjectName[0] != 0x30)
           || ((aDescriptor.iIdentitySubjectName[1] != 0x81)
               && (aDescriptor.iIdentitySubjectName[1] != 0x82)
               && ((aDescriptor.iIdentitySubjectName[1] + 2) != aDescriptor.iIdentitySubjectName.Length())))
            {
            LOG(Log::Printf(_L("SubjectName: %S\n"), &temp));
            }
        else
            {
            LOG(Log::Printf(_L("SubjectName:")));
            LOG(Log::HexDump(NULL, NULL, aDescriptor.iIdentitySubjectName.Ptr(), aDescriptor.iIdentitySubjectName.Length()));
            }
        }
    if(aDescriptor.iIdentityRfc822NameUsed)
        {
        temp.Copy(aDescriptor.iIdentityRfc822Name);
        LOG(Log::Printf(_L("Rfc822Name: %S\n"), &temp));
        }
    if(aDescriptor.iSerialNumberUsed)
        {
        LOG(Log::Printf(_L("SerialNumber:")));
        LOG(Log::HexDump(NULL, NULL, aDescriptor.iSerialNumber.Ptr(), aDescriptor.iSerialNumber.Length()));
        }
    if(aDescriptor.iSubjectKeyIdUsed)
        {
        LOG(Log::Printf(_L("KeyId:")));
        LOG(Log::HexDump(NULL, NULL, aDescriptor.iSubjectKeyId.Ptr(), aDescriptor.iSubjectKeyId.Length()));
        }
    if(aDescriptor.iOwnerTypeUsed)
        {
        LOG(Log::Printf(_L("OwnerType: %d\n"), aDescriptor.iOwnerType));
        }
    if(aDescriptor.iKeyUsageUsed)
        {
        LOG(Log::Printf(_L("KeyUsage:")));
        LOG(Log::HexDump(NULL, NULL, aDescriptor.iKeyUsageDer.Ptr(), aDescriptor.iKeyUsageDer.Length()));
        }
    if(aDescriptor.iKeySizeUsed)
        {
        LOG(Log::Printf(_L("KeySize: %d\n"), aDescriptor.iKeySize));
        }
    if(aDescriptor.iKeyAlgorithmUsed)
        {
        LOG(Log::Printf(_L("KeyAlgorithm: %d\n"), aDescriptor.iKeyAlgorithm));
        }
    }   

