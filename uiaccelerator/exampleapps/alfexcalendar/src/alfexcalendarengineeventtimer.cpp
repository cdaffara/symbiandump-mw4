/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/



// INCLUDE FILES
#include "alfexcalendarengineeventtimer.h"

// CONSTANTS
const TReal KAlfExCalendarEngineArrayGranularity = 4;

// LOCAL CONSTANTS AND MACROS

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CAlfExCalendarEngineEventTimer::NewL
// ----------------------------------------------------
CAlfExCalendarEngineEventTimer* CAlfExCalendarEngineEventTimer::NewL(
    MAlfExCalendarEngineEventTimerObserver& aObserver)
    {
    CAlfExCalendarEngineEventTimer* self = 
        CAlfExCalendarEngineEventTimer::NewLC(aObserver);
    CleanupStack::Pop( self );
    return self;
    }
    
    
// ----------------------------------------------------
// CAlfExCalendarEngineEventTimer::NewLC
// ----------------------------------------------------
CAlfExCalendarEngineEventTimer* CAlfExCalendarEngineEventTimer::NewLC(
    MAlfExCalendarEngineEventTimerObserver& aObserver)
    {
    CAlfExCalendarEngineEventTimer* self = new (ELeave) CAlfExCalendarEngineEventTimer(aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
    
// ----------------------------------------------------
// CAlfExCalendarEngineEventTimer::CAlfExCalendarEngineEventTimer
// ----------------------------------------------------
CAlfExCalendarEngineEventTimer::CAlfExCalendarEngineEventTimer(
    MAlfExCalendarEngineEventTimerObserver& aObserver)
    :
    CActive(EPriorityLow),
    iObserver(aObserver)
    {
	CActiveScheduler::Add(this);
    }

    
// ----------------------------------------------------
// CAlfExCalendarEngineEventTimer::ConstructL
// ----------------------------------------------------
void CAlfExCalendarEngineEventTimer::ConstructL()
    {
	User::LeaveIfError(iTimer.CreateLocal());
    }

    
// ----------------------------------------------------
// CAlfExCalendarEngineEventTimer::~CAlfExCalendarEngineEventTimer
// ----------------------------------------------------
CAlfExCalendarEngineEventTimer::~CAlfExCalendarEngineEventTimer()
    {
	Cancel();
    iTimer.Close();
    }


// ----------------------------------------------------
// CAlfExCalendarEngineEventTimer::StartTimer
// ----------------------------------------------------
void CAlfExCalendarEngineEventTimer::StartTimer(TTimeIntervalMicroSeconds32 aDelay)
	{
	Cancel();
	iTimer.After( iStatus, aDelay );
	SetActive();
	}

// ----------------------------------------------------
// CAlfExCalendarEngineEventTimer::DoCancel
// ----------------------------------------------------
void CAlfExCalendarEngineEventTimer::DoCancel()
	{
	iTimer.Cancel();
	}


// ----------------------------------------------------
// CAlfExCalendarEngineEventTimer::RunL
// ----------------------------------------------------
void CAlfExCalendarEngineEventTimer::RunL()
	{
	if ( iStatus == KErrNone )
	    {
        iObserver.HandleTimerEventL();
	    }
	}

    
// ----------------------------------------------------
// CAlfExCalendarEngineEventTimer::RunError
// ----------------------------------------------------
TInt CAlfExCalendarEngineEventTimer::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}
	
	
// end of file    
