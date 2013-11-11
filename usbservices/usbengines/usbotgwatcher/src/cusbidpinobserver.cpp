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

#include "cusbidpinobserver.h"

#include "definitions.h"
#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbIdPinObserver::CUsbIdPinObserver() :
    CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbIdPinObserver::ConstructL()
    {
    LOG_FUNC

    LEAVEIFERROR(iIdPin.Attach(KUidUsbManCategory,
            KUsbOtgIdPinPresentProperty));

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbIdPinObserver* CUsbIdPinObserver::NewL()
    {
    LOG_FUNC

    CUsbIdPinObserver* self = new (ELeave) CUsbIdPinObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbIdPinObserver::~CUsbIdPinObserver()
    {
    LOG_FUNC

    Cancel();

    iIdPin.Close();

    iObservers.Close();

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbIdPinObserver::TState CUsbIdPinObserver::IdPin()
/* this getter is not const, because for some reason RProperty::Get is not const */
    {

    TInt val(0);

    TInt err = iIdPin.Get(val);

    if (KErrNone != err)
        {
        LOG("CanNotGetIdPinProperty" );
        PANIC( ECanNotGetIdPinProperty);
        }

    return (EFalse == val ? EIdPinOff : EIdPinOn);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
void CUsbIdPinObserver::SubscribeL(MUsbIdPinObserver& aObserver)
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
        iIdPin.Subscribe(iStatus);
        SetActive();

        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbIdPinObserver::UnsubscribeL(MUsbIdPinObserver& aObserver)
    {
    LOG_FUNC

    TInt i(iObservers.Find(&aObserver));
    if (KErrNotFound == i)
        {
        LOG("Observer not found");
        PANIC( ECanNotFindIdPinObserver);
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
void CUsbIdPinObserver::RunL()
    {
    LOG_FUNC

    LOG1( "iStatus = %d" , iStatus.Int());

    // if error occured, tell to Observers
    if (KErrNone != iStatus.Int())
        {
        for (TInt i(0); i < iObservers.Count(); ++i)
            {
            iObservers[i]->IdPinErrorL(iStatus.Int());
            }
        return;
        }

    // re-issue request first
    iIdPin.Subscribe(iStatus);
    SetActive();

    // then process property change
    TState state(IdPin());

    // report change   
    switch (state)
        {
        case EIdPinOn:
            {
            LOG("IdPin ON");

            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->IdPinOnL();
                }
            break;
            }

        case EIdPinOff:
            {
            LOG("IdPin OFF");

            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->IdPinOffL();
                }
            break;
            }

        default:
            {
            LOG("WrongIdPinState");
            PANIC( EWrongIdPinState);
            }
        }

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbIdPinObserver::DoCancel()
    {
    iIdPin.Cancel();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CUsbIdPinObserver::RunError(TInt aError)
    {
    LOG_FUNC

    LOG1( "aError = %d", aError);

    // try to continue	
    return KErrNone;

    }
