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
* Description:   Definition of CHuiImageLoaderUtil, a utility to load skin-able image/icon.
*
*/



#ifndef __HUIIMAGELOADERUTIL_H__
#define __HUIIMAGELOADERUTIL_H__

#include <AknsUtils.h>
#include <AknIconUtils.h>
#include <uiacceltk/HuiTextureManager.h>


class MHuiBitmapProvider;
class CHuiImageLoader;

/**
 * CHuiImageLoaderUtil is a utility class to create image loader with the system skin 
 * applied. CHuiImageLoaderUtil holds all MHuiBitmapProvider instance created through
 * CreateImageLoaderL() so that appplication/control does not need to keep it by itself.
 * 
 * Typically, an application or a control needs to hold instance of CHuiImageLoaderUtil
 * during entire its object life time, because bitmap provision callback from texture
 * manager may be called even after instanciation of texture. For instance, image re-loading
 * happens when application coming back to foreground and resource was released.
 */
class CHuiImageLoaderUtil: public CBase
	{
public:
	/**
	 * Constructor
	 */
	IMPORT_C CHuiImageLoaderUtil();

	/**
	 * Deconstructor
	 */
	IMPORT_C ~CHuiImageLoaderUtil();
	
	/**
     * Create MHuiBitmapProvider instance to load image with Avkon skin item ID with 
     * fallback. The fallback is used when the current skin does not contain the image with 
     * skin item ID. If either fails, this leaves.
     * Use CHuiTextureManager::CreateTextureL() with return value of this method as a
     * bitmap provider to create a HUI texture.
     *
     * @param aID  An Akvon skin item ID of the graphical item (e.g. an icon) to load.
     * @param aFileName  File name of graphics file e.g. mif file) when sepcified item is not found in the current skin.
     * @param aBitmapId  ID of the bitmap when fallback is used.
     * @param aMaskId  ID of the mask bitmap when fallback is used.
     */
	IMPORT_C MHuiBitmapProvider* CreateImageLoaderL(const TAknsItemID &aID, const TDesC &aFileName, TInt aBitmapId, TInt aMaskId);

    /**
     * Create MHuiBitmapProvider instance to load Application icon image.
     * Use CHuiTextureManager::CreateTextureL() with return value of this method as a
     * bitmap provider to create a HUI texture.
     *
     * @param aAppUid  Application UID of the icon to be created.
     * @param aType  Type of the application icon. Likely EAknsAppIconTypeContext is used...
     */
	IMPORT_C MHuiBitmapProvider* CreateImageLoaderL(TUid aAppUid, TAknsAppIconType aType);


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

    IMPORT_C virtual void ImageLoaderExtension(const TUid& aExtensionUid, TAny** aExtensionParamters );

private:
	/** Skin instance. */
	MAknsSkinInstance *iSkinInstance;
	
	/** Size of image to be created */
	TSize iSize;
	
	/** Scale mode of image to be created */
	TScaleMode iScaleMode;
	    
protected:

	/** An array to hold all loader created. */
	RPointerArray<CHuiImageLoader> iImageLoaderList;

private:

    TAny* iSpare;
	};


class CHuiAutoSizeImageLoaderUtil : public CHuiImageLoaderUtil, public MHuiTextureAutoSizeObserver
    {
    // From MHuiTextureAutoSizeObserver
    IMPORT_C virtual TBool PreferredSizeChanged(const CHuiTexture& aTexture, TInt aTextureId, const THuiRealSize& aPreferredSize);    
    
    // From MHuiTextureAutoSizeObserver
    IMPORT_C virtual void PreferredSizeReportCompleted();        
    };

#endif // __HUIIMAGELOADERUTIL_H__