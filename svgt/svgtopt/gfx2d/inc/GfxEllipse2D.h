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


#ifndef GFXELLIPSE2D_H
#define GFXELLIPSE2D_H

#include <e32std.h>
#include "GfxFloatFixPt.h"

#include "GfxRectangle2D.h"
#include "GfxRectangularShape.h"
#include "GfxPoint2D.h"
#include "GfxPathIterator.h"
#include "GfxAffineTransform.h"



enum TEllipseConstructType
    {
    EElipseCenterAndRadius, // cx, cy, rx, ry
    EElipseRectangular      // x, y, w, h
    };


/**
 * Implementation of an ellipse.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxEllipse2D : public TGfxRectangularShape
    {
    public:

        /**
         * Construct a TGfxEllipse2D object.
         *
         * @since 1.0
         * @param aX1 : focus point 1 or top-left  x coordinate
         * @param aY1 : focus point 1 or top-left y coordinate
         * @param aWidth : focus point 2 or width
         * @param aHeight : focus point 2 or height
         * @return
         */
               TGfxEllipse2D( const TFloatFixPt & aX,
                                      const TFloatFixPt & aY,
                                      const TFloatFixPt & aWidth,
                                      const TFloatFixPt & aHeight );

        /**
         * Construct a TGfxEllipse2D object.  The TEllipseConstructType
         * parameter defines the type of ellipse to create.
         *
         * @since 1.0
         * @param aX1 : focus point 1 or top-left  x coordinate
         * @param aY1 : focus point 1 or top-left y coordinate
         * @param aX1 : focus point 2 or width
         * @param aY1 : focus point 2 or height
         * @param aEtype : elliptical or rectangular data given
         * @return
         */
                TGfxEllipse2D( const TFloatFixPt & aX1,
                                       const TFloatFixPt & aY1,
                                       const TFloatFixPt & aX2,
                                       const TFloatFixPt & aY2,
                                       TEllipseConstructType aEtype );

        /**
         * Get the path iterator for this shape.
         * Implementation of MGfxShape method.
         *
         * @since 1.0
         * @param aAt : transform to apply the defining points.
         * @param aPitr : path iterator data storage.
         * @return
         */
         void   GetPathIteratorL( TGfxAffineTransform* aAt,
                                          CGfxPathIterator*& aPitr );

        /**
         * Get the path iterator for this shape.
         * Implementation of MGfxShape method.
         *
         * @since 1.0
         * @param aAt : transform to apply the defining points.
         * @param aLimit : number of points defining the path.
         * @param aPitr : path iterator data storage.
         * @return
         */
         void   GetPathIteratorL( TGfxAffineTransform* aAt,
                                          TInt aLimit,
                                          CGfxPathIterator*& aPitr );


        /* Return shape type */
        inline virtual TInt ShapeType () { return EEllipse; };

    };

#endif      // GFXELLIPSE2D_H
