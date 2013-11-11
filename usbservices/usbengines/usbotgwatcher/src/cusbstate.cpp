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

#include "cusbotgwatcher.h"
#include "cusbstate.h"

#include "panic.h"
#include "debug.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbState::CUsbState(CUsbOtgWatcher& aWatcher) :
    iWatcher(aWatcher)
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbState::~CUsbState()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::ConstructL()
    {

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::JustAdvancedToThisStateL()
    {
    iWatcher.PrintStateToLog();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::JustBeforeLeavingThisStateL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::ChangeStateL(TUsbStateIds aNewStateId)
    {

    iWatcher.ChangeStateL(aNewStateId);

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::ChangeHostStateL(TUsbStateIds aNewStateId)
    {

    iWatcher.ChangeHostStateL(aNewStateId);

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::HandleL(TInt aWhat, TUsbStateIds aWhereToHandle)
    {
    LOG_FUNC

    LOG2( "aWhat = %d aWhere = %d" , aWhat, aWhereToHandle);

    iWatcher.HandleHostProblemL(aWhat, aWhereToHandle);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::SetPersonalityL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::CancelSetPersonalityL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::SetPreviousPersonalityL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::CancelSetPreviousPersonalityL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::SetPreviousPreviousPersonalityOnDisconnectL()
    {
    }

/////////////////////////////////////////////////////////////////////////////////////

// From IdPin observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::IdPinOffL()
    {
    LOG_FUNC
    PANIC(EIdPinOffNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::IdPinOnL()
    {
    LOG_FUNC
    PANIC(EIdPinOnNotExpected);
    }

// From VBus observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::VBusDownL()
    {
    LOG_FUNC
    PANIC(EVBusDownNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::VBusUpL()
    {
    LOG_FUNC
    PANIC(EVBusUpNotExpected);
    }

// From OTG state observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::AIdleL()
    {
    LOG_FUNC
    PANIC(EAIdleNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::AHostL()
    {
    LOG_FUNC
    PANIC(EAHostNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::APeripheralL()
    {
    LOG_FUNC
    PANIC(EAPeripheralNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::AVBusErrorL()
    {
    LOG_FUNC
    PANIC(EAVBusErrorNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::BIdleL()
    {
    LOG_FUNC
    PANIC(EBIdleNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::BPeripheralL()
    {
    LOG_FUNC
    PANIC(EBPeripheralNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::BHostL()
    {
    LOG_FUNC
    PANIC(EBHostNotExpected);
    }

// From bus activity observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::BusIdleL()
    {
    LOG_FUNC
    PANIC(EBusIdleNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::BusActiveL()
    {
    LOG_FUNC
    PANIC(EBusActiveNotExpected);
    }

// From Host Event notification observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::DeviceAttachedL(TDeviceEventInformation)
    {
    LOG_FUNC
    PANIC(EDeviceAttachedNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::DeviceDetachedL(TDeviceEventInformation)
    {
    LOG_FUNC
    PANIC(EDeviceDetachedNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::DriverLoadSuccessL(TDeviceEventInformation)
    {
    LOG_FUNC
    PANIC(EDriverLoadSuccessNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::DriverLoadPartialSuccessL(TDeviceEventInformation)
    {
    LOG_FUNC
    PANIC(EDriverLoadPartialSuccessNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::DriverLoadFailureL(TDeviceEventInformation)
    {
    LOG_FUNC
    PANIC(EDriverLoadFailureNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::BadHubPositionL()
    {
    LOG_FUNC
    PANIC(EBadHubPositionNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::VBusErrorL()
    {
    LOG_FUNC
    PANIC(EVBusErrorNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::MessageNotificationReceivedL(TInt)
    {
    LOG_FUNC
    PANIC(EMessageNotificationNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::SrpReceivedL()
    {
    LOG_FUNC
    PANIC(ESrpNotExpected);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbState::SessionRequestedL()
    {
    LOG_FUNC
    PANIC(ESessionRequestNotExpected);
    }
