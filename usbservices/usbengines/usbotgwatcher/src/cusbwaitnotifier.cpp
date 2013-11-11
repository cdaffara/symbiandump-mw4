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
 * Description:  Usb waiting notifier implementation
 *
 */

#include <e32std.h>
#include <usbuinotif.h>

#include "cusbnotifmanager.h"
#include "cusbwaitnotifier.h"

#include "debug.h"
#include "panic.h"

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUsbWaitNotifier* CUsbWaitNotifier::NewL(RNotifier& aNotifier,
        CUsbNotifManager& aNotifManager, TUint aNotifId)
    {
    LOG_FUNC

    CUsbWaitNotifier* self = new (ELeave) CUsbWaitNotifier(aNotifier,
            aNotifManager, aNotifId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CUsbWaitNotifier::~CUsbWaitNotifier()
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CUsbWaitNotifier::CUsbWaitNotifier(RNotifier& aNotifier,
        CUsbNotifManager& aNotifManager, TUint aNotifId) :
    CUsbNoteNotifier(aNotifier, aNotifManager, KUsbUiNotifOtgError, aNotifId)
    {
    SetFeedbackNeeded();
    }

// ---------------------------------------------------------------------------
// Second-phase constructor
// ---------------------------------------------------------------------------
//
void CUsbWaitNotifier::ConstructL()
    {
    LOG_FUNC

    CUsbNoteNotifier::ConstructL();
    }

// End of file
