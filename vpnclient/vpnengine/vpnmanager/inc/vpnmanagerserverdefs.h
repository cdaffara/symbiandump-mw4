/*
* Copyright (c) 2000-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Server component for VPN (Ext) Manager, used by VPN (Ext) API. 
* VPN Ext API uses the definition, but provides its own implementation.
*
*/



#ifndef __VPNMANAGERDEFS_H__
#define __VPNMANAGERDEFS_H__

#include <e32base.h>

static const TUid KVpnManagerUid3 = {0x101F7993};

_LIT(KVpnManagerFile, "vpnmanager");

_LIT(KVpnManagerServer, "!VpnManagerServer");

// The server version. A version must be specified when 
// creating a session with the server
const TUint KVpnManagerMajorVersionNumber = 0;
const TUint KVpnManagerMinorVersionNumber = 1;
const TUint KVpnManagerBuildVersionNumber = 1;

// Opcodes used in message passing between client and server
enum TVpnApiRequest
	{
    EVpnImportPolicy = 1000, // To *not* conflict with VPN Connection API opcodes
    EVpnCancelImport,
    EVpnEnumeratePolicies,
    EVpnGetPolicyInfo,
    EVpnGetPolicyDetails,
    EVpnDeletePolicy,
    EVpnChangePassword,
    EVpnCancelChange,
    EVpnGetPolicySize,
    EVpnGetPolicyData,
    // New ones (to facilitate
    // OMA DM based policy management)
    EVpnAddPolicy,
    EVpnUpdatePolicyDetails,
    EVpnUpdatePolicyData
    };

// Panic codes
enum TVpnManagerPanic
    {
    ECreateTrapCleanup = 1,
    ECreateServer,
    EBadDescriptor,
    EBadRequest,
    EInvalidImportState
    };

#endif // __VPNMANAGERDEFS_H__
