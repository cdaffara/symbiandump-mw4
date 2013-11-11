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

#include "mockcusbhosteventnotificationobserver.h"

#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CUsbHostEventNotificationObserver::CUsbHostEventNotificationObserver(
        RUsb* aUsb) :
    iUsb(aUsb)
    {
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CUsbHostEventNotificationObserver::ConstructL()
    {
    }

CUsbHostEventNotificationObserver* CUsbHostEventNotificationObserver::NewL(
        RUsb* aUsb)
    {
    LOG(( "[USBOTGWATCHER]\tMockCUsbHostEventNotificationObserver::NewL" ) );

    CUsbHostEventNotificationObserver* self = new( ELeave ) CUsbHostEventNotificationObserver(aUsb);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CUsbHostEventNotificationObserver::~CUsbHostEventNotificationObserver()
    {
    LOG(( "[USBOTGWATCHER]\tMockCUsbHostEventNotificationObserver::~CUsbHostEventNotificationObserver" ) );
    }

void CUsbHostEventNotificationObserver::SubscribeL(
        MUsbHostEventNotificationObserver& aObserver)
    {
    LOG(("[USBOTGWATCHER]\tMockCUsbHostEventNotificationObserver::SubscribeL"));
    STIF_ASSERT_NOT_NULL(&aObserver);

    iObserver = &aObserver;
    }

void CUsbHostEventNotificationObserver::UnsubscribeL(
        MUsbHostEventNotificationObserver& aObserver)
    {
    LOG(("[USBOTGWATCHER]\tMockCUsbHostEventNotificationObserver::UnsubscribeL"));
    STIF_ASSERT_NOT_NULL(&aObserver);

    iObserver = NULL;
    }

void CUsbHostEventNotificationObserver::TriggerDeviceAttachedL(TDeviceEventInformation eventInfo)
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    //.iEventType = EDeviceAttachment
    iObserver->DeviceAttachedL(eventInfo);

    }

void CUsbHostEventNotificationObserver::TriggerDeviceDetachedL(TDeviceEventInformation eventInfo)
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    //.iEventType = EDeviceDetachment
    iObserver->DeviceDetachedL(eventInfo);
    }

void CUsbHostEventNotificationObserver::TriggerDriverLoadSuccessL(TDeviceEventInformation eventInfo)
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    //.iEventType = EDriverLoad, .iDriverLoadStatus = EDriverLoadSuccess
    iObserver->DriverLoadSuccessL(eventInfo);
    }

void CUsbHostEventNotificationObserver::TriggerDriverLoadPartialSuccessL(TDeviceEventInformation eventInfo)
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    //.iEventType = EDriverLoad, .iDriverLoadStatus = EDriverLoadPartialSuccess
    iObserver->DriverLoadPartialSuccessL(eventInfo);
    }

void CUsbHostEventNotificationObserver::TriggerDriverLoadFailureL(TDeviceEventInformation eventInfo)
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    //.iEventType = EDriverLoad, .iDriverLoadStatus = EDriverLoadFailure
    iObserver->DriverLoadFailureL(eventInfo);
    }

