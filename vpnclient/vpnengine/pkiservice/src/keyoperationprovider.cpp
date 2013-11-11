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
* Description:   Class, which provides keypair operations.
*
*/


#include <asymmetric.h>

#include "keyoperationprovider.h"
#include "pkiservicesigner.h"
#include "pkiservicedecryptor.h"
#include "pkisession.h"
#include "pkidefs.h"
#include "logonservices.h"
#include "keymanager.h"
#include "pkiserviceassert.h"
#include "keyoperationqueue.h"

static const TInt KTimeoutNever = -1;

CKeyPairOperationProvider* CKeyPairOperationProvider::NewL(CKeyOperationQueue& aKeyOperationQueue)
    {
    CKeyPairOperationProvider* self = new (ELeave)CKeyPairOperationProvider(aKeyOperationQueue);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    return self;
    }


CKeyPairOperationProvider::CKeyPairOperationProvider(CKeyOperationQueue& aKeyOperationQueue) 
:CActive(EPriorityStandard), iKeyOperationQueue(aKeyOperationQueue), iOutputDataPtr(NULL, 0)
    {
    CActiveScheduler::Add(this);
    }


void CKeyPairOperationProvider::ConstructL()
    {    
    User::LeaveIfError(iFileServer.Connect());
    iUnifiedKeyStore = CUnifiedKeyStore::NewL(iFileServer);        
    iLogonService = CLogonServices::NewL(*iUnifiedKeyStore);
    iKeyManager = CKeyManager::NewL(*iLogonService);
    iPkiSigner = CPkiServiceSigner::NewL(*iLogonService);
    iPkiDecryptor = CPkiServiceDecryptor::NewL(*iLogonService);
    }


CKeyPairOperationProvider::~CKeyPairOperationProvider()
    {
    Cancel();
    
    delete iPkiDecryptor;
    delete iPkiSigner;
    delete iKeyManager;
    delete iLogonService;
    delete iUnifiedKeyStore;
    iFileServer.Close();
    }


void CKeyPairOperationProvider::Initialize()
	{	
	PKISERVICE_ASSERT(!IsActive());
	PKISERVICE_ASSERT(iKeyOperation == EKeyOperationIdle);
	
	iKeyOperation = EKeyOperationInitialize;
	if (!iIsInitialized)
    	{	    	    
    	iUnifiedKeyStore->Initialize(iStatus);
    	SetActive();
    	}
    else
        {
        TRequestStatus* ownStatus = &iStatus;
        *ownStatus = KRequestPending;        
        SetActive();
        User::RequestComplete(ownStatus, KErrNone);
        }
	}


void CKeyPairOperationProvider::GetKeyPairList(const TPKIKeyIdentifier& aKeyId, TInt aUsedKeyStore)
	{
	PKISERVICE_ASSERT(iIsInitialized);
    PKISERVICE_ASSERT(!IsActive());	
	PKISERVICE_ASSERT(iKeyOperation == EKeyOperationIdle);    
	
	iKeyOperation = EKeyOperationGetKeyList;
	iUsedKeyStore = aUsedKeyStore;
	
    TCTKeyAttributeFilter filter;    
    if (aKeyId.Length() > 0)
        {        
        filter.iKeyId = aKeyId;
        }
    
	iUnifiedKeyStore->List(iKeysList, filter, iStatus);
	SetActive();
	}


void CKeyPairOperationProvider::Decrypt(const TPKIKeyIdentifier& aKeyId,
                                        TInt aUsedKeyStore,
                                        HBufC8* aEncryptedData,
                                        TInt aOutputLength)
    {
    PKISERVICE_ASSERT(iIsInitialized);
    PKISERVICE_ASSERT(!IsActive());    
	PKISERVICE_ASSERT(iKeyOperation == EKeyOperationIdle);
    PKISERVICE_ASSERT(aKeyId.Length() > 0);
        
    iKeyOperation = EKeyOperationDecrypting;
    TRAPD(err, DecryptL(aKeyId, aUsedKeyStore, aEncryptedData, aOutputLength));
    if (err != KErrNone)
        {
        iStatus = KRequestPending;
        TRequestStatus* ownStatus = &iStatus;
        SetActive();
        
        User::RequestComplete(ownStatus, err);
        }   
    }


void CKeyPairOperationProvider::DecryptL(const TPKIKeyIdentifier& aKeyId,
                                         TInt aUsedKeyStore,
                                         HBufC8* aEncryptedData,
                                         TInt aOutputLength)
    {
    PKISERVICE_ASSERT(iKeysList.Count() == 0);
    PKISERVICE_ASSERT(iInputData == NULL);
    PKISERVICE_ASSERT(iKeyOperation == EKeyOperationDecrypting);        
          
    iInputData = aEncryptedData;
    iOutputData = HBufC8::NewL(aOutputLength);
    iOutputDataPtr.Set(iOutputData->Des());
    
    iPkiDecryptor->Decrypt(aKeyId,
                           *iInputData,
                           iOutputDataPtr,
                           *iUnifiedKeyStore,
                           aUsedKeyStore,
                           iStatus);
                            
    SetActive();                                
    }


void CKeyPairOperationProvider::Sign(const TPKIKeyIdentifier& aKeyId,
                                     TInt aUsedKeyStore,
                                     HBufC8* aDataToBeSigned)
    {
    PKISERVICE_ASSERT(iIsInitialized);
    PKISERVICE_ASSERT(!IsActive());    
	PKISERVICE_ASSERT(iKeyOperation == EKeyOperationIdle);
    PKISERVICE_ASSERT(aKeyId.Length() > 0);    
    PKISERVICE_ASSERT(iKeysList.Count() == 0);
    PKISERVICE_ASSERT(iInputData == NULL);
    
    
    iKeyOperation = EKeyOperationSigning;
    iInputData = aDataToBeSigned;
    
    iPkiSigner->Sign(aKeyId,
                     *iInputData,
                     iOutputData,
                     *iUnifiedKeyStore,
                     aUsedKeyStore,
                     iStatus);
    SetActive();                  
    }


void CKeyPairOperationProvider::Logon()
    {
    PKISERVICE_ASSERT(iIsInitialized);
    PKISERVICE_ASSERT(!IsActive()); 
    PKISERVICE_ASSERT(iKeyOperation == EKeyOperationIdle);           
    
    iKeyOperation = EKeyOperationLogon;
    iLogonService->Logon(iStatus);
    SetActive();		
    }


void CKeyPairOperationProvider::Logoff()
    {
    PKISERVICE_ASSERT(iIsInitialized);
    PKISERVICE_ASSERT(!IsActive());    
    PKISERVICE_ASSERT(iKeyOperation == EKeyOperationIdle);    
   
    iKeyOperation = EKeyOperationLogoff;
    iLogonService->Logoff(iStatus);
    SetActive();		        
    }


void CKeyPairOperationProvider::ChangePassword()
    {
    PKISERVICE_ASSERT(iIsInitialized);
    PKISERVICE_ASSERT(!IsActive());    
    PKISERVICE_ASSERT(iKeyOperation == EKeyOperationIdle);    
   
    if (iLogonService->LogonCompleted())
        {
        iKeyOperation = EKeyOperationChangingPassword;
        iLogonService->ChangePassword(iStatus);
        }
    else
        {
        iKeyOperation = EKeyOperationLogonForChangePassword;
        iLogonService->Logon(iStatus);
        }    
        
    SetActive();        
    }


void CKeyPairOperationProvider::RemoveKeyPair(const TPKIKeyIdentifier& aKeyId, TInt aUsedKeyStore)
    {
    PKISERVICE_ASSERT(iIsInitialized);
    PKISERVICE_ASSERT(!IsActive());    
    PKISERVICE_ASSERT(iKeyOperation == EKeyOperationIdle);    

    iKeyOperation = EKeyOperationRemoveKeyPair;
    
    iKeyManager->RemoveKeyPair(aKeyId, *iUnifiedKeyStore, aUsedKeyStore, iStatus);
    SetActive();
    }


void CKeyPairOperationProvider::GenerateKeyPair(const TUint aKeySize, 
                                                TPKIKeyAlgorithm aKeyAlgorithm,
                                                TInt aUsedKeyStore)
    {
    PKISERVICE_ASSERT(iIsInitialized);
    PKISERVICE_ASSERT(!IsActive());    
    PKISERVICE_ASSERT(iKeyOperation == EKeyOperationIdle);    

    iKeyOperation = EKeyOperationGeneratingKeyPair;
    
    iKeyManager->GenerateKeyPair(*iUnifiedKeyStore, aUsedKeyStore, aKeySize, 
                                 aKeyAlgorithm, iKeyId, iStatus);
    SetActive();   
    }


void CKeyPairOperationProvider::ImportKeyPair(HBufC8* aKeyData, TInt aUsedKeyStore)
    {
    PKISERVICE_ASSERT(iIsInitialized);
    PKISERVICE_ASSERT(!IsActive());    
    PKISERVICE_ASSERT(iKeyOperation == EKeyOperationIdle);    

    iKeyOperation = EKeyOperationImportingKeyPair;
    iInputData = aKeyData;    
    
    iKeyManager->ImportKeyPair(*iUnifiedKeyStore, aUsedKeyStore, *iInputData, iKeyId, iStatus);
    SetActive();       
    }


void CKeyPairOperationProvider::GetPublicKey(const TPKIKeyIdentifier& aKeyId, TInt aUsedKeyStore)
    {	
    PKISERVICE_ASSERT(iIsInitialized);
    PKISERVICE_ASSERT(!IsActive());    
    PKISERVICE_ASSERT(iKeyOperation == EKeyOperationIdle);    

    iKeyOperation = EKeyOperationPublicKeyExport;
    
    iKeyManager->ExportPublicKey(*iUnifiedKeyStore, aUsedKeyStore, aKeyId, iOutputData, iStatus);
    SetActive();       	
    }



void CKeyPairOperationProvider::RunL()
	{
	switch(iKeyOperation)
		{
		case EKeyOperationInitialize:
		    {		        
			iKeyOperation = EKeyOperationIdle;	
			TInt err = iStatus.Int();		
			if (err == KErrNone)
				{
				if (!iIsInitialized)
    				{				    
    				iKeyOperation = EKeyOperationSetPassPhraseTimeout;
    			    iUnifiedKeyStore->SetPassphraseTimeout(KTimeoutNever, iStatus);	
    			    SetActive();
    				}
                else
                    {//keystore is already initialized
                    iKeyOperationQueue.KeyStoreInitComplete(err);
                    }
				}          
            else
                {                    				  				
			    iKeyOperationQueue.KeyStoreInitComplete(err);
                }
		    }
			break;
        case EKeyOperationSetPassPhraseTimeout:
            iKeyOperation = EKeyOperationIdle;
            if (iStatus.Int() == KErrNone)
                {
                iIsInitialized = ETrue;
                }
            iKeyOperationQueue.KeyStoreInitComplete(iStatus.Int());
            break;			
		case EKeyOperationGetKeyList:
		    {		        
			PKISERVICE_ASSERT(iIsInitialized);
			iKeyOperation = EKeyOperationIdle;
			
			CArrayFixFlat<TKeyListEntry>* list = NULL;
			TInt err = iStatus.Int();
			if (err == KErrNone)
    			{			    
    			TRAP(err, list = MakeKeyEntryListL(iKeysList, iUsedKeyStore));
    			}
            CleanupCryptoOperation();   			
			iKeyOperationQueue.KeyPairListComplete(err, list);
		    }
			break;
        case EKeyOperationDecrypting:
            {                
            PKISERVICE_ASSERT(iIsInitialized);            
            iKeyOperation = EKeyOperationIdle;                       
            HBufC8* plainTextData = NULL;
            if (iStatus.Int() == KErrNone)
                {       
                plainTextData = iOutputData;
                iOutputData = NULL;                
                }                
            CleanupCryptoOperation();
            iKeyOperationQueue.DecryptComplete(iStatus.Int(), plainTextData);
            }
            break;     
        case EKeyOperationSigning:
            {                
            PKISERVICE_ASSERT(iIsInitialized);                       
            iKeyOperation = EKeyOperationIdle;
            HBufC8* data = iOutputData;
            iOutputData = NULL;
            CleanupCryptoOperation(); 
            iKeyOperationQueue.SignComplete(iStatus.Int(), data);                             
            }
            break; 
        case EKeyOperationPublicKeyExport:
            {
            PKISERVICE_ASSERT(iIsInitialized);            
            iKeyOperation = EKeyOperationIdle;     
                        
            PKISERVICE_ASSERT((iStatus.Int() == KErrNone && iOutputData != NULL) ||
                           (iStatus.Int() != KErrNone && iOutputData == NULL));
                                                      
            HBufC8* publicKeyData = iOutputData;
            iOutputData = NULL;
            CleanupCryptoOperation();                 
            iKeyOperationQueue.GetPublicKeyCompleted(iStatus.Int(), publicKeyData);                                      
            }
            break;
		case EKeyOperationLogon:
		    iKeyOperation = EKeyOperationIdle;     
            iKeyOperationQueue.LogonCompleted(iStatus.Int());
		    break;
		case EKeyOperationLogoff:
		    iKeyOperation = EKeyOperationIdle;     
		    iKeyOperationQueue.LogoffCompleted(iStatus.Int());
            break;
		case EKeyOperationLogonForChangePassword:
		    iKeyOperation = EKeyOperationIdle;     
		    if (iStatus.Int() == KErrNone)
    		    {
    		    iKeyOperation = EKeyOperationChangingPassword;  
    		    iLogonService->ChangePassword(iStatus);
    		    SetActive();
    		    }
		    else
		        {
		        iKeyOperationQueue.PasswordChangeCompleted(iStatus.Int());
		        }
		    break;
		case EKeyOperationChangingPassword: 
		    iKeyOperation = EKeyOperationIdle;     
		    iKeyOperationQueue.PasswordChangeCompleted(iStatus.Int());
            break;
		case EKeyOperationRemoveKeyPair:
		    iKeyOperation = EKeyOperationIdle;     
		    iKeyOperationQueue.KeyPairRemoveCompleted(iStatus.Int());
		    break;
        case EKeyOperationGeneratingKeyPair:
            {                
            iKeyOperation = EKeyOperationIdle;
            TPKIKeyIdentifier keyId = iKeyId;
            iKeyId.Zero();
            iKeyOperationQueue.KeyGenerationCompleted(iStatus.Int(), keyId);        		                
            }
            break;
        case EKeyOperationImportingKeyPair:
            {
            delete iInputData;
            iInputData = NULL;
            iKeyOperation = EKeyOperationIdle;
            TPKIKeyIdentifier keyId = iKeyId;
            iKeyId.Zero();
            iKeyOperationQueue.StoreKeyPairCompleted(iStatus.Int(), keyId);        		                            
            }
            break;
		default:
			PKISERVICE_INVARIANT();
		}
	}


void CKeyPairOperationProvider::DoCancel()
	{		
	switch(iKeyOperation)
		{
		case EKeyOperationInitialize:
			PKISERVICE_ASSERT(!iIsInitialized);
			iUnifiedKeyStore->CancelInitialize();
			break;
        case EKeyOperationSetPassPhraseTimeout:
            iUnifiedKeyStore->CancelSetPassphraseTimeout();
            break;
		case EKeyOperationGetKeyList:
			PKISERVICE_ASSERT(iIsInitialized);
			iUnifiedKeyStore->CancelList();
			break;
		case EKeyOperationDecrypting:		    
		    //Deletes and cancels the decryptor also
            CleanupCryptoOperation();            		    
            break;		        
        case EKeyOperationSigning:
            //Deletes and cancels the signer also.
            CleanupCryptoOperation();
            break;
		case EKeyOperationLogon: //falls through
		case EKeyOperationLogoff: //falls through
		case EKeyOperationLogonForChangePassword: //falls through
		case EKeyOperationChangingPassword: 
		    iLogonService->Cancel();
		    break;
		case EKeyOperationRemoveKeyPair: //falls through
		case EKeyOperationGeneratingKeyPair://falls through
        case EKeyOperationImportingKeyPair://falls through
        case EKeyOperationPublicKeyExport:        
		    iKeyManager->Cancel();
		    break;
		default:
			PKISERVICE_INVARIANT();
		}
    iKeyOperation = EKeyOperationIdle;		
	}


TInt CKeyPairOperationProvider::RunError(TInt aError)
	{
	//This is not be called, because the RunL doesn't
	//leave.
	PKISERVICE_INVARIANT();
	return aError;
	}


CArrayFixFlat<TKeyListEntry>* CKeyPairOperationProvider::MakeKeyEntryListL(RMPointerArray<CCTKeyInfo> aKeysList,
                                                                           TInt aUsedKeyStore) const
    {
    CArrayFixFlat<TKeyListEntry>* list = new (ELeave) CArrayFixFlat<TKeyListEntry>(2);
    CleanupStack::PushL(list);
        
    for (TInt i = 0; i < aKeysList.Count(); ++i)
        {
        const CCTKeyInfo* info = aKeysList[i];
        if (aUsedKeyStore == 0 ||
            aUsedKeyStore == aKeysList[i]->Token().TokenType().Type().iUid)
            {
            TKeyListEntry keyInfo;	        
            keyInfo.iObjectName = info->Label();
            keyInfo.iSubjectKeyId = info->ID();
            keyInfo.iKeySize = info->Size();                  // Key size                            
            switch(info->Algorithm())
                {
                case CCTKeyInfo::ERSA:
                	keyInfo.iKeyAlgorithm = EPKIRSA;
                	break;
                case CCTKeyInfo::EDSA:
                	keyInfo.iKeyAlgorithm = EPKIDSA;
                	break;
                case CCTKeyInfo::EDH:
                	keyInfo.iKeyAlgorithm = EPKIDH;
                	break;
                default:	         
                	keyInfo.iKeyAlgorithm = EPKIInvalidAlgorithm;
                	break;
                }            
            list->AppendL(keyInfo);
            }        
        }    
    CleanupStack::Pop(list);    
    return list;
    }



void CKeyPairOperationProvider::CleanupCryptoOperation()
    {                
    delete iInputData;
    iInputData = NULL;
    
    delete iOutputData;
    iOutputData = NULL;
    iKeysList.Close();            		        
    }


