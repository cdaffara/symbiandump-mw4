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
* Description: Implementation of CDmAdCertReq.
*
*/


#include "dmadstorecertreq.h"
#include "vpnlogger.h"
#include "base64.h"
#include "dmadutil.h"

CDmAdCertReq* CDmAdCertReq::NewL(RPKIServiceAPI& aPkiServiceApi, MDmAdCallBack* aDmAdCallBack)
    {
    TRACE("CDmAdCertReq::NewL");
    
    CDmAdCertReq* self = NewLC(aPkiServiceApi, aDmAdCallBack);
    CleanupStack::Pop(self); 
    return self;
    }

CDmAdCertReq* CDmAdCertReq::NewLC(RPKIServiceAPI& aPkiServiceApi, MDmAdCallBack* aDmAdCallBack)
    {
    CDmAdCertReq* self = new (ELeave) CDmAdCertReq(aPkiServiceApi, aDmAdCallBack);
    CleanupStack::PushL(self);
    return self;
    }

CDmAdCertReq::CDmAdCertReq(RPKIServiceAPI& aPkiServiceApi, MDmAdCallBack* aDmAdCallBack) : 
    iPkiServiceApi(&aPkiServiceApi),
    iDmAdCallBack(aDmAdCallBack)
    {
    TRACE("CDmAdCertReq::CDmAdCertReq");
    }
    
CDmAdCertReq::~CDmAdCertReq()
    {
    TRACE("CDmAdCertReq::~CDmAdCertReq");
    }
    
TBool CDmAdCertReq::FindL(const TDesC8& aLuid)
    {
    TRACE("CDmAdCertReq::FindL");
    
    TBool found = EFalse;
    HBufC* certReqRef = LuidToCertReqRefL(aLuid);
    CleanupStack::PushL(certReqRef);
    
    const TInt KDmAdMaxCertReqSize = 4096; // ????????
    HBufC8* certRequest = HBufC8::NewLC(KDmAdMaxCertReqSize);
    TPtr8 certRequestPtr(certRequest->Des());
    

    TInt status = iPkiServiceApi->ReadCertificateRequest(*certReqRef,
                                                         certRequestPtr);
    if (status == KErrNone)
        {
        found = ETrue;
        }
    CleanupStack::PopAndDestroy(2); //certRequest, certReqRef
    return found;
    }
    
    
HBufC8* CDmAdCertReq::AddL(const CDmAdCertReqParms& aParms)
    {
    TRACE("CDmAdCertReq::AddL");
    
    const TInt KDmAdMaxCertReqRefLth = KMaxFileName;
    HBufC* certReqRef = HBufC::NewLC(KDmAdMaxCertReqRefLth);
    TPtr certReqRefPtr(certReqRef->Des());

    TPKIKeyIdentifier keyIdentifier(aParms.KeyIdentifierByUri());
    if (keyIdentifier.Length() == 0)
        {        
        CreateKeypairL(EPKIRSA,
                       aParms.KeyLength(),
                       keyIdentifier);        
        }
        
    DEBUG_LOG(_L("Key ID:"));            
    DEBUG_LOG_HEX(keyIdentifier);
    
    SetKeyIDMappingL(keyIdentifier);
    
    TInt certReqSize;
    iPkiServiceApi->CreateAndSaveCertificateRequestL(keyIdentifier,
                                                     aParms.SubjectName(), //aDistinguishedName,
                                                     aParms.Rfc822Name(), //aSubjectAltNameRfc822,       
                                                     KNullDesC8, //aChallengePassword,
                                                     KNullDesC8, //aDnsName,
                                                     certReqRefPtr,
                                                     certReqSize);

    HBufC8* luid = CertReqRefToLuidL(*certReqRef);
    CleanupStack::PopAndDestroy(certReqRef); 
    return luid;
    }

void CDmAdCertReq::SetKeyIDMappingL(const TPKIKeyIdentifier& keyIdentifier)
    {
    TRACE("CDmAdCertReq::SetKeyIDMapping");

    // First fetch all private key URIs
    CBufBase* uriList = CBufFlat::NewL(16);
    CleanupStack::PushL(uriList);
    _LIT8(KKeyUri, "NokiaPKI/PrivKey");
    MSmlDmAdapter::TError status;
    iDmAdCallBack->FetchLinkL(KKeyUri, *uriList, status);    
    User::LeaveIfError(status);
    
    // Find out key URI (cli<X>) for the newly created private key
    CArrayFix<TPtrC8>* uriSegList;
    TDmAdUtil::ParseUriLC(uriList->Ptr(0), uriSegList);
    // New key URI (cli<X>) is now in uriSegList->At(0)
    HBufC8* wholeUri = TDmAdUtil::BuildUriLC(KKeyUri, uriSegList->At(0));
    
    iDmAdCallBack->SetMappingL(*wholeUri, keyIdentifier);
    
    CleanupStack::PopAndDestroy(wholeUri);
    CleanupStack::PopAndDestroy(uriSegList);
    CleanupStack::PopAndDestroy(uriList);
    }

void CDmAdCertReq::UpdateL(const TDesC8& /*aLuid*/, const CDmAdCertReqParms& /*aParms*/)
    {
    TRACE("CDmAdCertReq::UpdateL");    
    DEBUG_LOG(_L("Method not implemented"));
    
    User::Leave(KErrGeneral);
    }
    
void CDmAdCertReq::FetchL(const TDesC8& aLuid, CDmAdCertReqParms& aParms)
    {
    TRACE("CDmAdCertReq::FetchL");
    
    HBufC* certReqRef = LuidToCertReqRefL(aLuid);
    CleanupStack::PushL(certReqRef);
    
    const TInt KDmAdMaxCertReqSize = 4096; // ????????
    HBufC8* certRequest = HBufC8::NewLC(KDmAdMaxCertReqSize);
    TPtr8 certRequestPtr(certRequest->Des());
    
    User::LeaveIfError(iPkiServiceApi->ReadCertificateRequest(*certReqRef,
                                                              certRequestPtr));

    TBase64Codec base64Codec;
    HBufC8* certRequestB64Decoded;
    if ((certRequestB64Decoded = base64Codec.Base64DecodeLC(*certRequest)) == NULL)
        {
        DEBUG_LOG(_L("base64Codec.Base64DecodeLC returned NULL"));        
        User::Leave(KErrNoMemory);
        }

    aParms.SetContentL(*certRequestB64Decoded);
    //ReadCertReqDetailsL(*certReqRef, aParms); // ???????????
    CleanupStack::PopAndDestroy(3); // certRequestB64Decoded, certRequest, certReqRef
    }
    
    
void CDmAdCertReq::DeleteL(const TDesC8& aLuid)
    {
    TRACE("CDmAdCertReq::DeleteL");
    
    HBufC* certReqRef = LuidToCertReqRefL(aLuid);
    CleanupStack::PushL(certReqRef);
   
    User::LeaveIfError(iPkiServiceApi->DeleteCertificateRequest(*certReqRef));

    CleanupStack::PopAndDestroy(certReqRef);
    }


void CDmAdCertReq::ListL(RPointerArray<HBufC8>& aLuidList)
    {
    TRACE("CDmAdCertReq::ListL");
    
    CArrayFix<TCertificateRequestListEntry>* certReqList;
    iPkiServiceApi->ListCertificateRequestsL(certReqList);
    CleanupStack::PushL(certReqList);

    for (TInt i=0; i<certReqList->Count(); ++i)
        {
        TCertificateRequestListEntry& entry = certReqList->At(i);
        HBufC8* luid = CertReqRefToLuidL(entry.iObjectName);
        CleanupStack::PushL(luid);
        aLuidList.AppendL(luid);        
        CleanupStack::Pop(luid);
        }
    
    CleanupStack::PopAndDestroy(certReqList); 
    }

void CDmAdCertReq::CreateKeypairL(TPKIKeyAlgorithm      aKeyAlgorithm,
                                  TInt                  aKeyLength,
                                  TPKIKeyIdentifier&    aKeyIdentifier)
    {
    TRACE("CDmAdCertReq::CreateKeypairL");
    
    TRequestStatus requestStatus;
    iPkiServiceApi->GenerateKeypair(aKeyIdentifier,
                                    (TUint)aKeyLength,
                                    aKeyAlgorithm,
                                    requestStatus);
    User::WaitForRequest(requestStatus);
    TInt status = requestStatus.Int();
    if (status != KErrNone)
        {
        DEBUG_LOG1(_L("GenerateKeypairL returned %d"), status);        
        User::Leave(status);
        }
    }

HBufC8* CDmAdCertReq::CertReqRefToLuidL(const TDesC& aCertReqRef)
    {
    TRACE("CDmAdCertReq::CertReqRefToLuidL");
    
    HBufC8* luid = HBufC8::NewL(aCertReqRef.Length() * 2);
    luid->Des().Copy((const TUint8*)aCertReqRef.Ptr(), aCertReqRef.Length() * 2);
    return luid;
    }
    
HBufC* CDmAdCertReq::LuidToCertReqRefL(const TDesC8& aLuid)
    {
    TRACE("CDmAdCertReq::LuidToCertReqRefL");
    
    HBufC* certReqRef = HBufC::NewL(aLuid.Length() / 2);
    certReqRef->Des().Copy((const TUint16*)aLuid.Ptr(), aLuid.Length() / 2);
    return certReqRef;
    }
