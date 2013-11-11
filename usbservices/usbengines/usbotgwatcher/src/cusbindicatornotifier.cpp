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

#include <usbuinotif.h>
#include <hb/hbcore/hbindicatorsymbian.h>

#include "cusbindicatornotifier.h"
#include "cusbstate.h"
#include "definitions.h"

#include "debug.h"
#include "panic.h"

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUsbIndicatorNotifier* CUsbIndicatorNotifier::NewL(
        CUsbNotifManager& aNotifManager, CUsbOtgWatcher& aOtgWatcher)
    {
    LOG_FUNC

    CUsbIndicatorNotifier* self = new (ELeave) CUsbIndicatorNotifier(
            aNotifManager, aOtgWatcher);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CUsbIndicatorNotifier::~CUsbIndicatorNotifier()
    {
    LOG_FUNC

    Close();

    if (iOtgWatcher.VBusObserver())
        {
        TRAP_IGNORE(iOtgWatcher.VBusObserver()->UnsubscribeL(*this));
        }

    // Unsubscribe from otg watcher states change notifications
    TRAP_IGNORE(iOtgWatcher.UnsubscribeL(*this));

    delete iUsbConnectingIndicator; 
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CUsbIndicatorNotifier::CUsbIndicatorNotifier(CUsbNotifManager& aNotifManager,
        CUsbOtgWatcher& aOtgWatcher) :
        iOtgWatcher(aOtgWatcher)
    {
    LOG_FUNC
    
    }

// ---------------------------------------------------------------------------
// Second-phase constructor
// ---------------------------------------------------------------------------
//
void CUsbIndicatorNotifier::ConstructL()
    {
    LOG_FUNC

    iUsbConnectingIndicator = CHbIndicatorSymbian::NewL();
    
    // Subscribe for VBus change notifications
    iOtgWatcher.VBusObserver()->SubscribeL(*this);

    // Subscribe for otg watcher states change notifications
    iOtgWatcher.SubscribeL(*this);

    iConnectingIndicatorOn = EFalse;

    // check here for condition to set usb indicator
    SetIndicator();
   
    }


// ---------------------------------------------------------------------------
// From CUsbNotifier
// ---------------------------------------------------------------------------
//
void CUsbIndicatorNotifier::Close()
    {
    LOG_FUNC

    ToggleConnectingIndicator( EFalse );
    }

// ---------------------------------------------------------------------------
// Set USB indicator On or Off
// ---------------------------------------------------------------------------
//
void CUsbIndicatorNotifier::ToggleConnectingIndicator(TBool aEnable)
    {
    LOG1( "toggle connecting indicator = %d" , aEnable);
    
    TBool success = ETrue;
    
    if (aEnable && !iConnectingIndicatorOn)
        {
        success = iUsbConnectingIndicator->Activate(KUsbConnectingIndicator);   
        LOG1( "calling CHbIndicatorSymbian::Activate(), returned %d", success);   
        if (success)
            {
            iConnectingIndicatorOn = ETrue;
            }  
                   
        }
   
    if (!aEnable && iConnectingIndicatorOn)
        {
        success = iUsbConnectingIndicator->Deactivate(KUsbConnectingIndicator); 
        if (success)
            {
            iConnectingIndicatorOn = EFalse;
            }  
        LOG1( "calling CHbIndicatorSymbian::Deactivate(), returned %d", success);                  
        }   
        
    // if success became false loading the indicator failed, log the error    
    if (!success)
        {
        TInt error = iUsbConnectingIndicator->Error();
        LOG1( "indicator error is %d", error);       
        }         
    }
       
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbIndicatorNotifier::OtgWatcherStateChangedL(TUsbStateIds aState)
    {
     LOG_FUNC
    SetIndicator();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbIndicatorNotifier::VBusDownL()
    {
     LOG_FUNC
    SetIndicator();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbIndicatorNotifier::VBusUpL()
    {
     LOG_FUNC
    SetIndicator();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CUsbIndicatorNotifier::VBusObserverErrorL(TInt aError)
    {
    LOG_FUNC
    // do nothing
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CUsbIndicatorNotifier::SetIndicator()
    {
    LOG_FUNC
    if (!(iOtgWatcher.IsDeviceA()) || iOtgWatcher.CurrentHostState()->Id() == EUsbStateHostAPeripheral)
        {
        // if B or peripheral, than other party (usbwatcher) takes care of usb indicator
        // in combined usbwatcher (if role swap allowed) one class has to manage usb indicator 
        return;
        }

    // if VBus up and we are not host -> show connecting indicator
    if ((iOtgWatcher.VBusObserver()->VBus() == CUsbVBusObserver::EVBusUp)
            && (iOtgWatcher.CurrentHostState()->Id() != EUsbStateHostAHost))
        {
        ToggleConnectingIndicator( ETrue );
        }
    else
    // Otherwise do not show indicator
        {
        ToggleConnectingIndicator( EFalse );
        }
    }

// End of file
