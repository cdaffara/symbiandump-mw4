/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CUpnpNotifyTimer
*
*/


// INCLUDE FILES

#include "upnpnotifytimer.h"
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpNotifyTimer::CUpnpNotifyTimer
// C++ default constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpNotifyTimer::CUpnpNotifyTimer(MUpnpNotifyTimerObserver* aObserver)
	: CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    iObserver = aObserver;
    }

// ----------------------------------------------------------------------------
// CUpnpNotifyTimer::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpNotifyTimer* CUpnpNotifyTimer::NewL(MUpnpNotifyTimerObserver* aObserver)
    {
    CUpnpNotifyTimer* self  = new (ELeave) CUpnpNotifyTimer(aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CUpnpNotifyTimer::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CUpnpNotifyTimer::ConstructL()
    {
    TInt err = iTimer.CreateLocal();
    if ( KErrNone != err )
        {
        LOGS1("CUpnpNotifyTimer::CUpnpNotifyTimer() CreateLocal FAILED: %d", err );
        User::Leave( err );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpNotifyTimer::~CUpnpNotifyTimer
// C++ default destructor
// -----------------------------------------------------------------------------
//
CUpnpNotifyTimer::~CUpnpNotifyTimer()
    {
    iObserver=NULL;
    Cancel();
    iTimer.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpNotifyTimer::After
// Starts timer
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpNotifyTimer::After( TInt aInterval, TBool /*aUnused*/ )
    {
    Start( TTimeIntervalMicroSeconds32( aInterval ) );
    }

// -----------------------------------------------------------------------------
// CUpnpNotifyTimer::After
// Starts timer
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpNotifyTimer::After( TTimeIntervalMicroSeconds32 aInterval,
                                  TBool /*aUnused*/ )
    {
    Start( aInterval );
    }

// -----------------------------------------------------------------------------
// CUpnpNotifyTimer::AfterSeconds
// Start timer
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpNotifyTimer::AfterSeconds( TInt aIntervalInSeconds )
    {
    if( !IsActive() )
        {
        ASSERT( 0 == iIterationsToRun );
        TInt secondsToWait = aIntervalInSeconds;
        if ( aIntervalInSeconds > KOneIterationSeconds )
            {
            iIterationsToRun = aIntervalInSeconds / KOneIterationSeconds;
            secondsToWait = aIntervalInSeconds % KOneIterationSeconds;
            }
        iTimer.After(  iStatus, TTimeIntervalMicroSeconds32( secondsToWait * KSecond ) );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpNotifyTimer::Start
// Start timer
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpNotifyTimer::Start( TTimeIntervalMicroSeconds32 aInterval,
                                  TBool /*aUnused*/ )
    {
    if ( !IsActive() )
        {
        ASSERT( 0 == iIterationsToRun );
        iTimer.After( iStatus, aInterval );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpNotifyTimer::RunL
// Timer RunL
// -----------------------------------------------------------------------------
//
void CUpnpNotifyTimer::RunL()
    {
    LOGS1( "CUpnpNotifyTimer::RunL() iStatus: %d", iStatus.Int() );
	User::LeaveIfError(iStatus.Int());

    if ( iIterationsToRun > 0 )
        {
        StartNextIteration();
        }
    else
        {
        if ( iObserver )
            {
        	iObserver->TimerEventL( this );
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpNotifyTimer::StartNextIteration
// only for internal use decrease repeat counter and
// waits for KOneIterationSeconds seconds
// This function should be run when object isn't active.
// --------------------------------------------------------------------- --------
//
void CUpnpNotifyTimer::StartNextIteration()
    {
    ASSERT( !IsActive() );
    iIterationsToRun--;
    iTimer.After( iStatus,
    TTimeIntervalMicroSeconds32( KOneIterationSeconds * KSecond ) );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CUpnpNotifyTimer::RunError
// RunError in case RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CUpnpNotifyTimer::RunError( TInt aError )
    {
    LOGS1("CUpnpNotifyTimer::RunError, error %d", aError);
    iObserver->TimerEventError( this, aError );
    iIterationsToRun = 0;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpNotifyTimer::DoCancel
// Timer DoCanel for active timers
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpNotifyTimer::DoCancel()
    {
    iIterationsToRun = 0;
    iTimer.Cancel();
    }

// -----------------------------------------------------------------------------
// CUpnpNotifyTimer::CompleteOneselfImmediate
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpNotifyTimer::CompleteOneselfImmediate()
    {
    if ( !IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    }


// End Of File
