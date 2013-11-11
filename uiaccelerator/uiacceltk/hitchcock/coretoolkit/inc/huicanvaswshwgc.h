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
* Description:   Definition of CHuiCanvasWsHwGc.
*
*/



#ifndef __HUICANVASWSHWGC_H__
#define __HUICANVASWSHWGC_H__

#include <uiacceltk/HuiVisual.h>
#include "uiacceltk/HuiTextMesh.h"
#include "huicanvaswsgc.h"
#include "huicanvaspainter.h"
#include "huicanvasrenderbuffer.h"

const TInt KHuiCanvasWsHwGcRegionGranularity = 10;

/**
 * This class is used internally by the toolkit and thus should
 * not be used by the applications directly.
 */
NONSHARABLE_CLASS(CHuiCanvasWsHwGc) : public CHuiCanvasWsGc
{

public:

	/**
     * Constructor.
     */
    static CHuiCanvasWsHwGc* NewL();
    
    /**
     * Destructor.
     */
    ~CHuiCanvasWsHwGc();

public:

	// CFbsBitGc functionality 
	virtual void WsBitBltL(TInt aBitmapHandle, TPoint aPoint);
	virtual void WsBitBltRectL(TInt aBitmapHandle, TPoint aPoint, TRect aRect) ;
	virtual void WsBitBltMaskedL(TInt aBitmapHandle, TInt aMaskHandle, TInt aInvertMask, TPoint aPoint, TRect aRect) ;
	virtual void WsBitBltMaskedPointL(TInt aBitmapHandle, TInt aMaskHandle, TPoint aPoint1, TPoint aPoint2, TRect aRect) ;
	virtual void WsResetClippingRegionL() ;
	virtual void WsClearL();
	virtual void WsClearRectL(TRect& aRect) ;
	virtual void WsResetBrushPatternL() ;
	virtual void WsResetFontL() ;
	virtual void WsDrawArcL(TPoint aPoint1, TPoint aPoint2, TRect aRect) ;
	virtual void WsDrawPieL(TPoint aPoint1, TPoint aPoint2, TRect aRect) ;
	virtual void WsDrawBitmap1L(TInt aBitmapHandle, TRect aRect) ;
	virtual void WsDrawBitmap2L(TInt aBitmapHandle, TRect aRect1, TRect aRect2) ;
	virtual void WsDrawBitmap3L(TInt aBitmapHandle, TPoint aPoint) ; 
	virtual void WsDrawBitmapMaskedL(TInt aBitmapHandle, TInt aMaskHandle, TInt aInvertMask, TRect& aRect1, TRect& aRect2) ;
	virtual void WsDrawRoundRectL(TPoint aPoint, TRect& aRect) ;
	virtual void WsDrawPolyLineL(CArrayFix<TPoint>* aPointerArray) ;
	virtual void WsDrawPolyLineNoEndPointL(TPoint* aPointerArray, TInt aCount);
	virtual void WsDrawPolygonL(CArrayFix<TPoint>* aPointerArray, TInt aFillRule ) ;
	virtual void WsDrawEllipseL(TRect& aRect) ;
	virtual void WsDrawLineL(TPoint& aStart, TPoint& aEnd) ;
	virtual void WsDrawLineToL(TPoint& aPoint);
	virtual void WsDrawLineByL(TPoint& aPoint) ;
	virtual void WsDrawRectL(TRect& aRect) ;
	virtual void WsDrawText1L(TPtr& aTextValue, THuiCanvasTextParameters& aTextParameters) ;
	virtual void WsDrawText2L(TPtr& aTextValue, TPoint& aPoint,THuiCanvasTextParameters& aTextParameters) ;
	virtual void WsDrawText3L(TPtr& aTextValue, TRect& aRect, THuiCanvasTextParameters& aTextParameters) ;
	virtual void WsDrawText4L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextAlign, TInt aTextMargin,THuiCanvasTextParameters& aTextParameters) ;
	virtual void WsDrawText5L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextAlign, TInt aTextMargin,TInt aTextWidth, THuiCanvasTextParameters& aTextParameters) ; 
	virtual void WsDrawTextVertical1L(TPtr& aTextValue, TInt aTextUp, THuiCanvasTextParameters& aTextParameters)	;
	virtual void WsDrawTextVertical2L(TPtr& aTextValue, TInt aTextUp,TPoint& aPoint, THuiCanvasTextParameters& aTextParameters) ;
	virtual void WsDrawTextVertical3L(TPtr& aTextValue, TInt aTextUp, TRect& aRect, THuiCanvasTextParameters& aTextParameters) ;
	virtual void WsDrawTextVertical4L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextUp, TInt aTextAlign, TInt aTextMargin, THuiCanvasTextParameters& aTextParameters) ;
	virtual void WsDrawTextVertical5L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextUp, TInt aTextAlign, TInt aTextMargin, TInt aTextWidth, THuiCanvasTextParameters& aTextParameters) ;
	virtual void WsMoveToL( TPoint& aPoint) ;
	virtual void WsMoveByL( TPoint& aPoint) ;
	virtual void WsPlotL(TPoint& aPoint) ;
	virtual void WsResetL() ;
	virtual void WsSetBrushColorL( TRgb aColor) ;
	virtual void WsSetBrushOriginL( TPoint& aPoint) ;
	virtual void WsSetBrushStyleL( TInt aStyle) ;
	virtual void WsSetClippingRegionL( RRegion& aRegion) ;
	virtual void WsSetDrawModeL( TInt aDrawModew) ;
	virtual void WsSetOriginL( TPoint& aOrigin);
	virtual void WsSetPenColorL( TRgb aColor) ;
	virtual void WsSetPenStyleL( TInt aStyle) ;
	virtual void WsSetPenSizeL( TInt aWidth, TInt aHeight) ;
	virtual void WsSetTextShadowColorL( TRgb aColor) ;
	virtual void WsSetCharJustificationL( TInt aExcessiveWidth, TInt aNumGap) ;
	virtual void WsSetWordJustificationL( TInt aExcessiveWidth, TInt aNumGap) ;
	virtual void WsSetUnderlineStyleL( TInt aValue) ;
	virtual void WsSetBrushPatternL( TInt aValue) ;
	virtual void WsSetFontL( TInt aFontId) ;
	virtual void WsCopyRectL( TPoint& aPoint, TRect& aRect) ;
	virtual void WsCopySettingsL() ;
	virtual void WsUpdateJustificationL( TPtr& aTextValue) ;
	virtual void WsUpdateJustificationVerticalL( TInt aValue, TPtr& aTextValue) ;
	virtual void WsSetFontNoDuplicateL( TInt aCurrentFont) ;		
	virtual void WsSetClippingRectL( TRect& aRect) ; 
	virtual void WsCancelClippingRectL() ; 
	virtual void WsSetFadedL( TInt& aFaded) ; 
	virtual void WsSetFadingParametersL( TInt aFaded1, TInt aFaded2) ; 
	virtual void WsFadeAreaL( RRegion& aRegion) ; 
	virtual void WsMapColorsL() ; 
	virtual void WsSetUserDisplayModeL( TInt aMode) ; 
	virtual void WsUseFontL(TInt aCurrentFont) ; 

	// Extended CFbsBitGc functionality     
    virtual void WsCombinedBitBltMaskedL(TRect aDestinationRect, const RArray<THuiCachedCombinedImageParams>& aBlits);	 

    // Canvas functionality
    
	virtual void BeginActionL(TInt aAction, TRect aDisplayRect,const CHuiCanvasVisual& aUser, TBool aIsCachePrepared, const TRegion& aUpdateRegion);
    virtual void EnableUpdateRegion(const TRegion& aUpdateRegion, TBool aClear);
    virtual void DisableUpdateRegion();
    virtual void EndActionL(const TRegion& aUpdateRegion, TBool aUpdateDisplay);
    virtual TInt SetCapturingBufferL(CFbsBitmap* aTarget);
    virtual CHuiCanvasGc& CanvasGc() const;  	
    virtual void SetPositionDelta( TPoint& aPoint );
    virtual void ClearCache();

    virtual void SetRelativeOrientation(CHuiGc::TOrientation aOrientation);
    
    virtual TBool IsRenderBufferEnabled() const;
    virtual TBool IsRenderBufferPrepared() const;
    virtual TBool IsRenderBufferInitialized() const;
    
    virtual TInt EnableRenderbuffer(TBool aEnable = ETrue);    
    virtual THuiCanvasWsGcType Type() const;

    virtual void ClearCapturingBufferArea(const TRect& aRect);

    void BeginDrawL(TRect aDisplayRect,const CHuiCanvasVisual& aUser, TBool aIsCachePrepared, const TRegion& aUpdateRegion);
    void EndDrawL(const TRegion& aUpdateRegion, TBool aUpdateDisplay);
    void BeginScanL(TRect aDisplayRect,const CHuiCanvasVisual& aUser, TBool aIsCachePrepared, const TRegion& aUpdateRegion);
    void EndScanL(const TRegion& aUpdateRegion, TBool aUpdateDisplay);

public:

	void DoWsClearRectL(const THuiRealRect& aRect );  

private:

    /**
     * Constructor.
     */
    CHuiCanvasWsHwGc();

    /**
     * Second-phase constructor.
     */
    void ConstructL();

	    
    THuiCachedGcParams CachedGcParams();
	
	void ExpandDirtyRegion(TRect aNewRect);
	
    void AdjustCoordinates(TRect& aRect, TBool aUseWsOrigin = ETrue );
    
    void AdjustCoordinates(TPoint& aPoint , TBool aUseWsOrigin = ETrue );

    void DrawTextBrush( THuiRealRect& aRect );

    TRect RoundedDisplayRect();
    
    TBool IsRenderingEnabled() const;
    
    TBool IsRenderBufferUpdateOngoing() const;
    
    THuiCanvasDrawMode SelectCanvasDrawMode(TInt aWsDrawMode);
    
    void BindRenderBuffer(CHuiCanvasRenderBuffer* aRenderbuffer, const TRegion& aUpdateRegion);

    void UnBindRenderBuffer(CHuiCanvasRenderBuffer* aRenderbuffer);
    
    void UseBrushPattern();
    
    void DiscardBrushPattern();
    
    TBool IsDifferent(const TRgb& aFirst, const TRgb& aSecond, TBool aIgnoreAlpha);

    /**
	 * UseCanvasState
	 *
	 *	Saves canvas state for color, opacity, drawmode, pen width and polygon drawmode            
	 */
    void UseCanvasState();

    /**
	 * UseWsState
	 *
	 *	Restores canvas state (stored by the call of  UseCanvasState) for color,
	 *  opacity, drawmode, pen width and polygon drawmode            
	 */
    void UseWsState();
    
private:
  
 /**
  * CFbsBitGc attributes
  */
  TInt iWsCurrentFont;
  TPoint iWsPenPosition;
  TPoint iWsOrigin;
  TRgb iWsFadeColor;
  TRgb iWsPenColor;
  TRgb iWsBrushColor;
  TInt iWsPenStyle;
  TInt iWsDrawMode;
  TInt iWsBrushStyle;
  TInt iWsShadowMode;
  TInt iWsStrikethrough;
  TInt iWsUnderline;
  TInt iWsUserDisplayMode;
  TRgb iWsShadowColor;
  TSize iWsPenSize;
  TInt iWsBrushPattern;
  TPoint iWsBrushOrigin;
  
 /**
  * RRegionBuf is used instead of RRegion for better performance.
  */
  RRegionBuf<KHuiCanvasWsHwGcRegionGranularity> iWsClipRegion; 
  RRegionBuf<KHuiCanvasWsHwGcRegionGranularity> iTempRegion;  

 /**
  * Canvas internal misc attributes
  */
  TInt iAction;
  CHuiCanvasGc* iCanvasGc;
  CHuiCanvasVisual* iVisual;
  TBool iAllCommandsSupported;
  
 /**
  * The difference between the original position (when first drawn) of the 
  * window and the current position after visual position has been changed
  */
  TPoint iPosDelta;
  
  /**
   * Render buffer is (or is not) ready to be drawn into display 
   */
  TBool iRenderBufferPrepared; 

  /**
   * Render buffer is (or is not) used when drawing 
   */
  TBool iRenderBufferEnabled;
    
  /**
   * Render buffer is (or is not) available when drawing. It may be unavailable for
   * various reasons, e.g. when memory is low. 
   */
  TBool iRenderBufferAvailable;
  
  /**
   * Render buffer is (or is not) initalized. It has just been created, resized or recycled. 
   */
  TBool iRenderBufferInitialized;
  
  /**
   * Relative orientation to display orientation. E.g. after layout switch display is in
   * EOrientationCCW90 but stored old drawing commands have orientation EOrientationNormal. In that
   * case setting iRelativeOrientation to EOrientationCW90 causes drawing commands to appear
   * at their original orientation in the physical screen.
   * 
   */  
  CHuiGc::TOrientation iRelativeOrientation;
  
  TRgb iOldColor;            
  TReal32 iOldOpacity;
  THuiCanvasDrawMode iOldDrawMode;
  TInt iOldPenWidth;
  THuiFillMode iOldPolygonDrawMode;
#ifdef __WINS__
  TInt iPushCount;
#endif
};    
#endif  // __HUICANVASWSHWGC_H__
