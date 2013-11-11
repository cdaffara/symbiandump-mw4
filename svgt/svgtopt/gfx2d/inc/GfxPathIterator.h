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


#ifndef GFXPATHITERATOR_H
#define GFXPATHITERATOR_H

#include <e32base.h>
#include <e32std.h>
#include "GfxFloatFixPt.h"

class TGfxAffineTransform;
class MGfxShape;
class CGfxEdgeListP;


/**
 * This class defines the methods to cycle through points in a path.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class CGfxPathIterator : public CBase
    {
    public:

        /**
         * Get the point defining the current segment.
         *
         * @since 1.0
         * @param aCoords : storage for points.
         * @return segment type.
         */
        virtual TGfxSegType CurrentSegment( TFloatFixPt* aCoords ) = 0;

        /**
         * Determine if end of iterator has been reached.
         *
         * @since 1.0
         * @return true, if iterator reached end.
         */
        virtual TBool       IsDone() = 0;

        /**
         * Cycle to the next segment of the path.
         *
         * @since 1.0
         * @return
         */
        virtual void        NextL() = 0;

        /**
         * Convert to a polygon.
         *
         * @since 1.0
         * @param aRenderer : storage for points of polygon.
         * @param aFlatness : level of smoothness.
         * @return true, if iterator reached end.
         */
        virtual void        PolygonizeL( CGfxEdgeListP* /*aRenderer*/,
			TInt /*aFlatness*/ ) {};


    protected:
        TGfxAffineTransform*    iTransform;
    };

#endif      // GFXPATHITERATOR_H
