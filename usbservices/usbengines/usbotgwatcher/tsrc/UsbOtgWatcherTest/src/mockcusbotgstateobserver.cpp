/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include <e32base.h>
#include <NormalHardcodedAssert.h>

#include "stifassertmacros.h"

#include "mockcusbotgstateobserver.h"

#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CUsbOtgStateObserver::CUsbOtgStateObserver()
    {
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CUsbOtgStateObserver::ConstructL()
    {
    }

CUsbOtgStateObserver* CUsbOtgStateObserver::NewL()
    {
    LOG(("[USBOTGWATCHER]\tMockCUsbOtgStateObserver::NewL" ));

    CUsbOtgStateObserver* self = new( ELeave ) CUsbOtgStateObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CUsbOtgStateObserver::~CUsbOtgStateObserver()
    {
    LOG(("[USBOTGWATCHER]\tMockCUsbOtgStateObserver::~CUsbOtgStateObserver" ));
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
TUsbOtgState CUsbOtgStateObserver::OtgState()
    {
    return (TUsbOtgState)iOtgState;
    }

void CUsbOtgStateObserver::SubscribeL(MUsbOtgStateObserver& aObserver)
    {
    LOG(("[USBOTGWATCHER]\tMockCUsbOtgStateObserver::SubscribeL"));
    STIF_ASSERT_NOT_NULL(&aObserver);

    iObserver = &aObserver;
    }

void CUsbOtgStateObserver::UnsubscribeL(MUsbOtgStateObserver& aObserver)
    {
    LOG(("[USBOTGWATCHER]\tMockCUsbOtgStateObserver::UnsubscribeL"));
    STIF_ASSERT_NOT_NULL(&aObserver);

    iObserver = NULL;
    }

//ignore EUsbOtgStateReset

void CUsbOtgStateObserver::TriggerAIdleL()
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    //EUsbOtgStateAIdle
    iObserver->AIdleL();
    }

void CUsbOtgStateObserver::TriggerAHostL()
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    //EUsbOtgStateAHost
    iObserver->AHostL();
    }

void CUsbOtgStateObserver::TriggerAPeripheralL()
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    //EUsbOtgStateAPeripheral
    iObserver->APeripheralL();
    }

void CUsbOtgStateObserver::TriggerAVBusErrorL()
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    //EUsbOtgStateAVbusError
    iObserver->AVBusErrorL();
    }

void CUsbOtgStateObserver::TriggerBIdleL()
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    //EUsbOtgStateBIdle
    iObserver->BIdleL();
    }

void CUsbOtgStateObserver::TriggerBPeripheralL()
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    //EUsbOtgStateBPeripheral
    iObserver->BPeripheralL();
    }

void CUsbOtgStateObserver::TriggerBHostL()
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    //EUsbOtgStateBHost
    iObserver->BHostL();
    }
