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
* Description:   ?Description
*
*/



#ifndef T_ALFIMAGE_H
#define T_ALFIMAGE_H

#include <e32base.h>
#include <AknIconUtils.h>
#include <AknsUtils.h>
#include <alf/alftimedvalue.h>
#include <alf/alftexture.h>
#include <alf/alftexturemanager.h>


class TAknsItemID;
class CAlfTexture;
struct TAlfRealPoint;
class TAlfTimedPoint;
class CAlfTextureManager;
class CAlfImageLoaderUtil;

/**
 * Image Class
 * Image specifies an area on a texture object.
 * Image is a class that is used to specify the region of a texture that is going to be drawn. 
 * API methods in visuals and brushes use Image as the type of any parameters that are to 
 * specify textures used for drawing. Image is a small class that contains a reference to a 
 * texture object and the coordinates of a rectangular subregion within that texture, 
 * in normalized coordinates (0.0–1.0). 
 * 
 * Usage
 * 
 * @code
 * 
 * //Get TextureManager
 * CAlfTextureManage* textureManager = &iEnv->TextureManager();
 * 
 * //SetPath
 * textureManager->setImagePath( _L( "C:\\data\\Images\\Pictures\\" ) );
 * 
 * //LoadTexture
 * CAlfTexture* texture = textureManager->LoadTexture( _L( "1.bmp" ), 
 * 						EAlfTextureFlagDefault, AUTO_GENERATED_TEXTURE_ID );
 * 
 * //Create Image
 * TAlfImage image( *texture );
 * 
 * //Create icon as Image
 * TSize size(10,10);
 * TAlfImage image( iconUid, EAknsAppIconTypeList, size );
 * 
 * //Create image from skin logical name
 * TAlfImage image( logicalskin, size );
 * 
 * @endcode
 * 
 * @lib alfclient.lib
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( TAlfImage )
    {
public:

    /* Constructors. */
    /** @beginAPI */

    /**
     * Default constructor, creates an empty TAlfImage.
     */
    IMPORT_C TAlfImage();

    IMPORT_C ~TAlfImage();

    /**
     * Constructor. Textures coordinates will be normalized to match the
     * size of the texture, so that (1,1) is always the lower right corner.
     * Note that the TAlfImage instance stores a pointer to aTexture.
     * aTexture must continue to exist for the entire lifetime of the
     * TAlfImage instance. 
     *
     * @param aTexture  Texture being drawn.
     */
    IMPORT_C TAlfImage(const CAlfTexture& aTexture, 
                       TReal32 aTlX = 0.f, TReal32 aTlY = 0.f,
                       TReal32 aBrX = 1.f, TReal32 aBrY = 1.f) __SOFTFP;

    /**
     * Constructor. Creates image specification from the given skin parameteres. 
     * This constructor is intended to be used when one-piece skin graphics is to be defined as
     * the image.
     *
     * Actual texture object will not get created immediately, toolkit creates it later when
     * it is needed.
     *
     * If skin id does not exist in the system, image is tried to be created from the given
     * fallback .MIF or .MBM fallback iconfile.
     *
     * @param aID Skin id of the graphics to be used in the texture.
     * @param aPreferredSize Size to which the skinned graphics will be texturized.
     * @param aPreferredScaleMode Scalemode that is used when skinned graphics is texturized.
     *        Possible values are: EAspectRatioPreserved, 
     *                             EAspectRatioPreservedAndUnusedSpaceRemoved
     *                             EAspectRatioNotPreserved
     *                             EAspectRatioPreservedSlice 
     *
     * @param aFallbackFileName Filename of the .MIF iconfile. Note that ownership remains in the caller.
     * @param aFallbackBitmapId Bitmap id inside iconfile.
     * @param aFallbackMaskId Mask id inside iconfile.
     * @param aTlX Texture topleft X-coordinate.
     * @param aTlY Texture topleft Y-coordinate.    
     * @param aBrX Texture bottomright X-coordinate.
     * @param aBrY Texture bottomright Y-coordinate.
     * @param aTextureFlags Texture flags to be used when skin data is texturized.
     * @param aPreferredTextureId Texture id to be given when skin data is texturized.
     * @param aPreferredTextureManager Texture manager to be used when skin data is texturized.
     *
     */
    IMPORT_C TAlfImage(TAknsItemID aID,
                       TSize aPreferredSize,
                       TScaleMode aPreferredScaleMode = EAspectRatioPreserved, 
                       const TDesC *aFallbackFileName = NULL, 
                       TInt aFallbackBitmapId = -1, 
                       TInt aFallbackMaskId = -1,
                       TReal32 aTlX = 0.f, TReal32 aTlY = 0.f,
                       TReal32 aBrX = 1.f, TReal32 aBrY = 1.f,
                       TAlfTextureFlags aTextureFlags = EAlfTextureFlagSkinContent,
                       TInt aPreferredTextureId = KAlfAutoGeneratedTextureId,
                       CAlfTextureManager* aPreferredTextureManager = NULL) __SOFTFP;

    
    /**
     * @deprecated <b>DO NOT USE! Won't create usable TAlfImage<\b>
     * 
     * Constructor. Creates image specification from the given skin parameteres. 
     * This constructor is intended to be used when one-piece skin graphics is to be defined as
     * the image.
     *
     * Actual texture object will not get created immediately, toolkit creates it later when
     * it is needed.
     *
     * If skin id does not exist in the system, image is tried to be created from the given
     * fallback .MIF or .MBM fallback iconfile.
     *
     * @param aLogical Name Skin name of the graphics to be used in the texture.
     * @param aPreferredSize Size to which the skinned graphics will be texturized.
     * @param aPreferredScaleMode Scalemode that is used when skinned graphics is texturized.
     *        Possible values are: EAspectRatioPreserved, 
     *                             EAspectRatioPreservedAndUnusedSpaceRemoved
     *                             EAspectRatioNotPreserved
     *                             EAspectRatioPreservedSlice 
     *
     * @param aFallbackFileName Filename of the .MIF iconfile. Note that ownership remains in the caller.
     * @param aFallbackBitmapId Bitmap id inside iconfile.
     * @param aFallbackMaskId Mask id inside iconfile.
     * @param aTlX Texture topleft X-coordinate.
     * @param aTlY Texture topleft Y-coordinate.    
     * @param aBrX Texture bottomright X-coordinate.
     * @param aBrY Texture bottomright Y-coordinate.
     * @param aTextureFlags Texture flags to be used when skin data is texturized.
     * @param aPreferredTextureId Texture id to be given when skin data is texturized.
     * @param aPreferredTextureManager Texture manager to be used when skin data is texturized.
     *
     */
    IMPORT_C TAlfImage(const TDesC* aLogicalName,
            TSize aPreferredSize,
            TScaleMode aPreferredScaleMode, 
            const TDesC* aFallbackFileName, 
            TInt aFallbackBitmapId, 
            TInt aFallbackMaskId,
            TReal32 aTlX, TReal32 aTlY,
            TReal32 aBrX, TReal32 aBrY,
            TAlfTextureFlags aFlags,
            TInt aPreferredTextureId,
            CAlfTextureManager* aPreferredTextureManager) __SOFTFP;
    /**
     * Constructor. Creates image specification from the given skin parameteres. This
     * constructor is intended to be used when application icons are to be defined as the image. 
     *
     * Actual texture object will not get created immediately, toolkit creates it later when
     * it is needed.
     *
     * @param aAppIconUid Application uid which icon is to be used.
     * @param aAppIconType Application icon type.
     * @param aPreferredSize Size to which the skinned graphics will be texturized.
     * @param aPreferredScaleMode Scalemode that is used when skinned graphics is texturized.
     *        Possible values are: EAspectRatioPreserved, 
     *                             EAspectRatioPreservedAndUnusedSpaceRemoved
     *                             EAspectRatioNotPreserved
     *                             EAspectRatioPreservedSlice 
     *
     * @param aTlX Texture topleft X-coordinate.
     * @param aTlY Texture topleft Y-coordinate.    
     * @param aBrX Texture bottomright X-coordinate.
     * @param aBrY Texture bottomright Y-coordinate.
     * @param aTextureFlags Texture flags to be used when skin data is texturized.
     * @param aPreferredTextureId Texture id to be given when skin data is texturized.
     * @param aPreferredTextureManager Texture manager to be used when skin data is texturized.
     */
    IMPORT_C TAlfImage(TUid aAppIconUid, 
                       TAknsAppIconType aIconType,
                       TSize aPreferredSize,
                       TScaleMode aPreferredScaleMode = EAspectRatioPreserved, 
                       TReal32 aTlX = 0.f, TReal32 aTlY = 0.f,
                       TReal32 aBrX = 1.f, TReal32 aBrY = 1.f,
                       TAlfTextureFlags aTextureFlags = EAlfTextureFlagSkinContent,
                       TInt aPreferredTextureId = KAlfAutoGeneratedTextureId,
                       CAlfTextureManager* aPreferredTextureManager = NULL) __SOFTFP;
                       
    /**
     * Returns texture of the image.
     * @return  Texture being drawn.
     */
    IMPORT_C const CAlfTexture& Texture() const; 	                       

    /**
     * Determines the current texture coordinates of the top-left corner of
     * this image.
     */
    IMPORT_C TAlfRealPoint TopLeft() const;

    /**
     * Determines the current texture coordinates of the bottom-right corner of
     * this image.
     */
    IMPORT_C TAlfRealPoint BottomRight() const;

    /**
     * Determines if a texture has been specified for the image.
     */
    IMPORT_C TBool HasTexture() const;

    /**
     * Sets the texture of the image.
     */
    IMPORT_C void SetTexture(CAlfTexture& aTexture);

    /**
     * Apply an offset to the texture coordinates.
     */
    IMPORT_C void Translate(TReal32 aDx, TReal32 aDy, TInt aTransitionTime = 0) __SOFTFP;

    /**
     * Apply a scaling factor to the texture coordinates.
     */
    IMPORT_C void Scale(TReal32 aScaleX, TReal32 aScaleY, TInt aTransitionTime = 0) __SOFTFP;

    /**
     * Sets the texture coordinates for the whole image.
     */
    IMPORT_C void SetTexCoords(TReal32 aTlX = 0.f, TReal32 aTlY = 0.f,
                                  TReal32 aBrX = 1.f, TReal32 aBrY = 1.f,
                                  TInt aTransitionTime = 0) __SOFTFP;

    /** @endAPI */

    TAlfTimedPoint TimedTopLeft() const;

    TAlfTimedPoint TimedBottomRight() const;

    TAknsItemID SkinID() const;
    
    TSize PreferredSize() const;
    
    TScaleMode PreferredScaleMode() const;
    
    const TDesC* FallbackFileName() const;
    
    TInt FallbackBitmapId() const;
    
    TInt FallbackMaskId() const;
    
    CAlfTextureManager* PreferredTextureManager() const;

    TBool SkinnableImage() const;
    
    TAlfTextureFlags TextureFlags() const;

    TInt PreferredTextureId() const;
    
    TUid AppIconUid() const;

    TAknsAppIconType AppIconType() const;

private:

    void InitSkinData();    
    
private:
	
    /** Texture  for the image. */
	const CAlfTexture* iTexture;

    /** Texture coordinates of the top left corner, for the whole image. */
    TAlfTimedPoint iTl;

    /** Texture coordinates of the bottom right corner, for the whole image. */
    TAlfTimedPoint iBr;
	
    /** Skin id for the image. */
	TAknsItemID iSkinID;

    /** App uid for application image */
    TUid iAppIconUid;
    
    /** Icon type for application image */
    TAknsAppIconType iAppIconType;

    /** Rendering size for the image. */
    TSize iPreferredSize;
    
    /** Rendering scalemode for the image. */
    TScaleMode iPreferredScaleMode;
    
    /** Fallback image filename for the image. */
  	const TDesC* iFallbackFileName;
  	
    /** Fallback image bitmap id for the image. */
  	TInt iFallbackBitmapId;
	  
    /** Fallback image mask id for the image. */
    TInt iFallbackMaskId;
  
    /** Preferred texturemanager for loading the skinned image. */
    CAlfTextureManager* iPreferredTextureManager;

    /** Preferred texture id for skinned image. */
    TInt iPreferredTextureId;
    
    /** Flags for loading the skinned image. */
    TAlfTextureFlags iFlags;
    
    /** Flags to tell if this is skinned image. */
    TBool iSkinnableImage;
    
    /** Reserved for future */
    TInt iSpare1;
    TInt iSpare2;
    TInt iSpare3;
	
};

#endif
