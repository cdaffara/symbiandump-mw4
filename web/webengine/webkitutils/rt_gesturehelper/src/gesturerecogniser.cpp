/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "gesturerecogniser.h"

#include <e32math.h>

#include "gesturedefs.h"
#include "rt_gestureobserver.h"
#include "pointarray.h"
#include "utils.h"

using namespace RT_GestureHelper;

/**
 * Vector class (math)
 */
NONSHARABLE_CLASS( TVector )
    {
public:
    /** 
     * Constructor
     * @param aFrom starting point of the vector
     * @param aTo ending point of the vector
     */
    TVector( const TPoint& aFrom, const TPoint& aTo )
            : iX( aTo.iX - aFrom.iX ),
              iY( aTo.iY - aFrom.iY )
        {
        }
        
    /** @return angle of the vector */
    TReal Angle() const
        {
        TReal angle = 0;   
        TReal length = Length();
        if ( length != 0 )
            {
            Math::ACos( angle, iX / Length() );
            if ( iY < 0 )
                {
                angle = 2 * KPi - angle;
                }
            }
        return Degrees( angle ); 
        }
        
    /** @return length of the vector */
    TReal Length() const
        {
        TReal length = 0;
        Math::Sqrt( length, iX * iX + iY * iY );
        return length;
        }

private:
    /** @return radians in degrees */
    inline TReal Degrees( TReal aRadians ) const
        {
        return aRadians * 180 / KPi;
        }

public:
    /// x coordinate that represent the vector
    TReal iX;
    /// y coordinate that represent the vector
    TReal iY;
    };

inline TPoint LastPoint( const TPointArray& aPoints ) 
    {
    __ASSERT_DEBUG( aPoints.Count() > 0, Panic( EGesturePanicIllegalLogic ) );
    return aPoints[ aPoints.Count() - 1 ] ;
    }

/** 
 * @return Length of the gesture in points
 */
inline TReal GestureLength( const TPointArray& aPoints )
    {
    return TVector( aPoints[0], LastPoint( aPoints ) ).Length();
    }

/** 
 * @return ETrue if aAngleUnderTest is almost aAngle
 *         Closeness of the angles is controlled by KAngleTolerance
 */
static TBool IsNear( TReal aAngleUnderTest, TReal aAngle )
    {
    return aAngle - KAngleTolerance <= aAngleUnderTest && 
           aAngleUnderTest <= aAngle + KAngleTolerance;
    }
    
/**
 * @return the angle as a direction flags of TGesture
 */
inline TGestureCode Direction( TReal aAngle ) 
    {
    TGestureCode direction = EGestureUnknown;

    if ( IsNear( aAngle, 90.0 ) )
        {
        direction = EGestureSwipeDown;
        }
    else if ( IsNear( aAngle, 180.0 ) )
        {
        direction = EGestureSwipeLeft;
        }
    else if ( IsNear( aAngle, 270.0 ) )
        {
        direction = EGestureSwipeUp;
        }
    else if ( 360.0 - KAngleTolerance <= aAngle || aAngle <= KAngleTolerance )
        {
        direction = EGestureSwipeRight;
        }
    else // for lint warning
        {
        // unknown angle
        }
    
    return direction;
    }

/** @return direction between points */
inline TGestureCode Direction( const TPoint& aFromPoint, const TPoint& aToPoint )
    {
    return Direction( TVector( aFromPoint, aToPoint ).Angle() );
    }
    
/** @return overall direction between points */
static TGestureCode GeneralDirection( const TPointArray& aPoints ) 
    {
    // If the start and end points are too close to each other, direction
    // is undefined
    if ( ToleranceRect( aPoints[0] ).Contains( LastPoint( aPoints ) ) )
        {
        return EGestureUnknown;
        }
    return Direction( aPoints[0], LastPoint( aPoints ) );
    }

// function type to get a point in the point array
typedef TPoint (TPointArray::*PointByIndexFunc)( TInt aIndex ) const;    

/// @return latest point outside tolerance area or KErrNotFound if not point outside it
TInt LatestCertainPointIndex( const TPointArray& aPoints, PointByIndexFunc aPointByIndex )   
    {
    __ASSERT_DEBUG( aPoints.Count() > 0, Panic( EGesturePanicIllegalLogic ) );

    int i = aPoints.Count();
    TRect toleranceRect = ToleranceRect( (aPoints.*aPointByIndex)( aPoints.Count() - 1 ) );
    // Find out the first point from the end of the array  
    // that is not contained in the tolerance rect.
    while( 0 <= --i )
        {
        // if the point does not belong inside tolerance rect, it is the first point
        // outside the rect
        if( !toleranceRect.Contains( (aPoints.*aPointByIndex)(i) ) )
            {
            break;
            }
        }
    return i;
    }
    
/** @return last direction of dragging */
inline TGestureCode LastDirection( const TPointArray& aPoints ) 
    {
    TInt latestPointIndex = LatestCertainPointIndex( aPoints, &TPointArray::operator[] );
    if ( KErrNotFound != latestPointIndex )
        {
        return Direction( aPoints[latestPointIndex], LastPoint( aPoints ) );
        }
    // no points were outside the rect, and hence the direction is unknown
    return EGestureUnknown;
    }
    
/**
 * @return ETrue if points for a tap event
 */
inline TBool IsTap( const TPointArray& aPoints )
    {
    return KErrNotFound == LatestCertainPointIndex( aPoints, &TPointArray::Raw );
    }  
    
// ----------------------------------------------------------------------------
// Return gesture code of a gesture formed by a sequence of points
// ----------------------------------------------------------------------------
//
TGestureCode TGestureRecogniser::GestureCode( const TPointArray& aPoints ) const
    {
    __ASSERT_DEBUG( aPoints.Count() > 0, Panic( EGesturePanicIllegalLogic ) );
    
    // tap needs to be treated separately, because recognising that needs to consider
    // raw points (and not points in which x or y axis has been filtered out)
    if ( IsTap( aPoints ) )
        {
        return EGestureTap;
        }
    
    TGestureCode direction = GeneralDirection( aPoints );
    // if last direction is opposite of the general one, user has cancelled a swipe
    if ( direction != LastDirection( aPoints ) )
        {
        direction = EGestureUnknown;
        }
    return direction;
    }
