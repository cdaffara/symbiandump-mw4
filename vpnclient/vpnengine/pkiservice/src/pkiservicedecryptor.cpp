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



#include "pkiservicedecryptor.h"
#include "logonservices.h"
#include "pkiserviceconstants.h"
#include "pkiserviceassert.h"

CPkiServiceDecryptor* CPkiServiceDecryptor::NewL(CLogonServices& aLogonServices)
    {
    CPkiServiceDecryptor* self = new (ELeave)CPkiServiceDecryptor(aLogonServices);
    return self;
    }


CPkiServiceDecryptor::CPkiServiceDecryptor(CLogonServices& aLogonServices)
:CActive(EPriorityStandard), iLogonServices(aLogonServices)
    {
    CActiveScheduler::Add(this);
    }


CPkiServiceDecryptor::~CPkiServiceDecryptor()
    {
    Cancel();
    }


void CPkiServiceDecryptor::Decrypt(const TPKIKeyIdentifier& aKeyId,
                                   const TDesC8& aEncryptedData,
                                   TPtr8& aPlainTextData,
                                   CUnifiedKeyStore& aUnifiedKeyStore,
                                   TInt aUsedKeyStore, 
                                   TRequestStatus& aClientStatus)
    {
    PKISERVICE_ASSERT(iKeyStore == NULL);
    PKISERVICE_ASSERT(iInput == NULL);    
    PKISERVICE_ASSERT(iDecryptor == NULL);
    PKISERVICE_ASSERT(iKeysList.Count() == 0);
    PKISERVICE_ASSERT(iState == EDecryptorIdle);
    
    iState = EDecryptorGettingKey;
    
    iInput = &aEncryptedData;
    iPlainText = &aPlainTextData;
    iKeyStore = &aUnifiedKeyStore;      
    iUsedKeyStore = aUsedKeyStore;
    aClientStatus = KRequestPending;
    iClientStatus = &aClientStatus;
    
    TCTKeyAttributeFilter filter;
    filter.iKeyId = aKeyId;
    
    iKeyStore->List(iKeysList, filter, iStatus);
    SetActive();        
    }


void CPkiServiceDecryptor::RunL()
    {
    if (iStatus == KErrNone)
        {
        switch(iState)
            {
            case EDecryptorGettingKey:                
                iState = EDecryptorIdle;
                TInt keyIndex;
                for (keyIndex = 0; keyIndex < iKeysList.Count(); keyIndex++)
                    {
                    if (iUsedKeyStore == STORETYPE_ANY_KEY_ID ||
                        iUsedKeyStore == iKeysList[keyIndex]->Token().TokenType().Type().iUid)
                        {
                        break;
                        }
                    }
                
                if (keyIndex < iKeysList.Count())

                    {
                    iKeyInfo = iKeysList[keyIndex];
                    TCTTokenObjectHandle tokeHandle = iKeyInfo->Handle();
                    
                    iState = EDecryptorOpeningDecryptor;
                    iKeyStore->Open(tokeHandle, iDecryptor, iStatus);
                    SetActive();
                    }
                else
                    {
                    Cleanup();
                    User::RequestComplete(iClientStatus, KPKIErrNotFound);                
                    }

                break;
            case EDecryptorOpeningDecryptor:
                iState = EDecryptorDecrypting;                                
                iDecryptor->Decrypt(*iInput, *iPlainText, iStatus);
                SetActive();                
                break;
            case EDecryptorDecrypting:                            
                iState = EDecryptorIdle;
                if (iKeyInfo->Protector() != NULL)
                    {
                    //authObject is NULL for device store
                    iLogonServices.SetAuthenticationObject(iKeyInfo->Protector());
                    }
                
                Cleanup();  
                User::RequestComplete(iClientStatus, KErrNone);
                break;
            default:
                PKISERVICE_INVARIANT();
                break;            
            }        
        }
    else
        {
        iState = EDecryptorIdle;        
        Cleanup();
        User::RequestComplete(iClientStatus, iStatus.Int());
        }
    }

    
void CPkiServiceDecryptor::DoCancel()
    {
    switch(iState)
        {
        case EDecryptorGettingKey:
            iKeyStore->CancelList();
            break;
        case EDecryptorOpeningDecryptor:
            iKeyStore->CancelOpen();
            break;
        case EDecryptorDecrypting:
            iDecryptor->CancelDecrypt();
            break;
        default:
            PKISERVICE_INVARIANT();
            break;            
        }
    Cleanup();        
    iState = EDecryptorIdle;    
    User::RequestComplete(iClientStatus, KErrCancel);
    }


void CPkiServiceDecryptor::Cleanup()
    {
    if (iDecryptor != NULL)
        {        
        iDecryptor->Release();
        iDecryptor = NULL;
        }
    
    iKeyInfo = NULL;
    iKeysList.Close();    
    iInput = NULL;
    iKeyStore = NULL;
    }

