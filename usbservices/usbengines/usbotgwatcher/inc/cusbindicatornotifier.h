/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Blink/show Usb indicator notifier implementation
 *
 */

#ifndef C_USBINDICATORNOTIFIER_H
#define C_USBINDICATORNOTIFIER_H

#include <e32base.h>

#include "cusbnotifier.h"
#include "cusbvbusobserver.h"
#include "cusbotgwatcher.h"

class CHbIndicatorSymbian;

_LIT(KUsbConnectingIndicator, "com.nokia.hb.indicator.usb.device.inprogress/1.0");  

// indicator states
enum  
    {
    EIndicatorStateOff,
    EIndicatorStateOn,
    EIndicatorConnecting
    };
  
/**
 * Class implements functionality of showing/blinking usb indicator
 * Class does not provide method to get response from the user
 */
NONSHARABLE_CLASS( CUsbIndicatorNotifier ): public MUsbOtgWatcherStateObserver, MUsbVBusObserver
    {

    friend class CtUsbOtgWatcher;
    
public:
    /**
     * Two-phased constructor.
     * @param aNotifManager parent
     * @param aOtgWatcher provides VBus observer and usb otg watcher state notifications
     * @return Pointer to the new instance of CUsbIndicatorNotifier
     */
    static CUsbIndicatorNotifier* NewL(CUsbNotifManager& aNotifManager, CUsbOtgWatcher& aOtgWatcher);

    /**
     * Destructor.
     */
    virtual ~CUsbIndicatorNotifier();

    // From MUsbOtgWatcherStateObserver
    /**
     * Otg Watcher state changed call back
     * @param aState new state
     */
    virtual void OtgWatcherStateChangedL(TUsbStateIds aState);

    // From MUsbVBusObserver
    /**
     * VBus down event received
     */
    virtual void VBusDownL();
    /**
     * VBus up event received
     */
    virtual void VBusUpL();
    /**
     * error handler
     * @param aError error code
     */
    virtual void VBusObserverErrorL(TInt aError);

    /**
     * Stop showing notifier
     */
    virtual void Close();

private:

    /**
     * Default constructor.
     * @param aNotifManger parent
     * @param aOtgWatcher Otg watcher
     */
    CUsbIndicatorNotifier(CUsbNotifManager& aNotifManager, CUsbOtgWatcher& aOtgWatcher);

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Set USB Connecting indicator On or Off
     * @param aState Indicator states 
     */
    void ToggleConnectingIndicator(TBool aEnable);

    /**
     * Sets indicator accordingly
     */
    void SetIndicator();

private:
    // data

    /**
     * OtgWatcher
     * not own
     */
    CUsbOtgWatcher& iOtgWatcher;


    /**    
    * Pointer to a class for using Orbit indicator framework
    * Owned  
    */
    CHbIndicatorSymbian* iUsbConnectingIndicator;
    
    /**    
    * Whether we have activated the connecting indicator 
    */    
    TBool iConnectingIndicatorOn;
    };

#endif // C_USBINDICATORNOTIFIER_H
