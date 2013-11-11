/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition of CHuiCanvasWsSwGc.
*
*/



#ifndef __HUICANVASWSSWGC_H__
#define __HUICANVASWSSWGC_H__

#include <uiacceltk/HuiVisual.h>
#include "huicanvaswsgc.h"
#include "huicanvaspainter.h"
#include <bitstd.h>

const TInt KHuiCanvasWsSwGcRegionGranularity = 10;

/**
 * This class is used internally by the toolkit and thus should
 * not be used by the applications directly.
 */
NONSHARABLE_CLASS(CHuiCanvasWsSwGc) : public CHuiCanvasWsGc
{

protected:
    /**
     * Constructor.
     */
    CHuiCanvasWsSwGc();

    /**
     * Second-phase constructor.
     */
    void ConstructL();

public:
		/**
     * Constructor.
     */
    static CHuiCanvasWsSwGc* NewL();
    
    /**
     * Destructor.
     */
    ~CHuiCanvasWsSwGc();

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
	virtual void WsUpdateJustificationVerticalL( TInt aValue, TPtr&  aTextValue) ;
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
    
protected:
    
    void DoBeginDrawL(TBool aIsCachePrepared);
    void DoEndDrawL();
    TPoint AdjustCoordinates(TPoint aPoint);
    TRect AdjustCoordinates(TRect aRect);	
	TDisplayMode SelectDisplayMode();	
	TRect RoundedDisplayRect() const;
    CFbsBitmap* CreateMovedMaskL(CFbsBitmap& aOriginalMask, TPoint aNewTopLeftCorner);
    TBool IsRenderingEnabled() const;
	

    /**
     * Drawing related data
     */
    TRect iWsClipRect;
    TPoint iWsOrigin; 
  
    /**
     * The difference between the original position (when first drawn) of the 
     * window and the current position after visual position has been changed
     */
    TPoint iPosDelta;
    
    /**
     * Currently ongiong action, scan or draw.
     */ 
    TInt iAction;

    /**
     * Flag to tell wheter something really has been drawn into this gc
     */
    TBool iIsReallyDirty;
  
    /**
     * Backbuffer
     */
    CFbsBitGc* iBitGc;
    CFbsBitmap* iBitBitmap;
    CFbsBitmapDevice* iBitDevice;
  
    /**
     * Buffer for capturing window drawing into bitmap
     */
    CFbsBitGc* iCapturingBitGc;
    CFbsBitmap* iCapturingBitBitmap;
    CFbsBitmapDevice* iCapturingBitDevice;
  
    /**
     * Canvas related
     */
    CHuiCanvasGc* iCanvasGc;
    const CHuiCanvasVisual* iVisual;

    /**
     * RRegionBuf is used instead of RRegion for better performance.
     */
    RRegionBuf<KHuiCanvasWsSwGcRegionGranularity> iWsClipRegion; 
    RRegionBuf<KHuiCanvasWsSwGcRegionGranularity> iTempRegion;  
    RRegionBuf<KHuiCanvasWsSwGcRegionGranularity> iDefaultClipRegion;

    /**
     * ETrue if cache is prepared, EFalse otherwise.
     */    
    TBool iBitmapPrepared; 
    
    /**
     * ETrue if cache is initialized, EFalse otherwise.
     */
    TBool iBitmapInitialized;

};    
#endif  // __HUICANVASWSSWGC_H__
