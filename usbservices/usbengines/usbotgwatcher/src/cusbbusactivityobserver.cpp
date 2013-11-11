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

#include <usbotgdefs.h>

#include "cusbbusactivityobserver.h"

#include "definitions.h"
#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbBusActivityObserver::CUsbBusActivityObserver() :
    CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbBusActivityObserver::ConstructL()
    {
    LOG_FUNC

    LEAVEIFERROR(iBusActivity.Attach(KUidUsbManCategory,
            KUsbOtgConnectionIdleProperty));

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbBusActivityObserver* CUsbBusActivityObserver::NewL()
    {
    LOG_FUNC

    CUsbBusActivityObserver* self = new (ELeave) CUsbBusActivityObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbBusActivityObserver::~CUsbBusActivityObserver()

    {
    LOG_FUNC

    Cancel();

    iBusActivity.Close();

    iObservers.Close();

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbBusActivityObserver::TBusActivity CUsbBusActivityObserver::BusActivity()
    {

    TInt val(0);

    TInt err = iBusActivity.Get(val);

    if (KErrNone != err)
        {
        LOG("ECanNotGetBusActivityProperty" );
        PANIC( ECanNotGetBusActivityProperty);
        }

    return (0 == val ? EBusActive : EBusIdle);

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbBusActivityObserver::SubscribeL(MUsbBusActivityObserver& aObserver)
    {
    LOG_FUNC

    // check if the same observer already exist in a list
    if (KErrNotFound != iObservers.Find(&aObserver))
        {
        LOG( "Observer already exists" );
        PANIC( EObserverAlreadyExists);
        return;
        }
    iObservers.AppendL(&aObserver);

    if (KFirst == iObservers.Count()) // first item
        {
        iBusActivity.Subscribe(iStatus);
        SetActive();

        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbBusActivityObserver::UnsubscribeL(MUsbBusActivityObserver& aObserver)
    {
    LOG_FUNC

    TInt i(iObservers.Find(&aObserver));
    if (KErrNotFound == i)
        {
        LOG( "Observer not found" );
        PANIC( ECanNotFindBusActivityObserver);
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

// ---------------------------------------------------------------------------
//
void CUsbBusActivityObserver::RunL()
    {
    LOG_FUNC

    LOG1( "iStatus = %d" , iStatus.Int());

    // if error occured, inform observers
    if (KErrNone != iStatus.Int())
        {
        for (TInt i(0); i < iObservers.Count(); ++i)
            {
            iObservers[i]->BusActivityErrorL(iStatus.Int());
            }
        }

    // re-issue request first
    iBusActivity.Subscribe(iStatus);
    SetActive();

    // then process property change
    TBusActivity state(BusActivity());

    switch (state)
        {
        case EBusIdle:
            {
            LOG("BusIdle");

            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->BusIdleL();
                }
            break;
            }

        case EBusActive:
            {
            LOG("BusActive");

            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->BusActiveL();
                }
            break;
            }

        default:
            {
            LOG("WrongBusState" );
            PANIC( EWrongBusState);
            }
        }

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbBusActivityObserver::DoCancel()
    {
    iBusActivity.Cancel();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CUsbBusActivityObserver::RunError(TInt aError)
    {
    LOG_FUNC

    LOG1( "aError = %d", aError );

    // try to continue	
    return KErrNone;

    }

