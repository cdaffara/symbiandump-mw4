/*
* Copyright (c) 2000-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of CDmAdRtNodeData
*
*/



#include <utf.h>

#include <x509cert.h>
#include <x509keys.h>
#include <x500dn.h>
#include <x509certext.h>

#include "DmAdRtNodeData.h"
#include "dmadddf.h"
#include "dmadutil.h"
#include "vpnlogger.h"
#include "dmadstoreprivkey.h"
#include "pkidefs.h"
#include "XwImpl.h"
#include "dmadcertxmldefs.h"
#include "XppImpl.h"
#include <vpnlogmessages.rsg>


// Macro to encode key usage bits
// See method CDmAdCertData::KeyUsageLC
#define ENC_BIT(a)                         \
    if( EFalse == keyUsageExt->IsSet(a) )  \
        {                                  \
        keyUsagePtr.Append('0');           \
        }                                  \
    else                                   \
        {                                  \
        keyUsagePtr.Append('1');           \
        }


CDmAdRtNodeData* CDmAdRtNodeData::NewL(const TDesC8& aUri, MDmAdCallBack* aCallBack)
    {
    TRACE("CDmAdRtNodeData::NewL");
    
    CDmAdRtNodeData *self = new (ELeave) CDmAdRtNodeData();
    CleanupStack::PushL(self);
    self->ConstructL(aUri, aCallBack);
    CleanupStack::Pop(self);
    return self;
    }

void CDmAdRtNodeData::ConstructL(const TDesC8& aUri, MDmAdCallBack* aCallBack)
    {
    TRACE("CDmAdRtNodeData::ConstructL");
    
    iUriType = UriTypeL(aUri);
    
    switch (iUriType)
        {
        case EDmAdUriTypeCert:
            u.iCert = CDmAdCertData::NewL(aCallBack);
            break;
        case EDmAdUriTypeCertReq:
            u.iCertReq = CDmAdCertReqData::NewL(aCallBack);
            break;
        case EDmAdUriTypePrivKey:
            u.iPrivKey = CDmAdPrivKeyData::NewL();
            break;
        case EDmAdUriTypePKCS12:
            u.iPKCS12 = CDmAdPKCS12Data::NewL();
            break;
        default:
            DEBUG_LOG(_L("Unknown URI type"));            
            User::Leave(KErrGeneral);
            break;
        }    
    }
    
CDmAdRtNodeData::CDmAdRtNodeData()
    {
    }

CDmAdRtNodeData::~CDmAdRtNodeData()
    {
    TRACE("CDmAdRtNodeData::~CDmAdRtNodeData");
    
    switch (iUriType)
        {
        case EDmAdUriTypeCert:
            delete u.iCert;
            break;
        case EDmAdUriTypeCertReq:
            delete u.iCertReq;
            break;
        case EDmAdUriTypePrivKey:
            delete u.iPrivKey;
            break;
        case EDmAdUriTypePKCS12:
            delete u.iPKCS12;
            break;
        default:
            DEBUG_LOG(_L("Unknown URI type"));
            break;
        }   
    }
    
//--------------------------------------------------------------------------

TDmAdUriType CDmAdRtNodeData::UriType() const
    {
    TRACE("CDmAdRtNodeData::UriType");
    
    return iUriType;
    }
    
TDmAdUriType CDmAdRtNodeData::UriTypeL(const TDesC8& aUri)
    {
    TRACE("CDmAdRtNodeData::UriTypeL");
    
    TDmAdUriType ret = EDmAdUriTypeCert;
    CArrayFix<TPtrC8>* uriSegList;
    TDmAdUtil::ParseUriLC(aUri, uriSegList);
    
    if (uriSegList->Count() < 2)
        {
        DEBUG_LOG(_L("uriSegList->Count() is less than 2"));        
        User::Leave(KErrGeneral);
        }

    if (uriSegList->At(0).Compare(KDmAdNodeNokiaPki) == 0)
        {
        if (uriSegList->At(1).Compare(KDmAdNodeCert) == 0)
            {
            ret = EDmAdUriTypeCert;
            }
        else if (uriSegList->At(1).Compare(KDmAdNodeCertReq) == 0)
            {
            ret = EDmAdUriTypeCertReq;
            }
        else if (uriSegList->At(1).Compare(KDmAdNodePrivKey) == 0)
            {
            ret = EDmAdUriTypePrivKey;
            }
        else if (uriSegList->At(1).Compare(KDmAdNodePKCS12) == 0)
            {
            ret = EDmAdUriTypePKCS12;
            }
        else
            {
            DEBUG_LOG(_L("Unknown URI type"));
            User::Leave(KErrGeneral);
            }
        }
    else
        {
        DEBUG_LOG(_L("Unknown URI type"));
        User::Leave(KErrGeneral);
        }

    CleanupStack::PopAndDestroy(uriSegList);
    return ret;
    }
    
//--------------------------------------------------------------------------
    
void CDmAdRtNodeData::UpdateLeafDataL(const TDesC8& aUri, const TDesC8& aObject)
    {
    TRACE("CDmAdRtNodeData::UpdateLeafDataL");
    
    switch (iUriType)
        {
        case EDmAdUriTypeCert:
            TRAPD(err, UpdateLeafDataCertL(aUri, aObject));
            if(err != KErrNone)
                User::Leave(err);
            break;
        case EDmAdUriTypeCertReq:
            UpdateLeafDataCertReqL(aUri, aObject);
            break;
        case EDmAdUriTypePrivKey:
            UpdateLeafDataPrivKeyL(aUri, aObject);
            break;
        case EDmAdUriTypePKCS12:
            UpdateLeafDataPKCS12L(aUri, aObject);
            break;
        default:
            DEBUG_LOG(_L("Unknown URI type"));
            User::Leave(KErrGeneral);
            break;
        }
    }

void CDmAdRtNodeData::UpdateLeafDataCertL(const TDesC8& aUri, const TDesC8& aObject)
    {
    TRACE("CDmAdRtNodeData::UpdateLeafDataCertL");
    
    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdLeafContent) == 0)
        {
        u.iCert->SetContentL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafType) == 0)
        {
        u.iCert->SetTypeL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafFormat) == 0)
        {
        u.iCert->SetFormatL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafDeletable) == 0)
        {
        u.iCert->SetDeletableL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafTrusted) == 0)
        {
        u.iCert->SetTrustedL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafApplicability) == 0)
        {
        RArray<TUid> applicability;
        CleanupClosePushL(applicability);
        
        ParseApplicabilityL(aObject, applicability);                                                    
        u.iCert->SetApplicabilityL(applicability);
        
        CleanupStack::PopAndDestroy(); //applicability
        }
    else
        {
        DEBUG_LOG(_L("No match found"));        
        User::Leave(KErrNotFound);
        }
    }

void CDmAdRtNodeData::UpdateLeafDataCertReqL(const TDesC8& aUri, const TDesC8& aObject)
    {
    TRACE("CDmAdRtNodeData::UpdateLeafDataCertReqL");
    
    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdLeafSubjectName) == 0)
        {
        u.iCertReq->SetSubjectNameL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafRfc822Name) == 0)
        {
        u.iCertReq->SetRfc822NameL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafKeyURI) == 0)
        {
        u.iCertReq->SetKeyUriL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafKeyLength) == 0)
        {
        u.iCertReq->SetKeyLength(aObject);
        }
    else
        {
        DEBUG_LOG(_L("No match found"));        
        User::Leave(KErrNotFound);
        }
    }

void CDmAdRtNodeData::UpdateLeafDataPrivKeyL(const TDesC8& aUri, const TDesC8& aObject)
    {
    TRACE("CDmAdRtNodeData::UpdateLeafDataPrivKeyL");
    
    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdLeafKeyType) == 0)
        {
        u.iPrivKey->SetKeyTypeL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafKeyLength) == 0)
        {
        u.iPrivKey->SetKeyLength(aObject);
        }
    else
        {        
        DEBUG_LOG(_L("No match found"));        
        User::Leave(KErrNotFound);
        }
    }

void CDmAdRtNodeData::UpdateLeafDataPKCS12L(const TDesC8& aUri, const TDesC8& aObject)
    {
    TRACE("CDmAdRtNodeData::UpdateLeafDataPKCS12L");
    
    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdLeafContent) == 0)
        {
        u.iPKCS12->SetContentL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafDeletable) == 0)
        {
        u.iPKCS12->SetDeletableL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafPKCS12Password) == 0)
        {
        u.iPKCS12->SetPasswordL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafApplicability) == 0)
        {
        RArray<TUid> applicability;
        CleanupClosePushL(applicability);
        
        ParseApplicabilityL(aObject, applicability);                                                    
        u.iPKCS12->SetApplicabilityL(applicability);
        
        CleanupStack::PopAndDestroy(); //applicability
        }
    else
        {
        DEBUG_LOG(_L("No match found"));        
        User::Leave(KErrNotFound);
        }

    }

void CDmAdRtNodeData::FetchLeafObjectLC(const TDesC8& aUri, const TDesC8& aLuid, CBufBase*& aObject)
    {   
    switch (iUriType)
        {
        case EDmAdUriTypeCert:
            FetchLeafObjectCertLC(aUri, aLuid, aObject);
            break;
        case EDmAdUriTypeCertReq:
            FetchLeafObjectCertReqLC(aUri, aLuid, aObject);
            break;
        case EDmAdUriTypePrivKey:
            FetchLeafObjectPrivKeyLC(aUri, aLuid, aObject);
            break;
        case EDmAdUriTypePKCS12:
            // PKCS#12 special case
            break;
        default:
            DEBUG_LOG(_L("Unknown URI type"));
            User::Leave(KErrGeneral);
            break;
        }
    }
    
void CDmAdRtNodeData::FetchLeafObjectCertLC(const TDesC8& aUri, const TDesC8& /*aLuid*/, CBufBase*& aObject)
    {    
    CBufBase* object = CBufFlat::NewL(32);
    CleanupStack::PushL(object);

    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdLeafType) == 0)
        {
        HBufC8* obj = u.iCert->TypeLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(obj);
        }
    else if (lastSeg.Compare(KDmAdLeafFormat) == 0)
        {
        HBufC8* obj = u.iCert->FormatLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(obj);
        }
    else if (lastSeg.Compare(KDmAdLeafDeletable) == 0)
        {
        HBufC8* obj = u.iCert->DeletableLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(obj);
        }
    else if (lastSeg.Compare(KDmAdLeafTrusted) == 0)
        {
        HBufC8* obj = u.iCert->TrustedLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(obj);
        }
    else if (lastSeg.Compare(KDmAdLeafApplicability) == 0)
        {
        
        DEBUG_LOG(_L("Fetching applicability"));
        const RArray<TUid>& applicability = u.iCert->Applicability();

        CXmlWriter* writer = CXmlWriter::NewL();
        CleanupStack::PushL(static_cast<CBase*>(writer));
        writer->AddStartTagL(KXmlElemCertApps);
        
        DEBUG_LOG(_L("KXmlElemCertApps Start tag added"));
        
        for (TInt i = 0; i < applicability.Count(); ++i)
            {
            HBufC8* idInt = TDmAdUtil::IntToDes8LC((TInt)applicability[i].iUid);
            writer->OpenStartTagL(KXmlElemApp);
            writer->AddAttributeL(KXmlAttrId, *idInt);
            writer->CloseStartTagL(ETrue);
            CleanupStack::PopAndDestroy(idInt); 
            }
        writer->AddEndTagL(KXmlElemCertApps);
        
        DEBUG_LOG(_L("KXmlElemCertApps end tag added"));
        
        TPtrC8 docDataPtr(writer->DocPart(0, writer->Length()-1));                
        object->InsertL(0, docDataPtr);
        
        DEBUG_LOG(_L("document inserted"));
        
        CleanupStack::PopAndDestroy(static_cast<CBase*>(writer));
        
        DEBUG_LOG(_L("writer popped"));
        }
    else if (lastSeg.Compare(KDmAdLeafContent) == 0)
        {
        TPtrC8 obj(u.iCert->Content());
        object->InsertL(0, obj);
        }
    else if (lastSeg.Compare(KDmAdLeafSerialNumber) == 0)
        {
        HBufC8* obj = u.iCert->SerialNumberLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(obj);
        }
    else if (lastSeg.Compare(KDmAdLeafIssuerName) == 0)
        {
        HBufC8* obj = u.iCert->IssuerNameLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(obj);
        }
    else if (lastSeg.Compare(KDmAdLeafFingerprintAlg) == 0)
        {
        HBufC8* obj = u.iCert->FingerprintAlgLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(obj);
        }
    else if (lastSeg.Compare(KDmAdLeafFingerprintValue) == 0)
        {
        HBufC8* obj = u.iCert->FingerprintValueLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(obj);
        }
    else if (lastSeg.Compare(KDmAdLeafValidityBegin) == 0)
        {
        HBufC8* obj = u.iCert->ValidityBeginLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(obj);
        }
    else if (lastSeg.Compare(KDmAdLeafValidityEnd) == 0)
        {
        HBufC8* obj = u.iCert->ValidityEndLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(obj);
        }
    else if (lastSeg.Compare(KDmAdLeafSubjectName) == 0)
        {
        HBufC8* obj = u.iCert->SubjectNameLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(obj);
        }
    else if (lastSeg.Compare(KDmAdLeafSubjectAltName) == 0)
        {
        HBufC8* obj = u.iCert->SubjectAltNameLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(obj);
        }
    else if (lastSeg.Compare(KDmAdLeafKeyURI) == 0)
        {
        HBufC8* obj = u.iCert->KeyUriLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(obj);
        }
    else if (lastSeg.Compare(KDmAdLeafKeyId) == 0)
        {
        HBufC8* obj = u.iCert->KeyIdLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(obj);
        }
    else if (lastSeg.Compare(KDmAdLeafKeyUsage) == 0)
        {
        HBufC8* obj = u.iCert->KeyUsageLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(obj);
        }
    else
        {
        DEBUG_LOG(_L("No match found"));        
        User::Leave(KErrNotFound);
        }
    
    aObject = object;
    }
    
void CDmAdRtNodeData::FetchLeafObjectCertReqLC(const TDesC8& aUri, const TDesC8& /*aLuid*/, CBufBase*& aObject)
    {
    CBufBase* object = CBufFlat::NewL(32);
    CleanupStack::PushL(object);

    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdLeafSubjectName) == 0)
        {
        TPtrC8 obj(u.iCertReq->SubjectName());
        object->InsertL(0, obj);
        }
    else if (lastSeg.Compare(KDmAdLeafRfc822Name) == 0)
        {
        TPtrC8 obj(u.iCertReq->Rfc822Name());
        object->InsertL(0, obj);
        }
    else if (lastSeg.Compare(KDmAdLeafKeyURI) == 0)
        {
        HBufC8* obj = u.iCertReq->KeyUriL();
        CleanupStack::PushL(obj);
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(obj);
        }
    else if (lastSeg.Compare(KDmAdLeafKeyLength) == 0)
        {
        HBufC8* obj = u.iCertReq->KeyLengthLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(); //obj
        }
    else if (lastSeg.Compare(KDmAdLeafContent) == 0)
        {
        TPtrC8 obj(u.iCertReq->Content());
        object->InsertL(0, obj);
        }
    else
        {
        DEBUG_LOG(_L("No match found"));        
        User::Leave(KErrNotFound);
        }
    
    aObject = object;
    }
    
void CDmAdRtNodeData::FetchLeafObjectPrivKeyLC(const TDesC8& aUri, const TDesC8& /*aLuid*/, CBufBase*& aObject)
    {
    CBufBase* object = CBufFlat::NewL(32);
    CleanupStack::PushL(object);

    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdLeafKeyType) == 0)
        {
        HBufC8* obj = u.iPrivKey->KeyTypeLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(); //obj
        }
    else if (lastSeg.Compare(KDmAdLeafKeyLength) == 0)
        {
        HBufC8* obj = u.iPrivKey->KeyLengthLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(); //obj
        }
    else if (lastSeg.Compare(KDmAdLeafKeyId) == 0)
        {
        TPtrC8 obj(u.iPrivKey->KeyId());
        object->InsertL(0, obj);
        }
    else
        {
        DEBUG_LOG(_L("No match found"));        
        User::Leave(KErrNotFound);
        }
    
    aObject = object;
    }
    
void CDmAdRtNodeData::SetDefaultSettingsL(const TDesC8& /*aUri*/)
    {
    TRACE("CDmAdRtNodeData::SetDefaultSettingsL");
    
    switch (iUriType)
        {
        case EDmAdUriTypeCert:
            u.iCert->SetDefaultSettings();
            break;
        case EDmAdUriTypeCertReq:
            u.iCertReq->SetDefaultSettings();
            break;
        case EDmAdUriTypePrivKey:
            u.iPrivKey->SetDefaultSettingsL();
            break;
        case EDmAdUriTypePKCS12:
            u.iPKCS12->SetDefaultSettings();
            break;
        default:
            DEBUG_LOG(_L("Unknown URI type"));
            User::Leave(KErrGeneral);
            break;
        }
    }


void CDmAdRtNodeData::ParseApplicabilityL(const TDesC8& aApplicabilityData,
                                        RArray<TUid>& aApplicabilityList) const
    {       
    TRACE("CDmAdRtNodeData::ParseApplicabilityL");
    
    aApplicabilityList.Reset();
    
    if (aApplicabilityData.Length() == 0)
        {
        return;
        }
            
    CDesC8ArrayFlat* parserValueArray = new (ELeave) CDesC8ArrayFlat(10);
    CleanupStack::PushL(parserValueArray);
    CXmlPullParser* parser = CXmlPullParser::NewLC(*parserValueArray);
    TPtrC8 name;
    TPtrC8 attribute;
    
    parser->SetInput(aApplicabilityData);
    parser->NextL();
    parser->Name(name);
    
    if (parser->State() != CXmlPullParser::EStateStartTag || name != KXmlElemCertApps)
        {                        
        DEBUG_LOG(_L("Wrong state or name. (Expected EStateStartTag and CertApps"));        
        DEBUG_LOG2(_L8("State = %d, name = %S"), parser->State(), &name);
        User::Leave(KErrCorrupt);
        }
    
    parser->NextL();
    while (parser->State() == CXmlPullParser::EStateText)
        {
        DEBUG_LOG(_L8("Found text. Ignoring it..."));
        parser->NextL();
        }
    
    while (parser->State() == CXmlPullParser::EStateStartTag)
        {
        parser->Name(name);
        DEBUG_LOG1(_L8("Parsed: %S"), &name);
                        
        if (name != KXmlElemApp)
            {
            DEBUG_LOG1(_L8("Invalid name: %S"), &name);            
            User::Leave(KErrCorrupt);
            }

        parser->AttributeValueL(KXmlAttrId, attribute);
        TUid uid = { TDmAdUtil::DesToInt(attribute) };
        DEBUG_LOG1(_L("Uid: 0x%x"), uid.iUid);
        
        aApplicabilityList.Append(uid);
        parser->NextL();
        
        while (parser->State() == CXmlPullParser::EStateText)
            {
            DEBUG_LOG(_L8("Found text. Ignoring it..."));
            parser->NextL();
            }                
        }
    parser->Name(name);
    
    if (parser->State() != CXmlPullParser::EStateEndTag || name != KXmlElemCertApps)
        {        
        DEBUG_LOG(_L("Wrong state or name. (Expected EStateEndTag and CertApps)"));        
        DEBUG_LOG2(_L8("State = %d, name = %S"), parser->State(), &name);
        User::Leave(KErrCorrupt);
        }
    CleanupStack::PopAndDestroy(2); //parser, parserValueArray
    }


//======================================================================================
//======================================================================================
    
CDmAdCertData* CDmAdCertData::NewL(MDmAdCallBack* aCallBack)
    {
    TRACE("CDmAdCertData::NewL");
    
    CDmAdCertData* self = new (ELeave) CDmAdCertData(aCallBack);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CDmAdCertData::ConstructL()
    {
    TRACE("CDmAdCertData::ConstructL");
    
    iCertParms = CDmAdCertParms::NewL();
    }
    
CDmAdCertData::CDmAdCertData(MDmAdCallBack* aCallBack) : iCallBack(aCallBack)
    {    
    }

CDmAdCertData::~CDmAdCertData()
    {
    TRACE("CDmAdCertData::~CDmAdCertData");
    
    delete iCertParms;
    delete iCertCx509;
    }

void CDmAdCertData::SetDefaultSettings()
    {
    TRACE("CDmAdCertData::SetDefaultSettings");
    
    iCertParms->SetDeletable(ETrue);
    iCertParms->SetTrusted(ETrue);
    }

//--------------------------------------------------------------------------

void CDmAdCertData::SetTypeL(const TDesC8& aType)
    {
    TRACE("CDmAdCertData::SetType");
    
    TInt value = TDmAdUtil::DesToInt(aType);
    
    TPKICertificateOwnerType type = EPKICACertificate;        
    switch(value)
        {
        case 1:
            type = EPKICACertificate;
            break;
        case 2:
            type = EPKIUserCertificate;
            break;
        case 3:
            type = EPKIPeerCertificate;
            break;            
        default:
            User::Leave(KErrCorrupt);
            break;
        }
    
    iCertParms->SetType(type);
    }

HBufC8* CDmAdCertData::TypeLC()
    {
    
    TPKICertificateOwnerType type = iCertParms->Type();
    TInt omaDmType = 0;
    switch(type)
        {
        case EPKICACertificate:
            omaDmType = 1;
            break;
        case EPKIUserCertificate:
            omaDmType = 2;
            break;
        case EPKIPeerCertificate:
            omaDmType = 3;
            break;
        default:
            User::Leave(KErrCorrupt);
            break;
        }
    
    return TDmAdUtil::IntToDes8LC(omaDmType);
    }

void CDmAdCertData::SetFormatL(const TDesC8& aFormat)
    {
    TRACE("CDmAdCertData::SetFormat");
    TInt value = TDmAdUtil::DesToInt(aFormat);
    
    if (value != 1)
        {        
        User::Leave(KErrCorrupt);
        }        
    }

HBufC8* CDmAdCertData::FormatLC()
    {
    //The format is always 1, which means X509 cert.    
    return TDmAdUtil::IntToDes8LC(1);
    }

void CDmAdCertData::SetDeletableL(const TDesC8& aDeletable)
    {
    TRACE("CDmAdCertData::SetDeletableL");
    
    if (aDeletable.Compare(KDmAdDfFormatBoolTrue) == 0)
        {
        iCertParms->SetDeletable(ETrue);
        }
    else if (aDeletable.Compare(KDmAdDfFormatBoolFalse) == 0)
        {
        iCertParms->SetDeletable(EFalse);
        }
    else
        {
        DEBUG_LOG(_L("Comparization failed"));                
        User::Leave(KErrGeneral);
        }
    }

HBufC8* CDmAdCertData::DeletableLC()
    {    
    if (iCertParms->Deletable())
        {
        return KDmAdDfFormatBoolTrue().AllocLC();
        }
    else
        {
        return KDmAdDfFormatBoolFalse().AllocLC();
        }
    }

void CDmAdCertData::SetTrustedL(const TDesC8& aTrusted)
    {
    TRACE("CDmAdCertData::SetTrustedL");
    
    if (aTrusted.Compare(KDmAdDfFormatBoolTrue) == 0)
        {
        iCertParms->SetTrusted(ETrue);
        }
    else if (aTrusted.Compare(KDmAdDfFormatBoolFalse) == 0)
        {
        iCertParms->SetTrusted(EFalse);
        }
    else
        {
        DEBUG_LOG(_L("No match found")); 
        User::Leave(KErrGeneral);
        }
    }

HBufC8* CDmAdCertData::TrustedLC()
    {
    if (iCertParms->Trusted())
        {
        return KDmAdDfFormatBoolTrue().AllocLC();
        }
    else
        {
        return KDmAdDfFormatBoolFalse().AllocLC();
        }
    }

void CDmAdCertData::SetApplicabilityL(const RArray<TUid>& aApplicability)
    {
    TRACE("CDmAdCertData::SetApplicabilityL");
    
    iCertParms->SetApplicabilityL(aApplicability);
    }

const RArray<TUid>& CDmAdCertData::Applicability() const
    {
    TRACE("CDmAdCertData::Applicability");
    
    return iCertParms->Applicability();
    }

void CDmAdCertData::SetContentL(const TDesC8& aContent)
    {
    TRACE("CDmAdCertData::SetContentL");
    
    delete iCertCx509;
    iCertCx509 = NULL;
    iCertParms->SetContentL(aContent);
    if (iCertParms->Content().Length() > 0)
        {
        iCertCx509 = CX509Certificate::NewL(iCertParms->Content());
        }
    }

TPtrC8 CDmAdCertData::Content()
    {
    TRACE("CDmAdCertData::Content");
    
    return iCertParms->Content();
    }

//-----------------
    
HBufC8* CDmAdCertData::SerialNumberLC()
    {
    HBufC8* ret = NULL;
    const TPtrC8 serialNumber(iCertCx509->SerialNumber());
    //const TPtrC8 serialNumber(*(iCertCx509->DataElementEncoding(CX509Certificate::ESerialNumber)));
    if (serialNumber.Length() == 0)
        {
        ret = HBufC8::NewLC(1);
        }
    else
        {
        ret = serialNumber.AllocLC();
        }
    return ret;
    }
    
HBufC8* CDmAdCertData::IssuerNameLC()
    {
    HBufC8* ret = NULL;
    const TPtrC8 issuerName(*(iCertCx509->DataElementEncoding(CX509Certificate::EIssuerName)));
    if (issuerName.Length() == 0)
        {
        ret = HBufC8::NewLC(1);
        }
    else
        {
        ret = issuerName.AllocLC();
        }
    return ret;
    }

HBufC8* CDmAdCertData::FingerprintAlgLC()
    {
    if (iCertCx509->Fingerprint().Length() == 16)
        {
        return TDmAdUtil::IntToDes8LC(1); // MD5
        }
    else
        {
        return TDmAdUtil::IntToDes8LC(2); // SHA1
        }
    }
    
HBufC8* CDmAdCertData::FingerprintValueLC()
    {
    return iCertCx509->Fingerprint().AllocLC();
    }
    
HBufC8* CDmAdCertData::ValidityBeginLC()
    {
    TTime beginTime = iCertCx509->ValidityPeriod().Start();
    return FormatCertValidityTimeLC(beginTime);
    }
    
HBufC8* CDmAdCertData::ValidityEndLC()
    {
    TTime endTime = iCertCx509->ValidityPeriod().Finish();
    return FormatCertValidityTimeLC(endTime);
    }
    
HBufC8* CDmAdCertData::SubjectNameLC()
    {
    HBufC8* ret = NULL;
    const TPtrC8 subjectName(*(iCertCx509->DataElementEncoding(CX509Certificate::ESubjectName)));
    if (subjectName.Length() == 0)
        {
        ret = HBufC8::NewLC(1);
        }
    else
        {
        ret = subjectName.AllocLC();
        }
    return ret;
    }
    
HBufC8* CDmAdCertData::SubjectAltNameLC()
    {
    HBufC8* subjectAltNameExtRfc822 = HBufC8::NewLC(KMaxRfc822);
    TPtr8 subjectAltNameExtRfc822Ptr(subjectAltNameExtRfc822->Des());

    const CX509CertExtension* subjAltName;
    subjAltName = iCertCx509->Extension(KSubjectAltName);
    if (subjAltName)
        {
        CX509AltNameExt* subjectAlt = CX509AltNameExt::NewLC(subjAltName->Data());
        if (subjectAlt)
            {
            const CArrayPtrFlat<CX509GeneralName>* nameArray; 
            nameArray = &subjectAlt->AltName();
            // Search rfc822
            for (TInt i = 0; i < nameArray->Count(); i++)
                {
                if(nameArray->At(i)->Tag() == EX509RFC822Name)
                    {
                    TPtrC8 data = nameArray->At(i)->Data();
                    subjectAltNameExtRfc822Ptr.Copy(data.Right(data.Length() - 2));
                    break;
                    }
                }
            CleanupStack::PopAndDestroy(subjectAlt);
            }
        }
    return subjectAltNameExtRfc822;
    }
    
HBufC8* CDmAdCertData::KeyUriLC()
    {
    HBufC8* keyId = KeyIdLC();
    HBufC8* keyUri = FindKeyUriLC(*keyId);
    CleanupStack::Pop(keyUri);
    CleanupStack::PopAndDestroy(keyId); 
    CleanupStack::PushL(keyUri);
    return keyUri;
    }
    
HBufC8* CDmAdCertData::KeyIdLC()
    {
#if 0
    const CSubjectPublicKeyInfo* subjectPublicKeyInfo = &iCertCx509->PublicKey();
    if (subjectPublicKeyInfo->AlgorithmId() != ERSA)
        {
        return HBufC8::NewLC(1);
        }
    
    return TDmAdCertUtil::RSAKeyIdentifierLC(subjectPublicKeyInfo->KeyData());
#else
    return iCertCx509->KeyIdentifierL().AllocLC();
#endif
    }


HBufC8* CDmAdCertData::KeyUsageLC()
    {
    const TInt KKeyUsageTxtLen = 12;   
    HBufC8* keyUsage = HBufC8::NewLC(KKeyUsageTxtLen);
    TPtr8 keyUsagePtr(keyUsage->Des());

    const CX509CertExtension* certExt = iCertCx509->Extension(KKeyUsage);

    if(0 == certExt)
        {
        return keyUsage;
        }
        
    const CX509KeyUsageExt* keyUsageExt =
        CX509KeyUsageExt::NewLC(certExt->Data());
    
    // Encode key usage values to bstring format
    // See IETF RFC 3641.
    keyUsagePtr.Append('\'');

    ENC_BIT( EX509DigitalSignature );
    ENC_BIT( EX509NonRepudiation );
    ENC_BIT( EX509KeyEncipherment );
    ENC_BIT( EX509DataEncipherment );
    ENC_BIT( EX509KeyAgreement );
    ENC_BIT( EX509KeyCertSign );
    ENC_BIT( EX509CRLSign );
    ENC_BIT( EX509EncipherOnly );
    ENC_BIT( EX509DecipherOnly );

    keyUsagePtr.Append(_L8("'B"));
        
    CleanupStack::PopAndDestroy();  // keyUsageExt
    
    return keyUsage;
    }


HBufC8* CDmAdCertData::FormatCertValidityTimeLC(const TTime& aTime)
    {
    TDateTime time = aTime.DateTime();
    
    HBufC8* ret = HBufC8::NewLC(20);
    TPtr8 retPtr(ret->Des());

    _LIT8(KFormatTxt,"%4d%02d%02dT%02d%02d%02dZ");
    retPtr.Format(KFormatTxt,
                  time.Year(),
                  TInt(time.Month()+1), 
                   // Format the month as a TInt to preserve locale independence
                  time.Day()+1, 
                   // Day and month ranges begin at zero (0-30 and 0-11), 
                   // so add one when formatting
                  time.Hour(),
                  time.Minute(),
                  time.Second());
    return ret;
    }

HBufC8* CDmAdCertData::FindKeyUriLC(const TDesC8& aKeyId)
    {
    DEBUG_LOG(_L("CDmAdCertData::FindKeyUriLC:"));        
    DEBUG_LOG_HEX(aKeyId);    
    
    _LIT8(KDmAdPrivKeyUri, "NokiaPKI/PrivKey");
    
#if 1

    HBufC8* uri = iCallBack->FindChildUriL(KDmAdPrivKeyUri, aKeyId);
    if (uri != 0)
        {
        DEBUG_LOG(_L("CDmAdCertData::FindKeyUriLC:"));
        DEBUG_LOG_HEX(*uri);            
        }
    else
        {
        DEBUG_LOG(_L("URI not found"));        
        uri = HBufC8::NewL(1);
        }
    
    CleanupStack::PushL(uri);
    return uri;
    
#else
    
    HBufC8* ret = 0;
    
    CBufBase* childUriList = CBufFlat::NewL(16);
    CleanupStack::PushL(childUriList);
    MSmlDmAdapter::TError status;
    iCallBack->FetchLinkL(KDmAdPrivKeyUri, *childUriList, status);
    if (status != MSmlDmAdapter::EOk)
        {
        DMADERR(DmAdErr::Printf(_L("***CDmAdCertData::FindKeyUriLC: %d (line=%d)\n"), KDmAdErr1, __LINE__));
        User::Leave(KErrGeneral);
        }

    CArrayFix<TPtrC8>* uriSegList;
    TDmAdUtil::ParseUriLC(childUriList->Ptr(0), uriSegList);

    TBool found = EFalse;
    HBufC8* uri = 0;
    HBufC8* uriKeyId = 0;
    for (TInt i=0; i<uriSegList->Count(); ++i)
        {
        uri = TDmAdUtil::BuildUriLC(KDmAdPrivKeyUri, uriSegList->At(i));
        uriKeyId = TDmAdUtil::BuildUriLC(*uri, KDmAdLeafKeyId);
        
        CBufBase* keyId = CBufFlat::NewL(16);
        CleanupStack::PushL(keyId);
        iCallBack->FetchLinkL(*uriKeyId, *keyId, status);
        if (status != MSmlDmAdapter::EOk)
            {
            DMADERR(DmAdErr::Printf(_L("***CDmAdCertData::FindKeyUriLC: %d (line=%d)\n"), KDmAdErr1, __LINE__));
            User::Leave(KErrGeneral);
            }
        if (keyId->Ptr(0).Compare(aKeyId) == 0)
            {
            found = ETrue;
            CleanupStack::PopAndDestroy(2); // keyId, uriKeyId
            break;
            }
        CleanupStack::PopAndDestroy(3); // keyId, uriKeyId, uri
        }
    
    if (found)
        {
        DMADLOG(DmAdLog::Printf(_L("CDmAdCertData::FindKeyUriLC:\n")));
        DMADLOG(DmAdLog::HexDump(NULL, NULL, uri->Ptr(), uri->Size()));
        CleanupStack::Pop(uri);
        ret = uri;
        }
    else
        {
        DMADLOG(DmAdLog::Printf(_L("CDmAdCertData::FindKeyUriLC: not found\n")));
        ret = HBufC8::NewL(1);
        }
    
    CleanupStack::PopAndDestroy(2); //uriSegList, childUriList
    CleanupStack::PushL(ret);
    return ret;
#endif
    }
    

    
//--------------------------------------------------------------------------

void CDmAdCertData::RestoreL(const CDmAdCertParms& aCertParms)
    {
    TRACE("CDmAdCertData::RestoreL");
    
    iCertParms->SetType(aCertParms.Type());
    iCertParms->SetDeletable(aCertParms.Deletable());
    iCertParms->SetTrusted(aCertParms.Trusted());
    iCertParms->SetApplicabilityL(aCertParms.Applicability());
    
    delete iCertCx509;
    iCertCx509 = 0;
    iCertParms->SetContentL(aCertParms.Content());
    if (iCertParms->Content().Length() > 0)
        {
        iCertCx509 = CX509Certificate::NewL(iCertParms->Content());
        }

    iCertParms->SetKeyId(aCertParms.KeyId()); // now NULL   
    }

void CDmAdCertData::ToStoreL(CDmAdCertParms& aCertParms)
    {
    TRACE("CDmAdCertData::ToStoreL");
    
    aCertParms.SetType(iCertParms->Type());
    aCertParms.SetDeletable(iCertParms->Deletable());
    aCertParms.SetTrusted(iCertParms->Trusted());
    aCertParms.SetApplicabilityL(iCertParms->Applicability());
    aCertParms.SetContentL(iCertParms->Content());

    if (iCertCx509 == NULL)
    	{
    	DEBUG_LOG(_L("No cert!"));  
    	User::Leave(KErrGeneral);
    	}
    TKeyIdentifier keyId = iCertCx509->KeyIdentifierL(); //TKeyIdentifier is 20 bytes long
    aCertParms.SetKeyId(keyId);
    }



//======================================================================================
//======================================================================================
    
CDmAdCertReqData* CDmAdCertReqData::NewL(MDmAdCallBack* aCallBack)
    {
    TRACE("CDmAdCertReqData::NewL");
    
    CDmAdCertReqData* self = new (ELeave) CDmAdCertReqData(aCallBack);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self); 
    return self;
    }

void CDmAdCertReqData::ConstructL()
    {
    TRACE("CDmAdCertReqData::ConstructL");
    iCertReqParms = CDmAdCertReqParms::NewL();
    }
    
CDmAdCertReqData::CDmAdCertReqData(MDmAdCallBack* aCallBack) : iCallBack(aCallBack)
    {
    }

CDmAdCertReqData::~CDmAdCertReqData()
    {
    TRACE("CDmAdCertReqData::~CDmAdCertReqData");    
    delete iCertReqParms;
    }

void CDmAdCertReqData::SetDefaultSettings()
    {
    TRACE("CDmAdCertReqData::SetDefaultSettings");
    iCertReqParms->SetKeyLength(1024);
    }

//--------------------------------------------------------------------------

void CDmAdCertReqData::SetSubjectNameL(const TDesC8& aSubjectName)
    {
    TRACE("CDmAdCertReqData::SetSubjectNameL");
    iCertReqParms->SetSubjectNameL(aSubjectName);
    }

TPtrC8 CDmAdCertReqData::SubjectName()
    {
    TRACE("CDmAdCertReqData::SubjectName");
    return iCertReqParms->SubjectName();
    }

void CDmAdCertReqData::SetRfc822NameL(const TDesC8& aRfc822Name)
    {
    TRACE("CDmAdCertReqData::SetRfc822NameL");
    iCertReqParms->SetRfc822NameL(aRfc822Name);
    }

TPtrC8 CDmAdCertReqData::Rfc822Name()
    {
    TRACE("CDmAdCertReqData::Rfc822Name");
    return iCertReqParms->Rfc822Name();
    }

void CDmAdCertReqData::SetKeyUriL(const TDesC8& aKeyUri)
    {
    TRACE("CDmAdCertReqData::SetKeyUriL");  
    
    //We save key luid (which is also key id) to the data
    //structure. Not the actual key uri.
    
    HBufC8* keyId = iCallBack->GetLuidAllocL(aKeyUri);
    CleanupStack::PushL(keyId);  
    
    if (keyId->Length() == 0)
        {
        User::Leave(KErrNotFound);
        }
        
    iCertReqParms->SetKeyIdentifierByUriL(*keyId);
    CleanupStack::PopAndDestroy(keyId);
    }

HBufC8* CDmAdCertReqData::KeyUriL() const
    {
    TRACE("CDmAdCertReqData::KeyUri");  
    
    TPtrC8 keyId = iCertReqParms->KeyIdentifierByUri();        
    HBufC8* keyUri = iCallBack->FindChildUriL(KDmAdOwnAdUriForPrivateKeys, keyId); 
    CleanupStack::PushL(keyUri);
    
    if (keyUri->Length() == 0)
        {
        User::Leave(KErrNotFound);
        }
    
    CleanupStack::Pop(keyUri);
    return keyUri;      
    }
    
void CDmAdCertReqData::SetKeyLength(const TDesC8& aKeyLength)
    {
    TRACE("CDmAdCertReqData::SetKeyLength");
    iCertReqParms->SetKeyLength(TDmAdUtil::DesToInt(aKeyLength));
    }

HBufC8* CDmAdCertReqData::KeyLengthLC()
    {    
    return TDmAdUtil::IntToDes8LC(iCertReqParms->KeyLength());
    }

//------------

    
TPtrC8 CDmAdCertReqData::Content()
    {
    TRACE("CDmAdCertReqData::Content");
    return iCertReqParms->Content();
    }

//--------------------------------------------------------------------------

void CDmAdCertReqData::RestoreL(const CDmAdCertReqParms& aCertReqParms)
    {
    TRACE("CDmAdCertReqData::RestoreL");
    iCertReqParms->SetSubjectNameL(aCertReqParms.SubjectName());
    iCertReqParms->SetRfc822NameL(aCertReqParms.Rfc822Name());
    iCertReqParms->SetKeyLength(aCertReqParms.KeyLength());
    iCertReqParms->SetKeyIdentifierByUriL(aCertReqParms.KeyIdentifierByUri());
    iCertReqParms->SetContentL(aCertReqParms.Content());
    }

void CDmAdCertReqData::ToStoreL(CDmAdCertReqParms& aCertReqParms)
    {
    TRACE("CDmAdCertReqData::ToStoreL");
    
    aCertReqParms.SetSubjectNameL(iCertReqParms->SubjectName());
    aCertReqParms.SetRfc822NameL(iCertReqParms->Rfc822Name());
    aCertReqParms.SetKeyLength(iCertReqParms->KeyLength());
    aCertReqParms.SetKeyIdentifierByUriL(iCertReqParms->KeyIdentifierByUri());
    aCertReqParms.SetContentL(iCertReqParms->Content());
    }

//======================================================================================
//======================================================================================
    
CDmAdPrivKeyData* CDmAdPrivKeyData::NewL()
    {
    TRACE("CDmAdPrivKeyData::NewL");
    CDmAdPrivKeyData *self = new (ELeave) CDmAdPrivKeyData();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CDmAdPrivKeyData::ConstructL()
    {
    TRACE("CDmAdPrivKeyData::ConstructL");
    iPrivKeyParms = CDmAdPrivKeyParms::NewL();
    }
    
CDmAdPrivKeyData::CDmAdPrivKeyData()
    {
    }

CDmAdPrivKeyData::~CDmAdPrivKeyData()
    {
    TRACE("CDmAdPrivKeyData::~CDmAdPrivKeyData");
    delete iPrivKeyParms;
    }

void CDmAdPrivKeyData::SetDefaultSettingsL()
    {
    TRACE("CDmAdPrivKeyData::SetDefaultSettings");
    iPrivKeyParms->SetKeyTypeL(EPKIRSA); 
    iPrivKeyParms->SetKeyLength(2048);
    }

//--------------------------------------------------------------------------

void CDmAdPrivKeyData::SetKeyTypeL(const TDesC8& aKeyType)
    {
    TRACE("CDmAdPrivKeyData::SetKeyType");
    TInt value = TDmAdUtil::DesToInt(aKeyType);
    
    TPKIKeyAlgorithm keyType = EPKIRSA;
    switch(value)
        {
        case 1:
            keyType = EPKIRSA;
            break;
        case 2:            
            keyType = EPKIDSA;
            break;
        default:  
            User::Leave(KErrCorrupt);          
            break;
        }
    
    iPrivKeyParms->SetKeyTypeL(keyType);
    }

HBufC8* CDmAdPrivKeyData::KeyTypeLC()
    {
    TInt keyType = 1;            
    switch(iPrivKeyParms->KeyType())
        {
        case EPKIRSA:
            keyType = 1;
            break;
        case EPKIDSA:
            keyType = 2;
            break;
        default:
            User::Invariant();
            break;            
        }          
    return TDmAdUtil::IntToDes8LC(keyType);
    }

void CDmAdPrivKeyData::SetKeyLength(const TDesC8& aKeyLength)
    {
    TRACE("CDmAdPrivKeyData::SetKeyLength");
    iPrivKeyParms->SetKeyLength(TDmAdUtil::DesToInt(aKeyLength));
    }

HBufC8* CDmAdPrivKeyData::KeyLengthLC()
    {
    return TDmAdUtil::IntToDes8LC(iPrivKeyParms->KeyLength());
    }
    
//------------

TPtrC8 CDmAdPrivKeyData::KeyId()
    {
    TRACE("CDmAdPrivKeyData::KeyId");
    return iPrivKeyParms->KeyId();
    }
    
//--------------------------------------------------------------------------

void CDmAdPrivKeyData::RestoreL(const CDmAdPrivKeyParms& aPrivKeyParms)
    {
    TRACE("CDmAdPrivKeyData::RestoreL");
    iPrivKeyParms->SetKeyTypeL(aPrivKeyParms.KeyType());
    iPrivKeyParms->SetKeyIdL(aPrivKeyParms.KeyId());
    iPrivKeyParms->SetKeyLength(aPrivKeyParms.KeyLength());
    }

void CDmAdPrivKeyData::ToStoreL(CDmAdPrivKeyParms& aPrivKeyParms)
    {
    TRACE("CDmAdPrivKeyData::ToStoreL");
    aPrivKeyParms.SetKeyTypeL(iPrivKeyParms->KeyType());
    aPrivKeyParms.SetKeyIdL(iPrivKeyParms->KeyId());
    aPrivKeyParms.SetKeyLength(iPrivKeyParms->KeyLength());
    }



//======================================================================================
//======================================================================================
    
CDmAdPKCS12Data* CDmAdPKCS12Data::NewL(/*MDmAdCallBack* aCallBack*/)
    {
    TRACE("CDmAdPKCS12Data::NewL");
    
//    CDmAdPKCS12Data* self = new (ELeave) CDmAdPKCS12Data(aCallBack);
    CDmAdPKCS12Data* self = new (ELeave) CDmAdPKCS12Data();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CDmAdPKCS12Data::ConstructL()
    {
    TRACE("CDmAdPKCS12Data::ConstructL");
    
    iPKCS12Parms = CDmAdPKCS12Parms::NewL();
    }
    
CDmAdPKCS12Data::CDmAdPKCS12Data(/*MDmAdCallBack* aCallBack*/)
// : iCallBack(aCallBack)
    {    
    }

CDmAdPKCS12Data::~CDmAdPKCS12Data()
    {
    TRACE("CDmAdPKCS12Data::~CDmAdPKCS12Data");
    
    delete iPKCS12Parms;
    }

void CDmAdPKCS12Data::SetDefaultSettings()
    {
    TRACE("CDmAdPKCS12Data::SetDefaultSettings");
    
    TPtrC8 emptyPwd(KNullDesC8);
    iPKCS12Parms->SetDeletable(ETrue);
    TRAP_IGNORE(iPKCS12Parms->SetPasswordL(emptyPwd));
    }

//--------------------------------------------------------------------------

void CDmAdPKCS12Data::SetDeletableL(const TDesC8& aDeletable)
    {
    TRACE("CDmAdPKCS12Data::SetDeletableL");
    
    if (aDeletable.Compare(KDmAdDfFormatBoolTrue) == 0)
        {
        iPKCS12Parms->SetDeletable(ETrue);
        }
    else if (aDeletable.Compare(KDmAdDfFormatBoolFalse) == 0)
        {
        iPKCS12Parms->SetDeletable(EFalse);
        }
    else
        {
        DEBUG_LOG(_L("Comparization failed"));                
        User::Leave(KErrGeneral);
        }
    }

HBufC8* CDmAdPKCS12Data::DeletableLC()
    {    
    if (iPKCS12Parms->Deletable())
        {
        return KDmAdDfFormatBoolTrue().AllocLC();
        }
    else
        {
        return KDmAdDfFormatBoolFalse().AllocLC();
        }
    }

void CDmAdPKCS12Data::SetApplicabilityL(const RArray<TUid>& aApplicability)
    {
    TRACE("CDmAdPKCS12Data::SetApplicabilityL");
    
    iPKCS12Parms->SetApplicabilityL(aApplicability);
    }

const RArray<TUid>& CDmAdPKCS12Data::Applicability() const
    {
    TRACE("CDmAdPKCS12Data::Applicability");
    
    return iPKCS12Parms->Applicability();
    }

void CDmAdPKCS12Data::SetContentL(const TDesC8& aContent)
    {
    TRACE("CDmAdPKCS12Data::SetContentL");

    iPKCS12Parms->SetContentL(aContent);    
    }

TPtrC8 CDmAdPKCS12Data::Content()
    {
    TRACE("CDmAdPKCS12Data::Content");
    
    return iPKCS12Parms->Content();
    }

void CDmAdPKCS12Data::SetPasswordL(const TDesC8& aPassword)
    {
    TRACE("CDmAdPKCS12Data::SetPasswordL");

    iPKCS12Parms->SetPasswordL(aPassword);
    }

TPtrC8 CDmAdPKCS12Data::Password()
    {
    TRACE("CDmAdPKCS12Data::Password");
    
    return iPKCS12Parms->Password();
    }

//-----------------
    

    
//--------------------------------------------------------------------------

void CDmAdPKCS12Data::RestoreL(const CDmAdPKCS12Parms& aPKCS12Parms)
    {
    TRACE("CDmAdPKCS12Data::RestoreL");
    iPKCS12Parms->SetDeletable(aPKCS12Parms.Deletable());
    iPKCS12Parms->SetApplicabilityL(aPKCS12Parms.Applicability());
    iPKCS12Parms->SetContentL(aPKCS12Parms.Content());
    iPKCS12Parms->SetPasswordL(aPKCS12Parms.Content());
    }

void CDmAdPKCS12Data::ToStoreL(CDmAdPKCS12Parms& aPKCS12Parms)
    {
    TRACE("CDmAdPKCS12Data::ToStoreL");
    
    aPKCS12Parms.SetDeletable(iPKCS12Parms->Deletable());
    aPKCS12Parms.SetApplicabilityL(iPKCS12Parms->Applicability());
    aPKCS12Parms.SetContentL(iPKCS12Parms->Content());
    aPKCS12Parms.SetPasswordL(iPKCS12Parms->Password());
    }

