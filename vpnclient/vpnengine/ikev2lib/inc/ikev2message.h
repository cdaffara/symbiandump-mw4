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
* Description:  Class to represent IKEv2 payloads and to construct IKEv2 datagrams
*
*/

#ifndef IKEV2MESSAGE_H_
#define IKEV2MESSAGE_H_

#include <e32base.h>

class CIkeCaList;
class CDesC8Array;
class TIkeV2TrafficSelector;
class CIkeV2Identity;

NONSHARABLE_CLASS(CIkeV2Payload) : public CBase
    {
public:        
    ~CIkeV2Payload();
    
    TUint8 NextPayload() const;
    void SetNextPayload(TUint8 aNextPayload);
    
    TUint16 PayloadLength() const;
    void SetPayloadLength(TUint16 aLength);

    TPtrC8 PayloadData() const;
    TUint8 PayloadType() const;
    
protected:
    CIkeV2Payload(TUint8 aPayloadType);
    
    HBufC8* iPayloadData;

private:
    TUint8 iPayloadType;
    };


NONSHARABLE_CLASS(CIkevV2CertReqPayload) : public CIkeV2Payload
    {
public:
    static CIkevV2CertReqPayload* NewL(const CIkeCaList& aCaList);   
    
private:
    CIkevV2CertReqPayload();
    void ConstructL(const CIkeCaList& aCaList);        
    };

NONSHARABLE_CLASS(CIkevV2CertPayload) : public CIkeV2Payload
    {
public:
    static CIkevV2CertPayload* NewL(const TDesC8& aCertData);   
    
private:
    CIkevV2CertPayload();
    void ConstructL(const TDesC8& aCertData);        
    };



NONSHARABLE_CLASS(CIkevV2SaPayload) : public CIkeV2Payload
    {
public:
    static CIkevV2SaPayload* NewL(const TDesC8& aSaData);   
    
private:
    CIkevV2SaPayload();
    void ConstructL(const TDesC8& aSaData);
        
    };


NONSHARABLE_CLASS(CIkevV2KePayload) : public CIkeV2Payload
    {
public:
    static CIkevV2KePayload* NewL(TUint16 aDHGroup, const TDesC8& aKeData);   
    
private:
    CIkevV2KePayload();
    void ConstructL(TUint16 aDHGroup, const TDesC8& aKeData);        
    };


NONSHARABLE_CLASS(CIkevV2NoncePayload) : public CIkeV2Payload
    {
public:
    static CIkevV2NoncePayload* NewL(const TDesC8& aNonceData);   
    
private:
    CIkevV2NoncePayload();
    void ConstructL(const TDesC8& aNonceData);        
    };


NONSHARABLE_CLASS(CIkevV2IdPayload) : public CIkeV2Payload
    {    
protected:
    CIkevV2IdPayload(TUint8 aPayloadType);
    void ConstructL(const CIkeV2Identity& aIdentity);        
    };

NONSHARABLE_CLASS(CIkevV2IdiPayload) : public CIkevV2IdPayload
    {
public:
    static CIkevV2IdiPayload* NewL(const CIkeV2Identity& aIdentity);   
    
private:
    CIkevV2IdiPayload();        
    };


NONSHARABLE_CLASS(CIkevV2IdrPayload) : public CIkevV2IdPayload
    {
public:
    static CIkevV2IdrPayload* NewL(const CIkeV2Identity& aIdentity);   
    
private:
    CIkevV2IdrPayload();        
    };


NONSHARABLE_CLASS(CIkeV2AuthPayload) : public CIkeV2Payload
    {
public:
    static CIkeV2AuthPayload* NewL(TUint8 aAuthMethod, const TDesC8& aAuthData);  
    
private:
    CIkeV2AuthPayload();
    void ConstructL(TUint8 aAuthMethod, const TDesC8& aAuthData);    
    };


NONSHARABLE_CLASS(CIkeV2NotifyPayload) : public CIkeV2Payload
    {
public:
    static CIkeV2NotifyPayload* NewL(TUint8 aProtocolId,
                                     const TDesC8& aSpi,
                                     TUint16 aNotifyType,
                                     const TDesC8& aNotifyData);  
    
private:
    CIkeV2NotifyPayload();
    void ConstructL(TUint8 aProtocolId,
                    const TDesC8& aSpi,
                    TUint16 aNotifyType,
                    const TDesC8& aNotifyData);    
    };


NONSHARABLE_CLASS(CIkeV2ConfigurationPayload) : public CIkeV2Payload
    {
public:
    static CIkeV2ConfigurationPayload* NewL(TUint8 aCfgType, 
                                            const TDesC8& aConfigurationData);      
    
private:
    CIkeV2ConfigurationPayload();
    void ConstructL(TUint8 aCfgType, 
                    const TDesC8& aConfigurationData);
    };


NONSHARABLE_CLASS(CIkeV2VendorIdPayload) : public CIkeV2Payload
    {
public:
    static CIkeV2VendorIdPayload* NewL(const TDesC8& aVendorIdData);      
    
private:
    CIkeV2VendorIdPayload();
    void ConstructL(const TDesC8& aVendorIdData);
    };


NONSHARABLE_CLASS(CIkeV2DeletePayload) : public CIkeV2Payload
    {
public:
    static CIkeV2DeletePayload* NewL(TUint8 aProtocolId, const CDesC8Array& aSpiList);      
    
private:
    CIkeV2DeletePayload();
    void ConstructL(TUint8 aProtocolId, const CDesC8Array& aSpiList);
    };


NONSHARABLE_CLASS(CIkeV2EapPayload) : public CIkeV2Payload
    {
public:
    static CIkeV2EapPayload* NewL(const TDesC8& aEapData);      
    
private:
    CIkeV2EapPayload();
    void ConstructL(const TDesC8& aEapData);
    };


NONSHARABLE_CLASS(CIkeV2TsPayload) : public CIkeV2Payload
    {            
protected:
    CIkeV2TsPayload(TUint aPayloadType);
    void ConstructL(const CArrayFix<TIkeV2TrafficSelector>& aTsList);
    };

class CIkeV2TsiPayload : public CIkeV2TsPayload
    {            
public: 
    static CIkeV2TsiPayload* NewL(const CArrayFix<TIkeV2TrafficSelector>& aTsList);
    
private:
    CIkeV2TsiPayload();
    };

NONSHARABLE_CLASS(CIkeV2TsrPayload) : public CIkeV2TsPayload
    {            
public: 
    static CIkeV2TsrPayload* NewL(const CArrayFix<TIkeV2TrafficSelector>& aTsList);
    
private:
    CIkeV2TsrPayload();
    };


NONSHARABLE_CLASS(CIkeV2EncryptedPayload) : public CIkeV2TsPayload
    {
public:
    static CIkeV2EncryptedPayload* NewL(TUint aBlockSize);
    
    TUint BlockSize() const;
    TPtrC8 InitializationVector() const;
    
    void SetContentLength(TUint16 aLength);
private:
    CIkeV2EncryptedPayload();
    void ConstructL(TUint aBlockSize);
    
    TUint iBlockSize;
    };

class MIkeDebug;
class TInetAddr;

NONSHARABLE_CLASS(CIkeV2Message) : public CBase
    {
public:
    
    /**
     *  Constructs new IKE message.
     *  
     *  @param aInitiatorSpi Initiator SPI 
     *  @param aResponderSpi Responder SPI
     *  @param aExchangeType Type of the exchange. Possible values are:
     *                       IKE_SA_INIT, IKE_AUTH, CREATE_CHILD_SA and INFORMATIONAL.
     *  @param aFlags Message flags. Possible flags are:
     *                IKEV2_INITIATOR and IKEV2_RESPONSE_MSG.                             
     */
    static CIkeV2Message* NewL(const TDesC8& aInitiatorSpi,
                               const TDesC8& aResponderSpi,
                               TUint8 aExchangeType,
                               TBool aIntiator,
                               TBool aResponse,
                               TUint32 aMessageId,
                               MIkeDebug& aDebug);
    ~CIkeV2Message();
    
    TPtrC8 InitiatorSpi()const;        
    TPtrC8 ResponderSpi() const;
    
    TUint8 NextPayload() const;
    
    TUint8 MajorVersion() const;
    TUint8 MinorVersion() const;
    
    TUint8 ExchangeType() const;
    
    TUint8 Flags() const;
        
    TUint32 MessageId() const;   
      
    TUint32 Length() const;
    
    void AppendCertReqPayloadL(const CIkeCaList& aCaList);
    void AppendCertPayloadL(const TDesC8& aCertificateData);     
    void AppendSaPayloadL(const TDesC8& aSaData);  
    void AppendKePayloadL(TUint16 aDHGroup, const TDesC8& aKeData); 
    void AppendNoncePayloadL(const TDesC8& aNonceData);
    void AppendIdiPayloadL(const CIkeV2Identity& aIdentity);
    void AppendIdrPayloadL(const CIkeV2Identity& aIdentity);
    void AppendAuthPayloadL(TUint8 aAuthMethod, const TDesC8& aAuthData);
    void AppendNotifyPayloadL(TUint8 aProtocolId,
                              const TDesC8& aSpi,
                              TUint16 aNotifyType,
                              const TDesC8& aNotifyData);

    /**
     * A special method for adding a cookie notify in the
     * beginning of an existing ike message. 
     */
    void PrependCookieNotifyPayloadL(TUint8 aProtocolId,
                                     const TDesC8& aCookieData);    
    
    void AppendConfigurationPayloadL(TUint8 aCfgType, 
                                    const TDesC8& aConfigurationData);
    void AppendVendorIdPayloadL(const TDesC8& aVendorIdData);    
    void AppendDeletePayloadL(TUint8 aProtocolId, const CDesC8Array& aSpiList);
    void AppendEapPayloadL(const TDesC8& aEapData);
    void AppendTsiPayloadL(const CArrayFix<TIkeV2TrafficSelector>& aTsList);
    void AppendTsrPayloadL(const CArrayFix<TIkeV2TrafficSelector>& aTsList);
    
    /**
     *  Adds the encrypted payload to the message.
     *  The added encrypted payload has to be the first added payload.
     */
    void AppendEncryptedPayloadL(TUint aBlockSize);
    
    
    /**
     *  Gets the ike message datagram, which is ready to be
     *  send to the receiver.  
     *  
     *  @param aEncryptionAlgorith Encryption algorithm to be used to encrypt the datagram.
     *                             If the message does not contain an encryption payload, this
     *                             parameter is ignored.
     *  @param aEncryptionKey Encryption key used to used to encrypt the datagram.
     *                        If the message does not contain an encryption payload, this
     *                        parameter is ignored.
     *  @param aIntegrityAlgorithm Algorithm used to calculate the integrity checks sum.
     *                             If the message does not contain an encryption payload, this
     *                             parameter is ignored.                
     *  @param aSourceAddress Source address of the datagram. Needed for message tracing in
     *                        debug builds.
     *  @param aDestinationAddress Destination address of the datagram. Needed for message tracing in
     *                        debug builds.                        
     */
    void PrepareIkeMessageDatagramL(TUint16 aEncryptionAlgorithm, 
                                    const TDesC8& aEncryptionKey,
                                    TUint16 aIntegrityAlgorithm,
                                    const TDesC8& aIntegrityKey,
                                    const TInetAddr& aSourceAddress,
                                    const TInetAddr& aDestinationAddress);
    
    TPtrC8 IkeMessageDatagram() const;
private:    
    CIkeV2Message(MIkeDebug& aDebug);
    void ConstructL(const TDesC8& aInitiatorSpi,
                    const TDesC8& aResponderSpi,
                    TUint8 aExchangeType,
                    TBool aIntiator,
                    TBool aResponse,
                    TUint32 aMessageId);
    
    void AppendPayloadL(CIkeV2Payload* aPayload);
    
    void SetFlags(TUint8 aFlags);
    void SetLength(TUint32 aDatagramLength);
    void SetNextPayload(TUint8 aNextPayload);
        
    
    MIkeDebug& iDebug;
    HBufC8* iIkeV2MessageHeader;
    RPointerArray<CIkeV2Payload> iPayloads;
    
    TBool   iModified;
    HBufC8* iIkeV2Datagram;
    };

#endif /* IKEV2MESSAGE_H_ */
