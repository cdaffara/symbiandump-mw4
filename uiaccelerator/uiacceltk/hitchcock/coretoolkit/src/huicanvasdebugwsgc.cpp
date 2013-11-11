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


#include "huicanvasdebugwsgc.h"
#include "huicanvaswsgc.h"
#include "alfcommanddebug.h"

void CHuiCanvasDebugWsGc::WsBitBltL(TInt aBitmapHandle, TPoint aPoint)
    {
    iCommandDebugger.SetHandle(aBitmapHandle);
    iCommandDebugger.SetPoint(aPoint);
    iCommandDebugger.Print();
    iCanvasWsGc->WsBitBltL(aBitmapHandle, aPoint);
    }

void CHuiCanvasDebugWsGc::WsBitBltRectL(TInt aBitmapHandle, TPoint aPoint,
        TRect aRect)
    {
    iCommandDebugger.SetHandle(aBitmapHandle);
    iCommandDebugger.SetRect(aRect);
    iCommandDebugger.SetPoint(aPoint);
    iCommandDebugger.Print();
    iCanvasWsGc->WsBitBltRectL(
            aBitmapHandle,
            aPoint,
            aRect);
    }

void CHuiCanvasDebugWsGc::WsBitBltMaskedL(TInt aBitmapHandle,
        TInt aMaskHandle, TInt aInvertMask, TPoint aPoint, TRect aRect)
    {
    iCommandDebugger.SetHandle(aBitmapHandle);
    iCommandDebugger.SetRect(aRect);
    iCommandDebugger.SetPoint(aPoint);
    iCommandDebugger.Print();

    iCanvasWsGc->WsBitBltMaskedL(aBitmapHandle, aMaskHandle, aInvertMask,
            aPoint, aRect);
    }

void CHuiCanvasDebugWsGc::WsBitBltMaskedPointL(TInt aBitmapHandle,
        TInt aMaskHandle, TPoint aPoint1, TPoint aPoint2, TRect aRect)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsBitBltMaskedPointL(aBitmapHandle, aMaskHandle, aPoint1,
            aPoint2, aRect);
    }

void CHuiCanvasDebugWsGc::WsResetClippingRegionL()
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsResetClippingRegionL();
    }

void CHuiCanvasDebugWsGc::WsClearL()
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsClearL();
    }

void CHuiCanvasDebugWsGc::WsClearRectL(TRect& aRect)
    {
    iCommandDebugger.SetRect(aRect);
    iCommandDebugger.Print();
    iCanvasWsGc->WsClearRectL(aRect);
    }
void CHuiCanvasDebugWsGc::WsResetBrushPatternL()
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsResetBrushPatternL();
    }

void CHuiCanvasDebugWsGc::WsResetFontL()
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsResetFontL();
    }
;
void CHuiCanvasDebugWsGc::WsDrawArcL(TPoint aPoint1, TPoint aPoint2,
        TRect aRect)
    {
    iCommandDebugger.Print();    
    iCanvasWsGc->WsDrawArcL(aPoint1, aPoint2, aRect);
    }

void CHuiCanvasDebugWsGc::WsDrawPieL(TPoint aPoint1, TPoint aPoint2,
        TRect aRect)
    {
    iCommandDebugger.Print();    
    iCanvasWsGc->WsDrawPieL(aPoint1, aPoint2, aRect);
    }

void CHuiCanvasDebugWsGc::WsDrawBitmap1L(TInt aBitmapHandle, TRect aRect)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsDrawBitmap1L(aBitmapHandle, aRect);
    }

void CHuiCanvasDebugWsGc::WsDrawBitmap2L(TInt aBitmapHandle, TRect aRect1,
        TRect aRect2)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsDrawBitmap2L(aBitmapHandle, aRect1, aRect2);
    }

void CHuiCanvasDebugWsGc::WsDrawBitmap3L(TInt aBitmapHandle, TPoint aPoint)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsDrawBitmap3L(aBitmapHandle, aPoint);
    }
void CHuiCanvasDebugWsGc::WsDrawBitmapMaskedL(TInt aBitmapHandle,
        TInt aMaskHandle, TInt aInvertMask, TRect& aRect1, TRect& aRect2)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsDrawBitmapMaskedL(aBitmapHandle,
            aMaskHandle,
            aInvertMask,
            aRect1, aRect2);
    }

void CHuiCanvasDebugWsGc::WsDrawRoundRectL(TPoint aPoint, TRect& aRect)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsDrawRoundRectL(aPoint, aRect);
    }

void CHuiCanvasDebugWsGc::WsDrawPolyLineL(CArrayFix<TPoint>* aPointerArray)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsDrawPolyLineL(aPointerArray);
    }

void CHuiCanvasDebugWsGc::WsDrawPolyLineNoEndPointL(TPoint* aPointerArray,
        TInt aCount)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsDrawPolyLineNoEndPointL(aPointerArray, aCount);
    }

void CHuiCanvasDebugWsGc::WsDrawPolygonL(CArrayFix<TPoint>* aPointerArray,
        TInt aFillRule)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsDrawPolygonL(aPointerArray, aFillRule);
    }

void CHuiCanvasDebugWsGc::WsDrawEllipseL(TRect& aRect)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsDrawEllipseL(aRect);
    }

void CHuiCanvasDebugWsGc::WsDrawLineL(TPoint& aStart, TPoint& aEnd)
    {
    iCommandDebugger.SetPoint( aStart );
    iCommandDebugger.SetPoint( aEnd );
    iCommandDebugger.Print();
    iCanvasWsGc->WsDrawLineL( aStart, aEnd);
    }

void CHuiCanvasDebugWsGc::WsDrawLineToL(TPoint& aPoint)
    {
    iCommandDebugger.SetPoint( aPoint );
    iCommandDebugger.Print();
    iCanvasWsGc->WsDrawLineToL(aPoint);
    }

void CHuiCanvasDebugWsGc::WsDrawLineByL(TPoint& aPoint)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsDrawLineByL(aPoint);
    }

void CHuiCanvasDebugWsGc::WsDrawRectL(TRect& aRect)
    {
    iCommandDebugger.SetRect( aRect );
    iCommandDebugger.Print();
    iCanvasWsGc->WsDrawRectL(aRect);
    }

void CHuiCanvasDebugWsGc::WsDrawText1L(TPtr& aTextValue,
        THuiCanvasTextParameters& aTextParameters)
    {
    iCommandDebugger.SetText( aTextValue );
    iCommandDebugger.Print();
    iCanvasWsGc->WsDrawText1L(aTextValue, aTextParameters);
    }

void CHuiCanvasDebugWsGc::WsDrawText2L(TPtr& aTextValue, TPoint& aPoint,
        THuiCanvasTextParameters& aTextParameters)
    {
    iCommandDebugger.SetText( aTextValue );
     iCommandDebugger.Print();
        iCanvasWsGc->WsDrawText2L(aTextValue, aPoint, aTextParameters);
    }

void CHuiCanvasDebugWsGc::WsDrawText3L(TPtr& aTextValue, TRect& aRect,
        THuiCanvasTextParameters& aTextParameters)
    {
    iCommandDebugger.SetText( aTextValue );
        iCommandDebugger.Print();
       
    iCanvasWsGc->WsDrawText3L(aTextValue, aRect, aTextParameters);
    }

void CHuiCanvasDebugWsGc::WsDrawText4L(TPtr& aTextValue, TRect& aRect,
        TInt aBaselineOffset, TInt aTextAlign, TInt aTextMargin,
        THuiCanvasTextParameters& aTextParameters)
    {
    iCommandDebugger.SetText(aTextValue);
    iCommandDebugger.SetRect(aRect);
    iCommandDebugger.Print();
    iCanvasWsGc->WsDrawText4L(aTextValue, aRect,
            aBaselineOffset, // textBaselineOffset
            aTextAlign, // textAlign
            aTextMargin, // textMargin,
            aTextParameters);
    }

void CHuiCanvasDebugWsGc::WsDrawText5L(TPtr& aTextValue, TRect& aRect,
        TInt aBaselineOffset, TInt aTextAlign, TInt aTextMargin,
        TInt aTextWidth, THuiCanvasTextParameters& aTextParameters)
    {
    iCommandDebugger.SetText(aTextValue);
    iCommandDebugger.SetRect(aRect);
    iCommandDebugger.Print();

    iCanvasWsGc->WsDrawText5L(aTextValue, aRect,
            aBaselineOffset, // textBaselineOffset
            aTextAlign, // textAlign
            aTextMargin, // textMargin,
            aTextWidth, // textWidth
            aTextParameters);
    }

void CHuiCanvasDebugWsGc::WsDrawTextVertical1L(TPtr& aTextValue,
        TInt aTextUp, THuiCanvasTextParameters& aTextParameters)
    {
    iCommandDebugger.SetText(aTextValue);
    iCommandDebugger.Print();

    iCanvasWsGc->WsDrawTextVertical1L(aTextValue, aTextUp,
            aTextParameters);
    }

void CHuiCanvasDebugWsGc::WsDrawTextVertical2L(TPtr& aTextValue,
        TInt aTextUp, TPoint& aPoint,
        THuiCanvasTextParameters& aTextParameters)
    {
    iCommandDebugger.SetText(aTextValue);
    iCommandDebugger.SetPoint( aPoint );
    iCommandDebugger.Print();

    iCanvasWsGc->WsDrawTextVertical2L(aTextValue, aTextUp, aPoint,
            aTextParameters);
    }

void CHuiCanvasDebugWsGc::WsDrawTextVertical3L(TPtr& aTextValue,
        TInt aTextUp, TRect& aRect, THuiCanvasTextParameters& aTextParameters)
    {
    iCommandDebugger.SetText(aTextValue);
    iCommandDebugger.Print();
    iCanvasWsGc->WsDrawTextVertical3L(aTextValue, aTextUp, aRect,
            aTextParameters);
    }

void CHuiCanvasDebugWsGc::WsDrawTextVertical4L(TPtr& aTextValue,
        TRect& aRect, TInt aBaselineOffset, TInt aTextUp, TInt aTextAlign,
        TInt aTextMargin, THuiCanvasTextParameters& aTextParameters)
    {
    iCommandDebugger.SetText(aTextValue);
    iCommandDebugger.Print();
        
    iCanvasWsGc->WsDrawTextVertical4L(aTextValue, aRect, aBaselineOffset,
            aTextUp, aTextAlign, aTextMargin, aTextParameters);
    }

void CHuiCanvasDebugWsGc::WsDrawTextVertical5L(TPtr& aTextValue,
        TRect& aRect, TInt aBaselineOffset, TInt aTextUp, TInt aTextAlign,
        TInt aTextMargin, TInt aTextWidth,
        THuiCanvasTextParameters& aTextParameters)
    {
    iCommandDebugger.SetText(aTextValue);
    iCommandDebugger.Print();
        
    iCanvasWsGc->WsDrawTextVertical5L(aTextValue, aRect, aBaselineOffset,
            aTextUp, aTextAlign, aTextMargin, aTextWidth, aTextParameters);
    }

void CHuiCanvasDebugWsGc::WsMoveToL(TPoint& aPoint)
    {
    iCommandDebugger.SetPoint(aPoint);
    iCommandDebugger.Print();
    iCanvasWsGc->WsMoveToL(aPoint);
    }

void CHuiCanvasDebugWsGc::WsMoveByL(TPoint& aPoint)
    {
    iCommandDebugger.SetPoint(aPoint);
    iCommandDebugger.Print();
    iCanvasWsGc->WsMoveByL(aPoint);
    }

void CHuiCanvasDebugWsGc::WsPlotL(TPoint& aPoint)
    {
    iCommandDebugger.SetPoint(aPoint);
    iCommandDebugger.Print();
        
    iCanvasWsGc->WsPlotL(aPoint);
    }

void CHuiCanvasDebugWsGc::WsResetL()
    {
    iCommandDebugger.Print();
        
    iCanvasWsGc->WsResetL();
    }

void CHuiCanvasDebugWsGc::WsSetBrushColorL(TRgb aColor)
    {
    iCommandDebugger.SetColor(aColor);
    iCommandDebugger.Print();
    iCanvasWsGc->WsSetBrushColorL(aColor);
    }

void CHuiCanvasDebugWsGc::WsSetBrushOriginL(TPoint& aPoint)
    {
    iCommandDebugger.SetPoint(aPoint);
    iCommandDebugger.Print();
    iCanvasWsGc->WsSetBrushOriginL(aPoint);
    }
;
void CHuiCanvasDebugWsGc::WsSetBrushStyleL(TInt aStyle)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsSetBrushStyleL(aStyle);
    }
;
void CHuiCanvasDebugWsGc::WsSetClippingRegionL(RRegion& aRegion)
    {
    iCommandDebugger.SetRegion(aRegion);
    iCommandDebugger.Print();
    iCanvasWsGc->WsSetClippingRegionL(aRegion);
    }

void CHuiCanvasDebugWsGc::WsSetDrawModeL(TInt aDrawMode)
    {
    iCommandDebugger.SetHandle(aDrawMode);
    iCommandDebugger.Print();
    iCanvasWsGc->WsSetDrawModeL(aDrawMode);
    }

void CHuiCanvasDebugWsGc::WsSetOriginL(TPoint& aOrigin)
    {
    iCommandDebugger.SetPoint( aOrigin );
    iCommandDebugger.Print();
    iCanvasWsGc->WsSetOriginL(aOrigin);

    }
void CHuiCanvasDebugWsGc::WsSetPenColorL(TRgb aColor)
    {
    iCommandDebugger.SetColor(aColor);
    iCommandDebugger.Print();
    iCanvasWsGc->WsSetPenColorL(aColor);
    }

void CHuiCanvasDebugWsGc::WsSetPenStyleL(TInt aStyle)
    {
    iCommandDebugger.SetHandle( aStyle );
    iCommandDebugger.Print();
    iCanvasWsGc->WsSetPenStyleL(aStyle);
    }

void CHuiCanvasDebugWsGc::WsSetPenSizeL(TInt aWidth, TInt aHeight)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsSetPenSizeL( aWidth, aHeight);
    }

void CHuiCanvasDebugWsGc::WsSetTextShadowColorL(TRgb aColor)
    {
    iCommandDebugger.SetColor(aColor);
    iCommandDebugger.Print();
    iCanvasWsGc->WsSetTextShadowColorL(aColor);
    }
;
void CHuiCanvasDebugWsGc::WsSetCharJustificationL(TInt aExcessiveWidth,
        TInt aNumGap)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsSetCharJustificationL(aExcessiveWidth, aNumGap);
    }

void CHuiCanvasDebugWsGc::WsSetWordJustificationL(TInt aExcessiveWidth,
        TInt aNumGap)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsSetWordJustificationL(aExcessiveWidth, aNumGap);
    }

void CHuiCanvasDebugWsGc::WsSetUnderlineStyleL(TInt aValue)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsSetUnderlineStyleL( aValue );
    }

void CHuiCanvasDebugWsGc::WsSetBrushPatternL(TInt aValue)
    {
    iCommandDebugger.Print();
        
    iCanvasWsGc->WsSetBrushPatternL(aValue);
    }

//  void CHuiCanvasDebugWsGc::SetBrushPattern(TInt aFbsBitmapHandle){};
void CHuiCanvasDebugWsGc::WsSetFontL(TInt aFontId)
    {
    iCommandDebugger.SetHandle( aFontId );
    iCommandDebugger.Print();
        
    iCanvasWsGc->WsSetFontL(aFontId);
    }

void CHuiCanvasDebugWsGc::WsCopyRectL(TPoint& aPoint, TRect& aRect)
    {
    iCommandDebugger.SetPoint( aPoint );
    iCommandDebugger.SetRect( aRect );
    iCommandDebugger.Print();
    iCanvasWsGc->WsCopyRectL(aPoint, aRect);
    }

void CHuiCanvasDebugWsGc::WsCopySettingsL()
    {
    iCommandDebugger.Print();
        
    iCanvasWsGc->WsCopySettingsL();
    }
;
void CHuiCanvasDebugWsGc::WsUpdateJustificationL(TPtr& aTextValue)
    {
    iCommandDebugger.SetText( aTextValue );
    iCommandDebugger.Print();
        
    iCanvasWsGc->WsUpdateJustificationL(aTextValue);
    }

void CHuiCanvasDebugWsGc::WsUpdateJustificationVerticalL(TInt aValue,
        TPtr& aTextValue)
    {
    iCommandDebugger.Print();
        
    iCanvasWsGc->WsUpdateJustificationVerticalL( aValue , aTextValue);
    }

void CHuiCanvasDebugWsGc::WsSetFontNoDuplicateL(TInt aCurrentFont)
    {
    iCommandDebugger.SetHandle( aCurrentFont );
    iCommandDebugger.Print();
        
    iCanvasWsGc->WsSetFontNoDuplicateL(aCurrentFont);
    }


void CHuiCanvasDebugWsGc::WsSetClippingRectL(TRect& aRect)
    {
    iCommandDebugger.SetRect(aRect);
    iCommandDebugger.Print();
    iCanvasWsGc->WsSetClippingRectL(aRect);
    }

void CHuiCanvasDebugWsGc::WsCancelClippingRectL()
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsCancelClippingRectL();
    }

void CHuiCanvasDebugWsGc::WsSetFadedL(TInt& aFaded)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsSetFadedL(aFaded);
    }

void CHuiCanvasDebugWsGc::WsSetFadingParametersL(TInt aFaded1, TInt aFaded2)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsSetFadingParametersL(aFaded1, aFaded2);
    }

void CHuiCanvasDebugWsGc::WsFadeAreaL(RRegion& aRegion)
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsFadeAreaL(aRegion);
    }

void CHuiCanvasDebugWsGc::WsMapColorsL()
    {
    iCommandDebugger.Print();
    iCanvasWsGc->WsMapColorsL();
    }

void CHuiCanvasDebugWsGc::WsSetUserDisplayModeL(TInt aMode)
    {
    iCommandDebugger.SetHandle(aMode);
    iCommandDebugger.Print();
    iCanvasWsGc->WsSetUserDisplayModeL(aMode);
    }

void CHuiCanvasDebugWsGc::WsUseFontL(TInt aCurrentFont)
    {
    iCommandDebugger.SetHandle(aCurrentFont);
    iCommandDebugger.Print();
    iCanvasWsGc->WsUseFontL(aCurrentFont);
    }

void CHuiCanvasDebugWsGc::WsCombinedBitBltMaskedL(TRect aDestinationRect,
        const RArray<THuiCachedCombinedImageParams>& aBlits)
    {
    iCommandDebugger.SetRect(aDestinationRect);
    iCommandDebugger.Print();

    iCanvasWsGc->WsCombinedBitBltMaskedL(aDestinationRect, aBlits );
    }


void CHuiCanvasDebugWsGc::BeginActionL(TInt aAction, TRect aDisplayRect,
        const CHuiCanvasVisual& aUser, TBool aIsCachePrepared, const TRegion& aUpdateRegion)
    {
    iCanvasWsGc->BeginActionL(aAction, aDisplayRect, aUser, aIsCachePrepared, aUpdateRegion);
    }

void CHuiCanvasDebugWsGc::EndActionL(const TRegion& aUpdateRegion, TBool aUpdateDisplay)
    {
    iCanvasWsGc->EndActionL(aUpdateRegion, aUpdateDisplay);
    }


TInt CHuiCanvasDebugWsGc::SetCapturingBufferL(CFbsBitmap* aTarget)
    {
    return iCanvasWsGc->SetCapturingBufferL( aTarget );
    }

CHuiCanvasGc& CHuiCanvasDebugWsGc::CanvasGc() const
    {
    return iCanvasWsGc->CanvasGc();
    }

void CHuiCanvasDebugWsGc::ClearCache()
    {
    iCanvasWsGc->ClearCache();        
    }

void CHuiCanvasDebugWsGc::SetRelativeOrientation(CHuiGc::TOrientation aOrientation)
    {
    iCanvasWsGc->SetRelativeOrientation(aOrientation);
    }

TBool CHuiCanvasDebugWsGc::IsRenderBufferEnabled() const
    {
    return iCanvasWsGc->IsRenderBufferEnabled();    
    }

TBool CHuiCanvasDebugWsGc::IsRenderBufferPrepared() const
    {
    return iCanvasWsGc->IsRenderBufferPrepared();    
    }

TBool CHuiCanvasDebugWsGc::IsRenderBufferInitialized() const
    {
    return iCanvasWsGc->IsRenderBufferInitialized();    
    }

TInt CHuiCanvasDebugWsGc::EnableRenderbuffer(TBool aEnable)
    {
    return iCanvasWsGc->EnableRenderbuffer(aEnable);    
    }

void CHuiCanvasDebugWsGc::ClearCapturingBufferArea(const TRect& aRect)
    {
    iCanvasWsGc->ClearCapturingBufferArea(aRect);
    }

THuiCanvasWsGcType CHuiCanvasDebugWsGc::Type() const
    {
    return iCanvasWsGc->Type();
    }

CHuiCanvasDebugWsGc::CHuiCanvasDebugWsGc( CAlfCommandDebug& aCommandDebugger ) : iCommandDebugger( aCommandDebugger )
    {
    
    }

CHuiCanvasDebugWsGc* CHuiCanvasDebugWsGc::NewL( CHuiCanvasWsGc* aHuiCanvasGc, CAlfCommandDebug& aCommandDebugger )
    {
    CHuiCanvasDebugWsGc* self;
    self = new(ELeave)CHuiCanvasDebugWsGc( aCommandDebugger );
    CleanupStack::PushL( self );
    self->ConstructL( aHuiCanvasGc);
    CleanupStack::Pop();
    return self;
    }

void CHuiCanvasDebugWsGc::ConstructL(
        CHuiCanvasWsGc* aHuiCanvasGc )
    {
    
    iCanvasWsGc = aHuiCanvasGc;
     
    }

void CHuiCanvasDebugWsGc::SetInternalGc(CHuiCanvasWsGc* aHuiCanvasGc)
    {
    iCanvasWsGc = aHuiCanvasGc;
    }

void CHuiCanvasDebugWsGc::SetPositionDelta(TPoint& aPoint)
    {
    iCanvasWsGc->SetPositionDelta(aPoint);
    }

CHuiCanvasDebugWsGc::~CHuiCanvasDebugWsGc()
    {
    }

void CHuiCanvasDebugWsGc::EnableUpdateRegion(const TRegion& aUpdateRegion, TBool aClear)
    {
    iCanvasWsGc->EnableUpdateRegion(aUpdateRegion, aClear);
    }

void CHuiCanvasDebugWsGc::DisableUpdateRegion()
    {
    iCanvasWsGc->DisableUpdateRegion();
    }

    
