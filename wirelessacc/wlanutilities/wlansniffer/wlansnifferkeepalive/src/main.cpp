/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Wlan Sniffer Keepalive main function.
*/

// System include files

#include <e32base.h>
#include <e32std.h>

// User include files

#include "wsfkeepalive.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "mainTraces.h"
#endif

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Main implementation run in the active scheduler.
// ---------------------------------------------------------------------------
//
LOCAL_C void MainL()
    {
    OstTraceFunctionEntry0( WSFKEEPALIVEMAINL_ENTRY );
    
    // Create the main implementation class and start monitoring events
    CWsfKeepalive* implementation = CWsfKeepalive::NewL();
    
    // Start active scheduler
    CActiveScheduler::Start();
    
    // All finished, cleanup
    delete implementation;

    OstTraceFunctionExit0( WSFKEEPALIVEMAINL_EXIT );
    }

// ---------------------------------------------------------------------------
// Starts main implementation.
// ---------------------------------------------------------------------------
//
LOCAL_C void DoStartL()
    {
    OstTraceFunctionEntry0( WSFKEEPALIVEDOSTARTL_ENTRY );
    
    // Create active scheduler (to run active objects)
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    MainL();

    // Delete active scheduler
    CleanupStack::PopAndDestroy( scheduler );

    OstTraceFunctionExit0( WSFKEEPALIVEDOSTARTL_EXIT );
    }

// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Main function of the application executable.
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    OstTraceFunctionEntry0( WSFKEEPALIVEE32MAIN_ENTRY );

    // Create cleanup stack
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();

    // Run application code inside TRAP harness
    TRAPD( mainError, DoStartL() );
    if ( mainError )
        {
        OstTrace1(
            TRACE_FATAL,
            _E32MAIN_ERROR,
            "::E32Main Error;mainError=%d",
            mainError );
        }
    
    delete cleanup;
    __UHEAP_MARKEND;
    
    OstTraceFunctionExit0( WSFKEEPALIVEE32MAIN_EXIT );
    return KErrNone;
    }
