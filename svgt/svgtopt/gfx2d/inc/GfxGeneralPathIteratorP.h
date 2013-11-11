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


#ifndef CGFXGENERALPATHITERATORP
#define CGFXGENERALPATHITERATORP

#include <e32std.h>
#include <e32base.h>

#include "GfxFloatFixPt.h"
#include "GfxPathIterator.h"
#include "GfxAffineTransform.h"
#include "GfxPoint2D.h"



class CGfxGeneralPath;


/**
 * This class provides the methods to cycle through points in a path.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class CGfxGeneralPathIteratorP : public CGfxPathIterator
    {
    public:

        /**
         * Contructor.
         *
         * @since 1.0
         * @param aPath : path to cycle through.
         * @param aTransform : transform to apply.
         * @return
         */
                        CGfxGeneralPathIteratorP( CGfxGeneralPath* aPath,
                                                          TGfxAffineTransform* aTransform );

        /**
         * Destructor.
         *
         * @since 1.0
         * @return
         */
                        ~CGfxGeneralPathIteratorP();

        // implementation of CGfxPathIterator

        /**
         * Get the point defining the current segment.
         *
         * @since 1.0
         * @param aCoords : storage for points.
         * @return segment type.
         */
         TGfxSegType    CurrentSegment( TFloatFixPt* aCoords );

        /**
         * Determine if end of iterator has been reached.
         *
         * @since 1.0
         * @return true, if iterator reached end.
         */
         TBool          IsDone();

        /**
         * Cycle to the next segment of the path.
         *
         * @since 1.0
         * @return
         */
         void           NextL();



    private:
        TInt32                  iTypeIdx;
        TInt32                  iPointIdx;
        CGfxGeneralPath*        iPath;



        TGfxPoint2D             iStartPoint;
    };

#endif      // CGFXGENERALPATHITERATORP
