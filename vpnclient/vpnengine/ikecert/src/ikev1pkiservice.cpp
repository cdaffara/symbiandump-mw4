/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   PKI store and Certificate verification interface class
*                implementation for IKEv1 plug-in
*
*/

#include <x500dn.h>
#include <x509cert.h>
#include <asn1dec.h>
#include <utf.h>

#include "ikedebug.h"
#include "ikev1pkiservice.h"
#include "utlcrypto.h"
#include "ikecert.h"
#include "ikecaelem.h"
#include "ikepublickey.h"
#include "ikecalist.h"
#include "ikepkiutils.h"
#include "pkcs10.h"
#include "vpnapidefs.h"
#include "pkiutil.h"
#include "ikecertconst.h"

//
// CIkeV1PkiService Class
//
_LIT8(KEmptyString, "");


EXPORT_C CIkeV1PkiService* CIkeV1PkiService::NewL(
    CIkeData*             aIkeData,
    MIkeDebug&            aDebug
)
{
    CIkeV1PkiService* self =
        new (ELeave) CIkeV1PkiService(aIkeData, aDebug);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}


CIkeV1PkiService::CIkeV1PkiService(
    CIkeData*            aIkeData,
    MIkeDebug&           aDebug
) :
    iOperation(KNoOperation),
    iIkeData(aIkeData),
    iCertPtr(NULL, 0),
    iCertBfrSize(2048),
    iDebug(aDebug)
{
}


void CIkeV1PkiService::ConstructL()
{
    User::LeaveIfError(iPkiService.Connect());
    
    // Set certificate store type to device certificate store,
    // if Own_cert_type is defined as "DEVICE"
    if ( iIkeData->iClientCertType != NULL )
    {
        TPtrC16 certStoreType = iIkeData->iClientCertType->GetData();
        if ( certStoreType.CompareF(_L("DEVICE")) == 0 )
        {
			User::LeaveIfError(iPkiService.SetStoreType(EPkiStoreTypeDevice));            
        }
        else
        {
			User::LeaveIfError(iPkiService.SetStoreType(EPkiStoreTypeUser));                    
        }
    }

    iTrustedCAList   = new (ELeave) CIkeCaList(2);
    iReadCertificate = HBufC8::NewL(iCertBfrSize);
    

    //The code assumes that these are not NULL.
    //Reallocated, when needed
    iSubjName = HBufC8::NewL(2);
    iRfc822Name = HBufC8::NewL(2);
}


EXPORT_C CIkeV1PkiService::~CIkeV1PkiService()
{
    if ( iApplUidList )
    {
        iApplUidList->Reset();
        delete iApplUidList;
    }
    
    if ( iCaCertList )
    {
        iCaCertList->Reset();
        delete iCaCertList;
    }

    delete iTrustedCAList;

    iCasTrustedByPeer.Reset();
    iCasTrustedByPeer.Close();

    delete iCaName;
    delete iCa1Name;
    delete iCa2Name;
   
    delete iReadCertificate;
    delete iSubjName;
    delete iRfc822Name;

    
    iPkiService.Close();
}


EXPORT_C TBool CIkeV1PkiService::ImportCACertsL(
    CArrayFixFlat<TCertInfo*> *aCAList
)
{
    DEBUG_LOG(_L("-> CIkeV1PkiService::ImportCACertsL"));

    //
    // Build trusted CA certificate list into CIkeV1PkiService object
    // aCAList call parameter array contains the list of trusted CA:s
    // (names in ASCII format).
    // Read corresponding certificate from PKI store and if found,
    // add a new element (=CIkeCaElem) into CIkeCaList
    //
    TBool Status = EFalse;

    if ( aCAList && aCAList->Count() )
    {
        delete iCaName;

        iCaName     = NULL;
        iCaName     = HBufC8::NewL(256);
        iCaNameList = aCAList;
        iCurrIndex  = 0;
        iOperation  = KBuildingCaList;

        Status = GetNextCaElemL();
    }

    return Status;
}


//
// CIkeV1PkiService::ReadCertWithNameL
// This method is used to read a certificate from the PKI store.
// Input parameters:
// -- const TDesC8& Trusted CA name
// -- TBool  aGetCACert
//    ETrue = Read a CA certificate; EFalse read an user certificate
// Output parameters:
// -- X509 certificate into iReadCertificate buffer
//
EXPORT_C TInt CIkeV1PkiService::ReadUserCertWithNameL(
    const TDesC8& aTrustedCaName, CIkeData* aIkeData, TBool aDnType)

{
    iIkeData = aIkeData;
    
    delete iCaName;
    iCaName = NULL;
    iCaName = HBufC8::NewL(aTrustedCaName.Length());
    iCaName->Des().Copy(aTrustedCaName);
    
    delete iReadCertificate;
    iReadCertificate=NULL;
    
    TInt Status = ReadCertificateL(*iCaName, EFalse);

    iUserCertDerType=aDnType;
    return Status;
}
EXPORT_C TInt CIkeV1PkiService::ReadChainL(CIkeData* aIkeData, const HBufC8* aCAName)
{
    iIkeData = aIkeData;
    delete iReadCertificate;
    iReadCertificate=NULL;
    //read own certificate
    TInt Status = ReadCertificateL(KEmptyString, EFalse);
    TInt StatusICA1 = KErrNotFound;
    TInt StatusICA2 = KErrNotFound;
    TInt StatusICA = KErrNotFound;
       
    if ( Status == KErrNone )
        {
        iReadCertificateOrig = HBufC8::NewL(iReadCertificate->Length());
        TPtr8 iReadCertCopy(iReadCertificateOrig->Des());
        iReadCertCopy.Copy(iReadCertificate->Des());
        delete iCaName;
        iCaName = NULL;
        iCaName =  IkeCert::GetCertificateFieldDERL(iReadCertificate, KIssuerName);
        
        //Read ICA2
        StatusICA2 = ReadCertificateL(KEmptyString, ETrue);
       
        if ( StatusICA2 != KErrNone)
            {
            delete iReadCertificateOrig;
            iReadCertificateOrig=NULL;
            return KErrNotFound;
            }
        }
    if ( Status == KErrNone && StatusICA2 == KErrNone)
        {
        
        delete iCaName;
        iCaName = NULL;
        iCaName =  IkeCert::GetCertificateFieldDERL(iReadCertificate, KIssuerName);
        delete iCa2Name;
        iCa2Name=NULL;
        iCa2Name = GetCertificate();
        
        CX500DistinguishedName* dn=NULL;
        CX500DistinguishedName* asn1DnNameofICaName = NULL;
        dn = CX500DistinguishedName::NewLC(*aCAName);
        asn1DnNameofICaName = CX500DistinguishedName::NewLC(*iCaName);
               
        if (  asn1DnNameofICaName->ExactMatchL(*dn)  )
            
            {
             StatusICA=KErrNone;
             //read ICA1
             StatusICA1 = ReadCertificateL(KEmptyString, ETrue);
             if ( StatusICA1 != KErrNone)
                 {
                 delete iReadCertificateOrig;
                 iReadCertificateOrig=NULL;
                 
                 CleanupStack::PopAndDestroy(asn1DnNameofICaName);
                 asn1DnNameofICaName=NULL;
                 
                 CleanupStack::PopAndDestroy(dn);
                 dn=NULL;
                 
                 return KErrNotFound;
                 }
            }
        else
            {
            StatusICA1 = ReadCertificateL(KEmptyString, ETrue);
            if ( StatusICA1 == KErrNotFound)
                {
                delete iReadCertificateOrig;
                iReadCertificateOrig=NULL;
                             
                CleanupStack::PopAndDestroy(asn1DnNameofICaName);
                asn1DnNameofICaName=NULL;

                CleanupStack::PopAndDestroy(dn);
                dn=NULL;
            
                return KVpnErrInvalidCaCertFile;
                }
            else
	            StatusICA1=KErrNone;
            }
        CleanupStack::PopAndDestroy(asn1DnNameofICaName);
        asn1DnNameofICaName=NULL;
        
        CleanupStack::PopAndDestroy(dn);
        dn=NULL;
       
        }
 
    if ( Status == KErrNone && StatusICA1 == KErrNone && StatusICA2 == KErrNone)
        {
        if ( StatusICA == KErrNotFound )
           {
           delete iCaName;
           iCaName = NULL;
           iCaName =  IkeCert::GetCertificateFieldDERL(iReadCertificate, KIssuerName);
           }
        delete iCa1Name;
        iCa1Name=NULL;
        iCa1Name = GetCertificate();
        
        CX500DistinguishedName* dn=NULL;
        CX500DistinguishedName* asn1DnNameofICaName = NULL;
        
        dn = CX500DistinguishedName::NewLC(*aCAName);
        
        asn1DnNameofICaName = CX500DistinguishedName::NewLC(*iCaName);
        
        if ( asn1DnNameofICaName->ExactMatchL(*dn) )
           {
           delete iCaName;
           iCaName=NULL;
           iCaName=HBufC8::NewL(aCAName->Length());
           iCaName->Des().Copy(*aCAName);
           
           TInt Status = ReadCertificateL(KEmptyString, ETrue);
           
           delete iCaName;
           iCaName = NULL;
           iCaName =  IkeCert::GetCertificateFieldDERL(iCa2Name, KSubjectName);
           
           delete iReadCertificate;
           iReadCertificate=iReadCertificateOrig;
           iReadCertificateOrig=NULL;
           
           CleanupStack::PopAndDestroy(asn1DnNameofICaName);
           asn1DnNameofICaName=NULL;
           
           CleanupStack::PopAndDestroy(dn);
           dn=NULL;
          
           if ( Status!=KErrNone )
               return KVpnErrInvalidCaCertFile;
           else    
	           return KErrNone;
           }
        else
           {
           delete iReadCertificate;
           
           iReadCertificate=iReadCertificateOrig;
           iReadCertificateOrig=NULL;
           delete iReadCertificateOrig; 
           
           delete iReadCertificate;
           iReadCertificate=NULL;
           
           CleanupStack::PopAndDestroy(asn1DnNameofICaName);
           asn1DnNameofICaName=NULL;
           
           CleanupStack::PopAndDestroy(dn);
           dn=NULL;
           
           return KErrNotFound;
           }
        }
     return KErrNotFound;
}

//
// CIkeV1PkiService::Ikev1SignatureL
// This method is used to compute IKEv1 signature with a specified private key.
// Actually a signature computed happens by referring the related certificate
// when the PKI store produces the signature with corresponding private key.
// Parameters:
// -- const TDesC8& aTrustedAuthority
//    Trusted CA name coded either in ASN1 (DN) format or ASCII format
// -- CIkeData* aHostData
//    Related IKE configuration section. Used to get IdentitySubjectName or
//    Identity Rfc822 Name information for actual PKI service ReadCertificateL
//    method call
// -- const TDesC8& aHashIn
//    Hash data signed (in matter of fact the hash data is simply
//    encrypted with private key)
// Return:
// -- TInt, sign length
//
EXPORT_C TInt CIkeV1PkiService::Ikev1SignatureL(
    const TDesC8&  aTrustedCaName,
    CIkeData*      aIkeData,
    const TDesC8&  aHashIn,
    TDes8&         aSignature
)
{
    iIkeData = aIkeData;
    return ComputeSignatureL(aTrustedCaName, aHashIn, aSignature, EFalse);
}


EXPORT_C CIkeCaList* CIkeV1PkiService::CaList()
{
    return iTrustedCAList;
}


EXPORT_C HBufC8* CIkeV1PkiService::GetCertificate()
{
    HBufC8* Cert = iReadCertificate;
    iReadCertificate = NULL;
    return Cert;
}


EXPORT_C HBufC8* CIkeV1PkiService::GetTrustedCA()
{
    HBufC8* Cert = iCaName;
    iCaName = NULL;
    return Cert;
}


EXPORT_C HBufC8* CIkeV1PkiService::GetTrustedICA1()
{
    HBufC8* Cert = iCa1Name;
    iCa1Name = NULL;
    return Cert;
}

EXPORT_C HBufC8* CIkeV1PkiService::GetTrustedICA2()
{
    HBufC8* Cert = iCa2Name;
    iCa2Name = NULL;
    return Cert;
}

TInt CIkeV1PkiService::ComputeSignatureL(
    const TDesC8&  aTrustedAuthority,
    const TDesC8&  aHashIn,
    TDes8&         aSignature,
    TBool          aRsaSignature
)
{
    DEBUG_LOG(_L("-> CIkeV1PkiService::ComputeSignatureL"));

    TPKIKeyAlgorithm keyAlgorithm = EPKIRSA;
    TUint keySize = InitUserCertIdentDataL();
    HBufC8* Asn1EncodedHash = NULL;
    TPtrC8 hashIn(aHashIn);

    if ( aRsaSignature )
    {
        //
        // Build PKCS1v15 format signature (ASN1 encoded)
        //
        Asn1EncodedHash = IkeCert::BuildPkcs1v15HashL(aHashIn);
        
        ASSERT( Asn1EncodedHash != NULL );        
        hashIn.Set(Asn1EncodedHash->Des());
    }

    TInt SignLth = 0;
    TInt err = iPkiService.Sign(aTrustedAuthority, *iSubjName, *iRfc822Name,
                                EX509DigitalSignature, keySize,
                                keyAlgorithm, hashIn, aSignature);

    if (err == KErrNone)
    {
        SignLth = aSignature.Length();
    }

    DEBUG_LOG2(_L("Sign returned %d, length=%d"), err, SignLth);
    User::LeaveIfError(err);

    delete Asn1EncodedHash;
    return SignLth;
}


TInt CIkeV1PkiService::ReadCertificateL(
    const TDesC8& aTrustedAuthority, TBool aGetCACert
)
{
    //
    // Read certificate from PKI store using pkiserviceapi
    //
    DEBUG_LOG(
        _L("-> ReadCertificateL(aTrustedAuthority, aGetCACert)")
    );

    TInt Status = KErrNone;
    TPKIKeyAlgorithm keyAlgorithm = EPKIRSA;
    TPKICertificateOwnerType ownerType;
    TUint keySize = 0;

    if (aGetCACert)
    {
        DEBUG_LOG(_L("Reading CA certificate"));

        ownerType = EPKICACertificate;

        //Init CA cert ident data.
        //aTrustedAuthority (issuer) checking for CA certs is not supported.
        if ( aTrustedAuthority.Length() == 0 )
            {
            delete iSubjName;
            iSubjName = NULL;
            iSubjName = iCaName->AllocL();
            iRfc822Name->Des().Zero();
            } 
      }
    else
    {
        DEBUG_LOG(_L("Reading User certificate"));
        ownerType = EPKIUserCertificate;
        keySize = InitUserCertIdentDataL();
    }

    for (;;)    // Only for easy exits...
    {
        if ( iReallocated )
        {
            //
            // Allocate a new buffer for ASN1 coded certificate read from
            // PKI store. Buffer size is now asked from pkiserviceapi
            //
            delete iReadCertificate;
            iReadCertificate = NULL;
            TInt RealCertSize;
            
            if ( iPkiService.GetRequiredBufferSize(RealCertSize) == KErrNone )
                iCertBfrSize = (RealCertSize | 0x3) + 1;
            // Try double size in error case
            else iCertBfrSize = (iCertBfrSize << 1);
        }
        
        if ( !iReadCertificate )
        {
            iReadCertificate=NULL;
            iReadCertificate = HBufC8::NewL(iCertBfrSize);
        }
        
        iCertPtr.Set(iReadCertificate->Des());
        iCertPtr.Zero();

        TRequestStatus status;
        iPkiService.ReadCertificateL(aTrustedAuthority,
                                     *iSubjName, *iRfc822Name,
                                     ownerType, keySize,
                                     keyAlgorithm, iCertPtr,
                                     &iResArray, status);


        User::WaitForRequest(status);
        Status = status.Int();
        iPkiService.Finalize(iResArray);
        iResArray = NULL;
        
        if ( (Status == KPKIErrBufferTooShort) && !iReallocated )
        {
            //
            // Certificate buffer was too small try to read once more if
            // not already tried
            //
            iReallocated = ETrue;
        }
        else
        {
            if ( Status == KErrNone )
            {
                //iReadCertificate->Des().SetLength(iCertPtr.Length());
                iReallocated = EFalse;
            }
            break;
        }
        
    }

    DEBUG_LOG(
        _L("<- ReadCertificateL(aTrustedAuthority, aGetCACert)")
    );

    return Status;
}


TUint CIkeV1PkiService::InitUserCertIdentDataL()
{
    DEBUG_LOG(_L("-> CIkeV1PkiService::InitUserCertIdentDataL"));
    __ASSERT_ALWAYS(iIkeData != NULL, User::Invariant());

    TUint keySize = 0;        // Default: Length is undefined

    if ( !iReallocated )
    {
        //
        //  Get possible user identity information from current IKE policy
        //  section and convert it from 16-bit Unicode into UTF-8 format
        //
        TInt Lth = 3*( iIkeData->iOwnCert.iSubjectDnSuffix.Length() );

        if ( Lth )
        {
            delete iSubjName;
            iSubjName = NULL;
            iSubjName = HBufC8::NewL(Lth);

            TPtr8   dn8 = iSubjName->Des();
            TPtrC16 dn16( iIkeData->iOwnCert.iSubjectDnSuffix );

            if ( 0 != CnvUtfConverter::ConvertFromUnicodeToUtf8(
                dn8, dn16 ) )
            {
                User::Leave(KErrCorrupt);
            }
        }
        else
        {
            iSubjName->Des().Zero();
        }

        Lth = iIkeData->iOwnCert.iRfc822NameFqdn.Length();

        if ( Lth )
        {
            delete iRfc822Name;
            iRfc822Name = NULL;
            iRfc822Name = HBufC8::NewL(Lth);
            iRfc822Name->Des().Copy(iIkeData->iOwnCert.iRfc822NameFqdn);
        }
        else
        {
            iRfc822Name->Des().Zero();
        }

        if ( iIkeData->iOwnCert.iPrivateKeyLength )
        {
            keySize = iIkeData->iOwnCert.iPrivateKeyLength;
        }
    }

    DEBUG_LOG(_L("<- CIkeV1PkiService::InitUserCertIdentDataL"));
    return keySize;
}


TBool CIkeV1PkiService::GetNextCaElemL()
{
    //
    // Get next CA certificate from PKI store using current CA name in
    // iCaNameList.
    //
    DEBUG_LOG(_L("-> CIkeV1PkiService::GetNextCaElemL"));

    TCertInfo* CertInfo;
    TBool Ret;

    Ret = EFalse;
    TInt Status;

    while ( iCurrIndex < iCaNameList->Count() )
    {
        CertInfo = iCaNameList->At(iCurrIndex);
        
        if ( CertInfo->iFormat == CA_NAME )
        {
            TPtr8   dn8 = iCaName->Des();
            TPtrC16 dn16( CertInfo->iData );

            if ( 0 != CnvUtfConverter::ConvertFromUnicodeToUtf8(
                dn8, dn16
            ) )
            {
                User::Leave(KErrCorrupt);
            }

            Status = ReadCertificateL(KEmptyString, ETrue);
            Ret |= AddNextCaElemL(Status);
        }
        else if ( CertInfo->iFormat == KEY_ID )
        {
            Status = GetCertificateWithKeyIdL(CertInfo->iData);
            Ret |= AddNextCaElemL(Status);
        }
        else if ( CertInfo->iFormat == APPL_UID )
        {
            Ret |= GetApplUidListL(CertInfo->iData);
        }
        else
        {
            Ret |= EFalse;
            iCurrIndex ++;
            DEBUG_LOG1(
                _L("Unsupported CA certificate element format = %d"),
                CertInfo->iFormat
            );
        }

    }

    iCaNameList = NULL;

    DEBUG_LOG(_L("<- CIkeV1PkiService::GetNextCaElemL"));
    return Ret;
}


TBool CIkeV1PkiService::AddNextCaElemL(TInt& aStatus)
{
    DEBUG_LOG(_L("-> CIkeV1PkiService::AddNextCaElemL()"));
    
    //
    // CA has been read PKI store. Build and add a new CIkeCaElem to CIkeCaList
    //
#ifdef _DEBUG    
    CertReadCompleted(ETrue, aStatus, __LINE__);
#endif // _DEBUG    

    if (aStatus == KErrNotFound)
    {
        DEBUG_LOG(_L(" Leave: status == KErrNotFound"));
        User::Leave(KVpnErrInvalidCaCertFile);
    }

    TBool Ret;
    
    if ( aStatus == KErrNone )
    {
        ASSERT(iReadCertificate);
        HBufC8* CaCert = iReadCertificate; // Link CA buffer to CIkeCaElem
        CaCert->Des().SetLength(iCertPtr.Length());
        iReadCertificate = NULL;
        CleanupStack::PushL(CaCert);
        CIkeCaElem* CaElem = CIkeCaElem::NewL(CaCert);
        CleanupStack::Pop(CaCert);
        CleanupStack::PushL(CaElem);
        iTrustedCAList->AppendL(CaElem);
        CleanupStack::Pop(CaElem);

        if ( iOperation == KProcessingApplUidList )
            iListIndex ++;
        else iCurrIndex ++;

        Ret = ETrue;
    }
    else
    {
        if ( iOperation == KProcessingApplUidList )
            iListIndex ++;
        else iCurrIndex ++;

        Ret = EFalse;
    }

    DEBUG_LOG(_L("<- CIkeV1PkiService::AddNextCaElemL()"));
    
    return Ret;
}

TInt CIkeV1PkiService::GetNextCertificateL()
{
    DEBUG_LOG(_L("-> CIkeV1PkiService::GetNextCertificateL"));
    //
    // Get next user certificate from PKI store using either Key
    // identifier or CA name as read argument
    //
    TInt Status = KErrNotFound;
    if ( iCasTrustedByPeer.Count() > 0 )
    {
        CIkeCaElem* CaElem = iCasTrustedByPeer[0];
        iCasTrustedByPeer.Remove(0);
        iOperation = KReadingCertificate;
        
        HBufC8* CaName = IkeCert::GetCertificateFieldDERL(
            CaElem->Certificate(), KSubjectName
        );
        
        if ( CaName )
        {
            delete iCaName;
            iCaName = CaName;
            ReadCertificateL(*iCaName, EFalse);
            Status = KErrNone;
        }

    }

    DEBUG_LOG(_L("<- CIkeV1PkiService::GetNextCertificateL"));
    return Status;
}


TBool CIkeV1PkiService::CertificateReadL(TInt& aStatus)
{
    //
    // A Certificate has been read PKI store.
    // Build X509 certificate object from certificate data
    //
#ifdef _DEBUG
    CertReadCompleted(EFalse, aStatus, __LINE__);
#endif // _DEBUG    
    TBool Status = ETrue;
    
    if ( aStatus == KErrNone )
    {
        iReallocated = EFalse;
        iReadCertificate->Des().SetLength(iCertPtr.Length());
    }
    else
    {
        if ( (aStatus == KPKIErrBufferTooShort) && !iReallocated )
        {
            //
            // Certificate buffer was too small try to read once more if
            // not already tried
            //
            Status  = EFalse;
            aStatus = KErrNone;
            iReallocated = ETrue;
            ReadCertificateL(*iCaName, EFalse);
        }

        if ( (aStatus != KErrNone) && ( aStatus != KPKIErrBufferTooShort) )
        {
            //
            // User certificate not found from PKI store, try to read next
            //
            iReallocated = EFalse;
            aStatus = GetNextCertificateL();
            
            if ( aStatus == KErrNone )
            {
                Status = EFalse;
            }
        }
    }

    return Status;
}


TInt CIkeV1PkiService::ReadCertificateL(const TPKIKeyIdentifier& aKeyIdentifier)
{
    //
    // Read certificate from PKI store using pkiserviceapi
    //    
    TRequestStatus status;

    for (;;)    // Only for easy exits...
    {
        if ( iReallocated )
        {
            //
            // Allocate a new buffer for ASN1 coded certificate read from
            // PKI store. Buffer size is now asked from pkiserviceapi
            //
            delete iReadCertificate;
            iReadCertificate = NULL;
            TInt RealCertSize;
            
            if ( iPkiService.GetRequiredBufferSize(RealCertSize) == KErrNone )
                iCertBfrSize = (RealCertSize | 0x3) + 1;
            // Try double size in error case
            else iCertBfrSize = (iCertBfrSize << 1);
        }

        if ( !iReadCertificate )
            iReadCertificate = HBufC8::NewL(iCertBfrSize);
            
        iCertPtr.Set((TUint8*)iReadCertificate->Ptr(), 0, iCertBfrSize);

        iPkiService.ReadCertificateL(aKeyIdentifier, iCertPtr,
                                     &iResArray, status);
                    
        User::WaitForRequest(status);
        iPkiService.Finalize(iResArray);
        iResArray = NULL;
  
        if ( (status.Int() == KPKIErrBufferTooShort) && !iReallocated )
        {
            //
            // Certificate buffer was too small try to read once more if
            // not already tried
            //
            iReallocated = ETrue;
        }
        else
        {
            if ( status.Int() == KErrNone )
            {
                iReadCertificate->Des().SetLength(iCertPtr.Length());
                iReallocated = EFalse;
            }
            break;
        }
        
    }

    return status.Int();
}


TInt CIkeV1PkiService::ReadCertificateListL()
{
    //
    // Read certificate list with Application UID:s
    //
    if ( iCaCertList )
    {
        iCaCertList->Reset();
        delete iCaCertList;
        iCaCertList = NULL;
    }
    
    iOperation  = KBuildingApplUidList;

    iPkiService.ListApplicableCertificatesL(
        (const RArray<TUid>&)(*iApplUidList), iCaCertList
    );

    return KErrNone;
}


TInt CIkeV1PkiService::GetCertificateWithKeyIdL(const TDesC16& aKeyIdString)
{
    TInt Status;
    
    if ( IkeParser::TextToHexOctets(aKeyIdString, iCertKeyId) )
        Status = ReadCertificateL(iCertKeyId);
    else Status = KErrArgument;

    return Status;
}


TBool CIkeV1PkiService::GetApplUidListL(const TDesC16& aApplUidString)
{
    //
    // Build application UID array to get trusted CA certificate list
    // from PKI service.
    //
    if ( iApplUidList )
    {
        iApplUidList->Reset();
        delete iApplUidList;
        iApplUidList = NULL;
    }

    iApplUidList = IkeParser::GetApplUidListL(aApplUidString);

    TBool Status = (iApplUidList->Count() != 0);
    
    if ( Status )
    {
        TInt Ret = ReadCertificateListL();
        Status = ( Ret == KErrNone);
        iListIndex = 0;
        
        Status = ApplUidCertListCompletedL(Ret);
    }

    return Status;
}


TBool CIkeV1PkiService::ApplUidCertListCompletedL(TInt aStatus)
{
    DEBUG_LOG2(
        _L("Certificate list read completed, status= %d, list elem count= %d"),
        aStatus, iCaCertList->Count()
    );
    DEBUG_LOG1(
        _L(" APPL UID(s) = %S\n"), &iCaNameList->At(iCurrIndex)->iData
    );
  
    TBool Ret;
  
    if ( (aStatus == KErrNone) && iCaCertList->Count() )
    {
        //
        // Start to read in trusted CA certificates provided in list
        //
        iOperation = KProcessingApplUidList;
        Ret = ReadNextInListL();
    }
    else
    {
        //
        // No trusted CA certificates found with current application
        // UID:s. Continue processing CA elements
        //
        Ret = EFalse;
        iOperation = KBuildingCaList;
        iCurrIndex ++;
        
    }

    return Ret;
}


TBool CIkeV1PkiService::ReadNextInListL()
{
    TBool Status = EFalse;
    
    TInt Ret;
    
    while ( iListIndex < iCaCertList->Count() )
    {
        Ret = ReadCertificateL(iCaCertList->At(iListIndex).iSubjectKeyId);
        if ( AddNextCaElemL(Ret) )
            Status = ETrue;
    }
    
    iCurrIndex ++;

    return Status;
}


#ifdef _DEBUG

void CIkeV1PkiService::CertReadCompleted(TBool aCaCert, TInt aStatus, TInt aLine )
{
  TBuf<320>DebugMsg;
  if ( aCaCert )
  {
        ASSERT( iCurrIndex < iCaNameList->Count() );
     DebugMsg.Format(_L("Trusted CA certificate read completed with status = %d (line = %d)"),
                     aStatus, aLine);
     DebugMsg.AppendFormat(_L(" ; Search criteria: "));
     TCertInfo* CertInfo = iCaNameList->At(iCurrIndex);
     switch ( CertInfo->iFormat )
     {
       case CA_NAME:
         DebugMsg.AppendFormat(_L("CA_NAME = %S\n"), &CertInfo->iData);
         break;
       case KEY_ID:
         DebugMsg.AppendFormat(_L("KEY_ID = %S\n"), &CertInfo->iData);
         break;
       default:
         TBuf<48> KeyIdString;
                ASSERT( iListIndex < iCaCertList->Count() );
         HexToString(iCaCertList->At(iListIndex).iSubjectKeyId, KeyIdString);
         DebugMsg.AppendFormat(_L("APPL_UID/<KEY_ID> = %S\n"), &KeyIdString);
         break;
     }
  }
  else
  {
     DEBUG_LOG2(_L("End user certificate read completed with status = %d (line = %d)\n"),
                     aStatus, aLine);
  }
    DEBUG_LOG(DebugMsg);
}

void CIkeV1PkiService::HexToString(const TDesC8& aKeyId, TDes16& aKeyIdString)
{
  TInt i = 0;
  TUint x;
  TUint y;

  while (i < aKeyId.Length())
  {
    x = (TUint)aKeyId[i];
    for ( TInt j = 4; j >= 0; j -= 4 )
    {
          y  = (x >> j) & 0xf;
        TChar ch(y);
            if ( y < 0xa )
           ch += 0x30;
        else if ( (y > 9) && (y < 0x10) )
             ch += (0x61 - 0xa);
        else ch += (0x30 - ch);
        aKeyIdString.Append(ch);
    }
    i ++;
  }
}

#endif //_DEBUG
