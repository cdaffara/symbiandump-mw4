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


#include "GfxRectangle2D.h"
#include "GfxRectangleIteratorP.h"


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
//  TGfxRectangle2D::TGfxRectangle2D()
// ---------------------------------------------------------------------------
 TGfxRectangle2D::TGfxRectangle2D()
    {
    }

// --------------------------------------------------------------------------
//  TGfxRectangle2D::TGfxRectangle2D( const TFloatFixPt& aX,
// ---------------------------------------------------------------------------
 TGfxRectangle2D::TGfxRectangle2D( const TFloatFixPt& aX,
                                           const TFloatFixPt& aY,
                                           const TFloatFixPt& aWidth,
                                           const TFloatFixPt& aHeight )
    : TGfxRectangularShape( aX,
                            aY,
                            aWidth,
                            aHeight )
    {
    }

// --------------------------------------------------------------------------
//  TGfxRectangle2D::TGfxRectangle2D( const TGfxRectangle2D& aRect )
// ---------------------------------------------------------------------------
 TGfxRectangle2D::TGfxRectangle2D( const TGfxRectangle2D& aRect )
	:TGfxRectangularShape(aRect.iX,
							aRect.iY,
							aRect.iWidth,
							aRect.iHeight)
	{
    }

// --------------------------------------------------------------------------
//  TBool TGfxRectangle2D::Intersects( const TGfxRectangle2D& aRect )
// ---------------------------------------------------------------------------
 TBool TGfxRectangle2D::Intersects( const TGfxRectangle2D& aRect )
    {
    return  ( !( aRect.iX + aRect.iWidth < iX ||
                 iX + iWidth < aRect.iX ||
                 aRect.iY + aRect.iHeight < iY ||
                 iY + iHeight < aRect.iY ) );
    }

// --------------------------------------------------------------------------
//  TBool TGfxRectangle2D::Contains( TGfxPoint2D aPoint )
// ---------------------------------------------------------------------------
 TBool TGfxRectangle2D::Contains( TGfxPoint2D aPoint )
    {
    return ( iX <= aPoint.iX &&
             aPoint.iX <= ( iX + iWidth ) &&
             iY <= aPoint.iY &&
             aPoint.iY <= ( iY + iHeight ) );
    }


// --------------------------------------------------------------------------
//  void TGfxRectangle2D::GetPathIteratorL( TGfxAffineTransform* aAt,
// ---------------------------------------------------------------------------
 void TGfxRectangle2D::GetPathIteratorL( TGfxAffineTransform* aAt,
                                                 CGfxPathIterator*& aPitr )
    {
    aPitr = new ( ELeave ) CGfxRectangleIteratorP( this, aAt );
    }

// --------------------------------------------------------------------------
//  void TGfxRectangle2D::GetPathIteratorL( TGfxAffineTransform* aAt,
// ---------------------------------------------------------------------------
 void TGfxRectangle2D::GetPathIteratorL( TGfxAffineTransform* aAt,
                                                 TInt /* aLimit */,
                                                 CGfxPathIterator*& aPitr )
    {
    aPitr = new ( ELeave ) CGfxRectangleIteratorP( this, aAt );
    }

// --------------------------------------------------------------------------
//  void TGfxRectangle2D::operator==( const TGfxRectangle2D& aRect ) const;
// ---------------------------------------------------------------------------
 TInt TGfxRectangle2D::operator==( const TGfxRectangle2D& aRect ) const
{
	return (iX == aRect.iX && iY == aRect.iY && iWidth == aRect.iWidth && iHeight == aRect.iHeight);
}

// --------------------------------------------------------------------------
//  void TGfxRectangle2D::operator!=( const TGfxRectangle2D& aRect ) const;
// ---------------------------------------------------------------------------
 TInt TGfxRectangle2D::operator!=( const TGfxRectangle2D& aRect ) const
{
	return (!(iX == aRect.iX && iY == aRect.iY && iWidth == aRect.iWidth && iHeight == aRect.iHeight));
}
