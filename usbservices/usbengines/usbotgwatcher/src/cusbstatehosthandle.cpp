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

#include "cusbstatehosthandle.h"

#include "errors.h"
#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostHandle::CUsbStateHostHandle(CUsbOtgWatcher& aWatcher) :
    CUsbStateHostABase(aWatcher)
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandle::ConstructL()
    {
    LOG_FUNC

    CUsbStateHostABase::ConstructL();

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostHandle::~CUsbStateHostHandle()
    {
    LOG_FUNC
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandle::JustAdvancedToThisStateL()
    {
    LOG_FUNC

    // first do general things
    CUsbStateHostABase::JustAdvancedToThisStateL();

    // then, do specific things
    DoHandleL();

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandle::SetWhat(TInt aWhat)
    {
    iWhat = aWhat;
    }

// From message notification observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandle::MessageNotificationReceivedL(TInt aMessage)
    {
    LOG_FUNC
    LOG1( "Unhandled message aMessage = %d" , aMessage);

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostHandle::SessionRequestedL()
    {
    LOG_FUNC
    }
