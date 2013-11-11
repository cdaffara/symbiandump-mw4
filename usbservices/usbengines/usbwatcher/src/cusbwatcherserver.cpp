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
* Description:  USB Watcher server class implementation.
*
*/


#include <e32svr.h>
#include "usbwatchershared.h"
#include "cusbwatcherserver.h"
#include "cusbwatchersession.h"
#include "usbwatcherserversecuritypolicy.h"
#include "cusbwatcher.h"
#include "debug.h"

// ----------------------------------------------------------------------------
// Symbian two-phase constructor
// ----------------------------------------------------------------------------
//
CUsbWatcherServer* CUsbWatcherServer::NewLC()
    {
    LOG_FUNC

    CUsbWatcherServer* r = new ( ELeave ) CUsbWatcherServer();
    CleanupStack::PushL( r );
    r->StartL( KUsbWatcherServerName );
    r->ConstructL();
    return r;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CUsbWatcherServer::~CUsbWatcherServer()
    {
    LOG_FUNC

    delete iUsbWatcher;
    }

// ----------------------------------------------------------------------------
// C++ constructor
// ----------------------------------------------------------------------------
//
CUsbWatcherServer::CUsbWatcherServer()
     : CPolicyServer( EPriorityHigh, KUsbWatcherServerPolicy )
    {
    LOG_FUNC

    }

// ----------------------------------------------------------------------------
// Second-phase constructor
// ----------------------------------------------------------------------------
//
void CUsbWatcherServer::ConstructL()
    {
    LOG_FUNC

    iUsbWatcher = CUsbWatcher::NewL();
    }

// ----------------------------------------------------------------------------
// Create a new session
// ----------------------------------------------------------------------------
//
CSession2* CUsbWatcherServer::NewSessionL( const TVersion &aVersion,
        const RMessage2& aMessage ) const
    {
    LOG_FUNC

    (void)aMessage;//Remove compiler warning

    TVersion v( KUsbWatcherSrvMajorVersionNumber,
        KUsbWatcherSrvMinorVersionNumber, KUsbWatcherSrvBuildVersionNumber );

    if ( !User::QueryVersionSupported( v, aVersion ) )
        {
        LEAVE( KErrNotSupported );
        }

    CUsbWatcherServer* ncThis = const_cast< CUsbWatcherServer* >( this );

    CUsbWatcherSession* sess = CUsbWatcherSession::NewL( ncThis );

    return sess;
    }

// ----------------------------------------------------------------------------
// Error handle
// ----------------------------------------------------------------------------
//
void CUsbWatcherServer::Error( TInt aError )
    {
    LOG_FUNC

    LOG1( "Error = %d", aError );
    Message().Complete( aError );
    LOG( "Restarting..." );
    ReStart();
    }

// End of file
