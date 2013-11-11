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

#include "cusbstatehostaperipheral.h"

#include "errors.h"
#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostAPeripheral::CUsbStateHostAPeripheral(CUsbOtgWatcher& aWatcher) :
    CUsbStateHostABase(aWatcher)
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostAPeripheral::ConstructL()
    {
    LOG_FUNC

    CUsbStateHostABase::ConstructL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostAPeripheral* CUsbStateHostAPeripheral::NewL(
        CUsbOtgWatcher& aWatcher)
    {
    LOG_FUNC

    CUsbStateHostAPeripheral* self = new (ELeave) CUsbStateHostAPeripheral(
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
CUsbStateHostAPeripheral::~CUsbStateHostAPeripheral()
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUsbStateIds CUsbStateHostAPeripheral::Id()
    {
    return EUsbStateHostAPeripheral;
    }

// From OTG state observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostAPeripheral::AHostL()
    {
    LOG_FUNC
    ChangeHostStateL( EUsbStateHostAInitiate);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CUsbStateHostAPeripheral::AIdleL()
    {
    LOG_FUNC
    ChangeHostStateL( EUsbStateHostAInitiate);
    }
