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
// Description: Shared client/server definitions
// 
// 

#ifndef USBMSCFILESHARED_H
#define USBMSCFILESHARED_H

#include <e32std.h>

const TInt KUsbMsSrvMajorVersionNumber = 1;
const TInt KUsbMsSrvMinorVersionNumber = 0;
const TInt KUsbMsSrvBuildVersionNumber = 0;

const TInt KUsbMsResourceVersion = 0;
_LIT(KMscFileServerName, "usbmscfileserver");
_LIT(KMscFileServerFileName, "usbmscfileserver.exe");

class TMassStorageConfig
    {
public:
    TBuf<8> iVendorId;
    TBuf<16> iProductId;
    TBuf<4> iProductRev;
    };

/** Types of requests USB mass storage class controller can make */
enum TMscFileServerReq
    {
    EMscFileSetupLu, EMscFileStart, EMscFileStop, EMscFileShutdown,
    };

#endif //USBMSCFILESHARED_H
