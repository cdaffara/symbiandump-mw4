/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
*
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:   Defines CHuiTextureManager class for texture objects loading 
*                and management.
*
*/



#ifndef __HUITEXTUREMANAGER_H__
#define __HUITEXTUREMANAGER_H__


#include <e32base.h>
#include <gdi.h>
#include <imageconversion.h>

#include <uiacceltk/HuiObserverArray.h>
#include <uiacceltk/HuiTimeObserver.h>
#include <uiacceltk/HuiTexture.h>
#include <uiacceltk/HuiBitmapProvider.h>


/* Forward declarations */
class CHuiEnv;
class CHuiTextureManager;
class CHuiTextureProcessor;
class CHuiTextureAnimationState;

/**
 * Provides callback methods for getting notifications of texture manager state
 * changes. An observer of a CHuiTextureManager.
 *
 * The class willing to receive these events must implement the methods
 * of this interface, and then register itself to the obsevers list of
 * the texture manager.
 *
 * @see CHuiTextureManager
 */
class MHuiTextureManagerStateChangedObserver
    {
public:
    /**
     * Called to notify the observer that the state of the texture manager
     * has changed. This is called when the state changes between Idle and
     * Loading.
     *
     * @param aManager  Texture manager.
     */
    virtual void TextureManagerStateChanged(const CHuiTextureManager& aManager) = 0;

    };

/**
 * Provides callback methods for getting notifications when texture manager
 * finishes asynchronous texture loads. An observer of a CHuiTextureManager.
 *
 * The class willing to receive these events must implement the methods
 * of this interface, and then register itself to the obsevers list of
 * the texture manager.
 *
 * @see CHuiTextureManager
 */
class MHuiTextureLoadingCompletedObserver
    {
public:
    /**
     * Called to notify the observer that loading of a texture has
     * been completed.
     * @param aTexture Reference to the texture that has been loaded.
     * @param aTextureId Id of the texture in the texture manager. Can be used
     * to identify the loaded texture, provided that an id was assigned to the
     * texture.
     * @param aErrorCode KErrNone if the load was successful, otherwise one of
     * the system-wide error codes indicating reason why the texture loading
     * failed.
     * @note One should not commence loading of a new texture in this callback method.
     */
    virtual void TextureLoadingCompleted(CHuiTexture& aTexture,
                                         TInt aTextureId,
                                         TInt aErrorCode) = 0;

    };


/**
 * Provides callback methods for getting notifications when texture manager
 * notices a texture which preferrred size is changed.
 *
 * The class willing to receive these events must implement the methods
 * of this interface, and then register itself to the obsevers list of
 * the texture manager.
 *
 * @see CHuiTextureManager
 */
class MHuiTextureAutoSizeObserver
    {
public:
    /**
     * Called to notify the observer that the preferred size of a texture
     * has beem changed.
     *
     * @param aTexture  Texture which preferred size has changed.
     * @param aTextureId Id of the changed texture.
     * @param aSize New preferred size for texture.
     * @return ETrue if observer accepts new size, EFalse if it does not accept or 
     * doesn't care. 
     */
    virtual TBool PreferredSizeChanged(const CHuiTexture& aTexture, TInt aTextureId, const THuiRealSize& aSize) = 0;    
    
    /**
     * Called to notify the observer that all changed preferred sizes of textures are
     * reported (for this frame).
     */
    virtual void PreferredSizeReportCompleted() = 0;
    };

/**
 * CHuiTextureManager is responsible for managing the texture objects used by
 * the application. It provides asynchronous loading of image data into
 * CHuiTexture instances. The texture manager also manages font textures,
 * which can be used for compositing text in text meshes.
 *
 * The texture manager makes sure that only one copy of each image is loaded
 * at a time, so even if the application requests the same image several times
 * it is only loaded once. Textures are identified either by an integer ID or
 * by their file name.
 *
 * @todo Document supported image (file) formats / how are the images loaded
 *
 * CHuiTextureManager is owned by CHuiEnv, and should be accessed via a
 * CHuiEnv instance. You should never construct your own texturemanagers.
 *
 * @lib hitchcock.lib
 */
class CHuiTextureManager : public CActive
    {
public:

    /**
     * States of a CHuiTextureManager object.
     */
    enum TState
        {
        /** Indicates that the manager is in idle state. A new bitmap loading
            operation can be started. */
        EIdle,

        /** Indicates that the manager is loading a bitmap. */
        ELoading
        };


    /* Constructors and destructor. */

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CHuiTextureManager();


    /* Methods. */

    /** @beginAPI */

    /**
     * Determines the environment the manager belongs to.
     */
    IMPORT_C CHuiEnv& Env();

    /**
     * Retuns a texture having given id. Will return a blank
     * texture if the id was not found. With this method
     * there is a danger of editing the blank texture
     * instead of the real texture you intended to edit.
     */
    IMPORT_C CHuiTexture* Texture(TInt aId);

    /**
     * Retuns a texture having given id. Will return a default blank
     * texture if the id was not found.
     */
    IMPORT_C const CHuiTexture* Texture(TInt aId) const;

    /**
     * Returns a texture associated with the given id.
     * Will leave if no associated texture exists. If the texture
     * is defined using the DefineFileNameL but the texture data
     * is not yet loaded then this method call will result in
     * LoadTextureL call.
     *
     * @param aId Id of the retrieved texture.
     * @return Pointer to the texture. If the method leaves the return value is undefined.
     * @leave KErrNotFound If a texture with the given id does not exist.
     */
    IMPORT_C CHuiTexture* TextureL(TInt aId);

    /**
     * Sets the path where bitmaps are loaded from. If drive is not included, tries to resolver that 
     * using CCoeEnv
     *
     * @param aPath  Path for bitmap files.
     * @see LoadTexture() Call to load textures from this location.
     */
    IMPORT_C void SetImagePathL(const TDesC& aPath);

    /**
     * Returns the current image path.
     * @see LoadTexture() Call to load textures from this location.
     */
    IMPORT_C const TDesC& ImagePath() const;

    /**
     * Returns the blank texture. This can be used as a dummy texture if
     * no real texture is available.
     */
    IMPORT_C const CHuiTexture& BlankTexture() const;

    /**
     * Returns the blank texture. This can be used as a dummy texture if
     * no real texture is available. Will generate the blank texture
     * if the texture is not yet available.
     */
    IMPORT_C CHuiTexture& BlankTexture();

    /**
     * Loads an image and makes it a texture. The bitmap files are searched in
     * the path specified with SetImagePathL. The format of the loaded image
     * must be supported by the system (Series 60) codecs.
     *
     * LoadTextureL is an asynchronous method, which returns an empty texture
     * which is loaded and filled in the background. Register an
     * MHuiTextureLoadingCompletedObserver instance to the iLoadedObservers
     * array, whose TextureLoadingCompleted() method will be called when
     * textures finish loading. The same observer is also called if an error
     * occurs during the loading process. The other method is to check if an
     * image has been loaded with TextureManagers IsLoaded().
     *
     * If a texture with the given id or filename is already loaded the previously
     * loaded texture is returned. If texture with the given id has been unloaded
     * with UnloadTexture() method then the id is reused by loading a new texture
     * on the id.
     *
     * @param aImageFileName    Name of the image bitmap file to load. Relative
     *                          to the image load path. If empty filename is
     *                          used, will check if a filename for the id has
     *                          been defined and load a texture using that resource
     *                          location, if possible.
     * @param aFlags            Specify flags for the texture loading and
     *                          uploading - how to convert the bitmap to
     *                          texture.
     * @param aId               Id for the texture. Must be unique, but may be
     *                          left unassigned. Use zero for unassigned id.
     *
     * @note                    If both image name and id are left undefined
     *                          ("" and zero), will return a blank texture.
     *
     * @return                  Reference to the texture.
     *
     * @see SetImagePathL() To set the image search path. Set to "" to use
     * absolute image filenames.
     * @see iLoadObservers
     * @see MHuiTextureLoadingCompletedObserver::TextureLoadingCompleted()
     * @see IsLoaded()
     */
    IMPORT_C CHuiTexture& LoadTextureL(const TDesC& aImageFileName,
                                       THuiTextureUploadFlags aFlags
                                         = EHuiTextureUploadFlagDefault,
                                       TInt aId = 0,
                                       TInt aFrameNumber = 0);

   /**
     * Loads an image based on pre-registered id and file name.
     *
     * @see DefineFileName()
     *
     * LoadTextureL is an asynchronous method, which returns an empty texture
     * which is loaded and filled in the background. Register an
     * MHuiTextureLoadingCompletedObserver instance to the iLoadedObservers
     * array, whose TextureLoadingCompleted() method will be called when
     * textures finish loading. The same observer is also called if an error
     * occurs during the loading process. The other method is to check if an
     * image has been loaded with TextureManagers IsLoaded().
     *
     * If a texture with the given id or filename is already loaded the previously
     * loaded texture is returned. If texture with the given id has been unloaded
     * with UnloadTexture() method then the id is reused by loading a new texture
     * on the id.
     *
     * @param aId             The id of the texture/filename to load. The id must
     *                        have a filename assigned by a call to DefineFileName().
     *                        If the id is left undefined (zero), will return a
     *                        blank texture.
     * @param aTextureMaxSize Can be used to define a maximum dimensions for
     *                        the final texture. The image data loaded will be
     *                        scaled (down) and fitted to these dimensions. Use
     *                        zero or below-zero values if size does not matter.
     *                        If the texture size is larger than the texture size
     *                        supported by the GL, the texture will be split to
     *                        multiple segments.
     * @param aFlags          Specify upload behavior - how to convert the bitmap
     *                        to texture and other load/upload behavior.
     *
     * @return                Reference to the texture.
     *
     * @see SetImagePathL()   To set the image search path. Set to "" to use
     *                        absolute image filenames.
     * @see iLoadObservers
     * @see MHuiTextureLoadingCompletedObserver::TextureLoadingCompleted()
     * @see IsLoaded()
     */
    IMPORT_C CHuiTexture& LoadTextureL(const TInt aId,
                                       const TSize& aTextureMaxSize = TSize(0,0),
                                       THuiTextureUploadFlags aFlags = EHuiTextureUploadFlagDefault);

    /**
     * Loads an image and makes it a texture. The bitmap files are searched in
     * the path specified with SetImagePathL. The format of the loaded image
     * must be supported by the system (Series 60) codecs.
     *
     * LoadTextureL is an asynchronous method, which returns an empty texture
     * which is loaded and filled in the background. Register an
     * MHuiTextureLoadingCompletedObserver instance to the iLoadedObservers
     * array, whose TextureLoadingCompleted() method will be called when
     * textures finish loading. The same observer is also called if an error
     * occurs during the loading process. The other method is to check if an
     * image has been loaded with TextureManagers IsLoaded().
     *
     * If a texture with the given id or filename is already loaded the previously
     * loaded texture is returned. If texture with the given id has been unloaded
     * with UnloadTexture() method then the id is reused by loading a new texture
     * on the id.
     *
     * @param aImageName            Name of the image bitmap file to load. If
     *                              empty filename is used, will check if a
     *                              filename for the aId has been defined
     *                              (using DefineFileNameL()) and load a texture
     *                              using that resource location, if possible.
     * @param aTextureMaxSize       Can be used to define a maximum dimensions
     *                              for the final texture. The image data
     *                              loaded will be scaled (down) and fitted to
     *                              these dimensions. Use zero or below-zero
     *                              values if size does not matter. If the
     *                              texture size is larger than the texture
     *                              size supported by the GL, the texture will
     *                              be split to multiple segments.
     * @param aFlags                Specify load/upload behavior - how to convert
     *                              the bitmap to texture.
     * @param aId                   Id for the texture. Must be unique.
     *                              Use zero for unassigned id.
     *
     * @note                        If both image name and id are left undefined
     *                              ("" and zero), will return a blank texture.
     *
     * @return Reference to the texture.
     *
     * @see SetImagePathL() To set the image search path. Set to "" to use
     * absolute image filenames.
     * @see iLoadObservers
     * @see MHuiTextureLoadingCompletedObserver::TextureLoadingCompleted()
     * @see IsLoaded()
     */
    IMPORT_C CHuiTexture& LoadTextureL(const TDesC& aImageName,
                                       const TSize& aTextureMaxSize,
                                       THuiTextureUploadFlags aFlags
                                         = EHuiTextureUploadFlagDefault,
                                       TInt aId = 0,
                                       TInt aFrameNumber = 0);

    /**
     * Creates a texture by calling the ProvideBitmapL method of the passed MHuiBitmapProvider.
     *
     * If a texture with the given id is already created or loaded the previously
     * existing texture is returned. If texture with the given id has been unloaded
     * with UnloadTexture() method then the id is reused by loading a new texture
     * on the id.
     *
     * If EHuiTextureFlagAllowDirectBitmapUsage flag is used, the provided bitmaps should be in the 
     * preferred format. That can be found using CHuiDisplay::GetPreferredTextureFormats(). Also 
     * the bitmaps shall not be compressed or have duplicated handle.
     *
     * @param aBitmapProvider       A bitmap provider that will load the bitmaps for us. The
     *                              ProvideBitmapL method of this will be called, which should
     *                              load or generate the needed bitmaps.
     * @param aFlags                Specify load/upload behavior - how to convert
     *                              the bitmap to texture.
     * @param aId                   Id for the texture. Must be unique.
     *                              Use zero for unassigned id.
     * @return Reference to the created texture.
     * @leave KErrArgument The bitmap and the mask bitmap are incompatible (different size) or 
                           in incorrect format
     */
    IMPORT_C CHuiTexture& CreateTextureL(TInt aId,
                                         MHuiBitmapProvider* aBitmapProvider,
                                         THuiTextureUploadFlags aFlags);

    /**
     * Update the texture content by calling the ProvideBitmapL method for the existing provider. 
     * Also a new provider can be given as a parameter. The texture must exist prior calling this 
     * method. 
     *
     * If EHuiTextureFlagAllowDirectBitmapUsage flag is used, the provided bitmaps should be in the 
     * preferred format. That can be found using CHuiDisplay::GetPreferredTextureFormats(). Also 
     * the bitmaps shall not be compressed or have duplicated handle.
     *
     * @param aId                   Id for the texture. Must be unique.
     *                              Use zero for unassigned id.
     * @param aBitmapProvider       A bitmap provider that will load the bitmaps for us. The
     *                              ProvideBitmapL method of this will be called, which should
     *                              load or generate the needed bitmaps.
     * @leave KErrNotFound The texture id does not exist or the source bitmap is not found.
     * @leave KErrArgument The bitmap and the mask bitmap are incompatible (different size) or 
                           in incorrect format
     */
 	IMPORT_C void UpdateTextureFromBitmapL(TInt aId, MHuiBitmapProvider* aBitmapProvider = NULL);
 
   /**
     * Unloads a texture from memory.
     *
     * This method releases the texture id and image name for reusing.
     * @see LoadTexture().
     *
     * @note May unload several textures from memory, if they have the
     * sane image name assigned!
     */
    IMPORT_C void UnloadTexture(const TDesC& aImageName, const TInt aFrameNumber = 0);

    /**
     * Unloads a texture from memory.
     *
     * This method releases the texture id and image name for reusing.
     * @see LoadTexture().
     */
    IMPORT_C void UnloadTexture(TInt aId);

    /**
     * Define (register) a texture resource (filename/path) for
     * a texture id. Enables calling the LoadTextureL only with an id.
     * This resource will then be loaded when LoadTextureL
     * is called with the id.
     */
    IMPORT_C void DefineFileNameL(TInt aId, const TDesC& aImageName);

    /**
     * Returns the number of images waiting to be loaded.
     *
     * @return  Length of the load queue.
     */
    IMPORT_C TInt LoadQueueCount() const;

    /**
     * Returns the state of the texture manager.
     */
    inline TState State() const
        {
        return iState;
        }

    /**
     * Returns a reference to the texture processor.
     *
     * @panic THuiPanic::ETextureManagerNoProcessor  Texture processor is not available.
     */
    IMPORT_C CHuiTextureProcessor& Processor();

    /**
     * Prepends the image path to the beginning of the file name,
     * if necessary.
     */
    IMPORT_C void PrependImagePath(TDes& aFileName) const;

    /**
     * Iterates through the managed textures to find aTexture, and then
     * assigns it a new ID of aId.
     *
     * @param aTexture  The managed texture whose ID we want to change.
     * @param aId       The texture ID that we want to assign to the managed texture.
     *
     * @return          ETrue if the texture was found and the ID set, EFalse if the
     *                  texture was not found in the manager's list.
     */
    IMPORT_C TBool SetTextureId(CHuiTexture* aTexture, TInt aId);

    /**
     * Checks if a texture exists, has content and is not found
     * from the texture load queue.
     * 
     * Mainly meant for file-based textures to check if the 
     * texture is ready to be used. 
     * 
     * Note that textures may have some temporary placeholder
     * content already before they have been fully loaded!
     * 
     * For CreateTextureL -based textures it is recommended to use
     * CHuiTexture::HasContent() directly.
     * 
     * @see LoadTextureL()
     * @see CreateTextureL()
     * @see CHuiTexture::HasContent()
     *
     * @param aImageName  Name of the image bitmap file to check. 
     * 					  ImagePath (if set) will be prepended to 
     * 					  this name.
     * @return 			  ETrue if texture exists, has content and 
     * 					  is not found from the texture load queue.
     */
    IMPORT_C TBool IsLoaded(const TDesC& aImageName, const TInt aFrameNumber = 0) const;

    /**
     * Checks if a texture exists, has content and is not found
     * from the texture load queue.
     * 
     * Mainly meant for file-based textures to check if the 
     * texture is ready to be used. 
     * 
     * Note that textures may have some temporary placeholder
     * content already before they have been fully loaded!
     * 
     * For CreateTextureL -based textures it is recommended to use
     * CHuiTexture::HasContent() directly.
     * 
     * @see LoadTextureL()
     * @see CreateTextureL()
     * @see CHuiTexture::HasContent()
     *
     * @param aId 		Id of the texture.
     * @return 			ETrue if texture exists, has content and is not found
     * 					from the texture load queue.
     */
    IMPORT_C TBool IsLoaded(TInt aId) const;

    /**
     * Checks if a texture exists, has content and is not found
     * from the texture load queue.
     * 
     * Mainly meant for file-based textures to check when the 
     * texture is ready to be used. 
     * 
     * Note that textures may have some temporary placeholder
     * content already before they have been fully loaded!
     * 
     * For CreateTextureL -based textures it is recommended to use
     * CHuiTexture::HasContent() directly.
     * 
     * @see LoadTextureL()
     * @see CreateTextureL()
     * @see CHuiTexture::HasContent()
     *
     * @param aTexture Texture object.
     * @return True if texture exists, has content and is not found
     * from the texture load queue.
     */
    IMPORT_C TBool IsLoaded(const CHuiTexture * aTexture) const;

    /** @endAPI */


    /**
     * Appends a texture to the texture manager's list of managed textures.
     * Ownership of the texture is transferred to the manager; when the manager
     * is destroyed, the texture will be destroyed, too.
     *
     * @param aTexture  Texture to transfer to the manager.
     * @param aId       Id for the texture. Must be unique. Use zero for
     * unassigned id.
     * @leave KErrAlreadyExists if a texture with the given id already exists.
     *
     * @todo Does not check (or replace) already existing duplicate textures
     * having matching id.
     */
    IMPORT_C virtual void AppendTextureL(CHuiTexture* aTexture, TInt aId = 0);

    /**
     * Removes a texture from management. The texture's ownership is
     * transferred to the caller.
     *
     * @param aTexture  Texture to remove from management.
     */
    IMPORT_C virtual void RemoveTexture(CHuiTexture& aTexture);

    /**
     * Informs the texture manager that time has progressed. The manager will
     * update any animated textures.
     */
    IMPORT_C void AdvanceTime(TReal32 aElapsedTime) const;

    /**
     * Releases texture manager resources. Releases all textures. Called when the
     * render plugin is released.
     *
     * While the release operation is in progress, it is not possible to delete
     * CHuiTexture instances or create new ones. After the release has been
     * completed, textures can again be deleted and created.
     *
     * @return ETrue if the all the textures were released.
     *         EFalse if some of the textures 
     * @panic ETextureManagerTextureConstructedDuringRelease
     *      A new CHuiTexture was constructed and added to the texture
     *      manager during the release operation.
     * @panic ETextureManagerTextureDestroyedDuringRelease
     *      An existing CHuiTexture was destroyed and removed from the texture
     *      manager during the release operation.
     *
     * @see RestoreL()
     */
    IMPORT_C virtual TBool Release();

    /**
     * Restores texture manager resources. Restores the content of all textures
     * released in Release(). Called when render plugin restored.
     *
     * While the restore operation is in progress, it is not possible to delete
     * CHuiTexture instances or create new ones. After the restore has been
     * completed, textures can again be deleted and created.
     *
     * @panic ETextureManagerTextureConstructedDuringRestore
     *      A new CHuiTexture was constructed and added to the texture
     *      manager during the restore operation.
     * @panic ETextureManagerTextureDestroyedDuringRestore
     *      An existing CHuiTexture was destroyed and removed from the texture
     *      manager during the restore operation.
     *
     * @see Release()
     */
    IMPORT_C virtual void RestoreL();
    
    /**
     * Releases and restores skin dependent textures. Usually called after
     * system skin has been changed.
     */
    IMPORT_C void NotifySkinChangedL();
    
    /**
     * Adds animated texture group. 
     * Use this method to inform texture manager that when texture is loaded, 
     * it's allowed to use previous state of the group. Each texture id should 
     * belong at most to one group. This method should be called before
     * using these texture ids to load textures.
     * @param aTextureIds texture ids that form a group.
     * @return animated texture group id.
     */
    IMPORT_C TInt AddAnimatedTextureGroupL( const RArray<TInt>& aTextureIds );
    
    /**
     * Removes animated texture group.
     * @param aGroupId animated texture group id.
     */
    IMPORT_C void RemoveAnimatedTextureGroup( TInt aGroupId );

    
    /**
     * Clears texture changed flags.
     */
    IMPORT_C void ClearChangedTextures();
    
    /**
     * Sets a flag that indicates that there are changed textures.
     */
    IMPORT_C void SetHasChangedTextures();
    
    /**
     * Sets a flag if texture memory usage calculation should be done and printed.
     */
    IMPORT_C void EnableTexMemoryCalculation( TBool aEnableTeMemCal );

    /**
     * Estimated mem usage for textures assuming that textures which have pixel content
     * are using given amount of memory per pixel.
     * @return Estimated texture memory usage in bytes
     */
    TInt EstimatedTextureMemUsage(TInt aAverageBitsPerPixel) const;

    /**
     * Enables (or disables) texture autosize calculations.
     */
    void EnableAutoSizeCalculation(TBool aEnable = ETrue);

protected:

    /* Protected data structures. */

    /**
     * Permanent registration entry for textures.
     */
    NONSHARABLE_CLASS(CTextureEntry) : public CBase, public MHuiTextureContentObserver
        {
    private:
        CTextureEntry(); // the dummy constructor is private!
    public:
        CTextureEntry(TInt aId,
                      const TFileName & aFileName,
                      const TSize& aMaxTextureSize = TSize(0,0),
                      THuiTextureUploadFlags aFlags = EHuiTextureUploadFlagDefault,
                      TInt aFrameNumber = 0);
                      
        CTextureEntry(TInt aId, CHuiTexture * aTexture);
        
        CTextureEntry & operator=(const CTextureEntry &aSrc);
        
    private:
        CTextureEntry(const CTextureEntry &aSrc);
        
    public:
    
        ~CTextureEntry();
        
        void SetFileName(const TFileName & aFileName);

        void TextureContentUploaded(CHuiTexture& aTexture);

        void TextureContentReleased(CHuiTexture& aTexture);

        void RestoreTextureContentL(CHuiTexture& aTexture);

        /** The texture id. Set to zero for no id. */
        TInt iId;

        /** Resource location for the texture. */
        HBufC*  iFileName;

        /** The texture entry. */
        CHuiTexture* iTexture;

        /** Requested maximum size for the texture. */
        TSize iMaxTextureSize;

        /** Specify upload behavior - how to convert the bitmap
          to texture. */
        THuiTextureUploadFlags iFlags;

        /** Bitmap content provider. Alternative content (re)loading
         * mechanism to filenames (loading from a file). */
        MHuiBitmapProvider* iBitmapProvider;
        
        /* Number of bits perpixel, used by texture. This field is used while 
         * calculating total memory consumption by texturemanager, for textures.
         */
        TInt iBitDepth;       
        
        /** The frame number. */
        TInt iFrameNumber;
        TInt iFrameCount;
        TInt iFrameInterval;
        };

    /**
     * Temporary asynchronous load entry for textures.
     */
    struct SLoadQueueEntry
        {
        /** The texture entry that is being loaded. */
        CTextureEntry* iLoading;

        /** The image being loaded has an alpha channel. */
        TBool iHasAlpha;

        /** The original, non-downscaled size of the image. */
        TSize iOriginalSize;

        /** Image decoder to load bitmap images. */
        CImageDecoder* iDecoder;

        /** True, if the texture was already unloaded before it finished
            loading. */
        TBool iUnloaded;

        };


    enum TReleaseState
        {
        /** The texture manager is operating normally in the non-released state. */
        ENormal,

        /** The texture manager is currently releasing textures. */
        EReleasing,

        /** The texture manager is operating normally in the released state. */
        EReleased,
        
        /** The texture manager is operating normally in the partly released state. 
            This occurs if some of the textures cannot be released*/
        EPartiallyReleased,

        /** The texture manager is current restoring textures. */
        ERestoring
        };


protected:

    /* Constructors. */

    /**
     * Constructor.
     */
    IMPORT_C CHuiTextureManager(CHuiEnv& aEnv /*MHuiTextureManagerObserver* aObserver = NULL*/);

    /**
     * Second-phase constructor.
     */
    IMPORT_C void BaseConstructL();


    /* Methods. */

    /**
     * Sets the texture processor of the manager. Ownership of the processor
     * is given to the manager.
     *
     * @param aProcessor  Texture processor instance. Ownership is transferred.
     */
    IMPORT_C void SetProcessor(CHuiTextureProcessor* aProcessor);

    /**
     * Returns a texture entry.
     *
     * @param aIndex  Index of the texture.
     *
     * @return  Returns a texture entry from the set of existing textures.
     */
    IMPORT_C CTextureEntry* TextureEntry(TInt aIndex);

    /**
     * Returns the number of texture entries.
     */
    IMPORT_C TInt TextureEntryCount() const;

    /**
     * Check the existence of a texture.
     *
     * @param aFileName  File name of the texture.
     *
     * @return  Index of the texture, in the iTextures array, or -1 if not found.
     */
    IMPORT_C TInt CheckTexture(const TDesC& aFileName, TInt aFrameNumber = 0) const;

    /**
     * Check the existence of a texture.
     * @param aId Id of the texture to check.
     * @return  Index of the texture, in the iTextures array, or
     * -1 if not found.
     */
    IMPORT_C TInt CheckTexture(TInt aId) const;

    /**
     * Helper method that notifies observers of texture load completion.
     * @see iLoadObservers
     */
    IMPORT_C void NotifyTextureLoaded(CHuiTexture& aTexture,
                             TInt aTextureId,
                             TInt aErrorCode) const;

protected: // New methods
    
    /**
     * Provides expandability, helps keeping the binary compatibility. Since virtual
     * table is now exported and this class is dll derivable and the implementation
     * is more difficult to change, hence this method, which can provide additional
     * extension APIs.
     * 
     * @param aExtensionUid     UID, which is being used for recognizing the extension
     * @param aExtensionParams  Return pointer to the extension API, once recognized from the extension uid
     */
    IMPORT_C virtual void TextureManagerExtension(const TUid& aExtensionUid, TAny** aExtensionParams);

private:
    
    /* Private methods */

    /**
     * Creates a texture from the given CFbsBitmap and its mask bitmap.
     * This is a synchronous method call and will result in consequent call
     * of TextureLoadingCompleted of all observers if the texture creation
     * was successful. As filename cannot be acquired for created
     * textures the user provided string will be used. Note that the filename
     * has to be unique for all textures. Textures are identified by their
     * filename and if a texture with existing filename is loaded / created
     * then already loaded / created texture will be returned instead of
     * creating a new texture.
     *
     * @note There are problems in creating textures from icon bitmaps exported
     * using AknsUtils::CreateAppIconLC(). It seems that it is impossible to
     * access the bitmap data of created icon directly using DataAddtess() - method
     * or THuiBitmapUtil - method. HUIToolkit relies on these methods.
     * To create textures from the system icons you have to copy the icons to temporary
     * CFbsBitmaps. Notice that duplicating the bitmap handle with CFbsBitmap::Duplicate is
     * not enough. You have to make a physical copy.
     *
     * @param aBitmap    The source bitmap from which the texture is created.
     * @param aMask      Pointer to the mask bitmap that is used as alpha blending
     *                   mask for the texture. This parameter can be NULL for
     *                   textures without alpha channel.
     * @param aId        The id of the texture/filename to load. The id must have
     *                   a filename assigned by a call to DefineFileName().
     * @return Reference to the created texture.
     * @leave KErrArgument The bitmap and the mask bitmap are incompatible (different size).
     */
    IMPORT_C virtual CHuiTexture& CreateTextureL(CFbsBitmap& aBitmap,
                                         const CFbsBitmap* aMask,
                                         THuiTextureUploadFlags aFlags,
                                         TInt id);

    /**
     * Returns the file server session that should be used by the manager.
     */
    RFs& FsSession() const;

    /** Returns true if the given texture is still in the load queue. */
    TBool IsInLoadQueue(const CHuiTexture * texture) const;

    /** Returns true if the given texture is still in the load queue. */
    TBool IsInLoadQueue(TInt id) const;

    /**
     * Called when image loading (decoding) operation is complete.
     */
    IMPORT_C void RunL();

    /**
     * Handles a leave occurring in the request completion event handler RunL().
     */
    IMPORT_C TInt RunError( TInt aError );

    /**
     * Called when a sub-image has been loaded
     * (before calling @c ImageLoadingCompleteL).
     * This method checks if we need to continue loading.
     * @return Error code or positive integer. If positive integer,
     *         stop and do not continue to frame generation.
     */
    TInt ImageLoadingContinue();
    
    /**
     * Called when an image has finished loading. Handles
     * uploading of the loaded bitmap to the texture.
     * @param aEntry queue entry.
     */
    void ImageLoadingCompleteL(SLoadQueueEntry& aEntry);

    /**
     * Processes animated frame.
     * @param aEntry queue entry.
     */
    void ProcessAnimatedFrameL(SLoadQueueEntry& aEntry);

    /**
     * Cancel active object operation.
     */
    IMPORT_C void DoCancel();

    /**
     * Starts loading next images from the load queue. If scheduling a
     * queue entry fails for some reason, the entry is discarded and the
     * next queue entry is processed.
     */
    void StartLoading();

    /**
     * Cleans up the topmost load queue entry and removes it from the
     * load queue. Deletes the decoder, but not the loaded texture entry.
     */
    SLoadQueueEntry PopLoadedQueueEntry();

    /**
     * Sets up image decoding and starts to load the first entry in the load
     * queue.
     * @leave Standard error code if decoder initialization/resource read
     * fails.
     */
    void DoLoadNextL();

    /**
     * Unloads a texture having given index in the texture array (iTextures).
     */
    void DoUnload(TInt index);

    /**
     * Cancels the scheduled or ongoing loading of a texture.
     * If the texture is found from load queue it is immediately removed from
     * the queue.
     * If the removed texture was being loaded at the time this method is called
     * the loading is cancelled and restarted from the next unloaded texture.
     *
     * @param aTexture Reference to the texture of which load is cancelled.
     */
    void CancelLoadingOfTexture(CHuiTexture& aTexture);

    /**
     * Helper method that notifies observers of texture manager state change.
     * @see iStateObservers
     */
    void NotifyStateChange() const;
    
    /**
     * Update texture flags if needed.
     * @param aBitmap    Bitmap.
     * @param aMask      Mask.
     * @param aFlags     Texture flags. This returns the changed value.
     */
    void UpdateFlags(const CFbsBitmap& aBitmap, const CFbsBitmap* aMaskBitmap, THuiTextureUploadFlags& aFlags);
    
    void NotifyTextureAutoSizeObservers() const;

    /**
     * Creates animation state.
     * @param aGroupId group id.
     * @param aImageFile image file.
     * @param aFrameNumber frame number.
     * @param aFrameCount frame count.
     */
    CHuiTextureAnimationState* CreateAnimationStateL( 
        TInt aGroupId, const TDesC& aImageFile, 
        TInt aFrameNumber, TInt aFrameCount );
    
    /**
     * Finds animation state owned by aGroupId.
     * @return animation state or NULL if not found.
     */
    CHuiTextureAnimationState* FindAnimationState( TInt aGroupId );
    
    /**
     * Removes animation state.
     * @param aState state to be removed.
     */
    void RemoveAnimationState( CHuiTextureAnimationState* aState );
    
    /**
     * Finds group id by texture id.
     * @param aTextureId texture id to look for.
     * @param aGroupId updated to contain found group id.
     * @return ETrue if succeeded, EFalse otherwise.
     */
    TBool FindGroupIdByTextureId( TInt aTextureId, TInt& aGroupId ) const;
    
    /**
     * Checks if animation state is needed.
     * @param aDecoder decoder.
     * @return ETrue if needed, EFalse otherwise.
     */
    static TBool NeedsAnimationState( const CImageDecoder& aDecoder );
    
    // For debugging purposes! Used to calculate texture memory usage
    /* Calculates memory used by texturemanager for all the textures and prints as info messages
     */
    void TextureMemUsage() const;

public:

    /* Public properties. */

    /**
     * Observers that are notified of changes in the state of the manager.
     * Applications may directly add and remove their own observers using
     * this public property.
     */
    RHuiObserverArray<MHuiTextureManagerStateChangedObserver> iStateObservers;

    /**
     * Observers that are notified loaded textures of the manager.
     * Applications may directly add and remove their own observers using
     * this public property.
     */
    RHuiObserverArray<MHuiTextureLoadingCompletedObserver> iLoadObservers;

    /** Animated textures. */
    RHuiObserverArray<MHuiTimeObserver> iAnimatedTextures;

    /** 
     * Observers that are notified when toolkit notices a change in texture
     * auto size e.g. it is now being drawn as different size than before.
     * Observers are notified possibly between every frame, they should handle
     * notifications as quickly as possible.
     */
    RHuiObserverArray<MHuiTextureAutoSizeObserver> iTextureAutoSizeObservers;

private:

    /** Environment. */
    CHuiEnv& iEnv;

    /** Empty texture that is returned if an invalid image is specified. */
    CHuiTexture* iBlankTexture;

    /** State of the texture manager. */
    TState iState;

    /** This variable tells whether there has been a valid texture in the load queue
        that we have started to load in the current texture load loop. This variable 
        indicates that we have started to load at least one texture during the load loop. */
    TBool iLoadQueueHadValidTexture;

    /** State of the release/restore process. */
    TReleaseState iReleaseState;

    /** Path where image files are loaded from. */
    HBufC* iImagePath;

    /** Queue of loading tasks. */
    RArray<SLoadQueueEntry> iLoadQueue;

    /** Counter for generating new font identifiers. */
    TInt iFontIdEnumerator;

    /**
     * Registry of all textures within this toolkit.
     * Accessed by texture ids (iId) or texture filenames (iFileName).
     * Members may be NULL if a texture is not loaded.
     */
    RPointerArray<CTextureEntry> iTextures;

    /** Bitmap for loading asynchronously into. */
    CFbsBitmap* iBitmap;

    /** Mask bitmap for loading alpha channels. */
    CFbsBitmap* iMaskBitmap;

    /** Texture processor instance. */
    CHuiTextureProcessor* iProcessor;

    /** Open file server session for image loading. */
    RFs& iFS;

    /**
     * Struct containing animated texture group item.
     * Typically, each group consists of two textures and amount
     * of groups is small. Thus, simple array based approach was
     * selected. 
     */
    struct TAnimatedTextureGroupItem
        {
        TInt iGroupId;
        TInt iTextureId;
        };
    
    /**
     * Array of animated texture group items.
     */
    RArray< TAnimatedTextureGroupItem > iAnimatedTextureGroupItems;
    
    /** 
     * Array of animation states. 
     * Own.
     */
    RPointerArray< CHuiTextureAnimationState > iAnimations;

    /**
     * Animation state that does not belong to any group.
     * Own.
     */    
    CHuiTextureAnimationState* iAnimationWithoutGroup;

    /** Flag to optimize changed texture lookup */
    TBool iHasChangedTextures;

    /** To do texture memory usage calculation and print it as info messages */
    TBool iEnableTexMemCalculation;   
    
    /** Flag to tell wheter texture autosizing is enabled */
    mutable TBool iTextureAutoSizingEnabled;
    };

#endif  // __HUITEXTUREMANAGER_H__
