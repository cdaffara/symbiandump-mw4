/** @file
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Connection manager / common definitions for cliet and server
*
*/

#ifndef T_UPNPCONNECTIONMANAGEREVENTTYPES_H
#define T_UPNPCONNECTIONMANAGEREVENTTYPES_H

// DATA TYPES
// opcodes used in message passing between client and server
enum TUpnpCmRqst
    {
    EEnsureStartRConnection = 1,
    EIsRConnectionStarted,
    EEventSubscribe,
    ECancelEventSubscribe,
    ECmActiveIap,
    ECmGetAddress
    };

// server response for subscription
enum TUpnpCmSubRsp
    {
    EUnknownEvent = 1,
    EConnectionClosedEvent,
    EWlanLostEvent,
    EAddressChangeEvent    
    };
    
#endif  // T_UPNPCONNECTIONMANAGEREVENTTYPES_H

