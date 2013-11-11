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
* Description:   ddf tree implementation of DmAdPKI
*
*/



#include <utf.h>
#include <certificateapps.h>

#include "dmadddf.h"
#include "dmadutil.h"
#include "vpnlogger.h"
#include "dmadcertxmldefs.h"
#include "XwImpl.h"

static const TInt KUserStore = 1;
static const TInt KDeviceStore = 2;

CDmAdDdf* CDmAdDdf::NewL(MDmAdCallBack* aDmAdCallBack, CDmAdStore* aStore)
    {
    TRACE("CDmAdDdf::NewL");
    
    CDmAdDdf* self = NewLC(aDmAdCallBack, aStore);
    CleanupStack::Pop(self);
    return self;
    }

CDmAdDdf* CDmAdDdf::NewLC(MDmAdCallBack* aDmAdCallBack, CDmAdStore* aStore)
    {       
    CDmAdDdf* self = new (ELeave) CDmAdDdf(aDmAdCallBack, aStore);
    CleanupStack::PushL(self);
    return self;
    }
    
CDmAdDdf::CDmAdDdf(MDmAdCallBack* aDmAdCallBack, CDmAdStore* aStore) :
   iCallBack(aDmAdCallBack), iStore(aStore)
    {
    TRACE("CDmAdDdf::CDmAdDdf");
    }

CDmAdDdf::~CDmAdDdf()
    {
    TRACE("CDmAdDdf::~CDmAdDdf");
    }
    
//===================================================================================================
    
void CDmAdDdf::BuildDDFVersionL(CBufBase& aDDFVersion)
    {
    TRACE("CDmAdDdf::BuildDDFVersionL");    
    aDDFVersion.InsertL(0, KDmAdDdfVersion);
    }

void CDmAdDdf::BuildDDFStructureL(MSmlDmDDFObject& aDDF)
    {
    TRACE("CDmAdDdf::BuildDDFStructureL");
    
    TSmlDmAccessTypes accessTypesExec;
    accessTypesExec.SetReplace(); // accessTypesExec.SetExec();

    TSmlDmAccessTypes accessTypesGet;
    accessTypesGet.SetGet();

    TSmlDmAccessTypes accessTypesAdd;
    accessTypesAdd.SetAdd();
    
    TSmlDmAccessTypes accessTypesGetAdd;
    accessTypesGetAdd.SetGet();
    accessTypesGetAdd.SetAdd();
    
    TSmlDmAccessTypes accessTypesAll;
    accessTypesAll.SetGet();
    accessTypesAll.SetAdd();
    accessTypesAll.SetDelete();
    accessTypesAll.SetReplace();
    
    TSmlDmAccessTypes accessTypesNoDelete;
    accessTypesNoDelete.SetGet();
    accessTypesNoDelete.SetAdd();
    accessTypesNoDelete.SetReplace();

    MSmlDmDDFObject* obj;
    MSmlDmDDFObject* objNokiaPki;
    MSmlDmDDFObject* objPkiGeneral;
    MSmlDmDDFObject* objCert;
    MSmlDmDDFObject* objCertX;
    MSmlDmDDFObject* objCertReq;
    MSmlDmDDFObject* objCertReqX;
    MSmlDmDDFObject* objPrivKey;
    MSmlDmDDFObject* objPrivKeyX;
    MSmlDmDDFObject* objPKCS12;
    MSmlDmDDFObject* objPKCS12X;
    

//----------------------------------------------------------------
    
    objNokiaPki = &TDmAdUtil::AddChildObjectL(aDDF, KDmAdNodeNokiaPki);
    TDmAdUtil::FillNodeInfoL(*objNokiaPki,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EPermanent,
                             MSmlDmDDFObject::ENode,
                             KDmAdDescNodeNokiaPki,
                             ETrue,
                             KDmAdNokiaPkiRootTypeProperty);
    
//----------------------------------------------------------------
    
    
    obj = &TDmAdUtil::AddChildObjectL(*objNokiaPki, KDmAdLeafLogon);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesExec,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::ENull,
                             KDmAdDescLeafLogon,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objNokiaPki, KDmAdLeafLogoff);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesExec,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::ENull,
                             KDmAdDescLeafLogoff,
                             EFalse);

    obj = &TDmAdUtil::AddChildObjectL(*objNokiaPki, KDmAdLeafKeyStore);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesExec,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EInt,
                             KDmAdDescLeafKeyStore,
                             EFalse);
                             
    obj = &TDmAdUtil::AddChildObjectL(*objNokiaPki, KDmAdLeafCertStore);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesExec,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EInt,
                             KDmAdDescLeafCertStore,
                             EFalse);
                                                          
    objPkiGeneral = &TDmAdUtil::AddChildObjectL(*objNokiaPki, KDmAdNodeGeneral);
    TDmAdUtil::FillNodeInfoL(*objPkiGeneral,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EPermanent,
                             MSmlDmDDFObject::ENode,
                             KDmAdDescNodeGeneral,
                             EFalse,
                             KNullDesC8);
    
    obj = &TDmAdUtil::AddChildObjectL(*objPkiGeneral, KDmAdLeafCertApplications);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EXml,
                             KDmAdDescLeafCertApplications,
                             EFalse);
    
//----------------------------------------------------------------
    
    objCert = &TDmAdUtil::AddChildObjectL(*objNokiaPki, KDmAdNodeCert);
    TDmAdUtil::FillNodeInfoL(*objCert,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EPermanent,
                             MSmlDmDDFObject::ENode,
                             KDmAdDescNodeCert,
                             EFalse,
                             KNullDesC8);
    
    objCertX = &TDmAdUtil::AddChildObjectL(*objCert, KDmAdNodeRt);
    TDmAdUtil::FillNodeInfoL(*objCertX,
                             accessTypesAll,
                             MSmlDmDDFObject::EZeroOrMore,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::ENode,
                             KDmAdDescNodeRt,
                             EFalse,
                             KNullDesC8);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertX, KDmAdLeafType);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGetAdd,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EInt,
                             KDmAdDescLeafType,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertX, KDmAdLeafFormat);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGetAdd,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EInt,
                             KDmAdDescLeafFormat,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertX, KDmAdLeafSerialNumber);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EBin,
                             KDmAdDescLeafSerialNumber,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertX, KDmAdLeafIssuerName);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EBin,
                             KDmAdDescLeafIssuerName,
                             EFalse);    
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertX, KDmAdLeafFingerprintAlg);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EInt,
                             KDmAdDescLeafFingerprintAlg,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertX, KDmAdLeafFingerprintValue);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EBin,
                             KDmAdDescLeafFingerprintAlg,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertX, KDmAdLeafValidityBegin);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafValidityBegin,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertX, KDmAdLeafValidityEnd);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafValidityEnd,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertX, KDmAdLeafSubjectName);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EBin,
                             KDmAdDescLeafSubjectName,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertX, KDmAdLeafSubjectAltName);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGet,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EBin,
                             KDmAdDescLeafSubjectAltName,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertX, KDmAdLeafKeyURI);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGet,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafKeyUri,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertX, KDmAdLeafKeyId);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGet,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EBin,
                             KDmAdDescLeafKeyId,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertX, KDmAdLeafKeyUsage);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGet,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafKeyUsage,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertX, KDmAdLeafDeletable);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGetAdd,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EBool,
                             KDmAdDescLeafDeletable,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertX, KDmAdLeafTrusted);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesNoDelete,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EBool,
                             KDmAdDescLeafTrusted,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertX, KDmAdLeafApplicability);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesNoDelete,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EXml,
                             KDmAdDescLeafApplicability,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertX, KDmAdLeafContent);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGetAdd,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EBin,
                             KDmAdDescLeafContent,
                             EFalse);
    
    
//----------------------------------------------------------------
    
    objCertReq = &TDmAdUtil::AddChildObjectL(*objNokiaPki, KDmAdNodeCertReq);
    TDmAdUtil::FillNodeInfoL(*objCertReq,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EPermanent,
                             MSmlDmDDFObject::ENode,
                             KDmAdDescNodeCertReq,
                             EFalse,
                             KNullDesC8);

    
    objCertReqX = &TDmAdUtil::AddChildObjectL(*objCertReq, KDmAdNodeRt);
    TDmAdUtil::FillNodeInfoL(*objCertReqX,
                             accessTypesAll,
                             MSmlDmDDFObject::EZeroOrMore,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::ENode,
                             KDmAdDescNodeRt,
                             EFalse,
                             KNullDesC8);  
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertReqX, KDmAdLeafSubjectName);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesAdd,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EBin,
                             KDmAdDescLeafSubjectName,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertReqX, KDmAdLeafRfc822Name);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesAdd,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EBin,
                             KDmAdDescLeafRfc822Name,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objCertReqX, KDmAdLeafKeyURI);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesAdd,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafKeyUri,
                             EFalse);
        
    obj = &TDmAdUtil::AddChildObjectL(*objCertReqX, KDmAdLeafKeyLength);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesAdd,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EInt,
                             KDmAdDescLeafKeyLength,
                             EFalse);
           
    obj = &TDmAdUtil::AddChildObjectL(*objCertReqX, KDmAdLeafContent);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGet,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EBin,
                             KDmAdDescLeafContent,
                             EFalse);
    
//----------------------------------------------------------------
    
    objPrivKey = &TDmAdUtil::AddChildObjectL(*objNokiaPki, KDmAdNodePrivKey);
    TDmAdUtil::FillNodeInfoL(*objPrivKey,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EPermanent,
                             MSmlDmDDFObject::ENode,
                             KDmAdDescNodePrivKey,
                             EFalse,
                             KNullDesC8);

    
    objPrivKeyX = &TDmAdUtil::AddChildObjectL(*objPrivKey, KDmAdNodeRt);
    TDmAdUtil::FillNodeInfoL(*objPrivKeyX,
                             accessTypesAll,
                             MSmlDmDDFObject::EZeroOrMore,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::ENode,
                             KDmAdDescNodeRt,
                             EFalse,
                             KNullDesC8);
    
    obj = &TDmAdUtil::AddChildObjectL(*objPrivKeyX, KDmAdLeafKeyType);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGetAdd,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EInt,
                             KDmAdDescLeafKeyType,
                             EFalse);
        
    obj = &TDmAdUtil::AddChildObjectL(*objPrivKeyX, KDmAdLeafKeyId);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafKeyId,
                             EFalse);
    
    obj = &TDmAdUtil::AddChildObjectL(*objPrivKeyX, KDmAdLeafKeyLength);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesGetAdd,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EInt,
                             KDmAdDescLeafKeyLength,
                             EFalse);
        
    obj = &TDmAdUtil::AddChildObjectL(*objPrivKeyX, KDmAdLeafContent);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesAdd,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EBin,
                             KDmAdDescLeafContent,
                             EFalse);

    //------- PKCS#12 Node definitions -----------------

    DEBUG_LOG(_L("Adding PKCS#12 definitions"));

    // PKCS12 root node
    objPKCS12 = &TDmAdUtil::AddChildObjectL(*objNokiaPki, KDmAdNodePKCS12);
    TDmAdUtil::FillNodeInfoL(*objPKCS12,
                             accessTypesGet,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EPermanent,
                             MSmlDmDDFObject::ENode,
                             KDmAdDescNodePKCS12,
                             EFalse,
                             KNullDesC8);

    // PKCS#12/X node
    objPKCS12X = &TDmAdUtil::AddChildObjectL(*objPKCS12, KDmAdNodeRt);
    TDmAdUtil::FillNodeInfoL(*objPKCS12X,
                             accessTypesAdd,
                             MSmlDmDDFObject::EZeroOrMore,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::ENode,
                             KDmAdDescNodeRt,
                             EFalse,
                             KNullDesC8);

    // PKCS#12/X/Password leaf node
    obj = &TDmAdUtil::AddChildObjectL(*objPKCS12X, KDmAdLeafPKCS12Password);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesAdd,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EChr,
                             KDmAdDescLeafPKCS12Password,
                             EFalse);

    // PKCS#12/X/Deletable leaf node
    obj = &TDmAdUtil::AddChildObjectL(*objPKCS12X, KDmAdLeafDeletable);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesAdd,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EBool,
                             KDmAdDescLeafDeletable,
                             EFalse);

    // PKCS#12/X/Applicability leaf node
    obj = &TDmAdUtil::AddChildObjectL(*objPKCS12X, KDmAdLeafApplicability);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesAdd,
                             MSmlDmDDFObject::EZeroOrOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EXml,
                             KDmAdDescLeafApplicability,
                             EFalse);

    // PKCS#12/X/Content leaf node
    obj = &TDmAdUtil::AddChildObjectL(*objPKCS12X, KDmAdLeafContent);
    TDmAdUtil::FillNodeInfoL(*obj,
                             accessTypesAdd,
                             MSmlDmDDFObject::EOne,
                             MSmlDmDDFObject::EDynamic,
                             MSmlDmDDFObject::EBin,
                             KDmAdDescLeafContent,
                             EFalse);

    DEBUG_LOG(_L("Adding PKCS#12 definitions - COMPLETE"));

    }
    
//---------------------------------------------------------------------------------------
    
void CDmAdDdf::NotRtNodeAddNodeObjectL(const TDesC8& /*aUri*/, const TDesC8& /*aParentLuid*/, TInt aStatusRef)
    {
    TRACE("CDmAdDdf::NotRtNodeAddNodeObjectL");
    
    iCallBack->SetStatusL(aStatusRef, KErrNone);
    }

void CDmAdDdf::NotRtNodeUpdateLeafObjectL(const TDesC8& aUri, const TDesC8& /*aLuid*/, const TDesC8& aObject, const TDesC8& /*aType*/, TInt aStatusRef)
    {
    TRACE("CDmAdDdf::NotRtNodeUpdateLeafObjectL");
    
    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdLeafLogon) == 0)
        {
        iStore->PkiLogonL();
        }
    else if (lastSeg.Compare(KDmAdLeafLogoff) == 0)
        {
        iStore->PkiLogoffL();
        }
    else if (lastSeg.Compare(KDmAdLeafKeyStore) == 0)
        {        
        TInt storeFromDm = TDmAdUtil::DesToInt(aObject);
        TPkiServiceStoreType pkiStoreType = EPkiStoreTypeAny;
        switch(storeFromDm)
            {
            case KUserStore:
                pkiStoreType = EPkiStoreTypeUser;
                break;
            case KDeviceStore:
                pkiStoreType = EPkiStoreTypeDevice;
                break;
            default:
                User::Leave(KPKIErrNotSupported);
                break;                
            }
        iStore->SetStoreTypeL(STORE_KEYSTORE, pkiStoreType);
        }
    else if (lastSeg.Compare(KDmAdLeafCertStore) == 0)
        {
        TInt storeFromDm = TDmAdUtil::DesToInt(aObject);
        TPkiServiceStoreType pkiStoreType = EPkiStoreTypeAny;
        switch(storeFromDm)
            {
            case KUserStore:
                pkiStoreType = EPkiStoreTypeUser;
                break;
            case KDeviceStore:
                pkiStoreType = EPkiStoreTypeDevice;
                break;
            default:
                User::Leave(KPKIErrNotSupported);
                break;                
            }
        
        iStore->SetStoreTypeL(STORE_CERTSTORE, pkiStoreType);
        }
       
    iCallBack->SetStatusL(aStatusRef, KErrNone);
    }

TInt CDmAdDdf::NotRtNodeFetchLeafObjectLC(const TDesC8& aUri, const TDesC8& /*aLuid*/, const TDesC8& /*aType*/, CBufBase*& aObject)
    {    
    TInt status = KErrNotFound;
    CBufBase* object = CBufFlat::NewL(32);
    CleanupStack::PushL(object);
    
    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdLeafCertApplications) == 0)
        {

        CCertificateAppInfoManager* appsMan  = CCertificateAppInfoManager::NewLC();

        const RArray<TCertificateAppInfo>& apps = appsMan->Applications();
        TInt count = apps.Count();
        CXmlWriter* writer = CXmlWriter::NewL();
        CleanupStack::PushL(static_cast<CBase*>(writer));
        
        writer->AddStartTagL(KXmlElemCertApps);
        for (TInt i=0; i<count; ++i)
            {
            const TUid& id = apps[i].Id();
            const TName& name = apps[i].Name();

            HBufC8* idInt = TDmAdUtil::IntToDes8LC((TInt)id.iUid);
            HBufC8* name8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(name);
            CleanupStack::PushL(name8);
            
            writer->OpenStartTagL(KXmlElemApp);
            writer->AddAttributeL(KXmlAttrId, *idInt);
            writer->AddAttributeL(KXmlAttrName, *name8);
            writer->CloseStartTagL(ETrue);
            CleanupStack::PopAndDestroy(2); //name8, idInt
            }
        writer->AddEndTagL(KXmlElemCertApps);
        TPtrC8 docDataPtr(writer->DocPart(0, writer->Length()-1));
        object->InsertL(object->Size(), docDataPtr);
        
        DEBUG_LOG(_L("popping writer"));
        CleanupStack::PopAndDestroy(static_cast<CBase*>(writer));        
        DEBUG_LOG(_L("writer popped"));
        
        CleanupStack::PopAndDestroy(appsMan); 
        status = KErrNone;
        }
    aObject = object;
    return status;
    }

void CDmAdDdf::NotRtNodeDeleteObjectL(const TDesC8& /*aUri*/, const TDesC8& /*aLuid*/, TInt aStatusRef)
    {
    TRACE("CDmAdDdf::NotRtNodeDeleteObjectL");
    
    iCallBack->SetStatusL(aStatusRef, KErrNone);
    }
    
//---------------------------------------------------------------------------------------
    
TBool CDmAdDdf::IsNodeRtNodeL(const TDesC8& aUri)
    {
    TRACE("CDmAdDdf::IsNodeRtNodeL");
    
    TBool ret = EFalse;
    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdNodeNokiaPki) == 0 ||
        lastSeg.Compare(KDmAdNodeGeneral) == 0 ||
        lastSeg.Compare(KDmAdNodeCert) == 0 ||
        lastSeg.Compare(KDmAdNodeCertReq) == 0 ||
        lastSeg.Compare(KDmAdNodePrivKey) == 0 ||
        lastSeg.Compare(KDmAdNodePKCS12) == 0) // Added
        {
        if(lastSeg.Compare(KDmAdNodePKCS12) == 0) // Added
            {
            DEBUG_LOG(_L("LastSeg == KDmAdNodePKCS12, ret = FALSE"));
            }
        ret = EFalse;
        }
    else
        {
        if (TDmAdUtil::NumOfURISegs(aUri) < 2)
            {
            DEBUG_LOG(_L("Number of URI segments is < 2"));            
            User::Leave(KErrNotFound);
            }
        TPtrC8 prevSeg = TDmAdUtil::LastUriSeg(TDmAdUtil::RemoveLastUriSeg(aUri));
        if (prevSeg.Compare(KDmAdNodeCert) == 0 ||
            prevSeg.Compare(KDmAdNodeCertReq) == 0 ||
            prevSeg.Compare(KDmAdNodePrivKey) == 0 ||
            prevSeg.Compare(KDmAdNodePKCS12) == 0) // Added
            {
            if(prevSeg.Compare(KDmAdNodePKCS12) == 0) // Added
                {
                DEBUG_LOG(_L("prevSeg == KDmAdNodePKCS12, ret = TRUE"));
                }
            ret = ETrue;
            }
        else
            {
            DEBUG_LOG(_L("URI segment not found"));            
            User::Leave(KErrNotFound);
            }
        }
    return ret;
    }

TBool CDmAdDdf::IsLeafUnderRtNodeL(const TDesC8& aUri)
    {
    TRACE("CDmAdDdf::IsLeafUnderRtNodeL");
    
    TBool ret = EFalse;
    TPtrC8 nodeUri(TDmAdUtil::RemoveLastUriSeg(aUri));
    ret = IsNodeRtNodeL(nodeUri);
    return ret;
    }


TPtrC8 CDmAdDdf::RtNodeUriForLeafL(const TDesC8& aLeafUri)
    {
    TRACE("CDmAdDdf::RtNodeUriForLeafL");
    
    TPtrC8 ret;
    TPtrC8 nodeUri(TDmAdUtil::RemoveLastUriSeg(aLeafUri));    
    TPtrC8 prevNodeUri(TDmAdUtil::RemoveLastUriSeg(nodeUri));
    TPtrC8 prevNodeUriSeg(TDmAdUtil::LastUriSeg(prevNodeUri));
    
    if (prevNodeUriSeg.Compare(KDmAdNodeCert) == 0 ||
        prevNodeUriSeg.Compare(KDmAdNodeCertReq) == 0 ||
        prevNodeUriSeg.Compare(KDmAdNodePrivKey) == 0 ||
        prevNodeUriSeg.Compare(KDmAdNodePKCS12) == 0) // Added
        {
        if (prevNodeUriSeg.Compare(KDmAdNodePKCS12) == 0) 
            {
            DEBUG_LOG(_L("prevNodeUriSeg == KDmAdNodePKCS12"));
            }
        ret.Set(nodeUri);
        }
    else
        {
        DEBUG_LOG(_L("Unknown URI"));        
        User::Leave(KErrNotFound);
        }
    return ret;
    }

TBool CDmAdDdf::IsTopLevelRtNode(const TDesC8& /*aUri*/)
    {
    TRACE("CDmAdDdf::IsTopLevelRtNode");
    return ETrue;
    }

HBufC8* CDmAdDdf::ParentRtNodeUriForRtNodeLC(const TDesC8& /*aUri*/)
    {       
    DEBUG_LOG(_L("Method CDmAdDdf::ParentRtNodeUriForRtNodeLC not implemented"));        
    User::Leave(KErrGeneral);
    return 0;
    }

//---------------------------------------------------------------------------------------
    
void CDmAdDdf::BuildChildUriListLC(const TDesC8& aUri, const TDesC8& aParentLuid, const CArrayFix<TSmlDmMappingInfo>& aPreviousUriSegmentList, CBufBase*& aCurrentList)
    {  
    CBufBase *currentList = CBufFlat::NewL(128);
    CleanupStack::PushL(currentList);

    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdNodeNokiaPki) == 0)
        {
        currentList->InsertL(0, KDmAdListOfNokiaPkiChildren);
        }
    else if (lastSeg.Compare(KDmAdNodeGeneral) == 0)
        {
        currentList->InsertL(0, KDmAdListOfPkiGeneralChildren);
        }
    else if (lastSeg.Compare(KDmAdNodeCert) == 0 ||
             lastSeg.Compare(KDmAdNodeCertReq) == 0 ||
             lastSeg.Compare(KDmAdNodePrivKey) == 0 ||
             lastSeg.Compare(KDmAdNodePKCS12) == 0) // Added
        {
        if (lastSeg.Compare(KDmAdNodePKCS12) == 0) 
            {
            // Special treatment for PKCS12 nodes
            DEBUG_LOG(_L("lastSeg == KDmAdNodePKCS12"));
            TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
            TInt slashcount = 0;
            TInt urisegcount = aPreviousUriSegmentList.Count();

            DEBUG_LOG1(_L(" URI segment count: %d:"), urisegcount);

            for (TInt i=0; i < urisegcount; i++)
                {
                const TSmlDmMappingInfo& mappingInfo = aPreviousUriSegmentList.At(i);

                DEBUG_LOG1(_L(" SetResult URI seg entry %d:"), i);            
                DEBUG_LOG1(_L8(" Uri seg: '%S'"), &(mappingInfo.iURISeg));
                DEBUG_LOG_HEX(mappingInfo.iURISegLUID);

                // Add slash to separate between URIs (no slash after the last one or
                // before the first one)
                if ((slashcount > 0) && (slashcount <= urisegcount)) 
                    {
                    currentList->InsertL(currentList->Size(), KDmAdSeparator);
                    }

                currentList->InsertL(currentList->Size(), mappingInfo.iURISeg);
                slashcount++;
                }
            }
        else 
            {
            TDmAdUtil::BuildRtNodeChildUriListL(iCallBack, iStore, aUri, aParentLuid, aPreviousUriSegmentList, *currentList);
            }
            
#if 0       
        if (currentList->Size() > 0)
            {
            currentList->InsertL(currentList->Size(), KDmAdAppendChildSlashExt);
            }
        else
            {
            currentList->InsertL(currentList->Size(), KDmAdAppendChildExt);
            }
#endif
        }
    else
        {
        TPtrC8 prevSeg = TDmAdUtil::LastUriSeg(TDmAdUtil::RemoveLastUriSeg(aUri));
        if (prevSeg.Compare(KDmAdNodeCert) == 0 ||
            prevSeg.Compare(KDmAdNodeCertReq) == 0 ||
            prevSeg.Compare(KDmAdNodePrivKey) == 0 ||
            prevSeg.Compare(KDmAdNodePKCS12) == 0) // Added
            {
            if (prevSeg.Compare(KDmAdNodePKCS12) == 0) 
                {
                DEBUG_LOG(_L("prevSeg == KDmAdNodePKCS12"));
                }
            if (!iStore->FindRtNodeL(aParentLuid, aUri))
                {
                DEBUG_LOG(_L("Node not found"));
                User::Leave(KErrNotFound);
                }
            
            if (prevSeg.Compare(KDmAdNodeCert) == 0)
                {
                currentList->InsertL(0, KDmAdListOfCertXChildren);
                }
            else if (prevSeg.Compare(KDmAdNodeCertReq) == 0)
                {
                currentList->InsertL(0, KDmAdListOfCertReqXChildren);
                }
            else if (prevSeg.Compare(KDmAdNodePrivKey) == 0)
                {
                currentList->InsertL(0, KDmAdListOfPrivKeyXChildren);
                }
            else if (prevSeg.Compare(KDmAdNodePKCS12) == 0) 
                {
                DEBUG_LOG(_L("inserting KDmAdListOfPKCS12XChildren to currentList"));

                // Added handler for pkcs#12
                currentList->InsertL(0, KDmAdListOfPKCS12XChildren);
                }
            }
        else
            {
            DEBUG_LOG(_L("Unknown URI"));            
            User::Leave(KErrNotFound);
            }
        }

    aCurrentList = currentList;
    }
