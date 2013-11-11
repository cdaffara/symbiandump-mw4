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
* Description:   Implementation of THuiImage, which specifies an area on 
*                a (segmented) texture object.
*
*/



#include "uiacceltk/HuiImage.h"  // Class definition
#include "uiacceltk/HuiTexture.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiPanic.h"

enum THuiImageInternalFlags
    {
    EHuiImageFlagDefaultTextureType = 0x1        
    };


EXPORT_C THuiImage::THuiImage()
        : iTexture(0), iTl( 0.f, 0.f ), iBr( 0.f, 0.f )
    {
    }


EXPORT_C THuiImage::THuiImage(const MHuiTexture& aTexture,
                              TReal32 aTlX, TReal32 aTlY,
                              TReal32 aBrX, TReal32 aBrY) __SOFTFP
        : iTexture( &aTexture ),
          iTl( aTlX, aTlY ), 
          iBr( aBrX, aBrY ),
          iFlags(0)
    {
    }

EXPORT_C THuiImage::THuiImage(const CHuiTexture& aTexture,
                     TReal32 aTlX, TReal32 aTlY,
                     TReal32 aBrX, TReal32 aBrY) __SOFTFP
        : iTexture( &aTexture ),
          iTl( aTlX, aTlY ), 
          iBr( aBrX, aBrY ),
          iFlags(EHuiImageFlagDefaultTextureType)
    {
    }


EXPORT_C TInt THuiImage::SegmentCount() const
    {
    if(HasTexture())
        {
        return iTexture->SegmentedTexture()->SegmentCount();
        }
    else
        {
        return 0;
        }
    }


EXPORT_C void THuiImage::GetVertexCoords(TInt16* aOutCoords, TInt aSegment) const
    {
    if(!iTexture)
        {
        for(TInt i = 0; i < 8; ++i)
            {
            aOutCoords[i] = 0;
            }
        return;
        }
    
    const MHuiSegmentedTexture* tex = iTexture->SegmentedTexture();
    HUI_ASSERT2(tex, THuiPanic::EImageNotSegmentedTexture);
    
    // Use dummy coordinates, if texture is not available
    if(SegmentCount() == 0)
        {
        for(TInt i = 0; i < 8; ++i)
            {
            aOutCoords[i] = 0;
            }
        }
    else
        {
        // for handling cases where segmentcount >= 1 ..

        // hmm should we use floating point type here?
        // anyway here we have TInt to TInt16 conversions..
        TSize segmentSize = tex->SegmentSize(aSegment);
        TPoint segmentOffset = tex->SegmentOffset(aSegment);
        aOutCoords[0] = segmentOffset.iX;
        aOutCoords[1] = segmentOffset.iY;
        aOutCoords[2] = segmentOffset.iX + segmentSize.iWidth;
        aOutCoords[3] = segmentOffset.iY;
        aOutCoords[4] = aOutCoords[2];
        aOutCoords[5] = segmentOffset.iY + segmentSize.iHeight;
        aOutCoords[6] = aOutCoords[0];
        aOutCoords[7] = aOutCoords[5];
        }
    }


EXPORT_C void THuiImage::GetTexCoords(TReal32* aTexCoordsOut, TInt aSegment) const
    {
    if(!iTexture)
        {
        for(TInt i = 0; i < 8; ++i)
            {
            aTexCoordsOut[i] = 0;
            }
        return;
        }
    
    const MHuiSegmentedTexture* tex = iTexture->SegmentedTexture();
    HUI_ASSERT2(tex, THuiPanic::EImageNotSegmentedTexture);

    // Use dummy coordinates, if texture is not available
    if(!HasTexture())
        {
        for(TInt i = 0; i < 8; ++i)
            {
            aTexCoordsOut[i] = 0;
            }
        return;
        }

    if(tex->SegmentCount() == 1)
        {
        // user-defined custom coordinates.
        aTexCoordsOut[0] = iTl.iX.Now();
        aTexCoordsOut[1] = iTl.iY.Now();
        aTexCoordsOut[2] = iBr.iX.Now();
        aTexCoordsOut[3] = iTl.iY.Now();
        aTexCoordsOut[4] = iBr.iX.Now();
        aTexCoordsOut[5] = iBr.iY.Now();
        aTexCoordsOut[6] = iTl.iX.Now();
        aTexCoordsOut[7] = iBr.iY.Now();

        // NOTE: by default custom coordinates modulate the
        // coordinates calculated from the textures

        // calculate coordinates based on texture
        TSize size = tex->SegmentSize(0);
        TSize textureSize = tex->SegmentTextureSize(0);

        if (textureSize.iWidth)
            {
            // if we have partially filled texture ...
            if (textureSize.iWidth > size.iWidth)
            	{
            	// There has been some visible "ghost lines" and therefore substraction of 0.2 pixels
                aTexCoordsOut[0] = aTexCoordsOut[0] * (size.iWidth - 0.2f) / textureSize.iWidth;
                aTexCoordsOut[2] = aTexCoordsOut[2] * (size.iWidth - 0.2f) / textureSize.iWidth;
                aTexCoordsOut[6] = aTexCoordsOut[0] ;
                aTexCoordsOut[4] = aTexCoordsOut[2] ;

                }
            else
                {
                // 100% of the texture used,
                // just use custom coordinates directly
                }
            }
        if(textureSize.iHeight)
            {
            // if we have partially filled texture ...
            if (textureSize.iHeight > size.iHeight)
                {
                // There has been some visible "ghost lines" and therefore substraction of 0.2 pixels
                aTexCoordsOut[3] = aTexCoordsOut[3] * (size.iHeight - 0.2f) / textureSize.iHeight;
                aTexCoordsOut[1] = aTexCoordsOut[3];
                aTexCoordsOut[5] = aTexCoordsOut[5] * (size.iHeight - 0.2f) / textureSize.iHeight;
                aTexCoordsOut[7] = aTexCoordsOut[5];
                }
            else
                {
                // 100% of the texture used,
                // just use custom coordinates directly
                }
            }
        }
    else
        {
        // calculate coordinates for multiple segments..
        TReal32 leftu; // top u coordinate
        TReal32 rightu; // bottom u coordinate
        TReal32 topv; // top v coordinate
        TReal32 bottomv; // bottom v coordinate

        THuiRealSize  size = tex->Size();
        THuiRealSize  segmentSize = tex->SegmentSize(aSegment);
        THuiRealPoint segmentOffset = tex->SegmentOffset(aSegment);
        // lets do some consistency checks..

        ASSERT(segmentOffset.iX >= 0 && segmentOffset.iX < size.iWidth);
        ASSERT(segmentOffset.iY >= 0 && segmentOffset.iY < size.iHeight);
        ASSERT(segmentSize.iWidth > 0 && segmentSize.iWidth <= size.iWidth);
        ASSERT(segmentSize.iHeight > 0 && segmentSize.iHeight <= size.iHeight);
        // transform offset and size to texture space
        segmentOffset.iX /= size.iWidth;
        segmentOffset.iY /= size.iHeight;
        segmentSize.iWidth /= size.iWidth;
        segmentSize.iHeight /= size.iHeight;

        // calculate coordinates
        // left side..
        TReal32 edge = iTl.iX.Now();
        TReal32 offset = segmentOffset.iX;
        TReal32 width = segmentSize.iWidth;
        TReal32 coord;
        if (edge <= offset)
            {
            // visible texture edge is at the left/top region (outside),
            // so this texture segment is fully visible
            coord = 0.0;
            }
        else if (edge - offset < width)
            {
            // the edge of the visible texture is somewhere
            // inside this block, so texture coordinate has
            // to be set inside
            coord = (edge - offset)/width;
            ASSERT(coord>0.0 && coord<1.0);
            }
        else
            {
            // the edge is outside, at the right/bottom side of this segment.
            coord = 1.0;
            }
        leftu = coord;

        // right side..
        edge = iBr.iX.Now();
        if (edge <= offset)
            {
            coord = 0.0;
            }
        else if (edge - offset < width)
            {
            coord = (edge - offset)/width;
            ASSERT(coord>0.0 && coord<1.0);
            }
        else
            {
            coord = 1.0;
            }
        rightu = coord;

        // top side
        edge = iTl.iY.Now();
        offset = segmentOffset.iY;
        width = segmentSize.iHeight;
        if (edge <= offset)
            {
            coord = 0.0;
            }
        else if (edge - offset < width)
            {
            coord = (edge - offset)/width;
            ASSERT(coord>0.0 && coord<1.0);
            }
        else
            {
            coord = 1.0;
            }
        topv = coord;

        // bottom side
        edge = iBr.iY.Now();
        if (edge <= offset)
            {
            coord = 0.0;
            }
        else if (edge - offset < width)
            {
            coord = (edge - offset)/width;
            ASSERT(coord>0.0 && coord<1.0);
            }
        else
            {
            coord = 1.0;
            }
        bottomv = coord;

        // modulate coordinates (if texture utilization != 100%)
        segmentSize = tex->SegmentSize(aSegment);
        TSize  segmentTexSize = tex->SegmentTextureSize(aSegment);
        TReal32 uscale = (segmentTexSize.iWidth > 0 ?
                        (TReal32(segmentSize.iWidth) / TReal32(segmentTexSize.iWidth))
                        : 1);
        TReal32 vscale = (segmentTexSize.iHeight > 0 ?
                        (TReal32(segmentSize.iHeight) / TReal32(segmentTexSize.iHeight))
                        : 1);

        leftu *= uscale;
        rightu *= uscale;
        topv *= vscale;
        bottomv *= vscale;

        // set output coordinates (width)
        aTexCoordsOut[0] = leftu;    // top-left u
        aTexCoordsOut[2] = rightu;   // top-right u
        aTexCoordsOut[4] = aTexCoordsOut[2]; // bottom-right u
        aTexCoordsOut[6] = aTexCoordsOut[0]; // bottom-left u
        // set output coordinates (height)
        aTexCoordsOut[1] = topv; // top-left v
        aTexCoordsOut[3] = aTexCoordsOut[1]; // top-right v
        aTexCoordsOut[5] = bottomv; // bottom-left v
        aTexCoordsOut[7] = aTexCoordsOut[5]; // bottom-right v

        }
    }


EXPORT_C void THuiImage::SetTexCoords(TReal32 aTlX, TReal32 aTlY,
                                      TReal32 aBrX, TReal32 aBrY,
                                      TInt aTransitionTime) __SOFTFP
    {
    iTl.Set(THuiRealPoint(aTlX, aTlY), aTransitionTime);
    iBr.Set(THuiRealPoint(aBrX, aBrY), aTransitionTime);
    }


EXPORT_C THuiRealPoint THuiImage::TopLeft() const __SOFTFP
    {
    return iTl.RealNow();
    }


EXPORT_C THuiRealPoint THuiImage::BottomRight() const __SOFTFP
    {
    return iBr.RealNow();
    }


EXPORT_C void THuiImage::SetTexture(const MHuiTexture& aTexture)
    {
    HUI_ASSERT2(aTexture.SegmentedTexture(), THuiPanic::EImageNotSegmentedTexture)
        
    iTexture = &aTexture;
    
    // Clear default type flag as we cannot know it for sure
    iFlags &= ~EHuiImageFlagDefaultTextureType;
    }

EXPORT_C void THuiImage::SetTexture(const CHuiTexture& aTexture)
    {
    iTexture = &aTexture;

    // Set default type flag 
    iFlags |= EHuiImageFlagDefaultTextureType;
    }


EXPORT_C TBool THuiImage::HasTexture() const
    {
    return (iTexture && iTexture->SegmentedTexture() && iTexture->HasContent());
    }


EXPORT_C const MHuiTexture& THuiImage::TextureIf() const
    {
    HUI_ASSERT2(iTexture != 0, THuiPanic::EImageNoTexture)

    return *iTexture;
    }


EXPORT_C const MHuiSegmentedTexture& THuiImage::Texture() const
    {
    HUI_ASSERT2(iTexture != 0, THuiPanic::EImageNoTexture)
    HUI_ASSERT2(iTexture->SegmentedTexture() != 0, THuiPanic::EImageNotSegmentedTexture)

    return *iTexture->SegmentedTexture();
    }
  
  
EXPORT_C TBool THuiImage::HasShadow() const
    {
    if(!iTexture || !iTexture->ShadowedTexture())
        {
        return EFalse;
        }
    return iTexture->ShadowedTexture()->IsShadowEnabled();
    }
  
    
EXPORT_C THuiTextureHandle THuiImage::ShadowTexture() const
    {
    THuiTextureHandle handle;
    
    if(iTexture && iTexture->ShadowedTexture())
        {
        iTexture->ShadowedTexture()->GetShadowTexture(handle);
        }
        
    return handle;
    }    


EXPORT_C void THuiImage::Translate(TReal32 aDx, TReal32 aDy, TInt aTransitionTime) __SOFTFP
    {
    iTl.iX.Set(iTl.iX.Target() + aDx, aTransitionTime);
    iTl.iY.Set(iTl.iY.Target() + aDy, aTransitionTime);
    iBr.iX.Set(iBr.iX.Target() + aDx, aTransitionTime);
    iBr.iY.Set(iBr.iY.Target() + aDy, aTransitionTime);
    }


EXPORT_C void THuiImage::Scale(TReal32 aScaleX, TReal32 aScaleY, TInt aTransitionTime) __SOFTFP
    {
    iTl.iX.Set(iTl.iX.Target() * aScaleX, aTransitionTime);
    iTl.iY.Set(iTl.iY.Target() * aScaleY, aTransitionTime);
    iBr.iX.Set(iBr.iX.Target() * aScaleX, aTransitionTime);
    iBr.iY.Set(iBr.iY.Target() * aScaleY, aTransitionTime);
    }


TBool THuiImage::Changed() const
	{
	if(iTexture && iTexture->TextureChanged())
		{
		return ETrue;
		}
		
	return (iTl.Changed() || iBr.Changed());	
	}


void THuiImage::ClearChanged()
	{
	if(iTexture)
		{
		iTexture->TextureClearChanged();
		}	
		
    iTl.ClearChanged();
    iBr.ClearChanged();		
	}	    

const MHuiTexture* THuiImage::ImageTexture() const
    {
    return iTexture;    
    }

const CHuiTexture* THuiImage::ImageDefaultTexture() const
    {
    if (iTexture)
        {            
        // Dynamic cast is slow and this method is called
        // often, hence the optmization.        
        if (iFlags & EHuiImageFlagDefaultTextureType)
            {
            return static_cast<const CHuiTexture*> (iTexture);    
            }
        else
            {
            return dynamic_cast<const CHuiTexture*> (iTexture);                    
            }    
        }
    else
        {
        return NULL;    
        }    
    }
