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


#ifndef GFX2DGCINT_H
#define GFX2DGCINT_H


#include <w32std.h>
#include <e32std.h>
#include <bitstd.h>
#include <badesca.h>
#include <fbs.h>
#include <gdi.h>

#include "GfxRenderingHints.h"
#include "GfxStroke.h"
#include "GfxColor.h"

#include "SvgBitmapFontProvider.h"
class MGfxShape;
class MGfxPaint;
class CSvgtBitmap;

typedef enum
{
    SVG_CAP_BUTT                    = 0,
    SVG_CAP_ROUND                   = 1,
    SVG_CAP_SQUARE                  = 2,
    SVG_INVALID_LINE_CAP            = -1
} SVGLineCap;

typedef enum
{
    SVG_JOIN_MITER                  = 0,
    SVG_JOIN_ROUND                  = 1,
    SVG_JOIN_BEVEL                  = 2,
    SVG_INVALID_LINE_JOIN           = -1
} SVGLineJoin;

/**
 * This class provides the interface create a graphics context.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class CGfx2dGc : public CBase
    {
    public:

        /**
         * Draw the given shape.
         *
         * @since 1.0
         * @param aShape : shape to draw.
         * @return
         */
        virtual void                   DrawL( MGfxShape* aShape )=0;

        /**
         * Draw the given string starting at the given context coordinate,
         * and stylized by the given parameters.
         *
         * @since 1.0
         * @param aDesc : string to draw
         * @param aX : x coordinate
         * @param aTextAnchor : text anchor
         * @param aTextDecoration : text decoration
         * @param aFamilies : font family names
         * @param aWordSpacing : spacing description for each word
         * @param aLetterSpacing : spacing description for each letter
         * @param aArrayRotate : angle of rotation
         * @param aArrayX : array of x coordinates
         * @param aArrayY : array of x coordinates
         * @return
         */
        virtual void                   DrawStringL( const TDesC& aDesc,
                                                     TFloatFixPt aX,
                                                     TFloatFixPt aY,
                                                     TInt8 aTextAnchor,
                                                     TInt8 aTextDecoration,
                                                     CDesCArrayFlat* aFamilies,
                                                     TReal aWordSpacing,
                                                     TReal aLetterSpacing,
                                                     CArrayFix<TReal32>* aArrayRotate,
                                                     CArrayFix<TFloatFixPt>* aArrayX,
                                                     CArrayFix<TFloatFixPt>* aArrayY,
                                                     TSize& aBoundingBoxResult,
						     CFont*& aFont,
						     TFontSpec& aFontSpec )=0;

        /**
         * Draw the given bitmap at the given top-left coordinates.
         *
         * @since 1.0
         * @param aDesc : bitmap image to draw
         * @param aX : x coordinate
         * @param aY : y coordinate
         * @return
         */
        virtual void                   DrawImage( CFbsBitmap* aImage,
                                                  const TGfxRectangle2D& aImageRect,
                                                  TBool aHasAlpha = EFalse )=0;

        /**
         * Clear the whole graphics context.
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                   Clear(TUint32 aRGBA8888Color)=0;

        /**
         * Get the foreground color setting.
         *
         * @since 1.0
         * @param
         * @return foreground color.
         */
        virtual TGfxColor              ForegroundColor()=0;

        /**
         * Get the paint setting.
         *
         * @since 1.0
         * @param
         * @return paint.
         */
        virtual MGfxPaint*             Paint()=0;

        /**
         * Get the rendering hints setting.
         *
         * @since 1.0
         * @param
         * @return rendering hints.
         */
        virtual TGfxRenderingHints*    RenderingHints()=0;

        /**
         * Get the stroke setting.
         *
         * @since 1.0
         * @param
         * @return stroke.
         */
        virtual TGfxStroke             Stroke()=0;

        /**
         * Get the boundary setting.
         *
         * @since 1.0
         * @param
         * @return boundary.
         */
        virtual TSize                  DeviceBounds()=0;
        /**
         * Get the font setting.
         *
         * @since 1.0
         * @param
         * @return font.
         */

        virtual CFont*                 Font()=0;


        /**
         * Set the foreground color.
         *
         * @since 1.0
         * @param aColor : foreground color
         * @return
         */
        virtual void                   SetForegroundColor( const TGfxColor& aColor )=0;


        /**
         * Set the background color.
         *
         * @since 1.0
         * @param aColor : background color
         * @return
         */
        virtual void                   SetBackgroundColor( const TGfxColor& aColor )=0;

		
        /**
         * Set the stroke-width.
         *
         * @since 3.2
         * @param aWidth : stroke-width
         * @return none.
         */
		virtual void SetStrokeWidth( const TFloatFixPt& aWidth )=0;
        
        
        /**
         * Set the paint info.
         *
         * @since 1.0
         * @param aPaint : paint info
         * @return
         */
        virtual void                   SetPaint( MGfxPaint* aPaint )=0;

        /**
         * Set the stroke info.
         *
         * @since 1.0
         * @param aStroke : stroke info
         * @return
         */
        virtual void                   SetStroke( TGfxStroke aStroke )=0;

        /**
         * Set the rendering hints.
         *
         * @since 1.0
         * @param aRenderingHints : rendering hints
         * @return
         */
        virtual void                   SetRenderingHints( const TGfxRenderingHints& aRenderingHints )=0;

        /**
         * Set the transform info.
         *
         * @since 1.0
         * @param aTransform : transform info
         * @return
         */
        virtual void                   SetTransform( const TGfxAffineTransform& aTransform )=0;

        /**
         * Set the clipping info.
         *
         * @since 1.0
         * @param aClip : clipping rectangle
         * @return
         */
        virtual void                   SetClip( const TGfxRectangle2D& aClip )=0;

        /**
         * Set the dash array info.
         *
         * @since 1.0
         * @param aArray : array of points
         * @param aOffset : offset point
         * @return
         */
        virtual void                   SetDashArrayL( CArrayFix<TFloatFixPt>* aArray)=0;
		virtual void					SetDashOffset( TFloatFixPt aDashOffset ) =0;
        /**
         * Set the font size.
         *
         * @since 1.0
         * @param aFontSize : font size
         * @return
         */
        virtual void                   SetFontSize( const TFloatFixPt aFontSize )=0;

        /**
         * Set the winding rule.
         *
         * @since 1.0
         * @param aRule : winding rule
         * @return
         */
        virtual void                   SetWindingRule( TGfxWindingRule aRule )=0;

        /**
         * Set the font weight.
         *
         * @since 1.0
         * @param aFontWeight : font weight value
         * @return
         */
        virtual void                   SetFontWeight( const TInt32 aFontWeight )=0;

        /**
         * Set the font style.
         *
         * @since 1.0
         * @param aFontStyle : font style
         * @return
         */
        virtual void                   SetFontStyle( const TInt32 aFontStyle )=0;

        /**
         * Set the font family.
         *
         * @since 1.0
         * @param aFamilies : font family names
         * @return
         */
        virtual void                   SetFontFamily( CDesCArrayFlat* aFamilies )=0;

        /**
         * Set the text anchor
         *
         * @since 1.0
         * @param aTextAnchor : text anchor value
         * @return
         */
        virtual void                   SetTextanchor( const TInt32 aTextAnchor )=0;

        /**
         * Set the text decoration
         *
         * @since 1.0
         * @param aTextDecoration : text decoration value
         * @return
         */
        virtual void                   SetTextDecoration( const TInt32 aTextDecoration )=0;

        /**
         * Set the anti-alias mode
         *
         * @since 1.0
         * @param aTextDecoration : anti-alias mode value
         * @return
         */
        virtual void                   SetAntialiasingMode( const TInt32 aAntialiasingEnable )=0;

        /**
         * This method copies the internal RGBA8888 framebuffer to CfbsBitmap that
         * is specified in constructor. The CfbsBitmap could be 4k color (RGB0444),
         * 64k color (RGB565), or 16M color (RGB888).
         *
         * @since
         * @param
         * @return
         */
        virtual void UpdateFramebufferL( CFbsBitmap* aFrameBuffer, CFbsBitmap* aMask ) = 0;
        virtual void UpdateFramebufferL( CFbsBitmap* aFrameBuffer, CFbsBitmap* aMask,TSize aFrameBufferSize,TDisplayMode aFrameBufferMode,TDisplayMode aMaskDspMode ) =0;

        
        virtual void UpdateFramebufferL( CSvgtBitmap* /*aFrameBuffer*/, CSvgtBitmap* /*aMask*/ ){}
        virtual void UpdateFramebufferL( CSvgtBitmap* /*aFrameBuffer*/, CSvgtBitmap* /*aMask*/,TSize /*aFrameBufferSize*/,TDisplayMode /*aFrameBufferMode*/,TDisplayMode /*aMaskDspMode*/ ){}
        /**
         * Blend aFrameBuffer with background buffer using the opacity value
         *
         * @since 1.0
         * @param aFrameBuffer: Buffer to be blended with background
         *        aOpacity: opacity value (0<=aOpacity<=1.0)
         * @return
         */
         virtual void BlendWithBackground(TUint32* aFrameBuffer, TReal32 aOpacity)=0;

         /**
          * Generate mask
          *
          * @since
          * @param aMask: pointer to the mask
          * @return
          */
		virtual void GenerateMask(CFbsBitmap* aMask)=0;
        virtual void GenerateMask(CSvgtBitmap* /*aMask*/){};

		  /**
         * Set the fill opacity value
        *
         * @since 1.0
         * @param aFillOpacity : opacity value (0<=aFillOpacity<=1.0)
         * @return
         */
        virtual void                   SetFillOpacity(TFloatFixPt aFillOpacity)=0;

        /**
         * Set the stroke opacity value
         *
         * @since 1.0
         * @param aStrokeOpacity : opacity value (0<=aStrokeOpacity<=1.0)
         * @return
         */
        virtual void                   SetStrokeOpacity(TFloatFixPt aStrokeOpacity)=0;

        /**
         * Set the flag to do or ignore dithering.
         *
         * @since 1.0
         * @param aDoDithering : TRUE or False
         * @return
         */
        virtual void                   SetDoDithering( TBool aDoDithering )=0;

        /**
         * Request to change the buffer size.
         *
         * @since 1.0
         * @param aSize : size of new buffer
         * @return
         */
        virtual void                   ChangeBufferSizeL( const TSize aSize ) = 0;

        // Rasterizer methods for group-opacity
        virtual TUint32* ColorBuffer() { return NULL; };
        virtual void UpdateColorBuffer() {};

        // OpenVG methods for group-opacity
        virtual void BindToImageL() {};
        virtual void UnbindFromImageL( TReal32 /*aOpacity*/ ) {};

        virtual void GetFontScaled( TFloatFixPt aHeight,
			     const TDesC& aTypefaceName,
                             CFont*& aFont,
                             TFontSpec& aFontSpec ) = 0;

        // This is for work-around for OpenVG to simulate multiple contexts
        // VGR does not need to do anything.
        virtual void SetupContextL() {};
        virtual void Flush() {};
        virtual void DestroyContext()=0;
        /**
          * Get the NVG-TLV Encoded data.
          *
          * @since 1.0
          * @return : pointer to the NVG-TLV data
        */
        virtual const TPtrC8 TLVEncodedData() const=0;

		protected:
		
		//Stores Font Type information as passed by CSvgEngineInterfaceImpl
        CSvgBitmapFontProvider *iSvgBitmapFontProvider;
    };

/**
 * DUMMY placeholder to prevent BC break in SVGEngine - deprecated and not used.
 * 
 *  @since 1.0
 */
class CGfx2dGcVGR : public CGfx2dGc
    {

    /**
     * Two-phase constructor.
     *
     * @since 1.0
     * @param
     * @return
     */
     static CGfx2dGcVGR*       NewL( const TSize aBufferSize, TFontSpec& aFontSpec, CSvgBitmapFontProvider *aSvgBitmapFontProvider );

    /**
     * Destructor
     */
     virtual                ~CGfx2dGcVGR();

    //old  CFont*               GetFontScaled( TFixPt aHeight, const TDesC& aTypefaceName );
     void       GetFontScaled( TFloatFixPt aHeight, 
                         const TDesC& aTypefaceName,
                         CFont*& aFont,
                         TFontSpec& aFontSpec );

    /**
     * Adjust the shape complexity.
     *
     * @since 1.0
     * @param aVertexSize : size of vertices to render.
     * @return
     */
     void                   AdjustShapeComplexityL( TInt32 aVertexSize );

    /**
     * Draw the given shape.
     *
     * @since 1.0
     * @param aShape : shape to draw.
     * @return
     */
     void                   DrawL( MGfxShape* aShape );

    /**
     * Determine if given string should be drawn right to left
     *
     * @since 1.0
     * @param aDesc : string to check if right to left
     * @return true if right to left false otherwise
     */
    TBool                   IsRightToLeft(const TDesC& aDesc );

    /**
     * Draw the given string starting at the given context coordinate,
     * and stylized by the given parameters.
     *
     * @since 1.0
     * @param aDesc : string to draw
     * @param aX : x coordinate
     * @param aTextAnchor : text anchor
     * @param aTextDecoration : text decoration
     * @param aFamilies : font family names
     * @param aWordSpacing : spacing description for each word
     * @param aLetterSpacing : spacing description for each letter
     * @param aArrayRotate : angle of rotation
     * @param aArrayX : array of x coordinates
     * @param aArrayY : array of x coordinates
     * @return
     */
     void                   DrawStringL( const TDesC& aDesc,
                                                 TFloatFixPt aX,
                                                 TFloatFixPt aY,
                                                 TInt8 aTextAnchor,
                                                 TInt8 aTextDecoration,
                                                 CDesCArrayFlat* aFamilies,
                                                 TReal aWordSpacing,
                                                 TReal aLetterSpacing,
                                                 CArrayFix<TReal32>* aArrayRotate,
                                                 CArrayFix<TFloatFixPt>* aArrayX,
                                                 CArrayFix<TFloatFixPt>* aArrayY,
                                                 TSize& aBboxSizeResult, 
                         CFont*& aFont, 
                         TFontSpec& aFontSpec);

    /**
     * Draw the given bitmap at the given top-left coordinates.
     *
     * @since 1.0
     * @param aDesc : bitmap image to draw
     * @param aX : x coordinate
     * @param aY : y coordinate
     * @return
     */
     void                   DrawImage( CFbsBitmap* aImage,
                                               const TGfxRectangle2D& aImageRect,
                                               TBool aHasAlpha = EFalse );

    /**
     * Clear the whole graphics context.
     *
     * @since 1.0
     * @param
     * @return
     */
     void                   Clear(TUint32 aRGBA8888Color);

    /**
     * Get the composite setting.
     *
     * @since 1.0
     * @param
     * @return composite value.
     */
     TUint8                 Composite();

    /**
     * Get the foreground color setting.
     *
     * @since 1.0
     * @param
     * @return foreground color.
     */
     TGfxColor              ForegroundColor() ;

    /**
     * Get the background color setting.
     *
     * @since 1.0
     * @param
     * @return background color.
     */
     TGfxColor              BackgroundColor();

    /**
     * Get the paint setting.
     *
     * @since 1.0
     * @param
     * @return paint.
     */
     MGfxPaint*             Paint();

    /**
     * Get the rendering hints setting.
     *
     * @since 1.0
     * @param
     * @return rendering hints.
     */
     TGfxRenderingHints*    RenderingHints();

    /**
     * Get the stroke setting.
     *
     * @since 1.0
     * @param
     * @return stroke.
     */
     TGfxStroke             Stroke();

    /**
     * Get the transform setting.
     *
     * @since 1.0
     * @param
     * @return transform.
     */
     TGfxAffineTransform    Transform();

    /**
     * Get the clipping area setting.
     *
     * @since 1.0
     * @param
     * @return clipping area.
     */
     MGfxShape*             Clip();

    /**
     * Get the boundary setting.
     *
     * @since 1.0
     * @param
     * @return boundary.
     */
     TSize                  DeviceBounds();
    /**
     * Get the font setting.
     *
     * @since 1.0
     * @param
     * @return font.
     */

     CFont*                 Font();

    /**
     * Get the stroke setting.
     *
     * @since 1.0
     * @param aArray : array of points to store dash array info
     * @param aOffset : offset point dash array offset info
     * @return
     */
     void                   GetDashArray( CArrayFix<TFloatFixPt>*& aArray,
                                                  TFloatFixPt& aOffset );

    /**
     * Set the composite info.
     *
     * @since 1.0
     * @param aComposite : composite info
     * @return
     */
     void                   SetComposite( const TUint8 aComposite );

    /**
     * Set the foreground color.
     *
     * @since 1.0
     * @param aColor : foreground color
     * @return
     */
     void                   SetForegroundColor( const TGfxColor& aColor );

    /**
     * Set the background color.
     *
     * @since 1.0
     * @param aColor : background color
     * @return
     */
     void                   SetBackgroundColor( const TGfxColor& aColor );

    /**
     * Set the paint info.
     *
     * @since 1.0
     * @param aPaint : paint info
     * @return
     */
     void                   SetPaint( MGfxPaint* aPaint );

    /**
     * Set the stroke info.
     *
     * @since 1.0
     * @param aStroke : stroke info
     * @return
     */
     void                   SetStroke( TGfxStroke aStroke );

    /**
     * Set the rendering hints.
     *
     * @since 1.0
     * @param aRenderingHints : rendering hints
     * @return
     */
     void                   SetRenderingHints( const TGfxRenderingHints& aRenderingHints );

    /**
     * Set the transform info.
     *
     * @since 1.0
     * @param aTransform : transform info
     * @return
     */
     void                   SetTransform( const TGfxAffineTransform& aTransform );

    /**
     * Set the clipping info.
     *
     * @since 1.0
     * @param aClip : clipping rectangle
     * @return
     */
     void                   SetClip( const TGfxRectangle2D& aClip );

    /**
     * Set the font.
     *
     * @since 1.0
     * @param aFont : font
     * @return
     */

     void                   SetFont( const CFont* aFont );

    /**
     * Set the dash array info.
     *
     * @since 1.0
     * @param aArray : array of points
     * @param aOffset : offset point
     * @return
     */
     void                   SetDashArrayL( CArrayFix<TFloatFixPt>* aArray);
     void                   SetDashOffset( TFloatFixPt aDashOffset );
    /**
     * Set the font size.
     *
     * @since 1.0
     * @param aFontSize : font size
     * @return
     */
     void                   SetFontSize( const TFloatFixPt aFontSize );

    /**
     * Set the winding rule.
     *
     * @since 1.0
     * @param aRule : winding rule
     * @return
     */
     void                   SetWindingRule( TGfxWindingRule aRule );

    /**
     * Set the font weight.
     *
     * @since 1.0
     * @param aFontWeight : font weight value
     * @return
     */
     void                   SetFontWeight( const TInt32 aFontWeight );

    /**
     * Set the font style.
     *
     * @since 1.0
     * @param aFontStyle : font style
     * @return
     */
     void                   SetFontStyle( const TInt32 aFontStyle );

    /**
     * Set the font family.
     *
     * @since 1.0
     * @param aFamilies : font family names
     * @return
     */
     void                   SetFontFamily( CDesCArrayFlat* aFamilies );

    /**
     * Set the text anchor
     *
     * @since 1.0
     * @param aTextAnchor : text anchor value
     * @return
     */
     void                   SetTextanchor( const TInt32 aTextAnchor );

    /**
     * Set the text decoration
     *
     * @since 1.0
     * @param aTextDecoration : text decoration value
     * @return
     */
     void                   SetTextDecoration( const TInt32 aTextDecoration );

    /**
     * Set the anti-alias mode
     *
     * @since 1.0
     * @param aTextDecoration : anti-alias mode value
     * @return
     */
     void                   SetAntialiasingMode( const TInt32 aAntialiasingEnable );

    /**
     * This method copies the internal RGBA8888 framebuffer to CfbsBitmap that
     * is specified in constructor. The CfbsBitmap could be 4k color (RGB0444),
     * 64k color (RGB565), or 16M color (RGB888).
     *
     * @since
     * @param
     * @return
     */

    void UpdateFramebufferL( CFbsBitmap* aBitmap, CFbsBitmap* aMask );
    void UpdateFramebufferL( CFbsBitmap* aFrameBuffer, CFbsBitmap* aMask,TSize aFrameBufferSize,TDisplayMode aFrameBufferMode,TDisplayMode aMaskDspMode );
        
    /**
     * Blend aFrameBuffer with background buffer using the opacity value
     *
     * @since 1.0
     * @param aFrameBuffer: Buffer to be blended with background
     *        aOpacity: opacity value (0<=aOpacity<=1.0)
     * @return
     */
     void BlendWithBackground(TUint32* aFrameBuffer, TReal32 aOpacity);

    /**
     * return ibuffer
     *
     * @since 1.0
     * @param
     *
     * @return pointer
     */

    void GenerateMask(CFbsBitmap* aMask);

      /**
     * Set the fill opacity value
     *
     * @since 1.0
     * @param aFillOpacity : opacity value (0<=aFillOpacity<=1.0)
     * @return
     */
    void                   SetFillOpacity(TFloatFixPt aFillOpacity);

    /**
     * Set the stroke opacity value
     *
     * @since 1.0
     * @param aStrokeOpacity : opacity value (0<=aStrokeOpacity<=1.0)
     * @return
     */
    void                   SetStrokeOpacity(TFloatFixPt aStrokeOpacity);

    /**
     * Set the flag to do or ignore dithering.
     *
     * @since 1.0
     * @param aDoDithering : TRUE or False
     * @return
     */
    void                   SetDoDithering( TBool aDoDithering );

    /**
     * Request to change the buffer size.
     *
     * @since 1.0
     * @param aSize : size of new buffer
     * @return
     */
     void                   ChangeBufferSizeL( const TSize aBufferSize );

    /**
     * Request to update the context color buffer.
     *
     * @since 1.0
     * @return
     */
     void                   UpdateColorBuffer();

    /**
     * Get the 32-bit color buffer for this gfx context.
     *
     * @since 1.0
     * @return
     */
      TUint32* ColorBuffer();

private:

    /**
     * Constructor
     */
                                    CGfx2dGcVGR();

    /**
     * Two-phase constructor
     */
    void                            ConstructL( const TSize aBufferSize, TFontSpec& aFontSpec, CSvgBitmapFontProvider *aSvgBitmapFontProvider);

    /**
     * Perform the drawing operations of the given shape.
     *
     * @since 1.0
     * @param aShape : shape to draw
     * @return
     */
    void                            DoDrawL( MGfxShape* aShape );


    /**
     * Clear context routine in assembly for performance.
     * @since 1.0
     * @param pBitmap : pointer to bitmap
     * @param aColor : color to clear background
     * @param aSize : number of pixels to clear
     * @return
     */
    void                            ClearAsm( TInt* pBitmap,
                                              TInt aColor,
                                              TInt aSize );

    /**
     * Activate this context with the given graphics device.
     *
     * @since 1.0
     * @param : aDevice -- graphics device.
     * @return
     */
    void                   ActivateL();


    void ColorConvertTo64K( TUint32* aDestAddress, const TSize& aSize );

    void PostDraw();
    };
#endif      // GFX2DGCINT_H
