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
* Description:   implementation of DmAdStore
*
*/



#include "DmAdStore.h"
#include "DmAdRtNodeData.h"
#include "dmadutil.h"
#include "vpnlogger.h"

#include "pkcs12vpn.h"
    
CDmAdStore* CDmAdStore::NewL(MDmAdCallBack* aDmAdCallBack)
    {
    TRACE("CDmAdStore::NewL");
    
    CDmAdStore *self = new (ELeave) CDmAdStore();
    CleanupStack::PushL(self);
    self->ConstructL(aDmAdCallBack);
    CleanupStack::Pop(self);
    return self;
    }

void CDmAdStore::ConstructL(MDmAdCallBack* aDmAdCallBack)
    {
    TRACE("CDmAdStore::ConstructL");
            
    TInt status = iPkiServiceApi.Connect();
    DEBUG_LOG1(_L("Connect status: %d"), status);
    User::LeaveIfError(status);    
    iCert = CDmAdCert::NewL(iPkiServiceApi);
    iCertReq = CDmAdCertReq::NewL(iPkiServiceApi, aDmAdCallBack);
    iPrivKey = CDmAdPrivKey::NewL(iPkiServiceApi);    
    iPKCS12Obj = CDmAdPKCS12::NewL(iPkiServiceApi);
    }
    
CDmAdStore::CDmAdStore()
    {
    TRACE("CDmAdStore::CDmAdStore");
    }

CDmAdStore::~CDmAdStore()
    {
    TRACE("CDmAdStore::~CDmAdStore");
    
    iPkiServiceApi.Close();
    delete iCert;
    delete iCertReq;
    delete iPrivKey;
    delete iPKCS12Obj;
    }
    
TBool CDmAdStore::FindRtNodeL(const TDesC8& aLuid, const TDesC8& aUri)
    {
    TRACE("CDmAdStore::FindRtNodeL");
    
    TDmAdUriType uriType = CDmAdRtNodeData::UriTypeL(aUri);
    switch (uriType)
        {
        case EDmAdUriTypeCert:
            return CertFindRtNodeL(aLuid, aUri);
        case EDmAdUriTypeCertReq:
            return CertReqFindRtNodeL(aLuid, aUri);
        case EDmAdUriTypePrivKey:
            return PrivKeyFindRtNodeL(aLuid, aUri);
        case EDmAdUriTypePKCS12:
            // For PKCS#12, a special case
            return EFalse;
        default:
            DEBUG_LOG(_L("Unexcepted URI type"));
            User::Leave(KErrGeneral);
            return EFalse;
        }
    }

void CDmAdStore::LuidListL(const TDesC8& aUri, const TDesC8& aLuid, RPointerArray<HBufC8>& aLuidList)
    {
    TRACE("CDmAdStore::LuidListL");
    
    TDmAdUriType uriType = CDmAdRtNodeData::UriTypeL(aUri);
    switch (uriType)
        {
        case EDmAdUriTypeCert:
            CertLuidListL(aUri, aLuid, aLuidList);
            break;
        case EDmAdUriTypeCertReq:
            CertReqLuidListL(aUri, aLuid, aLuidList);
            break;
        case EDmAdUriTypePrivKey:
            PrivKeyLuidListL(aUri, aLuid, aLuidList);
            break;
        case EDmAdUriTypePKCS12:
            // For PKCS#12, a special case
            break;
        default:
            DEBUG_LOG(_L("CDmAdStore::LuidListLC: Unexcepted URI type"));
            User::Leave(KErrGeneral);
            break;
        }    
    }
    
void CDmAdStore::FetchRtNodeL(CDmAdRtNode& aRtNode)
    {
    TRACE("CDmAdStore::FetchRtNodeL");
        
    TDmAdUriType uriType = aRtNode.Data()->UriType();
    switch (uriType)
        {
        case EDmAdUriTypeCert:
            CertFetchRtNodeL(aRtNode);
            break;
        case EDmAdUriTypeCertReq:
            CertReqFetchRtNodeL(aRtNode);
            break;
        case EDmAdUriTypePrivKey:
            PrivKeyFetchRtNodeL(aRtNode);
            break;
        case EDmAdUriTypePKCS12:
            // For PKCS#12, a special case
            break;
        default:
            DEBUG_LOG(_L("Unexcepted URI type"));
            User::Leave(KErrGeneral);
            break;
        }
    }
    
void CDmAdStore::SaveTopLevelRtNodeL(CDmAdRtNode& aRtNode, CArrayPtr<CDmAdRtNode>& aChildRtNodes)
    {
    TRACE("CDmAdStore::SaveTopLevelRtNodeL");
    
    TDmAdUriType uriType = aRtNode.Data()->UriType();
    switch (uriType)
        {
        case EDmAdUriTypeCert:
            CertSaveTopLevelRtNodeL(aRtNode, aChildRtNodes);
            break;
        case EDmAdUriTypeCertReq:
            CertReqSaveTopLevelRtNodeL(aRtNode, aChildRtNodes);
            break;
        case EDmAdUriTypePKCS12:
            PKCS12SaveTopLevelRtNodeL(aRtNode, aChildRtNodes);
            break;
        default:
            DEBUG_LOG(_L("Unexcepted URI type"));
            User::Leave(KErrGeneral);
            break;
        }
    }

void CDmAdStore::SaveChildLevelRtNodeL(CDmAdRtNode& aRtNode)
    {
    TRACE("CDmAdStore::SaveChildLevelRtNodeL");
    
    TDmAdUriType uriType = aRtNode.Data()->UriType();
    switch (uriType)
        {
        case EDmAdUriTypeCert:
            CertSaveChildLevelRtNodeL(aRtNode);
            break;
        case EDmAdUriTypeCertReq:
            CertReqSaveChildLevelRtNodeL(aRtNode);
            break;
        case EDmAdUriTypePKCS12:
            // For PKCS#12, a special case
            break;
        default:
            DEBUG_LOG(_L("Unexcepted URI type"));
            User::Leave(KErrGeneral);
            break;
        }
    }

void CDmAdStore::DeleteRtNodeL(const TDesC8& aLuid, const TDesC8& aUri)
    {
    TRACE("CDmAdStore::DeleteRtNodeL");
    
    TDmAdUriType uriType = CDmAdRtNodeData::UriTypeL(aUri);
    switch (uriType)
        {
        case EDmAdUriTypeCert:
            CertDeleteRtNodeL(aLuid, aUri);
            break;
        case EDmAdUriTypeCertReq:
            CertReqDeleteRtNodeL(aLuid, aUri);
            break;
        case EDmAdUriTypePrivKey:
            PrivKeyDeleteRtNodeL(aLuid, aUri);
            break;
        case EDmAdUriTypePKCS12:
            // For PKCS#12, a special case
            break;
        default:
            DEBUG_LOG(_L("Unexcepted URI type"));
            User::Leave(KErrGeneral);
            break;
        }
    }

//=========================================================================
//=========================================================================

TBool CDmAdStore::CertFindRtNodeL(const TDesC8& aLuid, const TDesC8& /*aUri*/)
    {
    TRACE("CDmAdStore::CertFindRtNodeL");
    
    TBool ret = EFalse;
    if (aLuid.Length() > 0)
        {
        if (iCert->FindL(aLuid))
            {
            ret = ETrue;
            }
        }
    return ret;
    }

void CDmAdStore::CertLuidListL(const TDesC8& /*aUri*/, const TDesC8& /*aLuid*/, RPointerArray<HBufC8>& aLuidList)
    {
    TRACE("CDmAdStore::CertLuidListL");
    
    iCert->ListL(aLuidList);
    }
    
void CDmAdStore::CertFetchRtNodeL(CDmAdRtNode& aRtNode)
    {
    TRACE("CDmAdStore::CertFetchRtNodeL");
    
    TPtrC8 luid(aRtNode.Luid());
    if (luid.Length() == 0)
        {
        DEBUG_LOG(_L("Zero length luid"));
        User::Leave(KErrNotFound);
        }

    CDmAdCertData* data = aRtNode.Data()->CertData();
    CDmAdCertParms* certParms = CDmAdCertParms::NewL();
    CleanupStack::PushL(certParms);
    iCert->FetchL(luid, *certParms);
    data->RestoreL(*certParms);
    CleanupStack::PopAndDestroy(certParms);
    }
    
void CDmAdStore::CertSaveTopLevelRtNodeL(CDmAdRtNode& aRtNode, CArrayPtr<CDmAdRtNode>& aChildRtNodes)
    {
    TRACE("CDmAdStore::CertSaveTopLevelRtNodeL");
    
    if (aChildRtNodes.Count() > 0)
        {
        DEBUG_LOG(_L("ChildRtNode count > 0"));        
        User::Leave(KErrGeneral);
        }

    if (aRtNode.IsJustFetched())
        {
        return;
        }
    
    CDmAdCertData* data = aRtNode.Data()->CertData();
    
    CDmAdCertParms* certParms = CDmAdCertParms::NewL();
    CleanupStack::PushL(certParms);
    
    data->ToStoreL(*certParms);

    if (aRtNode.IsSomeLeafAddedToRtNode())
        {
        HBufC8* luid = iCert->AddL(*certParms);
        CleanupStack::PushL(luid);
        aRtNode.SetLuidL(*luid);
        CleanupStack::PopAndDestroy(luid); 
        }
    else
        {
        iCert->UpdateL(aRtNode.Luid(), *certParms);
        }
    
    CleanupStack::PopAndDestroy(certParms);
    }

void CDmAdStore::CertSaveChildLevelRtNodeL(CDmAdRtNode& /*aRtNode*/)
    {
    TRACE("CDmAdStore::CertSaveChildLevelRtNodeL");
    
    DEBUG_LOG(_L("Method not implemented"));
    User::Leave(KErrGeneral);
    }

void CDmAdStore::CertDeleteRtNodeL(const TDesC8& aLuid, const TDesC8& /*aUri*/)
    {
    TRACE("CDmAdStore::CertDeleteRtNodeL");
    
    if (aLuid.Length() == 0)
        {
        DEBUG_LOG(_L("aLuid length is 0"));
        User::Leave(KErrNotFound);
        }

    iCert->DeleteL(aLuid);
    }

//=========================================================================
//=========================================================================

TBool CDmAdStore::CertReqFindRtNodeL(const TDesC8& aLuid, const TDesC8& /*aUri*/)
    {
    TRACE("CDmAdStore::CertReqFindRtNodeL");
    
    TBool ret = EFalse;
    if (aLuid.Length() > 0)
        {
        if (iCertReq->FindL(aLuid))
            {
            ret = ETrue;
            }
        }
    return ret;
    }

void CDmAdStore::CertReqLuidListL(const TDesC8& /*aUri*/, const TDesC8& /*aLuid*/,RPointerArray<HBufC8>& aLuidList)
    {
    TRACE("CDmAdStore::CertReqLuidListL");
    
    iCertReq->ListL(aLuidList);
    }
    
void CDmAdStore::CertReqFetchRtNodeL(CDmAdRtNode& aRtNode)
    {
    TRACE("CDmAdStore::CertReqFetchRtNodeL");
    
    TPtrC8 luid(aRtNode.Luid());
    if (luid.Length() == 0)
        {
        DEBUG_LOG(_L("luid length is 0"));
        User::Leave(KErrNotFound);
        }

    CDmAdCertReqData* data = aRtNode.Data()->CertReqData();
    CDmAdCertReqParms* CertReqParms = CDmAdCertReqParms::NewL();
    CleanupStack::PushL(CertReqParms);
    iCertReq->FetchL(luid, *CertReqParms);
    data->RestoreL(*CertReqParms);
    CleanupStack::PopAndDestroy(CertReqParms);
    }
    
void CDmAdStore::CertReqSaveTopLevelRtNodeL(CDmAdRtNode& aRtNode, CArrayPtr<CDmAdRtNode>& aChildRtNodes)
    {
    TRACE("CDmAdStore::CertReqSaveTopLevelRtNodeL");
    
    if (aChildRtNodes.Count() > 0)
        {
        DEBUG_LOG(_L("aChildRtNodes count > 0"));
        User::Leave(KErrGeneral);
        }

    if (aRtNode.IsJustFetched())
        {
        return;
        }
    
    CDmAdCertReqData* data = aRtNode.Data()->CertReqData();
    
    CDmAdCertReqParms* CertReqParms = CDmAdCertReqParms::NewL();
    CleanupStack::PushL(CertReqParms);
    
    data->ToStoreL(*CertReqParms);

    if (aRtNode.IsSomeLeafAddedToRtNode())
        {
        HBufC8* luid = iCertReq->AddL(*CertReqParms);
        CleanupStack::PushL(luid);
        aRtNode.SetLuidL(*luid);
        CleanupStack::PopAndDestroy(luid);
        }
    else
        {
        DEBUG_LOG(_L("aRtNode.IsSomeLeafAddedToRtNode() returned false"));
        User::Leave(KErrGeneral);
        /*
        //iCertReq->UpdateL(aRtNode.Luid(), *CertReqParms);
        CertReqDeleteRtNodeL(aRtNode.Luid(), aRtNode.Uri());
        
        HBufC8* luid = iCertReq->AddL(*CertReqParms);
        CleanupStack::PushL(luid);
        aRtNode.SetLuidL(*luid);
        iCallBack->SetMappingL(aRtNode.Uri(), aRtNode.Luid());
        CleanupStack::PopAndDestroy(); //luid
        */
        }
    
    CleanupStack::PopAndDestroy(CertReqParms);
    }

void CDmAdStore::CertReqSaveChildLevelRtNodeL(CDmAdRtNode& /*aRtNode*/)
    {
    TRACE("CDmAdStore::CertReqSaveChildLevelRtNodeL");
    
    DEBUG_LOG(_L("Method not implemented"));
    User::Leave(KErrGeneral);
    }

void CDmAdStore::CertReqDeleteRtNodeL(const TDesC8& aLuid, const TDesC8& /*aUri*/)
    {
    TRACE("CDmAdStore::CertReqDeleteRtNodeL");
    
    if (aLuid.Length() == 0)
        {
        DEBUG_LOG(_L("aLuid length is 0"));        
        User::Leave(KErrNotFound);
        }

    iCertReq->DeleteL(aLuid);
    }

//=========================================================================
//=========================================================================

TBool CDmAdStore::PrivKeyFindRtNodeL(const TDesC8& aLuid, const TDesC8& /*aUri*/)
    {
    TRACE("CDmAdStore::PrivKeyFindRtNodeL");
    
    TBool ret = EFalse;
    if (aLuid.Length() > 0)
        {
        if (iPrivKey->FindL(aLuid))
            {
            ret = ETrue;
            }
        }
    return ret;
    }

void CDmAdStore::PrivKeyLuidListL(const TDesC8& /*aUri*/, const TDesC8& /*aLuid*/, RPointerArray<HBufC8>& aLuidList)
    {
    TRACE("CDmAdStore::PrivKeyLuidListL");
    iPrivKey->ListL(aLuidList);
    }
    
void CDmAdStore::PrivKeyFetchRtNodeL(CDmAdRtNode& aRtNode)
    {
    TRACE("CDmAdStore::PrivKeyFetchRtNodeL");
    
    TPtrC8 luid(aRtNode.Luid());
    if (luid.Length() == 0)
        {
        DEBUG_LOG(_L("aLuid length is 0"));        
        User::Leave(KErrNotFound);
        }

    CDmAdPrivKeyData* data = aRtNode.Data()->PrivKeyData();
    CDmAdPrivKeyParms* PrivKeyParms = CDmAdPrivKeyParms::NewL();
    CleanupStack::PushL(PrivKeyParms);
    iPrivKey->FetchL(luid, *PrivKeyParms);
    data->RestoreL(*PrivKeyParms);
    CleanupStack::PopAndDestroy(PrivKeyParms);
    }
    

void CDmAdStore::PrivKeyDeleteRtNodeL(const TDesC8& aLuid, const TDesC8& /*aUri*/)
    {
    TRACE("CDmAdStore::PrivKeyDeleteRtNodeL");
    
    if (aLuid.Length() == 0)
        {
        DEBUG_LOG(_L("aLuid length is 0"));        
        User::Leave(KErrNotFound);
        }

    iPrivKey->DeleteL(aLuid);
    }

//=========================================================================
//=========================================================================

// PKCS#12 handling
void CDmAdStore::PKCS12SaveTopLevelRtNodeL(CDmAdRtNode& aRtNode,
                               CArrayPtr<CDmAdRtNode>& aChildRtNodes) 
    {
    TRACE("CDmAdStore::PKCS12SaveTopLevelRtNodeL");

    if (aChildRtNodes.Count() > 0)
        {
        DEBUG_LOG(_L("ChildRtNode count > 0"));        
        User::Leave(KErrGeneral);
        }

    CDmAdPKCS12Data* data = aRtNode.Data()->PKCS12Data();
    
    CDmAdPKCS12Parms* pkcs12Parms = CDmAdPKCS12Parms::NewL();
    CleanupStack::PushL(pkcs12Parms);
    
    // Extract parameters from data
    data->ToStoreL(*pkcs12Parms);

    // Store PKCS#12 object contents to PKI
    iPKCS12Obj->AddL(*pkcs12Parms);

    TPtrC8 uri = aRtNode.Uri();
    DEBUG_LOG1(_L8(" Setting LUID to '%S'"), &uri);
    aRtNode.SetLuidL(uri);
    
    CleanupStack::PopAndDestroy(pkcs12Parms);
    }

//=========================================================================
//=========================================================================
void CDmAdStore::PkiLogonL()
    {
    TRACE("CDmAdStore::PkiLogonL");
    
    TRequestStatus requestStatus;
    iPkiServiceApi.Logon(requestStatus);
    User::WaitForRequest(requestStatus);
    TInt status = requestStatus.Int();
    if (status != KErrNone && status != KPKIErrKeyStoreEmpty)
        {
        DEBUG_LOG1(_L("status = %d"), status);        
        User::Leave(status);
        }
    }

void CDmAdStore::PkiLogoffL()
    {
    TRACE("CDmAdStore::PkiLogoffL");
    
    TRequestStatus requestStatus;
    iPkiServiceApi.Logoff(requestStatus);
    User::WaitForRequest(requestStatus);
    TInt status = requestStatus.Int();
    if (status != KErrNone && status != KPKIErrKeyStoreEmpty)
        {
        DEBUG_LOG1(_L("status = %d"), status);        
        User::Leave(status);
        }
    }

void CDmAdStore::SetStoreTypeL(TInt aStore, TPkiServiceStoreType aStoreType)
    {
    TRACE("CDmAdStore::SetStoreTypeL");
    
    TInt status = iPkiServiceApi.SetStoreType(aStore, aStoreType);
    if (status != KErrNone)
        {
        DEBUG_LOG1(_L("status = %d"), status);        
        User::Leave(status);
        }
    }
