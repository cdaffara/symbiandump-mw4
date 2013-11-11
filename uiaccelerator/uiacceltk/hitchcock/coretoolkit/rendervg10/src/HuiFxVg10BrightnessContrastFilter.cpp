/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   
*
*/



#include "HuiFxVg10BrightnessContrastFilter.h"
#include "HuiFxConstants.h"

CHuiFxVg10BrightnessContrastFilter* CHuiFxVg10BrightnessContrastFilter::NewL()
    {
    CHuiFxVg10BrightnessContrastFilter* e = new (ELeave) CHuiFxVg10BrightnessContrastFilter();
    CleanupStack::PushL(e);
    e->ConstructL();
    CleanupStack::Pop(e);
    return e;
    }

void CHuiFxVg10BrightnessContrastFilter::ConstructL()
    {
    CHuiFxVg10ColorMatrixFilterBase::ConstructL();
    // neutral settings
    iBrightness = 0.0f;
    iContrast = 1.0f;
    RegisterParameterL(KLitBrightness, &iBrightness);
    RegisterParameterL(KLitContrast, &iContrast);
    }
CHuiFxVg10BrightnessContrastFilter *CHuiFxVg10BrightnessContrastFilter::CloneL() const
    {
    CHuiFxVg10BrightnessContrastFilter *filter = new(ELeave) CHuiFxVg10BrightnessContrastFilter;
    filter->CHuiFxVg10ColorMatrixFilterBase::CopyFromL(this);
    filter->iBrightness = iBrightness;
    filter->iContrast = iContrast;
    filter->CopyParameterL(KLitBrightness, &filter->iBrightness, this);
    filter->CopyParameterL(KLitContrast, &filter->iContrast, this);    
    return filter;
    }

void CHuiFxVg10BrightnessContrastFilter::UpdateColorMatrix(void)
    {
    // brightness [-1, 1]
    const VGfloat offset_br = clamp(iBrightness, -1.0f, 1.0f);
    const VGfloat scale_br = 1.0f - 0.5 * ((offset_br < 0.0f) ? -offset_br : offset_br);
    
    // contrast [0, N]
    const VGfloat scale_con = clamp(iContrast, 0.0f, 100.0f);
    const VGfloat offset_con = -0.5f * scale_con + 0.5f ;
    
    // combine the effects of brightness and contrast
    const VGfloat off = offset_br + offset_con;
    const VGfloat sc  = scale_br * scale_con;

    // take opacity into account
    const VGfloat opacity = clamp(iOpacity, 0.0f, 1.0f);
    const VGfloat oOff = off * opacity;
    const VGfloat oSc  = (sc * opacity) + (1.0f - opacity);
    
    iColorMatrix[0] = oSc;
    iColorMatrix[1] = 0.0f;
    iColorMatrix[2] = 0.0f;
    iColorMatrix[3] = 0.0f;
    iColorMatrix[4] = 0.0f;
    iColorMatrix[5] = oSc;
    iColorMatrix[6] = 0.0f;
    iColorMatrix[7] = 0.0f;
    iColorMatrix[8] = 0.0f;
    iColorMatrix[9] = 0.0f;
    iColorMatrix[10] = oSc;
    iColorMatrix[11] = 0.0f;
    iColorMatrix[12] = 0.0f;
    iColorMatrix[13] = 0.0f;
    iColorMatrix[14] = 0.0f;
    iColorMatrix[15] = 1.0f;
    iColorMatrix[16] = oOff;
    iColorMatrix[17] = oOff;
    iColorMatrix[18] = oOff;
    iColorMatrix[19] = 0.0f; 
    }
