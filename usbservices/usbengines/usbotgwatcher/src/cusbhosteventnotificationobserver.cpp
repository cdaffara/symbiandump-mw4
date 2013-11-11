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

#include "cusbhosteventnotificationobserver.h"

#include "definitions.h"
#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbHostEventNotificationObserver::CUsbHostEventNotificationObserver(
        RUsb* aUsb) :
    CActive(EPriorityStandard), iUsb(aUsb)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbHostEventNotificationObserver::ConstructL()
    {
    LOG_FUNC

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbHostEventNotificationObserver* CUsbHostEventNotificationObserver::NewL(
        RUsb* aUsb)
    {
    LOG_FUNC

    CUsbHostEventNotificationObserver* self =
            new (ELeave) CUsbHostEventNotificationObserver(aUsb);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbHostEventNotificationObserver::~CUsbHostEventNotificationObserver()
    {
    LOG_FUNC

    Cancel();

    iObservers.Close();

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbHostEventNotificationObserver::SubscribeL(
        MUsbHostEventNotificationObserver& aObserver)
    {
    LOG_FUNC

    // check if the same observer already exist in a list
    if (KErrNotFound != iObservers.Find(&aObserver))
        {
        LOG("Observer already exists" );
        PANIC( EObserverAlreadyExists);
        return;
        }

    iObservers.AppendL(&aObserver);

    if (KFirst == iObservers.Count()) // first item
        {
        iUsb->HostEventNotification(iStatus, iEventInfo);
        SetActive();

        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbHostEventNotificationObserver::UnsubscribeL(
        MUsbHostEventNotificationObserver& aObserver)
    {
    LOG_FUNC

    TInt i(iObservers.Find(&aObserver));
    if (KErrNotFound == i)
        {
        LOG("Observer not found");
        PANIC( ECanNotFindHostEventNotificationObserver);
        return;
        }

    iObservers.Remove(i);

    if (0 == iObservers.Count()) // no observers anymore
        {
        // cancel pending request
        Cancel();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CUsbHostEventNotificationObserver::RunL()
    {
    LOG_FUNC

    LOG1( "iStatus = %d", iStatus.Int());

    // if error occured, tell to Observers
    if (KErrNone != iStatus.Int())
        {
        for (TInt i(0); i < iObservers.Count(); ++i)
            {
            iObservers[i]->HostEventNotificationErrorL(iStatus.Int());
            }
        return;
        }

    TDeviceEventInformation dei(iEventInfo);

    // re-issue request first
    iUsb->HostEventNotification(iStatus, iEventInfo);
    SetActive();

    // Log the event
    LOG1( "iEventInfo.iDeviceId         = %d" , dei.iDeviceId);
    LOG1( "iEventInfo.iEventType        = %d" , dei.iEventType);
    LOG1( "iEventInfo.iError            = %d" , dei.iError);
    LOG1( "iEventInfo.iDriverLoadStatus = %d" , dei.iDriverLoadStatus);
    LOG1( "iEventInfo.iVid              = %d" , dei.iVid);
    LOG1( "iEventInfo.iPid              = %d" , dei.iPid);

    // then process property change
    switch (dei.iEventType)
        {
        case EDeviceAttachment:
            {
            LOG("DeviceAttachment" );

            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->DeviceAttachedL(dei);
                }
            break;
            }

        case EDeviceDetachment:
            {
            LOG( "DeviceDetachment" );

            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->DeviceDetachedL(dei);
                }
            break;
            }

        case EDriverLoad:
            {
            switch (dei.iDriverLoadStatus)
                {
                case EDriverLoadSuccess:
                    {
                    LOG( "DriverLoadSuccess" );

                    for (TInt i(0); i < iObservers.Count(); ++i)
                        {
                        iObservers[i]->DriverLoadSuccessL(dei);
                        }

                    break;
                    }
                case EDriverLoadPartialSuccess:
                    {
                    LOG( "DriverLoadPartialSuccess" );

                    for (TInt i(0); i < iObservers.Count(); ++i)
                        {
                        iObservers[i]->DriverLoadPartialSuccessL(dei);
                        }

                    break;
                    }
                case EDriverLoadFailure:
                    {
                    LOG( "DriverLoadFailure");

                    for (TInt i(0); i < iObservers.Count(); ++i)
                        {
                        iObservers[i]->DriverLoadFailureL(dei);
                        }
                    break;
                    }
                default:
                    {
                    LOG("WrongDriverLoadStatus" );
                    PANIC( EWrongDriverLoadStatus);
                    }
                }
            break;

            }
        default:
            {
            LOG( "WrongHostEventNotification" );
            PANIC( EWrongHostEventNotification);
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbHostEventNotificationObserver::DoCancel()
    {
    iUsb->HostEventNotificationCancel();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CUsbHostEventNotificationObserver::RunError(TInt aError)
    {
    LOG_FUNC

    LOG1( "aError = %d" , aError);

    // try to recover and continue	
    return KErrNone;

    }

