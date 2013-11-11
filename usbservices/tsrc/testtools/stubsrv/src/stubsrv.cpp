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

#include "stubsrv.h"
#include "stubsrvsession.h"
#include "stubsrvapiexecutor.h"
#include "apibehavior.h"
#include "stubsrvsecuritypolicy.h"

CStubSrv::CStubSrv(TInt aPriority)
  	:CPolicyServer(aPriority, KStubSrvPolicy)
    {
    }

CStubSrv* CStubSrv::NewLC()
    {
    CStubSrv* self = new (ELeave) CStubSrv(EPriorityNormal);
    CleanupStack::PushL(self);
    self->StartL(KStubSrvName);
    self->ConstructL();
    return self;
    }

CStubSrv::~CStubSrv()
    {
    iBehQueue.ResetAndDestroy();
    iBehQueue.Close();
    iSessions.Close();
    delete iTimer;
    }

const TInt KShutdownDelay = 1000000;

void CStubSrv::StartShutdownTimerIfNoSessions()
	{
	if (iSessions.Count() == 0 && !iTimer->IsActive())
		{
		iTimer->Start(KShutdownDelay, 0, TCallBack(CStubSrv::TimerFired, this));
		}
	}

TInt CStubSrv::ClientOpened(CStubSrvSess& aSession)
	{
	return iSessions.Append(&aSession);
	}

void CStubSrv::ClientClosed(CStubSrvSess& aSession)
	{
	const TUint sessCount = iSessions.Count();
	for ( TUint ii = 0 ; ii < sessCount ; ++ii )
		{
		if ( iSessions[ii] == &aSession )
			{
			iSessions.Remove(ii);
			break;
			}
		}
	StartShutdownTimerIfNoSessions();
	}

void CStubSrv::EnqueueL(const RMessage2& aMessage)
    {
    TApiBehaviorPckgBuf buf;
    aMessage.ReadL(0, buf);
    TInt duration = aMessage.Int1();
    
    TInt sesss = iSessions.Count();
    for (TInt i = 0; i < sesss; i++)
        {
        TInt err = iSessions[i]->ConsumeApiBehavior(buf(), duration);
        if (err == KErrNone)
            {
            return;
            }
        }
    TApiQueueItem* item = new (ELeave) TApiQueueItem();
    item->iBeh = buf();
    item->iExeDuration = duration;
    item->iTransactionId = 0;  
    CleanupStack::PushL(item);
    iBehQueue.AppendL(item);
    CleanupStack::Pop(item);
    }

void CStubSrv::DeleteQueues(const RMessage2& aMessage)
    {
    TUint lib = aMessage.Int0();
    TUint api = aMessage.Int1();
    TInt count = iBehQueue.Count();
    for (TInt i = count - 1; i >= 0; i--)
        {
        if (iBehQueue[i]->iBeh.MatchId(lib, api))
            {
            delete iBehQueue[i];
            iBehQueue.Remove(i);
            }
        }
    }

TInt CStubSrv::Dequeue(TApiBehavior& aBeh, TInt& aExeDuration, TInt aTransactionId)
	{
    TInt count = iBehQueue.Count();
    for (TInt i = 0; i < count; i++)
        {
        if (iBehQueue[i]->iBeh.MatchId(aBeh.iLib, aBeh.iApi) &&
            aTransactionId == iBehQueue[i]->iTransactionId)
            {
            aBeh = iBehQueue[i]->iBeh;
            aExeDuration = iBehQueue[i]->iExeDuration;
            delete iBehQueue[i];
            iBehQueue.Remove(i);
            return KErrNone;
            }
        }
    return KErrNotFound;	
	}

void CStubSrv::GetApiCompleteCodeL(const RMessage2& aMessage)
    {
    TUint lib = aMessage.Int0();
    TUint api = aMessage.Int1();
    TInt count = iBehQueue.Count();
    for (TInt i = 0; i < count; i++)
        {
        if (iBehQueue[i]->iBeh.MatchId(lib, api))
            {
            iBehQueue[i]->iTransactionId = iTransctionUnique++;
            TPckgBuf<TInt> buf(iBehQueue[i]->iTransactionId);
            aMessage.WriteL(2, buf);
            aMessage.Complete(iBehQueue[i]->iBeh.iCompleteCode);
            return;
            }
        }
    aMessage.Complete(0);
    }

void CStubSrv::ConstructL()
    {
    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    }

CSession2* CStubSrv::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
    {
	TVersion v(KStubSrvMajorVersionNumber, KStubSrvMinorVersionNumber, KStubSrvBuildNumber);
	if ( !User::QueryVersionSupported(v, aVersion) )
		User::Leave(KErrNotSupported);
    CStubSrvSess* session = CStubSrvSess::NewL(const_cast<CStubSrv&>(*this));
    const_cast<CStubSrv*>(this)->CancelShutdownTimer();
    return session;
    }

void CStubSrv::CancelShutdownTimer()
    {
    iTimer->Cancel();
    }

TInt CStubSrv::TimerFired(TAny* /*aThis*/)
	{
	CActiveScheduler::Stop();
	return KErrNone;
	}
    
static void RunServerL()
	{
    static_cast<void>(User::LeaveIfError(User::RenameThread(KStubSrvName)));	
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
    CStubSrv::NewLC();
	RProcess::Rendezvous(KErrNone);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(2, scheduler);
	}

GLDEF_C TInt E32Main()
    {
	TInt ret = KErrNoMemory;
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup)
		{
		TRAP(ret, RunServerL());
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return ret;    
    }
