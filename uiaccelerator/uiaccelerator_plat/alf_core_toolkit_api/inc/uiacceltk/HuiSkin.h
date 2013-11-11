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
* Description:   Defines CHuiSkin abstract base class for skins. Skins are a 
*                collection resources forming custom graphical appearance.
*
*/



#ifndef __HUISKIN_H__
#define __HUISKIN_H__


#include <e32base.h>
#include <gdi.h>
#include <uiacceltk/HuiTexture.h>
#include <uiacceltk/huitextstylemanager.h>

/* Forward declarations. */
class CHuiEnv;
class CHuiGc;
//class THuiFont;

/** Background type suggestions.
 * 
 * These enums suggest which sort of background is under the visual,
 * in order for it to be rendered in an appropriate colour.
 * A dark background means that the text will be rendered in white, and a
 * light background means that the text will be black.*/
enum THuiBackgroundType
    {
    /** background beneath the font is light. */
    EHuiBackgroundTypeLight,
    
    /** background beneath the font is dark. */
    EHuiBackgroundTypeDark
    };

/** Element of a skin. */
enum THuiSkinElement
    {
    /** Soft key labels. */
    EHuiSkinElementSoftKeys
    };


/** Location specifier for an element of the skin. */
enum THuiSkinLocation
    {
    /** Location is not determined. */
    EHuiSkinLocationNone,

    /** Location is in the top. */
    EHuiSkinLocationTop,

    /** Location is on the right. */
    EHuiSkinLocationRight,

    /** Location is in the bottom. */
    EHuiSkinLocationBottom,

    /** Location is on the left. */
    EHuiSkinLocationLeft
    };


/**
 * Skin (texture) Resources.
 */
enum THuiSkinTextureResource
    {
    EHuiSkinBackgroundTexture,
    EHuiSkinShadowTexture,
    EHuiSkinGlyphShadowTexture,
    EHuiSkinLineTexture
    };
    
    
/**
 * Orientations.
 */     
enum THuiSkinOrientation
    {
    EHuiSkinOrientationPortrait,
    EHuiSkinOrientationLandscape    
    };


/**
 * Skin defines a collection of textures, fonts, and layout resources forming
 * custom graphical appearance for a GUI that can be applied
 * in order to suit the different tastes of different users.
 *
 * CHuiSkin is an abstract base class for all skin implementations.
 *
 * @todo  It should be possible to define a skin for each display separately.
 *        The environment can still own the skins, but each display would
 *        know the skin it is using. This way off-screen displays can look
 *        different and use different DPI (e.g., different font sizes)
 *        than on-screen displays.
 */
NONSHARABLE_CLASS(CHuiSkin) : public CBase, public MHuiTextureContentObserver
	{
public:

	/* Constructors and destructor. */

    /**
     * Constructor.
     */
    CHuiSkin(CHuiEnv& aEnv);

    /**
     * Second-phase constructor for skin implementations. Note that the
     * texture manager is not available when this is called, which means
     * textures and fonts cannot be loaded during the execution of this
     * method.
     */
    virtual void ConstructL() = 0;

	/**
	 * Destructor.
	 */
	~CHuiSkin();


	/* Methods. */

	/**
	 * Returns the environment of the skin.
	 */
	inline CHuiEnv& Env()
	    {
	    return iEnv;
	    }

    /**
     * Determines the orientation of the skin. Controls may use this to adapt 
     * their content to a specific layout orientation.
     *
     * @return  Orientation of the skin.
     */
    IMPORT_C virtual THuiSkinOrientation Orientation() const;     

	/**
	 * Determines the preferred location for an element of the skin.
	 *
	 * @param aElement  Element whose location to determine.
	 *
	 * @return Location specifier.
	 */
	IMPORT_C virtual THuiSkinLocation Location(THuiSkinElement aElement);

	/**
	 * Called to notify the skin that there has been a change in the display
	 * size. This will make the skin do things like determine new font sizes.
	 * The application should call this manually from the AppUi class.
	 */
	IMPORT_C virtual void NotifyDisplaySizeChangedL();

	/**
	 * Makes a graphics context current.
	 *
	 * @param aGc  Graphics context.
	 */
	IMPORT_C virtual void SetContext(CHuiGc& aGc);

	/**
	 * Returns the current graphics context.
	 *
	 * @return  Graphics context.
	 */
	IMPORT_C virtual CHuiGc& Context() const;

    /**
     * Returns the text color to be used with a text style on a certain
     * kind of background.
     *
     * @param aStyle           Text style.
     * @param aBackgroundType  Type of background on which text is over.
     */
    virtual TRgb StyleTextColor(THuiPreconfiguredTextStyle aStyle,
                                THuiBackgroundType aBackgroundType) const = 0;

	/**
	 * Returns a texture corresponding the requested skin texture resource.
     *
     * If the defined texture was not yet created the function will try to
	 * create it and leaves if creation fails.
	 *
	 * @param aSkinTextureResource Enum of the texture to be created.
     * @see THuiSkinTextureResource.
     * @see ReleaseTexture().
	 * @return Reference to the skin-owned texture object.
     * @leave KErrNotSupported if invalid skin texture resource enum is provided.
	 */
	IMPORT_C virtual const CHuiTexture& TextureL(TInt aSkinTextureResource);
	
	/**
	 * Explicit way for releasing textures from memory that are owned by this
	 * skin. TextureL recreates the released texture if called after this.
	 * This allows application developers to reduce amount of used texture memory on demand.
	 *
	 * @param aSkinTextureResource Enum of the texture to be released.
	 * @see THuiSkinTextureResource.
	 * @see TextureL().
	 */
	IMPORT_C virtual void ReleaseTexture(TInt aSkinTextureResource);

    /**
     * Returns a texture corresponding the requested skin texture resource.
     *
     * If the defined texture was not yet created the function will try to
     * create it and returns an error value if creation fails. If texture
     * cannot be created a blank texture is provided. Texture is passed
     * in the output parameter.
     *
     * @param aSkinTextureResource Enum of the texture to be created.
     * @see THuiSkinTextureResource.
     *
	 * @param aOutTexture Output parameter: Reference to a texture pointer
     * where the requested texture will be stored. If texture creation fails
     * a blank texture reference is assigned to this pointer, thus there should
     * never be a situation where texture pointer is NULL or invalid.
     *
	 * @return Error code indicating whether the texture was acquired
     * succesfully or not. KErrNone if everything went ok. KErrNoMemory if
     * allocation of the texture failed. KErrNotSupported if invalid skin
     * texture id is provided.
	 */
    IMPORT_C virtual TInt GetTexture(TInt aSkinTextureResource,
                                     const CHuiTexture*& aOutTexture);
                                 
    /**
     * Release cached textures.
     */                                     
    void ReleaseCachedTextures();
                                         
public:

    /* Implementation of MHuiTextureContentObserver. */
    
    IMPORT_C void TextureContentUploaded(CHuiTexture& aTexture);

    IMPORT_C void TextureContentReleased(CHuiTexture& aTexture);
    
    IMPORT_C void RestoreTextureContentL(CHuiTexture& aTexture);


protected:

    virtual void SkinExtension(const TUid& aExtensionUid, TAny** aExtensionParameters);

private:

    /* Private methods. */

	/**
	 * Returns a texture corresponding the requested skin texture resource.
     *
     * If the defined texture was not yet created the function will try to
	 * create it and leaves if creation fails.
	 *
	 * @param aSkinTextureResource Enum of the texture to be created.
	 * @param aForcedPrepare Content of texture always recreated.
	 *
	 *
     * @see THuiSkinTextureResource.
     * @see ReleaseTexture().
	 * @return Reference to the skin-owned texture object.
     * @leave KErrNotSupported if invalid skin texture resource enum is provided.
	 */
    const CHuiTexture& TextureL(TInt aSkinTextureId, TBool aForcedPrepare);

    /**
     * Prepares a shadow texture
     */
    void PrepareShadowTextureL(CHuiTexture& aTexture);

    /**
     * Prepares a glyph shadow texture
     */
    void PrepareGlyphShadowTextureL(CHuiTexture& aTexture);

    /**
     * Prepares a line texture
     */
    void PrepareLineTextureL(CHuiTexture& aTexture);


private:

    /** Environment. */
    CHuiEnv& iEnv;

    /** Current graphics context. */
    CHuiGc* iGc;

    /** Texture used for drawing shadows. */
    CHuiTexture* iShadowTexture;

    /** Texture used for soft lines. */
    CHuiTexture* iLineTexture;

    /** Texture used for text glyph shadows (managed). */
    CHuiTexture* iGlyphShadowTexture;

	};

#endif  // __HUISKIN_H__
