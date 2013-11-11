/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   New Symbian crypto API
*
*/



#define DummyForLeaveScanL(a)

#include "msymmetriccipher.h"
#include <bufferedtransformation.h>
#include <cbcmode.h>
#include <des.h>
#include <3des.h>
#include <rijndael.h>
#include <padding.h>
#include <hash.h>

#include <asymmetrickeys.h>
#include <asymmetric.h>
#include <bigint.h>
#include <random.h>
#include <cryptostrength.h>
#include <x509keys.h>
#include <x509cert.h>

#include "utlcrypto.h"



EXPORT_C CUtlSymmetricCipher::~CUtlSymmetricCipher()
/**
* Destructor.
*/
    {
    delete iSymmetricCipher;
    }
    
EXPORT_C void CUtlSymmetricCipher::Process(const TDesC8& aInput, TDes8& aOutput)
/**
* Runs the underlying transformation on aInput and appends the result to
* aOutput.
* @param aInput The input data to be processed.
* @param aOutput The resulting processed data appended to aOutput.  aOutput must
* have MaxOutputLength() empty bytes remaining in its length.
*/
    {
    iSymmetricCipher->Process(aInput, aOutput);
    }
    
EXPORT_C void CUtlSymmetricCipher::ProcessFinalL(const TDesC8& aInput, TDes8& aOutput)
/**
* Pads aInput to be block aligned using the underlying padding system, if any,
* and then runs the underlying transformation on aInput, and appends the result
* to aOutput.  
* @param aInput The input data to be processed.
* @param aOutput The resulting, possibly padded, processed data appended to
* aOutput.  aOutput must have MaxFinalOutputLength() empty bytes remaining in
* its length.
*/
    {
    //Do dot call ProcessFinalL, but Process, because CPaddingNone is used.
    //ProcessFinalL causes panic in this case.
    DummyForLeaveScanL(1);
    iSymmetricCipher->Process(aInput, aOutput);
    }
    
EXPORT_C TInt CUtlSymmetricCipher::MaxOutputLength(TInt aInputTextLength) const
/** 
* Gets a tight upper bound on the number of bytes that would be returned by a
* call to Process() with aInputLength bytes of data.
* @param aInputLength The length of data to be supplied to Process() in bytes.
* @return The length of data which would result from a call to Process() with
* an aInputLength number of bytes.
*/
    {
    return iSymmetricCipher->MaxOutputLength(aInputTextLength);
    }

EXPORT_C TInt CUtlSymmetricCipher::MaxFinalOutputLength(TInt aInputTextLength) const
/** 
* Gets as tight an upper bound as possible on the number of bytes that would
* be returned by a call to ProcessFinalL() with aInputLength bytes of data.
* @param aInputLength The length of data to be supplied to Process() in bytes.
* @return An upper bound on the length of data which would result from a call to
* ProcessFinalL() with an aInputLength number of bytes.
*/
    {
    return iSymmetricCipher->MaxFinalOutputLength(aInputTextLength);
    }

EXPORT_C void CUtlSymmetricCipher::Reset()
/**
* Resets the cipher back to its original state. Clears all its buffers.
*/
    {
    iSymmetricCipher->Reset();
    }
    
EXPORT_C TInt CUtlSymmetricCipher::BlockSize() const
/**
* Gets the block size in bytes (1 for stream ciphers).
* @return Block size of underlying cipher in bytes.
*/
    {
    return iSymmetricCipher->BlockSize();
    }
    
EXPORT_C TInt CUtlSymmetricCipher::KeySize() const
/**
* Gets the key size in bits.    
* @return Key size in bits.
*/
    {
    return iSymmetricCipher->KeySize();
    }
    
CUtlSymmetricCipher::CUtlSymmetricCipher()
    {
    ;
    }

//--------------------------------------------------------
    
EXPORT_C CUtlMessageDigest::~CUtlMessageDigest()
/**
* Destructor.
*/
    {
    delete iMessageDigest;
    }

EXPORT_C CMessageDigest* CUtlMessageDigest::ReplicateL(void)
/** 
* Creates a brand new reset CMessageDigest object containing no state
* information from the current object.  To make a copy of a message
* digest with its internal state intact, see CopyL().
*/
    {
    return iMessageDigest->ReplicateL();
    }
    
EXPORT_C void CUtlMessageDigest::Update(const TDesC8& aMessage)
/** 
* Adds data to the internal representation of messages to be hashed
* @param aMessage Data to be included in the hash.
*/
    {
    iMessageDigest->Update(aMessage);
    }

EXPORT_C TPtrC8 CUtlMessageDigest::Final(const TDesC8& aMessage)
/**
* Adds aMessage to the internal representation of data to be hashed,
* returns a TPtrC8 of the finalised hash of all the previously
* appended messages, and calls Reset().
* 
* @param aMessage Data to be included in the hash 
*
* @return TPtrC8 A descriptor pointer to the buffer containing the
* resulting hash.
*/
    {
    return iMessageDigest->Final(aMessage);
    }

EXPORT_C TPtrC8 CUtlMessageDigest::Final(void)
/**
* Returns a TPtrC8 of the finalised hash of all the previously
* appended messages and then calls Reset().
* 
* @return TPtrC8 A descriptor pointer to the buffer containing the
* resulting hash.
*/
    {
    return iMessageDigest->Final();
    }

EXPORT_C CMessageDigest* CUtlMessageDigest::CopyL(void)
/**
* Creates a new CMessageDigest object with the exact same state as
* the current object.  This function copies all internal state of the
* message digest.  To create a new CMessageDigest object without the
* state of the current object, see ReplicateL().
*/
    {
    return iMessageDigest->CopyL();
    }

EXPORT_C TInt CUtlMessageDigest::BlockSize(void) const
/** 
* Returns the internal block size of the message digest.
* @return TInt internal block size of message digest in bytes.
*/
    {
    return iMessageDigest->BlockSize();
    }

EXPORT_C TInt CUtlMessageDigest::HashSize(void) const
/** 
* Returns the size of the message digest output.
* @return TInt output size of the message digest in bytes.
*/
    {
    return iMessageDigest->HashSize();
    }

EXPORT_C void CUtlMessageDigest::Reset(void)
/**
* Resets the internal state of the message digest.  A reset hash
* object loses all internal state representing the hashed data.  A
* reset message digest is suitable to begin a new, distinct hash of
* different data.  Any previously returned TPtrC8 from a call to
* Final() remains valid until any subsequent call to Update() or
* Final().
*/
    {
    iMessageDigest->Reset();
    }

    
CUtlMessageDigest::CUtlMessageDigest()
    {
    ;
    }

//--------------------------------------------------------
    
EXPORT_C CUtlDiffieHellman::~CUtlDiffieHellman()
/**
* Destructor.
*/
    {
    delete iDhKeyPair;
    delete iDhKeyAgreement;
    //delete iKBuf;
    }

EXPORT_C const HBufC8* CUtlDiffieHellman::GenerateXL(void)
/** 
* Generates a new Diffie-Hellman key exchange.
* @return HBufC8* X.
*/
    {
    const CDHPublicKey* dhPublicKey = &iDhKeyPair->PublicKey();
    const TInteger* x = &dhPublicKey->X();
    HBufC8* resultBuf = NULL;
    HBufC8* xBuf = x->BufferLC();                   
    
    TInt padLength = iModulusLength - xBuf->Length(); 
    if ( padLength > 0 )
        {
        // Fill prepending zero bits to DH public value.        
        resultBuf = HBufC8::NewL(iModulusLength);
        TChar zero(0);   
        resultBuf->Des().AppendFill(zero, padLength);
        resultBuf->Des().Append(*xBuf);
        CleanupStack::PopAndDestroy(xBuf);
        }
    else
        {
        CleanupStack::Pop(xBuf);
        resultBuf = xBuf;
        }    
    
    return resultBuf;
    }
    
EXPORT_C const HBufC8* CUtlDiffieHellman::CompleteKL(const TDesC8& aY)
/** 
* Completes a Diffie-Hellman key exchange.
* @param aY Y.
* @return HBufC8* K.
*/
    {
    const CDHPrivateKey* privateKey = &iDhKeyPair->PrivateKey();
    
    RInteger n = RInteger::NewL(privateKey->N());
    CleanupClosePushL(n);
    RInteger g = RInteger::NewL(privateKey->G());
    CleanupClosePushL(g);
    RInteger Y = RInteger::NewL(aY);
    CleanupClosePushL(Y);
    
    CDHPublicKey* dhPublicKeyY = CDHPublicKey::NewL(n, g, Y);
    CleanupStack::Pop(3); // Y, g, n
    CleanupStack::PushL(dhPublicKeyY);

    const HBufC8* kBuf = iDhKeyAgreement->AgreeL(*dhPublicKeyY);
    
    CleanupStack::PopAndDestroy(); // dhPublicKeyY
    
    //delete iKBuf;
    //iKBuf = const_cast<HBufC8*>(kBuf);
    //return iKBuf;
    return kBuf;
    }
    
CUtlDiffieHellman::CUtlDiffieHellman()
    {
    ;
    }

//--------------------------------------------------------  
    
EXPORT_C CUtlSymmetricCipher*
TUtlCrypto::MakeSymmetricEncryptorL(TUtlSymmetricCipherId aCipherId,
                                    const TDesC8& aKey,
                                    const TDesC8& aIv)
/** 
* Makes symmetric block encryptor without padding.
* @param aCipherId Cipher id.
* @param aKey Key.
* @param aIv Initialization vector.
* @return CUtlSymmetricCipher* Pointer to symmetric cipher.
*/
    {
    CSymmetricCipher* cipher = 0;
    CBlockTransformation* block = 0;
    
    switch (aCipherId)
        {
        case EUtlSymmetricCipherDesCbc:
            block = CDESEncryptor::NewLC(aKey);
            block = CModeCBCEncryptor::NewL(block, aIv);
            CleanupStack::Pop(); //1st block owned by 2nd
            CleanupStack::PushL(block);//2nd block
            break;
        case EUtlSymmetricCipher3DesCbc:
            block = C3DESEncryptor::NewLC(aKey);
            block = CModeCBCEncryptor::NewL(block, aIv);
            CleanupStack::Pop(); //1st block owned by 2nd
            CleanupStack::PushL(block);//2nd block
            break;
        case EUtlSymmetricCipherAesCbc:
            block = CAESEncryptor::NewLC(aKey);
            block = CModeCBCEncryptor::NewL(block, aIv);
            CleanupStack::Pop(); //1st block owned by 2nd
            CleanupStack::PushL(block);//2nd block
            break;
        default:
            User::Leave(KErrGeneral);
            break;
        }

    if (cipher == 0) // it's a block cipher -> make a buffered version
        {
        CPadding* padding = CPaddingNone::NewLC();
        cipher = CBufferedEncryptor::NewL(block, padding);
        CleanupStack::Pop(); //padding - owned by cipher
        CleanupStack::Pop(); //block - owned by cipher
        }
    else
        {
        //-- it's a stream cipher -> everything is already made
        }

    CleanupStack::PushL(cipher);
    CUtlSymmetricCipher* utlCipher = new (ELeave) CUtlSymmetricCipher();
    utlCipher->iSymmetricCipher = cipher;   
    CleanupStack::Pop(); //cipher - owned by utlCipher

    return utlCipher;
    }

    
EXPORT_C CUtlSymmetricCipher*
TUtlCrypto::MakeSymmetricDecryptorL(TUtlSymmetricCipherId aCipherId,
                                    const TDesC8& aKey,
                                    const TDesC8& aIv)
/** 
* Makes symmetric block decryptor without padding.
* @param aCipherId Cipher id.
* @param aKey Key.
* @param aIv Initialization vector.
* @return CUtlSymmetricCipher* Pointer to symmetric cipher.
*/
    {
    if (aKey.Length() < 1)
    {
        User::Leave(KErrArgument);
    }
    CSymmetricCipher* cipher = 0;
    CBlockTransformation* block = 0;
    
    switch (aCipherId)
        {
        case EUtlSymmetricCipherDesCbc:
            block = CDESDecryptor::NewLC(aKey);
            block = CModeCBCDecryptor::NewL(block, aIv);
            CleanupStack::Pop(); //1st block owned by 2nd
            CleanupStack::PushL(block);//2nd block
            break;
        case EUtlSymmetricCipher3DesCbc:
            block = C3DESDecryptor::NewLC(aKey);
            block = CModeCBCDecryptor::NewL(block, aIv);
            CleanupStack::Pop(); //1st block owned by 2nd
            CleanupStack::PushL(block);//2nd block
            break;
        case EUtlSymmetricCipherAesCbc:
            block = CAESDecryptor::NewLC(aKey);
            block = CModeCBCDecryptor::NewL(block, aIv);
            CleanupStack::Pop(); //1st block owned by 2nd
            CleanupStack::PushL(block);//2nd block
            break;
        default:
            User::Leave(KErrGeneral);
            break;
        }

    if (cipher == 0) // it's a block cipher -> make a buffered version
        {
        CPadding* padding = CPaddingNone::NewLC();
        cipher = CBufferedDecryptor::NewL(block, padding);
        CleanupStack::Pop(); //padding - owned by cipher
        CleanupStack::Pop(); //block - owned by cipher
        }
    else
        {
        //-- it's a stream cipher -> everything is already made
        }

    CleanupStack::PushL(cipher);
    CUtlSymmetricCipher* utlCipher = new (ELeave) CUtlSymmetricCipher();
    utlCipher->iSymmetricCipher = cipher;   
    CleanupStack::Pop(); //cipher - owned by utlCipher

    return utlCipher;
    }

EXPORT_C CUtlMessageDigest*
TUtlCrypto::MakeMessageDigesterL(TUtlMessageDigestId aDigestId,
                                 const TDesC8&       aHmacKey)
/** 
* Makes message digester.
* @param aDigestId Digest id.
* @param aHmacKey HMAC key, if HMAC.
* @return CUtlMessageDigest* Pointer to message digester.
*/
    {
    CMessageDigest* digest = 0;
    
    switch (aDigestId)
        {
        case EUtlMessageDigestMd5:
            digest = CMD5::NewL();
            break;
        case EUtlMessageDigestSha1:
            digest = CSHA1::NewL();
            break;
        default:
            User::Leave(KErrGeneral);
            break;
        }
    CleanupStack::PushL(digest);

    if (aHmacKey.Length() > 0)
        {
        digest = CHMAC::NewL(aHmacKey, digest);
        CleanupStack::Pop(); //original digest - owned by CHMAC
        CleanupStack::PushL(digest);
        }
    
    CUtlMessageDigest* utlMessageDigest = new (ELeave) CUtlMessageDigest();
    utlMessageDigest->iMessageDigest = digest;
    CleanupStack::Pop(); //digest - owned by utlMessageDigest

    return utlMessageDigest;
    }

EXPORT_C CUtlDiffieHellman*
TUtlCrypto::MakeDiffieHellmanL(const TDesC8& aN, const TDesC8& aG)
/** 
* Makes Diffie-Hellman key exchange object.
* @param aN N.
* @param aG G.
* @return CUtlDiffieHellman* Pointer to Diffie-Hellman key exchange object.
*/
    {
    RInteger n = RInteger::NewL(aN);
    CleanupClosePushL(n);
    RInteger g = RInteger::NewL(aG);
    CleanupClosePushL(g);

    CDHKeyPair* dhKeyPair = CDHKeyPair::NewL(n, g);
    CleanupStack::PushL(dhKeyPair);
    
    CDH* dhKeyAgreement = CDH::NewLC(dhKeyPair->PrivateKey());
    
    CUtlDiffieHellman* utlDiffieHellman = new (ELeave) CUtlDiffieHellman();
    utlDiffieHellman->iDhKeyAgreement = dhKeyAgreement;
    utlDiffieHellman->iDhKeyPair = dhKeyPair;
    utlDiffieHellman->iModulusLength = aN.Length();
    CleanupStack::Pop(2); //dhKeyAgreement, dhKeyPair
    CleanupStack::Pop(2); // g, n    

    return utlDiffieHellman;
    }
    
EXPORT_C void TUtlCrypto::RsaPublicKeyEncryptL(const TDesC8&    aPublicKeyData,
                                               const TDesC8&    aPlaintext,
                                               HBufC8*&         aCiphertext)
/** 
* RSA encrypts the plain text with the public key.
* @param aPublicKeyData Public key.
* @param aPlaintext Plain text.
* @param aCiphertext Cipher text.
*/
    {
    CX509RSAPublicKey* publicKey = CX509RSAPublicKey::NewLC(aPublicKeyData);
    CRSAPKCS1v15Encryptor* rsaEncryptor = CRSAPKCS1v15Encryptor::NewLC(*publicKey);
    
    TInt publicKeySize = 2048; //publicKey->Size()
    TInt encrLth = publicKeySize / 8;
    HBufC8* ciphertext = HBufC8::NewLC(encrLth);
    TPtr8 ciphertextDesc(ciphertext->Des());

    rsaEncryptor->EncryptL(aPlaintext, ciphertextDesc);

    CleanupStack::Pop();            //ciphertext
    CleanupStack::PopAndDestroy(2); //rsaEncryptor, publicKey
    aCiphertext = ciphertext;
    }

EXPORT_C void TUtlCrypto::RsaPublicKeyDecryptL(const TDesC8&    aPublicKeyData,
                                               const TDesC8&    aCiphertext,
                                               HBufC8*&         aPlaintext)
/** 
* RSA decrypts the cipher text with the public key.
* @param aPublicKeyData Public key.
* @param aCiphertext Cipher text.
* @param aPlaintext Plain text.
*/
    {
    CX509RSAPublicKey* publicKey = CX509RSAPublicKey::NewLC(aPublicKeyData);
    CRSAPKCS1v15Verifier* verifier = CRSAPKCS1v15Verifier::NewLC(*publicKey);

    RInteger S = RInteger::NewL(aCiphertext);
    CleanupClosePushL(S);
    CRSASignature* signature = CRSASignature::NewL(S);
    CleanupStack::Pop(); //S
    CleanupStack::PushL(signature);
    
    aPlaintext = verifier->InverseSignLC(*signature);
    
    CleanupStack::Pop();            //aPlaintext
    CleanupStack::PopAndDestroy(3); //signature, verifier, publicKey
    }
    
EXPORT_C TBool TUtlCrypto::DsaVerifySignatureL(const TDesC8&       aPublicKeyData,
                                               const TDesC8&       aDsaParams,
                                               const TDesC8&       aDsaSignatureR,
                                               const TDesC8&       aDsaSignatureS,
                                               const TDesC8&       aHashData)
/** 
* Verifies DSA signature.
* @param aPublicKeyData Public key.
* @param aDsaParams DSA parameters.
* @param aDsaSignatureR R.
* @param aDsaSignatureS S.
* @param aHashData Hash data.
* @return TBool Verify signature status: ETrue, if OK.
*/
    {
    TBool ret;
    TX509KeyFactory keyFactory;
    CDSAParameters* params = keyFactory.DSAParametersL(aDsaParams);
    CleanupStack::PushL(params);
    CDSAPublicKey* key = keyFactory.DSAPublicKeyL(*params, aPublicKeyData);
    CleanupStack::PushL(key);
    
    RInteger R = RInteger::NewL(aDsaSignatureR);
    CleanupClosePushL(R);
    RInteger S = RInteger::NewL(aDsaSignatureS);
    CleanupClosePushL(S);
    CDSASignature* signature = CDSASignature::NewL(R, S);
    CleanupStack::Pop(2); //S, R
    CleanupStack::PushL(signature);
    
    CDSAVerifier* verifier = CDSAVerifier::NewLC(*key);
    ret = verifier->VerifyL(aHashData, *signature);
    
    CleanupStack::PopAndDestroy(4); //verifier, signature, key, params
    return ret;
    }
    
EXPORT_C TBool TUtlCrypto::IsWeakCryptoLibrary(void)
/** 
* Tests the strength of the crypto libary.
* @return TBool ETrue, if weak crypto library.
*/
    {
    TCrypto::TStrength strength = TCrypto::Strength();
    if (strength == TCrypto::EWeak)
        return ETrue;
    else
        return EFalse;
    }

EXPORT_C TUtlCrypto::TUtlCryptoVersion TUtlCrypto::CryptoVersion(void)
/** 
* Returns the version of the crypto libary.
* @return TUtlCryptoVersion, crypto version
*/
    {
    return EUtlCryptoVersionSymbian1;
    }
    
