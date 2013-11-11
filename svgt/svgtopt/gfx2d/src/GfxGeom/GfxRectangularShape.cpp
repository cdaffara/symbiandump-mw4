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
* Description:  Graphics Extension Library source file
*
*/


#include "GfxRectangularShape.h"
#include "GfxRectangle2D.h"


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
//  TGfxRectangularShape::TGfxRectangularShape()
// ---------------------------------------------------------------------------
 TGfxRectangularShape::TGfxRectangularShape()
    {
    }

// --------------------------------------------------------------------------
//  TGfxRectangularShape::TGfxRectangularShape( const TFloatFixPt& aX,
// ---------------------------------------------------------------------------
 TGfxRectangularShape::TGfxRectangularShape( const TFloatFixPt& aX,
                                                     const TFloatFixPt& aY,
                                                     const TFloatFixPt& aWidth,
                                                     const TFloatFixPt& aHeight )
													 :iWidth(aWidth),
													 iHeight(aHeight),
													 iX(aX),
													 iY(aY)
	{
	}


// --------------------------------------------------------------------------
//  void TGfxRectangularShape::GetBounds( const TGfxAffineTransform& aAt,
// ---------------------------------------------------------------------------
 void TGfxRectangularShape::GetBounds( const TGfxAffineTransform& aAt,
                                               TGfxRectangle2D& aRect )
    {
    TGfxPoint2D p[4];
    p[0].iX = iX;
    p[0].iY = iY;
    p[1].iX = iX + iWidth;
    p[1].iY = iY;
    p[2].iX = iX + iWidth;
    p[2].iY = iY + iHeight;
    p[3].iX = iX;
    p[3].iY = iY + iHeight;
    aAt.Transform( p, p, 4 );

    TFloatFixPt xMin( KMAXFLOATFIX ), yMin( KMAXFLOATFIX ), xMax( KMINFLOATFIX ), yMax( KMINFLOATFIX );
    for ( TInt32 i = 0; i < 4; i++ )
        {
        if ( p[i].iX < xMin )
            xMin = p[i].iX;
        if ( p[i].iX > xMax )
            xMax = p[i].iX;
        if ( p[i].iY < yMin )
            yMin = p[i].iY;
        if ( p[i].iY > yMax )
            yMax = p[i].iY;
        }

    aRect.iX = xMin;
    aRect.iY = yMin;
    aRect.iWidth = xMax - xMin;
    aRect.iHeight = yMax - yMin;
    }

// --------------------------------------------------------------------------
//  TFloatFixPt TGfxRectangularShape::AreaSize()
// ---------------------------------------------------------------------------
 TFloatFixPt TGfxRectangularShape::AreaSize()
    {
    if ( iWidth > KMAXFLOATFIX || iHeight > KMAXFLOATFIX )
        {
        TInt area = ( ( TInt ) iWidth ) * ( ( TInt ) iHeight );
        if ( area < KMAXFLOATFIXVALUE )
            return TFloatFixPt( area );
        else
            return KMAXFLOATFIX;
        }
    else
        {
        return iWidth * iHeight;
        }
    }

// --------------------------------------------------------------------------
//  TFloatFixPt TGfxRectangularShape::CenterX()
// ---------------------------------------------------------------------------
 TFloatFixPt TGfxRectangularShape::CenterX()
  {
	#ifdef SVG_FLOAT_BUILD
    return ( iX + (iWidth * TFloatFixPt(.5f)));
	#else
    return ( iX + (iWidth >> 1));
	#endif
	}


// --------------------------------------------------------------------------
//  TFloatFixPt TGfxRectangularShape::CenterY()
// ---------------------------------------------------------------------------
 TFloatFixPt TGfxRectangularShape::CenterY()
    {
	#ifdef SVG_FLOAT_BUILD
    return ( iY + (iHeight * TFloatFixPt(.5f))) ;
	#else
    return ( iY + (iHeight >> 1)) ;
	#endif
    }


// --------------------------------------------------------------------------
//  TFloatFixPt TGfxRectangularShape::MinX()
// ---------------------------------------------------------------------------
 TFloatFixPt TGfxRectangularShape::MinX()
    {
    return TFloatFixPt( iX );
    }

// --------------------------------------------------------------------------
//  TFloatFixPt TGfxRectangularShape::MinY()
// ---------------------------------------------------------------------------
 TFloatFixPt TGfxRectangularShape::MinY()
    {
    return TFloatFixPt( iY );
    }

// --------------------------------------------------------------------------
//  TFloatFixPt TGfxRectangularShape::MaxX()
// ---------------------------------------------------------------------------
 TFloatFixPt TGfxRectangularShape::MaxX()
    {
    return TFloatFixPt( iX + iWidth );
    }

// --------------------------------------------------------------------------
//  TFloatFixPt TGfxRectangularShape::MaxY()
// ---------------------------------------------------------------------------
 TFloatFixPt TGfxRectangularShape::MaxY()
    {
    return TFloatFixPt( iY + iHeight );
    }

