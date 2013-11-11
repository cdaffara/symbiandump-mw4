/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Base class for all task handlers
*
*/



// @file taskhandler.h

#ifndef __TASK_HANDLER_H__
#define __TASK_HANDLER_H__

#include "sit.h"

class MTaskHandlerManager;

/**
 * Parent class of all task handlers
 */
NONSHARABLE_CLASS(CTaskHandler) : public CActive, public MEventObserver
    {
public:
    CTaskHandler(MTaskHandlerManager* aManager, const TTaskArrivedEventData& aTaskInfo,
                 TEventType aCancelEventType, TDes8* iEventSpecPtr);
    void Start();
    void TaskDone();
    void SetDelayedTaskEnd(TBool aDelayedTaskEnd);
    
public: // From MEventObserver
    void EventOccured(TInt aStatus, TEventType aType, TDesC8* aData);

private: // From CActive
    void DoCancel();
    void RunL();

protected: // From CActive
    TInt RunError(TInt aError);

private:
    void OnFetchTaskInfoEvent(TInt aStatus, TDesC8* aData);
    void OnTaskRequestCancelledEvent(TInt aStatus);

protected:
    virtual void StartTaskHandling() = 0;
    virtual void ReportResult(TInt aStatus) = 0;
    
    void GotoState(TInt aState);
    void SetCurrState(TInt aState);
    void SetNextState(TInt aState);
    TInt CurrState();
    TInt NextState();
    virtual void ChangeStateL() = 0;
    virtual void CancelOngoingOperation() = 0;

    void ReportEvent(TEventType aEventType, TDesC8& aEventSpec, TDesC8& aEventData);
    void TaskComplete(TInt aStatus = KErrNone);
    void FatalError(TInt aStatus);

protected:
    MTaskHandlerManager* iManager;
    const TTaskArrivedEventData& iTaskInfo;
    TEventType iCancelEventType;
    TDes8* iEventSpecPtr;

    TInt iCurrState;
    TInt iNextState;

    TBool iDelayedTaskEnd;

public:
    REventMediator iEventMediator;
    };

/**
 * An interface that the manager of task handlers must implement
 */
class MTaskHandlerManager
    {
public:
    virtual void TaskHandlerComplete(CTaskHandler* aTaskHandler) = 0;
    virtual void TaskHandlerFatalError(CTaskHandler* aTaskHandler, TInt aError) = 0;
    };

#endif // __TASK_HANDLER_H__
