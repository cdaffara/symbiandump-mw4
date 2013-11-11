/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __PKIUTIL_H__
#define __PKIUTIL_H__

#include <e32std.h>
#include "pkidefs.h"
#include "ikepolparser.h"
#include "fileutil.h"

const TInt KCertDnSizeIncrement = 64;

_LIT(KComma,    ",");
_LIT(KEquals,   "=");

_LIT(KC,    "C");
_LIT(KO,    "O");
_LIT(KOU,   "OU");
_LIT(KL,    "L");
_LIT(KST,   "ST");
_LIT(KCN,   "CN");

_LIT8(KCN8,   "CN");

const TInt KExpectedMaxCertSize = 4092;

class CX500DistinguishedName;
class CX520AttributeTypeAndValue;
class CX509Certificate;
class RPKIServiceAPI;
class CDesC16ArrayFlat;
class TCertInfo;


enum TCertStatus
    {
    ECertValid = 1,
    ECertNotValidYet,
    ECertExpired,
    ECertNotFound,
    ECertNotNeeded,
    ECertStatusUnknown
    };

/**
 * PKI-related static utility methods
 */
class PkiUtil
    {
public:
    static HBufC* CertSubjectNameL(const TDesC8& aCertData);
    static HBufC* CertIssuerNameL(const TDesC8& aCertData);
    static TCertStatus CertStatusL(RPKIServiceAPI& aPkiService, const TDesC8& aTrustedCaDn,
                                   const TDesC8& aSubjectDnSuffix, const TDesC8& aRfc822Name,
                                   TUint aPrivKeyLength, TInt aCertRenewalThreshold = -1);

    static  TCertStatus GetValidCaCertSubjectNameListL(RPKIServiceAPI& aPkiService,
                                                       const CArrayFixFlat<TCertInfo*>& aIkeCAList,
                                                       CDesC8ArrayFlat& aCaCertNameList);

    static TCertStatus CheckUserCertValidityL(RPKIServiceAPI& aPkiService,
                                              CDesC8ArrayFlat& aValidCaCertNameList,
                                              TOwnCertInfo& aOwnCert);


    static CX500DistinguishedName* DnFromStringL(const TDesC8& aString);
    static CX500DistinguishedName* DnWithoutCnFromStringL(const TDesC8& aString);
    static TBool DnMatchL(const TDesC8& aDnString1, const TDesC8& aDnString2);

    static TBool MatchL(const CX500DistinguishedName& aDn1,
                        const CX500DistinguishedName& aDn2);
    static HBufC* CertDnL(const CX500DistinguishedName& aName);

    static TInt CertKeySizeL(const TDesC8& aCertData);

private:

    static TCertStatus CertStatusL(const TDesC8& aCertData, TInt aCertRenewalThreshold = -1);
    static TCertStatus CertStatusL(const CX509Certificate& aCert, TInt aCertRenewalThreshold = -1);


    static void AppendAttributeL(HBufC*& aBuf, const CX520AttributeTypeAndValue& aAttribute);
    static HBufC* AttributeTypeToNameL(const TDesC &aType);
    static void SmartAppendL(HBufC*& aBuf, const TDesC& aText);
    static TBool HasElementL(const CX500DistinguishedName& aDn,
                             const CX520AttributeTypeAndValue& aElement);

    /**
     * Returns the list of validated CA certificate Subject names.
     * If some of the certificates in aIkeCAList doesn't match to any CA certificates
     * stored in the phone LEAVES with KErrNotFound. If some of the elements in the aIkeCAList
     * has iFormat value PEM_CERT or BIN_CERT this function panics. If aIkeCAList->Count < 1 this
     * method panics.
     */
    static RPointerArray<CX509Certificate> GetCaCertListL(RPKIServiceAPI& aPkiService,
                                                          const CArrayFixFlat<TCertInfo*>& aIkeCAList);


    static CX509Certificate* ReadCertificateLC(RPKIServiceAPI& aPkiService,
                                               const TDesC8& aTrustedAuthority,
                                               const TDesC8& aIdentitySubjectName,
                                               const TDesC8& aIdentityRfc822Name,
                                               const TPKICertificateOwnerType aOwnerType);

    static CX509Certificate* ReadCertificateLC(RPKIServiceAPI& aPkiService,
                                               const TPKIKeyIdentifier& aKeyId);
    static HBufC8* PkiUtil::To8BitL(const TDesC16& aDes);

    };

#endif // __PKIUTIL_H__