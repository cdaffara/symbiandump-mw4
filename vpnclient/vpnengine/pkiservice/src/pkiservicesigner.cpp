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
* Description: A class that provides a signing operation.
*
*/



#include <unifiedkeystore.h>
#include <asymmetric.h>

#include "pkiservicesigner.h"
#include "logonservices.h"
#include "pkidefs.h"
#include "pkiserviceconstants.h"
#include "pkiserviceassert.h"

CPkiServiceSigner* CPkiServiceSigner::NewL(CLogonServices& aLogonServices)
    {
    CPkiServiceSigner* self = new (ELeave) CPkiServiceSigner(aLogonServices);
    return self;
    }


CPkiServiceSigner::CPkiServiceSigner(CLogonServices& aLogonServices)
:CActive(EPriorityStandard), iLogonServices(aLogonServices)
    {
    CActiveScheduler::Add(this);
    }


CPkiServiceSigner::~CPkiServiceSigner()
    {
    Cancel();   
    }


void CPkiServiceSigner::Sign(const TPKIKeyIdentifier& aKeyId,
                             const TDesC8& aDataToBeSigned,
                             HBufC8*& aSignature,
                             CUnifiedKeyStore& aUnifiedKeyStore,
                             TInt aUsedKeyStore,
                             TRequestStatus& aStatus)
    {
    PKISERVICE_ASSERT(iState == ESignerIdle);
    PKISERVICE_ASSERT(iClientStatus == NULL);
    PKISERVICE_ASSERT(iKeyStore == NULL);
    PKISERVICE_ASSERT(iOutputBuffer == NULL);
    PKISERVICE_ASSERT(iRsaSigner == NULL);
    PKISERVICE_ASSERT(iDsaSigner == NULL);
    PKISERVICE_ASSERT(iRsaSignature == NULL);
    PKISERVICE_ASSERT(iDsaSignature == NULL);
    PKISERVICE_ASSERT(iKeysList.Count() == 0);
    
    
    iState = ESignerGettingKey;
            
    
    iInput.Set(aDataToBeSigned);
    
    iOutputBuffer = &aSignature;
    iKeyStore = &aUnifiedKeyStore;
    iUsedKeyStore = aUsedKeyStore;
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    
        
    TCTKeyAttributeFilter filter;
    filter.iKeyId = aKeyId;
    
    iKeyStore->List(iKeysList, filter, iStatus);
    SetActive();    
    }


void CPkiServiceSigner::DoCancel()
    {
    switch(iState)
        {
        case ESignerGettingKey:
            iKeyStore->CancelList();
            break;
        case ESignerOpeningSigner:
            iKeyStore->CancelOpen();
            break;
        case ESignerSigning:
            if (iRsaSigner != NULL)
                {         
                PKISERVICE_ASSERT(iDsaSigner == NULL);        
                iRsaSigner->CancelSign();          
                }
            else
                {
                PKISERVICE_ASSERT(iDsaSigner != NULL);
                PKISERVICE_ASSERT(iRsaSigner == NULL );
                iDsaSigner->CancelSign();
                }
            Cleanup();       
            break;
        default:
            PKISERVICE_INVARIANT();
            break;        
        }
    iState = ESignerIdle;
    Cleanup();
    User::RequestComplete(iClientStatus, KErrCancel);
    }


void CPkiServiceSigner::RunL()
    {
    if (iStatus.Int() == KErrNone)
        {        
        switch(iState)
            {
            case ESignerGettingKey:
                iState = ESignerIdle;
                
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
                    iUsedKeyInfo = iKeysList[keyIndex];
                    TCTTokenObjectHandle tokenHandle = iUsedKeyInfo->Handle();                                            
                    switch(iUsedKeyInfo->Algorithm())
                        {
                        case CKeyInfoBase::ERSA:
                            iState = ESignerOpeningSigner;
                            iKeyStore->Open(tokenHandle, iRsaSigner, iStatus);
                            SetActive();
                            break;
                        case CKeyInfoBase::EDSA:            
                            iState = ESignerOpeningSigner;  
                            iKeyStore->Open(tokenHandle, iDsaSigner, iStatus);
                            SetActive();                            
                            break;
                        default:
                            Cleanup();                         
                            User::RequestComplete(iClientStatus, KPKIErrNotSupported);
                            break;                            
                        }                    
                    }
                else
                    {
                    Cleanup();
                    User::RequestComplete(iClientStatus, KPKIErrNotFound);                    
                    }                
            
                break;            
            case ESignerOpeningSigner:
                iState = ESignerSigning;            
                if (iRsaSigner != NULL)
                    {         
                    PKISERVICE_ASSERT(iDsaSigner == NULL);        
                    iRsaSigner->Sign(iInput, iRsaSignature, iStatus);
                    SetActive();                
                    }
                else
                    {
                    PKISERVICE_ASSERT(iDsaSigner != NULL);                    
                    iDsaSigner->Sign(iInput, iDsaSignature, iStatus);
                    SetActive();   
                    }
                break;
            case ESignerSigning:
                iState = ESignerIdle;
                                                                
                if (iUsedKeyInfo->Protector() != NULL)
                    {
                    //authObject is NULL for device store
                    iLogonServices.SetAuthenticationObject(iUsedKeyInfo->Protector());
                    }

                
                TRAPD(err,                                 
                    if (iRsaSignature != NULL)
                        {
                        PKISERVICE_ASSERT(iDsaSignature == NULL);
                        
                        const TInteger& integer = iRsaSignature->S();                    
                        *iOutputBuffer = integer.BufferLC();
                        CleanupStack::Pop(*iOutputBuffer);  
                        }
                    else
                        {
                        PKISERVICE_ASSERT(iDsaSignature != NULL);
                        
                        const TInteger& integerR = iDsaSignature->R();
                        HBufC8* rBuf = integerR.BufferLC();                        
                                                                        
                        const TInteger& integerS = iDsaSignature->S();
                        HBufC8* sBuf = integerS.BufferLC();
                        
                        (*iOutputBuffer) = HBufC8::NewL(rBuf->Length() + sBuf->Length());
                        TPtr8 outputBufferPtr = (*iOutputBuffer)->Des();
                        outputBufferPtr.Append(*rBuf);
                        outputBufferPtr.Append(*sBuf);
                        
                        CleanupStack::PopAndDestroy(sBuf);
                        CleanupStack::PopAndDestroy(rBuf);                        
                        }                    
                    );
                Cleanup();                  
                User::RequestComplete(iClientStatus, err);
                break; 
            default:
                PKISERVICE_INVARIANT();
                break;                       
            }    
        }
    else
        {
        iState = ESignerIdle;
        Cleanup();
        User::RequestComplete(iClientStatus, iStatus.Int());
        }
    }


void CPkiServiceSigner::Cleanup()
    {
    iUsedKeyInfo = NULL;
    
    delete iRsaSignature;
    iRsaSignature = NULL;
    
    delete iDsaSignature;    
    iDsaSignature = NULL;

    
    if (iRsaSigner != NULL)
        {
        iRsaSigner->Release();
        iRsaSigner = NULL;
        }
    
    if (iDsaSigner != NULL)
        {
        iDsaSigner->Release();
        iDsaSigner = NULL;
        }    
        
    iKeysList.Close();  
    
    iOutputBuffer = NULL;
    iKeyStore = NULL;  
            		                    
    }
