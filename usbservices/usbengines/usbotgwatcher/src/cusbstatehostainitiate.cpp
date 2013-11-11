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
#include <usbhosterrors.h>

#include "cusbotgwatcher.h"
#include "cusbstatehostainitiate.h"

#ifndef STIF
#include "cusbtimer.h"
#include "cusbnotifmanager.h"
#else
#include "mockcusbnotifmanager.h"
#include "mockcusbtimer.h"
#endif

#include "definitions.h"
#include "errors.h"

#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostAInitiate::CUsbStateHostAInitiate(CUsbOtgWatcher& aWatcher) :
    CUsbStateHostABase(aWatcher), iDeviceAttached(EFalse)
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostAInitiate::ConstructL()
    {
    LOG_FUNC

    CUsbStateHostABase::ConstructL();

    iAttachmentTimer = CUsbTimer::NewL(*this, EDeviceAttachmentTimer);

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostAInitiate* CUsbStateHostAInitiate::NewL(CUsbOtgWatcher& aWatcher)
    {
    LOG_FUNC

    CUsbStateHostAInitiate* self = new (ELeave) CUsbStateHostAInitiate(
            aWatcher);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostAInitiate::~CUsbStateHostAInitiate()
    {
    LOG_FUNC

    delete iAttachmentTimer;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUsbStateIds CUsbStateHostAInitiate::Id()
    {
    return EUsbStateHostAInitiate;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostAInitiate::JustBeforeLeavingThisStateL()
    {
    LOG_FUNC

    iAttachmentTimer->Cancel();

    // do general things 
    CUsbStateHostABase::JustBeforeLeavingThisStateL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostAInitiate::JustAdvancedToThisStateL()
    {
    LOG_FUNC

    // do general things 
    CUsbStateHostABase::JustAdvancedToThisStateL();

    // clear this attribute, and wait for attachment
    // one attchment has to correspond to only one detachment
    iDeviceAttached = EFalse; // to catch not corresponding Detachment, if any 

    TInt err = iWatcher.Usb().EnableFunctionDriverLoading();

    if (KErrNone != err)
        {
        LOG1( "EnableFunctionDriverLoading err = %d", err );
        iWatcher.HandleHostProblemL(EUsbWatcherCanNotEnableDriverLoading,
                EUsbStateHostHandleDropping);
        return;
        }

    // do BusRequest, if down

    if (CUsbVBusObserver::EVBusUp != iWatcher.VBusObserver()->VBus())
        {
        const TUint maxTrial = 3;
        TInt busReqErr(KErrGeneral);
        TUint count(0);

        while (count < maxTrial && KErrNone != busReqErr)
            {
            busReqErr = iWatcher.Usb().BusRequest();
            LOG1( "BusRequest() busReqErr = %d" , busReqErr);

            if (KErrNone != busReqErr)
                {
                err = iWatcher.Usb().BusClearError();
                if (KErrNone != err)
                    {
                    iWatcher.HandleHostProblemL(
                            EUsbWatcherCanNotClearBusError,
                            EUsbStateHostHandleDropping);
                    return;
                    }
                }
            ++count;
            }
        if (KErrNone != busReqErr)
            {
            iWatcher.HandleHostProblemL(EUsbWatcherCanNotRaiseVBus,
                    EUsbStateHostHandleDropping);
            return;
            }
        }

    // start timer, waiting for device attachment
    iAttachmentTimer->After(KTimeToWaitForDeviceAttachment);

    }

// From TimerObserver
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostAInitiate::TimerElapsedL(TUsbTimerId aTimerId)
    {
    LOG_FUNC

    switch (aTimerId)
        {
        case EDeviceAttachmentTimer:
            {
            LOG("AttachmentTimer" );
            HandleL(EUsbWatcherErrDandlingCable, EUsbStateHostHandleDropping);
            break;
            }
        default:
            {
            LOG1( "Unknown timer id = %d", aTimerId );
            PANIC( EWrongTimerId);
            }
        }
    }

// From VBus observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostAInitiate::VBusUpL()
    {
    LOG_FUNC
    }

// From Host Event notification observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostAInitiate::DeviceAttachedL(TDeviceEventInformation aTdi)
    {
    LOG_FUNC

    iDeviceAttached = ETrue;
    iAttachmentTimer->Cancel();

    // check if an OTG device connected
    TOtgDescriptor otgDescriptor;

    // ignore all the errors, assume we connected to not otg
    TInt err = iWatcher.Usb().GetOtgDescriptor(aTdi.iDeviceId, otgDescriptor);
    LOG1("GetOtgDescriptor() err = %d", err );

    TBool hnpSupported(otgDescriptor.iAttributes & EUsbOtgHNPSupported);
    TBool srpSupported(otgDescriptor.iAttributes & EUsbOtgSRPSupported);

    // OTG device supports both hnp and srp
    if (hnpSupported && srpSupported)
        {

        HandleL(EUsbWatcherConnectedToOTG, EUsbStateHostHandleDropping);
        return;
        }

    switch (aTdi.iError)
        // error in attachement
        {
        case KErrNone:
            {
            break;
            }
        case KErrUsbDeviceDetachedDuringDriverLoading:
            {
            //do not show error, due to device is already detached
            // Device Detachment will not come after this (it was not attached yet)
            // therefore emulate detachment
            // content of aTdi is not important, due to detachment anyway
            DeviceDetachedL(aTdi);
            break;
            }
        case KErrBadPower:
            {
            LOG( "TooMuchPower" );
            HandleL(EUsbWatcherErrDeviceRequiresTooMuchPowerOnEnumeration,
                    EUsbStateHostDelayNotAttachedHandle);
            break;
            }
        default:
            {
            LOG1("AttachmentError aTdi.iError = %d" , aTdi.iError );
            HandleL(EUsbWatcherErrUnsupportedDevice,
                    EUsbStateHostHandleDropping);
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostAInitiate::DeviceDetachedL(TDeviceEventInformation aTdi)
    {
    LOG_FUNC

    // iDeviceAttached is used here to avoid one more state creation (init->attached->detached)
    // due to purpose of this state only to wait for drivers load
    if (!iDeviceAttached)
        {
        PANIC(EDeviceDetachedNotExpected)
        }

    // go out, and go in to the same state
    ChangeHostStateL( EUsbStateHostAInitiate);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostAInitiate::DriverLoadSuccessL(TDeviceEventInformation)
    {
    LOG_FUNC
    ChangeHostStateL( EUsbStateHostAHost);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostAInitiate::DriverLoadPartialSuccessL(
        TDeviceEventInformation aDei)
    {
    LOG_FUNC

    switch (aDei.iError)
        {
        case KErrUsbDeviceDetachedDuringDriverLoading:
            {
            //do not show warning, due to device is already detached
            break;
            }
        default:
            {
            iWatcher.NotifManager()->ShowNotifierL(KUsbUiNotifOtgWarning,
                    EUsbOtgPartiallySupportedDevice, NULL);
            }
        }
    ChangeHostStateL( EUsbStateHostAHost);

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostAInitiate::DriverLoadFailureL(TDeviceEventInformation aDei)
    {
    LOG_FUNC

    HandleL(EUsbWatcherErrDriversNotFound, EUsbStateHostDelayAttachedHandle);
    }
