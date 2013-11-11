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
* Description:   Implementation of CHuiBorderBrush class - a brush that draws 
*                a border at the edges of visuals.
*
*/



#include "uiacceltk/HuiBorderBrush.h"  // Class definition
#include "uiacceltk/HuiDrawing.h"
#include "uiacceltk/HuiVisual.h"


EXPORT_C CHuiBorderBrush* CHuiBorderBrush::NewL(
                      TInt aThicknessWidth, TInt aThicknessHeight,
                      TInt aEdgeOffsetX, TInt aEdgeOffsetY)
    {
    CHuiBorderBrush* self = NewLC(aThicknessWidth, aThicknessHeight,
                                  aEdgeOffsetX, aEdgeOffsetY);
    CleanupStack::Pop();
    return self;
    }


EXPORT_C CHuiBorderBrush* CHuiBorderBrush::NewLC(
                       TInt aThicknessWidth, TInt aThicknessHeight,
                       TInt aEdgeOffsetX, TInt aEdgeOffsetY)
    {
    CHuiBorderBrush* self = new (ELeave)
        CHuiBorderBrush(aThicknessWidth, aThicknessHeight,
                        aEdgeOffsetX, aEdgeOffsetY);
    CleanupStack::PushL(self);
    return self;
    }


CHuiBorderBrush::CHuiBorderBrush(TInt aThicknessWidth, TInt aThicknessHeight,
                                 TInt aEdgeOffsetX, TInt aEdgeOffsetY)
        : CHuiBrush(),
          iColor(KRgbWhite),
          iThickness( TReal32(aThicknessWidth), TReal32(aThicknessHeight)),
          iEdgeOffset( TReal32(aEdgeOffsetX), TReal32(aEdgeOffsetY))
    {
    SetClipToVisual(EFalse);
    }

EXPORT_C CHuiBorderBrush* CHuiBorderBrush::NewL(const THuiXYMetric& aThickness,
                                                const THuiXYMetric& aEdgeOffset)
    {
    CHuiBorderBrush* self = NewLC(aThickness, aEdgeOffset);
    CleanupStack::Pop();
    return self;
    }


EXPORT_C CHuiBorderBrush* CHuiBorderBrush::NewLC(const THuiXYMetric& aThickness,
                                                 const THuiXYMetric& aEdgeOffset)
    {
    CHuiBorderBrush* self = new (ELeave)
        CHuiBorderBrush(aThickness, aEdgeOffset);
    CleanupStack::PushL(self);
    return self;
    }


CHuiBorderBrush::CHuiBorderBrush(const THuiXYMetric& aThickness,
                                 const THuiXYMetric& aEdgeOffset)
        : CHuiBrush(),
          iColor(KRgbWhite),
          iThickness(aThickness.iX.iMagnitude, aThickness.iY.iMagnitude),
          iEdgeOffset(aEdgeOffset.iX.iMagnitude, aEdgeOffset.iY.iMagnitude),
          iThicknessMetric(aThickness),
          iEdgeOffsetMetric(aEdgeOffset)
    {
    SetClipToVisual(EFalse);
    }

    
CHuiBorderBrush::~CHuiBorderBrush()
    {
    }


EXPORT_C void CHuiBorderBrush::SetImage(const THuiImage& aImage)
    {
    iImage = aImage;
    SetChanged();
    }


EXPORT_C void CHuiBorderBrush::SetColor(const TRgb& aColor)
    {
    iColor = aColor;
    SetChanged();
    }

EXPORT_C void CHuiBorderBrush::SetThickness(const THuiXYMetric& aThickness)
    {
    iThicknessMetric = aThickness;
    iThickness.iX.Set(aThickness.iX.iMagnitude);    
    iThickness.iY.Set(aThickness.iY.iMagnitude);    
    }

EXPORT_C void CHuiBorderBrush::SetEdgeOffset(const THuiXYMetric& aEdgeOffset)
    {
    iEdgeOffsetMetric = aEdgeOffset;
    iEdgeOffset.iX.Set(aEdgeOffset.iX.iMagnitude);    
    iEdgeOffset.iY.Set(aEdgeOffset.iY.iMagnitude);            
    }

EXPORT_C void CHuiBorderBrush::SetImageOffset(const THuiXYMetric& aImageOffset)
    {
    iImageOffsetMetric = aImageOffset;
    iImageOffset.iX.Set(aImageOffset.iX.iMagnitude);    
    iImageOffset.iY.Set(aImageOffset.iY.iMagnitude);                    
    }


TBool CHuiBorderBrush::Changed() const
    {
    if ( CHuiBrush::Changed() )
        {
        return ETrue;
        }
    return iThickness.Changed() || iEdgeOffset.Changed() || iImageOffset.Changed() || iImage.Changed();
    }

void CHuiBorderBrush::ClearChanged()
    {
    CHuiBrush::ClearChanged();
    iThickness.ClearChanged();
    iEdgeOffset.ClearChanged();
    iImageOffset.ClearChanged();
    iImage.ClearChanged();
    }

void CHuiBorderBrush::ExpandVisualRect(TRect& aRect) const
    {
    // We need access to visual to convert metric units into pixels,
    // currently a pointer to visual is provided through separate
    // method since we want to avoid breaking binary compatibility 
    // of this class.
    THuiRealPoint edgeOffsetInPixels = EdgeOffsetInPixels(BrushGuide());
    aRect.Grow((TInt)edgeOffsetInPixels.iX,
               (TInt)edgeOffsetInPixels.iY);    
    }


void CHuiBorderBrush::Draw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const
    {
    THuiRealRect content = aGuide.BrushRect();
    TReal32 opacity = aGuide.BrushOpacity() * iOpacity.Now();
    
    THuiRealPoint edgeOffsetInPixels = EdgeOffsetInPixels(BrushGuide());
    THuiRealPoint thicknessInPixels = ThicknessInPixels(BrushGuide());
    THuiRealPoint imageOffsetInPixels = ImageOffsetInPixels(BrushGuide());

    aGc.SetPenColor(iColor);
    aGc.SetPenAlpha(TInt(opacity * 255));

    if(iImage.HasTexture())
        {
        aGc.Enable(CHuiGc::EFeatureTexturing);
        iImage.Texture().Bind();
        }
    else
        {
        aGc.Disable(CHuiGc::EFeatureTexturing);
        }
    aGc.Enable(CHuiGc::EFeatureBlending);

    // Apply the texture offset manually.
    aGc.Push(EHuiGcMatrixTexture);
    aGc.LoadIdentity(EHuiGcMatrixTexture);
    aGc.Translate(EHuiGcMatrixTexture, 
                    -imageOffsetInPixels.iX, -imageOffsetInPixels.iY, 0.f);

    content.Grow(edgeOffsetInPixels.iX, edgeOffsetInPixels.iY);
        
    aGc.DrawBorders(content.Round(), 
                    thicknessInPixels.iX, thicknessInPixels.iX,
                    thicknessInPixels.iY, thicknessInPixels.iY,
                    CHuiGc::EBorderFixedCorners, 
                    &iImage);

    aGc.Pop(EHuiGcMatrixTexture);
    }

THuiRealPoint CHuiBorderBrush::ThicknessInPixels(const CHuiVisual* aVisual) const 
    {
    return PointInPixels(aVisual, iThickness, iThicknessMetric);    
    }

THuiRealPoint CHuiBorderBrush::EdgeOffsetInPixels(const CHuiVisual* aVisual) const 
    {
    return PointInPixels(aVisual, iEdgeOffset, iEdgeOffsetMetric);            
    }

THuiRealPoint CHuiBorderBrush::ImageOffsetInPixels(const CHuiVisual* aVisual) const 
    {
    return PointInPixels(aVisual, iImageOffset, iImageOffsetMetric);            
    }

THuiRealPoint CHuiBorderBrush::PointInPixels(const CHuiVisual* aVisual, const THuiTimedPoint& aPoint, const THuiXYMetric& aMetric) const 
    {
    THuiRealPoint pointInPixels = aPoint.RealNow(); // Init, if no visual available for conversion then defaults to pixels
    if (aVisual)
        {
        THuiXYMetric metric = aMetric;
        metric.iX.iMagnitude = pointInPixels.iX;        
        metric.iY.iMagnitude = pointInPixels.iY;        
        pointInPixels = aVisual->MetricToPixels(metric); // Convert metric width in pixels  
        }    
    return pointInPixels;                    
    }
