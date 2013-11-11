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
* Description:   Definition of MHuiCanvasWsGc.
*
*/



#ifndef __HUICANVASWSGC_H__
#define __HUICANVASWSGC_H__


#include <e32cmn.h>
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include "huicanvastexturecache.h"
#include "uiacceltk/HuiCanvasVisual.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiGc.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiVisual.h"
#include "uiacceltk/HuiTextMesh.h"
#include "uiacceltk/HuiCanvasCmdBufferReader.h"
#include "huicanvasgc.h"
#include "HuiRenderSurface.h"
#include "huicanvastexturecache.h"
#include "huicanvasbackground.h"
#include <graphics/wsgraphicscontext.h>
#include <e32cmn.h>
#include <AknLayoutFont.h>

enum THuiCanvasWsGcType
    {
    /** HW accelerated. All window drawing utilizes HW acceleration. */
    EHuiCanvasWsHw,    
    /** SW/HW accelerated. Window internal drawing is done with SW, window "composition" with HW */
    EHuiCanvasWsSw,    
    /** Not accelerated. Drawing is done to internal bitmaps, use SetCapturingBufferL() to get window content */
    EHuiCanvasWsBit
    };

/**
 * This class is used internally by the toolkit and thus should
 * not be used by the applications directly.
 */
class CHuiCanvasWsGc : public CBase
    {

public:
	virtual void WsBitBltL(TInt aBitmapHandle, TPoint aPoint)= 0;
	virtual void WsBitBltRectL(TInt aBitmapHandle, TPoint aPoint, TRect aRect) = 0;
	virtual void WsBitBltMaskedL(TInt aBitmapHandle, TInt aMaskHandle, TInt aInvertMask, TPoint aPoint, TRect aRect) = 0;
	virtual void WsBitBltMaskedPointL(TInt aBitmapHandle, TInt aMaskHandle, TPoint aPoint1, TPoint aPoint2, TRect aRect) = 0;
	
	virtual void WsResetClippingRegionL() = 0;
	virtual void WsClearL()=0;
	virtual void WsClearRectL(TRect& aRect) =0 ;
	virtual void WsResetBrushPatternL() = 0;
	virtual void WsResetFontL() = 0;
	virtual void WsDrawArcL(TPoint aPoint1, TPoint aPoint2, TRect aRect) = 0;
	virtual void WsDrawPieL(TPoint aPoint1, TPoint aPoint2, TRect aRect) = 0;
	virtual void WsDrawBitmap1L(TInt aBitmapHandle, TRect aRect) = 0;
	virtual void WsDrawBitmap2L(TInt aBitmapHandle, TRect aRect1, TRect aRect2) = 0;
	virtual void WsDrawBitmap3L(TInt aBitmapHandle, TPoint aPoint) = 0; //XXX_CFbsBitGc_Hack Remove!!!
	virtual void WsDrawBitmapMaskedL(TInt aBitmapHandle, TInt aMaskHandle, TInt aInvertMask, TRect& aRect1, TRect& aRect2) = 0;
	virtual void WsDrawRoundRectL(TPoint aPoint, TRect& aRect) = 0;
	virtual void WsDrawPolyLineL(CArrayFix<TPoint>* aPointerArray) = 0;
	virtual void WsDrawPolyLineNoEndPointL(TPoint* aPointerArray, TInt aCount)= 0;
	virtual void WsDrawPolygonL(CArrayFix<TPoint>* aPointerArray, TInt aFillRule ) = 0;
	virtual void WsDrawEllipseL(TRect& aRect) = 0;
	virtual void WsDrawLineL(TPoint& aStart, TPoint& aEnd) = 0;
	virtual void WsDrawLineToL(TPoint& aPoint) = 0;
	virtual void WsDrawLineByL(TPoint& aPoint) = 0;
	virtual void WsDrawRectL(TRect& aRect) = 0;
	virtual void WsDrawText1L(TPtr& aTextValue, THuiCanvasTextParameters& aTextParameters) = 0;
	virtual void WsDrawText2L(TPtr& aTextValue, TPoint& aPoint,THuiCanvasTextParameters& aTextParameters) = 0;
	virtual void WsDrawText3L(TPtr& aTextValue, TRect& aRect, THuiCanvasTextParameters& aTextParameters) = 0;
	virtual void WsDrawText4L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextAlign , TInt aTextMargin ,THuiCanvasTextParameters& aTextParameters) = 0;
	virtual void WsDrawText5L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextAlign , TInt aTextMargin ,TInt aTextWidth, THuiCanvasTextParameters& aTextParameters) = 0; //XXX_CFbsBitGc_Hack Remove!!!
	virtual void WsDrawTextVertical1L(TPtr& aTextValue, TInt aTextUp, THuiCanvasTextParameters& aTextParameters)	= 0;
	virtual void WsDrawTextVertical2L(TPtr& aTextValue, TInt aTextUp,TPoint& aPoint, THuiCanvasTextParameters& aTextParameters) = 0;
	virtual void WsDrawTextVertical3L(TPtr& aTextValue, TInt aTextUp, TRect& aRect, THuiCanvasTextParameters& aTextParameters) = 0;
	virtual void WsDrawTextVertical4L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextUp, TInt aTextAlign, TInt aTextMargin, THuiCanvasTextParameters& aTextParameters) = 0;
	virtual void WsDrawTextVertical5L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextUp, TInt aTextAlign, TInt aTextMargin, TInt aTextWidth, THuiCanvasTextParameters& aTextParameters) = 0;
	virtual void WsMoveToL(TPoint& aPoint) = 0;
	virtual void WsMoveByL(TPoint& aPoint) = 0;
	virtual void WsPlotL(TPoint& aPoint) = 0;
	virtual void WsResetL() = 0;
	virtual void WsSetBrushColorL(TRgb aColor) = 0;
	virtual void WsSetBrushOriginL(TPoint& aPoint) = 0;
	virtual void WsSetBrushStyleL(TInt aStyle) = 0;
	virtual void WsSetClippingRegionL(RRegion& aRegion) = 0;
	virtual void WsSetDrawModeL(TInt aDrawModew) = 0;
	virtual void WsSetOriginL(TPoint& aOrigin)= 0;
	virtual void WsSetPenColorL(TRgb aColor) = 0;
	virtual void WsSetPenStyleL(TInt aStyle) = 0;
	virtual void WsSetPenSizeL(TInt aWidth, TInt aHeight) = 0;
	virtual void WsSetTextShadowColorL(TRgb aColor) = 0;
	virtual void WsSetCharJustificationL(TInt aExcessiveWidth, TInt aNumGap) = 0;
	virtual void WsSetWordJustificationL(TInt aExcessiveWidth, TInt aNumGap) = 0;
	virtual void WsSetUnderlineStyleL(TInt aValue) = 0;
	virtual void WsSetBrushPatternL(TInt aValue) = 0;
//	virtual void SetBrushPattern(TInt aFbsBitmapHandle) = 0;
	virtual void WsSetFontL(TInt aFontId) = 0;

	// CFbsBitGc functionality used by Wserv for its internal operations
	virtual void WsCopyRectL(TPoint& aPoint, TRect& aRect) = 0;
	virtual void WsCopySettingsL() = 0;
	virtual void WsUpdateJustificationL(TPtr&  aTextValue) = 0;
	virtual void WsUpdateJustificationVerticalL(TInt aValue, TPtr& aTextValue) = 0;
	virtual void WsSetFontNoDuplicateL(TInt aCurrentFont) = 0;
		
	virtual void WsSetClippingRectL(TRect& aRect) = 0; 
	virtual void WsCancelClippingRectL() = 0; 
	virtual void WsSetFadedL(TInt& aFaded) = 0; 
	virtual void WsSetFadingParametersL(TInt aFaded1, TInt aFaded2) = 0; 
	virtual void WsFadeAreaL(RRegion& aRegion) = 0; 
	virtual void WsMapColorsL() = 0; 
	virtual void WsSetUserDisplayModeL(TInt aMode) = 0; 
	virtual void WsUseFontL(TInt aCurrentFont) = 0;

	// Custom gc commands
    virtual void WsCombinedBitBltMaskedL(TRect aDestinationRect, const RArray<THuiCachedCombinedImageParams>& aBlits) = 0;	 

    // WS painter functionality
  	virtual void BeginActionL(TInt aAction, TRect aDisplayRect,const CHuiCanvasVisual& aUser, TBool aIsCachePrepared, const TRegion& aUpdateRegion)= 0;
    virtual void EnableUpdateRegion(const TRegion& aUpdateRegion, TBool aClear)=0;
    virtual void DisableUpdateRegion() = 0;
    virtual void EndActionL(const TRegion& aUpdateRegion, TBool aUpdateDisplay)= 0;    
  	virtual TInt SetCapturingBufferL(CFbsBitmap* aTarget) = 0;
    virtual CHuiCanvasGc& CanvasGc() const = 0;    
    virtual void SetPositionDelta( TPoint& aPoint ) = 0;
    virtual void ClearCache() = 0;

    virtual void SetRelativeOrientation(CHuiGc::TOrientation aOrientation) = 0;
    
    virtual TBool IsRenderBufferEnabled() const = 0;
    virtual TBool IsRenderBufferPrepared() const = 0;
    virtual TBool IsRenderBufferInitialized() const = 0;
    
    virtual TInt EnableRenderbuffer(TBool aEnable = ETrue) = 0;
    virtual THuiCanvasWsGcType Type() const = 0;

    virtual void ClearCapturingBufferArea(const TRect& aRect) = 0;
    	
    ~CHuiCanvasWsGc(){}         
    };
#endif  // __HUICANVASWSGC_H__
