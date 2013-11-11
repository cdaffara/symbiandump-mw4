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
* Description:   Implementation of CHuiTexture.
*
*/



#include "uiacceltk/HuiTexture.h"  // Class definition
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiPanic.h"
#include "HuiRenderSurface.h"
#include "uiacceltk/HuiProbe.h"

#include "uiacceltk/HuiTextureProcessor.h"

#include <e32math.h>

const TInt KHuiTextureDefaultPriority = 1; // currenly just != 0


// placeholder
EXPORT_C MHuiTexture::~MHuiTexture()
    {
    }

EXPORT_C CHuiTexture* CHuiTexture::NewL(const THuiTextureHandle* aExistingTexture)
    {
    return CHuiStatic::Renderer().CreateTextureL(aExistingTexture);
    }


EXPORT_C CHuiTexture* CHuiTexture::NewLC(const THuiTextureHandle* aExistingTexture)
    {
    CHuiTexture* self = CHuiTexture::NewL(aExistingTexture);
    CleanupStack::PushL(self);
    return self;
    }


EXPORT_C CHuiTexture::CHuiTexture()
    {
    HUI_PROBE_ASSOCIATE_WITH_CURRENT_SESSION
    HUI_PROBE_REPORT_CONSTRUCTED
    }


EXPORT_C void CHuiTexture::BaseConstructL()
    {
    // No name by default.
    SetImageFileNameL(_L(""));

    HUI_DEBUG(_L("CHuiTexture::BaseConstructL() - Auto-appending to texture manager."));

    // Make the texture automatically owned by the texture manager.
    CHuiStatic::Env().TextureManager().AppendTextureL(this);
    iInternalState |= EHuiTextureFlagAddedToTManager;
    
    iPriority  = KHuiTextureDefaultPriority;
    }


EXPORT_C CHuiTexture::~CHuiTexture()
    {
    iContentObservers.Close();

    HUI_DEBUG(_L("CHuiTexture::~CHuiTexture() - Auto-removing from texture manager."));

    // Remove the texture automatically from the texture manager.
    if (iInternalState & EHuiTextureFlagAddedToTManager)
        {
        CHuiStatic::Env().TextureManager().RemoveTexture(*this);
        }

    // NOTE: Do NOT call Reset, or SegSegmentCount(0) from
    // here since they have pure virtual method calls,
    // which won't work anymore :(
    iSegments.Close();
    delete iImageFileName;
    iImageFileName = NULL;
    iPlaceholder = NULL;
    iPreferredSizes.Close();
    
    // delete all created shadows
    ResetShadows();
    iShadows.Close();
    
    HUI_PROBE_REPORT_DESTRUCTED
    }


EXPORT_C MHuiSegmentedTexture* CHuiTexture::SegmentedTexture()
    {
    return this;
    }


EXPORT_C MHuiShadowedTexture* CHuiTexture::ShadowedTexture()
    {
    return this;
    }


EXPORT_C const MHuiSegmentedTexture* CHuiTexture::SegmentedTexture() const
    {
    return this;
    }


EXPORT_C const MHuiShadowedTexture* CHuiTexture::ShadowedTexture() const
    {
    return this;
    }


TBool CHuiTexture::GetShadowTexture(
        THuiTextureHandle& aHandle, 
        TInt aBlurDimension ) const
    {
    for ( TInt i = 0 ; i < iShadows.Count() ; i++ )
        {
        if ( iShadows[i].iBlurDimension == aBlurDimension )
            {
            aHandle = iShadows[i].iShadowTexture->Handle();
            return ETrue;
            }
        }
    return EFalse;
    }
        
void CHuiTexture::CreateShadowTextureL(
        TInt aBlurDimension,
        THuiShadowStyle aStyle )
    {
    // check if the shadow already exists
        {
        THuiTextureHandle dummy;
        if ( GetShadowTexture( dummy, aBlurDimension ) )
            {
            return;
            }   
        }
    
    // shadow not found -> generate one.
    CHuiTexture* newShadowTexture = CHuiTexture::NewL();
    CHuiStatic::Env().TextureManager().RemoveTexture(*newShadowTexture);
    newShadowTexture->iInternalState &= ~EHuiTextureFlagAddedToTManager;
    CleanupStack::PushL( newShadowTexture );
    newShadowTexture->SetSegmentCountL(1);
    
    // blur
    CHuiTextureProcessor& proc = CHuiStatic::Env().TextureManager().Processor();

    THuiTextureHandle shadowHandle = newShadowTexture->Handle();
    switch(aStyle)
        {
        case EHuiTextureShadowStyleIcon:
            proc.BlurSegmentedL(*this, shadowHandle, TSize(Size().iWidth/2, Size().iHeight/2), aBlurDimension,
                CHuiTextureProcessor::EBlurFlagWhite |
                CHuiTextureProcessor::EBlurFlagAlpha |
                CHuiTextureProcessor::EBlurFlagExpandEdges);
            break;

        case EHuiTextureShadowStyleRasterizedText:
            proc.BlurSegmentedL(*this, shadowHandle, TSize(Size().iWidth/*/2*/, Size().iHeight/*/2*/), aBlurDimension,
                CHuiTextureProcessor::EBlurFlagWhite |
                CHuiTextureProcessor::EBlurFlagAlpha |
                CHuiTextureProcessor::EBlurFlagExpandEdges);
            break;

        default:
            break;
        }   
        
    newShadowTexture->SetSegmentSize(0,shadowHandle.iSize );
    newShadowTexture->SetSegmentTextureSize(0,shadowHandle.iTextureSize );    
    
    // create item on the array
    TShadowData newData;
    newData.iShadowTexture = newShadowTexture;
    newData.iBlurDimension = aBlurDimension;
    
    iShadows.AppendL( newData );
    CleanupStack::Pop( newShadowTexture );
    }
    

EXPORT_C void CHuiTexture::EnableShadow(TBool aEnable)
    {
    if(aEnable)
    	{
    	iInternalState |= EHuiTextureFlagShadowEnabled;
    	}
    else
    	{
    	iInternalState &= ~EHuiTextureFlagShadowEnabled;
    	
    	// delete all created shadows
    	ResetShadows();
    	}
    }


EXPORT_C TBool CHuiTexture::IsShadowEnabled() const
    {
    return ((iInternalState & EHuiTextureFlagShadowEnabled) != 0);
    }


EXPORT_C void CHuiTexture::SetShadowStyle(THuiShadowStyle aShadowStyle)
    {
    iShadowStyle = aShadowStyle;
    }


EXPORT_C THuiShadowStyle CHuiTexture::ShadowStyle() const
    {
    return iShadowStyle;
    }
    
EXPORT_C TBool CHuiTexture::GetShadowTexture(THuiTextureHandle& aHandle) const
    {
    // Return the first shadow if generated
    if ( iShadows.Count() )
        {
        aHandle = iShadows[0].iShadowTexture->Handle();
        return ETrue;
        }
    return EFalse;
    }


EXPORT_C void CHuiTexture::SetSegmentCountL(TInt aSegmentCount)
    {
    // delete old segments (if needed)
    TInt count = iSegments.Count();
    while(count > aSegmentCount)
        {
        count--;
        HUI_DEBUG1(_L("CHuiTexture::SetSegmentCount - Deallocating segment %i"), count);
        // Deallocate resources associated with segment
        // NOTE: call to pure virtual method, will not work from the
        // destructor of this class..
        ResetSegment(count);
        iSegments.Remove(count);
        }

    // add new texture segments (if needed)
    while(count < aSegmentCount)
        {
        TSegment seg;
        User::LeaveIfError(iSegments.Append(seg));
        HUI_DEBUG1(_L("CHuiTexture::SetSegmentCount - Creating segment %i"), count);
        // Create initial segment with texture name etc.
        InitSegmentL(count);
        count++;
        }

    }


EXPORT_C TSize CHuiTexture::Size() const
    {
    if(iPlaceholder)
        {
        return iPlaceholder->Size();
        }
    return iSize;
    }


EXPORT_C TInt CHuiTexture::SegmentCount() const
    {
    if(iPlaceholder)
        {
        return iPlaceholder->SegmentCount();
        }
    return iSegments.Count();
    }


EXPORT_C TUint CHuiTexture::SegmentName(TInt aSegment) const
    {
    ASSERT(aSegment < SegmentCount());
    if(iPlaceholder)
        {
        return iPlaceholder->SegmentName(aSegment);
        }

    return iSegments[aSegment].iName;
    }


EXPORT_C TSize CHuiTexture::SegmentSize(TInt aSegment) const
    {
    ASSERT(aSegment < SegmentCount());
    if(iPlaceholder)
        {
        return iPlaceholder->SegmentSize(aSegment);
        }

    return iSegments[aSegment].iSize;
    }


EXPORT_C TSize CHuiTexture::SegmentTextureSize(TInt aSegment) const
    {
    ASSERT(aSegment < SegmentCount());
    if(iPlaceholder)
        {
        return iPlaceholder->SegmentTextureSize(aSegment);
        }

    return iSegments[aSegment].iTextureSize;
    }


EXPORT_C TPoint CHuiTexture::SegmentOffset(TInt aSegment) const
    {
    ASSERT(aSegment < SegmentCount());
    if(iPlaceholder)
        {
        return iPlaceholder->SegmentOffset(aSegment);
        }

    return iSegments[aSegment].iOffset;
    }


EXPORT_C void CHuiTexture::SetSize(const TSize& aSize)
    {
    // if there are multiple
    // segments already defined we have to recalculate (rescale)
    // the logical sizes and offsets for the individual segments
    // to match the new logical size
    if (SegmentCount()>0 && iSize.iWidth > 0 && iSize.iHeight > 0)
        {
        for (TInt s = 0; s < SegmentCount(); s++)
            {
            TSize oldSize(SegmentSize(s));
            TSize newSize(oldSize.iWidth * aSize.iWidth / iSize.iWidth,
                          oldSize.iHeight * aSize.iHeight / iSize.iHeight);
            TPoint oldOffset(SegmentOffset(s));
            TPoint newOffset(oldOffset.iX * aSize.iWidth / iSize.iWidth,
                             oldOffset.iY * aSize.iHeight / iSize.iHeight);
            SetSegmentSize(s, newSize);
            SetSegmentOffset(s, newOffset);
            }
        }

    // assign new logical size
    iSize = aSize;

    }


EXPORT_C void CHuiTexture::SetSegmentName(TInt aSegment, TUint aName)
    {
    ASSERT(aSegment < SegmentCount());
    iSegments[aSegment].iName = aName;
    }


EXPORT_C void CHuiTexture::SetSegmentSize(TInt aSegment, const TSize& aSegmentSize)
    {
    ASSERT(aSegment < SegmentCount());
    iSegments[aSegment].iSize = aSegmentSize;
    }


EXPORT_C void CHuiTexture::SetSegmentTextureSize(TInt aSegment, const TSize& aSegmentTextureSize)
    {
    ASSERT(aSegment < SegmentCount());
    iSegments[aSegment].iTextureSize = aSegmentTextureSize;
    }


EXPORT_C void CHuiTexture::SetSegmentOffset(TInt aSegment, const TPoint& aOffset)
    {
    ASSERT(aSegment < SegmentCount());
    iSegments[aSegment].iOffset = aOffset;
    }


EXPORT_C void CHuiTexture::SetSkinContent(TBool aSkinContent)
    {
    if(aSkinContent)
    	{
    	iInternalState |= EHuiTextureFlagSkinContent;
    	}
    else
    	{
    	iInternalState &= ~EHuiTextureFlagSkinContent;
    	}    
    
    }
    

EXPORT_C TBool CHuiTexture::IsSkinContent() const
    {
    return ((iInternalState & EHuiTextureFlagSkinContent) != 0);    
    }


EXPORT_C void CHuiTexture::Reset()
    {
    EnableShadow(EFalse); // deletes the shadow textures
    RemoveAllSegments();
    iActiveSegment = 0;
    iSize = TSize(0, 0);
    iPlaceholder = NULL;
    // We're not resetting the texture name here.
    // this is intentional since the name may be
    // used to restore the texture content later on
    }

EXPORT_C void CHuiTexture::ResetShadows()
    {
    while ( iShadows.Count() )
        {
        delete iShadows[iShadows.Count()-1].iShadowTexture;
        iShadows.Remove( iShadows.Count()-1);
        }
    }


EXPORT_C const TDesC& CHuiTexture::ImageFileName() const
    {
    return *iImageFileName;
    }


EXPORT_C TBool CHuiTexture::HasContent() const
    {
    return (SegmentCount() > 0);
    }


EXPORT_C TInt CHuiTexture::ClearL(const TSize& aSize, const TRgb& aColor)
    {
    // Always assign new logical size
    SetSize(aSize);

    // if there are no segments, configure them based on the given size!
    ASSERT(SegmentCount() != 0 || (aSize.iWidth > 0 && aSize.iHeight > 0));
    if(SegmentCount() == 0)
        {
        SetupSegmentsL(aSize, aSize);
        }
    // clear all segments..
    for(TInt segment = 0; segment < SegmentCount(); ++segment)
        {
        TRAPD(err, SegmentClearL(segment, EFalse, aColor, 0) );
        if(err == KErrNoMemory)
            {
            HUI_DEBUG2(_L("CHuiTexture::Clear() - Out of memory trying to clear segment %i from \"%S\"."),
                       segment, &ImageFileName());
            return err;
            }
        else if(err != KErrNone)
            {
            HUI_DEBUG1(_L("CHuiTexture::Clear() - SegmentClearL failed with error code %i."), err);
            return err;
            }
        else
            {
            // for PC lint
            }
        }
    // succeeded!
    return KErrNone;
    }


EXPORT_C TInt CHuiTexture::ClearWithAlphaL(const TSize& aSize, const TRgb& aColor,
        TUint8 aAlpha)
    {
    if(Size()==TSize(0,0))
        {
        SetSize(aSize);
        }
    // if there are no segments, configure them based on the given size!
    ASSERT(SegmentCount() != 0 || (aSize.iWidth > 0 && aSize.iHeight > 0));
    if(SegmentCount() == 0)
        {
        SetupSegmentsL(aSize, aSize);
        }
    // clear all segments..
    for (TInt segment = 0; segment < SegmentCount(); ++segment)
        {
        TRAPD(err, SegmentClearL(segment, ETrue, aColor, aAlpha) );
        if(err == KErrNoMemory)
            {
            HUI_DEBUG2(_L("CHuiTexture::ClearWithAlpha() - Out of memory trying to clear segment %i from \"%S\"."),
                       segment, &ImageFileName());
            return err;
            }
        else if(err != KErrNone)
            {
            HUI_DEBUG1(_L("CHuiTexture::ClearWithAlpha() - SegmentClearL failed with error code %i."), err);
            return err;
            }
        else
            {
            // for PC lint
            }
        }
    // succeeded!
    return KErrNone;
    }

/**
 * Set segment data.
 */
EXPORT_C void CHuiTexture::SetSegment(TInt aSegment,
                             const TSize & aSize,
                             const TPoint & aOffset,
                             const TSize & aTextureSize)
    {
    SetSegmentSize(aSegment, aSize);
    SetSegmentOffset(aSegment, aOffset);
    SetSegmentTextureSize(aSegment, aTextureSize);

    // Note: segment name is assigned by the GL specialization class during InitSegment()!
    }

void CHuiTexture::SetPlaceholder(THuiTextureHandle* aPlaceholder)
    {
    iPlaceholder = aPlaceholder;
    }


void CHuiTexture::GenerateBlobL(const TSize& aSize)
    {
    SetSize(aSize);
    // calculate parameters for the segments..
    SetupSegmentsL(aSize, aSize);

    ASSERT(SegmentCount()==1);

    TSize textureSize = SegmentTextureSize(0);
    TUint8* buffer = new (ELeave) TUint8[
                         textureSize.iWidth * textureSize.iHeight * 4];
    CleanupStack::PushL( buffer );

    /** @todo  This is too slow. Just load a pregenerated texture. */

    TUint8* ptr = buffer;
    for(TInt y = 0; y < textureSize.iHeight; y++)
        {
        for(TInt x = 0; x < textureSize.iWidth; x++, ptr += 4)
            {
            ptr[0] = ptr[1] = ptr[2] = 255;

            TReal32 dx = TReal32(x) - textureSize.iWidth/2.f;
            TReal32 dy = TReal32(y) - textureSize.iHeight/2.f;
            TReal32 src = dx * dx + dy * dy;
            TReal trg = 0;

            if(dx != 0 || dy != 0)
                {
                Math::Sqrt(trg, src);
                }

            trg = trg / (textureSize.iWidth/2.0f);
            if(trg < 0)
                {
                trg = 0;
                }
            if(trg > 1)
                {
                trg = 1;
                }

            ptr[3] = (TInt) ((1 - trg) * 255);
            }
        }

    // Upload the solid color buffer.
    SegmentUploadL(0, EHuiTextureFormatRgba8888, textureSize, buffer);

    CleanupStack::PopAndDestroy( buffer );
    }


void CHuiTexture::SetImageFileNameL(const TDesC& aFileName)
    {
    delete iImageFileName;
    iImageFileName = 0;
    iImageFileName = aFileName.AllocL();
    }

    
void CHuiTexture::UploadDirectL(const CFbsBitmap& aBitmap, const CFbsBitmap* aMaskBitmap, THuiTextureUploadFlags aFlags)
	{
	// Setup extension parameters
	CHuiTexture::TExtParams_UploadDirectL params = {aBitmap, aMaskBitmap, aFlags, 0};
    CHuiTexture::TExtParams_UploadDirectL* pparams = &params;
    
    // Call extension
    TextureExtension(KHuiTextureExtUid_UploadDirectL, (TAny**)&pparams); // virtual, deriving classes define the actual implementation 
    
    // Handle return value
    User::LeaveIfError(pparams->iLeaveCode);	
	}
    

void CHuiTexture::RemoveAllSegments()
    {
    // delete segments
    TInt count = iSegments.Count();
    while(count)
        {
        count--;
        HUI_DEBUG1(_L("CHuiTexture::RemoveAllSegments - Removing segment %i"), count);
        // Deallocate resources associated with segment
        // NOTE: call to pure virtual method, will not work from the
        // destructor of this class..
        ResetSegment(count);
        iSegments.Remove(count);
        }
    }


EXPORT_C TBool CHuiTexture::TextureChanged() const
    {
    return iTextureChanged;
    }


EXPORT_C void CHuiTexture::TextureClearChanged() const
    {
    iTextureChanged = EFalse;
    }


EXPORT_C void CHuiTexture::Release()
    {
    HUI_DEBUG2(_L("CHuiTexture::Release() - Resetting texture 0x%x (\"%S\")."),
               this, &ImageFileName());

    // Reset the content of the texture.
    Reset();

    // Notify observers.
    for(TInt i = 0; i < iContentObservers.Count(); ++i)
        {
        iContentObservers[i].TextureContentReleased(*this);
        }
        
    HUI_PROBE_REPORT_RELEASED
    }


EXPORT_C void CHuiTexture::RestoreL()
    {
    // Tell observers to restore the content of this texture.
    if (iContentObservers.Count() > 0)
        {
        for(TInt i = 0; i < iContentObservers.Count(); ++i)
            {
            HUI_DEBUG4(_L("CHuiTexture::RestoreL() - Texture 0x%x requesting restore from content observer 0x%x (%i/%i)."),
                       this, &(iContentObservers[i]), i+1, iContentObservers.Count());
            iContentObservers[i].RestoreTextureContentL(*this);
            }
        HUI_DEBUG1(_L("CHuiTexture::RestoreL() - Texture 0x%x restored!"), this);
        }
    else
        {
        HUI_DEBUG1(_L("CHuiTexture::RestoreL() - WARNING! This Texture (0x%x) has no content observers. Unable to Restore at this point."), this);
        }
        
    HUI_PROBE_REPORT_RESTORED
    }
    
EXPORT_C void CHuiTexture::SetPriority( TInt aPriority )
    {
    iPriority = aPriority;
    }
    
EXPORT_C TInt CHuiTexture::Priority() const
    {
    return iPriority;
    }

HUI_SESSION_OBJECT_IMPL_EXPORT(CHuiTexture, ETypeTexture)

EXPORT_C void CHuiTexture::TextureExtension(const TUid& /*aExtensionUid*/, TAny** /*aExtensionParameters*/)
    {
    
    }

void CHuiTexture::NotifyPreferredSize(const THuiRealSize& aSize)
    {
    // Consider size to be (0,0) if either width or height is zero
    // to avoid unnecessary change notifications. 
    if (HuiUtil::RealCompare(aSize.iHeight,0,0.5) || HuiUtil::RealCompare(aSize.iWidth,0,0.5))
        {
        iPreferredSizes.Append(THuiRealSize(0,0));            
        }
    else
        {
        iPreferredSizes.Append(aSize);        
        }    
    }
    
void CHuiTexture::ResetAutoSizeCalculation()
    {
    iPreferredSizes.Reset();
    iCalculatedPreviousAutoSize = iCalculatedAutoSize;
    }

TBool CHuiTexture::IsAutoSizeCalculationEnabled() const
    {
    return iAutoSizeCalculationEnabled;    
    }

EXPORT_C void CHuiTexture::EnableAutoSizeCalculation(TBool aEnable)
    {
    iAutoSizeCalculationEnabled = aEnable;    
    
    // Initialize everything
    iPreferredSizes.Reset();
    iCalculatedPreviousAutoSize = THuiRealSize(0,0);
    iCalculatedAutoSize = THuiRealSize(0,0);
    
    // Make sure texturemanager is aware of me
    if (aEnable)
        {
        CHuiStatic::Env().TextureManager().EnableAutoSizeCalculation(ETrue);        
        }    
    }

TBool CHuiTexture::CalculatedAutoSizeChanged() const
    {
    if (!IsAutoSizeCalculationEnabled() || 
        (HUI_ROUND_FLOAT_TO_INT(iCalculatedAutoSize.iWidth) == HUI_ROUND_FLOAT_TO_INT(iCalculatedPreviousAutoSize.iWidth) &&
         HUI_ROUND_FLOAT_TO_INT(iCalculatedAutoSize.iHeight) == HUI_ROUND_FLOAT_TO_INT(iCalculatedPreviousAutoSize.iHeight)))
        {
        return EFalse;    
        }
    else
        {
        return ETrue;                    
        }    
    }

THuiRealSize CHuiTexture::CalculateAutoSize()
    {
    // If only one size, retun it.
    if (iPreferredSizes.Count() == 1)
        {
        iCalculatedAutoSize = iPreferredSizes[0];                
        }
    else
        {
        // If more sizes reported, then try to report largest one.
        if (iPreferredSizes.Count())
            {
            iCalculatedAutoSize = iPreferredSizes[0];
            TInt autoSizeArea = iCalculatedAutoSize.iWidth * iCalculatedAutoSize.iHeight;
            for (TInt i=0; i<iPreferredSizes.Count();i++)
                {
                if ((iPreferredSizes[i].iWidth * iPreferredSizes[i].iHeight) > autoSizeArea)
                    {
                    iCalculatedAutoSize = iPreferredSizes[i];
                    autoSizeArea = iPreferredSizes[i].iWidth * iPreferredSizes[i].iHeight;                        
                    }                
                }
            }                            
        }    
            
    return iCalculatedAutoSize;    
    }


EXPORT_C void CHuiTexture::SetAutoSizeParams(const THuiTextureAutoSizeParams& aParams)
    {
    iAutoSizeParams = aParams;
    }
    
EXPORT_C THuiTextureAutoSizeParams CHuiTexture::AutoSizeParams() const
    {
    return iAutoSizeParams;        
    }

EXPORT_C void CHuiTexture::SetTextureChanged(TBool aChanged)
    {
    if (aChanged && !iTextureChanged)
        {
        CHuiEnv& env = CHuiStatic::Env();
        env.TextureManager().SetHasChangedTextures();
        env.ContinueRefresh();        
        }
    
    iTextureChanged = aChanged;   
    }

EXPORT_C void CHuiTexture::SetNvgContent(TBool aNvgContent)
    {
    if(aNvgContent)
        {
        iInternalState |= EHuiTextureFlagNvgContent;
        }
    else
        {
        iInternalState &= ~EHuiTextureFlagNvgContent;
        }
    }

EXPORT_C TBool CHuiTexture::IsNvgContent() const
    {
    return ((iInternalState & EHuiTextureFlagNvgContent) != 0);
    }


// End of file
