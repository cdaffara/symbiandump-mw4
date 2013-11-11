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

#include <usbman.h>
#include <d32usbdi_errors.h>
#include <d32otgdi_errors.h>

#include "cusbmessagenotificationobserver.h"

#include "definitions.h" 
#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbMessageNotificationObserver::CUsbMessageNotificationObserver(RUsb* aUsb) :
    CActive(EPriorityStandard), iUsb(aUsb)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbMessageNotificationObserver::ConstructL()
    {
    LOG_FUNC

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbMessageNotificationObserver* CUsbMessageNotificationObserver::NewL(
        RUsb* aUsb)
    {
    LOG_FUNC

    CUsbMessageNotificationObserver* self =
            new (ELeave) CUsbMessageNotificationObserver(aUsb);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbMessageNotificationObserver::~CUsbMessageNotificationObserver()
    {
    LOG_FUNC

    Cancel();

    iObservers.Close();

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbMessageNotificationObserver::SubscribeL(
        MUsbMessageNotificationObserver& aObserver)
    {
    LOG_FUNC

    // check if the same observer already exist in a list
    if (KErrNotFound != iObservers.Find(&aObserver))
        {
        LOG("Observer already exists");
        PANIC( EObserverAlreadyExists);
        return;
        }
    iObservers.AppendL(&aObserver);

    if (KFirst == iObservers.Count()) // first item
        {
        iUsb->MessageNotification(iStatus, iMessage);
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbMessageNotificationObserver::UnsubscribeL(
        MUsbMessageNotificationObserver& aObserver)
    {
    LOG_FUNC

    TInt i(iObservers.Find(&aObserver));
    if (KErrNotFound == i)
        {
        LOG("Observer not found" )
        PANIC( ECanNotFindMessageNotificationObserver);
        return;
        }

    iObservers.Remove(i);

    if (0 == iObservers.Count()) // no items
        {
        // cancel pending request
        Cancel();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CUsbMessageNotificationObserver::RunL()
    {
    // Log the event
    LOG1( "Message notification observer iMessage = %d" , iMessage);

    // if error occured, tell to Observers
    if (KErrNone != iStatus.Int())
        {
        for (TInt i(0); i < iObservers.Count(); ++i)
            {
            iObservers[i]->MessageNotificationErrorL(iStatus.Int());
            }
        return;
        }

    TInt message(iMessage);

    // re-issue request first
    iUsb->MessageNotification(iStatus, iMessage);
    SetActive();

    // then process property change
    switch (message)
        {
        case KErrUsbBadHubPosition:
            {
            LOG("HubBadPosition" );

            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->BadHubPositionL();
                }
            break;
            }
        case KErrUsbOtgVbusError:
            {
            LOG( "VBusError");

            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->VBusErrorL();
                }
            break;
            }
        case KUsbMessageSrpReceived:
            {
            LOG("SRP received" );

            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->SrpReceivedL();
                }
            break;
            }
        case KUsbMessageRequestSession:
            {
            LOG("Session requested" );

            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->SessionRequestedL();
                }
            break;
            }
            // notify states with other messages  
        default:
            {

            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->MessageNotificationReceivedL(message);
                }
            break;
            }

        }

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbMessageNotificationObserver::DoCancel()
    {
    iUsb->MessageNotificationCancel();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CUsbMessageNotificationObserver::RunError(TInt aError)
    {
    LOG_FUNC

    LOG1( "aError = %d" , aError);

    // try to recover and continue	
    return KErrNone;

    }

