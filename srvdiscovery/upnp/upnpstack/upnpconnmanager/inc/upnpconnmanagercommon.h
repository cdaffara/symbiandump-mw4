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

#ifndef T_UPNPCONNECTIONMANAGERCOMMON_H
#define T_UPNPCONNECTIONMANAGERCOMMON_H

#include "upnpcompvariant.hrh"
#include "upnpconnmanagereventtypes.h"
#include <in_sock.h> 

// the name of the server, used by client
_LIT( KUpnpConnectionManagerFilename, "upnpconnmanager" );

//server's name
_LIT( KUpnpConnectionManagerName, "upnpconnectionmanager" );
_LIT( KUpnpConnectionManagerThreadName, "upnpconnectionmanager_MainThread" );

const TUid KServerUid3 = { 0x2001B2BB };

// the server version
// A version must be specified when creating a session with the server
const TUint KUpnpConnectionManagerMajorVersionNumber = 0;
const TUint KUpnpConnectionManagerMinorVersionNumber = 1;
const TUint KUpnpConnectionManagerBuildVersionNumber = 1;
const TInt KNetworkEventSlot = 0;

#endif  // T_UPNPCONNECTIONMANAGERCOMMON_H

