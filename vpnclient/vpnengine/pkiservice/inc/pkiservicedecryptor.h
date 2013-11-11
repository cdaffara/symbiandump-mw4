/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: A class that provides a decrypt operation.
*
*/



#ifndef C_PKISERVICEDECRYPTOR_H
#define C_PKISERVICEDECRYPTOR_H

#include <e32base.h>
#include <unifiedkeystore.h>

#include "pkidefs.h"

class MCTKeyStoreManager;
class CLogonServices;


/**
 * A class that decrypts data using a private key.
 *
 * This class handles the decryption of a data, which is
 * encrypted by using a public key from a stored keypair.
 */
class CPkiServiceDecryptor : public CActive
    {
public:
    static CPkiServiceDecryptor* NewL(CLogonServices& aLogonServices);
    ~CPkiServiceDecryptor();

    void Decrypt(const TPKIKeyIdentifier& aKeyId,
                 const TDesC8& aEncryptedData,
                 TPtr8& aPlainTextData,
                 CUnifiedKeyStore& aUnifiedKeyStore,
                 TInt aUsedKeyStore, 
                 TRequestStatus& aClientstatus);


    void RunL();
    void DoCancel();

private:
    enum TDecryptorState
        {
        EDecryptorIdle = 0,
        EDecryptorGettingKey,
		EDecryptorOpeningDecryptor,
		EDecryptorDecrypting        
        };

    CPkiServiceDecryptor(CLogonServices& aLogonServices);
    void Cleanup();
    
    CLogonServices&     iLogonServices;
    TDecryptorState     iState;
    CUnifiedKeyStore*   iKeyStore; //Not owned by this class
    TInt                iUsedKeyStore;
    const TDesC8*       iInput;    //Not owned by this class
    TPtr8*              iPlainText;//Not owned by this class
    TRequestStatus*     iClientStatus;
    
    MCTDecryptor*              iDecryptor;        
    RMPointerArray<CCTKeyInfo> iKeysList;
    
    CCTKeyInfo*                iKeyInfo;
    };

#endif //C_PKISERVICEDECRYPTOR_H
