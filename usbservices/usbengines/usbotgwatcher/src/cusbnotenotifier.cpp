/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Base classes for Usb notifier wrapper
 *
 */

#include "cusbnotifmanager.h"
#include "cusbnotenotifier.h"

#include "debug.h"
#include "panic.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CUsbNoteNotifier::CUsbNoteNotifier(RNotifier& aNotifier,
        CUsbNotifManager& aNotifManager, TUid aCat, TUint aNotifId) :
    CUsbNotifier(aNotifManager, aCat, aNotifId), iNotifier(aNotifier)
    {
    }

// ---------------------------------------------------------------------------
// Second-phase constructor
// ---------------------------------------------------------------------------
//
void CUsbNoteNotifier::ConstructL()
    {
    LOG_FUNC

    iNotifierActive = new (ELeave) CUsbNoteNotifier::CNotifierActive(
            iNotifier, *this);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CUsbNoteNotifier::~CUsbNoteNotifier()
    {
    LOG_FUNC

    delete iNotifierActive;
    }

// ---------------------------------------------------------------------------
// From base class CUsbNotifier
// ---------------------------------------------------------------------------
//
void CUsbNoteNotifier::ShowL()
    {
    LOG_FUNC

    LOG2( "aCat = 0x%X aNotifId = 0x%X" , iCat, iNotifId);

    iNotifierActive->StartL();
    }

// ---------------------------------------------------------------------------
// From base class CUsbNotifier
// ---------------------------------------------------------------------------
//
void CUsbNoteNotifier::Close()
    {
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//
CUsbNoteNotifier::CNotifierActive::CNotifierActive(RNotifier& aNotifier,
        CUsbNoteNotifier& aUsbNoteNotifier) :
    CUsbNoteNotifier::CNotifierActive::CActive(EPriorityStandard),
            iUsbNoteNotifier(aUsbNoteNotifier), iNotifier(aNotifier), iRes(0)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CUsbNoteNotifier::CNotifierActive::~CNotifierActive()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Start to show the notifier
// ---------------------------------------------------------------------------
//
void CUsbNoteNotifier::CNotifierActive::StartL()
    {
    LOG_FUNC

    if (IsActive())
        {
        PANIC( ENotifierIsActiveAlready);
        return;
        }

    iNotifIdPckg() = iUsbNoteNotifier.iNotifId;
    iNotifier.StartNotifierAndGetResponse(iStatus, iUsbNoteNotifier.iCat,
            iNotifIdPckg, iRes);
    SetActive();
    }

// ---------------------------------------------------------------------------
// From base class CActive
// ---------------------------------------------------------------------------
//
void CUsbNoteNotifier::CNotifierActive::RunL()
    {
    LOG_FUNC

    LOG1( "iStatus = %d" , iStatus.Int());

    // if iStatus contains some other than KErrNone code
    // do not perform any special actions, 
    // just forward the error code to caller

    iNotifier.CancelNotifier(iUsbNoteNotifier.iCat);

    // report to owner that show is over
    iUsbNoteNotifier.iNotifManager.NotifierShowCompletedL(iUsbNoteNotifier,
            iStatus.Int(), iRes());
    }

// ---------------------------------------------------------------------------
// From base class CActive
// ---------------------------------------------------------------------------
//
void CUsbNoteNotifier::CNotifierActive::DoCancel()
    {
    iNotifier.CancelNotifier(iUsbNoteNotifier.iCat);
    }

// ---------------------------------------------------------------------------
// From base class CActive
// ---------------------------------------------------------------------------
//
TInt CUsbNoteNotifier::CNotifierActive::RunError(TInt aError)
    {
    LOG_FUNC

    LOG1("aError = %d" , aError);

    // try to continue  
    return KErrNone;
    }

// End of file
