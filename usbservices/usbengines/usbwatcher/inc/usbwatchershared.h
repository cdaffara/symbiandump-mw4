/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Shared definitions between client and server
*
*/


#ifndef USBWATCHERSHARED_H
#define USBWATCHERSHARED_H

#include <e32std.h>

const TInt KUsbWatcherSrvMajorVersionNumber = 1;
const TInt KUsbWatcherSrvMinorVersionNumber = 0;
const TInt KUsbWatcherSrvBuildVersionNumber = 0;

_LIT(KUsbWatcherServerName, "!usbwatcher");

const TInt KUsbWatcherMaxSupportedPersonalities = 64;

enum TUsbWatcherMessages
    {
    EUsbWatcherSetPersonality,
    EUsbWatcherCancelSetPersonality,
    EUsbWatcherSetPreviousPersonality,
    EUsbWatcherSetPreviousPersonalitySync,
    EUsbWatcherCancelSetPreviousPersonality,
    EUsbWatcherSetPreviousPersonalityOnDisconnect
    };

_LIT(KUsbWatcherCliPncCat, "UsbWatcher-Client");

enum TUsbWatcherPanicClient
    {
    EUsbWatcherCreateFailure,
    EUsbWatcherPanicIllegalIPC,
    EUsbWatcherPanicRemovedExport
    };

#endif //USBWATCHERSHARED_H

// End of file
