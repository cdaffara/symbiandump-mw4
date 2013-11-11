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


#include "GfxLine2D.h"
#include "GfxLineIteratorP.h"

#include "GfxFloatFixPt.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
//  TGfxLine2D::TGfxLine2D( const TFloatFixPt& aX1,
// ---------------------------------------------------------------------------
 TGfxLine2D::TGfxLine2D( const TFloatFixPt& aX1,
                                 const TFloatFixPt& aY1,
                                 const TFloatFixPt& aX2,
                                 const TFloatFixPt& aY2 ) :
								 iX1( aX1 ),
								 iY1( aY1 ),
								 iX2( aX2 ),
								 iY2( aY2 )
	{
	}

// --------------------------------------------------------------------------
//  TGfxLine2D::TGfxLine2D( TGfxPoint2D /* aP1 */, TGfxPoint2D /* aP2 */ )
// ---------------------------------------------------------------------------
 TGfxLine2D::TGfxLine2D( TGfxPoint2D /* aP1 */, TGfxPoint2D /* aP2 */ )
    {
    }

// ---------------------------------------------------------------------------
// From MGfxShape
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
//  void TGfxLine2D::GetBounds( const TGfxAffineTransform& aAt,
// ---------------------------------------------------------------------------
 void TGfxLine2D::GetBounds( const TGfxAffineTransform& aAt,
                                     TGfxRectangle2D& aRect )
    {
    TGfxPoint2D p1( iX1, iY1 ), p2( iX2, iY2 );
    aAt.Transform( &p1, & p1, 1 );
    aAt.Transform( &p2, & p2, 1 );
    aRect.iX = ( p1.iX > p2.iX ) ? p2.iX : p1.iX;
    aRect.iY = ( p1.iY > p2.iY ) ? p2.iY : p1.iY;
    aRect.iWidth = ( p1.iX > p2.iX ) ? p1.iX - p2.iX : p2.iX - p1.iX;
    aRect.iHeight = ( p1.iY > p2.iY ) ? p1.iY - p2.iY : p2.iY - p1.iY;
    }

// --------------------------------------------------------------------------
//  void TGfxLine2D::GetPathIteratorL( TGfxAffineTransform* aAt,
// ---------------------------------------------------------------------------
 void TGfxLine2D::GetPathIteratorL( TGfxAffineTransform* aAt,
                                            CGfxPathIterator*& aPitr )
    {
    aPitr = ( CGfxPathIterator * ) new( ELeave )
            CGfxLineIteratorP( this, aAt );
    }

// --------------------------------------------------------------------------
//  void TGfxLine2D::GetPathIteratorL( TGfxAffineTransform* aAt,
// ---------------------------------------------------------------------------
 void TGfxLine2D::GetPathIteratorL( TGfxAffineTransform* aAt,
                                            TInt /* aLimit */,
                                            CGfxPathIterator*& aPitr )
    {
    aPitr = new( ELeave ) CGfxLineIteratorP( this, aAt );
    }

// --------------------------------------------------------------------------
//  TFloatFixPt TGfxLine2D::Length()
// ---------------------------------------------------------------------------
 TFloatFixPt TGfxLine2D::Length()
    {
    TFloatFixPt dx, dy, int0x7f( KMAXFLOATFIX );

    dx = iX1 - iX2;
    dy = iY1 - iY2;
    if ( dx > int0x7f || dy > int0x7f )
        {
        // calculate as integer
        TInt32 tmplen, tx, ty;
        tx = ( TInt32 ) dx;
        ty = ( TInt32 ) dy;
	#ifdef SVG_FLOAT_BUILD
        tmplen = TFloatFixPt::Sqrt( tx * tx + ty * ty );
	#else
        tmplen = TFloatFixPt::FixedSqrtGeneral( tx * tx + ty * ty, 0 );
	#endif
        return TFloatFixPt( tmplen );
        }

    return TFloatFixPt::Sqrt( dx * dx + dy * dy );
    }

// --------------------------------------------------------------------------
// TFloatFixPt TGfxLine2D::AreaSize()
// ---------------------------------------------------------------------------
TFloatFixPt TGfxLine2D::AreaSize()
    {
    return TFloatFixPt( 0 );
    }
    
