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
 * Description:  Central place for debug-type macros & functions
 *
 */
#ifndef C_PANIC_H
#define C_PANIC_H

#include <e32base.h>
#include <e32debug.h>

_LIT(KUsbWatcherPanic, "UsbOTGWatcher");

enum TUsbWatcherPanic
    {
    ECanNotGetIdPinProperty, //0
    EWrongIdPinState, //1
    ECanNotGetVBusProperty, //2
    EWrongVBusState, //3
    ECanNotGetOtgStateProperty, //4
    ENotifierIsActiveAlready, //5
    EWrongNotifierCategory, //6
    EUsbWatcherBadErrorCode, //7
    ECanNotFindIdPinObserver, //8
    ECanNotFindVBusObserver, //9
    EWrongOtgState, //10
    ECanNotGetBusActivityProperty, //11
    EWrongBusState, //12
    ECanNotFindBusActivityObserver, //13
    EWrongDriverLoadStatus, //14
    EWrongHostEventNotification, //15
    ECanNotFindMessageNotificationObserver, //16
    ECanNotFindOtgStateObserver, //17
    EBadState, //18
    EBadHostState, // 19

    //From IdPin observer
    EIdPinOnNotExpected, //20
    EIdPinOffNotExpected, //21

    // From VBus observer
    EVBusDownNotExpected, //22
    EVBusUpNotExpected, //23

    // From OTG state observer
    EAIdleNotExpected, //24
    EAHostNotExpected, //25
    EAPeripheralNotExpected, //26
    EAVBusErrorNotExpected, //27
    EBIdleNotExpected, //28
    EBPeripheralNotExpected, //29
    EBHostNotExpected, //30

    // From bus activity observer
    EBusIdleNotExpected, //31
    EBusActiveNotExpected, //32

    // From Host Event notification observer
    EDeviceAttachedNotExpected, //33
    EDeviceDetachedNotExpected, //34
    EDriverLoadSuccessNotExpected, //35
    EDriverLoadPartialSuccessNotExpected, //36
    EDriverLoadFailureNotExpected, //37

    // From message notification observer
    EMessageNotificationNotExpected, //38
    EBadHubPositionNotExpected, //39
    EVBusErrorNotExpected, //40
    ESrpNotExpected, //41
    ESessionRequestNotExpected, //42

    EIdPinObserverNULLPointer, //43
    ECanNotStartUsbServices, //44
    ECanNotStopUsbServices, //45

    EDanglingCable, //46
    EPartiallySupportedDevice, //47
    EDriversLoadingFail, //48
    EUnsupportedDevice, //49
    EBadHubPosition, //50
    ETooMuchPower, //51
    EBusRespondSrpError, //52
    EEnableFunctionDriverLoadingError, //53
    EErrorInConnection, //54
    EWatcherDriverLoadPartialSuccess, //55
    EUnsupportedDeviceOrErrorInConnection, //56
    EBusRequestError, //57
    EAttachmentError, //58
    EVBusError, // 59
    ENoActivity, //60
    EDeviceDetached, //61
    EBadHubPositionEventNotExpected, //62
    EUnhandledMessage, //63
    EUnexpectedSituationToHandle, //64
    EConnectedToOTGNotExpected, //65

    EWrongTimerId, //66
    EUnexpectedNotifId, //67
    EUnknownUsbServiceState, //68
    EUnexpectedUsbServiceState, //69
    EUnexpectedUsbSwitchPersonalityState, //70

    ECanNotFindUsbOtgWatcherStateObserver, //71
    ENoObserverToUnsubscribe, // 72
    EObserverAlreadyExists, //73
    EIdForNotDefinedStateRequested, //74
    ECanNotFindHostEventNotificationObserver, //75
    ENoTimersDefinedForThisClass    //76

    };

#endif // C_PANIC_H
