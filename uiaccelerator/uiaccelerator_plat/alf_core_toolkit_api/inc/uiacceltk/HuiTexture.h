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
* Description:   Declares CHuiTexture texture base class and THuiTextureFormat 
*                texture data buffer formats.
*
*/



#ifndef __HUITEXTURE_H__
#define __HUITEXTURE_H__


#include <e32base.h>
#include <e32std.h>
#include <gdi.h>

#include <uiacceltk/HuiTextureIf.h>
#include <uiacceltk/HuiTextureHandle.h>
#include <uiacceltk/HuiSegmentedTexture.h>
#include <uiacceltk/HuiShadowedTexture.h>
#include <uiacceltk/HuiObserverArray.h>
#include <uiacceltk/HuiSessionObject.h>
#include <uiacceltk/HuiRealSize.h>
#include <uiacceltk/huitextureautosizeparams.h>

// Uids for the extension functions used by TextureExtension()
const TUid KHuiTextureExtUid_UploadDirectL = {0x200100C3};

/* Forward declaration. */
class CHuiTexture;


/**
 * Texture content observing interface.
 */
class MHuiTextureContentObserver
    {
public:

    virtual ~MHuiTextureContentObserver() 
        {
        }
    
    /**
     * Called when new content has been uploaded to the specified texture.
     * 
     * The implementor should call the relevant methods in CHuiTexture to
     * get more information about the new content (CFbsBitmaps or raw data;
     * these are available while the notification is taking place).
     *
     * @param aTexture  Texture that has received new content.
     */
    virtual void TextureContentUploaded(CHuiTexture& aTexture) = 0;

    /**
     * Called when the texture's content has been released.
     *
     * @param aTexture  Texture that has been released.
     */
    virtual void TextureContentReleased(CHuiTexture& aTexture) = 0;
    
    /**
     * Called when the content of the texture needs to be restored to the
     * same state it was when the texture was released.
     *
     * @param aTexture  Texture that has been restored.
     */
    virtual void RestoreTextureContentL(CHuiTexture& aTexture) = 0;
        
    };



/**
 * CHuiTexture is the base class for all texture objects. It implements
 * the MHuiSegmentedTexture interface for multipart support. Multi-segment
 * textures can be operated in the same way as one would operate
 * single-segment textures.
 *
 * The application will only need to call a  method with "Segment" in its name
 * if it explicitly wants to manipulate a  specific segment in a texture.
 * (E.g., when it wants to upload a new  sub-texture into the segment.)
 *
 * The base class enables CHuiTexture to handle large textures as smaller
 * texture segments. Each segment has a position, dimensions, and a name that
 * identifies the texture image of the segment. The purpose is that it is
 * possible to compose large images out of smaller segments. The segments can
 * overlap.
 *
 * One use case is where the OpenGL texture size is limited (e.g., 256x256).
 * CHuiTexture can split the uploaded source image to multiple
 * segments and create a different OpenGL texture for each. All of the segments
 * are hidden inside CHuiTexture, though, so it could be treated as
 * a single entity.
 *
 * A CHuiTexture always represents one or more fully loaded (OpenGL ES) texture
 * objects. When drawing something, THuiImages are used for determining the
 * area of  the texture to draw. However, different segments may utilize
 * textures in different resolutions.
 *
 * @see THuiImage
 * @see CHuiGc
 *
 */
class CHuiTexture : public CBase, public MHuiTexture,
                    public MHuiSegmentedTexture, public MHuiShadowedTexture,
                    public MHuiSessionObject
    {
public:

    /* Constructors and destructor. */

    /** @beginAPI */

    /**
     * Exported static factory method. Creates a texture appropriate for the current
     * renderer.
     */
    IMPORT_C static CHuiTexture* NewL(const THuiTextureHandle* aExistingTexture = 0);

    /**
     * Exported static factory method. Creates a texture appropriate for the current
     * renderer.
     */
    IMPORT_C static CHuiTexture* NewLC(const THuiTextureHandle* aExistingTexture = 0);

    /**
     * Destructor.
     */
    IMPORT_C ~CHuiTexture();


public: // new methods
    /**
     * Returns the file name of the texture's source image (if any).
     */
    IMPORT_C const TDesC& ImageFileName() const;

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
    IMPORT_C virtual TBool HasContent() const;



    /* CHuiTexture pure virtual methods (interface methods). */

    /**
     * Determines if the texture has an alpha channel.
     *
     * @return  <code>ETrue</code>, if the texture has an alpha channel.
     *          Otherwise, <code>EFalse</code>.
     */
    virtual TBool HasAlpha() const = 0;

    /**
     * Determines the maximum possible actual (OpenGL/in-memory) size for
     * textures. Typically returns power-of-two dimensions. This limit is
     * usually set by the underlying graphics driver/hardware.
     */
    virtual TSize MaxTextureSize() const = 0;

    /**
     * Uploads texture data from an FBS bitmap onto this texture.
     *
     * Bitmap to texture upload consists of the following tasks:
     * <ul>
     *    <li> Setups the texture (logical) size.</li>
     *    <li> Calculates if segmentation is needed, and setups the segments
     *         (as necessary / optional).</li>
     *    <li> Convert / downscale and crop the bitmap data to create bitmap(s)
     *         suitable for uploading.</li>
     *    <li> Transfers bitmap data to the graphics device driver.
     * </ul>
     *
     * If the texture uses EHuiTextureFlagAllowDirectBitmapUsage flag, the method
     * CHuiTextureManager::UpdateTextureFromBitmapL() should be used instead. This 
     * method leaves with KErrNotSupported in that case.  
     *
     * @param aBitmap       Bitmap to copy image contents and format from
     *                      (RGB data).  The support for input CFbsBitmap formats 
     *                      varies between different renderers. 
     *                      See \ref cfbsbitmapsupport Table describing renderer 
     *                      bitmap support. 
     * 
     * @param aMaskBitmap   Optional Bitmap for alpha (transparency) channel.
     *                      If the image does not require an alpha channel or the
     *                      aBitmap already has alpha information (RGBA image)
     *                      this value may be left as NULL. The bitmap will be
     *                      combined with aBitmap to single RGBA image. If used,
     *                      this alpha mask bitmap will replace any alpha
     *                      information in aBitmap.
     * 
     * @param aFlags        Specify texture upload behavior - how to convert the
     *                      bitmap to texture. You can OR these flags together.
     *
     * If the UploadL runs out of memory while uploading/converting
     * the method may try to upload with smaller bitmap sizes and
     * force use of more memory-efficient single-segment texture.
     *
     * @see SetSize() Use if you want to change the logical size (for example to
     * fix the aspect ratio of the bitmap) of the texture after uploading.
     */
    virtual void UploadL(const CFbsBitmap& aBitmap,
                         const CFbsBitmap * aMaskBitmap = NULL,
                         THuiTextureUploadFlags aFlags = EHuiTextureUploadFlagDefault) = 0;

    /**
     * Uploads bitmap data onto this texture.
     *
     * Bitmap to texture upload consists of the following tasks:
     * <ul>
     *    <li> Setups the texture (logical) size.</li>
     *    <li> Transfers bitmap data to the graphics device driver.
     * </ul>
     *
     * Note: This method does NOT support texture segmentation. See the
     * CFbsBitmap upload methods (UploadL()) to create segmented textures.
     *
     * @param aFormat  Format of aBuffer, the image data buffer.
     * @param aSize    Size (width and height in pixels) of the texture in the
     *                 data buffer (aBuffer).
     * @param aBuffer  Source Image data buffer.
     * @param aFlags   Specify upload behavior - how to convert the bitmap to texure.
     *
     * @param aBufferSize Length of the compressed image data. Set as 0 for
     * non-compressed formats.
     */
    virtual void UploadL(THuiTextureFormat aFormat,
                         const TSize& aSize,
                         const TUint8* aBuffer,
                         THuiTextureUploadFlags aFlags = EHuiTextureUploadFlagDefault,
                         TUint aBufferSize = 0) = 0;

    /**
     * Updates a partial area of this texture. Does not change the
     * dimensions of the texture (logical size or segmentation).
     *
     * @note You must first call Upload() at least once so the texture object
     * has dimensions.
     *
     * No local copy of the buffer data
     * is made.
     *
     * @note Does not work with multisegmented textures!
     *
     * @param aFormat  Format of the data buffer.
     * @param aOrigin  Position to place the partial texture data.
     * @param aSize    Size of the texture in the data buffer.
     * @param aBuffer  Texture data buffer.
     */
     virtual void PartialUploadL(THuiTextureFormat aFormat,
                       const TPoint& aOrigin,
                       const TSize& aSize,
                       const TUint8* aBuffer) = 0;

    /**
     * Clears the texture object and initializes it with solid color pixels. Will
     * setup the logical size and reserve space for the texture if size and
     * segmentation has not yet been prepared.
     *
     * @param aSize Size of the texture. Does not matter if the size
     * and segmentation of this texture has already been set up.
     * @param aColor Initial color of the texture.
     * @return System error code if task failed and KErrNone if successful.
     */
    IMPORT_C TInt ClearL(const TSize& aSize, const TRgb& aColor);

    /**
     * Clears the texture object and initializes it with solid color pixels.
     * The texture will be initialized with alpha data. Will
     * setup the logical size and reserve space for the texture, if needed.
     *
     * @param aSize Size of the texture. Does not matter if the size
     * and segmentation of this texture has already been set up. Use
     * Reset() before calling Clear() to change the texture resolution and
     * dimensions.
     * @param aColor  Initial color of the texture.
     * @param aAlpha  Initial alpha level of the texture.
     * @return System error code if task failed and KErrNone if successful.
     */
    IMPORT_C TInt ClearWithAlphaL(const TSize& aSize, const TRgb& aColor, TUint8 aAlpha);



    /**
     * @internal
     *
     * Creates shadow with given parameters. If the shadow already exists with the
     * given paramters, this function does not do a thing.
     *
     * @param aBlurDimension 2*radius used by the blur filter
     * @param aStyle Defines the used shadow texture size
     */
    void CreateShadowTextureL( TInt aBlurDimension, THuiShadowStyle aStyle );
    
    /**
     * @internal
     *
     * Fetches shadow with given parameters.
     *
     * @param aHandle Will be filled with the shadow information if found.
     *                i.e. the function returns ETrue,
     * @param aBlurDimension 2*radius used by the blur filter
     *
     * @return ETrue if a shadow texture is found with the given dimension.
     */
    TBool GetShadowTexture( THuiTextureHandle& aHandle, TInt aBlurDimension ) const;
        

public: // from baseclasses

    /* Implementation of MHuiTexture. */

    IMPORT_C MHuiSegmentedTexture* SegmentedTexture();

    IMPORT_C const MHuiSegmentedTexture* SegmentedTexture() const;

    IMPORT_C MHuiShadowedTexture* ShadowedTexture();

    IMPORT_C const MHuiShadowedTexture* ShadowedTexture() const;

    /* Implementation of MHuiShadowedTexture. */

    // @deprecated 
    IMPORT_C virtual void EnableShadow(TBool aEnable = ETrue);
    
    // @deprecated 
    IMPORT_C virtual TBool IsShadowEnabled() const;

    /**
     * Sets the shadow generation style of the texture.
     *
     * @param aShadowStyle  Shadowing style.
     */
    IMPORT_C virtual void SetShadowStyle(THuiShadowStyle aShadowStyle);

    /**
     * Determines the current shadow style.
     */
    IMPORT_C THuiShadowStyle ShadowStyle() const;
    
    IMPORT_C TBool GetShadowTexture(THuiTextureHandle& aHandle) const;


    /**
     * Set the logical dimensions of the texture. This is the original size
     * of the image loaded as a texture. Also will update (resize) the
     * logical sizes and offsets of texture segments, if such segments have
     * been previously configured.
     *
     * @see Size() Use to check the logical size of the texture.
     */
    IMPORT_C virtual void SetSize(const TSize& aSize);


    /* Implementation of MHuiSegmentedTexture. */

    /**
     * Return logical dimensions of texture.
     * @return The logical dimensions of the texture.
     * @see SetSize()
     */
    
    IMPORT_C TSize Size() const;

    /**
     * Number of segments in the texture.
     * @return The number of segments in the texture.
     */
    IMPORT_C TInt SegmentCount() const;

    /**
     * ID of the given segment.
     * @param aSegment The ordinal of the segment that we want the name of.
     * @return The ID of the segment at the passed ordinal.
     */
    IMPORT_C TUint SegmentName(TInt aSegment) const;

    /**
     * Return logical dimensions of a segment.
     * @param aSegment The ordinal of the segment that we want the dimensions of. 
     * @return The logical dimensions of the segment.
     */
    IMPORT_C TSize SegmentSize(TInt aSegment) const;

    /**
     * Return texture dimensions of a segment.
     * @param aSegment The ordinal of the segment that we want the texture dimensions of. 
     * @return The dimensions of the segment's texture.
     */
    IMPORT_C TSize SegmentTextureSize(TInt aSegment) const;

    /**
     * Return offset of a segment.
     * @param aSegment The ordinal of the segment that we want the offset of. 
     * @return The the segment's offset.
     */
    IMPORT_C TPoint SegmentOffset(TInt aSegment) const;

    /**
     * Has texture changed.
     * Check if the texture has changed since the last update.
     * @return ETrue if the texture has changed.
     */
	IMPORT_C virtual TBool TextureChanged() const;

    /**
     * Set texture as not changed.
     * Clear the changed flag of this texture once changes have been handled.
     * @see TextureChanged()
     */
	IMPORT_C virtual void TextureClearChanged() const;

    /**
     * Set texture as (not) changed.
     * Clear the changed flag of this texture once changes have been handled.
     * @see TextureChanged()
     */
	IMPORT_C void SetTextureChanged(TBool aChanged);

    /* Segmented texture data methods. */

    /**
     * Sets the number of segments. Will delete/create new segments as
     * needed.
     *
     * @param aSegmentCount  Number of segments.
     */
    IMPORT_C virtual void SetSegmentCountL(TInt aSegmentCount);

    /**
     * Sets the name of a segment.
     */
    IMPORT_C virtual void SetSegmentName(TInt aSegment, TUint aName);

    /**
     * Sets the size of a segment.
     */
    IMPORT_C virtual void SetSegmentSize(TInt aSegment, const TSize& aSegmentSize);

    /**
     * Sets the texture size of the segment.
     *
     * @param aSegment Valid segment index for this texture.
     */
    IMPORT_C virtual void SetSegmentTextureSize(TInt aSegment, const TSize& aSegmentTextureSize);

    /**
     * Sets the logical offset of a texture segment.
     *
     * @param aSegment Valid segment index for this texture.
     * @param aOffset Offset of the texture segment. Must be inside the logical
     * dimensions of the whole texture.
     */
    IMPORT_C virtual void SetSegmentOffset(TInt aSegment, const TPoint& aOffset);
    
    /**
     * Defines whether the texture is of skin content or not. If this texture is
     * a part of skin content this texture will be released and restored whenever
     * CHuiEnv::NotifySkinChangedL is called. This allows skin specific texture
     * observers to update the content of this texture depending on the skin and
     * it's parameters. Textures that are not part of skin content will be inaffected
     * by the call to the CHuiEnv::NotifySkinChangedL.
     * 
     * @see CHuiEnv::NotifySkinChangedL()
     * @see IsSkinContent()
     *
     * @param aSkinContent ETrue to set this texture as part of skin content. EFalse
     *                     to mark that this texture is not part of skin content.
     */
    IMPORT_C virtual void SetSkinContent(TBool aSkinContent);
    
    /**
     * Informs whether this texture is part of skin content or not. By default textures
     * are not part of skin content.
     *
     * @see SetSkinContent()
     *
     * @return ETrue if this texture is part of skin content, EFalse otherwise.
     */
    IMPORT_C virtual TBool IsSkinContent() const;

    /* CHuiTexture segmentation pure virtual methods (interface methods). */

    /**
     * Calculates and sets up segmentation for given size. If
     * the given size exceeds the available maximum texture
     * size, the area is split up to segments having dimensions no
     * more than the maximum texture size.
     *
     * @param aLogicalSize  Logical size of the full image.
     * @param aTextureSize  Texture resolution required, in pixels.
     * @param aUploadFlags  Specify segmentation behavior - how the uploaded
     *                      data is laid out to a texture.
     */
    virtual void SetupSegmentsL(const TSize& aLogicalSize,
                                const TSize& aTextureSize,
                                THuiTextureUploadFlags aFlags = EHuiTextureUploadFlagDefault) = 0;

    /**
     * Uploads bitmap data onto a texture segment. Does not
     * change the dimensions of the texture (logical size or segmentation).
     *
     * This method allows direct access to texture segments. @see UploadL()
     * if you do not care about texture segmentation.
     *
     * @note The image segment has to have valid dimensions.
     *
     * @param aSegment Valid segment index for this texture.
     * @param aFormat  Format of the image data buffer.
     * @param aSize    Size (width and height in pixels) of the texture in the
     * data buffer (aBuffer).
     * @param aBuffer  Image data buffer.
     * @param aBufferSize Length of the compressed image data. Set as 0 for
     * non-compressed formats.
     */
    virtual void SegmentUploadL(TInt aSegment,
                               THuiTextureFormat aFormat,
                               const TSize& aSize,
                               const TUint8* aBuffer,
                               TUint aBufferSize = 0) = 0;

    /**
     * Uploads FBS bitmap data onto a texture segment. Does not
     * change the dimensions of the texture (logical size or segmentation).
     *
     * This method allows direct access to texture segments. @see UploadL()
     * if you do not care about texture segmentation.
     *
     * @note The image segment has to have valid dimensions.
     *
     * @param aSegment      Valid segment index for this texture.
     * @param aBitmap       Bitmap to copy image contents and format from
     *                      (RGB data).
     * @param aMaskBitmap   Optional Bitmap for alpha (transparency) channel.
     *                      If the image does not require an alpha channel or the
     *                      aBitmap already has alpha information (RGBA image)
     *                      this value may be left as NULL. The bitmap will be
     *                      combined with aBitmap to single RGBA image. If used,
     *                      this alpha mask bitmap will replace any alpha
     *                      information in aBitmap.
     * @param aFlags        Specify how the uploaded data is laid out to a
     *                      texture.
     */
    virtual void SegmentUploadL(TInt aSegment,
                                const CFbsBitmap& aBitmap,
                                const CFbsBitmap * aMaskBitmap = NULL,
                                THuiTextureUploadFlags aFlags = EHuiTextureUploadFlagDefault) = 0;

    /**
     * Updates a partial area to a texture segment. Does not change the
     * dimensions of the texture (logical size or segmentation).
     *
     * This method allows direct access to texture segments.
     * @see PartialUploadL() if you do not care about texture segmentation.

     * @note You must first call Upload() at least once so the texture object
     * has dimensions and at least one segment to be updated.
     *
     * No local copy of the buffer data is made.
     *
     * @param aSegment Valid segment index for this texture.
     * @param aFormat  Format of the data buffer.
     * @param aOrigin  Position to place the partial texture data.
     * @param aSize    Size of the texture in the data buffer.
     * @param aBuffer  Texture data buffer.
     */
     virtual void SegmentPartialUpload(
                       TInt aSegment,
                       THuiTextureFormat aFormat,
                       const TPoint& aOrigin,
                       const TSize& aSize,
                       const TUint8* aBuffer) = 0;

    /**
     * Clear a texture segment to a solid color.
     */
    virtual void SegmentClearL(TInt aSegment,
                               TBool aWithAlpha,
                               const TRgb& aColor,
                               TUint8 aAlpha) = 0;


    // Implementation of MHuiSessionObject
    
    /**
     * Gets the object type.
     */
    IMPORT_C TType Type() const;

    /**
     * Gets the session id for object.
     */
    IMPORT_C TInt SessionId() const;

    /**
     * Sets the session id for object.
     */
    IMPORT_C void SetSessionId(TInt aSessionId);

    /** @endAPI */


    /**
     * Delete the texture.
     */
    IMPORT_C virtual void Reset();
    
    /**
     * Deletes the shadow textures.
     */
    IMPORT_C void ResetShadows();
    
    /**
     * Releases the texture. The caller is responsible of checking the 
     * release priority of the texture.
     */
    IMPORT_C virtual void Release();
    
    /**
     * Restores the texture.
     */
    IMPORT_C virtual void RestoreL();
    
    /**
     * Sets the texture priority. The prioriry is used when the textures are
     * released/restored.
     *
     * @param aPriority 0 = never release
     *                  other values to be decided
     *                  By default != 0
     */ 
    IMPORT_C void SetPriority( TInt aPriority );
    
    /**
     * Returns the priority of the texture
     *
     * @return The priority
     *
     * @see SetPriority()
     */
    IMPORT_C TInt Priority() const;

    /**
     * Sets the placeholder texture. Set to zero to disable the placeholder.
     *
     * @param aPlaceholder  Placeholder texture.
     */
    void SetPlaceholder(THuiTextureHandle* aPlaceholder);

    /**
     * Generates a blob texture. The texture can be then used for drawing
     * shadow edges, for example.
     *
     * @param aSize  Dimensions of the blob texture.
     */
    void GenerateBlobL(const TSize& aSize);

    /**
     * Sets the file name of the texture's source image.
     *
     * @param aFileName  Image file name.
     */
    void SetImageFileNameL(const TDesC& aFileName);

    /**
     * Uploads the texture data to the texture. Allows using EHuiTextureFlagAllowDirectBitmapUsage flag,
     * but the caller have to take care of deleting the bitmaps. 
     *
     * The bitmaps must be suitable for direct usage. Otherwise this can cause panic in rendering.
     *
     * @param aBitmap       Bitmap to copy image contents from.
     * @param aMaskBitmap   Optional Bitmap for alpha (transparency) channel.
     * @param aFlags        Specify texture upload behavior. 
     */
	void UploadDirectL(const CFbsBitmap& aBitmap, const CFbsBitmap* aMaskBitmap, THuiTextureUploadFlags aFlags);

    /**
     * Notifies one size what would be the most optimal size for this texture. 
     * This method can be called e.g. from some CHuiGc image drawing operation which knows
     * the excact size that the image appears in the display.
     *
     * @param aSize Auto size candidate. 
     */
    void NotifyPreferredSize(const THuiRealSize& aSize);
    
    /**
     * Clears auto size candidates that has been reported (during last frame). This
     * is called by the texture manager after each frame.
     */
    void ResetAutoSizeCalculation();

    /**
     * Flag to indicate that auto size calculations for this texture is enabled.
     * @return ETrue if auto size calculation is enabled. 
     */
    TBool IsAutoSizeCalculationEnabled() const;

    /**
     * Enables auto size calculations for this texture. 
     *
     * Caller of this method may observe auto size changes through texture managers 
     * MHuiTextureAutoSizeObserver interface.
     *
     * @param aEnable ETrue if auto size calculation is to be enabled. 
     */
    IMPORT_C void EnableAutoSizeCalculation(TBool aEnable=ETrue);

    /**
     * Checks if calculated auto size has been changed since last frame.
     * @return ETrue if calculated auto size is different than the size before last reset.
     */
    TBool CalculatedAutoSizeChanged() const;

    /**
     * Calculates one size which is most optimal based on all the reported sizes.
     * @return Automatically determined size for this texture.
     */
    THuiRealSize CalculateAutoSize();

    /**
     * Sets parameters to be used when calculating auto size.
     * @param aParams New parameters.
     */
    IMPORT_C void SetAutoSizeParams(const THuiTextureAutoSizeParams& aParams);
    
    /**
     * Gets parameters to be used when calculating auto size.
     * @return Currently used parameters.
     */
    IMPORT_C THuiTextureAutoSizeParams AutoSizeParams() const;

    
    /**
     * Defines whether the texture is in NVG format or not. If the texture is in
     * NVG format, it will not eat up any texture memory. Also the autosizing
     * is being disabled by default, since the sizing is practically free with
     * NVG and OpenVG backend.
     * 
     * @see EnableAutoSizeCalculation()
     * @see IsNVGContent()
     *
     * @param aNvgContent ETrue to set this texture as extended with NVG content. EFalse
     *                    to mark that this is not an extended texture.
     */
    IMPORT_C void SetNvgContent(TBool aNvgContent);

    /**
     * Informs whether this texture is in NVG format or not. 
     *
     * @see SetNvgContent()
     *
     * @return ETrue if this texture is extended and in NVG format, EFalse otherwise.
     */
    IMPORT_C TBool IsNvgContent() const;


protected:


    struct TExtParams_UploadDirectL
   		{
   		const CFbsBitmap& iBitmap;
   		const CFbsBitmap* iMaskBitmap;
   		THuiTextureUploadFlags iFlags;
   		TInt iLeaveCode;
   		};     


protected:

    /* Constructors. */

    /**
     * Constructor. Creates a new texture object with no texture maps.
     */
    IMPORT_C CHuiTexture();

    /**
     * Second-phase constructor.
     */
    IMPORT_C void BaseConstructL();


    /* Methods. */

    /**
     * Set segment data. Note that the texture name is
     * allocated when calling InitSegmentL().
     */
    IMPORT_C virtual void SetSegment(TInt aSegment,
             const TSize & iSize,
             const TPoint & iOffset,
             const TSize & iTextureSize);


    /**
     * Deallocate resources associated with segment. Will free
     * all resources associated with a texture segment.
     * @see SetSegmentCount()
     */
    virtual void ResetSegment(TInt aSegment) = 0;

    /**
     * Initialize segment for holding texture data.
     * Allocate texture name etc.
     */
    virtual void InitSegmentL(TInt aSegment) = 0;
    
        // from MHuiTexture
    
    IMPORT_C void TextureExtension(const TUid& aExtensionUid, TAny** aExtensionParameters); 


private:

	/** Flags for texture state. */
	enum THuiTextureFlags
	    {
	    /** Default flags. All special properties disabled. */
	    EHuiTextureFlagDefault = 0,
	
	    /**
	     * Shadow is enabled in the texture.
	     */
	    EHuiTextureFlagShadowEnabled = 1,
	
	    /**
	     * Texture is included in the skin content.
	     */
	    EHuiTextureFlagSkinContent = 2,
	    
	    EHuiTextureFlagAddedToTManager = 4,

	    /**
	     * Texture is extended and has NVG content
	     */
	    EHuiTextureFlagNvgContent = 8

	    // note: next flag value is 16 (we use bit positions because
	    // we want to be able to bitwise OR these together)
	    };

	/**
	 * A subdivided segment of this texture. This may have its OpenGL texture
	 * manipulated independently of its parent CHuiTexture.
	 */
    struct TSegment
        {
        /** Name of texture. */
        TUint iName;
        /** Dimensions of texture. */
        TSize iSize;
        /** Offset of segment within parent. */
        TPoint iOffset;
        /** Size of the texture. */
        TSize iTextureSize;
        };

	/**
	 * Resets and removes all texture segments
	 */
	void RemoveAllSegments();
	
public:

    /**
     * Array of content observers.
     */
    RHuiObserverArray<MHuiTextureContentObserver> iContentObservers;	
    TInt iFrameNumber;
    TInt iFrameCount;
    TInt iFrameInterval;


private:

    /** Name of the image file where the texture was loaded from. */
    HBufC* iImageFileName;

    /** Temporary texture to use until the real texture has been loaded.*/
    THuiTextureHandle* iPlaceholder;

    /** Index of the active segment. */
    mutable TInt iActiveSegment;

    /** Names of the segments. */
    RArray<TSegment> iSegments;

    /** The logical size of this texture. */
    TSize iSize;

    /** The internal texture state. Composed of THuiTextureFlags. */
    TUint iInternalState;
        
    /** Shadow generation style. */
    THuiShadowStyle iShadowStyle;

    /** Session id */
    TInt iSessionId;
    
    /** Priority */
    TInt iPriority;
 
    /** Flag to indicate that auto size calculations for this texture is enabled */
    TBool iAutoSizeCalculationEnabled;    
    
    /** Sizes reported for this texture during last frame */
    RArray<THuiRealSize> iPreferredSizes;
    
    /** Most optimal size calculated based on all reported sizes */
    THuiRealSize iCalculatedAutoSize;
    
    /** Previous most optimal size calculated based on all reported sizes */
    THuiRealSize iCalculatedPreviousAutoSize;
    
    THuiTextureAutoSizeParams iAutoSizeParams;
    
    
    NONSHARABLE_STRUCT( TShadowData )
        {
        CHuiTexture* iShadowTexture; 
        TInt iBlurDimension;
        };
        
    RArray<TShadowData> iShadows;
    
    mutable TBool iTextureChanged;
    };

#endif  // __HUITEXTURE_H__
