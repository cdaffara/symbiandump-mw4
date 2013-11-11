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
 * Description:  Implements concrete state
 *
 */

#ifndef C_USBSTATEHOSTDELAYANOTTTACHEDHANDLE_H
#define C_USBSTATEHOSTDELAYNOTATTACHEDHANDLE_H

#include "cusbstatehostdelayhandle.h"

#ifndef STIF
#include "cusbtimer.h"
#else
#include "mockcusbtimer.h"
#endif

class CUsbOtgWatcher;
/**
 *  problem handling, device is not attached, vbus not dropping
 *
 */
NONSHARABLE_CLASS( CUsbStateHostDelayNotAttachedHandle ) : public CUsbStateHostDelayHandle,
MUsbTimerObserver
    {
    friend class CtUsbOtgWatcher;
    friend class CUsbOtgWatcher;

public:

    /**
     * Two-phased constructor.
     * @param aWatcher owner
     */
    static CUsbStateHostDelayNotAttachedHandle* NewL(CUsbOtgWatcher& aWatcher);

    /**
     * Destructor.
     */
    virtual ~CUsbStateHostDelayNotAttachedHandle();

private:

    // From MUsbTimerObserver
    /**
     * Call back when timer expired
     * @param aTimerId timer id
     */
    void TimerElapsedL(TUsbTimerId aTimerId);

    /**
     * Device is detached
     * @param aInfo Device event data
     */
    void DeviceDetachedL(TDeviceEventInformation aInfo);

    // From OTG state observer
    /**
     * Became to Idle when A
     */
    void AIdleL();

    //from CUsbState
    /**
     * State id
     * @return state id
     */
    TUsbStateIds Id();

    /**
     * This is called when leaving this state, 
     *
     */
    void JustBeforeLeavingThisStateL();

    /**
     * handles issue
     */
    void DoHandleL();

    /**
     * Default constructor
     * @param aWatcher owner
     */
    CUsbStateHostDelayNotAttachedHandle(CUsbOtgWatcher& aWatcher);

    /**
     * 2nd phase construction
     */
    void ConstructL();

    // data

    /** 
     * too much power timer
     * own
     */
    CUsbTimer* iTooMuchPowerTimer;

    };

#endif //  C_USBSTATEHOSTDELAYNOTATTACHEDHANDLE_H
