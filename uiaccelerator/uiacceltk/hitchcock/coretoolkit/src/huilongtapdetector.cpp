/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Class which detects the long pointer press.
*
*/



#include "huilongtapdetector.h"
#include "uiacceltk/HuiRoster.h"
#include "uiacceltk/HuiUtil.h"

const TInt KHuiLongTapDelay = 800000;              // 0,8 seconds

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CHuiLongTapDetector::CHuiLongTapDetector(CHuiRoster& aRoster)
 : CTimer( EPriorityStandard ), iRoster( aRoster ), iPreviousPointerEvent(0)
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CHuiLongTapDetector::ConstructL()
    {
    CTimer::ConstructL();
    }



// ---------------------------------------------------------------------------
// Two phased constructor
// ---------------------------------------------------------------------------
//
CHuiLongTapDetector* CHuiLongTapDetector::NewL(CHuiRoster& aRoster)
    {
    CHuiLongTapDetector* self = new( ELeave ) CHuiLongTapDetector( aRoster );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHuiLongTapDetector::~CHuiLongTapDetector()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Called when a pointer event is received
// ---------------------------------------------------------------------------
//
void CHuiLongTapDetector::OfferPointerEvent( const THuiEvent& aEvent )
    {
    if ( aEvent.IsPointerEvent() )
        {
        // Store the coordinates.
        iPreviousPointerEvent = aEvent;
        
        // On pointer down, start the timer
        if ( aEvent.PointerDown() )
            {
            Cancel();
            After( KHuiLongTapDelay );
            }
            
        // Cancel the timed on pointer up
        if ( aEvent.PointerUp() )
            {
            Cancel();
            }
        }
    }

// ---------------------------------------------------------------------------
// From class CActive
// Called when timer expires.
// ---------------------------------------------------------------------------
//
void CHuiLongTapDetector::RunL()
    {
    if ( iStatus == KErrNone )
        {
        // Post the repeat event to back to the roster.
        iPreviousPointerEvent.iPointerEvent.iType = TPointerEvent::EButtonRepeat;
        iRoster.HandleEventL( iPreviousPointerEvent );
        }
    }

// ---------------------------------------------------------------------------
// From class CActive
// Called when RunL leaves.
// ---------------------------------------------------------------------------
//
TInt CHuiLongTapDetector::RunError( TInt aError )
    {
    HUI_DEBUG1(_L("CHuiLongTapDetector::RunError() - Ignore error %i."), aError);
    (void)aError; // for compiler
    return KErrNone;
    }



