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
* Description:   Utility for loading skin graphics.
*
*/




#include "alf/alfimageloaderutil.h"
#include "alf/alfbitmapprovider.h"
#include "alf/alfstatic.h"
#include "alf/alfenv.h"
#include <SVGEngineInterfaceImpl.h>
#include <SVGRequestObserver.h>
#include <AknsItemData.h>
#include <AknsUtils.h>

#include <uiacceltk/HuiUtil.h>

// Set this flag to ETrue if autosize textures need to be exluded from Avkon
// icon cache, e.g. because it get filled with different size variants of same icon.
// Excluding is likely to decrease texture upload performance.
const TBool KExcludeAutosizeTextureIconsFromAvkonIconCache = EFalse;

const TInt KSkinFrameBorderWidthDefaultValue = 8;
const TInt KSkinFrameBorderWidthSmallValue = 4;
const TInt KSkinFrameBorderWidthSmallestValue = 1;
const TInt KSkinFrameWidthMinValue = 1;

const TInt KAlfImageLoaderDefaultWidth = 4;
const TInt KAlfImageLoaderDefaultHeight = 4;
const TScaleMode KAlfImageLoaderDefaultScaleMode = EAspectRatioPreserved;

struct TAlfProvidedBitmap
    {
    TInt iId;
    TSize iPreferredSize;        
    };

/**
 * Base class of the image loader classes
 */
class CAlfImageLoader: public CBase, public MAlfBitmapProvider, public MAlfTextureAutoSizeObserver
	{
public:
	CAlfImageLoader();

    ~CAlfImageLoader();

    /**
     * Set size of the icon. This must be called before creating texture.
     * This call only sets information and dees *NOT* cause any scalable icon rasterization.
     *
     * @param aSize     Size in pixel.
     */
	void SetSize(TSize aSize) {iSize = aSize;}

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
    virtual TBool PreferredSizeChanged(const CAlfTexture& aChangedTexture, TSize aPreferredSize);

protected:
	MAknsSkinInstance* GetValidSkinInstance();
    TSize PreferredSize(TInt aId) const;

protected:
	// common source info data
	MAknsSkinInstance *iSkinInstance;
	TSize iSize;
	TScaleMode iScaleMode;
	RArray<TAlfProvidedBitmap> iProvidedBitmaps;
	TBool iExcludeFromAvkonIconCache;
	};


/**
 * Image loader utility for skin item image loading.
 *
 */
NONSHARABLE_CLASS(CAlfSkinItemImageLoader): public CAlfImageLoader
{
public:

    /**
     * Default constructor      
     */
    CAlfSkinItemImageLoader();

    /**
     * Construct an icon source information with Avkon skin item ID. AknsUtils::CreateIconL()
     * will be called in RAlfImageLoaderUtil::CreateIconL().
     *
     * @param aInstance     Skin instance.
     * @param aID  An ID of the graphical item (e.g. an icon).
     * @param aBitmapId  ID of the bitmap when fallback is used.
     * @param aMaskId  ID of the mask bitmap when fallback is used.
     */
    CAlfSkinItemImageLoader( const TAknsItemID &aID, TInt aBitmapId, TInt aMaskId );

    /**
     * Construct an icon source information with Avkon skin item ID. AknsUtils::CreateIconL()
     * will be called in RAlfImageLoaderUtil::CreateIconL().
     *
     * @param aFileName  File name of graphics file e.g. mif file) when sepcified item is not found in the current skin.
     */
	void ConstructL( const TDesC &aFileName );

	/**
     * Destructor
     */		
	~CAlfSkinItemImageLoader();

    /**
     * Implementation of MAlfBitmapProvider interface.
     *
     * @param aId     A unique ID of the texture needed to provide bitmaps.
     * @param aBitmap     A bitmap.
     * @param aMaskBitmap     A mask bitmap.
     */
	virtual void ProvideBitmapL (TInt aId, CFbsBitmap *& aBitmap, CFbsBitmap *& aMaskBitmap) ;

protected:
	TAknsItemID iId;
	HBufC* iFileName;
	TInt iBitmapId;
	TInt iMaskId;
};


/**
 * Image loader utility for skin frame item image loading.
 *
 */
NONSHARABLE_CLASS(CAlfSkinFrameItemImageLoader): public CAlfSkinItemImageLoader
{
public:

    enum TFrameParts
            {
            EFramePartCornerTl = 0,
            EFramePartCornerTr,
            EFramePartCornerBl, 
            EFramePartCornerBr,
            EFramePartSideT,
            EFramePartSideB,
            EFramePartSideL,
            EFramePartSideR,
            EFramePartCenter
            };
            
    /**
     * Create MAlfBitmapProvider instance to load frame image with Avkon skin item ID.
     * @param aFrameID  An Akvon skin item ID of the graphical frame item to load.
     * @param aInnerRect Inner rect of the frame
     * @param aOuterRect Outer rect of the frame
     */
    CAlfSkinFrameItemImageLoader( const TAknsItemID &aFrameID, const TRect& aInnerRect, const TRect& aOuterRect );

    /**
     * Construct an icon source information with Avkon skin item ID. AknsUtils::CreateIconL()
     * will be called in RAlfImageLoaderUtil::CreateIconL().
     *
     * @param aFileName  File name of graphics file e.g. mif file) when sepcified item is not found in the current skin.
     * @param aBitmapId  ID of the bitmap when fallback is used.
     * @param aMaskId  ID of the mask bitmap when fallback is used.
     */
	void ConstructL( const TDesC &aFileName, TInt aBitmapId, TInt aMaskId );

	/**
     * Destructor
     */		
	~CAlfSkinFrameItemImageLoader();

    /**
     * Implementation of MAlfBitmapProvider interface.
     *
     * @param aId     A unique ID of the texture needed to provide bitmaps.
     * @param aBitmap     A bitmap.
     * @param aMaskBitmap     A mask bitmap.
     */
	virtual void ProvideBitmapL (TInt aId, CFbsBitmap *& aBitmap, CFbsBitmap *& aMaskBitmap) ;

private:
    
    TSize GetFramePartSize(const TFrameParts aFramePart, const TSize& aFullFrameSize) const;
    TPoint GetFramePartPos(const TFrameParts aFramePart, const TSize& aFullFrameSize) const;
    TRect AdjustedOuterRect(const TSize& aFullFrameSize) const;
    TRect AdjustedInnerRect(const TSize& aFullFrameSize) const;
    TInt FrameBorderWidth() const;

private:

    TRect iInnerRect; 
    TRect iOuterRect;
};

/**
 * Image loader utility for loading Svg Images
 *
 */
NONSHARABLE_CLASS(CAlfSvgImageLoader): public CAlfImageLoader, public MSvgRequestObserver
{
public:

    /**
     * Construct an SVG loader
     *
     * @param aSVGFile     Svg File name.     
     */
    static CAlfSvgImageLoader* NewLC(const TDesC& aSVGFile); 

    /**
     * Destructor
     */		
	~CAlfSvgImageLoader();
    
    /**
     * Implementation of MAlfBitmapProvider interface.
     *
     * @param aId     A unique ID of the texture needed to provide bitmaps.
     * @param aBitmap     A bitmap.
     * @param aMaskBitmap     A mask bitmap.
     */
	virtual void ProvideBitmapL (TInt aId, CFbsBitmap *& aBitmap, CFbsBitmap *& aMaskBitmap) ;
	
    /**
     * Implementation of MSvgRequestObserver interface. - UpdateScreen
     */
	void UpdateScreen();
	
    /**
     * Implementation of MSvgRequestObserver interface. - ScriptCall
     */	
	TBool ScriptCall( const TDesC& aScript,CSvgElementImpl* aCallerElement );
	
    /**
     * Implementation of MSvgRequestObserver interface.- FetchImage
     */	
    TInt FetchImage( const TDesC& aUri, RFs& aSession, RFile& aFileHandle );
    
    /**
     * Implementation of MSvgRequestObserver interface.-UpdatePresentation
     */    
    void UpdatePresentation(const TInt32&  aNoOfAnimation);
    
    /**
     * Implementation of MSvgRequestObserver interface.-FetchFont
     */    
    TInt FetchFont( const TDesC& aUri, RFs& aSession, RFile& aFileHandle );	
	
private:	
	CAlfSvgImageLoader();
	void ConstructL(const TDesC& aSVGFile);
	void LoadSVGFileL(TInt aId);
private:
    CSvgEngineInterfaceImpl* iSvgEngine;//owned
    CFbsBitmap* iBitmap;//not owned
    CFbsBitmap* iMask; //not owned
    CFbsBitmap* iDummyBitmap;//owned
    HBufC* iFileName;
};

CAlfSvgImageLoader::~CAlfSvgImageLoader()
{
	delete iSvgEngine;
    delete iDummyBitmap;
    delete iBitmap;
    delete iMask; 	
    delete iFileName;
}
// -----------------------------------------------------------------------------
// CSVGPerfTestAppView::UpdateScreen
// -----------------------------------------------------------------------------
//
void CAlfSvgImageLoader::UpdateScreen()
    {
    }
    
// -----------------------------------------------------------------------------
// CSVGPerfTestAppView::ScriptCall
// -----------------------------------------------------------------------------
//
TBool CAlfSvgImageLoader::ScriptCall(
    const TDesC& /*aScript*/,
    CSvgElementImpl* /*aCallerElement*/ )
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CAlfSvgImageLoader::FetchImage
// -----------------------------------------------------------------------------
//
TInt CAlfSvgImageLoader:: FetchImage(
    const TDesC& /*aUri*/, RFs& /*aSession*/, RFile& /*aFileHandle*/ )
    {
    return KErrNotSupported;
    }
    
// -----------------------------------------------------------------------------
// CSVGPerfTestAppView::UpdatePresentation
// -----------------------------------------------------------------------------
//
void CAlfSvgImageLoader::UpdatePresentation(
    const TInt32& /*aNoOfAnimation*/ )
    {
    }
    
// -----------------------------------------------------------------------------
// CAlfSvgImageLoader::UpdatePresentation
// -----------------------------------------------------------------------------
//    
TInt CAlfSvgImageLoader::FetchFont( const TDesC& /*aUri*/, RFs& /*aSession*/, RFile& /*aFileHandle*/ )
    {
    return KErrNotSupported;
    }   


void CAlfSvgImageLoader::LoadSVGFileL(TInt aId)
{
  	// delete previous content
	
	delete iBitmap;
	iBitmap = NULL;
	delete iMask;
	iMask = NULL;
	
	// create frame and frame mask buffers		
	iBitmap = new( ELeave ) CFbsBitmap;
	User::LeaveIfError( iBitmap->Create( PreferredSize(aId), EColor64K ) );

	iMask = new( ELeave ) CFbsBitmap;
	User::LeaveIfError( iMask->Create( PreferredSize(aId), EGray256 ) );
	

    // If filename does not contain full path, then prepend the path from 
    // primary envs default texture manager.
    TFileName filename;
    TParsePtrC parse(*iFileName);
    if (!parse.DrivePresent())
        {
        filename.Append(CAlfStatic::Env().TextureManager().ImagePath());        
        }    
    filename.Append(*iFileName);
	
	// Load and render SVG file
	TInt handle = 0;
	iSvgEngine->PrepareDom( filename, handle ) ;
	iSvgEngine->UseDom( handle, iBitmap, iMask );
	
	// check the aspect ratio (taken from AknSvgFormatHandler.cpp)
	TSvgPreserveAspectAlignType alignTypeValue = ESvgPreserveAspectRatio_XmidYmid;
    TSvgMeetOrSliceType meetOrSliceTypeValue = ESvgMeetOrSlice_Meet;
    switch ( iScaleMode )
        {
        case EAspectRatioPreserved: // fall through
            {
            // use default
            break;
            }
        // Ensures SVG content fully covers            
        // the area of the icon whilst preserving aspect ratio.
        case EAspectRatioPreservedSlice:
            {
            // alignTypeValue use default
            meetOrSliceTypeValue = ESvgMeetOrSlice_Slice;
            break;
            } 
        // EAspectRatioPreservedAndUnusedSpaceRemoved is mapped to
        // the same values as EAspectRatioNotPreserved
        // because we already have a frame buffer
        // with the dimensions that preserves the aspect ratio.
        // This mapping ensures that SVG engine does not calculate aspect
        // ratio twice and potentially resulting in precision loss.
        case EAspectRatioPreservedAndUnusedSpaceRemoved:                        
        case EAspectRatioNotPreserved:
           {            
            alignTypeValue = ESvgPreserveAspectRatio_None;
           // meetOrSliceTypeValue use default
            break;
            }
        default:
            {
            __ASSERT_DEBUG( 0, USER_INVARIANT() );
           }
        }    
            
    iSvgEngine->SetPreserveAspectRatio( NULL, alignTypeValue, meetOrSliceTypeValue, ETrue);      
	

	// render frame and frame mask
	iSvgEngine->Start();	
	iSvgEngine->UseDom( handle, NULL, NULL );
	iSvgEngine->DeleteDom( handle );  

}

CAlfSvgImageLoader* CAlfSvgImageLoader::NewLC(const TDesC& aSVGFile)
    {
    CAlfSvgImageLoader* self = new ( ELeave ) CAlfSvgImageLoader();
    CleanupStack::PushL( self );
    self->ConstructL(aSVGFile);
    return self;
    }

CAlfSvgImageLoader::CAlfSvgImageLoader()
    {
    }

void CAlfSvgImageLoader::ConstructL(const TDesC& aSVGFile)
    {
    iFileName = aSVGFile.AllocL();
    // Initialize SVG engine
    TFontSpec spec;
	if ( !iDummyBitmap )
	    {
	    // Have to give some bitmap to the engine in the constructor.
	    iDummyBitmap = new( ELeave ) CFbsBitmap;
	    TInt error = iDummyBitmap->Create( TSize( 0, 0 ), EGray2 );
	    if ( error != KErrNone )
	        {
	        delete iDummyBitmap;
	        iDummyBitmap = NULL;
	        User::Leave( error );
	        }
	    }
	    
	iSvgEngine = CSvgEngineInterfaceImpl::NewL( iDummyBitmap, this, spec );
	iSvgEngine->SetBackgroundColor( 0 );
	// No DRM check needed.
	iSvgEngine->SetDRMMode( EFalse );
    }

void CAlfSvgImageLoader::ProvideBitmapL (TInt aId, CFbsBitmap *& aBitmap, CFbsBitmap *& aMaskBitmap)
{
	CAlfImageLoader::ProvideBitmapL (aId, aBitmap, aMaskBitmap);

	LoadSVGFileL(aId);
	aBitmap = iBitmap; //ownership transfered
	aMaskBitmap = iMask; // ownership transfered
    iBitmap = NULL;
    iMask = NULL;
}


/**
 * Image loader utility for application icon loading.
 *
 */
NONSHARABLE_CLASS(CAlfAppIconImageLoader): public CAlfImageLoader
{
public:
    /**
     * Construct an icon source information to create application icon. AknsUtils::CreateAppIconLC()
     * will be called in RAlfImageLoaderUtil::CreateIconL().
     *
     * @param aInstance     Skin instance.
     * @param aAppUid  Application UID of the icon to be created.
     * @param aType  Type of the application icon. Likely EAknsAppIconTypeContext is used...
     */
	CAlfAppIconImageLoader(TUid aAppUid, TAknsAppIconType aType);
    
    /**
     * Implementation of MAlfBitmapProvider interface.
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
EXPORT_C CAlfImageLoaderUtil::CAlfImageLoaderUtil()
		:iSize(KAlfImageLoaderDefaultWidth,
		       KAlfImageLoaderDefaultHeight), 
		 iScaleMode(KAlfImageLoaderDefaultScaleMode)
	{
	}


EXPORT_C CAlfImageLoaderUtil::~CAlfImageLoaderUtil()
	{
	for (TInt i=0; i<iImageLoaderList.Count(); i++)
		delete iImageLoaderList[i];
	iImageLoaderList.Close();
	}


EXPORT_C MAlfBitmapProvider* CAlfImageLoaderUtil::CreateImageLoaderL(
				const TAknsItemID &aID, 
				const TDesC &aFileName, 
				TInt aBitmapId, 
				TInt aMaskId)
	{
	// Create frame item loader, it can also handle normal items.
	CAlfSkinFrameItemImageLoader* imgLoader = new (ELeave) CAlfSkinFrameItemImageLoader(
			aID, TRect(0,0,0,0), TRect(0,0,0,0));

    CleanupStack::PushL( imgLoader );
    imgLoader->ConstructL( aFileName, aBitmapId, aMaskId );
	imgLoader->SetSize(iSize);
	imgLoader->SetScaleMode(iScaleMode);
	imgLoader->SetSkinInstance(iSkinInstance);
	iImageLoaderList.AppendL(imgLoader);
	CleanupStack::Pop( imgLoader );
	return imgLoader;
	}

EXPORT_C MAlfBitmapProvider* CAlfImageLoaderUtil::CreateImageLoaderL(
    const TAknsItemID &aFrameID, 
    const TRect& aInnerRect, 
    const TRect& aOuterRect)
    {
	CAlfSkinItemImageLoader* imgLoader = new (ELeave) CAlfSkinFrameItemImageLoader(
			aFrameID, aInnerRect, aOuterRect);
    CleanupStack::PushL( imgLoader );
	imgLoader->SetSize(aOuterRect.Size());
	imgLoader->SetScaleMode(iScaleMode);
	imgLoader->SetSkinInstance(iSkinInstance);
	iImageLoaderList.AppendL(imgLoader);
	CleanupStack::Pop( imgLoader );	
	return imgLoader;        
    }



EXPORT_C MAlfBitmapProvider* CAlfImageLoaderUtil::CreateSVGImageLoaderL(
				const TDesC &aFileName)
	{
	CAlfSvgImageLoader* imgLoader = CAlfSvgImageLoader::NewLC(aFileName);
	imgLoader->SetSize(iSize);
	imgLoader->SetScaleMode(iScaleMode);
    iImageLoaderList.AppendL(imgLoader);
	CleanupStack::Pop( imgLoader );
	return imgLoader;
	}


EXPORT_C MAlfBitmapProvider* CAlfImageLoaderUtil::CreateImageLoaderL(
				TUid aAppUid, 
				TAknsAppIconType aType)
	{
	CAlfAppIconImageLoader* imgLoader = new (ELeave) CAlfAppIconImageLoader(
			aAppUid, aType);
    CleanupStack::PushL( imgLoader );
	imgLoader->SetSize(iSize);
	imgLoader->SetScaleMode(iScaleMode);
	imgLoader->SetSkinInstance(iSkinInstance);
	iImageLoaderList.AppendL(imgLoader);
	CleanupStack::Pop( imgLoader );
	return imgLoader;
	}


EXPORT_C void CAlfImageLoaderUtil::SetSize(TSize aSize, TScaleMode aMode)
	{
	iSize = aSize;
	iScaleMode = aMode;
	}


EXPORT_C void CAlfImageLoaderUtil::SetSkinInstance(MAknsSkinInstance *aSkinInstance)
	{
	iSkinInstance = aSkinInstance;
	}


// Image loaders
CAlfImageLoader::CAlfImageLoader():
    iSize(KAlfImageLoaderDefaultWidth,KAlfImageLoaderDefaultHeight), 
    iScaleMode(KAlfImageLoaderDefaultScaleMode)
    {
    }

CAlfImageLoader::~CAlfImageLoader()
    {
    iProvidedBitmaps.Close();
    };

MAknsSkinInstance* CAlfImageLoader::GetValidSkinInstance()
	{
	if (iSkinInstance == NULL)
		{
		return AknsUtils::SkinInstance();
		}
	else
		{
		return iSkinInstance;
		}
	}

TSize CAlfImageLoader::PreferredSize(TInt aId) const
    {
    for(TInt i = 0; i < iProvidedBitmaps.Count(); i++)
        {
        if (iProvidedBitmaps[i].iId == aId)
            {
            // Already in list
            return iProvidedBitmaps[i].iPreferredSize;   
            }
        }
    return iSize;    
    }


TBool CAlfImageLoader::PreferredSizeChanged(const CAlfTexture& aChangedTexture, TSize aPreferredSize)
    {
    // Check if any of my bitmaps preferred size has changed
    for(TInt ii = 0; ii < iProvidedBitmaps.Count(); ii++)
        {
        if (iProvidedBitmaps[ii].iId == aChangedTexture.Id())
            {
            iProvidedBitmaps[ii].iPreferredSize = aPreferredSize;     
            
            TInt flags = aChangedTexture.Flags();
            
            // Exclude autosized bitmaps from avkon icon cache
            if (KExcludeAutosizeTextureIconsFromAvkonIconCache && 
                (flags & EAlfTextureFlagAutoSize))
                {                
                iExcludeFromAvkonIconCache = ETrue;
                }            

            // If bitmap is excuded from avkon cache, we can use direct upload to gain slight
            // performance improvement in bitgdi. No fear of anyone sharing the bitmap or compressing
            // it etc.
            if (iExcludeFromAvkonIconCache)
                {
                flags |= 0x20; // "Secret" flag, not in public API    
                CAlfTexture* changedTexture = const_cast<CAlfTexture*> (&aChangedTexture);
                changedTexture->SetFlags(TAlfTextureFlags(flags));                    
                }
            
            return ETrue;
            }
        }        
    return EFalse;
    }

void CAlfImageLoader::ProvideBitmapL (TInt aId, CFbsBitmap *& /*aBitmap*/, CFbsBitmap *& /*aMaskBitmap*/)
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
    TAlfProvidedBitmap providedBitmap;
    providedBitmap.iId = aId;
    providedBitmap.iPreferredSize = iSize;        
    iProvidedBitmaps.AppendL(providedBitmap);            
    }


CAlfSkinItemImageLoader::CAlfSkinItemImageLoader():
		iId(KAknsIIDNone),
		iFileName(NULL),
		iBitmapId(0),
		iMaskId(0)
	{
	}

CAlfSkinItemImageLoader::CAlfSkinItemImageLoader(
		const TAknsItemID &aID, 
		TInt aBitmapId, 
		TInt aMaskId):
		iId(aID),
		iBitmapId(aBitmapId),
		iMaskId(aMaskId)
	{
	}

void CAlfSkinItemImageLoader::ConstructL( const TDesC &aFileName )
	{
	iFileName = aFileName.AllocL();
	}

CAlfSkinItemImageLoader::~CAlfSkinItemImageLoader()
    {
    delete iFileName;
    }


void CAlfSkinItemImageLoader::ProvideBitmapL (TInt aId, CFbsBitmap *& aBitmap, CFbsBitmap *& aMaskBitmap)
	{
	CAlfImageLoader::ProvideBitmapL (aId, aBitmap, aMaskBitmap);
	
	AknsUtils::CreateIconL(
					GetValidSkinInstance(), iId,
					aBitmap, aMaskBitmap, 
					*iFileName, iBitmapId, iMaskId);

    // Leave if bitmap or mask is NULL.
    User::LeaveIfNull( aBitmap );
    //User::LeaveIfNull( aMaskBitmap );

	// Disable compression so that the bitmap may be able to be duplicated inside
	// AlfTK when BITGDI renderer is in use.
	AknIconUtils::DisableCompression( aBitmap );

    // Exclude from icon cache to avoid flooding it or to avoid bitmap sharing
    if (iExcludeFromAvkonIconCache)
        {
        AknIconUtils::ExcludeFromCache( aBitmap );    
        }

    // Set icon sizes and render them on the bitmaps.
    // This does the actual rendering to specified size. (SVG -> bitmap)
    AknIconUtils::SetSize( aBitmap, PreferredSize(aId), iScaleMode );
	}



CAlfSkinFrameItemImageLoader::CAlfSkinFrameItemImageLoader( const TAknsItemID &aFrameID, 
    const TRect& aInnerRect, 
    const TRect& aOuterRect ):
    iInnerRect(aInnerRect),
    iOuterRect(aOuterRect)
    {
    iId = aFrameID;                

    // Sanity check to avoid evil things happening later
    if (aOuterRect.iTl.iX - aInnerRect.iTl.iX > KSkinFrameWidthMinValue ||
        aOuterRect.iTl.iY - aInnerRect.iTl.iY > KSkinFrameWidthMinValue ||
        aOuterRect.iBr.iX - aInnerRect.iBr.iX < KSkinFrameWidthMinValue ||
        aOuterRect.iBr.iY - aInnerRect.iBr.iY < KSkinFrameWidthMinValue)
        {
        iInnerRect = TRect(0,0,0,0);
        iOuterRect = TRect(0,0,0,0);
        }    
    }

void CAlfSkinFrameItemImageLoader::ConstructL( const TDesC &aFileName, TInt aBitmapId, TInt aMaskId )
	{
	iBitmapId = aBitmapId;
	iMaskId = aMaskId;
	iFileName = aFileName.AllocL();
	}

CAlfSkinFrameItemImageLoader::~CAlfSkinFrameItemImageLoader()
    {        
    // base class deletes iFileName
    }

void CAlfSkinFrameItemImageLoader::ProvideBitmapL(TInt aId, CFbsBitmap *& aBitmap, CFbsBitmap *& aMaskBitmap) 
    {
    CAknsItemData* itemData = NULL;    				
    itemData = AknsUtils::SkinInstance()->CreateUncachedItemDataL( iId, EAknsITImageTable );

    if (itemData)
        {
    	// Call grandparent class functionality instead of parent here.
    	CAlfImageLoader::ProvideBitmapL (aId, aBitmap, aMaskBitmap);
        
        CAknsImageTableItemData* itemTableData = static_cast<CAknsImageTableItemData*>(itemData);        
        CleanupStack::PushL( itemTableData );
        
        TSize frameSize = PreferredSize(aId);

        // Create target bitmap
        aBitmap = new CFbsBitmap();
        aBitmap->Create( frameSize, EColor64K );

        aMaskBitmap = new CFbsBitmap();
        aMaskBitmap->Create( frameSize, EGray256 );

        // Create bitmap device for target rendering
        CFbsBitmapDevice* targetDevice = CFbsBitmapDevice::NewL( aBitmap );
        CleanupStack::PushL( targetDevice );

        CFbsBitmapDevice* targetMaskDevice = CFbsBitmapDevice::NewL( aMaskBitmap );
        CleanupStack::PushL( targetMaskDevice );

        // Create bitmap graphics context
        CFbsBitGc* bitgc = CFbsBitGc::NewL();
        CleanupStack::PushL( bitgc );
        bitgc->Activate( targetDevice );

        CFbsBitGc* maskbitgc = CFbsBitGc::NewL();
        CleanupStack::PushL( maskbitgc );
        maskbitgc->Activate( targetMaskDevice );
        
        // Fill mask just in case there are unmasked elements in the frame
        maskbitgc->SetBrushStyle(CGraphicsContext::ESolidBrush);
        maskbitgc->SetBrushColor(KRgbWhite);
        maskbitgc->SetPenColor(KRgbWhite);
        maskbitgc->DrawRect(TRect(TPoint(0,0), frameSize));
        
        // 9 part frame graphics
        for (TInt count = 0; count < 9; count++)
            {
            TSize partsize = GetFramePartSize((TFrameParts)(count), frameSize);
            TPoint partpos = GetFramePartPos((TFrameParts)(count), frameSize);
            
            TAknsItemID partSkinId = itemTableData->ImageIID(count);

            CFbsBitmap* partBitmap = NULL;
            CFbsBitmap* partMaskBitmap = NULL;
            AknsUtils::GetCachedMaskedBitmap(AknsUtils::SkinInstance(), itemTableData->ImageIID(count), partBitmap, partMaskBitmap );
            
            TRect sourcerect = TRect(TPoint(0,0), partsize);    

            if (partBitmap)
                {
                AknIconUtils::SetSize( partBitmap, partsize, EAspectRatioNotPreserved );    
                bitgc->BitBlt(partpos, partBitmap,sourcerect);                
                }
                
            if (partMaskBitmap)
                {
                AknIconUtils::SetSize( partMaskBitmap, partsize, EAspectRatioNotPreserved );    
                maskbitgc->BitBlt(partpos, partMaskBitmap,sourcerect);                
                }
            }
        
        CleanupStack::PopAndDestroy( maskbitgc );
        CleanupStack::PopAndDestroy( bitgc );
        CleanupStack::PopAndDestroy( targetMaskDevice );
        CleanupStack::PopAndDestroy( targetDevice );
        CleanupStack::PopAndDestroy( itemTableData );
        }
    else
        {
        // 1 part frame graphics comes from the base class
        CAlfSkinItemImageLoader::ProvideBitmapL(aId, aBitmap, aMaskBitmap);    
        }    

    }

TSize CAlfSkinFrameItemImageLoader::GetFramePartSize(const TFrameParts aFramePart, const TSize& aFullFrameSize) const
    {
    TRect innerRect = AdjustedInnerRect(aFullFrameSize);
    TRect outerRect = AdjustedOuterRect(aFullFrameSize);    
    
    TSize size(0,0);
    
    switch (aFramePart)
        {
        case EFramePartCornerTl:
            size.iWidth = innerRect.iTl.iX - outerRect.iTl.iX;
            size.iHeight = innerRect.iTl.iY - outerRect.iTl.iY;
            break;
        case EFramePartCornerTr:
            size.iWidth = outerRect.iBr.iX - innerRect.iBr.iX;
            size.iHeight = innerRect.iTl.iY - outerRect.iTl.iY;
            break;
        case EFramePartCornerBl:
            size.iWidth = innerRect.iTl.iX - outerRect.iTl.iX;
            size.iHeight = outerRect.iBr.iY - innerRect.iBr.iY;					
            break;
        case EFramePartCornerBr:
            size.iWidth = outerRect.iBr.iX - innerRect.iBr.iX;
            size.iHeight = outerRect.iBr.iY - innerRect.iBr.iY;					
            break;
        case EFramePartSideT:
            size.iWidth = innerRect.iBr.iX - innerRect.iTl.iX;
            size.iHeight = innerRect.iTl.iY - outerRect.iTl.iY;					
            break;
        case EFramePartSideB:
            size.iWidth = innerRect.iBr.iX - innerRect.iTl.iX;
            size.iHeight = outerRect.iBr.iY - innerRect.iBr.iY;					
            break;
        case EFramePartSideL:
            size.iWidth = innerRect.iTl.iX - outerRect.iTl.iX;
            size.iHeight = innerRect.iBr.iY - innerRect.iTl.iY;					
            break;
        case EFramePartSideR:
            size.iWidth = outerRect.iBr.iX - innerRect.iBr.iX;
            size.iHeight = innerRect.iBr.iY - innerRect.iTl.iY;					
            break;
        case EFramePartCenter:
            size = innerRect.Size();
            break;
        default:
            break;
        }
    return size;
    }

TPoint CAlfSkinFrameItemImageLoader::GetFramePartPos(const TFrameParts aFramePart, const TSize& aFullFrameSize) const
    {
    TRect innerRect = AdjustedInnerRect(aFullFrameSize);
    TRect outerRect = AdjustedOuterRect(aFullFrameSize);    

    TPoint pos(0,0);

    switch (aFramePart)
        {
        case EFramePartCornerTl:
            pos.iX = outerRect.iTl.iX;
            pos.iY = outerRect.iTl.iY;
            break;
        case EFramePartCornerTr:
            pos.iX = innerRect.iBr.iX;
            pos.iY = outerRect.iTl.iY;
            break;
        case EFramePartCornerBl:
            pos.iX = outerRect.iTl.iX;
            pos.iY = innerRect.iBr.iY;
            break;
        case EFramePartCornerBr:
            pos.iX = innerRect.iBr.iX;
            pos.iY = innerRect.iBr.iY;
            break;
        case EFramePartSideT:
            pos.iX = innerRect.iTl.iX;
            pos.iY = outerRect.iTl.iY;
            break;
        case EFramePartSideB:
            pos.iX = innerRect.iTl.iX;
            pos.iY = innerRect.iBr.iY;
            break;
        case EFramePartSideL:
            pos.iX = outerRect.iTl.iX;
            pos.iY = innerRect.iTl.iY;
            break;
        case EFramePartSideR:
            pos.iX = innerRect.iBr.iX;
            pos.iY = innerRect.iTl.iY;
            break;
        case EFramePartCenter:
            pos.iX = innerRect.iTl.iX;
            pos.iY = innerRect.iTl.iY;
            break;
        default:
            break;
        }
    return pos;
    }

TRect CAlfSkinFrameItemImageLoader::AdjustedOuterRect(const TSize& aFullFrameSize) const
    {
    if (aFullFrameSize != iOuterRect.Size())
        {
        return TRect(TPoint(0,0), aFullFrameSize);    
        }
    return iOuterRect;    
    }

TRect CAlfSkinFrameItemImageLoader::AdjustedInnerRect(const TSize& aFullFrameSize) const
    {
    TRect adjustedOuterRect = AdjustedOuterRect(aFullFrameSize);
    if (adjustedOuterRect != iOuterRect)
        {
        TInt adjustment = 0;
        
        if ((adjustedOuterRect.Width() >= KSkinFrameBorderWidthDefaultValue * 2) &&
            (adjustedOuterRect.Height() >= KSkinFrameBorderWidthDefaultValue * 2))
            {
            // Use default border width
            adjustment = KSkinFrameBorderWidthDefaultValue;
            }
        else if ((adjustedOuterRect.Width() >= KSkinFrameBorderWidthSmallValue * 2) &&
                 (adjustedOuterRect.Height() >= KSkinFrameBorderWidthSmallValue * 2))
            {
            // Frame size is too small, use small border
            adjustment = KSkinFrameBorderWidthSmallValue;
            }        
        else if ((adjustedOuterRect.Width() >= KSkinFrameBorderWidthSmallestValue * 2) &&
                 (adjustedOuterRect.Height() >= KSkinFrameBorderWidthSmallestValue * 2))
            {
            // Frame size is too small, use smallest border
            adjustment = KSkinFrameBorderWidthSmallestValue;
            }        
        else
            {
            // Frame size is too small, just draw the center part
            adjustment = 0;    
            }
                                            
        adjustedOuterRect.iTl.iX += adjustment;
        adjustedOuterRect.iBr.iX -= adjustment;            
        adjustedOuterRect.iTl.iY += adjustment;
        adjustedOuterRect.iBr.iY -= adjustment;
        
        return adjustedOuterRect;    
        }
    
    return iInnerRect;    
    }


CAlfAppIconImageLoader::CAlfAppIconImageLoader(
		TUid aAppUid, 
		TAknsAppIconType aType):
		iAppUid(aAppUid),
		iType(aType)
	{
	}


void CAlfAppIconImageLoader::ProvideBitmapL (
		TInt aId, 
		CFbsBitmap *& aBitmap, 
		CFbsBitmap *& aMaskBitmap)
	{
	CAlfImageLoader::ProvideBitmapL (aId, aBitmap, aMaskBitmap);

    AknsUtils::CreateAppIconLC( 
					GetValidSkinInstance(), 
					iAppUid, iType,
					aBitmap, aMaskBitmap );

    // Leave if bitmap or mask is NULL.
    User::LeaveIfNull( aBitmap );
    //User::LeaveIfNull( aMaskBitmap );

	// diable compression so that the bitmap may be able to be duplicated inside
	// AlfTK when BITGDI renderer is in use.
	AknIconUtils::DisableCompression( aBitmap );

    // Exclude from icon cache to avoid flooding it or to avoid bitmap sharing
    if (iExcludeFromAvkonIconCache)
        {
        AknIconUtils::ExcludeFromCache( aBitmap );    
        }

    // Set icon sizes and render them on the bitmaps.
    // This does the actual rendering to specified size. (SVG -> bitmap)
    AknIconUtils::SetSize( aBitmap, PreferredSize(aId), iScaleMode );
    
	// pop the passed bitmap and mask 
	CleanupStack::Pop(2);
	}

EXPORT_C CAlfAutoSizeImageLoaderUtil::CAlfAutoSizeImageLoaderUtil()
    {
    }

EXPORT_C TBool CAlfAutoSizeImageLoaderUtil::PreferredSizeChanged(const CAlfTexture& aChangedTexture, TSize aPreferredSize)
    {
    TBool ret = EFalse;
    TInt count = iImageLoaderList.Count();
    for (TInt i=0; i<count;i++)
        {
        ret |= iImageLoaderList[i]->PreferredSizeChanged(aChangedTexture, aPreferredSize);    
        }
    return ret;    
    }
