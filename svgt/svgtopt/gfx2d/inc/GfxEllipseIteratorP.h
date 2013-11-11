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


#ifndef GFXELLIPSEITERATORP_H
#define GFXELLIPSEITERATORP_H



#include <e32std.h>

#include "GfxFloatFixPt.h"
#include "GfxPathIterator.h"

class TGfxEllipse2D;
class TGfxAffineTransform;


/**
 * Implementation of an ellipse iterator.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class CGfxEllipseIteratorP : public CGfxPathIterator
    {
    public:

        /**
         * Construct an ellipse iterator.
         *
         * @since 1.0
         * @param aEllipse : ellipse to create iterator for.
         * @param aTransform : transform to apply to defining points.
         * @return
         */
                        CGfxEllipseIteratorP( TGfxEllipse2D* aEllipse,
                                              TGfxAffineTransform* aTransform );

        /**
         * Get the points defining the ellipse and the segment type.
         *
         * @since 1.0
         * @param aCoords : point storage
         * @return segment type
         */
        TGfxSegType     CurrentSegment( TFloatFixPt * aCoords );

        /**
         * Determine wether end of iterator has been reached.
         *
         * @since 1.0
         * @return true if end of iterator
         */
        TBool           IsDone();

        /**
         * Cycle to the next point of this iterator.
         *
         * @since 1.0
         * @return
         */
        void            NextL();

        /**
         * Write points of the ellipse to be a polygon.
         *
         * @since 1.0
         * @param aRenderer : point storage
         * @param aFlatness : number of points to generate.
         * @return
         */

    private:
        TGfxEllipse2D*  iEllipse;

        TInt32          iIdx;
    };

#endif      // GFXELLIPSEITERATORP_H
