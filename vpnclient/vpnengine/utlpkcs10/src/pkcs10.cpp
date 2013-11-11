/*
* Copyright (c) 2003 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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



#include <eikenv.h>
#include <asn1cons.h>
#include <asn1enc.h>
#include <x520ava.h>

#include "pkcs10.h"
#include "base64.h"
#include "pkidefs.h"
#include "utlcrypto.h"

EXPORT_C CPkcs10Req::CPkcs10Req()
	{
	}

//////////////////////////////////////////////////////////////////////////////
// Destructor
//////////////////////////////////////////////////////////////////////////////
EXPORT_C CPkcs10Req::~CPkcs10Req()
{
    delete iDNDerBuf;
}

//////////////////////////////////////////////////////////////////////////////
// Set distinguished name parameter, Only CommonName given
//////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CPkcs10Req::SetDistinguishedName(const TDesC8 &aDistinguishedName)
{
    TInt status = KErrNone;

    // Set distiguished name
    if(aDistinguishedName.Length() > 0)
    {
        iDistinguishedName.Set(aDistinguishedName);
        iUseExtendedDistinguishedName = EFalse;
    }
    else
        status = KErrArgument;

    return (status);
}

//////////////////////////////////////////////////////////////////////////////
// SetDistinguishedNameExtended
//////////////////////////////////////////////////////////////////////////////
// Set distinguished name parameter. Complete distinguished name in format
// CN=xxx,O=abc,OU=def or CN=xxx/O=abc/OU=def
// OIds and corresponding texts (case insensitive):
// KX520CountryName             "2.5.4.6"       "c"                     {0x55, 0x04, 0x06}
// KX520OrganizationName        "2.5.4.10"      "o"                     {0x55, 0x04, 0x0a}
// KX520OrganizationalUnitName  "2.5.4.11"      "ou"                    {0x55, 0x04, 0x0b}
// KX520LocalityName            "2.5.4.7"       "l"                     {0x55, 0x04, 0x07}
// KX520StateOrProvinceName     "2.5.4.8"       "st"                    {0x55, 0x04, 0x08}
// KX520Title                   "2.5.4.12"      "title"                 {0x55, 0x04, 0x0c}
// KX520CommonName              "2.5.4.3"       "cn"                    {0x55, 0x04, 0x03}
// KX520GivenName               "2.5.4.42"      "GivenName"             {0x55, 0x04, 0x2a}
// KX520Surname                 "2.5.4.4"       "sn"                    {0x55, 0x04, 0x04}
// KX520Initials                "2.5.4.43"      "initials"              {0x55, 0x04, 0x2b}
// KX520GenerationQualifier     "2.5.4.44"      "generationQualifier"   {0x55, 0x04, 0x2c}
// KX520DNQualifier             "2.5.4.46"      "dnQualifier"           {0x55, 0x04, 0x2e}
// KX520SerialNumber            "2.5.4.5"       "serialNumber"          {0x55, 0x04, 0x05}
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CPkcs10Req::SetDistinguishedNameExtendedL(const TDesC8 &aDistinguishedName)
{
    TInt status = KErrNone;

    // Set distiguished name
    if(aDistinguishedName.Length() > 0)
    {
        delete iDNDerBuf;       // delete possible old buffer
        iDNDerBuf = NULL;
        TRAPD(status, BuildDistinguishedNameDerFromTextL(iDNDerBuf, aDistinguishedName, EFalse, KCountryName()));
        if(status == KErrNone)
        {
            iUseExtendedDistinguishedName = ETrue;
        }
    }
    else
        status = KErrArgument;

    return (status);
}

//////////////////////////////////////////////////////////////////////////////
// BuildDistinguishedNameDerFromText
//////////////////////////////////////////////////////////////////////////////
EXPORT_C void CPkcs10Req::BuildDistinguishedNameDerFromTextL(
    HBufC8        *&derBuffer,
    const TDesC8   &aDistinguishedName,
    TBool           aByPass,
    const TDesC8   &aByPassType
)
{    
    HBufC8 *shrinkedBuffer;
    
    CArrayFixFlat<TIndexPair> *valueIndexArray  =
      new (ELeave) CArrayFixFlat<TIndexPair>(1);
    
    CleanupStack::PushL(valueIndexArray);
    
    CArrayFixFlat<TIndexPair> *typeIndexArray  =
      new (ELeave) CArrayFixFlat<TIndexPair>(1);
    
    CleanupStack::PushL(typeIndexArray);
    
    CArrayFixFlat<TPtr8> *valuePtrArray  =
      new (ELeave) CArrayFixFlat<TPtr8>(1);
    
    CleanupStack::PushL(valuePtrArray);
    
    CArrayFixFlat<TPtr8> *typePtrArray  =
      new (ELeave) CArrayFixFlat<TPtr8>(1);
    
    CleanupStack::PushL(typePtrArray);

    // Scan input and build index arrays for types and values
    CPkcs10Req::BuildIndexPairsLC(
        shrinkedBuffer,
        aDistinguishedName,
        typeIndexArray,
        valueIndexArray
    );
    
    CPkcs10Req::BuildDerL(
        shrinkedBuffer,
        derBuffer,
        typeIndexArray,
        valueIndexArray,
        typePtrArray,
        valuePtrArray,
        aByPass,
        aByPassType
    );
    
    CleanupStack::PopAndDestroy(5);    
}

//////////////////////////////////////////////////////////////////////////////
// AttributeTypeOidToText
//////////////////////////////////////////////////////////////////////////////
EXPORT_C TPtrC8 CPkcs10Req::AttributeTypeOidToText(const TDesC &aType)
{
    TPtrC8 temp;

    if(aType.Compare(KX520CountryName) == 0)
        temp.Set(KCountryName);
    else if(aType.Compare(KX520OrganizationName) == 0)
        temp.Set(KOrganizationName);
    else if(aType.Compare(KX520OrganizationalUnitName) == 0)
        temp.Set(KOrganizationalUnitName);
    else if(aType.Compare(KX520LocalityName) == 0)
        temp.Set(KLocalityName);
    else if(aType.Compare(KX520StateOrProvinceName) == 0)
        temp.Set(KStateOrProvinceName);
    else if(aType.Compare(KX520Title) == 0)
        temp.Set(KTitle);
    else if(aType.Compare(KX520CommonName) == 0)
        temp.Set(KCommonName);
    else if(aType.Compare(KX520GivenName) == 0)
        temp.Set(KGivenName);
    else if(aType.Compare(KX520Surname) == 0)
        temp.Set(KSurname);
    else if(aType.Compare(KX520Initials) == 0)
        temp.Set(KInitials);
    else if(aType.Compare(KX520GenerationQualifier) == 0)
        temp.Set(KGenerationQualifier);
    else if(aType.Compare(KX520DNQualifier) == 0)
        temp.Set(KDNQualifier);
    else if(aType.Compare(KX520SerialNumber) == 0)
        temp.Set(KSerialNumber);
    else if(aType.Compare(KRFC2247DomainComponent) == 0)
        temp.Set(KDomainComponent);
    else
        {
        // Return dotted oid, length = 0
        }
    
    return temp;
}

//////////////////////////////////////////////////////////////////////////////
// BuildIndexPairsLC
//////////////////////////////////////////////////////////////////////////////
void CPkcs10Req::BuildIndexPairsLC(
    HBufC8                     *&buffer,
    const TDesC8                &aDistinguishedName,
    CArrayFixFlat<TIndexPair>   *aTypePairArray,
    CArrayFixFlat<TIndexPair>   *aValuePairArray
)
{
    // Some rules:
    // 1) Separator: ','
    // 2) Escape character '\'
    // 3) Value may be quoted. Then separator in value is allowed
    //    (without preceding escape char)

    HBufC8 *sourceBuffer;
    HBufC8 *targetBuffer;
    TUint i, j;
    targetBuffer = HBufC8::NewLC(aDistinguishedName.Length());
    targetBuffer->Des().SetLength(aDistinguishedName.Length());
    sourceBuffer = HBufC8::NewLC(aDistinguishedName.Length());
    sourceBuffer->Des().Copy(aDistinguishedName);
    TPtr8 sourcePointer = sourceBuffer->Des();
    TPtr8 targetPointer = targetBuffer->Des();
    TBool insideQuotes = EFalse;
    TIndexPair pair;
    TUint elementStartPosition = 0;
    TUint elementEndPosition = 0;

    // Delete leading and trailing spaces
    sourcePointer.Trim();

    // Scan text in order to collect name value pairs
    for(i = 0, j = 0; (TInt)i < sourcePointer.Length(); i++)
    {
        if(sourcePointer[i] == '"')
        {
            // Toggle quote state
            // Don't ignore quotes in order to ease trimming.
            // (Spaces inside quotes are meaningful)
            insideQuotes = !insideQuotes;
            targetPointer[j] = sourcePointer[i];
            j++;
            continue;
        }       

        if(insideQuotes)
        {
            // Allow any character
            // Check for escape
            if(sourcePointer[i] == '\\')
            {
                if((TInt)i < sourcePointer.Length() - 1)
                {
                    targetPointer[j] = sourcePointer[i + 1];
                    j++;
                    i++;    // skip char
                }
                else
                    User::Leave(KErrArgument);
            }
            else
            {
                targetPointer[j] = sourcePointer[i];
                j++;
            }
            continue;
        }

        // Check for escape
        if(sourcePointer[i] == '\\')
        {
            if((TInt)i < sourcePointer.Length() - 1)
            {
                targetPointer[j] = sourcePointer[i + 1];
                j++;
                i++;    // skip char
            }
            else
                User::Leave(KErrArgument);

            continue;
        }

        // Check for pair separator
        if(sourcePointer[i] == ',')
        {
            // End of type/value pair
            elementEndPosition = j;
            pair.startIndex = elementStartPosition;
            pair.endIndex = elementEndPosition;
            aValuePairArray->AppendL(pair);
            elementStartPosition = j;
        }
        // Check for type/value separator
        else if(sourcePointer[i] == '=')
        {
            elementEndPosition = j;
            pair.startIndex = elementStartPosition;
            pair.endIndex = elementEndPosition;
            aTypePairArray->AppendL(pair);
            elementStartPosition = j;
        }
        else
        {
            // Plain copy
            targetPointer[j] = sourcePointer[i];
            j++;
        }
    }

    // Append final value
    elementEndPosition = j;
    pair.startIndex = elementStartPosition;
    pair.endIndex = elementEndPosition;
    aValuePairArray->AppendL(pair);
    
    // Ensure we have consistent type/value pairs
    if(aTypePairArray->Count() != aValuePairArray->Count())
    {
        User::Leave(KErrArgument);
    }

    CleanupStack::PopAndDestroy(1);         // source
    // Return allocated shrinked buffer and OK status
    targetBuffer->Des().SetLength(j);
    buffer = targetBuffer;    
}

//////////////////////////////////////////////////////////////////////////////
// BuildDerLC
//////////////////////////////////////////////////////////////////////////////
void CPkcs10Req::BuildDerL(
    HBufC8                      *aSource,
    HBufC8                     *&derBuffer,
    CArrayFixFlat<TIndexPair>   *aTypeIndexArray,
    CArrayFixFlat<TIndexPair>   *aValueIndexArray,
    CArrayFixFlat<TPtr8>        *aTypePtrArray,
    CArrayFixFlat<TPtr8>        *aValuePtrArray,
    TBool                        aByPass,
    const TDesC8                &aByPassType
)
{
    TUint i;
    // trim types and values and count required der length
    TUint elementLength = 0;
    TUint totalLength = 0;
    TUint wholeLength = 0;
    TUint setLength = 0;
    TUint seqLength = 0;
    TUint oidLength = COMMON_NAME_OID_LTH;
    TUint stringLength = 0;
    TUint contentLength = 0;
    TUint oidIndex = 0;
    TBuf8<32> dottedOidDer;

    CArrayFixFlat<HBufC8 *> *elementArray =
      new (ELeave) CArrayFixFlat<HBufC8 *>(1);
    
    CleanupStack::PushL(elementArray);

    for(i = 0; (TInt)i < aTypeIndexArray->Count(); i++)  // counts are equal
    {
        TPtr8 tempPtr(
            CONST_CAST(TUint8*, aSource->Des().Ptr()) + aTypeIndexArray->At(i).startIndex,
            aTypeIndexArray->At(i).endIndex - aTypeIndexArray->At(i).startIndex
        );
        
        tempPtr.SetLength(
            aTypeIndexArray->At(i).endIndex - aTypeIndexArray->At(i).startIndex
        );
        
        aTypePtrArray->AppendL(tempPtr);
        
        tempPtr.Set(
            CONST_CAST(TUint8*, aSource->Des().Ptr()) + aValueIndexArray->At(i).startIndex,
            aValueIndexArray->At(i).endIndex - aValueIndexArray->At(i).startIndex,
            aValueIndexArray->At(i).endIndex - aValueIndexArray->At(i).startIndex
        );
        
        tempPtr.SetLength(aValueIndexArray->At(i).endIndex - aValueIndexArray->At(i).startIndex);
        
        tempPtr.Trim();

        if(0 == tempPtr.Length())
        {
            User::Leave(KErrArgument);
        }
        
        // Remove quotes. Spaces inside quotes are meaningful,
        // so Trim must be called before this operation
        if(tempPtr[0] == '"')
        {
            if(tempPtr[tempPtr.Length() -1 ] != '"')
            {                
                User::Leave(KErrArgument);
            }
            else
            {
                TInt tempLength = tempPtr.Length() - 2; 
                tempPtr.Set(CONST_CAST(TUint8*, tempPtr.Ptr() + 1), tempLength, tempLength);
                tempPtr.SetLength(tempLength);
            }
        }
        
        if(0 == tempPtr.Length())
        {
            User::Leave(KErrArgument);
        }
        
        aValuePtrArray->AppendL(tempPtr);
        
        aTypePtrArray->At(i).Trim();
        aTypePtrArray->At(i).UpperCase();
        
        // Check if type is supported, it is ignored if not
        if(CPkcs10Req::OidExistsL(aTypePtrArray->At(i), oidIndex, dottedOidDer))
        {
            if(KNullAttributeIndex == oidIndex)
            {
                oidLength = dottedOidDer.Length() - ASN1_SHORT_TAG_HDR_LTH;
            }
            else if(KDomainComponentIndex == oidIndex)
            {
                oidLength = DC_OID_LTH;
            }
            else
            {
                oidLength = COMMON_NAME_OID_LTH;
            }

            if(!(aByPass && aTypePtrArray->At(i).Compare(aByPassType) == 0))
            {
                // Printable string
                elementLength = aValuePtrArray->At(i).Length();
                stringLength = elementLength;
                if(elementLength > 127)
                    elementLength++;
                if(elementLength > 255)
                    elementLength++;
                elementLength += ASN1_SHORT_TAG_HDR_LTH;

                elementLength += oidLength;               // Oid 
                elementLength += ASN1_SHORT_TAG_HDR_LTH;        
                seqLength = elementLength;
                if(elementLength > 127)
                    elementLength++;
                if(elementLength > 255)
                    elementLength++;
                elementLength += ASN1_SHORT_TAG_HDR_LTH;  // Sequence
                setLength = elementLength;
                if(elementLength > 127)
                    elementLength++;
                if(elementLength > 255)
                    elementLength++;
                elementLength += ASN1_SHORT_TAG_HDR_LTH;  // Set
                wholeLength = elementLength;

                // Build one element
                CPkcs10Req::BuildElementDerLC(
                    elementArray,
                    wholeLength, 
                    setLength,
                    seqLength, 
                    oidLength,
                    stringLength, 
                    oidIndex,
                    dottedOidDer, 
                    aValuePtrArray->At(i)
                );            

                totalLength += elementLength;
            }
        }
    }
    
    if( 0 == totalLength )
    {
        User::Leave(KErrArgument);
    }
    
    contentLength = totalLength;
    if(totalLength > 127)                                   // Sequence
        totalLength++;
    if(totalLength > 255)
        totalLength++;
    
    totalLength += ASN1_SHORT_TAG_HDR_LTH;

    // Allocate buffer
    derBuffer = HBufC8::NewL(totalLength);
    derBuffer->Des().SetLength(totalLength);
    TUint8 *derBufferPtr = (TUint8 *)derBuffer->Ptr();
    derBufferPtr+= CPkcs10Req::DERSetTag(derBufferPtr, (EASN1Sequence | ASN1_CONSTRUCTED));
    derBufferPtr+= CPkcs10Req::DERSetLength(derBufferPtr, contentLength);
    
    for(i = 0; (TInt)i < elementArray->Count(); i++)
    {
        derBufferPtr+= CPkcs10Req::ASNCopy(
            derBufferPtr,
            CONST_CAST(unsigned char *, elementArray->At(i)->Des().Ptr()),
            elementArray->At(i)->Length()
        );
    }
    
    CleanupStack::PopAndDestroy(elementArray->Count());
    CleanupStack::PopAndDestroy(elementArray);
}

//////////////////////////////////////////////////////////////////////////////
// OidExistsLC
///////////////////////////////////////////////////////////////////////////////
TBool CPkcs10Req::OidExistsL(
    TPtr8 &aTypePtrArray,
    TUint &aIndex,
    TBuf8<32> &aDottedOidDer
)
{
TBool validOId = EFalse;
 
    aIndex = KNullAttributeIndex;
        
    if(aTypePtrArray.Compare(KCountryName) == 0)
        aIndex = KCountryNameIndex;
    else if(aTypePtrArray.Compare(KOrganizationName) == 0)
        aIndex = KOrganizationNameIndex;
    else if(aTypePtrArray.Compare(KOrganizationalUnitName) == 0)
        aIndex = KOrganizationalUnitNameIndex;
    else if(aTypePtrArray.Compare(KLocalityName) == 0)
        aIndex = KLocalityNameIndex;
    else if(aTypePtrArray.Compare(KStateOrProvinceName) == 0)
        aIndex = KStateOrProvinceNameIndex;
    else if(aTypePtrArray.Compare(KTitle) == 0)
        aIndex = KTitleIndex;
    else if(aTypePtrArray.Compare(KCommonName) == 0)
        aIndex = KCommonNameIndex;
    else if(aTypePtrArray.Compare(KGivenName) == 0)
        aIndex = KGivenNameIndex;
    else if(aTypePtrArray.Compare(KSurname) == 0)
        aIndex = KSurnameIndex;
    else if(aTypePtrArray.Compare(KInitials) == 0)
        aIndex = KInitialsIndex;
    else if(aTypePtrArray.Compare(KGenerationQualifier) == 0)
        aIndex = KGenerationQualifierIndex;
    else if(aTypePtrArray.Compare(KDNQualifier) == 0)
        aIndex = KDNQualifierIndex;
    else if(aTypePtrArray.Compare(KSerialNumber) == 0)
        aIndex = KSerialNumberIndex;
    else if(aTypePtrArray.Compare(KDomainComponent) == 0)
        aIndex = KDomainComponentIndex;

    if(KNullAttributeIndex != aIndex)
    {
        validOId = ETrue;
    }
    else
    {
        if(aTypePtrArray.Find(KDot) != KErrNotFound)
        {
            // Dotted oid notation
            TUint offset = 0;
            TBuf<32> tempOidBuf;
            tempOidBuf.Copy(aTypePtrArray);
            
            CASN1EncObjectIdentifier *oidIdentifier =
              CASN1EncObjectIdentifier::NewLC(tempOidBuf);
            
            // WriteDERL uses Length() instead of MaxLength() ?
            aDottedOidDer.SetLength(32);
            oidIdentifier->WriteDERL(aDottedOidDer, offset);
            aDottedOidDer.SetLength(oidIdentifier->LengthDER());
            CleanupStack::PopAndDestroy();  // oidIdentifier
            validOId = ETrue;
        }
    }

    return validOId;
}

////////////////////////////////////////////////////////////////////////////////////////
// GetOid
////////////////////////////////////////////////////////////////////////////////////////
TInt CPkcs10Req::GetOid(TUint aIndex, const TUint8 *&aValue)
{
    switch(aIndex)
    {
        case KCountryNameIndex:
            aValue = KCountryNameOid;
            break;
        case KOrganizationNameIndex: 
            aValue = KOrganizationNameOid;
            break;
        case KOrganizationalUnitNameIndex: 
            aValue = KOrganizationalUnitNameOid;
            break;
        case KLocalityNameIndex: 
            aValue = KLocalityNameOid;
            break;
        case KStateOrProvinceNameIndex: 
            aValue = KStateOrProvinceNameOid;
            break;
        case KTitleIndex: 
            aValue = KTitleOid;
            break;
        case KCommonNameIndex: 
            aValue = KCommonNOid;
            break;
        case KGivenNameIndex: 
            aValue = KGivenNameOid;
            break;
        case KSurnameIndex: 
            aValue = KSurnameOid;
            break;
        case KInitialsIndex:    
            aValue = KInitialsOid;
            break;
        case KGenerationQualifierIndex:    
            aValue = KGenerationQualifierOid;
            break;
        case KDNQualifierIndex:    
            aValue = KDNQualifierOid;
            break;
        case KSerialNumberIndex:    
            aValue = KSerialNumberOid;
            break;
        case KDomainComponentIndex:
            aValue = KDomainComponentOid;
            break;
        default:
            return KErrArgument;
    }
    
  return KErrNone;
}

////////////////////////////////////////////////////////////////////////////////////////
// FillDer
////////////////////////////////////////////////////////////////////////////////////////
void CPkcs10Req::BuildElementDerLC(CArrayFixFlat<HBufC8 *> *aElementArray, TUint aWholeLength, TUint aSetLength, TUint aSeqLength, TUint aOidLength, TUint aStringLength, TUint aOidIndex, TBuf8<32> &aDottedOidDer, TPtr8 &aValuePtr)
{
    HBufC8 *elementBuffer = HBufC8::NewLC(aWholeLength);
    elementBuffer->Des().SetLength(aWholeLength);
    TUint8 *elementBufferPtr = (TUint8 *)elementBuffer->Ptr();
    const TUint8 *oidPtr;
    
    elementBufferPtr+= CPkcs10Req::DERSetTag(elementBufferPtr, (EASN1Set | ASN1_CONSTRUCTED));
    elementBufferPtr+= CPkcs10Req::DERSetLength(elementBufferPtr, aSetLength);
    elementBufferPtr+= CPkcs10Req::DERSetTag(elementBufferPtr, (EASN1Sequence | ASN1_CONSTRUCTED));
    elementBufferPtr+= CPkcs10Req::DERSetLength(elementBufferPtr, aSeqLength);
    if(aOidIndex != 0)
        {
        GetOid(aOidIndex, oidPtr);
        elementBufferPtr+= CPkcs10Req::DERSetTag(elementBufferPtr, EASN1ObjectIdentifier);
        elementBufferPtr+= CPkcs10Req::DERSetLength(elementBufferPtr, aOidLength);
        elementBufferPtr+= CPkcs10Req::DERSetOid(elementBufferPtr, oidPtr, aOidLength);
        }
    else
        {
        // Dotted oid representation
        elementBufferPtr+= CPkcs10Req::ASNCopy(elementBufferPtr, CONST_CAST(unsigned char *, aDottedOidDer.Ptr()), aDottedOidDer.Length()); 
        }
    elementBufferPtr+= CPkcs10Req::DERSetTag(elementBufferPtr, EASN1UTF8String);
    elementBufferPtr+= CPkcs10Req::DERSetLength(elementBufferPtr, aStringLength);
    elementBufferPtr+= CPkcs10Req::ASNCopy(elementBufferPtr, CONST_CAST(unsigned char *, aValuePtr.Ptr()), aValuePtr.Length());
    
    aElementArray->AppendL(elementBuffer);
}

////////////////////////////////////////////////////////////////////////////////////////
// Set subject alt name extension, email
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CPkcs10Req::SetSubjectAltNameRfc822(const TDesC8 &aSubjectAltNameRfc822)
{
    TInt status = KErrNone;
    
    // Set subject alt name
    if(aSubjectAltNameRfc822.Length() > 0)
        iSubjectAltName.Set(aSubjectAltNameRfc822);

    return (status);
}

////////////////////////////////////////////////////////////////////////////////////////
// Set DNSname extension
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CPkcs10Req::SetDNSName(const TDesC8 &aDNSName)
{
    TInt status = KErrNone;

    // Set subject alt name
    if(aDNSName.Length() > 0)
        iDNSName.Set(aDNSName);

    return (status);    // DNS name is optional
}

////////////////////////////////////////////////////////////////////////////////////////
// Set challenge password extension
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CPkcs10Req::SetChallengePw(const TDesC8 &aChallengePw)
{
    // Set challenge password
    if(aChallengePw.Length() > 0)
        iChallengePW.Set(aChallengePw);
    
    return KErrNone;    // ChallengePw is optional
}

////////////////////////////////////////////////////////////////////////////////////////
// Set public key and key type (only RSA supported!)
////////////////////////////////////////////////////////////////////////////////////////
EXPORT_C TInt CPkcs10Req::SetPublicKey(const TDesC8 &aPublicKeyBits)
{
    TInt status = KErrNone;

    // Build public key DER
    if(aPublicKeyBits.Length() > 0 )
    {
        iPublicKey.Set(aPublicKeyBits);
    }
    else
    {        
        status = KErrArgument;
    }
    
    return (status);
}

////////////////////////////////////////////////////////////////////////////////////////
// Create the request
//
//  In:     THashType aSignatureAlgorithmId, MD5 os SHA1
//          const TDesC& aPrivateKeyFilename, keyid used in kmdserver
//          TBool doPEM, set ETrue if PEM encoding requested, otherwise out format is DER
//          TSignCallback aSignCallBackL, signing callback function address, defined in pkcs10.h
//  Out:    HBufC8 * containing the request. Remember to pop it out of the cleanupstack and delete it when not needed anymore.
//
//  Output format is either der (in example below, ASN1 decoding done)
//
//  0:d=0  hl=4 l= 431 cons: SEQUENCE
//  4:d=1  hl=4 l= 281 cons:  SEQUENCE
//  8:d=2  hl=2 l=   1 prim:   INTEGER           :00
//  11:d=2  hl=2 l=  52 cons:   SEQUENCE             : Distinguished name
//  13:d=3  hl=2 l=  16 cons:    SET
//  15:d=4  hl=2 l=  14 cons:     SEQUENCE
//  17:d=5  hl=2 l=   3 prim:      OBJECT            :commonName
//  22:d=5  hl=2 l=   7 prim:      PRINTABLESTRING   :subject
//  31:d=3  hl=2 l=  32 cons:    SET                                        // 
//  33:d=4  hl=2 l=  30 cons:     SEQUENCE
//  35:d=5  hl=2 l=   9 prim:      OBJECT            :emailAddress
//  46:d=5  hl=2 l=  17 prim:      IA5STRING
//  65:d=2  hl=3 l= 138 cons:   SEQUENCE
//  68:d=3  hl=2 l=  13 cons:    SEQUENCE
//  70:d=4  hl=2 l=   9 prim:     OBJECT            :rsaEncryption
//  81:d=4  hl=2 l=   0 prim:     NULL
//  83:d=3  hl=2 l= 121 prim:    BIT STRING
//  206:d=2  hl=2 l=  81 cons:   cont [ 0 ]
//  208:d=3  hl=2 l=  26 cons:    SEQUENCE
//  210:d=4  hl=2 l=   9 prim:     OBJECT            :challengePassword
//  221:d=4  hl=2 l=  13 cons:     SET
//  223:d=5  hl=2 l=  11 prim:      PRINTABLESTRING   :challengepw
//  236:d=3  hl=2 l=  51 cons:    SEQUENCE
//  238:d=4  hl=2 l=   9 prim:     OBJECT            :Extension Request
//  249:d=4  hl=2 l=  38 cons:     SET
//  251:d=5  hl=2 l=  36 cons:      SEQUENCE
//  253:d=6  hl=2 l=  34 cons:       SEQUENCE
//  255:d=7  hl=2 l=   9 prim:        OBJECT            :emailAddress
//  266:d=7  hl=2 l=  21 prim:        OCTET STRING
//  289:d=1  hl=2 l=  13 cons:  SEQUENCE
//  291:d=2  hl=2 l=   9 prim:   OBJECT            :sha1WithRSAEncryption
//  302:d=2  hl=2 l=   0 prim:   NULL
//  304:d=1  hl=3 l= 129 prim:  BIT STRING
//
//  or PEM, which looks like this
//  -----BEGIN NEW CERTIFICATE REQUEST-----
//  MIIBqDCCARECAQAwFjEUMBIGA1UEAxMLa3Vra3VsdXVydXUwgZ8wDQYJKoZIhvcN
//  AQEBBQADgY0AMIGJAoGBALCCSQ305j5l/qh4TeIJeaLdLEtKe9IpAPTCvZqZhIv/
//  VpVRw3fdq2qgBMe6zsO0kctBYkvdZ67Yq3Gdmzx1Ofx4S0F4BUDT6TGH2uP3zi04
//  zMb3IoDxF9BAp9drc6BKzMnhN5HBIV0RUoTLhQ8HKbFcwmqIAo0uJkUx2gbr+aOD
//  AgMBAAGgUjAXBgkqhkiG9w0BCQcxChMIcGFzc3dvcmQwNwYJKoZIhvcNAQkOMSow
//  KDAmBgNVHREEHzAdggtrdWtrdWx1dXJ1dYEOanVraUBub2tpYS5jb20wDQYJKoZI
//  hvcNAQEEBQADgYEAbltAG0DNe2NuyMHoJPg4QQExhUb9rep6eSJvrACbyT2qVgJe
//  vrdpOesdw7//MsOhgTP+MU36DlTVRbpoUJ8xO1wM10ljrxkWcwNfdEU4kf3NOiWB
//  4i+V99jPK2nbK/ofNpJU7m2nMvi4Z66WlM/kDEwVJe5GWwIzOIyOlfkiyc8=
//  -----END NEW CERTIFICATE REQUEST-----
//
///////////////////////////////////////////////////////////////////////////////
EXPORT_C HBufC8* CPkcs10Req::CreateCertificateRequestLC(
THashType aSignatureAlgorithmId, 
const TPKIKeyIdentifier& aPrivateKeyFilename, 
TBool aDoBase64, 
TBool aDoPEM, 
TSignCallback aSignCallBackL, 
TAny* aSignCallBackContext)
{
    // Build certificate request
    HBufC8  *CRInfoBuffer = NULL;
    HBufC8  *CRBuffer = NULL;
    TBuf8<SHA1_HASH_LTH> hash;
    TBase64Codec base64Codec;
    HBufC8 *encodedCRBuffer = NULL;
    HBufC8 *tempBuffer = NULL;
    TBuf8<MAX_SIGNATURE_LENGTH> signature;

    // Create CertificationRequestInfo consisting
    //
    //      - subject distinguished name
    //      - public key
    //      - Attributes
    //          -Subject alt name Rfc822
    //          -Challenge PW

    CRInfoBuffer = ASNEncodeCertificationRequestInfoLC();
    if(CRInfoBuffer)
    {
        // Sign CertificationRequestInfo
        //
        //      - create hash using the defined signature algorithm
        //      - create digital signature of the hash by calling the given callback function

        DigestL(CRInfoBuffer->Des(), hash, aSignatureAlgorithmId);

        // Capsulate hash in digest info
        TUint digestInfoLength = hash.Length() + 5 * ASN1_SHORT_TAG_HDR_LTH + MD5SIGNATURE_OID_LTH;
        HBufC8 *digestInfo = HBufC8::NewLC(digestInfoLength);
        
        digestInfo->Des().SetLength(digestInfoLength);
        TUint8 *digestInfoPtr = (TUint8*)digestInfo->Ptr();
        digestInfoPtr+= CPkcs10Req::DERSetTag(digestInfoPtr, (EASN1Sequence | ASN1_CONSTRUCTED));
        digestInfoPtr+= CPkcs10Req::DERSetLength(digestInfoPtr, digestInfoLength - ASN1_SHORT_TAG_HDR_LTH);
        digestInfoPtr+= CPkcs10Req::DERSetTag(digestInfoPtr, (EASN1Sequence | ASN1_CONSTRUCTED));
        digestInfoPtr+= CPkcs10Req::DERSetLength(digestInfoPtr, MD5SIGNATURE_OID_LTH + 2 * ASN1_SHORT_TAG_HDR_LTH);
        digestInfoPtr+= CPkcs10Req::DERSetTag(digestInfoPtr, EASN1ObjectIdentifier);
        digestInfoPtr+= CPkcs10Req::DERSetLength(digestInfoPtr, MD5SIGNATURE_OID_LTH);
        digestInfoPtr+= CPkcs10Req::DERSetOid(digestInfoPtr, (TUint8*)KMd5SignatureOid, MD5SIGNATURE_OID_LTH);
        digestInfoPtr+= CPkcs10Req::DERSetTag(digestInfoPtr, EASN1Null);
        digestInfoPtr+= CPkcs10Req::DERSetLength(digestInfoPtr, 0);
        digestInfoPtr+= CPkcs10Req::DERSetTag(digestInfoPtr, EASN1OctetString);
        digestInfoPtr+= CPkcs10Req::DERSetLength(digestInfoPtr, hash.Length());
        digestInfoPtr+= CPkcs10Req::ASNCopy(digestInfoPtr, CONST_CAST(TUint8 *, hash.Ptr()), hash.Length());

        // Sign it
        // Only RSA supported by now
        iSignatureAlgorithmId = aSignatureAlgorithmId;
        aSignCallBackL(
        	*digestInfo, 
        	signature, 
        	aPrivateKeyFilename, 
        	aSignCallBackContext);
        CleanupStack::PopAndDestroy( digestInfo );

        // Collect CertificationRequestInfo, signature algoritm identifier (NO OPTIONS)
        // and signature into PKCS#10 CertificationRequest

        CRBuffer = ASNEncodeCertificationRequestLC(CRInfoBuffer->Des(), signature);
        if(CRBuffer)
        {
            if(aDoBase64)
            {
                // Base64 encode the CertificationRequest
                tempBuffer = base64Codec.Base64EncodeLC(CRBuffer->Des());
                if(tempBuffer != NULL)
                {
                    // Append PEM header and trailer
                    encodedCRBuffer = HBufC8::NewL(tempBuffer->Length() + HEADERLENGTH + TRAILERLENGTH + 1);
                    TPtr8 encodedCRBufferPtr = encodedCRBuffer->Des();
                    if(aDoPEM)
                    {
                        encodedCRBufferPtr.Copy(HEADER);
                        encodedCRBufferPtr.Append(tempBuffer->Des());
                        encodedCRBufferPtr.Append(TRAILER);
                    }
                    else
                        encodedCRBufferPtr.Copy(tempBuffer->Des());
                        
                    CleanupStack::PopAndDestroy(3);  //tempBuffer, CRInfoBuffer, CRBuffer
                    CleanupStack::PushL(encodedCRBuffer);
                }
                else
                    CleanupStack::PopAndDestroy(2);  //CRInfoBuffer, CRBuffer
            }
            else
            {
                CleanupStack::Pop(1);
                CleanupStack::PopAndDestroy(1);  //CRInfoBuffer
                CleanupStack::PushL(CRBuffer);
                return (CRBuffer);
            }
        }
        else
            CleanupStack::PopAndDestroy(1);  //CRInfoBuffer
    }
    
    return (encodedCRBuffer);
}


////////////////////////////////////////////////////////////////////////////////////////
// MD5 hash function
////////////////////////////////////////////////////////////////////////////////////////
void CPkcs10Req::MD5_HashL(const TDesC8 &aInData, TDes8& aOutData)
{
    CUtlMessageDigest* md5 = TUtlCrypto::MakeMessageDigesterL(TUtlCrypto::EUtlMessageDigestMd5);
    aOutData.Copy(md5->Final(aInData));
    delete md5;
}

////////////////////////////////////////////////////////////////////////////////////////
// SHA1 hash function
////////////////////////////////////////////////////////////////////////////////////////
void CPkcs10Req::SHA1_HashL(const TDesC8 &aInData, TDes8& aOutData)
{
    CUtlMessageDigest* sha1 = TUtlCrypto::MakeMessageDigesterL(TUtlCrypto::EUtlMessageDigestSha1);
    aOutData.Copy(sha1->Final(aInData));
    delete sha1;
}

////////////////////////////////////////////////////////////////////////////////////////
// Digest function, calling either MD5 or SHA1 hash
////////////////////////////////////////////////////////////////////////////////////////
void CPkcs10Req::DigestL(const TDesC8& aData, TDes8& aDigest, THashType aHashType)
{
    if (aHashType == HASH_TYPE_MD5)
        MD5_HashL(aData, aDigest);
    else
        SHA1_HashL(aData, aDigest);
}


////////////////////////////////////////////////////////////////////////////
//
//  ASN1 related operations
//
///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
// Set ASN1 length
////////////////////////////////////////////////////////////////////////////////////////
TInt CPkcs10Req::DERSetLength(TUint8 *p, TInt aLen)
{
/*------------------------------------------------------
 *
 * Encode length value (max value supported 65535)
 *
 *------------------------------------------------------*/
    TInt CodingLth;
    aLen &= ASN1_MAX_LENGTH;
    if ( aLen > 127 )
    {
        if ( aLen < 256 )
        {  
            *p = 0x81;
            CodingLth = 2;         
        }
        else
        {
            *p = 0x82;
            p ++;
            *p = (TUint8)((aLen >> 8) & 0xff);
            CodingLth = 3;
        }  
        p ++;        
    }
    else
    {
        CodingLth = 1;       
    }
    *p = (TUint8)(aLen & 0xff);

    return CodingLth;
}

////////////////////////////////////////////////////////////////////////////////////////
// Set ASN1 integer
////////////////////////////////////////////////////////////////////////////////////////
TInt CPkcs10Req::DERSetInteger(TUint8 *p, TInt aValue)
{
/*------------------------------------------------------
 *
 * Encode Integer value (max value supported 65535)
 *
 *------------------------------------------------------*/
    TInt CodingLth; 
    aValue &= ASN1_MAX_INTEGER;
    if ( aValue > 255 ) 
        CodingLth = CPkcs10Req::DERSetLength(p, 2);
    else CodingLth = CPkcs10Req::DERSetLength(p, 1);

    p += CodingLth;  //skip length  
    if ( aValue > 255 ) {
        *p = (TUint8)((aValue >> 8) & 0xff);
        p ++;
        CodingLth ++;
    }
    *p = (TUint8)(aValue & 0xff);
    CodingLth ++;   

    return CodingLth;
}


////////////////////////////////////////////////////////////////////////////////////////
// Encode Certification Request Info
////////////////////////////////////////////////////////////////////////////////////////
HBufC8* CPkcs10Req::ASNEncodeCertificationRequestInfoLC()
{
    // Build certificate request info
    HBufC8  *CRInfoBuffer = NULL;
    TUint8  *CRInfoPtr;
    TUint   requestInfoLength;
    TUint   coverSeqHeaderLength = 2;

    HBufC8 *commonAttributes;
    HBufC8 *pkiInfo;
    HBufC8 *extendedAttributes;

    // Build components
    // Encode common attributes
    commonAttributes = ASNEncodeCommonAttributesLC();
    // Encode private key
    pkiInfo = ASNEncodePKIinfoLC();
    // Encode common attributes
    extendedAttributes = ASNEncodeExtendedAttributesLC();
    
    requestInfoLength   =  commonAttributes->Length() + pkiInfo->Length() + extendedAttributes->Length() + ASN1_SHORT_TAG_HDR_LTH + 1;  // Fixed one byte version number!
    
    if(requestInfoLength > 127)
        coverSeqHeaderLength++;
    if(requestInfoLength > 255)
        coverSeqHeaderLength++;
    
    requestInfoLength   =  requestInfoLength + coverSeqHeaderLength;

    if(requestInfoLength > 0)
    {
        CRInfoBuffer = HBufC8::NewL(requestInfoLength);
        CRInfoBuffer->Des().SetLength(requestInfoLength);   
        CRInfoPtr   = (TUint8*)CRInfoBuffer->Ptr();

        // Build header
        CRInfoPtr += CPkcs10Req::DERSetTag(CRInfoPtr, (EASN1Sequence | ASN1_CONSTRUCTED));
        CRInfoPtr += CPkcs10Req::DERSetLength(CRInfoPtr, requestInfoLength - coverSeqHeaderLength);
        CRInfoPtr += CPkcs10Req::DERSetTag(CRInfoPtr, EASN1Integer);
        CRInfoPtr += DERSetInteger(CRInfoPtr, 0);       // version

        // Copy component ders
        if(commonAttributes->Length() > 0)
            CRInfoPtr += CPkcs10Req::ASNCopy(CRInfoPtr, CONST_CAST(TUint8 *, commonAttributes->Ptr()), commonAttributes->Length());

        if(pkiInfo->Length() > 0)
            CRInfoPtr += CPkcs10Req::ASNCopy(CRInfoPtr, CONST_CAST(TUint8 *, pkiInfo->Ptr()), pkiInfo->Length());

        if(extendedAttributes->Length() > 0)
            CRInfoPtr += CPkcs10Req::ASNCopy(CRInfoPtr, CONST_CAST(TUint8 *, extendedAttributes->Ptr()), extendedAttributes->Length());
    }
    
    CleanupStack::PopAndDestroy(extendedAttributes);
    CleanupStack::PopAndDestroy(pkiInfo);
    CleanupStack::PopAndDestroy(commonAttributes);
   
    if(CRInfoBuffer != NULL)
        CleanupStack::PushL(CRInfoBuffer);      // to be destroyed by the caller
    
    return (CRInfoBuffer);
}

////////////////////////////////////////////////////////////////////////////////////////
// Encode Certification Request
////////////////////////////////////////////////////////////////////////////////////////
HBufC8* CPkcs10Req::ASNEncodeCertificationRequestLC(const TDes8 &CRInfoBuffer, const TDes8 &signature)
{
    HBufC8 *CRBuffer = NULL;
    TUint8 *CRPtr;
    TUint   requestLength;
    TUint   coverSeqHeaderLength = 2;
    TUint   signatureLength = 0;

    if(CRInfoBuffer.Length() > 0 && signature.Length() > 0)
    {
        signatureLength = signature.Length() + 1; // No padding zero added
        if(signatureLength > 127)
            signatureLength++;
        if(signatureLength > 255)
            signatureLength++;

        if(iSignatureAlgorithmId == HASH_TYPE_MD5)
            requestLength = CRInfoBuffer.Length() + signatureLength + 4 * ASN1_SHORT_TAG_HDR_LTH + MD5WITHRSA_OID_LTH;
        else
            requestLength = CRInfoBuffer.Length() + signatureLength + 4 * ASN1_SHORT_TAG_HDR_LTH + SHA1WITHRSA_OID_LTH;

        if(requestLength > 127)
            coverSeqHeaderLength++;
        if(requestLength > 255)
            coverSeqHeaderLength++;

        requestLength = requestLength + coverSeqHeaderLength;
        
        CRBuffer = HBufC8::NewL(requestLength);
        CRBuffer->Des().SetLength(requestLength);   
        CRPtr   = (TUint8*)CRBuffer->Ptr();

        CRPtr += CPkcs10Req::DERSetTag(CRPtr, (EASN1Sequence | ASN1_CONSTRUCTED));
        CRPtr += CPkcs10Req::DERSetLength(CRPtr, requestLength - coverSeqHeaderLength);

        // Copy Certificate request info
        CRPtr += CPkcs10Req::ASNCopy(CRPtr, CONST_CAST(TUint8 *, CRInfoBuffer.Ptr()), CRInfoBuffer.Length());
        CRPtr += CPkcs10Req::DERSetTag(CRPtr, (EASN1Sequence | ASN1_CONSTRUCTED));
        CRPtr += CPkcs10Req::DERSetLength(CRPtr, MD5WITHRSA_OID_LTH + 2 * ASN1_SHORT_TAG_HDR_LTH);      // null included!
        // Set algorithm identifier
        CRPtr += CPkcs10Req::DERSetTag(CRPtr, EASN1ObjectIdentifier);
        if(iSignatureAlgorithmId == HASH_TYPE_MD5)
        {
            CRPtr += CPkcs10Req::DERSetLength(CRPtr, MD5WITHRSA_OID_LTH);
            CRPtr += CPkcs10Req::DERSetOid(CRPtr, (TUint8*)KMd5WithRSAEncryptionOid, MD5WITHRSA_OID_LTH);
        }
        else
        {
            CRPtr += CPkcs10Req::DERSetLength(CRPtr, SHA1WITHRSA_OID_LTH);
            CRPtr += CPkcs10Req::DERSetOid(CRPtr, (TUint8*)KSha1WithRSAEncryptionOid, SHA1WITHRSA_OID_LTH);
        }
            
        CRPtr += CPkcs10Req::DERSetTag(CRPtr, EASN1Null);
        CRPtr += CPkcs10Req::DERSetLength(CRPtr, 0);
        // Copy signature
        CRPtr += CPkcs10Req::DERSetTag(CRPtr, EASN1BitString);
        CRPtr += CPkcs10Req::DERSetLength(CRPtr, signature.Length() + 1);
        CRPtr += CPkcs10Req::DERSetLength(CRPtr, 0);    // add 0, no padding
        CRPtr += CPkcs10Req::ASNCopy(CRPtr, CONST_CAST(TUint8 *, signature.Ptr()), signature.Length());

        CleanupStack::PushL(CRBuffer);          // to be destroyed by the caller
    }
    return (CRBuffer);
}

////////////////////////////////////////////////////////////////////////////////////////
// Encode common attributes
////////////////////////////////////////////////////////////////////////////////////////
HBufC8* CPkcs10Req::ASNEncodeCommonAttributesLC()
{
    HBufC8 *commonAttributesBuffer = NULL;
    TUint8 *commonAttributesPtr;
    TUint distinguishedNameAttrLength;
    TUint totalLength = 0;
    TUint coverSeqHeaderLength = 2;

    if(iUseExtendedDistinguishedName)
    {
        if(iDNDerBuf != NULL && iDNDerBuf->Des().Length() > 0)
            commonAttributesBuffer = iDNDerBuf->Des().AllocLC();
        return (commonAttributesBuffer);
    }
    if(iDistinguishedName.Length() == 0)
        distinguishedNameAttrLength = 0;
    else
        distinguishedNameAttrLength = 4 * ASN1_SHORT_TAG_HDR_LTH + COMMON_NAME_OID_LTH + iDistinguishedName.Length();
    
    totalLength = distinguishedNameAttrLength;
    if(totalLength > 127)
        coverSeqHeaderLength++;
    if(totalLength > 255)
        coverSeqHeaderLength++;

    totalLength = totalLength + coverSeqHeaderLength;
    
    if(totalLength > 0)
    {
        commonAttributesBuffer = HBufC8::NewL(totalLength);
        commonAttributesBuffer->Des().SetLength(totalLength);
        commonAttributesPtr = (TUint8*)commonAttributesBuffer->Ptr();

        commonAttributesPtr+= CPkcs10Req::DERSetTag(commonAttributesPtr, (EASN1Sequence | ASN1_CONSTRUCTED));
        commonAttributesPtr+= CPkcs10Req::DERSetLength(commonAttributesPtr, totalLength - coverSeqHeaderLength);
        
        if(iDistinguishedName.Length() > 0)
        {
            // Encode common name
            commonAttributesPtr+= CPkcs10Req::DERSetTag(commonAttributesPtr, (EASN1Set | ASN1_CONSTRUCTED));
            commonAttributesPtr+= CPkcs10Req::DERSetLength(commonAttributesPtr, distinguishedNameAttrLength - ASN1_SHORT_TAG_HDR_LTH);
            commonAttributesPtr+= CPkcs10Req::DERSetTag(commonAttributesPtr, (EASN1Sequence | ASN1_CONSTRUCTED));
            commonAttributesPtr+= CPkcs10Req::DERSetLength(commonAttributesPtr, distinguishedNameAttrLength - 2 * ASN1_SHORT_TAG_HDR_LTH);
            commonAttributesPtr+= CPkcs10Req::DERSetTag(commonAttributesPtr, EASN1ObjectIdentifier);
            commonAttributesPtr+= CPkcs10Req::DERSetLength(commonAttributesPtr, COMMON_NAME_OID_LTH);
            commonAttributesPtr+= CPkcs10Req::DERSetOid(commonAttributesPtr, (TUint8*)KCommonNameOid, COMMON_NAME_OID_LTH);
            commonAttributesPtr+= CPkcs10Req::DERSetTag(commonAttributesPtr, EASN1UTF8String);
            commonAttributesPtr+= CPkcs10Req::DERSetLength(commonAttributesPtr, iDistinguishedName.Length());
            commonAttributesPtr+= CPkcs10Req::ASNCopy(commonAttributesPtr, CONST_CAST(TUint8 *, iDistinguishedName.Ptr()), iDistinguishedName.Length());
        }
        CleanupStack::PushL(commonAttributesBuffer);
    }
    return (commonAttributesBuffer);
}

////////////////////////////////////////////////////////////////////////////////////////
// Encode Private key information
////////////////////////////////////////////////////////////////////////////////////////
HBufC8* CPkcs10Req::ASNEncodePKIinfoLC()
{
    HBufC8 *pkiInfoBuffer = NULL;
    TUint8 *pkiInfoPtr;
    TUint entityLength;
    TUint bitStringHeaderLength = 2;
    TUint coverSeqHeaderLength = 2;
    
    if(iPublicKey.Length() == 0)
        entityLength = 0;
    else
    {
        entityLength = iPublicKey.Length() + 1;   // Add 1 for padding indicator byte
        if(entityLength > 127)
            bitStringHeaderLength++;
        if(entityLength > 255)
            bitStringHeaderLength++;
        
        entityLength = entityLength + bitStringHeaderLength + 3 * ASN1_SHORT_TAG_HDR_LTH + KEY_TYPE_RSA_OID_LTH;
        
        if(entityLength > 127)
            coverSeqHeaderLength++;
        if(entityLength > 255)
            coverSeqHeaderLength++;
        
        entityLength = entityLength + coverSeqHeaderLength;
    }

    if(entityLength > 0)
    {
        pkiInfoBuffer = HBufC8::NewL(entityLength);
        pkiInfoBuffer->Des().SetLength(entityLength);
        pkiInfoPtr = (TUint8*)pkiInfoBuffer->Ptr();

        pkiInfoPtr+= CPkcs10Req::DERSetTag(pkiInfoPtr, (EASN1Sequence | ASN1_CONSTRUCTED));
        pkiInfoPtr+= CPkcs10Req::DERSetLength(pkiInfoPtr, entityLength - coverSeqHeaderLength);
        pkiInfoPtr+= CPkcs10Req::DERSetTag(pkiInfoPtr, (EASN1Sequence | ASN1_CONSTRUCTED));
        pkiInfoPtr+= CPkcs10Req::DERSetLength(pkiInfoPtr, KEY_TYPE_RSA_OID_LTH + ASN1_SHORT_TAG_HDR_LTH + ASN1_NULL_LTH);
        // Only RSA key supported
        pkiInfoPtr+= CPkcs10Req::DERSetTag(pkiInfoPtr, EASN1ObjectIdentifier);
        pkiInfoPtr+= CPkcs10Req::DERSetLength(pkiInfoPtr, KEY_TYPE_RSA_OID_LTH);
        pkiInfoPtr+= CPkcs10Req::DERSetOid(pkiInfoPtr, (TUint8*)KKeyTypeRSAOid, KEY_TYPE_RSA_OID_LTH);
        pkiInfoPtr+= CPkcs10Req::DERSetTag(pkiInfoPtr, EASN1Null);
        pkiInfoPtr+= CPkcs10Req::DERSetLength(pkiInfoPtr, 0);
        pkiInfoPtr+= CPkcs10Req::DERSetTag(pkiInfoPtr, EASN1BitString);
        pkiInfoPtr+= CPkcs10Req::DERSetLength(pkiInfoPtr, iPublicKey.Length() + 1);
        pkiInfoPtr+= CPkcs10Req::DERSetLength(pkiInfoPtr, 0);   // add 0, no padding
        pkiInfoPtr+= CPkcs10Req::ASNCopy(pkiInfoPtr, CONST_CAST(TUint8 *, iPublicKey.Ptr()), iPublicKey.Length());

        CleanupStack::PushL(pkiInfoBuffer);
    }
    return (pkiInfoBuffer);
}

////////////////////////////////////////////////////////////////////////////////////////
// Encode Extended attributes
////////////////////////////////////////////////////////////////////////////////////////
HBufC8* CPkcs10Req::ASNEncodeExtendedAttributesLC()
{
    HBufC8 *extendedAttributesBuffer = NULL;
    TUint8 *extendedAttributesPtr;
    TUint challengePWLength;
    TUint DNSLength;
    TUint SubAltNLength;
    TUint emailExtensionLength;
    TUint totalLength;
    TUint tempLength;
    TUint coverTagLength = 2;
    
    if(iChallengePW.Length() == 0)
        challengePWLength = 0;
    else
        challengePWLength = 4 * ASN1_SHORT_TAG_HDR_LTH + CHALLENGEPW_OID_LTH + iChallengePW.Length();

    if((iSubjectAltName.Length() == 0) && (iDNSName.Length() == 0))
        emailExtensionLength = 0;
    else
        // Header part
        emailExtensionLength = 8 * ASN1_SHORT_TAG_HDR_LTH + SUBJECTALTNAME_OID_LTH + PKCS10_EXTENSION_OID_LTH;
    
    if(iSubjectAltName.Length() == 0)
        SubAltNLength = 0;
    else
        SubAltNLength = ASN1_SHORT_TAG_HDR_LTH + iSubjectAltName.Length();
    
    emailExtensionLength = emailExtensionLength + SubAltNLength;

    if(iDNSName.Length() == 0)
        DNSLength = 0;
    else
        DNSLength = ASN1_SHORT_TAG_HDR_LTH + iDNSName.Length();

    emailExtensionLength = emailExtensionLength + DNSLength;
    totalLength = challengePWLength + emailExtensionLength;

    if(totalLength > 127)
        coverTagLength++;

    totalLength = totalLength + coverTagLength;
    
    if(totalLength > 0)
    {
        extendedAttributesBuffer = HBufC8::NewL(totalLength);
        extendedAttributesBuffer->Des().SetLength(totalLength);
        extendedAttributesPtr = (TUint8*)extendedAttributesBuffer->Ptr();

        extendedAttributesPtr+= CPkcs10Req::DERSetTag(extendedAttributesPtr, (EASN1EOC | ASN1_CONTEXT_SPECIFIC | ASN1_CONSTRUCTED));
        extendedAttributesPtr+= CPkcs10Req::DERSetLength(extendedAttributesPtr, totalLength - coverTagLength);

        if(challengePWLength > 0)
        {
            // Challenge password
            extendedAttributesPtr+= CPkcs10Req::DERSetTag(extendedAttributesPtr, (EASN1Sequence | ASN1_CONSTRUCTED));
            extendedAttributesPtr+= CPkcs10Req::DERSetLength(extendedAttributesPtr, challengePWLength - ASN1_SHORT_TAG_HDR_LTH );
            extendedAttributesPtr+= CPkcs10Req::DERSetTag(extendedAttributesPtr, EASN1ObjectIdentifier);
            extendedAttributesPtr+= CPkcs10Req::DERSetLength(extendedAttributesPtr, CHALLENGEPW_OID_LTH);
            extendedAttributesPtr+= CPkcs10Req::DERSetOid(extendedAttributesPtr, (TUint8*)KChallengePWOid, CHALLENGEPW_OID_LTH);
            extendedAttributesPtr+= CPkcs10Req::DERSetTag(extendedAttributesPtr, (EASN1Set | ASN1_CONSTRUCTED));
            extendedAttributesPtr+= CPkcs10Req::DERSetLength(extendedAttributesPtr, iChallengePW.Length() + ASN1_SHORT_TAG_HDR_LTH );
            extendedAttributesPtr+= CPkcs10Req::DERSetTag(extendedAttributesPtr, (EASN1PrintableString));
            extendedAttributesPtr+= CPkcs10Req::DERSetLength(extendedAttributesPtr, iChallengePW.Length());
            extendedAttributesPtr+= CPkcs10Req::ASNCopy(extendedAttributesPtr, CONST_CAST(TUint8 *, iChallengePW.Ptr()), iChallengePW.Length());
        }
        
        if(emailExtensionLength > 0)
        {
            // SubjectAltName.Email assumed to be less than 97 bytes
            extendedAttributesPtr+= CPkcs10Req::DERSetTag(extendedAttributesPtr, (EASN1Sequence | ASN1_CONSTRUCTED));
            extendedAttributesPtr+= CPkcs10Req::DERSetLength(extendedAttributesPtr, emailExtensionLength - ASN1_SHORT_TAG_HDR_LTH );
            extendedAttributesPtr+= CPkcs10Req::DERSetTag(extendedAttributesPtr, EASN1ObjectIdentifier);
            extendedAttributesPtr+= CPkcs10Req::DERSetLength(extendedAttributesPtr, PKCS10_EXTENSION_OID_LTH);
            extendedAttributesPtr+= CPkcs10Req::DERSetOid(extendedAttributesPtr, (TUint8*)KPkcs10ExtensionOid, PKCS10_EXTENSION_OID_LTH);
            extendedAttributesPtr+= CPkcs10Req::DERSetTag(extendedAttributesPtr, (EASN1Set | ASN1_CONSTRUCTED));
            tempLength = SUBJECTALTNAME_OID_LTH + 5 * ASN1_SHORT_TAG_HDR_LTH + DNSLength + SubAltNLength;
            extendedAttributesPtr+= CPkcs10Req::DERSetLength(extendedAttributesPtr, tempLength);
            extendedAttributesPtr+= CPkcs10Req::DERSetTag(extendedAttributesPtr, (EASN1Sequence | ASN1_CONSTRUCTED));
            extendedAttributesPtr+= CPkcs10Req::DERSetLength(extendedAttributesPtr, tempLength - ASN1_SHORT_TAG_HDR_LTH );
            extendedAttributesPtr+= CPkcs10Req::DERSetTag(extendedAttributesPtr, (EASN1Sequence | ASN1_CONSTRUCTED));
            extendedAttributesPtr+= CPkcs10Req::DERSetLength(extendedAttributesPtr, tempLength - 2 * ASN1_SHORT_TAG_HDR_LTH);
            extendedAttributesPtr+= CPkcs10Req::DERSetTag(extendedAttributesPtr, EASN1ObjectIdentifier);
            extendedAttributesPtr+= CPkcs10Req::DERSetLength(extendedAttributesPtr, SUBJECTALTNAME_OID_LTH);
            extendedAttributesPtr+= CPkcs10Req::DERSetOid(extendedAttributesPtr, (TUint8*)KSubjectAltNameOid, SUBJECTALTNAME_OID_LTH);
            extendedAttributesPtr+= CPkcs10Req::DERSetTag(extendedAttributesPtr, EASN1OctetString);
            extendedAttributesPtr+= CPkcs10Req::DERSetLength(extendedAttributesPtr,  ASN1_SHORT_TAG_HDR_LTH + DNSLength + SubAltNLength);
            extendedAttributesPtr+= CPkcs10Req::DERSetTag(extendedAttributesPtr, (EASN1Sequence | ASN1_CONSTRUCTED));
            extendedAttributesPtr+= CPkcs10Req::DERSetLength(extendedAttributesPtr, DNSLength  + SubAltNLength);

            if(SubAltNLength > 0)
            {
                extendedAttributesPtr+= CPkcs10Req::DERSetTag(extendedAttributesPtr, (EASN1EOC | ASN1_CONTEXT_SPECIFIC | 0x1));     // context specific 1
                extendedAttributesPtr+= CPkcs10Req::DERSetLength(extendedAttributesPtr,  iSubjectAltName.Length());
                extendedAttributesPtr+= CPkcs10Req::ASNCopy(extendedAttributesPtr, CONST_CAST(TUint8 *, iSubjectAltName.Ptr()), iSubjectAltName.Length());
            }
            
            if(DNSLength > 0)
            {
                extendedAttributesPtr+= CPkcs10Req::DERSetTag(extendedAttributesPtr, (EASN1EOC | ASN1_CONTEXT_SPECIFIC | 0x2));     // context specific 2
                extendedAttributesPtr+= CPkcs10Req::DERSetLength(extendedAttributesPtr,  iDNSName.Length());
                extendedAttributesPtr+= CPkcs10Req::ASNCopy(extendedAttributesPtr, CONST_CAST(TUint8 *, iDNSName.Ptr()), iDNSName.Length());
            }
        }
        CleanupStack::PushL(extendedAttributesBuffer);
    }

    return (extendedAttributesBuffer);
}

////////////////////////////////////////////////////////////////////////////////////////
// ASN1 copy
////////////////////////////////////////////////////////////////////////////////////////
TInt CPkcs10Req::ASNCopy(TUint8 *dataPtr, TUint8 *valuePtr, TInt length)
{
    Mem::Copy(dataPtr, valuePtr, length);
    return (length);
}
