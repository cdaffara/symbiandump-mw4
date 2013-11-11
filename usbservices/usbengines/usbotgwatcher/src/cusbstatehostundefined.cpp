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

#include "cusbotgwatcher.h"
#include "cusbstatehostundefined.h"

#include "definitions.h"
#include "errors.h"

#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostUndefined::CUsbStateHostUndefined(CUsbOtgWatcher& aWatcher) :
    CUsbStateHostABase(aWatcher)
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostUndefined* CUsbStateHostUndefined::NewL(CUsbOtgWatcher& aWatcher)
    {
    LOG_FUNC

    CUsbStateHostUndefined* self = new (ELeave) CUsbStateHostUndefined(
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
void CUsbStateHostUndefined::ConstructL()
    {
    LOG_FUNC

    CUsbStateHostABase::ConstructL();

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostUndefined::~CUsbStateHostUndefined()
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUsbStateIds CUsbStateHostUndefined::Id()
    {
    return EUsbStateHostUndefined;
    }

// From VBus observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostUndefined::VBusDownL()
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostUndefined::VBusUpL()
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostUndefined::VBusErrorL()
    {
    LOG_FUNC
    iWatcher.Usb().BusClearError();
    }

// From OTG state observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CUsbStateHostUndefined::AVBusErrorL()
    {
    LOG_FUNC
    iWatcher.Usb().BusClearError();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostUndefined::BIdleL()
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostUndefined::BPeripheralL()
    {
    LOG_FUNC
    }

// From host state observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostUndefined::DeviceAttachedL(TDeviceEventInformation)
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostUndefined::DeviceDetachedL(TDeviceEventInformation)
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostUndefined::DriverLoadSuccessL(TDeviceEventInformation)
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostUndefined::DriverLoadPartialSuccessL(TDeviceEventInformation)
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostUndefined::DriverLoadFailureL(TDeviceEventInformation)
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostUndefined::SrpReceivedL()
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostUndefined::SessionRequestedL()
    {
    LOG_FUNC
    }

// From message notification observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostUndefined::MessageNotificationReceivedL(TInt aMessage)
    {
    LOG_FUNC
    LOG1( "Unhandled message aMessage = %d" , aMessage);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostUndefined::BadHubPositionL()
    {
    LOG_FUNC
    PANIC( EBadHubPositionNotExpected);
    }
