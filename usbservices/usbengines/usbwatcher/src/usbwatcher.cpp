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
* Description:  Entry point for UsbWatcher
*
*/


#include <c32comm.h>
#include "cusbwatcher.h"
#include "cusbwatcherscheduler.h"
#include "cusbwatcherserver.h"
#include "usbwatchershared.h"

// LITERALS
_LIT( KUsbWatcherName, "UsbWatcher" );

// LOCAL FUNCTION PROTOTYPES
static void StartUsbWatcherL();

// ============================= LOCAL FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Entry-point for the USB Watcher.
// ----------------------------------------------------------------------------
//
TInt E32Main()
    {
    LOG_FUNC

    // rename the thread so it is easy to find the panic application
    TInt ret = User::RenameThread( KUsbWatcherName );
    if( KErrNone != ret ) // Not fatal
        {
        LOG1( "ERROR: User::RenameThread = %d", ret );
        }

    __UHEAP_MARK;
    // create clean-up stack
    CTrapCleanup* cleanup = CTrapCleanup::New();

    TRAP( ret, StartUsbWatcherL() );
    if ( KErrAlreadyExists == ret )
        {
        LOG( "UsbWatcher is already running." );
        }
    else if ( KErrNone != ret )
        {
        LOG1( "ERROR: StartUsbWatcherL = %d", ret );
        }

    delete cleanup; // destroy clean-up stack
    __UHEAP_MARKEND;

    LOG1( "E32Main = %d", ret );
    return ret;
    }

// ----------------------------------------------------------------------------
// Constructs and installs the active scheduler, constructs USB Watcher's
// objects.
// ----------------------------------------------------------------------------
//
static void StartUsbWatcherL()
    {
    LOG_FUNC
    LOG( "Starting USB Watcher..." );

    LOG( "Create CUsbWatcherScheduler..." );
    // Construct and install the active scheduler
    CUsbWatcherScheduler *myScheduler = CUsbWatcherScheduler::NewL();

    // Push onto the cleanup stack
    CleanupStack::PushL( myScheduler );

    LOG( "Install ActiveScheduler..." );
    // Install as the active scheduler
    CActiveScheduler::Install( myScheduler );

    CUsbWatcherServer* usbwatcher;

    LOG( "Create CUsbWatcherServer..." );
    usbwatcher = CUsbWatcherServer::NewLC();
    RProcess::Rendezvous(KErrNone);
        
    LOG( "Set server..." );
	myScheduler->SetServer( *usbwatcher );


    LOG( "Start Active scheduler..." );
    CActiveScheduler::Start();

    LOG( "Cleanup CUsbWatcherServer and CUsbWatcherScheduler ..." );
    CleanupStack::PopAndDestroy( 2, myScheduler ); //usbwatcher, myScheduler
    }

// End of file
