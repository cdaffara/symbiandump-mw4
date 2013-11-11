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
* Description:  IKEv2 IPsec SA data definition
*                Class TIkeV2IpsecSAData is the IKEv2 specific version of
*                IPsec SA parameter definition which
*                is used to pass SA information between store IPsec SA
*                information internally by the IKEv2 plugin.
*
*/

#ifndef __IKEV2IPSECSADATA_H__
#define __IKEV2IPSECSADATA_H__

#include <e32base.h>

class CIpsecSARekeyData;
class MIkeDebug;

class TIkeV2IpsecSAData 
    {
public:
    TIkeV2IpsecSAData(MIkeDebug& aDebug);
    void Copy(const TIkeV2IpsecSAData& aSrc);
    //void StoreKeyMaterial(HBufC8* aKeyMaterial);
    void PurgeKeyMaterial();
    void DeleteRekeyData();
    
    void GenerateIpsecKeysL(const TDesC8& aSKd, 
                            const TDesC8& aGPowIr, 
                            const TDesC8& aNonceI, 
                            const TDesC8& aNonceR,
                            TUint16 aPrfAlg);

public: 
    
    
    TBuf8<4> iSPI_In;        // Local SPI 
    TBuf8<4> iSPI_Out;       // Remote SPI
    
//
// Selected IPSEC SA proposal 
//
    TUint16 iEncrAlg;       // Encryption algorithm (transform ID 1)
    TUint16 iIntegAlg;      // Integrity algorithm  (transform ID 3)
    TUint8  iSaType;        // AH, ESP SA type from pfkeyv2.h
    TUint8  iESN;           // 0 = no ESN; 1 = ESN with ESP
    TInt    iCipherKeyLth;  // Cipher key length        
    TInt    iIntegKeyLth;   // Integrity key length
    TBool   iTransport;     // if Transport mode value is ETrue
    TBool   iSrcSpecific;   // if IPSec SA is source specific is ETrue              
  
    CIpsecSARekeyData* iRekeyData;              
//
// IPSEC keymaterial buffers
//
    HBufC8* iKeyMaterial;   // Cipher key buffer
    TIkeV2IpsecSAData* iNext;    // Next Ipsec SA pair
    
private:
    MIkeDebug& iDebug;       
    };


#endif /* __IKEV2IPSECSADATA_H__ */
