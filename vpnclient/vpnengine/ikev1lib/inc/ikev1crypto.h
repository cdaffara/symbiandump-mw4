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
* Description: 
* Cryptographic Intermediate Layer to use and change any crypto library easily.
*
*/

#ifndef IKEV1CRYPTO_H
#define IKEV1CRYPTO_H

#include <e32base.h>
#include "ikev1isakmpct.h"


class CUtlDiffieHellman;

NONSHARABLE_CLASS(CIkeKeys) : public CBase
{
public:
    static CIkeKeys* NewL(const TDesC8& aN,const TDesC8& aG);   
    HBufC8 *GetPubKey();
    void XValueL(); 
    const HBufC8* KValueL(const TDesC8& aY) const;
	inline TInt ModulusLength() {return iModuluslength;}

    ~CIkeKeys();
private:
    CUtlDiffieHellman* iDHKey;
    const HBufC8*      iPubKey;
	TInt               iModuluslength;
};


// SSL function implementation using Symbian apis
//  - des_ede3_cbc_encrypt
#define DES_ENCRYPT 1
#define DES_DECRYPT 0

#define HMAC_MD5_SIZE   128
#define HMAC_SHA1_SIZE  160

#define DESCBC_KEY_LEN  8
#define DESCBC_IV_LEN   8

#define AESCBC_DEF_KEY_LEN 16
#define AESCBC_IV_LEN   16

#define DEFAULT_NONCE_SIZE   128/8   //128 bits = 16 bytes

#define PAD_SIZE	64 + 2 * DEFAULT_NONCE_SIZE + 1000

//DH functions
CIkeKeys* GeneratePubPrivKeysL(TUint aGroupDesc);
HBufC8* ComputeAgreedKeyL(TUint aGroupDesc, const TDesC8 &aPeerPublicKey, CIkeKeys *aOwnKeys);

//
//Symmetric encryption/decryption functions
//
void DecryptL(const TUint8* aInputPayload, TUint8* aOutputPayload, TUint32 aLength, TDes8 &aIV, TDesC8& aKey, TUint16 aEncrAlg);
TBool EncryptL(TDes8& aInputPayload, TDes8& aOutputPayload,TDes8 &aIV, TDesC8& aKey , TUint16 aEncrAlg);
void Cipher3DesL(TUint8 *aInData, TInt aInDataLen, const TDesC8 &aPrfKey, TDes8 &aIV, TDes8 &aPrfOutput);

//
//Message Digest Algorithms
//
void MD5HashL(const TDesC8 &aInData, TDes8& aOutData);
void SHA1HashL(const TDesC8 &aInData, TDes8& aOutData);

void MD5HmacL(const TDesC8 &aInData, TDes8& aOutData, const TDesC8& aKeyData);
void SHA1HmacL(const TDesC8 &aInData, TDes8& aOutData, const TDesC8& aKeyData);

//The length of aInData must be less than PAD_SIZE. The caller of this function
//has to take care of handling the size of this parameter.
void Hmac3DesCbcL(const TDesC8 &aInData, TDes8& aOutData, const TDesC8& aKeyData);

TInt SymmetricCipherL(TUint8 *aInput,  TUint8 *aOutput, TInt aLength,
					  TUint8 *aKey,    TUint8 *aIV, TBool aEncr, TInt aEncAlg = DES_CBC);

#endif
