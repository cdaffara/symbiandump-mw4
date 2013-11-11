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

#ifndef C_USBTIMER_H
#define C_USBTIMER_H

#include <e32base.h>

#include <StifTestModule.h>
#include <StifLogger.h>

#include "tassertbase.h"

class CUsbTimer;

/**
 * timer ids for usbotgwatcher
 */
enum TUsbTimerId
    {
    EDeviceAttachmentTimer,
    EInactiveTimer,
    EIconBlinkingTimer,
    ETooMuchPowerRequiredTimer,
    EDriversNotFoundTimer
    };


class MUsbTimerObserver
    {
    friend class CtUsbOtgWatcher;
    friend class CUsbOtgWatcherTest;
public:

    virtual void TimerElapsedL(TUsbTimerId aTimerId) = 0;
    };

// Wrapper class, will report to MUsbTimerObserver once time is over
// Name CUsbTimer is given due to CTimer name already used
NONSHARABLE_CLASS(CUsbTimer) : public TAssertBase
    {
public:

    /**
     * Two-phased constructor
     *
     * @param aTimeOver pointer to a call back function. Called when time is over.
     */
    static CUsbTimer* NewL(MUsbTimerObserver& aObserver, TUsbTimerId aTimerId);

    /**
     * Destructor.
     */
    virtual ~CUsbTimer();

    // calls RunL after aMilliseconds
    void After(TInt aMilliseconds);

    //cancels timer
    void Cancel();

    TBool IsActive();

    void TriggerTimerElapsedL();

public:

    // from base class CActive

    void RunL();

    TInt RunError( TInt /*aError*/);

    void DoCancel();

private:

    CUsbTimer(MUsbTimerObserver& aObserver, TUsbTimerId aTimerId);

    void ConstructL();

private: // data

    MUsbTimerObserver* iObserver;
    TBool iActive;
    TUsbTimerId iTimerId;
    };

#endif //  C_USBTIMER_H
