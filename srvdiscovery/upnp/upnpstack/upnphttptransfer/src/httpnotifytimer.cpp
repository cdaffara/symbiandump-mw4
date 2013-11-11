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
* Description:  Session timeout
*
*/


// INCLUDE FILES

#include "httpnotifytimer.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpNotifyTimer::CHttpNotifyTimer
// C++ default constructor
// -----------------------------------------------------------------------------
//
CHttpNotifyTimer::CHttpNotifyTimer(MHttpNotifyTimerObserver* aObserver)
	: CActive(EPriorityStandard)
	{	
	CActiveScheduler::Add(this);
	iObserver = aObserver;
	}

// ----------------------------------------------------------------------------
// CHttpNotifyTimer::NewL
// -----------------------------------------------------------------------------
//
CHttpNotifyTimer* CHttpNotifyTimer::NewL(MHttpNotifyTimerObserver* aObserver)
    {
    CHttpNotifyTimer* self  = new (ELeave) CHttpNotifyTimer(aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CHttpNotifyTimer::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CHttpNotifyTimer::ConstructL()
    {
    TInt err = iTimer.CreateLocal();
    if ( KErrNone != err )
        {
        User::Leave( err );
        }
    }
// -----------------------------------------------------------------------------
// CHttpNotifyTimer::~CHttpNotifyTimer
// C++ default destructor
// -----------------------------------------------------------------------------
//
CHttpNotifyTimer::~CHttpNotifyTimer()
	{
	iObserver=NULL;
	Cancel();
	iTimer.Close();
	}

// -----------------------------------------------------------------------------
// CHttpNotifyTimer::AfterSeconds
// Start timer
// -----------------------------------------------------------------------------
//
void CHttpNotifyTimer::AfterSeconds( TInt aIntervalInSeconds )
	{
	if ( !IsActive() )
		{
		TUint32 microsec = aIntervalInSeconds * KSecond;
		iTimer.After( iStatus, microsec );
		SetActive();
		}
	}

// -----------------------------------------------------------------------------
// CHttpNotifyTimer::RunL
// Timer RunL
// -----------------------------------------------------------------------------
//
void CHttpNotifyTimer::RunL()
	{
	if ( iObserver )
	    {
	    TRAP_IGNORE( iObserver->TimerEventL( this ) );
	    }
	}

// -----------------------------------------------------------------------------
// CHttpNotifyTimer::RunError
// RunError in case RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CHttpNotifyTimer::RunError( TInt /*aError*/ )
	{
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CHttpNotifyTimer::DoCancel
// Timer DoCanel for active timers
// -----------------------------------------------------------------------------
//
void CHttpNotifyTimer::DoCancel()
	{
	iTimer.Cancel();
	}

// End Of File
