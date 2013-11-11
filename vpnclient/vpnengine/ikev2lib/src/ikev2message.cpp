/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#include <es_sock.h>
#include <badesca.h>
#include <random.h>

#include "ikev2message.h"
#include "ikev2const.h"
#include "ikecalist.h"
#include "ikecaelem.h"
#include "ikev2trafficselector.h"
#include "ikecrypto.h"
#include "ikev2identity.h"
#include "ikemsgheader.h"
#include "ikedebug.h"

static const TUint8 KMessageIdFieldOffset = 20; 

static const TUint KIkePayloadGenericHeaderLength = 4;
static const TUint8 KLengthFieldOffset = 2;

static const TUint32 KSha1Length = 20;
static const TUint32 KCertReqHeaderLength = 5;
static const TUint32 KCertHeaderLength = 5;
static const TUint32 KSaHeaderLength = 4;
static const TUint32 KKeHeaderLength = 8;
static const TUint32 KNonceHeaderLength = 4;
static const TUint32 KAuthHeaderLength = 8;
static const TUint32 KNotifyHeaderLength = 8;
static const TUint32 KConfigurationHeaderLength = 8;
static const TUint32 KVendorIdHeaderLength = 4;
static const TUint32 KDeleteHeaderLength = 8;
static const TUint32 KEapHeaderLength = 4;
static const TUint32 KTsHeaderLength = 8;
static const TUint32 KEncryptedHeaderLength = 4;

_LIT8(KNonEspMarker, "\0\0\0\0");

CIkeV2Payload::CIkeV2Payload(TUint8 aPayloadType)
:iPayloadType(aPayloadType)
    {    
    }


CIkeV2Payload::~CIkeV2Payload()
    {
    delete iPayloadData;
    }

TUint8 CIkeV2Payload::PayloadType() const
    {
    return iPayloadType;
    }


TUint8 CIkeV2Payload::NextPayload() const
    {
    __ASSERT_DEBUG(iPayloadData->Length() >= KIkePayloadGenericHeaderLength,
                   User::Invariant());
    
    return (*iPayloadData)[0];
    }


void CIkeV2Payload::SetNextPayload(TUint8 aNextPayload)
    {
    __ASSERT_DEBUG(iPayloadData->Length() >= KIkePayloadGenericHeaderLength,
                   User::Invariant());
    
    TPtr8 payloadDataPtr(iPayloadData->Des());
    payloadDataPtr[0] = aNextPayload;
    }


TUint16 CIkeV2Payload::PayloadLength() const
    {
    __ASSERT_DEBUG(iPayloadData->Length() >= KIkePayloadGenericHeaderLength,
                   User::Invariant());
    
    return BigEndian::Get16(iPayloadData->Ptr() + KLengthFieldOffset);
    }


void CIkeV2Payload::SetPayloadLength(TUint16 aLength)
    {
    __ASSERT_DEBUG(iPayloadData->Length() >= KIkePayloadGenericHeaderLength,
                   User::Invariant());
    const TUint KLengthPosition = 2; 
    
    BigEndian::Put16(reinterpret_cast<TUint8*>(&aLength), aLength);
    TPtrC8 length(reinterpret_cast<TUint8*>(&aLength), sizeof(aLength));
    
    TPtr8 lengthPtr(iPayloadData->Des().MidTPtr(KLengthPosition, 
                                                length.Length()));    
    lengthPtr = length; 
    }

TPtrC8 CIkeV2Payload::PayloadData() const
    {
    return TPtrC8(*iPayloadData);
    }


CIkevV2CertReqPayload* CIkevV2CertReqPayload::NewL(const CIkeCaList& aCaList)
    {
    CIkevV2CertReqPayload* self = new (ELeave) CIkevV2CertReqPayload;
    CleanupStack::PushL(self);
    self->ConstructL(aCaList);
    CleanupStack::Pop(self);
    
    return self;
    }



CIkevV2CertReqPayload::CIkevV2CertReqPayload()
:CIkeV2Payload(IKEV2_PAYLOAD_CR)
    {
    }


void CIkevV2CertReqPayload::ConstructL(const CIkeCaList& aCaList)
    {
    __ASSERT_DEBUG(aCaList.Count() > 0, User::Invariant());
    TUint16 length = (aCaList.Count() * KSha1Length) + KCertReqHeaderLength;
    
    iPayloadData = HBufC8::NewL(length);
    TPtr8 payloadDataPtr(iPayloadData->Des());
    payloadDataPtr.SetLength(KIkePayloadGenericHeaderLength);
    payloadDataPtr.FillZ();
    
    SetPayloadLength(length);
    
    TUint8 encoding = X509_CERTIFICATE_SIGN;       
    TPtrC8 encodingPtr(&encoding, sizeof(encoding));
    payloadDataPtr.Append(encodingPtr);
    
    for (TUint i = 0; i < aCaList.Count(); ++i)
        {
        payloadDataPtr.Append(aCaList[i]->KeyHash());
        }
    
    __ASSERT_DEBUG(PayloadLength() == iPayloadData->Length(), User::Invariant());
    }


CIkevV2CertPayload* CIkevV2CertPayload::NewL(const TDesC8& aCertData)
    {
    CIkevV2CertPayload* self = new (ELeave) CIkevV2CertPayload;
    CleanupStack::PushL(self);
    self->ConstructL(aCertData);
    CleanupStack::Pop(self);
    
    return self;
    }


CIkevV2CertPayload::CIkevV2CertPayload()
:CIkeV2Payload(IKEV2_PAYLOAD_CERT)
    {
    
    }


void CIkevV2CertPayload::ConstructL(const TDesC8& aCertData)
    {
    TUint16 length = aCertData.Length() + KCertHeaderLength;
    
    iPayloadData = HBufC8::NewL(length);
    TPtr8 payloadDataPtr(iPayloadData->Des());
    payloadDataPtr.SetLength(KIkePayloadGenericHeaderLength);  
    payloadDataPtr.FillZ();
    
    SetPayloadLength(length);

    TUint8 encoding = X509_CERTIFICATE_SIGN;     
    TPtrC8 encodingPtr(&encoding, sizeof(encoding));
    payloadDataPtr.Append(encodingPtr);       
    payloadDataPtr.Append(aCertData);   
    
    __ASSERT_DEBUG(PayloadLength() == iPayloadData->Length(), User::Invariant());
    }


CIkevV2SaPayload* CIkevV2SaPayload::NewL(const TDesC8& aSaData)
    {
    CIkevV2SaPayload* self = new (ELeave) CIkevV2SaPayload; 
    CleanupStack::PushL(self);
    self->ConstructL(aSaData);
    CleanupStack::Pop(self);
    
    return self;
    }


CIkevV2SaPayload::CIkevV2SaPayload()
:CIkeV2Payload(IKEV2_PAYLOAD_SA)
    {    
    }


void CIkevV2SaPayload::ConstructL(const TDesC8& aSaData)
    {
    TUint16 length = aSaData.Length() + KSaHeaderLength;
    
    iPayloadData = HBufC8::NewL(length);
    TPtr8 payloadDataPtr(iPayloadData->Des());
    payloadDataPtr.SetLength(KIkePayloadGenericHeaderLength);
    payloadDataPtr.FillZ();
    
    SetPayloadLength(length);
    
    payloadDataPtr.Append(aSaData);
    
    __ASSERT_DEBUG(PayloadLength() == iPayloadData->Length(), User::Invariant());
    }


CIkevV2KePayload* CIkevV2KePayload::NewL(TUint16 aDHGroup, const TDesC8& aKeData)
    {    
    CIkevV2KePayload* self = new (ELeave) CIkevV2KePayload;
    CleanupStack::PushL(self);
    self->ConstructL(aDHGroup, aKeData);
    CleanupStack::Pop(self);
    
    return self;
    }


CIkevV2KePayload::CIkevV2KePayload()
:CIkeV2Payload(IKEV2_PAYLOAD_KE)
    {
    }

void CIkevV2KePayload::ConstructL(TUint16 aDHGroup, const TDesC8& aKeData)
    {
    static const TUint8 KReservedFieldLength = 2;
    
    TUint16 length = aKeData.Length() + KKeHeaderLength;
    
    iPayloadData = HBufC8::NewL(length);
    TPtr8 payloadDataPtr(iPayloadData->Des());
    payloadDataPtr.SetLength(KIkePayloadGenericHeaderLength);
    payloadDataPtr.FillZ();
    
    SetPayloadLength(length);
    
    BigEndian::Put16(reinterpret_cast<TUint8*>(&aDHGroup), aDHGroup);
    TPtrC8 dhGroupPtr(reinterpret_cast<TUint8*>(&aDHGroup), sizeof(aDHGroup));
    payloadDataPtr.Append(dhGroupPtr);
    
    //Leave reserved bytes zero
    payloadDataPtr.SetLength(payloadDataPtr.Length() + KReservedFieldLength);
    TPtr8 reservedBytes = payloadDataPtr.RightTPtr(KReservedFieldLength);
    reservedBytes.FillZ();
    
    payloadDataPtr.Append(aKeData);  
    
    __ASSERT_DEBUG(PayloadLength() == iPayloadData->Length(), User::Invariant());
    }


CIkevV2NoncePayload* CIkevV2NoncePayload::NewL(const TDesC8& aNonceData)
    {
    CIkevV2NoncePayload* self = new (ELeave) CIkevV2NoncePayload;
    CleanupStack::PushL(self);
    self->ConstructL(aNonceData);
    CleanupStack::Pop(self);
    
    return self;
    }


CIkevV2NoncePayload::CIkevV2NoncePayload()
:CIkeV2Payload(IKEV2_PAYLOAD_NONCE)
    {
    }


void CIkevV2NoncePayload::ConstructL(const TDesC8& aNonceData)
    {
    TUint16 length = aNonceData.Length() + KNonceHeaderLength;

    iPayloadData = HBufC8::NewL(length);
    TPtr8 payloadDataPtr(iPayloadData->Des());
    payloadDataPtr.SetLength(KIkePayloadGenericHeaderLength);  
    payloadDataPtr.FillZ();
    
    SetPayloadLength(length);

    payloadDataPtr.Append(aNonceData);
    
    __ASSERT_DEBUG(PayloadLength() == iPayloadData->Length(), User::Invariant());
    }


CIkevV2IdPayload::CIkevV2IdPayload(TUint8 aPayloadType)
:CIkeV2Payload(aPayloadType)
    {    
    }

void CIkevV2IdPayload::ConstructL(const CIkeV2Identity& aIdentity)
    {        
    TPtrC8 idPayloadData = aIdentity.PayloadData();
    TUint32 length = idPayloadData.Length() + KIkePayloadGenericHeaderLength;

    iPayloadData = HBufC8::NewL(length);
    TPtr8 payloadDataPtr(iPayloadData->Des());
    payloadDataPtr.SetLength(KIkePayloadGenericHeaderLength);    
    payloadDataPtr.FillZ();
    
    SetPayloadLength(length);

    payloadDataPtr.Append(idPayloadData);
    
    __ASSERT_DEBUG(PayloadLength() == iPayloadData->Length(), User::Invariant());
    }


CIkevV2IdiPayload* CIkevV2IdiPayload::NewL(const CIkeV2Identity& aIdentity)
    {    
    CIkevV2IdiPayload* self = new (ELeave) CIkevV2IdiPayload;
    CleanupStack::PushL(self);
    self->ConstructL(aIdentity);
    CleanupStack::Pop(self);
    return self;
    }


CIkevV2IdiPayload::CIkevV2IdiPayload()
:CIkevV2IdPayload(IKEV2_PAYLOAD_ID_I)
    {  
    }


CIkevV2IdrPayload* CIkevV2IdrPayload::NewL(const CIkeV2Identity& aIdentity)
    {    
    CIkevV2IdrPayload* self = new (ELeave) CIkevV2IdrPayload;
    CleanupStack::PushL(self);
    self->ConstructL(aIdentity);
    CleanupStack::Pop(self);
    return self;    
    }


CIkevV2IdrPayload::CIkevV2IdrPayload()
:CIkevV2IdPayload(IKEV2_PAYLOAD_ID_R)
    {    
    }


CIkeV2AuthPayload* CIkeV2AuthPayload::NewL(TUint8 aAuthMethod, const TDesC8& aAuthData)
    {
    CIkeV2AuthPayload* self = new (ELeave) CIkeV2AuthPayload;
    CleanupStack::PushL(self);
    self->ConstructL(aAuthMethod, aAuthData);
    CleanupStack::Pop(self);
    
    return self;
    }


CIkeV2AuthPayload::CIkeV2AuthPayload()
:CIkeV2Payload(IKEV2_PAYLOAD_AUTH)
    {
    }


void CIkeV2AuthPayload::ConstructL(TUint8 aAuthMethod, const TDesC8& aAuthData)
    {
    static const TUint8 KReservedFieldLength = 3;
    
    TUint32 length = aAuthData.Length() + KAuthHeaderLength;

    iPayloadData = HBufC8::NewL(length);
    TPtr8 payloadDataPtr(iPayloadData->Des());
    payloadDataPtr.SetLength(KIkePayloadGenericHeaderLength);    
    payloadDataPtr.FillZ();
    
    SetPayloadLength(length);
    
    TPtrC8 authTypePtr(&aAuthMethod, sizeof(aAuthMethod));
    payloadDataPtr.Append(authTypePtr);
    //Leave reserved bytes zero
    payloadDataPtr.SetLength(payloadDataPtr.Length() + KReservedFieldLength);    
    TPtr8 reservedField = payloadDataPtr.RightTPtr(KReservedFieldLength);
    reservedField.FillZ();
    
    payloadDataPtr.Append(aAuthData);   
    
    __ASSERT_DEBUG(PayloadLength() == iPayloadData->Length(), User::Invariant());
    }


CIkeV2NotifyPayload* CIkeV2NotifyPayload::NewL(TUint8 aProtocolId,
                                               const TDesC8& aSpi,
                                               TUint16 aNotifyType,
                                               const TDesC8& aNotifyData)
    {    
    CIkeV2NotifyPayload* self = new (ELeave) CIkeV2NotifyPayload;
    CleanupStack::PushL(self);
    self->ConstructL(aProtocolId, aSpi, aNotifyType, aNotifyData);
    CleanupStack::Pop(self);
    
    return self;
    }


CIkeV2NotifyPayload::CIkeV2NotifyPayload()
:CIkeV2Payload(IKEV2_PAYLOAD_NOTIF)
    {
    }


void CIkeV2NotifyPayload::ConstructL(TUint8 aProtocolId,
                                     const TDesC8& aSpi,
                                     TUint16 aNotifyType,
                                     const TDesC8& aNotifyData)
    {    
    TUint32 length = aSpi.Length() + aNotifyData.Length() + KNotifyHeaderLength;

    iPayloadData = HBufC8::NewL(length);
    TPtr8 payloadDataPtr(iPayloadData->Des());
    payloadDataPtr.SetLength(KIkePayloadGenericHeaderLength);    
    payloadDataPtr.FillZ();
    
    SetPayloadLength(length);

    TPtrC8 protocolIdPtr(&aProtocolId, sizeof(aProtocolId));
    TUint8 spiSize = aSpi.Length();
    TPtrC8 spiSizePtr(&spiSize, sizeof(spiSize));
    
    BigEndian::Put16(reinterpret_cast<TUint8*>(&aNotifyType), aNotifyType);
    TPtrC8 notifyTypePtr(reinterpret_cast<TUint8*>(&aNotifyType), sizeof(aNotifyType));
    
    
    payloadDataPtr.Append(protocolIdPtr);
    payloadDataPtr.Append(spiSizePtr);
    payloadDataPtr.Append(notifyTypePtr);
    payloadDataPtr.Append(aSpi);
    payloadDataPtr.Append(aNotifyData);  
    
    __ASSERT_DEBUG(PayloadLength() == iPayloadData->Length(), User::Invariant());
    }


CIkeV2ConfigurationPayload* CIkeV2ConfigurationPayload::NewL(TUint8 aCfgType, 
                                                             const TDesC8& aConfigurationData)
    {
    CIkeV2ConfigurationPayload* self = new (ELeave) CIkeV2ConfigurationPayload;
    CleanupStack::PushL(self);
    self->ConstructL(aCfgType, aConfigurationData);
    CleanupStack::Pop(self);
    
    return self;
    }


CIkeV2ConfigurationPayload::CIkeV2ConfigurationPayload()
: CIkeV2Payload(IKEV2_PAYLOAD_CONFIG)
    {    
    }


void CIkeV2ConfigurationPayload::ConstructL(TUint8 aCfgType, 
                                            const TDesC8& aConfigurationData)
    {
    static const TUint8 KReservedFieldLength = 3;
    
    TUint32 length = aConfigurationData.Length() + KConfigurationHeaderLength;

    iPayloadData = HBufC8::NewL(length);
    TPtr8 payloadDataPtr(iPayloadData->Des());
    payloadDataPtr.SetLength(KIkePayloadGenericHeaderLength);    
    payloadDataPtr.FillZ();
    
    SetPayloadLength(length);

    TPtrC8 cfgTypePtr(reinterpret_cast<TUint8*>(&aCfgType), sizeof(aCfgType));
    payloadDataPtr.Append(cfgTypePtr);
    
    //Leave reserved bytes zero
    payloadDataPtr.SetLength(payloadDataPtr.Length() + KReservedFieldLength);    
    TPtr8 reservedField = payloadDataPtr.RightTPtr(KReservedFieldLength);
    reservedField.FillZ();
    
    payloadDataPtr.Append(aConfigurationData);  

    __ASSERT_DEBUG(PayloadLength() == iPayloadData->Length(), User::Invariant());
    }


CIkeV2VendorIdPayload* CIkeV2VendorIdPayload::NewL(const TDesC8& aVendorIdData)
    {
    CIkeV2VendorIdPayload* self = new (ELeave) CIkeV2VendorIdPayload;
    CleanupStack::PushL(self);
    self->ConstructL(aVendorIdData);
    CleanupStack::Pop(self);
    
    return self;
    }


CIkeV2VendorIdPayload::CIkeV2VendorIdPayload()
:CIkeV2Payload(IKEV2_PAYLOAD_VID)
    {    
    }


void CIkeV2VendorIdPayload::ConstructL(const TDesC8& aVendorIdData)
    {        
    TUint32 length = aVendorIdData.Length() + KVendorIdHeaderLength;

    iPayloadData = HBufC8::NewL(length);
    TPtr8 payloadDataPtr(iPayloadData->Des());
    payloadDataPtr.SetLength(KIkePayloadGenericHeaderLength);
    payloadDataPtr.FillZ();
    
    SetPayloadLength(length);
    
    payloadDataPtr.Append(aVendorIdData);    
    
    __ASSERT_DEBUG(PayloadLength() == iPayloadData->Length(), User::Invariant());
    }


CIkeV2DeletePayload* CIkeV2DeletePayload::NewL(TUint8 aProtocolId, 
                                              const CDesC8Array& aSpiList)
    {
    CIkeV2DeletePayload* self = new (ELeave) CIkeV2DeletePayload;
    CleanupStack::PushL(self);
    self->ConstructL(aProtocolId, aSpiList);
    CleanupStack::Pop(self);
    
    return self;
    }


CIkeV2DeletePayload::CIkeV2DeletePayload()
:CIkeV2Payload(IKEV2_PAYLOAD_DELETE)
    {    
    }


void CIkeV2DeletePayload::ConstructL(TUint8 aProtocolId, const CDesC8Array& aSpiList)
    {
    TUint16 spiCount = aSpiList.Count();
    TUint8 spiLength = (spiCount > 0) ? aSpiList[0].Length() : 0; 
    TUint32 length = KDeleteHeaderLength + (spiCount * spiLength);
    
    iPayloadData = HBufC8::NewL(length);
    TPtr8 payloadDataPtr(iPayloadData->Des());
    payloadDataPtr.SetLength(KIkePayloadGenericHeaderLength);  
    payloadDataPtr.FillZ();
    
    SetPayloadLength(length);

    TPtrC8 protocolIdPtr(&aProtocolId, sizeof(aProtocolId));
    TPtrC8 spiLengthPtr(&spiLength, sizeof(spiLength));
    
    BigEndian::Put16(reinterpret_cast<TUint8*>(&spiCount), spiCount);    
    TPtrC8 spiCountPtr(reinterpret_cast<TUint8*>(&spiCount), sizeof(spiCount));
    
    payloadDataPtr.Append(protocolIdPtr);
    payloadDataPtr.Append(spiLengthPtr);
    payloadDataPtr.Append(spiCountPtr);
    
    for (TInt i = 0; i < aSpiList.Count(); ++i)
        {
        const TDesC8& spi = aSpiList[i];
        __ASSERT_DEBUG(spi.Length() == spiLength, User::Invariant());
        payloadDataPtr.Append(spi);
        }
    
    __ASSERT_DEBUG(PayloadLength() == iPayloadData->Length(), User::Invariant());
    }


CIkeV2EapPayload* CIkeV2EapPayload::NewL(const TDesC8& aEapData)
    {
    CIkeV2EapPayload* self = new (ELeave) CIkeV2EapPayload;
    CleanupStack::PushL(self);
    self->ConstructL(aEapData);
    CleanupStack::Pop(self);
    
    return self;
    }


CIkeV2EapPayload::CIkeV2EapPayload()
:CIkeV2Payload(IKEV2_PAYLOAD_EAP)
    {    
    }


void CIkeV2EapPayload::ConstructL(const TDesC8& aEapData)
    {       
    TUint32 length = aEapData.Length() + KEapHeaderLength;

    iPayloadData = HBufC8::NewL(length);
    TPtr8 payloadDataPtr(iPayloadData->Des());    
    payloadDataPtr.SetLength(KIkePayloadGenericHeaderLength);
    payloadDataPtr.FillZ();
    SetPayloadLength(length);

    payloadDataPtr.Append(aEapData);
    
    __ASSERT_DEBUG(PayloadLength() == iPayloadData->Length(), User::Invariant());
    }



CIkeV2TsPayload::CIkeV2TsPayload(TUint aPayloadType)
:CIkeV2Payload(aPayloadType)
    {    
    }


void CIkeV2TsPayload::ConstructL(const CArrayFix<TIkeV2TrafficSelector>& aTsList)
    {       
    //selector format: 
    //                       1                   2                   3
    //   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //   !   TS Type     !IP Protocol ID*|       Selector Length         |
    //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //   |           Start Port*         |           End Port*           |
    //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //   !                                                               !
    //   ~                         Starting Address*                     ~
    //   !                                                               !
    //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //   !                                                               !
    //   ~                         Ending Address*                       ~
    //   !                                                               !
    //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+        
    const TUint16 KIpv4SelectorLength = 2*4 + 2*4; //fixed selector header + 2*IPv4 address length
    const TUint16 KIpv6SelectorLength = 2*4 + 2*16;//fixed selector header + 2*IPv6 address length   
    
    TUint8 tsCount = aTsList.Count();
    __ASSERT_DEBUG(tsCount > 0, User::Invariant());

    
    TUint32 length = KTsHeaderLength;
    
    TUint i;
    for (i = 0; i < aTsList.Count(); ++i)
        {
        if (aTsList[i].Type() == TS_IPV4_ADDR_RANGE)          
            {
            length += KIpv4SelectorLength; //fixed selector header + 2*IPv4 address length
            }
        else
            {
            __ASSERT_DEBUG(aTsList[i].Type() == TS_IPV6_ADDR_RANGE, User::Invariant());
            length += KIpv6SelectorLength; //fixed selector header + 2*IPv6 address length       
            }
        }

    iPayloadData = HBufC8::NewL(length);
    TPtr8 payloadDataPtr(iPayloadData->Des());
    payloadDataPtr.SetLength(KIkePayloadGenericHeaderLength);  
    payloadDataPtr.FillZ();
    
    SetPayloadLength(length);
    
    const TPtrC8 tsCountPtr(&tsCount, sizeof(tsCount));
    payloadDataPtr.Append(tsCountPtr);
    
    _LIT(KReservedField, "\0\0\0");
    payloadDataPtr.Append(KReservedField);
    
    for (i = 0; i < aTsList.Count(); ++i)
        {
        TBuf8<KIpv6SelectorLength> selector;
        selector.Zero();
        const TIkeV2TrafficSelector& selectorData = aTsList[i]; 
        TUint8 type = selectorData.Type();
        selector.Append(&type, sizeof(type));
        
        TUint8 protocol = selectorData.ProtocolId();
        selector.Append(&protocol, sizeof(protocol));
        
        TUint16 selectorLength = 0;
        if (selectorData.Type() == TS_IPV4_ADDR_RANGE)          
             {
             BigEndian::Put16(reinterpret_cast<TUint8*>(&selectorLength), KIpv4SelectorLength);
             }
         else
             {
             BigEndian::Put16(reinterpret_cast<TUint8*>(&selectorLength), KIpv6SelectorLength);
             }  
        selector.Append(reinterpret_cast<TUint8*>(&selectorLength), sizeof(selectorLength));
        
        TInetAddr startAddress = selectorData.StartingAddress();
        TInetAddr endAddress = selectorData.EndingAddress();
        
        TUint16 startPort = startAddress.Port(); 
        TUint16 endPort = endAddress.Port();
        
        BigEndian::Put16(reinterpret_cast<TUint8*>(&startPort), startPort);
        BigEndian::Put16(reinterpret_cast<TUint8*>(&endPort), endPort);
        
        selector.Append(reinterpret_cast<TUint8*>(&startPort), sizeof(startPort));
        selector.Append(reinterpret_cast<TUint8*>(&endPort), sizeof(endPort));
        
        if (selectorData.Type() == TS_IPV4_ADDR_RANGE)          
             {
             TUint32 start = 0;
             TUint32 end = 0;
             BigEndian::Put32(reinterpret_cast<TUint8*>(&start), startAddress.Address());
             BigEndian::Put32(reinterpret_cast<TUint8*>(&end), endAddress.Address());
             
             selector.Append(reinterpret_cast<TUint8*>(&start), sizeof(start));
             selector.Append(reinterpret_cast<TUint8*>(&end), sizeof(end));
             }
         else
             {
             TPtrC8 start(&startAddress.Ip6Address().u.iAddr8[0], 16);
             TPtrC8 end(&endAddress.Ip6Address().u.iAddr8[0], 16);
             selector.Append(start);
             selector.Append(end);             
             }   
        payloadDataPtr.Append(selector);
        }    
    
    __ASSERT_DEBUG(PayloadLength() == iPayloadData->Length(), User::Invariant());
    }


CIkeV2TsiPayload* CIkeV2TsiPayload::NewL(const CArrayFix<TIkeV2TrafficSelector>& aTsList)
    {
    CIkeV2TsiPayload* self = new (ELeave) CIkeV2TsiPayload();
    CleanupStack::PushL(self);
    self->ConstructL(aTsList);
    CleanupStack::Pop(self);
    
    return self;
    }


CIkeV2TsiPayload::CIkeV2TsiPayload()
:CIkeV2TsPayload(IKEV2_PAYLOAD_TS_I)
    {
    
    }


CIkeV2TsrPayload* CIkeV2TsrPayload::NewL(const CArrayFix<TIkeV2TrafficSelector>& aTsList)
    {
    CIkeV2TsrPayload* self = new (ELeave) CIkeV2TsrPayload();
    CleanupStack::PushL(self);
    self->ConstructL(aTsList);
    CleanupStack::Pop(self);
    
    return self;    
    }


CIkeV2TsrPayload::CIkeV2TsrPayload()
:CIkeV2TsPayload(IKEV2_PAYLOAD_TS_R)
    {    
    }


CIkeV2EncryptedPayload* CIkeV2EncryptedPayload::NewL(TUint aBlockSize)
    {
    CIkeV2EncryptedPayload* self = new (ELeave) CIkeV2EncryptedPayload;
    CleanupStack::PushL(self);
    self->ConstructL(aBlockSize);
    CleanupStack::Pop(self);
    
    return self;
    }


CIkeV2EncryptedPayload::CIkeV2EncryptedPayload()
:CIkeV2TsPayload(IKEV2_PAYLOAD_ENCR)
    {    
    }


void CIkeV2EncryptedPayload::ConstructL(TUint aBlockSize)
    {
    TUint32 length = aBlockSize + KEncryptedHeaderLength;

    iPayloadData = HBufC8::NewL(length);
    TPtr8 payloadDataPtr(iPayloadData->Des());
    payloadDataPtr.SetLength(KIkePayloadGenericHeaderLength);    
    payloadDataPtr.FillZ();
    
    SetPayloadLength(length);
    payloadDataPtr.SetLength(length);
    
    TPtr8 Iv = payloadDataPtr.MidTPtr(KEncryptedHeaderLength);
    Iv.SetLength(aBlockSize);
    TRandom::RandomL(Iv);        
    
    iBlockSize = aBlockSize;
    
    __ASSERT_DEBUG(PayloadLength() == iPayloadData->Length(), User::Invariant());
    }

TUint CIkeV2EncryptedPayload::BlockSize() const
    {
    return iBlockSize;
    }

TPtrC8 CIkeV2EncryptedPayload::InitializationVector() const
    {
    return iPayloadData->Right(iBlockSize);
    }

void CIkeV2EncryptedPayload::SetContentLength(TUint16 aLength)
    {
    //Set the payload length to be: header + Iv + content
    SetPayloadLength(aLength + iPayloadData->Length());
    }

static const TUint   KDefaultMessageSize = 4096;
static const TUint32 KIkeV2MsgHeaderLength = 28;
static const TUint8  KIkeV2Version = 2 << 4;

CIkeV2Message* CIkeV2Message::NewL(const TDesC8& aInitiatorSpi,
                                   const TDesC8& aResponderSpi,
                                   TUint8 aExchangeType,
                                   TBool aIntiator,
                                   TBool aResponse,
                                   TUint32 aMessageId,
                                   MIkeDebug& aDebug)
    {
    CIkeV2Message* self = new (ELeave)CIkeV2Message(aDebug);
    CleanupStack::PushL(self);
    self->ConstructL(aInitiatorSpi,
                     aResponderSpi,
                     aExchangeType,
                     aIntiator,
                     aResponse,
                     aMessageId);
    CleanupStack::Pop(self);
    
    return self;
    }


CIkeV2Message::CIkeV2Message(MIkeDebug& aDebug)
:iDebug(aDebug), iModified(ETrue)
    {    
    }


void CIkeV2Message::ConstructL(const TDesC8& aInitiatorSpi,
                               const TDesC8& aResponderSpi,
                               TUint8 aExchangeType,   
                               TBool aIntiator,
                               TBool aResponse,
                               TUint32 aMessageId)
    {
    __ASSERT_DEBUG(aInitiatorSpi.Length() == 8, User::Invariant());
    __ASSERT_DEBUG(aResponderSpi.Length() == 8, User::Invariant());
    
    iIkeV2MessageHeader = HBufC8::NewL(KDefaultMessageSize);    
    TPtr8 messageDataPtr = iIkeV2MessageHeader->Des();
    
   
    const TPtrC8 versionPtr(&KIkeV2Version, sizeof(TUint8));    
    const TPtrC8 exchangeTypePtr(&aExchangeType, sizeof(TUint8));
    
    TUint8 flags = (aIntiator) ? IKEV2_INITIATOR : 0;
    flags |= (aResponse) ? IKEV2_RESPONSE_MSG : 0; 
    
    
    const TPtrC8 flagsPtr(&flags, sizeof(flags));
    BigEndian::Put32(reinterpret_cast<TUint8*>(&aMessageId), aMessageId);
    const TPtrC8 messageIdPtr(reinterpret_cast<TUint8*>(&aMessageId), sizeof(TUint32));
    
    TUint32 length;
    BigEndian::Put32(reinterpret_cast<TUint8*>(&length), KIkeV2MsgHeaderLength);
    const TPtrC8 lengthPtr(reinterpret_cast<TUint8*>(&length), sizeof(TUint32));
    
    //Add SPIs
    messageDataPtr.Append(aInitiatorSpi);
    messageDataPtr.Append(aResponderSpi);
    
    //Left next payload as zero
    TUint8 nextPayload = 0;
    TPtrC8 nextPayloadPtr(&nextPayload, sizeof(nextPayload));
    messageDataPtr.Append(nextPayloadPtr);
    
    messageDataPtr.Append(versionPtr);
    messageDataPtr.Append(exchangeTypePtr);
    messageDataPtr.Append(flagsPtr);
    messageDataPtr.Append(messageIdPtr);
    messageDataPtr.Append(lengthPtr);
    
    __ASSERT_DEBUG(iIkeV2MessageHeader->Length() == KIkeV2MsgHeaderLength, User::Invariant());
    }


CIkeV2Message::~CIkeV2Message()
    {
    delete iIkeV2Datagram;
    delete iIkeV2MessageHeader;
    iPayloads.ResetAndDestroy();
    iPayloads.Close();
    }


TPtrC8 CIkeV2Message::InitiatorSpi()const
    {
    TUint KInitiatorSpiPosition = 0;
    TUint KSpiLength = 8;
    
    return iIkeV2MessageHeader->Mid(KInitiatorSpiPosition, KSpiLength);
    }


TPtrC8 CIkeV2Message::ResponderSpi() const
    {
    TUint KResponderSpiPosition = 8;
    TUint KSpiLength = 8;
    
    return iIkeV2MessageHeader->Mid(KResponderSpiPosition, KSpiLength);
    }


TUint8 CIkeV2Message::Flags() const
    {
    TUint KFlagsPosition = 19;
    return (*iIkeV2MessageHeader)[KFlagsPosition];
    }


TUint32 CIkeV2Message::MessageId() const
    {
    __ASSERT_DEBUG(iIkeV2MessageHeader->Length() >= KIkeV2MsgHeaderLength, User::Invariant());
    const TUint8* messageIdPtr = iIkeV2MessageHeader->Ptr() + KMessageIdFieldOffset; 
    return BigEndian::Get32(messageIdPtr);
    
    }


void CIkeV2Message::AppendCertReqPayloadL(const CIkeCaList& aCaList)
    {
    __ASSERT_DEBUG(aCaList.Count() > 0, User::Invariant());

    CIkevV2CertReqPayload* certReqPayload = CIkevV2CertReqPayload::NewL(aCaList);
    AppendPayloadL(certReqPayload);
    }


void CIkeV2Message::AppendCertPayloadL(const TDesC8& aCertificateData)
    {
    CIkevV2CertPayload* certPayload = CIkevV2CertPayload::NewL(aCertificateData);
    AppendPayloadL(certPayload);
    }


void CIkeV2Message::AppendSaPayloadL(const TDesC8& aSaData)
    {
    CIkevV2SaPayload* saPayload = CIkevV2SaPayload::NewL(aSaData);
    AppendPayloadL(saPayload);
    }


void CIkeV2Message::AppendKePayloadL(TUint16 aDHGroup, const TDesC8& aKeData)
    {
    CIkevV2KePayload* kePayload = CIkevV2KePayload::NewL(aDHGroup, aKeData);
    AppendPayloadL(kePayload);
    }


void CIkeV2Message::AppendNoncePayloadL(const TDesC8& aNonceData)
    {
    CIkevV2NoncePayload* noncePayload = CIkevV2NoncePayload::NewL(aNonceData);
    AppendPayloadL(noncePayload);
    }


void CIkeV2Message::AppendIdiPayloadL(const CIkeV2Identity& aIdentity)
    {
    CIkevV2IdiPayload* idiPayload = CIkevV2IdiPayload::NewL(aIdentity);  
    AppendPayloadL(idiPayload);
    }


void CIkeV2Message::AppendIdrPayloadL(const CIkeV2Identity& aIdentity)
    {
    CIkevV2IdrPayload* idrPayload = CIkevV2IdrPayload::NewL(aIdentity);  
    AppendPayloadL(idrPayload);    
    }

void CIkeV2Message::AppendAuthPayloadL(TUint8 aAuthMethod, const TDesC8& aAuthData)
    {
    CIkeV2AuthPayload* authPayload = CIkeV2AuthPayload::NewL(aAuthMethod, aAuthData);
    AppendPayloadL(authPayload);
    }


void CIkeV2Message::AppendNotifyPayloadL(TUint8 aProtocolId,
                                         const TDesC8& aSpi,
                                         TUint16 aNotifyType,
                                         const TDesC8& aNotifyData)
    {
    CIkeV2NotifyPayload* notifyPayload = CIkeV2NotifyPayload::NewL(aProtocolId, aSpi,
                                                                   aNotifyType, aNotifyData);
    AppendPayloadL(notifyPayload);
    }

void CIkeV2Message::PrependCookieNotifyPayloadL(TUint8 aProtocolId, const TDesC8& aCookieData)
    {
    _LIT8(KZeroDesc, "");
    CIkeV2NotifyPayload* notifyPayload = CIkeV2NotifyPayload::NewL(aProtocolId, KZeroDesc,
                                                                   COOKIE, aCookieData);
    
    delete iIkeV2Datagram;
    iIkeV2Datagram = NULL;    
    iModified = ETrue;

    if (iPayloads.Count() > 0)
        {
        notifyPayload->SetNextPayload(iPayloads[0]->PayloadType());
        }
    
    TInt err = iPayloads.Insert(notifyPayload, 0);
    if (err != KErrNone)
        {
        delete notifyPayload;
        User::Leave(err);
        }
    
    SetNextPayload(notifyPayload->PayloadType());    
    }


void CIkeV2Message::AppendConfigurationPayloadL(TUint8 aCfgType, 
                                               const TDesC8& aConfigurationData)
    {
    CIkeV2ConfigurationPayload* configPayload = 
                        CIkeV2ConfigurationPayload::NewL(aCfgType, aConfigurationData);
    AppendPayloadL(configPayload);
    }


void CIkeV2Message::AppendVendorIdPayloadL(const TDesC8& aVendorIdData)
    {
    CIkeV2VendorIdPayload* vendorIdPayload = CIkeV2VendorIdPayload::NewL(aVendorIdData);
    AppendPayloadL(vendorIdPayload);
    }

void CIkeV2Message::AppendDeletePayloadL(TUint8 aProtocolId, const CDesC8Array& aSpiList)
    {
    CIkeV2DeletePayload* deletePayload = CIkeV2DeletePayload::NewL(aProtocolId, aSpiList);
    AppendPayloadL(deletePayload);
    }


void CIkeV2Message::AppendEapPayloadL(const TDesC8& aEapData)
    {
    CIkeV2EapPayload* eapPayload = CIkeV2EapPayload::NewL(aEapData);
    AppendPayloadL(eapPayload);
    }


void CIkeV2Message::AppendTsiPayloadL(const CArrayFix<TIkeV2TrafficSelector>& aTsList)
    {
    CIkeV2TsiPayload* tsPayload = CIkeV2TsiPayload::NewL(aTsList);
    AppendPayloadL(tsPayload);
    }


void CIkeV2Message::AppendTsrPayloadL(const CArrayFix<TIkeV2TrafficSelector>& aTsList)
    {
    CIkeV2TsrPayload* tsPayload = CIkeV2TsrPayload::NewL(aTsList);
    AppendPayloadL(tsPayload);    
    }


void CIkeV2Message::AppendEncryptedPayloadL(TUint aBlockSize)
    {
    __ASSERT_DEBUG(iPayloads.Count() == 0, User::Invariant());
    CIkeV2EncryptedPayload* encryptedPayload = CIkeV2EncryptedPayload::NewL(aBlockSize);
    AppendPayloadL(encryptedPayload);
    }

void CIkeV2Message::PrepareIkeMessageDatagramL(TUint16 aEncryptionAlgorith, 
                                               const TDesC8& aEncryptionKey,
                                               TUint16 aIntegrityAlgorithm,
                                               const TDesC8& aIntegrityKey,
#ifdef _DEBUG                                               
                                               const TInetAddr& aSourceAddress,
#else
                                               const TInetAddr& /*aSourceAddress*/,
#endif                                               
                                               const TInetAddr& aDestinationAddress)
    {    
    __ASSERT_DEBUG(iPayloads.Count() > 0, User::Invariant());
    
    if (iModified)
        {        
        __ASSERT_DEBUG(iIkeV2Datagram == NULL, User::Invariant());
        
        if (iPayloads[0]->PayloadType() == IKEV2_PAYLOAD_ENCR)
            {
            //Datagram is should be encrypted
            //Calculate the length of the padding
            CIkeV2EncryptedPayload* encryptedPayload = static_cast<CIkeV2EncryptedPayload*>(iPayloads[0]);
            TUint encryptedDataLength = 0;
            for(TInt i = 1; i < iPayloads.Count(); ++i)
                {
                encryptedDataLength += iPayloads[i]->PayloadLength();
                }
            
            //If the data length is multiple of the blocksize, we add full block length
            //of padding. Otherwise we just add padding enough to fill the block.
            TUint8 paddingLength = encryptedPayload->BlockSize() -
                                   encryptedDataLength % encryptedPayload->BlockSize();
            //The last octet of the padding tells the length of the padding.
            //we just use that value to fill the entire padding.
            TInt integrityCheckSumLength = 0;
            IkeCrypto::AlgorithmInfo(IKEV2_INTEG, aIntegrityAlgorithm, &integrityCheckSumLength);
            
            
            //The length of the whole datagram:
            TUint32 datagramLength = iIkeV2MessageHeader->Length() +
                                     encryptedPayload->PayloadLength() +
                                     encryptedDataLength + 
                                     paddingLength +
                                     integrityCheckSumLength;
    
            //Update header fields
            SetLength(datagramLength);
            encryptedPayload->SetContentLength((TUint16)(encryptedDataLength + 
                                                         paddingLength + 
                                                         integrityCheckSumLength));
    
            //Allocate buffer, which has space for the whole datagram. (+ Non ESP marker)
            HBufC8* datagram = HBufC8::NewLC(datagramLength + KNonEspMarker().Length());
            TPtr8 datagramPtr = datagram->Des();
            
            datagramPtr = *iIkeV2MessageHeader;
            datagramPtr.Append(encryptedPayload->PayloadData());
            
            //buffer for data, which is encrypted
            HBufC8* encryptionSource = HBufC8::NewLC(encryptedDataLength + 
                                                     paddingLength);
            TPtr8 encryptionSourcePtr = encryptionSource->Des();
            
            for (TInt i = 1; i < iPayloads.Count(); ++i)
                {
                const CIkeV2Payload* pl = iPayloads[i];                 
                __ASSERT_DEBUG(pl->PayloadData().Length() == pl->PayloadLength(), User::Invariant());
                
                encryptionSourcePtr.Append(pl->PayloadData());                
                datagramPtr.Append(pl->PayloadData()); //This is because we want to trace the datagram              
                }
             
            
            //Last byte of the padding has to be the length of the padding.
            //We fillup the whole padding with this same number
            TUint8 paddingValue = paddingLength - 1;
            for (TInt i = 0; i < paddingLength; ++i)
                {
                encryptionSourcePtr.Append(&paddingValue, 1);                
                datagramPtr.Append(&paddingValue, 1);
                }
            
            
            datagramPtr.SetLength(datagram->Length() + integrityCheckSumLength);
            TRACE_MSG(*datagram, aSourceAddress, aDestinationAddress, 
                      (CIkePcapTrace::TEncryptionType)aEncryptionAlgorith);
            datagramPtr.SetLength(datagram->Length() - integrityCheckSumLength);
            
            //Extracts the data, which is encrypted. 
            //(Excludes IKE hdr, Encrypted payload hdr and Iv)
            TPtr8 encryptionBuffer = datagramPtr.MidTPtr(iIkeV2MessageHeader->Length() + 
                                                         KEncryptedHeaderLength + 
                                                         encryptedPayload->BlockSize());
            __ASSERT_DEBUG(encryptionBuffer.Length() == encryptionSource->Length(), User::Invariant());
            encryptionBuffer.SetLength(0);
            IkeCrypto::EncryptL(*encryptionSource, encryptionBuffer, 
                                encryptedPayload->InitializationVector(), 
                                aEncryptionKey, aEncryptionAlgorith);
            
            CleanupStack::PopAndDestroy(encryptionSource);
            
            //Extracts the space for the checksum from the end of the buffer
            TUint lengthWithoutItegrityCheckSum = datagramPtr.Length();
            datagramPtr.SetLength(lengthWithoutItegrityCheckSum + integrityCheckSumLength);
            TPtr8 checksum = datagramPtr.MidTPtr(lengthWithoutItegrityCheckSum);           
            
            //Extracts the source for the integrity checksum calculation
            TPtrC8 integrityCheckSumSource = datagram->Left(lengthWithoutItegrityCheckSum);            
            IkeCrypto::IntegHMACL(integrityCheckSumSource, checksum, aIntegrityKey, aIntegrityAlgorithm);
                        
            CleanupStack::Pop(datagram);
            iIkeV2Datagram = datagram;             
            }    
        else
            {
            //calculate the length of unencrypted datagram
            TUint datagramLength = iIkeV2MessageHeader->Length();
            for (TInt i = 0; i < iPayloads.Count(); ++i)
                {
                datagramLength += iPayloads[i]->PayloadLength();         
                }
            SetLength(datagramLength);
            
            iIkeV2Datagram = HBufC8::NewL(datagramLength + KNonEspMarker().Length());
            TPtr8 ikeV2DatargramPtr = iIkeV2Datagram->Des();
            ikeV2DatargramPtr.Append(*iIkeV2MessageHeader);
            
            for (TInt i = 0; i < iPayloads.Count(); ++i)
                {
                ikeV2DatargramPtr.Append(iPayloads[i]->PayloadData());
                }
            TRACE_MSG(*iIkeV2Datagram, aSourceAddress, aDestinationAddress, 
                      (CIkePcapTrace::TEncryptionType)aEncryptionAlgorith);

            }  
        
        if (aDestinationAddress.Port() == FLOATED_IKE_PORT)
            {
            //insert non esp marker
            iIkeV2Datagram->Des().Insert(0, KNonEspMarker);
            }
        iModified = EFalse;
        }
   
    __ASSERT_DEBUG(!iModified && iIkeV2Datagram != NULL, User::Invariant());
    }


TPtrC8 CIkeV2Message::IkeMessageDatagram() const
    {
    __ASSERT_DEBUG(!iModified && iIkeV2Datagram != NULL, User::Invariant());
    return *iIkeV2Datagram;
    }


void CIkeV2Message::AppendPayloadL(CIkeV2Payload* aPayload)
    {
    TInt err = iPayloads.Append(aPayload);    
    if (err != KErrNone)
        {
        delete aPayload;
        User::Leave(err);
        }
    
    if (iPayloads.Count() > 1)
        {
        iPayloads[iPayloads.Count() - 2]->SetNextPayload(aPayload->PayloadType());
        }
    else
        {
        SetNextPayload(aPayload->PayloadType());
        }    
    
    delete iIkeV2Datagram;
    iIkeV2Datagram = NULL;    
    iModified = ETrue;
    }

void CIkeV2Message::SetLength(TUint32 aDatagramLength)
    {
    static const TUint KLengthFieldPosition = 6*4;
    BigEndian::Put32(reinterpret_cast<TUint8*>(&aDatagramLength), aDatagramLength);
    TPtr8 lengthField = iIkeV2MessageHeader->Des().MidTPtr(KLengthFieldPosition, sizeof(aDatagramLength));
    lengthField = TPtrC8(reinterpret_cast<TUint8*>(&aDatagramLength), sizeof(aDatagramLength));
    }


void CIkeV2Message::SetNextPayload(TUint8 aNextPayload)
    {
    const TUint KNextPayloadPosition =  16;
    TPtr8 ikeHeaderPtr = iIkeV2MessageHeader->Des();
    ikeHeaderPtr[KNextPayloadPosition] = aNextPayload;
    }
