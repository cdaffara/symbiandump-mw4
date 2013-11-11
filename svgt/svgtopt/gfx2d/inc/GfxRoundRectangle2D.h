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


#ifndef GFXROUNDRECTANGLE2D_H
#define GFXROUNDRECTANGLE2D_H

#include <e32std.h>

#include "GfxFloatFixPt.h"
#include "GfxRectangularShape.h"
#include "GfxRectangle2D.h"
#include "GfxPoint2D.h"
#include "GfxPathIterator.h"
#include "GfxAffineTransform.h"



/**
 * Implementation of a rounded-corner rectangle
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxRoundRectangle2D : public TGfxRectangularShape
    {
    public:
        /**
         * Constructor.
         *
         * @since 1.0
         * @return
         */
                TGfxRoundRectangle2D();

        /**
         * Construct a rounded rectangle from top-left, width/height info.
         *
         * @since 1.0
         * @param aX : x coordinate of top-left corner
         * @param aY : y coordinate of top-left corner
         * @param aWidth: width of rectangle
         * @param aHeight : height of rectangle
         * @param aArcWidth : rounded corner width
         * @param aArcHeight : rounded corner height
         * @return
         */
                TGfxRoundRectangle2D( const TFloatFixPt& aX,
                                              const TFloatFixPt& aY,
                                              const TFloatFixPt& aW,
                                              const TFloatFixPt& aH,
                                              const TFloatFixPt& aArcWidth,
                                              const TFloatFixPt& aArcHeight );

        /**
         * Get the path iterator for this rounded rectangle.
         *
         * @since 1.0
         * @param aAt : transform to apply.
         * @param aPitr : path iterator holder.
         * @return
         */
         void   GetPathIteratorL( TGfxAffineTransform* aAt,
                                          CGfxPathIterator*& aPitr );

        /**
         * Get the path iterator for rounded rectangle.
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
        inline virtual TInt ShapeType () { return ERoundRect; };


    public:
        TFloatFixPt          iArcWidth;
        TFloatFixPt          iArcHeight;

    private:
        friend class    CGfxRoundRectangleIteratorP;

    };

#endif      // GFXROUNDRECTANGLE2D_H

