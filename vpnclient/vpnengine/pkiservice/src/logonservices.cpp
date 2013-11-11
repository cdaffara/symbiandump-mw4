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
* Description: A class that provides logon and logoff functionality.
*
*/


#include <unifiedkeystore.h>
#include <mctauthobject.h>

#include "logonservices.h"
#include "pkidefs.h"
#include "pkiserviceassert.h"
#include "pkiserviceconstants.h"

CLogonServices* CLogonServices::NewL(CUnifiedKeyStore& aUnifiedKeyStore)
    {
    CLogonServices* self = new (ELeave) CLogonServices(aUnifiedKeyStore);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    return self;
    }


CLogonServices::CLogonServices(CUnifiedKeyStore& aUnifiedKeyStore)
:CActive(EPriorityStandard), iUnifiedKeyStore(aUnifiedKeyStore)
    {
    CActiveScheduler::Add(this);
    }


void CLogonServices::ConstructL()
    {
    }


CLogonServices::~CLogonServices()
    {
    Cancel();
    }


void CLogonServices::Logon(TRequestStatus& aClientStatus)
    {
    PKISERVICE_ASSERT(iState == ELogonServiceIdle);    
    iState = ELogonServiceListingKeys;
    
    iClientStatus = &aClientStatus;
    *iClientStatus = KRequestPending;
    
    if (iAuthenticationObject == NULL)
        {        
        //First find the User key store
        TInt keyStoreCount = iUnifiedKeyStore.KeyStoreManagerCount(); 
        TInt i = 0;
        for (i = 0; i < keyStoreCount; ++i)
            {
            MCTKeyStoreManager& keyStore = iUnifiedKeyStore.KeyStoreManager(i);        
            if (keyStore.Token().TokenType().Type().iUid == STORETYPE_USER_KEY_ID)        
                {
                iUserKeyStore = &keyStore;
                TCTKeyAttributeFilter filter;
                iUserKeyStore->List(iKeysList, filter, iStatus);
                SetActive();
                break;
                }
            }

        if (i == keyStoreCount)
            {
            iStatus = KRequestPending;
            SetActive();
            
            TRequestStatus* ownStatus = &iStatus;
            User::RequestComplete(ownStatus, KPKIErrNotSupported);
            }               
        }
    else
        {
        //Logon already done
        iState = ELogonServiceAlreadyLoggedIn;
        
        iStatus = KRequestPending;
        SetActive();
        
        TRequestStatus* ownStatus = &iStatus;
        User::RequestComplete(ownStatus, KErrNone);        
        }
    }


void CLogonServices::Logoff(TRequestStatus& aClientStatus)
    {
    PKISERVICE_ASSERT(iState == ELogonServiceIdle);    
    iState = ELogonServiceClosingAuthentication;
    
    iClientStatus = &aClientStatus;
    *iClientStatus = KRequestPending;
    
    if (iAuthenticationObject != NULL)
        {
        iAuthenticationObject->Close(iStatus);
        SetActive();
        }
    else    
        {
        iStatus = KRequestPending;
        SetActive();
        
        TRequestStatus* ownStatus = &iStatus;
        User::RequestComplete(ownStatus, KErrNone);
        }    
    }
    

void CLogonServices::SetAuthenticationObject(MCTAuthenticationObject* aAuthenticationObject)
    {
    if (iAuthenticationObject == NULL)
        {
        iAuthenticationObject = aAuthenticationObject;
        }
    }


void CLogonServices::ChangePassword(TRequestStatus& aClientStatus)
    {
    PKISERVICE_ASSERT(iState == ELogonServiceIdle);    
    PKISERVICE_ASSERT(LogonCompleted()); 
    
    iState = ELogonServiceChangingPassword;
    iClientStatus = &aClientStatus;
    *iClientStatus = KRequestPending;
    
    iAuthenticationObject->ChangeReferenceData(iStatus);
    SetActive();           
    }
    

void CLogonServices::RunL()
    {
    if (iStatus.Int() == KErrNone)
        {
        switch(iState)
            {
            case ELogonServiceAlreadyLoggedIn:
                iState = ELogonServiceIdle;
                User::RequestComplete(iClientStatus, KErrNone);                
                break;
            case ELogonServiceListingKeys:
                iState = ELogonServiceIdle;
                if (iKeysList.Count() > 0)
                    {
                    iState = ELogonServiceOpeningAuthentication;  
                    
                    CCTKeyInfo* keyInfo = iKeysList[0];
                    iAuthenticationObject = keyInfo->Protector();
                    iAuthenticationObject->Open(iStatus);
                    SetActive();                    
                    }
                else
                    {
                    Cleanup();
                    User::RequestComplete(iClientStatus, KPKIErrKeyStoreEmpty);                
                    }
                break;
            case ELogonServiceOpeningAuthentication:
                iState = ELogonServiceIdle;                
                Cleanup();
                User::RequestComplete(iClientStatus, KErrNone);                                
                break;
            case ELogonServiceClosingAuthentication:                                
                iState = ELogonServiceIdle; 
                iAuthenticationObject = NULL;
                User::RequestComplete(iClientStatus, KErrNone);                
                break;
            case ELogonServiceChangingPassword:
                iState = ELogonServiceIdle; 
                User::RequestComplete(iClientStatus, KErrNone);
                break;                
            default:
                PKISERVICE_INVARIANT();
                break;                
            }
        
        }
    else
        {
        Cleanup();
        iState = ELogonServiceIdle;
        User::RequestComplete(iClientStatus, iStatus.Int());
        }
    }


void CLogonServices::DoCancel()
    {
    switch(iState)
        {
        case ELogonServiceListingKeys:
            iUserKeyStore->CancelList();
            break;
        case ELogonServiceOpeningAuthentication:
            iAuthenticationObject->CancelOpen();
            iAuthenticationObject = NULL;
            break;
        case ELogonServiceClosingAuthentication:            
            if (iAuthenticationObject != NULL)
                {
                iAuthenticationObject->CancelClose();
                iAuthenticationObject = NULL;
                }
            //else do nothing                
            break;
        case ELogonServiceChangingPassword:
            iAuthenticationObject->CancelChangeReferenceData();
            break;
        default:
            PKISERVICE_INVARIANT();
            break;                
        }
        
    Cleanup();
    iState = ELogonServiceIdle;          
    User::RequestComplete(iClientStatus, KErrCancel);     
    }


void CLogonServices::Cleanup()
    {   
    iKeysList.Close();
    }
    
    
TBool CLogonServices::LogonCompleted() const
    {
    if (iState == ELogonServiceIdle && 
        iAuthenticationObject != NULL)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
