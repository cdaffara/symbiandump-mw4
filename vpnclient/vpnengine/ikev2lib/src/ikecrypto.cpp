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
* Description: Intermediate class between IKEv2 and crypto library.
*
*/


#include "dhparameters.h"
#include "ikecrypto.h"
#include "ikev2const.h"

CDHKeys* CDHKeys::NewL(const TDesC8& aN, const TDesC8& aG)
{
    CDHKeys* keys = new (ELeave) CDHKeys();
    CleanupStack::PushL(keys);
    keys->iDHKey = TUtlCrypto::MakeDiffieHellmanL(aN, aG);
    keys->iModuluslength = aN.Length();
    CleanupStack::Pop(keys);
    return keys;
}

CDHKeys* CDHKeys::CreateDHKeyL(TUint aGroupDesc)
{
	TPtrC8 prime_ptr(NULL, 0);
	TPtrC8 gen_ptr(NULL, 0);

	switch (aGroupDesc)
	{
		case DH_GROUP_768:
			prime_ptr.Set((TUint8 *)&MODP_768_PRIME[0], MODP_768_PRIME_LENGTH);
			gen_ptr.Set((TUint8 *)&MODP_768_GENERATOR[0], MODP_768_GENERATOR_LENGTH);
			break;
		case DH_GROUP_1024:
			prime_ptr.Set((TUint8 *)&MODP_1024_PRIME[0], MODP_1024_PRIME_LENGTH);
			gen_ptr.Set((TUint8 *)&MODP_1024_GENERATOR[0], MODP_1024_GENERATOR_LENGTH);
			break;
		case DH_GROUP_1536:
			prime_ptr.Set((TUint8 *)&MODP_1536_PRIME[0], MODP_1536_PRIME_LENGTH);
			gen_ptr.Set((TUint8 *)&MODP_1536_GENERATOR[0], MODP_1536_GENERATOR_LENGTH);
			break;
    case DH_GROUP_2048:
			prime_ptr.Set((TUint8 *)&MODP_2048_PRIME[0], MODP_2048_PRIME_LENGTH);
			gen_ptr.Set((TUint8 *)&MODP_2048_GENERATOR[0], MODP_2048_GENERATOR_LENGTH);
			break;
		default:    //Cannot happen because checked before!!!
			User::Leave(KErrNotSupported);
	}

	CDHKeys *DhKeys = CDHKeys::NewL(prime_ptr, gen_ptr);

	return DhKeys;
}

HBufC8* CDHKeys::ComputeAgreedKeyL(const TDesC8 &aPeerPublicKey)
{
	return (HBufC8*)KValueL(aPeerPublicKey);
}


HBufC8* CDHKeys::GetPubKey()
{
    HBufC8* DHPublicKey = (HBufC8*)iPubKey;
    iPubKey = NULL; // Exclusive ownership of iPubKey buffer is returned to caller
    return DHPublicKey;
}

void CDHKeys::XValueL()
{
	if ( !iPubKey ) 
       iPubKey = iDHKey->GenerateXL();
}

CDHKeys::~CDHKeys()
{
    delete iDHKey;
}


void IkeCrypto::DecryptL(const TUint8* aInput, TUint8* aOutput, TInt aLength, 
                         TUint8* aIV, const TDesC8& aKey, TUint16 aEncrAlg)
{
	TUtlCrypto::TUtlSymmetricCipherId CipherId;
	TInt IVLth;
	IkeCrypto::AlgorithmInfo(IKEV2_ENCR, aEncrAlg, &IVLth, &CipherId);
	
	if ( !aInput || !aOutput || !aIV )
		User::Leave(KErrArgument);
	
	TPtrC8 IvPtr(aIV, IVLth);
	TPtrC8 CipherText(aInput, aLength);
	TPtr8  PlainText(aOutput, aLength);

	CUtlSymmetricCipher* UtlCipher = 
	    TUtlCrypto::MakeSymmetricDecryptorL(CipherId, aKey, IvPtr);
	CleanupStack::PushL(UtlCipher);
	UtlCipher->ProcessFinalL(CipherText, PlainText);
	CleanupStack::PopAndDestroy(UtlCipher);
}


void IkeCrypto::EncryptL(const TDesC8& aInput, TPtr8& aOutput, 
                         const TDesC8& aIv, const TDesC8& aKey, TUint16 aEncrAlg)
{
	TUtlCrypto::TUtlSymmetricCipherId CipherId;
	TInt CbLth;
	IkeCrypto::AlgorithmInfo(IKEV2_ENCR, aEncrAlg, &CbLth, &CipherId);

	__ASSERT_DEBUG(CbLth == aIv.Length(), User::Invariant());
	__ASSERT_DEBUG(aInput.Length() % CbLth == 0, User::Invariant());
	__ASSERT_DEBUG(aInput.Length() <= aOutput.MaxLength(), User::Invariant());

	CUtlSymmetricCipher* UtlCipher = 
	    TUtlCrypto::MakeSymmetricEncryptorL(CipherId, aKey, aIv);
	CleanupStack::PushL(UtlCipher);     
	UtlCipher->ProcessFinalL(aInput, aOutput);
	CleanupStack::PopAndDestroy(UtlCipher);
}

TInt IkeCrypto::IntegHMACL(const TDesC8& aInput, TDes8& aChecksum, const TDesC8& aKeyData, TUint16 aIntegAlg)
{
	TUtlCrypto::TUtlMessageDigestId DigestId;
	TInt HmacLth;	
	IkeCrypto::AlgorithmInfo(IKEV2_INTEG, aIntegAlg, &HmacLth, NULL, &DigestId);	
	CUtlMessageDigest* Digest = TUtlCrypto::MakeMessageDigesterL(DigestId, aKeyData);
		
	const TPtrC8 hash = Digest->Final(aInput); 
	
	//We don't always use the whole hash. 
	//(e.g., HMAC-SHA1-96 uses only first 12 bytes)
	__ASSERT_DEBUG(hash.Length() >= HmacLth, User::Invariant());
	
	aChecksum = hash.Left(HmacLth);	
	delete Digest;

	return HmacLth;
}

HBufC8* IkeCrypto::PrfhmacL(const TDesC8& aInput, const TDesC8& aKeyData, TUint16 aPrfAlg)
{
	TUtlCrypto::TUtlMessageDigestId DigestId;
	TInt PrfLth;	
	IkeCrypto::AlgorithmInfo(IKEV2_PRF, aPrfAlg, &PrfLth, NULL, &DigestId);	
	CUtlMessageDigest* Digest = TUtlCrypto::MakeMessageDigesterL(DigestId, aKeyData);
	CleanupStack::PushL(Digest);
	
	HBufC8* checksum = Digest->Final(aInput).AllocL();
	
	CleanupStack::PopAndDestroy(Digest);

	return checksum;
	
}

HBufC8* IkeCrypto::PrfL(const TDesC8& aInput, TUint16 aPrfAlg)
{
	TUtlCrypto::TUtlMessageDigestId DigestId;
	TInt PrfLth;	
	IkeCrypto::AlgorithmInfo(IKEV2_PRF, aPrfAlg, &PrfLth, NULL, &DigestId);	
	CUtlMessageDigest* digest = TUtlCrypto::MakeMessageDigesterL(DigestId);
	CleanupStack::PushL(digest);
	
	HBufC8* hash = digest->Final(aInput).AllocL();
			
	CleanupStack::PopAndDestroy(digest);

	return hash;

}

TInt IkeCrypto::AlgorithmInfo(TUint16 aTransform, TUint16 aAlgCode, TInt* aBlockLth,
							  TUtlCrypto::TUtlSymmetricCipherId* aCipherId,
							  TUtlCrypto::TUtlMessageDigestId*   aDigestId) 
{
	TInt KeyLth   = 0;
	TInt BlockLth = 0;
    	
	switch ( aTransform  )
	{
		case IKEV2_ENCR:
			switch ( aAlgCode )
			{
				case ENCR_DES:
					KeyLth   = 8;
					BlockLth = 8;
					if ( aCipherId )
						*aCipherId = TUtlCrypto::EUtlSymmetricCipherDesCbc;
					break;
					
				case ENCR_3DES:
					KeyLth   = 24;
					BlockLth = 8;
				    if ( aCipherId )
					   *aCipherId = TUtlCrypto::EUtlSymmetricCipher3DesCbc;
					break;

				case ENCR_AES_CBC:
					KeyLth   = 0;
					BlockLth = 16;
					if ( aCipherId )
						*aCipherId = TUtlCrypto::EUtlSymmetricCipherAesCbc;
				   break;

				case ENCR_NULL:
					KeyLth   = 0;
					BlockLth = 0;
					break;
				default:
					break;
			}	
			break;

		case IKEV2_PRF:
			switch ( aAlgCode )
			{
				case PRF_HMAC_MD5:
					KeyLth   = 16;
					BlockLth = 16;
					if ( aDigestId )
						*aDigestId = TUtlCrypto::EUtlMessageDigestMd5;
					break;

				case PRF_HMAC_SHA1:
					KeyLth   = 20;
					BlockLth = 20;
					if ( aDigestId )
						*aDigestId = TUtlCrypto::EUtlMessageDigestSha1;
					break;

				default:
					break;
			}
			break;

		case IKEV2_INTEG:
			switch ( aAlgCode )
			{
				case AUTH_HMAC_MD5_96:
					KeyLth   = 16;
					BlockLth = 12;
					if ( aDigestId )
						*aDigestId = TUtlCrypto::EUtlMessageDigestMd5;
					break;

				case AUTH_HMAC_SHA1_96:
					KeyLth   = 20;
					BlockLth = 12;
					if ( aDigestId )
						*aDigestId = TUtlCrypto::EUtlMessageDigestSha1;
					break;

				default:
					break;
			}
			break;			

		default:
			break;
			
	}
	
	if ( aBlockLth )
		*aBlockLth = BlockLth;

	return KeyLth;

}

HBufC8* IkeCrypto::GenerateKeyingMaterialL(const TDesC8& aK, const TDesC8& aS, TInt aKeyMatLth, TUint16 aPRFAlg)
{
    //
    //  Since the amount of keying material needed may be greater than
    //  the size of the output of the prf algorithm prf+ is used as
    //  follows    prf+ (K,S) = T1 | T2 | T3 | T4 | ...
    //    where:   T1 = prf (K, S | 0x01)
    //             T2 = prf (K, T1 | S | 0x02) ..
    //             TN = prf (K, TN-1 | S | 0xN ) ;[ N < 256 ]
    //
    TInt PrfKeyLth    = IkeCrypto::AlgorithmInfo(IKEV2_PRF, aPRFAlg, NULL);
    TInt S_Lth        = aS.Length();
    TUint8 IterCount  = (TUint8)((aKeyMatLth/PrfKeyLth) + 1);
    HBufC8* KeyMat    = HBufC8::NewL((IterCount * PrfKeyLth) + S_Lth + 1);
    CleanupStack::PushL(KeyMat);
    TPtr8 KeyMatPtr(KeyMat->Des());
            
    TUint8 i = 1;
    //
    // Produce key material T1 | T2 | T3 | T4 | ...
    //
    
    HBufC8* tValue = NULL;
    HBufC8* inputBuffer = HBufC8::NewLC(PrfKeyLth + aS.Length() + sizeof(i));
    TPtr8 inputBufferPtr(inputBuffer->Des());
    while ( i <= IterCount )
    {
        inputBufferPtr.Zero();
        if (tValue != NULL)
            {
            inputBufferPtr.Append(*tValue);
            }
        //
        // Append value S into key material buffer and concatenate 8 bit integer
        // value i into S
        //
        inputBufferPtr.Append(aS);
        inputBufferPtr.Append(&i, sizeof(i));

        delete tValue;
        tValue = NULL;
        //
        // Calculate TN = prf (SKEYSEED, TN-1 | S | 0xN)
        //
        tValue = IkeCrypto::PrfhmacL(*inputBuffer, aK, aPRFAlg);
        KeyMatPtr.Append(*tValue);
        i++;
    }
    delete tValue;
        
    CleanupStack::PopAndDestroy(inputBuffer);
    CleanupStack::Pop();  // Keymat
    
    return KeyMat;
}

