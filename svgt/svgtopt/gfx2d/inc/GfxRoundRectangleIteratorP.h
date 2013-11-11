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


#ifndef GFXROUNDRECTANGLEITERATORP_H
#define GFXROUNDRECTANGLEITERATORP_H

#include <e32base.h>
#include <e32std.h>


#include "GfxFloatFixPt.h"
#include "GfxPathIterator.h"
#include "GfxAffineTransform.h"

class TGfxRoundRectangle2D;



/**
 * This class provides the methods to cycle through points in a rounded rectangle.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class CGfxRoundRectangleIteratorP : public CGfxPathIterator
    {
    public:

        /**
         * Constructor.
         *
         * @since 1.0
         * @param aRect : rounded rectangle to iterator through.
         * @param aTransform : transform to apply.
         * @return
         */
                                CGfxRoundRectangleIteratorP( TGfxRoundRectangle2D* aRoundRect,
                                                             TGfxAffineTransform* aAffine );


        /**
         * Destructor.
         *
         * @since 1.0
         * @return
         */
         virtual                ~CGfxRoundRectangleIteratorP();

        // Implementation of MGfxPathIterator

        /**
         * Get the point defining the current segment.
         *
         * @since 1.0
         * @param aCoords : storage for points.
         * @return segment type.
         */
        TGfxSegType             CurrentSegment( TFloatFixPt* aCoords );

        /**
         * Determine if end of iterator has been reached.
         *
         * @since 1.0
         * @return true, if iterator reached end.
         */
        TBool                   IsDone();

        /**
         * Cycle to the next segment of the path.
         *
         * @since 1.0
         * @return
         */
        void                    NextL();


        /**
         * Convert to a polygon.
         *
         * @since 1.0
         * @param aRenderer : storage for points of polygon.
         * @param aFlatness : level of smoothness.
         * @return true, if iterator reached end.
         */

    private:
        TGfxRoundRectangle2D*   iRoundRect;

        TInt32                  iIdx;
        TFloatFixPt                  iRx, iRy;
    };

#endif      // GFXROUNDRECTANGLEITERATORP_H
