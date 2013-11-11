/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   PKI definitions module
*
*/



#ifndef __PKIDEFS_H__
#define __PKIDEFS_H__

#include <x509certext.h>

const TInt MAX_FILENAME_LENGTH = 64;
const TInt SHORT_FILENAME_LENGTH = 64;

const TUint32 KMaxX500DN = 256;
const TUint32 KMaxRfc822 = 64;
const TUint32 KMaxUsageDer = 16;
const TUint32 KMaxSerial = 128;


// Certificate / Key Store type identifiers
enum TPkiServiceStoreType
    {
    EPkiStoreTypeAny       = 0,
    EPkiStoreTypeUser      = 0x01,
    EPkiStoreTypeDevice    = 0x10    
    };

// Certificate / Key Store identifiers
const TInt STORE_KEYSTORE = 1;
const TInt STORE_CERTSTORE = 2;



/**
* Definitions used by PKI Service API
*/
const TInt KPKISHA1HashLengthBytes = 20;
typedef  TBuf8<KPKISHA1HashLengthBytes> TPKISHA1Hash;

typedef TPKISHA1Hash TPKIKeyIdentifier;

// PKI Service API error codes
// NOTE! The error code values below MUST be kept in sync with
// the corresponding error code values defined together by
// vpnapi/data/vpnerr.rss and vpnapi/data/vpnerr.ra
const TInt KPKIErrWrongObjectType           = -5240;
const TInt KPKIErrObjectUninitialized       = -5241;
const TInt KPKIErrPassword                  = -5242;
const TInt KPKIErrServiceBusy               = -5243;
const TInt KPKIErrCancel                    = -5244;
const TInt KPKIErrBufferTooShort            = -5245;
const TInt KPKIErrCertRequestParam          = -5246;
const TInt KPKIErrCertRequest               = -5247;
const TInt KPKIErrNotFound                  = -5248;
const TInt KPKIErrAmbiguous                 = -5249;
const TInt KPKIErrNotSupported              = -5250;
const TInt KPKIErrUnexpectedState           = -5251;
const TInt KPKIErrKeyStoreEmpty             = -5252;


enum TPKICertificateOwnerType
{
    EPKICACertificate,
    EPKIUserCertificate,
    EPKIPeerCertificate
};


enum TPKIKeyAlgorithm
{
    EPKIInvalidAlgorithm    = 0,
    EPKIRSA                 = 1,
    EPKIDSA                 = 2,
    EPKIDH                  = 3
};


typedef TX509KeyUsage TPKIKeyUsage;
   


class TCertificateRequestListEntry
{
    public:
        TBuf<SHORT_FILENAME_LENGTH> iObjectName;
};


class TCertificateListEntry
{
	public:
        TPKICertificateOwnerType    iOwnerType;                 // User, CA or peer. If user certificate, at least key usage must be set
        TBuf8<KMaxX500DN>           iTrustedAuthority;          // Cert TrustedAuthority
        TBuf8<KMaxX500DN>           iIdentitySubjectName;       // Identity subject name
        TBuf8<KMaxSerial>           iSerialNumber;              // Serialnumber
        TPKIKeyIdentifier           iSubjectKeyId;              // SHA1 hash of the corresponding private key
        TPkiServiceStoreType        iStoreType;	
		TBuf<SHORT_FILENAME_LENGTH> iObjectName;
        TUint                       iKeySize;                   // Key size
        TPKIKeyAlgorithm            iKeyAlgorithm;              // RSA, DSA
        TBool                       iIsDeletable;               //
};

class TKeyListEntry
{
    public:
        TBuf<SHORT_FILENAME_LENGTH> iObjectName;
        TPKIKeyIdentifier           iSubjectKeyId;              // SHA1 hash of the corresponding private key
        TUint                       iKeySize;                   // Key size
        TPKIKeyAlgorithm            iKeyAlgorithm;              // RSA, DSA
};

#endif