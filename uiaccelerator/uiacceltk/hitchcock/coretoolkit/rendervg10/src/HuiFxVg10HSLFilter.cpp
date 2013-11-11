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



#include "HuiFxVg10HSLFilter.h"
#include <math.h>
#include "HuiFxConstants.h"

// local helper functions
static void getSaturationRotationMatrix(VGfloat *effectMatrix, const VGfloat opacity, const VGfloat angle, const VGfloat saturation);
static void getSaturationMatrix(VGfloat *effectMatrix, const VGfloat opacity, const VGfloat saturation);
static void getRotationMatrix(VGfloat *effectMatrix, const VGfloat opacity, const VGfloat angle);
static void getIdentityMatrix(VGfloat *effectMatrix);

CHuiFxVg10HSLFilter* CHuiFxVg10HSLFilter::NewL()
    {
    CHuiFxVg10HSLFilter* e = new (ELeave) CHuiFxVg10HSLFilter();
    CleanupStack::PushL(e);
    e->ConstructL();
    CleanupStack::Pop(e);
    return e;
    }

void CHuiFxVg10HSLFilter::ConstructL()
    {
    CHuiFxVg10ColorMatrixFilterBase::ConstructL();
    // neutral settings
    iHue            = 0.0f;    //  0.0f to 360.0f, neutral: 0.0f
    iSaturation     = 1.0f;    //  0.0f to 100.0f, neutral: 1.0f
    iLightness      = 0.0f;    // -1.0f to 1.0f, neutral: 0.0f
    RegisterParameterL(KLitHue, &iHue);
    RegisterParameterL(KLitSaturation, &iSaturation);
    RegisterParameterL(KLitLightness, &iLightness);
    }
CHuiFxVg10HSLFilter *CHuiFxVg10HSLFilter::CloneL() const
{
    CHuiFxVg10HSLFilter *filter = new (ELeave) CHuiFxVg10HSLFilter;
    filter->CHuiFxVg10ColorMatrixFilterBase::CopyFromL(this);
    filter->iHue = iHue;
    filter->iSaturation = iSaturation;
    filter->iLightness = iLightness;
    filter->CopyParameterL(KLitHue, &filter->iHue, this);
    filter->CopyParameterL(KLitSaturation, &filter->iSaturation, this);
    filter->CopyParameterL(KLitLightness, &filter->iLightness, this);
    return filter;
}

//
// Background for HSL matrices can be found at http://www.opengl.org/resources/code/samples/advanced/advanced97/notes/node137.html
//
void CHuiFxVg10HSLFilter::UpdateColorMatrix(void)
    {
    // a helpful constant
    const VGfloat radsPerDeg = 2.0f * (float)M_PI / 360.0f;

    // make sure parametres are in range
    const VGfloat opacity = clamp(iOpacity, 0.0f, 1.0f);
    const VGfloat angle = clamp(iHue * radsPerDeg, 0.0f, 2.0f * (float)M_PI);   // angle [0, 2*pi]
    const VGfloat saturation = clamp(iSaturation, 0.0f, 100.0f);                // saturation [0, N]
    const VGfloat lightness = clamp(iLightness, -1.0f, 1.0f);                   // lightness [-1, 1]
    
    // check parametres which precalculated matrix we have to use.
    // Note: lightness affects offset and not matrix so we don't bother optimising that.
    const bool enableSaturation  = (saturation < 1.0f - EPSILON || saturation > 1.0f + EPSILON);
    const bool enableHueRotation = (EPSILON < angle && angle < (2.0f * (float)M_PI - EPSILON));

    if(enableSaturation && enableHueRotation)
        {
        // contains SaturateT*PrerotationT*HuerotationT*PostrotationT*I*opacity+I*(1-opacity) matrices --- ugly, but saves lot of operations in FPU.
        // note: there are plenty of redundancy in these calculations --- let compiler optimise them.
        getSaturationRotationMatrix(&iColorMatrix[0], opacity, saturation, angle);
        }
    else if(enableSaturation && !enableHueRotation)
        {
        // saturationT*I*opacity+I*(1 - opacity) matrix without hue rotation
        getSaturationMatrix(&iColorMatrix[0], opacity, saturation);
        }
    else if(!enableSaturation && enableHueRotation)
        {
        // PrerotationT*HuerotationT*PostrotationT*I*opacity+I*(1-opacity) matrices without saturation matrix
        getRotationMatrix(&iColorMatrix[0], opacity, angle);
        }
    else
        {
        // identity matrix
        getIdentityMatrix(&iColorMatrix[0]);
        }

    // colour component offsets
    iColorMatrix[16] = lightness * opacity;
    iColorMatrix[17] = lightness * opacity;
    iColorMatrix[18] = lightness * opacity;
    iColorMatrix[19] = 0.0f;
    }


static void getSaturationRotationMatrix(VGfloat *effectMatrix, const VGfloat opacity, const VGfloat saturation, const VGfloat angle) 
    {
    const VGfloat sa = saturation;            
    const VGfloat as = 1.0f - saturation;
    
    const VGfloat o = opacity;
    const VGfloat ao = 1.0f - o;
    
    const VGfloat c = cos(angle);
    const VGfloat s = sin(angle);
    
    effectMatrix[0] = o * ((-0.02473f*as+0.66667f*sa)*c+ (0.30450f*as*s+(0.33333f*as+0.33333f*sa))) + ao;
    effectMatrix[1] = o * ((-0.02473f*as-0.33333f*sa)*c+((0.30450f*as+0.57736f*sa)*s+(0.33333f*as+0.33333f*sa)));
    effectMatrix[2] = o * ((-0.02473f*as-0.33333f*sa)*c+((0.30450f*as-0.57736f*sa)*s+(0.33333f*as+0.33333f*sa)));
    effectMatrix[3] = 0.0f;
    effectMatrix[4] = o * ((0.27607f*as-0.33333f*sa)*c+((-0.13083f*as-0.57736f*sa)*s+(0.33333f*as+0.33333f*sa)));
    effectMatrix[5] = o * ((0.27607f*as+0.66667f*sa)*c+ (-0.13083f*as*s+(0.33333f*as+0.33333f*sa))) + ao;
    effectMatrix[6] = o * ((0.27607f*as-0.33333f*sa)*c+((-0.13083f*as+0.57736f*sa)*s+(0.33333f*as+0.33333f*sa)));
    effectMatrix[7] = 0.0f;
    effectMatrix[8] = o * ((-0.25134f*as-0.33333f*sa)*c+((-0.17367f*as+0.57736f*sa)*s+(0.33333f*as+0.33333f*sa)));
    effectMatrix[9] = o * ((-0.25134f*as-0.33333f*sa)*c+((-0.17367f*as-0.57736f*sa)*s+(0.33333f*as+0.33333f*sa)));
    effectMatrix[10] = o * ((-0.25134f*as+0.66667f*sa)*c+ (-0.17367f*as*s+(0.33333f*as+0.33333f*sa))) + ao;
    effectMatrix[11] = 0.0f;
    effectMatrix[12] = 0.0f;
    effectMatrix[13] = 0.0f;
    effectMatrix[14] = 0.0f;
    effectMatrix[15] = 1.0f;
}


static void getSaturationMatrix(VGfloat *effectMatrix, const VGfloat opacity, const VGfloat saturation)
    {
    const VGfloat Rw = 0.3086f;
    const VGfloat Gw = 0.6094f;
    const VGfloat Bw = 0.0820f;
    
    const VGfloat sa = saturation;            
    const VGfloat as = 1.0f - saturation;
    
    const VGfloat o = opacity;
    const VGfloat ao = 1.0f - o;

    const VGfloat asRw = o * as * Rw;
    const VGfloat asGw = o * as * Gw;
    const VGfloat asBw = o * as * Bw;
    
    effectMatrix[0] = asRw + sa + ao;
    effectMatrix[1] = asRw;
    effectMatrix[2] = asRw;
    effectMatrix[3] = 0.0f;
    effectMatrix[4] = asGw;
    effectMatrix[5] = asGw + sa + ao;
    effectMatrix[6] = asGw;
    effectMatrix[7] = 0.0f;
    effectMatrix[8] = asBw;
    effectMatrix[9] = asBw;
    effectMatrix[10] = asBw + sa + ao;
    effectMatrix[11] = 0.0f;
    effectMatrix[12] = 0.0f;
    effectMatrix[13] = 0.0f;
    effectMatrix[14] = 0.0f;
    effectMatrix[15] = 1.0f;
    }


static void getRotationMatrix(VGfloat *effectMatrix, const VGfloat opacity, const VGfloat angle)
    {
    const VGfloat o = opacity;
    const VGfloat ao = 1.0f - o;
    
    const VGfloat c = cos(angle);
    const VGfloat s = sin(angle);
    
    effectMatrix[0] = o * ( 0.66667f*c+0.33333f) + ao;
    effectMatrix[1] = o * (-0.33333f*c+(0.57736f*s+0.33333f));
    effectMatrix[2] = o * (-0.33333f*c+(-0.57736f*s+0.33333f));
    effectMatrix[3] =  0.0f;
    effectMatrix[4] = o * (-0.33333f*c+(-0.57736f*s+0.33333f));
    effectMatrix[5] = o * ( 0.66667f*c+0.33333f) + ao;
    effectMatrix[6] = o * (-0.33333f*c+(0.57736f*s+0.33333f));
    effectMatrix[7] =  0.0f;
    effectMatrix[8] = o * (-0.33333f*c+(0.57736f*s+0.33333f));
    effectMatrix[9] = o * (-0.33333f*c+(-0.57736f*s+0.33333f));
    effectMatrix[10] = o * (0.66667f*c+0.33333f) + ao;
    effectMatrix[11] = 0.0f;
    effectMatrix[12] = 0.0f;
    effectMatrix[13] = 0.0f;
    effectMatrix[14] = 0.0f;
    effectMatrix[15] = 1.0f;
    }


static void getIdentityMatrix(VGfloat *effectMatrix)
    {
    effectMatrix[0] = 1.0f;
    effectMatrix[1] = 0.0f;
    effectMatrix[2] = 0.0f;
    effectMatrix[3] = 0.0f;
    effectMatrix[4] = 0.0f;
    effectMatrix[5] = 1.0f;
    effectMatrix[6] = 0.0f;
    effectMatrix[7] = 0.0f;
    effectMatrix[8] = 0.0f;
    effectMatrix[9] = 0.0f;
    effectMatrix[10] = 1.0f;
    effectMatrix[11] = 0.0f;
    effectMatrix[12] = 0.0f;
    effectMatrix[13] = 0.0f;
    effectMatrix[14] = 0.0f;
    effectMatrix[15] = 1.0f;
    }
