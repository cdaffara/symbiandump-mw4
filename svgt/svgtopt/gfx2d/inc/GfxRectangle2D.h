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


#ifndef GFXRECTANGLE2D_H
#define GFXRECTANGLE2D_H

#include <e32std.h>


#include "GfxFloatFixPt.h"
#include "GfxRectangularShape.h"
#include "GfxPoint2D.h"
#include "GfxPathIterator.h"
#include "GfxAffineTransform.h"


/**
 * Implementation of a rectangle.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxRectangle2D : public TGfxRectangularShape
    {
    public:

        /**
         * Construct a rectangle.
         *
         * @since 1.0
         * @return
         */
                TGfxRectangle2D();

        /**
         * Construct a rectangle from another TGfxRectangle2D.
         *
         * @since 1.0
         * @param aRect : rectangle to copy
         * @return
         */
                TGfxRectangle2D( const TGfxRectangle2D& aRect );


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
                TGfxRectangle2D( const TFloatFixPt& aX,
                                         const TFloatFixPt& aY,
                                         const TFloatFixPt& aWidth,
                                         const TFloatFixPt& aHeight );

        /**
         * Determine if the given rectangle intersects with this rectangle.
         *
         * @since 1.0
         * @param aRect : rectangle to check for intersection.
         * @return true, if intersection occurs.
         */
         TBool  Intersects( const TGfxRectangle2D& aRect );

        /**
         * Determine if the given point is contained within this rectangle.
         *
         * @since 1.0
         * @param aRect : rectangle to check for intersection.
         * @return true, if points is contained within rectangle
         */
         TBool  Contains( TGfxPoint2D aPoint );

        /**
         * Get the path iterator for this rectangle.
         *
         * @since 1.0
         * @param aAt : transform to apply.
         * @param aPitr : path iterator holder.
         * @return
         */
         void   GetPathIteratorL( TGfxAffineTransform* aAt,
                                          CGfxPathIterator*& aPitr );

        /**
         * Get the path iterator for this rectangle.
         *
         * @since 1.0
         * @param aAt : transform to apply.
         * @param aLimit : maximum of points to define the iterator.
         * @param aPitr : path iterator holder.
         * @return
         */
         void   GetPathIteratorL( TGfxAffineTransform* aAt,
                                          TInt aLimit,
                                          CGfxPathIterator*& aPitr );

        /* Return shape type */
        inline virtual TInt ShapeType () { return ERect; };

		 TInt operator==( const TGfxRectangle2D& aRect ) const;
		
		 TInt operator!=( const TGfxRectangle2D& aRect ) const;	
    };

#endif      // GFXRECTANGLE2D_H

