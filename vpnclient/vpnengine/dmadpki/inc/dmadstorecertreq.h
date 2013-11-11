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
* Description: Class definition of CDmAdCertReq.
*
*/



#ifndef __DMADSTORECERTREQ_H__
#define __DMADSTORECERTREQ_H__

#include <e32std.h>
#include <x509cert.h>

#include "pkiserviceapi.h"
#include "dmadcertreqparms.h"

class MDmAdCallBack;

class CDmAdCertReq : public CBase
    {
public:
    static CDmAdCertReq* NewL(RPKIServiceAPI& aPkiServiceApi, MDmAdCallBack* aDmAdCallBack);
    static CDmAdCertReq* NewLC(RPKIServiceAPI& aPkiServiceApi, MDmAdCallBack* aDmAdCallBack);
    ~CDmAdCertReq();
    
    TBool FindL(const TDesC8& aLuid);
    HBufC8* AddL(const CDmAdCertReqParms& aData); // returns Luid
    void UpdateL(const TDesC8& aLuid, const CDmAdCertReqParms& aData);
    void FetchL(const TDesC8& aLuid, CDmAdCertReqParms& aData);
    void DeleteL(const TDesC8& aLuid);
    void ListL(RPointerArray<HBufC8>& aLuidList);
    
private:
    CDmAdCertReq(RPKIServiceAPI& aPkiServiceApi, MDmAdCallBack* aDmAdCallBack);
    void CreateKeypairL(TPKIKeyAlgorithm      aKeyAlgorithm,
                        TInt                  aKeyLength,
                        TPKIKeyIdentifier&    aKeyIdentifier);
    HBufC8* CertReqRefToLuidL(const TDesC& aCertReqRef);
    HBufC* LuidToCertReqRefL(const TDesC8& aLuid);
    void SetKeyIDMappingL(const TPKIKeyIdentifier& keyIdentifier);
    
private:
    RPKIServiceAPI* iPkiServiceApi;
    MDmAdCallBack* iDmAdCallBack;
    };

#endif
