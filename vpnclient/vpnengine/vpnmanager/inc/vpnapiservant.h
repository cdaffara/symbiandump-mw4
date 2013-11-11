/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Handles API requests for VPN Manager.
*
*/



#ifndef __VPNAPISERVANT_H__
#define __VPNAPISERVANT_H__

#include <e32base.h>
#include "vpnapi.h"
#include "eventlogger.h"

class RFs;
class CPolicyStore;
class CPwdChanger;
class CPolicyImporter;

/**
 * Services the requests received from clients that use the VPN API. 
 */
class CVpnApiServant : public CBase
    {
public:
    static CVpnApiServant* NewL(RFs& aFs);
    ~CVpnApiServant();

    TBool ServiceL(const RMessage2& aMessage);
    
    void PasswordChangeComplete();
    void PolicyImportComplete();

    CPolicyStore* PolicyStore();
    void ImportSinglePolicyL(const TDesC& aDir, TVpnPolicyId& aNewPolicyId,
                             TRequestStatus& aStatus);
    void CancelImportSinglePolicy();

    DEFINE_EVENT_LOGGER

private:
    CVpnApiServant(RFs& aFs);
    void ConstructL();

    void ImportPolicyL(const RMessage2& aMessage);
    void CancelImportPolicy(const RMessage2& aMessage);
    void EnumeratePoliciesL(const RMessage2& aMessage);
    void GetPolicyInfoL(const RMessage2& aMessage);
    void GetPolicyDetailsL(const RMessage2& aMessage);
    void DeletePolicyL(const RMessage2& aMessage);
    void ChangePasswordL(const RMessage2& aMessage);
    void CancelChangePassword(const RMessage2& aMessage);
    void GetPolicySizeL(const RMessage2& aMessage);
    void GetPolicyDataL(const RMessage2& aMessage);
    TInt PolicySizeL(const TVpnPolicyId& aPolicyId);
    void AddPolicyL(const RMessage2& aMessage);
    void UpdatePolicyDetailsL(const RMessage2& aMessage);
    void UpdatePolicyDataL(const RMessage2& aMessage);

private:
    CPolicyStore* iPolicyStore;
    CPwdChanger* iPwdChanger;
    CPolicyImporter* iPolicyImporter;
    RFs& iFs;
    };

#endif // __VPNAPISERVANT_H__
