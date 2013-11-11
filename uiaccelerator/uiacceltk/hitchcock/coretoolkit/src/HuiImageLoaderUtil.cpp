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
* Description:   Implements CHuiImageLoaderUtil, a utility to load skin-able image/icon.
*
*/



#include "uiacceltk/HuiImageLoaderUtil.h"
#include "uiacceltk/HuiBitmapProvider.h"
#include "uiacceltk/HuiTextureManager.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiEnv.h"

struct THuiProvidedBitmap
    {
    TInt iId;
    THuiRealSize iPreferredSize;        
    };

/**
 * Base class of the image loder classes
 */
NONSHARABLE_CLASS(CHuiImageLoader): public CBase, public MHuiBitmapProvider, public MHuiTextureAutoSizeObserver
	{
public:
	CHuiImageLoader();

    ~CHuiImageLoader();
    /**
     * Set size of the icon. This must be called before creating texture.
     * This call only sets information and dees *NOT* cause any scalable icon rasterization.
     *
     * @param aSize     Size in pixel.
     */
	void SetSize(const TSize& aSize) {iSize = aSize;}

	void SetScaleMode(TScaleMode aScaleMode) {iScaleMode = aScaleMode;}

    /**
     * Set skin instance, which is acquired from AknsUtils::SkinInstance().
     *
     * @param aSkinInstance     An Avkon skin instamce.
     */
	void SetSkinInstance(MAknsSkinInstance *aSkinInstance) {iSkinInstance = aSkinInstance;}

public:
    // From MHuiBitmapProvider
	virtual void ProvideBitmapL (TInt aId, CFbsBitmap *& aBitmap, CFbsBitmap *& aMaskBitmap) ;

    // From MHuiTextureAutoSizeObserver
    virtual TBool PreferredSizeChanged(const CHuiTexture& aTexture, TInt aTextureId, const THuiRealSize& aPreferredSize);

    // From MHuiTextureAutoSizeObserver
    virtual void PreferredSizeReportCompleted();

protected:
	MAknsSkinInstance* GetValidSkinInstance();
    TSize PreferredSize(TInt aId);

protected:
	// common source info data
	MAknsSkinInstance *iSkinInstance;
	TSize iSize;
	TScaleMode iScaleMode;
	RArray<THuiProvidedBitmap> iProvidedBitmaps;
	};


/**
 * Image loader utility for skin item image loading.
 *
 */
NONSHARABLE_CLASS(CHuiSkinItemImageLoader): public CHuiImageLoader
{
public:
    /**
     * Construct an icon source information with Avkon skin item ID. AknsUtils::CreateIconL()
     * will be called in RHuiImageLoaderUtil::CreateIconL().
     *
     * @param aInstance     Skin instance.
     * @param aID  An ID of the graphical item (e.g. an icon).
     * @param aFileName  File name of graphics file e.g. mif file) when sepcified item is not found in the current skin.
     * @param aBitmapId  ID of the bitmap when fallback is used.
     * @param aMaskId  ID of the mask bitmap when fallback is used.
     */
	CHuiSkinItemImageLoader(const TAknsItemID &aID, const TDesC &aFileName, TInt aBitmapId, TInt aMaskId);

    /**
     * Implementation of MHuiBitmapProvider interface.
     *
     * @param aId     A unique ID of the texture needed to provide bitmaps.
     * @param aBitmap     A bitmap.
     * @param aMaskBitmap     A mask bitmap.
     */
	virtual void ProvideBitmapL (TInt aId, CFbsBitmap *& aBitmap, CFbsBitmap *& aMaskBitmap) ;

private:
	TAknsItemID iId;
	TPtrC iFileName;
	TInt iBitmapId;
	TInt iMaskId;
};


/**
 * Image loader utility for application icon loading.
 *
 */
NONSHARABLE_CLASS(CHuiAppIconImageLoader): public CHuiImageLoader
{
public:
    /**
     * Construct an icon source information to create application icon. AknsUtils::CreateAppIconLC()
     * will be called in RHuiImageLoaderUtil::CreateIconL().
     *
     * @param aInstance     Skin instance.
     * @param aAppUid  Application UID of the icon to be created.
     * @param aType  Type of the application icon. Likely EAknsAppIconTypeContext is used...
     */
	CHuiAppIconImageLoader(TUid aAppUid, TAknsAppIconType aType);
    
    /**
     * Implementation of MHuiBitmapProvider interface.
     *
     * @param aId     A unique ID of the texture needed to provide bitmaps.
     * @param aBitmap     A bitmap.
     * @param aMaskBitmap     A mask bitmap.
     */
	virtual void ProvideBitmapL (TInt aId, CFbsBitmap *& aBitmap, CFbsBitmap *& aMaskBitmap) ;
private:
	TUid iAppUid;
	TAknsAppIconType iType;
};

// Image loader util
EXPORT_C CHuiImageLoaderUtil::CHuiImageLoaderUtil()
		:iSize(4,4), iScaleMode(EAspectRatioPreserved)
	{
	}


EXPORT_C CHuiImageLoaderUtil::~CHuiImageLoaderUtil()
	{
	for (TInt i=0; i<iImageLoaderList.Count(); i++)
	    {
	    delete iImageLoaderList[i];
	    }
	iImageLoaderList.Close();
	iSkinInstance = NULL;
	}


EXPORT_C MHuiBitmapProvider* CHuiImageLoaderUtil::CreateImageLoaderL(
				const TAknsItemID &aID, 
				const TDesC &aFileName, 
				TInt aBitmapId, 
				TInt aMaskId)
	{
	CHuiSkinItemImageLoader* imgLoader = new (ELeave) CHuiSkinItemImageLoader(
			aID, aFileName, aBitmapId, aMaskId);
    CleanupStack::PushL( imgLoader );
	imgLoader->SetSize(iSize);
	imgLoader->SetScaleMode(iScaleMode);
	imgLoader->SetSkinInstance(iSkinInstance);
	iImageLoaderList.AppendL(imgLoader);
	CleanupStack::Pop( imgLoader );
	return imgLoader;
	}


EXPORT_C MHuiBitmapProvider* CHuiImageLoaderUtil::CreateImageLoaderL(
				TUid aAppUid, 
				TAknsAppIconType aType)
	{
	CHuiAppIconImageLoader* imgLoader = new (ELeave) CHuiAppIconImageLoader(
			aAppUid, aType);
    CleanupStack::PushL( imgLoader );
	imgLoader->SetSize(iSize);
	imgLoader->SetScaleMode(iScaleMode);
	imgLoader->SetSkinInstance(iSkinInstance);
	iImageLoaderList.AppendL(imgLoader);
	CleanupStack::Pop( imgLoader );
	return imgLoader;
	}


EXPORT_C void CHuiImageLoaderUtil::SetSize(TSize aSize, TScaleMode aMode)
	{
	iSize = aSize;
	iScaleMode = aMode;
	}


EXPORT_C void CHuiImageLoaderUtil::SetSkinInstance(MAknsSkinInstance *aSkinInstance)
	{
	iSkinInstance = aSkinInstance;
	}



// Image loaders
MAknsSkinInstance* CHuiImageLoader::GetValidSkinInstance()
	{
	if (iSkinInstance == NULL && CCoeEnv::Static())
		{
		return AknsUtils::SkinInstance();
		}
	else
		{
		return iSkinInstance;
		}
	}

TBool CHuiImageLoader::PreferredSizeChanged(const CHuiTexture& /*aTexture*/, TInt aTextureId, const THuiRealSize& aPreferredSize)    {
    for(TInt ii = 0; ii < iProvidedBitmaps.Count(); ii++)
        {
        if (iProvidedBitmaps[ii].iId == aTextureId)
            {
            iProvidedBitmaps[ii].iPreferredSize = aPreferredSize;     
            return ETrue;
            }
        }
    return EFalse;            
    }

void CHuiImageLoader::PreferredSizeReportCompleted()
    {
    // Nothing to do    
    }

void CHuiImageLoader::ProvideBitmapL (TInt aId, CFbsBitmap *& /*aBitmap*/, CFbsBitmap *& /*aMaskBitmap*/)
    {
    for(TInt i = 0; i < iProvidedBitmaps.Count(); i++)
        {
        if (iProvidedBitmaps[i].iId == aId)
            {            
            // Already in list, do nothing
            return;   
            }
        }
    
    // If provided bitmap is not yet in the list, add it.    
    THuiProvidedBitmap providedBitmap;
    providedBitmap.iId = aId;
    providedBitmap.iPreferredSize = iSize;        
    iProvidedBitmaps.AppendL(providedBitmap);            
    }

CHuiImageLoader::CHuiImageLoader():iSize(4,4), iScaleMode(EAspectRatioPreserved)
    {
    }

CHuiImageLoader::~CHuiImageLoader()
    {
    iProvidedBitmaps.Close();
    };

TSize CHuiImageLoader::PreferredSize(TInt aId)
    {
    for(TInt i = 0; i < iProvidedBitmaps.Count(); i++)
        {
        if (iProvidedBitmaps[i].iId == aId)
            {
            // Already in list, do nothing
            return iProvidedBitmaps[i].iPreferredSize;   
            }
        }
    return iSize;    
    }


CHuiSkinItemImageLoader::CHuiSkinItemImageLoader(
		const TAknsItemID &aID, 
		const TDesC &aFileName, 
		TInt aBitmapId, 
		TInt aMaskId):
		iId(aID),
		iBitmapId(aBitmapId),
		iMaskId(aMaskId)
	{
	iFileName.Set(aFileName);
	}


void CHuiSkinItemImageLoader::ProvideBitmapL (TInt aId, CFbsBitmap *& aBitmap, CFbsBitmap *& aMaskBitmap)
	{
	CHuiImageLoader::ProvideBitmapL( aId, aBitmap, aMaskBitmap);
	MAknsSkinInstance* skininstance = GetValidSkinInstance();
	
	if (skininstance)
	    {
	    TRAPD(err, AknsUtils::CreateIconL(
					skininstance, iId,
					aBitmap, aMaskBitmap, 
					iFileName, iBitmapId, iMaskId););
	    if (err == KErrNotFound)
	        {
	        AknsUtils::CreateIconL(skininstance,
	                iId,
	                aBitmap,
	                iFileName,
	                iBitmapId );
	        }
	    else
	        {
	        User::LeaveIfError(err);
	        }
	    }
	else // proxy from other process 
	    {
	    CHuiStatic::GetMaskedBitmapL(iId,
                aBitmap, aMaskBitmap, iFileName, iBitmapId, iMaskId, PreferredSize(aId), iScaleMode );
	    return;
	    }
	    

    // Leave if bitmap or mask is NULL.
    User::LeaveIfNull( aBitmap );
    //User::LeaveIfNull( aMaskBitmap );

	// Disable compression so that the bitmap may be able to be duplicated inside
	// HUITK when BITGDI renderer is in use.
#ifndef __SERIES60_30__
	AknIconUtils::DisableCompression( aBitmap );
#else
#pragma message ("S60R30: FBS Compression can't be disabled, graphics may look crappy, deal with it")
#endif

    // Set icon sizes and render them on the bitmaps.
    // This does the actual rendering to specified size. (SVG -> bitmap)
    AknIconUtils::SetSize( aBitmap, PreferredSize(aId), iScaleMode );
	}


CHuiAppIconImageLoader::CHuiAppIconImageLoader(
		TUid aAppUid, 
		TAknsAppIconType aType):
		iAppUid(aAppUid),
		iType(aType)
	{
	}


void CHuiAppIconImageLoader::ProvideBitmapL (
		TInt aId, 
		CFbsBitmap *& aBitmap, 
		CFbsBitmap *& aMaskBitmap)
	{
	CHuiImageLoader::ProvideBitmapL( aId, aBitmap, aMaskBitmap);
    MAknsSkinInstance* skin = GetValidSkinInstance();
    if (skin)
        {
        AknsUtils::CreateAppIconLC( 
					skin, 
					iAppUid, iType,
					aBitmap, aMaskBitmap );
        }
    // Leave if bitmap or mask is NULL.
    User::LeaveIfNull( aBitmap );
    //User::LeaveIfNull( aMaskBitmap );

	// diable compression so that the bitmap may be able to be duplicated inside
	// HUITK when BITGDI renderer is in use.
#ifndef __SERIES60_30__
	AknIconUtils::DisableCompression( aBitmap );
#else
#pragma message ("S60R30: FBS Compression can't be disabled, graphics may look crappy, deal with it")
#endif

    // Set icon sizes and render them on the bitmaps.
    // This does the actual rendering to specified size. (SVG -> bitmap)
    AknIconUtils::SetSize( aBitmap, PreferredSize(aId), iScaleMode );
    
	// pop the passed bitmap and mask 
	CleanupStack::Pop(2);
	}

EXPORT_C void CHuiImageLoaderUtil::ImageLoaderExtension(const TUid& /*aExtensionUid*/, TAny** /*aExtensionParamters*/ )
    {
    
    }

EXPORT_C TBool CHuiAutoSizeImageLoaderUtil::PreferredSizeChanged(const CHuiTexture& aTexture, TInt aTextureId, const THuiRealSize& aPreferredSize)
    {
    TBool ret = EFalse;
    TInt count = iImageLoaderList.Count();
    for (TInt i=0; i<count;i++)
        {
        ret |= iImageLoaderList[i]->PreferredSizeChanged(aTexture, aTextureId, aPreferredSize);    
        }
    return ret;    
        
    }
    
EXPORT_C void CHuiAutoSizeImageLoaderUtil::PreferredSizeReportCompleted()
    {
    // Do nothing    
    }
