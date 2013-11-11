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
 * Description:  Implementation
 *
 */

#include <usbuinotif.h>
#include <d32usbdi_errors.h>

#include "cusbotgwatcher.h"
#include "cusbstatehosthandledropping.h"

#ifndef STIF
#include "cusbnotifmanager.h"
#else
#include "mockcusbnotifmanager.h"
#endif

#include "errors.h"
#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostHandleDropping::CUsbStateHostHandleDropping(
        CUsbOtgWatcher& aWatcher) :
    CUsbStateHostHandle(aWatcher)
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostHandleDropping* CUsbStateHostHandleDropping::NewL(
        CUsbOtgWatcher& aWatcher)
    {
    LOG_FUNC

    CUsbStateHostHandleDropping* self =
            new (ELeave) CUsbStateHostHandleDropping(aWatcher);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandleDropping::ConstructL()
    {
    LOG_FUNC
    CUsbStateHostHandle::ConstructL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostHandleDropping::~CUsbStateHostHandleDropping()
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUsbStateIds CUsbStateHostHandleDropping::Id()
    {
    return EUsbStateHostHandleDropping;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandleDropping::JustBeforeLeavingThisStateL()
    {
    LOG_FUNC

    iWatcher.NotifManager()->CloseAllNotifiers();

    // do general things 
    CUsbStateHostHandle::JustBeforeLeavingThisStateL();
    }

// this means only one wait notifier at a time can be shown by this state
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandleDropping::WaitNotifierCompletedL(TInt /*aFeedback*/)
    {
    LOG_FUNC

    switch (iWhat)
        {
        case EUsbWatcherHubsNotSupported:
            {
            LOG( "EUsbWatcherHubsNotSupported" );
            ChangeHostStateL( EUsbStateHostAInitiate);
            break;
            }
        case EUsbWatcherErrDeviceRequiresTooMuchPower:
            {
            LOG( "EUsbWatcherErrDeviceRequiresTooMuchPower" );
            ChangeHostStateL( EUsbStateHostAInitiate);
            break;
            }
        case EUsbWatcherErrUnsupportedDevice:
            {
            LOG("EUsbWatcherErrUnsupportedDevice" );
            ChangeHostStateL( EUsbStateHostAInitiate);
            break;
            }
        case EUsbWatcherConnectedToOTG:
            {
            LOG("EUsbWatcherConnectedToOTG" );
            ChangeHostStateL( EUsbStateHostAInitiate);
            break;
            }
        case EUsbWatcherErrDandlingCable:
            {
            LOG("EUsbWatcherErrDandlingCable" );
            ChangeHostStateL( EUsbStateHostAInitiate);
            break;
            }
        case EUsbWatcherNoActivity:
            {
            LOG("EUsbWatcherNoActivity" );
            ChangeHostStateL( EUsbStateHostAInitiate);
            break;
            }
        case EUsbWatcherErrorInConnection:
            {
            LOG( "EUsbWatcherErrorInConnection" );
            ChangeHostStateL( EUsbStateHostAInitiate);
            break;
            }

            // fatal errors
        case EUsbWatcherCanNotStartUsbServices:
            {
            LOG( "EUsbWatcherCanNotStartUsbServices" );

            // no break here                     
            }

            // no break statement for following cases 
        case EUsbWatcherNoMemory:
        case EUsbWatcherIdPinError:
        case EUsbWatcherVBusObserverError:
        case EUsbWatcherHostEventNotificationError:
        case EUsbWatcherOtgStateError:
        case EUsbWatcherMessageNotificationError:
        case EUsbWatcherCanNotStartSession:
        case EUsbWatcherCanNotEnableDriverLoading:
        case EUsbWatcherCanNotClearBusError:
        case EUsbWatcherCanNotRaiseVBus:
            {
            LOG("Fatal error" );
            ChangeHostStateL( EUsbStateHostUndefined);
            break;
            }
        default:
            {
            LOG1("Unexpected situation to be handled iWhat = %d" , iWhat );
            PANIC( EUnexpectedSituationToHandle);
            break;
            }

        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandleDropping::DoHandleL()
    {
    LOG_FUNC

    LOG1( "iWhat = %d" , iWhat);

    // Drop VBus first    
    // Ignore any errors when calling BusDrop(). Those indicate that VBus already dropped 
    iWatcher.Usb().BusDrop();

    switch (iWhat)
        {

        case EUsbWatcherHubsNotSupported:
            {
            LOG("EUsbWatcherHubsNotSupported" );
            iWatcher.NotifManager()->ShowNotifierL(KUsbUiNotifOtgError,
                    EUsbOtgHubUnsupported, this);
            break;
            }
        case EUsbWatcherErrDeviceRequiresTooMuchPower:
            {
            LOG("EUsbWatcherErrDeviceRequiresTooMuchPower" );
            iWatcher.NotifManager()->ShowNotifierL(KUsbUiNotifOtgError,
                    EUsbOtgTooMuchPower, this);
            break;
            }
        case EUsbWatcherErrUnsupportedDevice:
            {
            LOG("EUsbWatcherErrUnsupportedDevice" );
            iWatcher.NotifManager()->ShowNotifierL(KUsbUiNotifOtgError,
                    EUsbOtgUnsupportedDevice, this);
            break;
            }
        case EUsbWatcherConnectedToOTG:
            {
            LOG("EUsbWatcherConnectedToOTG" );
            iWatcher.NotifManager()->ShowNotifierL(KUsbUiNotifOtgError,
                    EUsbOtgUnsupportedDevice, this);

            break;
            }
        case EUsbWatcherErrDandlingCable:
            {
            LOG( "EUsbWatcherErrDandlingCable" );
            iWatcher.NotifManager()->ShowNotifierL(KUsbUiNotifOtgError,
                    EUsbOtgErrorAttachTimedOut, this);
            break;
            }
        case EUsbWatcherNoActivity:
            {
            LOG( "EUsbWatcherNoActivity" );
            iWatcher.NotifManager()->ShowNotifierL(KUsbUiNotifOtgError,
                    EUsbOtgUnsupportedDevice, this);

            break;
            }
        case EUsbWatcherErrorInConnection:
            {
            LOG("EUsbWatcherErrorInConnection" );
            iWatcher.NotifManager()->ShowNotifierL(KUsbUiNotifOtgError,
                    EUsbOtgErrorInConnection, this);
            break;
            }
        case EUsbWatcherNoMemory:
            {
            LOG("EUsbWatcherNoMemory" );
            iWatcher.NotifManager()->ShowNotifierL(KUsbUiNotifOtgError,
                    EUsbOtgErrorNoMemory, this);
            break;
            }
        case EUsbWatcherCanNotStartUsbServices:
            {
            LOG("EUsbWatcherCanNotStartUsbServices" );
            // no break here
            }
            // no break in following cases
        case EUsbWatcherIdPinError:
        case EUsbWatcherVBusObserverError:
        case EUsbWatcherHostEventNotificationError:
        case EUsbWatcherOtgStateError:
        case EUsbWatcherMessageNotificationError:
        case EUsbWatcherCanNotStartSession:
        case EUsbWatcherCanNotEnableDriverLoading:
        case EUsbWatcherCanNotClearBusError:
        case EUsbWatcherCanNotRaiseVBus:
            {
            LOG1( "Fatal error iWhat = %d", iWhat );
            iWatcher.NotifManager()->ShowNotifierL(KUsbUiNotifOtgError,
                    EUsbOtgErrorInConnection, this);
            break;
            }
        default:
            {
            LOG1("Unexpected situation to be handled iWhat = %d", iWhat );
            PANIC( EUnexpectedSituationToHandle);
            break;
            }
        }
    }

/////////////////////////////////////////////////////////////////////////////////////
// From VBus observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandleDropping::VBusUpL()
    {
    LOG_FUNC
    ChangeHostStateL( EUsbStateHostAInitiate);
    }

// ---------------------------------------------------------------------------
// this is expected in the state, for example after calling BusDrop
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandleDropping::VBusDownL()
    {
    LOG_FUNC
    }

// From Host Event notification observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandleDropping::AVBusErrorL()
    {
    LOG_FUNC
    iWatcher.Usb().BusClearError();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandleDropping::DeviceAttachedL(
        TDeviceEventInformation aDevEventInfo)
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandleDropping::DeviceDetachedL(TDeviceEventInformation)
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandleDropping::DriverLoadSuccessL(TDeviceEventInformation)
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandleDropping::DriverLoadPartialSuccessL(
        TDeviceEventInformation)
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandleDropping::DriverLoadFailureL(TDeviceEventInformation)
    {
    LOG_FUNC
    }

// From message notification observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandleDropping::MessageNotificationReceivedL(TInt aMessage)
    {
    LOG_FUNC

    LOG1( "aMessage = %d", aMessage )

    switch (aMessage)
        {
        // USBDI
        case KEventUsbBadDeviceDetached:
            {
            LOG("BadDeviceDetached" );
            ChangeHostStateL( EUsbStateHostUndefined);
            break;
            }
        default:
            {
            LOG1( "Unhandled message aMessage = %d" , aMessage);
            break;
            }

        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandleDropping::VBusErrorL()
    {
    LOG_FUNC
    iWatcher.Usb().BusClearError();
    }
