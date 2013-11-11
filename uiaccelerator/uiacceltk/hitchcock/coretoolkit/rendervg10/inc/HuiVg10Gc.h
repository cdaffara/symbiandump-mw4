/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Defines CHuiVg10Gc, an OpenVG 1.0 version of the HUITK
*                graphics context.
*
*/

 

#ifndef __HUIVG10GC_H__
#define __HUIVG10GC_H__


#include <e32def.h>
#include <VG/openvg.h>
#include <VG/vgu.h>
#include <e32cmn.h>

#include "uiacceltk/HuiGc.h"
#include "uiacceltk/HuiRealPoint.h"
#include "uiacceltk/HuiRealSize.h"
#include "uiacceltk/HuiRealRect.h"
#include "uiacceltk/HuiSegmentedTexture.h"
#include "HuiMatrixStack.h"

/* Forward declarations. */
#ifdef __NVG
class MNVGIcon;
class CNvgEngine;
#endif
class CFbsBitmap;

/**
 * CHuiVg10Gc implements an OpenVG 1.0 version of the HUITK
 * graphics context.
 */
NONSHARABLE_CLASS(CHuiVg10Gc) : public CHuiGc
    {
    friend class CHuiVg10CurvePath;

public:

    /* Constructors and destructor. */

    /**
     * Constructs a new graphics context.
     */
    static CHuiVg10Gc* NewL();

    /**
     * Constructs a new graphics context and leaves it on the cleanup stack.
     */
    static CHuiVg10Gc* NewLC();

    /**
     * Destructor.
     */
    virtual ~CHuiVg10Gc();


    /* Methods. */
    
    virtual void Push(THuiGcMatrix aStack);

    virtual void Pop(THuiGcMatrix aStack);

    virtual void Multiply(THuiGcMatrix aStack, TReal32 aMatrix[16]);

    virtual void LoadIdentity(THuiGcMatrix aStack);

    virtual void Translate(THuiGcMatrix aStack,
                           TReal32 aX, TReal32 aY, TReal32 aZ) __SOFTFP;

    virtual void Scale(THuiGcMatrix aStack,
                       TReal32 aX, TReal32 aY, TReal32 aZ) __SOFTFP;

    virtual void Rotate(THuiGcMatrix aStack, TReal32 aAngle,
                         TReal32 aX, TReal32 aY, TReal32 aZ) __SOFTFP;

    virtual void Shear(THuiGcMatrix aStack,
                       TReal32 aX, TReal32 aY, TReal32 aZ) __SOFTFP;
    
    virtual void InitState();

    /**
     * Selects the default data arrays.
     */
    void RestoreState() const;

    /**
     * Restores OpenVg state according to flags set up in the render plugin
     */
    void RestoreFlaggedState() const;
    
    
    /**
     * Updates the projection matrix without changing any settings.
     */
    void UpdateProjection();

    /**
     * Returns the number of available texture units.
     *
     * @return  Number of units (at least 1).
     */
    TInt TextureUnits() const;

    /**
     * Sets the number of active texture units.
     *
     * @param aTextureUnitCount  Number of units to activate (e.g., 2 => 0,1)
     */
    void SetTextureUnits(TInt aTextureUnitCount);

    /**
     * Enable a feature.
     */
    void Enable(TFeature aFeature, TBool aDoEnable = ETrue);

    /**
     * Changes the blending mode.
     */
    void SetBlendMode(TBlendMode aBlendMode);

    /**
     * Changes the texture mode.
     *
     * @param aTextureMode  Texturing mode.
     * @param aParam        Parameter for the texturing.
     */
    void SetTextureMode(TTextureMode aTextureMode, TReal32 aParam = 0) __SOFTFP;

    /**
     * Sets the strength of dimming fog. In order to have effect, enable the
     * Fog feature.
     *
     * @param aColor   Color of dimming.
     * @param aAmount  Amout of dimming (0...1).
     */
    void SetDimmingFog(const TRgb& aColor, TReal32 aAmount) __SOFTFP;

    /**
     * Clear rect using the current pen color
     */
    void Clear(const TRect& aRect);
    
    /**
     * Clear using the current pen color.
     */
    void Clear();

    /**
     * Clear the depth buffer.
     */
    void ClearDepth();

    /**
     * Clear both color and depth buffer
     */
    void ClearColorAndDepth();

	/**
	 * Do initialization of new frame.
	 * Called by CHuiDisplay when starting to render new frame.
	 * Graphics context can do renderer specific state initializations here.
	 */
	void InitNewFrame();

    /**
     * Apply a clipping rectangle to the current clipping region.
     *
     * @param aClipRect  Clipping rectangle.
     */
    virtual void Clip(const TRect& aClipRect);

    /**
     * Apply a clipping region to the current clipping region.
     *
     * @param aRegion  Clipping region.
     */
    virtual void Clip(const TRegion& aRegion);

    /**
     * Replace the current clipping rectangle.
     *
     * @param aClipRect  Clipping rectangle in display coordinates.
     */
    virtual void SetClip(const TRect& aClipRect);
    
    /**
     * Replace the current clipping region.
     *
     * @param aRegion  List of Clipping rectangles.
     */
    virtual void SetClipRegion(const TRegion& aRegion);
    
    /**
     * Cancel an applied clipping rectangle or region.
     */
    virtual void CancelClipping();

    /**
     * Draws a single line of text using the current pen color, font, and
     * alignment mode.
     *
     * @param aTextMesh  Text string.
     * @param aBounds    Bounding box for the text.
     * @param aShadow    Darkness of text shadow.
     */
    void DrawText(const CHuiTextMesh& aTextMesh,
                  const THuiRealRect& aBounds,
                  TReal32 aShadow = 0) __SOFTFP;

    /**
     * Draws a rectangle with the current graphics context parameters.
     *
     * @param aRect  Rectangle to draw.
     */
    void DrawRect(const TRect& aRect);

    /**
     * Draws a solid line of uniform thickness with the current graphics context parameters.
     *
     * @param aStart     Start of line.
     * @param aEnd       End of line.     
     * @param aThickness Thickness of line in pixels.
     */
    void DrawLine(const TPoint& aStart, const TPoint& aEnd, const TInt aThickness);

    /**
     * Draws a solid or unfilled round rectangle.
     *
     * @param aDestinationRect     Start of line.
     * @param aSize       	Arc size. 
	 * @param aDrawMode		THuiFillMode 
     * @param aThickness 	Thickness of border line in pixels.
     */
    void DrawRoundRect(const TRect& aDestinationRect, const THuiRealSize& aSize, THuiFillMode aDrawMode, const TInt aThickness);

    /**
     * Draws a stretched image.
     *
     * @param aMode        Stretch mode: horizontal or vertical.
     * @param aImage       Image.
     * @param aRect        Rectangle to fill.
     * @param aStartWidth
     * @param aEndWidth
     */
    void DrawStretchImage(TStretchMode aMode,
                          const THuiImage& aImage, const THuiRealRect& aRect,
                          TInt aStartWidth, TInt aEndWidth);

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
     * 							 aImage needs to be 
     * 							 defined in order to this modes to be
     * 					         effective.
     * 
     * @param aImage			 The image to be used to draw borders,
     * 							 according to the border drawing mode.
     * 							 If left NULL will use the current pen 
     * 							 color to draw single-color borders.
     */
    void DrawBorders(const TRect& aOuterRect, 
                     TReal32 aLeftBorderWidth,
                     TReal32 aRightBorderWidth, 
                     TReal32 aTopBorderHeight,
                     TReal32 aBottomBorderHeight, 
                     TBorderMode aBorderMode,
                     const THuiImage* aImage) __SOFTFP;

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
    void DrawPath(const CHuiCurvePath& aPath,
                  const TPoint& aOrigin,
                  TReal32 aStartPos,
                  TReal32 aEndPos,
                  MHuiMappingFunction* aAlphaFunction = 0,
                  MHuiMappingFunction* aWidthFunction = 0) __SOFTFP;

    void DrawMesh(const CHuiMesh& aMesh, const THuiImage* aImage = 0,
                  const THuiImage* aSecondaryImage = 0,
                  TReal32 aSecondaryAlpha = 0.0) __SOFTFP;

    void DrawArc(const TPoint& aOrigin, const TSize& aRadius,
                 TReal32 aStart, TReal32 aEnd, TInt aAnglePerSegment,
                 TReal32 aWidth, const THuiImage& aImage,
                 TReal32 (*aAlphaFunc)(TReal32)=0, TBool aAbsoluteAngleParm=EFalse) __SOFTFP;

    void DrawGradient(TGradientType aType, const TRect& aRect,
                      const TRgb& aStartColor, const TRgb& aEndColor,
                      TReal32 aStartOpacity = 1.0,
                      TReal32 aEndOpacity = 1.0,
                      const THuiRealRect* aTexCoords = 0) __SOFTFP;
                      
    void DrawPolygon(RArray<THuiRealPoint>& aPoints);                      

    void DrawEllipse(const TRect& aRect, THuiFillMode aDrawMode, const TInt aThickness);                      
    
    /**
     * Sets a depth offset. The depth offset is used when drawing overlaid
     * surfaces with the same Z coordinates. Normally the depth offset is
     * always zero.
     *
     * @param aOffset  Offset units.
     */
    void SetDepthOffset(TInt aOffset);

    /**
     * Makes a screen capture of the graphics context's current content.
     * The data is returned in an array of raw pixel data of size:
     * aSize.Width() * aSize.Height() * 4.
     *
     * @return  Rgba8888 raw bitmap data of the screen's contents.
     *          Caller must pop and destroy the returned buffer.
     */
    TUint8* CaptureLC(TSize& aSize) const;

    /**
     * Transforms a dirty rectangle. Implementing this method in
     * inherited classes is optional (if e.g. dirty rects aren't supported)
     *
     * @param aRect  Rectangle to transform.
     */
    void TransformDirtyRect(THuiRealRect& aRect);

    /*
     * Implemented.
     */
    virtual void UpdateColor(TReal32 aAlphaFactor = 1.0f) __SOFTFP;
    
public: // new

    /**
     * Sets the rule by which a polygon should be filled
     * Fill rule is useful while filling self-intersecting polygons.
     *
     * @param aFillMode Polygon filling mode, which will be mapped to
     *  openVg Fill_Rule.
     */

    void SetFillRule( THuiFillMode aFillMode );

    /*
     * Ownership is not transferred.
     */
    void SetPaintPattern(CHuiTexture* aTexture, const TPoint& aTextureOrigin);
    
    void DrawArc(const TRect& aRect, THuiFillMode aDrawMode, const TInt aThickness, TReal32 aStart, TReal32 aEnd, TBool aIsPie) __SOFTFP;

protected:

    /* Constructors. */

    /**
     * Constructor.
     */
    CHuiVg10Gc();

    /**
     * Second-phase constructor.
     */
    void ConstructL();


    /* Methods. */

    /**
     * Draw the image.
     *
     * @param aImage    Image to draw.
     * @param aTopLeft  Coordinates of the top left corner.
     * @param aSize     Size of the image.
     */
    void DoDrawImage(const THuiImage& aImage, const THuiRealPoint& aTopLeft,
                     const THuiRealSize& aSize);

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
    void DoDrawImages(const THuiImage& aImage,
                      const THuiImage& aImage2,
                      const THuiRealPoint& aTopLeft,
                      const THuiRealSize& aSize);

    CHuiMatrixStack* MatrixStack(THuiGcMatrix aMatrix);

    void SetQuality(THuiQuality aQuality);
    
    /**
     * From CHuiGc.
     */
    void GcExtension(const TUid& aExtensionUid, TAny** aExtensionParams);
    
private:
    /** 
     *  Synchronize a specific OpenVG matrix with the main matrix.
     */
    void UpdateMatrix(VGMatrixMode aMatrix);
    
    /**
     *  Makes sure the latest transformation matrix is used for client-side calculation.
     */
    void UpdateClientMatrix();

    /**
     *  Draw a cropped and possibly scaled portion of an image onto the screen.
     * 
     *  @param aTexture     Texture to draw
     *  @param aSourceRect  Source rectangle
     *  @param aDestRect    Destination rectangle
     */
    void DrawCroppedTexture(const MHuiSegmentedTexture& aTexture, 
                            const THuiRealRect& aSourceRect, 
                            const THuiRealRect& aDestRect);

    /**
     *  Draw a possibly cropped or scaled portion of an image onto the screen.
     * 
     *  @param aTexture     Texture to draw
     *  @param aSourceRect  Source rectangle
     *  @param aDestRect    Destination rectangle
     */
    void DrawTexture(const MHuiSegmentedTexture& aTexture, 
                     const THuiRealRect& aSourceRect, 
                     const THuiRealRect& aDestRect);


    /**
     * DrawBorders implementation for EBorderImage
     * border drawing mode.
     * @see CHuiGc::DrawBorders()
     */
    void DrawBorderImages(const TRect& aOuterRect, 
                          TReal32 aBorderWidth,
                          TReal32 aBorderHeight, 
                          THuiRealPoint aOffset,
                          const THuiImage* aImage) __SOFTFP;


    /**
     * DrawBorders implementation for EBorderFixedCorners
     * border drawing mode.
     * @see CHuiGc::DrawBorders()
     */
    void DrawBorderFixedCorners(const TRect& aOuterRect, 
                                TReal32 aLeftBorderWidth,
                                TReal32 aRightBorderWidth, 
                                TReal32 aTopBorderHeight,
                                TReal32 aBottomBorderHeight, 
                                const THuiImage* aImage) __SOFTFP;
    
    
    void CreateVgObjectsL();
    
	
#ifdef __NVG
    /**
     * Draws the icon from NVG data using the NVGDecoder. Does the parsing of the icon header as well
     * 
     * @param aNVGData              Extended bitmap data in NVG buffer format to be drawn
     * @param aIconCmds             Extended bitmap data in VG Object Cached format to be drawn
     * @param aImageSize            Size of the image
     * @param aIgnoreAspectRatio    Ignore aspect ratio defined in the icon's header
     */
    void DrawNVG(HBufC8* aNVGData, MNVGIcon* aIconCmds, const TSize& aImageSize, TBool aIgnoreAspectRatio);
#endif
    
    void UsePaintPattern();
    void DiscardPaintPattern();
    
    /**
     * Copy screen to bitmap.
     * @internal
     */
    TInt DoCopyScreenToBitmap(CFbsBitmap* aBitmap);
    
private:
    /** Matrix characteristics */
    enum TMatrixFlags
        {
        EMatrixFlagsRotation = 0x1,
        };

    /** Model view matrix stack */
    CHuiMatrixStack* iMatrixStack;

    typedef RArray<TUint> RMatrixFlagsStack;
    
    /** Stack for storing additional information about the matrices */
    RMatrixFlagsStack iMatrixFlagsStack;
    
    /** Flags for the current matrix */
    TUint iMatrixFlags;
    
    /** Rendering state */
	TBool iStateScissorTest;
	TRect iScissorRect;
	
    /** A path for drawing dynamic shapes */
	VGPath iPath;
    
	/** A path for drawing rectangles */
	VGPath iRectPath;
	
	/** A path for drawing round rectangles */
	VGPath iRoundRectPath;

    /** A path for drawing cropped images */
    VGPath iImagePath;
    
    /** A path for drawing borders */
    VGPath iBorderPath;
    
    /** A path for drawing lines */
    VGPath iLinePath;

    /** A path for drawing arcs */
    VGPath iArcPath;

    /** A path for drawing ellipses */
    VGPath iEllipsePath;
    
    /** Default fill and stroke paint */
	VGPaint iPaint;
	TUint32 iPaintColor;
	
    /** Gradient fill paint */
    VGPaint iGradientPaint;
    
    /** The current blend mode */
    VGBlendMode iBlendMode;
    
#ifdef __NVG
    CNvgEngine* iNvgEngine;
#endif
        
    TBool iVgObjectsCreated;
    
    /** Private temp region for general purpose, used to avoid excessive allocations of RRegion heap objects */
    mutable RRegionBuf<KHuiGcClipRegionGranularity> iTempRegion;

    /** Image pattern for paint, NOT owned */
    VGImage iPaintPattern;
    TPoint iPaintPatternOrigin;
    };

#endif  // __HUIVG10GC_H__
