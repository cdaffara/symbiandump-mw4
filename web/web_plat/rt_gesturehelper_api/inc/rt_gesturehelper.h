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
* Description:  RT Gesture helper interface
*
*/


#ifndef _RT_GESTUREHELPER_H_
#define _RT_GESTUREHELPER_H_

#warning The RT Gesture API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

#include <e32base.h>
#include <rt_gestureobserver.h>

class CAlfEnv;
class CAlfDisplay;
class TAlfEvent;
struct TPointerEvent;

namespace RT_GestureHelper
{

class CGestureHelperImpl;

/** 
 * Gesture helper provides functionality to convert a stream of pointer events 
 * into a logical gesture, and to assist clients in calculation of gesture 
 * speed and distance.
 * 
 * Interfaces
 *  - CGestureHelper is a parser of pointer events. Recognises gestures from 
 *    a sequence of pointer events, and converts them to gesture commands. 
 *    For documentation of callbacks, see @ref MGestureObserver. 
 *  - MGestureObserver allows clients to get notified of gestures
 *  - MGestureEvent represents the gesture (event)
 */
NONSHARABLE_CLASS( CGestureHelper ) : public CBase
    {
public:
    /** 
     * The helper is expected to be a member variable, hence NewLC is not provided 
     */
    IMPORT_C static CGestureHelper* NewL( MGestureObserver& aObserver );
    
    /** Destructor */
    IMPORT_C ~CGestureHelper();

    /**
     * Specify whether the helper will send hold events. By default, holding is enabled.
     * "Hold" means user has held stylus/finger on the same position for a longer duration.
     * 
     * Clients that require holding to be treated with no special meaning should disable 
     * holding to simplify their event handling. 
     * For example, assume the client uses swipe left/right to control navigation, and 
     * holding is enabled. If user holds, the client gets hold left/right
     * event, and no swipe event. If user now drags left or right while still pressing 
     * stylus down, the client will not get a swipe event (since it already received a hold
     * event). Upon release event, the client no reliable way of knowing whether the 
     * user swiped left/right or cancelled the swipe. This problem is removed if the
     * client simply disables holding. In the above scenario while holding disabled,
     * the client would get only swipe events (and released event).
     * (Swipe can be cancelled by dragging towards, but not beyond, the starting position.) 
     */
    IMPORT_C void SetHoldingEnabled( TBool aEnabled );
    
    /**
     * @return whether sending hold events is currently enabled
     * ("Hold" means user has held stylus/finger on the same position for a longer duration.)
     */
    IMPORT_C TBool IsHoldingEnabled() const;
    
    /**
     * Enables/disables double tap support. Double tap is disabled by default.
     * When double tap is disabled, gesture helper emits tap events immediately when
     * user lifts stylus/finger. 
     * When double tap is enabled, tap events are emitted after the double tap timeout passes. 
     * The timeout is the maximum time within which the second tap will be treated 
     * as a double tap. That is, there is a delay (sluggishness) before client receives 
     * the tap event when double tap is enabled. 
     * tap + timeout => tap event emitted after timeout
     * tap + tap before timeout => double tap event emitted immediately after the second tap
     * tap + swipe => tap + swipe events
     * (tap + timeout + tap + timeout => two tap events)
     */
    IMPORT_C void SetDoubleTapEnabled( TBool aEnabled );

    /**
     * @return whether double tap is currently enabled. See SetDoubleTapEnabled
     */
    IMPORT_C TBool IsDoubleTapEnabled() const;
    
    /** 
     * Initialise pointer capture for Alfred 
     * This means that helper will receive drag events and pointer events that 
     * go outside the original visual area
     */
    IMPORT_C void InitAlfredPointerCaptureL( CAlfEnv& aEnv, CAlfDisplay& aDisplay,
        TInt aFreeControlGroupId );
    
    /** 
     * Give a pointer event to the helper, to form a part of a gesture
     * For AVKON-based client, this interface is the only option.
     * Alfred-based client should use OfferEventL, as it allows gesture events to
     * provider the visual on which the pointer event started.
     * @param aEvent pointer event
     * @return whether event was consumed or not
     *         EFalse the event is not a pointer events and if pointer up/drag 
     *				  event received without pointer down event
     *         ETrue in all the other cases of pointer event 
     */
    IMPORT_C TBool HandlePointerEventL( const TPointerEvent& aEvent );
    
    /**
     * Offer an Alf event. See HandlePointerEventL.
     * @return whether event was consumed or not
     *         EFalse the event is not a pointer events and if pointer up/drag 
     *				  event received without pointer down event
     *         ETrue in all the other cases of pointer event 
     */
    IMPORT_C TBool OfferEventL( const TAlfEvent& aEvent );
    
    /** 
     * Cancel ongoing recognision. Purges all pointer events given earlier, and
     * starts afresh. It is not necessary to call Cancel before deleting the object.
     */
    IMPORT_C void Cancel();
        
private:
    /// interface implementation
    CGestureHelperImpl* iImpl;
    };

} // namespace RT_GestureHelper

#endif // _RT_GESTUREHELPER_H_
