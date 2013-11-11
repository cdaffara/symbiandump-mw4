/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Crypto Layer to use and change any cryptolibrary easily.
* Contains all the cryptographic functions used in IKEv1.
*
*/



#include "ikev1crypto.h"
#include "ikemsgheader.h"
#include "dhparameters.h"
#include "utlcrypto.h"

CIkeKeys* CIkeKeys::NewL(const TDesC8& aN, const TDesC8& aG)
{
    CIkeKeys *keys = new (ELeave) CIkeKeys();
    keys->iDHKey = TUtlCrypto::MakeDiffieHellmanL(aN, aG);
	keys->iModuluslength = aN.Length();
    return keys;
}

CIkeKeys::~CIkeKeys()
{
    delete iDHKey;
}


HBufC8* CIkeKeys::GetPubKey()
{
    HBufC8* DHPublicKey = (HBufC8*)iPubKey;
    iPubKey = NULL; // Exclusive ownership of iPubKey buffer is returned to caller
    return DHPublicKey;
}

void CIkeKeys::XValueL()
{
    iPubKey = iDHKey->GenerateXL();
}


const HBufC8* CIkeKeys::KValueL(const TDesC8& aY) const 
{
    return iDHKey->CompleteKL(aY);
}




//Generates a group of parameters depending on the group.
CIkeKeys *CreateDHKeyL(TUint aGroupDesc)
{
    TPtrC8 prime_ptr(NULL, 0);
    TPtrC8 gen_ptr(NULL, 0);

    switch (aGroupDesc)
    {
    case MODP_768:
        prime_ptr.Set((TUint8 *)&MODP_768_PRIME[0], MODP_768_PRIME_LENGTH);
        gen_ptr.Set((TUint8 *)&MODP_768_GENERATOR[0], MODP_768_GENERATOR_LENGTH);
        break;
    case MODP_1024:
        prime_ptr.Set((TUint8 *)&MODP_1024_PRIME[0], MODP_1024_PRIME_LENGTH);
        gen_ptr.Set((TUint8 *)&MODP_1024_GENERATOR[0], MODP_1024_GENERATOR_LENGTH);
        break;
    case MODP_1536:
        prime_ptr.Set((TUint8 *)&MODP_1536_PRIME[0], MODP_1536_PRIME_LENGTH);
        gen_ptr.Set((TUint8 *)&MODP_1536_GENERATOR[0], MODP_1536_GENERATOR_LENGTH);
        break;
    case MODP_2048:
        prime_ptr.Set((TUint8 *)&MODP_2048_PRIME[0], MODP_2048_PRIME_LENGTH);
        gen_ptr.Set((TUint8 *)&MODP_2048_GENERATOR[0], MODP_2048_GENERATOR_LENGTH);
        break;
    case EC2N_155:
    case EC2N_185:
        return NULL;
    default:    //Cannot happen because checked before!!!
        return NULL;
    }
    
    CIkeKeys *arg = CIkeKeys::NewL(prime_ptr, gen_ptr);
    
    return arg;
}

CIkeKeys *GeneratePubPrivKeysL(TUint aGroupDesc)
{
    CIkeKeys *dh_key = CreateDHKeyL(aGroupDesc);
    if (!dh_key)
        return NULL;
    dh_key->XValueL();    //Initializes the public and private keys.

    return dh_key;

}

HBufC8* ComputeAgreedKeyL(TUint /*aGroupDesc*/, const TDesC8 &aPeerPublicKey, CIkeKeys *aOwnKeys)
{
    if(!aOwnKeys)
        return NULL;

	return (HBufC8*)aOwnKeys->KValueL(aPeerPublicKey);
}

void DecryptL(const TUint8* aInputPayload, TUint8* aOutputPayload, TUint32 aLength, TDes8& aIV, TDesC8& aKey, TUint16 aEncrAlg)
{
    TUtlCrypto::TUtlSymmetricCipherId CipherId = TUtlCrypto::EUtlSymmetricCipherAesCbc;  // Defaults
    TInt IVLth = AESCBC_IV_LEN;
	
    if ( aEncrAlg == DES3_CBC )
	{
		CipherId = TUtlCrypto::EUtlSymmetricCipher3DesCbc;
		IVLth    = DESCBC_IV_LEN;
    }    
    else if ( aEncrAlg == DES_CBC)
	{
		CipherId = TUtlCrypto::EUtlSymmetricCipherDesCbc;
		IVLth    = DESCBC_IV_LEN;		
    }       
    //
    //  Construct cipher object for symmetric decrypt operation
    //
    TPtrC8 iv_ptr(aIV.Ptr(), IVLth);
    TPtrC8 ciphertext(aInputPayload, aLength);
    TPtr8  plaintext(aOutputPayload, aLength);
	
	CUtlSymmetricCipher* UtlCipher = TUtlCrypto::MakeSymmetricDecryptorL(CipherId,
		                                                                 aKey,
	                                                                     iv_ptr);
	
    CleanupStack::PushL(UtlCipher);
    UtlCipher->ProcessFinalL(ciphertext, plaintext);
    CleanupStack::PopAndDestroy();
    aIV.Copy(&aInputPayload[aLength - IVLth], IVLth);   //Next IV (last 8 bytes of ciphertext)     
                                                                           
}

TBool EncryptL(TDes8& aInputPayload, TDes8& aOutputPayload, TDes8& aIV, TDesC8& aKey, TUint16 aEncrAlg)
{
    TUtlCrypto::TUtlSymmetricCipherId CipherId = TUtlCrypto::EUtlSymmetricCipherAesCbc;  // Defaults
    TInt CbLth = AESCBC_IV_LEN;
	
    if ( aEncrAlg == DES3_CBC )
	{
        CipherId = TUtlCrypto::EUtlSymmetricCipher3DesCbc;
		CbLth    = DESCBC_IV_LEN;
    }    
    else if ( aEncrAlg == DES_CBC)
	{
        CipherId = TUtlCrypto::EUtlSymmetricCipherDesCbc;
		CbLth    = DESCBC_IV_LEN;
    }
    TPtrC8 iv_ptr(aIV.Ptr(), CbLth);
    //
    // Add padding, if needed
    //
    TUint padding_bytes = (aInputPayload.Length() - ISAKMP_HDR_SIZE) % CbLth;
    if (padding_bytes != 0)   //Padd with 0 at the end if needed
    {
        TChar c(0);
        aInputPayload.AppendFill(c,CbLth-padding_bytes);    //Append at the end
    }
    //
    // ISAKMP fixed header not encrypted
    //
    TPtrC8 plaintext(aInputPayload.Ptr() + ISAKMP_HDR_SIZE, aInputPayload.Length() - ISAKMP_HDR_SIZE);//skip hdr
    aOutputPayload.Copy(aInputPayload.Ptr(), ISAKMP_HDR_SIZE);  //The same HDR in output
    
    TPtr8 ciphertext((TUint8 *)aOutputPayload.Ptr() + ISAKMP_HDR_SIZE, 0, aInputPayload.Length() - ISAKMP_HDR_SIZE);//skip hdr  
    //
    //  Construct cipher object for symmetric decrypt operation
    //
	CUtlSymmetricCipher* UtlCipher = TUtlCrypto::MakeSymmetricEncryptorL(CipherId,
	                                                                     aKey,
		                                                                 iv_ptr);
    CleanupStack::PushL(UtlCipher);     
    UtlCipher->ProcessFinalL(plaintext, ciphertext);
    CleanupStack::PopAndDestroy();     
    //
    // Next IV (last cipher block of encrypted buffer)
    //
    aOutputPayload.SetLength(ISAKMP_HDR_SIZE + ciphertext.Length());
    aIV.Copy(aOutputPayload.Ptr() + aOutputPayload.Length() - CbLth, CbLth); 
    
    return ETrue;
                                                                           
}


void MD5HashL(const TDesC8 &aInData, TDes8& aOutData)
{
    CUtlMessageDigest* Digest = TUtlCrypto::MakeMessageDigesterL(TUtlCrypto::EUtlMessageDigestMd5);
    aOutData.Copy(Digest->Final(aInData));
    delete Digest;
}

void SHA1HashL(const TDesC8 &aInData, TDes8& aOutData)
{
    CUtlMessageDigest* Digest = TUtlCrypto::MakeMessageDigesterL(TUtlCrypto::EUtlMessageDigestSha1);
    aOutData.Copy(Digest->Final(aInData));
    delete Digest;
}

void MD5HmacL(const TDesC8 &aInData, TDes8& aOutData, const TDesC8& aKeyData)
{
    CUtlMessageDigest* Digest = TUtlCrypto::MakeMessageDigesterL(TUtlCrypto::EUtlMessageDigestMd5,
                                                                 aKeyData);
    aOutData.Copy(Digest->Final(aInData));  
    delete Digest;
}

void SHA1HmacL(const TDesC8 &aInData, TDes8& aOutData, const TDesC8& aKeyData)
{
    CUtlMessageDigest* Digest = TUtlCrypto::MakeMessageDigesterL(TUtlCrypto::EUtlMessageDigestSha1,
                                                                 aKeyData);
    aOutData.Copy(Digest->Final(aInData));  
    delete Digest;
}

void Hmac3DesCbcL(const TDesC8 &aInData, TDes8& aOutData, const TDesC8& aKeyData)
{
    TUint8 *pad = new (ELeave) TUint8[PAD_SIZE];
    TInt pad_len = 0;
    CleanupStack::PushL(pad);
    TBuf8<8> iv;
    TBuf8<24> prf_key2;
    TPtrC8 prf_key_ptr;
        
    if ( aKeyData.Length() < 24)  //if less than 24 bits the rest must be 0's
    {
       prf_key2.FillZ(24);
       prf_key2.Copy(aKeyData);
       prf_key2.SetLength(24);
       prf_key_ptr.Set(prf_key2);
    }
    else
    {   
       prf_key_ptr.Set(aKeyData);
    }   
    iv.FillZ(8);
    Mem::Copy(pad, aInData.Ptr(), aInData.Length());
    pad_len = aInData.Length();
    if ( pad_len & 0x07 ) //Add padding to align to byte pieces??????????
    {
       Mem::FillZ(&pad[pad_len], 7);
       pad_len += 7;
       pad_len = (pad_len & 65528) + 8;
    }
    Cipher3DesL(pad, pad_len, prf_key_ptr, iv, aOutData);

    CleanupStack::PopAndDestroy();  //pad       
    
}

void Cipher3DesL(TUint8 *aInData, TInt aInDataLen, const TDesC8 &aPrfKey, TDes8 &aIV, TDes8 &aOutData)
{

    TPtrC8 iv_ptr(aIV.Ptr(), DESCBC_IV_LEN);
    TPtrC8 key_ptr(aPrfKey.Ptr(), 3*DESCBC_KEY_LEN);
    
    HBufC8 *des_input = HBufC8::NewL(aInDataLen + 8);
    CleanupStack::PushL(des_input);         
    TPtr8 des_input_ptr((TUint8 *)des_input->Des().Ptr(), aInDataLen + 8, aInDataLen + 8);  //Contains the PRF input text
    TPtrC8 plain_input_text(aInData, aInDataLen);//skip hdr
    //
    //  Construct cipher object for symmetric decrypt operation
    //
    CUtlSymmetricCipher* UtlCipher = TUtlCrypto::MakeSymmetricEncryptorL(TUtlCrypto::EUtlSymmetricCipher3DesCbc,
                                                                         key_ptr,
                                                                         iv_ptr);
    CleanupStack::PushL(UtlCipher);
    
    HBufC8 *des_output= HBufC8::NewL(des_input_ptr.Length() + UtlCipher->BlockSize());
    CleanupStack::PushL(des_output);            
    TPtr8 des_output_ptr((TUint8 *)des_output->Des().Ptr(), des_input_ptr.Length() + UtlCipher->BlockSize());

    UtlCipher->ProcessFinalL(plain_input_text, des_output_ptr);
    TPtrC8 tmp_input_data(&des_output_ptr[des_output_ptr.Length() - 8], 8); //Input Data to the 2nd 8-bit chunk encryption  
    UtlCipher->Reset();    //Restart
    UtlCipher->ProcessFinalL(tmp_input_data, aOutData); 
    
    // Do second eight bytes
    des_input_ptr.Copy(aOutData);
    des_input_ptr.Append(plain_input_text); //reattach the input data
    UtlCipher->Reset();    //Restart
    UtlCipher->ProcessFinalL(des_input_ptr, des_output_ptr);
    
    tmp_input_data.Set(&des_output_ptr[des_output_ptr.Length() - 8], 8);
    TPtr8 out_data_ptr((TUint8 *)aOutData.Ptr() + 8, 0, 8);
    UtlCipher->Reset();    //Restart
    UtlCipher->ProcessFinalL(tmp_input_data, out_data_ptr);
    
    // Do third eight bytes 
    Mem::Copy((TUint8 *)des_input_ptr.Ptr(), aOutData.Ptr() + 8, 8);    //last 8 bytes of previous result put at the beginning
    //The length of des_input_ptr won't change so the data after byte 8 is still used!!!    
    UtlCipher->Reset();    //Restart
    UtlCipher->ProcessFinalL(des_input_ptr, des_output_ptr);
    
    tmp_input_data.Set(&des_output_ptr[des_output_ptr.Length() - 8], 8);
    out_data_ptr.Set((TUint8 *)aOutData.Ptr() + 16, 0, 8);  //the last 8 bytes (16 to 23)
    UtlCipher->Reset();    //Restart
    UtlCipher->ProcessFinalL(tmp_input_data, out_data_ptr);
    
    aOutData.SetLength(24);

    CleanupStack::PopAndDestroy(3); // des_output, UtlCipher and  des_input     
}

TInt SymmetricCipherL(TUint8 *aInput,  TUint8 *aOutput, TInt aLength,
				      TUint8 *aKey,    TUint8 *aIV, TBool aEncr, TInt aEncAlg)
{
	(void)aEncAlg;
	TPtrC8 iv_ptr(aIV, DESCBC_IV_LEN );
	TPtrC8 key_ptr(aKey, DESCBC_IV_LEN);

	CUtlSymmetricCipher* UtlCipher; 
	if ( aEncr ) {
    	//
	    // Add padding (according RFC 1423) and encrypt data 
	    //
       TInt PaddingBytes = 8 - (aLength % 8);
	   for ( TInt i = 0; i < PaddingBytes; i++ ) {
		   *(aInput + aLength + i) = (TUint8)PaddingBytes;
	   }
	   aLength += PaddingBytes;
	   UtlCipher = TUtlCrypto::MakeSymmetricEncryptorL(TUtlCrypto::EUtlSymmetricCipherDesCbc,
			                                            key_ptr, iv_ptr);
	}	 
	else {
		//
		// Decrypt data 
		//
	   UtlCipher = TUtlCrypto::MakeSymmetricDecryptorL(TUtlCrypto::EUtlSymmetricCipherDesCbc,
		                                               key_ptr, iv_ptr);
	}
	CleanupStack::PushL(UtlCipher);
	TPtrC8 inp(aInput, aLength);
	TPtr8 outp(aOutput, aLength);
	UtlCipher->ProcessFinalL(inp, outp);
	CleanupStack::PopAndDestroy();

	if ( !aEncr ) {
	   //
	   // Remove padding from decrypted data
	   //
	   TUint8 PaddingLth = *(aOutput + aLength - 1);
	   if ( PaddingLth < 9 )
		    aLength -= (TInt)PaddingLth;      // Ok padding count
	   else aLength = 0;
	}

	return aLength;
		
}



