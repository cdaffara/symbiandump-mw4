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
* Description:   Class, which provides operation for deleting, saving and creating keypairs.
*
*/



#include <mctauthobject.h>

#include "keymanager.h"
#include "logonservices.h"
#include "utlcrypto.h"
#include "base64.h"
#include "pkiserviceassert.h"
#include "pkiserviceconstants.h"

const TTimeIntervalYears KValidityPeriod(20);
_LIT_SECURITY_POLICY_C1(KSymbianKeyStoreMgmtPolicy, ECapabilityWriteDeviceData);
_LIT_SECURITY_POLICY_C1(KSymbianKeyStoreUsePolicy, ECapabilityReadDeviceData);

CKeyManager* CKeyManager::NewL(CLogonServices& aLogonServices)
    {
    CKeyManager* self = new (ELeave) CKeyManager(aLogonServices);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);    
    return self;
    }


CKeyManager::CKeyManager(CLogonServices& aLogonServices)
:CActive(EPriorityNormal), iLogonServices(aLogonServices)
    {
    CActiveScheduler::Add(this);
    }


void CKeyManager::ConstructL()
    {
    }


CKeyManager::~CKeyManager()
    {
    Cleanup();
    }


void CKeyManager::RemoveKeyPair(const TPKIKeyIdentifier& aKeyId, 
                                CUnifiedKeyStore& aUnifiedKeyStore,
                                TInt aUsedKeyStore,
                                TRequestStatus& aClientStatus)
    {
    PKISERVICE_ASSERT(iState == EKeyManagerIdle);
    PKISERVICE_ASSERT(iClientStatus == NULL);
    PKISERVICE_ASSERT(aUsedKeyStore == STORETYPE_USER_KEY_ID ||
                   aUsedKeyStore == STORETYPE_DEVICE_KEY_ID ||
                   aUsedKeyStore == STORETYPE_ANY_KEY_ID);
    
    
    iState = ERetrievingKeyPairForRemove;
    
    iClientStatus = &aClientStatus;
    *iClientStatus = KRequestPending;
    
    iUsedKeyStore = aUsedKeyStore;
    iUnifiedKeyStore = &aUnifiedKeyStore;
    
    TCTKeyAttributeFilter filter;
    filter.iKeyId = aKeyId;

    iUnifiedKeyStore->List(iKeysList, filter, iStatus);
    SetActive();        
    }


void CKeyManager::GenerateKeyPair(CUnifiedKeyStore& aUnifiedKeyStore,
                                  TInt aUsedKeyStore, 
                                  const TUint aKeySize, 
                                  TPKIKeyAlgorithm aKeyAlgorithm,
                                  TPKIKeyIdentifier& aKeyId,
                                  TRequestStatus& aClientStatus)                          
    {        
    PKISERVICE_ASSERT(iState == EKeyManagerIdle);
    PKISERVICE_ASSERT(iObjectName == NULL);    
    PKISERVICE_ASSERT(iClientStatus == NULL);    
    PKISERVICE_ASSERT(aUsedKeyStore == STORETYPE_USER_KEY_ID ||
                   aUsedKeyStore == STORETYPE_DEVICE_KEY_ID ||
                   aUsedKeyStore == STORETYPE_ANY_KEY_ID);

    iState = EGeneratingKeyPair;

    iKeyId = &aKeyId;
    iClientStatus = &aClientStatus;
    *iClientStatus = KRequestPending;
            
    iUnifiedKeyStore = &aUnifiedKeyStore;   
                 
    if (aUsedKeyStore == STORETYPE_ANY_KEY_ID)
        {
        //If any type is used the key is greated in 
        //user store.
        aUsedKeyStore = STORETYPE_USER_KEY_ID;
        }
            
    TRAPD(err, iObjectName = GetUniqueNameL());
    if (err == KErrNone)
        {                   
        TTime startDate;
        TTime endDate;
        startDate.UniversalTime();
        endDate.UniversalTime();
        endDate += KValidityPeriod;
                    
        TInt keyStoreCount = iUnifiedKeyStore->KeyStoreManagerCount(); 
        TInt i = 0;
        for (i = 0; i < keyStoreCount; ++i)
            {
            MCTKeyStoreManager& keyStore = iUnifiedKeyStore->KeyStoreManager(i);        
            if (keyStore.Token().TokenType().Type().iUid == aUsedKeyStore)            
                {
                break;
                }
            }
        PKISERVICE_ASSERT(i < keyStoreCount);
        
         
        iUnifiedKeyStore->CreateKey(i, EPKCS15UsageSignDecrypt, 
                                    aKeySize,
                                    *iObjectName,
                                    ConvertPKIAlgorithm(aKeyAlgorithm),
                                    CKeyInfoBase::EExtractable,
                                    startDate,
                                    endDate,
                                    iKeyInfo,
                                    iStatus);        
        SetActive();

        }
    else    
        {
        TRequestStatus* ownStatus = &iStatus;
        *ownStatus = KRequestPending;
        SetActive();
        
        User::RequestComplete(ownStatus, err);
        }
    }


void CKeyManager::ImportKeyPair(CUnifiedKeyStore& aUnifiedKeyStore,
                                TInt aUsedKeyStore, 
                                const TDesC8& aKeyData,
                                TPKIKeyIdentifier& aKeyId,
                                TRequestStatus& aClientStatus)
    {
    PKISERVICE_ASSERT(iState == EKeyManagerIdle);
    PKISERVICE_ASSERT(iObjectName == NULL);    
    PKISERVICE_ASSERT(iClientStatus == NULL);  
    PKISERVICE_ASSERT(aUsedKeyStore == STORETYPE_USER_KEY_ID ||
                   aUsedKeyStore == STORETYPE_DEVICE_KEY_ID ||
                   aUsedKeyStore == STORETYPE_ANY_KEY_ID);
      

    iState = EImportingKeyPair;

    iKeyId = &aKeyId;
    iClientStatus = &aClientStatus;
    *iClientStatus = KRequestPending;
            
    iUnifiedKeyStore = &aUnifiedKeyStore;            
          
    if (aUsedKeyStore == STORETYPE_ANY_KEY_ID)
        {
        //If any type is used the key is greated in 
        //user store.
        aUsedKeyStore = STORETYPE_USER_KEY_ID;
        }
          
            
    TRAPD(err, iObjectName = GetUniqueNameL());
    if (err == KErrNone)
        {                   
        TTime startDate;
        TTime endDate;
        startDate.UniversalTime();
        endDate.UniversalTime();
        endDate += KValidityPeriod;
        
        TInt keyStoreCount = iUnifiedKeyStore->KeyStoreManagerCount(); 
        TInt i = 0;
        for (i = 0; i < keyStoreCount; ++i)
            {
            MCTKeyStoreManager& keyStore = iUnifiedKeyStore->KeyStoreManager(i);        
            if (keyStore.Token().TokenType().Type().iUid == aUsedKeyStore)            
                {
                break;
                }
            }
        PKISERVICE_ASSERT(i < keyStoreCount);
        
         
        iUnifiedKeyStore->ImportKey(i, aKeyData,
                                    EPKCS15UsageSignDecrypt, 
                                    *iObjectName,
                                    CKeyInfoBase::EExtractable,
                                    startDate,
                                    endDate,
                                    iKeyInfo,
                                    iStatus);        
        SetActive();
        }
    else    
        {
        TRequestStatus* ownStatus = &iStatus;
        *ownStatus = KRequestPending;
        SetActive();
        
        User::RequestComplete(ownStatus, err);
        }    
    }


void CKeyManager::ExportPublicKey(CUnifiedKeyStore& aUnifiedKeyStore,
                                  TInt aUsedKeyStore,                          
                                  const TPKIKeyIdentifier& aKeyId,
                                  HBufC8*& aPublicKeyData,
                                  TRequestStatus& aClientStatus)
    {
    PKISERVICE_ASSERT(iState == EKeyManagerIdle);
    PKISERVICE_ASSERT(iPublicKeyData == NULL);    
    PKISERVICE_ASSERT(iClientStatus == NULL);  
    PKISERVICE_ASSERT(aUsedKeyStore == STORETYPE_USER_KEY_ID ||
                   aUsedKeyStore == STORETYPE_DEVICE_KEY_ID ||
                   aUsedKeyStore == STORETYPE_ANY_KEY_ID);
                   
    iState = ERetrievingKeyListForExport;                   
    iPublicKeyData = &aPublicKeyData;    
    iUsedKeyStore = aUsedKeyStore;

    iClientStatus = &aClientStatus;
    *iClientStatus = KRequestPending;
            
    iUnifiedKeyStore = &aUnifiedKeyStore;            

    
	TCTKeyAttributeFilter filter;
    filter.iKeyId = aKeyId;

	iUnifiedKeyStore->List(iKeysList, filter, iStatus);
	SetActive();        
    }



void CKeyManager::RunL()
    {
    if (iStatus.Int() == KErrNone)
        {        
        switch(iState)
            {
            case ERetrievingKeyPairForRemove:                
                {                                        
                TInt keyIndex = GetKeyIndex(iUsedKeyStore, iKeysList);
                if ( keyIndex >= 0)
                    {                    
                    iState = ERemovingKeyPair;
                    iUnifiedKeyStore->DeleteKey(iKeysList[keyIndex]->Handle(), iStatus);
                    SetActive();
                    }
                else
                    {
                    Cleanup();
                    User::RequestComplete(iClientStatus, KPKIErrNotFound);                    
                    }                
                }
                break;
            case ERemovingKeyPair:
                Cleanup(); 
                User::RequestComplete(iClientStatus, iStatus.Int());   
                break;   
            case EImportingKeyPair: //falls through                
            case EGeneratingKeyPair:                  
                {                                  
                iState = ESettingManagementPolicy;   
                
                MCTAuthenticationObject* authObject = iKeyInfo->Protector();
                if (authObject != NULL)
                    {
                    //authObject is NULL for device store
                    iLogonServices.SetAuthenticationObject(authObject);
                    }
                 
                iUnifiedKeyStore->SetManagementPolicy(iKeyInfo->Handle(), 
                                                      KSymbianKeyStoreMgmtPolicy, 
                                                      iStatus);                    
                SetActive();
                }
                break;
            case ESettingManagementPolicy:
                iState = ESettingUsePolicy;
                iUnifiedKeyStore->SetUsePolicy(iKeyInfo->Handle(), 
                                               KSymbianKeyStoreUsePolicy, 
                                               iStatus);
                SetActive();                                               
                break;
            case ESettingUsePolicy:
                *iKeyId = iKeyInfo->ID();
                Cleanup();
                User::RequestComplete(iClientStatus, KErrNone);
                break;
            case ERetrievingKeyListForExport:
                {                    
                TInt keyIndex = GetKeyIndex(iUsedKeyStore, iKeysList);
                if ( keyIndex >= 0)
                    {                    
                    iState = EExportingPublicKey;                    
                    TCTTokenObjectHandle tokenHandle = iKeysList[keyIndex]->Handle();                    
                    iUnifiedKeyStore->ExportPublic(tokenHandle, *iPublicKeyData, iStatus);
                    SetActive();
                    
                    }
                else
                    {
                    Cleanup();
                    User::RequestComplete(iClientStatus, KPKIErrNotFound);                    
                    }                            
                }
                break;
            case EExportingPublicKey:
                {                    
                iState = EKeyManagerIdle;     
                                           
                if (iStatus.Int() == KErrNone)
                    {                                                
                    TPtr8 publicKeyPtr = (*iPublicKeyData)->Des();

                    // Fix length and strip header (not a
                    // perfect solution!), but certificate
                    // enrollment request wants to have
                    // only PKCS#1 key data.
                    TInt tempLength = 0;
                    TInt skip = 0;
                    if(publicKeyPtr[1] == 0x82)
                        {
                        tempLength = (publicKeyPtr[2] << 8) + publicKeyPtr[3] + 4 - 0x18;
                        skip = 0x18;
                        }
                    else
                        {
                        tempLength = publicKeyPtr[2] + 3 - 0x16;
                        skip = 0x16;
                        }
                        
                    PKISERVICE_ASSERT(tempLength <= publicKeyPtr.MaxLength());
                    publicKeyPtr.Copy(publicKeyPtr.Ptr() + skip, tempLength);                                
                    }
                else
                		{
                		delete *iPublicKeyData;	
                		*iPublicKeyData = NULL;
                		}
                Cleanup();
                User::RequestComplete(iClientStatus, iStatus.Int());                       
                }
                break;                
            default:
                PKISERVICE_INVARIANT();            
            }    
        }
    else
        {
        Cleanup();
        User::RequestComplete(iClientStatus, iStatus.Int());
        }
    }


void CKeyManager::DoCancel()
    {
    switch(iState)
        {
        case ERetrievingKeyListForExport: //falls through
        case ERetrievingKeyPairForRemove:
            iUnifiedKeyStore->CancelList();
            break;
        case ERemovingKeyPair:
            iUnifiedKeyStore->CancelDeleteKey();
            break;
        case EGeneratingKeyPair:
            iUnifiedKeyStore->CancelCreateKey();
            break;
        case EImportingKeyPair:
            iUnifiedKeyStore->CancelImportKey();
            break;
        case ESettingManagementPolicy:
            iUnifiedKeyStore->CancelSetManagementPolicy();
            break;
        case ESettingUsePolicy:
            iUnifiedKeyStore->CancelSetUsePolicy();
            break;
        case EExportingPublicKey:
            iUnifiedKeyStore->CancelExportPublic();
            delete *iPublicKeyData;
            *iPublicKeyData = NULL;
            break;
        default:
            PKISERVICE_INVARIANT();            
        }
    Cleanup();        
    User::RequestComplete(iClientStatus, KErrCancel);
    }


void CKeyManager::RunError()
    {
    //RunL doesn't leave
    PKISERVICE_INVARIANT();
    }

void CKeyManager::Cleanup()
    {
    iState = EKeyManagerIdle;   
    
    iPublicKeyData = NULL;
    
    if (iKeyInfo != NULL)
        {
        iKeyInfo->Release();
        iKeyInfo = NULL;
        }
    iKeysList.Close();
          
    delete iObjectName;
    iObjectName = NULL;          
          
    iUnifiedKeyStore = NULL;        
    
    iUsedKeyStore = 0;    
    }


HBufC* CKeyManager::GetUniqueNameL() const
    {
    TBuf<MAX_FILENAME_LENGTH> date;
    TTime time;
    TDateTime dateTime;

    time.HomeTime();
    dateTime = time.DateTime();

    TBuf8<16> dateString;

    _LIT8(KFormatTxt,"%4d%02d%02d%02d%02d%02d%02d");
    dateString.Format(KFormatTxt,
                      dateTime.Year(),
                      dateTime.Month()+1, 
                      // Format the month as a TInt to preserve locale independence
                      dateTime.Day()+1, 
                      // Day and month ranges begin at zero (0-30 and 0-11), 
                      // so add one when formatting
                      dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond()
                      );

    TPKISHA1Hash hash;
    CUtlMessageDigest* digester = TUtlCrypto::MakeMessageDigesterL(TUtlCrypto::EUtlMessageDigestSha1);   
    CleanupStack::PushL(digester);
    TPtrC8 hashValue = digester->Final(dateString);
    
    TBase64Codec base64Codec;    
    HBufC8* uniqueName8 = base64Codec.Base64EncodeLC(hashValue);
    TPtr8 uniqueName8Ptr = uniqueName8->Des();

    // Replace /
    for(TInt i = 0; i < uniqueName8->Length(); i++)
        {
        if(uniqueName8Ptr[i] == '/')
            {
            uniqueName8Ptr[i] = '_';
            }
        }
    
    HBufC* uniqueName = HBufC::NewL(uniqueName8->Length());
    uniqueName->Des().Copy(*uniqueName8);
    
    CleanupStack::PopAndDestroy(uniqueName8);  
    CleanupStack::PopAndDestroy(digester);  
            
    return uniqueName;
    }


CCTKeyInfo::EKeyAlgorithm CKeyManager::ConvertPKIAlgorithm(TPKIKeyAlgorithm aAlg) const
    {
    CCTKeyInfo::EKeyAlgorithm algorithm = CCTKeyInfo::EInvalidAlgorithm;

    switch(aAlg)
        {
        case EPKIRSA:
            algorithm = CCTKeyInfo::ERSA;
            break;
        case EPKIDSA:
            algorithm = CCTKeyInfo::EDSA;
            break;
        case EPKIDH:
            algorithm = CCTKeyInfo::EDH;
            break;
        default:
            break;
        }
    return algorithm;
    }


TInt CKeyManager::GetKeyIndex(TInt aUsedKeyStore, const RMPointerArray<CCTKeyInfo>& aKeysList) const
    {
    TInt i;
    for (i = 0; i < aKeysList.Count(); ++i)
        {
        if (aUsedKeyStore == STORETYPE_ANY_KEY_ID ||
            aUsedKeyStore == aKeysList[i]->Token().TokenType().Type().iUid)
            {
            break;
            }
        }
    if ( i >= aKeysList.Count())
        {                    
        i = KErrNotFound;
        }    
    
    return i;
    }
