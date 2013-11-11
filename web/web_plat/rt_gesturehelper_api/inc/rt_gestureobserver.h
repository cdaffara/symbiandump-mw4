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
* Description:  RT Gesture helper interface
*
*/

#ifndef _RT_GESTUREOBSERVER_H_
#define _RT_GESTUREOBSERVER_H_

#warning The RT Gesture API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

#include <e32std.h>

namespace RT_GestureHelper
{

/// flag that indicates gesture code is a holding code
/// clients should use MGestureEvent::IsHolding
const TInt EFlagHold       = 0x10000000;

enum TGestureCode
    {
    // states
    EGestureStart          = 0x00000001, // gesture just started (user pressed stylus down)
    EGestureDrag           = 0x00000002, // user moved stylus (may be holding or not holding)
    EGestureReleased       = 0x00000003, // user lifted stylus while user was holding
    // gestures
    EGestureUnknown        = 0x00000005, // gesture was not recognised
    // these codes are sent when user lifts stylus (if holding not started)
    EGestureTap            = 0x00000006,
    EGestureDoubleTap      = 0x00000007, // only if double tap is enabled 
                                         // (via CGestureHelper::SetDoubleTapEnabled or
                                         // CGestureControl::SetDoubleTapEnabled)
    EGestureSwipeLeft      = 0x00000008,
    EGestureSwipeRight     = 0x00000009,
    EGestureSwipeUp        = 0x0000000A,
    EGestureSwipeDown      = 0x0000000B,
    
    EGestureFlick          = 0x0000000C,
    EGestureLongTap        = 0x0000000D,
    EGestureDrop           = 0x0000000E,
        
    // these codes are sent when user initiates holding by keeping stylus in
    // same place for a longer duration
    EGestureHoldLeft       = EGestureSwipeLeft  | EFlagHold,
    EGestureHoldRight      = EGestureSwipeRight | EFlagHold,
    EGestureHoldUp         = EGestureSwipeUp    | EFlagHold,
    EGestureHoldDown       = EGestureSwipeDown  | EFlagHold
    };

/**
 * Point of float accuracy
 */
NONSHARABLE_STRUCT( TRealPoint )
    {
    inline TRealPoint();
    inline TRealPoint( const TRealPoint& aPoint );
    inline TRealPoint( TReal aX, TReal aY );
    inline TBool operator==( const TRealPoint& aPoint ) const;

    TReal32 iX;
    TReal32 iY;
    };

/**
 * a gesture event
 */

enum TAxis
    {
    EAxisBoth,
    EAxisHorizontal,
    EAxisVertical
    };


class TGestureEvent
{
public:    
    inline TGestureCode Code( TAxis /*aRelevantAxis*/ ) const { return iCode; };
    inline TBool IsHolding() const { return iIsHolding; };
    inline TPoint StartPos() const { return iStartPos; };
    inline TPoint CurrentPos() const { return iCurrPos; }; 
    inline TRealPoint Speed() const { return iSpeed; };
    inline TRealPoint SpeedPercent ( const TRect& /*aEdges*/ ) const { return iSpeedPercent; };
    inline TPoint Distance() const { return iDistance; }; 
    
    inline void SetCurrentPos(TPoint aPos) { iCurrPos = aPos; };
    inline void SetStartPos(TPoint aPos) {iStartPos = aPos; };
    inline void SetCode(TGestureCode aCode) {iCode = aCode; };
    inline void SetSpeed(TRealPoint aSpeed) { iSpeed = aSpeed; };
    inline void SetDistance(TPoint aDistance) { iDistance = aDistance; };
    inline void SetSpeedPercent(TRealPoint aSpeedPercent) { iSpeedPercent = aSpeedPercent; };
    
    inline void SetIsHolding(TBool aIsHolding) { iIsHolding = aIsHolding; };
    
private:    
    TGestureCode   iCode;
    TBool          iIsHolding;
    TPoint         iStartPos;
    TPoint         iCurrPos;
    TRealPoint     iSpeed;
    TRealPoint     iSpeedPercent;
    TPoint         iDistance;
};

    
/** 
 * Observer that will be notified when user makes gestures
 */
class MGestureObserver
    {
public:
    /**
     * Handle the gesture event
     * @param aEvent event describing the gesture 
     */
    virtual void HandleGestureL( const TGestureEvent& aEvent ) = 0;
    };

// ----------------------------------------------------------------------------
// Default constructor for real point
// ----------------------------------------------------------------------------
//
inline TRealPoint::TRealPoint()
        : iX( 0 ), iY( 0 )
    {    
    }

// ----------------------------------------------------------------------------
// Copy constructor for real point
// ----------------------------------------------------------------------------
//
inline TRealPoint::TRealPoint( const TRealPoint& aPoint )
        : iX( aPoint.iX ), iY( aPoint.iY )
    {    
    }

// ----------------------------------------------------------------------------
// Copy constructor for real point
// ----------------------------------------------------------------------------
//
inline TRealPoint::TRealPoint( TReal aX, TReal aY )
        : iX( aX ), iY( aY )
    {    
    }

// ----------------------------------------------------------------------------
// Default constructor for real point
// ----------------------------------------------------------------------------
//
inline TBool TRealPoint::operator==( const TRealPoint& aPoint ) const
    {    
    return iX == aPoint.iX && iY == aPoint.iY;
    }

    
} // namespace RT_GestureHelper

#endif // _RT_GESTUREOBSERVER_H_
