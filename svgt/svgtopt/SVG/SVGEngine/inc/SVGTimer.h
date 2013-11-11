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
* Description:  SVG Engine header file
 *
*/


#ifndef __INC_CSVGTIMER__
#define __INC_CSVGTIMER__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif
#include "GfxFloatFixPt.h"

// Maximum frame rate: 15 fps
const TUint32 KMinFrameDelay = 33;       // milliseconds
const TUint32 KMinSleepDuration = 1;     // milliseconds

// Minimum sleep duration in msecs
const TUint32 KMinSleepDurationInMsecs = 1000;

class CSvgEngineImpl;
class CSvgTimeContainer;

/**
 * A Timer class that wraps Symbian CTimer class
 * All the essential functionality is same
 * Adds additional capabilities such as frames, frames per second information,
 * ability to specify/change duration etc.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgTimer : public CTimer
    {
    public:

        /**
         * Two phase constructor
         *
         * @since 1.0
         * @param aTimeContainer - Pointer to the Time Container
         * @return An instance of the CSvgTimer
         */
        static CSvgTimer*   NewLC( CSvgTimeContainer* 
            aTimeContainer );

        /**
         * Two phase constructor
         *
         * @since 1.0
         * @param aTimeContainer - Pointer to the Time Container
         * @return An instance of the CSvgTimer
         */
        static CSvgTimer*   NewL( CSvgTimeContainer* 
            aTimeContainer );
            
        /**
         * Destructor
         *
         * @since 1.0
         * @param None
         * @return None
         */
                            ~CSvgTimer();

    public:
        // Defined as pure virtual by CActive;

        /**
         * Standard Active Object DoCancel
         *
         * @since 1.0
         * @param None
         * @return None
         */
        void                DoCancel();

        /**
         * Standard Active Object RunL
         *
         * @since 1.0
         * @param
         * @return
         */
        void                RunL();


       /**
         * Reset the time
         *
         * @since 1.0
         * @param None
         * @return None
         */
        void    ResetTime();

        /**
         * CHange the duration of the timer
         *
         * @since 1.0
         * @param aTimerDuration - An integer that is the new duration
         * @return None
         */
        void    ChangeFrameDuration( TUint32 aTimerDuration );


        /**
         * Return the frames per second information
         *
         * @since 1.0
         * @param None
         * @return An integer that is the number of frames per second
         */
        TUint   Fps();

        /**
         * Resume function
         *
         * @since 1.0
         * @param aTime Time in msecs to resume after(Must be Positive)
         * @return An integer that is the number of frames per second
         */
        void   SVGResume( TInt32 aTime );

        /**
         * Pause function
         *
         * @since 1.0
         * @param None
         * @return An integer that is the number of frames per second
         */
        void   SVGStop();

        /**
         * Set Start Time for animation
         *
         * @since 1.0
         * @param None
         * @return None
         */
        void SetStartTime();

        /**
         * Set the timer to forward/rewind animation
         *
         * @since 1.0
         * @param aTime - in microseconds
         * @return None
         */
        void SetSeekTime( TUint32 aTime );

        /**
         * Set whether each frame generated should be time-base or
         * frame-based.
         *
         * @since 1.0
         * @param aKeepStrictFrameDuration - if ETrue then
         * every cycle of RunL will generate a frame with the elapsed
         * time set by ChangeFrameDuration().
         * @return None
         */
        void SetStrictFrameDuration( TBool aKeepStrictFrameDuration );

    protected:

        /**
         * Private constructor
         *
         * @since 1.0
         * @param aSvgEngine - A pointer to the SVG Engine
         * @return None
         */
         CSvgTimer( CSvgTimeContainer* 
            aTimeContainer );
        /**
         * A private constructor that constructs heap objects
         *
         * @since 1.0
         * @param None
         * @return None
         */
        void                ConstructL();

    private:
        static TUint32 GetElapsedMilliseconds( TTime& aStart, TTime& aEnd );

        /**
         * Request svg engine to generate frame at the given time in
         * milliseconds
         *
         * @since 1.0
         * @param aTime - milliseconds
         * @return None
         */
        TUint32 GenerateFrameL( TUint32 aTime );


        CSvgTimeContainer* iTimeContainer;

        TUint32             iTime;          // in millisecond
        TUint32             iFrameDuration; // in millisecond
        TTime               iPreviousFrameTime;

        TUint32             iFrames;        // for debug. counting FPS

        // Frame-based or time-based (default)
        TBool               iIsStrictFrameDuration;

        TBool               iFirstFrameDrawn;
    };

#endif //__INC_CSVGTIMER__
