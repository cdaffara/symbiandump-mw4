/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef C_STUBFAPISPEC_H
#define C_STUBFAPISPEC_H

#include <e32base.h>
#include <apibehavior.h>
#include "stubsrvconst.h"

class CStubSrvSess;

class CApiExecutor : public CActive
    {
    
public:
    
    static CApiExecutor* NewL();

    virtual ~CApiExecutor();

    const TApiBehavior& ApiBehavior() const;
    
    TApiInvokeMode Mode() const;

    void ExecuteL(const RMessage2& aMessage, CStubSrvSess& aSession, TUint aLib, TUint aApi);
    
    void ExecuteL(const RMessage2& aMessage, CStubSrvSess& aSession, const TApiBehavior& aBeh, TInt aExeDuration);

    void Resume(const TApiBehavior& aBeh, TInt aExeDuration);

private:
    
    void RunL();
    
    TInt RunError(TInt aReason);
    
    void DoCancel();
    
private:

    CApiExecutor();

    void ConstructL();

private:
    
    CStubSrvSess* iSess; // The associated client who started this API.
    TApiBehavior iBeh;
    
    TApiInvokeMode iMode;
    
    TInt iTimeout;
    
    RTimer iTimer;
    
    RMessage2 iMessage;
    };

#endif      // C_STUBFAPISPEC_H

// End of File
