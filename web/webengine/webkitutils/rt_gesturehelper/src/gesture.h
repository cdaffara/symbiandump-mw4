/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Gesture class
*
*/


#ifndef _GESTURE_H_
#define _GESTURE_H_

#include <e32base.h>

#include "rt_gestureobserver.h"
#include "gesturerecogniser.h"
#include "pointarray.h"

namespace RT_GestureHelper
{


enum TEventState
    {     
    ENoEvent = 0,
    ETouchDown,            
    ETouchUp,      
    EMove,
    ELongTouchDown
    };

/**  
 * TGesture represents the gesture that the user has made.
 * Implements MGestureEvent interface to allow clients to 
 * inspect the gesture
 */
NONSHARABLE_CLASS( CGesture ) : public CBase
    {
public:  
    /** destructor */
    ~CGesture();
    
    /** 
     * @return a new gesture that will have only the first point
     * of this gesture. Can be used to create gesture's a "start" event
     * after some other points have been received. 
     */ 
    CGesture* AsStartEventLC() const;
    
    /** 
     * Sets the gesture as empty. resets timers to receive points
     * immediately
     */
    void Reset();
    
    /** @return ETrue if the gesture has no points */
    TBool IsEmpty() const;
    
    /**
     * Add a point to the sequence of points that forms the gesture
     * Call Reset() just before adding the first point
     * @param aPoint the point to add
     * @return error code
     */
    TInt AddPoint( const TPoint& aPoint, const TTime& eventtime);
    
    /** Set the visual of this gesture. Should be called when gesture starts */
    
    /** @return ETrue if the point is very near the holding point */
    TBool IsNearHoldingPoint( const TPoint& aPoint ) const;
    /** Set the latest point as the holding point */
    void SetHoldingPoint();
    /** @return ETrue if aPoint is the same as point added last */
    TBool IsLatestPoint( const TPoint& aPoint ) const;
    
    /** 
     * After call, Code(...) will return appropriate holding gesture code.
     */
    void StartHolding();
    /** 
     * After call, Code(...) will return a "hold released" gesture code when
     * the gesture is completed. Meanwhile, code will be drag.
     */
    void ContinueHolding();
    /** Set as stylus released */
    void SetReleased();
    /** Set the gesture as complete. Gesture is completed at pointer up */
    void SetComplete();
    /** After call, Code(...) will return a "cancelled" gesture code */
    void SetCancelled();
    
    /** force returning a code as double tap when gesture is complete */
    void SetDoubleTap();
    
    /** 
     * @return Whether the current gesture is a tap (and hence not a swipe)
     *         Does not consider holding or other state information (such as
     *         whether the gesture was just started or is dragging).  
     *         Results are based purely on the current stream of points.
     */ 
    TBool IsTap() const;
    
    void SetLongTap(TBool aLongTap);
    
// From MGestureEvent
    TTime TimeOfLastEntry() const;
    inline TGestureCode PreviousGestureCode() { return iPrevGestureCode; };
    TGestureCode Code( TAxis aRelevantAxis );
    TBool IsHolding() const;
    TPoint StartPos() const;
    TPoint CurrentPos() const; 
    TRealPoint Speed() const;
    TRealPoint SpeedPercent( const TRect& aEdges ) const;
    TPoint Distance() const; 
    
    
    
    
private:
    /** @return elapsed time between the latest and previous points */
    inline TTimeIntervalMicroSeconds32 TimeFromPreviousPoint() const; 
    
    /** @return elapsed time between the latest and first points */
    inline TTimeIntervalMicroSeconds32 TimeFromStart() const; 
    
    /** @return ETrue if user has stopped moving the stylus before lifting it */
    inline TBool IsMovementStopped() const;
    
    /** @return aOffset'th last entry in the list of points */
    inline const TPointEntry& NthLastEntry( TInt aOffset ) const;
    
    /** @return previous point (may not be exactly the point before last) */
    inline const TPointEntry& PreviousEntry() const;
    
    /** @return position of point received just before the latest point */
    inline TPoint PreviousPos() const;
    
    
    
    TBool IsFlick() const;
private:
    /// sequence of points that make up the gesture. own.
    RArray< TPointEntry > iPoints;
    /// index in iPoints of the point in which user started holding
    TInt iHoldingPointIndex;

    /// the different states that indicate whether user has made a holding gesture 
    enum THoldingState
        {
        ENotHolding = 0, // before holding activated (i.e., holding not activated)
        EHoldStarting,   // holding activated just now (lasts just one callback round)
        EHolding         // holding activated earlier 
        };
  
    /// state that indicates is user has held the pointer down in one position 
    /// for long time to indicate a holding gesture    
    THoldingState iHoldingState;
    
    /// Whether the gesture completion code will be forcefully overridden to be double tap,
    /// regardless of what the points are
    TBool iIsDoubleTap;
    
    enum TCompletionState 
        {
        // Note: implementation assumes this order of enums
        ENotComplete = 0,
        ECancelled,
        EComplete,
        EReleased
        };
    /// gesture only becomes complete when user lifts the pointer
    TCompletionState iState;

    /// time when the stylus was lifted.
    TTime iCompletionTime;
    
    /// visual relevant to this gesture. null if not known.
    
    TGestureCode iPrevGestureCode;
    
    TBool iIsLongTap;
    TBool iIsStopFlick;
    };

} // namespace GestureHelper

#endif // _GESTURE_H_
