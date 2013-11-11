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
* Description:   Implementation of CHuiBrush. Brushes augment visuals with 
*                graphical effects.
*
*/



#include "uiacceltk/HuiBrush.h"
#include "uiacceltk/HuiGc.h"
#include "uiacceltk/HuiVisual.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiProbe.h"
#include "uiacceltk/HuiUtil.h"


EXPORT_C CHuiBrush::CHuiBrush()
        : iOpacity(1.0f), iEnabled(ETrue), iChanged(EFalse) 
    {
    HUI_PROBE_ASSOCIATE_WITH_CURRENT_SESSION
    HUI_PROBE_REPORT_CONSTRUCTED
    }


EXPORT_C CHuiBrush::~CHuiBrush()
    {
    iGuideVisual = NULL;
    
    HUI_PROBE_REPORT_DESTRUCTED
    }
    
    
EXPORT_C void CHuiBrush::ExpandVisualRect(TRect& /*aRect*/) const
    {
    // The brush does not expand the rectangle by default.
    }
    
    
EXPORT_C void CHuiBrush::SetChanged()
    {
    iChanged = ETrue;
    }


EXPORT_C TBool CHuiBrush::Changed() const
    {
    if ( iChanged )
        {
        return ETrue;
        }
    
    return iOpacity.Changed();
    }
    
EXPORT_C void CHuiBrush::ClearChanged()
    {
    iChanged = EFalse;
    iOpacity.ClearChanged();
    }


EXPORT_C void CHuiBrush::SetClipToVisual(TBool aClipToVisual)
    {
    iClipToVisual = aClipToVisual;
    }
    
    
EXPORT_C TBool CHuiBrush::ClipToVisual() const
    {
    return iClipToVisual;
    }
    

EXPORT_C void CHuiBrush::SetLayer(THuiBrushLayer aLayer)
    {
    iLayer = aLayer;
    }
    

EXPORT_C THuiBrushLayer CHuiBrush::Layer() const
    {
    return iLayer;
    }
    
    
EXPORT_C void CHuiBrush::BeginDraw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const
    {
    if(iClipToVisual)
        {
        // Visual content clipping is affected by transformation.
        aGc.PushClip();
        aGc.Clip(aGuide.BrushRect());
        }
    }


EXPORT_C void CHuiBrush::Draw(CHuiGc& /*aGc*/, const MHuiBrushGuide& /*aGuide*/) const
    {
    }


EXPORT_C void CHuiBrush::EndDraw(CHuiGc& aGc, const MHuiBrushGuide& /*aGuide*/) const
    {
    if(iClipToVisual)
        {
        // Visual content clipping is affected by transformation.
        aGc.PopClip();
        }

    iChanged = EFalse;
    }


HUI_SESSION_OBJECT_IMPL_EXPORT(CHuiBrush, ETypeBrush)

EXPORT_C void CHuiBrush::BrushExtension(const TUid& /*aExtensionUid*/, TAny** /*aExtensionParameters*/)
    {
    }
    
void CHuiBrush::ActivateBrushGuide(const CHuiVisual* aNewGuide) const
    {
    iGuideVisual = const_cast<CHuiVisual*>(aNewGuide);     
    }

CHuiVisual* CHuiBrush::BrushGuide() const
    {
    return iGuideVisual;     
    }
                                                         
