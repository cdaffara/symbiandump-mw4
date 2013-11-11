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
* Description: Combined task arrival observer and task handler manager.
*
*/



#include "taskarrivalobserver.h"
#include "taskhandlercreator.h"
#include "log.h"

CTaskArrivalObserver* CTaskArrivalObserver::NewL()
    {
    LOG(Log::Printf(_L("CTaskArrivalObserver::NewL - begin\n")));
    CTaskArrivalObserver* self = new (ELeave) CTaskArrivalObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    LOG(Log::Printf(_L("CTaskArrivalObserver::NewL - end\n")));
    return self;
    }
    
CTaskArrivalObserver::CTaskArrivalObserver()
    : CAsyncOneShot(EPriorityNormal)
    {
    }

void CTaskArrivalObserver::ConstructL()
    {
    User::LeaveIfError(iEventMediator.Connect());
    iTaskHandlerList = new (ELeave) CArrayPtrFlat<CTaskHandler>(2);
    iAsyncCleanerList = new (ELeave) CArrayPtrFlat<CAsyncCleaner>(2);
    }

CTaskArrivalObserver::~CTaskArrivalObserver()
    {
    LOG(Log::Printf(_L("CTaskArrivalObserver::~CTaskArrivalObserver\n")));
    Cancel();
    iEventMediator.Close();
    if (iTaskHandlerList)
        {
        iTaskHandlerList->ResetAndDestroy();
        delete iTaskHandlerList;
        }
    if (iAsyncCleanerList)
        {
        iAsyncCleanerList->ResetAndDestroy();
        delete iAsyncCleanerList;
        }
    }
    
void CTaskArrivalObserver::Start()
    {
    LOG(Log::Printf(_L("CTaskArrivalObserver::Start\n")));
    iEventMediator.ListenToEvent(ETaskArrivedEvent, *this);
    }

void CTaskArrivalObserver::Cancel()
    {
    LOG(Log::Printf(_L("CTaskArrivalObserver::Cancel\n")));
    iEventMediator.CancelListening(ETaskArrivedEvent);
    }

void CTaskArrivalObserver::EventOccured(TInt aStatus, TEventType /*aType*/, TDesC8* aData)
    {
    LOG(Log::Printf(_L("CTaskArrivalObserver::EventOccured\n")));
    if (aStatus == KErrNone && aData)
        {
        LOG(Log::Printf(_L("CTaskArrivalObserver::EventOccured - launching a new task handler\n")));
        // The event specification of the task request is delivered
        // to us as the event data of the task arrivat event
        TTaskArrivedEventData eventSpec;
        TPckg<TTaskArrivedEventData> eventSpecDes(eventSpec);
        eventSpecDes.Copy(*aData);
        
        TRAPD(ret, LaunchTaskHandlerL(eventSpec));

        // Not being able to create a task handler means that
        // the client that has issued the corresponding task
        // request would never be served. This is a fatal error
        // and not acceptable. Thus, we we need to terminate the
        // SIT thread in order to notify the Event Mediator and
        // its clients about problems.
        if (ret != KErrNone)
            {
            CActiveScheduler::Stop();
            return;
            }
        
        // Continue observing the
        // arrival of new tasks
        Start();
        }
    else
        {
        LOG(Log::Printf(_L("CTaskArrivalObserver::EventOccured - stopping the scheduler and thus the SIT\n")));
        // We cannot receive new tasks anymore
        // so the SIT can be terminated
        CActiveScheduler::Stop();
        }
    }

void CTaskArrivalObserver::LaunchTaskHandlerL(const TTaskArrivedEventData& aEventSpec)
    {
    LOG(Log::Printf(_L("CTaskArrivalObserver::LaunchTaskHandlerL\n")));
    // A new task has arrived so create a task handler for it
    CTaskHandler* taskHandler = CreateTaskHandlerL(aEventSpec);

    // Add the handler to the list of active handlers
    iTaskHandlerList->AppendL(taskHandler);

    // And start performing the task
    taskHandler->Start();
    }

CTaskHandler* CTaskArrivalObserver::CreateTaskHandlerL(const TTaskArrivedEventData& aEventSpec)
    {
    LOG(Log::Printf(_L("CTaskArrivalObserver::CreateTaskHandlerL\n")));
    CTaskHandler* taskHandler = NULL;

    taskHandler = TaskHandlerCreator::CreateTaskHandlerL(this, aEventSpec);

    if (!taskHandler)
        {
        User::Panic(KSitName, EPanicUnknownEventType);
        }

    return taskHandler;
    }

TInt CTaskArrivalObserver::FindTaskHandler(CTaskHandler* aTaskHandler)
    {
    TInt foundIndex = KUnfoundIndex;
    
    for (TInt i = 0; i < iTaskHandlerList->Count(); i++)
        {
        if (iTaskHandlerList->At(i) == aTaskHandler)
            {
            foundIndex = i;
            break;
            };
        }

    return foundIndex;
    }
    
TInt CTaskArrivalObserver::FindAsyncCleaner(CAsyncCleaner* aAsyncCleaner)
    {
    TInt foundIndex = KUnfoundIndex;
    
    for (TInt i = 0; i < iAsyncCleanerList->Count(); i++)
        {
        if (iAsyncCleanerList->At(i) == aAsyncCleaner)
            {
            foundIndex = i;
            break;
            };
        }

    return foundIndex;
    }

void CTaskArrivalObserver::TaskHandlerComplete(CTaskHandler* aTaskHandler)
    {
    LOG(Log::Printf(_L("CTaskArrivalObserver::TaskHandlerComplete\n")));

    // In the case several task handlers delete themselves at about the
    // same time, we need to have a separate cleaner instance for each.
    // Otherwise we'll get panic E32USER-CBase 42 (SetActive called
    // while active object is already active).
    
    // NOTE. Each asyncCleaner instance will cause itself to be deleted
    CAsyncCleaner* asyncCleaner = new CAsyncCleaner(this, aTaskHandler);
    if (asyncCleaner)
        {
        // Add the handler to a list of cleaners. This list
        // is needed to handle some rare cases where the SIT
        // thread dies before one or more async cleaners get
        // the chance to delete themselves. Such cleaner
        // instances get destroyed by the CTaskArrivalObserver
        // destructor.
        TRAP_IGNORE(iAsyncCleanerList->AppendL(asyncCleaner));
        // Initiate the task handler delete operation
        asyncCleaner->Start();
        }
    else
        {
        // Backup - just in case asyncCleaner could not be created
        AsyncDeleteTaskHandler(aTaskHandler);
        }
    }

void CTaskArrivalObserver::AsyncDeleteTaskHandler(CTaskHandler* aTaskHandler)
    {
    LOG(Log::Printf(_L("CTaskArrivalObserver::AsyncDeleteTaskHandler\n")));
    iTaskHandlerToDelete = aTaskHandler;
    Call();
    }

void CTaskArrivalObserver::RunL() // Called as a result of AsyncDeleteTaskHandler
    {
    LOG(Log::Printf(_L("CTaskArrivalObserver::RunL\n")));

    DeleteTaskHandler(iTaskHandlerToDelete);

    iTaskHandlerToDelete = NULL;    
    }

void CTaskArrivalObserver::DeleteTaskHandler(CTaskHandler* aTaskHandler)
    {
    LOG(Log::Printf(_L("CTaskArrivalObserver::DeleteTaskHandler\n")));
    
    // The specified task handler has done its
    // job succesfully so it can be deleted
    TInt taskHandlerIndex = FindTaskHandler(aTaskHandler);
    
    if (taskHandlerIndex != KUnfoundIndex)
        {
        LOG(Log::Printf(_L("CTaskArrivalObserver::DeleteTaskHandler - deleting task handler\n")));
        // Delete the task handler
        delete iTaskHandlerList->At(taskHandlerIndex);
        // Delete the list item
        iTaskHandlerList->Delete(taskHandlerIndex);
        // Deleting elements from the array does not cause
        // the array buffer to be automatically compressed.
        // Compress it to return excess space to the heap
        // as task handlers come and go.
        iTaskHandlerList->Compress();
        }
    else
        {
        // 
        delete aTaskHandler;
        }
    }
    
void CTaskArrivalObserver::DeleteAsyncCleaner(CAsyncCleaner* aAsyncCleaner)
    {
    LOG(Log::Printf(_L("CTaskArrivalObserver::DeleteAsyncCleaner\n")));
    
    // The specified asynchronous cleaner
    // has done its job and be deleted
    TInt asyncCleanerIndex = FindAsyncCleaner(aAsyncCleaner);
    
    if (asyncCleanerIndex != KUnfoundIndex)
        {
        LOG(Log::Printf(_L("CTaskArrivalObserver::DeleteAsyncCleaner - deleting async cleaner\n")));
        // Delete the cleaner object
        delete iAsyncCleanerList->At(asyncCleanerIndex);
        // Delete the list item
        iAsyncCleanerList->Delete(asyncCleanerIndex);
        // Deleting elements from the array does not cause
        // the array buffer to be automatically compressed.
        // Compress it to return excess space to the heap
        // as cleaner objects come and go.
        iAsyncCleanerList->Compress();
        }
    else
        {
        // Always delete the cleaner instance even
        // though it have not been added to the list
        delete aAsyncCleaner;
        }
    }

void CTaskArrivalObserver::TaskHandlerFatalError(CTaskHandler* /*aTaskHandler*/, TInt /*aError*/)
    {
    LOG(Log::Printf(_L("CTaskArrivalObserver::TaskHandlerFatalError - stopping the scheduler and thus the SIT\n")));
    // The specified task handler has encountered a fatal error
    // indicating that it cannot fulfill the task request it was
    // created to fulfill, meaning that the client that has issued
    // the corresponding task request would never be served. This
    // is a fatal error and not acceptable. Thus, we we need to
    // terminate the SIT thread in order to notify the Event Mediator
    // and its clients about problems.
    CActiveScheduler::Stop();
    }

// CAsyncCleaner
    
CAsyncCleaner::CAsyncCleaner(CTaskArrivalObserver* aTaskArrivalObserver,
                             CTaskHandler* aTaskHandlerToDelete)
    : CAsyncOneShot(EPriorityNormal), iTaskArrivalObserver(aTaskArrivalObserver),
      iTaskHandlerToDelete(aTaskHandlerToDelete)
    {
    }

void CAsyncCleaner::Start()
    {
    Call();
    }

void CAsyncCleaner::RunL()
    {
    // Delete the task handler
    iTaskArrivalObserver->DeleteTaskHandler(iTaskHandlerToDelete);

    // Delete this cleaner object instance as well
    iTaskArrivalObserver->DeleteAsyncCleaner(this);
    }
