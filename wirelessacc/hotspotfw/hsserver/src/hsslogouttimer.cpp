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
* Description:   Timer for client logout procedures
*
*/



#include "hsslogouttimer.h"
#include "hsslogouttimerobserver.h"
#include "am_debug.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CHssLogoutTimer::CHssLogoutTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CHssLogoutTimer::CHssLogoutTimer( MHssLogoutTimerObserver& aObserver )
: CTimer( EPriorityStandard ), iObserver( aObserver )
    {
    
    }


// ---------------------------------------------------------------------------
// CHssLogoutTimer::ConstructL
// ---------------------------------------------------------------------------
//
void CHssLogoutTimer::ConstructL()
    {
    DEBUG("CHssLogoutTimer::ConstructL()");
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------------------------
// CHssLogoutTimer::NewL
// ---------------------------------------------------------------------------
//
CHssLogoutTimer* CHssLogoutTimer::NewL( MHssLogoutTimerObserver& aObserver )
    {
    CHssLogoutTimer* self = new( ELeave ) CHssLogoutTimer( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// ---------------------------------------------------------------------------
// CHssLogoutTimer::~CHssLogoutTimer
// ---------------------------------------------------------------------------
//
CHssLogoutTimer::~CHssLogoutTimer()
    {
    CTimer::Cancel();
    }

// ---------------------------------------------------------------------------
// CHssLogoutTimer::RunL
// ---------------------------------------------------------------------------
//  
void CHssLogoutTimer::RunL()
	{
	DEBUG("CHssLogoutTimer::RunL()");
    iObserver.LogoutTimeout();
	}    
	
//  End of File  
