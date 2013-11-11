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

#include "cusbvbusobserver.h"

#include "definitions.h"
#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbVBusObserver::CUsbVBusObserver() :
    CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbVBusObserver::ConstructL()
    {
    LOG_FUNC

    LEAVEIFERROR(iVBus.Attach(KUidUsbManCategory,
            KUsbOtgVBusPoweredProperty));

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbVBusObserver* CUsbVBusObserver::NewL()
    {
    LOG_FUNC

    CUsbVBusObserver* self = new (ELeave) CUsbVBusObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbVBusObserver::~CUsbVBusObserver()
    {
    LOG_FUNC

    Cancel();

    iVBus.Close();

    iObservers.Close();

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbVBusObserver::TState CUsbVBusObserver::VBus() /* not const, because for some reason RProperty::Get is not const */
    {

    TInt val(0);

    TInt err = iVBus.Get(val);

    if (KErrNone != err)
        {
        LOG("CanNotGetVBusProperty" )
        PANIC( ECanNotGetVBusProperty);
        }

    return (0 == val ? EVBusDown : EVBusUp);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbVBusObserver::SubscribeL(MUsbVBusObserver& aObserver)
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
        iVBus.Subscribe(iStatus);
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbVBusObserver::UnsubscribeL(MUsbVBusObserver& aObserver)
    {
    LOG_FUNC

    TInt i(iObservers.Find(&aObserver));
    if (KErrNotFound == i)
        {
        LOG("Observer not found" );
        PANIC( ECanNotFindVBusObserver);
        return;
        }

    iObservers.Remove(i);

    if (0 == iObservers.Count()) // no observers anymore
        {
        // cancel pending request, if any
        Cancel();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CUsbVBusObserver::RunL()
    {
    LOG_FUNC

    LOG1( "iStatus = %d" , iStatus.Int());

    // if error occured, tell to Observers
    if (KErrNone != iStatus.Int())
        {
        for (TInt i(0); i < iObservers.Count(); ++i)
            {
            iObservers[i]->VBusObserverErrorL(iStatus.Int());
            }
        return;
        }

    // re-issue request first
    iVBus.Subscribe(iStatus);
    SetActive();

    // then process property change
    TState state(VBus());

    // report change   
    switch (state)
        {
        case EVBusUp:
            {
            LOG("VBus UP");

            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->VBusUpL();
                }
            break;
            }
            ;

        case EVBusDown:
            {
            LOG("VBus DOWN");

            for (TInt i(0); i < iObservers.Count(); ++i)
                {
                iObservers[i]->VBusDownL();
                }
            break;
            }
            ;

        default:
            {
            LOG("WrongVBusState");
            PANIC( EWrongVBusState);
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbVBusObserver::DoCancel()
    {
    iVBus.Cancel();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CUsbVBusObserver::RunError(TInt aError)
    {
    LOG_FUNC

    LOG1( "aError = %d" , aError);

    // try to continue	
    return KErrNone;
    }
