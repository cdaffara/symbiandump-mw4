/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition of CHuiCanvasDebugWsGc.
*
*/



#ifndef __HUICANVASDEBUGWSGC_H__
#define __HUICANVASDEBUGWSGC_H__

#include <e32base.h>
#include "uiacceltk/HuiCanvasCmdBufferReader.h"

#include <uiacceltk/HuiOwnedPointer.h>
#include <uiacceltk/HuiVisual.h>    
#include <uiacceltk/HuiDisplay.h>    
#include "huicanvaswshwgc.h"

class CAlfCommandDebug;

/**
 * This class is used internally by the toolkit and thus should
 * not be used by the applications directly.
 */

class CHuiCanvasDebugWsGc : public CHuiCanvasWsGc
    {

public:

	virtual void WsBitBltL(TInt aBitmapHandle, TPoint aPoint);
	virtual void WsBitBltRectL(TInt aBitmapHandle, TPoint aPoint, TRect aRect) ;
	virtual void WsBitBltMaskedL(TInt aBitmapHandle, TInt aMaskHandle, TInt aInvertMask, TPoint aPoint, TRect aRect) ;
	virtual void WsBitBltMaskedPointL(TInt aBitmapHandle, TInt aMaskHandle, TPoint aPoint1, TPoint aPoint2, TRect aRect) ;
	
	virtual void WsResetClippingRegionL() ;
	virtual void WsClearL();
	virtual void WsClearRectL(TRect& aRect);
	virtual void WsResetBrushPatternL() ;
	virtual void WsResetFontL() ;
	virtual void WsDrawArcL(TPoint aPoint1, TPoint aPoint2, TRect aRect) ;
	virtual void WsDrawPieL(TPoint aPoint1, TPoint aPoint2, TRect aRect) ;
	virtual void WsDrawBitmap1L(TInt aBitmapHandle, TRect aRect) ;
	virtual void WsDrawBitmap2L(TInt aBitmapHandle, TRect aRect1, TRect aRect2) ;
	virtual void WsDrawBitmap3L(TInt aBitmapHandle, TPoint aPoint) ; //XXX_CFbsBitGc_Hack Remove!!!
	virtual void WsDrawBitmapMaskedL(TInt aBitmapHandle, TInt aMaskHandle, TInt aInvertMask, TRect& aRect1, TRect& aRect2) ;
	virtual void WsDrawRoundRectL(TPoint aPoint, TRect& aRect) ;
	virtual void WsDrawPolyLineL(CArrayFix<TPoint>* aPointerArray) ;
	virtual void WsDrawPolyLineNoEndPointL(TPoint* aPointerArray, TInt aCount);
	virtual void WsDrawPolygonL(CArrayFix<TPoint>* aPointerArray, TInt aFillRule ) ;
	virtual void WsDrawEllipseL(TRect& aRect) ;
	virtual void WsDrawLineL(TPoint& aStart, TPoint& aEnd) ;
	virtual void WsDrawLineToL(TPoint& aPoint) ;
	virtual void WsDrawLineByL(TPoint& aPoint) ;
	virtual void WsDrawRectL(TRect& aRect) ;
	virtual void WsDrawText1L(TPtr& aTextValue, THuiCanvasTextParameters& aTextParameters) ;
	virtual void WsDrawText2L(TPtr& aTextValue, TPoint& aPoint,THuiCanvasTextParameters& aTextParameters) ;
	virtual void WsDrawText3L(TPtr& aTextValue, TRect& aRect, THuiCanvasTextParameters& aTextParameters) ;
	virtual void WsDrawText4L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextAlign , TInt aTextMargin ,THuiCanvasTextParameters& aTextParameters) ;
	virtual void WsDrawText5L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextAlign , TInt aTextMargin ,TInt aTextWidth, THuiCanvasTextParameters& aTextParameters) ; //XXX_CFbsBitGc_Hack Remove!!!
	virtual void WsDrawTextVertical1L(TPtr& aTextValue, TInt aTextUp, THuiCanvasTextParameters& aTextParameters)	;
	virtual void WsDrawTextVertical2L(TPtr& aTextValue, TInt aTextUp,TPoint& aPoint, THuiCanvasTextParameters& aTextParameters) ;
	virtual void WsDrawTextVertical3L(TPtr& aTextValue, TInt aTextUp, TRect& aRect, THuiCanvasTextParameters& aTextParameters) ;
	virtual void WsDrawTextVertical4L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextUp, TInt aTextAlign, TInt aTextMargin, THuiCanvasTextParameters& aTextParameters) ;
	virtual void WsDrawTextVertical5L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextUp, TInt aTextAlign, TInt aTextMargin, TInt aTextWidth, THuiCanvasTextParameters& aTextParameters) ;
	virtual void WsMoveToL(TPoint& aPoint) ;
	virtual void WsMoveByL(TPoint& aPoint) ;
	virtual void WsPlotL(TPoint& aPoint) ;
	virtual void WsResetL() ;
	virtual void WsSetBrushColorL(TRgb aColor) ;
	virtual void WsSetBrushOriginL(TPoint& aPoint) ;
	virtual void WsSetBrushStyleL(TInt aStyle) ;
	virtual void WsSetClippingRegionL(RRegion& aRegion) ;
	virtual void WsSetDrawModeL(TInt aDrawModew) ;
	virtual void WsSetOriginL(TPoint& aOrigin);
	virtual void WsSetPenColorL(TRgb aColor) ;
	virtual void WsSetPenStyleL(TInt aStyle) ;
	virtual void WsSetPenSizeL(TInt aWidth, TInt aHeight) ;
	virtual void WsSetTextShadowColorL(TRgb aColor) ;
	virtual void WsSetCharJustificationL(TInt aExcessiveWidth, TInt aNumGap) ;
	virtual void WsSetWordJustificationL(TInt aExcessiveWidth, TInt aNumGap) ;
	virtual void WsSetUnderlineStyleL(TInt aValue) ;
	virtual void WsSetBrushPatternL(TInt aValue) ;
//	virtual void SetBrushPattern(TInt aFbsBitmapHandle) ;
	virtual void WsSetFontL(TInt aFontId) ;

	// CFbsBitGc functionality used by Wserv for its internal operations
	virtual void WsCopyRectL(TPoint& aPoint, TRect& aRect) ;
	virtual void WsCopySettingsL() ;
	virtual void WsUpdateJustificationL(TPtr&  aTextValue) ;
	virtual void WsUpdateJustificationVerticalL(TInt aValue, TPtr& aTextValue) ;
	virtual void WsSetFontNoDuplicateL(TInt aCurrentFont) ;
		
	virtual void WsSetClippingRectL(TRect& aRect) ; 
	virtual void WsCancelClippingRectL() ; 
	virtual void WsSetFadedL(TInt& aFaded) ; 
	virtual void WsSetFadingParametersL(TInt aFaded1, TInt aFaded2) ; 
	virtual void WsFadeAreaL(RRegion& aRegion) ; 
	virtual void WsMapColorsL() ; 
	virtual void WsSetUserDisplayModeL(TInt aMode) ; 
	virtual void WsUseFontL(TInt aCurrentFont) ;
	
    virtual void WsCombinedBitBltMaskedL(TRect aDestinationRect, const RArray<THuiCachedCombinedImageParams>& aBlits) ;	 

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
    
  ~CHuiCanvasDebugWsGc();
  
  static CHuiCanvasDebugWsGc* NewL( CHuiCanvasWsGc* aHuiCanvasGc, CAlfCommandDebug& aCommandDebugger ); 
  
  void SetInternalGc(CHuiCanvasWsGc* aHuiCanvasGc);
  
private:
    
    CHuiCanvasDebugWsGc( CAlfCommandDebug& aCommandDebugger );
    
  	void ConstructL( CHuiCanvasWsGc* aHuiCanvasGc ); 
  
  	private:
  	    
  	CHuiCanvasWsGc* iCanvasWsGc; // not owned
  	CAlfCommandDebug& iCommandDebugger; // not owned
    };
    
#endif  // __HUICANVASDEBUGWSGC_H__
