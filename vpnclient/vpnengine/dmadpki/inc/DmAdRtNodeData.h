/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class definition of CDmAdCertData
*
*/



#ifndef __DMADRTNODEDATA_H__
#define __DMADRTNODEDATA_H__


#include <e32base.h>
#include <e32cmn.h>

#include "dmadcallback.h"
#include "dmadcertparms.h"
#include "dmadcertreqparms.h"
#include "dmadprivkeyparms.h"
#include "dmadpkcs12parms.h"
#include "eventlogger.h"

class CX509Certificate;

typedef enum
    {
    EDmAdUriTypeCert,
    EDmAdUriTypeCertReq,
    EDmAdUriTypePrivKey,
    EDmAdUriTypePKCS12
    } TDmAdUriType;
    
//------------------------------------------------------------------------------------------------
// CDmAdCertData
//------------------------------------------------------------------------------------------------
class CDmAdCertData : public CBase
    {
public:
    static CDmAdCertData* NewL(MDmAdCallBack* aCallBack);
    ~CDmAdCertData();
    
    void SetDefaultSettings();
    
    void SetTypeL(const TDesC8& aType);
    HBufC8* TypeLC();
    void SetFormatL(const TDesC8& aFormat);
    HBufC8* FormatLC();
    void SetDeletableL(const TDesC8& aDeletable);
    HBufC8* DeletableLC();
    void SetTrustedL(const TDesC8& aTrusted);
    HBufC8* TrustedLC();
    void SetApplicabilityL(const RArray<TUid>& aApplicability);
    const RArray<TUid>& Applicability() const;
    void SetContentL(const TDesC8& aContent);
    TPtrC8 Content();
    HBufC8* SerialNumberLC();
    HBufC8* IssuerNameLC();
    HBufC8* FingerprintAlgLC();
    HBufC8* FingerprintValueLC();
    HBufC8* ValidityBeginLC();
    HBufC8* ValidityEndLC();
    HBufC8* SubjectNameLC();
    HBufC8* SubjectAltNameLC();
    HBufC8* KeyUriLC();
    HBufC8* KeyIdLC();
    HBufC8* KeyUsageLC();
    HBufC8* FormatCertValidityTimeLC(const TTime& aTime);
    HBufC8* FindKeyUriLC(const TDesC8& aKeyId);
    void RestoreL(const CDmAdCertParms& aCertParms);
    void ToStoreL(CDmAdCertParms& aCertParms);

private:
    void ConstructL();
    CDmAdCertData(MDmAdCallBack* aCallBack);
        
private:
    MDmAdCallBack*      iCallBack;
    
    CDmAdCertParms*     iCertParms;
    CX509Certificate*   iCertCx509;
    };

//------------------------------------------------------------------------------------------------
// CDmAdCertReqData
//------------------------------------------------------------------------------------------------
class CDmAdCertReqData : public CBase
    {
public:
    static CDmAdCertReqData* NewL(MDmAdCallBack* aCallBack);
    ~CDmAdCertReqData();
    
    void SetDefaultSettings();
    void SetSubjectNameL(const TDesC8& aSubjectName);
    TPtrC8 SubjectName();
    void SetRfc822NameL(const TDesC8& aRfc822Name);
    TPtrC8 Rfc822Name();
    void SetKeyUriL(const TDesC8& aKeyUri);
    HBufC8* KeyUriL() const;
    void SetKeyLength(const TDesC8& aKeyLength);
    HBufC8* KeyLengthLC();

    TPtrC8 Content();
    
    void RestoreL(const CDmAdCertReqParms& aCertReqParms);
    void ToStoreL(CDmAdCertReqParms& aCertReqParms);

private:
    void ConstructL();
    CDmAdCertReqData(MDmAdCallBack* aCallBack);
    
private:

    MDmAdCallBack*          iCallBack;   
    CDmAdCertReqParms*      iCertReqParms;
    };

//------------------------------------------------------------------------------------------------
// CDmAdPrivKeyData
//------------------------------------------------------------------------------------------------
class CDmAdPrivKeyData : public CBase
    {
public:
    static CDmAdPrivKeyData* NewL();
    ~CDmAdPrivKeyData();
    
    void SetDefaultSettingsL();
    void SetKeyTypeL(const TDesC8& aKeyType);
    HBufC8* KeyTypeLC();
    void SetKeyLength(const TDesC8& aKeyLength);
    HBufC8* KeyLengthLC();
    TPtrC8 KeyId();
    
    void RestoreL(const CDmAdPrivKeyParms& aPrivKeyParms);
    void ToStoreL(CDmAdPrivKeyParms& aPrivKeyParms);

private:
    void ConstructL();
    CDmAdPrivKeyData();
    
private:
    CDmAdPrivKeyParms*     iPrivKeyParms;
    };
    
//------------------------------------------------------------------------------------------------
// CDmAdPKCS12Data
//------------------------------------------------------------------------------------------------
class CDmAdPKCS12Data : public CBase
    {
public:
    static CDmAdPKCS12Data* NewL(/*MDmAdCallBack* aCallBack*/);
    ~CDmAdPKCS12Data();
    
    void SetDefaultSettings();
    
    void SetDeletableL(const TDesC8& aDeletable);
    HBufC8* DeletableLC();
    void SetApplicabilityL(const RArray<TUid>& aApplicability);
    const RArray<TUid>& Applicability() const;
    void SetContentL(const TDesC8& aContent);
    TPtrC8 Content();
    void SetPasswordL(const TDesC8& aContent);
    TPtrC8 Password();

    void RestoreL(const CDmAdPKCS12Parms& aPKCS12Parms);
    void ToStoreL(CDmAdPKCS12Parms& aPKCS12Parms);

private:
    void ConstructL();
    CDmAdPKCS12Data();
    
private:
    CDmAdPKCS12Parms*     iPKCS12Parms;
    };

//------------------------------------------------------------------------------------------------
// CDmAdRtNodeData
//------------------------------------------------------------------------------------------------
class CDmAdRtNodeData : public CBase
    {
public:
    static CDmAdRtNodeData* NewL(const TDesC8& aUri, MDmAdCallBack* aCallBack);
    ~CDmAdRtNodeData();
    TDmAdUriType UriType() const;
    static TDmAdUriType UriTypeL(const TDesC8& aUri);
    void UpdateLeafDataL(const TDesC8& aUri, const TDesC8& aObject);
    void FetchLeafObjectLC(const TDesC8& aUri, const TDesC8& aLuid, CBufBase*& aObject);
    void SetDefaultSettingsL(const TDesC8& aUri);

    inline CDmAdCertData* CertData() const;
    inline CDmAdCertReqData* CertReqData() const;
    inline CDmAdPrivKeyData* PrivKeyData() const;
    inline CDmAdPKCS12Data* PKCS12Data() const;
    
private:
    void ConstructL(const TDesC8& aUri, MDmAdCallBack* aCallBack);
    CDmAdRtNodeData();
    void UpdateLeafDataCertL(const TDesC8& aUri, const TDesC8& aObject);
    void UpdateLeafDataCertReqL(const TDesC8& aUri, const TDesC8& aObject);
    void UpdateLeafDataPrivKeyL(const TDesC8& aUri, const TDesC8& aObject);
    void FetchLeafObjectCertLC(const TDesC8& aUri, const TDesC8& aLuid, CBufBase*& aObject);
    void FetchLeafObjectCertReqLC(const TDesC8& aUri, const TDesC8& aLuid, CBufBase*& aObject);
    void FetchLeafObjectPrivKeyLC(const TDesC8& aUri, const TDesC8& aLuid, CBufBase*& aObject);
        
    void UpdateLeafDataPKCS12L(const TDesC8& aUri, const TDesC8& aObject);
    void ParseApplicabilityL(const TDesC8& aApplicabilityData,
                             RArray<TUid>& aApplicabilityList) const; 
        
        
private:
    TDmAdUriType    iUriType;
    union
        {
        CDmAdCertData*      iCert;
        CDmAdCertReqData*   iCertReq;
        CDmAdPrivKeyData*   iPrivKey;
        CDmAdPKCS12Data*    iPKCS12;
        } u;
    DEFINE_EVENT_LOGGER    
    };

#include "DmAdRtNodeData.inl"

#endif
