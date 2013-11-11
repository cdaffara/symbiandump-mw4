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
#include "cusbnotifier.h"

#include "debug.h"
#include "panic.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Desctructor
// ---------------------------------------------------------------------------
//
CUsbNotifier::~CUsbNotifier()
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// Getter for returning if the notifier needs input from end-user
// ---------------------------------------------------------------------------
//
TBool CUsbNotifier::IsFeedbackNeeded() const
    {
    return iIsFeedbackNeeded;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CUsbNotifier::CUsbNotifier(CUsbNotifManager& aNotifManager, TUid aCat,
        TUint aNotifId) :
    iNotifManager(aNotifManager), iCat(aCat), iNotifId(aNotifId),
            iIsFeedbackNeeded(EFalse)
    {
    }

// ---------------------------------------------------------------------------
// Set by child-classe if the concrete notifier needs feedback from end-user
// ---------------------------------------------------------------------------
//
void CUsbNotifier::SetFeedbackNeeded()
    {
    iIsFeedbackNeeded = ETrue;
    }

// End of file
