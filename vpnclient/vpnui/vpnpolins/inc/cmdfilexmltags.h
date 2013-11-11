/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Literal constants for VPN command file XML tags
*
*/




#ifndef C_VPNCMDFILEXMLTAGS_H
#define C_VPNCMDFILEXMLTAGS_H

_LIT(KCmdFileStart, "vpncommands");

// Versioninfo and its subtree
_LIT(KCmdVersionInfo, "versionInfo");
_LIT(KCmdVersion, "version");
_LIT(KCmdDescription, "description");
_LIT(KCmdCreated, "created");

// Device and its subtree
_LIT(KCmdDevice, "device");
_LIT(KCmdDeviceLock, "devicelock");
_LIT(KCmdLockPolicy, "lockPolicy");
_LIT(KCmdRequired, "required");

// PKCS#12 and its subtree
_LIT(KCmdPKCS12, "pkcs12");
_LIT(KCmdP12Pwd, "p12pwd");

_LIT(KCmdUriSeparator, "/");

_LIT8(KCmdCr, "\r");
_LIT8(KCmdLf, "\n");

_LIT(KTrue, "true");
_LIT(KFalse, "false");

// URIs for parameter value extracting

// PKCS#12 password parameter
_LIT8(KCmdPKCS12PasswordUri, "vpncommands/pkcs12/p12pwd/");

// Devlock policy level setting
_LIT8(KCmdDevlockPolicyUri, "vpncommands/device/devicelock/lockPolicy/");

// Devlock policy level optionality
_LIT8(KCmdDevlockRequiredUri, "vpncommands/device/devicelock/required/");

#endif // C_VPNCMDFILEXMLTAGS_H

