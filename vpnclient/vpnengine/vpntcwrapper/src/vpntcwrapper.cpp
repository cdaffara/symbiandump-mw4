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
* Description:   PKCS#12 data handler
*
*/



#include <x509cert.h>
#include <mpkcs12.h>

#include <e32std.h>
#include <apacmdln.h>
#include <bautils.h>

#include <securityerr.h>
#include <sysutil.h>

#include "vpntcwrapper.h"
#include "logvpncommon.h"

EXPORT_C CVpnTcStub* NewTcStubL()
    {
    return new (ELeave) CVpnTcWrapper;
    }

void CVpnTcWrapper::SetTCAttributeL(const TInt aAttribId, const TDesC8& aVal) 
    {
    LOG_("-> CVpnTcWrapper::SetTCAttribute()");

    if (!iConnected)
        {
        LOG_(" Not connected yet...");
        ConnectToDevLockL();
        }

    TInt err = iTs.SetDeviceLockParameter(aAttribId, aVal);

    LOG_1(" Errorcode: %d", err);

    User::LeaveIfError(err);

    LOG_("<- CVpnTcWrapper::SetTCAttribute()");
    }

HBufC8* CVpnTcWrapper::GetTCAttributeL(TInt aAttribId) 
    {
    LOG_("-> CVpnTcWrapper::GetTCAttribute()");

    if (!iConnected)
        {
        LOG_(" Not connected yet...");
        ConnectToDevLockL();
        }

    LOG_(" Getting size");

    TInt size = iTs.GetDeviceLockParameterSize(aAttribId);
    if(size < 0)
        {
        LOG_1("<- CVpnTcWrapper::GetTCAttribute() Leave: %d", size);
        User::Leave(size);
        }

    LOG_1(" Attribute found, size: %d", size);

    HBufC8* buffer = HBufC8::NewLC(size);
    TPtr8 ptr8(buffer->Des());

    // Read the actual attribute value
    User::LeaveIfError(iTs.GetDeviceLockParameter(aAttribId, ptr8));

    CleanupStack::Pop(buffer);

    LOG_("<- CVpnTcWrapper::GetTCAttribute()");
    return buffer;
    }

void CVpnTcWrapper::ConnectToDevLockL() 
    {
    LOG_("-> CVpnTcWrapper::ConnectToDevLockL()");

    User::LeaveIfError(iTc.Connect());
    User::LeaveIfError(iTs.Open(iTc));
    
    iConnected = ETrue;

    LOG_("<- CVpnTcWrapper::ConnectToDevLockL()");
    }

CVpnTcWrapper::~CVpnTcWrapper() 
    {
    LOG_("-> CVpnTcWrapper::~CVpnTcWrapper()");
    
    iTs.Close();
    iTc.Close();
    iConnected = EFalse;
    
    LOG_("<- CVpnTcWrapper::~CVpnTcWrapper()");
    }



