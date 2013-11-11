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
 * Description:  Observes OTG states
 *
 */

#ifndef C_USBOTGSTATEOBSERVER_H
#define C_USBOTGSTATEOBSERVER_H

#include <e32base.h>
#include <e32property.h>
#include <usbotgdefs.h>

/**
 *  Observer need to implement this interface to get OTG state change notifications
 */
NONSHARABLE_CLASS( MUsbOtgStateObserver)
    {
public:

    /**
     * Local device is A, and get to Idle state
     */
    virtual void AIdleL() = 0;

    /**
     * Local device is A, and get to Host state
     */
    virtual void AHostL() = 0;

    /**
     * Local device is A, and get to Peripheral state
     */
    virtual void APeripheralL() = 0;

    /**
     * Local device is A, and get VBus error
     */
    virtual void AVBusErrorL() = 0;

    /**
     * Local device is B, and get to Idle state
     */
    virtual void BIdleL() = 0;

    /**
     * Local device is B, and get to Peripheral state
     */
    virtual void BPeripheralL() = 0;

    /**
     * Local device is B, and get to Host state
     */
    virtual void BHostL() = 0;

    /**
     * Error handler
     * @param aError error code
     */
    virtual void OtgStateErrorL(TInt aError) = 0;
    };

/**
 *  Class observes OTG State property
 *
 */
NONSHARABLE_CLASS( CUsbOtgStateObserver ) : public CActive
    {

public:

    /**
     * Two-phased constructor.
     */
    static CUsbOtgStateObserver* NewL();

    /**
     * Destructor.
     */
    virtual ~CUsbOtgStateObserver();

    /**
     * Gets current OTG state
     * @return OTG state
     */
    TUsbOtgState OtgState();

    /**
     * Subscribes for getting notifications
     * @param aObserver Observer
     */
    void SubscribeL(MUsbOtgStateObserver& aObserver);

    /**
     * Unsubscribes from getting notifications
     * @param aObserver Observer
     */
    void UnsubscribeL(MUsbOtgStateObserver& aObserver);

private:

    // From CActive
    /**
     * Called when outstanding request completed
     *
     */
    void RunL();

    /**
     * Called when outstanding request is cancelled
     *
     */
    void DoCancel();

    /**
     * Called when RunL leaves
     *
     * @param errorcode
     * @return errorcode
     */
    TInt RunError(TInt aError);

private:

    /**
     * Default constructor
     *
     */
    CUsbOtgStateObserver();

    /**
     * Second phase construction
     *
     */
    void ConstructL();

private:
    // data

    /**
     * The observer reports state changes to own observers
     * Not owns
     */
    RPointerArray<MUsbOtgStateObserver> iObservers;

    /**
     * The observer observes property change
     */
    RProperty iOtgState;

    };

#endif //  C_USBOTGSTATEOBSERVER_H
