/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   A Text style encapsulates a unique text style 
*                in Hitchcock UI Toolkit usage.
*
*/



#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/huitextstyle.h"
#include "uiacceltk/huitextstylemanager.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiUtil.h"

#include <AknFontProvider.h>
#include <e32math.h>

void Blur(CFbsBitmap& aImage, TInt aRadius )
    {
    if (aRadius < 1)
        {
        return;
        }
 
    TReal expn = 0.0f;
    Math::Exp(expn, -2.3f/(aRadius+1.f));
    TInt alpha = (TInt)((1<<16)*(1.0f-expn));

    TSize imagesize = aImage.SizeInPixels();
    TInt imageW = imagesize.iWidth;//aImage.SizeInPixels().Width
    TInt imageH = imagesize.iHeight;
    TInt imageP = aImage.ScanLineLength(imageW,EGray256);
    for(TInt row=0;row<imageH;row++) 
        {
        TInt zA;

        TUint8 *ptr = ((TUint8*)(aImage.DataAddress())+(row*imageP));

        zA = (*ptr)<<7;

        for(TInt index=1; index<imageW; index++) 
            {
                {
                TInt A;
                A = ptr[index];
                zA += (alpha * ((A<<7)-zA))>>16;
                ptr[index] = zA>>7;
                }
            }
        for(TInt index=imageW-2; index>=0; index--) 
            {
            TInt A;
            A = ptr[index];
            zA += (alpha * ((A<<7)-zA))>>16;
            ptr[index] = zA>>7;
            }
        }

    for(TInt col=0;col<imageW;col++) 
        {
        TInt zA;

        TUint8 *ptr = (TUint8*)aImage.DataAddress();
        ptr+=col;

        zA = *((TUint8 *)ptr)<<7;
        for(TInt index=imageP; index<(imageH-1)*imageP;index+=imageP) 
            {
            TInt A;
            A = ptr[index];
            zA += (alpha * ((A<<7)-zA))>>16;
            ptr[index] = zA>>7;
            }

        for(TInt index=(imageH-2)*imageP; index>=0;index-=imageP)
            {
            TInt A;
            A = ptr[index];
            zA += (alpha * ((A<<7)-zA))>>16;
            ptr[index] = zA>>7;
            }    		
        }
    }

// ---------------------------------------------------------------------------
// Default constructor. Will create the root (default) text style with
// default parameters.
// ---------------------------------------------------------------------------
//
THuiTextStyle::THuiTextStyle(TInt aFontStyleId,  TInt aParentId)
		: iFontStyleId( aFontStyleId ),
		  iParentId(aParentId),
		  iTextColor(KRgbWhite),
		  iIsUnderline(EFalse),
          iIsStrikeThrough(EFalse),
          iClippingEnabled(EFalse)
    {
    // This is the default style, everything is defined here.
    if(aParentId == -1) 
        {
        iLocalDefinitionFlags = 0xFFFFFFFF;
        }
    // other styles, by default, nothing is defined, everything comes from the parent
    else
        {
        iLocalDefinitionFlags = 0x00000000;
        }
   }
     
// ---------------------------------------------------------------------------
// Copy constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C THuiTextStyle::THuiTextStyle( const THuiTextStyle& aTextStyle )
	{
	iFont = aTextStyle.Font();
	iFontStyleId = aTextStyle.FontStyleId();
    iParentId = aTextStyle.ParentId();
    iId = -1; // Id is not yet set. A unique Id will be set by the text style manager.
    iBackgroundColor = aTextStyle.BackgroundColor();
    iTextColor = aTextStyle.TextColor();
    iIsUnderline = aTextStyle.Underline();
    iIsStrikeThrough = aTextStyle.StrikeThrough();
    iLocalDefinitionFlags = aTextStyle.LocalDefinitionFlags();
    iClippingEnabled = aTextStyle.iClippingEnabled;
    iClipRect = aTextStyle.iClipRect;
	}

// ---------------------------------------------------------------------------
// THuiTextStyle::Id
// ---------------------------------------------------------------------------
//
EXPORT_C TInt THuiTextStyle::Id() const
	{
	return iId;   
	}
	
// ---------------------------------------------------------------------------
// THuiTextStyle::SetId
// ---------------------------------------------------------------------------
//
void THuiTextStyle::SetId(TInt aId)
    {
    iId = aId;
    }
	
// ---------------------------------------------------------------------------
// THuiTextStyle::ParentId
// ---------------------------------------------------------------------------
//
EXPORT_C TInt THuiTextStyle::ParentId() const
	{
	return iParentId;     
	}
	
// ---------------------------------------------------------------------------
// THuiTextStyle::SetParentId
// ---------------------------------------------------------------------------
//
EXPORT_C void THuiTextStyle::SetParentId(TInt aParentId)
	{
	if(iParentId != aParentId)
	    {
	    iParentId = aParentId;
	    }
	
	iParentId = aParentId;   
	}
	
// ---------------------------------------------------------------------------
// THuiTextStyle::LocalDefinitionFlags
// ---------------------------------------------------------------------------
//
TInt THuiTextStyle::LocalDefinitionFlags() const
    {
    return iLocalDefinitionFlags;
    }

// ---------------------------------------------------------------------------
// THuiTextStyle::SetLocalDefinitionFlags
// ---------------------------------------------------------------------------
//
void THuiTextStyle::SetLocalDefinitionFlags(TInt aLocalDefinitionFlags)
    {
    iLocalDefinitionFlags = aLocalDefinitionFlags;
    }

// ---------------------------------------------------------------------------
// THuiTextStyle::Font
// ---------------------------------------------------------------------------
//
EXPORT_C THuiFont& THuiTextStyle::Font()
    {
    if(iLocalDefinitionFlags & KFontDefined)
        {
        return iFont;
        }
    return CHuiStatic::Env().TextStyleManager().TextStyle(iParentId)->Font();
    }
    
// ---------------------------------------------------------------------------
// THuiTextStyle::Font
// ---------------------------------------------------------------------------
//
EXPORT_C const THuiFont& THuiTextStyle::Font() const
    {
    if(iLocalDefinitionFlags & KFontDefined)
        {
        return iFont;
        }
    return CHuiStatic::Env().TextStyleManager().TextStyle(iParentId)->Font();
    }

// ---------------------------------------------------------------------------
// THuiTextStyle::SetFont
// ---------------------------------------------------------------------------
//
EXPORT_C void THuiTextStyle::SetFont(THuiFont aFont)
    {
    iFont = aFont;
    iLocalDefinitionFlags |= KFontDefined;
    }

// ---------------------------------------------------------------------------
// THuiTextStyle::TextColor
// ---------------------------------------------------------------------------
//
EXPORT_C TRgb THuiTextStyle::TextColor() const
	{
    if(iLocalDefinitionFlags & KTextColorDefined)
        {
        return iTextColor;
        }
    return CHuiStatic::Env().TextStyleManager().TextStyle(iParentId)->TextColor();
	}
	
// ---------------------------------------------------------------------------
// THuiTextStyle::SetTextColor
// ---------------------------------------------------------------------------
//
EXPORT_C void THuiTextStyle::SetTextColor(const TRgb& aTextColor)
    {
    iTextColor = aTextColor;
    iLocalDefinitionFlags |= KTextColorDefined;
    }
	
// ---------------------------------------------------------------------------
// THuiTextStyle::BackgroundColor
// ---------------------------------------------------------------------------
//
EXPORT_C TRgb THuiTextStyle::BackgroundColor() const // @todo: not currently implemented, does nothing.
    {
    if(iLocalDefinitionFlags & KBackgroundColorDefined)
        {
        return iBackgroundColor;
        }
    return CHuiStatic::Env().TextStyleManager().TextStyle(iParentId)->BackgroundColor();
    }
	
// ---------------------------------------------------------------------------
// THuiTextStyle::SetBackgroundColor
// ---------------------------------------------------------------------------
//
EXPORT_C void THuiTextStyle::SetBackgroundColor(const TRgb& aBackgroundColor)// @todo: not currently implemented, does nothing.
    {
    iBackgroundColor = aBackgroundColor;
    iLocalDefinitionFlags |= KBackgroundColorDefined;
    }
	
// ---------------------------------------------------------------------------
// THuiTextStyle::TextSizeInTwips
// ---------------------------------------------------------------------------
//
EXPORT_C TInt THuiTextStyle::TextSizeInTwips(TBool /*aIsDecoratedSize*/)
	{
    if(iLocalDefinitionFlags & KTextSizeDefined)
        {
        TFontSpec spec = iFont.FontSpec();
        TInt size = spec.iHeight;
            
        return size;
        }
    return CHuiStatic::Env().TextStyleManager().TextStyle(iParentId)->TextSizeInTwips();
	}
	
// ---------------------------------------------------------------------------
// THuiTextStyle::SetTextSizeInTwips
// ---------------------------------------------------------------------------
//
EXPORT_C void THuiTextStyle::SetTextSizeInTwips(TInt aTextSize, TBool /*aIsDecoratedSize*/)
    {
	TFontSpec spec = iFont.FontSpec();
    
    spec.iHeight = aTextSize;
    iFont.SetFontSpec(spec);
    iLocalDefinitionFlags |= KTextSizeDefined;
    }
	
// ---------------------------------------------------------------------------
// THuiTextStyle::TextSizeInPixels
// ---------------------------------------------------------------------------
//
EXPORT_C TInt THuiTextStyle::TextSizeInPixels(TBool /*aIsDecoratedSize*/)
	{
    if(iLocalDefinitionFlags & KTextSizeDefined)
        {
        TFontSpec spec = iFont.FontSpec();
        CWsScreenDevice* screenDevice = CHuiStatic::ScreenDevice();
        TInt textsizeInPix = screenDevice->VerticalTwipsToPixels(spec.iHeight);
            
        return textsizeInPix;
        }
    return CHuiStatic::Env().TextStyleManager().TextStyle(iParentId)->TextSizeInTwips();
	}
	
// ---------------------------------------------------------------------------
// THuiTextStyle::SetTextSizeInPixels
// ---------------------------------------------------------------------------
//
EXPORT_C void THuiTextStyle::SetTextSizeInPixels(TInt aTextSizeInPix, TBool /*aIsDecoratedSize*/)
    {
    TFontSpec spec = iFont.FontSpec(); 
    CWsScreenDevice* screenDev = CHuiStatic::ScreenDevice();
    
    TInt textsizeInTwips = screenDev->VerticalPixelsToTwips(aTextSizeInPix);
    spec.iHeight = textsizeInTwips;
    iFont.SetFontSpec(spec);
    iLocalDefinitionFlags |= KTextSizeDefined;
    }
	
// ---------------------------------------------------------------------------
// THuiTextStyle::StrokeWeight
// ---------------------------------------------------------------------------
//
EXPORT_C TBool THuiTextStyle::StrokeWeight()
    {
    TFontSpec spec = iFont.FontSpec();

    if(iLocalDefinitionFlags & KStrokeWeightDefined)
        {
        return spec.iFontStyle.StrokeWeight();
        }
    return CHuiStatic::Env().TextStyleManager().TextStyle(iParentId)->StrokeWeight();
    }
	
// ---------------------------------------------------------------------------
// THuiTextStyle::SetStrokeWeight
// ---------------------------------------------------------------------------
//
EXPORT_C void THuiTextStyle::SetStrokeWeight(TBool aIsStrokeWeight)
    {
    TFontSpec spec = iFont.FontSpec();
	spec.iFontStyle.SetStrokeWeight((aIsStrokeWeight ? EStrokeWeightBold : EStrokeWeightNormal));
    iFont.SetFontSpec(spec);
    iLocalDefinitionFlags |= KStrokeWeightDefined;
    }
	
// ---------------------------------------------------------------------------
// THuiTextStyle::Posture
// ---------------------------------------------------------------------------
//
EXPORT_C TBool THuiTextStyle::Posture()
    {
    TFontSpec spec = iFont.FontSpec();

    if(iLocalDefinitionFlags & KPostureDefined)
        {
        return spec.iFontStyle.Posture();
        }
    return CHuiStatic::Env().TextStyleManager().TextStyle(iParentId)->Posture();
    }
	
// ---------------------------------------------------------------------------
// THuiTextStyle::SetPosture
// ---------------------------------------------------------------------------
//
EXPORT_C void THuiTextStyle::SetPosture(TBool aIsItalic)
    {
    TFontSpec spec = iFont.FontSpec();
	spec.iFontStyle.SetPosture((aIsItalic ? EPostureItalic : EPostureUpright));
    iFont.SetFontSpec(spec);
    iLocalDefinitionFlags |= KPostureDefined;
    }
	
// ---------------------------------------------------------------------------
// THuiTextStyle::Underline
// ---------------------------------------------------------------------------
//
EXPORT_C TBool THuiTextStyle::Underline() const
    {
    if(iLocalDefinitionFlags & KUnderlineDefined)
        {
        return iIsUnderline;
        }
    return CHuiStatic::Env().TextStyleManager().TextStyle(iParentId)->Underline();
    }
	
// ---------------------------------------------------------------------------
// THuiTextStyle::SetUnderline
// ---------------------------------------------------------------------------
//
EXPORT_C void THuiTextStyle::SetUnderline(TBool aIsUnderline)
    {
    iIsUnderline = aIsUnderline;
    iLocalDefinitionFlags |= KUnderlineDefined;
    }
	
// ---------------------------------------------------------------------------
// THuiTextStyle::StrikeThrough
// ---------------------------------------------------------------------------
//
EXPORT_C TBool THuiTextStyle::StrikeThrough() const
    {
    if(iLocalDefinitionFlags & KStrikeThroughDefined)
        {
        return iIsStrikeThrough;
        }
    return CHuiStatic::Env().TextStyleManager().TextStyle(iParentId)->StrikeThrough();
    }
	
// ---------------------------------------------------------------------------
// THuiTextStyle::SetStrikeThrough
// ---------------------------------------------------------------------------
//
EXPORT_C void THuiTextStyle::SetStrikeThrough(TBool aIsStrikeThrough)
    {
    iIsStrikeThrough = aIsStrikeThrough;
    iLocalDefinitionFlags |= KStrikeThroughDefined;
    }

// ---------------------------------------------------------------------------
// THuiTextStyle::FontStyleId
// ---------------------------------------------------------------------------
//
EXPORT_C TInt THuiTextStyle::FontStyleId() const
    {
    return iFontStyleId;     
    }

// ---------------------------------------------------------------------------
// THuiTextStyle::GetTypeface
// ---------------------------------------------------------------------------
//
EXPORT_C void THuiTextStyle::GetTypeface( TTypeface& aTypeface ) const
    {
    TFontSpec spec = iFont.FontSpec();
    aTypeface = spec.iTypeface; // Structure copy
    }

// ---------------------------------------------------------------------------
// THuiTextStyle::SetTextPaneHeightInPixels
// ---------------------------------------------------------------------------
EXPORT_C void THuiTextStyle::SetTextPaneHeightInPixels(TInt aTextPaneHeight, TBool /*aIsDecoratedSize*/)
    {
    if (!CCoeEnv::Static())
        return;
    
    TFontSpec oldFontSpec = iFont.FontSpec(); 
    
    // now generate a font with the new text pane height 
    CWsScreenDevice* screenDev = CHuiStatic::ScreenDevice();
    TAknFontSpecification aknFs(iFont.Category(), oldFontSpec, screenDev);
    aknFs.SetTextPaneHeight(aTextPaneHeight);

    // and get its corresponding spec
    TFontSpec newFontSpec;
    TInt foundIndex(KErrNotFound);
    AknFontProvider::GetFontSpecFromMetrics(*screenDev, 0, aknFs, newFontSpec, foundIndex);

    if(foundIndex != KErrNotFound)
        {
        TInt textsizeInTwips = screenDev->VerticalPixelsToTwips(newFontSpec.iHeight);
        newFontSpec.iHeight = textsizeInTwips;
        iFont.SetFontSpec(newFontSpec);
        iLocalDefinitionFlags |= KTextSizeDefined;
        }
    }

// ---------------------------------------------------------------------------
// THuiTextStyle::RasterizeLineL
// Rasterizes the given string on to the target texture using this text style.
// ---------------------------------------------------------------------------
//
void THuiTextStyle::RasterizeLineL(const TDesC& aTextLine, CHuiTexture& aTargetTexture)
	{
	// Calculate the pixel extents of the text line.
	TSize textureSize = LineExtentsL(aTextLine);
	
	TSize maxTextureSize = aTargetTexture.MaxTextureSize();
	
	// With OpenGL renderers we have possibility to segment larger textures
	// so we don't need to limit texture size to maximum texture size
	if(CHuiStatic::Renderer().Id() != EHuiRenderPluginGles10 &&
	   CHuiStatic::Renderer().Id() != EHuiRenderPluginGles11 ) 
        {
        textureSize.iWidth = Min(textureSize.iWidth, maxTextureSize.iWidth);
        textureSize.iHeight = Min(textureSize.iHeight, maxTextureSize.iHeight);
        }
        
    // Verify that max texture dimensions are greater than zero.        
	if((maxTextureSize.iWidth == 0) || (maxTextureSize.iHeight == 0))
	    {
	    // Cannot draw into this tiny texture, so leave.
	    HUI_DEBUG2(_L("CHuiRasterizedTextMesh::RasterizeLineL() - texture size was too small to draw into (%i, %i)."), textureSize.iWidth, textureSize.iHeight);
	    User::Leave(KErrAbort);
	    }
	
	// Create target alpha bitmap to rasterize the text onto.
	CFbsBitmap* alphaBitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(alphaBitmap);
    User::LeaveIfError(alphaBitmap->Create(textureSize, EGray256));
	
	// Create the bitmap device to be used in rasterization.
    CFbsBitmapDevice* device = CFbsBitmapDevice::NewL(alphaBitmap);
	CleanupStack::PushL(device);
	
	// Create drawing context.
	CFbsBitGc* gc = 0;
	User::LeaveIfError(device->CreateContext(gc));
	CleanupStack::PushL(gc);
	
	// Prepare the bitmap for drawing.
	gc->SetBrushColor(KRgbBlack);
	gc->Clear();
	
	// Draw the text onto the bitmap.    
	gc->SetPenColor(KRgbWhite);
		
	// Do underlining
	gc->SetUnderlineStyle(Underline() ? EUnderlineOn : EUnderlineOff);
	// Do strikethrough
	gc->SetStrikethroughStyle(StrikeThrough() ? EStrikethroughOn : EStrikethroughOff);
	
	if(iClippingEnabled)
	{
    	gc->SetClippingRect(iClipRect);
	}
    
	// Rasterize text string using the configured font.
	Font().RasterizeLineL(aTextLine, *gc);
	
	// Destroy graphics context and drawing device.
	CleanupStack::PopAndDestroy(gc);
	CleanupStack::PopAndDestroy(device);
	
	// Create target color bitmap.
	CFbsBitmap* colorBitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(colorBitmap);		
	User::LeaveIfError(colorBitmap->Create(textureSize, EColor64K));
		
	// Lock the color bitmap heap and acquire the pixel data pointers for per-pixel operations.
	colorBitmap->LockHeapLC(ETrue);
	
	// Color bitmap poses EColor64K pixel format.
	TUint32 bitmappitch = CFbsBitmap::ScanLineLength(textureSize.iWidth, EColor64K)>>1;
	
	if(CHuiStatic::Renderer().Id() == EHuiRenderPluginBitgdi)
	    {
        // fill the color bitmap with white -> color modulation works correctly
        Mem::Fill(colorBitmap->DataAddress(), bitmappitch*2*textureSize.iHeight ,0xff);
        
   	    }
	else
	    {
	    // opengl renderer.
	    // Clear the font image bitmap with a solid color. The alpha mask contains the shape of the letters.
	    CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL(colorBitmap);
	    CleanupStack::PushL(dev);
	
	    // Create drawing context for color bitmap.
	    CFbsBitGc* gct = 0;
	    User::LeaveIfError(dev->CreateContext(gct));
	    CleanupStack::PushL(gct);
	
	    // fill the color bitmap with white -> color modulation works correctly
	    gct->SetBrushColor(KRgbWhite);
	    gct->Clear();  
	    
	    // Destroy the color bitmap drawing context.
	    CleanupStack::PopAndDestroy(gct);
	    CleanupStack::PopAndDestroy(dev);                  
	    }
	
	// Upload the rasterized text onto a texture.
	CleanupStack::PopAndDestroy(); // Heaplock
	aTargetTexture.UploadL(*colorBitmap, alphaBitmap, EHuiTextureUploadFlagRetainResolution);

	CleanupStack::PopAndDestroy(); // Color bitmap
	
	// Re-assign the size of the text
	aTargetTexture.SetSize(textureSize);
		
    // Delete target bitmap.
	CleanupStack::PopAndDestroy(alphaBitmap);	
	}

// ---------------------------------------------------------------------------
// THuiTextStyle::LineExtentsL
// Provides dimensions of the rasterization of the given string. 
// ---------------------------------------------------------------------------
//
TSize THuiTextStyle::LineExtentsL(const TDesC& aTextLine)
	{    
	// Retrieve the non-decorated line extents from the used font.
	TSize textSize = Font().LineExtentsL(aTextLine);
	
	// Add the decorations (shadow, etc.) to the line extents.
    if(textSize.iWidth == 0)
        {
        return textSize;
        }
    return textSize;
	}
	
void  THuiTextStyle::EnableClipping(TRect aClipRect)
	{
	iClippingEnabled = ETrue;
	iClipRect = aClipRect;
	}
	
void  THuiTextStyle::DisableClipping()		
	{
	iClippingEnabled = EFalse;
	}
