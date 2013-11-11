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
* Description: 
* Handles the calling of PKI Service Server module’s password change function.
*
*/



#ifndef __PWDCHANGER_H__
#define __PWDCHANGER_H__

#include <e32std.h>
#include <e32base.h>

class CX509Certificate;
#include "pkiserviceapi.h"

class CVpnApiServant;

/**
 * Handles the calling of the PKI Services 
 * Server module's change password call.
 */
class CPwdChanger : public CActive
    {
public:
    static CPwdChanger* NewL(const RMessage2& aMessage, CVpnApiServant& aVpnApiServant);
    ~CPwdChanger();
    
    void ChangePassword();
    
private:
    CPwdChanger(const RMessage2& aMessage, CVpnApiServant& aVpnApiServant);
    void ConstructL();

protected: // From CActive
    void DoCancel();
    void RunL();
    
private:
    RMessage2 iMessage;
    CVpnApiServant& iVpnApiServant;
    RPKIServiceAPI iPkiService;
    };

#endif // __PWDCHANGER_H__
