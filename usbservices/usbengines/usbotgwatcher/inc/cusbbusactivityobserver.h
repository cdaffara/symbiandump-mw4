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
 * Description:  Monitors USB activity
 *
 */

#ifndef C_USBBUSACTIVITYOBSERVER_H
#define C_USBBUSACTIVITYOBSERVER_H

#include <e32base.h>
#include <e32property.h>

/**
 *  This interface has to be implemented by observer
 *  to receive notifications from USB bus activity property
 */
NONSHARABLE_CLASS(MUsbBusActivityObserver)
    {
public:

    /**
     * Called when bus turn to idle
     */
    virtual void BusIdleL() = 0;

    /**
     * Called when when bus is in Idle, and some activity appears on the bus
     */
    virtual void BusActiveL() = 0;

    /**
     * Called when when error happened
     * @param aError errorcode
     */
    virtual void BusActivityErrorL(TInt aError) = 0;

    };

/**
 *  Class observes USB OTG State property
 *
 */
NONSHARABLE_CLASS(CUsbBusActivityObserver) : public CActive
    {

public:

    enum TBusActivity
        {
        EBusIdle = 0, EBusActive = 1
        };

    /**
     * Two-phased constructor.
     */
    static CUsbBusActivityObserver* NewL();

    /**
     * Destructor.
     */
    virtual ~CUsbBusActivityObserver();

    /**
     * Returns bus activity state
     * @return bus activity
     */
    TBusActivity BusActivity();

    /**
     * Adds subscriber for the notification
     * @param aObserver Observer
     */
    void SubscribeL(MUsbBusActivityObserver& aObserver);

    /**
     * Removes observer
     * @param aObserver Observer
     */
    void UnsubscribeL(MUsbBusActivityObserver& aObserver);

private:

    // From CActive
    /**
     * Called when request on bus activity completed
     */
    void RunL();

    /**
     * Called by framwork, when canceling outstanding request
     */
    void DoCancel();

    /**
     * Called when RunL leaves
     * @param aError error code
     */
    TInt RunError(TInt aError);

private:

    /**
     * Default constructor
     */
    CUsbBusActivityObserver();

    /**
     * 2nd phase construction
     */
    void ConstructL();

private:
    // data

    /**
     * The observer reports state changes to own observers
     * Not owns
     */
    RPointerArray<MUsbBusActivityObserver> iObservers;

    /**
     * The observer observes property change
     */
    RProperty iBusActivity;

    };

#endif //  C_USBOTGSTATEOBSERVER_H
