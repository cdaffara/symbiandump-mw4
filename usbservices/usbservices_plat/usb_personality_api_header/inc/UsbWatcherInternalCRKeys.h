/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  USB Watcher's internal Central Repository keys
*
*/


#ifndef USBWATCHERINTERNALCRKEYS_H
#define USBWATCHERINTERNALCRKEYS_H

// CONSTANTS
const TUid KCRUidUsbWatcher = { 0x101F8801 };

const TUint32 KUsbWatcherChangeOnConnectionSetting = 0x00000001;
    const TInt KUsbWatcherChangeOnConnectionOff = 0;
    const TInt KUsbWatcherChangeOnConnectionOn = 1;

const TUint32 KUsbWatcherPersonality = 0x00000002;

const TUint32 KUsbWatcherChargingDefaultPersonality = 0x00000003;

#endif      // USBWATCHERINTERNALCRKEYS_H

// End of File