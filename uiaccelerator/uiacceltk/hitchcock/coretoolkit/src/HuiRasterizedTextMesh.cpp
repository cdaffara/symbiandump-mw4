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
* Description:   Implementation of CHuiRasterizedTextMesh. CHuiRasterizedTextMesh 
*                stores a rasterized bitmap version of a text string.
*
*/


#include <s32mem.h>
#include "HuiRasterizedTextMesh.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiSkin.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiFont.h"
#include "uiacceltk/HuiTexture.h"
#include "uiacceltk/HuiGc.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiPanic.h"

#include "uiacceltk/huitextstylemanager.h" // @todo remove when text drawing is moved to THuiFont
#include "uiacceltk/huitextstyle.h" // @todo remove when text drawing is moved to THuiFont
#include "uiacceltk/huidropshadow.h"
#include "uiacceltk/HuiTextureProcessor.h"

#include <AknBidiTextUtils.h>
#include <AknPictographInterface.h>
#include <AknPictographDrawerInterface.h>


/** Granularity of line wrapping array. */
const TInt KLineArrayGranularity = 4;

enum THuiRasterizeLevel
    {
    ERasterizeNone = 0x0,
    ERasterizeText = 0x1,           
    ERasterizePictographs = 0x2,           
    ERasterizeAll = 0xFFFFFFFF
    };

EXPORT_C CHuiRasterizedTextMesh* CHuiRasterizedTextMesh::NewL()
    {
    CHuiRasterizedTextMesh* self = CHuiRasterizedTextMesh::NewLC();
    CleanupStack::Pop(self);
    return self;
    }


CHuiRasterizedTextMesh* CHuiRasterizedTextMesh::NewLC()
    {
    CHuiRasterizedTextMesh* self = new (ELeave) CHuiRasterizedTextMesh();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


CHuiRasterizedTextMesh::CHuiRasterizedTextMesh()
    {
    }

CHuiRasterizedTextMesh::~CHuiRasterizedTextMesh()
    {
    ResetLines();
    iLines.Close();
    ResetPictographLines();
    iPictographLines.Close();
    delete iPictographBitmap;
    }


void CHuiRasterizedTextMesh::Reset()
    {
    if (!iUsingPreRasterizedMesh)
        {
        CHuiTextMesh::Reset();
        ResetLines();
        ResetPictographLines();
        }
    }

void CHuiRasterizedTextMesh::ResetLines()
    {
    HUI_DEBUG(_L("CHuiRasterizedTextMesh::ResetLines() - Deleting textures for all rasterized lines."));

    for(TInt i = 0; i < iLines.Count(); ++i)
        {
        if (!iUsingPreRasterizedMesh) 
            {
            delete iLines[i].iTexture;
            }
        iLines[i].iTexture = NULL;
        }

    iLines.Reset();
    }

void CHuiRasterizedTextMesh::ResetPictographLines()
    {
    HUI_DEBUG(_L("CHuiRasterizedTextMesh::ResetPictographLines() - Deleting textures for all rasterized lines."));

    for(TInt i = 0; i < iPictographLines.Count(); ++i)
        {
        delete iPictographLines[i].iTexture; iPictographLines[i].iTexture = NULL;
        }

    iPictographLines.Reset();
    }


TBool CHuiRasterizedTextMesh::IsMaxLineCountReached() const
    {
    return iLines.Count() >= MaxLineCount();
    }


TBool CHuiRasterizedTextMesh::RasterizeLineL(const TDesC& aTextLine, SRasterizedLine & aLineOut)
    {
    if(iUsingPreRasterizedMesh)
        {
        return ETrue;
        }
    // Retrieve the used text style.
    THuiTextStyle* textStyle = CHuiStatic::Env().TextStyleManager().TextStyle(iTextStyleId);
    
    // Calculate line extents and assign it to texture size.
    TSize textureSize = textStyle->LineExtentsL(aTextLine);
    
    if(textureSize.iWidth == 0)
        {
        // This is an empty string. We will not rasterize it.
        // Just add a gap.
        aLineOut.iTexture = NULL;
        aLineOut.iGap = textureSize.iHeight; // @todo: refacture/rename iGap? iGap is used as a size of an empty line?

        HUI_DEBUG1(_L("CHuiRasterizedTextMesh::RasterizeLineL() - Added line gap: %i"),
                   aLineOut.iGap);

        return !IsMaxLineCountReached();
        }

    // Create a texture for storing the text into.
    if (aLineOut.iTexture == NULL)
        {
        HUI_DEBUG1(_L("CHuiRasterizedTextMesh::RasterizeLineL() - Registering self (0x%x) as a texture content observer."), this);
        aLineOut.iTexture = CHuiTexture::NewL();
        // Register one content observer for the first texture that
        // is able to restore all lines in a single run
        if (iLines.Count()==1)
            {
            aLineOut.iTexture->iContentObservers.AppendL(*this);
            }
        aLineOut.iGap = 0;
        }

    // set a name for the texture
    aLineOut.iTexture->SetImageFileNameL(aTextLine);
    
    // Rasterize string using the defined text style.
    textStyle->RasterizeLineL(aTextLine, *aLineOut.iTexture);

    if ( RasterizedShadow() && aLineOut.iTexture )
        {
        const TInt requestedBlurredSize = HUI_ROUND_FLOAT_TO_INT( 2*iVisual->DropShadowHandler()->iRadius.Now() );
        aLineOut.iTexture->CreateShadowTextureL( requestedBlurredSize, EHuiTextureShadowStyleRasterizedText );
        }


    return !IsMaxLineCountReached();
    }

TBool CHuiRasterizedTextMesh::RasterizePictographLineL(const TDesC& aTextLine, CFont* aFont, SRasterizedLine & aLineOut)
    {
    if(iUsingPreRasterizedMesh)
        {
        return EFalse;
        }

    // Retrieve the used text style.
    THuiTextStyle* textStyle = CHuiStatic::Env().TextStyleManager().TextStyle(iTextStyleId);
    
    // Calculate line extents and assign it to texture size.
    TSize textureSize = textStyle->LineExtentsL(aTextLine);
	    
    if(textureSize.iWidth == 0 || !iPictographInterface || !iPictographInterface->Interface()->ContainsPictographs(aTextLine))
        {
        // This is an empty string or it does not contain pictographs. We will not rasterize it.
        // Just add a gap.
        aLineOut.iTexture = NULL;
        aLineOut.iGap = textureSize.iHeight;
        return !IsMaxLineCountReached(); 
        }

    // store the actual size to be assigned as the textures logical size
    TSize actualsize(textureSize);

    if (aLineOut.iTexture == NULL)
        {
    // Create a texture for storing the pictographs into.
        aLineOut.iTexture = CHuiTexture::NewL();
        HUI_DEBUG1(_L("CHuiRasterizedTextMesh::RasterizePictographLineL() - Registering self (0x%x) as a texture content observer."), this);        
        // Register one content observer for the first texture that
        // is able to restore all lines in a single run
        if (iLines.Count()==1)
            {
            aLineOut.iTexture->iContentObservers.AppendL(*this);
            }
        aLineOut.iGap = 0;
        }

    // set a name for the texture
    // @todo is this needed, what names to use
    aLineOut.iTexture->SetImageFileNameL(_L("Pictographs"));

    TSize maxTextureSize = aLineOut.iTexture->MaxTextureSize();
    textureSize.iWidth = Min(textureSize.iWidth, maxTextureSize.iWidth);
    textureSize.iHeight = Min(textureSize.iHeight, maxTextureSize.iHeight);

    if((textureSize.iWidth == 0) || (textureSize.iHeight == 0))
        {
        // Cannot draw into this tiny texture, so leave.
        HUI_DEBUG2(_L("CHuiRasterizedTextMesh::RasterizePictographLineL() - texture size was too small to draw into (%i, %i)."), textureSize.iWidth, textureSize.iHeight);
        User::Leave(KErrAbort);
        }

    User::LeaveIfError( iPictographBitmap->Resize(textureSize) );

    CFbsBitmapDevice* device = CFbsBitmapDevice::NewL(iPictographBitmap);
    CleanupStack::PushL(device);

    CFbsBitGc* gc = 0;
    User::LeaveIfError( device->CreateContext(gc) );
    CleanupStack::PushL(gc);

    // Prepare the bitmap for drawing...set drawmode because of EColor16MA mode...
    gc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha); 

    TRgb color = KRgbWhite;
    color.SetAlpha(0x00);
    gc->SetBrushColor(color);
    gc->Clear();
    gc->UseFont(aFont);  
    
	// Draw pictorgraphs
    iPictographInterface->Interface()->DrawPictographsInText(
            *gc,
            *aFont,
            aTextLine, TPoint(0, aFont->FontMaxAscent()));

    CleanupStack::PopAndDestroy(gc);
    CleanupStack::PopAndDestroy(device);

    aLineOut.iTexture->UploadL(*iPictographBitmap, NULL, EHuiTextureUploadFlagRetainResolution);
    aLineOut.iTexture->SetSize(actualsize);
    return !IsMaxLineCountReached();
    }

void CHuiRasterizedTextMesh::BuildL(TBool aRasterize)
    {
    if (aRasterize)
        {
        DoBuildL(ERasterizeAll);    
        }
    else
        {
        DoBuildL(ERasterizeNone);                
        }    
    }

void CHuiRasterizedTextMesh::DoBuildL(TInt aRasterizeFlags)
    {
    if(iUsingPreRasterizedMesh)
        {
        return;
        }

    TSize extents(0, 0);
    
    HUI_DEBUG(_L("CHuiRasterizedTextMesh::BuildL() - Updating rasterized text."));

    // This is never NULL during BuildL().
    const TDesC& text = *Text();
    
    // Retrieve the text style used when rasterizing this text mesh.
    THuiTextStyle* textStyle = CHuiStatic::Env().TextStyleManager().TextStyle(iTextStyleId);

    // Retrieve the CFont object used when rasterizing this text mesh.
    CFont* font = textStyle->Font().NearestFontL(iTextMeshScale);

    // Maximum width of a text line in pixels.
    TInt maxWidth = MaxLineWidth();

    TInt startIndex = 0;
    TInt index = 0;
    TInt lineCount = 0;

    CArrayFixFlat<TPtrC>* linePtrs = new (ELeave) CArrayFixFlat<TPtrC>(KLineArrayGranularity);
    CleanupStack::PushL(linePtrs);

    while(startIndex < text.Length())
        {
        /// @todo What is the Symbian way to determine line break chars?
#define HUI_IS_LINE_BREAK(aChar) (aChar == '\n')

        // Find the next logical line.
        while(index < text.Length() && !HUI_IS_LINE_BREAK(text[index]))
            {
            index++;
            }

        TPtrC logicalLine = text.Mid(startIndex, index - startIndex);
        ++index; // Skip the line break.
        startIndex = index;

        switch(LineMode())
            {
            case ELineModeTruncate:
                {
                ++lineCount; // there's always one line created per logical line
                HBufC* buf = logicalLine.AllocLC();
                TPtr ptr = buf->Des();
                // truncate line
                CHuiStatic::ConvertToVisualAndClipL(ptr, *font, maxWidth, maxWidth);
                // create the line entry if not already existing
                
                if (aRasterizeFlags != ERasterizeNone)
                	{
                    if (iLines.Count() < lineCount)
                        {
                        SRasterizedLine line;
                        line.iTexture = NULL;
                        line.iGap = 0;
                        iLines.AppendL(line);
                        
                        if (iPictographInterface)
                            {
                            SRasterizedLine pictographline;
                            pictographline.iTexture = NULL;
                            pictographline.iGap = 0;
                            iPictographLines.AppendL(pictographline);                                
                            }
                        }                	
                        
                	TInt currentLine = lineCount-1;
    	            if (aRasterizeFlags & ERasterizeText)
    	                {
    	                // rasterize a single line (updates texture in iLines[0].iTexture)
        	            RasterizeLineL(ptr, iLines[currentLine]);	                    
    	                }

    	            if (aRasterizeFlags & ERasterizePictographs && iPictographInterface)
    	                {
                        // Rasterize pictographs if needed
        	            RasterizePictographLineL(ptr, font, iPictographLines[currentLine]);
    	                }

    	            // Get extents from the texture we just created
                    CHuiTexture* tex = iLines[currentLine].iTexture;
                    extents.iHeight += iLines[currentLine].iGap;
                    if(tex)
                        {
                        extents.iWidth = Max(extents.iWidth, tex->Size().iWidth);
                        extents.iHeight += tex->Size().iHeight;
                        }	                
                	}
                else
                    {
                    // Don't rasterise or create textures, just get the extents of this text.
                    TSize lineExtents = textStyle->LineExtentsL(ptr);
                    extents.iWidth = Max(extents.iWidth, lineExtents.iWidth);
                    extents.iHeight += lineExtents.iHeight;                    
                    }

                	
        	    CleanupStack::PopAndDestroy(buf);
                break;
                }

            case ELineModeWrap:
                {
                // wrap lines to array
                HBufC* buf = CHuiStatic::ConvertToVisualAndWrapToArrayL(
                    logicalLine, maxWidth, *font, *linePtrs);
                CleanupStack::PushL(buf);

                // one line may create several wrapped lines
                lineCount += linePtrs->Count();

                if (aRasterizeFlags != ERasterizeNone)
                  	{    

                    // create new entries..
                    while (iLines.Count() < lineCount)
                        {
                        SRasterizedLine line;
                        line.iTexture = NULL;
                        line.iGap = 0;
                        iLines.AppendL(line);

                        if (iPictographInterface)
                            {
                            SRasterizedLine pictographline;
                            pictographline.iTexture = NULL;
                            pictographline.iGap = 0;
                            iPictographLines.AppendL(pictographline);                                
                            }
                        }
                    
                   	// Do rasterisation if we want to render to texture.
                    for(TInt i = 0; i < linePtrs->Count(); ++i)
                        {
                        TInt currentLine = (lineCount - linePtrs->Count()) + i;   
          	   	                
    	                if (aRasterizeFlags & ERasterizeText)
    	                    {
                            // rasterize a single line (updates texture in iLines[i].iTexture)
                            RasterizeLineL(linePtrs->At(i), iLines[currentLine]);
    	                    }
    	                    
	                    if (aRasterizeFlags & ERasterizePictographs && iPictographInterface)
	                        {
                            // Rasterize pictographs if needed
                            RasterizePictographLineL(linePtrs->At(i), font, iPictographLines[currentLine]);                                                                 	                            
	                        }
           	                // Get extents from the texture we just created
                            CHuiTexture* tex = iLines[i].iTexture;
                            extents.iHeight += iLines[i].iGap;
                            
                            if(tex)
                                {
                                extents.iWidth = Max(extents.iWidth, tex->Size().iWidth);
                                extents.iHeight += tex->Size().iHeight;
                                }    
                            	                                                         
    	                TBool moreAvailable = (currentLine + 1 < MaxLineCount());
                        if (!moreAvailable)
          	                {
       	                    // Maximum number of lines reached.
       	                    break;
       	                    }
                        }
                  	}
   	            else
   	                {
                    // Don't rasterise or create textures, just get the extents of this text.   	                    
                    for(TInt i = 0; i < linePtrs->Count(); ++i)
                        {  
                        TSize lineExtents = textStyle->LineExtentsL(linePtrs->At(i));
                        extents.iWidth = Max(extents.iWidth, lineExtents.iWidth);
                        extents.iHeight += lineExtents.iHeight;                                                              
                        }
       	            }
                	
                linePtrs->Reset();
                CleanupStack::PopAndDestroy(buf);
                break;
                }

            default:
                break;
            }

        // If we have reached the maximum number of lines, stop building.
        if(IsMaxLineCountReached())
            {
            break;
            }
        }

    HUI_DEBUG(_L("CHuiRasterizedTextMesh::BuildL() - Finished rasterizing text."));

    CleanupStack::PopAndDestroy(linePtrs); linePtrs = 0;

    if (iPictographBitmap)
        {
        iPictographBitmap->Resize(TSize(0, 0));
        }

    HUI_DEBUG(_L("CHuiRasterizedTextMesh::BuildL() - Updating text extents.."));
    // The extents of the mesh depend on how many lines there are.
    SetExtents(extents);

    HUI_DEBUG(_L("CHuiRasterizedTextMesh::BuildL() - Done!"));

    }

void CHuiRasterizedTextMesh::ExpandRectWithShadow(TRect& aRect) const
    {
    if ( iVisual && iLines.Count() )
        {
        CHuiDropShadow* shadowHandler = iVisual->DropShadowHandler();
        if ( shadowHandler &&
             shadowHandler->IsShadowVisible() &&
             iLines[0].iTexture )
            {
            const TInt requestedBlurredSize = HUI_ROUND_FLOAT_TO_INT( 2*shadowHandler->iRadius.Now() );
            THuiTextureHandle shadow;
            // take the first line as an example
            TBool haveShadowTexture = iLines[0].iTexture->GetShadowTexture( shadow,requestedBlurredSize );
            
            if ( haveShadowTexture )
                {
                const TRect shadowRect = shadowHandler->ShadowDrawingTRect( 
                        aRect.iTl,
                        aRect.Size(),
                        shadow.Size(),
                        *iVisual );           
                           
                aRect.BoundingRect( shadowRect );
                }
            }
        }
    }
    

void CHuiRasterizedTextMesh::Draw(CHuiGc& aGc, TReal32 aShadowOpacity) const __SOFTFP
    {
    THuiAlignHorizontal oldHorizAlign = aGc.AlignHorizontal();
    THuiAlignVertical oldVertAlign = aGc.AlignVertical();

    // Because we are using DrawImage, which respects Gc alignments, and the
    // context has already set up the appropriate alignment offset, we must
    // disable the alignment temporarily.
    aGc.SetAlign(EHuiAlignHLeft, EHuiAlignVTop);


    // The actual text.
    DrawLines(aGc, THuiRealPoint(0.f, 0.f), oldHorizAlign, aShadowOpacity);

    if(!iUsingPreRasterizedMesh)
        {
        // Pictographs, if needed.
        DrawPictographLines(aGc, THuiRealPoint(0.f, 0.f), oldHorizAlign);
        }
    
    aGc.SetAlign(oldHorizAlign, oldVertAlign);
    }


void CHuiRasterizedTextMesh::DrawLines(CHuiGc& aGc, const THuiRealPoint& aOffset,
                                       THuiAlignHorizontal aLineAlignment, TReal32 aShadowOpacity) const
    {    
    TInt y = 0;

    // Draw the built lines using THuiImages.
    for(TInt i = 0; i < iLines.Count(); ++i)
        {
        const SRasterizedLine& line = iLines[i];
        if(line.iTexture)
            {

            THuiImage textImage(*line.iTexture);
            THuiRealPoint linePos(0.f, TReal32(y));
            THuiRealSize lineSize = line.iTexture->Size();
                   
            // Do a downward scaling for line texture from TV resolution to LCD resolution.
            if(iTextMeshScale != 1)
                {
                lineSize.iHeight = lineSize.iHeight/iTextMeshScale;
                lineSize.iWidth = lineSize.iWidth/iTextMeshScale;
                }
            
            // Choose the line-specific alignment.
            switch(aLineAlignment)
                {
                case EHuiAlignHRight:
                    linePos.iX = Extents().iWidth - lineSize.iWidth;
                    break;

                case EHuiAlignHCenter:
                    linePos.iX = (Extents().iWidth - lineSize.iWidth) / 2;
                    break;

                default:
                    break;
                }

            
            // Is there a shadow?
            if ( RasterizedShadow() )
                {
                const TInt requestedBlurredSize = HUI_ROUND_FLOAT_TO_INT( 2*iVisual->DropShadowHandler()->iRadius.Now() );
                THuiTextureHandle shadow;
                TBool haveShadowTexture = line.iTexture->GetShadowTexture(shadow,requestedBlurredSize );
                
                if ( haveShadowTexture )
                    {
                    THuiImage shadowImage(shadow);
                    const THuiRealRect shadowDrawingRect = iVisual->DropShadowHandler()->ShadowDrawingRealRect( 
                        linePos,
                        lineSize,
                        shadow.Size(),
                        *iVisual );
                    
                    const TRgb oldColor = aGc.PenColorAlpha();
                    aGc.SetPenColor(iVisual->DropShadowHandler()->Color());
                    aGc.SetPenAlpha(HUI_ROUND_FLOAT_TO_INT(aShadowOpacity * 255.0f));
                    
                    const THuiQuality oldQuality = aGc.Quality();
                    aGc.SetQuality(EHuiQualityFast);
                    
                    aGc.DrawImage(shadowImage, shadowDrawingRect.iTl + aOffset, shadowDrawingRect.Size());
                    
                    aGc.SetPenColorAlpha(oldColor);
                    aGc.SetQuality(oldQuality);
                    }
                }
                
            aGc.DrawImage(textImage, linePos + aOffset, lineSize);

            // Move one line downwards.
            y += TInt(lineSize.iHeight) + line.iGap;
            }

        // Move extra gap downwards.
        y += line.iGap;
        
        // Add line spacing.
        y += iLineSpacing;
        }
    }

void CHuiRasterizedTextMesh::DrawPictographLines(CHuiGc& aGc, const THuiRealPoint& aOffset,
                                       THuiAlignHorizontal aLineAlignment) const
    {
    if (!iPictographInterface || iUsingPreRasterizedMesh)
        {
        return;    
        }
    
    
    TInt y = 0;

    // Draw the built lines using THuiImages.
    for(TInt i = 0; i < iPictographLines.Count(); ++i)
        {
        const SRasterizedLine& line = iPictographLines[i];
        if(line.iTexture)
            {

            THuiImage textImage(*line.iTexture);
            THuiRealPoint linePos(0.f, TReal32(y));
            THuiRealSize lineSize = line.iTexture->Size();

            // Choose the line-specific alignment.
            switch(aLineAlignment)
                {
                case EHuiAlignHRight:
                    linePos.iX = Extents().iWidth - lineSize.iWidth;
                    break;

                case EHuiAlignHCenter:
                    linePos.iX = (Extents().iWidth - lineSize.iWidth) / 2;
                    break;

                default:
                    break;
                }

            aGc.SetPenColor(KRgbWhite);
            aGc.DrawImage(textImage, linePos + aOffset, lineSize);

            // Move one line downwards.
            y += TInt(lineSize.iHeight) + line.iGap;
            }

        // Move extra gap downwards.
        y += line.iGap;
        
        // Add line spacing.
        y += iLineSpacing;
        }
    }


void CHuiRasterizedTextMesh::TextureContentUploaded(CHuiTexture& /*aTexture*/)
    {
    }


void CHuiRasterizedTextMesh::TextureContentReleased(CHuiTexture& /*aTexture*/)
    {
    }


void CHuiRasterizedTextMesh::RestoreTextureContentL(CHuiTexture& /*aTexture*/)
    {
    // We only get one of these, so let's rebuild the text mesh.
    HUI_DEBUG(_L("CHuiRasterizedTextMesh::RestoreTextureContentL() - Rebuilding text."));
    
    // We want to render the mesh so pass true.
    BuildL(ETrue);
    }
void CHuiRasterizedTextMesh::InitPictographsL(CAknPictographInterface* aInterface)
    {
    if(!iUsingPreRasterizedMesh)
        {
        iPictographInterface = aInterface;
        delete iPictographBitmap;
        iPictographBitmap = NULL;
        iPictographBitmap = new (ELeave) CFbsBitmap();    
        User::LeaveIfError( iPictographBitmap->Create(TSize(0, 0), EColor16MA) );        
        }
    }

void CHuiRasterizedTextMesh::BuildPictographsL()
    {
    if(!iUsingPreRasterizedMesh)
        {
        DoBuildL(ERasterizePictographs);
        }
    }

void  CHuiRasterizedTextMesh::UpdateMeshL(const TDesC8& aBuffer)
    {
    iUsingPreRasterizedMesh = ETrue;
    ResetLines();
    RDesReadStream stream(aBuffer);
    TInt count = stream.ReadInt32L();
    for (TInt i=count-1;i>=0;i--)
        {
        // lines are in reverse order
        SRasterizedLine line;
        line.iTexture = dynamic_cast<CHuiTexture*>((MHuiTexture*)stream.ReadInt32L()); //scary
        line.iGap = stream.ReadInt32L();
        iLines.InsertL(line, 0);
        }
    TSize extents;
    extents.iWidth = stream.ReadInt32L();
    extents.iHeight = stream.ReadInt32L();
    SetExtents(extents);
    stream.Close();
    
    if (RasterizedShadow()) // update shadow
        {
        for (TInt i = iLines.Count()-1; i >=0; i-- )
            {
            if (iLines[i].iTexture)
                {
                const TInt requestedBlurredSize = HUI_ROUND_FLOAT_TO_INT( 2*iVisual->DropShadowHandler()->iRadius.Now() );
                iLines[i].iTexture->CreateShadowTextureL( requestedBlurredSize, EHuiTextureShadowStyleRasterizedText );
                }
            }
        }
    }

