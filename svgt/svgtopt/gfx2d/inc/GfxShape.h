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


#ifndef GFXSHAPE_H
#define GFXSHAPE_H

#include <e32std.h>

#include "GfxFloatFixPt.h"
#include "GfxPoint2D.h"


class CGfxPathIterator;
class TGfxRectangle2D;
class TGfxAffineTransform;


/**
 * This interface defines the methods that must be implement to be a 'Shape'.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class MGfxShape
    {
    public:
        /**
         * Get the bounding box
         *
         * @since 1.0
         * @param aAt : transform to apply.
         * @param aRect : rectangle to store bounding box info.
         * @return
         */        virtual void    GetBounds( const TGfxAffineTransform& aAt,
                                   TGfxRectangle2D& aRect ) = 0;

        /**
         * Get the path iterator for this rectangle.
         *
         * @since 1.0
         * @param aAt : transform to apply.
         * @param aPitr : path iterator holder.
         * @return
         */
        virtual void    GetPathIteratorL( TGfxAffineTransform* aAt,
                                          CGfxPathIterator*& aPitr ) = 0;

        /**
         * Get the path iterator for this rectangle.
         *
         * @since 1.0
         * @param aAt : transform to apply.
         * @param aLimit : maximum of points to define the iterator.
         * @param aPitr : path iterator holder.
         * @return
         */
        virtual void    GetPathIteratorL( TGfxAffineTransform* aAt,
                                          TInt aLimit,
                                          CGfxPathIterator*& aPitr ) = 0;

        /**
         * Get the area of this shape.
         *
         * @since 1.0
         * @return area of shape.
         */
        virtual TFloatFixPt  AreaSize() = 0;


        /* Return shape type */
        virtual TInt ShapeType () = 0;

        enum HYB_SHAPE_TYPE
            {
            ELine, ERect, ERoundRect, EEllipse, EPath
            };

    };

#endif      // GFXSHAPE_H
