/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Engine source file
 *
*/


#include "SVGTimer.h"
#include "SVGEngineImpl.h"
#include "GfxAffineTransform.h"
#include "GfxPoint2D.h"
#include "SVGTimeContainer.h"

// ---------------------------------------------------------------------------
// Two phase constructor
// ---------------------------------------------------------------------------
CSvgTimer* CSvgTimer::NewLC( CSvgTimeContainer* aTimeContainer )
    {
    CSvgTimer* self = new ( ELeave ) CSvgTimer( aTimeContainer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

//
// ---------------------------------------------------------------------------
// Two phase constructor
// ---------------------------------------------------------------------------
CSvgTimer* CSvgTimer::NewL( CSvgTimeContainer* aTimeContainer )
    {
    CSvgTimer* self = NewLC( aTimeContainer );
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// Private method that builds the heap objects or performs leavable actions
// during object construction
// ---------------------------------------------------------------------------
void CSvgTimer::ConstructL()
    {
    // Call the base class ConstructL
    CTimer::ConstructL();
    // Add to active scheduler
    CActiveScheduler::Add( this );
    }

//
// ---------------------------------------------------------------------------
// Constructor
// Initialized with a pointer to the time container
// ---------------------------------------------------------------------------
CSvgTimer::CSvgTimer( CSvgTimeContainer* aTimeContainer ) : 
    CTimer( CActive::EPriorityStandard ),
    iTimeContainer( aTimeContainer )
    {
    ChangeFrameDuration( KMinFrameDelay );
    SetStrictFrameDuration( EFalse );
    ResetTime();
    }

//
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CSvgTimer::~CSvgTimer()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Reset the time
// ---------------------------------------------------------------------------
void CSvgTimer::ResetTime()
    {
    iTime = 0;
    iFrames = 0;
    iFirstFrameDrawn = EFalse;
    }

// ---------------------------------------------------------------------------
// Change the duration of the timer in milliseconds
// ---------------------------------------------------------------------------
void CSvgTimer::ChangeFrameDuration( TUint32 aFrameDuration )
    {
    // If delay duration is less then minimum-delay, set to minimum-delay
    if ( aFrameDuration < KMinFrameDelay )
        {
        aFrameDuration =  KMinFrameDelay;
        }
    iFrameDuration = aFrameDuration;
    }

// ---------------------------------------------------------------------------
// Return the frames per 10-seconds
// ---------------------------------------------------------------------------
TUint CSvgTimer::Fps()
    {
	if ( iTime == 0 )
		{
		return 0;
		}
	// iTime is in milliseconds
	return ( 10000 * iFrames ) / iTime ;
    }


// ---------------------------------------------------------------------------
// Standard Active Object DoCancel
// ---------------------------------------------------------------------------
void CSvgTimer::DoCancel()
    {
    CTimer::DoCancel();
    }

// ---------------------------------------------------------------------------
// Standard Active Object RunL
// ---------------------------------------------------------------------------
void CSvgTimer::RunL()
    {
    // Check if timed entity not null
    if ( !iTimeContainer )
        {
        return;
        }
    
#if 0    
    // Check engine and document
    if ( !iSvgEngine || !iSvgEngine->Document() )
        {
        return;
        }

	// wait for images to finish decoding
    if ( !iSvgEngine->ReadyToRender() )
    	{
        CActive::Deque();
        CActiveScheduler::Add( this );
        CTimer::After( 10 * 1000 );
        return;
		}
#endif
    /********************** Generate first frame ********************/
    if ( !iFirstFrameDrawn )
    	{
        iFirstFrameDrawn = ETrue;
    	}
	else
		{
		/********************** After first frame ********************/
		TTime currentTime;
		currentTime.HomeTime();
		TUint32 elapsedTime = GetElapsedMilliseconds( iPreviousFrameTime, currentTime );

		// Cap elapsed time, if needed
		if ( iIsStrictFrameDuration &&
			 elapsedTime > iFrameDuration )
			{
			elapsedTime = iFrameDuration;
			}

		// Generate frame
		iTime += elapsedTime;
		}

    // Mark time of new frame
    iPreviousFrameTime.HomeTime();

    // Generate new frame
    TUint32 sleepTime = GenerateFrameL( iTime );

		// Sleep to next frame
	if ( 
	( iTimeContainer->Document()->Engine()->SVGEngineState() == ESVGEngineRunning ) 
	&& ( iTimeContainer->GetCurTick().iParentTcTick < iTimeContainer->Document()->AnimationDuration() ) 
	)
	
		{
        // Dequeue to move this ActiveObject to the end of the list
        // to share processor with other AOs.
		CActive::Deque();
		CActiveScheduler::Add( this );
		CTimer::After( 1000 * sleepTime );
		}
    }

// --------------------------------------------------------------------------
// void CSvgTimer::SVGStop()
// ---------------------------------------------------------------------------
void CSvgTimer::SVGStop()
    {
    if ( IsActive() )
        {
        Cancel(); // stop current timer
        }
    iFirstFrameDrawn = EFalse;
    }

// --------------------------------------------------------------------------
// void CSvgTimer::SVGResume()
// ---------------------------------------------------------------------------
void CSvgTimer::SVGResume( TInt32 aTime )
    {
    if ( !IsActive() )
        {
        if ( aTime < 0 )
            {
            // Error, ( negative time ) set to default
            aTime = 1000;
            }
            
        CTimer::After( aTime ); // invoke initial timer event
        }
    }

// --------------------------------------------------------------------------
// void CSvgTimer::SetStartTime()
// Reset animation time to zero
// ---------------------------------------------------------------------------
void CSvgTimer::SetStartTime()
    {
    SetSeekTime( 0 );
    }

// --------------------------------------------------------------------------
// void CSvgTimer::SetSeekTime( TUint32 aTime )
// ---------------------------------------------------------------------------
void CSvgTimer::SetSeekTime( TUint32 aTime )
    {
    // update the current time.
    iFirstFrameDrawn = EFalse;
#if 0
    // the current home time needs to be updated.
    if ( iSvgEngine && iSvgEngine->Document() && aTime != iTime )
        {
        TSvgTimerEvent timeEvent( aTime );
        iSvgEngine->Document()->Reset( &timeEvent );
        }
#endif
    iTime = aTime;

     if ( !IsActive() )
        {
        CTimer::After( 1000 );
        }

    }

// --------------------------------------------------------------------------
// void CSvgTimer::SetStrictFrameDuration( TBool aKeepStrictFrameDuration )
// ---------------------------------------------------------------------------
void CSvgTimer::SetStrictFrameDuration( TBool aKeepStrictFrameDuration )
    {
    iIsStrictFrameDuration = aKeepStrictFrameDuration;
    }

// --------------------------------------------------------------------------
// TUint32 CSvgTimer::GenerateFrameL( TUint32 aTime )
// ---------------------------------------------------------------------------
TUint32 CSvgTimer::GenerateFrameL( TUint32 aTime )
    {
    TTime startTime;
    startTime.HomeTime();
    TSvgTick lTick;
    lTick.iRealTimeTick = aTime;
    lTick.iParentTcTick = 0;
    
    iTimeContainer->ParentTimeContainerTick( lTick );
    
#if 0
    // Request to drawn frame at given time
    TSvgTimerEvent timeEvent( aTime );
    iSvgEngine->ProcessEventL( iSvgEngine->Document(), &timeEvent );
#endif
    TTime endTime;
    endTime.HomeTime();

    TUint32 elapsedTime = GetElapsedMilliseconds( startTime, endTime );

    // Calculate sleep time to next frame
    TUint32 sleepTime = KMinSleepDuration;
    if ( elapsedTime < iFrameDuration )
        {
        sleepTime = iFrameDuration - elapsedTime;
        if ( sleepTime < KMinSleepDuration )
            {
            sleepTime = KMinSleepDuration;
            }
        }

	iFrames++;
    return sleepTime;
    }

// --------------------------------------------------------------------------
// TUint32 CSvgTimer::GetElapsedMilliseconds( TTime& aStart, TTime& aEnd )
// ---------------------------------------------------------------------------
TUint32 CSvgTimer::GetElapsedMilliseconds( TTime& aStart, TTime& aEnd )
    {
    TTimeIntervalMicroSeconds interval64 = aEnd.MicroSecondsFrom( aStart );
    // in milliseconds
    return I64INT( interval64.Int64() ) / 1000;
    }
