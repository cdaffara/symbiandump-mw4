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


#ifndef GFXRECTANGULARSHAPE_H
#define GFXRECTANGULARSHAPE_H

#include <e32base.h>
#include <e32std.h>

#include <VG/openvg.h>

#include "GfxFloatFixPt.h"
#include "GfxShape.h"


/**
 * This class implements the properties of a rectangle.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxRectangularShape : public MGfxShape
    {
    public:

        /**
         * Constructor.
         *
         * @since 1.0
         * @return
         */
                TGfxRectangularShape();

        /**
         * Construct a rectangle from top-left, width/height info.
         *
         * @since 1.0
         * @param aX : x coordinate of top-left corner
         * @param aY : y coordinate of top-left corner
         * @param aWidth: width of rectangle
         * @param aHeight : height of rectangle
         * @return
         */
                TGfxRectangularShape( const TFloatFixPt& aX,
                                              const TFloatFixPt& aY,
                                              const TFloatFixPt& aWidth,
                                              const TFloatFixPt& aHeight );

        /**
         * Get the center x coordinate.
         *
         * @since 1.0
         * @return x coordinate of the center.
         */
         TFloatFixPt CenterX();

        /**
         * Get the center y coordinate.
         *
         * @since 1.0
         * @return y coordinate of the center.
         */
         TFloatFixPt CenterY();

        /**
         * Get the minimum x coordinate value.
         *
         * @since 1.0
         * @return minimum x coordinate value.
         */
         TFloatFixPt MinX();

        /**
         * Get the minimum y coordinate value.
         *
         * @since 1.0
         * @return minimum y coordinate value.
         */
         TFloatFixPt MinY();

        /**
         * Get the maximum x coordinate value.
         *
         * @since 1.0
         * @return maximum x coordinate value.
         */
         TFloatFixPt MaxX();

        /**
         * Get the maximum y coordinate value.
         *
         * @since 1.0
         * @return maximum y coordinate value.
         */
         TFloatFixPt MaxY();

        /**
         * Get the bounding box for this rectangle.
         *
         * @since 1.0
         * @param aAt : transform to apply.
         * @param aRect : rectangle to store bounding box info.
         * @return maximum y coordinate value.
         */
         void   GetBounds( const TGfxAffineTransform& aAt,
                                   TGfxRectangle2D& aRect );

        /**
         * Get the area.
         *
         * @since 1.0
         * @return area
         */

         TFloatFixPt AreaSize();

		//
		// OpenVG Path accessor methods..
		//
		//

    public:
        TFloatFixPt          iWidth;
        TFloatFixPt          iHeight;
        TFloatFixPt          iX;
        TFloatFixPt          iY;
    };

#endif      // GFXRECTANGULARSHAPE_H
