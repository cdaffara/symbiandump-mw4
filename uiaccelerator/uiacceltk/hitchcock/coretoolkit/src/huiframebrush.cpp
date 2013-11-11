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
* Description:   Implementation of CHuiFrameBrush class - a brush that draws a 
*                border at the edges of visuals.
*
*/



#include "uiacceltk/huiframebrush.h"  // Class definition
#include "uiacceltk/HuiDrawing.h"
#include "uiacceltk/HuiVisual.h"
#include "uiacceltk/HuiImageLoaderUtil.h"
#include "uiacceltk/HuiGc.h"
#include "uiacceltk/HuiUtil.h"
#include <AknsItemData.h>
#include <AknsUtils.h>

const TInt KHuiNumberFramesInFullDefinedSkin = 9;
const TInt KSkinFrameBorderWidthDefaultValue = 8;
const TInt KSkinFrameBorderWidthSmallValue = 4;
const TInt KSkinFrameBorderWidthSmallestValue = 1;


CHuiFrameBrush::CHuiImageWithID::~CHuiImageWithID()
    {
    // Delete only if framebrush has ownership
    if (iId)
        {            
        CHuiTextureManager& tm = CHuiStatic::Env().TextureManager();
        CHuiTexture* tex = tm.Texture(iId);
        delete tex; // calls TextureManager::RemoveTexture()
        }
    }
    
const MHuiSegmentedTexture& CHuiFrameBrush::CHuiImageWithID::Texture() 
    {
    return iImage.Texture();
    }

THuiImage& CHuiFrameBrush::CHuiImageWithID::Image()
    {
    return iImage;
    }

void CHuiFrameBrush::CHuiImageWithID::SetImage(const THuiImage& aImage)
    {
    iImage = aImage;    
    }

EXPORT_C CHuiFrameBrush* CHuiFrameBrush::NewL(
        const THuiImage& aImage,
        const THuiXYMetric& aEdgeOffset)
    {
    CHuiFrameBrush* self = CHuiFrameBrush::NewLC(aImage, aEdgeOffset);
    CleanupStack::Pop();
    return self;        
    }
    
EXPORT_C CHuiFrameBrush* CHuiFrameBrush::NewLC(
        const THuiImage& aImage,
        const THuiXYMetric& aEdgeOffset)
    {
    CHuiFrameBrush* self = new (ELeave) CHuiFrameBrush(aImage, 
                                                       aEdgeOffset);
    self->ConstructL(aImage);
    CleanupStack::PushL(self);
    return self;        
    }

EXPORT_C CHuiFrameBrush* CHuiFrameBrush::NewL(
        const TAknsItemID& aIID,
        const THuiXYMetric& aEdgeOffset)
    {
    CHuiFrameBrush* self = CHuiFrameBrush::NewLC(aIID, aEdgeOffset);
    CleanupStack::Pop();
    return self;        
    }
    
EXPORT_C CHuiFrameBrush* CHuiFrameBrush::NewLC(
        const TAknsItemID& aIID,
        const THuiXYMetric& aEdgeOffset)
    {
    CHuiFrameBrush* self = new (ELeave) CHuiFrameBrush(aIID, 
                                                       aEdgeOffset);
    CleanupStack::PushL(self);
    return self;        
    }

EXPORT_C CHuiFrameBrush* CHuiFrameBrush::NewL(
    const TAknsItemID& aIID,
	TInt aEdgeOffsetX, 
	TInt aEdgeOffsetY)
    {
    CHuiFrameBrush* self = CHuiFrameBrush::NewLC(aIID, aEdgeOffsetX, aEdgeOffsetY);
    CleanupStack::Pop();
    return self;
    }

EXPORT_C CHuiFrameBrush* CHuiFrameBrush::NewLC(const TAknsItemID& aIID,
    TInt aEdgeOffsetX, TInt aEdgeOffsetY)
    {
    CHuiFrameBrush* self = new (ELeave) CHuiFrameBrush(aIID, 
                                                       aEdgeOffsetX, 
                                                       aEdgeOffsetY);
    CleanupStack::PushL(self);
    return self;
    }

void CHuiFrameBrush::ConstructL(const THuiImage& aImage)
    {
    SetImage(aImage);    
    }

CHuiFrameBrush::CHuiFrameBrush(const THuiImage& /*aImage*/, const THuiXYMetric& aEdgeOffset)
    : CHuiBrush(),
     iFrameIID(KAknsIIDNone),
     iInnerRect(0,0,0,0),
     iOuterRect(0,0,0,0),
     iEdgeOffset(aEdgeOffset.iX.iMagnitude,aEdgeOffset.iY.iMagnitude),
     iEdgeOffsetMetric()
    {
    SetClipToVisual(EFalse);        
    }

CHuiFrameBrush::CHuiFrameBrush(const TAknsItemID& aIID, const THuiXYMetric& aEdgeOffset)
    : CHuiBrush(),
     iFrameIID(aIID),
     iInnerRect(0,0,0,0),
     iOuterRect(0,0,0,0),
     iEdgeOffset(aEdgeOffset.iX.iMagnitude,aEdgeOffset.iY.iMagnitude),
     iEdgeOffsetMetric()
    {
    SetClipToVisual(EFalse);        
    }


CHuiFrameBrush::CHuiFrameBrush(
    const TAknsItemID& aIID, 
    TInt aEdgeOffsetX, 
    TInt aEdgeOffsetY)
    : CHuiBrush(),
     iFrameIID(aIID),
     iInnerRect(0,0,0,0),
     iOuterRect(0,0,0,0),
     iEdgeOffset(TReal32(aEdgeOffsetX),TReal32(aEdgeOffsetY)),
     iEdgeOffsetMetric(aEdgeOffsetX, aEdgeOffsetY)
    {
    SetClipToVisual(EFalse);
    }

CHuiFrameBrush::CHuiFrameBrush()
    {
	}

void CHuiFrameBrush::HandleActionL(const THuiActionCommand& aActionCommand)
    {
    if (iFrameIID != KAknsIIDNone)
        {            
        if (aActionCommand.Id() == KAknsMessageSkinChange)
            {
            if (iEnvReleased)
                {
                // If env is released, creating new textures may fail. Just set flag.
            	iSkinChanged = ETrue;
            	// And destroy textures because new skin may not have same items defined, restoring might fail.
            	iImages.ResetAndDestroy();
            	
            	delete iImageloader;
                iImageloader = NULL;
                }
            else
    	        {
    	        ConstructFrameL();	
            	iSkinChanged = EFalse;
    	        }
            }
        else if (aActionCommand.Id() == KHuiEnvReleasedActionId)
            {
            iEnvReleased = ETrue;    
            }
        else if (aActionCommand.Id() == KHuiEnvRestoredActionId)
            {
            iEnvReleased = EFalse;
            if (iSkinChanged)
                {
                ConstructFrameL();    
                iSkinChanged = EFalse;
                }            
            }
        }
    }

TSize CHuiFrameBrush::GetFramePartSize(const TFrameParts aFramePart) const
    {
    TSize size(0,0);
    switch (aFramePart)
        {
        case EFramePartCornerTl:
            size.iWidth = iInnerRect.iTl.iX - iOuterRect.iTl.iX;
            size.iHeight = iInnerRect.iTl.iY - iOuterRect.iTl.iY;
            break;
        case EFramePartCornerTr:
            size.iWidth = iOuterRect.iBr.iX - iInnerRect.iBr.iX;
            size.iHeight = iInnerRect.iTl.iY - iOuterRect.iTl.iY;
            break;
        case EFramePartCornerBl:
            size.iWidth = iInnerRect.iTl.iX - iOuterRect.iTl.iX;
            size.iHeight = iOuterRect.iBr.iY - iInnerRect.iBr.iY;					
            break;
        case EFramePartCornerBr:
            size.iWidth = iOuterRect.iBr.iX - iInnerRect.iBr.iX;
            size.iHeight = iOuterRect.iBr.iY - iInnerRect.iBr.iY;					
            break;
        case EFramePartSideT:
            size.iWidth = iInnerRect.iBr.iX - iInnerRect.iTl.iX;
            size.iHeight = iInnerRect.iTl.iY - iOuterRect.iTl.iY;					
            break;
        case EFramePartSideB:
            size.iWidth = iInnerRect.iBr.iX - iInnerRect.iTl.iX;
            size.iHeight = iOuterRect.iBr.iY - iInnerRect.iBr.iY;					
            break;
        case EFramePartSideL:
            size.iWidth = iInnerRect.iTl.iX - iOuterRect.iTl.iX;
            size.iHeight = iInnerRect.iBr.iY - iInnerRect.iTl.iY;					
            break;
        case EFramePartSideR:
            size.iWidth = iOuterRect.iBr.iX - iInnerRect.iBr.iX;
            size.iHeight = iInnerRect.iBr.iY - iInnerRect.iTl.iY;					
            break;
        case EFramePartCenter:
            size = iInnerRect.Size();
            break;
        default:
            break;
        }
    return size;
    }


CHuiFrameBrush::~CHuiFrameBrush()
    {
    CHuiStatic::Env().iActionObservers.RemoveIfFound(*this);
    iImages.ResetAndDestroy();
    delete iImageloader;    
    }


void CHuiFrameBrush::ExpandVisualRect(TRect& aRect) const
    {
    // We need access to visual to convert metric units into pixels,
    // currently a pointer to visual is provided through separate
    // method since we want to avoid breaking binary compatibility 
    // of this class.
    THuiRealPoint edgeOffsetInPixels = PointInPixels(BrushGuide(), iEdgeOffset, iEdgeOffsetMetric);
    aRect.Grow(HUI_ROUND_FLOAT_TO_INT(edgeOffsetInPixels.iX), HUI_ROUND_FLOAT_TO_INT(edgeOffsetInPixels.iY));
    }
   
TBool CHuiFrameBrush::Changed() const
    {
    if ( CHuiBrush::Changed() )
        {
        return ETrue;
        }
    
    for (TInt i=0; i<iImages.Count();i++)
        {
        if (iImages[i]->Image().Changed())
            {
            return ETrue;    
            }
        }     
        
    return iEdgeOffset.Changed();
    }

void CHuiFrameBrush::ClearChanged()
    {
    CHuiBrush::ClearChanged();

    for (TInt i=0; i<iImages.Count();i++)
        {
        iImages[i]->Image().ClearChanged();
        }     

    iEdgeOffset.ClearChanged();
    }


void CHuiFrameBrush::Draw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const
    {            
    THuiRealRect content = aGuide.BrushRect();

    TRect adjustedInnerRect = AdjustedInnerRect(content.Size());
    TRect adjustedOuterRect = AdjustedOuterRect(content.Size());

    if (adjustedInnerRect == TRect(0,0,0,0) || adjustedOuterRect== TRect(0,0,0,0))
        {
        return;        
        }

    THuiRealPoint edgeOffsetInPixels = PointInPixels(BrushGuide(), iEdgeOffset, iEdgeOffsetMetric);
    content.Grow(edgeOffsetInPixels.iX, edgeOffsetInPixels.iY);

    TReal32 opacity = aGuide.BrushOpacity() * iOpacity.Now();

    aGc.SetPenColor(KRgbWhite);
    aGc.SetPenAlpha(TInt(opacity * 255));

    aGc.Enable(CHuiGc::EFeatureTexturing);
    aGc.Enable(CHuiGc::EFeatureBlending);

    // Apply the texture offset manually.
    aGc.Push(EHuiGcMatrixTexture);
    aGc.LoadIdentity(EHuiGcMatrixTexture);

    if (iImages.Count() == KHuiNumberFramesInFullDefinedSkin )
        {

        // draw 9 part image
        THuiRealSize size(0.f,0.f);
        size.iWidth = adjustedInnerRect.iTl.iX - adjustedOuterRect.iTl.iX;
        size.iHeight = adjustedInnerRect.iTl.iY - adjustedOuterRect.iTl.iY;
        aGc.DrawStretchImage(CHuiGc::EStretchFull, iImages[EFramePartCornerTl]->Image(), THuiRealRect(content.iTl,size),0,0);

        aGc.DrawStretchImage(CHuiGc::EStretchFull, iImages[EFramePartSideT]->Image(), THuiRealRect(THuiRealPoint(content.iTl.iX+(adjustedInnerRect.iTl.iX - adjustedOuterRect.iTl.iX), content.iTl.iY),THuiRealPoint(content.iBr.iX-(adjustedOuterRect.iBr.iX - adjustedInnerRect.iBr.iX),content.iTl.iY+(adjustedInnerRect.iTl.iY - adjustedOuterRect.iTl.iY))),0,0);

        size.iWidth = adjustedOuterRect.iBr.iX - adjustedInnerRect.iBr.iX;
        size.iHeight = adjustedInnerRect.iTl.iY - adjustedOuterRect.iTl.iY;
        aGc.DrawStretchImage(CHuiGc::EStretchFull, iImages[EFramePartCornerTr]->Image(), THuiRealRect(THuiRealPoint(content.iBr.iX-size.iWidth, content.iTl.iY),size),0,0);

        aGc.DrawStretchImage(CHuiGc::EStretchFull, iImages[EFramePartSideL]->Image(), THuiRealRect(THuiRealPoint(content.iTl.iX, content.iTl.iY+(adjustedInnerRect.iTl.iY - adjustedOuterRect.iTl.iY)),THuiRealPoint(content.iTl.iX+(adjustedInnerRect.iTl.iX - adjustedOuterRect.iTl.iX),content.iBr.iY-(adjustedOuterRect.iBr.iY - adjustedInnerRect.iBr.iY))),0,0);

        aGc.DrawStretchImage(CHuiGc::EStretchFull, iImages[EFramePartCenter]->Image(), THuiRealRect(THuiRealPoint(content.iTl.iX+(adjustedInnerRect.iTl.iX - adjustedOuterRect.iTl.iX), content.iTl.iY+(adjustedInnerRect.iTl.iY - adjustedOuterRect.iTl.iY)),THuiRealPoint(content.iBr.iX-(adjustedOuterRect.iBr.iX - adjustedInnerRect.iBr.iX),content.iBr.iY-(adjustedOuterRect.iBr.iY - adjustedInnerRect.iBr.iY))),0,0);

        aGc.DrawStretchImage(CHuiGc::EStretchFull, iImages[EFramePartSideR]->Image(), THuiRealRect(THuiRealPoint(content.iBr.iX-(adjustedOuterRect.iBr.iX - adjustedInnerRect.iBr.iX), content.iTl.iY+(adjustedInnerRect.iTl.iY - adjustedOuterRect.iTl.iY)),THuiRealPoint(content.iBr.iX,content.iBr.iY-(adjustedOuterRect.iBr.iY - adjustedInnerRect.iBr.iY))),0,0);


        size.iWidth = adjustedInnerRect.iTl.iX - adjustedOuterRect.iTl.iX;
        size.iHeight = adjustedOuterRect.iBr.iY - adjustedInnerRect.iBr.iY;
        aGc.DrawStretchImage(CHuiGc::EStretchFull, iImages[EFramePartCornerBl]->Image(), THuiRealRect(THuiRealPoint(content.iTl.iX,content.iBr.iY-size.iHeight),size),0,0);

        aGc.DrawStretchImage(CHuiGc::EStretchFull, iImages[EFramePartSideB]->Image(), THuiRealRect(THuiRealPoint(content.iTl.iX+(adjustedInnerRect.iTl.iX - adjustedOuterRect.iTl.iX), content.iBr.iY-(adjustedOuterRect.iBr.iY - adjustedInnerRect.iBr.iY)),THuiRealPoint(content.iBr.iX-(adjustedOuterRect.iBr.iX - adjustedInnerRect.iBr.iX),content.iBr.iY)),0,0);


        size.iWidth = adjustedOuterRect.iBr.iX - adjustedInnerRect.iBr.iX;
        size.iHeight = adjustedOuterRect.iBr.iY - adjustedInnerRect.iBr.iY;	
        aGc.DrawStretchImage(CHuiGc::EStretchFull, iImages[EFramePartCornerBr]->Image(), THuiRealRect(THuiRealPoint(content.iBr.iX-size.iWidth, content.iBr.iY-size.iHeight),size),0,0);

        }    
    else if ( iImages.Count() )
        {

        // draw from one part
        aGc.DrawStretchImage(CHuiGc::EStretchFull,
            iImages[0]->Image(),
            content);		    			
        }

    aGc.Pop(EHuiGcMatrixTexture);
    }

void CHuiFrameBrush::ResetAndDestroyImageArray( TAny* aAny )
    {
    CHuiFrameBrush* self = static_cast<CHuiFrameBrush*>(aAny);
    self->iImages.ResetAndDestroy();
    delete self->iImageloader;
    self->iImageloader = NULL;
    }

void CHuiFrameBrush::ConstructFrameL()
    {
    MAknsSkinInstance* skin = CHuiStatic::SkinInstance();
        
    if (iFrameIID != KAknsIIDNone)
        {            
        iImages.ResetAndDestroy();
        delete iImageloader;
        iImageloader = NULL;
        
        // Use cleanup item to make sure that if some of frame allocations
        // fail, the previously loaded items are released.
        CleanupStack::PushL( TCleanupItem( ResetAndDestroyImageArray, this ) );

        CAknsItemData* idata = NULL;    				
        if (skin)
            {
            TRAP_IGNORE(idata = skin->CreateUncachedItemDataL( iFrameIID, EAknsITImageTable ))
            }
        else
            {
            TRAP_IGNORE(idata = CHuiStatic::GetCachedSkinItemDataL(iFrameIID, EAknsITImageTable));
            }
            
        CleanupStack::PushL(idata);
        MHuiBitmapProvider* imageprovider = NULL; 
        
        iImageloader = new (ELeave) CHuiImageLoaderUtil;            

        if (idata)
            {
            // 9 part frame graphics
            CAknsImageTableItemData* itdata = static_cast<CAknsImageTableItemData*>(idata);

            for (TInt count = 0; count < KHuiNumberFramesInFullDefinedSkin; count++)
                {
                iImageloader->SetSize(GetFramePartSize((TFrameParts)(count)), EAspectRatioNotPreserved);
                imageprovider = iImageloader->CreateImageLoaderL(itdata->ImageIID(count), KNullDesC, 0, 0);
                TInt id = CHuiStatic::GenerateId();
                CHuiTexture & texture = CHuiStatic::Env().TextureManager().CreateTextureL(id, imageprovider, EHuiTextureUploadFlagRetainResolution);
                texture.SetSkinContent(EFalse); // we manage skins change w/o help from textures management
                THuiImage newImage = THuiImage(texture); 
                CHuiImageWithID* entry = new CHuiImageWithID(newImage,id); // ownership transferred on success
                if (entry)
                    {
                    CleanupStack::PushL(entry);
                    iImages.AppendL( entry );
                    CleanupStack::Pop();
                    }
                }
            }
        else
            {
            // one part frame graphics
            iImageloader->SetSize(iOuterRect.Size(), EAspectRatioNotPreserved); 
            imageprovider = iImageloader->CreateImageLoaderL(iFrameIID, KNullDesC, 0, 0);
            TInt id = CHuiStatic::GenerateId();
            CHuiTexture & texture = CHuiStatic::Env().TextureManager().CreateTextureL(id, imageprovider, EHuiTextureUploadFlagRetainResolution);
            texture.SetSkinContent(EFalse); // we manage skins change w/o help from textures management
            THuiImage newImage = THuiImage(texture); 
            CHuiImageWithID* entry = new CHuiImageWithID(newImage,id); // ownership transferred on success
            if (entry)
                {
                CleanupStack::PushL(entry);
                iImages.AppendL( entry );
                CleanupStack::Pop();
                }
            }
        
        CleanupStack::PopAndDestroy(); // idata
        CleanupStack::Pop(); // iImages.ResetAndDestroy();
        
        // we don't need skins change information until textures have been set
        CHuiStatic::Env().iActionObservers.AppendIfNotFoundL(*this);
        SetChanged();
        }
    }


EXPORT_C void CHuiFrameBrush::SetFrameRectsL(const TRect& aInnerRect, const TRect& aOuterRect)
    {
    // Sanity check to avoid evil things happening later
    TInt KMinValue = 1;
    if (aOuterRect.iTl.iX - aInnerRect.iTl.iX > KMinValue ||
        aOuterRect.iTl.iY - aInnerRect.iTl.iY > KMinValue ||
        aOuterRect.iBr.iX - aInnerRect.iBr.iX < KMinValue ||
        aOuterRect.iBr.iY - aInnerRect.iBr.iY < KMinValue)
        {
        User::Leave(KErrArgument);    
        }
    
    iInnerRect = aInnerRect;
    iOuterRect = aOuterRect;
    ConstructFrameL();
    }

THuiRealPoint CHuiFrameBrush::PointInPixels(const CHuiVisual* aVisual, const THuiTimedPoint& aPoint, const THuiXYMetric& aMetric) const 
    {
    THuiRealPoint pointInPixels = aPoint.RealNow(); // Init, if no visual available for conversion then defaults to pixels
    if (aVisual)
        {
        THuiXYMetric metric = aMetric;
        metric.iX.iMagnitude = pointInPixels.iX;        
        metric.iY.iMagnitude = pointInPixels.iY;        
        pointInPixels = aVisual->MetricToPixels(metric); // Convert metric width in pixels  
        }    
    return pointInPixels;                    
    }

EXPORT_C void CHuiFrameBrush::SetEdgeOffset(const THuiXYMetric& aEdgeOffset)
    {
    iEdgeOffsetMetric = aEdgeOffset;
    iEdgeOffset.iX.Set(aEdgeOffset.iX.iMagnitude);    
    iEdgeOffset.iY.Set(aEdgeOffset.iY.iMagnitude);    
    }

EXPORT_C void CHuiFrameBrush::SetImage(const THuiImage& aImage)
    {
    TRAPD(err, InitImagesL(1))
    if (err == KErrNone)
        {
        iImages[0]->SetImage(aImage);        
        }
    }
    
EXPORT_C void CHuiFrameBrush::SetImage(TFrameParts aFramePart, const THuiImage& aImage)
    {
    if (aFramePart >= 0 && aFramePart <= EFramePartCenter)
        {
        TRAPD(err, InitImagesL(KHuiNumberFramesInFullDefinedSkin))
        if (err == KErrNone)
            {
            // index is ok because number of images has been just initialize
            // and the maximum allowed value for aFramePart matches that
            iImages[aFramePart]->SetImage(aImage);        
            }        
        }
    }
    
void CHuiFrameBrush::InitImagesL(TInt aNumberOfImages)
    {
    // Remove possible skin textures and make sure there is right amount of images in the array
    if (iFrameIID != KAknsIIDNone || iImages.Count() != aNumberOfImages)
        {            
        iImages.ResetAndDestroy();
        delete iImageloader;
        iImageloader = NULL;
        iFrameIID = KAknsIIDNone;

        for (TInt count = 0; count < aNumberOfImages; count++)
            {
            THuiImage newImage; 
            CHuiImageWithID* entry = new CHuiImageWithID(newImage,0); 
            if (entry)
                {
                CleanupStack::PushL(entry);
                iImages.AppendL( entry );
                CleanupStack::Pop();
                }        
            }            
        }
    }
    
TRect CHuiFrameBrush::AdjustedOuterRect(const TSize& aFullFrameSize) const
    {
    if (iOuterRect == TRect(0,0,0,0))
        {
        return TRect(TPoint(0,0), aFullFrameSize);    
        }
    return iOuterRect;    
    }

TRect CHuiFrameBrush::AdjustedInnerRect(const TSize& aFullFrameSize) const
    {
    if (iInnerRect == TRect(0,0,0,0))
        {
        TRect adjustedOuterRect = AdjustedOuterRect(aFullFrameSize);
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
    
