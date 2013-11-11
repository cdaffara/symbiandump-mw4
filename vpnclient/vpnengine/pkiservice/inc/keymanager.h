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




#ifndef C_KEYMANAGER_H
#define C_KEYMANAGER_H

#include <e32base.h>
#include <unifiedkeystore.h>
#include "pkidefs.h"

class CUnifiedKeyStore;
class CCTKeyInfo;
class CLogonServices;

/**
 * A class that handles simple access to the keypairs.
 *
 * This class provieds following simple key access functionality:
 * key pair removal, key pair generation, key pair import and public key
 * export.
 */
class CKeyManager : public CActive
    {
public:
    static CKeyManager* NewL(CLogonServices& aLogonServices);
    ~CKeyManager();

    void RemoveKeyPair(const TPKIKeyIdentifier& aKeyId,                        
                       CUnifiedKeyStore& aUnifiedKeyStore,
                       TInt aUsedKeyStore,
                       TRequestStatus& aClientStatus);
                       
    void GenerateKeyPair(CUnifiedKeyStore& aUnifiedKeyStore,
                         TInt aUsedKeyStore, 
                         const TUint aKeySize, 
                         TPKIKeyAlgorithm aKeyAlgorithm,
                         TPKIKeyIdentifier& aKeyId,
                         TRequestStatus& aClientStatus);                          
                         
    void ImportKeyPair(CUnifiedKeyStore& aUnifiedKeyStore,
                       TInt aUsedKeyStore, 
                       const TDesC8& aKeyData,
                       TPKIKeyIdentifier& aKeyId,
                       TRequestStatus& aClientStatus);                          
                       
    void ExportPublicKey(CUnifiedKeyStore& aUnifiedKeyStore,
                         TInt aUsedKeyStore,                          
                         const TPKIKeyIdentifier& aKeyId,
                         HBufC8*& aPublicKeyData,
                         TRequestStatus& aClientStatus);                          
                       

protected:

    void RunL();
    void DoCancel();
    void RunError();

private:
    enum TKeyManagerState
        {
        EKeyManagerIdle = 0,
        ERetrievingKeyPairForRemove,
        ERemovingKeyPair,
        EGeneratingKeyPair,
        EImportingKeyPair,
        ESettingManagementPolicy,
        ESettingUsePolicy,
        ERetrievingKeyListForExport,
        EExportingPublicKey
        };

    CKeyManager(CLogonServices& aLogonServices);
    void ConstructL();    
    void Cleanup();
    
    TInt GetKeyIndex(TInt aUsedKeyStore, const RMPointerArray<CCTKeyInfo>& aKeysList) const;
    
    HBufC* GetUniqueNameL() const;
    CCTKeyInfo::EKeyAlgorithm ConvertPKIAlgorithm(TPKIKeyAlgorithm aAlg) const;
    
    CLogonServices&  iLogonServices;
    TKeyManagerState iState;
    
    TRequestStatus* iClientStatus;    
    CUnifiedKeyStore* iUnifiedKeyStore; //Not owned by this class
    RMPointerArray<CCTKeyInfo> iKeysList;
    
    TInt iUsedKeyStore;
    
    HBufC*             iObjectName;
    HBufC8**           iPublicKeyData; //Now owned by this class
    TPKIKeyIdentifier* iKeyId; //Not owned by this class    
    CCTKeyInfo*        iKeyInfo; //Not owned by this class           
    };

#endif //C_KEYMANAGER_H