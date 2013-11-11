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


#ifndef GFX2DGCOPENVG_H
#define GFX2DGCOPENVG_H

#include "Gfx2dGcInterface.h"
#include "GfxRendererInfoP.h"
#include "VGRenderer.h"
#include <SVGRendererId.h>
#include "VGSurface.h"
#include <VG/openvg.h>

class CSvgtBitmap;

const TUint8 KMediaAnimationVisible =  2;
  
/**
 * This class provides the interface create a graphics context.
 *
 *  @lib Gfx2D.lib
 *  @since 1.0
 */
class CGfx2dGcOpenVG : public CGfx2dGc
    {
    public:

        /**
         * Two-phase constructor.
         *
         * @since 1.0
         * @param
         * @return
         */
         static CGfx2dGcOpenVG* NewL( const TSize aBufferSize, TFontSpec& aFontSpec, CSvgBitmapFontProvider* aSvgBitmapFontProvider, TBool aIsMain = ETrue );

         static CGfx2dGcOpenVG* NewL( const TSize aBufferSize, TFontSpec& aFontSpec, CSvgBitmapFontProvider* aSvgBitmapFontProvider,SVGRendererId aRendererType,TBool aRenderOption = ETrue );
         void                 GetFontScaled( TFloatFixPt aHeight, const TDesC& aTypefaceName, CFont*& aFont, TFontSpec& aFontSpec );

        /**
         * Destructor
         */
         virtual                ~CGfx2dGcOpenVG();

				 
        /**
         * Draw the given shape.
         *
         * @since 1.0
         * @param aShape : shape to draw.
         * @return
         */
         void                   DrawL( MGfxShape* aShape );

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
                                                     TSize& aBoundingBoxResult,
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
         * Get the foreground color setting.
         *
         * @since 1.0
         * @param
         * @return foreground color.
         */
         TGfxColor              ForegroundColor();

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
         * Set the stroke-width.
         *
         * @since 3.2
         * @param aWidth : stroke-width
         * @return none.
         */
         void SetStrokeWidth( const TFloatFixPt& aWidth );
         
         
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
		 void					SetDashOffset( TFloatFixPt aDashOffset ) ;
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

         void UpdateFramebufferL( CFbsBitmap* aBitmap, CFbsBitmap* aMask,TSize BitmapSize,TDisplayMode aBitmapDspMode,TDisplayMode aMaskDspMode );

         void UpdateFramebufferL( CSvgtBitmap* aBitmap, CSvgtBitmap* aMask );
         void UpdateFramebufferL( CSvgtBitmap* aBitmap, CSvgtBitmap* aMask,TSize BitmapSize,TDisplayMode aBitmapDspMode,TDisplayMode aMaskDspMode );
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
          * Generate mask
          *
          * @since
          * @param aMask: pointer to the mask
          * @return
          */
		 void GenerateMask(CFbsBitmap* aMask);
		 void GenerateMask(CSvgtBitmap* aMask);
		  
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

         // Group opacity methods
         void                   BindToImageL();
         void                   UnbindFromImageL( TReal32 aOpacity );

         void                   BindToMediaImageL(TInt aWidth, TInt aHeigth);
         void                   UnbindFromMediaImageL(const TGfxAffineTransform& aAffineTransform,TReal32 aOpacity,TInt aWidth, TInt aHeight, TInt aDraw );
         void                   SetClipMediaElement( TSize aSize );
         void                   GetMatrix(TReal32 * m);
         void SetBitmapHeader(const TDesC* aHeaderData);
		 /**
         * Get the NVG-TLV data 
         *
         * @since 1.0
         * @return : pointer to the NVG-TLV data
         */
         const TPtrC8 TLVEncodedData() const;
  private:

        /**
         * Constructor
         */
                                        CGfx2dGcOpenVG( TBool aIsMainContext );
        /**

        /**
         * Constructor
         */
                                        CGfx2dGcOpenVG();

        /**
         * Two-phase constructor
         */
        void                            ConstructL(const TSize aBufferSize, TFontSpec& aFontSpec, CSvgBitmapFontProvider* aSvgBitmapFontProvider);

        /**

        * Constructor NGA with the renderer selector parameter

        */

        void                            ConstructL(const TSize aBufferSize, TFontSpec& aFontSpec, CSvgBitmapFontProvider* aSvgBitmapFontProvider,SVGRendererId aRendererType);

        /**
         * Perform the drawing operations of the given shape.
         *
         * @since 1.0
         * @param aShape : shape to draw
         * @return
         */
        void                            DoDrawL( MGfxShape* aShape );

		void 							PseudoDoDrawL( MGfxShape* aShape );

        /**
         * Destroy the OpenVG context and delete the 32-bit intermediate color buffer
         *
         * @since 1.0
         * @return
         */
        void DestroyContext();
        /**
         * Converts OpenVG Errors into Symbian error codes
         *
         * @since 
         * @return TInt
         */
        TInt OpenVGErrorToSymbianError( TInt aError );

		void ApplyTransform( TGfxAffineTransform& aAffineTransform );
		void ApplyTransformMediaElement(const TGfxAffineTransform& aAffineTransform,TSize& aSize );

		VGbitfield SetFillStroke(VGPaint aFillPaint, VGPaint aStrokePaint, TGfxRectangle2D& bBox);

        VGImage CreateVGImage( void* aBuffer, TSize aSize, TDisplayMode aDisplayMode, TBool aPreMultiplied = ETrue );

        // This is for work-around for OpenVG to simulate multiple contexts
        void SetupContextL();
        void Flush();
        /**
         * Sets the Paint,Stroke and Path handle back to VG_INVALID_HANDLE
         *
         * @since 
         * @return void
         */
        void ResetContextHandle();
		//To find the bounding box for system text..
		TGfxRectangle2D GetBBoxForSystemText( const TDesC& aText, 
                                     TFloatFixPt aX,
                                     TFloatFixPt aY );
                                     
		TGfxRectangle2D GetTRect(TGfxPoint2D lTransTopLeftCorner,
												TGfxPoint2D lTransTopRightCorner,
												TGfxPoint2D lTransBottomLeftCorner,
												TGfxPoint2D lTransBottomRightCorner);                                     

        // Method to handle cleanup (turning off settings, such as dasharray) for efficiency.
        void PostDraw();

    private:
        // 32-bit buffer for OpenVG to draw to
        // Color conversion/copy to Symbian bitmap is needed as final step
        TSize                           iColorBufferSize;

        TDisplayMode    iDisplayMode;
        
        // M2G: Variable to specify the target rendering option (rendering onto bitmap or onto target)
        TBool iRenderOption;
        TBool iGraphicsContextCreated;
                
        TGfxRectangle2D                 iClip;
        MGfxPaint*                      iFillColor;
        TFloatFixPt						iFillOpacity;
      	TFloatFixPt                     iScale;
        TGfxStroke                      iStroke;
        TGfxAffineTransform             iTransform;
        TGfxColor                       iStrokeColor;
        TFloatFixPt						iStrokeOpacity;
        TGfxColor                       iBackgroundColor;
        CFont*                          iFont;
        CArrayFix<VGfloat>*             iDashArray;
        TFloatFixPt                     iDashOffset;
        TFloatFixPt                     iFontSize;
        TInt32                          iFontWeight;
        TInt32                          iFontStyle;
        CDesCArrayFlat*                 iFamilies;
        TInt32                          iTextAnchor;
        TInt32                          iTextDecoration;
        TGfxRendererInfoP               iRenderInfo;
        
        // Flag used for using CFbsBitmap::DataAddress() instead of allocating
        // TInt32 array to draw shapes.
        TFontSpec                       iFontSpec;
        //OpenVG stuff

        VGPaint							iFillPaint;
        VGPaint							iStrokePaint;
        VGPath                          iPath;

        RArray<VGImage>                 iGroupOpacityImages;
        RPointerArray<TUint32>          iGroupOpacityBuffers;

        VGubyte*                        iPathSegmentTypes;
        TInt                            iPathSegmentTypeCount;
        TGfxRectangle2D                 iClipMain;
        TSize                           iColorBufferSizeMain;
        TInt32                          iRenderQuality;

        CVGRenderer *     iVgRenderer;

        CVGSurface  *     iVgSurface;

        SVGRendererId    iCurrentRendererType;
    };

#endif      // GFX2DGCOPENVG_H
