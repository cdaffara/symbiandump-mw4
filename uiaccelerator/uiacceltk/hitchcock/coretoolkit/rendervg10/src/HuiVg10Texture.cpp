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
* Description:   Implementation for CHuiVg10Texture, OpenVG 1.0 Texture.
*
*/



#include <e32def.h>
#include <e32math.h>

#include "HuiVg10RenderPlugin.h"
#include "HuiVg10Texture.h"
#include "HuiVg10TextureManager.h"
#include "uiacceltk/HuiTextureHandle.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiTextureManager.h"
#include "uiacceltk/HuiTextureProcessor.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiPanic.h"
#include "appiconcache.h"

#include "huiextension.h"

// temporary hack until the openvg headers are fixed..
#ifndef OPENVG_VERSION_1_0_1
    #warning using temporary hack to define OPENVG_VERSION_1_0_1, see TSW: SKYA-7QQB8
    #define OPENVG_VERSION_1_0_1
#endif

#ifdef __NVG
    #include <EGL/egl.h>
    #include <nvg.h>
    #include <AknIconUtils.h>
    #include "HuiRenderSurface.h"
    #include "NVGIcon.h"
#endif


/**
 *  Bitmap color mode conversion.
 *
 *  @param aBitmap      Source bitmap to convert.
 *  @param aDisplayMode Target display mode.
 *  @param aCopyMode    Desired copy behavior.
 *  @returns a new bitmap with the desired display mode.
 */
static CFbsBitmap& ConvertBitmapL(const CFbsBitmap& aBitmap, TDisplayMode aDisplayMode, TBitmapCopyMode aCopyMode = EAllowDuplication, TSize aNewSize = TSize(0,0))
    {
    // Functionality moved to Texture Manager 
    // (to avoid recreating the bitmaps & bmpDevices every time)
    CHuiTextureManager& tmpMgr = CHuiStatic::Env().TextureManager();
    CHuiVg10TextureManager& textureMgr = static_cast<CHuiVg10TextureManager&>(tmpMgr);
    
    return textureMgr.ConvertBitmapL(aBitmap, aDisplayMode, aCopyMode, aNewSize);
    }

/**
 *  Replace the alpha channel of an EColor16MA bitmap (in-place).
 * 
 *  @param aBitmap      Destination bitmap.
 *  @param aMaskBitmap  Alpha channel source bitmap.
 */
static void ReplaceAlphaChannelL(CFbsBitmap& aBitmap, const CFbsBitmap& aMaskBitmap)
    {
    ASSERT(aBitmap.DisplayMode() == EColor16MA);
    ASSERT(aBitmap.SizeInPixels() == aMaskBitmap.SizeInPixels());
    
    CFbsBitmap& convMask = ConvertBitmapL(aMaskBitmap, EGray256);
    
    aBitmap.BeginDataAccess();
    convMask.BeginDataAccess();
    TSize size           = aBitmap.SizeInPixels();
    TUint32* dest32      = (TUint32*)aBitmap.DataAddress();
    const TUint8* src8   = (const TUint8*)convMask.DataAddress();
    TInt destStride      = CFbsBitmap::ScanLineLength(size.iWidth, aBitmap.DisplayMode()) >> 2;
    TInt srcStride       = CFbsBitmap::ScanLineLength(size.iWidth, convMask.DisplayMode());
    
    for (TInt y = 0; y < size.iHeight; y++)
        {
        TUint32* d      = dest32;
        const TUint8* s = src8;

        for (TInt x = 0; x < size.iWidth; x++)
            {
            *d   &= ~0xff000000;
            *d++ |= (*s++) << 24;
            }
        
        dest32 += destStride;
        src8   += srcStride;
        }

    aBitmap.EndDataAccess();
    convMask.EndDataAccess(ETrue);
    }

#if !defined(OPENVG_VERSION_1_0_1)
/**
 *  Convert a native Symbian EColor16MU/A bitmap to an OpenVG 1.0 equivalent (in-place).
 * 
 *  @param aBitmap      Bitmap to convert.
 */
static void ConvertPixelComponentOrdering(CFbsBitmap& aBitmap)
    {
    ASSERT(aBitmap.DisplayMode() == EColor16MA || aBitmap.DisplayMode() == EColor16MU);
    
    aBitmap.BeginDataAccess();
    TSize size           = aBitmap.SizeInPixels();
    TUint32* dest32      = (TUint32*)aBitmap.DataAddress();
    TInt destStride      = CFbsBitmap::ScanLineLength(size.iWidth, aBitmap.DisplayMode()) >> 2;
    
    for (TInt y = 0; y < size.iHeight; y++)
        {
        TUint32* d = dest32;

        for (TInt x = 0; x < size.iWidth; x++)
            {
            *d++ = (*d << 8) | ((*d & 0xff000000) >> 24);
            }
        
        dest32 += destStride;
        }
    aBitmap.EndDataAccess();
    }
#endif

/* Vg10Texture internal flags. */
enum
    {
    /** Content has been uploaded. */
    EFlagHasContent = 0x1,

    /** The texture has an alpha channel. */
    EFlagHasAlpha = 0x2
    };


CHuiVg10Texture* CHuiVg10Texture::NewL(CHuiVg10RenderPlugin& aRenderPlugin, const THuiTextureHandle* aExistingTexture)
    {
    CHuiVg10Texture* self = CHuiVg10Texture::NewLC(aRenderPlugin, aExistingTexture);
    CleanupStack::Pop(self);
    return self;
    }


CHuiVg10Texture* CHuiVg10Texture::NewLC(CHuiVg10RenderPlugin& aRenderPlugin, const THuiTextureHandle* aExistingTexture)
    {
    CHuiVg10Texture* self = new (ELeave) CHuiVg10Texture(aRenderPlugin);
    CleanupStack::PushL(self);
    self->ConstructL(aExistingTexture);
    return self;
    }


CHuiVg10Texture::CHuiVg10Texture(CHuiVg10RenderPlugin& aRenderPlugin)
    : iRenderPlugin(aRenderPlugin),
      iInternalFlags(0),
      iShadowEnabled(EFalse)
    {
    }


void CHuiVg10Texture::ConstructL(const THuiTextureHandle* aExistingTexture)
    {
    CHuiTexture::BaseConstructL();
    
    SetSegmentCountL(1);
    SetSegmentName(0, 0);
    SetSegmentSize(0, TSize(0, 0));
    SetSegmentTextureSize(0, TSize(0, 0));

    if(aExistingTexture)
        {
        // Take over the existing texture (single named texture object).
        ASSERT(aExistingTexture->SegmentCount() == 1);
        ASSERT(SegmentCount() == 1);
        SetSegmentName(0, aExistingTexture->Name());
        SetSegmentSize(0, aExistingTexture->SegmentSize(0));
        SetSegmentTextureSize(0, aExistingTexture->SegmentTextureSize(0));
        SetSize(aExistingTexture->Size());
        
        // Copy attributes
        const CHuiVg10Texture* vgTex = reinterpret_cast<const CHuiVg10Texture*>(aExistingTexture);
        iInternalFlags = vgTex->iInternalFlags;
        // Note: shared shadows not supported
        }
    }


CHuiVg10Texture::~CHuiVg10Texture()
    {
    Reset();
    }


TBool CHuiVg10Texture::HasAlpha() const
    {
    return (iInternalFlags & EFlagHasAlpha) != 0;
    }
    
    
TBool CHuiVg10Texture::HasContent() const
    {
    if(!CHuiTexture::HasContent())
        {
        return EFalse;
        }
        
    return (iInternalFlags & EFlagHasContent) != 0;
    }    


TSize CHuiVg10Texture::MaxTextureSize() const
    {
    if (iRenderPlugin.IsReleased())
        {
        HUI_DEBUG(_L("CHuiVg10Texture::MaxTextureSize() - Vg10 context not available, cannot get maximum texture size, returning (0,0)."));
        return TSize(0,0);
        }
    
    int maxWidth  = vgGeti(VG_MAX_IMAGE_WIDTH)-1;
    int maxHeight = vgGeti(VG_MAX_IMAGE_HEIGHT)-1;
    HUI_VG_INVARIANT();
    ASSERT(maxWidth > 0 && maxHeight > 0);
    
    return TSize(maxWidth, maxHeight);
    }

void CHuiVg10Texture::UploadL(const CFbsBitmap& aBitmap,
                              const CFbsBitmap* aMaskBitmap,
                              THuiTextureUploadFlags aTextureFlags)
    {
    Reset(); // Must delete old content before calling SetupSegmentsL
    
    TBool invalidMaskSize = (aMaskBitmap && (aBitmap.SizeInPixels() != aMaskBitmap->SizeInPixels()));
    
    CFbsBitmap* maskBitmap = (CFbsBitmap*)aMaskBitmap;

    if (invalidMaskSize)
        {
        maskBitmap = &ConvertBitmapL(*aMaskBitmap, aMaskBitmap->DisplayMode(), EAlwaysCopy, aBitmap.SizeInPixels());
        }
    
    SetupSegmentsL(aBitmap.SizeInPixels(), aBitmap.SizeInPixels(), aTextureFlags);
    SegmentUploadL(0, aBitmap, maskBitmap, aTextureFlags);
    }

void CHuiVg10Texture::UploadL(THuiTextureFormat aTextureFormat,
                              const TSize& aTextureSize,
                              const TUint8* aTextureBuffer,
                              THuiTextureUploadFlags aTextureFlags,
                              TUint aTextureBufferSize)
    {
    Reset(); // Must delete old content before calling SetupSegmentsL
    SetupSegmentsL(aTextureSize, aTextureSize, aTextureFlags);
    SegmentUploadL(0, aTextureFormat, aTextureSize, aTextureBuffer, aTextureBufferSize);
    }
    

void CHuiVg10Texture::PartialUploadL(THuiTextureFormat aFormat,
                                     const TPoint& aOrigin,
                                     const TSize& aSize,
                                     const TUint8* aBuffer)
    {
    ASSERT(SegmentCount() == 1);
    SegmentPartialUpload(0, aFormat, aOrigin, aSize, aBuffer);
    }


void CHuiVg10Texture::SegmentUploadL(TInt aSegment,
                                     const CFbsBitmap& aBitmap,
                                     const CFbsBitmap* aMaskBitmap,
                                     THuiTextureUploadFlags aFlags)
    {
    ASSERT(aSegment >= 0 && aSegment < SegmentCount());
    ASSERT(aBitmap.SizeInPixels() == Size());
    ASSERT(aMaskBitmap && aBitmap.SizeInPixels() == aMaskBitmap->SizeInPixels() || !aMaskBitmap);

    PushEGLContext();
    if(aSegment < 0 || aSegment >= SegmentCount())
        {
        User::Leave(KErrArgument);
        }
    
    // Clear any previous image data
    ResetSegment(aSegment);
    
#ifdef __NVG
    // Check if the bitmap is extended and has NVG data
    TUid bitmaptype = aBitmap.ExtendedBitmapType();
    TUid masktype = KNullUid;
    if (aMaskBitmap)
        {
        masktype = aMaskBitmap->ExtendedBitmapType();
        // extended bitmap and mask
        if (bitmaptype == masktype && bitmaptype != KNullUid)
            {
            SegmentUploadNvgL(aBitmap, aMaskBitmap, aFlags);
            // We now have working texture data, no need to do anything else
            PopEGLContext();
            return;
            }
        }
    else if (bitmaptype != KNullUid)
        {
        SegmentUploadNvgL(aBitmap, aMaskBitmap, aFlags);
        // We now have working texture data, no need to do anything else
        PopEGLContext();
        return;
        }
#endif
    
    VGImageFormat imageSourceFormat = (VGImageFormat)(-1);
    VGImageFormat imageInternalFormat = VG_sXRGB_8888; // TODO: get the most optimal image format from the renderer
    VGbitfield qualityFlags = VG_IMAGE_QUALITY_BETTER | VG_IMAGE_QUALITY_FASTER | VG_IMAGE_QUALITY_NONANTIALIASED;
    TBool hasAlpha = (aMaskBitmap != NULL);
    TBool conversionRequired = ETrue;
    TSize size = Size();
    TSize textureSize = MaxTextureSize();
    textureSize.iWidth = Min(size.iWidth, textureSize.iWidth);
    textureSize.iHeight = Min(size.iHeight, textureSize.iHeight);

    // See if there is a native color mode we could use without conversion
    switch (aBitmap.DisplayMode())
        {
        case EGray256:
            imageSourceFormat = VG_A_8;
            conversionRequired = EFalse;
            break;
        case EColor64K:
            imageSourceFormat = VG_sRGB_565;
            conversionRequired = EFalse;
            break;
        case EColor4K:
            imageSourceFormat = VG_sRGB_565;
            conversionRequired = ETrue;
            break;    
#if defined(OPENVG_VERSION_1_0_1)
        case EColor16MU:
            imageSourceFormat = VG_sXRGB_8888;
            conversionRequired = EFalse;
            break;
		case EColor16M:
            imageSourceFormat = VG_sXRGB_8888;
            conversionRequired = ETrue;
            break;
        case EColor16MA:
            imageSourceFormat = VG_sARGB_8888;
            hasAlpha = ETrue;
            conversionRequired = EFalse;
            break;
        case EColor16MAP:
            imageSourceFormat = VG_sARGB_8888_PRE;
            hasAlpha = ETrue;
            conversionRequired = EFalse;
            break;
#else // defined(OPENVG_VERSION_1_0_1)
        case EColor16MU:
		case EColor16M:
            imageSourceFormat = VG_sRGBX_8888;
            break;
        case EColor16MA:
            imageSourceFormat = VG_sRGBA_8888;
            hasAlpha = ETrue;
            break;
        case EColor16MAP:
            imageSourceFormat = VG_sRGBA_8888_PRE;
            hasAlpha = ETrue;
            break;
#endif // defined(OPENVG_VERSION_1_0_1)
        default:
#ifdef _DEBUG
            RDebug::Printf("CHuiVg10Texture::SegmentUploadL - unknown display mode %d", aBitmap.DisplayMode());
#endif
            conversionRequired = ETrue;
            imageSourceFormat = VG_sXRGB_8888;
        }
    
    if (aBitmap.IsCompressedInRAM() || aMaskBitmap)
        {
        conversionRequired = ETrue;
        }

    if (hasAlpha)
        {
        imageInternalFormat = VG_sRGBA_8888_PRE;
        }
    else
        {
        imageInternalFormat = imageSourceFormat;
        }

    // Create the actual image
    VGImage image = vgCreateImage(imageInternalFormat, textureSize.iWidth, textureSize.iHeight, qualityFlags);

    if (image == VG_INVALID_HANDLE)
        {
        User::Leave(KErrNoMemory);
        }
    
    if (!conversionRequired)
        {
        // No data conversion is required and we can upload the pixels as such
        aBitmap.BeginDataAccess();
        const void* data = aBitmap.DataAddress();
        TInt stride      = CFbsBitmap::ScanLineLength(size.iWidth, aBitmap.DisplayMode());
        vgImageSubData(image, data, stride, imageSourceFormat, 0, 0, textureSize.iWidth, textureSize.iHeight);
        aBitmap.EndDataAccess( ETrue );
        }
    else
        {
        // One or more conversion steps are needed
        if (hasAlpha)
            {
            // Alpha channel needs to be accounted for
#if defined(OPENVG_VERSION_1_0_1)
            imageSourceFormat = VG_sARGB_8888;
            CFbsBitmap& convBitmap = ConvertBitmapL(aBitmap, EColor16MA);
#else // defined(OPENVG_VERSION_1_0_1)
            imageSourceFormat = VG_sRGBA_8888;
            CFbsBitmap& convBitmap = ConvertBitmapL(aBitmap, EColor16MA);
#endif // defined(OPENVG_VERSION_1_0_1)
            
            // Bake the alpha channel into the converted bitmap
            if (aMaskBitmap)
                {
                if (aMaskBitmap->IsCompressedInRAM() || masktype != KNullUid)
                    {
                    CFbsBitmap& noncompressedMask = ConvertBitmapL(*aMaskBitmap, EGray256, EAlwaysCopy);
                    ReplaceAlphaChannelL(convBitmap, noncompressedMask);                                                
                    }
                 else
                    {
                    ReplaceAlphaChannelL(convBitmap, *aMaskBitmap);
                    }
                }
            #if !defined(OPENVG_VERSION_1_0_1)
                    // Match the native OpenVG 1.0 pixel component ordering
                    ASSERT(convBitmap.Handle() != aBitmap.Handle());
                    ConvertPixelComponentOrdering(convBitmap);
            #endif 
            convBitmap.BeginDataAccess();
            const void* data = convBitmap.DataAddress();
            TInt stride      = CFbsBitmap::ScanLineLength(size.iWidth, convBitmap.DisplayMode());
            vgImageSubData(image, data, stride, imageSourceFormat, 0, 0, textureSize.iWidth, textureSize.iHeight);
            convBitmap.EndDataAccess( ETrue );
            }
        else
            {
            // No alpha channel -- just opaque pixel data
#if defined(OPENVG_VERSION_1_0_1)
            TDisplayMode mode = EColor16MU;
            if (imageSourceFormat == VG_sRGB_565)
                {
                mode = EColor64K;
                }
            else if (imageSourceFormat == VG_A_8)
                {
                mode = EGray256;
                }
            else
                {
                imageSourceFormat = VG_sXRGB_8888;
                }
            CFbsBitmap& convBitmap = ConvertBitmapL(aBitmap, mode);
#else // defined(OPENVG_VERSION_1_0_1)
            imageSourceFormat = VG_sRGBX_8888;
            CFbsBitmap& convBitmap = ConvertBitmapL(aBitmap, EColor16MU);
            
            // Match the native OpenVG 1.0 pixel component ordering
            ASSERT(convBitmap.Handle() != aBitmap.Handle());
            ConvertPixelComponentOrdering(convBitmap);
#endif // defined(OPENVG_VERSION_1_0_1)
            
            convBitmap.BeginDataAccess();
            const void* data = convBitmap.DataAddress();
            TInt stride      = CFbsBitmap::ScanLineLength(size.iWidth, convBitmap.DisplayMode());
            vgImageSubData(image, data, stride, imageSourceFormat, 0, 0, textureSize.iWidth, textureSize.iHeight);
            convBitmap.EndDataAccess( ETrue );
            }
        }
    
    // Save the image handle
    SetSegmentName(0, (TUint)image);
    
    // The texture now has content.
    iInternalFlags |= EFlagHasContent;

    if (hasAlpha)
        {
        iInternalFlags |= EFlagHasAlpha;
        }
    
    // Keep the shadow in sync with the image contents
    if (IsShadowEnabled())
        {
        // if this fails there is nothing we can do
        // There will be no shadow if something goes wrong here
        TRAP_IGNORE( GenerateShadowL() );
        }
    PopEGLContext();
    // Wake up refresh. It is likely that the new texture will be visible on
    // the screen.
    CHuiStatic::ContinueRefresh();
    }

static void ConvertBufferRgb888TosRGBX8888(const TUint8* aSrc, TUint8* aDest, TInt aCount)
{
    while (aCount--)
        {
        *aDest++ = *aSrc++;
        *aDest++ = *aSrc++;
        *aDest++ = *aSrc++;
        *aDest++ = 0xff;
        }
}

static void ConvertBufferLa88TosRGBA8888(const TUint8* aSrc, TUint8* aDest, TInt aCount)
{
    while (aCount--)
        {
        TUint8 l = *aSrc++;
        TUint8 a = *aSrc++;
        *aDest++ = l;
        *aDest++ = l;
        *aDest++ = l;
        *aDest++ = a;
        }
}

#if !defined(OPENVG_VERSION_1_0_1)
static void ConvertBufferRgba8888TosRGBA8888(const TUint8* aSrc, TUint8* aDest, TInt aCount)
{
    while (aCount--)
        {
        TUint8 r = *aSrc++;
        TUint8 g = *aSrc++;
        TUint8 b = *aSrc++;
        TUint8 a = *aSrc++;
        *aDest++ = a;
        *aDest++ = r;
        *aDest++ = g;
        *aDest++ = b;
        }
}
#endif

void CHuiVg10Texture::SegmentClearL(TInt aSegment,
                                    TBool aWithAlpha,
                                    const TRgb& aColor,
                                    TUint8 aAlpha)
    {
    ASSERT(aSegment >= 0 && aSegment < SegmentCount());

    if(aSegment < 0 || aSegment >= SegmentCount())
        {
        User::Leave(KErrArgument);
        }

    // Clear any previous image data
    TSize size = Size();
    ResetSegment(aSegment);

    // Create the actual image
    VGImageFormat imageInternalFormat = aWithAlpha ? VG_sRGBA_8888 : VG_sRGBX_8888;
    VGbitfield qualityFlags = VG_IMAGE_QUALITY_BETTER | VG_IMAGE_QUALITY_FASTER | VG_IMAGE_QUALITY_NONANTIALIASED;
    VGImage image = vgCreateImage(imageInternalFormat, size.iWidth, size.iHeight, qualityFlags);
    
    if (image == VG_INVALID_HANDLE)
        {
        User::Leave(KErrNoMemory);
        }

    // Set the clear color
    VGfloat scale = 1.0f / 255.0f;
    VGfloat color[] = {
        aColor.Red()   * scale,
        aColor.Green() * scale,
        aColor.Blue()  * scale,
        aWithAlpha ? (aAlpha * scale) : 1.0f
    };
    
    vgSetfv(VG_CLEAR_COLOR, 4, color);
    vgClearImage(image, 0, 0, size.iWidth, size.iHeight);

    // The texture now has content.
    iInternalFlags |= EFlagHasContent;

    if (aWithAlpha)
        {
        iInternalFlags |= EFlagHasAlpha;
        }

    // Keep the shadow in sync with the image contents
    if (IsShadowEnabled())
        {
        GenerateShadowL();
        }
    
    HUI_VG_INVARIANT();
    }

void CHuiVg10Texture::SegmentUploadL(TInt aSegment,
                                     THuiTextureFormat aFormat,
                                     const TSize& aSize,
                                     const TUint8* aBuffer,
                                     TUint /*aBufferSize*/)
    {
    ASSERT(aSegment >= 0 && aSegment < SegmentCount());
    ASSERT(aSize == Size());
    
    SegmentPartialUploadInternal(aSegment, aFormat, TPoint(0, 0), aSize, aBuffer, ETrue);
    }

void CHuiVg10Texture::SegmentPartialUpload(
        TInt aSegment,
        THuiTextureFormat aFormat,
        const TPoint& aOrigin,
        const TSize& aSize,
        const TUint8* aBuffer)
    {
    SegmentPartialUploadInternal(aSegment, aFormat, aOrigin, aSize, aBuffer, EFalse);
    }

void CHuiVg10Texture::SegmentPartialUploadInternal(
        TInt aSegment,
        THuiTextureFormat aFormat,
        const TPoint& aOrigin,
        const TSize& aSize,
        const TUint8* aBuffer,
        TBool aCreateImage)
    {
    ASSERT(aSegment >= 0 && aSegment < SegmentCount());

    if (aSegment < 0 || aSegment >= SegmentCount())
        {
        // we return without doing anything because we are called by a non-leaving function
        return;
        }

    if (aOrigin.iX < 0 || aOrigin.iY < 0)
        {
        // we return without doing anything because we are called by a non-leaving function
        return;
        }
    
    HUI_DEBUG4(_L("CHuiVg10Texture::SegmentPartialUploadInternal() - Uploading partial image (%ix%i pixels, offset x: %i y: %i)."),
               aSize.iWidth, aSize.iHeight, aOrigin.iX, aOrigin.iY);
    
    VGImageFormat imageSourceFormat = (VGImageFormat)(-1);
    VGImageFormat imageInternalFormat = VG_sRGBX_8888; // TODO: get the most optimal image format from the renderer
    VGbitfield qualityFlags = VG_IMAGE_QUALITY_FASTER | VG_IMAGE_QUALITY_BETTER; // TODO: get this from hitchcock
    TBool hasAlpha = EFalse;
    TBool conversionRequired = ETrue;
    TInt sourceStride = 0;
    
    switch (aFormat)
        {
        case EHuiTextureFormatRgb565:
            imageSourceFormat = VG_sRGB_565;
            conversionRequired = EFalse;
            sourceStride = aSize.iWidth * 2;
            break;
        case EHuiTextureFormatRgb888:
            imageSourceFormat = VG_sRGBX_8888;
            sourceStride = aSize.iWidth * 4;
            break;
        case EHuiTextureFormatLa88:
            // We have to expand this format to RGBA, since there is no luminance-alpha format in OpenVG 1.0.1
            imageSourceFormat = VG_sRGBA_8888;
            sourceStride = aSize.iWidth * 4;
            hasAlpha = ETrue;
            break;
#if defined(OPENVG_VERSION_1_0_1)
        case EHuiTextureFormatRgba8888:
            imageSourceFormat = VG_sARGB_8888;
            conversionRequired = EFalse;
            hasAlpha = ETrue;
            sourceStride = aSize.iWidth * 4;
            break;
#else // defined(OPENVG_VERSION_1_0_1)
        case EHuiTextureFormatRgba8888:
            imageSourceFormat = VG_sRGBA_8888;
            sourceStride = aSize.iWidth * 4;
            hasAlpha = ETrue;
            break;
#endif // defined(OPENVG_VERSION_1_0_1)
        
        // Compressed formats are not supported
        default:
            // we return without doing anything because we are called by a non-leaving function
            return;
        }
    
    // Use a matching internal format
    imageInternalFormat = imageSourceFormat;

    // Create the actual image if required
    VGImage image = VG_INVALID_HANDLE;
    
    if (aCreateImage)
        {
        image = vgCreateImage(imageInternalFormat, aSize.iWidth, aSize.iHeight, qualityFlags);
        }
    else
        {
        image = (VGImage)SegmentName(0);
        
        // Verify the dimensions
        if (aOrigin.iX + aSize.iWidth  >= Size().iWidth || 
            aOrigin.iY + aSize.iHeight >= Size().iHeight)
            {
            // we return without doing anything because we are called by a non-leaving function
            return;
            }
        }

    if (image == VG_INVALID_HANDLE)
        {
        // we return without doing anything because we are called by a non-leaving function
        return;
        }
    
    if (!conversionRequired)
        {
        // No data conversion is required and we can upload the pixels as such
        vgImageSubData(image, aBuffer, sourceStride, imageSourceFormat, aOrigin.iX, aOrigin.iY, aSize.iWidth, aSize.iHeight);
        HUI_VG_INVARIANT();
        }
    else
        {
        // One or more conversion steps are needed
        TInt pixels = aSize.iWidth * aSize.iHeight;
        TUint8* convBuffer = NULL;
        convBuffer = new TUint8[sourceStride * aSize.iHeight];
        // there are no leaving functions until our buffer is deleted
        // We cannot put convBuffer onto cleanup stack because we cannot leave.
        
        if ( !convBuffer )
            {
            // we return without doing anything because we are called by a non-leaving function
            return;
            }
        
        switch (aFormat)
            {
            case EHuiTextureFormatRgb888:
                ConvertBufferRgb888TosRGBX8888(aBuffer, convBuffer, pixels);
                break;
            case EHuiTextureFormatLa88:
                ConvertBufferLa88TosRGBA8888(aBuffer, convBuffer, pixels);
                break;
#if !defined(OPENVG_VERSION_1_0_1)
            case EHuiTextureFormatRgba8888:
                ConvertBufferRgba8888TosRGBA8888(aBuffer, convBuffer, pixels);
                break;
#endif // defined(OPENVG_VERSION_1_0_1)
            default:
                ASSERT(0);
            }
        
        vgImageSubData(image, convBuffer, sourceStride, imageSourceFormat, aOrigin.iX, aOrigin.iY, aSize.iWidth, aSize.iHeight);
        delete[] convBuffer;
        HUI_VG_INVARIANT();
        }

    // Save the image handle
    SetSegmentName(0, (TUint)image);
    
    // The texture now has content.
    iInternalFlags |= EFlagHasContent;

    if (hasAlpha)
        {
        iInternalFlags |= EFlagHasAlpha;
        }
    
    // Keep the shadow in sync with the image contents
    if (IsShadowEnabled())
        {
        // We cannot leave, we just try our best.
        // If this fails, we have no shadow
        TRAP_IGNORE( GenerateShadowL() );
        }
    
    HUI_VG_INVARIANT();
    }

void CHuiVg10Texture::Reset()
    {
    CHuiTexture::Reset();
    
    if (iShadow.Name())
        {
        vgDestroyImage((VGImage)iShadow.Name());
        iShadow.SetName(0);
        }
    
    iShadowEnabled = EFalse;
    
#ifdef __NVG
    CHuiTexture::SetNvgContent(EFalse);
    iIsExtended = EFalse;
    
    delete iNVGData;
    iNVGData = NULL;
    delete iIconCommands;
    iIconCommands = NULL;
#endif
    }

void CHuiVg10Texture::ResetSegment(TInt aSegment)
    {
    HUI_DEBUG1(_L("CHuiVg10Texture::ResetSegment() - Trying to delete image for segment %i."), aSegment);
    HUI_VG_INVARIANT();
    ASSERT(aSegment>=0 && aSegment < SegmentCount());

    VGImage image = (VGImage)SegmentName(aSegment);
    if (image != VG_INVALID_HANDLE)
        {
        vgDestroyImage(image);
        }

    SetSegmentName(aSegment, VG_INVALID_HANDLE);
    SetSegmentTextureSize(aSegment, TSize(0, 0));
    SetSegmentSize(aSegment, TSize(0, 0));
    SetSegmentOffset(aSegment, TPoint(0, 0));

    // There is no content anymore
    iInternalFlags &= ~(EFlagHasContent | EFlagHasAlpha);
    
    HUI_VG_INVARIANT();
    }


void CHuiVg10Texture::InitSegmentL(TInt /*aSegment*/)
    {
    // Nothing to do
    }


void CHuiVg10Texture::SetupSegmentsL(const TSize& aLogicalSize,
                                     const TSize& aTextureSize,
                                     THuiTextureUploadFlags aFlags)
    {
    // Assert that the sizes are valid.
    ASSERT(aTextureSize.iWidth > 0 && aTextureSize.iHeight > 0);
    ASSERT(aLogicalSize.iWidth > 0 && aLogicalSize.iHeight > 0);

    HUI_DEBUG4(_L("CHuiVg10Texture::SetupSegments() - Configuring a single %ix%i texture segment for %ix%i bitmap."),
               aTextureSize.iWidth, aTextureSize.iHeight,
               aTextureSize.iWidth, aTextureSize.iHeight);

    // All textures use only a single segment with OpenVG.
    SetSegmentCountL(1);
    
    SetSegment(0, aLogicalSize, TPoint(0, 0), aTextureSize);
    SetSegmentName(0, (TUint)VG_INVALID_HANDLE);
    
    // The logical size always matches the texture size
    SetSize(aLogicalSize);
    
    // Enable shadow generation if requested
    if (aFlags & EHuiTextureUploadFlagGenerateShadow)
        {
        EnableShadow(ETrue);
        }
    }
    
void CHuiVg10Texture::TextureExtension(const TUid& aExtensionUid, TAny** aExtensionParameters)
    {
    if ( aExtensionUid == KHuiTexturePartialBitmapUploadUid && 
         aExtensionParameters && *aExtensionParameters )
        {
        THuiTexturePartialBitmapUploadParams* params = 
            static_cast<THuiTexturePartialBitmapUploadParams*>(*aExtensionParameters);
        PartialBitmapUpload(params);
        }
    else
        {
	    CHuiTexture::TextureExtension(aExtensionUid, aExtensionParameters);    	
	    }
    }

void CHuiVg10Texture::EnableShadow(TBool aEnable)
    {
    CHuiTexture::EnableShadow(aEnable);
    iShadowEnabled = aEnable;

    if (!aEnable && iShadow.Name())
        {
        vgDestroyImage((VGImage)iShadow.Name());
        iShadow.SetName(0);
        HUI_VG_INVARIANT();
        }
    
    if (aEnable)
        {
        // We cannot leave, we just try our best.
        // If this fails, we have no shadow
        TRAP_IGNORE( GenerateShadowL() ); 
        }

    HUI_VG_INVARIANT();
    }

TBool CHuiVg10Texture::IsShadowEnabled() const
    {
    return iShadowEnabled;
    }

TBool CHuiVg10Texture::GetShadowTexture(THuiTextureHandle& aHandle) const
    {
    if (iShadowEnabled && iShadow.SegmentName(0))
        {
        aHandle = iShadow;
        return ETrue;
        }
    return EFalse;
    }

void CHuiVg10Texture::UpdateShadowSizeL(const TSize& aSize)
    {
    VGImage shadow = VG_INVALID_HANDLE;

    if (iShadow.Name())
        {
        shadow = (VGImage)(iShadow.Name());
        TInt w = vgGetParameteri(shadow, VG_IMAGE_WIDTH);
        TInt h = vgGetParameteri(shadow, VG_IMAGE_HEIGHT);
        
        if (w != aSize.iWidth || h != aSize.iHeight)
            {
            // Destroy the old shadow image -- a new one will be created below
            vgDestroyImage(shadow);
            iShadow.SetName(0);
            }
        else
            {
            return;
            }
        }
    
    ASSERT(!iShadow.Name());
    
    shadow = vgCreateImage(VG_sRGBA_8888_PRE, aSize.iWidth, aSize.iHeight, 
                           VG_IMAGE_QUALITY_FASTER | VG_IMAGE_QUALITY_NONANTIALIASED);

    if (shadow == VG_INVALID_HANDLE)
        {
        return;
        }
    iShadow.SetName(shadow);
    iShadow.SetSize(aSize);
    
    HUI_VG_INVARIANT();
    }

void CHuiVg10Texture::GenerateShadowL()
    {
    if (iRenderPlugin.IsReleased())
        {
        HUI_DEBUG(_L("CHuiVg10Texture::GenerateShadowL - VG context not available, cannot generate texture object."));
        return;
        }
    
    if (!IsShadowEnabled() || !(iInternalFlags & EFlagHasContent))
        {
        return;
        }

    CHuiTextureProcessor& proc = CHuiStatic::Env().TextureManager().Processor();
    TSize size = Size();

    switch(ShadowStyle())
        {
        case EHuiTextureShadowStyleIcon:
            {
            // TODO: Allow for smaller shadow images as an optimization
            TSize shadowSize(size.iWidth, size.iHeight);
            UpdateShadowSizeL(shadowSize);
            proc.BlurL(Handle(), iShadow, shadowSize, 4,
                CHuiTextureProcessor::EBlurFlagWhite |
                CHuiTextureProcessor::EBlurFlagAlpha |
                CHuiTextureProcessor::EBlurFlagExpandEdges);
            break;
            }

        case EHuiTextureShadowStyleRasterizedText:
            {
            TSize shadowSize(size.iWidth, size.iHeight);
            UpdateShadowSizeL(shadowSize);
            proc.BlurL(Handle(), iShadow, shadowSize, 4,
                CHuiTextureProcessor::EBlurFlagWhite |
                CHuiTextureProcessor::EBlurFlagAlpha |
                CHuiTextureProcessor::EBlurFlagExpandEdges);
            break;
            }

        default:
            break;
        }

    HUI_VG_INVARIANT();
    }

void CHuiVg10Texture::PartialBitmapUpload(THuiTexturePartialBitmapUploadParams* aParams)
    {
    if ( !aParams->iBitmap || !aParams->iBitmap->Handle() ||
         aParams->iBitmap->DisplayMode() != EColor16MAP )
        {
        // Only specific format supported.
        aParams->iErrorCode = KErrArgument;
        return;
        }
        
    if ( SegmentCount() != 1 || ((VGImage)SegmentName(0)) == VG_INVALID_HANDLE || 
         Size() != aParams->iBitmap->SizeInPixels() )
        {
        // You must perform initial upload using normal methods.
        aParams->iErrorCode = KErrNotReady;
        return;
        }
    
    //TRect segmentRect(SegmentSize(0));
    TRect segmentRect(Size());
    TRect rect(aParams->iRect);
    rect.Intersection(segmentRect);
    
    if ( rect != aParams->iRect )
        {
        // Rect must be fully within segment rect
        aParams->iErrorCode = KErrArgument;
        return;        
        }

    aParams->iErrorCode = KErrNone;
    
    if ( rect.IsEmpty() )
        {
        // Nothing to upload.
        return;
        }

    PushEGLContext();
    aParams->iBitmap->BeginDataAccess();

    const TInt scanLineLength = CFbsBitmap::ScanLineLength(
        aParams->iBitmap->SizeInPixels().iWidth, EColor16MAP);
    const TInt bytesPerPixel = 4;
    
    TUint8* dataAddress = (TUint8*)aParams->iBitmap->DataAddress();
    dataAddress += rect.iTl.iX * bytesPerPixel;
    dataAddress += rect.iTl.iY * scanLineLength;
        
    vgImageSubData(
        (VGImage)SegmentName(0), // image
        dataAddress,             // data
        scanLineLength,          // dataStride
        VG_sARGB_8888_PRE,       // dataFormat
        rect.iTl.iX,             // x
        rect.iTl.iY,             // y
        rect.Width(),            // width
        rect.Height()            // height
        );
    
    aParams->iBitmap->EndDataAccess( ETrue );
    PopEGLContext();
    }

#ifdef __NVG
HBufC8* CHuiVg10Texture::ReadNVGDataL(const CFbsBitmap& aBitmap)
    {
    // Fetch the extended data
    aBitmap.BeginDataAccess();
    const TUint32* data = aBitmap.DataAddress();
    TInt dataSize = aBitmap.DataSize();
    TUint8* compressedData = new (ELeave) TUint8[dataSize];
    CleanupStack::PushL(compressedData);
    Mem::Copy(compressedData, data, dataSize);
    aBitmap.EndDataAccess(ETrue);
    
    // Create a descriptor out of the extended bitmap data. The iNVGData
    // will now contain the direct OpenVG commands
    TPtr8 nvgDataPtr(compressedData, dataSize, dataSize);
    HBufC8* dataBuf = nvgDataPtr.AllocL();
    
    CleanupStack::PopAndDestroy(compressedData);
    return dataBuf;
    }

void CHuiVg10Texture::SegmentUploadNvgL(const CFbsBitmap& aBitmap, const CFbsBitmap* aMaskBitmap, THuiTextureUploadFlags aFlags)
    {
    HUI_VG_INVARIANT();
    
    VGImage image = VG_INVALID_HANDLE;
    HBufC8* dataBuf = ReadNVGDataL(aBitmap);
    CleanupStack::PushL(dataBuf);
    HBufC8* maskDataBuf = NULL;
    TAknIconHeader header = GetNvgIconHeader(dataBuf);
    TAknIconHeader maskHeader(header);  // DUMMY Creation, since default constructor is missing!
    
    if (aMaskBitmap)
        {
        maskDataBuf = ReadNVGDataL(*aMaskBitmap);
        CleanupStack::PushL(maskDataBuf);
        maskHeader = GetNvgIconHeader(maskDataBuf);
        }
    
    // The trick here is to check the texture upload flags, and determine
    // whether we want to draw from NVG object cache directly or raster
    // the NVG data to a PBuffer
    if (aFlags & EHuiTextureUploadFlagUsePureNvg)
        {
        // Save the NVG data for future use
        if (iNVGData)
            {
            delete iNVGData;
            iNVGData = NULL;
            }
        iNVGData = dataBuf;
        
        // TODO: If the NVG ObjectCache is still up-to-date, we could possibly
        // create the object cached NVG icon for faster drawing, and maybe
        // discard the iNVGData completely (to save RAM?)
        CreateObjCachedNVGIconL();  // NOTE: The OPENVG_OBJECT_CACHING has to be defined in NVG!
        
        // Mark the texture as an NVG texture
        CHuiTexture::SetNvgContent(ETrue);
        iIsExtended = ETrue;
        }
    else    // By default, use the "OPTION C" -way!
        {
        // Use the same way to create the rendered image as in OPTION_C (see below)
        CNvgEngine& nvgEngine = iRenderPlugin.NvgEngine();
        iIsExtended = ETrue;    // I'm an NVG texture yet..
    
        // Make sure we don't leak memory
        if (iNVGData)
            {
            delete iNVGData;
            iNVGData = NULL;
            }
        
        // Create the real image from NVG databuf
        iNVGData = dataBuf;
        image = CreateRenderedImage(&nvgEngine, dataBuf, Size(),aBitmap.SerialNumber());
        
        // New functionality for checking the mask
        if (header.GetBitmapId() != maskHeader.GetBitmapId() && maskDataBuf &&
            CompareNvgData(dataBuf, maskDataBuf) != 0)
            {
            VGImage maskImg = VG_INVALID_HANDLE;
            // The mask is from different bitmap => we have to create
            // a separate mask image for masking the NVG icon
            maskImg = CreateRenderedImage(&nvgEngine, maskDataBuf, Size());
            ReplaceVGImageAlphaChannelL(image, maskImg, Size());
            vgDestroyImage(maskImg);
            }
        
        // Save the image handle
        SetSegmentName(0, (TUint)image);
        
        // No need for object cached cmds, if any did exist
        delete iIconCommands; // No effect, if iIconCommands aren't created
        iIconCommands = NULL;
        
        // If the VGImage texture creation was successful, we are no longer "NVG 
        // texture" in a sense. If not, we will use the NVG data directly for drawing
        if (image != VG_INVALID_HANDLE)
            {
            CHuiTexture::SetNvgContent(EFalse);
            iIsExtended = EFalse;
            }
        }
    
    if (aMaskBitmap)
        {
        CleanupStack::PopAndDestroy(maskDataBuf);
        }
    CleanupStack::Pop(dataBuf);
    
    // The texture now has content.
    iInternalFlags |= EFlagHasContent;
    iInternalFlags |= EFlagHasAlpha;
    
    HUI_VG_INVARIANT();
    }

TInt CHuiVg10Texture::CompareNvgData(HBufC8* aNVGData1, HBufC8* aNVGData2)
    {
    TInt lengthAfterHeader1 = aNVGData1->Length() - KIconHeaderLength;
    TInt lengthAfterHeader2 = aNVGData2->Length() - KIconHeaderLength;
    
    TPtr8 firstNoHeader((TUint8 *)aNVGData1->Des().Ptr() + KIconHeaderLength, lengthAfterHeader1, lengthAfterHeader1);
    TPtr8 secondNoHeader((TUint8 *)aNVGData2->Des().Ptr() + KIconHeaderLength, lengthAfterHeader2, lengthAfterHeader2);
    
    TInt returnValue = firstNoHeader.Compare(secondNoHeader);
    return returnValue;
    }



VGImage CHuiVg10Texture::CreateRenderedImage(CNvgEngine* aNvgEngine, HBufC8* aNVGData, const TSize& aDestSize, TInt64 aSerialNumber)
    {
    HUI_VG_INVARIANT();
    
    // Image placeholder
    VGImage image = VG_INVALID_HANDLE;

    if ( !iIsExtended )
        {
        // If called outside of the context, do nothing.
        // Image will be VG_INVALID_HANDLE
        HUI_DEBUG(_L("CHuiVg10Texture::CreateRenderedImage() - Texture not extended, can't create rendered image"));
        return image;
        }
    
    // Check if we already have an existing image.
    if (SegmentCount() == 1)
        {
        image = (VGImage)SegmentName(0);
        if( image != VG_INVALID_HANDLE )
            {
            HUI_DEBUG1(_L("CHuiVg10Texture::CreateRenderedImage() - Image already exists: %d"), image);
            return image;
            }
        }
    
    // NVGEngine is needed to do the drawing of the extended bitmap to VGImage
    if (!aNvgEngine || !aNVGData)
        {
        HUI_DEBUG(_L("CHuiVg10Texture::CreateRenderedImage() - No NvgEngine / NVGData!"));
        return image;
        }
    
    // Get the needed egl contexts & stuff for creating the proper eglSurface
    EGLContext context = iRenderPlugin.EglSharedContext();
    MHuiRenderSurface* oldSurface = CHuiStatic::CurrentRenderSurface();

#ifndef __WINS__ // Should possibly query the supported mode instead?
    VGImageFormat imageInternalFormat = VG_sARGB_8888_PRE;
#else
    VGImageFormat imageInternalFormat = VG_sARGB_8888;
#endif
    
    VGbitfield qualityFlags = VG_IMAGE_QUALITY_NONANTIALIASED; // | VG_IMAGE_QUALITY_BETTER | VG_IMAGE_QUALITY_FASTER;
    image = vgCreateImage(imageInternalFormat, aDestSize.iWidth, aDestSize.iHeight, qualityFlags);
    
    // Get the configs and displays etc. needed for creating the surface
    EGLDisplay display = iRenderPlugin.EglDisplay();

#if 1
    // Returns the same config, as below!
    // The config used with the current renderer surface has the same parameters
    // that are needed with the PBufferSurface from vgImage
    EGLConfig config = iRenderPlugin.EglConfig(0);
#else
    // This structure can be used, if surface config params have to be changed
    // Better way though might be to change the vgImage VGImageFormat accordingly..
    const TInt BITS_PER_CHANNEL = 8;
    // Choose an EGL config
    const EGLint attrs[] =
        {
        EGL_RENDERABLE_TYPE,    EGL_OPENVG_BIT,
        EGL_SURFACE_TYPE,       EGL_PBUFFER_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT,
        EGL_RED_SIZE,           BITS_PER_CHANNEL,
        EGL_GREEN_SIZE,         BITS_PER_CHANNEL,
        EGL_BLUE_SIZE,          BITS_PER_CHANNEL,
        EGL_ALPHA_SIZE,         BITS_PER_CHANNEL,
        EGL_NONE
        };
    TInt configCount = iRenderPlugin.EglChooseConfig(attrs);
    ASSERT(configCount > 0);
    EGLConfig config = iRenderPlugin.EglConfig(0);
#endif
    
    // The VGImage will act as the surface, so the drawing will go directly to the VGImage
    EGLSurface newSurface = eglCreatePbufferFromClientBuffer(
        display, EGL_OPENVG_IMAGE,
        static_cast<EGLClientBuffer>(image),    // Use the image as buffer
        config, NULL);
    
    // Report error in debug mode, if failed creating the surface
    if ( newSurface == EGL_NO_SURFACE )
        {
        HUI_DEBUG1(_L("CHuiVg10Texture::CreateRenderedImage() - EGL Surface could not be created, eglErr: %04x"), eglGetError() );
        if ( image != VG_INVALID_HANDLE )
            {
            vgDestroyImage( image );
            image = VG_INVALID_HANDLE;
            }
        
        HUI_VG_INVARIANT();
        return image;
        }
    
    // Set the new VGImage related eglSurface as current, and start drawing onto it!  
    // We use the old context, our surface should be compatible with it          
    if ( eglMakeCurrent( display, newSurface, newSurface, context ) == EGL_FALSE )
        {
        // Report error in debug mode
        HUI_DEBUG1(_L("CHuiVg10Texture::CreateRenderedImage() - EGL Surface could not be made current, eglErr: %04x"), eglGetError());
        if ( image != VG_INVALID_HANDLE )
            {
            vgDestroyImage( image );
            image = VG_INVALID_HANDLE;
            }
        
        eglDestroySurface( display, newSurface );
        HUI_VG_INVARIANT();
        return image;
        }
    CHuiStatic::SetCurrentRenderSurface( NULL );
    
    
    // VKN TODO: It would be better, if the GC could somehow spot that openVg
    // state has changed, and would restore it's own state. Could use the same
    // AddRestoreStateFlags as the paint is currently using..
    //
    // Get the current statematrix and store it for the duration of NVG drawing
    TReal32 matrix[9];
    vgGetMatrix(matrix);
    vgLoadIdentity();
    
    // Render the NVGtexture into the image buffer. No transformations are done for this.
    SetNvgParamsFromIconHeader(*aNvgEngine, aNVGData);
    
    // Always set blending mode to SRC_OVER before drawing NVG content (so that the  
    // drawn icons would show correctly, and no issues with transparency would arise)
    VGint blendMode = vgGeti(VG_BLEND_MODE);
    vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_OVER);
    TAknIconHeader iconheader = GetNvgIconHeader(aNVGData);
    TBool direct = EFalse;
    if (iconheader.IsMarginCorrection() && aSerialNumber)
        {
        CFbsBitmap* entry = CHuiStatic::Env().AppIconCache().Find(aSerialNumber, aDestSize);
        if (entry)
            {
            TSize entrySize = entry->SizeInPixels();
            entry->BeginDataAccess();
            TUint8* dataPtr = (TUint8*)entry->DataAddress();
            vgImageSubData(image, dataPtr, CFbsBitmap::ScanLineLength(entrySize.iWidth, EColor16MAP), VG_sARGB_8888_PRE, 0, 0, entrySize.iWidth, entrySize.iHeight);
            entry->EndDataAccess();
            direct = ETrue;
            }
        }
        
    if (!direct)
        {
        if (iIconCommands)
            {
            //HUI_DEBUG(_L("CHuiVg10Texture::CreateRenderedImage() - Drawing iIconCommands"));
            iIconCommands->Draw(aDestSize, aNvgEngine);
            }
        else
            {
            // If ObjectCached version failed, try to use the old way
            //HUI_DEBUG(_L("CHuiVg10Texture::CreateRenderedImage() - Drawing with DrawNvg"));
            aNvgEngine->DrawNvg(GetNvgDataWithoutHeader(aNVGData), aDestSize, NULL, NULL);
            }
        }
    
    // NVG-TLV icon margin special case check:
    // Check, if the icon has to be margin corrected
    TSize size = aDestSize; // For using the correct size also later on
    if (iconheader.IsMarginCorrection() && !direct)
        {
        size = ApplyMargin(image, aDestSize, display, newSurface, context);
        if( size != aDestSize)
            {
            // Redo the drawing, this time to the correct size
            if (iIconCommands)
                iIconCommands->Draw(size, aNvgEngine);
            else
                aNvgEngine->DrawNvg(GetNvgDataWithoutHeader(aNVGData), size, NULL, NULL);
            }
#ifndef __WINS__
        // assume 32bpp for the icon, don't cache if it's too large
        TInt bitmapBytes = aDestSize.iWidth*aDestSize.iHeight*4;
        if (bitmapBytes < 128*128*4)
            {
            // can't leave here (we would screw up the transformation matrix
            // for the primary surface)
            CFbsBitmap* rasterizedIcon = new CFbsBitmap;
            TInt rastererr(KErrNone);
            if (rasterizedIcon)
                {
                rastererr = rasterizedIcon->Create(aDestSize, EColor16MAP);
                }
            if (!rastererr)
                {                    
                rasterizedIcon->BeginDataAccess();
                TUint8* dataPtr = (TUint8*)rasterizedIcon->DataAddress();
                vgGetImageSubData(image, dataPtr, CFbsBitmap::ScanLineLength(aDestSize.iWidth, EColor16MAP), VG_sARGB_8888_PRE, 0, 0, aDestSize.iWidth, aDestSize.iHeight);
                rasterizedIcon->EndDataAccess();
                // store for future use....
                TRasterizedBitmap rbmp(aSerialNumber, rasterizedIcon);            
                rastererr = CHuiStatic::Env().AppIconCache().Insert(rbmp);        
                }
            // either the bitmap creation failed, or cache insert failed
            // delete the bitmap so that we don't leak memory
            if (rastererr)
                {
                delete rasterizedIcon;
                }
            }
#endif
        }

    // restore the old surface before restoring original modes
    oldSurface->MakeCurrent();  // This will also call the SetCurrentRenderSurface
    
    // Return the original blend mode
    vgSeti(VG_BLEND_MODE, blendMode);
    // The NVG draw messes up the paint, scissoring & rects, so mark them as dirty
    TInt dirtyFlags = EHuiVg10GcStateFlagDirtyPaint |
                    EHuiVg10GcStateFlagDirtyScissor | 
                    EHuiVg10GcStateFlagDirtyScissorRects;
    iRenderPlugin.AddRestoreStateFlags(dirtyFlags);
    
    HUI_VG_INVARIANT();
    
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
    vgLoadMatrix(matrix);
    
    // Now we should have a rendered image in the image variable!
    // Release the surface, but not the context because we used a shared context
    if ( newSurface != EGL_NO_SURFACE )
        {
        eglDestroySurface( display, newSurface );
        }
    
    // NVG-TLV color correction special case check: If the icon has 
    // some color specified in the icon header, set the new icon color
    TUint32 iconColor = iconheader.GetIconColor();
    if (iconColor & 0x00FFFFFF)
        {
        SetIconColor(image, size, iconColor);
        }
    
    HUI_VG_INVARIANT();
    
    return image;
    }

TBool CHuiVg10Texture::IsExtended() const
    {
    return iIsExtended;
    }

HBufC8* CHuiVg10Texture::GetExtendedTextureData() const
    {
    return iNVGData;
    }

MNVGIcon* CHuiVg10Texture::GetIconCommandsData() const
    {
    return iIconCommands;
    }

void CHuiVg10Texture::CreateObjCachedNVGIconL()
    {
    HUI_VG_INVARIANT();
    // Just to be sure that we don't leak memory
    if (iIconCommands)
        {
        HUI_DEBUG(_L("CHuiVg10Texture::CreateObjCachedNVGIconL() - deleting old iIconCommands. Should never come here"));
        delete iIconCommands;
        iIconCommands = NULL;
        }

    // Fetch the NvgDecoder engine for creating the obj cached icon
    CNvgEngine& nvgEngine = iRenderPlugin.NvgEngine();
    
    //Set Parameters from Icon Header
    SetNvgParamsFromIconHeader(nvgEngine, iNVGData);
    
    // Parse the header info out of the nvg data 
    TPtr8 nvgDataVoidIC = GetNvgDataWithoutHeader(iNVGData);
    
	iIconCommands = nvgEngine.CreateNVGIcon(nvgDataVoidIC, Size());
	
	HUI_VG_INVARIANT();
    }

TAknIconHeader CHuiVg10Texture::GetNvgIconHeader(HBufC8* aNVGData)
    {
    // Parse the icon header info from the extended data
    TPtr8 IconHeaderPtr((TUint8*)aNVGData->Des().Ptr(), KIconHeaderLength, KIconHeaderLength);
    TAknIconHeader iconheader(IconHeaderPtr);
    
    return iconheader;
    }

TPtr8 CHuiVg10Texture::GetNvgDataWithoutHeader(HBufC8* aNVGData)
    {
    // The rest of the data (after the iconheader) are the OVG drawing instructions
    TInt lengthAfterHeader = aNVGData->Length() - KIconHeaderLength;
    TPtr8 nvgDataVoidIC((TUint8 *)aNVGData->Des().Ptr() + KIconHeaderLength, lengthAfterHeader, lengthAfterHeader);
    
    return nvgDataVoidIC;
    }

void CHuiVg10Texture::SetNvgParamsFromIconHeader(CNvgEngine& aNvgEngine, HBufC8* aNVGData)
    {
    TAknIconHeader iconheader = GetNvgIconHeader(aNVGData);
    
    // Set preserve aspect ratio according to the header info
    TNvgAlignStatusType alignTypeValue = ENvgPreserveAspectRatio_XmidYmid;
    TNvgMeetOrSliceType meetOrSliceTypeValue = ENvgMeet;
    
    switch ( iconheader.GetScaleMode() )
        {
        case EAspectRatioPreserved: // Fall through
            {
            // Use default
            break;
            }
            
        // Ensures NVG content fully covers
        // the area of the icon whilst preserving aspect ratio.
        case EAspectRatioPreservedSlice:
            {
            // alignTypeValue use default
            meetOrSliceTypeValue = ENvgSlice;
            break;
            } 
            
        // EAspectRatioPreservedAndUnusedSpaceRemoved is mapped to the same
        // values as EAspectRatioNotPreserved because we already have a 
        // frame buffer with the dimensions that preserves the aspect ratio.
        // This mapping ensures that NVG engine does not calculate aspect
        // ratio twice and potentially resulting in precision loss.
        case EAspectRatioPreservedAndUnusedSpaceRemoved:
        case EAspectRatioNotPreserved:
            {            
            alignTypeValue = ENvgPreserveAspectRatio_None;
            // meetOrSliceTypeValue used default
            break;
            }
        }
    aNvgEngine.SetPreserveAspectRatio(alignTypeValue, meetOrSliceTypeValue);
    aNvgEngine.Rotate(iconheader.GetRotation(),Size().iWidth >>1, Size().iHeight >>1);
    }

TSize CHuiVg10Texture::ApplyMargin(VGImage aImage, TSize aSize, EGLDisplay aDisplay, EGLSurface aSurface, EGLContext aContext)
    {
    HUI_VG_INVARIANT();

#ifndef __WINSCW__  // Should possibly query the supported mode instead?
    VGImageFormat imageInternalFormat = VG_sARGB_8888_PRE;
#else
    // This doesn't work in the Emulator anyways.. => remove?
    VGImageFormat imageInternalFormat = VG_sARGB_8888;
    
    // If the icon is also a current EGL surface, the getImageSubData
    // won't succeed and return "image in use" -error..
    if ( eglMakeCurrent( aDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT ) == EGL_FALSE )
        {
        HUI_DEBUG1(_L("CHuiVg10Texture::ApplyMargin() - EGL NO_Surface could not be made current, eglErr: %04x"), eglGetError());
        return aSize;
        }
#endif
    
    TInt stride = aSize.iWidth * 4; // VG_sARGB_8888(_PRE) is four bytes long (8888)
    HBufC8* buf = HBufC8::New(stride);
    if (!buf)
        {
        HUI_DEBUG(_L("CHuiVg10Texture::ApplyMargin() - Ran out of memory!"));
        return aSize;        
        }
    TUint32* ptr = (TUint32*)(buf->Des()).Ptr();
    
    const TInt lValidMargin = aSize.iHeight * 12 / 100;
    
    const TInt Ha = aSize.iHeight;
    TInt hTa = 0;
    TInt hNT = 0;
    TInt C = 0;
    TInt hNTN = Ha - 2.0 * 0.12 * Ha;
    TReal R = 1.0;
    TInt HaN = Ha;
    
    const TInt lastColumn = aSize.iWidth - 1;
    for (TInt curRow = 0; curRow < lValidMargin; curRow++)
        {
        const TInt y = (aSize.iHeight - 1) - curRow; // h - 1 is the last line
        // Get just one stride at a time (iWidth wide, 1 pixel high)
        vgGetImageSubData(aImage, ptr, stride, imageInternalFormat, 0, y, aSize.iWidth, 1);
        for (TInt s = lastColumn; s >= 0; --s)
            {
            if (ptr[s] & 0xFF000000)
                {
                hTa = curRow;
                hNT = Ha - 2 * hTa;
                C = 2 * hTa;
                R = ( ( (TReal)hNTN / (TReal)hNT ) > 1.0 ) ? 1 : (TReal)hNTN / (TReal)hNT;
                HaN = Ha * R - C * R + C;
                curRow = lValidMargin; // to exit the outer loop
                break; // to exit the inner
                }
            }
        }
    delete buf;
    HUI_VG_INVARIANT();
    
#ifdef __WINSCW__    
    // Make the PBuffer surface current again 
    if ( eglMakeCurrent(aDisplay, aSurface, aSurface, aContext) == EGL_FALSE )
        {
        HUI_DEBUG1(_L("CHuiVg10Texture::ApplyMargin() - EGL aSurface could not be made current, eglErr: %04x"), eglGetError());
        return aSize;
        }
#endif
    
    // If icon size has to be changed, clear out old area for new DrawNVG round!
    if(aSize.iHeight > HaN)
        {
        vgLoadIdentity();
        
        VGfloat color[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
        vgSetfv(VG_CLEAR_COLOR, 4, color);
        vgClear(0, 0, aSize.iWidth, aSize.iHeight);
        // Or should it be clearImage instead?
        //vgClearImage(aImage, 0, 0, aSize.iWidth, aSize.iHeight);
        
        VGfloat Hr = (VGfloat)HaN/(aSize.iHeight);        
        TInt WaN = aSize.iWidth*Hr;
        
        VGfloat Tx = (aSize.iHeight-HaN)/2;
        VGfloat Ty = (aSize.iWidth-WaN)/2;
        vgTranslate(Tx,Ty);
        
        HUI_VG_INVARIANT();
        return(TSize(HaN,WaN));
        }
    
    HUI_VG_INVARIANT();
    return aSize;
    }

void CHuiVg10Texture::SetIconColor(VGImage& aSrcImage, TSize aSize, TUint32 aColor)
    {
    // TODO: DOESN'T WORK IN EMULATOR FOR SOME REASON! Figure out why!
    HUI_VG_INVARIANT();
    
    vgSeti(VG_FILTER_FORMAT_LINEAR, VG_FALSE);
    
#ifndef __WINS__ // Should possibly query the supported mode instead?
    VGImageFormat imageInternalFormat = VG_sARGB_8888_PRE;
    // TODO: !!! SHOULD FORMAT_PREMULTIPLIED BE APPLIED?? !!!
    //vgSeti(VG_FILTER_FORMAT_PREMULTIPLIED, VG_TRUE);
#else
    // This doesn't work in the Emulator anyways.. => remove?
    VGImageFormat imageInternalFormat = VG_sARGB_8888;
#endif
    
    // Rip the RGB components from the aColor param & scale to [0..1]
    VGfloat red      = (aColor&0xff)/255.0f;
    VGfloat green    = ((aColor>>8)&0xff)/255.0f;
    VGfloat blue     = ((aColor>>16)&0xff)/255.0f;
    
    //vgSeti(VG_FILTER_CHANNEL_MASK, VG_ALPHA );
    // This matrix will set all color components in the source image to 0,
    // and replaces the RGB with the values from the iconheader 
    VGfloat matrix[20] = {
            0,0,0,0,
            0,0,0,0,
            0,0,0,0,
            0,0,0,1, // <- Preserves the original alpha value
            red, green, blue, 0};
    
    // Create the destination image & start modifying colors
    VGImage dstImg = vgCreateImage(imageInternalFormat, aSize.iWidth, aSize.iHeight, VG_IMAGE_QUALITY_NONANTIALIASED);
    vgColorMatrix(dstImg, aSrcImage, matrix);
    HUI_VG_INVARIANT();
    
    // Destroy the old source image and return the new image with changed colors
    vgDestroyImage(aSrcImage);
    aSrcImage = dstImg;
    
    HUI_VG_INVARIANT();
    }

void CHuiVg10Texture::ReplaceVGImageAlphaChannelL(VGImage aImage, VGImage aMaskImage, TSize aSize)
    {
    HUI_VG_INVARIANT();
    
#ifndef __WINS__ // Should possibly query the supported mode instead?
    VGImageFormat imageInternalFormat = VG_sARGB_8888_PRE;
#else
    VGImageFormat imageInternalFormat = VG_sARGB_8888;
#endif
    TInt height = aSize.iHeight;
    TInt width = aSize.iWidth;
    const TInt colorDepth = 4;  // VG_sARGB_8888(_PRE) 4 bytes long
    TInt stride = width * colorDepth;
    
    // Create the pointers to RAM in which the image data will be saved temporarily
    HBufC8* imgBuf = HBufC8::NewL(width * height * colorDepth);
    CleanupStack::PushL(imgBuf);
    TUint32* imgPtr = (TUint32*)(imgBuf->Des()).Ptr();
    const TUint32* imgSavePtr = (TUint32*)(imgBuf->Des()).Ptr(); 
    
    HBufC8* maskBuf = HBufC8::NewL(width * height * colorDepth);
    CleanupStack::PushL(maskBuf);
    TUint32* maskPtr = (TUint32*)(maskBuf->Des()).Ptr();
    
    // Get the image pixel data
    vgGetImageSubData(aImage, imgPtr, stride, imageInternalFormat, 0, 0, width, height);
    vgGetImageSubData(aMaskImage, maskPtr, stride, imageInternalFormat, 0, 0, width, height);
    
    // Replace destination image's alpha values with mask image
    for (TInt y = 0; y < height; y++)
        {
        for (TInt x = 0; x < width; x++)
            {
            *maskPtr    &= 0xff000000;      // Remove other than alpha component from source pixel
            *imgPtr     &= ~0xff000000;     // Remove alpha channel value from destination pixel
            *imgPtr++   |= *maskPtr++;      // Bitwise OR the maskImg alpha info to destination image pixel 
            }
        }

    // Replace the destination image with combined alpha information from src image
    vgImageSubData(aImage, imgSavePtr, stride, imageInternalFormat, 0, 0, width, height);

    CleanupStack::PopAndDestroy(maskBuf);
    CleanupStack::PopAndDestroy(imgBuf);
    
    HUI_VG_INVARIANT();
    }
#endif


void CHuiVg10Texture::PushEGLContext()
    {
    iPreviousEGLState.iContext= eglGetCurrentContext(); 
    TEGLState& state = iRenderPlugin.GetUploadState();
    if (state.iContext == KErrNotFound)
        {
		TEGLState& state = iRenderPlugin.GetUploadState();
        // the first context used for uploading will be used for all texture uploads
        state.iContext      = iPreviousEGLState.iContext; 
        state.iDrawSurface  = eglGetCurrentSurface(EGL_DRAW);
        state.iReadSurface  = eglGetCurrentSurface(EGL_READ);
        state.iDisplay      = eglGetCurrentDisplay();
        }
    else
        {
        // change context only if necessary
        if (iPreviousEGLState.iContext != state.iContext)
            {
            iPreviousEGLState.iDrawSurface  = eglGetCurrentSurface(EGL_DRAW);
            iPreviousEGLState.iReadSurface  = eglGetCurrentSurface(EGL_READ);
            iPreviousEGLState.iDisplay      = eglGetCurrentDisplay();
            eglMakeCurrent(state.iDisplay, state.iDrawSurface, state.iReadSurface, state.iContext);
            }
        }
    }

void CHuiVg10Texture::PopEGLContext()
    {
    if (iPreviousEGLState.iContext != iRenderPlugin.GetUploadState().iContext)
        {
        eglMakeCurrent(iPreviousEGLState.iDisplay,  iPreviousEGLState.iDrawSurface, iPreviousEGLState.iReadSurface,iPreviousEGLState.iContext);
        }
    }
// End of file

