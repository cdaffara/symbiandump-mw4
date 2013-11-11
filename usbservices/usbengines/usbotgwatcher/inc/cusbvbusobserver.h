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
 * Description:  Implements VBus observer
 *
 */

#ifndef C_USBVBUSOBSERVER_H
#define C_USBVBUSOBSERVER_H

#include <e32base.h>
#include <e32property.h>

/**
 * Observers of this class have to implement this interface
 * to get call back on VBus events
 */
NONSHARABLE_CLASS( MUsbVBusObserver)
    {
public:

    /**
     * VBus down event received
     */
    virtual void VBusDownL() = 0;
    /**
     * VBus up event received
     */
    virtual void VBusUpL() = 0;
    /**
     * error handler
     * @param aError error code
     */
    virtual void VBusObserverErrorL(TInt aError)=0;

    };

/**
 *  Class observes VBUS property
 *
 */
NONSHARABLE_CLASS( CUsbVBusObserver ) : public CActive
    {

public:
    enum TState
        {
        EVBusDown = 0, EVBusUp = 1

        };

    /**
     * Two-phased constructor.
     */
    static CUsbVBusObserver* NewL();

    /**
     * Destructor.
     */
    virtual ~CUsbVBusObserver();

    /**
     * VBus state getter
     * @return VBus state
     */
    TState VBus() /* not const. read comment in impl*/;

    /**
     * Adds observer to a list
     * @param aObserver observer
     */
    void SubscribeL(MUsbVBusObserver& aObserver);

    /**
     * removes observer from list
     * @param aObserver observer
     */
    void UnsubscribeL(MUsbVBusObserver& aObserver);

private:

    // From CActive
    /**
     * Called when request completed
     *
     */
    void RunL();

    /**
     * Called when request is cancelled
     *
     */
    void DoCancel();

    /**
     * Called when RunL leaves
     * @param aError error code
     * @return error code
     *
     */
    TInt RunError(TInt aError);

private:

    /**
     * Default constructor
     */
    CUsbVBusObserver();

    /**
     * 2nd phase constructor
     */
    void ConstructL();

private:
    // data

    /**
     * The observer reports state changes to own observers
     * Not owns
     */
    RPointerArray<MUsbVBusObserver> iObservers;

    /**
     * The observer observes property change
     */
    RProperty iVBus;

    };

#endif //  C_USBVBUSOBSERVER_H
