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
* Description:   
*
*/



#ifndef __HUICANVASTEXTURECACHE_H__
#define __HUICANVASTEXTURECACHE_H__


#include <e32base.h>
#include <uiacceltk/HuiCanvasCmdBufferReader.h>
#include <uiacceltk/HuiImage.h>
#include <uiacceltk/HuiTexture.h>
#include <uiacceltk/HuiCanvasVisual.h>
#include <uiacceltk/huilowmemoryobserver.h>

class CHuiTexture;
class CHuiCanvasVisual;
class CHuiCanvasRenderBuffer;
class CHuiCanvasTextImage;
class CHuiCanvasGraphicImage;
class CHuiCanvasRenderBufferImage;
class CHuiCanvasTextImageRasterizer;
class CHuiCanvasGraphicImageRasterizer;
class CHuiCanvasCombinedGraphicImageRasterizer;
class CHuiCanvasTextureCache;

NONSHARABLE_CLASS( THuiCachedGcParams )  
    {
public:
    THuiCachedGcParams() :    
        iPenColor(KHuiWsDefaultPenColor),
        iPenStyle(KHuiWsDefaultPenStyle),
        iDrawMode(KHuiWsDefaultDrawMode),
        iBrushColor(KHuiWsDefaultBrushColor),
        iBrushStyle(KHuiWsDefaultBrushStyle),
        iShadowMode(KHuiWsDefaultShadowMode),
        iStrikethrough(KHuiWsDefaultStrikethrough),
        iUnderline(KHuiWsDefaultUnderline),
        iUserDisplayMode(KHuiWsDefaultUserDisplayMode),
        iShadowColor(KHuiWsDefaultShadowColor)
            {
            }

    TRgb  iPenColor;
    TInt  iPenStyle;
    TInt  iDrawMode;
    TRgb  iBrushColor;
    TInt  iBrushStyle;
    TInt  iShadowMode;
    TInt  iStrikethrough;
    TInt  iUnderline;
    TInt  iUserDisplayMode;
    TRgb  iShadowColor;        
    };


NONSHARABLE_CLASS( THuiCachedImageParams )  
    {
public:
    THuiCachedImageParams() :    
        iBitmapHandle(0),
        iMaskHandle(0),
        iInvertedMask(EFalse),
        iMaskOriginPoint(TPoint(0,0))
            {
            }

    TInt iBitmapHandle;
    TInt iMaskHandle;
    TBool iInvertedMask;
    TPoint iMaskOriginPoint;
    THuiCachedGcParams iGcParams;
    };

NONSHARABLE_CLASS( THuiCachedCombinedImageParams )  
    {
public:
    THuiCachedCombinedImageParams() :    
        iBitmapHandle(0),
        iMaskHandle(0),
        iCombinedBitmapPoint(TPoint(0,0))
            {
            }

    TInt iBitmapHandle;
    TInt iMaskHandle;
    TPoint iCombinedBitmapPoint;
    };

NONSHARABLE_CLASS( THuiCachedTextParams )
    {      
public:
    THuiCachedTextParams() :
        iText(NULL),
        iFontHandle(0),
        iRasterizationOffset(TPoint(0,0)),
        iTextParams(THuiCanvasTextParameters()),
        iTextBoxMaxSize(TSize(KMinTInt,KMinTInt)),
        iTextWidth(KMinTInt),
        iBaseLineOffset(KMinTInt),
        iTextAlign(CGraphicsContext::ELeft),
        iMargin(0),
        iAngle(0)
            {
            }
    
    HBufC* iText;
    TPtr* iFindTextPtr;
    TInt iFontHandle;
    TPoint iRasterizationOffset;
    THuiCanvasTextParameters iTextParams;
    TSize iTextBoxMaxSize;        
    TInt iTextWidth;
    TInt iBaseLineOffset;
    TInt iTextAlign;
    TInt iMargin;
    TInt iAngle;
    THuiCachedGcParams iGcParams;
    };

NONSHARABLE_CLASS( THuiCachedBitmap )  
    {
public:
    THuiCachedBitmap() :    
        iBitmapHandle(0),
        iMaskHandle(0),
        iBitmap(0),
        iMask(0),
        iBitmapTouchCount(-1),
        iMaskTouchCount(-1),
        iInvertedMask(EFalse),
        iMaskOriginPoint(TPoint(0,0)),
        iCombinedBitmapPoint(TPoint(0,0))       
            {
            }

    TInt iBitmapHandle;
    TInt iMaskHandle;
    CFbsBitmap* iBitmap;
    CFbsBitmap* iMask;    
    TInt iBitmapTouchCount;
    TInt iMaskTouchCount;
    TBool iInvertedMask;
    TPoint iMaskOriginPoint;
    TPoint iCombinedBitmapPoint;
    };


NONSHARABLE_CLASS( CHuiCanvasImage ) : public CBase
    {
public:
    
    CHuiCanvasImage();

    ~CHuiCanvasImage();

    void RefreshUser(const CHuiCanvasVisual& aUser);

    void RemoveUser(const CHuiCanvasVisual& aUser);

    TBool IsUser(const CHuiCanvasVisual& aUser) const;

    TBool IsAnyUser() const;
        
    CHuiTexture* Texture() const;
        
    virtual void CopyAttributes(CHuiCanvasImage& aSrc);

    virtual void Reset();

public:

    CHuiTexture* iTexture;
    THuiCachedGcParams iGcParams;
    RPointerArray<CHuiCanvasVisual> iActiveUsers;
    TTime iLastUsed;
    CHuiCanvasTextureCache* iCache;
    };


NONSHARABLE_CLASS( CHuiCanvasGraphicImage ) : public CHuiCanvasImage
    {
public:
    
    CHuiCanvasGraphicImage();

    ~CHuiCanvasGraphicImage();

    virtual void CopyAttributes(CHuiCanvasGraphicImage& aSrc);

    virtual void Reset();

public:

    TInt iBitmapHandle;
    TInt iMaskHandle;
    CFbsBitmap* iBitmap;
    CFbsBitmap* iMask;    
    TBool iInvertedMask;
    TPoint iMaskOriginPoint;
    TInt iBitmapTouchCount;        
    TInt iMaskTouchCount;
    
    // Combined image
    RArray<THuiCachedBitmap> iSubImages;
    TSize iImageSize; 
    
	// Used for identifying recycled bitmap handles
    TInt64 iSerialNumber;
    };


NONSHARABLE_CLASS( CHuiCanvasTextImage ) : public CHuiCanvasImage
    {
public:
    
    CHuiCanvasTextImage();

    ~CHuiCanvasTextImage();

    TBool UseColorModulation() const;
    
    virtual void CopyAttributes(CHuiCanvasTextImage& aSrc);

    virtual void Reset();

public:

    HBufC* iText;
    TPtr* iFindTextPtr;
    TInt iFontHandle;
    TPoint iRasterizationOffset;
    THuiCanvasTextParameters iTextParams;
    TSize iTextBoxMaxSize;        
    TInt iTextWidth;
    TInt iBaseLineOffset;
    TInt iTextAlign;
    TInt iMargin;
    TInt iAngle;
    TBool iUseColorModulation;
    // not owned
    CFbsBitGcFont* iFont;
    };


class CHuiCanvasTextureCache : public CBase, public MHuiLowMemoryObserver, public MHuiMemoryLevelObserver
    {
public:

	/**
	 * Constructor.
	 */
	CHuiCanvasTextureCache();


	/**
	 * Destructor.
	 */ 
	~CHuiCanvasTextureCache();

	/**
	 * Constructor.
	 */
	static CHuiCanvasTextureCache* NewL();

	/**
	 * Constructor.
	 */
    static CHuiCanvasTextureCache* NewLC();
    
    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Enables (disables) low memory state.
     * Deprecated
     */
    void EnableLowMemoryState(TBool aEnable);

    /**
     * Sets memory level. Alternative for EnableLowMemoryState().
     */
    void SetMemoryLevel(THuiMemoryLevel aLevel);

    /**
     * Gets memory level.
     */
    THuiMemoryLevel MemoryLevel();
    
	/**
	 * Finds cached combined image and updates it if needed. If not found creates new one into cache.
	 */ 
    const CHuiCanvasGraphicImage* CreateCombinedCachedImageL(const RArray<THuiCachedCombinedImageParams>& aCachedImageParams, 
        const THuiCachedGcParams& aGcParams,
        const TSize& aImageSize, 
        const CHuiCanvasVisual& aUser);

	/**
	 * Finds cached image and updates it if needed. If not found creates new one into cache.
	 */ 
    const CHuiCanvasGraphicImage* CreateCachedImageL(const THuiCachedImageParams& aCachedImageParams, const CHuiCanvasVisual& aUser);

	/**
	 * Finds cached image, but does not update it. If not found, leave happens.
	 */ 
    const CHuiCanvasGraphicImage* FindCachedImageL(const THuiCachedImageParams& aCachedImageParams, const CHuiCanvasVisual& aUser);

	/**
	 * Finds cached text and updates it if needed. If not found creates new one into cache.
	 */ 
    const CHuiCanvasTextImage* CreateCachedTextL(const THuiCachedTextParams& aCachedTextParams, const CHuiCanvasVisual& aUser);
    
    /**
     * Finds cached render buffer, if not found it is created. Ownership is not transferred to the caller.  
     * 
     * If current renderer does not support canvas render buffers, NULL is returned. 
     * In unexpected error situations leave happens. 
     */ 
    CHuiCanvasRenderBuffer* CreateCachedRenderBufferL(const CHuiCanvasVisual& aUser, TSize aSizeHint = TSize(0,0));

    /**
     * Finds cached render buffer. Ownership is not transferred to the caller.  
     */ 
    CHuiCanvasRenderBuffer* FindCachedRenderBuffer(const CHuiCanvasVisual& aUser);

    /**
     * Releases render buffer of the given user. Render buffer is not
     * neccessarely deleted immediately, thus it may continue to be available in the cache.
     */ 
    void ReleaseCachedRenderBuffer(const CHuiCanvasVisual& aUser);
    
	/**
	 * This method releases all images and texts that are used by the given user. Images are not
	 * neccessarely deleted immediately,thus they may continue to be available in the cache.
	 */ 
    void ReleaseAllCachedEntries(const CHuiCanvasVisual& aUser);

	/**
	 * Called by the env when frame has been completed. 
	 */ 
    void AdvanceTime();

	/**
	 * Called when texture memory has run out. This method tries to release more texture memory.
	 */ 
    void HandleOutOfTextureMemory();

	/**
	 * Utility to create new (or reuse recylced) texture.
	 */ 
    CHuiTexture* CreateRecycledTextureL();

	/**
	 * Deletes (or moves to recycling) given texture.
	 */ 
    void DeleteRecycledTexture(CHuiTexture* aTexture);

    void EnableTouchCountCheck(TBool aEnable = ETrue);
    TBool IsTouchCountCheckEnabled();

private:
    
    void DeleteAllReleasedEntries(TBool aAllowKeepCached);        
    
    void FindUnusedImageEntries(RPointerArray<CHuiCanvasGraphicImage>& aIndexEntries);    
    void FindUnusedTextEntries(RPointerArray<CHuiCanvasTextImage>& aIndexEntries);
    void FindUnusedRenderBufferEntries(RPointerArray<CHuiCanvasRenderBufferImage>& aIndexEntries);
    
    void SelectPreservedUnusedImageEntries(RPointerArray<CHuiCanvasGraphicImage>& aIndexEntries);    
    void SelectPreservedUnusedTextEntries(RPointerArray<CHuiCanvasTextImage>& aIndexEntries);
    void SelectPreservedUnusedRenderBufferEntries(RPointerArray<CHuiCanvasRenderBufferImage>& aIndexEntries);
    
    void DeleteUnusedImageEntries(RPointerArray<CHuiCanvasGraphicImage>& aIndexEntries);
    void DeleteUnusedTextEntries(RPointerArray<CHuiCanvasTextImage>& aIndexEntries);
    void DeleteUnusedRenderBufferEntries(RPointerArray<CHuiCanvasRenderBufferImage>& aIndexEntries);
    
    void CalculateGraphicsMemoryUsage();    

    TInt CalculateUnusedCanvasTextureUsageInKbytes();
    TInt CalculateUnusedCanvasRenderBufferUsageInKbytes();

    TInt CalculateTotalCanvasRenderBufferUsageInKbytes();
    
    CHuiCanvasRenderBufferImage* ReUseReleasedRenderBufferL(TSize aSizehint);
    
    TBool IsLowMemoryStateEnabled() const;
    
private:

    /** Temp entry for doing quick search op */
    CHuiCanvasTextImage* iSearchedTextImageEntry;

    /** Temp entry for doing quick search op */
    CHuiCanvasGraphicImage* iSearchedGraphicImageEntry;

    /** Temp entry for doing quick search op */
    CHuiCanvasRenderBufferImage* iSearchedRenderBufferImageEntry;
    
    /** Cached images */
    RPointerArray<CHuiCanvasGraphicImage> iCachedImages;

    /** Cached texts */
    RPointerArray<CHuiCanvasTextImage>  iCachedTexts;

    /** Cached render buffers */
    RPointerArray<CHuiCanvasRenderBufferImage>  iCachedRenderBuffers;
    
    /** Flag to tell that there are unused images which should be possibly deleted */
    TBool iHasReleasedImages;

    /** Flag to tell that there are unused texts which should be possibly deleted */
    TBool iHasReleasedTexts;

    /** Flag to tell that there are unused render buffers which should be possibly deleted */
    TBool iHasReleasedRenderBuffers;
    
    /** Amount of texture memory reserved for unused texts */
    TInt iUnusedCanvasTextTextureCacheSizeInKBytes;
    
    /** Amount of texture memory reserved for unused images */
    TInt iUnusedCanvasImageTextureCacheSizeInKBytes;

    /** Amount of texture memory reserved for unused render buffers */
    TInt iUnusedCanvasRenderBufferCacheSizeInKBytes;
    
    /** Total amount of all hui textures are attempted (but not promised) to keep below this limit */
    TInt iMaxTextureMemoryInKBytes;
    
    /** Total amount of all render buffers are attempted (but not promised) to keep below this limit */
    TInt iMaxRenderBufferMemoryInKBytes;

    /** Texture recycling array */
    RPointerArray<CHuiTexture> iRecycledTextures;

    /** Rasterizer for texts */
    CHuiCanvasTextImageRasterizer* iTextImageRasterizer;

    /** Rasterizer for outline texts */
    CHuiCanvasTextImageRasterizer* iOutLineTextImageRasterizer;

    /** Rasterizer for image */
    CHuiCanvasGraphicImageRasterizer* iGraphicImageRasterizer;

    /** Rasterizer for combined image */
    CHuiCanvasCombinedGraphicImageRasterizer* iCombinedGraphicImageRasterizer;

    /** Canvas gc instance for internal use */
    CHuiCanvasGc* iCanvasGc;
    
    /** This controls the amount of cached textures, buffers, etc. */
    THuiMemoryLevel iMemoryLevel;
    
    /** RnD utils */
    TInt iEglProfiling;
	
	TBool iTouchCountCheckEnabled;
    };

#endif        
    
