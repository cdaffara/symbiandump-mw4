// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description: Adheres to the UsbMan USB Class API and talks to mass storage file system
// 
// 
#ifndef USBMSCFILECLASSCONTROLLER_CRKEYS_H
#define USBMSCFILECLASSCONTROLLER_CRKEYS_H

// CONSTANTS
const TUid KCRUidUsbPhoneAsModemMscFileClassController = { 0x2000FDA8 };

/*
the least significant four bits is used for columns(unit file attributes: unit number, full name), 
the remains bit is used for rows(which unit file).

for example, if two units is used, the following can be added for the second unit keys.

const TUint32 KCRIdMscFileLogicalUnitNumber2 = 0x00000010;
const TUint32 KCRIdMscFileImageFullName2     = 0x00000011;

refer to central repository how-to document section 5, Structured Data in the Central Repository

*/
const TUint32 KCRIdMscFileLogicalUnitNumber1 	= 0x00000000;
const TUint32 KCRIdMscFileImageFullName1     	= 0x00000001;

const TUint32 KCRMscFileLogicalUnitNumberColumn = 0x00000000;
const TUint32 KCRMscFileImageFullNameColumn 	= 0x00000001;

const TUint32 KCRMscFileColumnMask 				= 0x0000000F;
const TUint32 KCRMscFileRowMask 				= 0xFFFFFFF0;
#endif      // USBMSCFILECLASSCONTROLLER_CRKEYS_H
