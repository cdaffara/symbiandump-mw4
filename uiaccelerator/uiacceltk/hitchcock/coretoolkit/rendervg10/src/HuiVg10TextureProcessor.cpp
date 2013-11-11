/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of CHuiVg10TextureProcessor.
*                CHuiVg10TextureProcessor is a concrete implementation of a texture
*                processor for OpenVG 1.0
*
*/



#include "HuiVg10TextureProcessor.h"  // Class definition
#include "HuiVg10PBufferSurface.h"
#include "HuiVg10RenderPlugin.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiImage.h"
#include "HuiRenderSurface.h"

#include <VG/openvg.h>

CHuiVg10TextureProcessor* CHuiVg10TextureProcessor::NewL(CHuiEnv& aEnv)
    {
    CHuiVg10TextureProcessor* self = CHuiVg10TextureProcessor::NewLC(aEnv);
    CleanupStack::Pop(self);
    return self;
    }


CHuiVg10TextureProcessor* CHuiVg10TextureProcessor::NewLC(CHuiEnv& aEnv)
    {
    CHuiVg10TextureProcessor* self = new (ELeave) CHuiVg10TextureProcessor(aEnv);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


CHuiVg10TextureProcessor::CHuiVg10TextureProcessor(CHuiEnv& aEnv)
        : CHuiTextureProcessor(aEnv)
    {
    }


void CHuiVg10TextureProcessor::ConstructL()
    {
    }


CHuiVg10TextureProcessor::~CHuiVg10TextureProcessor()
    {
    }


void CHuiVg10TextureProcessor::BlurL(const THuiTextureHandle& aSrc, THuiTextureHandle& aDest,
                                     TInt aFilterSize, TInt aFlags)
    {
    // By default, use a quarter of the original size.
    TSize originalSize = aSrc.Size();
       
    return BlurL(aSrc, aDest,
                 TSize(originalSize.iWidth / 2, originalSize.iHeight / 2),
                 aFilterSize, aFlags);
    }


void CHuiVg10TextureProcessor::BlurL(const THuiTextureHandle& aSrc,
                                     THuiTextureHandle& aDest,
                                     const TSize& /*aPreferredSize*/,
                                     TInt aFilterSize, TInt /*aFlags*/)
    {
    if (!aSrc.HasContent())
        {
        return;
        }

    VGImage src  = (VGImage)aSrc.SegmentName(0);

    if (src == VG_INVALID_HANDLE)
        {
        return;
        }
    
    // If src and dest overlap, openvg cannot handle it. Create temp source as workaround.
    if (aSrc.Name() == aDest.Name())
        {
    	TInt width  = vgGetParameteri(src, VG_IMAGE_WIDTH);
    	TInt height = vgGetParameteri(src, VG_IMAGE_HEIGHT);
	    VGImage tempsrc = vgCreateImage(VG_sRGBA_8888_PRE, width, height, 
	                                  VG_IMAGE_QUALITY_FASTER | VG_IMAGE_QUALITY_NONANTIALIASED);
	
	    VGImage dest = (VGImage)aDest.SegmentName(0);
		if (tempsrc == VG_INVALID_HANDLE)
			{
			HUI_DEBUG(_L("CHuiVg10TextureProcessor::BlurL() - Unable to allocate tempsrc texture."));
			return;
			}		

        // Copy from dest to tempsrc
        vgCopyImage(tempsrc, 0, 0, dest, 0, 0, width, height, VG_FALSE);

        if (dest != VG_INVALID_HANDLE)
            {
            vgGaussianBlur(dest, tempsrc, aFilterSize / 3.0f, aFilterSize / 3.0f, VG_TILE_PAD);                    
            }
        
        vgDestroyImage(tempsrc);            
        return;
        }
    
    if (!aDest.SegmentCount() || !aDest.SegmentName(0))
    	{
    	// No destination texture, so we must create a new one
    	// TODO: Refactor this to a more proper place
    	TInt width  = vgGetParameteri(src, VG_IMAGE_WIDTH);
    	TInt height = vgGetParameteri(src, VG_IMAGE_HEIGHT);
	    VGImage image = vgCreateImage(VG_sRGBA_8888_PRE, width, height, 
	                                  VG_IMAGE_QUALITY_FASTER | VG_IMAGE_QUALITY_NONANTIALIASED);
	
		if (image == VG_INVALID_HANDLE)
			{
			HUI_DEBUG(_L("CHuiVg10TextureProcessor::BlurL() - Unable to allocate blurred texture."));
			return;
			}
		
		aDest.SetName(image);
		aDest.SetSize(TSize(width, height));
    	}
    
    VGImage dest = (VGImage)aDest.SegmentName(0);
    
    // TODO: EBlurFlagWhite and EBlurFlagBlack
    
    if (src == VG_INVALID_HANDLE || dest == VG_INVALID_HANDLE)
        {
        return;
        }

    vgGaussianBlur(dest, src, aFilterSize / 3.0f, aFilterSize / 3.0f, VG_TILE_PAD);
    HUI_VG_INVARIANT();
    }

void CHuiVg10TextureProcessor::BlurSegmentedL(const MHuiSegmentedTexture& aSrc,
                       THuiTextureHandle& aDest,
                       TInt aFilterSize, TInt aFlags)
{
    BlurL(aSrc.Handle(), aDest, aFilterSize, aFlags);
}
    
void CHuiVg10TextureProcessor::BlurSegmentedL(const MHuiSegmentedTexture& aSrc, 
                       THuiTextureHandle& aDest,
                       const TSize& aPreferredSize,
                       TInt aFilterSize, TInt aFlags)
{
    BlurL(aSrc.Handle(), aDest, aPreferredSize, aFilterSize, aFlags);
}

void CHuiVg10TextureProcessor::Release()
    {
    HUI_DEBUG(_L("CHuiVg10TextureProcessor::Release() - Destroying processing buffer."));
    }


void CHuiVg10TextureProcessor::RestoreL()
    {
    HUI_DEBUG(_L("CHuiVg10TextureProcessor::RestoreL() - Called."));
    }
