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


#ifndef GFXFLATTENINGPATHITERATOR_H
#define GFXFLATTENINGPATHITERATOR_H

#include <e32base.h>
#include <e32std.h>

#include "GfxFloatFixPt.h"
#include "GfxPathIterator.h"



class TGfxAffineTransform;
class MGfxShape;

/**
 * This class provides the methods to smooth a path.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class CGfxFlatteningPathIterator : public CGfxPathIterator
    {
    public:

        /**
         * Two-phase constructor.
         *
         * @since 1.0
         * @param aSrc : source of path of flatten
         * @param aAt : transform to apply.
         * @param aFlatness : degree of flattening to apply.
         * @param aLimit : maximum number of points used for flattening.
         * @return
         */
         static CGfxFlatteningPathIterator*     NewL( MGfxShape* aSrc,
                                                              TGfxAffineTransform* aAt,
                                                              TInt32 aLimit );


        /**
         * Destructor
         *
         * @since 1.0
         * @return
         */
         virtual                                ~CGfxFlatteningPathIterator();


        /**
         * Set the degree of flattening to apply.
         *
         * @since 1.0
         * @param aFlatness : degree of flattening.
         * @return
         */

         void                                   SetFlatness( TFloatFixPt& aFlatness );

        /**
         * Set the recursion level.
         *
         * @since 1.0
         * @param aLimit : recursion level.
         * @return
         */
         void                                   SetRecursionLimit( TInt32 aLimit );

        /**
         * Get the flattening level..
         *
         * @since 1.0
         * @param aLimit : recursion level.
         * @return flattening value.
         */

         TFloatFixPt                                 Flatness();

        /**
         * Get the recursion level.
         *
         * @since 1.0
         * @return the recursion level.
         */
         TInt32                                 RecursionLimit();


        /**
         * Get the points defining the current segment of iteration.
         *
         * @since 1.0
         * @param aCoords : array to store segment points.
         * @return segment type.
         */
         TGfxSegType                            CurrentSegment( TFloatFixPt* aCoords );

        /**
         * Determine wether the end has been reached.
         *
         * @since 1.0
         * @return true, if end of iterator reached.
         */
         TBool                                  IsDone();

        /**
         * Cycle to the next segment of this iterator.
         *
         * @since 1.0
         * @return
         */
         void                                   NextL();


        /**
         * Create a polygon from iterator.  This method is does nothing.
         *
         * @since 1.0
         * @return
         */
         void PolygonizeL( CGfxEdgeListP* aRenderer, TInt aFlatness );

    private:

        /**
         * Constructor
         *
         * @since 1.0
         * @param aFlatness : degree of flattening to apply.
         * @param aLimit : maximum number of points used for flattening.
         * @return
         */
                            CGfxFlatteningPathIterator( TInt32 aLimit );

        /**
         * Second phase of constrction.
         *
         * @since 1.0
         * @param aSrc : source of path of flatten
         * @param aAt : transform to apply.
         * @return
         */
        void                ConstructL( MGfxShape* aSrc,
                                        TGfxAffineTransform* aAt );


        /**
         * Create a quadratic sub-curve.
         *
         * @since 1.0
         * @param aCtrlPoints : quadratic control points.
         * @return
         */
        void                CreateSubcurveQuadL( RArray<TFloatFixPt>* aCtrlPoints );

        /**
         * Create a cubic sub-curve.
         *
         * @since 1.0
         * @param aCtrlPoints : cubic control points.
         * @return
         */
        void                CreateSubcurveCubicL( RArray<TFloatFixPt>* aCtrlPoints );

    private:
        RArray<TFloatFixPt>*     iCoords;
        TInt32              iIdx;
        CGfxPathIterator*   iSrc;
        TFloatFixPt              iLastX;
        TFloatFixPt              iLastY;
        TGfxSegType         iSegType;
        TBool               iIsDone;

        TInt32              iLimit;

        TFloatFixPt              iFlatness;



    };

#endif      // GFXFLATTENINGPATHITERATOR_H
