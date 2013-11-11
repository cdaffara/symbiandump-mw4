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
* Description:   Implementation for CHuiGc (graphics context) class.
*
*/



#include "uiacceltk/HuiGc.h"  // Class definition
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiSegmentedTexture.h"
#include "uiacceltk/HuiTextMesh.h"
#include "uiacceltk/HuiFont.h"
#include "uiacceltk/HuiImage.h"
#include "uiacceltk/HuiTransformation.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiUtil.h"  // For hui assert.
#include "uiacceltk/HuiTransformation.h"

#include "huiextension.h"

#include <e32math.h>
#include <AknUtils.h>

const TInt KHuiMaxRecycledRegionCount = 20;

/** Default offset from the Z=0 plane (which 2D projection uses) to the
    eyepoint, in 3D projection mode. */
LOCAL_D const TReal32 KDefaultOffset2D = 1199.0;


EXPORT_C CHuiGc::CHuiGc()
        : iOrientation(EOrientationNormal),
          iNearClipDistance(20.f),
          iFarClipDistance(10000.f),
          iFovFactor(1.f),
          iPenColor(TRgb(255, 255, 255)), iPenAlpha(255),
          iAlignHorizontal(EHuiAlignHLeft), iAlignVertical(EHuiAlignVTop),
          iFrustumOffsetCompleteness(1.f),
          iQuality(EHuiQualityAccurate),
          iClipRegionStackCount(0)
    {
    /// @todo  Fix this: The real default is 1.f, but the application should
    ///        be able to adjust the completeness. Wowidle/Mmenu have been
    ///        developed with .666f.
    iFrustumOffsetCompleteness = .666f;
    }

EXPORT_C CHuiGc::~CHuiGc()
    {
    iTempRegion.Close();
    iClipRegion.Close();
    iClipRegionStack.Reset();
    iRecycledRegions.ResetAndDestroy();    
    iWindowTransform = NULL;
    iCurrentFont = NULL;
    }


EXPORT_C void CHuiGc::SetQuality(THuiQuality aQuality)
    {
    iQuality = aQuality;
    }


EXPORT_C THuiQuality CHuiGc::Quality() const
    {
    return iQuality;
    }


EXPORT_C void CHuiGc::SetDisplayArea(const TRect& aDisplayArea)
    {
    iDisplayArea = aDisplayArea;
    iClipRegion.Clear();
    iClipRegion.AddRect(aDisplayArea);
    }


EXPORT_C TRect CHuiGc::DisplayArea() const
    {
    if(iOrientation == EOrientationNormal)
        {
        return iDisplayArea;
        }
    else
        {
        TInt width = iDisplayArea.Width();
        TInt height = iDisplayArea.Height();
        /// @todo  Should not always be at the top left corner.
        TRect rect(TPoint(0, 0), TSize(height, width));
        return rect;
        }
    }


EXPORT_C void CHuiGc::SetOrientation(TOrientation aOrientation)
    {
    iOrientation = aOrientation;
    }


EXPORT_C CHuiGc::TOrientation CHuiGc::Orientation() const
    {
    return iOrientation;
    }


EXPORT_C void CHuiGc::SetProjection(TProjection aProjection)
    {
    SetProjection(aProjection, iDisplayArea);
    }


EXPORT_C CHuiGc::TProjection CHuiGc::Projection() const
    {
    return iProjection;
    }


EXPORT_C void CHuiGc::SetProjection(TProjection aProjection, const TRect& aViewport)
    {
    iProjection = aProjection;
    iProjectionViewport = aViewport;
    iFrustumOffset = THuiRealPoint(0.f, 0.f);

    UpdateProjection();
    }


EXPORT_C void CHuiGc::SetFrustumOffset(const THuiRealPoint& aFrustumOffset)
    {
    if(!iDisplayArea.Width() || !iDisplayArea.Height())
        {
        return;
        }

    THuiRealPoint local = aFrustumOffset;
    TRect area = DisplayArea(); // Orientation applied.
    local.iX -= area.Width()/2.f;
    local.iY -= area.Height()/2.f;
    local.iX /= area.Width()/2.f;
    local.iY /= area.Height()/2.f;

    // Adjust so that the perspective correction isn't complete.
    local.iX *= iFrustumOffsetCompleteness;
    local.iY *= iFrustumOffsetCompleteness;

    iFrustumOffset = local;

    UpdateProjection();
    }


EXPORT_C void CHuiGc::Disable(TFeature aFeature)
    {
    Enable(aFeature, EFalse);
    }


EXPORT_C TRect CHuiGc::UnOrientRect(const TRect& aOrientedRect) const
    {
    if(iOrientation == EOrientationCCW90)
        {
        // Un-orient 90 degrees clockwise.
        TSize size(aOrientedRect.Height(), aOrientedRect.Width());
        TPoint pos(iDisplayArea.Width() - aOrientedRect.iBr.iY,
                   aOrientedRect.iTl.iX);
        return TRect(pos, size);
        }
    else if(iOrientation == EOrientationCW90)
        {
        // Un-orient 90 degrees counterclockwise.
        TSize size(aOrientedRect.Height(), aOrientedRect.Width());
        TPoint pos(aOrientedRect.iTl.iY,
                   iDisplayArea.Height() - aOrientedRect.iBr.iX);
        return TRect(pos, size);
        }
    else
        {
        // No change because there is no orientation.
        return aOrientedRect;
        }
    }

EXPORT_C void CHuiGc::UnOrientRegion(TRegion& aOrientedRegion) const
    {
    if(iOrientation == EOrientationCCW90 || iOrientation == EOrientationCW90)
        {
        iTempRegion.Clear();
        for (TInt i=0; i < aOrientedRegion.Count(); i++)
            {
            iTempRegion.AddRect(UnOrientRect(aOrientedRegion[i]));
            }        
        aOrientedRegion.Copy(iTempRegion);
        }
    else
        {
        // No change because there is no orientation.
        }
    }

EXPORT_C void CHuiGc::PushClip()
    {
    RRegion* newRegion = CreateRecycledRegion();
    
    if (newRegion)
        {
        newRegion->Copy(iClipRegion);
        iClipRegionStack.Append(newRegion);    
        }
        
    // Out of memory situation is handled by keeping separate
    // counter which is checked at pop.   
    iClipRegionStackCount++;
    }

EXPORT_C void CHuiGc::PopClip()
    {
    __ASSERT_ALWAYS(iClipRegionStack.Count() > 0,
                    THuiPanic::Panic(THuiPanic::EGcClipStackPopFromEmpty));

    // Check if out of memory situation has happened in the push, so we don't pop
    // because we have not really pushed anything.  
    if (iClipRegionStackCount > iClipRegionStack.Count())
        {
        iClipRegionStackCount--;    
        return;
        }

    if(iClipRegionStack.Count() > 0)
        {
        TInt index = iClipRegionStack.Count() - 1;
        SetClipRegion(*iClipRegionStack[index]);        
        RRegion* region = iClipRegionStack[index];        
        iClipRegionStack.Remove(index);
        DeleteRecycledRegion(region);
        region = NULL;
        iClipRegionStackCount--;    
        }
    }

EXPORT_C void CHuiGc::SetClip(const TRect& aClipRect)
    {
    HUI_ASSERT(aClipRect.Width() >= 0);
    HUI_ASSERT(aClipRect.Height() >= 0);    
    
    iTempRegion.Clear();
    iTempRegion.AddRect(aClipRect);    
    CHuiGc::SetClipRegion(iTempRegion);
    }

EXPORT_C TRect CHuiGc::ClipRect() const
    {
    // This does not really provide actually valid data anymore
    // since clipping is done with a region. 
    return iClipRegion.BoundingRect();
    }

const TRegion& CHuiGc::ClipRegion() const
    {
    return iClipRegion;
    }


EXPORT_C void CHuiGc::Clip(const TRect& aClipRect)
    {
    HUI_ASSERT(aClipRect.Width() >= 0);
    HUI_ASSERT(aClipRect.Height() >= 0);
    
    if(iClipRegion.Intersects(aClipRect))
        {
        iTempRegion.Clear();
        iTempRegion.AddRect(aClipRect);
        iClipRegion.Intersect(iTempRegion);
        }
    else
        {
        // Empty clipping rectangle.
        iClipRegion.Clear();
        }
    
    SetClipRegion(iClipRegion);        
    }

EXPORT_C void CHuiGc::Clip(const TRegion& aClipRegion)
    {
    iClipRegion.Intersect(aClipRegion);
    SetClipRegion(iClipRegion);
    }
    
EXPORT_C void CHuiGc::SetClipRegion(const TRegion& aClipRegion)
	{
	if (&aClipRegion != &iClipRegion)
	    {
	    iClipRegion.Copy(aClipRegion);        
	    }
	}
	
EXPORT_C void CHuiGc::CancelClipping()
    {
    }

EXPORT_C TInt CHuiGc::ClipStackCount() const
    {
    return iClipRegionStack.Count();
    }


EXPORT_C void CHuiGc::SetAlign(THuiAlignHorizontal aHorizontalAlign,
                               THuiAlignVertical aVerticalAlign)
    {
    switch(aHorizontalAlign)
        {
        case EHuiAlignHLocale:
            iAlignHorizontal = LocaleTextAlignment();
            break;

        case EHuiAlignHLocaleMirrored:
            iAlignHorizontal = LocaleTextAlignment();
            if(iAlignHorizontal == EHuiAlignHLeft)
                {
                iAlignHorizontal = EHuiAlignHRight;
                }
            else
                {
                iAlignHorizontal = EHuiAlignHLeft;
                }
            break;

        default:
            iAlignHorizontal = aHorizontalAlign;
            break;
        }

    iAlignVertical = aVerticalAlign;
    }


EXPORT_C THuiAlignHorizontal CHuiGc::AlignHorizontal() const
    {
    return iAlignHorizontal;
    }


EXPORT_C THuiAlignVertical CHuiGc::AlignVertical() const
    {
    return iAlignVertical;
    }


EXPORT_C void CHuiGc::SetPenColor(const TRgb& aColor)
    {
    iPenColor = aColor;
    }


EXPORT_C TRgb CHuiGc::PenColor() const
    {
    return iPenColor;
    }


EXPORT_C void CHuiGc::SetPenColorAlpha(const TRgb& aColor)
    {
    iPenColor = aColor;
    iPenColor.SetAlpha(255);

    iPenAlpha = TUint8(aColor.Alpha()); // 8 bits should be enough
    }


EXPORT_C TRgb CHuiGc::PenColorAlpha() const
    {
    TRgb color = iPenColor;
    color.SetAlpha(iPenAlpha);
    return color;
    }


EXPORT_C void CHuiGc::SetPenAlpha(TInt aAlpha)
    {
    if(aAlpha < 0)
        {
        aAlpha = 0;
        }
    else if(aAlpha > 255)
        {
        aAlpha = 255;
        }
    else
        {
        // for PC lint
        }
    iPenAlpha = TUint8(aAlpha); // 8 bits should be enough
    }


EXPORT_C TInt CHuiGc::PenAlpha() const
    {
    return iPenAlpha;
    }


EXPORT_C void CHuiGc::SetFont(THuiFont& aFont)
    {
    iCurrentFont = &aFont;
    }


EXPORT_C void CHuiGc::DrawTextL(const TDesC& aText,
                                const THuiRealRect& aBounds,
                                TReal32 aShadow) __SOFTFP
    {
    if(!Font())
        {
        return;
        }
    CHuiTextMesh* mesh = CHuiStatic::Renderer().CreateTextMeshL();
    CleanupStack::PushL(mesh);
    mesh->SetTextL(aText, ETrue);
    DrawText(*mesh, aBounds, aShadow);
    CleanupStack::PopAndDestroy(mesh);
    }


EXPORT_C void CHuiGc::DrawText(const CHuiTextMesh& aTextMesh,
                               const THuiRealRect& aBounds,
                               TReal32 aShadow) __SOFTFP
    {
    /** @todo  This could be generalized: MHuiBoundedDrawable (for example) */

    // Font is defined in the text mesh.

    // Real bounding size of the text.
    TSize textBounds = aTextMesh.Extents();

    // Reference origin for the text.
    TPoint offset = AlignOffset(aBounds.Size());

    // Actual starting point for glyphs.
    THuiRealPoint pos = aBounds.iTl - offset;
    switch(AlignHorizontal())
        {
        case EHuiAlignHRight:
            pos.iX -= textBounds.iWidth;
            break;

        case EHuiAlignHCenter:
            pos.iX -= textBounds.iWidth / 2; // or 2.f?
            break;

        default:
            break;
        }
    switch(AlignVertical())
        {
        case EHuiAlignVBottom:
            pos.iY -= textBounds.iHeight;
            break;

        case EHuiAlignVCenter:
            pos.iY -= textBounds.iHeight / 2; // or 2.f?
            break;

        default:
            break;
        }


    // Apply the position.
    Push(EHuiGcMatrixModel);
    Translate(EHuiGcMatrixModel, pos.iX, pos.iY, 0.f);

    aTextMesh.Draw(*this, aShadow);

    Pop(EHuiGcMatrixModel);
    }


EXPORT_C void CHuiGc::DrawImage(const THuiImage& aImage,
                                const THuiRealPoint& aPos,
                                const THuiRealSize& aSize)
    {
    if(!aImage.HasTexture())
        {
        return;
        }

    THuiRealPoint topLeft = aPos + AlignOffset(aSize);
    UpdateColor();

    // Update texture preferred size, toolkit may use this information to resize texture
    NotifyImagePreferredSize(aImage, THuiRealRect(topLeft,aSize));
    
    DoDrawImage(aImage, topLeft, aSize);
    }


EXPORT_C void CHuiGc::DrawImages(const THuiImage& aImage,
                                 const THuiImage& aImage2,
                                 const THuiRealPoint& aPos,
                                 const THuiRealSize& aSize)
    {
    if(!aImage.HasTexture() || !aImage2.HasTexture())
        {
        return;
        }

    THuiRealPoint topLeft = aPos + AlignOffset(aSize);
    UpdateColor();
    
    // Update texture preferred size, toolkit may use this information to resize texture
    NotifyImagePreferredSize(aImage, THuiRealRect(topLeft,aSize));
    NotifyImagePreferredSize(aImage2, THuiRealRect(topLeft,aSize));

    DoDrawImages(aImage, aImage2, topLeft, aSize);
    }


EXPORT_C void CHuiGc::DrawImage(const THuiImage& aImage,
                                const THuiRealRect& aBounds)
    {
    if(!aImage.HasTexture())
        {
        return;
        }

    TSize space(TInt(aBounds.Width() - aImage.Texture().Size().iWidth),
                TInt(aBounds.Height() - aImage.Texture().Size().iHeight));

    TPoint topLeft = aBounds.iTl - AlignOffset(space);

    UpdateColor();

    // Update texture preferred size, toolkit may use this information to resize texture
    NotifyImagePreferredSize(aImage, aBounds);

    DoDrawImage(aImage, topLeft, aImage.Texture().Size());
    }


EXPORT_C void CHuiGc::DrawImage( const THuiImage& aImage,
                                 const THuiRealSize& aSize,
                                 const THuiRealRect& aBounds)
    {
    if(!aImage.HasTexture())
        {
        return;
        }

    TSize space(TInt(aBounds.Width() - aSize.iWidth),
                TInt(aBounds.Height() - aSize.iHeight));

    TPoint topLeft = aBounds.iTl - AlignOffset(space);

    UpdateColor();

    // Update texture preferred size, toolkit may use this information to resize texture
    NotifyImagePreferredSize(aImage, THuiRealRect(topLeft,aSize));

    DoDrawImage(aImage, topLeft, aSize);
    }


EXPORT_C void CHuiGc::DrawImages(const THuiImage& aImage,
                                 const THuiImage& aImage2,
                                 const THuiRealSize& aSize,
                                 const THuiRealRect& aBounds)
    {
    if(!aImage.HasTexture() || !aImage2.HasTexture())
        {
        return;
        }

    TSize space(TInt(aBounds.Width() - aSize.iWidth),
                TInt(aBounds.Height() - aSize.iHeight));

    TPoint topLeft = aBounds.iTl - AlignOffset(space);

    UpdateColor();

    // Update texture preferred size, toolkit may use this information to resize texture
    NotifyImagePreferredSize(aImage, THuiRealRect(topLeft,aSize));
    NotifyImagePreferredSize(aImage2, THuiRealRect(topLeft,aSize));

    DoDrawImages(aImage, aImage2, topLeft, aSize);
    }


EXPORT_C THuiFont* CHuiGc::Font()
    {
    return iCurrentFont;
    }


EXPORT_C TPoint CHuiGc::AlignOffset(const TSize& aBounds) const
    {
    TPoint offset(0, 0);

    switch(iAlignHorizontal)
        {
        case EHuiAlignHRight:
            offset.iX = -aBounds.iWidth;
            break;

        case EHuiAlignHCenter:
            offset.iX = -aBounds.iWidth / 2;
            break;

        default:
            offset.iX = 0;
            break;
        }

    switch(iAlignVertical)
        {
        case EHuiAlignVBottom:
            offset.iY = -aBounds.iHeight;
            break;

        case EHuiAlignVCenter:
            offset.iY = -aBounds.iHeight / 2;
            break;

        default:
            offset.iY = 0;
            break;
        }

    return offset;
    }


EXPORT_C TReal32 CHuiGc::Offset2D() const __SOFTFP
    {
    /// @todo  Make this configurable.
    //return 100.0 / FovFactor();
    return KDefaultOffset2D;
    }


EXPORT_C void
CHuiGc::SetWindowTransformation(CHuiTransformation* aWindowTransformation)
    {
    iWindowTransform = aWindowTransformation;
    }


EXPORT_C TReal32 CHuiGc::NearClipDistance() const __SOFTFP
    {
    return iNearClipDistance;
    }


EXPORT_C TReal32 CHuiGc::FarClipDistance() const __SOFTFP
    {
    return iFarClipDistance;
    }


EXPORT_C TRect CHuiGc::ProjectionViewport() const
    {
    return iProjectionViewport;
    }


EXPORT_C TRect CHuiGc::RawDisplayArea() const
    {
    return iDisplayArea;
    }


EXPORT_C const CHuiTransformation* CHuiGc::WindowTransformation() const
    {
    return iWindowTransform;
    }


EXPORT_C THuiRealPoint CHuiGc::FrustumOffset() const
    {
    return iFrustumOffset;
    }


EXPORT_C TReal32 CHuiGc::FovFactor() const __SOFTFP
    {
    return 1.f / iFovFactor;
    }


EXPORT_C THuiAlignHorizontal CHuiGc::LocaleTextAlignment()
    {
    if(CHuiStatic::LayoutMirrored())
        {
        return EHuiAlignHRight;
        }
    return EHuiAlignHLeft;
    }

EXPORT_C void CHuiGc::DrawStretchImage(TStretchMode /*aMode*/,
                      const THuiImage& aImage,
                      const THuiRealRect& aRect,
                      TInt /*aStretchStartWidth*/,
                      TInt /*aStretchEndWidth*/)                      
    {
    // Update texture preferred size, toolkit may use this information to resize texture
    NotifyImagePreferredSize(aImage, aRect);        
    }


EXPORT_C void CHuiGc::NotifyImagePreferredSize(const THuiImage& aImage, THuiRealRect aRect)
    {
    const CHuiTexture* texture = aImage.ImageDefaultTexture();    
    
    if  (texture)
        {        
        CHuiTexture* notifiedTexture = const_cast<CHuiTexture*> (texture);
        if (notifiedTexture->IsAutoSizeCalculationEnabled())
            {                
            TReal32 xratio = 1;
            TReal32 yratio = 1;
            
            // Take into account used texture coordinates of THuiImage       
            TReal32 relativeWidth = aImage.BottomRight().iX - aImage.TopLeft().iX;
            TReal32 relativeHeight = aImage.BottomRight().iY - aImage.TopLeft().iY;
            
            if (relativeWidth != 0.f && relativeHeight != 0.f)
                {
                xratio *= (1/relativeWidth);
                yratio *= (1/relativeHeight);                
                }

            TReal32 width = aRect.Size().iWidth * xratio;
            TReal32 height = aRect.Size().iHeight * yratio;
            
            // Take into account window transformation scale (To enable HiRes TV-OUT textures)
            // Other transformations are ignored for now. Possible those could be optionally
            // taken into account as well if resizing algortihm is lazy enough.
            CHuiTransformation* wt =  (CHuiTransformation*)WindowTransformation();
            if (wt)
                {
                TInt count = wt->Count();
                for (TInt i=0;i<count;i++)
                    {
                    CHuiTransformation::TTransform& step = wt->Step(i);
                    if (step.iType == CHuiTransformation::ETypeScale)
                        {
                        width = width * step.iParams[EHuiTransformParamTranslateX].Now();    
                        height = height * step.iParams[EHuiTransformParamTranslateY].Now();    
                        }                        
                    }                    
                }
            
            
            notifiedTexture->NotifyPreferredSize(THuiRealSize(width,height));    
            }
        }
    }

TInt CHuiGc::CopyScreenToBitmap(CFbsBitmap* aBitmap)
    {
    THuiGcCopyScreenToBitmapParams params;
    params.iErrorCode = KErrNotSupported;
    params.iBitmap = aBitmap;
    TAny* ptr = &params;
    
    GcExtension(KHuiGcCopyScreenToBitmap, &ptr);
    
    return params.iErrorCode;
    }

EXPORT_C void CHuiGc::TransformDirtyRect(THuiRealRect& /*aRect*/)
    {
    // No sensible implementation for HuiGc base class. Implement at least in
    // HuiBitgdiGc child class.
    }

EXPORT_C void CHuiGc::EnableTransformedClippingRects(TBool /*aEnable*/)
    {
    // No sensible implementation for HuiGc base class. Implement at least in
    // HuiBitgdiGc child class.
    }

EXPORT_C void CHuiGc::SetClip(const TRect& /*aClipRect*/, TBool /*aTransformClipRect*/)
    {
    // No sensible implementation for HuiGc base class. Implement at least in
    // HuiBitgdiGc child class.
    }

EXPORT_C void CHuiGc::GcExtension(const TUid& /*aExtensionUid*/, TAny** aExtensionParams)
    {
    // If no extension with given UID was found, indicate it by returning null
    *aExtensionParams = NULL;
    }

RRegion* CHuiGc::CreateRecycledRegion()
    {
    RRegion* region = NULL;
    if (iRecycledRegions.Count())
        {
        // If there are recycled regionbufs, use one of those
        TInt last = iRecycledRegions.Count() - 1;
        region = iRecycledRegions[last];
        iRecycledRegions.Remove(last);    
        }
    else
        {
        // No recycled regionbufs available, create new one
        region = new RRegionBuf<KHuiGcClipRegionGranularity>();   
        }    
    return region;        
    }

void CHuiGc::DeleteRecycledRegion(RRegion* aRegion)
    {
    TInt error = KErrNone;
    
    if (iRecycledRegions.Count() < KHuiMaxRecycledRegionCount)
        {
        aRegion->Clear();
        error = iRecycledRegions.Append(aRegion);            
        if (error)
            {
            aRegion->Destroy();
            }
        }
    else
        {
        aRegion->Destroy();
        }                    
    }
