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
* Description:   Task handler for listening to the real interface
*
*/



// @file realiapconnobserver.h

#ifndef __REAL_IAP_CONN_OBSERVER_H__
#define __REAL_IAP_CONN_OBSERVER_H__

#include "sit.h"
#include "taskhandler.h"
#include "eventdefssit.h"

// Task handler states
const TInt KStateStartObserving   = 1;
const TInt KStateFinishObserving  = 2;

/**
 * Real IAP connection observer task handler
 */
NONSHARABLE_CLASS(CRealIapConnObserver) : public CTaskHandler
    {
public:
    static CRealIapConnObserver* NewL(MTaskHandlerManager* aManager,
                                      const TTaskArrivedEventData& aTaskInfo);
    ~CRealIapConnObserver();

public: // From MEventObserver
    void EventOccured(TInt aStatus, TEventType aType, TDesC8* aData);
    
private: // From CTaskHandler
    void StartTaskHandling();
    void ChangeStateL();
    void CancelOngoingOperation();
    void ReportResult(TInt aStatus);

private:
    CRealIapConnObserver(MTaskHandlerManager* aManager,
                         const TTaskArrivedEventData& aTaskInfo);
    void ConstructL();

    void StateStartObservingL();
    void StateFinishObserving();

private:
    TPckgBuf<TObserveRealIapConnEventSpec> iEventSpecDes;
    };

#endif // __REAL_IAP_CONN_OBSERVER_H__
