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
* Description:   Defines CHuiVg10Texture, an OpenVG-based texture class.
*
*/



#ifndef __HUIVG10TEXTURE_H__
#define __HUIVG10TEXTURE_H__


#include <bldvariant.hrh>
#include <e32base.h>
#include <e32std.h>
#include <gdi.h>
#include <VG/openvg.h>

#include "uiacceltk/HuiTexture.h"
#include "uiacceltk/HuiTextureHandle.h"
#include "HuiVg10RenderPlugin.h"

struct THuiTexturePartialBitmapUploadParams;
#ifdef __NVG
    #include <AknIconHeader.h>
    class CNvgEngine;
    class MNVGIcon;
#endif

/**
 * CHuiVg10Texture wraps an OpenVG texture object. It provides methods 
 * for uploading image data. When an instance of CHuiVg10Texture is 
 * destroyed, the OpenVG texture object is deleted as well.
 *
 * @todo  Proper segment support. Support more than two segments.
 * @todo  Placeholder should be a THuiTextureHandle.
 */
NONSHARABLE_CLASS(CHuiVg10Texture) : public CHuiTexture
    {
public:

    /* Constructors and destructor. */

    /**
     * Constructs a new texture object. The texture has no segments initially.
     *
     * @param   aRenderPlugin The render plugin that created this texture.
     * @return  Texture object.
     */
    static CHuiVg10Texture* NewL(CHuiVg10RenderPlugin& aRenderPlugin, const THuiTextureHandle* aExistingTexture = 0);

    /**
     * Constructs a new texture object and leaves it on the cleanup stack.
     * The texture has no segments initially.
     *
     * @param   aRenderPlugin The render plugin that created this texture.
     * @return  Texture object.
     */
    static CHuiVg10Texture* NewLC(CHuiVg10RenderPlugin& aRenderPlugin, const THuiTextureHandle* aExistingTexture = 0);


    /** @beginAPI */

    /**
     * Destructor.
     */
    virtual ~CHuiVg10Texture();


    /* Methods. */
    
    TBool HasContent() const; // override

    /**
     * Determines if the texture has an alpha channel.
     *
     * @return  <code>ETrue</code>, if the texture has an alpha channel.
     *          Otherwise, <code>EFalse</code>.
     */
    TBool HasAlpha() const;

    /**
     * Determines the maximum possible actual (OpenVG) size for
     * textures. This limit is usually set by the underlying 
     * graphics driver/hardware.
     * @todo should be moved out?
     */
    TSize MaxTextureSize() const;

    void UploadL(const CFbsBitmap& aBitmap,
                 const CFbsBitmap * aMaskBitmap = NULL,
                 THuiTextureUploadFlags aFlags = EHuiTextureUploadFlagDefault);

    void UploadL(THuiTextureFormat aFormat,
                 const TSize& aSize,
                 const TUint8* aBuffer,
                 THuiTextureUploadFlags aFlags = EHuiTextureUploadFlagDefault,
                 TUint aBufferSize = 0);

    void PartialUploadL(THuiTextureFormat aFormat,
                        const TPoint& aOrigin,
                        const TSize& aSize,
                        const TUint8* aBuffer);

    void SegmentUploadL(TInt aSegment,
                        const CFbsBitmap& aBitmap,
                        const CFbsBitmap * aMaskBitmap = NULL,
                        THuiTextureUploadFlags aFlags = EHuiTextureUploadFlagDefault);

    void SegmentUploadL(TInt aSegment,
                        THuiTextureFormat aFormat,
                        const TSize& aSize,
                        const TUint8* aBuffer,
                        TUint aBufferSize = 0);

    void SegmentPartialUpload(TInt aSegment,
                              THuiTextureFormat aFormat,
                              const TPoint& aOrigin,
                              const TSize& aSize,
                              const TUint8* aBuffer);

    void SegmentClearL(TInt aSegment,
                       TBool aWithAlpha,
                       const TRgb& aColor,
                       TUint8 aAlpha);

    void SetupSegmentsL(const TSize& aLogicalSize,
                        const TSize& aTextureSize,
                        THuiTextureUploadFlags aFlags = EHuiTextureUploadFlagDefault);

    /** @endAPI */

    /**
     * Delete the texture.
     */
    void Reset();

    void EnableShadow(TBool aEnable = ETrue);

    TBool IsShadowEnabled() const;

    TBool GetShadowTexture(THuiTextureHandle& aHandle) const;
    
#ifdef __NVG
    /**
     * Creates a normal texture with the VGImage from extended texture
     * 
     * @param   aNvgEngine  Used to draw the extended bitmap data on the VGImage, if ObjectCached icon wasn't found
     * @param   aDestSize   Destination size for the VGImage
     * @return  Handle to the rasterized image or <code>VG_INVALID_HANDLE</code> if something went wrong.
     */ 
    VGImage CreateRenderedImage(CNvgEngine* aNvgEngine, HBufC8* aNVGData, const TSize& aDestSize, TInt64 aSerialNumber = 0);
    
    /** 
     * Identifies whether this texture contains extended texture data
     */
    TBool IsExtended() const;
    
    /**
     * Gets the extended texture data. Prefer the GetIconCommandsData() instead.
     * 
     * @return  The OpenVG command buffer for the texture
     */
    HBufC8* GetExtendedTextureData() const;
    
    /**
     * Gets the VG ObjectCached NVG icon, which can be drawn directly by calling the DrawL() method 
     * 
     * @return  The OpenVG Object Cached NVG icon
     */
    MNVGIcon* GetIconCommandsData() const;
#endif
    
protected:

    /* Constructors. */

    /**
     * Constructor. Creates a new texture object with no texture maps.
     */
    CHuiVg10Texture(CHuiVg10RenderPlugin& aRenderPlugin);

    /**
     * Second-phase constructor.
     * @param   aRenderPlugin The render plugin that created this texture.     
     */
    void ConstructL(const THuiTextureHandle* aExistingTexture);


    /* Methods. */

    /**
     * Deallocate resources associated with segment. Will free
     * all resources associated with a texture segment.
     * @see SetSegmentCount()
     */
    virtual void ResetSegment(TInt aSegment);

    /**
     * Initialize segment for holding texture data. Allocate texture name etc.
     */
    virtual void InitSegmentL(TInt aSegment);

    // from MHuiTexture
    
    void TextureExtension(const TUid& aExtensionUid, TAny** aExtensionParameters); 

private:
    /**
     *  Define the contents of an image using raw data.
     * 
     *  @param aCreateImage If ETrue, a new image is created, otherwise an existing image is updated.
     * 
     *  @see SegmentPartialUpload
     */
    void SegmentPartialUploadInternal(TInt aSegment, THuiTextureFormat aFormat, const TPoint& aOrigin,
                                      const TSize& aSize, const TUint8* aBuffer, TBool aCreateImage);


    /**
     *  Generate a shadow for this texture if shadowing is enabled.
     */
    void GenerateShadowL();
    
    /**
     *  Make sure the shadow texture is of the proper dimensions.
     */
    void UpdateShadowSizeL(const TSize& aSize);

    /**
     * Partial bitmap upload.
     */
    void PartialBitmapUpload(THuiTexturePartialBitmapUploadParams* aParams);
    
    /** The render plugin that created this texture. */
    CHuiVg10RenderPlugin& iRenderPlugin;
    
    /** Flags indicating the state of the image object */
    TInt iInternalFlags;
    
    /** Shadow texture */
    THuiTextureHandle iShadow;
    
    /** Should a shadow texture be generated */
    TBool iShadowEnabled;
    
#ifdef __NVG
    /**
     * Reads the NVG data from the extended CFbsBitmap
     * 
     * @param   aBitmap The extended bitmap, which carries the NVG data to be read
     * @return  The read NVG data in a 8-bit descriptor
     */
    HBufC8* ReadNVGDataL(const CFbsBitmap& aBitmap);
    
    /**
     * Compares two NVG data buffers. Ignores the iconheader part.
     */
    TInt CompareNvgData(HBufC8* aNVGData1, HBufC8* aNVGData2);
    
    /**
     * Uploads the NVG data into the texture.
     * 
     * @param aBitmap The bitmap that is to be uploaded. Includes the extended NVG data.
     * @param aFlags Texture upload flags, either EHuiTextureUploadFlagUsePureNvg => upload as object cached NVG. Otherwise will be rasterized into VGImage.
     */
    void SegmentUploadNvgL(const CFbsBitmap& aBitmap, const CFbsBitmap* aMaskBitmap, THuiTextureUploadFlags aFlags);
    
    /**
     * Create the Object cached icon out of the NVG data. Does the parsing of the icon header as well.
     */
    void CreateObjCachedNVGIconL();
    
    /**
     * Parses the NVG icon header out of the NVG data.
     * 
     * @param   aNVGData The NVG formatted data, from which to parse the icon header
     * @return  The iconheader information
     */
    TAknIconHeader GetNvgIconHeader(HBufC8* aNVGData);

    /**
    * This method has to be called before calling nvgEngine->DrawNvg().
    * 
    * @param   aNVGData Nvg data buffer
    * @return  Descriptor to the NVG data without the additional header information.
    */
    TPtr8 GetNvgDataWithoutHeader(HBufC8* aNVGData);
    
    /**
     * Sets the NVG icon aspect ratio and rotation (from the NVG icon header).
     * 
     * @param   aNvgEngine NvgEngine instance used for setting the aspect ratio.
     * @param   aNVGData Nvg data buffer
     * @param   aIgnoreAspectRatio Use this, if you want to set the rotation, but ignore the aspect ratio.
     */
    void SetNvgParamsFromIconHeader(CNvgEngine& aNvgEngine, HBufC8* aNVGData);
    
    /**
     * Calculates new iconsize that will have at least the required 12% margin.
     * 
     * @param   aVgImage Image that has been rasterized from the NvgData, used for determining if there's enough margin or not
     * @param   aSize Size for drawing the image. Used as a base for margin calculations.
     * @param   aDisplay EGLDisplay, needed for making the VGImage based ClientPBuffer current again.
     * @param   aSurface EGLSurface, -||-
     * @param   aContext EGLContext, -||-
     * @return  New size with the correct margin.
     */
    TSize ApplyMargin(VGImage aVgImage, TSize aSize, EGLDisplay aDisplay, EGLSurface aSurface, EGLContext aContext);
    
    /**
     * Sets the color for the icon, if such is defined in the icon header.
     * 
     * @param   aSrcImage Image that has been rasterized from the NvgData, used as a source for the color transformation.
     *          Will be replaced by the new image got from transformation.
     * @param   aSize Size for creating destination image.
     * @param   aColor Color from the iconheader, used to fill the icon with it.
     */
    void SetIconColor(VGImage& aSrcImage, TSize aSize, TUint32 aColor);
    
    /**
     * Replaces the alpha channel information within aImage, with the alpha mask
     * information from the aMaskImage. Both images have to be in VG_sARGB_8888 format.
     * 
     * @param   aImage VGImage, which's alpha channel is to be replaced.
     * @param   aMaskImage a mask VGImage.
     * @param   aSize Size of the images.
     */
    void ReplaceVGImageAlphaChannelL(VGImage aImage, VGImage aMaskImage, TSize aSize);
    
    /*
     * Assertains that all texture uploads are done into the same EGL context as the first upload.
     * Changes context if necessary.
     */
    void PushEGLContext();
    
    /*
     * Rolls back to the previous EGL context, if necessary.
     */
    void PopEGLContext();
    
	// Rolling back to state for PopEGLContext
    TEGLState iPreviousEGLState;
    
    /** Identifies whether this texture has extended bitmap NVG data **/
    TBool iIsExtended;
    
    /** Carries the extended NVG data, if any **/
    HBufC8* iNVGData;
    
    /** 
     * Stores the ObjectCached VG commands, which are ready to be drawn without
     * the need to parse the NVG data and create the VG objects again
     */
    MNVGIcon* iIconCommands;
#endif
    
    };

#endif  // __HUIVG10TEXTURE_H__
