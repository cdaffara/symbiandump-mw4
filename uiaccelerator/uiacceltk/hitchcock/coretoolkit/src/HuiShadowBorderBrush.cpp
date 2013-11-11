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
* Description:   Implementation of CHuiShadowBorderBrush - Shadow borders for visuals.
*
*/



#include "uiacceltk/HuiShadowBorderBrush.h"  // Class definition
#include "uiacceltk/HuiDrawing.h"
#include "uiacceltk/HuiVisual.h"
#include "uiacceltk/HuiUtil.h"

/** Default shadow opacity. */
const TReal32 KHuiShadowBorderBrushOpacity = 100.f/255.f;


EXPORT_C CHuiShadowBorderBrush* CHuiShadowBorderBrush::NewL(TInt aWidth)
    {
    CHuiShadowBorderBrush* self = NewLC(aWidth);
    CleanupStack::Pop();
    return self;
    }


EXPORT_C CHuiShadowBorderBrush* CHuiShadowBorderBrush::NewLC(TInt aWidth)
    {
    CHuiShadowBorderBrush* self = new (ELeave) CHuiShadowBorderBrush(aWidth);
    CleanupStack::PushL(self);
    return self;
    }

EXPORT_C CHuiShadowBorderBrush* CHuiShadowBorderBrush::NewL(const THuiMetric& aWidth)
    {
    CHuiShadowBorderBrush* self = NewLC(aWidth);
    CleanupStack::Pop();
    return self;
    }


EXPORT_C CHuiShadowBorderBrush* CHuiShadowBorderBrush::NewLC(const THuiMetric& aWidth)
    {
    CHuiShadowBorderBrush* self = new (ELeave) CHuiShadowBorderBrush(aWidth);
    CleanupStack::PushL(self);
    return self;
    }
 

CHuiShadowBorderBrush::CHuiShadowBorderBrush(TInt aWidth)
        : CHuiBrush(),
          iWidth(TReal32(aWidth))
    {
    SetClipToVisual(EFalse);
    iOpacity.Set(KHuiShadowBorderBrushOpacity);
    }

CHuiShadowBorderBrush::CHuiShadowBorderBrush(const THuiMetric& aWidth)
        : CHuiBrush(),
          iWidth(aWidth.iMagnitude),
          iMetricWidth(aWidth)
    {    
    SetClipToVisual(EFalse);
    iOpacity.Set(KHuiShadowBorderBrushOpacity);
    }

CHuiShadowBorderBrush::~CHuiShadowBorderBrush()
    {
    }


void CHuiShadowBorderBrush::ExpandVisualRect(TRect& aRect) const
    {
    // We need access to visual to convert metric units into pixels,
    // currently a pointer to visual is provided through separate
    // method since we want to avoid breaking binary compatibility 
    // of this class.
    TReal32 widthInPixels = WidthInPixels(BrushGuide()); 
    aRect.Grow(HUI_ROUND_FLOAT_TO_INT(widthInPixels), HUI_ROUND_FLOAT_TO_INT(widthInPixels)); 
    }
    
TBool CHuiShadowBorderBrush::Changed() const
    {
    if ( CHuiBrush::Changed() )
        {
        return ETrue;
        }
    
    return iWidth.Changed();
    }

void CHuiShadowBorderBrush::ClearChanged()
    {
    CHuiBrush::ClearChanged();
    iWidth.ClearChanged();
    }

void CHuiShadowBorderBrush::Draw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const
    {
    TRect content = aGuide.BrushRect().Round();
    TReal32 opacity = aGuide.BrushOpacity() * iOpacity.Now();

    if(opacity <= 0)
        {
        return;
        }

    aGc.SetPenAlpha(TInt(opacity * 255));
    aGc.SetPenColor(TRgb(0, 0, 30));

    const CHuiTexture* shadowTexture = NULL;
    TInt err = aGuide.BrushSkin().GetTexture(EHuiSkinShadowTexture, shadowTexture);
    if (err!=KErrNone)
    	{
    	ASSERT(EFalse); // failed to get the shadow texture - unable to proceed!
    	return;
	    }
    ASSERT(shadowTexture);
    THuiImage shadowImage(*shadowTexture);

    aGc.Disable(CHuiGc::EFeatureDepthWrite);
    
    TReal32 widthInPixels = WidthInPixels(BrushGuide()); 
    content.Grow(HUI_ROUND_FLOAT_TO_INT(widthInPixels), HUI_ROUND_FLOAT_TO_INT(widthInPixels)); 
    
    // ID:  EHJK-7G5AHB - shadow border leaves artifacts in the display while visual is moved.
    TReal32 offset = 0.0f; // NOTE: It was 1.f. 

    // Note: DrawBorders does not (yet) support different widths for every border.
    aGc.DrawBorders(content, 
                    widthInPixels-offset, 
                    widthInPixels+offset, 
                    widthInPixels-offset, 
                    widthInPixels+offset, 
                    CHuiGc::EBorderImage, 
                    &shadowImage);            

    aGc.Enable(CHuiGc::EFeatureDepthWrite);
    }
    
EXPORT_C void CHuiShadowBorderBrush::SetWidth(const THuiMetric& aWidth)
    {
    iMetricWidth = aWidth;
    iWidth.Set(aWidth.iMagnitude);      
    }


TReal32 CHuiShadowBorderBrush::WidthInPixels(const CHuiVisual* aVisual) const
    {
    TReal32 widthInPixels = iWidth.Now(); // Default to old pixel width
    if (aVisual)
        {
        THuiMetric metric = iMetricWidth;
        metric.iMagnitude = widthInPixels;        
        widthInPixels = aVisual->MetricToPixels(THuiXYMetric(metric, metric)).iX; // Calculate metric width in pixels  
        }    
    return widthInPixels;            
    }

