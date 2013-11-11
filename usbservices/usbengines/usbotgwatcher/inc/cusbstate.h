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
 * Description:  Base class for states in state machine
 *
 */

#ifndef C_USBSTATE_H
#define C_USBSTATE_H

#include <e32base.h>
#include <usbotgdefs.h>
#include <usbman.h>

#include "definitions.h"

class CUsbOtgWatcher;

/**
 *  Base class for usb states
 *  Defines default implementation for events
 *  Class is C, due to there are possible future extentions
 *  for the class to own heap-based data. Class is not inherited
 *  from CActive, due to not all states are active objects. 
 *  If concrete state need to be an active object, it can aggregate CActive,
 *  rather than inherit (due to multiple inheritence of C classes is not allowed)
 *
 */
NONSHARABLE_CLASS( CUsbState ) : public CBase

    {

    /**
     *  Only UsbOtgWatcher intended to call any of protected methods from the class
     * (except ChangeState and ChangeHostState, called by concrete subclasses),
     *	and those methods are protected, due to have to be re-implemented in subclasses
     * Class is not intended for instantiation, that's why no any public construction methods
     */
    friend class CUsbOtgWatcher;

    /******************************************************************************* */
public:

    /**
     * This function is needed to find a real state by id
     * This might be used in switching states in watcher. New state object searched by its id
     * @return Id
     */
    virtual TUsbStateIds Id() = 0;

    /**
     * Destruction
     */
    virtual ~CUsbState();

protected:
    /**
     * default constructor is protected to be able to be inherited be child classes
     * @param aOwner owner of the state
     */
    CUsbState(CUsbOtgWatcher& aOwner);

    /* *
     * Changes state in Device state machine
     * @param aNewStateId new state id
     */
    void ChangeStateL(TUsbStateIds aNewStateId);

    /**
     * Changes state in host state machine
     * @param aNewStateId new host state
     */
    void ChangeHostStateL(TUsbStateIds aNewStateId);

    /**
     * Handles special situation (error, or something else)
     * @param aWhat to hande (id)
     * @param aWhereTohandle state id where to handle
     */
    virtual void HandleL(TInt aWhat, TUsbStateIds aWhereTohandle);

    /**
     * State machine calls this, state object to perform any initial  
     * activity, once just entered this state
     *
     */
    virtual void JustAdvancedToThisStateL();

    /**
     * State machine calls this, state object to perform any initial  
     * activity, just before leaving this state
     *
     */
    virtual void JustBeforeLeavingThisStateL();

    /**
     * Following virtual functions are called by UsbOtgWatcher
     * on getting accodring event. Those are implemented in concret states.
     */

    /**
     * 2nd phase construction
     */
    void ConstructL();

    /**
     * Sets personality
     */
    virtual void SetPersonalityL();

    /**
     * Cancels setting personality async.
     */
    virtual void CancelSetPersonalityL();

    /**
     *
     * Sets previous personality
     */
    virtual void SetPreviousPersonalityL();

    /**
     * Cancels async. set prev personality request
     */
    virtual void CancelSetPreviousPersonalityL();

    /**
     *
     * Sets prev. personality on disconnecting device
     */
    virtual void SetPreviousPreviousPersonalityOnDisconnectL();

    /**
     * Id Pin OFF
     */
    virtual void IdPinOffL();
    /**
     * Id Pin On
     */
    virtual void IdPinOnL();

    // From VBus observer
    /**
     * VBus down
     */
    virtual void VBusDownL();
    /**
     * VBus UP
     */
    virtual void VBusUpL();

    // From OTG state observer
    /**
     * Became to Idle when A
     */
    virtual void AIdleL();
    /**
     * Became a Host when A
     */
    virtual void AHostL();
    /**
     * Became a peripheral when A
     */
    virtual void APeripheralL();
    /**
     * VBus error happen
     */
    virtual void AVBusErrorL();
    /**
     * Became to Idle when B
     */
    virtual void BIdleL();
    /**
     * Became a peripheral when B
     */
    virtual void BPeripheralL();
    /**
     * Became a Host when B
     */
    virtual void BHostL();

    // From bus activity observer
    /**
     * Bus is in idle
     */
    virtual void BusIdleL();
    /**
     * Bus active
     */
    virtual void BusActiveL();

    // From Host Event notification observer
    /**
     * Device is attached
     * @param aInfo Device event data
     */
    virtual void DeviceAttachedL(TDeviceEventInformation aInfo);
    /**
     * Device is detached
     * @param aInfo Device event data
     */
    virtual void DeviceDetachedL(TDeviceEventInformation aInfo);
    /**
     * Drivers successfully loaded
     * @param aInfo Device event data
     */
    virtual void DriverLoadSuccessL(TDeviceEventInformation aInfo);
    /**
     * Drivers loaded partially
     * @param aInfo Device event data
     */
    virtual void DriverLoadPartialSuccessL(TDeviceEventInformation aInfo);
    /**
     * Drivers loading failed
     * @param aInfo Device event data
     */
    virtual void DriverLoadFailureL(TDeviceEventInformation aInfo);

    // From message notification observer
    /**
     * Message received
     * @param aMessage message id
     */
    virtual void MessageNotificationReceivedL(TInt aMessage);
    /**
     * Connected to hub in wrong level 
     */
    virtual void BadHubPositionL();
    /**
     * VBus error happened
     */
    virtual void VBusErrorL();
    /**
     * SRP request received
     */
    virtual void SrpReceivedL();
    /**
     * Session request received
     */
    virtual void SessionRequestedL();

protected:
    // data

    /**
     * Owner
     * Not own.  
     */
    CUsbOtgWatcher& iWatcher;

    };

#endif // C_USBSTATE_H
