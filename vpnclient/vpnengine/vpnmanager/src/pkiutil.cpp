/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Provides static PKI-related helper functions 
* (such as building Distinguished Names)
*
*/

#include <x500dn.h>
#include <x509cert.h>
#include <x520ava.h>
#include <utf.h>
#include <x509keys.h>

#include "pkiutil.h"
#include "pkiserviceapi.h"
#include "pkcs10.h"
#include "ikepolparser.h"


HBufC* PkiUtil::CertSubjectNameL(const TDesC8& aCertData)
    {
    CX509Certificate* certificate = CX509Certificate::NewL(aCertData);
    CleanupStack::PushL(certificate);

    CX500DistinguishedName* subjectDn;
    subjectDn = CX500DistinguishedName::NewL(certificate->SubjectName());
    CleanupStack::PushL(subjectDn);

    HBufC* certDn = CertDnL(*subjectDn);

    CleanupStack::PopAndDestroy(2); // subjectDn, certificate

    return certDn;
    }

HBufC* PkiUtil::CertIssuerNameL(const TDesC8& aCertData)
    {
    CX509Certificate* certificate = CX509Certificate::NewL(aCertData);
    CleanupStack::PushL(certificate);

    CX500DistinguishedName* issuerDn;
    issuerDn = CX500DistinguishedName::NewL(certificate->IssuerName());
    CleanupStack::PushL(issuerDn);

    HBufC* certDn = CertDnL(*issuerDn);

    CleanupStack::PopAndDestroy(2); // issuerDn, certificate

    return certDn;
    }

HBufC* PkiUtil::CertDnL(const CX500DistinguishedName& aName)
    {
    TInt count = aName.Count();
    HBufC* certDn = HBufC::NewL(KCertDnSizeIncrement);
    CleanupStack::PushL(certDn);

    for (TInt i = 0; i < count; i++)
        {
        const CX520AttributeTypeAndValue& attribute = aName.Element(i);

        // Unsupported attribute is not appended (KErrNotSupported error)
        TRAPD( err, AppendAttributeL(certDn, attribute) );
        if ( err != KErrNotSupported )
            {
            User::LeaveIfError( err );
            }
        }

    CleanupStack::Pop(); // certDn

    return certDn;
    }

TInt PkiUtil::CertKeySizeL(const TDesC8& aCertData)
    {
    TInt keySize = 0;

    CX509Certificate* certificate = CX509Certificate::NewLC(aCertData);
    const CSubjectPublicKeyInfo& publicKeyInfo = certificate->PublicKey();
    const TPtrC8 keyData = publicKeyInfo.KeyData();

    TX509KeyFactory keyFactory;

    switch(publicKeyInfo.AlgorithmId())
        {
        case ERSA:
            {
            const CRSAPublicKey* keyRSA = keyFactory.RSAPublicKeyL(keyData);
            const TInteger&  n = keyRSA->N();
            keySize = n.BitCount();
            delete keyRSA;
            }
            break;
        case EDSA:
            {
            TPtrC8 params = publicKeyInfo.EncodedParams();

            const CDSAPublicKey* keyDSA =
                keyFactory.DSAPublicKeyL(params, keyData);

            const TInteger& y = keyDSA->Y();
            keySize = y.BitCount();
            delete keyDSA;
            }
            break;
        default:
            User::Leave(KErrNotSupported);
            break;
        }

    CleanupStack::PopAndDestroy(certificate);

    return keySize;
    }


void PkiUtil::AppendAttributeL(HBufC*& aBuf, const CX520AttributeTypeAndValue& aAttribute)
    {
    HBufC* attrName = AttributeTypeToNameL(aAttribute.Type());
    CleanupStack::PushL(attrName);

    HBufC* attrValue = aAttribute.ValueL();
    CleanupStack::PushL(attrValue);

    if ( aBuf->Length() )
        {
        // Append comma if not first attribute
        SmartAppendL(aBuf, KComma);
        }

    SmartAppendL(aBuf, attrName->Des());
    SmartAppendL(aBuf, KEquals);
    SmartAppendL(aBuf, attrValue->Des());

    CleanupStack::PopAndDestroy(2); // attrValue, attrName
    }

/*
   RFC 2253 (Lightweight Directory Access Protocol (v3):
   UTF-8 String Representation of Distinguished Names):

   "If the AttributeType is in a published table of attribute types
   associated with LDAP [4], then the type name string from that table
   is used, otherwise it is encoded as the dotted-decimal encoding of
   the AttributeType's OBJECT IDENTIFIER."

   RFC 2256 (A Summary of the X.500(96) User Schema for use with
   LDAPv3):

   "This document provides an overview of the attribute types and object
   classes defined by the ISO and ITU-T committees in the X.500
   documents, in particular those intended for use by directory
   clients."

   In essence, RFC 2256 lists the most common attribute types and their
   names.

   We choose to provide "user-friendly" name mapping for the following
   attribúte types:
   - countryName
   - organizationName
   - organizationalUnitName
   - localityName
   - stateOrProvinceName
   - commonName

   For other attribute types, we use a string representation of the OID
   as the attribute name.
*/

HBufC* PkiUtil::AttributeTypeToNameL(const TDesC &aType)
    {
    HBufC* name;

    if (aType.Compare(KX520CountryName) == 0)
        {
        name = KC().AllocL();
        }
    else if (aType.Compare(KX520OrganizationName) == 0)
        {
        name = KO().AllocL();
        }
    else if (aType.Compare(KX520OrganizationalUnitName) == 0)
        {
        name = KOU().AllocL();
        }
    else if (aType.Compare(KX520LocalityName) == 0)
        {
        name = KL().AllocL();
        }
    else if (aType.Compare(KX520StateOrProvinceName) == 0)
        {
        name = KST().AllocL();
        }
    else if (aType.Compare(KX520CommonName) == 0)
        {
        name = KCN().AllocL();
        }
    else
        {
        // Use the dotted-decimal encoding
        // of the OID as the attribute name
        name = aType.AllocL();
        }

    return name;
    }

void PkiUtil::SmartAppendL(HBufC*& aBuf, const TDesC& aText)
    {
    // Make sure that we have enough space for the new text

    TInt spaceLeft = aBuf->Des().MaxLength() - aBuf->Des().Length();

    if (aText.Length() > spaceLeft)
        {
        // Allocate enough space for the new text + some additional
        // free space so that allocations are not too frequent

        TInt newMaxLength = aBuf->Des().MaxLength() + aText.Length() + KCertDnSizeIncrement;

        aBuf = aBuf->ReAllocL(newMaxLength);
        }

    aBuf->Des().Append(aText);
    }

TCertStatus PkiUtil::CertStatusL(RPKIServiceAPI& aPkiService, const TDesC8& aTrustedCaDn,
                                 const TDesC8& aSubjectDnSuffix, const TDesC8& aRfc822NameFqdn,
                                 TUint aPrivKeyLength, TInt aCertRenewalThreshold)
    {
    // Try to find a user certificate with the
    // desired properties from the PKI store

    TAny* opContext;
    TRequestStatus status;
    TCertStatus certStatus;

    HBufC8* certData = HBufC8::NewL(KExpectedMaxCertSize);
    HBufC8* subjectNameString;
    _LIT8(KEmptyString, "");
    CleanupStack::PushL(certData);

    TPtr8 certDataPtr = certData->Des();

    aPkiService.ReadCertificateL(aTrustedCaDn, aSubjectDnSuffix, aRfc822NameFqdn, EPKIUserCertificate,
                                 aPrivKeyLength, EPKIRSA, certDataPtr,
                                 &opContext, status);

    User::WaitForRequest(status);

    aPkiService.Finalize(opContext);

    if (status.Int() == KPKIErrBufferTooShort)
        {
        TInt realCertSize;
        aPkiService.GetRequiredBufferSize(realCertSize);

        CleanupStack::PopAndDestroy(); //  certData
        certData = HBufC8::NewL(realCertSize);
        CleanupStack::PushL(certData);

        certDataPtr = certData->Des();
        aPkiService.ReadCertificateL(aTrustedCaDn, aSubjectDnSuffix, aRfc822NameFqdn, EPKIUserCertificate,
                                     aPrivKeyLength, EPKIRSA, certDataPtr,
                                     &opContext, status);

        User::WaitForRequest(status);

        aPkiService.Finalize(opContext);
        }


    // Make sure that the cert, if found, is valid
    if (status.Int() == KErrNone)
        {
        certStatus = PkiUtil::CertStatusL(*certData, aCertRenewalThreshold);
        }
    else //if not found, check wether certificate chain exists
        {
        certStatus = ECertNotFound;
        //checking if certificate chain is found
        CleanupStack::PopAndDestroy(); //  certData
        
        certData=NULL;
        certData = HBufC8::NewL(KExpectedMaxCertSize);
        CleanupStack::PushL(certData);
        
        certDataPtr = certData->Des();
        
        //user certificate
        aPkiService.ReadCertificateL(KEmptyString, aSubjectDnSuffix, aRfc822NameFqdn, EPKIUserCertificate,
                aPrivKeyLength, EPKIRSA, certDataPtr,
                &opContext, status);
        User::WaitForRequest(status);
        
        aPkiService.Finalize(opContext);
        if (status.Int() == KErrNone)
           {
            certStatus = PkiUtil::CertStatusL(*certData, aCertRenewalThreshold);
           }
        else
           {
           certStatus = ECertNotFound;
           CleanupStack::PopAndDestroy(certData);
           return certStatus;
           }
        
        //intermediate certificate level 2
        HBufC* issuerName=CertIssuerNameL(*certData);
        CleanupStack::PushL(issuerName);
        
        subjectNameString=To8BitL(*issuerName);
        
        CleanupStack::PopAndDestroy(issuerName);
        CleanupStack::PopAndDestroy(certData);
        
        certData=NULL;
        certData = HBufC8::NewL(KExpectedMaxCertSize);
        CleanupStack::PushL(certData);
        
        certDataPtr = certData->Des();
        
        CleanupStack::PushL(subjectNameString);
        aPkiService.ReadCertificateL(KEmptyString, *subjectNameString, aRfc822NameFqdn, EPKICACertificate,
                                    aPrivKeyLength, EPKIRSA, certDataPtr,
                                    &opContext, status);
        User::WaitForRequest(status);
        
        aPkiService.Finalize(opContext);
        if (status.Int() == KErrNone)
           {
            certStatus = PkiUtil::CertStatusL(*certData, aCertRenewalThreshold);
           }
        else
           {
           certStatus = ECertNotFound;
           CleanupStack::PopAndDestroy(subjectNameString);
           CleanupStack::PopAndDestroy(certData);
           return certStatus;
           }
        if ( subjectNameString->Compare(aTrustedCaDn) !=0 ) //checking if issuer is reached
            {
            //intermediate certificate level 1
            certStatus = ECertNotFound;
            CleanupStack::PopAndDestroy(subjectNameString);
            
            HBufC* issuerName=CertIssuerNameL(*certData);
            CleanupStack::PushL(issuerName);
            
            subjectNameString=To8BitL(*issuerName);
            
            CleanupStack::PopAndDestroy(issuerName);
            CleanupStack::PopAndDestroy(certData);
            
            certData = HBufC8::NewL(KExpectedMaxCertSize);
            CleanupStack::PushL(certData);
            certDataPtr = certData->Des();
            
            CleanupStack::PushL(subjectNameString);
            
            aPkiService.ReadCertificateL(KEmptyString, *subjectNameString, aRfc822NameFqdn, EPKICACertificate,
                                        aPrivKeyLength, EPKIRSA, certDataPtr,
                                        &opContext, status);
            User::WaitForRequest(status);
            
            aPkiService.Finalize(opContext);
            if (status.Int() == KErrNone)
                {
                certStatus = PkiUtil::CertStatusL(*certData, aCertRenewalThreshold);
                }
            else
                {
                certStatus = ECertNotFound;
                CleanupStack::PopAndDestroy(subjectNameString);
                CleanupStack::PopAndDestroy(certData);
                return certStatus;
                }
            }
        if ( subjectNameString->Compare(aTrustedCaDn) !=0 )  //checking if issuer is reached
            {
             //CA certificate
             certStatus = ECertNotFound;
             CleanupStack::PopAndDestroy(subjectNameString);
                        
             HBufC* issuerName=CertIssuerNameL(*certData);
             CleanupStack::PushL(issuerName);
                        
             subjectNameString=To8BitL(*issuerName);
                        
             CleanupStack::PopAndDestroy(issuerName);
             CleanupStack::PopAndDestroy(certData);
                        
             certData = HBufC8::NewL(KExpectedMaxCertSize);
             CleanupStack::PushL(certData);
             certDataPtr = certData->Des();
                        
             CleanupStack::PushL(subjectNameString);
                        
             aPkiService.ReadCertificateL(KEmptyString, *subjectNameString, aRfc822NameFqdn, EPKICACertificate,
                                          aPrivKeyLength, EPKIRSA, certDataPtr,
                                          &opContext, status);
             User::WaitForRequest(status);
                        
             aPkiService.Finalize(opContext);
             if (status.Int() == KErrNone)
                 {
                 certStatus = PkiUtil::CertStatusL(*certData, aCertRenewalThreshold);
                 }
             CleanupStack::PopAndDestroy(subjectNameString);
             }
        else
             CleanupStack::PopAndDestroy(subjectNameString);
        }
    CleanupStack::PopAndDestroy(certData);

    return certStatus;
    }

TCertStatus PkiUtil::CertStatusL(const TDesC8& aCertData, TInt aCertRenewalThreshold)
    {
    CX509Certificate* certificate = CX509Certificate::NewL(aCertData);
    CleanupStack::PushL(certificate);

    TCertStatus status = CertStatusL(*certificate, aCertRenewalThreshold);

    CleanupStack::PopAndDestroy(certificate);

    return status;
    }


TCertStatus PkiUtil::CertStatusL(const CX509Certificate& aCert, TInt aCertRenewalThreshold)
    {

    TTime validationTime;
    validationTime.UniversalTime();

    TCertStatus certStatus = ECertValid;

    if (aCert.ValidityPeriod().Finish() <= validationTime)
        {
        certStatus = ECertExpired;
        }

    if (aCert.ValidityPeriod().Start() >= validationTime)
        {
        certStatus = ECertNotValidYet;
        }

    // Additional validity check - if a certificate renewal threshold
    // has been specified, it can cause an otherwise valid certificate
    // to be marked as expired. This is determined as follows:
    // - Calculate the entire lifetime of the certificate (entireLifetime)
    // - Calculate the passed lifetime of the certificate (passedLifetime)
    // - Calculate thresholdLifetime as aCertRenewalTreshold % * entireLifetime
    //   (i.e. (aCertRenewalTreshold / 100) * entireLifetime
    // - If passedLifetime > thresholdLifetime --> certificate expired

    if (certStatus == ECertValid && aCertRenewalThreshold >= 0)
        {
        TInt64 entireLifetime = aCert.ValidityPeriod().Finish().Int64() -
                                aCert.ValidityPeriod().Start().Int64();

        TInt64 passedLifetime = validationTime.Int64() -
                                aCert.ValidityPeriod().Start().Int64();

        TInt64 thresholdInPercents(aCertRenewalThreshold);
        TInt64 hundred(100);
        TInt64 tresholdLifetime = (thresholdInPercents * entireLifetime) / hundred;

        if (passedLifetime > tresholdLifetime)
            {
            certStatus = ECertExpired;
            }
        }

    return certStatus;
    }


CX509Certificate* PkiUtil::ReadCertificateLC(RPKIServiceAPI& aPkiService,
                                             const TPKIKeyIdentifier& aKeyId)
    {

    TAny* opContext;
    TRequestStatus status;

    HBufC8* certData = HBufC8::NewL(KExpectedMaxCertSize);
    CleanupStack::PushL(certData);

    TPtr8 certDataPtr = certData->Des();

    aPkiService.ReadCertificateL(aKeyId, certDataPtr,
                                 &opContext,
                                 status);

    User::WaitForRequest(status);

    aPkiService.Finalize(opContext);

    if (status.Int() == KPKIErrBufferTooShort)
        {
        TInt realCertSize;
        aPkiService.GetRequiredBufferSize(realCertSize);

        CleanupStack::PopAndDestroy(); //  certData
        certData = HBufC8::NewL(realCertSize);
        CleanupStack::PushL(certData);

        certDataPtr = certData->Des();
        aPkiService.ReadCertificateL(aKeyId, certDataPtr,
                                     &opContext,
                                     status);

        User::WaitForRequest(status);
        aPkiService.Finalize(opContext);

        User::LeaveIfError(status.Int());
        }
    else
        {
        User::LeaveIfError(status.Int());
        }

    CX509Certificate* certificate = CX509Certificate::NewL(*certData);
    CleanupStack::PopAndDestroy(certData);
    CleanupStack::PushL(certificate);

    return certificate;
    }


CX509Certificate* PkiUtil::ReadCertificateLC(RPKIServiceAPI& aPkiService,
                                             const TDesC8& aTrustedAuthority,
                                             const TDesC8& aIdentitySubjectName,
                                             const TDesC8& aIdentityRfc822Name,
                                             const TPKICertificateOwnerType aOwnerType)
    {
    // Try to find a user certificate with the
    // desired properties from the PKI store
    static const TInt KUndefinedKeySize = 0;

    TAny* opContext;
    TRequestStatus status;

    HBufC8* certData = HBufC8::NewL(KExpectedMaxCertSize);
    CleanupStack::PushL(certData);

    TPtr8 certDataPtr = certData->Des();

    aPkiService.ReadCertificateL(aTrustedAuthority,
                                 aIdentitySubjectName,
                                 aIdentityRfc822Name,
                                 aOwnerType, KUndefinedKeySize,
                                 EPKIRSA, certDataPtr,
                                 &opContext,
                                 status);

    User::WaitForRequest(status);

    aPkiService.Finalize(opContext);

    if (status.Int() == KPKIErrBufferTooShort)
        {
        TInt realCertSize;
        aPkiService.GetRequiredBufferSize(realCertSize);

        CleanupStack::PopAndDestroy(); //  certData
        certData = HBufC8::NewL(realCertSize);
        CleanupStack::PushL(certData);

        certDataPtr = certData->Des();
        aPkiService.ReadCertificateL(aTrustedAuthority,
                                     aIdentitySubjectName,
                                     aIdentityRfc822Name,
                                     aOwnerType, KUndefinedKeySize,
                                     EPKIRSA, certDataPtr,
                                     &opContext,
                                     status);

        User::WaitForRequest(status);
        aPkiService.Finalize(opContext);

        User::LeaveIfError(status.Int());
        }
    else
        {
        User::LeaveIfError(status.Int());
        }

    CX509Certificate* certificate = CX509Certificate::NewL(*certData);
    CleanupStack::PopAndDestroy(certData);
    CleanupStack::PushL(certificate);

    return certificate;
    }


void ResetAndDestroyCleanup(TAny* aArray)
    {
    RPointerArray<CX509Certificate>* certificateArray = (RPointerArray<CX509Certificate>*) aArray;
    certificateArray->ResetAndDestroy();
    certificateArray->Close();
    }

void ResetAndDestroyPushL(RPointerArray<CX509Certificate>& aCertificateArray)
    {
    CleanupStack::PushL(TCleanupItem(ResetAndDestroyCleanup, &aCertificateArray));
    }


RPointerArray<CX509Certificate> PkiUtil::GetCaCertListL(RPKIServiceAPI& aPkiService,
                                                        const CArrayFixFlat<TCertInfo*>& aIkeCAList)
    {

    __ASSERT_ALWAYS(aIkeCAList.Count() > 0, User::Invariant());
    _LIT8(KEmptyString, "");

    RPointerArray<CX509Certificate> certificateArray;
    ResetAndDestroyPushL(certificateArray);

    RArray<TUid> applUidArray;
    CleanupClosePushL(applUidArray);

    for (TInt i = 0; i < aIkeCAList.Count(); ++i)
        {
        const TCertInfo* certInfo = aIkeCAList[i];
        switch(certInfo->iFormat)
            {
            case CA_NAME:
                {
                // Reserve enough space for UTF-8
                TInt len = 3*( certInfo->iData.Length() );
                HBufC8* caName = HBufC8::NewLC(len);
                TPtr8 caNamePtr(caName->Des());

                if (CnvUtfConverter::ConvertFromUnicodeToUtf8(caNamePtr, certInfo->iData) != 0)
                    {
                    User::Leave(KErrCorrupt);
                    }

                CX509Certificate* cert = ReadCertificateLC(aPkiService,
                                                           KEmptyString,
                                                           *caName,
                                                           KEmptyString,
                                                           EPKICACertificate);

                User::LeaveIfError(certificateArray.Append(cert));
                CleanupStack::Pop(cert);
                CleanupStack::PopAndDestroy(caName);
                }
                break;
            case KEY_ID:
                {
                TPKIKeyIdentifier keyId(NULL);

                for (TInt j = 0; j < certInfo->iData.Length(); j += 2)
                    {
                    TPtrC hexByte(certInfo->iData.Mid(j, 2));
                    TLex lex(hexByte);
                    TUint8 value;
                    User::LeaveIfError(lex.Val(value, EHex));
                    keyId.Append(&value, 1);
                    }

                CX509Certificate* cert = ReadCertificateLC(aPkiService,
                                                           keyId);
                User::LeaveIfError(certificateArray.Append(cert));
                CleanupStack::Pop(cert);
                }
                break;
            case APPL_UID:
                {
                TLex lex(certInfo->iData);
                TUint32 value;
                User::LeaveIfError(lex.Val(value, EHex));
                TUid id = { value };
                User::LeaveIfError(applUidArray.Append(id));
                }
                break;
            default:
                User::Leave(KErrArgument);
                break;
            }
        }

        if (applUidArray.Count() > 0)
            {
            CArrayFix<TCertificateListEntry>* certListArray = NULL;;
            aPkiService.ListApplicableCertificatesL(applUidArray, certListArray);

            CleanupStack::PushL(certListArray);
            if (certListArray->Count() == 0)
                {
                User::Leave(KErrNotFound);
                }

            for (TInt i = 0; i < certListArray->Count(); ++i)
                {
                TCertificateListEntry entry = (*certListArray)[i];
                if (entry.iOwnerType == EPKICACertificate)
                    {
                    CX509Certificate* cert = ReadCertificateLC(aPkiService,
                                                               KEmptyString,
                                                               entry.iIdentitySubjectName,
                                                               KEmptyString,
                                                               EPKICACertificate);
                    User::LeaveIfError(certificateArray.Append(cert));
                    CleanupStack::Pop(cert);
                    }
                }
            CleanupStack::PopAndDestroy(certListArray);
            }

        CleanupStack::PopAndDestroy(); //applUidArray

    CleanupStack::Pop();
    return certificateArray;
    }


TCertStatus PkiUtil::GetValidCaCertSubjectNameListL(RPKIServiceAPI& aPkiService,
                                                   const CArrayFixFlat<TCertInfo*>& aIkeCAList,
                                                   CDesC8ArrayFlat& aCaCertNameList)
    {

    TCertStatus status = ECertValid;
    RPointerArray<CX509Certificate> certificateList;
    TRAPD(err, certificateList = PkiUtil::GetCaCertListL(aPkiService, aIkeCAList));

    switch(err)
        {
        case KErrNone:
            for (TInt i = 0; i < certificateList.Count(); ++i)
                {
                const CX509Certificate* cert = certificateList[i];
                status = CertStatusL(*cert, -1);

                if (status == ECertValid)
                    {
                    const CX500DistinguishedName& subjectName = cert->SubjectName();
                    HBufC* subjectNameString = PkiUtil::CertDnL(subjectName);
                    CleanupStack::PushL(subjectNameString);

                    // Reserve enough space for UTF-8
                    TInt len = 3*( subjectNameString->Length() );
                    HBufC8* subjectNameString8Bit = HBufC8::NewLC(len);
                    TPtr8 subjectNameString8BitPtr = subjectNameString8Bit->Des();
                    if (CnvUtfConverter::ConvertFromUnicodeToUtf8(subjectNameString8BitPtr,
                                                                  *subjectNameString) != 0)
                        {
                        User::Leave(KErrCorrupt);
                        }
                    TRAPD(err, aCaCertNameList.InsertIsqL(*subjectNameString8Bit));

                    CleanupStack::PopAndDestroy(subjectNameString8Bit);
                    CleanupStack::PopAndDestroy(subjectNameString);

                    err = (err == KErrAlreadyExists) ? KErrNone : err;
                    User::LeaveIfError(err);
                    }
                else
                    {
                    aCaCertNameList.Reset();
                    break;
                    }
                }
            break;
        case KErrNotFound: // Flow through
        case KErrArgument: // CA cert is not found if policys CA DATA is invalid
            status = ECertNotFound;
            break;
        default:
            certificateList.ResetAndDestroy();
            certificateList.Close();
            User::Leave(err);
        }

    certificateList.ResetAndDestroy();
    certificateList.Close();

    return status;
    }


TCertStatus PkiUtil::CheckUserCertValidityL(RPKIServiceAPI& aPkiService,
                                            CDesC8ArrayFlat& aValidCaCertNameList,
                                            TOwnCertInfo& aOwnCert)
    {

    static const TInt KUndefinedKeySize = 0;

    TCertStatus status = ECertValid;

    HBufC8* subjectName = NULL;
    HBufC8* rfc822Name = NULL;

    if (aOwnCert.iOwnCertExists)
        {
        // Reserve enough space for UTF-8
        TInt len = 3*( aOwnCert.iSubjectDnSuffix.Length() );
        subjectName = HBufC8::NewLC(len);
        TPtr8 subjectNamePtr = subjectName->Des();
        if (CnvUtfConverter::ConvertFromUnicodeToUtf8(subjectNamePtr, aOwnCert.iSubjectDnSuffix) != 0)
            {
            User::Leave(KErrCorrupt);
            }

        if ( aOwnCert.iIdentityAsRfc822Name == EIdentityAsRfc822NameYes)
            {
            rfc822Name = HBufC8::NewLC(aOwnCert.iRfc822NameFqdn.Length());
            TPtr8 rfc822NamePtr = rfc822Name->Des();
            if (CnvUtfConverter::ConvertFromUnicodeToUtf8(rfc822NamePtr, aOwnCert.iRfc822NameFqdn) != 0)
                {
                User::Leave(KErrCorrupt);
                }

            }
        else
            {
            rfc822Name = HBufC8::NewLC(1); //empty string
            }

            status = ECertNotFound;
            for (TInt i = 0; i < aValidCaCertNameList.Count(); ++i)
                {

                status = CertStatusL(aPkiService, aValidCaCertNameList[i],
                                     *subjectName, *rfc822Name,
                                     KUndefinedKeySize);
                if (status != ECertNotFound)
                    {
                    break;
                    }
                }

            CleanupStack::PopAndDestroy(rfc822Name);
            CleanupStack::PopAndDestroy(subjectName);
        }

    return status;
    }


CX500DistinguishedName* PkiUtil::DnFromStringL(const TDesC8& aString)
    {
    HBufC8* dnDer;
    CPkcs10Req::BuildDistinguishedNameDerFromTextL(dnDer, aString,
                                                   EFalse, KNullDesC8);
    CleanupStack::PushL(dnDer);

    CX500DistinguishedName* dn = CX500DistinguishedName::NewL(*dnDer);

    CleanupStack::PopAndDestroy(); // dnDer

    return dn;
    }

CX500DistinguishedName* PkiUtil::DnWithoutCnFromStringL(const TDesC8& aString)
    {
    HBufC8* dnDer;
    CPkcs10Req::BuildDistinguishedNameDerFromTextL(dnDer, aString,
                                                   ETrue, KCN8);
    CleanupStack::PushL(dnDer);

    CX500DistinguishedName* dn = CX500DistinguishedName::NewL(*dnDer);

    CleanupStack::PopAndDestroy(); // dnDer

    return dn;
    }

TBool PkiUtil::DnMatchL(const TDesC8& aDnString1, const TDesC8& aDnString2)
    {
    TBool exactMatch = EFalse;

    if (aDnString1.Length() == 0 && aDnString2.Length() == 0)
        {
        exactMatch = ETrue;
        }
    else if (aDnString1.Length() > 0 && aDnString2.Length() > 0)
        {
        CX500DistinguishedName* dn1 = DnFromStringL(aDnString1);
        CleanupStack::PushL(dn1);

        CX500DistinguishedName* dn2 = DnFromStringL(aDnString2);
        CleanupStack::PushL(dn2);

        if (MatchL(*dn1, *dn2))
            {
            exactMatch = ETrue;
            }

        CleanupStack::PopAndDestroy(2); // dn2, dn1
        }

    return exactMatch;
    }

TBool PkiUtil::MatchL(const CX500DistinguishedName& aDn1, const CX500DistinguishedName& aDn2)
    {
    if (aDn1.Count() != aDn2.Count())
        {
        return EFalse;
        }

    // For each field in aDn1, aDn2 must contain a field with the same value
    for (TInt i = 0; i < aDn1.Count(); i++)
    {
        if (!HasElementL(aDn2, aDn1.Element(i)))
            {
            return EFalse;
            }
        }
    return ETrue;
    }

TBool PkiUtil::HasElementL(const CX500DistinguishedName& aDn, const CX520AttributeTypeAndValue& aElement)
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

HBufC8* PkiUtil::To8BitL(const TDesC16& aDes)
    {
    HBufC8* desCopy;
    
    TInt desLength = aDes.Length();
    
    if (desLength > 0)
        {
        desCopy = HBufC8::NewL(desLength);
        desCopy->Des().Copy(aDes);
        }
    else
        {
        desCopy = HBufC8::NewL(1);
        }

    return desCopy;
    }
