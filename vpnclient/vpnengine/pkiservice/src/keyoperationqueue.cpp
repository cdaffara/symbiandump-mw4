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



#include "keyoperationqueue.h"
#include "pkiserviceclientservercommon.h"
#include "keyoperationprovider.h"
#include "PKIMapper.h"
#include "pkisession.h"
#include "pkiserviceassert.h"

#include "log_r6.h"

CKeyOperationQueue::TKeyOperation::TKeyOperation(CPKISession& aOwner, 
                                                 const RMessage2& aMessage,
                                                 TInt aUsedKeyStore,
                                                 TPkiServiceStoreType aUsedCertStore)
:iOwner(aOwner), iMessage(aMessage), iUsedKeyStore(aUsedKeyStore), iUsedCertStore(aUsedCertStore)
    {
    }



CKeyOperationQueue* CKeyOperationQueue::NewL(CPKIMapper& aMapper)
    {
    CKeyOperationQueue* self = new (ELeave) CKeyOperationQueue(aMapper);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    return self;
    }


CKeyOperationQueue::CKeyOperationQueue(CPKIMapper& aMapper)
:iMapper(aMapper)
    {
    }


void CKeyOperationQueue::ConstructL()
    {
    iKeyOperationProvider = CKeyPairOperationProvider::NewL(*this);
    }

CKeyOperationQueue::~CKeyOperationQueue()
    {
    PKISERVICE_ASSERT(iOperationQue.Count() == 0);
    PKISERVICE_ASSERT(iCurrentOperation == NULL);

    delete iKeyOperationProvider;    
    iOperationQue.Close();
    }



void CKeyOperationQueue::AddOperationL(CPKISession& aOwner, 
                                     const RMessage2& aMessage,
                                     TInt aUsedKeyStore,
                                     TPkiServiceStoreType aUsedCertStore)
    {
    TKeyOperation* operation = new (ELeave) TKeyOperation(aOwner, aMessage, 
                                                          aUsedKeyStore, aUsedCertStore);
    CleanupDeletePushL(operation);
    User::LeaveIfError(iOperationQue.Append(operation));
    CleanupStack::Pop(operation);

    if (iCurrentOperation == NULL)
        {
        PKISERVICE_ASSERT(iOperationQue.Count() == 1);
        //No operations in progress.
        //Handle the new one.
        StartNextOperation();
        }
    }




void CKeyOperationQueue::StartNextOperation()
    {
    PKISERVICE_ASSERT(iOperationQue.Count() > 0);
    PKISERVICE_ASSERT(iCurrentOperation == NULL);

    iCurrentOperation = iOperationQue[0];
    iOperationQue.Remove(0);
    
    TRAPD(err, ServiceL(*iCurrentOperation));
    if (err != KErrNone)
        {
        iCurrentOperation->iMessage.Complete(err);
        iCurrentOperation = NULL;
        
        if (iOperationQue.Count() > 0)
            {
            StartNextOperation();
            }
        }
    }


void CKeyOperationQueue::ServiceL(TKeyOperation& aOperation)
    {
    switch(aOperation.iMessage.Function())
        {        
        case PkiService::EInitialize:
            iKeyOperationProvider->Initialize();
            break;                                                           
        case PkiService::EKeyCount:    
            {                
            TPKIKeyIdentifier keyId;
            keyId.Zero();
                                            	
        	iKeyOperationProvider->GetKeyPairList(keyId, aOperation.iUsedKeyStore);
            }
            break;                        
    		
        case PkiService::EGetKeyDetails:
    		{            
    		TPKIKeyIdentifier keyId;
    		aOperation.iMessage.ReadL(1, keyId);
        
        	iKeyOperationProvider->GetKeyPairList(keyId, aOperation.iUsedKeyStore);    		    		    		    		    		
    		}
    		break;    		
        case PkiService::EDecrypt:
            {                        
            TPKIKeyIdentifier keyId;
            aOperation.iMessage.ReadL(0, keyId);
            
            if (keyId.Length() > 0)
                {                                
                HBufC8* encryptedData = HBufC8::NewLC(aOperation.iMessage.GetDesLength(1));
                TPtr8 encryptedDataPtr = encryptedData->Des();
                aOperation.iMessage.ReadL(1, encryptedDataPtr);
                
                CleanupStack::Pop(encryptedData);
                //Transfer the ownership of the encryptedData
                
                TInt outputBufferMaxLength = aOperation.iMessage.GetDesMaxLength(2);
                User::LeaveIfError(outputBufferMaxLength);
                iKeyOperationProvider->Decrypt(keyId, aOperation.iUsedKeyStore, encryptedData,
                                               outputBufferMaxLength);                
                }
            else
                {
                CompleteCurrentOperation(KPKIErrNotFound);
                }
            }
            break;
        case PkiService::ESignWithKeyId:
            {         
            TPKIKeyIdentifier keyId;
            aOperation.iMessage.ReadL(0, keyId);
            
            if (keyId.Length() > 0)
                {
                HBufC8* dataIn = HBufC8::NewLC(aOperation.iMessage.GetDesLength(1));
                TPtr8 dataInPtr = dataIn->Des();
                aOperation.iMessage.ReadL(1, dataInPtr);                
                
                //transfers the dataIn buffer ownership  
                CleanupStack::Pop(dataIn);              
                iKeyOperationProvider->Sign(keyId, aOperation.iUsedKeyStore, dataIn);                
                }
            else
                {
                CompleteCurrentOperation(KPKIErrNotFound);
                }          
            }
            break;
        case PkiService::ESignWithCert:
            {
            TSecurityObjectDescriptor *securityObjectDesc = new (ELeave) TSecurityObjectDescriptor;
            CleanupDeletePushL(securityObjectDesc);
            
            TPckg<TSecurityObjectDescriptor> securityObjectDescPacketBuffer(*securityObjectDesc);            
            aOperation.iMessage.ReadL(0, securityObjectDescPacketBuffer);
            
            TPKIKeyIdentifier keyId;                        
            iMapper.GetCertificateKeyIdL(*securityObjectDesc, keyId,
                                         aOperation.iUsedCertStore);
            
            CleanupStack::PopAndDestroy(securityObjectDesc);            
            
           if (keyId.Length() > 0)
                {
                HBufC8* dataIn = HBufC8::NewLC(aOperation.iMessage.GetDesLength(1));
                TPtr8 dataInPtr = dataIn->Des();
                aOperation.iMessage.ReadL(1, dataInPtr);                
                
                //transfers the dataIn buffer ownership
                CleanupStack::Pop(dataIn);
                iKeyOperationProvider->Sign(keyId, aOperation.iUsedKeyStore, dataIn);
                
                }
            else
                {
                CompleteCurrentOperation(KPKIErrNotFound);
                }            
            }
            break;            
        case PkiService::EReadPublicKey:
            {
            TPKIKeyIdentifier keyId;
            aOperation.iMessage.ReadL(0, keyId);
            
            if (keyId.Length() > 0)
                {                
                iKeyOperationProvider->GetPublicKey(keyId, aOperation.iUsedKeyStore);                
                }
            else
                {
                CompleteCurrentOperation(KPKIErrNotFound);
                }            
            }            
            break;
        case PkiService::ELogon:
            iKeyOperationProvider->Logon();
            break;
        case PkiService::ELogoff:
            iKeyOperationProvider->Logoff();
            break;         
        case PkiService::EChangePassword:
            iKeyOperationProvider->ChangePassword();
            break;
        case PkiService::ERemoveKeypair:
            {                
            TPKIKeyIdentifier keyId;
            aOperation.iMessage.ReadL(0, keyId);
            
            if (keyId.Length() > 0)
                {                
                iKeyOperationProvider->RemoveKeyPair(keyId, aOperation.iUsedKeyStore);                
                }
            else
                {
                CompleteCurrentOperation(KPKIErrNotFound);
                }                        
            }
            break;
        case PkiService::EGenerateKeypair:
            {
            TUint keySize = aOperation.iMessage.Int1();
            TPKIKeyAlgorithm keyAlgorithm = static_cast<TPKIKeyAlgorithm>(aOperation.iMessage.Int2());
            iKeyOperationProvider->GenerateKeyPair(keySize, keyAlgorithm, aOperation.iUsedKeyStore);   
            }            
            break;                   
        case PkiService::EStoreKeypair:
            {
            HBufC8* keyData = HBufC8::NewLC(aOperation.iMessage.GetDesLength(1));
            TPtr8 keyDataPtr = keyData->Des();
            aOperation.iMessage.ReadL(1, keyDataPtr);                        
            
            CleanupStack::Pop(keyData);
            iKeyOperationProvider->ImportKeyPair(keyData, aOperation.iUsedKeyStore);
            }
            break;  
        default:      
            PKISERVICE_INVARIANT();
            break;
        }
    
    }


void CKeyOperationQueue::CompleteCurrentOperation(TInt aStatus)
    {
    TKeyOperation* operation = iCurrentOperation;
    iCurrentOperation = NULL;
    
    operation->iMessage.Complete(aStatus);
    delete operation;
    
    if (iOperationQue.Count() > 0)
        {
        StartNextOperation();
        }
    }
    

void CKeyOperationQueue::KeyStoreInitComplete(TInt aStatus)
	{			  
    PKISERVICE_ASSERT(iCurrentOperation != NULL);
    PKISERVICE_ASSERT(iCurrentOperation->iMessage.Function() == PkiService::EInitialize);
		
	if (aStatus == KErrNone)
		{
		TRAPD(err, iCurrentOperation->iOwner.InitializeWrapperL(iCurrentOperation->iMessage));
		if (err != KErrNone && iCurrentOperation->iMessage.Handle() != NULL)
			{
			CompleteCurrentOperation(err);
			}
        else
            {
            delete iCurrentOperation;
            iCurrentOperation = NULL;
            if (iOperationQue.Count() > 0)
                {
                StartNextOperation();
                }            
            }            
		}
	else
		{
		CompleteCurrentOperation(aStatus);
		}
	}


void CKeyOperationQueue::KeyPairListComplete(TInt aStatus, CArrayFixFlat<TKeyListEntry>* aKeyList)
	{	
	
	PKISERVICE_ASSERT((aStatus == KErrNone && aKeyList != NULL) ||
	               (aStatus != KErrNone && aKeyList == NULL));	
	
	if (iCurrentOperation->iMessage.Function() == PkiService::EKeyCount)
    	{
    	//If we are getting key count, we save the 
    	//iKeyList, because the next call will be the list
    	//retrieve.    	

    	if (aStatus == KErrNone)
    		{
            iCurrentOperation->iOwner.SetKeyList(aKeyList);    		
    		CompleteCurrentOperation(aKeyList->Count());
    		}
    	else
    		{
    		CompleteCurrentOperation(aStatus);
    		}    
    	}
    else    
        {
        //If we are just retrieving a key details.
        //We can clean up the key list at the end.
        
        PKISERVICE_ASSERT(iCurrentOperation->iMessage.Function() == PkiService::EGetKeyDetails);
    	if (aStatus == KErrNone)
    		{
    		if (aKeyList->Count() > 0)
        		{
        		const TKeyListEntry& entry = (*aKeyList)[0];        		
           		const TPckg<TKeyListEntry> listEntryBuf(entry);	                
                TInt err = iCurrentOperation->iMessage.Write(0, listEntryBuf);

                //We should newer get overflow, 
                //because API knows the length of the result.
                __ASSERT_DEBUG(err != KErrOverflow, User::Invariant());
                                    
                CompleteCurrentOperation(err);
        		}
            else
                {                                  
                CompleteCurrentOperation(KPKIErrNotFound);    
                }
    		}
    	else
    		{
    		CompleteCurrentOperation(aStatus);
    		}    
        delete aKeyList;    		    		
        }        
	}


void CKeyOperationQueue::DecryptComplete(TInt aStatus, HBufC8* aDecryptedData)
    {
	PKISERVICE_ASSERT(iCurrentOperation != NULL);
	PKISERVICE_ASSERT(iCurrentOperation->iMessage.Function() == PkiService::EDecrypt);        
    
    
    PKISERVICE_ASSERT((aStatus == KErrNone && aDecryptedData != NULL) ||
                   (aStatus != KErrNone && aDecryptedData == NULL));                   
    
            
    if (aDecryptedData != NULL)
        {
        aStatus = iCurrentOperation->iMessage.Write(2, *aDecryptedData);
        if (aStatus == KErrOverflow)
            {
            iCurrentOperation->iOwner.SetRequiredBufferSize(aDecryptedData->Length());
            aStatus = KPKIErrBufferTooShort;
            }
        delete aDecryptedData;
        }
    CompleteCurrentOperation(aStatus);
    }


void CKeyOperationQueue::SignComplete(TInt aStatus, HBufC8* aSignedData)
    {
	PKISERVICE_ASSERT(iCurrentOperation != NULL);
	PKISERVICE_ASSERT(iCurrentOperation->iMessage.Function() == PkiService::ESignWithKeyId ||
	                iCurrentOperation->iMessage.Function() == PkiService::ESignWithCert);        
    
    PKISERVICE_ASSERT((aStatus == KErrNone && aSignedData != NULL) ||
                   (aStatus != KErrNone && aSignedData == NULL));                   

            
    if (aSignedData != NULL)
        {
        aStatus = iCurrentOperation->iMessage.Write(2, *aSignedData);
        if (aStatus == KErrOverflow)
            {
            iCurrentOperation->iOwner.SetRequiredBufferSize(aSignedData->Length());
            aStatus = KPKIErrBufferTooShort;
            }
        
        delete aSignedData;
        }
    CompleteCurrentOperation(aStatus);    
    }


void CKeyOperationQueue::GetPublicKeyCompleted(TInt aStatus, HBufC8* aPublicKeyData)
    {
	PKISERVICE_ASSERT(iCurrentOperation != NULL);
	PKISERVICE_ASSERT(iCurrentOperation->iMessage.Function() == PkiService::EReadPublicKey);        
    
    PKISERVICE_ASSERT((aStatus == KErrNone && aPublicKeyData != NULL) ||
                      (aStatus != KErrNone && aPublicKeyData == NULL));                   

    
    if (aPublicKeyData != NULL)
        {
        aStatus = iCurrentOperation->iMessage.Write(1, *aPublicKeyData);  
        if (aStatus == KErrOverflow)
            {
            iCurrentOperation->iOwner.SetRequiredBufferSize(aPublicKeyData->Length());
            aStatus = KPKIErrBufferTooShort;
            }
        delete aPublicKeyData;
        }
    CompleteCurrentOperation(aStatus);        
    }


void CKeyOperationQueue::LogonCompleted(TInt aStatus)
    {
	PKISERVICE_ASSERT(iCurrentOperation != NULL);
	PKISERVICE_ASSERT(iCurrentOperation->iMessage.Function() == PkiService::ELogon);        
	                
    CompleteCurrentOperation(aStatus);
    }


void CKeyOperationQueue::LogoffCompleted(TInt aStatus)
    {
	PKISERVICE_ASSERT(iCurrentOperation != NULL);
	PKISERVICE_ASSERT(iCurrentOperation->iMessage.Function() == PkiService::ELogoff);        

    CompleteCurrentOperation(aStatus);
    }


void CKeyOperationQueue::PasswordChangeCompleted(TInt aStatus)
    {
	PKISERVICE_ASSERT(iCurrentOperation != NULL);
	PKISERVICE_ASSERT(iCurrentOperation->iMessage.Function() == PkiService::EChangePassword);        
    
    CompleteCurrentOperation(aStatus);        
    }
    
    
void CKeyOperationQueue::KeyPairRemoveCompleted(TInt aStatus)
    {
	PKISERVICE_ASSERT(iCurrentOperation != NULL);
	PKISERVICE_ASSERT(iCurrentOperation->iMessage.Function() == PkiService::ERemoveKeypair);        

    CompleteCurrentOperation(aStatus);
    }
    
    
void CKeyOperationQueue::KeyGenerationCompleted(TInt aStatus, TPKIKeyIdentifier& aKeyId)
    {
	PKISERVICE_ASSERT(iCurrentOperation != NULL);
	PKISERVICE_ASSERT(iCurrentOperation->iMessage.Function() == PkiService::EGenerateKeypair);        
    
    if (aStatus == KErrNone)
        {
        aStatus = iCurrentOperation->iMessage.Write(0, aKeyId);
        if (aStatus == KErrOverflow)
            {
            iCurrentOperation->iOwner.SetRequiredBufferSize(aKeyId.Length());
            aStatus = KPKIErrBufferTooShort;
            }

        }
    CompleteCurrentOperation(aStatus);
    }
    
    
void CKeyOperationQueue::StoreKeyPairCompleted(TInt aStatus, TPKIKeyIdentifier& aKeyId)
    {    
	PKISERVICE_ASSERT(iCurrentOperation != NULL);
	PKISERVICE_ASSERT(iCurrentOperation->iMessage.Function() == PkiService::EStoreKeypair);

    if (aStatus == KErrNone)
        {
        aStatus = iCurrentOperation->iMessage.Write(0, aKeyId);
        if (aStatus == KErrOverflow)
            {
            iCurrentOperation->iOwner.SetRequiredBufferSize(aKeyId.Length());
            aStatus = KPKIErrBufferTooShort;
            }        
        }
    CompleteCurrentOperation(aStatus);
    }
    
