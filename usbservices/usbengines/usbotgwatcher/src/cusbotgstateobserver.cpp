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

#include "cusbotgstateobserver.h"

#include "definitions.h"
#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbOtgStateObserver::CUsbOtgStateObserver() :
    CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgStateObserver::ConstructL()
    {
    LOG_FUNC

    LEAVEIFERROR(iOtgState.Attach(KUidUsbManCategory,
            KUsbOtgStateProperty));

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbOtgStateObserver* CUsbOtgStateObserver::NewL()
    {
    LOG_FUNC

    CUsbOtgStateObserver* self = new (ELeave) CUsbOtgStateObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbOtgStateObserver::~CUsbOtgStateObserver()
    {
    LOG_FUNC

    Cancel();

    iOtgState.Close();

    iObservers.Close();

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUsbOtgState CUsbOtgStateObserver::OtgState()
    {
    TInt val(0);

    TInt err = iOtgState.Get(val);

    if (KErrNone != err)
        {
        LOG("CanNotGetOtgStateProperty" );
        PANIC( ECanNotGetOtgStateProperty);
        }

    return (TUsbOtgState) val;

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgStateObserver::SubscribeL(MUsbOtgStateObserver& aObserver)
    {
    LOG_FUNC

    // check if the same observer already exist in a list
    if (KErrNotFound != iObservers.Find(&aObserver))
        {
        LOG( "Observer already exists"  );
        PANIC( EObserverAlreadyExists);
        return;
        }
    iObservers.AppendL(&aObserver);

    if (KFirst == iObservers.Count()) // first item
        {
        iOtgState.Subscribe(iStatus);
        SetActive();

        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgStateObserver::UnsubscribeL(MUsbOtgStateObserver& aObserver)
    {
    LOG_FUNC

    TInt i(iObservers.Find(&aObserver));
    if (KErrNotFound == i)
        {
        LOG("Observer not found");
        PANIC( ECanNotFindOtgStateObserver);
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
void CUsbOtgStateObserver::RunL()
    {
    LOG_FUNC

    LOG1( "iStatus = %d" , iStatus.Int());

    // if error occured, tell to Observers
    if (KErrNone != iStatus.Int())
        {
        for (TInt i(0); i < iObservers.Count(); ++i)
            {
            iObservers[i]->OtgStateErrorL(iStatus.Int());
            }
        return;
        }

    // re-issue request first
    iOtgState.Subscribe(iStatus);
    SetActive();

    // then process property change
    TUsbOtgState state(OtgState());

    switch (state)
        {
        case EUsbOtgStateReset:
            {
            LOG("OTGState == RESET" );
            break;
            }
        case EUsbOtgStateAIdle:
            {
            LOG("OTGState == AIdle" );
            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->AIdleL();
                }
            break;
            }
        case EUsbOtgStateAHost:
            {
            LOG( "OTGState == AHost");
            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->AHostL();
                }
            break;
            }
        case EUsbOtgStateAPeripheral:
            {
            LOG("OTGState == APeripheral" );
            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->APeripheralL();
                }
            break;
            }
        case EUsbOtgStateAVbusError:
            {
            LOG("OTGState == AVBusError" );
            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->AVBusErrorL();
                }
            break;
            }
        case EUsbOtgStateBIdle:
            {
            LOG( "OTGState == BIdle" );
            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->BIdleL();
                }
            break;
            }
        case EUsbOtgStateBPeripheral:
            {
            LOG("OTGState == BPeripheral" );
            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->BPeripheralL();
                }
            break;
            }
        case EUsbOtgStateBHost:
            {
            LOG("OTGState == BHost" );
            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->BHostL();
                }
            break;
            }
        default:
            {
            LOG("WrongOtgState" );
            PANIC( EWrongOtgState);
            }
        }

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbOtgStateObserver::DoCancel()
    {
    iOtgState.Cancel();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CUsbOtgStateObserver::RunError(TInt aError)
    {
    LOG_FUNC

    LOG1( "aError = %d" , aError);

    // try to continue	
    return KErrNone;

    }

