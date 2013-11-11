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
* Description: Class definition of TUtlCrypto.
*
*/



#if !defined(__UTLCRYPTO_H__)
#define __UTLCRYPTO_H__

#include <e32std.h>

class CSymmetricCipher;
class CDH;
class CDHKeyPair;
class CMessageDigest;
class CDSASignature;
class TUtlCrypto;


class CUtlSymmetricCipher : public CBase
/** 
* Symmetric cipher
* @internalComponent
*/
    {
    friend class TUtlCrypto;
public:
    IMPORT_C ~CUtlSymmetricCipher();
    
    IMPORT_C void Process(const TDesC8& aInput, TDes8& aOutput);
    IMPORT_C void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput);
    IMPORT_C TInt MaxOutputLength(TInt aInputTextLength) const;
    IMPORT_C TInt MaxFinalOutputLength(TInt aInputTextLength) const;
    IMPORT_C void Reset();
    IMPORT_C TInt BlockSize() const;
    IMPORT_C TInt KeySize() const;
private:
    CUtlSymmetricCipher();
private:
    CSymmetricCipher* iSymmetricCipher;
    };


class CUtlMessageDigest : public CBase
/** 
* Message digester
* @internalComponent
*/
    {
    friend class TUtlCrypto;
public:
    IMPORT_C ~CUtlMessageDigest();

    IMPORT_C CMessageDigest* ReplicateL(void);
    IMPORT_C void Update(const TDesC8& aMessage);
    IMPORT_C TPtrC8 Final(const TDesC8& aMessage);
    IMPORT_C TPtrC8 Final(void);
    IMPORT_C CMessageDigest* CopyL(void);
    IMPORT_C TInt BlockSize(void) const;
    IMPORT_C TInt HashSize(void) const;
    IMPORT_C void Reset(void);
    
private:
    CUtlMessageDigest();
private:
    CMessageDigest* iMessageDigest;
    };


class CUtlDiffieHellman : public CBase
/** 
* Diffie-Hellman key exchange
* @internalComponent
*/
    {
    friend class TUtlCrypto;
public:
    IMPORT_C ~CUtlDiffieHellman();
    IMPORT_C const HBufC8* GenerateXL(void);
    IMPORT_C const HBufC8* CompleteKL(const TDesC8& aY);
    
private:
    CUtlDiffieHellman();
private:
    CDHKeyPair*     iDhKeyPair;
    CDH*            iDhKeyAgreement;
    TInt            iModulusLength;
    };


class TUtlCrypto
/** 
* Crypto factory
* @internalComponent
*/
    {
public:
    enum TUtlSymmetricCipherId
        { 
        EUtlSymmetricCipherDesCbc,  ///< DES
        EUtlSymmetricCipher3DesCbc, ///< 3DES
        EUtlSymmetricCipherAesCbc   ///< AES
        };
    
    enum TUtlMessageDigestId
        { 
        EUtlMessageDigestMd5,       ///< MD5
        EUtlMessageDigestSha1       ///< SHA1
        };
    
    enum TUtlCryptoVersion
        { 
        EUtlCryptoVersionOld,       ///< not Symbian crypto library
        EUtlCryptoVersionSymbian1   ///< Symbian crypto library
        };
    
public:
    IMPORT_C static CUtlSymmetricCipher* MakeSymmetricEncryptorL(TUtlSymmetricCipherId aCipherId,
                                                                 const TDesC8& aKey,
                                                                 const TDesC8& aIv=KNullDesC8);
    IMPORT_C static CUtlSymmetricCipher* MakeSymmetricDecryptorL(TUtlSymmetricCipherId aCipherId,
                                                                 const TDesC8& aKey,
                                                                 const TDesC8& aIv=KNullDesC8);

    IMPORT_C static CUtlMessageDigest* MakeMessageDigesterL(TUtlMessageDigestId aDigestId,
                                                            const TDesC8&       aHmacKey=KNullDesC8);
    
    IMPORT_C static CUtlDiffieHellman* MakeDiffieHellmanL(const TDesC8& aN, const TDesC8& aG);

    IMPORT_C static void RsaPublicKeyEncryptL(const TDesC8&    aPublicKeyData,
                                              const TDesC8&    aPlaintext,
                                              HBufC8*&         aCiphertext);

    IMPORT_C static void RsaPublicKeyDecryptL(const TDesC8&    aPublicKeyData,
                                              const TDesC8&    aCiphertext,
                                              HBufC8*&         aPlaintext);

    IMPORT_C static TBool DsaVerifySignatureL(const TDesC8&       aPublicKeyData,
                                              const TDesC8&       aDsaParams,
                                              const TDesC8&       aDsaSignatureR,
                                              const TDesC8&       aDsaSignatureS,
                                              const TDesC8&       aHashData);
    
    IMPORT_C static TBool IsWeakCryptoLibrary(void);
    
    IMPORT_C static TUtlCryptoVersion CryptoVersion(void);
    };

#endif
