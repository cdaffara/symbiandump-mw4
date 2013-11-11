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
* Description:   Timer for polling interval
*
*/



#include "ictspollingintervaltimer.h"
#include "ictspollingintervalobserver.h"
#include "am_debug.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CIctsPollingIntervalTimer::CIctsPollingIntervalTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CIctsPollingIntervalTimer::CIctsPollingIntervalTimer( MIctsPollingIntervalObserver& aObserver )
: CTimer( EPriorityStandard ), iObserver( aObserver )
    {
    
    }

// ---------------------------------------------------------------------------
// CIctsPollingIntervalTimer::ConstructL
// ---------------------------------------------------------------------------
//
void CIctsPollingIntervalTimer::ConstructL()
    {
    DEBUG("CIctsPollingIntervalTimer::ConstructL()");
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------------------------
// CIctsPollingIntervalTimer::NewL
// ---------------------------------------------------------------------------
//
CIctsPollingIntervalTimer* CIctsPollingIntervalTimer::NewL( MIctsPollingIntervalObserver& aObserver )
    {
    CIctsPollingIntervalTimer* self = new( ELeave ) CIctsPollingIntervalTimer( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// ---------------------------------------------------------------------------
// CIctsPollingIntervalTimer::~CIctsPollingIntervalTimer
// ---------------------------------------------------------------------------
//
CIctsPollingIntervalTimer::~CIctsPollingIntervalTimer()
    {
    CTimer::Cancel();
    }

// ---------------------------------------------------------------------------
// CIctsPollingIntervalTimer::RunL
// ---------------------------------------------------------------------------
//  
void CIctsPollingIntervalTimer::RunL()
	{
	DEBUG("CIctsPollingIntervalTimer::RunL()");
    iObserver.PollingIntervalTimeout();
	}    
	
//  End of File
