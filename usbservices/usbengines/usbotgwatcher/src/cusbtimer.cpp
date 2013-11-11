/*
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Implementation
 *
 */

#include "cusbtimer.h"

#include "debug.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbTimer::CUsbTimer(MUsbTimerObserver& aObserver, TUsbTimerId aTimerId) :
    CActive(CActive::EPriorityStandard), iObserver(aObserver), iTimerId(
            aTimerId)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbTimer::~CUsbTimer()
    {
    LOG_FUNC
    Cancel();
    iTimer.Close();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbTimer::ConstructL()
    {
    LOG_FUNC
    LEAVEIFERROR(iTimer.CreateLocal());
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbTimer* CUsbTimer::NewL(MUsbTimerObserver& aObserver, TUsbTimerId aTimerId)
    {
    LOG_FUNC

    CUsbTimer* self = new (ELeave) CUsbTimer(aObserver, aTimerId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self); // pop self
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbTimer::After(TInt aMilliseconds)
    {
    if (IsActive())
        {
        Cancel();
        }

    // RunL will be called after KInactiveTimeForShutDown milliseconds
    iTimer.After(iStatus, TTimeIntervalMicroSeconds32(aMilliseconds));
    SetActive();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbTimer::RunL()
    {

    if (KErrNone != iStatus.Int())
        {
        LEAVE(iStatus.Int());
        }

    iObserver.TimerElapsedL(iTimerId);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CUsbTimer::RunError(TInt aError)
    {
    LOG_FUNC
    LOG1( "aError = %d" , aError );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbTimer::DoCancel()
    {
    iTimer.Cancel();
    }
