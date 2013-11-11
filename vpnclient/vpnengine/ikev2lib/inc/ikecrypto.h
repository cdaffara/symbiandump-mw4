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
* Description:   Cryptographic Intermediate Layer to use and change any crypto library easily.
*
*/

#ifndef __IKECRYPTO_H_
#define __IKECRYPTO_H__


#include <e32base.h>
#include "utlcrypto.h"

NONSHARABLE_CLASS(CDHKeys) : public CBase
{
public:
    static CDHKeys* NewL(const TDesC8& aN,const TDesC8& aG);
	static CDHKeys* CreateDHKeyL(TUint aGroupDesc);
	HBufC8* ComputeAgreedKeyL(const TDesC8 &aPeerPublicKey);
    HBufC8* GetPubKey();
    void XValueL(); 
    inline const HBufC8* KValueL(const TDesC8& aY) const {return iDHKey->CompleteKL(aY);}
	inline TInt ModulusLength() {return iModuluslength;}
    ~CDHKeys();
private:
    CUtlDiffieHellman* iDHKey;
    const HBufC8*      iPubKey;
	TInt               iModuluslength;
};


class IkeCrypto
{
public:
	static void DecryptL(const TUint8* aInputPayload, TUint8* aOutputPayload, TInt aLength, TUint8* aIV, 
                         const TDesC8& aKey, TUint16 aEncrAlg);
	static void EncryptL(const TDesC8& aInput, TPtr8& aOutput, const TDesC8& aIv, const TDesC8& aKey, TUint16 aEncrAlg);
	static TInt IntegHMACL(const TDesC8& aInput, TDes8& aChecksum, const TDesC8& aKeyData, TUint16 aIntegAlg);	
	static HBufC8* PrfhmacL(const TDesC8& aInput, const TDesC8& aKeyData, TUint16 aPrfAlg);
	static HBufC8* PrfL(const TDesC8& aInput, TUint16 aPrfAlg);	
	static TInt AlgorithmInfo(TUint16 aTransform, TUint16 aAlgCode, TInt* aBlockLth=NULL,
							  TUtlCrypto::TUtlSymmetricCipherId* aCipherId=NULL,
							  TUtlCrypto::TUtlMessageDigestId*   aDigestId=NULL);
	static HBufC8* GenerateKeyingMaterialL(const TDesC8& aK, const TDesC8& aS, TInt aKeyMatLth, TUint16 aPRFAlg);			
};
	
#endif
