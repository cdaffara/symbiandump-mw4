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
* Description : P&S key definition for communication between VPN client
*               and SIP Profile server
*
*/

#ifndef VPNSIPOBSERVERPSKEYS_H
#define VPNSIPOBSERVERPSKEYS_H


/**
* PubSub keys
*/

const TUid KPSVpnSipUid = {0x101FD285};  // UID of vpnapi

const TUint KVpnSipState = 0x00000001;

enum TVpnSipState
{
    /** Initial value */
    EVpnUndefined = 0,
    /** Vpn is about to be started */
    EVpnInitiating,                // VPN -> SIP
    /** Vpn ended */
    EVpnTerminated,                // VPN -> SIP
    /** Deregister complete */
    ESipDeregisterCompleted        // SIP -> VPN
};


#endif  // VPNSIPOBSERVERPSKEYS_H
