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
 * Description:  timer
 *
 */

#ifndef C_USBTIMER_H
#define C_USBTIMER_H

#include <e32base.h>

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

/**
 * Observers gets feedback by implementing this interface
 */
class MUsbTimerObserver
    {
    friend class CtUsbOtgWatcher;

public:

    /**
     * Observer must implement this interace
     * which is called back when timer expires
     * @param timer id
     */
    virtual void TimerElapsedL(TUsbTimerId aTimerId) = 0;
    };

/* *
 * Wrapper class, will report to MUsbTimerObserver once time is over
 * Name CUsbTimer is given due to CTimer name already used
 */
NONSHARABLE_CLASS(CUsbTimer) : public CActive
    {
public:

    /**
     * Two-phased constructor
     *
     * @param aObserver will get call back
     * @param aTimerId timer id
     */
    static CUsbTimer * NewL(MUsbTimerObserver& aObserver,
            TUsbTimerId aTimerId);

    /**
     * Destructor.
     */
    virtual ~CUsbTimer();

    /**
     * calls RunL after aMilliseconds
     * @param aMilliseconds time in millisecs
     */
    void After(TInt aMilliseconds);

public:

    // from base class CActive
    /**
     * Called when request is completed
     */
    void RunL();

    /**
     * called when RunL leaves
     * @param aError error code
     * @return error code
     */
    TInt RunError(TInt aError);

    /**
     * Called when request is cancelled
     */
    void DoCancel();

private:

    /**
     * Default constructor
     * @param aObserver will get call back
     * @param aTimerId timer id 
     */
    CUsbTimer(MUsbTimerObserver& aObserver, TUsbTimerId aTimerId);

    /**
     * 2nd phase construction
     */
    void ConstructL();

private:
    // data

    /**
     * Observer
     * not own
     */
    MUsbTimerObserver& iObserver;

    /**
     * RTimer API
     */
    RTimer iTimer;

    /**
     * timer id
     */
    TUsbTimerId iTimerId;
    };

#endif //  C_USBTIMER_H
