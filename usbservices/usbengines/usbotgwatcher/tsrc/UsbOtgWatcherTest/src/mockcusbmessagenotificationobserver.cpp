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

#include "mockcusbmessagenotificationobserver.h"

#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CUsbMessageNotificationObserver::CUsbMessageNotificationObserver(RUsb* aUsb) :
    iUsb(aUsb)
    {
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CUsbMessageNotificationObserver::ConstructL()
    {
    }

CUsbMessageNotificationObserver* CUsbMessageNotificationObserver::NewL(
        RUsb* aUsb)
    {
    LOG(  ( "[USBOTGWATCHER]\tMockCUsbMessageNotificationObserver::NewL" ) );

    CUsbMessageNotificationObserver* self = new( ELeave ) CUsbMessageNotificationObserver(aUsb);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CUsbMessageNotificationObserver::~CUsbMessageNotificationObserver()
    {
    LOG(  ( "[USBOTGWATCHER]\tMockCUsbMessageNotificationObserver::~CUsbMessageNotificationObserver" ) );
    }

void CUsbMessageNotificationObserver::SubscribeL(
        MUsbMessageNotificationObserver& aObserver)
    {
    LOG( ("[USBOTGWATCHER]\tMockCUsbMessageNotificationObserver::SubscribeL"));
    STIF_ASSERT_NOT_NULL(&aObserver);

    iObserver = &aObserver;
    }

void CUsbMessageNotificationObserver::UnsubscribeL(
        MUsbMessageNotificationObserver& aObserver)
    {
    LOG( ("[USBOTGWATCHER]\tMockCUsbMessageNotificationObserver::UnsubscribeL"));
    STIF_ASSERT_NOT_NULL(&aObserver);

    iObserver = NULL;
    }

void CUsbMessageNotificationObserver::TriggerMessageNotificationReceivedL(
        TInt aMessage)
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    // default, others
    iObserver->MessageNotificationReceivedL(aMessage);
    }

void CUsbMessageNotificationObserver::TriggerBadHubPositionL()
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    //KErrUsbBadHubPosition
    iObserver->BadHubPositionL();
    }

void CUsbMessageNotificationObserver::TriggerVBusErrorL()
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    //KErrUsbOtgVbusError
    iObserver->VBusErrorL();
    }

void CUsbMessageNotificationObserver::TriggerSrpReceivedL()
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    //KUsbMessageSrpReceived
    iObserver->SrpReceivedL();
    }

void CUsbMessageNotificationObserver::TriggerSessionRequestedL()
    {
    STIF_ASSERT_NOT_NULL(iObserver);

    //KUsbMessageRequestSession
    iObserver->SessionRequestedL();
    }
