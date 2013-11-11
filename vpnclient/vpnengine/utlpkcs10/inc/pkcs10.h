/*
* Copyright (c) 2003 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Pkcs10 certificate request.
*
*/



#ifndef _PKCS10_H_
#define _PKCS10_H_
#include "pkidefs.h"

////////////////////////////////////////////////////////////////////////////////////////
//  Definitions
////////////////////////////////////////////////////////////////////////////////////////
#define SHA1_HASH_LTH       20
#define MD5_HASH_LTH        16
#define ASN1_MAX_LENGTH     0xffff
#define ASN1_MAX_INTEGER    0xffff
#define MAX_SIGNATURE_LENGTH 512
#define ASN1_SHORT_TAG_HDR_LTH          2
#define ASN1_LONG_TAG_HDR_LTH           4
#define ASN1_NULL_LTH                   2
#define ASN1_CONSTRUCTED                0x20
#define ASN1_CONTEXT_SPECIFIC           0x80
#define COMMON_NAME_OID_LTH             3
#define KEY_TYPE_RSA_OID_LTH            9
#define SUBJECTALTNAME_EMAIL_OID_LTH    9
#define SUBJECTALTNAME_OID_LTH          3
#define CHALLENGEPW_OID_LTH             9
#define MD5WITHRSA_OID_LTH              9
#define MD5SIGNATURE_OID_LTH            8
#define SHA1WITHRSA_OID_LTH             9
#define PKCS10_EXTENSION_OID_LTH        9
#define DC_OID_LTH                     10

enum THashType {HASH_TYPE_MD5, HASH_TYPE_SHA1};
const TUint8 KCommonNameOid[COMMON_NAME_OID_LTH] = {0x55, 0x04, 0x03};
const TUint8 KEmailAddressOid[SUBJECTALTNAME_EMAIL_OID_LTH] = {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x01};
const TUint8 KSubjectAltNameOid[SUBJECTALTNAME_OID_LTH] = {0x55, 0x1D, 0x11};
const TUint8 KChallengePWOid[CHALLENGEPW_OID_LTH] = { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x07};
const TUint8 KKeyTypeRSAOid[KEY_TYPE_RSA_OID_LTH] = {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01};
const TUint8 KMd5WithRSAEncryptionOid[MD5WITHRSA_OID_LTH] = {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x04};
const TUint8 KMd5SignatureOid[MD5SIGNATURE_OID_LTH] = {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x02, 0x05};
const TUint8 KSha1WithRSAEncryptionOid[SHA1WITHRSA_OID_LTH] = {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x05};
const TUint8 KPkcs10ExtensionOid[PKCS10_EXTENSION_OID_LTH] = {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x0E};

#define HEADERLENGTH    40
#define TRAILERLENGTH   37
#define HEADER _L("-----BEGIN NEW CERTIFICATE REQUEST-----\n")
#define TRAILER _L("\n-----END NEW CERTIFICATE REQUEST-----")
   
#define StoreINTValue(d, s)  \
                             (*(unsigned char*)((unsigned char*)(d)+3)) = (unsigned char)((s) & 0xff);\
                             (*(unsigned char*)((unsigned char*)(d)+2)) = (unsigned char)(((s) >> 8 ) & 0xff);\
                             (*(unsigned char*)((unsigned char*)(d)+1)) = (unsigned char)(((s) >> 16) & 0xff);\
                             (*(unsigned char*)(d))                     = (unsigned char)(((s) >> 24) & 0xff)

typedef TBool (*TSignCallback)(const TDesC8 &aHash, TDes8 &aSig, const TPKIKeyIdentifier &aPrivKey, TAny* aContext);

class TIndexPair
{
    public:
        TUint startIndex;
        TUint endIndex;
};

const TUint8 KCountryNameOid[COMMON_NAME_OID_LTH] = {0x55, 0x04, 0x06};
const TUint8 KOrganizationNameOid[COMMON_NAME_OID_LTH] = {0x55, 0x04, 0x0a};
const TUint8 KOrganizationalUnitNameOid[COMMON_NAME_OID_LTH] = {0x55,0x04, 0x0b};
const TUint8 KLocalityNameOid[COMMON_NAME_OID_LTH]= {0x55, 0x04, 0x07};
const TUint8 KStateOrProvinceNameOid[COMMON_NAME_OID_LTH] = {0x55, 0x04, 0x08};
const TUint8 KTitleOid[COMMON_NAME_OID_LTH] = {0x55, 0x04, 0x0c};
const TUint8 KCommonNOid[COMMON_NAME_OID_LTH] = {0x55, 0x04, 0x03};
const TUint8 KGivenNameOid[COMMON_NAME_OID_LTH] = {0x55, 0x04, 0x2a};
const TUint8 KSurnameOid[COMMON_NAME_OID_LTH] = {0x55, 0x04, 0x04};
const TUint8 KInitialsOid[COMMON_NAME_OID_LTH] = {0x55, 0x04, 0x2b};
const TUint8 KGenerationQualifierOid[COMMON_NAME_OID_LTH] = {0x55, 0x04, 0x2c};
const TUint8 KDNQualifierOid[COMMON_NAME_OID_LTH] = {0x55, 0x04, 0x2e};
const TUint8 KSerialNumberOid[COMMON_NAME_OID_LTH] = {0x55, 0x04, 0x05};
const TUint8 KDomainComponentOid[DC_OID_LTH] = {0x09, 0x92, 0x26, 0x89, 0x93, 0xF2, 0x2C, 0x64, 0x01, 0x19};

const TUint KNullAttributeIndex          =  0;
const TUint KCountryNameIndex            =  1;
const TUint KOrganizationNameIndex       =  2;
const TUint KOrganizationalUnitNameIndex =  3;
const TUint KLocalityNameIndex           =  4;
const TUint KStateOrProvinceNameIndex    =  5;
const TUint KTitleIndex                  =  6;
const TUint KCommonNameIndex             =  7;
const TUint KGivenNameIndex              =  8;
const TUint KSurnameIndex                =  9;
const TUint KInitialsIndex               = 10;
const TUint KGenerationQualifierIndex    = 11;
const TUint KDNQualifierIndex            = 12;
const TUint KSerialNumberIndex           = 13;
const TUint KDomainComponentIndex        = 14;

_LIT8(KCountryName, "C");
_LIT8(KOrganizationName, "O");
_LIT8(KOrganizationalUnitName, "OU");
_LIT8(KLocalityName, "L"); 
_LIT8(KStateOrProvinceName, "ST");
_LIT8(KTitle, "TITLE");      
_LIT8(KCommonName, "CN");    
_LIT8(KGivenName, "GIVENNAME");
_LIT8(KSurname, "SN");   
_LIT8(KInitials, "INITIALS");
_LIT8(KGenerationQualifier, "GENERATIONQUALIFIER");
_LIT8(KDNQualifier, "DNQUALIFIER");
_LIT8(KSerialNumber, "SERIALNUMBER");
_LIT8(KDomainComponent, "DC");

_LIT8(KDot, ".");

//////////////////////////////////////////////////////////////////////////////
//  Class CPkcs10Req
//////////////////////////////////////////////////////////////////////////////
class CPkcs10Req : public CBase
{
    public:
        /** Cause vtable & typeinfo to be exported */
        IMPORT_C CPkcs10Req();
        IMPORT_C ~CPkcs10Req();
        //////////////////////////////////////////////////////////////////////
        //  Visible interface
        //////////////////////////////////////////////////////////////////////
        //  Setting parameters for PKCS#10 request
        //////////////////////////////////////////////////////////////////////
        IMPORT_C TInt SetDistinguishedName(const TDesC8 &aDistinguishedName);
        IMPORT_C TInt SetSubjectAltNameRfc822(const TDesC8 &aSubjectAltNameRfc822);
        IMPORT_C TInt SetChallengePw(const TDesC8 &aChallengePw);
        IMPORT_C TInt SetDNSName(const TDesC8 &aDNSName);
        IMPORT_C TInt SetPublicKey(const TDesC8 &aPublicKeyBits);
        IMPORT_C TInt SetDistinguishedNameExtendedL(const TDesC8 &aDistinguishedName);
        //////////////////////////////////////////////////////////////////////
        // Create PKCS#10 request
        //////////////////////////////////////////////////////////////////////
        IMPORT_C HBufC8* CreateCertificateRequestLC(THashType aSignatureAlgorithmId, const TPKIKeyIdentifier& aPrivateKeyFilename, TBool aDoBase64, TBool aDoPEM, TSignCallback aSignCallBack, TAny* aSignCallBackContext);
        // Static functions
        IMPORT_C static void BuildDistinguishedNameDerFromTextL(HBufC8 *&derBuffer, const TDesC8 &aDistinguishedName, TBool aByPass, const TDesC8 &aByPassType);
        IMPORT_C static TPtrC8 AttributeTypeOidToText(const TDesC &aType);
        static TInt ASNCopy(TUint8 *dataPtr, TUint8 *valuePtr, TInt length);
        static void BuildIndexPairsLC(HBufC8 *&buffer, const TDesC8 &aDistinguishedName, CArrayFixFlat<TIndexPair> *aTypeIndexArray, CArrayFixFlat<TIndexPair> *aValueIndexArray);
        static void BuildDerL(HBufC8 *aSource, HBufC8 *&derBuffer, CArrayFixFlat<TIndexPair> *aTypeIndexArray, CArrayFixFlat<TIndexPair> *aValueIndexArray, CArrayFixFlat<TPtr8> *aTypePtrArray, CArrayFixFlat<TPtr8> *aValuePtrArray, TBool aByPass, const TDesC8 &aByPassType);
        static TBool OidExistsL(TPtr8 &aTypePtrArray, TUint &aIndex, TBuf8<32> &aDottedOidDer);
        static TInt GetOid(TUint aIndex, const TUint8 *&aValue);
        static void BuildElementDerLC(CArrayFixFlat<HBufC8 *> *aElementArray, TUint aWholeLength, TUint aSetLength, TUint aSeqLength, TUint aOidLength, TUint aStringLength, TUint aOidIndex, TBuf8<32> &aDottedOidDerPtr, TPtr8 &aValuePtr);
        static TInt    DERSetLength(TUint8 *p, TInt datalen);
        static TInt    DERSetInteger(TUint8 *p, TInt aValue);
        static inline TInt DERSetTag(TUint8 *p, TUint8 aTag) { *p = aTag; return 1;};
        static inline TInt DERSetOid(TUint8 *p, const unsigned char *oid, TInt datalen) {Mem::Copy(p, oid, datalen); return datalen;};
    private:
        ////////////////////////////////////////////////////////////////////////////////////////
        // Class variables
        ////////////////////////////////////////////////////////////////////////////////////////
        TPtrC8 iDistinguishedName;
        TPtrC8 iSubjectAltName;
        TPtrC8 iChallengePW;
        TPtrC8 iDNSName;
        TPtrC8 iPublicKey;
        THashType iSignatureAlgorithmId;
        TBool   iUseExtendedDistinguishedName;
        HBufC8 *iDNDerBuf;
        ////////////////////////////////////////////////////////////////////////////////////////
        // Internal functions
        ////////////////////////////////////////////////////////////////////////////////////////
        void MD5_HashL(const TDesC8 &aInData, TDes8& aOutData);
        void SHA1_HashL(const TDesC8 &aInData, TDes8& aOutData);
        void DigestL(const TDesC8& aData, TDes8& aDigest, THashType aHashType);
        HBufC8* ASNEncodeCertificationRequestInfoLC();
        HBufC8* ASNEncodeCertificationRequestLC(const TDes8 &CRLInfoBuffer, const TDes8 &signature);
        HBufC8* ASNEncodeCommonAttributesLC();
        HBufC8* ASNEncodePKIinfoLC();
        HBufC8* ASNEncodeExtendedAttributesLC();
};

#endif

