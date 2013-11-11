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
* Description:   Class definition of CDmAdStore
*
*/



#ifndef __DMADSTORE_H__
#define __DMADSTORE_H__

#include <e32base.h>

#include "dmadstoreapi.h"
#include "DmAdRtNode.h"

#include "dmadstorecert.h"
#include "dmadstorecertreq.h"
#include "dmadstoreprivkey.h"
#include "dmadstorepkcs12.h"

//------------------------------------------------------------------------------------------------
// CDmAdStore
//------------------------------------------------------------------------------------------------
class CDmAdStore : public CBase, public MDmAdStoreApi
    {
public:
    static CDmAdStore* NewL(MDmAdCallBack* aDmAdCallBack);
    ~CDmAdStore();
    TBool FindRtNodeL(const TDesC8& aLuid, const TDesC8& aUri);
    void LuidListL(const TDesC8& aUri, const TDesC8& aLuid, RPointerArray<HBufC8>& aLuidList);
    void FetchRtNodeL(CDmAdRtNode& aRtNode);
    void SaveTopLevelRtNodeL(CDmAdRtNode& aRtNode, CArrayPtr<CDmAdRtNode>& aChildRtNodes);
    void SaveChildLevelRtNodeL(CDmAdRtNode& aRtNode);
    void DeleteRtNodeL(const TDesC8& aLuid, const TDesC8& aUri);

    void PkiLogonL();
    void PkiLogoffL();
    void SetStoreTypeL(TInt aStore, TPkiServiceStoreType aStoreType);

private:
    void ConstructL(MDmAdCallBack* aDmAdCallBack);
    CDmAdStore();

    TBool CertFindRtNodeL(const TDesC8& aLuid, const TDesC8& aUri);
    void CertLuidListL(const TDesC8& aUri, const TDesC8& aLuid, RPointerArray<HBufC8>& aLuidList);
    void CertFetchRtNodeL(CDmAdRtNode& aRtNode);
    void CertSaveTopLevelRtNodeL(CDmAdRtNode& aRtNode, CArrayPtr<CDmAdRtNode>& aChildRtNodes);
    void CertSaveChildLevelRtNodeL(CDmAdRtNode& aRtNode);
    void CertDeleteRtNodeL(const TDesC8& aLuid, const TDesC8& aUri);
    
    TBool CertReqFindRtNodeL(const TDesC8& aLuid, const TDesC8& aUri);
    void CertReqLuidListL(const TDesC8& aUri, const TDesC8& aLuid, RPointerArray<HBufC8>& aLuidList);
    void CertReqFetchRtNodeL(CDmAdRtNode& aRtNode);
    void CertReqSaveTopLevelRtNodeL(CDmAdRtNode& aRtNode, CArrayPtr<CDmAdRtNode>& aChildRtNodes);
    void CertReqSaveChildLevelRtNodeL(CDmAdRtNode& aRtNode);
    void CertReqDeleteRtNodeL(const TDesC8& aLuid, const TDesC8& aUri);
    
    TBool PrivKeyFindRtNodeL(const TDesC8& aLuid, const TDesC8& aUri);
    void PrivKeyLuidListL(const TDesC8& aUri, const TDesC8& aLuid, RPointerArray<HBufC8>& aLuidList);
    void PrivKeyFetchRtNodeL(CDmAdRtNode& aRtNode);
    void PrivKeyDeleteRtNodeL(const TDesC8& aLuid, const TDesC8& aUri);
    
    TBool PKCS12FindRtNodeL(const TDesC8& aLuid, const TDesC8& aUri);
    void PKCS12LuidListL(const TDesC8& aUri, const TDesC8& aLuid, RPointerArray<HBufC8>& aLuidList);
    void PKCS12SaveTopLevelRtNodeL(CDmAdRtNode& aRtNode, CArrayPtr<CDmAdRtNode>& aChildRtNodes);

private:
    CDmAdCert*      iCert;
    CDmAdCertReq*   iCertReq;
    CDmAdPrivKey*   iPrivKey;
    CDmAdPKCS12*    iPKCS12Obj;
    
    RPKIServiceAPI  iPkiServiceApi;
    };

#endif
