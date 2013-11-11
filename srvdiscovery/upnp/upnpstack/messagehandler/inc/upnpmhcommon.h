/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Common structs
*
*/

#ifndef C_UPNPMHCOMMON_H
#define C_UPNPMHCOMMON_H

#include <e32base.h>

#include "upnpcommonstructs.h"
#include "upnpcompvariant.hrh"
// CONSTANTS

// Server name


#if defined(__HN_31__) || defined(__HN_32__)
_LIT( KMessageHandlerName,          "hnMessageHandler" );
_LIT( KMessageHandlerThreadName,    "hnMessageHandler_MainThread" );
_LIT( KMessageHandlerFilename,      "hnMessageHandler" );
#else
_LIT( KMessageHandlerName,          "MessageHandler" );
_LIT( KMessageHandlerThreadName,    "MessageHandler_MainThread" );
_LIT( KMessageHandlerFilename,      "MessageHandler" );
#endif

// The server version. A version must be specified when 
// creating a session with the server
const TUint KMessageHandlerMajorVersionNumber=0;
const TUint KMessageHandlerMinorVersionNumber=1;
const TUint KMessageHandlerBuildVersionNumber=1;

// ENUMERATIONS

// Opcodes used in message passing between client and server
enum TMessageHandlerRqst
    {
    EAddLocalDevice = 100,
    ERemoveLocalDevice,
    ERemoveSilentLocalDevice,
    EAddControlPointClient,
    ERemoveControlPointClient,
    EStopFilteringDevice,
    EGetMessageSize,
    EGetMessageSizeSynch,
    EGetMessage,
    ESendMessageSize,
    ESendMessage,    
    EGetDeviceListSize,
    EGetDeviceList,
    ESsdpSearch,
    ETransferLength,
    ETransferStatus,
    EActiveIap,
    EStartSsdp,
    ECheckError,    
    
    ECancelRequest,
    
    EAddFilterAddrByIP,
    EAddFilterAddrByHostname,
    ERemoveFilterAddrByIP,
    ERemoveFilterAddrByHostname,
    ERemoveAllIPsAddresses,
    EIsIPAddressAllowed,
    ESubscribeIPListChanges,
    ECancelIPLIstSubsctiption,
    EGetIPFilterList,
    };

// Opcodes used by server to indicate which asynchronous service
// has completed
enum TMessageHandlerRqstComplete
    {
    EAddLocalDeviceComplete = 100,
    ERemoveLocalDeviceComplete,
    ERemoveSilentLocalDeviceComplete,
    EAddControlPointClientComplete,
    ERemoveControlPointClientComplete,
    EStopFilteringDeviceComplete,
    EGetMessageSizeComplete,
    EGetMessageSizeSynchComplete,
    EGetMessageComplete,
    ESendMessageSizeComplete,
    ESendMessageComplete,
    EGetDeviceListSizeComplete,
    EGetDeviceListComplete,
    ESsdpSearchComplete,
    ETransferLengthComplete,
    ETransferStatusComplete,
    EActiveIapComplete,
    EStartSsdpComplete, 
    ECheckErrorComplete,    

    ECancelRequestComplete
    };
#endif // C_UPNPMHCOMMON_H

// End Of File
