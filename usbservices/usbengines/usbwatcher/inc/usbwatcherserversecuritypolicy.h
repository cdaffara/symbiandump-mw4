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
* Description:  Server security policy definitions
*
*/

 
#ifndef USBWATCHERSERVERSECURITYPOLICY_H
#define USBWATCHERSERVERSECURITYPOLICY_H

/**
 * USB Wathcer server security policy definitions
 */
const TUint KUsbWatcherServerRangeCount = 2;

const TInt KUsbWatcherServerRanges[ KUsbWatcherServerRangeCount ] = 
	{
	EUsbWatcherSetPersonality,        // NetworkControl 
	EUsbWatcherSetPreviousPersonalityOnDisconnect + 1  
	};

/** 
 * Index numbers into KUsbWatcherServerElements[] 
 */
const TInt KPolicyLocalServices = 0;
const TInt KPolicyPass = 1;

/**
 * Mapping IPCs to poicy element 
 */
const TUint8 KUsbWatcherServerElementsIndex[ KUsbWatcherServerRangeCount ] = 
    {
    KPolicyLocalServices,
    CPolicyServer::ENotSupported, 
    };

/** 
 * Individual policy elements 
 */
const CPolicyServer::TPolicyElement KUsbWatcherServerElements[] = 
    {
    // the EFailClient means that the if the check fails the CheckFailed method
    //with return KErrPermissionDenied 
    { _INIT_SECURITY_POLICY_C1( ECapabilityLocalServices ), 
            CPolicyServer::EFailClient },
    { _INIT_SECURITY_POLICY_PASS },
	};

/** 
 * Main policy definition
 */
const CPolicyServer::TPolicy KUsbWatcherServerPolicy = 
    {
    CPolicyServer::EAlwaysPass, //Specifies all connect attempts should pass 
    KUsbWatcherServerRangeCount,
    KUsbWatcherServerRanges,
    KUsbWatcherServerElementsIndex,
    KUsbWatcherServerElements,
    };

#endif //USBWATCHERSERVERSECURITYPOLICY_H

// End of file
