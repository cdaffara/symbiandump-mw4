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


#include "GfxRenderingHints.h"


const TUint32 KCurveQualityMask = 0x00000007;  // bit 0-2
const TUint32 KVisibilityMask = 0x00000008;    // bit 3
const TUint32 KFontEngineMask = 0x00000010;    // bit 4


// --------------------------------------------------------------------------
//  TGfxRenderingHints::TGfxRenderingHints()
// ---------------------------------------------------------------------------
 TGfxRenderingHints::TGfxRenderingHints()
    {
    iHintBits = KVisibilityMask;    // Default visibility = visible
    iHintBits |= ( KCurveQualityMask & 2 ); // Default Flattening level = 2

    // Notes:
    //   KFontEngineMask bitp is 0 -> use scalable font as default
    }


// ==========================================================================
// Visibility
// ==========================================================================
// --------------------------------------------------------------------------
//  void TGfxRenderingHints::SetVisibility( TBool aVisibility )
// ---------------------------------------------------------------------------
 void TGfxRenderingHints::SetVisibility( TBool aVisibility )
    {
    if ( aVisibility )
        {
        iHintBits |= KVisibilityMask;
        }
    else
        {
        iHintBits &= ~KVisibilityMask;
        }
    }

// --------------------------------------------------------------------------
//  TBool TGfxRenderingHints::Visibility()
// ---------------------------------------------------------------------------
 TBool TGfxRenderingHints::Visibility()
    {
    return ( ( iHintBits & KVisibilityMask ) != 0 );
    }

// ==========================================================================
// Curve quality
// ==========================================================================
// --------------------------------------------------------------------------
//  void TGfxRenderingHints::SetCurveQuality( TInt aCurveQuality )
// ---------------------------------------------------------------------------
 void TGfxRenderingHints::SetCurveQuality( TInt aCurveQuality )
    {
    iHintBits &= ~KCurveQualityMask;                    // clear
    iHintBits |= ( KCurveQualityMask & aCurveQuality ); // set
    }

// --------------------------------------------------------------------------
//  TInt TGfxRenderingHints::CurveQuality()
// ---------------------------------------------------------------------------
 TInt TGfxRenderingHints::CurveQuality()
    {
    return ( iHintBits & KCurveQualityMask );
    }

// ==========================================================================
// switch rendering engine (Open Font system/bitmap font)
// ==========================================================================
// --------------------------------------------------------------------------
//  void TGfxRenderingHints::SetFastFontRendering( TBool aFastFontRendering )
// ---------------------------------------------------------------------------
 void TGfxRenderingHints::SetFastFontRendering( TBool aFastFontRendering )
    {
    if ( aFastFontRendering )
        {
        iHintBits |= KFontEngineMask;
        }
    else
        {
        iHintBits &= ~KFontEngineMask;
        }
    }

// --------------------------------------------------------------------------
//  TBool TGfxRenderingHints::FastFontRendering()
// ---------------------------------------------------------------------------
 TBool TGfxRenderingHints::FastFontRendering()
    {
    return ( ( iHintBits & KFontEngineMask ) != 0 );
    }


// --------------------------------------------------------------------------
//  void TGfxRenderingHints::SetShapeAntiAliasing( TBool /* aShapeAntiAliasing */ )
// ---------------------------------------------------------------------------
 void TGfxRenderingHints::SetShapeAntiAliasing( TBool /* aShapeAntiAliasing */ )
    {
    }

// --------------------------------------------------------------------------
//  TBool TGfxRenderingHints::ShapeAntiAliasing()
// ---------------------------------------------------------------------------
 TBool TGfxRenderingHints::ShapeAntiAliasing()
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
//  TGfxImageInterpolation TGfxRenderingHints::ImageInterpolation()
// ---------------------------------------------------------------------------
 TGfxImageInterpolation TGfxRenderingHints::ImageInterpolation()
    {
    return EGfxInterpolationNearestNeighbor;
    }

