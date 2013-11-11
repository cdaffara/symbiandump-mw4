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


#ifndef GFXLINE2D_H
#define GFXLINE2D_H

#include <e32std.h>


#include "GfxFloatFixPt.h"
#include "GfxRectangle2D.h"
#include "GfxRectangularShape.h"
#include "GfxPoint2D.h"
#include "GfxPathIterator.h"
#include "GfxAffineTransform.h"



/**
 * Implementation of a line.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class TGfxLine2D : public MGfxShape
    {
    public:

        /**
         * Contructor.
         *
         * @since 1.0
         * @param aX1 : x coordinate of starting point.
         * @param aY1 : y coordinate of starting point.
         * @param aX2 : x coordinate of ending point.
         * @param aY2 : y coordinate of ending point.
         * @return
         */
                 TGfxLine2D( const TFloatFixPt& aX1,
                                     const TFloatFixPt& aY1,
                                     const TFloatFixPt& aX2,
                                     const TFloatFixPt& aY2 );

        /**
         * Contructor.
         *
         * @since 1.0
         * @param aP1 : starting point.
         * @param aP2 : ending point.
         * @return
         */
                TGfxLine2D( TGfxPoint2D aP1, TGfxPoint2D aP2 );

        //

        /**
         * Get the length of this line.
         *
         * @since 1.0
         * @return length
         */
         TFloatFixPt Length();

        // Implementation of MGfShape

        /**
         * Get the bounding box.
         *
         * @since 1.0
         * @param aAt : transform to apply.
         * @param aRect : storage of bounding box info.
         * @return
         */
         void   GetBounds( const TGfxAffineTransform& aAt,
                                   TGfxRectangle2D& aRect );

        /**
         * Get the path iterator for this line.
         *
         * @since 1.0
         * @param aAt : transform to apply.
         * @param aPitr : storage for iterator.
         * @return
         */
         void   GetPathIteratorL( TGfxAffineTransform* aAt,
                                          CGfxPathIterator*& aPitr );

        /**
         * Get the path iterator for this line.
         *
         * @since 1.0
         * @param aAt : transform to apply.
         * @param aLimit : maximum of point to define iterator.
         * @param aPitr : storage for iterator.
         * @return
         */
         void   GetPathIteratorL( TGfxAffineTransform* aAt,
                                          TInt aLimit,
                                          CGfxPathIterator*& aPitr );

        /**
         * Get area of this line: zero.
         *
         * @since 1.0
         * @return
         */
        TFloatFixPt AreaSize();


        /* Return shape type */
        inline virtual TInt ShapeType () { return ELine; };

    public:
        TFloatFixPt  iX1;
        TFloatFixPt  iY1;
        TFloatFixPt  iX2;
        TFloatFixPt  iY2;
    };

#endif      // GFXLINE2D_H
