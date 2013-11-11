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
* Description:   ?Description
*
*/



#include "uiacceltk/HuiDropShadowBrush.h"  // Class definition
#include "uiacceltk/HuiDrawing.h"
#include "uiacceltk/HuiVisual.h"
#include "uiacceltk/HuiUtil.h"


/** Default shadow softness. */
const TReal32 KHuiDropShadowBrushSoftness = 3.f;

/** Default shadow opacity. */
const TReal32 KHuiDropShadowBrushOpacity = 100.f/255.f;


EXPORT_C CHuiDropShadowBrush* CHuiDropShadowBrush::NewL(const THuiMetric& aDepth)
    {
    CHuiDropShadowBrush* self = NewLC(aDepth);
    CleanupStack::Pop();
    return self;
    }


EXPORT_C CHuiDropShadowBrush* CHuiDropShadowBrush::NewLC(const THuiMetric& aDepth)
    {
    CHuiDropShadowBrush* self = new (ELeave) CHuiDropShadowBrush(aDepth);
    CleanupStack::PushL(self);
    return self;
    }


EXPORT_C CHuiDropShadowBrush* CHuiDropShadowBrush::NewL(TInt aDepth)
    {
    CHuiDropShadowBrush* self = NewLC(aDepth);
    CleanupStack::Pop();
    return self;
    }


EXPORT_C CHuiDropShadowBrush* CHuiDropShadowBrush::NewLC(TInt aDepth)
    {
    CHuiDropShadowBrush* self = new (ELeave) CHuiDropShadowBrush(aDepth);
    CleanupStack::PushL(self);
    return self;
    }

CHuiDropShadowBrush::CHuiDropShadowBrush(const THuiMetric& aDepth)
        : CHuiBrush(),
          iSoftness(KHuiDropShadowBrushSoftness),
          iDepth(aDepth.iMagnitude),
          iDepthMetric(aDepth),
          iSoftnessMetric(KHuiDropShadowBrushSoftness),
          iColor(TRgb(0, 0, 110))
    {
    SetClipToVisual(EFalse);
    iOpacity.Set(KHuiDropShadowBrushOpacity);
    }

CHuiDropShadowBrush::CHuiDropShadowBrush(TInt aDepth)
        : CHuiBrush(),
          iSoftness(KHuiDropShadowBrushSoftness),
          iDepth(TReal32(aDepth)),
          iDepthMetric(aDepth),
          iSoftnessMetric(KHuiDropShadowBrushSoftness),
          iColor(TRgb(0, 0, 110)) 
    {
    SetClipToVisual(EFalse);
    iOpacity.Set(KHuiDropShadowBrushOpacity);
    }


CHuiDropShadowBrush::~CHuiDropShadowBrush()
    {
    }
    
TBool CHuiDropShadowBrush::Changed() const
    {
    if ( CHuiBrush::Changed() )
        {
        return ETrue;
        }
    return iSoftness.Changed() || iDepth.Changed();
    }

void CHuiDropShadowBrush::ClearChanged()
    {
    CHuiBrush::ClearChanged();
    iSoftness.ClearChanged();
    iDepth.ClearChanged();
    }

void CHuiDropShadowBrush::Draw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const
    {
    THuiRealRect realRect = aGuide.BrushRect();
    TReal32 opacity = aGuide.BrushOpacity() * iOpacity.Now();
    TReal32 distanceInPixels = DepthInPixels(BrushGuide());
    TReal32 softnessInPixels = SoftnessInPixels(BrushGuide());

    TReal32 borderWidth = (softnessInPixels + distanceInPixels/2.f); //distance;

    aGc.Push(EHuiGcMatrixModel);
    aGc.Translate(EHuiGcMatrixModel, distanceInPixels, distanceInPixels, 0.0f);

    borderWidth = Min( borderWidth, realRect.Width()/2.f );
    borderWidth = Min( borderWidth, realRect.Height()/2.f );

    aGc.SetPenColor(iColor);
    aGc.SetPenAlpha(TInt(opacity * 255));

    const CHuiTexture* shadowTexture = NULL;
    TInt err = aGuide.BrushSkin().GetTexture(EHuiSkinShadowTexture, shadowTexture);
    if (err!=KErrNone)
    	{
    	ASSERT(EFalse); // failed to get the shadow texture - unable to proceed!
    	return;
	    }
    THuiImage dropShadowImage(*shadowTexture);

    aGc.Disable(CHuiGc::EFeatureDepthWrite);

	TRect roundedRect = realRect.Round();
	// draw drop shadow borders
    aGc.DrawBorders(roundedRect, borderWidth, borderWidth, borderWidth, borderWidth, 
                    CHuiGc::EBorderImage, &dropShadowImage);                   
    
    aGc.Disable(CHuiGc::EFeatureTexturing);
    roundedRect.Shrink( HUI_ROUND_FLOAT_TO_INT(borderWidth), HUI_ROUND_FLOAT_TO_INT(borderWidth));
    aGc.DrawRect(roundedRect);
    aGc.Enable(CHuiGc::EFeatureDepthWrite);

    aGc.Pop(EHuiGcMatrixModel);
    }


void CHuiDropShadowBrush::ExpandVisualRect(TRect& aRect) const
    {
    TReal32 depthInPixels = DepthInPixels(BrushGuide());
    
    if ( depthInPixels > 0 )
        {
        // Currently support only drop shadow on the bottom right of the visual
        aRect.iBr.iX += HUI_ROUND_FLOAT_TO_INT( depthInPixels );
        aRect.iBr.iY += HUI_ROUND_FLOAT_TO_INT( depthInPixels );
        }
    }
    
EXPORT_C void CHuiDropShadowBrush::SetDepth(const THuiMetric& aDepth)
    {
    iDepthMetric = aDepth;
    iDepth.Set(aDepth.iMagnitude);    
    }

EXPORT_C void CHuiDropShadowBrush::SetSoftness(const THuiMetric& aSoftness)
    {
    iSoftnessMetric = aSoftness;
    iSoftness.Set(aSoftness.iMagnitude);    
    }

EXPORT_C void CHuiDropShadowBrush::SetColor(const TRgb& aColor)
    {
    iColor = aColor;    
    }
    
TReal32 CHuiDropShadowBrush::DepthInPixels(const CHuiVisual* aVisual) const
    {
    return ValueInPixels(aVisual, iDepth);                    
    }
    
TReal32 CHuiDropShadowBrush::SoftnessInPixels(const CHuiVisual* aVisual) const
    {
    return ValueInPixels(aVisual, iSoftness);                            
    }

TReal32 CHuiDropShadowBrush::ValueInPixels(const CHuiVisual* aVisual, const THuiTimedValue& aValue) const
    {
    TReal32 valueInPixels = aValue.Now(); // Default to old pixel width
    if (aVisual)
        {
        THuiMetric metric = iDepthMetric;
        metric.iMagnitude = valueInPixels;        
        valueInPixels = aVisual->MetricToPixels(THuiXYMetric(metric, metric)).iX; // Calculate metric width in pixels  
        }    
    return valueInPixels;                            
    }
    
