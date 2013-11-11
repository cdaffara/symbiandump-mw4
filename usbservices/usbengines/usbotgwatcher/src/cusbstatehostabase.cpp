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

#include <e32base.h> 
#include <usbhosterrors.h>
#include <d32otgdi_errors.h>
#include <d32usbdi_errors.h>

#include "cusbotgwatcher.h"
#include "cusbstatehostabase.h"
#include "cusbstatehosthandledropping.h"

#include "errors.h"

#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostABase::CUsbStateHostABase(CUsbOtgWatcher& aWatcher) :
    CUsbState(aWatcher)
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostABase::ConstructL()
    {
    LOG_FUNC
    CUsbState::ConstructL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostABase::~CUsbStateHostABase()
    {
    LOG_FUNC
    }

// From VBus observer
// ---------------------------------------------------------------------------
// VBus can be dropped by lower leyer in case of a critical problem
// ---------------------------------------------------------------------------
//
void CUsbStateHostABase::VBusDownL()
    {
    LOG_FUNC
    HandleL(EUsbWatcherErrorInConnection, EUsbStateHostHandleDropping);
    }

// From OTG state observer
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CUsbStateHostABase::AIdleL()
    {
    LOG_FUNC
    // do nothing
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostABase::AHostL()
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostABase::APeripheralL()
    {
    LOG_FUNC
    ChangeHostStateL( EUsbStateHostAPeripheral);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostABase::AVBusErrorL()
    {
    LOG_FUNC
    // No need to handle BusClearError error code, due to Bus will be dropped anyway
    iWatcher.Usb().BusClearError();
    HandleL(EUsbWatcherErrDeviceRequiresTooMuchPower,
            EUsbStateHostHandleDropping);
    }

// From bus activity observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostABase::BusIdleL()
    {
    LOG_FUNC
    //do nothing
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostABase::BusActiveL()
    {
    LOG_FUNC
    // do nothing
    }

// From message notification observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostABase::MessageNotificationReceivedL(TInt aMessage)
    {
    LOG_FUNC
    LOG1( "aMessage = %d" , aMessage);

    switch (aMessage)
        {

        // OTGDI
        case KErrUsbOtgStackNotStarted://            = -6680;
        case KErrUsbOtgHnpNotResponding://           = -6683;
        case KErrUsbOtgBusControlProblem://          = -6685;
        case KErrUsbOtgVbusPowerUpError://           = -6686;
        case KErrUsbOtgHnpEnableProblem://           = -6687;
        case KErrUsbOtgVbusError://                  = -6690;

            // hosterrors.h
        case KErrUsbConfigurationHasNoInterfaces:
        case KErrUsbInterfaceCountMismatch:
        case KErrUsbDuplicateInterfaceNumbers:
        case KErrUsbBadDescriptor:
        case KErrUsbDeviceDetachedDuringDriverLoading:
        case KErrUsbAttachmentFailureGeneralError:
            {
            LOG( "ErrorInConnection" );
            HandleL(EUsbWatcherErrorInConnection, EUsbStateHostHandleDropping);
            break;
            }

            // OTGDI
        case KErrUsbOtgPeriphNotSupported: //        = -6688, for OPT only

            // USBDI
        case KErrUsbRequestsPending:
        case KErrUsbBadAddress:
        case KErrUsbNoAddress:
        case KErrUsbSetAddrFailed:
        case KErrUsbNoPower:
        case KErrUsbTooDeep:
        case KErrUsbIOError:
        case KErrUsbNotConfigured:
        case KErrUsbTimeout:
        case KErrUsbStalled:
        case KErrUsbTestFailure:
        case KErrUsbBadState:
        case KErrUsbDeviceSuspended:
        case KErrUsbBadDescriptorTopology:
        case KErrUsbDeviceRejected:
        case KErrUsbDeviceFailed:
        case KErrUsbBadDevice: // = -6656
        case KErrUsbBadHub:
        case KErrUsbEventOverflow:
        case KErrUsbBadDeviceAttached:

            // hosterrors.h
        case KErrUsbUnsupportedDevice:
            {
            LOG( "UnsupportedDevice" );
            HandleL(EUsbWatcherErrUnsupportedDevice,
                    EUsbStateHostHandleDropping);
            break;
            }
        default:
            {
            LOG1( "Unhandled message = %d" , aMessage);
            break;
            }

        }

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostABase::BadHubPositionL()
    {
    LOG_FUNC
    HandleL(EUsbWatcherHubsNotSupported, EUsbStateHostHandleDropping);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostABase::VBusErrorL()
    {
    LOG_FUNC
    // No error code handling after BusClearError call, due to bus will be dropped anyway
    iWatcher.Usb().BusClearError();
    HandleL(EUsbWatcherErrDeviceRequiresTooMuchPower,
            EUsbStateHostHandleDropping);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostABase::SrpReceivedL()
    {
    LOG_FUNC

    if (iWatcher.VBusObserver()->VBus() == CUsbVBusObserver::EVBusUp)
        {
        LOG( "Session is ongoing (VBus high); ignore SRP request." );
        return;
        }

    TInt err = iWatcher.Usb().BusRespondSrp();
    LOG1( "BusRespondSrp() error = %d" , err );

    if (KErrNone != err && KErrUsbOtgVbusAlreadyRaised != err)
        {
        iWatcher.HandleHostProblemL(EUsbWatcherErrorInConnection,
                EUsbStateHostHandleDropping);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostABase::SessionRequestedL()
    {
    LOG_FUNC

    if (iWatcher.VBusObserver()->VBus() == CUsbVBusObserver::EVBusUp)
        {
        // session already ongoing; BusRequest() in this case returns KErrUsbOtgBadState...
        LOG( "Session is ongoing (VBus high); ignore Session Request." );
        return;
        }

    TInt err = iWatcher.Usb().BusRequest();
    LOG1( "BusRequest() err = %d" , err);

    if (KErrNone != err && KErrUsbOtgVbusAlreadyRaised != err)
        {
        iWatcher.HandleHostProblemL(EUsbWatcherErrorInConnection,
                EUsbStateHostHandleDropping);
        }
    }
