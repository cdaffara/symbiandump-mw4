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

#include "stifassertmacros.h"
#include "mockcusbidpinobserver.h"

#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CUsbIdPinObserver::CUsbIdPinObserver() : iIdPinOn(EFalse)
    {
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CUsbIdPinObserver::ConstructL()
    {
    iWait = new ( ELeave )CActiveSchedulerWait();
    }
    
CUsbIdPinObserver* CUsbIdPinObserver::NewL()
    {
    LOG( "[USBOTGWATCHER]\tMockCUsbIdPinObserver::NewL" );
    
    CUsbIdPinObserver* self = new( ELeave ) CUsbIdPinObserver();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;        
    }
        
// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CUsbIdPinObserver::~CUsbIdPinObserver()
    {
    LOG( ( "[USBOTGWATCHER]\tMockCUsbIdPinObserver::~CUsbIdPinObserver" ) );
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CUsbIdPinObserver::TState CUsbIdPinObserver::IdPin() /* const! But for some reason RProperty::Get is not const! */
    {
    // not found in Symbian docs clear definition of this property. Verification is needed   
    return (0 == iIdPinOn? EIdPinOff : EIdPinOn);
    }
    
   
void CUsbIdPinObserver::SubscribeL(MUsbIdPinObserver& aObserver)
    {
    //iLog->Log(_L("[USBOTGWATCHER] >>MockCUsbIdPinObserver::SubscribeL"));
    LOG(("[USBOTGWATCHER]\tMockCUsbIdPinObserver::SubscribeL"));
    STIF_ASSERT_NOT_NULL(&aObserver);
    
    iObserver = &aObserver;

    //iLog->Log(_L("[USBOTGWATCHER] <<MockCUsbIdPinObserver::SubscribeL"));
    }

void CUsbIdPinObserver::UnsubscribeL(MUsbIdPinObserver& aObserver)
    {
    LOG(("[USBOTGWATCHER]\tMockCUsbIdPinObserver::UnsubscribeL"));
    STIF_ASSERT_NOT_NULL(&aObserver);    
    
    iObserver = NULL;
    }

void CUsbIdPinObserver::TriggerPinOnL()
    {
    LOG(("[USBOTGWATCHER]\tMockCUsbIdPinObserver::TriggerPinOn"));
    STIF_ASSERT_NOT_NULL(iObserver);

    iIdPinOn = 1;
    iObserver->IdPinOnL();
    LOG(("[USBOTGWATCHER] <<MockCUsbIdPinObserver::TriggerPinOn"));
    }

void CUsbIdPinObserver::TriggerPinOffL()
    {
    LOG(("[USBOTGWATCHER]\tMockCUsbIdPinObserver::TriggerPinOff"));
    STIF_ASSERT_NOT_NULL(iObserver);

    iIdPinOn = 0;
    iObserver->IdPinOffL();
    }
