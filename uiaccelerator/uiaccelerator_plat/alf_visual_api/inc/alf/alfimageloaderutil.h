/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Utility for creating textures from skin graphics
*
*/



#ifndef C_AlfIMAGELOADERUTIL_H
#define C_AlfIMAGELOADERUTIL_H

#include <AknsUtils.h>
#include <AknIconUtils.h>
#include <alf/alftexturemanager.h>

class CAlfImageLoader;
class MAlfBitmapProvider;
class CAlfTextureManager;

/**
 * CAlfImageLoaderUtil is a utility class to create image loader with the system skin 
 * applied. CAlfImageLoaderUtil holds all MAlfBitmapProvider instance created through
 * CreateImageLoaderL() so that appplication/control does not need to keep it by itself.
 * 
 * Typically, an application or a control needs to hold instance of CAlfImageLoaderUtil
 * during entire its object life time, because bitmap provision callback from texture
 * manager may be called even after instanciation of texture. For instance, image re-loading
 * happens when application coming back to foreground and resource was released.
 *  * 
 * Usage:
 * 
 * @code
 *  // Create ImageLoaderUtil
 * CAlfImageLoaderUtil* imageloader = new ( ELeave ) CAlfImageLoaderUtil();
 * 
 * //To create svgImageLoader
 * MAlfBitmapProvider* bitmapprovider = imageloader->CreateSVGImageLoaderL( svgfilename );	
 * 
 * //Create texture using bitmap provider istance
 * CAlfTexture* texture = iTextureMgr->CreateTexture( 42, *bitmapprovider, EAlfTextureFlagDefault );
 * 
 * //Use texture to create image istance
 * TAlfImage image( *texture );
 * 
 * //To create iconbased bitmap provider
 * bitmapprovider = imageloader->createImageLoader( iconUid, EAknsAppIconTypeList );
 * 
 * //Cleanup
 *  delete imageloader
 *  delete bitmapprovider;
 * @endcode
 * 
 * @see CAlfTextureManager::CreateTexture()
 * 
 * @lib alfclient.lib
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CAlfImageLoaderUtil ): public CBase
	{
public:
	/**
	 * Constructor
	 */
	IMPORT_C CAlfImageLoaderUtil();

	/**
	 * Deconstructor
	 */
	IMPORT_C ~CAlfImageLoaderUtil();
	
	/**
     * Create MAlfBitmapProvider instance to load image with Avkon skin item ID with 
     * fallback. The fallback is used when the current skin does not contain the image with 
     * skin item ID. If either fails, this leaves.
     * Use CAlfTextureManager::CreateTextureL() with return value of this method as a
     * bitmap provider to create a Alf texture. You can set the size of the provided bitmap
     * in pixels using SetSize().
     *
     * @param aID  An Akvon skin item ID of the graphical item (e.g. an icon) to load.
     * @param aFileName  File name of graphics file e.g. mif file) when sepcified item is not found in the current skin.
     * @param aBitmapId  ID of the bitmap when fallback is used.
     * @param aMaskId  ID of the mask bitmap when fallback is used.
     */
	IMPORT_C MAlfBitmapProvider* CreateImageLoaderL(const TAknsItemID &aID, const TDesC &aFileName, TInt aBitmapId, TInt aMaskId);

	/**
     * Create MAlfBitmapProvider instance to load frame image with Avkon skin item ID 
     * Use CAlfTextureManager::CreateTextureL() with return value of this method as a
     * bitmap provider to create a Alf texture. 
     *
     * @param aFrameID  An Avkon skin item ID of the graphical frame item to load.
     * @param aInnerRect Inner rect of the frame
     * @param aOuterRect Outer rect of the frame
     */
	IMPORT_C MAlfBitmapProvider* CreateImageLoaderL(const TAknsItemID &aFrameID, const TRect& aInnerRect, const TRect& aOuterRect);

    /**
     * Create MAlfBitmapProvider instance to load Application icon image.
     * Use CAlfTextureManager::CreateTextureL() with return value of this method as a
     * bitmap provider to create a Alf texture. You can set the size of the provided bitmap
     * in pixels using SetSize().
     *
     * @param aAppUid  Application UID of the icon to be created.
     * @param aType  Type of the application icon. Likely EAknsAppIconTypeContext is used...
     */
	IMPORT_C MAlfBitmapProvider* CreateImageLoaderL(TUid aAppUid, TAknsAppIconType aType);


    /**
     * Create MAlfBitmapProvider instance to load an SVG Image
     * Use CAlfTextureManager::CreateTextureL() with return value of this method as a
     * bitmap provider to create a Alf texture. You can set the size of the provided bitmap
     * in pixels using SetSize(). This function works only with true SVG-T files.
     *
     * @param aFilename  Filename of the SVG file to load. If filename does not contain full path
     * then path is taken from primary env default texturemanager image path. 
     * 
     */
	IMPORT_C MAlfBitmapProvider* CreateSVGImageLoaderL(const TDesC &aFileName);

    /**
     * Set size of the image to be loaded. This must be called before calling CreateImageLoaderL.
     * This call only sets information and does *NOT* cause any scalable icon rasterization. Also
     * Size setting does not affect any of image loaders already created.
     *
     * @param aSize     Size in pixel.
     * @param aMode     Scaling mode enumeration from Avkon. It takes either of following three.
     *                    EAspectRatioPreserved
     *                    EAspectRatioPreservedAndUnusedSpaceRemoved
     *                    EAspectRatioNotPreserved
     *
     */
	IMPORT_C void SetSize(TSize aSize, TScaleMode aMode=EAspectRatioPreserved);

    /**
     * Set skin instance, which is acquired from AknsUtils::SkinInstance(). If skin instance
     * is not explicitly set with this method, the image loader will use skin instance from 
     * AknsUtils::SkinInstance().
     *
     * @param aSkinInstance     An Avkon skin instamce.
     */
	IMPORT_C void SetSkinInstance(MAknsSkinInstance *aSkinInstance);

protected:

	/** An array to hold all loader created. */
	RPointerArray<CAlfImageLoader> iImageLoaderList;

private:
	/** Skin instance. */
	MAknsSkinInstance *iSkinInstance;
	
	/** Size of image to be created */
	TSize iSize;
	
	/** Scale mode of image to be created */
	TScaleMode iScaleMode;
		
	};


/**
 * CAlfAutoSizeImageLoaderUtil is an extended image loader utility class.
 * This class can automatically keep track of the preferred sizes for
 * textures and their bitmap providers and thus always provide correct size 
 * bitmaps.
 * 
 * Example of use:
 * 
 * @code
 *
 * // Create CAlfAutoSizeImageLoaderUtil
 * iImageloader = new (ELeave) CAlfAutoSizeImageLoaderUtil;
 *
 * // Register CAlfAutoSizeImageLoaderUtil as autosize observer
 * texturemanager->AddAutoSizeObserverL(iImageloader); 
 *
 * // Create provider for a skin item
 * MAlfBitmapProvider* imageprovider = iImageloader->CreateImageLoaderL(SkinID(), 
 *      FallbackFileName(),
 *      FallbackBitmapId(),
 *      FallbackMaskId());            
 * 
 * // Create a texture using provider, set autosize flags to enable automatic texture resizing.
 * texturemanager->CreateTextureL(KAlfAutoGeneratedTextureId,
 *     imageprovider,
 *     EAlfTextureFlagAutoSize);
 * 
 * @endcode
 */
NONSHARABLE_CLASS( CAlfAutoSizeImageLoaderUtil ): public CAlfImageLoaderUtil, public MAlfTextureAutoSizeObserver
    {
public:
    /**
     * Constructor
     */
    IMPORT_C CAlfAutoSizeImageLoaderUtil();
    
private:
    // From MAlfTexturePreferredSizeObserver
    IMPORT_C TBool PreferredSizeChanged(const CAlfTexture& aChangedTexture, TSize aPreferredSize);
        
    };

#endif // C_AlfIMAGELOADERUTIL_H