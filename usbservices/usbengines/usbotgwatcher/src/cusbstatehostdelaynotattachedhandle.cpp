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

#include "cusbstatehostdelaynotattachedhandle.h"

#include "errors.h"
#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostDelayNotAttachedHandle::CUsbStateHostDelayNotAttachedHandle(
        CUsbOtgWatcher& aWatcher) :
    CUsbStateHostDelayHandle(aWatcher)
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostDelayNotAttachedHandle* CUsbStateHostDelayNotAttachedHandle::NewL(
        CUsbOtgWatcher& aWatcher)
    {
    LOG_FUNC

    CUsbStateHostDelayNotAttachedHandle* self =
            new (ELeave) CUsbStateHostDelayNotAttachedHandle(aWatcher);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostDelayNotAttachedHandle::ConstructL()
    {
    LOG_FUNC

    CUsbStateHostDelayHandle::ConstructL();

    iTooMuchPowerTimer = CUsbTimer::NewL(*this, ETooMuchPowerRequiredTimer);

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostDelayNotAttachedHandle::~CUsbStateHostDelayNotAttachedHandle()
    {
    LOG_FUNC

    delete iTooMuchPowerTimer;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUsbStateIds CUsbStateHostDelayNotAttachedHandle::Id()
    {
    return EUsbStateHostDelayNotAttachedHandle;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostDelayNotAttachedHandle::JustBeforeLeavingThisStateL()
    {
    LOG_FUNC

    iTooMuchPowerTimer->Cancel();

    // do general things 
    CUsbStateHostDelayHandle::JustBeforeLeavingThisStateL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostDelayNotAttachedHandle::DoHandleL()
    {
    LOG_FUNC
    LOG1( "iWhat = %d" , iWhat);

    switch (iWhat)
        {
        case EUsbWatcherErrDeviceRequiresTooMuchPowerOnEnumeration:
            {
            LOG("EUsbWatcherErrDeviceRequiresTooMuchPowerOnEnumeration" );

            iTooMuchPowerTimer->After(KTimeTooMuchPowerRequired);
            break;
            }

        default:
            {
            LOG1("Unexpected request id = %d", iWhat);
            PANIC( EUnexpectedSituationToHandle);
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostDelayNotAttachedHandle::AIdleL()
    {
    LOG_FUNC

    // Exit on AIdle, due to Detachment not comes,
    // because no corresponding attachment with err == KErrNone
    ChangeHostStateL( EUsbStateHostAInitiate);

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostDelayNotAttachedHandle::DeviceDetachedL(
        TDeviceEventInformation)
    {
    LOG_FUNC
    PANIC( EDeviceDetachedNotExpected);

    }

// From TimerObserver
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostDelayNotAttachedHandle::TimerElapsedL(TUsbTimerId aTimerId)
    {
    LOG_FUNC
    switch (aTimerId)
        {
        case ETooMuchPowerRequiredTimer:
            {
            LOG("ETooMuchPowerRequiredTimer" );
            HandleL(EUsbWatcherErrUnsupportedDevice,
                    EUsbStateHostHandleDropping);
            break;
            }
        default:
            {
            LOG1("Unknown timer id = %d", aTimerId );
            PANIC( EWrongTimerId);
            }
        }
    }
