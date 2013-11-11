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



#include "HuiFxVg10OutlineFilter.h"
#include "HuiFxVg10RenderbufferBase.h"
#include "HuiFxConstants.h"


CHuiFxVg10OutlineFilter* CHuiFxVg10OutlineFilter::NewL()
    {
    CHuiFxVg10OutlineFilter* self = new (ELeave) CHuiFxVg10OutlineFilter();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CHuiFxVg10OutlineFilter::ConstructL()
    {
    CHuiFxFilter::ConstructL();
    iOutlineX = 1.0;
    iOutlineY = 1.0;
    iSteepness = 20.0;
    iColor = TRgb(0xff000000);
    memset(iLUT, '\0', sizeof(iLUT));
    
    RegisterParameterL(KLitOutlineX, &iOutlineX);
    RegisterParameterL(KLitOutlineY, &iOutlineY);
    RegisterParameterL(KLitSteepness, &iSteepness);
    RegisterParameterL(KLitColor, &iColor);
    }
CHuiFxVg10OutlineFilter *CHuiFxVg10OutlineFilter::CloneL() const
    {
    CHuiFxVg10OutlineFilter *filter = new(ELeave) CHuiFxVg10OutlineFilter;
    filter->CHuiFxFilter::CopyFromL(this);
    filter->iOutlineX = iOutlineX;
    filter->iOutlineY = iOutlineY;
    filter->iSteepness = iSteepness;
    filter->iColor = iColor;
    memcpy(filter->iLUT, iLUT, sizeof(iLUT));
    filter->CopyParameterL(KLitOutlineX, &filter->iOutlineX, this);
    filter->CopyParameterL(KLitOutlineY, &filter->iOutlineY, this);
    filter->CopyParameterL(KLitSteepness, &filter->iSteepness, this);
    filter->CopyParameterL(KLitColor, &filter->iColor, this);
    return filter;    
    }

void CHuiFxVg10OutlineFilter::CalculateMargin(TMargins &aMargin) const
{
    aMargin.iLeft = iOutlineX*2; 
    aMargin.iRight = iOutlineX*2;
    aMargin.iTop = iOutlineY*2;
    aMargin.iBottom = iOutlineY*2;
}


void CHuiFxVg10OutlineFilter::DrawEffect(CHuiFxEngine& aEngine, VGImage aTargetImage, VGImage aSourceImage, TInt aWidth, TInt aHeight, TBool /*aHasSurface*/)
    {
    VGfloat outline_x = clamp(iOutlineX, EPSILON, 128.0f);
    VGfloat outline_y = clamp(iOutlineY, EPSILON, 128.0f);
    VGubyte steepness = clamp((VGubyte)iSteepness, (VGubyte)0, (VGubyte)255);

//    VGint width = vgGetParameteri(aSourceImage, VG_IMAGE_WIDTH);
//    VGint height = vgGetParameteri(aSourceImage, VG_IMAGE_HEIGHT);
    VGint width = aWidth;
    VGint height = aHeight;

    if(outline_x > EPSILON && outline_y > EPSILON) 
        {
        CHuiFxRenderbuffer* auxBuffer = aEngine.AcquireRenderbuffer(TSize(width, height));
        if (!auxBuffer)
            {
            return;  // not enough memory --- filter not completed successfully
            }
        auxBuffer->BindAsTexture(ERenderbufferUsageReadWrite);
        
        VGImage auxImage = reinterpret_cast<CHuiFxVg10RenderbufferBase*>(auxBuffer)->Image();
        
        vgGaussianBlur(auxImage, aSourceImage, outline_x, outline_y, VG_TILE_PAD);

        if(ParameterChanged())
            {
            UpdateLUT();
            }
        vgLookupSingle(aTargetImage, auxImage, iLUT, VG_ALPHA, VG_TRUE, VG_FALSE);
        auxBuffer->UnbindAsTexture();
        aEngine.ReleaseRenderbuffer(auxBuffer);
        }
    else    // fast path --- just copy
        {
        vgCopyImage(aTargetImage, 0, 0, aSourceImage, 0, 0, width, height, VG_FALSE);   
        }
    }


void CHuiFxVg10OutlineFilter::UpdateLUT()
    {
    VGubyte red       = iColor.Red();
    VGubyte green     = iColor.Green();
    VGubyte blue      = iColor.Blue();
    VGubyte steepness = (VGubyte)clamp(iSteepness, 0.0f, 255.0f);
    VGubyte unnormalisedOpacity = (VGubyte)(clamp(iOpacity, 0.0f, 1.0f) * 255.0f);
    
    // generate ramp for alpha channel. the bigger the steepness the more solid the outline. 
    for(int i = 0; i < 256; i++) 
        {
        VGubyte alpha = (i*steepness > unnormalisedOpacity) ? unnormalisedOpacity : i*steepness;
        iLUT[i] = 0x00000000 | (red << 24) |(green << 16) | (blue << 8) | alpha;
        }
    }
