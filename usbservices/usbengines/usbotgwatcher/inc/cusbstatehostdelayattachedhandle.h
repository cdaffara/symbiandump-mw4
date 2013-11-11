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

#ifndef C_USBSTATEHOSTDALAYATTACHEDHANDLE_H
#define C_USBSTATEHOSTDALAYATTACHEDHANDLE_H

#include "cusbstatehostdelayhandle.h"

#ifndef STIF
#include "cusbtimer.h"
#else
#include "mockcusbtimer.h"
#endif

class CUsbOtgWatcher;

/**
 *  problem handling, device is attached, vbus not dropping
 *
 */
NONSHARABLE_CLASS( CUsbStateHostDelayAttachedHandle ) : public CUsbStateHostDelayHandle,
public MUsbTimerObserver
    {
    friend class CtUsbOtgWatcher;
    friend class CUsbOtgWatcher;

public:

    /**
     * Two-phased constructor.
     * @param aWatcher owner
     */
    static CUsbStateHostDelayAttachedHandle* NewL(CUsbOtgWatcher& aWatcher);

    /**
     * Destructor.
     */
    virtual ~CUsbStateHostDelayAttachedHandle();

protected:

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
    virtual void DeviceDetachedL(TDeviceEventInformation aInfo);

    //from CUsbState
    /**
     * State id
     * @return state id
     */
    virtual TUsbStateIds Id();

    /**
     * This is called when leaving this state, 
     *
     */
    virtual void JustBeforeLeavingThisStateL();

    /**
     * handles issue
     */
    virtual void DoHandleL();

    /**
     * Default constructor
     * @param aWatcher owner
     */
    CUsbStateHostDelayAttachedHandle(CUsbOtgWatcher& aWatcher);

    /**
     * 2nd phase construction
     */
    void ConstructL();

    // data

private:
    /** 
     * drivers not found
     * own
     */
    CUsbTimer* iDriversNotFoundTimer;
    };

#endif //  C_USBSTATEHOSTDELAYATTACHEDHANDLE_H
