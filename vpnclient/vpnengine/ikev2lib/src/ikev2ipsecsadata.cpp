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
* Description:  IKEv2 IPsec sa data
*
*/

#include "ikev2ipsecsadata.h"
#include "ikedebug.h"
#include "ikev2ipsecsarekeydata.h"
#include "ikecrypto.h"
#include "ikev2const.h"

TIkeV2IpsecSAData::TIkeV2IpsecSAData(MIkeDebug& aDebug)
:iSPI_In(0), iSPI_Out(0),
 iEncrAlg(0), iIntegAlg(0),
 iSaType(0), iESN(0),
 iCipherKeyLth(0), iIntegKeyLth(0),
 iTransport(EFalse), iSrcSpecific(EFalse),
 iRekeyData(NULL), iKeyMaterial(NULL),
 iNext(NULL), iDebug(aDebug)
    {
    }


void TIkeV2IpsecSAData::Copy(const TIkeV2IpsecSAData& aSrc)
    {
    Mem::Copy((TUint8*)&iSPI_In, (TUint8*)&aSrc.iSPI_In, sizeof(TIkeV2IpsecSAData));
    iNext = NULL;           
    }

/*
void TIkeV2IpsecSAData::StoreKeyMaterial(HBufC8* aKeyMaterial)
    {
    PurgeKeyMaterial();
    iKeyMaterial = aKeyMaterial;
    }
*/

void TIkeV2IpsecSAData::PurgeKeyMaterial()
    {
    if ( iKeyMaterial )
        {   
        iKeyMaterial->Des().FillZ(); // Wipe out key material data from buffer           
        delete iKeyMaterial;
        iKeyMaterial = NULL;
        }   
    }


void TIkeV2IpsecSAData::DeleteRekeyData()
    {
    if ( iRekeyData )
        {               
        delete iRekeyData;
        iRekeyData = NULL;
        }   
    }

void TIkeV2IpsecSAData::GenerateIpsecKeysL(const TDesC8& aSKd, 
                                           const TDesC8& aGPowIr, 
                                           const TDesC8& aNonceI, 
                                           const TDesC8& aNonceR,
                                           TUint16 aPrfAlg)
{
    //
    //  Generate Ipsec keying material.
    //  Keying material is created as follows:  KEYMAT = prf+(SK_d, Ni | Nr)
    //  If PFS used Keying material is: KEYMAT = prf+(SK_d, g^ir (new) | Ni | Nr )
    //
    HBufC8* s = HBufC8::NewLC(aGPowIr.Length() + aNonceI.Length() + aNonceR.Length());
    TPtr8 sPtr = s->Des();
    //
    //  Append Nonce data into keymaterial work buffer S
    //   
    sPtr = aGPowIr;
    sPtr.Append(aNonceI);
    sPtr.Append(aNonceR);
    
    TInt KeyMatLth = 0;
    if ( iEncrAlg )
    {   if ( iCipherKeyLth == 0) 
           iCipherKeyLth = IkeCrypto::AlgorithmInfo(IKEV2_ENCR, iEncrAlg, NULL);
        KeyMatLth = 2*iCipherKeyLth; 
    }   
    if ( iIntegAlg )
    {   
        iIntegKeyLth = IkeCrypto::AlgorithmInfo(IKEV2_INTEG, iIntegAlg, NULL);    
        KeyMatLth += 2*iIntegKeyLth;
    }   
    
    PurgeKeyMaterial();    
    iKeyMaterial = IkeCrypto::GenerateKeyingMaterialL(aSKd, *s, KeyMatLth, aPrfAlg);    
    CleanupStack::PopAndDestroy(s);
}


