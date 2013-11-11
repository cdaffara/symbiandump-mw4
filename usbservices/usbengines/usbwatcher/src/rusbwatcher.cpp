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
* Description:  This class implements UsbWatcher Server API
*
*/

#include <e32uid.h>
#include <e32base.h>
#include <usb.h>
#include <usbwatcher.h>
#include "rusbwatcher.h"
#include "usbwatchershared.h"
#include "debug.h"

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
EXPORT_C RUsbWatcher::RUsbWatcher()
    {
    LOG_FUNC
    
    }

// ---------------------------------------------------------------------------
// C++ default destructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
EXPORT_C RUsbWatcher::~RUsbWatcher()
    {
    LOG_FUNC

    }

// ---------------------------------------------------------------------------
// Server version.
// ---------------------------------------------------------------------------
//
EXPORT_C TVersion RUsbWatcher::Version() const
    {
    LOG_FUNC

    return( TVersion( KUsbWatcherSrvMajorVersionNumber,
            KUsbWatcherSrvMinorVersionNumber,
            KUsbWatcherSrvBuildVersionNumber ) );
    }

// ---------------------------------------------------------------------------
// Creates session to UsbWatcher server.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RUsbWatcher::Connect()
    {
    LOG_FUNC
    
    return CreateSession( KUsbWatcherServerName, Version(), 10 );
    }
    
// ---------------------------------------------------------------------------
// Set new personality. If USB cable is connected change will happen
// immediately.
// ---------------------------------------------------------------------------
//
EXPORT_C void RUsbWatcher::SetPersonality( TRequestStatus& aStatus, 
    TInt aPersonalityId, TBool aForce, TBool aNonBlocking )
    {   
    LOG_FUNC

    TIpcArgs ipcArgs( aPersonalityId, aForce, aNonBlocking );
    SendReceive( EUsbWatcherSetPersonality, ipcArgs, aStatus );
    }
    
// ---------------------------------------------------------------------------
// Cancel setting of new personality. 
// ---------------------------------------------------------------------------
//
EXPORT_C void RUsbWatcher::CancelSetPersonality()
    {
    LOG_FUNC

    SendReceive( EUsbWatcherCancelSetPersonality );
    }

// ---------------------------------------------------------------------------
// Set previous personality. 
// ---------------------------------------------------------------------------
//
EXPORT_C void RUsbWatcher::SetPreviousPersonality( TRequestStatus& aStatus )
    {
    LOG_FUNC

    SendReceive( EUsbWatcherSetPreviousPersonality, aStatus );
    }

// ---------------------------------------------------------------------------
// Set previous personality synchronously. Request is completed before
// it has been finished.
// ---------------------------------------------------------------------------
//
EXPORT_C void RUsbWatcher::SetPreviousPersonality()
    {
    LOG_FUNC

    SendReceive( EUsbWatcherSetPreviousPersonalitySync );
    }

// ---------------------------------------------------------------------------
// Cancel setting of previous personality. 
// ---------------------------------------------------------------------------
//
EXPORT_C void RUsbWatcher::CancelSetPreviousPersonality()
    {
    LOG_FUNC

    SendReceive( EUsbWatcherCancelSetPreviousPersonality );
    }

// ---------------------------------------------------------------------------
// Set previous personality when cable is disconnected. This will do nothing
// if cable isn't connected.
// ---------------------------------------------------------------------------
//
EXPORT_C void RUsbWatcher::SetPreviousPersonalityOnDisconnect()
    {
    LOG_FUNC

    SendReceive( EUsbWatcherSetPreviousPersonalityOnDisconnect );
    }

// End of file
