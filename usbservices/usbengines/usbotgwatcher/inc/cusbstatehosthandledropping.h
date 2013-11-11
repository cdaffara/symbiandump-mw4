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

#ifndef C_USBSTATEHOSTHANDLEDROPPING_H
#define C_USBSTATEHOSTHANDLEDROPPING_H

#include "cusbstatehosthandle.h"

#ifndef STIF
#include "cusbnotifmanager.h"
#else
#include "mockcusbnotifmanager.h"
#endif

class CUsbOtgWatcher;

/**
 *  problem handling class, dropping vbus
 *
 */
NONSHARABLE_CLASS( CUsbStateHostHandleDropping ) : public CUsbStateHostHandle,
MWaitNotifierObserver
// this means only one wait notifier at a time can be shown by this state, might be redesigned

    {
    friend class CtUsbOtgWatcher;
    friend class CUsbOtgWatcher;

public:

    /**
     * Two-phased constructor.
     * @param aWatcher owner
     */
    static CUsbStateHostHandleDropping* NewL(CUsbOtgWatcher& aWatcher);

    /**
     * Destructor.
     */
    virtual ~CUsbStateHostHandleDropping();

private:

    // from MWaitNotifierObserver
    /**
     * Callback when notifier is completed
     * @param aFeedback how notifier completed
     */
    void WaitNotifierCompletedL(TInt aFeedback);

    // From VBus observer
    /**
     * VBus up
     */
    void VBusUpL();

    /**
     * VBus down
     */
    void VBusDownL();

    /**
     * VBus error happen
     */
    void AVBusErrorL();

    // From Host Event notification observer
    /**
     * Device is attached
     * @param aInfo Device event data
     */
    void DeviceAttachedL(TDeviceEventInformation aInfo);

    /**
     * Device is detached
     * @param aInfo Device event data
     */
    void DeviceDetachedL(TDeviceEventInformation aInfo);

    /**
     * Drivers successfully loaded
     * @param aInfo Device event data
     */
    void DriverLoadSuccessL(TDeviceEventInformation aInfo);

    /**
     * Drivers loaded partially
     * @param aInfo Device event data
     */
    void DriverLoadPartialSuccessL(TDeviceEventInformation aInfo);

    /**
     * Drivers loading failed
     * @param aInfo Device event data
     */
    void DriverLoadFailureL(TDeviceEventInformation aInfo);

    // From message notification observer
    /**
     * Message received
     * @param aMessage message id
     */
    void MessageNotificationReceivedL(TInt aMessage);

    /**
     * VBus error happened
     */
    void VBusErrorL();

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
    CUsbStateHostHandleDropping(CUsbOtgWatcher& aWatcher);

    /**
     * 2nd phase construction
     */
    void ConstructL();

    };

#endif //  C_USBSTATEHOSTDROPPINGHANDLE_H
