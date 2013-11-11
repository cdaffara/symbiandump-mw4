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
* Description: Implementation of CDmAdCert.
*
*/


#include "dmadutil.h"
#include "dmadstorecert.h"
#include "vpnlogger.h"
#include "dmadcertxmldefs.h"
#include "XppImpl.h"
#include "XwImpl.h"
#include <vpnlogmessages.rsg>

CDmAdCert* CDmAdCert::NewL(RPKIServiceAPI& aPkiServiceApi)
    {
    TRACE("CDmAdCert::NewL");
    
    CDmAdCert* self = NewLC(aPkiServiceApi);
    CleanupStack::Pop(self);
    return self;
    }

CDmAdCert* CDmAdCert::NewLC(RPKIServiceAPI& aPkiServiceApi)
    {
    CDmAdCert* self = new (ELeave) CDmAdCert(aPkiServiceApi);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CDmAdCert::ConstructL()
    {
    TRACE("CDmAdCert::ConstructL");
    iCertLuidMapping = new (ELeave) CArrayPtrFlat<CDmAdCertLuidMappingElem>(4);
    BuildCertLuidMappingTableL();
    }
    
CDmAdCert::CDmAdCert(RPKIServiceAPI& aPkiServiceApi) : iPkiServiceApi(&aPkiServiceApi)
    {
    }
    
CDmAdCert::~CDmAdCert()
    {
    TRACE("CDmAdCert::~CDmAdCert");
    CDmAdCertLuidMappingElem::CleanupOperationDeleteCArrayPtr(iCertLuidMapping);    
    }
    
TBool CDmAdCert::FindL(const TDesC8& aLuid)
    {
    TRACE("CDmAdCert::FindL");
    if (!FindCertLuidMappingElemL(aLuid))
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }
    
HBufC8* CDmAdCert::AddL(const CDmAdCertParms& aParms)
    {    
    TRACE("CDmAdCert::AddL");
    
    TPKICertificateOwnerType ownerType = aParms.Type();

    HBufC8* certRef = BuildCertRefL(aParms.Content(), EFalse);
    CleanupStack::PushL(certRef);
    CDmAdCertLuidMappingElem* certLuidMappingElem = FindCertLuidMappingElemL(*certRef);
    if (certLuidMappingElem)
        {
        DEBUG_LOG(_L("Element already exists"));
        User::Leave(KErrAlreadyExists);
        }
    CleanupStack::PopAndDestroy(certRef); 
    

    TInt status = KErrNone;
    if (ownerType == EPKIUserCertificate)
        {
        DEBUG_LOG(_L("Attaching user certificate"));        
        
        TKeyListEntry* keyListEntry = new (ELeave) TKeyListEntry;
        CleanupStack::PushL(keyListEntry);
        
        TPKIKeyIdentifier keyId = aParms.KeyId();
        DEBUG_LOG(_L("Key id:"));
        DEBUG_LOG_HEX(keyId);
        
        
        User::LeaveIfError(iPkiServiceApi->KeyDetails(keyId,
						   *keyListEntry));
        
        TInt keyLength = keyListEntry->iKeySize;
        
        DEBUG_LOG(_L("Key info:"));
        DEBUG_LOG1(_L("Object name: %S"), &(keyListEntry->iObjectName));
        DEBUG_LOG_HEX(keyListEntry->iSubjectKeyId);
        DEBUG_LOG1(_L("Key size: %d"), keyListEntry->iKeySize);
        DEBUG_LOG1(_L("Algorithm: %d"), keyListEntry->iKeyAlgorithm);

        
        
        CleanupStack::PopAndDestroy(); //keyListEntry
        
        status = iPkiServiceApi->AttachCertificate(keyId,
                                                   aParms.Deletable(),
                                                   keyLength,
                                                   EPKIRSA,
                                                   aParms.Content());
        }
    else
        {
        status = iPkiServiceApi->StoreCertificate(ownerType,
                                                  aParms.Deletable(),
                                                  0, //Key length is undefined
                                                  EPKIRSA,
                                                  aParms.Content());            
        }

    if (status != KErrNone)
        {
        DEBUG_LOG1(_L("Operation failed with %d"), status);       
        User::Leave(status);
        }

    certRef = BuildCertRefL(aParms.Content(), ETrue);
    CleanupStack::PushL(certRef);
    UpdateL(*certRef, aParms);
    CleanupStack::Pop(certRef);
    return certRef;
    }

void CDmAdCert::UpdateL(const TDesC8& aLuid, const CDmAdCertParms& aParms)
    {
    TRACE("CDmAdCert::UpdateL");
    HBufC8* issuerName;
    HBufC8* serialNumber;
    GetIssuerAndSerialFromCertRefLC(aLuid, issuerName, serialNumber);

    if (aParms.Type() == EPKICACertificate) // CA cert
        {
        UpdateTrustedL(*issuerName, *serialNumber, aParms);
        UpdateApplicabilityL(*issuerName, *serialNumber, aParms);
        }
    
    CleanupStack::PopAndDestroy(2); //serialNumber, issuerName
    }
    
void CDmAdCert::FetchL(const TDesC8& aLuid, CDmAdCertParms& aParms)
    {
    TRACE("CDmAdCert::FetchL");
    HBufC8* issuerName;
    HBufC8* serialNumber;
    GetIssuerAndSerialFromCertRefLC(aLuid, issuerName, serialNumber);
    
    //TInt certSize = 1024;
    TInt certSize = 4096;
    HBufC8* certBuf = NULL;
    TPtr8 certBufDesc(NULL, 0);
    TBool errBufferTooShortOccurred = EFalse;
    for (;;)
        {
        if (certBuf)
            {
            CleanupStack::PopAndDestroy(certBuf);
            }
        certBuf = HBufC8::NewLC(certSize);
        certBufDesc.Set(certBuf->Des());
    
        // Both user and device certificate stores needs to be used for
        // certificate reading. Set certificate store type to STORETYPE_ANY.
        TPkiServiceStoreType certStoreType(EPkiStoreTypeAny);
        User::LeaveIfError(iPkiServiceApi->CertStoreType(certStoreType));
        User::LeaveIfError(iPkiServiceApi->SetStoreType(STORE_CERTSTORE, EPkiStoreTypeAny));        
        TInt err = iPkiServiceApi->ReadCertificate(*issuerName,
                                                   *serialNumber,
                                                   certBufDesc);
                
        // Set previous store type back
        User::LeaveIfError(iPkiServiceApi->SetStoreType(STORE_CERTSTORE, certStoreType));                
        
        if (err == KErrNone)
            {
            break;
            }
        else if (err == KPKIErrBufferTooShort)
            {
            if (errBufferTooShortOccurred)
                {
                DEBUG_LOG(_L("Buffer too short"));                
                User::Leave(KErrGeneral);
                }
            errBufferTooShortOccurred = ETrue;
            User::LeaveIfError(iPkiServiceApi->GetRequiredBufferSize(certSize));
            }
        else
            {
            DEBUG_LOG1(_L("Fetch failed with %d"), err);            
            User::Leave(err);
            }
        }

    aParms.SetContentL(*certBuf);
    ReadCertDetailsL(*issuerName, *serialNumber, aParms);

    CleanupStack::PopAndDestroy(3); //certBuf, serialNumber, issuerName
    }
    
void CDmAdCert::DeleteL(const TDesC8& aLuid)
    {
    TRACE("CDmAdCert::DeleteL");
    HBufC8* issuerName;
    HBufC8* serialNumber;
    GetIssuerAndSerialFromCertRefLC(aLuid, issuerName, serialNumber);
    while (FindCertLuidMappingElemL(aLuid))
        {
        User::LeaveIfError(iPkiServiceApi->RemoveCertificate(*issuerName,
                                                             *serialNumber));
        
        RemoveCertLuidMappingElem(aLuid);
        }
    CleanupStack::PopAndDestroy(2); //serialNumber, issuerName
    }

void CDmAdCert::ListL(RPointerArray<HBufC8>& aLuidList)
    {
    TRACE("CDmAdCert::ListL");
    for (TInt i=0; i<iCertLuidMapping->Count(); ++i)
        {
        CDmAdCertLuidMappingElem* certLuidMappingElem = iCertLuidMapping->At(i);
                
        HBufC8* luidElem = certLuidMappingElem->Luid().AllocLC();
        aLuidList.AppendL(luidElem);
        CleanupStack::Pop(luidElem);
        }
    }
    
//------------------------------------------------------------------------

void CDmAdCert::ReadCertDetailsL(const TDesC8& aIssuerName, const TDesC8& aSerialNumber, CDmAdCertParms& aParms)
    {
    TRACE("CDmAdCert::ReadCertDetailsL");
    TCertificateListEntry* entry = new (ELeave) TCertificateListEntry();
    CleanupStack::PushL(entry);
    
    User::LeaveIfError(iPkiServiceApi->CertificateDetails(aIssuerName,
                                                          aSerialNumber,
                                                          *entry)); 
    
    aParms.SetType(entry->iOwnerType);    
    aParms.SetDeletable(entry->iIsDeletable);
       
    TBool trusted = ETrue;
    if (entry->iOwnerType == EPKICACertificate) // CA cert
        {
        User::LeaveIfError(iPkiServiceApi->Trusted(aIssuerName,
                                                   aSerialNumber,
                                                   trusted));        
        }
    aParms.SetTrusted(trusted);

    if (entry->iOwnerType == EPKICACertificate) // CA cert
        {
        RArray<TUid> applications;
        CleanupClosePushL(applications);
        
        iPkiServiceApi->ApplicationsL(aIssuerName,
                                      aSerialNumber,
                                      applications);
        aParms.SetApplicabilityL(applications);                                      
        CleanupStack::PopAndDestroy(); //applications
        }    
        
    CleanupStack::PopAndDestroy(); // entry        
    }
    
void CDmAdCert::UpdateTrustedL(const TDesC8& aIssuerName, const TDesC8& aSerialNumber, const CDmAdCertParms& aParms)
    {
    TRACE("CDmAdCert::UpdateTrustedL");
    
    User::LeaveIfError(iPkiServiceApi->SetTrust(aIssuerName,
                                                aSerialNumber,
                                                aParms.Trusted()));
    }

void CDmAdCert::UpdateApplicabilityL(const TDesC8& aIssuerName, const TDesC8& aSerialNumber, const CDmAdCertParms& aParms)
    {
    TRACE("CDmAdCert::UpdateApplicabilityL");
    
    const RArray<TUid>& applications = aParms.Applicability();   
    if (applications.Count() > 0)
        {
        iPkiServiceApi->SetApplicabilityL(aIssuerName,
                                          aSerialNumber,
                                          applications);
        }
    }

TPtrC8 CDmAdCert::AppendCertLuidMappingElemL(const TDesC8& aIssuerName, const TDesC8& aSerialNumber)
    {
    TRACE("CDmAdCert::AppendCertLuidMappingElemL");
    TPtrC8 luid(KNullDesC8);
    
    CDmAdCertLuidMappingElem* certLuidMappingElem = CDmAdCertLuidMappingElem::NewLC(aIssuerName, aSerialNumber);
    luid.Set(certLuidMappingElem->Luid());
    
    iCertLuidMapping->AppendL(certLuidMappingElem);
    CleanupStack::Pop(certLuidMappingElem);
    
    return luid;
    }

void CDmAdCert::RemoveCertLuidMappingElem(const TDesC8& aLuid)
    {
    TRACE("CDmAdCert::RemoveCertLuidMappingElem");
    for (TInt i=0; i<iCertLuidMapping->Count(); ++i)
        {
        CDmAdCertLuidMappingElem* certLuidMappingElem = iCertLuidMapping->At(i);
        if (certLuidMappingElem->Luid().Compare(aLuid) == 0)
            {
            iCertLuidMapping->Delete(i);
            i--;
            delete certLuidMappingElem;
            break;
            }
        }
    }

CDmAdCertLuidMappingElem* CDmAdCert::FindCertLuidMappingElemL(const TDesC8& aLuid)
    {
    TRACE("CDmAdCert::FindCertLuidMappingElemL");
    for (TInt i=0; i<iCertLuidMapping->Count(); ++i)
        {
        CDmAdCertLuidMappingElem* certLuidMappingElem = iCertLuidMapping->At(i);
        if (certLuidMappingElem->Luid().Compare(aLuid) == 0)
            {
            return certLuidMappingElem;
            }
        }
    return NULL;
    }

void CDmAdCert::BuildCertLuidMappingTableL()
    {
    TRACE("CDmAdCert::BuildCertLuidMappingTableL");
    CArrayFix<TCertificateListEntry>* certList;
    iPkiServiceApi->ListCertificatesL(certList);
    CleanupStack::PushL(certList);

    for (TInt i=0; i<certList->Count(); ++i)
        {
        TCertificateListEntry& entry = certList->At(i);
        AppendCertLuidMappingElemL(entry.iTrustedAuthority, entry.iSerialNumber);
        }
    
    CleanupStack::PopAndDestroy(certList); 
    }

HBufC8* CDmAdCert::BuildCertRefL(const TDesC8& aCertificateDer, TBool aAppend)
    {
    TRACE("CDmAdCert::BuildCertRefL");
    
    CX509Certificate* certCx509 = CX509Certificate::NewL(aCertificateDer);
    CleanupStack::PushL(certCx509);

    const TPtrC8 issuerName(*(certCx509->DataElementEncoding(CX509Certificate::EIssuerName)));
    const TPtrC8 serialNumber(*(certCx509->DataElementEncoding(CX509Certificate::ESerialNumber)));

    HBufC8* certRef = 0;
    if (aAppend)
        {
        TPtrC8 luid(AppendCertLuidMappingElemL(issuerName, serialNumber));
        certRef = luid.AllocL();
        }
    else
        {
        certRef = CDmAdCertLuidMappingElem::BuildLuidL(issuerName, serialNumber);
        }

    CleanupStack::PopAndDestroy(certCx509);
    return certRef;
    }

void CDmAdCert::GetIssuerAndSerialFromCertRefLC(const TDesC8&  aCertRef,
                                                HBufC8*&       aIssuerName,
                                                HBufC8*&       aSerialNumber)
    {
    CDmAdCertLuidMappingElem* certLuidMappingElem = FindCertLuidMappingElemL(aCertRef);
    if (!certLuidMappingElem)
        {
        DEBUG_LOG(_L("Mapping not found"));        
        User::Leave(KErrNotFound);
        }

    HBufC8* issuerNameHBuf = certLuidMappingElem->IssuerName().AllocLC();
    HBufC8* serialNumberHBuf = certLuidMappingElem->SerialNumber().AllocLC();
    
    aIssuerName = issuerNameHBuf;
    aSerialNumber = serialNumberHBuf;
    }

HBufC8* CDmAdCert::CertSubjectNameL(const TDesC8& aCertificateDer)
    {
    CX509Certificate* certCx509 = CX509Certificate::NewLC(aCertificateDer);
    HBufC16* subjectName16 = certCx509->SubjectName().DisplayNameL();
    CleanupStack::PushL(subjectName16);    
    HBufC8* subjectName8 = HBufC8::NewL(subjectName16->Length());
    subjectName8->Des().Copy(*subjectName16);
    CleanupStack::PopAndDestroy(2, certCx509);
    return subjectName8;
    }
