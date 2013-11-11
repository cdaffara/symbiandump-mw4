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
* Description: Base class for all task handlers.
*
*/



#include "taskhandler.h"
#include "log.h"

CTaskHandler::CTaskHandler(MTaskHandlerManager* aManager, const TTaskArrivedEventData& aTaskInfo,
                           TEventType aCancelEventType, TDes8* aEventSpecPtr)
    : CActive(EPriorityNormal), iManager(aManager), iTaskInfo(aTaskInfo),
      iCancelEventType(aCancelEventType), iEventSpecPtr(aEventSpecPtr)
    {
    LOG(Log::Printf(_L("CTaskHandler::CTaskHandler - iCancelEventType = %d\n"), iCancelEventType));
    }

void CTaskHandler::Start()
    {
    LOG(Log::Printf(_L("CTaskHandler::Start\n")));
    // First fetch the event specification of the
    // task request that we are supposed to fulfill
    TPckg<TTaskArrivedEventData> taskInfoDes(iTaskInfo);
    iEventMediator.ListenToEvent(EFetchTaskInfoEvent, taskInfoDes, *this);
    // Operation continues from CTaskHandler::EventOccured
    // that calls StartTaskHandlingL if all goes well
    }

void CTaskHandler::RunL()
    {
    ChangeStateL();
    }

void CTaskHandler::GotoState(TInt aState)
    {
    SetNextState(aState);
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }
    
void CTaskHandler::SetCurrState(TInt aState)
    {
    iCurrState = aState;
    }

void CTaskHandler::SetNextState(TInt aState)
    {
    iNextState = aState;
    }

TInt CTaskHandler::CurrState()
    {
    return iCurrState;
    }

TInt CTaskHandler::NextState()
    {
    return iNextState;
    }

void CTaskHandler::DoCancel()
    {
    LOG(Log::Printf(_L("CTaskHandler::DoCancel\n")));
    // Cancel listening to the task cancellation
    iEventMediator.CancelListening(iCancelEventType, *iEventSpecPtr);

    CancelOngoingOperation();
            
    TaskDone();
    }

TInt CTaskHandler::RunError(TInt aError)
    {
    LOG(Log::Printf(_L("CTaskHandler::RunError - error = %d\n"), aError));
    TaskComplete(aError);
    return KErrNone;
    }

void CTaskHandler::EventOccured(TInt aStatus, TEventType aType, TDesC8* aData)
    {
    LOG(Log::Printf(_L("CTaskHandler::EventOccured\n")));
    if (aType == EFetchTaskInfoEvent)
        {
        OnFetchTaskInfoEvent(aStatus, aData);
        }
    else if (aType == iCancelEventType)
        {
        OnTaskRequestCancelledEvent(aStatus);
        }
    else
        {
        User::Panic(KSitName, EPanicUnexpectedEventOccured);
        }
    }

void CTaskHandler::OnFetchTaskInfoEvent(TInt aStatus, TDesC8* aData)
    {
    LOG(Log::Printf(_L("CTaskHandler::OnFetchTaskInfoEvent\n")));
    if (aStatus == KErrNone && aData)
        {
        // Copy the task request event specification
        // so that the actual task handler can start using it
        iEventSpecPtr->Copy(*aData);
        
        // Listen to the cancellation of this task
        iEventMediator.ListenToEvent(iCancelEventType, *iEventSpecPtr, *this);
        
        StartTaskHandling();
        }
    else if (aStatus == KErrNotFound)
        {
        // The task request was cancelled before 
        // we had the change to get its data
        TaskDone();
        }
    else
        {
        FatalError(aStatus);
        }
    }

void CTaskHandler::OnTaskRequestCancelledEvent(TInt aStatus)
    {
    LOG(Log::Printf(_L("CTaskHandler::OnTaskRequestCancelledEvent\n")));
    if (aStatus == KErrNone)
        {
        // Event Mediator tells that the task request
        // has been cancelled so we stop performing the task
        Cancel();
        }
    else if (aStatus != KErrCancel)
        {
        FatalError(aStatus);
        }
    }

void CTaskHandler::TaskComplete(TInt aError)
    {
    LOG(Log::Printf(_L("CTaskHandler::TaskComplete - aError = %d\n"), aError));
    // Cancel listening to the task cancellation
    iEventMediator.CancelListening(iCancelEventType, *iEventSpecPtr);

    ReportResult(aError);
    }
    
void CTaskHandler::ReportEvent(TEventType aEventType, TDesC8& aEventSpec, TDesC8& aEventData)
    {
    LOG(Log::Printf(_L("CTaskHandler::ReportEvent - aEventType = %d\n"), aEventType));
    TInt ret = iEventMediator.ReportEvent(aEventType, aEventSpec, aEventData);

    if (ret != KErrNone)
        {
        FatalError(ret);
        }
    else
        {
        TaskDone();
        }
    }

void CTaskHandler::TaskDone()
    {
    LOG(Log::Printf(_L("CTaskHandler::TaskDone\n")));
    if (!iDelayedTaskEnd)
        {
        LOG(Log::Printf(_L("CTaskHandler::TaskDone - calling iManager->TaskHandlerComplete\n")));
        iManager->TaskHandlerComplete(this);
        }
    }

void CTaskHandler::FatalError(TInt aError)
    {
    LOG(Log::Printf(_L("CTaskHandler::FatalError - error = %d\n"), aError));
    iManager->TaskHandlerFatalError(this, aError);
    }

void CTaskHandler::SetDelayedTaskEnd(TBool aDelayedTaskEnd)
    {
    iDelayedTaskEnd = aDelayedTaskEnd;
    }
