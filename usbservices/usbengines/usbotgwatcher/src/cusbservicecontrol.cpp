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

#include "cusbservicecontrol.h"
#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbServiceControl::CUsbServiceControl(MUsbServiceControlObserver& aObserver,
        RUsb& aUsb) :
    CActive(CActive::EPriorityStandard), iObserver(aObserver), iUsb(aUsb),
            iPersonalityId(0)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbServiceControl::~CUsbServiceControl()
    {
    LOG_FUNC

    Cancel();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbServiceControl::ConstructL()
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbServiceControl* CUsbServiceControl::NewL(
        MUsbServiceControlObserver& aObserver, RUsb& aUsb)
    {
    LOG_FUNC

    CUsbServiceControl* self = new (ELeave) CUsbServiceControl(aObserver,
            aUsb);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self); // pop self
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CUsbServiceControl::StartL(TInt aPersonalityId)
    {
    LOG_FUNC

    LOG1( "aPersonalityId = %d" , aPersonalityId);

    TUsbServiceState serviceState = EUsbServiceIdle;
    TInt err = iUsb.GetServiceState(serviceState);

    if (KErrNone != err)
        {
        return err;
        }

    switch (serviceState)
        {
        case EUsbServiceIdle:
            {
            LOG( "UsbServiceState == EUsbServiceIdle" );

            iPersonalityId = aPersonalityId; // when request completed, this will indicate that we started what we wanted
            if (IsActive())
                {
                LOG("Request is outstanding, cancelling first" );
                Cancel();
                }
            iUsb.TryStart(aPersonalityId, iStatus);
            SetActive();
            break;
            }

        case EUsbServiceStarted:
            {
            LOG("UsbServiceState == EUsbServiceStarted");

            TInt currentPersonality(0);
            err = iUsb.GetCurrentPersonalityId(currentPersonality);
            if (KErrNone != err)
                {
                LOG1( "Error getting current personality err = %d" , err);
                return err;
                }

            if (aPersonalityId == currentPersonality) // already started
                {
                LOG("Personality already started" );
                iObserver.UsbServiceControlReqCompletedL(KErrInUse);
                return KErrNone;
                }

            // we need to stop current personality and start service with new personailty id
            iPersonalityId = aPersonalityId; // this will indicate that we want to start this personality after

            StopL();

            // start new personality in RunL() when state is Idle

            break;
            }
        case EUsbServiceStarting:
            {
            LOG("UsbServiceState == EUsbServiceStarting" );
            // do exactly the same as in case of EUsbServiceStopping;
            // break statement is not required here
            }
        case EUsbServiceStopping:
            {
            LOG( "UsbServiceState == EUsbServiceStopping" );

            // subscribe for usb service state change, and start new usb service once current one started/stopped
            iPersonalityId = aPersonalityId; // this will indicate that we want to start this personality

            // this check is needed due to usb service might be stopping by other client  
            if (IsActive())
                {
                LOG( "Request outstanding. Waiting for completion" );
                return KErrNone; // when the outstanding request get completed, we start usb services with iPersonalityId
                }

            // otherwise, monitor usb service state
            iUsb.ServiceStateNotification(iServiceState, iStatus);
            SetActive();

            break;
            }
        case EUsbServiceFatalError:
            {
            LOG( "UsbServiceState == EUsbServiceFatalError" );
            return KErrGeneral;
            }
        default:
            {
            PANIC( EUnknownUsbServiceState);
            }
        }

    return KErrNone;

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CUsbServiceControl::StopL()
    {
    LOG_FUNC

    TUsbServiceState serviceState = EUsbServiceIdle;
    TInt err = iUsb.GetServiceState(serviceState);

    LOG2("err = %d; serviceState = %d" , err, serviceState);

    if (KErrNone != err)
        {
        return err;
        }

    switch (serviceState)
        {
        case EUsbServiceIdle:
            {
            LOG("UsbServiceState == EUsbServiceIdle" );

            return KErrNone;
            }

        case EUsbServiceStarted:
            {
            LOG("UsbServiceState == EUsbServiceStarted" );

            if (IsActive())
                {
                LOG("Request is outstanding, cancelling it" );
                Cancel();
                }
            iUsb.TryStop(iStatus);
            SetActive();

            break;
            }
        case EUsbServiceStopping:
            {
            LOG( "UsbServiceState == EUsbServiceStopping" );
            // do exactly the same as in case of EUsbServiceStarting;
            // break statement is not required here
            }
        case EUsbServiceStarting:
            {
            LOG("UsbServiceState == EUsbServiceStarting" );

            // subscribe for usb service state change, and stop usb service once current one started
            iPersonalityId = 0; // this will indicate that we do not want to start this personality

            // this check is needed due to usb service might be starting by other client  
            if (IsActive())
                {
                LOG("Request is outstanding, waiting for completion" );
                return KErrNone; // when this request get completed, we request to stop usb services
                }

            // otherwise, monitor usb service state
            iUsb.ServiceStateNotification(iServiceState, iStatus);
            SetActive();

            break;
            }
        case EUsbServiceFatalError:
            {
            LOG("UsbServiceState == EUsbServiceFatalError" );
            return KErrGeneral;
            }
        default:
            {
            PANIC( EUnknownUsbServiceState);
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbServiceControl::RunL()
    {
    LOG_FUNC

    LOG1("iStatus = %d" , iStatus.Int());

    if (KErrNone != iStatus.Int())
        {
        iObserver.UsbServiceControlReqCompletedL(iStatus.Int());
        return;
        }

    TUsbServiceState serviceState = EUsbServiceIdle;
    TInt err = iUsb.GetServiceState(serviceState);

    if (KErrNone != err)
        {
        LOG1("Error while getting service state %d" , err);
        iObserver.UsbServiceControlReqCompletedL(err);
        return;
        }

    switch (serviceState)
        {
        case EUsbServiceIdle: // usb service stopped
            {
            LOG("UsbServiceState == EUsbServiceIdle" );

            if (iPersonalityId != 0) // during service stopping, requested to start it
                {
                LOG1("Requested to start personality %d. Starting it." , iPersonalityId);
                TInt personalityId = iPersonalityId;
                iPersonalityId = 0; // reset
                err = StartL(personalityId);
                if (KErrNone != err)
                    {
                    iObserver.UsbServiceControlReqCompletedL(err);
                    }
                return;
                }

            // otherwise, we've done, notify
            iObserver.UsbServiceControlReqCompletedL(KErrNone);
            break;
            }

        case EUsbServiceStarted:
            {
            LOG("UsbServiceState == EUsbServiceStarted" );

            TInt currentPersonality(0);
            err = iUsb.GetCurrentPersonalityId(currentPersonality);
            if (KErrNone != err)
                {
                LOG1("Error while getting PersonalityId err = %d" , err);
                iObserver.UsbServiceControlReqCompletedL(err);
                return;
                }

            if (iPersonalityId == currentPersonality) // already done
                {
                LOG("Personality already started" );
                iPersonalityId = 0;
                iObserver.UsbServiceControlReqCompletedL(KErrNone);
                return;
                }

            if (iPersonalityId == 0) // during service start requested to stop it
                {
                LOG("Requested to stop personality. Stopping." );
                err = StopL();
                if (KErrNone != err)
                    {
                    LOG1("Error while stopping personality err = %d" , err);
                    iObserver.UsbServiceControlReqCompletedL(err);
                    }
                return;
                }

            // otherwise, during service start, requested to start it with another personality
            LOG1( "Requested to start personality %d. Starting it.", iPersonalityId);
            TInt personalityId = iPersonalityId;
            iPersonalityId = 0; // reset
            err = StartL(personalityId);
            if (KErrNone != err)
                {
                LOG1("Error while starting personality err = %d" , err);
                iObserver.UsbServiceControlReqCompletedL(err);
                }
            break;
            }

        case EUsbServiceStarting:
            {
            // do exactly same as in EUsbServiceStopping
            // break statement is not required here
            LOG("State == EUsbServiceStarting. Continue." );
            }
        case EUsbServiceStopping:
            {
            // we are not interested in these states, just continue monitoring
            LOG("State == EUsbServiceStopping. Continue." );
            iUsb.ServiceStateNotification(iServiceState, iStatus);
            SetActive();
            break;
            }
        case EUsbServiceFatalError:
            {
            LOG("UsbServiceState == EUsbServiceFatalError" );
            iObserver.UsbServiceControlReqCompletedL(KErrGeneral);
            break;
            }
        default:
            {
            PANIC( EUnknownUsbServiceState);
            }
        }

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CUsbServiceControl::RunError(TInt aError)
    {
    LOG_FUNC
    LOG1("aError = %d", aError );
    TRAP_IGNORE(iObserver.UsbServiceControlReqCompletedL(aError));

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbServiceControl::DoCancel()
    {
    LOG( "Cancelling interest to Usb TryStart.")
    iUsb.CancelInterest(RUsb::ETryStart);
    LOG( "Cancelling interest to Usb TrySop.")
    iUsb.CancelInterest(RUsb::ETryStop);
    LOG("Cancelling interest to usb states notifications.")
    iUsb.ServiceStateNotificationCancel();
    }
