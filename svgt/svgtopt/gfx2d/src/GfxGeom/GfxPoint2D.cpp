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


#include "GfxPoint2D.h"


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------
//  TGfxPoint2D::TGfxPoint2D()
// ---------------------------------------------------------------------------
 TGfxPoint2D::TGfxPoint2D()
    {
    }

// --------------------------------------------------------------------------
//  TGfxPoint2D::TGfxPoint2D( TFloatFixPt aX, TFloatFixPt aY ) : iX( aX ), iY( aY )
// ---------------------------------------------------------------------------
 TGfxPoint2D::TGfxPoint2D( TFloatFixPt aX, TFloatFixPt aY ) : iX( aX ), iY( aY )
    {
    }

// --------------------------------------------------------------------------
//  TFloatFixPt TGfxPoint2D::Distance( const TGfxPoint2D& aPoint )
// ---------------------------------------------------------------------------
 TFloatFixPt TGfxPoint2D::Distance( const TGfxPoint2D& aPoint )
    {
    return TFloatFixPt::Sqrt( DistanceSq( aPoint ) );
    }

// --------------------------------------------------------------------------
//  TFloatFixPt TGfxPoint2D::DistanceSq( const TGfxPoint2D& aPoint )
// ---------------------------------------------------------------------------
 TFloatFixPt TGfxPoint2D::DistanceSq( const TGfxPoint2D& aPoint )
    {
    TFloatFixPt dx = iX - aPoint.iX;
    TFloatFixPt dy = iY - aPoint.iY;
    return dx * dx + dy * dy;
    }
    
// ==========================================================================
// Equals operator
// ==========================================================================
 TInt TGfxPoint2D::operator==( const TGfxPoint2D& aVal ) const
    {
    return (iX == aVal.iX && iY == aVal.iY);
    }

// ==========================================================================
// Equals operator
// ==========================================================================
 TInt TGfxPoint2D::operator!=( const TGfxPoint2D& aVal ) const
    {
    return (!(iX == aVal.iX && iY == aVal.iY));
    }
