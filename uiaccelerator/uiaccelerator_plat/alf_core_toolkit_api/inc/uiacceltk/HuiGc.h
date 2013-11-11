/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition file for CHuiGc (graphics context) class.
*
*/



#ifndef __HUIGC_H__
#define __HUIGC_H__


#include <e32base.h>
#include <gdi.h>

#include <uiacceltk/HuiRealPoint.h>
#include <uiacceltk/HuiRealSize.h>
#include <uiacceltk/HuiRealRect.h>


/* Forward declarations. */
class CHuiTexture;
class THuiFont;
class THuiImage;
class CHuiMesh;
class CHuiTextMesh;
class CHuiCurvePath;
class CHuiTransformation;
class MHuiMappingFunction;

const TInt KHuiGcClipRegionGranularity = 10;

/** Point on a weighted path. */
struct THuiPathPoint
    {
    /** Position of the point. */
    TPoint iPoint;

    /** Weight of the point. */
    TReal32 iWeight;

    inline THuiPathPoint(const TPoint& aPoint, const TReal32& aWeight)
        : iPoint(aPoint), iWeight(aWeight) {}
    };


/** Weighted path. */
typedef RArray<THuiPathPoint> THuiWeightedPath;


/** Horizontal alignment. */
enum THuiAlignHorizontal
    {
    EHuiAlignHLeft,
    EHuiAlignHCenter,
    EHuiAlignHRight,

   /** Alignment that follows the directionality of the UI language. This is converted to left or
        right alignment internally. e.g. into right-aligned for Arabic */
    EHuiAlignHLocale,

    /** Alignment that is opposite to the directionality of the UI language. This is 
        converted to left or right alignment internally. e.g. into left-aligned for Arabic */
    EHuiAlignHLocaleMirrored,

    /** Alignment is text content dependent according to Unicode paragraph alignment rules. 
         This is converted to left or right alignment according to text content. 
         Where used in an editable context, the cursor alignment when empty comes from current input language.
         This should not be used in a non-textual context, but if used, behaves like EHuiAlignHLocale. */
    EHuiAlignHBidirectionalText,

    /** Alignment is text content dependent according to Unicode paragraph alignment rules, but then reversed. 
         This is converted to left or right alignment according to text content. 
         Where used in an editable context, the cursor alignment when empty is opposite to current input language.
         This should not be used in a non-textual context, but if used, behaves like EHuiAlignHLocaleMirrored. */
    EHuiAlignHBidirectionalTextMirrored
    };


/** Vertical alignment. */
enum THuiAlignVertical
    {
    EHuiAlignVTop,
    EHuiAlignVCenter,
    EHuiAlignVBottom
    };


/** Texture matrices. */
enum THuiGcMatrix
    {
    /** Modelview matrix. */
    EHuiGcMatrixModel,

    /** Projection matrix. */
    EHuiGcMatrixProjection,

    /** Texture matrix. */
    EHuiGcMatrixTexture
    };


/** Rendering quality. */
enum THuiQuality
    {
    /** Accurate quality ensures the best visual quality. */
    EHuiQualityAccurate,

    /** Fastest rendering speed in exchange for lower quality. */
    EHuiQualityFast
    };

/** Polygon draw mode */
enum THuiFillMode
    {
    EHuiNoFill,
    EHuiFillNonZero,
    EHuiFillEvenOdd
    };

/**
 * CHuiGc is a graphics context that is used for storing the state of the
 * drawing tools. The graphics context provides a number of relatively
 * low-level drawing routines.
 *
 * @todo  Check which exports are truly useful to applications.
 *
 */
class CHuiGc : public CBase
    {
public:

    /** Display orientation. */
    enum TOrientation
        {
        /** Normal orientation. */
        EOrientationNormal,

        /** 90 degrees counterclockwise. */
        EOrientationCCW90,

        /** 90 degrees clockwise. */
        EOrientationCW90,
        
        /** 180 degrees  */
        EOrientation180        
        };

    /** Rendering features. */
    enum TFeature
        {
        EFeatureTexturing,
        EFeatureBlending,
        EFeatureClipping,
        EFeatureDepthTest,
        EFeatureDepthWrite,
        EFeatureAntialias,
        EFeatureFog,
        };

    /** Blending modes. */
    enum TBlendMode
        {
        EBlendNormal,
        EBlendAdd,
        EBlendReplace
        };

    /** Texture modes. */
    enum TTextureMode
        {
        ETextureModeNormal,

        /** Interpolation between two texture units. Parameter controls how
            much interpolation is done. */
        ETextureModeInterpolate
        };

    /** Projections. */
    enum TProjection
        {
        EProjectionOrthographic,
        EProjectionPerspective
        };

    /**
     * Stretch modes.
     * @see DrawStretchImage()
     */
    enum TStretchMode
        {
        EStretchNone = 0,   ///< Does not stretch
        EStretchHorizontal, ///< Fits to horizontal width (left-right)
        EStretchVertical,   ///< Fits to vertical height
        EStretchFull		///< Stretches the image to the full area of the visual, disregarding the original aspect ratio.

        };
        
	/**
	 * Modes for drawing borders with images.
	 * @see DrawBorders()
	 */        
    enum TBorderMode    
	{
        /**
         * The whole image is stretched to the
         * borders so that the pixels at the center of the image are placed at the 
         * edges of the visual and the pixels at the edge of the image are placed
         * at the outer edge. 
         * 
         * For example, a round-shaped ball having red centrum
         * and white outer rim cretes a red square inner border having white
         * rounded outer border.
         */
        EBorderImage = 0,

        /**
         * Only the edge pixels of the image are used to draw the borders. The edge is 
         * defined with the (left,right,top,bottom-) border widths. The corners of the
         * image are not stretched. The center image inside the borders won't be
         * included in the border drawing.
         * 
         * This mode is used in CHuiImageBrush to draw background images to visuals 
		 * (eg. Soft key backgrounds) and texture-mapped borders to them (for example 
		 * to achieve rounded corners). 
         */
        EBorderFixedCorners

        };

    /** Gradient types. */
    enum TGradientType
        {
        EGradientLinearUp,
        EGradientLinearRight,
        EGradientLinearDown,
        EGradientLinearLeft
        };


    /**
     * Destructor.
     */
    IMPORT_C virtual ~CHuiGc();


    /* Methods. */

    /**
     * Sets the quality preference of the graphics context.
     *
     * @param aQuality  Quality setting.
     */
    IMPORT_C virtual void SetQuality(THuiQuality aQuality);

    /**
     * Determines the quality preference of the graphics context.
     *
     * @return  Quality preference.
     */
    IMPORT_C THuiQuality Quality() const;

    /**
     * Pushes the current global transformation matrix on the stack.
     */
    IMPORT_C virtual void Push(THuiGcMatrix aStack) = 0;

    /**
     * Pops a matrix from the stack.
     */
    IMPORT_C virtual void Pop(THuiGcMatrix aStack) = 0;

    /**
     * Loads an identity matrix as the current matrix.
     */
    IMPORT_C virtual void LoadIdentity(THuiGcMatrix aStack) = 0;

    /**
     * Multiplies the current matrix with another matrix.
     */
    IMPORT_C virtual void Multiply(THuiGcMatrix aStack, TReal32 aMatrix[16]) = 0;

    /**
     * Apply a translation to given matrix stack.
     */
    IMPORT_C virtual void Translate(THuiGcMatrix aStack,
                                    TReal32 aX, TReal32 aY, TReal32 aZ) __SOFTFP = 0;

    /**
     * Apply a scale to given matrix stack.
     */
    IMPORT_C virtual void Scale(THuiGcMatrix aStack,
                                TReal32 aX, TReal32 aY, TReal32 aZ) __SOFTFP = 0;

    /**
     * Apply a rotation to given matrix stack.
     */
    IMPORT_C virtual void Rotate(THuiGcMatrix aStack, TReal32 aAngle,
                                 TReal32 aX, TReal32 aY, TReal32 aZ) __SOFTFP = 0;

    /**
     * Apply a shear to given matrix stack.
     */
    virtual void Shear(THuiGcMatrix /*aStack*/,
                                TReal32 /*aX*/, TReal32 /*aY*/, TReal32 /*aZ*/) __SOFTFP {} // cheers ;)
    
    /**
     * Sets the orientation of the display. This affects clipping and
     * projection.
     *
     * @param aOrientation  Orientation of the display.
     */
    IMPORT_C void SetOrientation(TOrientation aOrientation);

    /**
     * Determines the orientation of the graphics context.
     *
     * @return  Orientation.
     */
    IMPORT_C TOrientation Orientation() const;

    /**
     * Initialize the state of the context.
     */
    IMPORT_C virtual void InitState() = 0;

    /**
     * Selects the default data arrays.
     */
    IMPORT_C virtual void RestoreState() const = 0;

    IMPORT_C void SetProjection(TProjection aProjection = EProjectionOrthographic);

    IMPORT_C TProjection Projection() const;

    /**
     * Sets projection within a custom viewport.
     */
    IMPORT_C void SetProjection(TProjection aProjection, const TRect& aCustomViewport);

    /**
     * Defines an offset for the 3D frustum and updates the projection.
     */
    IMPORT_C void SetFrustumOffset(const THuiRealPoint& aFrustumOffset);

    /**
     * Determines the current frustum offset.
     */
    IMPORT_C THuiRealPoint FrustumOffset() const;

    /**
     * Updates the projection matrix without changing any settings.
     */
    IMPORT_C virtual void UpdateProjection() = 0;

    /**
     * Sets the display area of the normal orientation.
     */
    IMPORT_C void SetDisplayArea(const TRect& aDisplayArea);

    /**
     * Returns the display area. Affected by the display orientation.
     */
    IMPORT_C TRect DisplayArea() const;

    /**
     * Returns the number of available texture units.
     *
     * @return  Number of units (at least 1).
     */
    IMPORT_C virtual TInt TextureUnits() const = 0;

    /**
     * Sets the number of active texture units.
     *
     * @param aTextureUnitCount  Number of units to activate (e.g., 2 => 0,1)
     */
    IMPORT_C virtual void SetTextureUnits(TInt aTextureUnitCount) = 0;

    /**
     * Enable a feature.
     */
    IMPORT_C virtual void Enable(TFeature aFeature, TBool aDoEnable = ETrue) = 0;

    /**
     * Disable a feature.
     */
    IMPORT_C virtual void Disable(TFeature aFeature);

    /**
     * Changes the blending mode.
     */
    IMPORT_C virtual void SetBlendMode(TBlendMode aBlendMode) = 0;

    /**
     * Changes the texture mode.
     *
     * @param aTextureMode  Texturing mode.
     * @param aParam        Parameter for the texturing.
     */
    IMPORT_C virtual void SetTextureMode(TTextureMode aTextureMode, TReal32 aParam = 0) __SOFTFP = 0;

    /**
     * Sets the strength of dimming fog. In order to have effect, enable the
     * Fog feature.
     *
     * @param aColor   Color of dimming.
     * @param aAmount  Amout of dimming (0...1).
     */
    IMPORT_C virtual void SetDimmingFog(const TRgb& aColor, TReal32 aAmount) __SOFTFP = 0;

    /**
     * Clear rect using the current pen color
     */
    IMPORT_C virtual void Clear(const TRect& aRect) = 0;
    
    /**
     * Clear using the current pen color.
     */
    IMPORT_C virtual void Clear() = 0;

    /**
     * Clear the depth buffer.
     */
    IMPORT_C virtual void ClearDepth() = 0;
    
    /**
     * Clear both color and depth buffer
     */
    IMPORT_C virtual void ClearColorAndDepth() = 0;

	/**
	 * Do initialization of new frame.
	 * Called by CHuiDisplay when starting to render new frame.
	 * Graphics context can do renderer specific state initializations here.
	 */
    IMPORT_C virtual void InitNewFrame() = 0;

    /**
     * Push the current clipping rectangle onto the clip stack.
     */
	IMPORT_C virtual void PushClip();

    /**
     * Pop a clipping rectangle off of the clip stack and activate it.
     */
    IMPORT_C virtual void PopClip();

    /**
     * Apply a clipping rectangle.
     *
     * @param aClipRect  Clipping rectangle.
     */
    IMPORT_C virtual void Clip(const TRect& aClipRect);

    /**
     * Apply a clipping region.
     *
     * @param aClipRegion  Clipping region.
     */
    IMPORT_C virtual void Clip(const TRegion& aClipRegion); 
    
    /**
     * Apply a clipping region.
     *
     * @param aClipRegion  Region containing clipping rectangles.
     */

	IMPORT_C virtual void SetClipRegion(const TRegion& aClipRegion);
	
	/**
	 * Cancel an applied clipping rectangle or region.
	 */

	IMPORT_C virtual void CancelClipping();
	
    /**
     * Replace the current clipping rectangle.
     *
     * @param aClipRect  Clipping rectangle in display coordinates.
     */
    IMPORT_C virtual void SetClip(const TRect& aClipRect);

    /**
     * Returns the currently active clipping rectangle.
     */
    IMPORT_C TRect ClipRect() const;

    /**
     * Returns the currently active clipping region.
     */
    const TRegion& ClipRegion() const;


    /**
     * Determines the number of pushed clipping rectangles on the clip stack.
     *
     * @return  Size of the clip stack.
     */
    IMPORT_C TInt ClipStackCount() const;

    /**
     * Sets the pen color and alpha.
     */
    IMPORT_C void SetPenColorAlpha(const TRgb& aColor);

    /**
     * Sets the pen color.
     */
    IMPORT_C void SetPenColor(const TRgb& aColor);

    /**
     * Sets the pen alpha level.
     *
     * @param aAlpha  Alpha level (0..255).
     */
    IMPORT_C void SetPenAlpha(TInt aAlpha);

    /**
     * Determines the pen color and alpha.
     *
     * @return  Pen color and alpha.
     */
    IMPORT_C TRgb PenColorAlpha() const;

    /**
     * Determines the pen color.
     *
     * @return  Pen color.
     */
    IMPORT_C TRgb PenColor() const;

    /**
     * Determines the pen alpha level.
     *
     * @return  Alpha level (0..255).
     */
    IMPORT_C TInt PenAlpha() const;

    /**
     * Sets the current font.
     *
     * @param aFont  Font texture.
     */
    IMPORT_C void SetFont(THuiFont& aFont);

    /**
     * Sets the object alignment mode for both horizontal and vertical
     * directions. Used in DrawImage() and DrawImages().
     *
     * @todo Should SetAlign() work also in other GC drawing operations?
     *
     * @param aHorizontalAlign  Horizontal alignment mode. The horizontal
     *                          alignment mode determines if the used
     *                          coordinates are relative to left edge, center
     *                          or right edge of the screen.
     * @param aVerticalAlign    Vertical alignment mode. The vertical
     *                          alignment mode determines if the used
     *                          coordinates are relative to top-edge, vertical
     *                          center or bottom of the screen.
     */
    IMPORT_C void SetAlign(THuiAlignHorizontal aHorizontalAlign,
                           THuiAlignVertical aVerticalAlign);

    /**
     * Determines the horizontal align mode.
     */
    IMPORT_C THuiAlignHorizontal AlignHorizontal() const;

    /**
     * Determines the vertical align mode.
     */
    IMPORT_C THuiAlignVertical AlignVertical() const;

    /**
     * Draws a line of text. Creates a temporary text mesh, so this is not as
     * efficient as calling DrawText on an existing mesh.
     */
    IMPORT_C virtual void DrawTextL(const TDesC& aText,
                                    const THuiRealRect& aBounds,
                                    TReal32 aShadow = 0) __SOFTFP;

    /**
     * Draws a single line of text using the current pen color, font, and
     * alignment mode.
     *
     * @param aTextMesh  Text string.
     * @param aBounds    Bounding box for the text.
     * @param aShadow    Darkness of text shadow.
     */
    IMPORT_C virtual void DrawText(const CHuiTextMesh& aTextMesh,
                                   const THuiRealRect& aBounds,
                                   TReal32 aShadow = 0) __SOFTFP;

    /**
     * Draws an image using the current pen color and alignment mode.
     *
     * @param aImage  Image specification.
     * @param aPos    Origin point. Relative to the selected alignment point,
     *                @see SetAlign()
     * @param aSize   Size of the image.
     */
    IMPORT_C virtual void DrawImage(const THuiImage& aImage,
                                    const THuiRealPoint& aPos,
                                    const THuiRealSize& aSize);

    /**
     * Draws two images using the current pen color and alignment mode.
     * The images are combined using the current texture mode.
     *
     * @param aImage   Image specification.
     * @param aImage2  Image specification.
     * @param aPos     Origin point. Relative to the selected alignment point,
     *                 @see SetAlign()
     * @param aSize    Size of the image.
     */
    IMPORT_C virtual void DrawImages(const THuiImage& aImage,
                                     const THuiImage& aImage2,
                                     const THuiRealPoint& aPos,
                                     const THuiRealSize& aSize);

    /**
     * Draws an image scaled to a specific size inside the bounds.
     *
     * @param aImage   Image specification.
     * @param aSize    Size of the image.
     * @param aBounds  Rectangle in which to place the image,
     *                 based on alignment. @see SetAlign()
     */
    IMPORT_C virtual void DrawImage(const THuiImage& aImage,
                                    const THuiRealSize& aSize,
                                    const THuiRealRect& aBounds);

    /**
     * Draws two images scaled to a specific size inside the bounds.
     * The images are combined using the current texture mode.
     *
     * @param aImage   First image specification.
     * @param aImage2  Second image specification.
     * @param aSize    Size of the image.
     * @param aBounds  Rectangle in which to place the image,
     *                 based on alignment. @see SetAlign()
     *
     * @todo If multitexturing is not supported, use two calls to
     * DrawImage with suitable blending to achieve the same effect in
     * two passes..
     *
     * Currently, if multitexturing is not supported, only the first
     * image will be drawn.
     */
    IMPORT_C virtual void DrawImages(const THuiImage& aImage,
                                     const THuiImage& aImage2,
                                     const THuiRealSize& aSize,
                                     const THuiRealRect& aBounds);

    /**
     * Draws an image, placing it inside the given bounds using the alignment
     * settings.
     */
    IMPORT_C virtual void DrawImage(const THuiImage& aImage,
                                    const THuiRealRect& aBounds);


    /**
     * Draws a rectangle with the current graphics context parameters.
     *
     * @param aRect  Rectangle to draw.
     */
    IMPORT_C virtual void DrawRect(const TRect& aRect) = 0;
    
    /**
     * Draws a solid line of uniform thickness with the current graphics context parameters.
     *
     * @param aStart     Start of line.
     * @param aEnd       End of line.     
     * @param aThickness Thickness of line in pixels.
     */
    IMPORT_C virtual void DrawLine(const TPoint& aStart, const TPoint& aEnd, const TInt aThickness) = 0;

    /**
     * Draws a stretched image. 
     * 
     * You may define a starting and ending sections for the image that will 
     * not be stretched. This is useful for example in drawing scrollbars with
     * arrows on a single run where the center of the image is stretched,
     * but the arrows at the ends are drawn unstretched.
     *
     * @param aMode        Stretch mode. 
     * @param aImage       Image to draw stretched.
     * @param aRect        Rectangle to fill.
     * @param aStretchStartWidth  
     * 					   Define width of unstretchable start section (left or 
     * 					   upper section). The pixels in this section will not be
     *                     stretched. Only the center section of the image
     *                     (between aStartWidth and aEndWidth) gets stretched.
     *                     Defaults to zero.
     * @param aStretchEndWidth    
     * 					   Define width of unstretchable end section (right or 
     * 					   lower section). The pixels in this section will not be
     *                     stretched. Only the center section of the image
     *                     (between aStartWidth and aEndWidth) gets stretched.
     *                     Defaults to zero.
     */
    IMPORT_C virtual void DrawStretchImage(TStretchMode aMode,
                                           const THuiImage& aImage,
                                           const THuiRealRect& aRect,
                                           TInt aStretchStartWidth = 0,
                                           TInt aStretchEndWidth = 0) = 0;

    /**
     * Draws edges with a color or using a texture.
     *
     * @param aOuterRect         The outer edge of the borders.
     * @param aLeftBorderWidth   The visible width of the left border, 
     * 							 the width is assumed to be pixel units 
     * 							 inside from the respective edge 
     * 							 border. Use 0.0 for no left border.
     * @param aRightBorderWidth  The visible width of the right border, 
     * 							 the width is assumed to be pixel units 
     * 							 inside from the respective edge 
     * 							 border. Use 0.0 for no right border.
     * @param aTopBorderWidth    The visible height of the top border, 
     * 							 the height is assumed to be pixel units 
     * 							 inside from the respective edge 
     * 							 border. Use 0.0 for no border.
     * @param aBottomBorderWidth The visible height of the bottom border, 
     * 							 the height is assumed to be pixel units 
     * 							 inside from the respective edge 
     * 							 border. Use 0.0 for no border.
     * @param aBorderMode		 Border drawing mode, defines how the
     * 							 image is streched to the borders.
     * 							 aImage needs to be defined in order 
     * 							 to this modes to be effective.
     * @param aImage			 The image to be used to draw borders,
     * 							 according to the border drawing mode.
     * 							 If left NULL will use the current pen 
     * 							 color to draw single-color borders.
     */
    IMPORT_C virtual void DrawBorders(const TRect& aOuterRect, 
    						  TReal32 aLeftBorderWidth,
                              TReal32 aRightBorderWidth, 
    						  TReal32 aTopBorderHeight,
                              TReal32 aBottomBorderHeight, 
                              TBorderMode aBorderMode = EBorderImage,
                              const THuiImage* aImage = 0) __SOFTFP = 0;


    /**
     * Draws a path from the beginning to aPos.
     *
     * @param aPath      Weighted path.
     * @param aOrigin    Offset.
     * @param aStartPos  Start position on the path.
     * @param aEndPos    End position on the path.
     * @param aAlphaFunction  Function for determining alpha values.
     * @param aWidthFunction  Function for determining line thickness.
     */
    IMPORT_C virtual void DrawPath(const CHuiCurvePath& aPath,
                          const TPoint& aOrigin,
                          TReal32 aStartPos,
                          TReal32 aEndPos,
                          MHuiMappingFunction* aAlphaFunction = 0,
                          MHuiMappingFunction* aWidthFunction = 0) __SOFTFP = 0;

    /**
     * Sets up the appropriate rendering mode and draws the mesh. The caller
     * is responsible for setting up the transformation and projection.
     *
     * @param aMesh            Mesh to draw.
     * @param aImage           Primary surface image.
     * @param aSecondaryImage  Secondary surface image.
     * @param aSecondaryAlpha  Alpha level of the secondary image.
     */
    IMPORT_C virtual void DrawMesh(const CHuiMesh& aMesh, 
                          const THuiImage* aImage = 0,
                          const THuiImage* aSecondaryImage = 0,
                          TReal32 aSecondaryAlpha = 0.0) __SOFTFP = 0;

    /**
     * Draws a thick arc.
     *
     * @param aOrigin           Center of the arc.
     * @param aRadius           Radius of the arc.
     * @param aStart            Start angle of the arc (degrees).
     * @param aEnd              End angle of the arc (degrees).
     * @param aAnglePerSegment  Degrees per segment.
     * @param aWidth            Width of the segment line.
     * @param aImage            Texture to use on the segment line.
     * @param aAlphaFunc        Angle (degrees) to alpha (0..1) mapping function.
     * @param aAbsoluteAngleParm  If true, absolute angles given to alpha func.
     */
    IMPORT_C virtual void DrawArc(const TPoint& aOrigin, const TSize& aRadius,
                                  TReal32 aStart, TReal32 aEnd, TInt aAnglePerSegment,
                                  TReal32 aWidth, const THuiImage& aImage,
                                  TReal32 (*aAlphaFunc)(TReal32)=0,
                                  TBool aAbsoluteAngleParm = EFalse) __SOFTFP = 0;

    /**
     * Draws a gradient.
     *
     * @param aType        Direction of the gradient.
     * @param aRect        Coordinates of the rectangle in which the gradient is drawn.
     * @param aStartColor  Color of the gradient on the start edge.
     * @param aEndColor    Color of the gradient on the end edge.
     * @param aStartOpacity  Optional opacity for the start edge. Opacity is gradually
     *                     interpolated as well.
     * @param aEndOpacity  Optional opacity for the end edge.
     * @param aTexCoords   Optional texture coordinates for the gradient rectangle.
     *                     If this is NULL, texturing is disabled when the gradient is
     *                     drawn. Otherwise, the currently bound texture is used as
     *                     the texture in the gradient rectangle.
     */
    IMPORT_C virtual void DrawGradient(TGradientType aType, const TRect& aRect,
                                       const TRgb& aStartColor, const TRgb& aEndColor,
                                       TReal32 aStartOpacity = 1.0,
                                       TReal32 aEndOpacity = 1.0,
                                       const THuiRealRect* aTexCoords = 0) __SOFTFP = 0;

    /**
     * Returns the currently active font.
     */
    IMPORT_C THuiFont* Font();

    /**
     * Determines the near clip distance of the context.
     */
    IMPORT_C TReal32 NearClipDistance() const __SOFTFP;

    /**
     * Determines the far clip distance of the context.
     */
    IMPORT_C TReal32 FarClipDistance() const __SOFTFP;

    /**
     * Returns the FOV factor.
     */
    IMPORT_C TReal32 FovFactor() const __SOFTFP;

    /**
     * Determines the Z offset for 2D graphics so that they are projected
     * correctly when using a 3D projection.
     */
    IMPORT_C TReal32 Offset2D() const __SOFTFP;

    /**
     * Sets a depth offset. The depth offset is used when drawing overlaid
     * surfaces with the same Z coordinates. Normally the depth offset is
     * always zero.
     *
     * @param aOffset  Offset units.
     */
    IMPORT_C virtual void SetDepthOffset(TInt aOffset) = 0;

    /**
     * Makes a screen capture of the graphics context's current content.
     *
     * @param aSize  Returns the size of the buffer.
     * @return  Rgb888 version of the content. Caller must pop and destroy
     *          the returned buffer.
     */
    IMPORT_C virtual TUint8* CaptureLC(TSize& aSize) const = 0;

    /**
     * Sets a transformation that is applied as the final step of projection.
     * To remove the transformation, set to <code>NULL</code>.
     *
     * @param aWindowTransformation  Window transformation steps.
     */
    IMPORT_C void SetWindowTransformation(CHuiTransformation* aWindowTransformation);


    // @TODO: Renderer pluginization change: Should these be pure virtual or not?
    /**
     * Enables transformations for clipping rects. Implementing this method in
     * inherited classes is optional (if e.g. transformed clipping rectangles
     * aren't supported)
     *
     * @param aEnable  To transform clipping rects or not.
     */
    IMPORT_C virtual void EnableTransformedClippingRects(TBool aEnable=ETrue);

    /**
     * Replace the current clipping rectangle. Implementing this method in
     * inherited classes is optional (if e.g. transformed clipping rectangles
     * aren't supported)
     *
     * @param aClipRect  Clipping rectangle in display coordinates.
     * @param aTransformClipRect  If ETrue. new clipping rect is transformed before it is taken as current rect.
     */
    IMPORT_C virtual void SetClip(const TRect& aClipRect, TBool aTransformClipRect);

    /**
     * Transforms a dirty rectangle. Implementing this method in
     * inherited classes is optional (if e.g. dirty rects aren't supported)
     *
     * @param aRect  Rectangle to transform.
     */
    IMPORT_C virtual void TransformDirtyRect(THuiRealRect& aRect);


    /* Static methods. */

    /**
     * Determines the horizontal alignment used for text in the current locale.
     * Applications can use this for any text whose placement should change
     * depending on the locale.
     *
     * @return  Left or right alignment.
     */
    IMPORT_C static THuiAlignHorizontal LocaleTextAlignment();

public:
    
    /**
     * Notifies image (and its texture object if it exists) that it is currently being 
     * drawn into the display as the given size. Image may use this information to
     * resize itself to a more optimal size.
     *
     * @aImage Image that is drawn.
     * @aRect Rect where image is drawn.
     * 
     */
    IMPORT_C void NotifyImagePreferredSize(const THuiImage& aImage, THuiRealRect aRect);

public:

    /**
     * Copy screen to bitmap.
     * @internal
     * @param aBitmap bitmap to which data should be copied.
     * @return error code, KErrNone upon success.
     */
    TInt CopyScreenToBitmap(CFbsBitmap* aBitmap);
    
protected:

    /* Constructors. */

    /**
     * Constructor.
     */
    IMPORT_C CHuiGc();

    /* Methods. */
    
    /**
     * Determines the projection viewport.
     */
    IMPORT_C TRect ProjectionViewport() const;
    
    /**
     * Determines the display area not affected by orientation.
     */
    IMPORT_C TRect RawDisplayArea() const;

    /**
     * Determines the current window transformation.
     *
     * @return  CHuiTransformation, or <code>NULL</code> if none defined.
     */
    IMPORT_C const CHuiTransformation* WindowTransformation() const;

    /**
     * Orients the rectangle back to the normal orientation.
     */
    IMPORT_C TRect UnOrientRect(const TRect& aOrientedRect) const;

    /**
     * Orients the region back to the normal orientation.
     */
    IMPORT_C void UnOrientRegion(TRegion& aOrientedRegion) const;
    
    /**
     * Calculate offset using the current alignment mode.
     *
     * @param bounds  Size of the object being aligned.
     *
     * @return  Offset from top-left corner.
     */
    IMPORT_C TPoint AlignOffset(const TSize& aBounds) const;

    /**
     * Sets the current OpenGL color based on the current selected Pen
     * color value.
     */
    IMPORT_C virtual void UpdateColor(TReal32 aAlphaFactor = 1.f) __SOFTFP = 0;

    /**
     * Draw the image.
     *
     * @param aImage    Image to draw.
     * @param aTopLeft  Coordinates of the top left corner.
     * @param aSize     Size of the image.
     */
    IMPORT_C virtual void DoDrawImage(const THuiImage& aImage, const THuiRealPoint& aTopLeft,
                             const THuiRealSize& aSize) = 0;

    /**
     * Draws two images simultaneously. Requires at least two texture units.
     *
     * Segment count must match and sizes must match if using multiple segments.
     * Otherwise nothing is drawn.
     *
     * @param aImage    Image to draw.
     * @param aImage2   Secondary image to draw.
     * @param aTopLeft  Coordinates for the top left corner.
     * @param aSize     Size of the image.
     */
    IMPORT_C virtual void DoDrawImages(const THuiImage& aImage,
                              const THuiImage& aImage2,
                              const THuiRealPoint& aTopLeft,
                              const THuiRealSize& aSize) = 0;


protected: // New methods
    
    /**
     * Provides expandability, helps keeping the binary compatibility. Since virtual
     * table is now exported and this class is dll derivable and the implementation
     * is more difficult to change, hence this method, which can provide additional
     * extension APIs.
     * 
     * @param aExtensionUid     UID, which is being used for recognizing the extension
     * @param aExtensionParams  Return pointer to the extension API, once recognized from the extension uid
     */
    IMPORT_C virtual void GcExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
    
private:

    RRegion* CreateRecycledRegion();

    void DeleteRecycledRegion(RRegion* aRegion);

protected:

    /** Stack of clipping regions. In user coordinate space (oriented). */
    RPointerArray<RRegion> iClipRegionStack;

    /** Current clipping region. In user coordinate space (oriented). */
    RRegionBuf<KHuiGcClipRegionGranularity> iClipRegion;

private:

    /** Private temp region for general purpose, used to avoid excessive allocations of RRegion heap objects */
    mutable RRegionBuf<KHuiGcClipRegionGranularity> iTempRegion;

    /** Recycled clipping region objects for performance reasons */
    mutable RPointerArray<RRegion> iRecycledRegions;

    /** Size and position of the display in normal orientation. */
    TRect iDisplayArea;

    /** Orientation of the display. */
    TOrientation iOrientation;

    /** Projection mode. */
    TProjection iProjection;

    /** Projection viewport. */
    TRect iProjectionViewport;

    TReal32 iNearClipDistance;

    TReal32 iFarClipDistance;

    TReal32 iFovFactor;

    /** Not owned. */
    CHuiTransformation* iWindowTransform;

    /** Offset for the 3D projection frustum center. */
    THuiRealPoint iFrustumOffset;

    /** Pen color for drawing lines. */
    TRgb iPenColor;

    /** Pen alpha level for drawing lines. */
    TUint8 iPenAlpha;

    /** Horizontal alignment mode. */
    THuiAlignHorizontal iAlignHorizontal;

    /** Vertical alignment mode. */
    THuiAlignVertical iAlignVertical;

    /** Current font. Not owned.
        Fonts are owned by the texture manager. */
    THuiFont* iCurrentFont;

    /** Completeness (0..1) of the frustum offset. This is used when adjusting
        the perspective effect in 3D projection. A complete frustum offset
        means that the perspective is fully local to the object being drawn
        (i.e., the visual's center point). A completeness of zero means that
        the perspective is always in the center of the display. */
    TReal32 iFrustumOffsetCompleteness;

    /** Rendering quality. */
    THuiQuality iQuality;
    
    /** Clip region count held separately in case OOM happens */
    TInt iClipRegionStackCount;
    };

#endif  // __HUIGC_H__
