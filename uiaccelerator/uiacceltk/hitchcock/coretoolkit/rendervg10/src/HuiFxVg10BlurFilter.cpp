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



#include "HuiFxVg10BlurFilter.h"
#include "HuiFxVg10RenderbufferBase.h"
#include "HuiFxConstants.h"

// identity mapping of colour components (i.e. 0->0, 1->1, ..., 255->255)
const VGubyte CHuiFxVg10BlurFilter::iColorLUT[256] = 
    {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF}; 

CHuiFxVg10BlurFilter* CHuiFxVg10BlurFilter::NewL()
    {
    CHuiFxVg10BlurFilter* self = new (ELeave) CHuiFxVg10BlurFilter();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CHuiFxVg10BlurFilter::ConstructL()
    {
    CHuiFxFilter::ConstructL();
    iOldOpacity = -1.0f;        // value out of range to force update of AlphaLUT
    iBlurX = 0.0;
    iBlurY = 0.0;

    memset(iAlphaLUT, '\0', sizeof(iAlphaLUT));
    
    RegisterParameterL(KLitBlurX, &iBlurX);
    RegisterParameterL(KLitBlurY, &iBlurY);
    }
CHuiFxVg10BlurFilter *CHuiFxVg10BlurFilter::CloneL() const
    {
    CHuiFxVg10BlurFilter *filter = new (ELeave) CHuiFxVg10BlurFilter;
    filter->CHuiFxFilter::CopyFromL(this);
    filter->iOldOpacity = iOldOpacity;
    filter->iBlurX = iBlurX;
    filter->iBlurY = iBlurY;
    memcpy(filter->iAlphaLUT, iAlphaLUT, sizeof(iAlphaLUT));
    filter->CopyParameterL(KLitBlurX, &filter->iBlurX, this);
    filter->CopyParameterL(KLitBlurY, &filter->iBlurY, this);
    return filter;
    }


void CHuiFxVg10BlurFilter::CalculateMargin(TMargins &aMargin) const
{
    aMargin.iLeft = iBlurX;
    aMargin.iRight = iBlurX;
    aMargin.iBottom = iBlurY;
    aMargin.iTop = iBlurY;
}

TBool CHuiFxVg10BlurFilter::Draw(CHuiFxEngine& aEngine, CHuiGc& /* aGc */, CHuiFxRenderbuffer& aTarget,
                                 CHuiFxRenderbuffer& aSource, const TRect& aTargetRect, const TRect& aSourceRect, TBool aHasSurface)
    {
    aTarget.BindAsTexture(ERenderbufferUsageWriteOnly);
    aSource.BindAsTexture(ERenderbufferUsageReadOnly);
    VGImage srcImage  = (reinterpret_cast<CHuiFxVg10RenderbufferBase*>(&aSource))->AcquireSubImage(aSourceRect);
    VGImage destImage = (reinterpret_cast<CHuiFxVg10RenderbufferBase*>(&aTarget))->AcquireSubImage(aTargetRect);
    
    // take opacity into account
    const VGfloat opacity = clamp(iOpacity, 0.0f, 1.0f);

    if(opacity > EPSILON)
        {
        DrawEffect(aEngine, destImage, srcImage, aSourceRect.Width(), aSourceRect.Height(), aHasSurface);
        }
    else
        {
//        VGint width = vgGetParameteri(srcImage, VG_IMAGE_WIDTH);
//        VGint height = vgGetParameteri(srcImage, VG_IMAGE_HEIGHT);
        VGint width = aSourceRect.Width();
        VGint height = aSourceRect.Height();
        vgClearImage(destImage, 0, 0, width, height);
        }

    HUIFX_VG_INVARIANT();    
    (reinterpret_cast<CHuiFxVg10RenderbufferBase*>(&aSource))->ReleaseSubImage(srcImage);
    (reinterpret_cast<CHuiFxVg10RenderbufferBase*>(&aTarget))->ReleaseSubImage(destImage);
    aTarget.UnbindAsTexture();
    aSource.UnbindAsTexture();
    
    return wasEnoughMemory();
    }

void CHuiFxVg10BlurFilter::DrawEffect(CHuiFxEngine& aEngine, VGImage aTargetImage, VGImage aSourceImage, TInt aWidth, TInt aHeight, TBool /*aHasSurface*/)
    {
    const VGfloat blur_x = clamp(iBlurX, EPSILON, 128.0f);
    const VGfloat blur_y = clamp(iBlurY, EPSILON, 128.0f);
    const VGfloat opacity = clamp(iOpacity, 0.0f, 1.0f);
    
    if(opacity < 1.0f - EPSILON)    // slow path --- take opacity into account
        {
//        const VGint width = vgGetParameteri(aSourceImage, VG_IMAGE_WIDTH);
//        const VGint height = vgGetParameteri(aSourceImage, VG_IMAGE_HEIGHT);
        const VGint width = aWidth;
        const VGint height = aHeight;
    
        CHuiFxRenderbuffer* auxBuffer = aEngine.AcquireRenderbuffer(TSize(width, height));
        if (!auxBuffer)
            {
            return;  // not enough memory --- filter not completed successfully
            }
        auxBuffer->BindAsTexture(ERenderbufferUsageReadWrite);
        
        VGImage auxImage = reinterpret_cast<CHuiFxVg10RenderbufferBase*>(auxBuffer)->Image();
        
        vgGaussianBlur(auxImage, aSourceImage, blur_x, blur_y, VG_TILE_PAD);
    
        if(OpacityChanged())
            {
            UpdateAlphaLUT();
            }
        vgLookup(aTargetImage, auxImage, iColorLUT, iColorLUT, iColorLUT, iAlphaLUT, VG_TRUE, VG_FALSE);
        auxBuffer->UnbindAsTexture();
        aEngine.ReleaseRenderbuffer(auxBuffer);
    }    
    else    // fast path --- the blur, just blur and nothing but the blur.
        {
        vgGaussianBlur(aTargetImage, aSourceImage, blur_x, blur_y, VG_TILE_PAD);   
        }
    }


TBool CHuiFxVg10BlurFilter::OpacityChanged()
    {
    if(iOpacity != iOldOpacity)
        {
        iOldOpacity = iOpacity;
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }


void CHuiFxVg10BlurFilter::UpdateAlphaLUT()
    {
    VGfloat opacity = clamp(iOpacity, 0.0f, 1.0f);
    
    // generate ramp for alpha channel. the bigger the steepness the more solid the outline. 
    for(int i = 0; i < 256; i++) 
        {
        iAlphaLUT[i] = (VGubyte)((VGfloat)i * opacity); 
        }
    }
