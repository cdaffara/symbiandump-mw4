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

#include <usbotgdefs.h>
#include <NormalHardcodedAssert.h>

#include "mockusbman.h"

#include "stifassertmacros.h"

#include "mockcusbvbusobserver.h"

#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CUsbVBusObserver::CUsbVBusObserver()
    {
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CUsbVBusObserver::ConstructL()
    {
    }
    
CUsbVBusObserver* CUsbVBusObserver::NewL()
	{
    LOG(("[USBOTGWATCHER]\tMockCUsbVBusObserver::NewL"));

    CUsbVBusObserver* self = new( ELeave ) CUsbVBusObserver();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;		
	}
		
// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CUsbVBusObserver::~CUsbVBusObserver()
    {
    LOG(("[USBOTGWATCHER]\tMockCUsbVBusObserver::~CUsbVBusObserver"));
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CUsbVBusObserver::TState CUsbVBusObserver::VBus() /* const! But for some reason RProperty::Get is not const! */
    {
    LOG(("[USBOTGWATCHER]\tMockCUsbVBusObserver::VBus"));
    
    return (iUsb->GetVBusUp() ? EVBusUp : EVBusDown);
    //return (iVBusDown ? EVBusDown : EVBusUp);
    }

void CUsbVBusObserver::SubscribeL(MUsbVBusObserver& aObserver)
    {
    LOG(("[USBOTGWATCHER]\tMockCUsbVBusObserver::SubscribeL"));
    STIF_ASSERT_NOT_NULL(&aObserver);    
    iObserver = &aObserver;
    }

void CUsbVBusObserver::UnsubscribeL(MUsbVBusObserver& aObserver)
    {
    LOG(("[USBOTGWATCHER]\tMockCUsbVBusObserver::UnsubscribeL"));
    STIF_ASSERT_NOT_NULL(&aObserver);    
    iObserver = NULL;
    }

void CUsbVBusObserver::SetRUsb(RUsb* aRUsb)
    {
    iUsb = aRUsb;
    }

void CUsbVBusObserver::TriggerVBusUpL()
    {
    LOG(("[USBOTGWATCHER]\tMockCUsbVBusObserver::TriggerVBusUpL"));
    STIF_ASSERT_NOT_NULL(iObserver);
    //STIF_ASSERT_TRUE(iUsb->GetVBusUp() == EVBusUp);
    //iVBusDown = EFalse;
    iObserver->VBusUpL();
    }

void CUsbVBusObserver::TriggerVBusDownL()
    {
    LOG(("[USBOTGWATCHER]\tMockCUsbVBusObserver::TriggerVBusDownL"));
    //STIF_ASSERT_NOT_NULL(iObserver);
    //STIF_ASSERT_TRUE(iUsb->GetVBusUp() == EVBusDown);

    //iVBusDown = ETrue;
    iObserver->VBusDownL();
    }
