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
#include <UsbWatcherInternalPSKeys.h>
#include <e32property.h>

#include "cusbstatehostahost.h"
#include "cusbnotifmanager.h"

#include "definitions.h"
#include "errors.h"
#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostAHost::CUsbStateHostAHost(CUsbOtgWatcher& aWatcher) :
    CUsbStateHostABase(aWatcher)
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostAHost::ConstructL()
    {
    LOG_FUNC

    CUsbStateHostABase::ConstructL();

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostAHost* CUsbStateHostAHost::NewL(CUsbOtgWatcher& aWatcher)
    {
    LOG_FUNC

    CUsbStateHostAHost* self = new (ELeave) CUsbStateHostAHost(aWatcher);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostAHost::~CUsbStateHostAHost()
    {
    LOG_FUNC

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUsbStateIds CUsbStateHostAHost::Id()
    {
    return EUsbStateHostAHost;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostAHost::JustAdvancedToThisStateL()
    {
    LOG_FUNC

    // do general things 
    CUsbStateHostABase::JustAdvancedToThisStateL();

    LEAVEIFERROR(RProperty::Set(KPSUidUsbWatcher,
            KUsbWatcherIsPeripheralConnected,
            KUsbWatcherPeripheralIsConnected));

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostAHost::JustBeforeLeavingThisStateL()
    {
    LOG_FUNC

    LEAVEIFERROR(RProperty::Set(KPSUidUsbWatcher,
            KUsbWatcherIsPeripheralConnected,
            KUsbWatcherPeripheralIsNotConnected));

    // do general things 
    CUsbStateHostABase::JustBeforeLeavingThisStateL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostAHost::DeviceDetachedL(TDeviceEventInformation)
    {
    LOG_FUNC
    ChangeHostStateL( EUsbStateHostAInitiate);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostAHost::BadHubPositionL()
    {
    LOG_FUNC
    PANIC( EBadHubPositionEventNotExpected);
    }
