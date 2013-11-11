/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef USBMANDLL_STUB_H_
#define USBMANDLL_STUB_H_

static const TUint KUsbManStubAgentDll = 0x00000033;

enum TUsbManStubAgentApi
    {
    EConnect,
    EGetDeviceState,
    EStart,
    EStop,
    ETryStartAsync,
    ETryStopAsync,
    EDeviceStateNotification,
    EGetCurrentPersonalityId,
    ECancelInterest,
    EDeviceStateNotificationCancel,
    EServiceState,
    ESetCtlSessionMode,
    EGetPersonalityProperty
    };

#endif /*USBMANDLL_STUB_H_*/
