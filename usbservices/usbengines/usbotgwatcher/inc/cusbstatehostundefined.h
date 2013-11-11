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

#ifndef C_USBSTATEHOSTUNDEFINED_H
#define C_USBSTATEHOSTUNDEFINED_H

#include "cusbstatehostabase.h"

class CUsbOtgWatcher;
/**
 *  This class implements behaviour in undefined state (when host)
 *
 */
NONSHARABLE_CLASS( CUsbStateHostUndefined ) : public CUsbStateHostABase
    {
public:

    /**
     * Destruction
     */
    virtual ~CUsbStateHostUndefined();

    /**
     * Two-phased constructor.
     * @param aWatcher owner
     */
    static CUsbStateHostUndefined* NewL(CUsbOtgWatcher& aWatcher);

private:
    /**
     * default constructor is protected to be able to be inherited by child classes
     * @param aOwner owner of the state
     */
    CUsbStateHostUndefined(CUsbOtgWatcher& aOwner);

    /**
     * Second phase construction
     */
    void ConstructL();

    /**
     * This function is needed to find a real state by id
     * This might be used in switching states in watcher. New state object searched by its id
     * @return Id
     */
    TUsbStateIds Id();

    // From VBus observer
    /**
     * VBus down
     */
    void VBusDownL();

    /**
     * VBus UP
     */
    void VBusUpL();

    /**
     * VBus error
     */
    void VBusErrorL();

    /**
     * AVBus Error
     */
    void AVBusErrorL();

    /**
     * Became to Idle when B
     */
    void BIdleL();

    /**
     * Became to Peripheral when B
     */
    void BPeripheralL();

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
     * SRP request received
     */
    void SrpReceivedL();

    /**
     * Connected to hub in wrong level 
     */
    void BadHubPositionL();

    /**
     * Session request received
     */
    void SessionRequestedL();

private:
    // data
    };

#endif //  C_USBSTATEHOSTUNDEFINED_H
