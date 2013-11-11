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
 * Description:  Implementation of the concret state
 *
 */

#ifndef C_USBSTATEHOSTAINITIATE_H
#define C_USBSTATEHOSTAINITIATE_H

#include "cusbstatehostabase.h"

#ifndef STIF
#include "cusbtimer.h"
#else
#include "mockcusbtimer.h"
#endif

class CUsbOtgWatcher;

/**
 *  This class implements behaviour when Id pin just detected (initiating host role, loading drivers)
 *
 */
NONSHARABLE_CLASS( CUsbStateHostAInitiate ) : public CUsbStateHostABase,
MUsbTimerObserver
    {
    friend class CtUsbOtgWatcher;

public:

    /**
     * Two-phased constructor.
     * @param aWatcher owner
     */
    static CUsbStateHostAInitiate* NewL(CUsbOtgWatcher& aWatcher);

    /**
     * Destructor.
     */
    virtual ~CUsbStateHostAInitiate();

private:

    // From MUsbTimerObserver
    /**
     * timer expired call back
     * @param aTimerId timer id
     */
    void TimerElapsedL(TUsbTimerId aTimerId);

    /**
     * default construction
     * @param aWatcher owner
     */
    CUsbStateHostAInitiate(CUsbOtgWatcher& aWatcher);

    /**
     * 2nd phase construction
     */
    void ConstructL();

    //from CUsbState
    /**
     * state id
     * @return state id
     */
    TUsbStateIds Id();

    /**
     * This is called when switched to this state, 
     * because If all conditions for transition to another state exist, nothing will 
     * trigger it and transition will not happen. This forces the transition in such cases.
     *
     */
    void JustAdvancedToThisStateL();

    /**
     * This is called when switched to this state, 
     * because If all conditions for transition to another state exist, nothing will 
     * trigger it and transition will not happen. This forces the transition in such cases.
     *
     */
    void JustBeforeLeavingThisStateL();

    // From VBus observer
    /**
     * VBus up
     */
    void VBusUpL();

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
     * Device is detached
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

private:
    // data

    /*
     * attachment timer
     * own
     */
    CUsbTimer* iAttachmentTimer;

    /*
     * attachment attribute
     */
    TBool iDeviceAttached;
    };

#endif //  C_USBSTATEHOSTINITIATE_H
