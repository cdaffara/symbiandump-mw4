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



#include "HuiFxVg10ColorizeFilter.h"
#include "HuiFxConstants.h"

CHuiFxVg10ColorizeFilter* CHuiFxVg10ColorizeFilter::NewL()
    {
    CHuiFxVg10ColorizeFilter* e = new (ELeave) CHuiFxVg10ColorizeFilter();
    CleanupStack::PushL(e);
    e->ConstructL();
    CleanupStack::Pop(e);
    return e;
    }

void CHuiFxVg10ColorizeFilter::ConstructL()
    {
    CHuiFxVg10ColorMatrixFilterBase::ConstructL();
    iColor = TRgb(0xffffffff);
    RegisterParameterL(KLitColor, &iColor);
    }
CHuiFxVg10ColorizeFilter *CHuiFxVg10ColorizeFilter::CloneL() const
    {
    CHuiFxVg10ColorizeFilter *filter = new(ELeave)CHuiFxVg10ColorizeFilter;
    filter->CHuiFxVg10ColorMatrixFilterBase::CopyFromL(this);
    filter->iColor = iColor;
    filter->CopyParameterL(KLitColor, &filter->iColor, this);
    return filter;
    }

void CHuiFxVg10ColorizeFilter::UpdateColorMatrix(void)
    {
    const VGfloat o = clamp(iOpacity, 0.0f, 1.0f);
    const VGfloat ao = 1 - o;
    const VGfloat Rw = 0.3086f;
    const VGfloat Gw = 0.6094f;
    const VGfloat Bw = 0.0820f;
    const VGfloat R = (o / 255.0f) * (VGfloat)iColor.Red();
    const VGfloat G = (o / 255.0f) * (VGfloat)iColor.Green();
    const VGfloat B = (o / 255.0f) * (VGfloat)iColor.Blue();
    
    // this matrix is ((I * (1 - opacity)) + (I * opacity) * RGBMatrix * desaturateMatrix)T
    // i.e. pixel is first converted to luminance, then multiplied by RGB, multiplied by opacity and finally the diagonal
    // compensated with 1 - opacity.
    // Precalculating abovementioned matrices to single matrix saves over a magnitude of multiplications and
    // almost two magnitudes of additions. Also less memory is used and no unnecessary copying is involved.
    iColorMatrix[0] = R*Rw + ao;
    iColorMatrix[1] = G*Rw;
    iColorMatrix[2] = B*Rw;
    iColorMatrix[3] = 0.0f;
    iColorMatrix[4] = R*Gw;
    iColorMatrix[5] = G*Gw + ao;
    iColorMatrix[6] = B*Gw;
    iColorMatrix[7] = 0.0f;
    iColorMatrix[8] = R*Bw;
    iColorMatrix[9] = G*Bw;
    iColorMatrix[10] = B*Bw + ao;
    iColorMatrix[11] = 0.0f;
    iColorMatrix[12] = 0.0f;
    iColorMatrix[13] = 0.0f;
    iColorMatrix[14] = 0.0f;
    iColorMatrix[15] = 1.0f;
    iColorMatrix[16] = 0.0f;
    iColorMatrix[17] = 0.0f;
    iColorMatrix[18] = 0.0f;
    iColorMatrix[19] = 0.0f;    
    }
