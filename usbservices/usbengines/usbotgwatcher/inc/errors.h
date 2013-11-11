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
 * Description:  File contains error codes, from Watcher perspective
 *
 */

#ifndef ERRORS_H
#define ERRORS_H

#include <e32base.h>

enum TUsbWatcherErrors
    {
    EUsbWatcherUndefined, //0	
    EUsbWatcherErrDriversNotFound, //1
    EUsbWatcherHubsNotSupported, //2
    EUsbWatcherErrDeviceRequiresTooMuchPower, //3
    EUsbWatcherErrUnsupportedDevice, //4
    EUsbWatcherConnectedToOTG, //5
    EUsbWatcherErrDandlingCable, //6
    EUsbWatcherNoActivity, //7
    EUsbWatcherErrorInConnection, //8
    EUsbWatcherErrDeviceRequiresTooMuchPowerOnEnumeration, //9
    EUsbWatcherCanNotStartUsbServices, //10
    EUsbWatcherIdPinError,
    EUsbWatcherVBusObserverError,
    EUsbWatcherHostEventNotificationError,
    EUsbWatcherOtgStateError,
    EUsbWatcherMessageNotificationError,
    EUsbWatcherCanNotStartSession,
    EUsbWatcherCanNotEnableDriverLoading,
    EUsbWatcherCanNotClearBusError,
    EUsbWatcherCanNotRaiseVBus,
    EUsbWatcherNoMemory

    };

#endif //  ERRRORS_H
