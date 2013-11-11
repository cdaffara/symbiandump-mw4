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
 * Description:  Monitors ID pin change
 *
 */

#ifndef C_USBIDPINOBSERVER_H
#define C_USBIDPINOBSERVER_H

#include <e32base.h>
#include <e32property.h>

/**
 *  Obserber has to implement this interface to get ID Pin change notifications
 *
 */
NONSHARABLE_CLASS( MUsbIdPinObserver)
    {
public:

    /**
     * IdPin disappeared
     */
    virtual void IdPinOffL() = 0;

    /**
     * IdPin appeared
     */
    virtual void IdPinOnL() = 0;

    /**
     * IdPin error
     * @param aError error code
     */
    virtual void IdPinErrorL(TInt aError) = 0;

    };

/**
 *  Class observes ID-PIN property
 *
 */
NONSHARABLE_CLASS( CUsbIdPinObserver) : public CActive
    {

public:
    enum TState
        {
        EIdPinOff = 0, EIdPinOn = 1
        };

    /**
     * Two-phased constructor.
     * @return instance of the objects of this class
     */
    static CUsbIdPinObserver* NewL();

    /**
     * Destructor.
     */
    virtual ~CUsbIdPinObserver();

    /**
     * Returns current state of the IdPin
     * @return state
     */
    TState IdPin() /*  not const - read comment in cpp*/;

    /**
     * Add observer
     * @param aObserver Observer
     */
    void SubscribeL(MUsbIdPinObserver& aObserver);

    /**
     * Remove observer
     * @param aObserver Observer
     */
    void UnsubscribeL(MUsbIdPinObserver& aObserver);

private:

    // From CActive
    /**
     * Request completed
     */
    void RunL();

    /**
     * Request cancelled
     */
    void DoCancel();

    /**
     * RunL leaved
     * @param errorcode from RunL
     * @return errorcode
     */
    TInt RunError(TInt aError);

private:

    /**
     * Default constructor.
     */
    CUsbIdPinObserver();

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

private:
    // data

    /**
     *  The observer reports state changes to its own observers
     * Not own
     */
    RPointerArray<MUsbIdPinObserver> iObservers;

    /**
     * The observer observes property change
     */
    RProperty iIdPin;

    };

#endif //  C_USBIDPINOBSERVER_H
