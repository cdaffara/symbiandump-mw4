/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Declares CAlfTexture texture class.
*
*/



#ifndef C_ALFTEXTURE_H
#define C_ALFTEXTURE_H

#include <coemain.h>
#include <alf/alftextureautosizeparams.h>

class CAlfEnv;
class MAlfBitmapProvider;
class CAlfTextureManager;

/** Flags for texture Uploading. */
enum TAlfTextureFlags
    {
    /** No special actions/behavior needed. */
    EAlfTextureFlagDefault = 0,

    /**
     * @deprecated This is default functionality
     */
    EAlfTextureFlagRetainResolution = 1,

    /**
     * @deprecated 
     * Automatically generates a shadow for the texture.
     */
    EAlfTextureFlagGenerateShadow = 2,

    /**
     * Use to retain the color depth of the
     * ((up)loaded) image as well as possible. Otherwise
     * the toolkit may reduce the color depth of the texture
     * to conserve memory.
     */
    EAlfTextureFlagRetainColorDepth = 4,

    /**
     * If this flag has been set, texturemanager automatically releases and 
     * restores texture after system skin changes so that it has the
     * updated content. 
     */
    EAlfTextureFlagSkinContent = 8,

    /**
     * Set if it is not necessary to retain the full resolution of the bitmap. 
     * Default functionality is try to match texture resolution
     * with bitmap resolution).
     * This may save some memory with hardware acceleration, but image quality changes to worse
     */
    EAlfTextureFlagDoNotRetainResolution = 0x10,

    /**
     * Lets texture manager report preferred texture size via MAlfTexturePreferredSizeObserver 
     * interface and automatically re-request upload from the bitmap provider 
     * (which should be aware of the new size).
     * 
     * Preferred size is determined by the actual usage of the texture and the size(s) that
     * texture appears on the display.
     *   
     * Automatically determined preferred size may not be always the most optimal and 
     * performance may be degraded due multiple (re)uploads if texture usage changes frequently.
     * 
     * Also it is not quaranteed that texture manager is able to report preferred size in 
     * all possible situations.
     */
    EAlfTextureFlagAutoSize = 0x40,
    
    /*
     * If defined the texture manager will not try to check whether or not the texture has
     * multiple frames. This can be used load animated texture's first frame as a simple image
     * or to reduce the loading time of a texture (e.g. in case of large size images) if the
     * user knows that the image does not have any frames. 
     */
    EAlfTextureFlagLoadAnimAsImage = 0x80
    };


    
/**
 * CAlfTexture is the client side class for all texture objects.
 *
 * A CAlfTexture always represents one or more fully loaded (OpenGL ES) texture
 * objects. When drawing something, TAlfImages are used for determining the
 * area of  the texture to draw. However, different segments may utilize
 * textures in different resolutions.
 * Usage:
 * @code
 *  //load texture from Texture Manager
 *   CAlfTexture* tex = iTextureMgr->LoadTexture(imageFileName1,
 * 								 EAlfTextureFlagDefault,
 * 								 AUTO_GENERATED_TEXTURE_ID);
 *  
 *  //Use texture to create Image instance
 *  TAlfImage image( *tex );
 * 
 *  //For textures, which should never be released, set priority to be 0
 *  //see CAlfEnv::Release()
 *  tex->SetPriority(0);
 *  
 *  //cleanup
 *   delete env; //  delete texturemanager and all the textures.
 * 
 * @endcode
 * @see CAlfTextureManager
 * @see TAlfImage
 * @lib alfclient.dll
 * @since S60 v3.2
 */

NONSHARABLE_CLASS( CAlfTexture ): public CBase
    {
public:

    /* Constructors and destructor. */

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CAlfTexture();

    /**
     * Static factory method. Creates a texture appropriate for the current
     * renderer.
     */
    static CAlfTexture* NewL(CAlfEnv& aEnv, TUid aManagerUid, 
        TInt aId = 0, TInt aBitmapHandle = 0, TInt aMaskHandle = 0, 
        TAlfTextureFlags aFlags = EAlfTextureFlagDefault, 
        TBool aAnimated = EFalse );

    /**
     * Static factory method. Creates a texture appropriate for the current
     * renderer.
     */
    static CAlfTexture* NewLC(CAlfEnv& aEnv, TUid aManagerUid, 
        TInt aId = 0, TInt aBitmapHandle = 0, TInt aMaskHandle = 0, 
        TAlfTextureFlags aFlags = EAlfTextureFlagDefault,
        TBool aAnimated = EFalse);

    /**
     * Sets server side handle.
     * @param aHandle Serverside handle
     */
    void SetServerHandle(TInt aHandle);

    /**
     * Sets bitmap provider.
     * @param bitmap provider.
     */
    void SetBitmapProvider(MAlfBitmapProvider* aBitmapProvider);
    
    /**
     * Gets bitmap provider.
     * @return bitmap provider.
     */
    MAlfBitmapProvider* BitmapProvider() const;
    
    /**
     * Sets filename.
     * @param file name.
     */    
    void SetFileNameL(const TFileName & aFileName);

    /**
     * Sets max texture size.
     * @param max texture size.
     */    
    void SetMaxTextureSize(TSize aTextureMaxSize);    

    /**
     * Gets texture flags.
     * @return Texture flags.
     */    
    TAlfTextureFlags Flags() const;

    /**
     * Sets texture flags.
     * @param Texture flags.
     */    
    void SetFlags(TAlfTextureFlags aFlags);
    
    /**
     * Sets texture logical size.
     * @param logical texture size.
     */    
    void SetSize(TSize aSize);        
    
    /** @beginAPI */

    /**
     * Gets texture id.
     * @return Texture id.
     */    
    IMPORT_C TInt Id() const;
        
    /**
     * Gets server side handle.
     * @return Serverside handle
     */
    IMPORT_C TInt ServerHandle() const;    

    /**
     * Gets filename.
     * @param file name.
     */    
    IMPORT_C HBufC* FileName() const;
    
    /**
     * Returns ETrue if the texture has content - existing texture
     * data. This indicates that the texture has been (up)loaded 
     * and is ready to be used.
     */
    IMPORT_C TBool HasContent() const;

    /**
     * Gets max texture size.
     * @return max texture size.
     */    
    IMPORT_C TSize MaxTextureSize();    

    /**
     * Gets texture logical size.
     * @return logical texture size.
     */    
    IMPORT_C TSize Size() const;    

    /**
     * Returns ETrue if the texture is animated,
     * EFalse otherwise
     * @return the animation status of the texture.
     */    
    IMPORT_C TBool IsAnimated() const;

    /**
     * Starts the animation if the texture is animated.
     * Does nothing if the texture is not animated, or
     * the animation has already been started
     */    
    IMPORT_C void StartAnimation();

    /**
     * Stops the animation if the texture is animated.
     * Does nothing if the texture is not animated, or
     * the animation has already been stopped
     */    
    IMPORT_C void StopAnimation();
    
    /**
     * Sets the texture priority. The priority is used when releasing/restoring
     * textures.
     *
     * @param aPriority 0 = never release
     *                  See TAlfTexturePriority from
     *                  alftexturemanager.h
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
     * Sets parameters to be used when automatically calculating preferred size.
     * @param aParams New parameters.
     */
    IMPORT_C void SetAutoSizeParams(const TAlfTextureAutoSizeParams& aParams);
    
    /**
     * Gets parameters to be used when automatically calculating preferred size.
     * @return Currently used parameters.
     */
    IMPORT_C TAlfTextureAutoSizeParams AutoSizeParams() const;

    /**
    * Enables texture reference counting. If enabled,the user
    * does not need to delete or unload the texture, but can leave 
    * the responsibility for the toolkit. Needs to be enabled per texture. Allows
    * keeping important textures in memory if disabled. Even if
    * the refcounting is disabled, internally bookkeeping is still
    * made in order to correctly free unused textures when 
    * refcounting is enabled again.
    *
    * The whole feature is disabled by default and needs to be enabled
    * when necessary.
    *
    * Action that will be taken when ref count reaches zero 
    * can be defined using SetRefCountingAction method, by default
    * it is ERefCountingActionUnload.
    *
    * @param aEnable if ETrue, reference counting is enabled.
    */
    IMPORT_C void EnableRefCounting(TBool aEnable=ETrue);
    
    /**
    * Queries the reference counting state
    *
    * @return ETrue if texture is refcounted, EFalse otherwise.
    */
    IMPORT_C TBool RefCountingEnabled();

    
    enum TAlfTextureRefCountingAction
        {
        ERefCountingActionDelete,
        ERefCountingActionUnload
        };

    /** 
    * Sets action that is performed when reference counting reaches zero.
    * @param aAction Possible values are ERefCountingActionDelete and
    * ERefCountingActionUnload but in the future there may be new ones.
    *   
    */
    IMPORT_C void SetRefCountingAction(TInt aAction);

    /**
    * Gets action that is performed when reference counting reaches zero.
    * @return Possible values are ERefCountingActionDelete and
    * ERefCountingActionUnload but in the future there may be new ones.
    */
    IMPORT_C TInt RefCountingAction() const;

    /** @endAPI */

    enum TAlfTextureReleaseFlags
        {
        EReleaseNone = 0x0,
        EReleaseFromEnv = 0x1,
        EReleaseFromRefCounting = 0x2,
        EReleaseFromUnload = 0x4           
        };
    /**
     * Returns the flags which tell if texture has been released.
     * @return Release flags
     */
    TInt ReleaseFlags() const;
    
    /**
     * Sets the flags which tell if texture has been released.
     * @param aReleased EReleaseNone when it is restored, others when texture gets released.
     */
    void SetReleaseFlags(TInt aReleaseFlags); 

    /**
     * Gets the texturemanager instance which manages this texture.
     * @return Texture manager for this texture.
     */
     CAlfTextureManager* TextureManager();
        
protected:

    /* Constructors. */

    /**
     * Constructor. Creates a new texture object with no texture maps.
     */
    CAlfTexture();

    /**
     * Second-phase constructor.
     */
    void ConstructL(CAlfEnv& aEnv, TUid aManagerUid, TInt aId, TInt aBitmapHandle, TInt aMaskHandle, TAlfTextureFlags aFlags, TBool aAnimated);

private:

	/** Private data */
    struct TPrivateData;    
    TPrivateData* iData;
    };

#endif  // C_ALFTEXTURE_H
