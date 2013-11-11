/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   THuiInteractionInterval is a utility class that can be used 
*                as specify intervals that adapt to user input.
*
*/

 

#ifndef __HUIINTERACTIONINTERVAL_H__
#define __HUIINTERACTIONINTERVAL_H__


#include <e32base.h>


/**
 * THuiInteractionInterval is a utility class that can be used to determine
 * the correct animation interval time, while taking into account how much time has
 * elapsed since the last time the interval was determined. 
 *
 * An interaction interval is useful for example when animating a list selector. By default,
 * the time interval for moving between two items could be 500 ms, but if
 * the user moves faster than this, the selector's animation may be 
 * unnecessarily delayed. In this situation, the interaction interval would
 * shorten the time interval for the moving animation, if necessary, to keep
 * better in sync with how quickly the user is moving in the list.
 */
class THuiInteractionInterval 
    {
public:

    /* Constructors and destructor. */

    /**
     * Initializes the interaction interval.
     *
     * @param aScalar  Determines how strongly the interval adapts. 
     *
     * @see SetScalar()
     */
    IMPORT_C THuiInteractionInterval(TReal32 aScalar = 2.0f) __SOFTFP;


    /* Methods. */

    /**
     * Sets the adaptation scalar. Determines how strongly the interval
     * will adapt to the elapsed time since the last evaluation.
     *
     * For example, 1.0 would mean that the maximum interval time is the 
     * time elapsed between the current evaluation and the
     * previous one. In this case, when the animation frequency 
     * stays the same, each sequence would have time to complete
     * before the next sequence begins. When the scalar is 2.0, the
     * maximum is twice the elapsed time, meaning that the 
     * ongoing animation sequence would always be finished halfway 
     * when the next sequence begins. This produces a smoother
     * end result.
     *
     * @param aScalar  New adaptation scalar.
     */
    IMPORT_C void SetScalar(TReal32 aScalar) __SOFTFP;
    
    /**
     * Returns the current adapation scalar.
     *
     * @return Adaptation scalar.
     */
    IMPORT_C TReal32 Scalar() const __SOFTFP;

    /**
     * Evaluates the current interval. The evaluated interval will always 
     * be at least as large as the elapsed time since the last evaluation, 
     * but at most <code>aIntervalTime</code>. 
     *
     * @param aIntervalTime  The requested normal interval time.
     *
     * @return  The evaluated interval, effective during the current frame.
     *
     * @see SetScalar()
     */
    IMPORT_C TInt Interval(TInt aIntervalTime);
    
    
private:

    /** Interval scalar. */
    TReal32 iScalar;
    
    /** Last time interaction interval was determined. */
    TUint32 iLastTimeMs;

    };


#endif  // __HUIINTERACTIONINTERVAL_H__
