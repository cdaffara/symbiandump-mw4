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

#include "mockcusbbusactivityobserver.h"

#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CUsbBusActivityObserver::CUsbBusActivityObserver() :
    iBusActivity(0)
    {
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CUsbBusActivityObserver::ConstructL()
    {
    }

CUsbBusActivityObserver* CUsbBusActivityObserver::NewL()
    {
    LOG(  "[USBOTGWATCHER]\tMockCUsbBusActivityObserver::NewL"  );

    CUsbBusActivityObserver* self = new( ELeave ) CUsbBusActivityObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CUsbBusActivityObserver::~CUsbBusActivityObserver()
    {
    LOG(  ( "[USBOTGWATCHER]\tMockCUsbBusActivityObserver::~CUsbBusActivityObserver" ) );
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CUsbBusActivityObserver::TBusActivity CUsbBusActivityObserver::BusActivity()
    {
    LOG1( "[USBOTGWATCHER]\tMockCUsbBusActivityObserver::State = %d", iBusActivity);

    return (0 == iBusActivity ? EBusActive : EBusIdle);
    }

void CUsbBusActivityObserver::SubscribeL(MUsbBusActivityObserver& aObserver)
    {
    LOG( ("[USBOTGWATCHER]\tMockCUsbBusActivityObserver::SubscribeL"));
    STIF_ASSERT_NOT_NULL(&aObserver);
    
    iObserver = &aObserver;
    }

void CUsbBusActivityObserver::UnsubscribeL(MUsbBusActivityObserver& aObserver)
    {
    LOG( ("[USBOTGWATCHER]\tMockCUsbBusActivityObserver::UnsubscribeL"));
    STIF_ASSERT_NOT_NULL(&aObserver);

    iObserver = NULL;
    }

void CUsbBusActivityObserver::TriggerBusIdleL()
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    iObserver->BusIdleL();
    }

void CUsbBusActivityObserver::TriggerBusActiveL()
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    iObserver->BusActiveL();
    }
