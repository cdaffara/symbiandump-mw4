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
* Description:   Class definition of CDmAdCert
*
*/



#ifndef __DMADSTORECERT_H__
#define __DMADSTORECERT_H__

#include <e32std.h>

#include <x509cert.h>
//#include <x509keys.h>
#include <x500dn.h>

#include "pkiserviceapi.h"
#include "dmadcertparms.h"
#include "dmadcertluidmappingelem.h"
#include "eventlogger.h"

class CDmAdCert : public CBase
    {
public:
    static CDmAdCert* NewL(RPKIServiceAPI& aPkiServiceApi);
    static CDmAdCert* NewLC(RPKIServiceAPI& aPkiServiceApi);
    ~CDmAdCert();
    
    TBool FindL(const TDesC8& aLuid);
    HBufC8* AddL(const CDmAdCertParms& aData); // returns Luid
    void UpdateL(const TDesC8& aLuid, const CDmAdCertParms& aData);
    void FetchL(const TDesC8& aLuid, CDmAdCertParms& aData);
    void DeleteL(const TDesC8& aLuid);
    void ListL(RPointerArray<HBufC8>& aLuidList);
    
private:
    void ConstructL();
    CDmAdCert(RPKIServiceAPI& aPkiServiceApi);
    void ReadCertDetailsL(const TDesC8& aIssuerName, const TDesC8& aSerialNumber, CDmAdCertParms& aParms);
    void UpdateTrustedL(const TDesC8& aIssuerName, const TDesC8& aSerialNumber, const CDmAdCertParms& aParms);
    void UpdateApplicabilityL(const TDesC8& aIssuerName, const TDesC8& aSerialNumber, const CDmAdCertParms& aParms);
    TPtrC8 AppendCertLuidMappingElemL(const TDesC8& aIssuerName, const TDesC8& aSerialNumber);
    void RemoveCertLuidMappingElem(const TDesC8& aLuid);
    CDmAdCertLuidMappingElem* FindCertLuidMappingElemL(const TDesC8& aLuid);
    void BuildCertLuidMappingTableL();
    HBufC8* BuildCertRefL(const TDesC8& aCertificateDer, TBool aAppend);
    void GetIssuerAndSerialFromCertRefLC(const TDesC8&  aCertRef,
                                         HBufC8*&       aIssuerName,
                                         HBufC8*&       aSerialNumber);
    HBufC8* CertSubjectNameL(const TDesC8& aCertificateDer);                                         
    
private:
    RPKIServiceAPI*                         iPkiServiceApi;
    CArrayPtr<CDmAdCertLuidMappingElem>*    iCertLuidMapping;
    DEFINE_EVENT_LOGGER
    };

#endif
