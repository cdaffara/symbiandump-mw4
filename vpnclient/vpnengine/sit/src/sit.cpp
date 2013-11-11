/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Socket Interaction Thread implementation.
*
*/



#include <e32math.h>
#include "sit.h"
#include "sitdeathobserver.h"
#include "taskarrivalobserver.h"
#include "taskhandlercreator.h"
#include "log.h"

// Exports

EXPORT_C CSit::CSit(MSitDeathListener* aSitDeathListener)
    : iTaskThread(NULL), iSitDeathListener(aSitDeathListener)
    {
    LOG(Log::Printf(_L("CSit::CSit\n")));
    }

EXPORT_C CSit::~CSit()
    {
    LOG(Log::Printf(_L("CSit::~CSit\n")));
    delete iSitDeathObserver;
    
    if (iTaskThread != NULL)
        {
        LOG(Log::Printf(_L("CSit::~CSit - closing task thread\n")));
        iTaskThread->Close();
        }
    delete iTaskThread;
    }

EXPORT_C void CSit::StartL()
    {
    LOG(Log::Printf(_L("CSit::StartL\n")));

    // Only start the thread if it is not already running
    if (!iTaskThread)
        {
        LOG(Log::Printf(_L("CSit::StartL - thread not running, starting it\n")));
        StartThreadL();
        }
    }
    
EXPORT_C TThreadId CSit::ThreadId()
    {
    if (iTaskThread)
        {
        return iTaskThread->Id();
        }
    else
        {
        return 0;
        }
    }

EXPORT_C TBool CSit::EventRequiresSit(TEventType aEventType)
    {
    return TaskHandlerCreator::EventRequiresSit(aEventType);
    }

EXPORT_C TEventType CSit::FindTaskRequestEventType(TEventType aCancelEventType)
    {
    return TaskHandlerCreator::FindTaskRequestEventType(aCancelEventType);
    }
    
EXPORT_C TEventType CSit::FindCancelEventType(TEventType aTaskRequestEventType)
    {
    return TaskHandlerCreator::FindCancelEventType(aTaskRequestEventType);
    }

EXPORT_C TBool CSit::IsTaskCancellationObservationRequest(TEventType aEventType)
    {
    return TaskHandlerCreator::IsTaskCancellationObservationRequest(aEventType);
    }

// Internals    


void CSit::StartThreadL()
    {
    LOG(Log::Printf(_L("CSit::StartThreadL\n")));
    TName threadName(KSitName);
    
    iTaskThread = new (ELeave) RThread;
    
    TInt ret = iTaskThread->Create(threadName,
                                   ThreadFunction,
                                   KDefaultStackSize,
                                   KMinHeapSize,
                                   KSitMaxHeapSize,
                                   this,
                                   EOwnerProcess);

    LOG(Log::Printf(_L("CSit::StartThreadL - iTaskThread->Create returned %d\n"), ret));
    User::LeaveIfError(ret);

    iSitDeathObserver = new (ELeave) CSitDeathObserver(iTaskThread->Id(), this);
    iSitDeathObserver->StartObservingL();
    
    iTaskThread->Resume();
    }

TInt CSit::ThreadFunction(TAny* aParameters)
    {
    LOG(Log::Printf(_L("CSit::ThreadFunction - begin\n")));
    __UHEAP_MARK;
    
    CTrapCleanup* cleanup = CTrapCleanup::New();
    
    TInt ret = KErrNoMemory;
    
    if (cleanup)
        {
        CSit* const sit = STATIC_CAST(CSit*, aParameters);
        TRAP(ret, sit->StartWorkingL());
        delete cleanup;
        }
    
    __UHEAP_MARKEND;

    LOG(Log::Printf(_L("CSit::ThreadFunction - end\n")));
    return ret;
    }

void CSit::StartWorkingL()
    {
    LOG(Log::Printf(_L("CSit::StartWorkingL - begin\n")));
    // Create and install the active scheduler we need
    CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);
    
    // Create the task arrival observer active object
    CTaskArrivalObserver* taskArrivalObserver = CTaskArrivalObserver::NewL();
    CleanupStack::PushL(taskArrivalObserver);

    // Start observing task arrivals
    taskArrivalObserver->Start();

    // Start running active objects
    CActiveScheduler::Start();

    // Cleanup the task handler and scheduler
    CleanupStack::PopAndDestroy(2); // taskArrivalObserver, scheduler

    LOG(Log::Printf(_L("CSit::StartWorkingL - end\n")));
    }

void CSit::SitDied()
    {
    LOG(Log::Printf(_L("CSit::SitDied\n")));
    
    if (iTaskThread != NULL)
        {
        iTaskThread->Close();
        delete iTaskThread;
        iTaskThread = NULL;

        iSitDeathListener->SitDied();

        // We must delete the observer object as well.
        // With this, the StartThreadL method works
        // correctly as it creates a new observer.
        delete iSitDeathObserver;
        iSitDeathObserver = NULL;
        }
    }

HBufC16* CSit::To16BitL(const TDesC8& aDes)
    {
    HBufC16* desCopy;
    
    TInt desLength = aDes.Length();
    
    if (desLength > 0)
        {
        desCopy = HBufC16::NewL(desLength);
        desCopy->Des().Copy(aDes);
        }
    else
        {
        desCopy = HBufC16::NewL(1);
        }

    return desCopy;
    }
