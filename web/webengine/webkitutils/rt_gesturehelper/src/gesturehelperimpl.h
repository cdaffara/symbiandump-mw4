/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Gesture helper implementation
*
*/


#ifndef _GESTUREHELPERIMPL_H_
#define _GESTUREHELPERIMPL_H_

#include <e32base.h>
#include <coemain.h>
#include <rt_gesturehelper.h>


struct TPointerEvent;

namespace RT_GestureHelper
{

class CCallbackTimer;
class CGesture;
class CPointerCapturer;
class MGestureObserver;
class CGestureEventFilter;
class CGestureEventSender;

/**
 * Implementation of CGestureHelper interface
 */
NONSHARABLE_CLASS( CGestureHelperImpl ) : public CBase
    {
public:
    /** 2-phase constructor */
    static CGestureHelperImpl* NewL( MGestureObserver& aObserver );
    
    /** Destructor */
    ~CGestureHelperImpl();    
    
    
    /** See @ref CGestureHelper::SetHoldingEnabled */
    void SetHoldingEnabled( TBool aEnabled );
    /** See @ref CGestureHelper::IsHoldingEnabled */
    TBool IsHoldingEnabled() const;
    /** See @ref CGestureHelper::SetDoubleTapEnabled */
    void SetDoubleTapEnabled( TBool aEnabled );
    /** See @ref CGestureHelper::IsDoubleTapEnabled */
    TBool IsDoubleTapEnabled() const;
    /** See @ref CGestureHelper::HandlePointerEventL */
    TBool HandlePointerEventL( const TPointerEvent& aEvent );
    
    /** Reset helper state */
    void Reset();
   
    void SetLastEventTime() {iLastEventTime.HomeTime(); } /*CCoeEnv::Static()->LastEvent().Time();*/ 
    
    
    TTime GetLastEventTime() const { return iLastEventTime; }
private:
    /** Constructor */
    CGestureHelperImpl( MGestureObserver& aObserver );

    
    
    TBool noneAlf_HandlePointerEventL( const TPointerEvent& aEvent);

    /** @return ETrue if the helper has not started processing a gesture */
    inline TBool IsIdle() const;
    /** Add a point to the sequence of points that make up the gesture */
    inline void AddPointL( const TPointerEvent& aEvent );    
    /** Add a point to the sequence of points that make up the gesture */
    inline TInt AddPoint( const TPointerEvent& aEvent );    
    /** Activates holding and notifies observer that holding has been started */
    void StartHoldingL();
    /* Set previous tag gesture to null (and recycle the gesture object) */
    static void RecyclePreviousTapGesture( TAny* aSelf );
    /** Emits a tap event since second tap of a double tap was not received within timeout */
    void EmitFirstTapEvent();
    /** Emit the start gesture event that aGesture would have produced */
    void EmitStartEventL( const CGesture& aGesture );
    /** 
     * Send code event if not holding. Also sets the event to released and emits again.
     * Modifies aGesture! 
     */
    void CompleteAndEmit( CGesture& aGesture );
    /** 
     * Send cancel event about the latest gesture client was notified about 
     * Sets the latest gesture to Cancelled.
     */
    void EmitCancelEvent();
    
    /**
     * Send Released event
     */
    void EmitReleasedEvent();
    
    /** Notify observer of the gesture */
    void EmitEvent( const CGesture& aGesture );
    /** 
     * (re)start holding timer at the current point.
     * Does not restart holding if the point (in aEvent) is near enough to where holding 
     * was started earlier. (Allows small movement but still starting holding.
     */
    void StartHoldingTimer( const TPointerEvent& aNewEvent );

    /** 
     * @return a gesture object from the gesture pool 
     * Getting gestures from the gesture pool avoids the need to create gestures
     * after construction. There is max only one gesture object in the pool, 
     * but no more is needed 
     */
    CGesture* NewGesture();
    /** Move the gesture into the gesture pool, and set aGesturePointer to null */
    void RecycleGesture( CGesture*& aGesturePointer );
    
    
    void HandleTouchUp(const TPointerEvent& aEvent);
    void HandleTouchDownL(const TPointerEvent& aEvent);
    void HandleMoveL(const TPointerEvent& aEvent);
    void HandleLongTouch();
    void EmitDoubleTapEvent();
    TBool IsMovementGesture(TGestureCode aCode);
    
private:
    /// Gesture is the logical representation of a sequence of points arriving
    /// at certain times
    /// This is the current gesture
    CGesture* iGesture;
    
    /// Tap gesture of the previous gesture. Used when double tap is enabled.
    /// The gesture is cached, since it cannot be emitted until gesture helper
    /// knows whether the next gesture forms a double tap
    /// @see CGestureHelperImpl::iDoubleTapTimer
    CGesture* iPreviousTapGesture;

    /// This is a recyclable object pool with just one object.
    /// This class needs one or two gesture objects at any one time. There is one
    /// less error condition to handle if gesture objects are not created after
    /// construction. Therefore, if only one gesture object is needed at a specific,
    /// point in time, the unused gesture object is stored here.
    CGesture* iUnusedGesture;
    
    /// When double tap is enabled, a tap
    /// event (and it's released event) is only emitted after the double tap timeout
    /// passes. If user taps again before the timeout (in the same position),
    /// this gesture is emitted as a double tap (followed by the released event, 
    /// and the second tap is not emitted at all.
    /// If user taps, and then swipes before the double tap timeout, this tap
    /// event (and it's released event) event is emitted, and only then the swipe's
    /// down and drag events are emitted. In other words, if user presses down
    /// during the timeout, the down event it not emitted until the gesture
    /// helper knows that the gesture is not another tap.
    /// Behavior while double tap enabled:
    /// Swipe: Down + drag + up => emits down + swipe/cancel + released
    /// Tap: Down + up => emits down ... timeout ... + tap + released
    /// Double tap: Down + up + down + up => emits down ... double tap + released
    /// Tap + quick swipe: Down + up + down + drag + ... => emits down ... tap + up (for tap) + down (for swipe ) + drag + ...
    /// Double tap timer is cancelled when the second pointer down event is received,
    /// i.e., it does not matter if user does this: 
    /// down + up + down ... long wait ... + up. It's still a double tap.
    CCallbackTimer* iDoubleTapTimer;
    
    /// holding is activated when user keeps pointer down in the same place for 
    /// a longer period. the holding timer activates off when enough time has passed.
    CCallbackTimer* iHoldingTimer;
    
    /// observer for gesture events
    MGestureObserver& iObserver;
    
    /// captures subsequent pointer events after pointer down received,
    /// until pointer up
    CPointerCapturer* iPointerCapturer;

    TTime  iLastEventTime;
    TTime  iLastTouchUpTime;
    TTime  iLastDoubleTapTime;
    
    CCallbackTimer*  iLongTouchTimer;
    CGestureEventFilter* iEventFilter;
    CGestureEventSender* iEventSender;
    };
    
} // GestureHelper

#endif // _GESTUREHELPERIMPL_H_
