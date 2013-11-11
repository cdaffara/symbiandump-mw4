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

#include <usbuinotif.h>

#include "cusbstatehostdelayattachedhandle.h"
#ifndef STIF
#include "cusbtimer.h"
#else
#include "mockcusbtimer.h"
#endif

#include "errors.h"
#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostDelayAttachedHandle::CUsbStateHostDelayAttachedHandle(
        CUsbOtgWatcher& aWatcher) :
    CUsbStateHostDelayHandle(aWatcher)
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostDelayAttachedHandle* CUsbStateHostDelayAttachedHandle::NewL(
        CUsbOtgWatcher& aWatcher)
    {
    LOG_FUNC

    CUsbStateHostDelayAttachedHandle* self =
            new (ELeave) CUsbStateHostDelayAttachedHandle(aWatcher);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostDelayAttachedHandle::ConstructL()
    {
    LOG_FUNC

    CUsbStateHostDelayHandle::ConstructL();
    iDriversNotFoundTimer = CUsbTimer::NewL(*this, EDriversNotFoundTimer);

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostDelayAttachedHandle::~CUsbStateHostDelayAttachedHandle()
    {
    LOG_FUNC

    delete iDriversNotFoundTimer;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUsbStateIds CUsbStateHostDelayAttachedHandle::Id()
    {
    return EUsbStateHostDelayAttachedHandle;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostDelayAttachedHandle::JustBeforeLeavingThisStateL()
    {
    LOG_FUNC

    iDriversNotFoundTimer->Cancel();

    // do general things 
    CUsbStateHostDelayHandle::JustBeforeLeavingThisStateL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostDelayAttachedHandle::DoHandleL()
    {
    LOG_FUNC
    LOG1( "iWhat = %d" , iWhat);

    switch (iWhat)
        {
        case EUsbWatcherErrDriversNotFound:
            {
            LOG("DriversNotFound" );

            iDriversNotFoundTimer->After(KTimeDriversNotFound);
            break;

            }

        default:
            {
            LOG1("Unexpected request id = %d" , iWhat );
            PANIC( EUnexpectedSituationToHandle);
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostDelayAttachedHandle::DeviceDetachedL(
        TDeviceEventInformation)
    {
    LOG_FUNC

    ChangeHostStateL( EUsbStateHostAInitiate);
    }

// From TimerObserver
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostDelayAttachedHandle::TimerElapsedL(TUsbTimerId aTimerId)
    {
    LOG_FUNC

    switch (aTimerId)
        {
        case EDriversNotFoundTimer:
            {
            LOG( "EDriversNotFoundTimer" );
            HandleL(EUsbWatcherErrUnsupportedDevice,
                    EUsbStateHostHandleDropping);

            break;
            }
        default:
            {
            LOG1( "Unknown timer id = %d", aTimerId );
            PANIC( EWrongTimerId);
            }
        }
    }
