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
 * Description:  Observes notifications from usb
 *
 */

#ifndef C_USBMESSAGENOTIFICATIONOBSERVER_H
#define C_USBMESSAGENOTIFICATIONOBSERVER_H

#include <e32base.h>
#include <e32property.h>
#include <usbotgdefs.h>

class RUsb;

/**
 * Class implements functionality of showing/blinking usb indicator
 * Class does not provide method to get response from the user
 */
NONSHARABLE_CLASS( MUsbMessageNotificationObserver)
    {
public:

    /**
     * Call back on message notification
     * @param aMessage Message id
     */
    virtual void MessageNotificationReceivedL(TInt aMessage) = 0;

    /**
     * Called when BadHubPosition error happen
     */
    virtual void BadHubPositionL() = 0;

    /**
     * Callen if VBus error happen
     */
    virtual void VBusErrorL() = 0;

    /**
     * Called when SRP request received
     */
    virtual void SrpReceivedL() = 0;

    /**
     * Called when session requested
     */
    virtual void SessionRequestedL() = 0;

    /**
     * Called when error happened
     * @param aError error code
     */
    virtual void MessageNotificationErrorL(TInt aError) = 0;
    };

/**
 *  Class observes USB Notifications
 *
 */
NONSHARABLE_CLASS( CUsbMessageNotificationObserver ) : public CActive
    {

public:

    /**
     * Two-phased constructor.
     * @param aUsb usbman
     */
    static CUsbMessageNotificationObserver* NewL(RUsb* aUsb);

    /**
     * Destructor.
     */
    virtual ~CUsbMessageNotificationObserver();

    /**
     * Subscribes for notifications
     * @param aObserver observer
     */
    void SubscribeL(MUsbMessageNotificationObserver& aObserver);

    /**
     * Unsubscribe from notifications
     * @param aObserver Observer
     */
    void UnsubscribeL(MUsbMessageNotificationObserver& aObserver);

private:

    // From CActive
    /**
     * Called by framework when outstanding request completed
     *
     */
    void RunL();

    /**
     * Called when outstanding request camcelled
     *
     */
    void DoCancel();

    /**
     * Called when RunL leaves
     *
     * @param aError error id
     * @return error id 
     */
    TInt RunError(TInt aError);

private:

    /**
     * Default constructor
     * @param aUsb usbman
     */
    CUsbMessageNotificationObserver(RUsb* aUsb);

    /**
     * Second phase construction
     */
    void ConstructL();

private:
    // data

    /**
     * The observer reports to own observers
     * Not owns
     */
    RPointerArray<MUsbMessageNotificationObserver> iObservers;

    /**
     * Notifications come from USB
     * Not own
     */
    RUsb* iUsb;

    /**
     * Info from notification comes to this member
     */
    TInt iMessage;

    };

#endif //  C_USBMESSAGENOTIFICATIONOBSERVER_H
