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


#include "GfxEllipse2D.h"
#include "GfxEllipseIteratorP.h"
#include "GfxFlatteningPathIterator.h"

// --------------------------------------------------------------------------
//  TGfxEllipse2D::TGfxEllipse2D( const TFloatFixPt& aX,
// ---------------------------------------------------------------------------
 TGfxEllipse2D::TGfxEllipse2D( const TFloatFixPt& aX,
                                       const TFloatFixPt& aY,
                                       const TFloatFixPt& aWidth,
                                       const TFloatFixPt& aHeight ) :
         TGfxRectangularShape( aX, aY, aWidth, aHeight )
{
}


// --------------------------------------------------------------------------
//  TGfxEllipse2D::TGfxEllipse2D( const TFloatFixPt& aX1,
// ---------------------------------------------------------------------------
 TGfxEllipse2D::TGfxEllipse2D( const TFloatFixPt& aX1,
                                       const TFloatFixPt& aY1,
                                       const TFloatFixPt& aX2,
                                       const TFloatFixPt& aY2,
                                       TEllipseConstructType aEtype )
    {
    if ( aEtype == EElipseRectangular )
        {
        iX = aX1;
        iY = aY1;
        iWidth = aX2;
        iHeight = aY2;
        }
    else
        {
        // convert cx,cy,rx,ry -> x,y,w,h
        iX = aX1 - aX2;
        iY = aY1 - aY2;
        iWidth = aX2 + aX2;
        iHeight = aY2 + aY2;
        }
    }

// --------------------------------------------------------------------------
//  void TGfxEllipse2D::GetPathIteratorL( TGfxAffineTransform* aAt,
// ---------------------------------------------------------------------------
 void TGfxEllipse2D::GetPathIteratorL( TGfxAffineTransform* aAt,
                                               CGfxPathIterator*& aPitr )
    {
    aPitr = new( ELeave ) CGfxEllipseIteratorP( this, aAt );
    }

// --------------------------------------------------------------------------
//  void TGfxEllipse2D::GetPathIteratorL( TGfxAffineTransform* aAt,
// ---------------------------------------------------------------------------
 void TGfxEllipse2D::GetPathIteratorL( TGfxAffineTransform* aAt,
                                               TInt aLimit,
                                               CGfxPathIterator*& aPitr )
    {
    aPitr = CGfxFlatteningPathIterator::NewL( this, aAt, aLimit );

    }

