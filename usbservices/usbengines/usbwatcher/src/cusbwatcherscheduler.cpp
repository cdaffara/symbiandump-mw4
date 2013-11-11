/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class reports possible scheduling errors to server.
*
*/


#include "cusbwatcherscheduler.h"
#include "cusbwatcherserver.h"
#include "debug.h"

// ----------------------------------------------------------------------------
// Symbian two-phase constructor
// ----------------------------------------------------------------------------
//
CUsbWatcherScheduler* CUsbWatcherScheduler::NewL()
    {
    LOG_FUNC

    CUsbWatcherScheduler* self = new( ELeave ) CUsbWatcherScheduler;
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CUsbWatcherScheduler::~CUsbWatcherScheduler()
    {
    LOG_FUNC

    }

// ----------------------------------------------------------------------------
// Set server
// ----------------------------------------------------------------------------
//
void CUsbWatcherScheduler::SetServer( CUsbWatcherServer& aServer )
    {
    LOG_FUNC

    iServer = &aServer;
    }

// ----------------------------------------------------------------------------
// Error handle
// ----------------------------------------------------------------------------
//
void CUsbWatcherScheduler::Error( TInt aError ) const
    {
    LOG_FUNC

    LOG1( "aError = %d", aError );
    if (iServer)
        {
        iServer->Error( aError );
        }
    }

// End of file
