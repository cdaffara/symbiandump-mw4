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



#include "HuiFxVg10BevelFilter.h"
#include "HuiFxVg10RenderbufferBase.h"
#include "HuiFxConstants.h"

CHuiFxVg10BevelFilter* CHuiFxVg10BevelFilter::NewL()
    {
    CHuiFxVg10BevelFilter* self = new (ELeave) CHuiFxVg10BevelFilter();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CHuiFxVg10BevelFilter::ConstructL()
    {
    CHuiFxFilter::ConstructL();
    iOutlineX = 1.0;
    iOutlineY = 1.0;
    iSteepness = 20.0;
/*    iRed = 0.0f;
    iGreen = 0.0f;
    iBlue = 0.0f;*/
    RegisterParameterL(KLitOutlineX, &iOutlineX);
    RegisterParameterL(KLitOutlineY, &iOutlineY);
    RegisterParameterL(KLitSteepness, &iSteepness);
/*    RegisterParameterL(KLitRed, &iRed);
    RegisterParameterL(KLitGreen, &iGreen);
    RegisterParameterL(KLitBlue, &iBlue);*/
    }
CHuiFxVg10BevelFilter *CHuiFxVg10BevelFilter::CloneL() const
    {
    CHuiFxVg10BevelFilter *filter = new(ELeave)CHuiFxVg10BevelFilter;
    filter->CHuiFxFilter::CopyFromL(this);
    filter->iOutlineX = iOutlineX;
    filter->iOutlineY = iOutlineY;
    filter->iSteepness = iSteepness;
    filter->CopyParameterL(KLitOutlineX, &filter->iOutlineX, this);
    filter->CopyParameterL(KLitOutlineY, &filter->iOutlineY, this);
    filter->CopyParameterL(KLitSteepness, &filter->iSteepness, this);
    return filter;
    }

void CHuiFxVg10BevelFilter::DrawEffect(CHuiFxEngine& aEngine, VGImage aTargetImage, VGImage aSourceImage, TInt aWidth, TInt aHeight, TBool /*aHasSurface*/)
    {
    VGfloat outline_x = clamp(iOutlineX, EPSILON, 128.0f);
    VGfloat outline_y = clamp(iOutlineY, EPSILON, 128.0f);
    VGubyte steepness = (VGubyte)clamp(iSteepness, 0.0f, 255.0f);
//    VGint width = vgGetParameteri(aSourceImage, VG_IMAGE_WIDTH);
//    VGint height = vgGetParameteri(aSourceImage, VG_IMAGE_HEIGHT);
    VGint width = aWidth;
    VGint height = aHeight;

    if((outline_x > EPSILON && outline_y > EPSILON) || steepness == 0) 
        {
        // get an aux buffer
        CHuiFxRenderbuffer* aux1Buffer = aEngine.AcquireRenderbuffer(TSize(width, height));
        CHuiFxRenderbuffer* aux2Buffer = aEngine.AcquireRenderbuffer(TSize(width, height));
        if (!aux1Buffer || !aux2Buffer) // can't render the effect
            {
            return;
            }
        aux1Buffer->BindAsTexture(ERenderbufferUsageReadWrite);
        aux2Buffer->BindAsTexture(ERenderbufferUsageReadWrite);
        
        VGImage aux1Image = (reinterpret_cast<CHuiFxVg10RenderbufferBase*>(aux1Buffer)->Image());
        VGImage aux2Image = (reinterpret_cast<CHuiFxVg10RenderbufferBase*>(aux2Buffer)->Image());

        VGuint lut[256];
        int threshold = 1;
        // generate ramp for alpha channel. the bigger the steepness the more solid the outline. 
        for(int i = 0; i < threshold; i++) 
            {
            lut[i] = 0x00000000; // | i;
            }
        for(int i = threshold; i < 256; i++)
            {
            lut[i] = 0xffffffff; // | i;
            }
        vgLookupSingle(aux1Image, aSourceImage, lut, VG_ALPHA, VG_TRUE, VG_FALSE);
        
        // TODO: pixel relative parameters
        // the variables are commented out because they are not used yet
/*        
        VGint shiftX = 0;
        VGint shiftY = 0;
//        VGshort kernel[9] = {0, -1, -2, 1, 0, -1, 2, 1, 0}; 
        VGshort kernel[9] = {1, 0, -1, 2, 0, -2, 1, 0, -1};
        VGfloat scale = 0.5f;
        VGfloat bias = 0.5f;
*/        

//        vgConvolve(aux2Image, aux1Image, 3, 3, shiftX, shiftY, kernel, scale, bias, VG_TILE_PAD);

/*        for(int i = 0; i < 126; i++)
            {
            lut[i] = 0x000000ff | i << 24 | i << 16 | i << 8 | i;
            }
*/        for(int i = 0; i < 10; i++)
            {
            lut[i] = 0x00000000  | i << 24 | i << 16 | i << 8 | 255 - i * 3;
            }
        for(int i = 20; i < 255; i++)
            {
            lut[i] = 0x00000000; // | i << 24 | i << 16 | i << 8 | i;
            }

//        vgLookupSingle(aux1Image, aux2Image, lut, VG_ALPHA, VG_TRUE, VG_FALSE);

//        vgGaussianBlur(aTargetImage, aux1Image, outline_x, outline_y, VG_TILE_PAD);        
        
        aux1Buffer->UnbindAsTexture();
        aux2Buffer->UnbindAsTexture();
        aEngine.ReleaseRenderbuffer(aux1Buffer);
        aEngine.ReleaseRenderbuffer(aux2Buffer);
        }
    else    // fast path --- just copy
        {
        vgCopyImage(aTargetImage, 0, 0, aSourceImage, 0, 0, width, height, VG_FALSE);   
        }
    }
