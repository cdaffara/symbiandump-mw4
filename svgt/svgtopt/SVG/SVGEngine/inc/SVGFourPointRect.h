/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Engine header file
 *
*/


#ifndef SVGFOURPOINTRECT_H
#define SVGFOURPOINTRECT_H

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include "SVGEngineInterfaceImpl.h"
#include "GfxFloatFixPt.h"
#include "GfxRectangle2D.h"

/**
 * This class implements the interface for a MRect object
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class TSvgFourPointRect: public MRect
    {
    public:

        /**
         * Construct a rectangle.
         *
         * @since 1.0
         * @return
         */
        TSvgFourPointRect();
		TSvgFourPointRect(TPoint aPoint1, TPoint aPoint2, TPoint aPoint3, TPoint aPoint4);
        TSvgFourPointRect( TGfxRectangle2D aRect );
		TSvgFourPointRect(TGfxPoint2D aPoint1, TGfxPoint2D aPoint2, TGfxPoint2D aPoint3, TGfxPoint2D aPoint4);
        TSvgFourPointRect( TFloatFixPt aX, TFloatFixPt aY, TFloatFixPt aWidth, TFloatFixPt aHeight );
        
		void SetRectPoints( TFloatFixPt aX, TFloatFixPt aY, TFloatFixPt aWidth, TFloatFixPt aHeight );
		void SetRectPoints(TPoint aPoint1, TPoint aPoint2, TPoint aPoint3, TPoint aPoint4);
		void SetRectPoints(TGfxPoint2D aPoint1, TGfxPoint2D aPoint2, TGfxPoint2D aPoint3, TGfxPoint2D aPoint4);
		void SetRectPoints( TGfxRectangle2D aRect );
		
		//IMPLEMENTED FOR MRect
		void GetPoints(TPoint& aPoint1, TPoint& aPoint2, TPoint& aPoint3, TPoint& aPoint4) const;
		
        void GetPoints(TGfxPoint2D& aPoint1, TGfxPoint2D& aPoint2, TGfxPoint2D& aPoint3, TGfxPoint2D& aPoint4);
        void GetTRect(TRect& aRect);
        void GetTRect(TGfxRectangle2D& aRect);
        
        
        //IMPLEMENTED FOR MRect
        /**
         * Determine if the given rectangle intersects with this rectangle.
         *
         * @since 1.0
         * @param aRect : rectangle to check for intersection.
         * @return true, if intersection occurs.
         */
		TBool  Intersects( const MRect& aRect );
		
		//IMPLEMENTED FOR MRect
		 /**
         * Determine the center point of this rectangle
         *
         * @since 1.0
         * @param aCenter the center point
         * @return
         */
         
		void Center(TPoint& aCenter);
		
        /**
         * Determine if the given point is contained within this rectangle.
         *
         * @since 1.0
         * @param aRect : rectangle to check for intersection.
         * @return true, if points is contained within rectangle
         */
  //      IMPORT_C TBool  Contains( TGfxPoint2D aPoint );

        /* Return shape type */
 //       inline virtual TInt ShapeType () { return ERect; };
 
        TGfxPoint2D		  iPoint1;
        TGfxPoint2D		  iPoint2;
        TGfxPoint2D		  iPoint3;
        TGfxPoint2D		  iPoint4;
    };

#endif

