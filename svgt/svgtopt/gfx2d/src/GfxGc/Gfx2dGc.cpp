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
#include "Gfx2dGcInterface.h"

// --------------------------------------------------------------------------
// CGfx2dGcVGR::CGfx2dGcVGR() : iScale( 1 ),
// ---------------------------------------------------------------------------
CGfx2dGcVGR::CGfx2dGcVGR() 
    {
    }

//
// ==========================================================================
// Create a new instance.
// ==========================================================================
// --------------------------------------------------------------------------
//  CGfx2dGcVGR* CGfx2dGcVGR::NewL( CFbsBitmap* aFrameBuffer, TFontSpec& aFontSpec )
// ---------------------------------------------------------------------------
 CGfx2dGcVGR* CGfx2dGcVGR::NewL( const TSize /*aBufferSize*/, TFontSpec& /*aFontSpec*/, CSvgBitmapFontProvider */*aSvgBitmapFontProvider*/ )
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::ConstructL( CFbsBitmap* aFrameBuffer, TFontSpec& aFontSpec )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::ConstructL( const TSize /*aBufferSize*/, TFontSpec& /*aFontSpec*/, CSvgBitmapFontProvider* /*aSvgBitmapFontProvider*/ )
    {
    }

// ==========================================================================
// Destructor.
// ==========================================================================
// --------------------------------------------------------------------------
//  CGfx2dGcVGR::~CGfx2dGcVGR()
// ---------------------------------------------------------------------------
CGfx2dGcVGR::~CGfx2dGcVGR()
    {
    }


// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetFrameBufferL( CFbsBitmap* aFrameBuffer )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::ChangeBufferSizeL( const TSize /*aSize */)
{
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::ActivateL()
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::ActivateL()
    {
    }

// void CGfx2dGcVGR::AdjustShapeComplexityL( TInt32 /*aVertexSize*/ )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::AdjustShapeComplexityL( TInt32 /*aVertexSize*/ )
    {
    }


void CGfx2dGcVGR::DoDrawL( MGfxShape* /*aShape*/ )
    {
    }

void CGfx2dGcVGR::PostDraw()
{
}

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetWindingRule( TGfxWindingRule aRule )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetWindingRule( TGfxWindingRule /*aRule*/ )
    {
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::Draw( MGfxShape* aShape )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::DrawL( MGfxShape* /*aShape*/ )
    {
    }


// --------------------------------------------------------------------------
// TUint8 CGfx2dGcVGR::Composite()
// ---------------------------------------------------------------------------
TUint8 CGfx2dGcVGR::Composite()
    {
    return 0;
    }

// --------------------------------------------------------------------------
// TGfxColor CGfx2dGcVGR::ForegroundColor()
// ---------------------------------------------------------------------------
TGfxColor CGfx2dGcVGR::ForegroundColor()
    {
    return TGfxColor(0);
    }

// --------------------------------------------------------------------------
// TGfxColor CGfx2dGcVGR::BackgroundColor()
// ---------------------------------------------------------------------------
TGfxColor CGfx2dGcVGR::BackgroundColor()
    {
    return TGfxColor(0);
    }

// --------------------------------------------------------------------------
// MGfxPaint* CGfx2dGcVGR::Paint()
// ---------------------------------------------------------------------------
MGfxPaint* CGfx2dGcVGR::Paint()
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// TGfxRenderingHints* CGfx2dGcVGR::RenderingHints()
// ---------------------------------------------------------------------------
TGfxRenderingHints* CGfx2dGcVGR::RenderingHints()
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// TGfxStroke CGfx2dGcVGR::Stroke()
// ---------------------------------------------------------------------------
TGfxStroke CGfx2dGcVGR::Stroke()
    {
    return TGfxStroke();
    }

// --------------------------------------------------------------------------
// TGfxAffineTransform CGfx2dGcVGR::Transform()
// ---------------------------------------------------------------------------
TGfxAffineTransform CGfx2dGcVGR::Transform()
    {
    return TGfxAffineTransform();
    }


// --------------------------------------------------------------------------
// MGfxShape* CGfx2dGcVGR::Clip()
// ---------------------------------------------------------------------------
MGfxShape* CGfx2dGcVGR::Clip()
    {
    return NULL;
    }


// --------------------------------------------------------------------------
// void CGfx2dGcVGR::GetDashArray( CArrayFix<TFixPt>*& aArray,
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::GetDashArray( CArrayFix<TFloatFixPt>*& /*aArray*/,
                                      TFloatFixPt& /*aOffset*/ )
    {
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetDashOffset( TFixPt aDashOffset )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetDashOffset( TFloatFixPt /*aDashOffset*/ )
{
}

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetComposite( const TUint8 aComposite )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetComposite( const TUint8 /*aComposite*/ )
    {
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetForegroundColor( const TGfxColor& aColor )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetForegroundColor( const TGfxColor& /*aColor*/ )
    {
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetBackgroundColor( const TGfxColor& aColor )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetBackgroundColor( const TGfxColor& /*aColor*/ )
    {
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetPaint( MGfxPaint* aPaint )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetPaint( MGfxPaint* /* aPaint*/ )
    {
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetStroke( TGfxStroke aStroke )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetStroke( TGfxStroke /*aStroke*/ )
    {
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetRenderingHints( const TGfxRenderingHints& aRenderingHints )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetRenderingHints( const TGfxRenderingHints& /*aRenderingHints*/ )
    {
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetTransform( const TGfxAffineTransform& aTransform )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetTransform( const TGfxAffineTransform& /*aTransform*/ )
    {
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetClip( const TGfxRectangle2D& aClip )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetClip( const TGfxRectangle2D& /*aClip*/ )
    {
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetDashArrayL( CArrayFix<TFloatFixPt>* aArray )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetDashArrayL( CArrayFix<TFloatFixPt>* /*aArray*/ )
    {
    }


// --------------------------------------------------------------------------
// TSize CGfx2dGcVGR::DeviceBounds()
// ---------------------------------------------------------------------------
TSize CGfx2dGcVGR::DeviceBounds()
    {
    return TSize();
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetAntialiasingMode( const TInt32 /*aAntialiasingEnable*/)
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetAntialiasingMode( const TInt32 /*aAntialiasingEnable*/)
    {
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetFont( const CFont* aFont )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetFont( const CFont* /*aFont*/ )
    {
    }
// --------------------------------------------------------------------------
// CFont* CGfx2dGcVGR::Font()
// ---------------------------------------------------------------------------
CFont* CGfx2dGcVGR::Font()
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::GetSystemFontScaled(TFloatFixPt aHeight, 
//                                     const TDesC& aTypefaceName,
//                                     CFont*& aFont,
//                                     TFontSpec& aFontSpec )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::GetFontScaled( TFloatFixPt /*aHeight*/, 
                             const TDesC& /*aTypefaceName*/,
                             CFont*& /*aFont*/,
                             TFontSpec& /*aFontSpec */)
{
}

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetFontSize( const TFloatFixPt aFontSize )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetFontSize( const TFloatFixPt /* aFontSize */)
    {
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetFontWeight( const TInt32 aFontWeight )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetFontWeight( const TInt32 /* aFontWeight */)
    {
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetFontStyle( const TInt32 aFontStyle )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetFontStyle( const TInt32 /*aFontStyle*/ )
    {
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetFontFamily( CDesCArrayFlat* aFamilies )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetFontFamily( CDesCArrayFlat* /*aFamilies*/ )
    {
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetTextanchor( const TInt32 aTextAnchor )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetTextanchor( const TInt32 /*aTextAnchor*/ )
    {
    }

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetTextDecoration( const TInt32 aTextDecoration )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetTextDecoration( const TInt32 /*aTextDecoration*/ )
    {
    }


// Text Drawing
// --------------------------------------------------------------------------
// void CGfx2dGcVGR::DrawStringL( const TDesC& aDesc,
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::DrawStringL( const TDesC& /*aDesc*/,
                                     TFloatFixPt /*aX*/,
                                     TFloatFixPt /*aY*/,
                                     TInt8 /* aTextAnchor */,
                                     TInt8 /* aTextDecoration */,
                                     CDesCArrayFlat* /* aFamilies */,
                                     TReal /*aWordSpacing*/,
                                     TReal /*aLetterSpacing*/,
                                     CArrayFix<TReal32>* /*aArrayRotate*/,
                                     CArrayFix<TFloatFixPt>* /*aArrayX*/,
                                     CArrayFix<TFloatFixPt>* /*aArrayY*/,
                                     TSize& /*aBboxSizeResult*/, 
				     CFont*& /*aFont*/, 
				     TFontSpec& /*aFontSpec*/ )
    {
    }



// Raster image drawing
// --------------------------------------------------------------------------
// void CGfx2dGcVGR::DrawImage( CFbsBitmap* aImage,
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::DrawImage( CFbsBitmap* /*aImage*/,
                                      const TGfxRectangle2D& /*aImageRect*/,
                                      TBool /*aHasAlpha*/ )
    {
    }


// ==========================================================================
// Clear the whole graphics context with the background color
// ==========================================================================
// --------------------------------------------------------------------------
// void CGfx2dGcVGR::Clear(TUint32 aRGBA8888Color)
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::Clear(TUint32 /*aRGBA8888Color*/)
    {
    }

// ==========================================================================
// This method copies the internal RGBA8888 framebuffer to CfbsBitmap that
// is specified in constructor. The CfbsBitmap could be 4k color (RGB0444),
// 64k color (RGB565), or 16M color (RGB888).
// ==========================================================================
// --------------------------------------------------------------------------
//  void CGfx2dGcVGR::UpdateFramebufferL()
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::UpdateFramebufferL( CFbsBitmap* /*aBitmap*/, CFbsBitmap* /*aMask*/ )
    {
    }

void CGfx2dGcVGR::UpdateFramebufferL( CFbsBitmap* /*aBitmap*/, CFbsBitmap* /*aMask*/,TSize /*BitmapSize*/,TDisplayMode /*aBitmapDspMode*/,TDisplayMode /*aMaskDspMode*/ )

    {



    }
// ====================================================================================
// Blend bitmap with background according to group opacity
// This function gets called with the opacity framebuffer and the group opacity value
// The opacity buffer is initialized with the 0xAA55AA55 value. The pixels not matching
// ====================================================================================

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::BlendWithBackground(TUint32* aFrameBuffer, TReal32 aOpacity)
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::BlendWithBackground(TUint32* /*aColorBuffer*/, TReal32 /*aOpacity*/)
{
}

// ====================================================================================
// return iBuffer
// ==========================================================================
// Generate 8 bit gray or 1bit B/W mask from internal ARGB8888 frame buffer.
// ==========================================================================
// --------------------------------------------------------------------------
// void CGfx2dGcVGR::GenerateMask(CFbsBitmap* aMask)
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::GenerateMask(CFbsBitmap* /*aMask*/)
    {
    }

// ==========================================================================
//  This function calls the low level function to set fill opacity value
// ==========================================================================

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetFillOpacity(TFixPt aFillOpacity)
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetFillOpacity(TFloatFixPt /*aFillOpacity*/)
    {
    }
// ==========================================================================
//  This function calls the low level function to set stroke opacity value
// ==========================================================================

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetStrokeOpacity(TFixPt aStrokeOpacity)
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetStrokeOpacity(TFloatFixPt /*aStrokeOpacity*/)
    {
    }
// ==========================================================================
//  This function is called to set the dithering flag
// ==========================================================================

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::SetDoDithering( TBool aDoDithering )
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::SetDoDithering( TBool /*aDoDithering*/ )
    {
    }

TUint32* CGfx2dGcVGR::ColorBuffer()
{
		return NULL;
}

// --------------------------------------------------------------------------
// void CGfx2dGcVGR::ColorConvertTo64K
// ---------------------------------------------------------------------------
void CGfx2dGcVGR::ColorConvertTo64K( TUint32* /*aDestAddress*/, const TSize& /*aSize*/ )
    {
}

void CGfx2dGcVGR::UpdateColorBuffer()
    {
    }
