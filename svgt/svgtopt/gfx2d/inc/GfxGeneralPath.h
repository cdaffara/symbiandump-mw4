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


#ifndef GFXGENERALPATH_H
#define GFXGENERALPATH_H

#include <e32base.h>
#include <e32std.h>

#include "GfxFloatFixPt.h"
#include "GfxShape.h"
#include "GfxAffineTransform.h"
#include "GfxRectangle2D.h"
#include "GfxPoint2D.h"
#include "GfxPathIterator.h"

#include <VG/openvg.h>


/**
 * Implementation of a path.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */

class CGfxGeneralPath : public CBase, public MGfxShape
    {
    public:
        /**
         * Two-phase constructor.
         *
         * @since 1.0
         * @return
         */
          static CGfxGeneralPath*    NewL();

        /**
         * Two-phase constructor.
         *
         * @since 1.0
         * @return
         */
         static CGfxGeneralPath*    NewLC();

        /**
         * Destructor.
         *
         * @since 1.0
         * @return
         */
         virtual                    ~CGfxGeneralPath();

        /**
         * Signal end of path to connect starting point to ending point.
         *
         * @since 1.0
         * @return
         */
         void                       ClosePathL();

		 void						RemoveLastPathCommand();

        /**
         * Add a cubic curve.
         *
         * @since 1.0
         * @param aX1 : x coordinate for control point 1.
         * @param aY1 : y coordinate for control point 1.
         * @param aX2 : x coordinate for control point 2.
         * @param aY2 : y coordinate for control point 2.
         * @param aX3 : x coordinate for control point 3.
         * @param aY3 : y coordinate for control point 3.
         * @param aAbsolute : flag to indicate values are absolute or not.
         * @return
         */
         void                       CubicToL( TFloatFixPt& aX1,
                                                      TFloatFixPt& aY1,
                                                      TFloatFixPt& aX2,
                                                      TFloatFixPt& aY2,
                                                      TFloatFixPt& aX3,
                                                      TFloatFixPt& aY3,
                                                      TBool aAbsolute );

        /**
         * Add a line.
         *
         * @since 1.0
         * @param aX1 : x coordinate of line end point.
         * @param aY1 : y coordinate of line end point.
         * @param aAbsolute : flag to indicate values are absolute or not.
         * @return
         */
         void                       LineToL( TFloatFixPt& aX,
                                                     TFloatFixPt& aY,
                                                     TBool aAbsolute );

        /**
         * Start a new sub-path at the given location.
         *
         * @since 1.0
         * @param aX1 : x coordinate of new location.
         * @param aY1 : y coordinate of new location.
         * @param aAbsolute : flag to indicate values are absolute or not.
         * @return
         */
         void                       MoveToL( TFloatFixPt& aX,
                                                     TFloatFixPt& aY,
                                                     TBool aAbsolute );

        /**
         * Add a quadratic curve.
         *
         * @since 1.0
         * @param aX1 : x coordinate for control point 1.
         * @param aY1 : y coordinate for control point 1.
         * @param aX2 : x coordinate for control point 2.
         * @param aY2 : y coordinate for control point 2.
         * @param aAbsolute : flag to indicate values are absolute or not.
         * @return
         */
         void                       QuadToL( TFloatFixPt& aX1,
                                                     TFloatFixPt& aY1,
                                                     TFloatFixPt& aX2,
                                                     TFloatFixPt& aY2,
                                                     TBool aAbsolute );

        /**
         * Add a horizontal line from the current location.
         *
         * @since 1.0
         * @param aX : x coordinate for ending point.
         * @param aAbsolute : flag to indicate values are absolute or not.
         * @return
         */
         void                       HorizonToL( TFloatFixPt& aX,
                                                        TBool aAbsolute );

        /**
         * Add a vertical line from the current location.
         *
         * @since 1.0
         * @param aY : y coordinate for ending point.
         * @param aAbsolute : flag to indicate values are absolute or not.
         * @return
         */
         void                       VertToL( TFloatFixPt& aY,
                                                     TBool aAbsolute );

        /**
         * Add a cubic curve, using the current location as the
         * first control point.
         *
         * @since 1.0
         * @param aX2 : x coordinate for control point 2.
         * @param aY2 : y coordinate for control point 2.
         * @param aX3 : x coordinate for control point 3.
         * @param aY3 : y coordinate for control point 3.
         * @param aAbsolute : flag to indicate values are absolute or not.
         * @return
         */
         void                       ShortCubicToL( TFloatFixPt& aX2,
                                                           TFloatFixPt& aY2,
                                                           TFloatFixPt& aX3,
                                                           TFloatFixPt& aY3,
                                                           TBool aAbsolute );
        /**
         * Add a quadratic curve, using the current location as the
         * first control point.
         *
         * @since 1.0
         * @param aX2 : x coordinate for control point 2.
         * @param aY2 : y coordinate for control point 2.
         * @param aAbsolute : flag to indicate values are absolute or not.
         * @return
         */
         void                       ShortQuadToL( TFloatFixPt& aX2,
                                                          TFloatFixPt& aY2,
                                                          TBool aAbsolute );



        /**
         * Clear all path info.
         *
         * @since 1.0
         * @return
         */
         void                       Reset();

        /**
         * Append the given points to this path.
         *
         * @since 1.0
         * @param aItr : points to append.
         * @return
         */
         void                       AppendL( CGfxPathIterator* aItr );


        // Implementation of MGfxShape

        /**
         * Get the bounding box for this path.
         *
         * @since 1.0
         * @param aAt : transform to apply.
         * @param aRect : bounding box info holder.
         * @return
         */
         void                       GetBounds( const TGfxAffineTransform& aAt,
                                                       TGfxRectangle2D& aRect );

        /**
         * Get the iterator for this path.
         *
         * @since 1.0
         * @param aAt : transform to apply.
         * @param aPitr : iterator info for this path.
         * @return
         */
         void                       GetPathIteratorL( TGfxAffineTransform* aAt,
                                                              CGfxPathIterator*& aPitr );

        /**
         * Get the iterator for this path.
         *
         * @since 1.0
         * @param aAt : transform to apply.
         * @param aLimit : maximum number of points to define path.
         * @param aPitr : iterator info for this path.
         * @return
         */
         void                       GetPathIteratorL( TGfxAffineTransform* aAt,
                                                              TInt aLimit,
                                                              CGfxPathIterator*& aPitr );
        /**
         * Get the area for this path.
         *
         * @since 1.0
         * @return area of path.
         */
        TFloatFixPt AreaSize();

        /**
         * Get the pointer to the type array.
         *
         * @since 1.0
         * @return type info array pointer
         */
         RArray<TUint32>* PointTypeArray();
         unsigned char* PathSegmentTypeArray();
         void PathSegmentTypeArray(unsigned char* aSegmentTypeArray);
         TUint8 CGfxGeneralPath::Count();
         void CGfxGeneralPath::Count(TUint8 aCount);

        /**
         * Get the pointer to the coordinate array.
         *
         * @since 1.0
         * @return coordinate array.
         */
         TFloatFixPt* PointCoordsArray();

        /**
         * Get the pointer to the type array.
         *
         * @since 1.0
         * @return type info array pointer
         */
         RArray<TFloatFixPt>* PointCoordsArrayAll();

        /**
         * Set the pointer to the type array.
         *
         * @since 1.0
         * @param aTypesArray : type info array pointer
         * @return
         */
         void       SetPointTypeArrayL( RArray<TUint32>* aTypesArray );

		// used in decoding to avoid local copying
		 void       SetPointTypeArrayRef( RArray<TUint32>*& aTypesArray );

        /**
         * Set the pointer to the coordinate array.
         *
         * @since 1.0
         * @param aPointCoords : coordinate array.
         * @return
         */
         void       SetPointCoordsArrayL( RArray<TFloatFixPt>* aPointCoords );

		// used in decoding to avoid local copying
    	  void       SetPointCoordsArrayRef( RArray<TFloatFixPt>*& aPointCoords );


        /* Return shape type */
        inline virtual TInt ShapeType () { return EPath; };

		/* gives the total number of segments in the path elementL */
		 TInt GetSegmentCount();

		/* gives the  segmentType in the path elementL */
		 TInt GetSegmentType(TInt aSegmentIndex );

		/* gives the  */
		 TReal32 GetSegmentParameter(TInt aSegmentIndex ,TInt aParameterIndex);
		void IsHorizontalLineOrVertical(TInt aSegmentIndex, TBool& aHorizontal, TBool& aVertical);

		 CGfxGeneralPath* CloneL();
		 void QuadToLWithNoControlPoint(TFloatFixPt aX, TFloatFixPt aY);

    protected:

        /**
         * Constructor
         *
         * @since 1.0
         * @return
         */
                            CGfxGeneralPath();

        /**
         * Second phase of construction.
         *
         * @since 1.0
         * @return
         */
        void                ConstructL();


        /**
         * Get the curve reference point for this last curve.
         *
         * @since 1.0
         * @param aX : storage for x coordinate of reference point.
         * @param aY : storage for y coordinate of reference point.
         * @return
         */
        void                GetCurveRefPoint( TFloatFixPt& aX, TFloatFixPt& aY );

    protected:
        RArray<TUint32>*     iPointTypes;
        unsigned char*       iPathSegmentTypeArray;
        TUint8               iCount;
        RArray<TFloatFixPt>*     iPointCoords;

        TFloatFixPt              iLastX;
        TFloatFixPt              iLastY;

    private:
        friend class        CGfxGeneralPathIteratorP;
    };

#endif      // GFXGENERALPATH_H
