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
* Description:   A task que to serialize the key operations among session.
*
*/



#ifndef C_KEYOPERATIONQUEUE_H
#define C_KEYOPERATIONQUEUE_H

#include <e32base.h>

#include "pkidefs.h"

class CPKISession;
class CKeyPairOperationProvider;
class CPKIMapper;


/**
 * A queueing class for keypair operations.
 *
 * This class queues the keypairoperations issued by client sessions.
 * The keyoperations are queued, because only one session handle to 
 * CUnifiedKeystore can be open in this process and the CUnifiedKeystore
 * can't handle concurrent asynchronous operations simultaniously. Only one
 * session handle can be open to CUnifiedKeystore, because of the required 
 * Login/Logoff functionality.
 *
 * Server side session add the operations to que by calling AddOperationL.
 * The operations are handled one at the time. When the operation is completed, the
 * key operation queue signals the associated RMessage.
 *
 * Rest of the operations are call backs, which are used to signal, that the current
 * operation is completed.
 *
 */
class CKeyOperationQueue : public CBase
    {
public:
    static CKeyOperationQueue* NewL(CPKIMapper& aMapper);
    ~CKeyOperationQueue();
    
    void AddOperationL(CPKISession& aOwner, 
                       const RMessage2& aMessage,
                       TInt aUsedKeyStore,
                       TPkiServiceStoreType aUsedCertStore);
    
    
    void KeyStoreInitComplete(TInt aStatus);

    /**
    *  Transfers the ownership of aKeyList.
    */                
    void KeyPairListComplete(TInt aStatus, CArrayFixFlat<TKeyListEntry>* aKeyList);

    /**
    *  Transfers the ownership of aDecryptedData.
    */
    void DecryptComplete(TInt aStatus, HBufC8* aDecryptedData);

    /**
    *  Transfers the ownership of aSignedData.
    */        
    void SignComplete(TInt aStatus, HBufC8* aSignedData);

    /**
    *  Transfers the ownership of aPublicKeyData.
    */                
    void GetPublicKeyCompleted(TInt aStatus, HBufC8* aPublicKeyData);

    void LogonCompleted(TInt aStatus);
    void LogoffCompleted(TInt aStatus);
    void PasswordChangeCompleted(TInt aStatus);
    void KeyPairRemoveCompleted(TInt aStatus);
    void KeyGenerationCompleted(TInt aStatus, TPKIKeyIdentifier& aKeyId);
    void StoreKeyPairCompleted(TInt aStatus, TPKIKeyIdentifier& aKeyId);

    
private:           

    class TKeyOperation
        {
        public:
        TKeyOperation(CPKISession& aOwner, 
                      const RMessage2& aMessage,
                      TInt aUsedKeyStore,
                      TPkiServiceStoreType aUsedCertStore);
        
        CPKISession& iOwner;
        const RMessage2 iMessage;    
        TInt iUsedKeyStore; 
        TPkiServiceStoreType iUsedCertStore;
        };
        

    CKeyOperationQueue(CPKIMapper& aMapper);
    void ConstructL();                
    
    void StartNextOperation();
    void ServiceL(TKeyOperation& aOperation);
    
    void CompleteCurrentOperation(TInt aStatus);
    
    CPKIMapper&                  iMapper;
    RPointerArray<TKeyOperation> iOperationQue;
    TKeyOperation*               iCurrentOperation;             
    
    CKeyPairOperationProvider*   iKeyOperationProvider;
    };

#endif //C_KEYOPERATIONQUEUE_H
