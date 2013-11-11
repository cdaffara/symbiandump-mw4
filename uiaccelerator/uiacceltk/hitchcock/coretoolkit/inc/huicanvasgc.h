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
* Description:   Definition of CHuiCanvasGc
*
*/



#ifndef __HUICANVASGC_H__
#define __HUICANVASGC_H__


#include <e32base.h>
#include <uiacceltk/HuiVisual.h>
#include <uiacceltk/HuiGc.h>
#include <uiacceltk/HuiDisplay.h>
#include <uiacceltk/HuiTexture.h>

const TInt KHuiCanvasGcRegionGranularity = 10;

struct THuiRealLine
    {
    THuiRealLine(){}
    THuiRealLine(const THuiRealPoint& aStart, const THuiRealPoint& aEnd):iStart(aStart),iEnd(aEnd){}

    THuiRealPoint iStart;
    THuiRealPoint iEnd;    

	private:
    TInt iSpare1;
    TInt iSpare2;
    };

enum THuiCanvasClipMode
    {
    /** Clipping is disabled */
    EHuiCanvasClipModeNone,
    
    /** Normal clipping, clipping is set immediately when clip methods are called */
    EHuiCanvasClipModeNormal,
    
    /** Delayed clipping, clipping is actually set just before every CHuiCanvasGc draw operation and only if really needed */
    EHuiCanvasClipModeDelayed            
    };

enum THuiCanvasDrawMode
    {
    /** Drawing is done by blending */
    EHuiCanvasDrawModeBlend,

    /** Drawing is done by writing alpha values directly without blending */
    EHuiCanvasDrawModeNormal            
    };

/* Forward declarations. */    
class CHuiCanvasRenderBuffer;
class CHuiCanvasVisual;
class MHuiTexture;
class CHuiTexture;

class CHuiCanvasGc : public CBase
    {
public:
    enum TClipRectVisibility
        {
        EFullyOutside,
        EFullyInside,
        EPartialOverlap,
        EUnknown
        };

	/**
	 * Constructor.
	 */
	IMPORT_C CHuiCanvasGc();

	/**
	 * Destructor.
	 */ 
	IMPORT_C ~CHuiCanvasGc();

    /** General */
    IMPORT_C virtual void SetGc( CHuiGc& aGc );
    IMPORT_C virtual void SetVisual(const CHuiCanvasVisual& aVisual);

    /** Clipping ops */
    IMPORT_C void SetClippingMode(THuiCanvasClipMode aClipMode);
	IMPORT_C void Clip(const TRect& aClipRect);
	IMPORT_C void ClipRegion(const TRegion& aClipRegion);
	IMPORT_C void CancelClipping();

    /** Drawing ops. Although these are virtual, render specific code is typically at DoDrawXXX method overrides */
    IMPORT_C virtual void DrawImage(const CHuiTexture& aImage, const THuiRealRect& aDestinationRect);
    IMPORT_C virtual void DrawImage(const CHuiTexture& aImage, const THuiRealRect& aDestinationRect, const THuiRealRect& aSourceRect);
    IMPORT_C virtual void DrawImage(const CHuiTexture& aImage, const THuiRealRect& aDestinationRect, const THuiRealRect& aSourceRect, CHuiGc::TStretchMode aStretchMode);
    IMPORT_C virtual void DrawImage(const CHuiCanvasRenderBuffer& aImage, const THuiRealPoint& aDestinationPoint);
    IMPORT_C virtual void DrawText(const CHuiTextMesh& aTextMesh, const THuiRealRect& aDestinationRect);    
    IMPORT_C virtual void DrawLines(RArray<THuiRealLine>& aLines);    
    IMPORT_C virtual void DrawEllipse(const THuiRealRect& aDestinationRect);
	IMPORT_C virtual void DrawPoints(RArray<THuiRealPoint>& aPoints);
	IMPORT_C virtual void DrawPolygon(RArray<THuiRealPoint>& aPoints);
	IMPORT_C virtual void DrawRects(RArray<THuiRealRect>& aRects);
	IMPORT_C virtual void DrawPie(const THuiRealRect& aDestinationRect, const THuiRealPoint& aStart, const THuiRealPoint& aEnd);
	IMPORT_C virtual void DrawArc(const THuiRealRect& aDestinationRect, const THuiRealPoint& aStart, const THuiRealPoint& aEnd);
	IMPORT_C virtual void DrawRoundRect(const THuiRealRect& aDestinationRect, const THuiRealSize& aSize);

    /** Setters */
    IMPORT_C virtual void SetTextStyle(TInt aStyleId);
    IMPORT_C virtual void SetPenColor(const TRgb& aColor);
  	IMPORT_C virtual void SetPenWidth(const TReal32& aWidth);
  	IMPORT_C virtual void SetOpacity(const TReal32& aOpacity);
  	IMPORT_C virtual void SetPolygonDrawMode(THuiFillMode aPolygonDrawMode);
    IMPORT_C virtual void SetPolygonFillTexture(CHuiTexture* aTexture);
    IMPORT_C virtual void SetPolygonFillTextureOrigin(const TPoint& aOrigin);
	IMPORT_C virtual void SetTextAlign(THuiAlignHorizontal aAlignHorizontal, THuiAlignVertical aAlignVertical);
	IMPORT_C virtual void SetDefaults();
    IMPORT_C virtual void SetDrawMode(THuiCanvasDrawMode aDrawMode);
	    
    /** Transformations */
    IMPORT_C virtual void LoadIdentity();
	IMPORT_C virtual void Translate(const TReal32& aX, const TReal32& aY, const TReal32& aZ);
	IMPORT_C virtual void Scale(const TReal32& aX, const TReal32& aY, const TReal32& aZ);
	IMPORT_C virtual void Rotate(const TReal32& aAngle, const TReal32& aX, const TReal32& aY, const TReal32& aZ);
    IMPORT_C virtual void PushTransformationMatrix();
    IMPORT_C virtual void PopTransformationMatrix();

    /** Getters */
    IMPORT_C virtual TInt TextStyle() const;
    IMPORT_C virtual TInt TextVerticalAlign() const;
    IMPORT_C virtual TInt TextHorizontalAlign() const;
    IMPORT_C virtual TReal32 PenWidth() const;
  	IMPORT_C virtual THuiFillMode PolygonDrawMode() const;
    IMPORT_C virtual CHuiTexture* PolygonFillTexture() const;
    IMPORT_C virtual TPoint PolygonFillTextureOrigin() const;
  	
  	/** Background drawing */
  	IMPORT_C virtual void Clear(const THuiRealRect& aRect);
  	IMPORT_C virtual void ClearWithSkinBackground(const THuiRealRect& aRect);
  	IMPORT_C virtual void ClearWithBackgroundItems(const THuiRealRect& aRect, const RArray<THuiDisplayBackgroundItem>& aItems);

  	/** Render buffer handling */
    IMPORT_C virtual CHuiCanvasRenderBuffer* CreateRenderBufferL(const TSize& aSize);
    IMPORT_C virtual void ClearRenderBuffer( CHuiCanvasRenderBuffer& aImage, const TRect & aRect );

    /** Misc */
    IMPORT_C void EnableBlendingIfNeeded(const CHuiTexture* aTexture = NULL);
    
    /** Misc getters */
    CHuiCanvasVisual* Visual() const;
    TRgb PenColor() const;
    TReal32 Opacity() const;
    CHuiGc* Gc() const;
    THuiCanvasDrawMode DrawMode() const;

    /** Clipping helpers */
    TClipRectVisibility IsClipped(const TRegion& aTestedRegion, const TRegion& aClippingRegion) const;

    /** Restores gc state (partially) */
    void RestoreState() const;
    
    /** Restores gc state (uses gc state dirty flags (if renderer supports) to avoid unnecessary restore ops */
    IMPORT_C virtual void RestoreFlaggedState() const;
    
    /** 
     * Enables (or disables) effective opacity, which means that visuals effective opacity is 
     * used to calculate real pen opacity. By default effective opacity is enabled.
     * */
    void EnableEffectiveOpacity(TBool aEnable);

protected:

    /** Misc clipping related methods */
    IMPORT_C virtual TInt MaxNumberOfClipRects() const;
    IMPORT_C TClipRectVisibility EnableDelayedClippingIfNeeded(const THuiRealRect& aUpdatedRect);
    IMPORT_C TClipRectVisibility EnableDelayedClippingIfNeeded(const RArray<THuiRealPoint>& aUpdatedPoints);
    IMPORT_C TClipRectVisibility EnableDelayedClippingIfNeeded(const RArray<THuiRealLine>& aUpdatedLines);
    IMPORT_C TClipRectVisibility EnableDelayedClippingIfNeeded(const RArray<THuiRealRect>& aUpdatedRects);    
    IMPORT_C void DisableDelayedClippingIfNeeded();
    IMPORT_C TBool ClipNext();

    /** Drawing ops steup/cleanup helpers */
    IMPORT_C void Setup();
    IMPORT_C void Cleanup();

    /** Clipping helpers */
    TClipRectVisibility IsClipped(const TRect& aRect) const;
    TClipRectVisibility IsClipped(const TPoint& aPoint) const;

    TClipRectVisibility IsClipped(const TRect& aRect, const TRegion& aClippingRegion) const;
    TClipRectVisibility IsClipped(const TPoint& aPoint, const TRegion& aClippingRegion) const;
    
    /** Drawing ops implementations, render specific code typically overrides these */
    IMPORT_C virtual void DoDrawRects(RArray<THuiRealRect>& aRects);
    IMPORT_C virtual void DoDrawLines(RArray<THuiRealLine>& aLines);
    IMPORT_C virtual void DoDrawArc(const THuiRealRect& aDestinationRect, const THuiRealPoint& aStart, const THuiRealPoint& aEnd);
    IMPORT_C virtual void DoDrawRoundRect(const THuiRealRect& aDestinationRect, const THuiRealSize& aSize);
    IMPORT_C virtual void DoDrawPolygon(RArray<THuiRealPoint>& aPoints);        
	IMPORT_C virtual void DoDrawPoints(RArray<THuiRealPoint>& aPoints);
    IMPORT_C virtual void DoDrawPie(const THuiRealRect& aDestinationRect, const THuiRealPoint& aStart, const THuiRealPoint& aEnd);    
    IMPORT_C virtual void DoDrawRenderBuffer(const CHuiCanvasRenderBuffer& aImage, const THuiRealPoint& aDestinationPoint);
    IMPORT_C virtual void DoClearRenderBuffer( CHuiCanvasRenderBuffer& aImage, const TRect & aRect );
    
protected:
 
    /** General attributes */    
    CHuiGc* iGc;
    CHuiCanvasVisual* iVisual;

    /** Drawing attributes */    
    TRgb    iPenColor;
    TReal32 iPenWidth;
    TReal32 iOpacity;
    THuiFillMode    iPolygonDrawMode;
    THuiAlignVertical iVerticalTextAlign;
    THuiAlignHorizontal iHorizontalTextAlign;
    TInt iTextStyleId;
    THuiCanvasDrawMode iDrawMode;
    
    /** Clipping attributes */    
    RRegionBuf<KHuiCanvasGcRegionGranularity> iClippingRegion;
    THuiCanvasClipMode iClipMode;

    /** Delayed clipping attributes */    
    RRegionBuf<KHuiCanvasGcRegionGranularity> iDelayedClipRegion;
    TInt  iDelayedClipCount;
    TBool iDelayedClipRectPushed;
    TClipRectVisibility iDelayedClipVisibility;
    
    /** Setup/cleanup attributes */    
    TRgb iOldPencolor;
    TInt iOldPenAlpha;    

    /** Temp region */    
    mutable RRegionBuf<KHuiCanvasGcRegionGranularity> iTempRegion;
    mutable RRegionBuf<KHuiCanvasGcRegionGranularity> iTempRegion2;
    
    /** Flag to decide wheter pen alpha is to be multiplied with visual effective opacity */    
    TBool iEffectiveOpacityEnabled;
    
    /** Polygon fill texture, NOT owned */
    CHuiTexture* iPolygonFillTexture;
    TPoint iPolygonFillTextureOrigin;
    };

#endif        
    
