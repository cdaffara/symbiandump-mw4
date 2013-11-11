/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Graphics Extension Library header file
*
*/


#ifndef GFXPOINT2D_H
#define GFXPOINT2D_H

#include <e32std.h>
#include "GfxFloatFixPt.h"


/**
 * Implementation of a 2D point.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxPoint2D
    {
    public:

        /**
         * Constructor.  Default coordinates to (0, 0).
         *
         * @since 1.0
         * @return
         */
         TGfxPoint2D();

        /**
         * Constructor.  Define point to be at the given coordinates.
         *
         * @since 1.0
         * @param aX : x coordinate
         * @param aY : y coordinate
         * @return
         */
         TGfxPoint2D( TFloatFixPt aX, TFloatFixPt aY );

        /**
         * Get distance between this point and the given point.
         *
         * @since 1.0
         * @param aPoint : point for distance calculation
         * @return distance between two points.
         */
         TFloatFixPt Distance( const TGfxPoint2D& aPoint );

        /**
         * Get distance squared between this point and the given point.
         *
         * @since 1.0
         * @param aPoint : point for distance calculation
         * @return distance squared between two points.
         */
         TFloatFixPt DistanceSq( const TGfxPoint2D& aPoint );
		
		 TInt operator==( const TGfxPoint2D& aPoint ) const;
		
		 TInt operator!=( const TGfxPoint2D& aPoint ) const;
		
    public:
        // Coordinates
        TFloatFixPt          iX;
        TFloatFixPt          iY;
    };

#endif      // GFXPOINT2D_H
