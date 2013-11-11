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

#ifndef SESSION_H
#define SESSION_H

#include <e32base.h>
#include <apibehavior.h>

class CStubSrv;
class CApiExecutor;

/**
*  CStubSrvSess class represent session on server side
*
*/
NONSHARABLE_CLASS(CStubSrvSess) : public CSession2
    {
public:

    static CStubSrvSess* NewL(CStubSrv& aServer);

    ~CStubSrvSess();

    void InvokeApiComplete();

    TInt ConsumeApiBehavior(const TApiBehavior& aBeh, TInt aExeDuration);
    
private:

    void ServiceL(const RMessage2& aMessage);

    void DispatchMessageL(const RMessage2& aMessage);


private:

    CStubSrvSess(CStubSrv& aServer);

    void ConstructL();

private:
    CStubSrv& iServer;
    
    CApiExecutor* iExecutor;       
    };

#endif      // SESSION_H

// End of File
