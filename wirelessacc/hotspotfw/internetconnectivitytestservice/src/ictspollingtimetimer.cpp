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
* Description:   Timer of time that will be used to polling
*
*/



#include "ictspollingtimetimer.h"
#include "ictspollingtimeobserver.h"
#include "am_debug.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CIctsPollingTimeTimer::CIctsPollingTimeTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CIctsPollingTimeTimer::CIctsPollingTimeTimer( MIctsPollingTimeObserver& aObserver )
: CTimer( EPriorityStandard ), iObserver( aObserver )
    {
    
    }


// ---------------------------------------------------------------------------
// CIctsPollingTimeTimer::ConstructL
// ---------------------------------------------------------------------------
//
void CIctsPollingTimeTimer::ConstructL()
    {
    DEBUG("CIctsPollingTimeTimer::ConstructL()");
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------------------------
// CIctsPollingTimeTimer::NewL
// ---------------------------------------------------------------------------
//
CIctsPollingTimeTimer* CIctsPollingTimeTimer::NewL( MIctsPollingTimeObserver& aObserver )
    {
    CIctsPollingTimeTimer* self = new( ELeave ) CIctsPollingTimeTimer( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// ---------------------------------------------------------------------------
// CIctsPollingTimeTimer::~CIctsPollingTimeTimer
// ---------------------------------------------------------------------------
//
CIctsPollingTimeTimer::~CIctsPollingTimeTimer()
    {
    CTimer::Cancel();

    }

// ---------------------------------------------------------------------------
// CIctsPollingTimeTimer::RunL
// ---------------------------------------------------------------------------
//  
void CIctsPollingTimeTimer::RunL()
	{
	DEBUG("CIctsPollingTimeTimer::RunL()");
    iObserver.PollingTimeTimeout();
	}    
	
//  End of File  
