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


#include "GfxRoundRectangle2D.h"
#include "GfxRoundRectangleIteratorP.h"
#include "GfxFlatteningPathIterator.h"
#include "GfxRectangleIteratorP.h"


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
//  TGfxRoundRectangle2D::TGfxRoundRectangle2D()
// ---------------------------------------------------------------------------
 TGfxRoundRectangle2D::TGfxRoundRectangle2D()
    : TGfxRectangularShape( 0,
                            0,
                            0,
                            0 ),
      iArcWidth( 0 ),
      iArcHeight( 0 )
    {
    }

// --------------------------------------------------------------------------
//  TGfxRoundRectangle2D::TGfxRoundRectangle2D( const TFloatFixPt& aX,
// ---------------------------------------------------------------------------
 TGfxRoundRectangle2D::TGfxRoundRectangle2D( const TFloatFixPt& aX,
                                                     const TFloatFixPt& aY,
                                                     const TFloatFixPt& aW,
                                                     const TFloatFixPt& aH,
                                                     const TFloatFixPt& aArcWidth,
                                                     const TFloatFixPt& aArcHeight )
    : TGfxRectangularShape( aX,
                            aY,
                            aW,
                            aH ),
      iArcWidth( aArcWidth ),
      iArcHeight( aArcHeight )
    {
    }

// --------------------------------------------------------------------------
//  void TGfxRoundRectangle2D::GetPathIteratorL( TGfxAffineTransform* aAt,
// ---------------------------------------------------------------------------
 void TGfxRoundRectangle2D::GetPathIteratorL( TGfxAffineTransform* aAt,
                                                      CGfxPathIterator*& aPitr )
    {
    TFloatFixPt KZero;
    if ( iArcWidth <= KZero && iArcHeight <= KZero )
        aPitr = new ( ELeave ) CGfxRectangleIteratorP( this, aAt );
    else
        aPitr = new ( ELeave ) CGfxRoundRectangleIteratorP( this, aAt );
    }

// --------------------------------------------------------------------------
//  void TGfxRoundRectangle2D::GetPathIteratorL( TGfxAffineTransform* aAt,
// ---------------------------------------------------------------------------
 void TGfxRoundRectangle2D::GetPathIteratorL( TGfxAffineTransform* aAt,
                                                      TInt aLimit,
                                                      CGfxPathIterator*& aPitr )
    {
    aPitr = CGfxFlatteningPathIterator::NewL( this, aAt, aLimit );

    }


