/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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



#include "huicanvastexturecache.h"
#include "uiacceltk/HuiImage.h"
#include "uiacceltk/HuiTexture.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiStatic.h"
#include "huicanvasgc.h"
#include "HuiRenderPlugin.h"
#include "huicanvasrenderbuffer.h"

#include <e32cmn.h>
#include <bitdev.h>
#include <AknLayoutFont.h>
#include <e32property.h>

#include "alfmoduletestconf.h"
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
    // Provides TLS object data for test cases.
    // This is used only if module test hooks are set on.
    #include "huistatictlsdata.h"
#endif // USE_MODULE_TEST_HOOKS_FOR_ALF
// Provides module test hook defines.
#include "alfmoduletestdefines.h"

// Enable this for debugging cache usage
//#define HUI_DEBUG_PRINT_CANVAS_TEXTURE_CACHE
//#define HUI_DEBUG_PRINT_CANVAS_TEXTURE_CACHE_EGL
//#define HUI_DEBUG_PRINT_CANVAS_TEXTURE_CACHE_EXTENDED

// Enable this for debugging performance
//#define HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL

/** Magic S60 font constant */
const TInt KHuiFontVerticalShiftInPixels = 1; 

/** Initial touch count value */
const TInt KHuiInitialBitmapTouchCount = -1; 

/** Estimated bits per pixel for all textures */
const TReal32 KHuiTextureEstimatedBpp = 32;

/** Estimated bits per pixel for rasterized canvas texts */
const TReal32 KHuiCanvasTextEstimatedBpp = 8;

/** Estimated bits per pixel for canvas images */
const TReal32 KHuiCanvasImageEstimatedBpp = 32;

/** Estimated bits per pixel for canvas render buffers */
const TReal32 KHuiCanvasRenderBufferEstimatedBpp = 32;

/** Constant to define target how much memory UI textures should use, 
    this is not a hard limit but effects how long unused textures are cached */
const TInt KHuiMaxRecommendedTextureAmountInKBytes = 6114;

/** Constant to define target how much memory UI render buffers should use, 
    this is not a hard limit but effects how long unused render buffers are cached */
const TInt KHuiMaxRecommendedRenderBufferAmountInKBytes = 0;

/** Ratio how cache is divided between canvas texts and images */
const TReal32 KHuiCanvasUnusedTextImageCacheRatio = 0.1f;

/** Max amount of recycled texture objects */
const TInt KHuiMaxRecycledTextureCount = 100;

/** Use color modulation or not for setting text color */
const TBool KHuiCanvasTextUseColorModulation = ETrue;

/** Store bitmap pointers to avoid handle duplication */
const TBool KHuiCanvasKeepBitmapPointer = ETrue;

CHuiCanvasImage::CHuiCanvasImage() :
        iTexture(NULL),
        iCache(NULL)
    {
    }

CHuiCanvasImage::~CHuiCanvasImage()
    {
    if (iCache && iTexture)
        {
        iCache->DeleteRecycledTexture(iTexture);
        }
    else
        {
        delete iTexture;        
        }    
    
    iActiveUsers.Close();            
    }

void CHuiCanvasImage::RefreshUser(const CHuiCanvasVisual& aUser)
    {
    if (iActiveUsers.FindInAddressOrder(&aUser) == KErrNotFound)
        {
        iActiveUsers.InsertInAddressOrder(&aUser);
        }

    // Update last usage time    
    iLastUsed.UniversalTime();        
    }

void CHuiCanvasImage::RemoveUser(const CHuiCanvasVisual& aUser)
    {
    TInt index = iActiveUsers.FindInAddressOrder(&aUser);

    if (index != KErrNotFound)
        {
        iActiveUsers.Remove(index);
        }    
    }

TBool CHuiCanvasImage::IsUser(const CHuiCanvasVisual& aUser) const
    {
    TInt index = iActiveUsers.FindInAddressOrder(&aUser);

    if (index != KErrNotFound)
        {
        return ETrue;
        }
    return EFalse;
    }

TBool CHuiCanvasImage::IsAnyUser() const
    {
    return iActiveUsers.Count() != 0;
    }

CHuiTexture* CHuiCanvasImage::Texture() const
    {
    return iTexture;
    }

void CHuiCanvasImage::CopyAttributes(CHuiCanvasImage& aSrc)
    {
    iTexture = aSrc.iTexture;
    iGcParams = aSrc.iGcParams;
    iLastUsed = aSrc.iLastUsed;
    iCache = aSrc.iCache;
    
    iActiveUsers.Reset();        
    for (TInt i=0; i < aSrc.iActiveUsers.Count(); i++)
        {
        iActiveUsers.Append(aSrc.iActiveUsers[i]);    
        }               
    }
    
void CHuiCanvasImage::Reset()
    {
    // Reset does not delete owned items !
    iTexture = NULL;            
    iGcParams = THuiCachedGcParams();       
    iActiveUsers.Reset();
    }


CHuiCanvasGraphicImage::CHuiCanvasGraphicImage() :
    iBitmapHandle(0),
    iMaskHandle(0),
    iBitmap(NULL),
    iMask(NULL),
    iInvertedMask(EFalse),
    iMaskOriginPoint(TPoint(0,0)),
    iBitmapTouchCount(-1),
    iMaskTouchCount(-1),
    iImageSize(TSize(0,0))        
        {
        }

void CHuiCanvasGraphicImage::CopyAttributes(CHuiCanvasGraphicImage& aSrc)
    {
    CHuiCanvasImage::CopyAttributes(aSrc);
    iBitmap = aSrc.iBitmap;
    iMask = aSrc.iMask;        
    iBitmapHandle = aSrc.iBitmapHandle;
    iMaskHandle = aSrc.iMaskHandle;
    iInvertedMask = aSrc.iInvertedMask;
    iMaskOriginPoint = aSrc.iMaskOriginPoint;
    iTexture = aSrc.iTexture;
    iBitmapTouchCount = aSrc.iBitmapTouchCount;
    iMaskTouchCount = aSrc.iMaskTouchCount;
    iImageSize = aSrc.iImageSize;        

    iSubImages.Reset();
    for (TInt i=0; i<aSrc.iSubImages.Count();i++)
        {
        iSubImages.Append(aSrc.iSubImages[i]);    
        }               
    }

void CHuiCanvasGraphicImage::Reset()
    {
    // Reset does not delete owned items !        
    CHuiCanvasImage::Reset();                
    iBitmap = NULL;
    iMask = NULL;        
    iSubImages.Reset();        
    iBitmapHandle = 0;
    iMaskHandle = 0;
    iInvertedMask = EFalse;
    iMaskOriginPoint = TPoint(0,0);
    iTexture = NULL;
    iBitmapTouchCount = -1;
    iMaskTouchCount = -1;
    iImageSize = TSize(0,0);        
    };

        

CHuiCanvasGraphicImage::~CHuiCanvasGraphicImage()
    {
    delete iBitmap;
    delete iMask;
    for (TInt i=0;i<iSubImages.Count();i++)
        {
        delete iSubImages[i].iBitmap;    
        delete iSubImages[i].iMask;    
        }
    iSubImages.Close();
    }




CHuiCanvasTextImage::CHuiCanvasTextImage() :
    iText(NULL),
    iFindTextPtr(NULL),
    iFontHandle(0),
    iRasterizationOffset(TPoint(0,0)),
    iTextBoxMaxSize(TSize(KMinTInt,KMinTInt)),
    iTextWidth(KMinTInt),
    iBaseLineOffset(KMinTInt),
    iTextAlign(CGraphicsContext::ELeft),
    iMargin(0),
    iAngle(0),
    iUseColorModulation(EFalse),
    iFont(NULL)                
        {
        }
        
void CHuiCanvasTextImage::CopyAttributes(CHuiCanvasTextImage& aSrc)
    {
    CHuiCanvasImage::CopyAttributes(aSrc);
    iText = aSrc.iText;
    iFindTextPtr = aSrc.iFindTextPtr;
    iFontHandle = aSrc.iFontHandle;
    iRasterizationOffset = aSrc.iRasterizationOffset;
    iTextBoxMaxSize = aSrc.iTextBoxMaxSize;
    iTextWidth = aSrc.iTextWidth;
    iBaseLineOffset = aSrc.iBaseLineOffset;
    iTextAlign = aSrc.iTextAlign;
    iMargin = aSrc.iMargin;
    iAngle = aSrc.iAngle;
    iUseColorModulation = aSrc.iUseColorModulation;            
    iTextParams = aSrc.iTextParams;
    iFont = aSrc.iFont;
    }

void CHuiCanvasTextImage::Reset()
    {
    // Reset does not delete owned items !
    CHuiCanvasImage::Reset();        
    iText = NULL;
    iFindTextPtr = NULL;
    iFontHandle = 0,
    iRasterizationOffset = TPoint(0,0);
    iTextBoxMaxSize = TSize(KMinTInt,KMinTInt);
    iTextWidth = KMinTInt;
    iBaseLineOffset = KMinTInt;
    iTextAlign = CGraphicsContext::ELeft;
    iMargin = 0;
    iAngle = 0;
    iUseColorModulation = EFalse;            
    iFont = NULL;
    }

CHuiCanvasTextImage::~CHuiCanvasTextImage()
    {
    delete iText;
    iText = NULL;
    }

TBool CHuiCanvasTextImage::UseColorModulation() const
    {
    return iUseColorModulation; 
    }

NONSHARABLE_CLASS( CHuiCanvasRenderBufferImage ) : public CHuiCanvasImage
    {
public:
    
    CHuiCanvasRenderBufferImage();

    ~CHuiCanvasRenderBufferImage();
            
    virtual void CopyAttributes(CHuiCanvasRenderBufferImage& aSrc);

    virtual void Reset();

public:

    CHuiCanvasRenderBuffer* iCanvasRenderBuffer;
    TAny* iOwner;
    };


CHuiCanvasRenderBufferImage::CHuiCanvasRenderBufferImage() :
    iCanvasRenderBuffer(NULL),
    iOwner(NULL)
        {
        }
        
void CHuiCanvasRenderBufferImage::CopyAttributes(CHuiCanvasRenderBufferImage& aSrc)
    {
    CHuiCanvasImage::CopyAttributes(aSrc);
    iCanvasRenderBuffer= aSrc.iCanvasRenderBuffer;
    iOwner = aSrc.iOwner;
    }

void CHuiCanvasRenderBufferImage::Reset()
    {
    // Reset does not delete owned items !
    CHuiCanvasImage::Reset();        
    iCanvasRenderBuffer = NULL;
    iOwner = NULL;
    }

CHuiCanvasRenderBufferImage::~CHuiCanvasRenderBufferImage()
    {
    delete iCanvasRenderBuffer;
    }


NONSHARABLE_CLASS( CHuiCanvasImageRasterizer ) : public CBase
    {
public:

    CHuiCanvasImageRasterizer();

    ~CHuiCanvasImageRasterizer();

    void ConstructL(TDisplayMode aPreferredMode = ENone);

    TBool IsSeparateTextMaskUsed();

    static CFbsBitmap* CreateMovedMaskL(CFbsBitmap& aOriginalMask, TPoint aNewTopLeftCorner);

    static CFbsBitmap* CreateInvertedMaskL(CFbsBitmap& aOriginalMask);

protected:

    CFbsBitmap* iTempMask;
    CFbsBitmapDevice* iTempMaskDevice;
    CFbsBitGc*  iTempMaskGc;
    
    CFbsBitmap* iTempBitmap;
    CFbsBitmapDevice* iTempBitmapDevice;
    CFbsBitGc*  iTempBitmapGc;    
    };

CHuiCanvasImageRasterizer::CHuiCanvasImageRasterizer()
    {    
    }

CHuiCanvasImageRasterizer::~CHuiCanvasImageRasterizer()
    {
    delete iTempMaskGc;
    delete iTempMaskDevice;
    delete iTempMask;

    delete iTempBitmapGc;
    delete iTempBitmapDevice;
    delete iTempBitmap; 
    }

void CHuiCanvasImageRasterizer::ConstructL(TDisplayMode aPreferredMode)
    {
    iTempMask = NULL;
    iTempMaskDevice = NULL;
    iTempMaskGc = NULL;

    iTempBitmap = NULL;
    iTempBitmapDevice = NULL;
    iTempBitmapGc = NULL;
    
    // Select fastest color mode for uploading rasterized texts, we assume that EColor16MA 
    // should be best for HW accelrated rendereres (requiring least amount of conversions
    // needed before uploading to texture memory) and EColor64K is good for
    // bitgdi because its backbuffer is in that format.
    //
    // NOTE: EHuiRendererGles10 and EHuiRendererGles11 temporarely use EColor64K
    // beacuse it looks as there are some problems with uploading 16MA 
    // mode bitmaps into multisegmented textures.
    //
    TDisplayMode colorMode = EColor16MA;
    if (aPreferredMode == EGray256 && 
        CHuiStatic::Env().Renderer() == EHuiRendererVg10)
        {
        colorMode = EGray256;
        }
    else if (aPreferredMode == EColor64K)
        {
        colorMode = EColor64K;
        }
    else if (CHuiStatic::Env().Renderer() == EHuiRendererBitgdi || 
        CHuiStatic::Env().Renderer() == EHuiRendererGles10 || 
        CHuiStatic::Env().Renderer() == EHuiRendererGles11)
        {
        colorMode = EColor64K;           
        }

    iTempBitmap = new (ELeave) CFbsBitmap;
    iTempBitmap->Create(TSize(4,4), colorMode);       
    iTempBitmapDevice = CFbsBitmapDevice::NewL(iTempBitmap);
    User::LeaveIfError(iTempBitmapDevice->CreateContext(iTempBitmapGc));             
    
    if (colorMode != EGray256)
        {
        iTempMask = new (ELeave) CFbsBitmap;
        iTempMask->Create(TSize(4,4), EGray256);       
        iTempMaskDevice = CFbsBitmapDevice::NewL(iTempMask);
        User::LeaveIfError(iTempMaskDevice->CreateContext(iTempMaskGc));                 
        }
    }

TBool CHuiCanvasImageRasterizer::IsSeparateTextMaskUsed()
    {
    if (iTempBitmap && 
        iTempBitmap->DisplayMode() != EColor16MA && 
        iTempBitmap->DisplayMode() != EColor16MAP &&
        iTempBitmap->DisplayMode() != EGray256)
        {
        return ETrue;    
        }
    else
        {
        return EFalse;    
        }            
    }

CFbsBitmap* CHuiCanvasImageRasterizer::CreateInvertedMaskL(CFbsBitmap& aOriginalMask)
    {
    TSize size = aOriginalMask.SizeInPixels();
    
    CFbsBitmap* newMask = new(ELeave)CFbsBitmap;                       
    CleanupStack::PushL(newMask);
    
    CFbsBitmap* bitmap = new(ELeave)CFbsBitmap;                       
    CleanupStack::PushL(bitmap);
        
    newMask->Create(size, aOriginalMask.DisplayMode());       
    bitmap->Create(size, aOriginalMask.DisplayMode());       
    
    CFbsDevice* newMaskdevice = NULL;
    CFbsBitGc* newMaskGc = NULL;

    CFbsDevice* bitmapDevice = NULL;
    CFbsBitGc* bitmapGc = NULL;

    newMaskdevice = CFbsBitmapDevice::NewL(newMask);
    CleanupStack::PushL(newMaskdevice);
    User::LeaveIfError(newMaskdevice->CreateContext(newMaskGc));

    bitmapDevice = CFbsBitmapDevice::NewL(bitmap);
    CleanupStack::PushL(bitmapDevice);
    User::LeaveIfError(bitmapDevice->CreateContext(bitmapGc));

    bitmapGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    bitmapGc->SetPenStyle(CGraphicsContext::ESolidPen);
    bitmapGc->SetPenColor(KRgbBlack);
    bitmapGc->SetBrushColor(KRgbBlack);
    bitmapGc->DrawRect(TRect(TPoint(0,0), size));
    
    newMaskGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    newMaskGc->SetPenStyle(CGraphicsContext::ESolidPen);
    newMaskGc->SetPenColor(KRgbWhite);
    newMaskGc->SetBrushColor(KRgbWhite);
    newMaskGc->DrawRect(TRect(TPoint(0,0), size));
    newMaskGc->BitBltMasked(TPoint(0,0), bitmap, TRect(TPoint(0,0), size), &aOriginalMask, EFalse);
    
    delete bitmapGc;
    delete newMaskGc;
    
    CleanupStack::PopAndDestroy(bitmapDevice);
    CleanupStack::PopAndDestroy(newMaskdevice);
    CleanupStack::PopAndDestroy(bitmap);
    CleanupStack::Pop(newMask);
    
    return newMask;
    }

CFbsBitmap* CHuiCanvasImageRasterizer::CreateMovedMaskL(CFbsBitmap& aOriginalMask, TPoint aNewTopLeftCorner)
    {
    TSize size = aOriginalMask.SizeInPixels();
    
    CFbsBitmap* newMask = new(ELeave)CFbsBitmap;                       
    CleanupStack::PushL(newMask);
            
    newMask->Create(size, aOriginalMask.DisplayMode());       
    
    CFbsDevice* newMaskdevice = NULL;
    CFbsBitGc* newMaskGc = NULL;

    newMaskdevice = CFbsBitmapDevice::NewL(newMask);
    CleanupStack::PushL(newMaskdevice);
    User::LeaveIfError(newMaskdevice->CreateContext(newMaskGc));

    newMaskGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    newMaskGc->SetPenStyle(CGraphicsContext::ESolidPen);
    newMaskGc->SetPenColor(KRgbWhite);
    newMaskGc->SetBrushColor(KRgbWhite);
    newMaskGc->DrawRect(TRect(TPoint(0,0), size));
    newMaskGc->BitBlt(TPoint(0,0), &aOriginalMask, TRect(aNewTopLeftCorner, size));
    newMaskGc->BitBlt(aNewTopLeftCorner, &aOriginalMask, TRect(TPoint(0,0), size));
    
    delete newMaskGc;    
    CleanupStack::PopAndDestroy(newMaskdevice);
    CleanupStack::Pop(newMask);
    
    return newMask;
    }
    

NONSHARABLE_CLASS( CHuiCanvasTextImageRasterizer ) : public CHuiCanvasImageRasterizer
    {
public:

    CHuiCanvasTextImageRasterizer();

    ~CHuiCanvasTextImageRasterizer();

    void RasterizeL(CHuiCanvasTextImage& aTextImage);

private:

    TSize CalculateRasterizedTextSize(const TDesC& aText, CFont* aFont) const;

    void InitL(CHuiCanvasTextImage& aTextImage, TSize& aTextsize);

    void DrawL(CHuiCanvasTextImage& aTextImage, TSize& aTextsize, TPoint& aRasterizedOffset);

    void UploadL(CHuiCanvasTextImage& aTextImage, TPoint& aRasterrizedOffset);

private:

    TInt iFontHandle;
    CFbsBitGcFont* iFont;
    };

CHuiCanvasTextImageRasterizer::CHuiCanvasTextImageRasterizer()
    {    
    }

CHuiCanvasTextImageRasterizer::~CHuiCanvasTextImageRasterizer()
    {
    delete iFont;
    }
    
TSize CHuiCanvasTextImageRasterizer::CalculateRasterizedTextSize(const TDesC& aText, CFont* aFont) const
    {
    CFont::TMeasureTextInput input; 
    input.iFlags = CFont::TMeasureTextInput::EFVisualOrder;
    CFont::TMeasureTextOutput output; 
    
    TInt textAdvance = aFont->MeasureText( aText, &input, &output );
	
	TRect bounds = output.iBounds;
	bounds.iTl.iX = Min(bounds.iTl.iX, 0);
	bounds.iBr.iX = Max(bounds.iBr.iX, textAdvance);    
    
    TSize textSize = TSize(bounds.Width(), aFont->FontMaxHeight());
 
    const CAknLayoutFont* layoutFont = 0;
  
    if (CCoeEnv::Static())
        {
        layoutFont = CAknLayoutFont::AsCAknLayoutFontOrNull( aFont );
        }
		
    if ( layoutFont )
        {
        textSize.iHeight = aFont->HeightInPixels();
        TInt textPaneHeight = layoutFont->TextPaneHeight();
        TInt textPaneTopToBaseline = layoutFont->TextPaneTopToBaseline();
        
        textSize.iHeight += textPaneHeight - textPaneTopToBaseline;
        textSize.iHeight += KHuiFontVerticalShiftInPixels;
        }
    else
        {
        textSize.iHeight = Max(textSize.iHeight, aFont->HeightInPixels());
        textSize.iHeight += 3; // the best approximation - fails on big (>=72) fonts
        }
                   
    return textSize;            
    }
    
    
void CHuiCanvasTextImageRasterizer::RasterizeL(CHuiCanvasTextImage& aTextImage)
    {
    TPoint rasterizedOffset(0, 0);
    
    if (aTextImage.iFontHandle && (iFontHandle != aTextImage.iFontHandle))
        {
        delete iFont;
        iFont = NULL;    

        // the font might have been duplicated earlier in order to
        // find out whether it is an outline font or not. As the CHuiCanvasTextImage
        // does not own the font pointer by it's member iFont, just copy the pointer
        // to the instance of this class..        
        if (aTextImage.iFont)
            {
            iFont = aTextImage.iFont;
            }
        else
            {
            // Font is cached as pointer because next text may same font and in that case
            // we are much faster as there is no need to do font duplication again.
            iFont = new (ELeave) CFbsBitGcFont();
            iFontHandle = aTextImage.iFontHandle;
            TInt fontDuplicateErrorCode = iFont->Duplicate(iFontHandle);            
            
            if (fontDuplicateErrorCode)
                {
                // Invalid handle which would panic inside fbsbitgc
                iFontHandle = 0;
                delete iFont;
                iFont = NULL;                    
                aTextImage.iFont = NULL;
                }
            else
                {
                // we need a reference to the font in CHuiCanvasTextImage aswell..
                aTextImage.iFont = iFont;
                }
            }
        }
    
    if (iFont)
        {
        TSize textSize = TSize(0,0);
        TInt textAscent = 0;

        // If no max size is provided, we need to approximate the needed texture size.
        if (aTextImage.iTextBoxMaxSize == TSize(KMinTInt,KMinTInt))
            {                    
            textSize = CalculateRasterizedTextSize(*aTextImage.iText, iFont);            

            // To get most closest values for the font metrics we could try to cast
            // font into aknlayoutfont, but that does not seem to work when font is
            // duplicated trough handle. This solution is not accurate and results slightly 
            // too  large textures but avoids text clipping.
            TReal32 height = textSize.iHeight;
            textSize.iHeight = height * 1.2;
            textAscent = height * 0.9;
                          
            // If rotated 90 degress, then flip width and heigths
            if (aTextImage.iAngle == 90 || aTextImage.iAngle == -90)
                {
                TSize horizontalTextSize = textSize;
                textSize.iWidth = horizontalTextSize.iHeight;
                textSize.iHeight = horizontalTextSize.iWidth;                    
                }
            
            // Since we modify the actual point where drawing is to happen
            // (so that image size gets minimized), we use rasterizedOffset to specify
            // offset we have used. That offset needs to be used when image of the text 
            // is to be drawn because with these coordinates text goes exactly where it ´
            // is supposed to go. 
            if (aTextImage.iAngle == 90)
                {
                // Rotated clockwise
                rasterizedOffset.iX = textSize.iWidth-textAscent;                                            
                }
            else if (aTextImage.iAngle == -90)
                {
                // Rotated anti clockwise
                rasterizedOffset.iX = textAscent;                                                                    
                // Drawing happens upwards
                rasterizedOffset.iY = textSize.iHeight;                                                                    
                }
            else
                {
                // No rotation, just drawn lower
                rasterizedOffset.iY = textAscent;                    
                }                                            
            }
        else
            {
            // Clipping text box is provided, much easier this way.
            textSize = aTextImage.iTextBoxMaxSize;
            }

        // Setup gc parameters
        InitL(aTextImage, textSize);                                
        
        // Draw the text 
        DrawL(aTextImage, textSize, rasterizedOffset);
        }
              
    // Upload content          
    UploadL(aTextImage, rasterizedOffset);    
        
    }
    
void CHuiCanvasTextImageRasterizer::InitL(CHuiCanvasTextImage& aTextImage, TSize& aTextsize)
    {
    TFontUnderline underline = (TFontUnderline)aTextImage.iGcParams.iUnderline;
    TFontStrikethrough strikethrough = (TFontStrikethrough)aTextImage.iGcParams.iStrikethrough;
    CGraphicsContext::TBrushStyle brushstyle = (CGraphicsContext::TBrushStyle)aTextImage.iGcParams.iBrushStyle;
    CGraphicsContext::TPenStyle penstyle = (CGraphicsContext::TPenStyle)aTextImage.iGcParams.iPenStyle;
    TInt shadowmode = aTextImage.iGcParams.iShadowMode; // TODO: Is this even needed for texts ?
    TBool outlinefont = iFont->FontSpecInTwips().iFontStyle.IsEffectOn(FontEffect::EOutline);
    if (IsSeparateTextMaskUsed())
        {
        iTempMask->Resize(aTextsize);
        iTempMaskDevice->Resize(aTextsize);

        delete iTempMaskGc;
        iTempMaskGc = NULL;                             
        User::LeaveIfError(iTempMaskDevice->CreateContext(iTempMaskGc));     

        // Mask bitmap, this defines the shape of the text.
    	iTempMaskGc->SetBrushColor(KRgbBlack);
    	iTempMaskGc->Clear();    	
   	    iTempMaskGc->SetPenColor(KRgbWhite);

        // We dont call the UseFont() variant with handle param because it is slower
        iTempMaskGc->UseFont(iFont);

        // Set correct gc settings
        iTempMaskGc->SetPenStyle(penstyle);
        iTempMaskGc->SetBrushStyle(brushstyle);
        iTempMaskGc->SetUnderlineStyle(underline);
        iTempMaskGc->SetStrikethroughStyle(strikethrough);
        iTempMaskGc->SetShadowMode(shadowmode);            
        }

    iTempBitmap->Resize(aTextsize);
    iTempBitmapDevice->Resize(aTextsize);

    delete iTempBitmapGc;
    iTempBitmapGc = NULL;        
    User::LeaveIfError(iTempBitmapDevice->CreateContext(iTempBitmapGc));                 
    
    if (IsSeparateTextMaskUsed())
        {
        // Just fill with correct color, text shape comes from mask
        if (KHuiCanvasTextUseColorModulation)
            {
            iTempBitmapGc->SetPenColor(KRgbWhite);
            }
        else
            {
            iTempBitmapGc->SetPenColor(aTextImage.iGcParams.iPenColor);
            }    

        iTempBitmapGc->SetBrushColor(aTextImage.iGcParams.iBrushColor);                
	    iTempBitmapGc->Clear();  
        }
    else
        {
        // First clear
        TRgb clearColor;
        if (iTempBitmap->DisplayMode() == EColor16MA)
            {
            clearColor = aTextImage.iGcParams.iPenColor;
            iTempBitmapGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
            }
        else
            {
            clearColor = KRgbBlack;
            iTempBitmapGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
            }

        clearColor.SetAlpha(0);
        iTempBitmapGc->SetPenColor(clearColor);
        iTempBitmapGc->SetBrushColor(clearColor);                            	    
	    iTempBitmapGc->Clear();                  

        // Set correct gc settings
        if (!outlinefont && KHuiCanvasTextUseColorModulation)
            {
            iTempBitmapGc->SetPenColor(KRgbWhite);                        
            }
        else
            {
            iTempBitmapGc->SetPenColor(aTextImage.iGcParams.iPenColor);        
            iTempBitmapGc->SetBrushColor(aTextImage.iGcParams.iBrushColor);        
            }
        
        iTempBitmapGc->SetPenStyle(penstyle);
        if (!outlinefont)
            {
            iTempBitmapGc->SetBrushStyle( CGraphicsContext::ENullBrush );
            }
        else
            {
            iTempBitmapGc->SetBrushStyle( brushstyle );
            }
        
        iTempBitmapGc->SetUnderlineStyle(underline);
        iTempBitmapGc->SetStrikethroughStyle(strikethrough);
        iTempBitmapGc->SetShadowMode(shadowmode);            
        }    
            
    // We dont call the UseFont() variant with handle param because it is slower
    iTempBitmapGc->UseFont(iFont);    

    // Are we using color modulation, store info to entry
    aTextImage.iUseColorModulation = outlinefont ? EFalse : KHuiCanvasTextUseColorModulation;

    // Debug, enable this to see texture outlines.
    //iTempMaskGc->DrawRect(TRect(TPoint(0,0), textSize));                
    }

void CHuiCanvasTextImageRasterizer::DrawL(CHuiCanvasTextImage& aTextImage, TSize& aTextsize, TPoint& aRasterizedOffset)
    {
#ifdef SYMBIAN_BUILD_GCE            
    CGraphicsContext::TTextParameters gcTextParams = *(CGraphicsContext::TTextParameters*)& aTextImage.iTextParams;
#endif            
    // Based on supplied params, select the approprite DrawText variant.
    TBool up = (aTextImage.iAngle == -90);
    if (aTextImage.iTextBoxMaxSize != TSize(KMinTInt,KMinTInt))
        {
        if (aTextImage.iBaseLineOffset != KMinTInt)
            {
            CGraphicsContext::TTextAlign align = (CGraphicsContext::TTextAlign) aTextImage.iTextAlign;

            if (aTextImage.iTextWidth != KMinTInt)
            
                {
                if (aTextImage.iAngle == 0)
                    {                                
                    if (IsSeparateTextMaskUsed())
                        {
                        iTempMaskGc->DrawText(*aTextImage.iText, 
#ifdef SYMBIAN_BUILD_GCE            
                        &gcTextParams,
#endif                            
                        TRect(TPoint(0,0), aTextsize), aTextImage.iBaseLineOffset, aTextImage.iTextWidth, align, aTextImage.iMargin);                            
                        }
                    else
                        {
                        // No need to draw separate mask, iTempBitmapGc is using EDrawModeWriteAlpha mode
                        }    
                    
                    iTempBitmapGc->DrawText(*aTextImage.iText, 
#ifdef SYMBIAN_BUILD_GCE            
                    &gcTextParams,
#endif                                                      
                    TRect(TPoint(0,0), aTextsize), aTextImage.iBaseLineOffset, aTextImage.iTextWidth, align, aTextImage.iMargin);
                    }
                else
                    {
                    if (IsSeparateTextMaskUsed())
                        {
                        iTempMaskGc->DrawTextVertical(*aTextImage.iText, 
#ifdef SYMBIAN_BUILD_GCE            
                        &gcTextParams,
#endif                                                      
                        TRect(TPoint(0,0), aTextsize), aTextImage.iBaseLineOffset, aTextImage.iTextWidth, up, align, aTextImage.iMargin);
                        }
                    else
                        {
                        // No need to draw separate mask, iTempBitmapGc is using EDrawModeWriteAlpha mode
                        }    
                    iTempBitmapGc->DrawTextVertical(*aTextImage.iText, 
#ifdef SYMBIAN_BUILD_GCE            
                    &gcTextParams,
#endif                            
                    TRect(TPoint(0,0), aTextsize), aTextImage.iBaseLineOffset, aTextImage.iTextWidth, up, align, aTextImage.iMargin);                                                            
                    }    
                }
            else
                {
                if (aTextImage.iAngle == 0)
                    {                               
                    if (IsSeparateTextMaskUsed())
                        {
                        iTempMaskGc->DrawText(*aTextImage.iText, 
#ifdef SYMBIAN_BUILD_GCE            
                        &gcTextParams,
#endif                            
                        TRect(TPoint(0,0), aTextsize), aTextImage.iBaseLineOffset, align, aTextImage.iMargin);
                        }
                    else
                        {
                        // No need to draw separate mask, iTempBitmapGc is using EDrawModeWriteAlpha mode
                        }    
                    iTempBitmapGc->DrawText(*aTextImage.iText, 
#ifdef SYMBIAN_BUILD_GCE            
                    &gcTextParams,
#endif                                                      
                    TRect(TPoint(0,0), aTextsize), aTextImage.iBaseLineOffset, align, aTextImage.iMargin);                            
                    }
                else
                    {
                    if (IsSeparateTextMaskUsed())
                        {                                
                        iTempMaskGc->DrawTextVertical(*aTextImage.iText, 
#ifdef SYMBIAN_BUILD_GCE            
                        &gcTextParams,
#endif                            
                        TRect(TPoint(0,0), aTextsize), aTextImage.iBaseLineOffset, up, align, aTextImage.iMargin);
                        }
                    else
                        {
                        // No need to draw separate mask, iTempBitmapGc is using EDrawModeWriteAlpha mode
                        }    
                    iTempBitmapGc->DrawTextVertical(*aTextImage.iText, 
#ifdef SYMBIAN_BUILD_GCE            
                    &gcTextParams,
#endif                            
                    TRect(TPoint(0,0), aTextsize), aTextImage.iBaseLineOffset, up, align, aTextImage.iMargin);                                                            
                    }    
                }                           
            }
        else
            {
            if (aTextImage.iAngle == 0)
                {                               
                if (IsSeparateTextMaskUsed())
                    {                                
                    iTempMaskGc->DrawText(*aTextImage.iText, 
#ifdef SYMBIAN_BUILD_GCE            
                    &gcTextParams,
#endif                            
                    TRect(TPoint(0,0), aTextsize) );
                    }
                else
                    {
                    // No need to draw separate mask, iTempBitmapGc is using EDrawModeWriteAlpha mode
                    }    
                    
                iTempBitmapGc->DrawText(*aTextImage.iText, 
#ifdef SYMBIAN_BUILD_GCE            
                    &gcTextParams,
#endif                            
                TRect(TPoint(0,0), aTextsize));                                                        
                }
            else
                {
                if (IsSeparateTextMaskUsed())
                    {                          
                    iTempMaskGc->DrawTextVertical(*aTextImage.iText, 
#ifdef SYMBIAN_BUILD_GCE            
                    &gcTextParams,
#endif                            
                    TRect(TPoint(0,0), aTextsize), up );
                    }
                else
                    {
                    // No need to draw separate mask, iTempBitmapGc is using EDrawModeWriteAlpha mode
                    }    
                
                iTempBitmapGc->DrawTextVertical(*aTextImage.iText, 
#ifdef SYMBIAN_BUILD_GCE            
                    &gcTextParams,
#endif                            
                TRect(TPoint(0,0), aTextsize), up);                                                                                    
                }    
            }    
        }
    else
        {
        if (aTextImage.iAngle == 0)
            {                                             
            if (IsSeparateTextMaskUsed())
                {                          
                iTempMaskGc->DrawText(*aTextImage.iText, 
#ifdef SYMBIAN_BUILD_GCE            
                &gcTextParams,
#endif                            
                aRasterizedOffset);
                }
            else
                {
                // No need to draw separate mask, iTempBitmapGc is using EDrawModeWriteAlpha mode
                }
                    
            iTempBitmapGc->DrawText(*aTextImage.iText, 
#ifdef SYMBIAN_BUILD_GCE            
                &gcTextParams,
#endif                            
            aRasterizedOffset);                                    
            }
        else
            {
            if (IsSeparateTextMaskUsed())
                {                          
                iTempMaskGc->DrawTextVertical(*aTextImage.iText, 
#ifdef SYMBIAN_BUILD_GCE            
                &gcTextParams,
#endif                            
                aRasterizedOffset, up);
                }
            else
                {
                // No need to draw separate mask, iTempBitmapGc is using EDrawModeWriteAlpha mode
                }
                
            iTempBitmapGc->DrawTextVertical(*aTextImage.iText, 
#ifdef SYMBIAN_BUILD_GCE            
                &gcTextParams,
#endif                                            
            aRasterizedOffset, up);                                                            
            }    
        }        
    }

void CHuiCanvasTextImageRasterizer::UploadL(CHuiCanvasTextImage& aTextImage, TPoint& aRasterizedOffset)
    {
    // Upload bitmap content...this may be slow depending on used HW acceleration !
    
    TSize colorBitmapSize = iTempBitmap->SizeInPixels();
    TSize maskBitmapSize = IsSeparateTextMaskUsed() ? iTempMask->SizeInPixels() : colorBitmapSize;
    CFbsBitmap* textColorBitmap = iTempBitmap;
    CFbsBitmap* textMaskBitmap = IsSeparateTextMaskUsed() ? iTempMask : NULL;
    
           
    if (maskBitmapSize.iWidth > 0 && 
        maskBitmapSize.iHeight > 0 &&
        colorBitmapSize.iWidth > 0 && 
        colorBitmapSize.iHeight > 0)
        {
        // TODO: For BitGdi it might be good to use direct upload, but it would require changes such as 
        // making new textColorBitmap, textMaskBitmap everytime.
        THuiTextureUploadFlags uploadFlags = THuiTextureUploadFlags(EHuiTextureUploadFlagDoNotRetainResolution | EHuiTextureUploadFlagRetainColorDepth);

#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        TTime startTime;
        startTime.UniversalTime();
#endif

        aTextImage.iRasterizationOffset = aRasterizedOffset;
        
        CHuiTexture* texture = aTextImage.iTexture;
        texture->UploadL(*textColorBitmap, textMaskBitmap, uploadFlags);                

#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        TTime endTime;
        endTime.UniversalTime();
        TInt uploadTimeInMilliSeconds = endTime.MicroSecondsFrom( startTime ).Int64()/1000;
        RDebug::Print(_L(">>> CHuiCanvasTextImageRasterizer::UpdateCachedTextL: Upload of %ix%i %S took %i ms"), 
            texture->Size().iWidth, 
            texture->Size().iHeight, 
            aTextImage.iText,            
            uploadTimeInMilliSeconds);
#endif    
        }        
    }
    

NONSHARABLE_CLASS( CHuiCanvasGraphicImageRasterizer ) : public CHuiCanvasImageRasterizer
    {
public:

    CHuiCanvasGraphicImageRasterizer();

    ~CHuiCanvasGraphicImageRasterizer();

    void ConstructL(TBool aUseGray256 = EFalse);

    void RasterizeL(CHuiCanvasGraphicImage& aImage);

private:
    };

CHuiCanvasGraphicImageRasterizer::CHuiCanvasGraphicImageRasterizer()
    {    
    }

CHuiCanvasGraphicImageRasterizer::~CHuiCanvasGraphicImageRasterizer()
    {
    }

void CHuiCanvasGraphicImageRasterizer::ConstructL(TBool /*aUseGray256*/)
    {        
    }

void CHuiCanvasGraphicImageRasterizer::RasterizeL(CHuiCanvasGraphicImage& aImage)
    {
    CFbsBitmap* bitmap = NULL;                       
    CFbsBitmap* mask = NULL;                       

    TInt bitmapTouchCount = KHuiInitialBitmapTouchCount;
    TInt maskTouchCount = KHuiInitialBitmapTouchCount;

#ifdef SYMBIAN_BUILD_GCE            
    TBool volatileBitmapOrMask = EFalse;
#else
    TBool volatileBitmapOrMask = ETrue;
#endif


    TInt bitmapHandle = aImage.iBitmapHandle;
    TInt maskHandle = aImage.iMaskHandle;
    TBool invertMask = aImage.iInvertedMask;
    TPoint maskOriginPoint = aImage.iMaskOriginPoint;

    if (!bitmapHandle)
        {
        return;    
        }
    
    // Lets see if we have already cached the pointer...
    if (aImage.iBitmap)
        {
        // ...yes lets use it
        bitmap = aImage.iBitmap;    
        }
    else
        {
        // ...no, so we need to duplicate the handle
        bitmap = new(ELeave)CFbsBitmap;                       
        bitmap->Duplicate(bitmapHandle);
        if (bitmap)
            {
            if (KHuiCanvasKeepBitmapPointer)
                {
                aImage.iBitmap = bitmap;                    
                }
            else
                {
                CleanupStack::PushL(bitmap);                
                }                
            }           
            
        }

#ifdef SYMBIAN_BUILD_GCE            
        if (bitmap)
            {
            bitmapTouchCount = bitmap->TouchCount();        
            volatileBitmapOrMask |= bitmap->IsVolatile();
            }
#endif            

    // Mask is optional
    if (maskHandle)
        {
        // Lets see if we have already cached the pointer...
        if (aImage.iMask)
            {
            // ...yes lets use it
            mask = aImage.iMask;    
            }
        else
            {
            // ...no, so we need to duplicate the handle
            mask = new(ELeave)CFbsBitmap;                       
            mask->Duplicate(maskHandle);
            if (mask)
                {
                if (KHuiCanvasKeepBitmapPointer)
                    {
                    aImage.iMask = mask;    
                    }
                else
                    {
                    CleanupStack::PushL(mask);                                    
                    }                
                }                                
            }

#ifdef SYMBIAN_BUILD_GCE            
        if (mask)
            {
            maskTouchCount = mask->TouchCount();    
            volatileBitmapOrMask |= mask->IsVolatile();
            }
#endif
        
        // If mask is defined not to be inverted, 
        // we must invert it because toolkit assumes they are always inverted. Huh.
#ifdef __NVG
        // Inverting masks is done ONLY for non-extended bitmaps!
        if (bitmap->ExtendedBitmapType() == KNullUid) // => Bitmap isn't extended
            {
#endif
            if (invertMask && mask->DisplayMode() == EGray2)
                {
                CFbsBitmap* invertedMask = CreateInvertedMaskL(*mask);
                
                // If we are not using stored pointer, delete it
                if (mask != aImage.iMask)
                    {
                    CleanupStack::PopAndDestroy(mask);        
                    }
                
                mask = invertedMask;
                CleanupStack::PushL(mask);    
                }
            
            // Upload expects mask to begin always at TPoint(0,0), so if mask
            // offset is defined we re-create mask with the given new
            // offset. Note that it might be a good idea to combine this op
            // with mask inversion op, but we assume that these are not really used
            // very often, even more rarely together. 
            if (maskOriginPoint != TPoint(0,0))
                {
                CFbsBitmap* movedMask = CreateMovedMaskL(*mask, maskOriginPoint);

                // If we are not using stored pointer, delete it
                if (mask != aImage.iMask)
                    {
                    CleanupStack::PopAndDestroy(mask);
                    }
                mask = movedMask;
                CleanupStack::PushL(mask);                            
                }
#ifdef __NVG
            }
#endif
        }

    // Bitmap has changed, must be updated ?
    TBool touchCountChanged = EFalse;    
    touchCountChanged |= volatileBitmapOrMask;
    touchCountChanged |= (aImage.iBitmapTouchCount != bitmapTouchCount);
    touchCountChanged |= (aImage.iMaskTouchCount != maskTouchCount);
    TBool serialNumberChanged = (aImage.iSerialNumber != bitmap->SerialNumber());
    
    // Is touch count check enabled ?
    TBool touchCountCheckEnabled = CHuiStatic::Env().CanvasTextureCache().IsTouchCountCheckEnabled();
    
    // Has it old content at all ?
    TBool hasContent = aImage.iTexture->HasContent();
    
    if (!hasContent || (touchCountCheckEnabled && touchCountChanged) || serialNumberChanged)
        {
        // Upload bitmap content...this may be slow depending on used HW acceleration !
        if (bitmap->SizeInPixels().iWidth > 0 && 
            bitmap->SizeInPixels().iHeight > 0 &&
            (!mask || (mask->SizeInPixels().iWidth > 0 && mask->SizeInPixels().iHeight > 0)))
            {
            // EHuiTextureUploadFlagDoNotRetainResolution should be removed when 
            // texturecoordinates with segmented textures work.
            // This flag causes ugly scaling to happen with small textures.
            THuiTextureUploadFlags uploadFlags = THuiTextureUploadFlags(EHuiTextureUploadFlagDoNotRetainResolution | EHuiTextureUploadFlagRetainColorDepth);

#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
            TTime startTime;
            startTime.UniversalTime();
#endif

            CHuiTexture* texture = aImage.iTexture;
            texture->UploadL(*bitmap, mask, uploadFlags);

            // Save the bitmap's serial number to the cached item
            aImage.iSerialNumber = bitmap->SerialNumber();

#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
            TTime endTime;
            endTime.UniversalTime();
            TInt uploadTimeInMilliSeconds = endTime.MicroSecondsFrom( startTime ).Int64()/1000;
            
            if (volatileBitmapOrMask)
                {
                RDebug::Print(_L(">>> CHuiCanvasGraphicImageRasterizer::UpdateCachedImageL: Reason for uploading is volatileBitmapOrMask"));                     
                }

            if (aImage.iBitmapTouchCount != bitmapTouchCount)
                {
                RDebug::Print(_L(">>> CHuiCanvasGraphicImageRasterizer::UpdateCachedImageL: Reason for uploading is iBitmapTouchCount %i, old was %i "), bitmapTouchCount, aImage.iBitmapTouchCount);                     
                }

            if (aImage.iMaskTouchCount != maskTouchCount)
                {
                RDebug::Print(_L(">>> CHuiCanvasGraphicImageRasterizer::UpdateCachedImageL: Reason for uploading is iMaskTouchCount %i, old was %i "), maskTouchCount, aImage.iMaskTouchCount);                     
                }
            
            if (serialNumberChanged)
                {
                RDebug::Print(_L(">>> CHuiCanvasGraphicImageRasterizer::UpdateCachedImageL: Reason for uploading is changed bitmap serial number %li, old was %li "), bitmap->SerialNumber(), aImage.iSerialNumber);
                }

            RDebug::Print(_L(">>> CHuiCanvasGraphicImageRasterizer::UpdateCachedImageL: Upload of %ix%i %i+%i took %i ms"), 
                texture->Size().iWidth, 
                texture->Size().iHeight,
                aImage.iBitmapHandle,
                aImage.iMaskHandle,
                uploadTimeInMilliSeconds);
#endif
            }            
        
#ifdef SYMBIAN_BUILD_GCE            
        // Texture upload itself must not modify bitmap or mask touch count.
        // Otherwise, it's impossible to detect client data access
        // (since there is not necessarily any synchronisation between client & alf).
        
        aImage.iBitmapTouchCount = bitmapTouchCount;
        aImage.iMaskTouchCount = maskTouchCount;
#endif                    
        }
    else
        {
        if (touchCountChanged && !touchCountCheckEnabled)
            {
#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
            RDebug::Print(_L(">>> CHuiCanvasGraphicImageRasterizer::UpdateCachedImageL: skipping uploading because touchcount check is disabled"));
#endif
            }        
        }

    // If we are not using stored pointer, delete it
    if (mask && (mask != aImage.iMask))
        {
        CleanupStack::PopAndDestroy(mask);
        mask = NULL;                    
        }

    // If we are not using stored pointer, delete it
    if (bitmap && (bitmap != aImage.iBitmap))
        {
        CleanupStack::PopAndDestroy(bitmap);
        bitmap = NULL;                    
        }                    
    }
    


NONSHARABLE_CLASS( CHuiCanvasCombinedGraphicImageRasterizer ) : public CHuiCanvasImageRasterizer
    {
public:

    CHuiCanvasCombinedGraphicImageRasterizer();

    ~CHuiCanvasCombinedGraphicImageRasterizer();

    void RasterizeL(CHuiCanvasGraphicImage& aImage);

private:

    void InitL(CHuiCanvasGraphicImage& aImage);

    void DrawL(CHuiCanvasGraphicImage& aImage);

    void UploadL(CHuiCanvasGraphicImage& aImage);

    };

CHuiCanvasCombinedGraphicImageRasterizer::CHuiCanvasCombinedGraphicImageRasterizer()
    {    
    }

CHuiCanvasCombinedGraphicImageRasterizer::~CHuiCanvasCombinedGraphicImageRasterizer()
    {
    }

void CHuiCanvasCombinedGraphicImageRasterizer::RasterizeL(CHuiCanvasGraphicImage& aImage)
    {
    // First check the bitmaps
    TBool bitmapsChanged = EFalse;    
    for (TInt i=0; i<aImage.iSubImages.Count(); i++)
        {
        // Lets see if we have already cached the pointer...
        if (!aImage.iSubImages[i].iBitmap)
            {
            // ...no we must duplicate handle
            if (aImage.iSubImages[i].iBitmapHandle)
                {
                aImage.iSubImages[i].iBitmap = new (ELeave) CFbsBitmap;                       
                aImage.iSubImages[i].iBitmap->Duplicate(aImage.iSubImages[i].iBitmapHandle);
                }
            }
                               
        if (!aImage.iSubImages[i].iMask)
            {
            // Lets see if we have already cached the pointer...
            if (aImage.iSubImages[i].iMaskHandle)
                {
                aImage.iSubImages[i].iMask = new (ELeave) CFbsBitmap;                       
                aImage.iSubImages[i].iMask->Duplicate(aImage.iSubImages[i].iMaskHandle);
                }
            }
                
        if (aImage.iSubImages[i].iBitmap)
            {
            bitmapsChanged |= (aImage.iSubImages[i].iBitmapTouchCount != aImage.iSubImages[i].iBitmap->TouchCount());                        
            }
            
        if (aImage.iSubImages[i].iMask)
            {
            bitmapsChanged |= (aImage.iSubImages[i].iMaskTouchCount != aImage.iSubImages[i].iMask->TouchCount());        
            }                
        }
         
   if (bitmapsChanged)
       {        
       InitL(aImage);
       
       DrawL(aImage);
       
       UploadL(aImage);

       // Touch counts updated after upload in case upload changes the count ?
       for (TInt i=0; i<aImage.iSubImages.Count(); i++)
           {
           if (aImage.iSubImages[i].iBitmap)
               aImage.iSubImages[i].iBitmapTouchCount = aImage.iSubImages[i].iBitmap->TouchCount();

           if (aImage.iSubImages[i].iMask)
               aImage.iSubImages[i].iMaskTouchCount = aImage.iSubImages[i].iMask->TouchCount();
           }
       }
   
   if (!KHuiCanvasKeepBitmapPointer)
        {
        for (TInt i=0; i<aImage.iSubImages.Count(); i++)
            {
            delete aImage.iSubImages[i].iBitmap;
            aImage.iSubImages[i].iBitmap = NULL;
            
            delete aImage.iSubImages[i].iMask;
            aImage.iSubImages[i].iMask = NULL;
            }
        }                                       
    }


void CHuiCanvasCombinedGraphicImageRasterizer::InitL(CHuiCanvasGraphicImage& aImage)
    {
    if (IsSeparateTextMaskUsed())
        {
        iTempMask->Resize(aImage.iImageSize);
        iTempMaskDevice->Resize(aImage.iImageSize);

        delete iTempMaskGc;
        iTempMaskGc = NULL;                             
        User::LeaveIfError(iTempMaskDevice->CreateContext(iTempMaskGc));     

        iTempMaskGc->Reset();
    	iTempMaskGc->SetBrushColor(KRgbBlack);
    	iTempMaskGc->Clear();    	
   	    iTempMaskGc->SetPenColor(KRgbWhite);
        }

    iTempBitmap->Resize(aImage.iImageSize);
    iTempBitmapDevice->Resize(aImage.iImageSize);

    delete iTempBitmapGc;
    iTempBitmapGc = NULL;        
    User::LeaveIfError(iTempBitmapDevice->CreateContext(iTempBitmapGc));                 
    
    if (IsSeparateTextMaskUsed())
        {
        TRgb clearColor(KRgbWhite);
        clearColor.SetAlpha(0);
        iTempBitmapGc->SetPenColor(clearColor);
        iTempBitmapGc->SetBrushColor(clearColor);
        iTempBitmapGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);                    	    
	    iTempBitmapGc->Clear();                  
        iTempBitmapGc->SetPenColor(KRgbWhite);                        

        iTempMaskGc->SetPenColor(clearColor);
        iTempMaskGc->SetBrushColor(clearColor);
        iTempMaskGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);                    	    
	    iTempMaskGc->Clear();                  
        iTempMaskGc->SetPenColor(KRgbWhite);                        
        }
    else
        {
        // First clear
        TRgb clearColor(KRgbWhite);
        clearColor.SetAlpha(0);
        iTempBitmapGc->SetPenColor(clearColor);
        iTempBitmapGc->SetBrushColor(clearColor);
        iTempBitmapGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);                    	    
	    iTempBitmapGc->Clear();                  
        iTempBitmapGc->SetPenColor(KRgbWhite);                        
        }    
    }

void CHuiCanvasCombinedGraphicImageRasterizer::DrawL(CHuiCanvasGraphicImage& aImage)
    {
    for (TInt i=0; i<aImage.iSubImages.Count(); i++)
        {        
        CFbsBitmap* bitmap = aImage.iSubImages[i].iBitmap;
        CFbsBitmap* mask = aImage.iSubImages[i].iMask;
        TPoint point = aImage.iSubImages[i].iCombinedBitmapPoint; 

        if (bitmap)
            {
            if (IsSeparateTextMaskUsed())
                {
                TRect sourceRect = TRect(TPoint(0,0), bitmap->SizeInPixels());                
                if (bitmap)
                    {
                    iTempBitmapGc->BitBlt(point, bitmap);                                           
                    }                                                                        
                if (mask)
                    {
                    iTempMaskGc->BitBlt(point, mask);                       
                    }
                }
            else
                {
                TRect sourceRect = TRect(TPoint(0,0), bitmap->SizeInPixels());
                if (mask)
                    {
                    TBool invertMask = (mask->DisplayMode() == EGray256);                       
                    iTempBitmapGc->BitBltMasked(point, bitmap, sourceRect, mask, invertMask);                       
                    }
                else
                    {
                    iTempBitmapGc->BitBlt(point, bitmap);                                           
                    }                                                    
                }    
            }
        }        
    }

void CHuiCanvasCombinedGraphicImageRasterizer::UploadL(CHuiCanvasGraphicImage& aImage)
    {
    CFbsBitmap* mask = IsSeparateTextMaskUsed() ? iTempMask : NULL; 
        
    // Upload bitmap content...this may be slow depending on used HW acceleration !
    if (iTempBitmap->SizeInPixels().iWidth > 0 && 
        iTempBitmap->SizeInPixels().iHeight > 0 &&
        (!mask || (mask->SizeInPixels().iWidth > 0 && mask->SizeInPixels().iHeight)))
        {
        // EHuiTextureUploadFlagDoNotRetainResolution should be removed when 
        // texturecoordinates with segmented textures work.
        // This flag causes ugly scaling to happen with small textures.
        THuiTextureUploadFlags uploadFlags = THuiTextureUploadFlags(EHuiTextureUploadFlagDoNotRetainResolution | EHuiTextureUploadFlagRetainColorDepth);

#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        TTime startTime;
        startTime.UniversalTime();
#endif

        CHuiTexture* texture = aImage.iTexture;                
        texture->UploadL(*iTempBitmap, mask, uploadFlags);

#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        TTime endTime;
        endTime.UniversalTime();
        TInt uploadTimeInMilliSeconds = endTime.MicroSecondsFrom( startTime ).Int64()/1000;
        
        RDebug::Print(_L(">>> CHuiCanvasCombinedGraphicImageRasterizer::UploadCombinedImageL: Upload of %ix%i %i+%i took %i ms"), 
            texture->Size().iWidth, 
            texture->Size().iHeight,
            aImage.iBitmapHandle,
            aImage.iMaskHandle,
            uploadTimeInMilliSeconds);
#endif
        }                    
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt ImageTimeOrderFunc(const CHuiCanvasGraphicImage& aFirst, const CHuiCanvasGraphicImage& aSecond)
    {
    if (aFirst.iLastUsed < aSecond.iLastUsed)
        {
        return -1; // less than
        }
    else if (aFirst.iLastUsed > aSecond.iLastUsed)
        {
        return 1; // more than	
        }
    else
        {
        return 0;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt TextTimeOrderFunc(const CHuiCanvasTextImage& aFirst, const CHuiCanvasTextImage& aSecond)
    {
    if (aFirst.iLastUsed < aSecond.iLastUsed)
        {
        return -1; // less than
        }
    else if (aFirst.iLastUsed > aSecond.iLastUsed)
        {
        return 1; // more than	
        }
    else
        {
        return 0;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt RenderBufferTimeOrderFunc(const CHuiCanvasRenderBufferImage& aFirst, const CHuiCanvasRenderBufferImage& aSecond)
    {
    if (aFirst.iLastUsed < aSecond.iLastUsed)
        {
        return -1; // less than
        }
    else if (aFirst.iLastUsed > aSecond.iLastUsed)
        {
        return 1; // more than  
        }
    else
        {
        return 0;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt GcImageOrderFunc(const THuiCachedGcParams& /*aFirst*/, const THuiCachedGcParams& /*aSecond*/)
    {
    return 0; // equal enough, note that we don't check every field
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt GcTextOrderFunc(const THuiCachedGcParams& aFirst, const THuiCachedGcParams& aSecond)
    {    
    if (!KHuiCanvasTextUseColorModulation)
        {            
        if (aFirst.iPenColor.Value() < aSecond.iPenColor.Value())
            {
            return -1; // less than
            }
        else if (aFirst.iPenColor.Value() > aSecond.iPenColor.Value())
            {
            return 1; // more than	
            }
        else
            {
            // For PC-lint
            }
        }

    if (aFirst.iPenStyle < aSecond.iPenStyle)
        {
        return -1; // less than
        }
    else if (aFirst.iPenStyle > aSecond.iPenStyle)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }

    if (aFirst.iDrawMode < aSecond.iDrawMode)
        {
        return -1; // less than
        }
    else if (aFirst.iDrawMode > aSecond.iDrawMode)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }

    if (aFirst.iStrikethrough < aSecond.iStrikethrough)
        {
        return -1; // less than
        }
    else if (aFirst.iStrikethrough > aSecond.iStrikethrough)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }

    if (aFirst.iUnderline < aSecond.iUnderline)
        {
        return -1; // less than
        }
    else if (aFirst.iUnderline > aSecond.iUnderline)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }

    return 0; // equal enough, note that we don't check every field
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt ImageOrderFunc(const CHuiCanvasGraphicImage& aFirst, const CHuiCanvasGraphicImage& aSecond)
    {
    // Bitmap handle
    if (aFirst.iBitmapHandle < aSecond.iBitmapHandle)
        {
        return -1; // less than
        }
    else if (aFirst.iBitmapHandle > aSecond.iBitmapHandle)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }

    // Mask handle
    if (aFirst.iMaskHandle < aSecond.iMaskHandle)
        {
        return -1; // less than
        }
    else if (aFirst.iMaskHandle > aSecond.iMaskHandle)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }

    // Inverted mask
    if (aFirst.iInvertedMask < aSecond.iInvertedMask)
        {
        return -1; // less than
        }
    else if (aFirst.iInvertedMask > aSecond.iInvertedMask)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }

    // Mask origin point X
    if (aFirst.iMaskOriginPoint.iX < aSecond.iMaskOriginPoint.iX)
        {
        return -1; // less than
        }
    else if (aFirst.iMaskOriginPoint.iX > aSecond.iMaskOriginPoint.iX)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }
        
    // Mask origin point Y
    if (aFirst.iMaskOriginPoint.iY < aSecond.iMaskOriginPoint.iY)
        {
        return -1; // less than
        }
    else if (aFirst.iMaskOriginPoint.iY > aSecond.iMaskOriginPoint.iY)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }

    if (aFirst.iImageSize.iHeight < aSecond.iImageSize.iHeight)
        {
        return -1; // less than
        }
    else if (aFirst.iImageSize.iHeight > aSecond.iImageSize.iHeight)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }

    if (aFirst.iImageSize.iWidth < aSecond.iImageSize.iWidth)
        {
        return -1; // less than
        }
    else if (aFirst.iImageSize.iWidth > aSecond.iImageSize.iWidth)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }
    
    if (aFirst.iSubImages.Count() < aSecond.iSubImages.Count())
        {
        return -1;    
        }
    else if (aFirst.iSubImages.Count() > aSecond.iSubImages.Count())
        {
        return 1;                
        }
    else
        {
        // Same amount, must check array content
        if (aFirst.iSubImages.Count() && aSecond.iSubImages.Count())
            {
            for (TInt i=0; i<aFirst.iSubImages.Count();i++)
                {
                if (aFirst.iSubImages[i].iBitmapHandle < aSecond.iSubImages[i].iBitmapHandle)
                    {
                    return -1;    
                    }
                else if (aFirst.iSubImages[i].iBitmapHandle > aSecond.iSubImages[i].iBitmapHandle)
                    {
                    return 1;                            
                    }
                else
                    {
                    // For PC-lint                                        
                    }                        

                if (aFirst.iSubImages[i].iMaskHandle < aSecond.iSubImages[i].iMaskHandle)
                    {
                    return -1;    
                    }
                else if (aFirst.iSubImages[i].iMaskHandle > aSecond.iSubImages[i].iMaskHandle)
                    {
                    return 1;                            
                    }
                else
                    {
                    // For PC-lint                                        
                    }    
                                        
                if (aFirst.iSubImages[i].iCombinedBitmapPoint.iX < aSecond.iSubImages[i].iCombinedBitmapPoint.iX)
                    {
                    return -1;    
                    }
                else if (aFirst.iSubImages[i].iCombinedBitmapPoint.iX > aSecond.iSubImages[i].iCombinedBitmapPoint.iX)
                    {
                    return 1;                            
                    }
                else
                    {
                    // For PC-lint                                        
                    }                        

                if (aFirst.iSubImages[i].iCombinedBitmapPoint.iY < aSecond.iSubImages[i].iCombinedBitmapPoint.iY)
                    {
                    return -1;    
                    }
                else if (aFirst.iSubImages[i].iCombinedBitmapPoint.iY > aSecond.iSubImages[i].iCombinedBitmapPoint.iY)
                    {
                    return 1;                            
                    }
                else
                    {
                    // For PC-lint                                        
                    }                        


                }                
            }
        else
            {
            // For PC-lint                
            }            
        }                
    return GcImageOrderFunc(aFirst.iGcParams, aSecond.iGcParams); 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt TextOrderFunc(const CHuiCanvasTextImage& aFirst, const CHuiCanvasTextImage& aSecond)
    {
    // Font handle
    if (aFirst.iFontHandle < aSecond.iFontHandle)
        {
        return -1; // less than
        }
    else if (aFirst.iFontHandle > aSecond.iFontHandle)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }
        
    // Text width
    if (aFirst.iTextWidth < aSecond.iTextWidth)
        {
        return -1; // less than
        }
    else if (aFirst.iTextWidth > aSecond.iTextWidth)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }

    // iTextBoxMaxSize.iWidth
    if (aFirst.iTextBoxMaxSize.iWidth < aSecond.iTextBoxMaxSize.iWidth)
        {
        return -1; // less than
        }
    else if (aFirst.iTextBoxMaxSize.iWidth > aSecond.iTextBoxMaxSize.iWidth)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }
    if (aFirst.iTextBoxMaxSize.iHeight < aSecond.iTextBoxMaxSize.iHeight)
           {
           return -1; // less than
           }
       else if (aFirst.iTextBoxMaxSize.iHeight > aSecond.iTextBoxMaxSize.iHeight)
           {
           return 1; // more than  
           }
       else
           {
           // For PC-lint
           }
    // Text content
    TInt textcompare = KErrNotFound;
    if ( aFirst.iFindTextPtr )
        {
        textcompare = aFirst.iFindTextPtr->Compare(*aSecond.iText);     
        }
    else
        {
        textcompare = aFirst.iText->Compare(*aSecond.iText);
        }
    
    if (textcompare < 0)
        {
        return -1;    
        }
    else if (textcompare > 0)
        {
        return 1;    
        }
    else
        {
        // For PC-lint
        }            
    
    // iBaseLineOffset
    if (aFirst.iBaseLineOffset < aSecond.iBaseLineOffset)
        {
        return -1; // less than
        }
    else if (aFirst.iBaseLineOffset > aSecond.iBaseLineOffset)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }

    // iTextAlign
    if (aFirst.iTextAlign < aSecond.iTextAlign)
        {
        return -1; // less than
        }
    else if (aFirst.iTextAlign > aSecond.iTextAlign)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }

    // iMargin
    if (aFirst.iMargin < aSecond.iMargin)
        {
        return -1; // less than
        }
    else if (aFirst.iMargin > aSecond.iMargin)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }

    // iAngle
    if (aFirst.iAngle < aSecond.iAngle)
        {
        return -1; // less than
        }
    else if (aFirst.iAngle > aSecond.iAngle)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }
       
    // Font handle
    if (aFirst.iTextParams.iStart < aSecond.iTextParams.iStart)
        {
        return -1; // less than
        }
    else if (aFirst.iTextParams.iStart > aSecond.iTextParams.iStart)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }

    // Font handle
    if (aFirst.iTextParams.iEnd < aSecond.iTextParams.iEnd)
        {
        return -1; // less than
        }
    else if (aFirst.iTextParams.iEnd > aSecond.iTextParams.iEnd)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }

    // Font handle
    if (aFirst.iTextParams.iFlags < aSecond.iTextParams.iFlags)
        {
        return -1; // less than
        }
    else if (aFirst.iTextParams.iFlags > aSecond.iTextParams.iFlags)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }
                    
    return GcTextOrderFunc(aFirst.iGcParams, aSecond.iGcParams); 
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt RenderBufferOrderFunc(const CHuiCanvasRenderBufferImage& aFirst, const CHuiCanvasRenderBufferImage& aSecond)
    {
    if (aFirst.iOwner < aSecond.iOwner)
        {
        return -1; // less than
        }
    else if (aFirst.iOwner > aSecond.iOwner)
        {
        return 1; // more than  
        }
    else
        {
        // For PC-lint
        }

    return 0; // Equal
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CHuiCanvasTextureCache::CHuiCanvasTextureCache()
    {
    SetMemoryLevel(EHuiMemoryLevelNormal);
    CHuiStatic::Env().AddMemoryLevelObserver(this);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHuiCanvasTextureCache::~CHuiCanvasTextureCache()
    {
    CHuiStatic::Env().RemoveMemoryLevelObserver(this);
    iCachedTexts.ResetAndDestroy();
    iCachedImages.ResetAndDestroy();        
    iRecycledTextures.ResetAndDestroy();
    
    if (iSearchedTextImageEntry)
        {
        iSearchedTextImageEntry->Reset();
        delete iSearchedTextImageEntry;
        }
    
    if (iSearchedGraphicImageEntry)
        {
        iSearchedGraphicImageEntry->Reset();
        delete iSearchedGraphicImageEntry;
        }
    
    if (iSearchedRenderBufferImageEntry)
        {
        iSearchedRenderBufferImageEntry->Reset();
        delete iSearchedRenderBufferImageEntry;        
        }

    delete iTextImageRasterizer;
    delete iOutLineTextImageRasterizer;
    delete iGraphicImageRasterizer;
    delete iCombinedGraphicImageRasterizer;
    delete iCanvasGc;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHuiCanvasTextureCache* CHuiCanvasTextureCache::NewL()
    {
    CHuiCanvasTextureCache* self = CHuiCanvasTextureCache::NewLC();
    CleanupStack::Pop(self);
    return self;        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHuiCanvasTextureCache* CHuiCanvasTextureCache::NewLC()
    {
    CHuiCanvasTextureCache* self = new (ELeave) CHuiCanvasTextureCache();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasTextureCache::ConstructL()
    {
    iMemoryLevel = EHuiMemoryLevelNormal;
    
    iCanvasGc = CHuiStatic::Renderer().CreateCanvasGcL();
    
    iTextImageRasterizer = new (ELeave) CHuiCanvasTextImageRasterizer;
    iTextImageRasterizer->ConstructL(EGray256);
    
    iOutLineTextImageRasterizer = new (ELeave) CHuiCanvasTextImageRasterizer;
    iOutLineTextImageRasterizer->ConstructL(EColor16MA);
    
    iGraphicImageRasterizer = new (ELeave) CHuiCanvasGraphicImageRasterizer;
    iGraphicImageRasterizer->ConstructL();

    iCombinedGraphicImageRasterizer = new (ELeave) CHuiCanvasCombinedGraphicImageRasterizer;    
    iCombinedGraphicImageRasterizer->ConstructL(EColor64K);
    
    iSearchedTextImageEntry = new (ELeave) CHuiCanvasTextImage;
    iSearchedGraphicImageEntry = new (ELeave) CHuiCanvasGraphicImage;
    iSearchedRenderBufferImageEntry = new (ELeave) CHuiCanvasRenderBufferImage;
    }
    

// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
//
const CHuiCanvasGraphicImage* CHuiCanvasTextureCache::FindCachedImageL(
    const THuiCachedImageParams& aCachedImageParams, 
    const CHuiCanvasVisual& aUser)
    {    
    TInt cachedEntry = KErrNotFound;

    iSearchedGraphicImageEntry->Reset();        
    iSearchedGraphicImageEntry->iBitmapHandle = aCachedImageParams.iBitmapHandle;
    iSearchedGraphicImageEntry->iMaskHandle = aCachedImageParams.iMaskHandle;
    iSearchedGraphicImageEntry->iTexture = NULL;
    iSearchedGraphicImageEntry->iInvertedMask = aCachedImageParams.iInvertedMask;
    iSearchedGraphicImageEntry->iMaskOriginPoint = aCachedImageParams.iMaskOriginPoint;
    iSearchedGraphicImageEntry->iBitmapTouchCount = KHuiInitialBitmapTouchCount;
    iSearchedGraphicImageEntry->iMaskTouchCount = KHuiInitialBitmapTouchCount;
    iSearchedGraphicImageEntry->iGcParams = aCachedImageParams.iGcParams;
    iSearchedGraphicImageEntry->iCache = this;
    
    cachedEntry = iCachedImages.FindInOrder(iSearchedGraphicImageEntry, ImageOrderFunc);
    if (cachedEntry == KErrNotFound)
        {
        User::Leave(KErrNotFound);
        }
    
    iCachedImages[cachedEntry]->RefreshUser(aUser);
    return iCachedImages[cachedEntry];    
    }

// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
//
const CHuiCanvasGraphicImage* CHuiCanvasTextureCache::CreateCombinedCachedImageL(
    const RArray<THuiCachedCombinedImageParams>& aCachedImageParams,
    const THuiCachedGcParams& aGcParams,
    const TSize& aImageSize, 
    const CHuiCanvasVisual& aUser)
    {
    TBool needUpdate = EFalse;
    TInt cachedEntry = KErrNotFound;
        
    iSearchedGraphicImageEntry->Reset();
    iSearchedGraphicImageEntry->iGcParams = aGcParams;
    iSearchedGraphicImageEntry->iImageSize = aImageSize;
    iSearchedGraphicImageEntry->iCache = this;
    
    
    for (TInt i=0; i<aCachedImageParams.Count();i++)
        {
        THuiCachedBitmap cachedBitmap;
        cachedBitmap.iBitmapHandle = aCachedImageParams[i].iBitmapHandle;
        cachedBitmap.iMaskHandle = aCachedImageParams[i].iMaskHandle;
        cachedBitmap.iCombinedBitmapPoint = aCachedImageParams[i].iCombinedBitmapPoint;        
        iSearchedGraphicImageEntry->iSubImages.Append(cachedBitmap);            
        }
            
    cachedEntry = iCachedImages.FindInOrder(iSearchedGraphicImageEntry, ImageOrderFunc);
    if (cachedEntry == KErrNotFound)
        {
        // Create new entry object
        CHuiCanvasGraphicImage* newEntry = new (ELeave) CHuiCanvasGraphicImage; 
        CleanupStack::PushL(newEntry);                    
        newEntry->CopyAttributes(*iSearchedGraphicImageEntry);
        
        // Create new accelerated image
        CHuiTexture* newtexture = CreateRecycledTextureL();
        newEntry->iTexture = newtexture;        
        
        User::LeaveIfError(iCachedImages.InsertInOrder(newEntry, ImageOrderFunc));
        cachedEntry = iCachedImages.FindInOrder(newEntry, ImageOrderFunc);

        CleanupStack::Pop(newEntry);
        needUpdate = ETrue;            
        }
    else
        {
        // Check if needs to be updated by checking active users list
        if (!iCachedImages[cachedEntry]->IsUser(aUser))
            {
            needUpdate = ETrue;            
            }
        }    

    if (needUpdate)
        {
        TRAPD(updateError, iCombinedGraphicImageRasterizer->RasterizeL(*iCachedImages[cachedEntry]));
        if (updateError == KErrNoMemory)
            {                
            // Call grim reaper to initiate system level texture memory actions !
            HandleOutOfTextureMemory();
            User::LeaveIfError(updateError);
            }
        }

    iCachedImages[cachedEntry]->RefreshUser(aUser);        
    return iCachedImages[cachedEntry];    
    }

    
// ---------------------------------------------------------------------------
// Improvement idea for HW accelerated, upload all images of the buffer 
// into single texture and use texture coordinates to draw each image. 
// ---------------------------------------------------------------------------
//
const CHuiCanvasGraphicImage* CHuiCanvasTextureCache::CreateCachedImageL(
    const THuiCachedImageParams& aCachedImageParams, 
    const CHuiCanvasVisual& aUser)
    {    
    TBool needUpdate = EFalse;
    TInt cachedEntry = KErrNotFound;
    
    iSearchedGraphicImageEntry->Reset();    
    iSearchedGraphicImageEntry->iBitmapHandle = aCachedImageParams.iBitmapHandle;
    iSearchedGraphicImageEntry->iMaskHandle = aCachedImageParams.iMaskHandle;
    iSearchedGraphicImageEntry->iTexture = NULL;
    iSearchedGraphicImageEntry->iInvertedMask = aCachedImageParams.iInvertedMask;
    iSearchedGraphicImageEntry->iMaskOriginPoint = aCachedImageParams.iMaskOriginPoint;
    iSearchedGraphicImageEntry->iBitmapTouchCount = KHuiInitialBitmapTouchCount;
    iSearchedGraphicImageEntry->iMaskTouchCount = KHuiInitialBitmapTouchCount;
    iSearchedGraphicImageEntry->iGcParams = aCachedImageParams.iGcParams;
    iSearchedGraphicImageEntry->iCache = this;
    
    cachedEntry = iCachedImages.FindInOrder(iSearchedGraphicImageEntry, ImageOrderFunc);
    if (cachedEntry == KErrNotFound)
        {
        // Create new entry object
        CHuiCanvasGraphicImage* newEntry = new (ELeave) CHuiCanvasGraphicImage; 
        CleanupStack::PushL(newEntry);
        
        newEntry->CopyAttributes(*iSearchedGraphicImageEntry);
        
        // Create new accelerated image
        CHuiTexture* newtexture = CreateRecycledTextureL();
        newEntry->iTexture = newtexture;        

        User::LeaveIfError(iCachedImages.InsertInOrder(newEntry, ImageOrderFunc));
        cachedEntry = iCachedImages.FindInOrder(newEntry, ImageOrderFunc);
        CleanupStack::Pop(newEntry);
        needUpdate = ETrue;
        }
    else
        {
        // Check if needs to be updated by checking active users list
        if (!iCachedImages[cachedEntry]->IsUser(aUser))
            {
            needUpdate = ETrue;            
            }
        }    

    if (needUpdate)
        {
        TRAPD(updateError, iGraphicImageRasterizer->RasterizeL(*iCachedImages[cachedEntry]));
        if (updateError == KErrNoMemory)
            {                                
            // Call grim reaper to initiate system level texture memory actions !
            HandleOutOfTextureMemory();
            User::LeaveIfError(updateError);
            }
        }

    iCachedImages[cachedEntry]->RefreshUser(aUser);        
    return iCachedImages[cachedEntry];    
    }
    
    

    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const CHuiCanvasTextImage* CHuiCanvasTextureCache::CreateCachedTextL( 
    const THuiCachedTextParams& aCachedTextParams,
    const CHuiCanvasVisual& aUser)
    {
    TBool needUpdate = EFalse;
    TInt cachedEntry = KErrNotFound;
    
    iSearchedTextImageEntry->Reset();        
    iSearchedTextImageEntry->iFindTextPtr= aCachedTextParams.iFindTextPtr; // Not owned, gets be updated if not found
    iSearchedTextImageEntry->iTexture = NULL; // Gets updated if not found
    iSearchedTextImageEntry->iFontHandle = aCachedTextParams.iFontHandle;
    iSearchedTextImageEntry->iRasterizationOffset = TPoint(0,0); // Gets updated when text has been rasterized        
    Mem::Copy( &iSearchedTextImageEntry->iTextParams, &aCachedTextParams.iTextParams, sizeof(THuiCanvasTextParameters));        
    iSearchedTextImageEntry->iTextBoxMaxSize = aCachedTextParams.iTextBoxMaxSize;
    iSearchedTextImageEntry->iTextWidth = aCachedTextParams.iTextWidth;
    iSearchedTextImageEntry->iBaseLineOffset = aCachedTextParams.iBaseLineOffset;
    iSearchedTextImageEntry->iTextAlign = aCachedTextParams.iTextAlign;
    iSearchedTextImageEntry->iMargin = aCachedTextParams.iMargin;
    iSearchedTextImageEntry->iAngle = aCachedTextParams.iAngle;
    iSearchedTextImageEntry->iGcParams = aCachedTextParams.iGcParams;

    
    cachedEntry = iCachedTexts.FindInOrder(iSearchedTextImageEntry, TextOrderFunc);
    if (cachedEntry == KErrNotFound)
        {
        // Create new entry object
        CHuiCanvasTextImage* newTextEntry = new (ELeave) CHuiCanvasTextImage; 
        CleanupStack::PushL(newTextEntry);                    
        
        newTextEntry->CopyAttributes(*iSearchedTextImageEntry);
        
        // Create new accelerated image
        CHuiTexture* newtexture = CreateRecycledTextureL();
        newTextEntry->iTexture = newtexture;

        // Create text which entry will own        
        HBufC* ownedText = aCachedTextParams.iFindTextPtr->AllocL();
        newTextEntry->iText = ownedText;
        newTextEntry->iFindTextPtr = NULL;
        TInt insertSuccess = iCachedTexts.InsertInOrder(newTextEntry, TextOrderFunc);
        __ASSERT_DEBUG( !insertSuccess, USER_INVARIANT() );
        User::LeaveIfError( insertSuccess );
        cachedEntry = iCachedTexts.FindInOrder(newTextEntry, TextOrderFunc);

        CleanupStack::Pop(newTextEntry); 
        needUpdate = ETrue;            
        }
    else
        {
        // If it was found, great. No need to update it.
        }        


    if (needUpdate)
        {
        TBool outlinefont = EFalse;

        if (!(*iCachedTexts[cachedEntry]).iFont)
            {
            CFbsBitGcFont* font = new (ELeave) CFbsBitGcFont();
            CleanupStack::PushL(font);
            User::LeaveIfError(font->Duplicate((*iCachedTexts[cachedEntry]).iFontHandle));
            (*iCachedTexts[cachedEntry]).iFont = font;
            CleanupStack::Pop(); // font                    
            }

        outlinefont = (*iCachedTexts[cachedEntry]).iFont->FontSpecInTwips().iFontStyle.IsEffectOn(FontEffect::EOutline);
        TInt updateError(KErrNone);
        if (!outlinefont)
            {
            TRAP(updateError, iTextImageRasterizer->RasterizeL(*iCachedTexts[cachedEntry]));
            }
        else
            {
            TRAP(updateError, iOutLineTextImageRasterizer->RasterizeL(*iCachedTexts[cachedEntry]));
            }
        
        
        
        if (updateError == KErrNoMemory)
            {                
            // Call grim reaper to initiate system level texture memory actions !
            HandleOutOfTextureMemory();
            User::LeaveIfError(updateError);
            }
        }

    iCachedTexts[cachedEntry]->RefreshUser(aUser);
    return iCachedTexts[cachedEntry];            
    }




// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasTextureCache::ReleaseAllCachedEntries(const CHuiCanvasVisual& aUser)
    {    
    // Texts
    for (TInt i=0; i < iCachedTexts.Count(); i++)
        {
        TInt index = iCachedTexts[i]->iActiveUsers.FindInAddressOrder(&aUser);

        if (index != KErrNotFound)
            {
            iCachedTexts[i]->iActiveUsers.Remove(index);
            iHasReleasedTexts = ETrue;
            }
        }        

    // Images
    for (TInt i=0; i < iCachedImages.Count(); i++)
        {
        TInt index = iCachedImages[i]->iActiveUsers.FindInAddressOrder(&aUser);

        if (index != KErrNotFound)
            {
            iCachedImages[i]->iActiveUsers.Remove(index);
            iHasReleasedImages = ETrue;
            }
        }
    
    // Render buffers are _not_ released together with images and text    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasTextureCache::DeleteAllReleasedEntries(TBool aAllowKeepCached)
    {
    // Texts
    if (iHasReleasedTexts)
        {
        RPointerArray<CHuiCanvasTextImage> unusedEntries;
        
        FindUnusedTextEntries(unusedEntries);
            
        // Remove unused entries if they fit into the cache, leave last used ones in the cache if cache size allows
        if (aAllowKeepCached)
            {                
            SelectPreservedUnusedTextEntries(unusedEntries);            
            }
        
        DeleteUnusedTextEntries(unusedEntries);

        unusedEntries.Close();                
        iHasReleasedTexts = EFalse;                        
        }

    // Images
    if (iHasReleasedImages)
        {
        RPointerArray<CHuiCanvasGraphicImage> unusedEntries;

        FindUnusedImageEntries(unusedEntries);

        // Remove unused entries if they fit into the cache, leave last used ones in the cache if cache size allows
        if (aAllowKeepCached)
            {                
            SelectPreservedUnusedImageEntries(unusedEntries);            
            }

        DeleteUnusedImageEntries(unusedEntries);
            
        unusedEntries.Close();                            
        iHasReleasedImages = EFalse;                                        
        }
    
    // Render buffers
    if (iHasReleasedRenderBuffers)
        {
        RPointerArray<CHuiCanvasRenderBufferImage> unusedEntries;

        FindUnusedRenderBufferEntries(unusedEntries);

        // Remove unused entries if they fit into the cache, leave last used ones in the cache if cache size allows
        if (aAllowKeepCached)
            {                
            SelectPreservedUnusedRenderBufferEntries(unusedEntries);            
            }

        DeleteUnusedRenderBufferEntries(unusedEntries);
            
        unusedEntries.Close();                            
        iHasReleasedRenderBuffers = EFalse;                                        
        }    
    }

#ifdef HUI_DEBUG_PRINT_CANVAS_TEXTURE_CACHE_EGL
#include "eglext.h"
typedef EGLBoolean (*NOK_resource_profiling)(EGLDisplay, EGLint, EGLint*, EGLint, EGLint*);

void ReportGraphicsMemoryUsage()
    {
	RDebug::Print(_L("About to ask EGL profiling ext"));
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	
    /*
     ** Search for extName in the extensions string.  Use of strstr()
     ** is not sufficient because extension names can be prefixes of
     ** other extension names.  Could use strtok() but the constant
     ** string returned by glGetString can be in read-only memory.
     */
   /* TBool extensionFound = EFalse;
    char *p = (char *) eglQueryString(dpy,EGL_EXTENSIONS);
    char *end;
    int extNameLen = strlen("EGL_NOK_resource_profiling");
    end = p + strlen(p);

    while (p < end) 
        {
        int n = strcspn(p, " ");
        if ((extNameLen == n) && (strncmp("EGL_NOK_resource_profiling", p, n) == 0)) 
            {
            extensionFound = ETrue;
            break;
            }
        p += (n + 1);
        }
  
    if (!extensionFound)
        {
        TRACES("EGL_NOK_resource_profiling not found from EGL_EXTENSIONS");
        }*/
    // okay, let's fetch the function ptr to our profiling functions
    // resource profiling extension function ptr
    NOK_resource_profiling eglQueryProfilingData = (NOK_resource_profiling)eglGetProcAddress("eglQueryProfilingDataNOK");
    if (!eglQueryProfilingData)
        {
	   	RDebug::Print(_L("EGL profiling ext not present"));
        return;
        }

    EGLint data_count;
    EGLint* prof_data;
    TInt i(0);

	/* Find out how much profiling data is available */
	eglQueryProfilingData(dpy, EGL_PROF_QUERY_GLOBAL_BIT_NOK | EGL_PROF_QUERY_MEMORY_USAGE_BIT_NOK,
							NULL, 0, &data_count);

	/* Allocate room for the profiling data */
	prof_data = (EGLint*)User::Alloc(data_count * sizeof(EGLint));
	if (prof_data == NULL)
		{
		RDebug::Print(_L("Could not get mem for EGL profiling ext"));
		return;
		}
	/* Retrieve the profiling data */
	eglQueryProfilingData(dpy,
							 EGL_PROF_QUERY_GLOBAL_BIT_NOK |
							 EGL_PROF_QUERY_MEMORY_USAGE_BIT_NOK,
							 prof_data,
							 data_count,
							 &data_count);

	/* Iterate over the returned data */
	while (i < data_count)
		{
		switch (prof_data[i++])
			{
			case EGL_PROF_USED_MEMORY_NOK:
				{
				RDebug::Print(_L("EGL Profiling: used mem %d"), prof_data[i++]);
				break;
				}
			case EGL_PROF_TOTAL_MEMORY_NOK:
				{
				RDebug::Print(_L("EGL Profiling: total mem available: %d"), prof_data[i++]);
				break;
				}
		    case EGL_PROF_THREAD_ID_NOK:
                {
                if (sizeof(EGLNativeThreadIdTypeNOK) == 8)
                    {
                    i+=2;
                    }
                else
                    {
                    i++;
                    }
                break;
                }
            case EGL_PROF_THREAD_USED_PRIVATE_MEMORY_NOK:
            case EGL_PROF_THREAD_USED_SHARED_MEMORY_NOK:
			default:
				{
                i++;
				break;
				}
			}
		}

	User::Free(prof_data);
    }

#endif
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasTextureCache::AdvanceTime()
    {
#ifdef HUI_DEBUG_PRINT_CANVAS_TEXTURE_CACHE
    RDebug::Print(_L("-- CHuiCanvasTextureCache::AdvanceTime: begin --"));
    if (IsLowMemoryStateEnabled())
        {        
        RDebug::Print(_L("-- CHuiCanvasTextureCache::AdvanceTime: LOW MEMORY STATE ENABLED"));
        }    
    if(iEglProfiling > 10)
        {
#ifdef HUI_DEBUG_PRINT_CANVAS_TEXTURE_CACHE_EGL
        ReportGraphicsMemoryUsage();
#endif
        iEglProfiling = 0;
        }
    else
        {
        iEglProfiling++; 
        }           
#endif

    // We do not delete all, by leaving something unused in the cache
    // we can improve performance.
    CalculateGraphicsMemoryUsage();
    DeleteAllReleasedEntries(ETrue);        

    // Reset bitmap sizes to save memory. Commented out as resize appears to
    // be relatively slow (0.2 - 0.5 ms). Not resizing here means that it uses
    // more RAM between frames.
    //iTempMask->Resize(KEmptyBitmapSize);
    //iTempBitmap->Resize(KEmptyBitmapSize);                        
    
    //iFontHandle = 0;
    //delete iFont;
    //iFont = NULL;

#ifdef HUI_DEBUG_PRINT_CANVAS_TEXTURE_CACHE
    RDebug::Print(_L("-- Cached texts: %i --"), iCachedTexts.Count());
    RDebug::Print(_L("-- Cached imgs : %i --"), iCachedImages.Count());

    TInt totalPixels = 0;

    for(TInt i=iCachedTexts.Count() - 1; i >= 0; i--)
        {    
        TSize size = iCachedTexts[i]->iTexture->Size();    
        RDebug::Print(_L("-- Cached text: %ix%i, users=%i, string=%S --"), size.iWidth, size.iHeight, iCachedTexts[i]->iActiveUsers.Count(), iCachedTexts[i]->iText );
        totalPixels += size.iWidth * size.iHeight;

#ifdef HUI_DEBUG_PRINT_CANVAS_TEXTURE_CACHE_EXTENDED

        RDebug::Print(_L("--   iFontHandle: %i --"), iCachedTexts[i]->iFontHandle );
        RDebug::Print(_L("--   iTextWidth: %i --"), iCachedTexts[i]->iTextWidth );
        RDebug::Print(_L("--   iTextBoxMaxSize.iWidth: %i --"), iCachedTexts[i]->iTextBoxMaxSize.iWidth );
        RDebug::Print(_L("--   iBaseLineOffset: %i --"), iCachedTexts[i]->iBaseLineOffset );
        RDebug::Print(_L("--   iTextAlign: %i --"), iCachedTexts[i]->iTextAlign );
        RDebug::Print(_L("--   iMargin: %i --"), iCachedTexts[i]->iMargin );
        RDebug::Print(_L("--   iAngle: %i --"), iCachedTexts[i]->iAngle );
        RDebug::Print(_L("--   iTextParams.iStart: %i --"), iCachedTexts[i]->iTextParams.iStart );
        RDebug::Print(_L("--   iTextParams.iEnd: %i --"), iCachedTexts[i]->iTextParams.iEnd );
        RDebug::Print(_L("--   iTextParams.iFlagst: %i --"), iCachedTexts[i]->iTextParams.iFlags );
        

        THuiCachedGcParams p = iCachedTexts[i]->iGcParams;    
        RDebug::Print(_L("--    iPenColor: %i --"), p.iPenColor );
        RDebug::Print(_L("--    iPenStyle: %i --"), p.iPenStyle );
        RDebug::Print(_L("--    iDrawMode: %i --"), p.iDrawMode );
        RDebug::Print(_L("--    iBrushColor: %i --"), p.iBrushColor );
        RDebug::Print(_L("--    iBrushStyle: %i --"), p.iBrushStyle );
        RDebug::Print(_L("--    iShadowMode: %i --"), p.iShadowMode );
        RDebug::Print(_L("--    iStrikethrough: %i --"), p.iStrikethrough );
        RDebug::Print(_L("--    iUnderline: %i --"), p.iUnderline );
        RDebug::Print(_L("--    iUserDisplayMode: %i --"), p.iUserDisplayMode );
        RDebug::Print(_L("--    iShadowColor: %i --"), p.iShadowColor );
#endif
        }

    for(TInt i=iCachedImages.Count() - 1; i >= 0; i--)
        {    
        TSize size = iCachedImages[i]->iTexture->Size();    
        if (iCachedImages[i]->iSubImages.Count())
            {
            RDebug::Print(_L("-- Cached combined image: %i+%i  %ix%i, users=%i --"), iCachedImages[i]->iBitmapHandle, iCachedImages[i]->iMaskHandle, size.iWidth, size.iHeight, iCachedImages[i]->iActiveUsers.Count());                        
            RDebug::Print(_L("--  Blit size: %ix%i --"), iCachedImages[i]->iImageSize.iWidth, iCachedImages[i]->iImageSize.iHeight);                            
            for (TInt j=0; j<iCachedImages[i]->iSubImages.Count();j++)
                {
                RDebug::Print(_L("--  subimage: %i+%i Point: %i,%i--"), 
                    iCachedImages[i]->iSubImages[j].iBitmapHandle, 
                    iCachedImages[i]->iSubImages[j].iMaskHandle, 
                    iCachedImages[i]->iSubImages[j].iCombinedBitmapPoint.iX,
                    iCachedImages[i]->iSubImages[j].iCombinedBitmapPoint.iY);        
                }            
            }
        else
            {
            RDebug::Print(_L("-- Cached image: %i+%i  %ix%i, users=%i --"), iCachedImages[i]->iBitmapHandle,iCachedImages[i]->iMaskHandle, size.iWidth, size.iHeight, iCachedImages[i]->iActiveUsers.Count());        
            }    
        
        totalPixels += size.iWidth * size.iHeight;
        }
        
    RDebug::Print(_L("-- Estimated canvas texture mem usage with 32bpp %i KBytes --"), totalPixels*4 / 1024);

    TInt unusedCanvasTextures = CalculateUnusedCanvasTextureUsageInKbytes();

    RDebug::Print(_L("-- Estimated canvas unused texture mem usage with 32bpp %i KBytes --"), unusedCanvasTextures);

    TInt totalUsedTextureMemoryInKBytes = CHuiStatic::Env().TextureManager().EstimatedTextureMemUsage(KHuiTextureEstimatedBpp) / 1024;
    
    RDebug::Print(_L("-- Estimated total texture mem usage with 32bpp %i KBytes --"), totalUsedTextureMemoryInKBytes);

    TInt totalPixelsForRenderBuffers = 0;
    for(TInt i=iCachedRenderBuffers.Count() - 1; i >= 0; i--)
        {
        if (iCachedRenderBuffers[i]->iCanvasRenderBuffer)
            {
            TSize size = iCachedRenderBuffers[i]->iCanvasRenderBuffer->Size();    
            RDebug::Print(_L("-- Cached render buffer: %ix%i, users=%i --"),  size.iWidth, size.iHeight, iCachedRenderBuffers[i]->iActiveUsers.Count());                
            totalPixelsForRenderBuffers += size.iWidth * size.iHeight;
            }
        else
            {
            RDebug::Print(_L("-- Cached render buffer: 0x0, users=%i --"), iCachedRenderBuffers[i]->iActiveUsers.Count());                
            }
        }
    RDebug::Print(_L("-- Estimated canvas render buffer mem usage with 32bpp %i KBytes --"), totalPixelsForRenderBuffers*4 / 1024);    
    
    TInt totalAllocSize = 0;
    User::AllocSize(totalAllocSize);
    RDebug::Print(_L("-- User::AllocSize = %i"), totalAllocSize);

    RDebug::Print(_L("-- CHuiCanvasTextureCache::AdvanceTime: end --"));
#endif
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasTextureCache::FindUnusedImageEntries(RPointerArray<CHuiCanvasGraphicImage>& aIndexEntries)
    {
    for(TInt i=iCachedImages.Count() - 1; i >= 0; i--)
        {
        CHuiCanvasGraphicImage* entry = iCachedImages[i];    
        if (!entry->iActiveUsers.Count())
            {
            aIndexEntries.InsertInOrderAllowRepeats(entry, ImageTimeOrderFunc);
            }
        }    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasTextureCache::FindUnusedTextEntries(RPointerArray<CHuiCanvasTextImage>& aIndexEntries)
    {
    for(TInt i=iCachedTexts.Count() - 1; i >= 0; i--)
        {                
        CHuiCanvasTextImage* entry = iCachedTexts[i];    

        if (!entry->iActiveUsers.Count())
            {
            aIndexEntries.InsertInOrderAllowRepeats(entry, TextTimeOrderFunc);
            }
        }        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasTextureCache::FindUnusedRenderBufferEntries(RPointerArray<CHuiCanvasRenderBufferImage>& aIndexEntries)
    {
    for(TInt i=iCachedRenderBuffers.Count() - 1; i >= 0; i--)
        {                
        CHuiCanvasRenderBufferImage* entry = iCachedRenderBuffers[i];    

        if (!entry->iActiveUsers.Count())
            {
            aIndexEntries.InsertInOrderAllowRepeats(entry, RenderBufferTimeOrderFunc);
            }
        }            
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasTextureCache::DeleteUnusedImageEntries(RPointerArray<CHuiCanvasGraphicImage>& aIndexEntries)
    {
    // Remove given entries
    for(TInt i=aIndexEntries.Count() - 1; i >= 0; i--)
        {
        TInt indexEntry = iCachedImages.FindInOrder(aIndexEntries[i], ImageOrderFunc);
        if (indexEntry != KErrNotFound)
            {                        
            CHuiCanvasGraphicImage* entry = iCachedImages[indexEntry];
            iCachedImages.Remove(indexEntry); 
            delete entry;
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasTextureCache::DeleteUnusedTextEntries(RPointerArray<CHuiCanvasTextImage>& aIndexEntries)
    {
    // Remove given entries
    for(TInt i=aIndexEntries.Count() - 1; i >= 0; i--)
        {                
        TInt indexEntry = iCachedTexts.FindInOrder(aIndexEntries[i], TextOrderFunc);
        if (indexEntry != KErrNotFound)
            {                        
            CHuiCanvasTextImage* entry = iCachedTexts[indexEntry];
            iCachedTexts.Remove(indexEntry); 
            delete entry;
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasTextureCache::DeleteUnusedRenderBufferEntries(RPointerArray<CHuiCanvasRenderBufferImage>& aIndexEntries)
    {
    // Remove given entries
    for(TInt i=aIndexEntries.Count() - 1; i >= 0; i--)
        {                
        TInt indexEntry = iCachedRenderBuffers.FindInOrder(aIndexEntries[i], RenderBufferOrderFunc);
        if (indexEntry != KErrNotFound)
            {                        
            CHuiCanvasRenderBufferImage* entry = iCachedRenderBuffers[indexEntry];
            iCachedRenderBuffers.Remove(indexEntry);
            delete entry;            
            }
        }    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasTextureCache::SelectPreservedUnusedImageEntries(RPointerArray<CHuiCanvasGraphicImage>& aIndexEntries)
    {
    TInt totalUnusedTextureBytes = 0;

    // Remove unused entries if they fit into the cache, leave last used ones in the cache if cache size allows
    for(TInt i=aIndexEntries.Count() - 1; i >= 0; i--)
        {
        // Always delete bitmaps from unused entries, we can again duplicate pointers from handles when needed.
        // Pointers are kept only for perfromance reasons.
        CHuiCanvasGraphicImage* entry = aIndexEntries[i];
        TBool is16bit = EFalse;
        if (entry->iBitmap && !entry->iMask && entry->iBitmap->ExtendedBitmapType() == KNullUid && entry->iBitmap->DisplayMode() == EColor64K)
            {
            is16bit = ETrue;
            }
        delete entry->iBitmap;    
        entry->iBitmap = NULL;

        delete entry->iMask;    
        entry->iMask = NULL;        
        
        for (TInt j=0; j < entry->iSubImages.Count(); j++)
            {
            delete entry->iSubImages[j].iBitmap;
            entry->iSubImages[j].iBitmap = NULL;
            
            delete entry->iSubImages[j].iMask;
            entry->iSubImages[j].iMask = NULL;    
            }
                    
        TSize textureSize = entry->iTexture->Size();    
        TInt textureEstimatedSizeInBytes = textureSize.iWidth * textureSize.iHeight * (is16bit ? KHuiCanvasImageEstimatedBpp/16.f : KHuiCanvasImageEstimatedBpp/8.f);

        if (totalUnusedTextureBytes + textureEstimatedSizeInBytes < iUnusedCanvasImageTextureCacheSizeInKBytes*1024)
            {
            // Fits in the cache, remove from unused list, but keep zero size textures                        
            if (textureEstimatedSizeInBytes != 0)
                {
                aIndexEntries.Remove(i);        
                }            
            }

        totalUnusedTextureBytes += textureEstimatedSizeInBytes;
        }                
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasTextureCache::SelectPreservedUnusedTextEntries(RPointerArray<CHuiCanvasTextImage>& aIndexEntries)
    {
    TInt totalUnusedTextureBytes = 0;

    // Remove unused entries if they fit into the cache, leave last used ones in the cache if cache size allows
    for(TInt i=aIndexEntries.Count() - 1; i >= 0; i--)
        {
        TSize textureSize = aIndexEntries[i]->iTexture->Size();    
        TInt textureEstimatedSizeInBytes = textureSize.iWidth * textureSize.iHeight * KHuiCanvasTextEstimatedBpp/8.f;
            
        if (totalUnusedTextureBytes + textureEstimatedSizeInBytes < iUnusedCanvasTextTextureCacheSizeInKBytes*1024)
            {
            // Fits in the cache, remove from unused list, but keep zero size textures                         
            if (textureEstimatedSizeInBytes != 0)
                {
                aIndexEntries.Remove(i);
                }
            }

        totalUnusedTextureBytes += textureEstimatedSizeInBytes;        
        }        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasTextureCache::SelectPreservedUnusedRenderBufferEntries(RPointerArray<CHuiCanvasRenderBufferImage>& aIndexEntries)
    {
    TInt totalUnusedRenderBufferBytes = 0;

    // Remove unused entries if they fit into the cache, leave last used ones in the cache if cache size allows
    for(TInt i=aIndexEntries.Count() - 1; i >= 0; i--)
        {
        CHuiCanvasRenderBufferImage* entry = aIndexEntries[i];
        if (entry->iCanvasRenderBuffer)
            {
            TSize renderBufferSize = entry->iCanvasRenderBuffer->Size();    
            TInt renderBufferEstimatedSizeInBytes = renderBufferSize.iWidth * renderBufferSize.iHeight * KHuiCanvasRenderBufferEstimatedBpp/8.f;
                
            if (totalUnusedRenderBufferBytes + renderBufferEstimatedSizeInBytes < iUnusedCanvasRenderBufferCacheSizeInKBytes*1024)
                {
                aIndexEntries.Remove(i);
                }
            totalUnusedRenderBufferBytes += renderBufferEstimatedSizeInBytes;        
            }
        else
            {
#ifdef HUI_DEBUG_PRINT_CANVAS_TEXTURE_CACHE            
            RDebug::Print(_L("CHuiCanvasTextureCache::PurgeUnusedRenderBufferEntries: Non existing render buffer, Render does not support canvas render buffers ?"));
#endif
            }
        }            
    }



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasTextureCache::HandleOutOfTextureMemory()
    {
    RDebug::Print(_L("CHuiCanvasTextureCache::HandleOutOfTextureMemory: Out of memory happened !"));

    DeleteAllReleasedEntries(EFalse);    
    
    // TODO: Who you gonna call when texture memory is full and we cannot ourself
    // delete anymore textures ? 
    //
    // Grim reaper ?!
    //
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasTextureCache::CalculateGraphicsMemoryUsage()
    {
    // TODO: This should handle NVG textures

    if (iMaxTextureMemoryInKBytes)
        {
        // How much all alfred textures use memory currently, this include alf apps and all canvas textures
        TInt totalUsedTextureMemoryInKBytes = CHuiStatic::Env().TextureManager().EstimatedTextureMemUsage(KHuiTextureEstimatedBpp) / 1024;

        // How much there are unused textures cached currently
        TInt totalUnusedCanvasTextureMemoryUsageInKBytes = CalculateUnusedCanvasTextureUsageInKbytes();

        // Calculate how much there is space for unused textures 
        TInt availableCacheSizeInKBytes = iMaxTextureMemoryInKBytes - (totalUsedTextureMemoryInKBytes - totalUnusedCanvasTextureMemoryUsageInKBytes);

        // Divide available space for unused textures between texts and images using defined ratio
        if (availableCacheSizeInKBytes > 0)
            {
            iUnusedCanvasTextTextureCacheSizeInKBytes = availableCacheSizeInKBytes * KHuiCanvasUnusedTextImageCacheRatio;
            iUnusedCanvasImageTextureCacheSizeInKBytes = availableCacheSizeInKBytes * (1.f - KHuiCanvasUnusedTextImageCacheRatio);                            
            }
        else
            {
            iUnusedCanvasTextTextureCacheSizeInKBytes = 0;
            iUnusedCanvasImageTextureCacheSizeInKBytes = 0;  
            }    
        }
    else
        {
        iUnusedCanvasTextTextureCacheSizeInKBytes = 0;
        iUnusedCanvasImageTextureCacheSizeInKBytes = 0;          
        }
    
    if (iMaxRenderBufferMemoryInKBytes)
        {
        // How much there are render buffers cached currently
        TInt totalInKBytes = CalculateTotalCanvasRenderBufferUsageInKbytes();

        // How much available for unused
        TInt availableCacheSizeInKBytes = iMaxRenderBufferMemoryInKBytes - totalInKBytes;

        if (availableCacheSizeInKBytes > 0)
            {
            iUnusedCanvasRenderBufferCacheSizeInKBytes = iMaxRenderBufferMemoryInKBytes - totalInKBytes;
            }
        else
            {
            iUnusedCanvasRenderBufferCacheSizeInKBytes = 0;
            }    
        }
    else
        {
        iUnusedCanvasRenderBufferCacheSizeInKBytes = 0;
        }

#ifdef HUI_DEBUG_PRINT_CANVAS_TEXTURE_CACHE
    RDebug::Print(_L("-- CHuiCanvasTextureCache::CalculateTextureUsage: iUnusedCanvasTextTextureCacheSizeInKBytes = %i"), iUnusedCanvasTextTextureCacheSizeInKBytes);
    RDebug::Print(_L("-- CHuiCanvasTextureCache::CalculateTextureUsage: iUnusedCanvasImageTextureCacheSizeInKBytes = %i"), iUnusedCanvasImageTextureCacheSizeInKBytes);
    RDebug::Print(_L("-- CHuiCanvasTextureCache::CalculateTextureUsage: iUnusedCanvasRenderBufferCacheSizeInKBytes = %i"), iUnusedCanvasRenderBufferCacheSizeInKBytes);
#endif
    }

void CHuiCanvasTextureCache::EnableTouchCountCheck(TBool aEnable)
    {
    iTouchCountCheckEnabled = aEnable;
    }

TBool CHuiCanvasTextureCache::IsTouchCountCheckEnabled()
    {
    return iTouchCountCheckEnabled;   
    }    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHuiCanvasTextureCache::CalculateUnusedCanvasTextureUsageInKbytes()
    {
    // TODO: This should handle NVG textures
    
    TInt totalUnusedTextureBytes = 0;
    
    // Texts
    RPointerArray<CHuiCanvasTextImage> textEntries;
    
    FindUnusedTextEntries(textEntries);

    for(TInt i=textEntries.Count() - 1; i >= 0; i--)
        {
        TSize textureSize = textEntries[i]->iTexture->Size();    
        totalUnusedTextureBytes += textureSize.iWidth * textureSize.iHeight * KHuiCanvasTextEstimatedBpp/8.f;
        }            
    
    textEntries.Close();        

    // Images
    RPointerArray<CHuiCanvasGraphicImage> imageEntries;
    
    FindUnusedImageEntries(imageEntries);

    for(TInt i=imageEntries.Count() - 1; i >= 0; i--)
        {
        TBool is16bit = EFalse;
        if (imageEntries[i]->iBitmap && !imageEntries[i]->iMask && imageEntries[i]->iBitmap->ExtendedBitmapType() == KNullUid &&imageEntries[i]->iBitmap->DisplayMode() == EColor64K)
            {
            is16bit = ETrue;
            }

        TSize textureSize = imageEntries[i]->iTexture->Size();    
        totalUnusedTextureBytes += textureSize.iWidth * textureSize.iHeight * (is16bit ? KHuiCanvasImageEstimatedBpp/16.f : KHuiCanvasImageEstimatedBpp/8.f);
        }            
    
    imageEntries.Close();        
    
    return totalUnusedTextureBytes/1024;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHuiCanvasTextureCache::CalculateUnusedCanvasRenderBufferUsageInKbytes()
    {
    TInt totalUnusedRenderBufferBytes = 0;
    
    RPointerArray<CHuiCanvasRenderBufferImage> entries;
    
    FindUnusedRenderBufferEntries(entries);

    for(TInt i=entries.Count() - 1; i >= 0; i--)
        {
        if (entries[i]->iCanvasRenderBuffer)
            {        
            TSize renderBufferSize = entries[i]->iCanvasRenderBuffer->Size();    
            totalUnusedRenderBufferBytes += renderBufferSize.iWidth * renderBufferSize.iHeight * KHuiCanvasRenderBufferEstimatedBpp/8.f;
            }
        }            
    
    entries.Close();        

    return totalUnusedRenderBufferBytes/1024;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHuiCanvasTextureCache::CalculateTotalCanvasRenderBufferUsageInKbytes()
    {
    TInt totalRenderBufferBytes = 0;
    for(TInt i=iCachedRenderBuffers.Count() - 1; i >= 0; i--)
        {
        if (iCachedRenderBuffers[i]->iCanvasRenderBuffer)
            {
            TSize renderBufferSize = iCachedRenderBuffers[i]->iCanvasRenderBuffer->Size();    
            totalRenderBufferBytes += renderBufferSize.iWidth * renderBufferSize.iHeight * KHuiCanvasRenderBufferEstimatedBpp/8.f;
            }
        }     
    return totalRenderBufferBytes/1024;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHuiTexture* CHuiCanvasTextureCache::CreateRecycledTextureL()
    {
    CHuiTexture* texture = NULL;
    if (iRecycledTextures.Count())
        {
        // If there are recycled textures, use one of those
        TInt last = iRecycledTextures.Count() - 1;
        texture = iRecycledTextures[last];
        iRecycledTextures.Remove(last);    
        }
    else
        {
        // No recycled textures available, create new one
        texture = CHuiTexture::NewL();    
        }    
    return texture;        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasTextureCache::DeleteRecycledTexture(CHuiTexture* aTexture)
    {
    aTexture->Reset();

    TInt error = KErrNone;
    
    if (iRecycledTextures.Count() < KHuiMaxRecycledTextureCount)
        {
        error = iRecycledTextures.Append(aTexture);            
        if (error)
            {
            delete aTexture;    
            }
        }
    else
        {
        delete aTexture;        
        }            
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHuiCanvasRenderBuffer* CHuiCanvasTextureCache::CreateCachedRenderBufferL(const CHuiCanvasVisual& aUser, TSize aSizeHint)
    {
    if (IsLowMemoryStateEnabled())
        {
        // No canvas render buffers created in low memory state
        return NULL;
        }    
    
    TInt cachedEntry = KErrNotFound;
    CHuiCanvasRenderBufferImage* newRenderBufferEntry = NULL;
    
    iSearchedRenderBufferImageEntry->Reset();        
    iSearchedRenderBufferImageEntry->iOwner = (TAny*)&aUser;
    
    cachedEntry = iCachedRenderBuffers.FindInOrder(iSearchedRenderBufferImageEntry, RenderBufferOrderFunc);
    
    if (cachedEntry == KErrNotFound)
        {
        // Try first getting reused render buffer
        newRenderBufferEntry = ReUseReleasedRenderBufferL(aSizeHint);

        // Create new entry object if needed
        if (!newRenderBufferEntry)
            {
            newRenderBufferEntry = new (ELeave) CHuiCanvasRenderBufferImage;             
            }

        // Copy attributes (only iOwner attribute)
        newRenderBufferEntry->iOwner = iSearchedRenderBufferImageEntry->iOwner;
        
        CleanupStack::PushL(newRenderBufferEntry);                    
        
        // Create new render buffer
        if (!newRenderBufferEntry->iCanvasRenderBuffer)
            {
            newRenderBufferEntry->iCanvasRenderBuffer = iCanvasGc->CreateRenderBufferL(TSize(0,0)); 
            }

        // Insert to cache array
        TInt insertSuccess = iCachedRenderBuffers.InsertInOrder(newRenderBufferEntry, RenderBufferOrderFunc);
        __ASSERT_DEBUG( !insertSuccess, USER_INVARIANT() );
        User::LeaveIfError( insertSuccess );
        
        // We don't know which index it got, so look it up
        cachedEntry = iCachedRenderBuffers.FindInOrder(newRenderBufferEntry, RenderBufferOrderFunc);

        CleanupStack::Pop(newRenderBufferEntry); 
        }
    else
        {
        // If it was found, great. 
        }        
    
    iCachedRenderBuffers[cachedEntry]->RefreshUser(aUser);

    return iCachedRenderBuffers[cachedEntry]->iCanvasRenderBuffer;                
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHuiCanvasRenderBuffer* CHuiCanvasTextureCache::FindCachedRenderBuffer(const CHuiCanvasVisual& aUser)
    {
    TInt cachedEntry = KErrNotFound;
    
    iSearchedRenderBufferImageEntry->Reset();        
    iSearchedRenderBufferImageEntry->iOwner = (TAny*)&aUser;
    
    cachedEntry = iCachedRenderBuffers.FindInOrder(iSearchedRenderBufferImageEntry, RenderBufferOrderFunc);
    if (cachedEntry != KErrNotFound)
        {
        iCachedRenderBuffers[cachedEntry]->RefreshUser(aUser);
        return iCachedRenderBuffers[cachedEntry]->iCanvasRenderBuffer;                    
        }
    else
        {
        return NULL;                            
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHuiCanvasTextureCache::ReleaseCachedRenderBuffer(const CHuiCanvasVisual& aUser)
    {
    TInt cachedEntry = KErrNotFound;
    
    iSearchedRenderBufferImageEntry->Reset();        
    iSearchedRenderBufferImageEntry->iOwner = (TAny*)&aUser;
    
    cachedEntry = iCachedRenderBuffers.FindInOrder(iSearchedRenderBufferImageEntry, RenderBufferOrderFunc);
    if (cachedEntry != KErrNotFound)
        {
        CHuiCanvasRenderBufferImage* releasedEntry = iCachedRenderBuffers[cachedEntry];        
        iCachedRenderBuffers.Remove(cachedEntry);
        releasedEntry->RemoveUser(aUser);
        releasedEntry->iOwner = releasedEntry; // No real owner anymore, se it self as owner
 
        if(!IsLowMemoryStateEnabled())
            {
            // re-insert released entry (new position in the array order because we clered iOwner)
            TInt insertSuccess = iCachedRenderBuffers.InsertInOrder(releasedEntry, RenderBufferOrderFunc);
        
            if (insertSuccess != KErrNone)
                {
                // oh no re-insert failed, now we must delete the entry to avoid memory leak.
                delete releasedEntry;
                }
            }
        else
            {
            delete releasedEntry;
            }
        
        iHasReleasedRenderBuffers = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHuiCanvasRenderBufferImage* CHuiCanvasTextureCache::ReUseReleasedRenderBufferL(TSize aSizeHint)
    {
    if (aSizeHint == TSize(0,0))
        {
        return NULL;
        }
    
    for(TInt i=iCachedRenderBuffers.Count() - 1; i >= 0; i--)
        {
        // Check if there are any released buffers with have requested size
        if (!iCachedRenderBuffers[i]->IsAnyUser() && 
            iCachedRenderBuffers[i]->iCanvasRenderBuffer && 
            iCachedRenderBuffers[i]->iCanvasRenderBuffer->Size() == aSizeHint)
            {
            // Remove from array and return instance to caller
            CHuiCanvasRenderBufferImage* reusedEntry = iCachedRenderBuffers[i];        
            iCachedRenderBuffers.Remove(i);
            
            if (reusedEntry->iCanvasRenderBuffer && reusedEntry->iCanvasRenderBuffer->IsInitialized())
                {
                reusedEntry->iCanvasRenderBuffer->InitializeL(aSizeHint, ETrue);
                }
            
            return reusedEntry;
            }
        }         

    return NULL;
    }

void CHuiCanvasTextureCache::EnableLowMemoryState(TBool aEnable)
    {
    // deprecated
    }

THuiMemoryLevel CHuiCanvasTextureCache::MemoryLevel()
    {
    return iMemoryLevel;
    }

void CHuiCanvasTextureCache::SetMemoryLevel(THuiMemoryLevel aLevel)
    {
    iMemoryLevel = aLevel;

    if (iMemoryLevel <= EHuiMemoryLevelLowest)
        {
#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
RDebug::Print(_L("-- CHuiCanvasTextureCache::SetMemoryLevel: Enabling lowest memory state"));
#endif

        // Set cache sizes to minimum
        iMaxTextureMemoryInKBytes = 0;
        iMaxRenderBufferMemoryInKBytes = 0;

        // Delete images, even if they are in use
        for(TInt i=iCachedImages.Count() - 1; i >= 0; i--)
            {
            CHuiCanvasGraphicImage* entry = iCachedImages[i];    
            iCachedImages.Remove(i);
            delete entry;
            }    

       // Delete texts, even if they are in use
        for(TInt i=iCachedTexts.Count() - 1; i >= 0; i--)
            {                
            CHuiCanvasTextImage* entry = iCachedTexts[i];    
            iCachedTexts.Remove(i);
            delete entry;
            }        
        
        
        // Delete canvas render buffers, even if they are in use
        for(TInt i=iCachedRenderBuffers.Count() - 1; i >= 0; i--)
            {                
            CHuiCanvasRenderBufferImage* entry = iCachedRenderBuffers[i];    
            iCachedRenderBuffers.Remove(i);
            delete entry;            
            }                    
        }
    else if (iMemoryLevel <= EHuiMemoryLevelLow)
        {
        #ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        RDebug::Print(_L("-- CHuiCanvasTextureCache::SetMemoryLevel: Enabling low memory state"));
        #endif
        // Set cache sizes to minimum
        iMaxTextureMemoryInKBytes = 0;
        iMaxRenderBufferMemoryInKBytes = 0;
        
        // Set flags to make sure we check all entries
        iHasReleasedTexts = ETrue;
        iHasReleasedImages = ETrue;
        iHasReleasedRenderBuffers = ETrue;
        
        // Delete released cached entries
        DeleteAllReleasedEntries(EFalse);

        // Delete canvas render buffers, even if they are in use
        for(TInt i=iCachedRenderBuffers.Count() - 1; i >= 0; i--)
            {                
            CHuiCanvasRenderBufferImage* entry = iCachedRenderBuffers[i];    
            iCachedRenderBuffers.Remove(i);
            delete entry;            
            }                    
        }
    else if(iMemoryLevel <= EHuiMemoryLevelReduced)
        {
        // free all the caches but allow using renderbuffers
        #ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        RDebug::Print(_L("-- CHuiCanvasTextureCache::SetMemoryLevel: Enabling recuded memory state"));
        RDebug::Print(_L("-- CHuiCanvasTextureCache::SetMemoryLevel: Caching of textures disabled but renderbuffers enabled"));
        #endif
        // Set cache sizes to minimum
        iMaxTextureMemoryInKBytes = 0;
        iMaxRenderBufferMemoryInKBytes = 0;
        
        // Set flags to make sure we check all entries
        iHasReleasedTexts = ETrue;
        iHasReleasedImages = ETrue;
        iHasReleasedRenderBuffers = ETrue;
        
        // Delete released cached entries
        DeleteAllReleasedEntries(EFalse);
        }
    else
        {
        #ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        RDebug::Print(_L("-- CHuiCanvasTextureCache::SetMemoryLevel: Disabling low memory state"));
        #endif

        // TODO: This could be configurable cenrep or something ?
        iMaxTextureMemoryInKBytes = KHuiMaxRecommendedTextureAmountInKBytes;
        iMaxRenderBufferMemoryInKBytes = KHuiMaxRecommendedRenderBufferAmountInKBytes;       
        }    

    AMT_MAP_CANVAS_TEXTURE_CACHE_SET_MEMORY_LEVEL();
    }

TBool CHuiCanvasTextureCache::IsLowMemoryStateEnabled() const
    {
    if (iMemoryLevel < EHuiMemoryLevelReduced)
        {
        return ETrue;
        }
    else
        {
        return EFalse;        
        }
    }
