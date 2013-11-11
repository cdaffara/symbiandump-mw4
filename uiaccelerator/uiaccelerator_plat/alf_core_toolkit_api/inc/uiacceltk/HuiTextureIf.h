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
* Description:   Declares the MHuiTexture interface. MHuiTexture is the most 
*                generic interface that all textures need to implement.
*
*/



#ifndef __HUITEXTUREIF_H__
#define __HUITEXTUREIF_H__


/* Forward declarations. */
class MHuiSegmentedTexture;
class MHuiShadowedTexture;


/** Data buffer formats for uploading textures. */
enum THuiTextureFormat
    {
    /** RGB, 5-6-5 bits per channel. */
    EHuiTextureFormatRgb565,

    /** RGB, 8 bits per channel. */
    EHuiTextureFormatRgb888,

    /** RGBA, 8 bits per channel. */
    EHuiTextureFormatRgba8888,

    /** Luminance + Alpha, 8 bits per channel. */
    EHuiTextureFormatLa88,

    /** RGB, PVRTC 4 bpp. */
    EHuiTextureFormatRgbPVRTC4,

    /** RGBA, PVRTC 4 bpp. */
    EHuiTextureFormatRgbaPVRTC4,

    /** RGB, PVRTC 2 bpp. */
    EHuiTextureFormatRgbPVRTC2,

    /** RGBA, PVRTC 2 bpp. */
    EHuiTextureFormatRgbaPVRTC2
    };


/** Flags for texture uploading. */
enum THuiTextureUploadFlags
    {
    /** No special actions/behavior needed. */
    EHuiTextureUploadFlagDefault = 0,

    /**
     * @deprecated This is default functionality 
     */
    EHuiTextureUploadFlagRetainResolution = 1,

    /**
     * Automatically generates a shadow for the texture in texture upload
     * phase.
     */
    EHuiTextureUploadFlagGenerateShadow = 2,

    /**
     * Use to retain the color depth of the
     * ((up)loaded) image as well as possible. Otherwise
     * the toolkit may reduce the color depth of the texture
     * to conserve memory.
     */
    EHuiTextureUploadFlagRetainColorDepth = 4,

    // note: value 0x8 is reserved in client server model so we leave hole here

    /**
     * Set if it is not necessary to retain the
     * full resolution of the bitmap (tries to match texture resolution
     * with bitmap resolution).
     * This may save some memory with hardware acceleration, but image quality changes to worse
     */
    EHuiTextureUploadFlagDoNotRetainResolution = 0x10,

    /**
     * Use the provided bitmap directly for the texture, without conversion or copying. The bitmaps
     * should be in the correct format and they shall not be compressed or have duplicated handle.
     * If the direct bitmap cannot be used, this flag is ignored internally.
     */
    EHuiTextureFlagAllowDirectBitmapUsage = 0x20,

    /**
     * If given to texture's SegmentUpload() while bitmap being uploaded is in NVG format,
     * the texture will have NVG ObjectCached data created for quicker drawing. 
     * 
     * By default if no flags is given as parameters, the NVG data will be rasterized
     * into PBuffer (=VGImage) during upload, since using VGImages is the fastet way.
     */
    EHuiTextureUploadFlagUsePureNvg = 0x40
    };


/**
 * MHuiTexture is the most generic interface that all texture classes need to
 * implement. Using MHuiTexture, it is possible to determine which interfaces
 * a specific texture supports.
 */
class MHuiTexture
    {
public:

    /* Destructor. */

    /**
     * Virtual destructor.
     */
    IMPORT_C virtual ~MHuiTexture();


    /* Interface methods. */

    /**
     * @return  The texture as a MHuiSegmentedTexture interface, or NULL is
     *          the texture does not implement the segmented texture interface.
     */
    virtual MHuiSegmentedTexture* SegmentedTexture() = 0;

    virtual const MHuiSegmentedTexture* SegmentedTexture() const = 0;

    /**
     * @return  The texture as a MHuiShadowedTexture interface, or NULL is
     *          the texture does not implement the shadowed texture interface.
     */
    virtual MHuiShadowedTexture* ShadowedTexture() = 0;

    virtual const MHuiShadowedTexture* ShadowedTexture() const = 0;

    /**
     * Returns true if the texture has changed since the last change
     * notification. Changed means that the bitmap data of the texture
     * has been modified, and must be re-uploaded to texture memory.
     */
	virtual TBool TextureChanged() const = 0;

    /**
     * Clears the changed status of the visual.
     */
	virtual void TextureClearChanged() const = 0;    
	
    /**
     * Determines whether the texture has content. Only textures that
     * have content can be drawn.
     *
     * Note that a texture that has not finished loading will return EFalse
     * here, because its content will only become available after it has
     * been fully loaded.
     *
     * @return ETrue, if this texture has content.
     */
    virtual TBool HasContent() const = 0;
    
    virtual void TextureExtension(const TUid& aExtensionUid, TAny** aExtensionParameters) = 0; 

    };


#endif // __HUITEXTUREIF_H__
