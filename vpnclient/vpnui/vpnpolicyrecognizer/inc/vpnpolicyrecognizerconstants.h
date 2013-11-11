/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Recognize vpn policy files
*
*/



#ifndef VPNPOLICYRECOGNIZERCONSTANTS_H
#define VPNPOLICYRECOGNIZERCONSTANTS_H

#include <e32base.h>


const TUint KVpnPolicyRecognizerId = 0x101F6D38;
const TUid KVpnPolicyRecognizerUid = { KVpnPolicyRecognizerId };

const TInt KSupporstedMimeTypeCount = 2;
_LIT8(KZippedPolicyFile, "application/x-vpn-policy");
_LIT8(KPolicyInfoFile, "application/x-ipsec-policy-info");

const TInt KFileExtensionLength = 4;
_LIT(KVpnZipPolicyFileExtension, ".vpn");
_LIT(KVpnPinPolicyFileExtension, ".pin");

#endif //VPNPOLICYRECOGNIZERCONSTANTS_H