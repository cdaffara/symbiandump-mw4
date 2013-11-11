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

#ifndef STUBSRV_H
#define STUBSRV_H

#include <e32svr.h>
#include <e32base.h>
#include <apibehavior.h>
#include "stubclientsrv.h"

enum TStubfPanic
    {
    EStubfBadRequest = 100000,
    EStubfBadDescriptor,
    EStubfNotCompletedApis,
    EStubfBadMessage,
    EStubfNoLimitedMessage,
    EStubfClassMemberVariableIsNull,
    EStubfClassMemberVariableIsNotNull,
    EMainSchedulerError,
    EStubfApiAlreadyStarted,
    EStubfApiNotStarted,
    };

class CStubSrvSess;

NONSHARABLE_CLASS(CStubSrv) : public CPolicyServer
    {

public:

    static CStubSrv* NewLC();

    ~CStubSrv();

public:

    void StartShutdownTimerIfNoSessions();
    
    TInt ClientOpened(CStubSrvSess& aSession);
        
    void ClientClosed(CStubSrvSess& aSession);

    void EnqueueL(const RMessage2& aMessage);
    
	void DeleteQueues(const RMessage2& aMessage);
	
	TInt Dequeue(TApiBehavior& aBeh, TInt& aExeDuration, TInt aTransactionId);

	void GetApiCompleteCodeL(const RMessage2& aMessage);

private:

    CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
    
private:

    CStubSrv(TInt aPriority);

    void ConstructL();
    
    void CancelShutdownTimer();
            
    static TInt TimerFired(TAny* aThis);

private:
    class TApiQueueItem
        {
    public:
        TApiBehavior iBeh;
        TInt iExeDuration;
        TInt iTransactionId;
        };

private:

    CPeriodic* iTimer;
    
    RPointerArray<CStubSrvSess> iSessions;
    
    RPointerArray<TApiQueueItem> iBehQueue;
    TInt iTransctionUnique;
    };

#endif


