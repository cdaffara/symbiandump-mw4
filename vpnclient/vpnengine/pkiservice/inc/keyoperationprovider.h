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
* Description:   PKI server main module
*
*/




#ifndef C_KEYOPERATIONPROVIDER_H
#define C_KEYOPERATIONPROVIDER_H

#include <e32base.h>
#include <f32file.h>
#include <unifiedkeystore.h>
#include "pkidefs.h"

class CUnifiedKeyStore;
class CPKISession;
class CPkiServiceSigner;
class CPkiServiceDecryptor;
class CLogonServices;
class CKeyManager;
class CKeyOperationQueue;
    
/**
* A class that collects all the keyoperation functionality.
*
* The instance of this class is owned by CKeyOperationQueue, which
* calls the methods according to the client request. When an operation
* is completed the CKeyPairOperationProvider calls the associated callback
* from CKeyOperationQueue.
*
* The Initialize operation must be completed succesfully, before any other
* requests can be made to this class.
*
* Any current time, there can be only one request under process.
*/
class CKeyPairOperationProvider : public CActive
    {
public:
    static CKeyPairOperationProvider* NewL(CKeyOperationQueue& aKeyOperationQueue);
     ~CKeyPairOperationProvider();

    /**
     * Initializes the keystore.
     * This is an asynchronous method. 
     * CPKISession::KeyStoreInitComplete is called when this
     * request is completed.
     */
    void Initialize();         
    void GetKeyPairList(const TPKIKeyIdentifier& aKeyId, TInt aUsedKeyStore);
                   
    /**
     * Decrypts the data using key specified in aKeyId.
     * The key has to be stored in the store defined by
     * method SetKeyStore.
     */
    void Decrypt(const TPKIKeyIdentifier& aKeyId,
                 TInt aUsedKeyStore,
                 HBufC8* aEncryptedData,
                 TInt aOutputLength);  
                 
    void Sign(const TPKIKeyIdentifier& aKeyId,
              TInt aUsedKeyStore,
              HBufC8* aDataToBeSigned);  
       
    void GetPublicKey(const TPKIKeyIdentifier& aKeyId, TInt aUsedKeyStore);        
            
    void Logon();
    void Logoff();
    void ChangePassword();
    
    void RemoveKeyPair(const TPKIKeyIdentifier& aKeyId, TInt aUsedKeyStore);        
    void GenerateKeyPair(const TUint aKeySize, TPKIKeyAlgorithm aKeyAlgorithm, TInt aUsedKeyStore);   
            
    void ImportKeyPair(HBufC8* aKeyData, TInt aUsedKeyStore);   //Takes the ownership of the data
    
protected:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
    
private:
	enum TKeyOperation
		{
		EKeyOperationIdle = 0,
		EKeyOperationInitialize,
		EKeyOperationSetPassPhraseTimeout,
		EKeyOperationGetKeyList,
		EKeyOperationGetKeyDetails,
		EKeyOperationDecrypting,
		EKeyOperationSigning,		
		EKeyOperationPublicKeyExport,
		EKeyOperationLogon,
		EKeyOperationLogoff,
		EKeyOperationLogonForChangePassword,
		EKeyOperationChangingPassword,
		EKeyOperationRemoveKeyPair,
		EKeyOperationGeneratingKeyPair,
		EKeyOperationImportingKeyPair
		};

    CKeyPairOperationProvider(CKeyOperationQueue& aKeyOperationQueue);
    void ConstructL();


    void DecryptL(const TPKIKeyIdentifier& aKeyId,
                  TInt aUsedKeyStore,
                  HBufC8* aEncryptedData,
                  TInt aOutputLength);                                    

    void CleanupCryptoOperation();

    CArrayFixFlat<TKeyListEntry>* MakeKeyEntryListL(RMPointerArray<CCTKeyInfo> aKeysList,
                                                    TInt aUsedKeyStore) const;

    TBool 				 iIsInitialized;
    TKeyOperation        iKeyOperation;

    CKeyOperationQueue&     iKeyOperationQueue;
    RFs 				    iFileServer;
    CUnifiedKeyStore* 	    iUnifiedKeyStore; 
    TInt                    iUsedKeyStore; //Not owned by this instance.
    
    RMPointerArray<CCTKeyInfo> iKeysList;
    
    HBufC8*                    iInputData;  //Used by Decrypt and Sign operations.
    HBufC8*                    iOutputData; //Used by Decrypt and Sign operations.
    TPtr8                      iOutputDataPtr;
    
    TPKIKeyIdentifier          iKeyId; //Used by store and generate keypair
    
    CPkiServiceDecryptor*      iPkiDecryptor;    
    CPkiServiceSigner*         iPkiSigner;
    CLogonServices*            iLogonService;
    CKeyManager*               iKeyManager;
    };

#endif // C_KEYOPERATIONPROVIDER_H
