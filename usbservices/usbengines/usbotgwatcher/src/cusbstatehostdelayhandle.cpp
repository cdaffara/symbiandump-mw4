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
#include "cusbstatehostdelayhandle.h"

#include "errors.h"
#include "debug.h"
#include "panic.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostDelayHandle::CUsbStateHostDelayHandle(CUsbOtgWatcher& aWatcher) :
    CUsbStateHostHandle(aWatcher)
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostDelayHandle::ConstructL()
    {
    LOG_FUNC

    CUsbStateHostHandle::ConstructL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CUsbStateHostDelayHandle::~CUsbStateHostDelayHandle()
    {
    LOG_FUNC
    }

/////////////////////////////////////////////////////////////////////////////////////
// From VBus observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostDelayHandle::VBusUpL()
    {
    LOG_FUNC
    // this is not expected, due to in this state vbus already up, since entering the state
    PANIC( EVBusUpNotExpected);
    }

// From Host Event notification observer
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CUsbStateHostDelayHandle::DeviceAttachedL(
        TDeviceEventInformation aDevEventInfo)
    {
    LOG_FUNC

    ChangeHostStateL( EUsbStateHostAInitiate);
    iWatcher.DeviceAttachedL(aDevEventInfo);
    }
