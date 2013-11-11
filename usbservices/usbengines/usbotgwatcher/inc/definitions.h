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
 * Description:  Definitions for otgwatcher
 *
 */

#ifndef DEFENITIONS_H
#define DEFENITIONS_H

#include <e32base.h>

const TInt KTimeToWaitForDeviceAttachment = 25000000; // 25 sec
const TInt KTimeToDiscreteNote = 2000000; // 2 sec
const TInt KMaxTimeAllowedForBusInactivity = 10000000; // 10 sec
const TInt KUsbIndicatorBlinkingInterval = 600000; // 0.6 sec
const TInt KTimeTooMuchPowerRequired = 10000000; // 10 sec
const TInt KTimeDriversNotFound = 25000000; // 25 sec

const TUint KFirst = 1;

/**
 * Types of states
 * Types of states are needed here, to make transition from one state to another by state Id
 * not by setting real state pointer to a state machine. States do not have access to
 * one another states objects, they just know ids
 */
enum TUsbStateIds
    {
    EUsbStateHostUndefined,

    EUsbStateHostAInitiate,
    EUsbStateHostAHost,
    EUsbStateHostAPeripheral,

    EUsbStateHostDelayAttachedHandle,
    EUsbStateHostDelayNotAttachedHandle,
    EUsbStateHostHandleDropping
    };

const TInt KUsbWatcherPeripheralIsNotConnected = 0; // means not(KUsbWatcherPeripheralIsConnected)
const TInt KUsbWatcherPeripheralIsConnected = 1; // means drivers loaded and session ON at least for one peripheral

#endif //  DEFENITIONS_H
