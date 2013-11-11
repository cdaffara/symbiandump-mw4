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



#ifndef C_LOGONSERVICES_H
#define C_LOGONSERVICES_H

#include <e32base.h>

class CUnifiedKeyStore;

class CLogonServices : public CActive
    {
public:
    static CLogonServices* NewL(CUnifiedKeyStore& aUnifiedKeyStore);
    ~CLogonServices();


    void Logon(TRequestStatus& aClientStatus);
    void Logoff(TRequestStatus& aClientStatus);        
    void ChangePassword(TRequestStatus& aClientStatus);

    //Used by explicit login
    void SetAuthenticationObject(MCTAuthenticationObject* aAuthenticationObject);

    TBool LogonCompleted() const;

    void RunL();
    void DoCancel();

private:
    enum TLogonServiceState
        {
        ELogonServiceIdle = 0,
        ELogonServiceAlreadyLoggedIn,
        ELogonServiceListingKeys,
        ELogonServiceOpeningAuthentication,
        ELogonServiceClosingAuthentication,
        ELogonServiceChangingPassword
        };

    CLogonServices(CUnifiedKeyStore& aUnifiedKeyStore);
    void ConstructL();

    void Cleanup();

    CUnifiedKeyStore& iUnifiedKeyStore;
    TRequestStatus* iClientStatus;
    TLogonServiceState iState;

    MCTKeyStoreManager* iUserKeyStore; //Not owned by this class    
    RMPointerArray<CCTKeyInfo> iKeysList;
    MCTAuthenticationObject* iAuthenticationObject;
    };

#endif //C_LOGONSERVICES_H

