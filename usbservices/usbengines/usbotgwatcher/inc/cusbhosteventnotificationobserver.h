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
 * Description:  Monitors host events
 *
 */

#ifndef C_USBHOSTEVENTNOTIFICATIONOBSERVER_H
#define C_USBHOSTEVENTNOTIFICATIONOBSERVER_H

#include <e32base.h>
#include <e32property.h>
#include <usbotgdefs.h>
#include <usbhostdefs.h>

class RUsb;

/**
 *  Observer must implement this interface to get notifications
 *
 */
NONSHARABLE_CLASS( MUsbHostEventNotificationObserver)
    {
public:

    /**
     *  Called when device is attached
     *  @param aInfo additional information
     */
    virtual void DeviceAttachedL(TDeviceEventInformation aInfo) = 0;

    /**
     *  Called when device is detached
     *  @param aInfo additional information
     */
    virtual void DeviceDetachedL(TDeviceEventInformation aInfo) = 0;

    /**
     *  Called when drivers are loaded
     *  @param aInfo additional information
     */
    virtual void DriverLoadSuccessL(TDeviceEventInformation aInfo) = 0;

    /**
     *  Called when drivers are loaded partially
     *  @param aInfo additional information
     */
    virtual void DriverLoadPartialSuccessL(TDeviceEventInformation aInfo) = 0;

    /**
     *  Called when drivers loading failed
     *  @param aInfo additional information
     */
    virtual void DriverLoadFailureL(TDeviceEventInformation aInfo) = 0;

    /**
     *  Called when error happens while observing
     *  @param aError error code
     */
    virtual void HostEventNotificationErrorL(TInt aError) = 0;

    };

/**
 *  Class observes USB Notifications
 */
NONSHARABLE_CLASS( CUsbHostEventNotificationObserver ) : public CActive
    {

public:

    /**
     * Two-phased constructor.
     * @param aUsb usbman
     * @return this class instance
     */
    static CUsbHostEventNotificationObserver* NewL(RUsb* aUsb);

    /**
     * Destructor.
     */
    virtual ~CUsbHostEventNotificationObserver();

    /**
     * Add observer to list of observers
     * @param aObserver observer
     */
    void SubscribeL(MUsbHostEventNotificationObserver& aObserver);

    /**
     * Removes observer from list of observers 
     * @param aObserver Observer
     */
    void UnsubscribeL(MUsbHostEventNotificationObserver& aObserver);

private:

    // From CActive
    /**
     * Called when request completed
     */
    void RunL();

    /**
     * Called when outstanding request cancelled
     */
    void DoCancel();

    /**
     * Called when RunL leaves
     * @param aError errorcode from RunL
     * @return aError errorcode
     */
    TInt RunError(TInt aError);

private:

    /**
     * Default constructor.
     * @param aUsb usbman
     */
    CUsbHostEventNotificationObserver(RUsb* aUsb);

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

private:
    // data

    /**
     * The observer reports changes to own observers
     * Not owns
     */
    RPointerArray<MUsbHostEventNotificationObserver> iObservers;

    /**
     * Notifications come from USB
     * Not own
     */
    RUsb* iUsb;

    /**
     * Info from notification comes to this member
     */
    TDeviceEventInformation iEventInfo;

    };

#endif //  C_USBHOSTEVENTNOTIFICATIONOBSERVER_H
