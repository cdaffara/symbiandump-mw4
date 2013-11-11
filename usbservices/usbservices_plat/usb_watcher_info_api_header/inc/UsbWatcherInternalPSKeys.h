/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  USB Watcher's internal Publish & Subscribe keys
*
*/


#ifndef USBWATCHERINTERNALPSKEYS_H
#define USBWATCHERINTERNALPSKEYS_H

// CONSTANTS
const TUid KPSUidUsbWatcher = { 0x101F8801 };

const TUint32 KUsbWatcherSelectedPersonality = 0x00000001;
const TInt KUsbWatcherSelectedPersonalityNone = -1;

// specifies whether peripheral is connected, and device perform as host. 
const TUint32 KUsbWatcherIsPeripheralConnected = 0x00000002;  
    
#endif      // USBWATCHERINTERNALPSKEYS_H

// End of File