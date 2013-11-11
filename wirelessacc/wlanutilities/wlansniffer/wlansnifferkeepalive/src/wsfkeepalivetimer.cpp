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
* Timer implementation.
*/

// System include files

// User include files

#include "wsfkeepalivetimer.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wsfkeepalivetimerTraces.h"
#endif

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

CWsfKeepaliveTimer* CWsfKeepaliveTimer::NewL(
    MWsfKeepaliveTimerNotify& aNotify )
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVETIMER_NEWL_ENTRY );
    
    CWsfKeepaliveTimer* me =
        new ( ELeave ) CWsfKeepaliveTimer( aNotify );
    CleanupStack::PushL( me );
    me->ConstructL();
    CleanupStack::Pop( me );
    
    OstTraceFunctionExit0( CWSFKEEPALIVETIMER_NEWL_EXIT );
    return me;
    }
 
CWsfKeepaliveTimer::~CWsfKeepaliveTimer()
    {   
    OstTraceFunctionEntry0( DUP1_CWSFKEEPALIVETIMER_CWSFKEEPALIVETIMER_ENTRY );

    Cancel();
    iTimer.Close();

    OstTraceFunctionExit0( DUP1_CWSFKEEPALIVETIMER_CWSFKEEPALIVETIMER_EXIT );
    }
 
void CWsfKeepaliveTimer::After( TTimeIntervalMicroSeconds32 aInterval )
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVETIMER_AFTER_ENTRY );

    // Cancel a (possibly) ongoing timer before starting new one
    Cancel();
    iTimer.After( iStatus, aInterval );
    SetActive();

    OstTraceFunctionExit0( CWSFKEEPALIVETIMER_AFTER_EXIT );
    }
 
void CWsfKeepaliveTimer::Stop()
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVETIMER_STOP_ENTRY );

    iTimer.Cancel();

    OstTraceFunctionExit0( CWSFKEEPALIVETIMER_STOP_EXIT );
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CWsfKeepaliveTimer::CWsfKeepaliveTimer( MWsfKeepaliveTimerNotify& aNotify ) :
    CActive( EPriorityStandard ),
    iNotify( aNotify )
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVETIMER_CWSFKEEPALIVETIMER_ENTRY );
    OstTraceFunctionExit0( CWSFKEEPALIVETIMER_CWSFKEEPALIVETIMER_EXIT );
    }
 
// ---------------------------------------------------------------------------
// Leaving constructor
// ---------------------------------------------------------------------------
//
void CWsfKeepaliveTimer::ConstructL()
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVETIMER_CONSTRUCTL_ENTRY );
 
    CActiveScheduler::Add( this );
    User::LeaveIfError( iTimer.CreateLocal() );

    OstTraceFunctionExit0( CWSFKEEPALIVETIMER_CONSTRUCTL_EXIT );
    }
 
// ---------------------------------------------------------------------------
// Handles an active object's request completion event
// ---------------------------------------------------------------------------
//
void CWsfKeepaliveTimer::RunL()
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVETIMER_RUNL_ENTRY );
    
    iNotify.TimerExpired( iStatus.Int() );

    OstTraceFunctionExit0( CWSFKEEPALIVETIMER_RUNL_EXIT );
    }

// ---------------------------------------------------------------------------
// Implements cancellation of an outstanding request
// ---------------------------------------------------------------------------
//
void CWsfKeepaliveTimer::DoCancel()
    {
    OstTraceFunctionEntry0( CWSFKEEPALIVETIMER_DOCANCEL_ENTRY );
    
    iTimer.Cancel();

    OstTraceFunctionExit0( CWSFKEEPALIVETIMER_DOCANCEL_EXIT );
    }
