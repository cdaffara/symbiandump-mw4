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
* Description:   Timer for client login procedures
*
*/



#include "hsslogintimer.h"
#include "hsslogintimerobserver.h"
#include "am_debug.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CHssLoginTimer::CHssLoginTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CHssLoginTimer::CHssLoginTimer( MHssLoginTimerObserver& aObserver )
: CTimer( EPriorityStandard ), iObserver( aObserver )
    {
    
    }


// ---------------------------------------------------------------------------
// CHssLoginTimer::ConstructL
// ---------------------------------------------------------------------------
//
void CHssLoginTimer::ConstructL()
    {
    DEBUG("CHssLoginTimer::ConstructL()");
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------------------------
// CHssLoginTimer::NewL
// ---------------------------------------------------------------------------
//
CHssLoginTimer* CHssLoginTimer::NewL( MHssLoginTimerObserver& aObserver )
    {
    CHssLoginTimer* self = new( ELeave ) CHssLoginTimer( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// ---------------------------------------------------------------------------
// CHssLoginTimer::~CHssLoginTimer
// ---------------------------------------------------------------------------
//
CHssLoginTimer::~CHssLoginTimer()
    {
    CTimer::Cancel();
    }

// ---------------------------------------------------------------------------
// CHssLoginTimer::RunL
// ---------------------------------------------------------------------------
//  
void CHssLoginTimer::RunL()
	{
	DEBUG("CHssLoginTimer::RunL()");
    iObserver.LoginTimeout();
	}    
	
//  End of File  
