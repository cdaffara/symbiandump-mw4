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
* Description:   Implementation of CHuiImageVisual. CHuiImageVisual is a visual
*                that is able to display images.
*
*/



#include "uiacceltk/HuiImageVisual.h"  // Class definition
#include "uiacceltk/HuiStatic.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiDrawing.h"
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/huifixmath.h"
#include "HuiMatrixStack.h"

#include "uiacceltk/huidropshadow.h"
#include "uiacceltk/HuiDisplay.h"

#include "HuiRosterImpl.h"


EXPORT_C CHuiImageVisual* CHuiImageVisual::AddNewL(CHuiControl& aOwnerControl,
                                                   CHuiLayout* aParentLayout)
    {
    CHuiImageVisual* image = static_cast<CHuiImageVisual*>(
        aOwnerControl.AppendVisualL(EHuiVisualTypeImage, aParentLayout));
    return image;
    }


CHuiImageVisual::CHuiImageVisual(MHuiVisualOwner& aOwner)
        : CHuiVisual(aOwner), iScaleMode(EScaleFit),
          iStretchMode(CHuiGc::EStretchVertical), iColor(KRgbWhite),
          iScale(1.f),iOffset(.5f,.5f),iDropShadow(0)
    {
    iSecondaryAlpha.SetStyle(EHuiTimedValueStyleLinear);
    }


void CHuiImageVisual::ConstructL()
    {
    CHuiVisual::ConstructL();
    }


CHuiImageVisual::~CHuiImageVisual()
    {
    }


EXPORT_C void CHuiImageVisual::SetScaleMode(TScaleMode aScaleMode)
    {
    iScaleMode = aScaleMode;
    SetChanged();
    }


EXPORT_C void CHuiImageVisual::SetStretch(TInt aTop, TInt aBottom)
    {
    iTopStretch = aTop;
    iBottomStretch = aBottom;
    SetChanged();
    }


EXPORT_C void CHuiImageVisual::SetStretchMode(CHuiGc::TStretchMode aStretchMode)
    {
    iStretchMode = aStretchMode;
    SetChanged();
    }

// deprecated
EXPORT_C void CHuiImageVisual::SetDropShadow(TInt aShadowDistance)
    {
    SetDropShadow(THuiXYMetric(aShadowDistance));
    }
    
EXPORT_C void CHuiImageVisual::SetDropShadow(const THuiMetric& aShadowDistance)
	{
    SetDropShadow(THuiXYMetric(aShadowDistance));
	}

EXPORT_C void CHuiImageVisual::SetDropShadow(const THuiXYMetric& aShadowDistance)
	{
    iDropShadow = aShadowDistance;
    SetChanged();	
    
    if(!HuiUtil::RealCompare(iDropShadow.iX.iMagnitude,0.f) || !HuiUtil::RealCompare(iDropShadow.iY.iMagnitude,0.f))
        {
        TRAP_IGNORE( EnableDropShadowL( ETrue ) );
        }
    else
        {
        TRAP_IGNORE( EnableDropShadowL( EFalse ) );
        }

	}


EXPORT_C void CHuiImageVisual::SetImage(const THuiImage& aImage)
    {
    iImage = aImage;
    SetChanged();
    }


EXPORT_C void CHuiImageVisual::SetSecondaryImage(const THuiImage& aImage)
    {
    iSecondaryImage = aImage;
    SetChanged();
    }


EXPORT_C const THuiImage& CHuiImageVisual::Image() const
    {
    return iImage;
    }


EXPORT_C void CHuiImageVisual::SetColor(const TRgb& aColor)
    {
    iColor = aColor;
    SetChanged();
    }


EXPORT_C void CHuiImageVisual::SetColorMode(TColorMode aColorMode, TReal32 aParam) __SOFTFP
    {
    iColorMode = aColorMode;
    iColorParam = aParam;
    SetChanged();
    }

TBool CHuiImageVisual::PrepareDrawL()
    {
	if (Flags() & EHuiVisualFlagDrawOnlyAsExternalContent)
	   	{
	    // This is used only as external content visual. Return now if we are not currently drawing
	    // external content.
	   	if (!Display() || !Display()->RosterImpl().IsDrawingExternalContent())
	   		{
			return ETrue;
	   		}
	   	}

    if ( iImage.HasTexture() )
        {   
        CHuiDropShadow* shadowHandler = DropShadowHandler();
        if ( shadowHandler && shadowHandler->IsShadowVisible() )
            {
            CHuiTexture* shadowTextureSource = shadowHandler->ShadowedTextureInstance( *iImage.ImageTexture() );
            if ( shadowTextureSource )
                {        
                const TInt requestedBlurredSize = HUI_ROUND_FLOAT_TO_INT( 2*DropShadowHandler()->iRadius.Now() );
                shadowTextureSource->CreateShadowTextureL( requestedBlurredSize, EHuiTextureShadowStyleIcon );
                }
            }
        }
    return ETrue;       
    }

void CHuiImageVisual::DrawSelf(CHuiGc& aGc, const TRect& /*aDisplayRect*/) const
    {
    TReal32 effectiveOpacity = EffectiveOpacity();

    THuiRealRect content = DisplayRect();
    content.Shrink(PaddingInPixels());

    if(!iImage.HasTexture() || effectiveOpacity <= 0)
        {        
        aGc.NotifyImagePreferredSize(iImage, content);
        return;
        }

    if(content.Width() <= 0 || content.Height() <= 0)
        {
        // Too small to see.
        return;
        }

    aGc.Enable(CHuiGc::EFeatureBlending);

    TReal32 turnAngle = iTurnAngle.Now();
    TReal32 xOffset = 0;
    TReal32 yOffset = 0;

    TReal32 scale = HuiUtil::CalculateScaleFactorFromScaleMode(
                content.Size(),
                iImage.Texture().Size(),
                iScaleMode,
                iScale.Now() );

    // Determine offset.
    TSize imageSize = iImage.Texture().Size();
    TReal32 scaled = scale * imageSize.iWidth;
    if(scaled > content.Width())
        {
        xOffset = (scaled - content.Width()) * (iOffset.iX.Now() - 0.5f);
        }
    scaled = scale * imageSize.iHeight;
    if(scaled > content.Height())
        {
        yOffset = (scaled - content.Height()) * (iOffset.iY.Now() - 0.5f);
        }

    if(turnAngle != 0 || scale != 1)
        {
        /** @todo  GC must provide some support for transformations. */
        aGc.Push(EHuiGcMatrixModel);

        // Rotate around the midpoint of the visual.
        THuiRealPoint mid = content.Center();

        aGc.Translate(EHuiGcMatrixModel, mid.iX - xOffset, mid.iY - yOffset, 0.f);
        if(turnAngle != 0)
            {
            aGc.Rotate(EHuiGcMatrixModel, turnAngle, 0.f, 0.f, -1.f);
            }
        if(scale != 1)
            {
            aGc.Scale(EHuiGcMatrixModel, scale, scale, 1.f);
            }
        aGc.Translate(EHuiGcMatrixModel, -mid.iX, -mid.iY, 0.f);
        }

    // Use the appropriate alignment in the graphics context.
    if(iScaleMode == EScaleFit)
        {
        aGc.SetAlign(EHuiAlignHLeft, EHuiAlignVTop);
        }
    else
        {
        aGc.SetAlign(EHuiAlignHCenter, EHuiAlignVCenter);
        }
   
    // draw drop shadow
    CHuiDropShadow* shadowHandler = DropShadowHandler();
    if ( shadowHandler && iImage.HasTexture() )
        {
        CHuiTexture* shadowTextureSource = shadowHandler->ShadowedTextureInstance( *iImage.ImageTexture() );
        if ( iImage.ImageTexture() &&
             shadowTextureSource &&
             shadowHandler->IsShadowVisible() )
            {
            const TInt requestedBlurredSize = HUI_ROUND_FLOAT_TO_INT( 2*DropShadowHandler()->iRadius.Now() );
            
            THuiTextureHandle shadow;
            TBool haveShadowTexture = shadowTextureSource->GetShadowTexture( shadow,requestedBlurredSize );

            if ( haveShadowTexture )
                {
                THuiImage shadowImage( shadow );
                const THuiRealRect shadowDrawingRect = shadowHandler->ShadowDrawingRealRect( 
                    content.iTl,
                    content.Size(),
                    shadow.Size(),
                    *this );
                
                // set drawing parameters
                aGc.SetPenAlpha(TInt(effectiveOpacity * shadowHandler->iOpacity.Now()*255.f));
                aGc.SetPenColor(shadowHandler->Color());
                const THuiQuality oldQuality = aGc.Quality(); // store the original quality
                aGc.SetQuality(EHuiQualityFast); // draw always with 'fast'
                
                if(iScaleMode == EScaleFit)
                    {
                    aGc.DrawImage(shadowImage, shadowDrawingRect.iTl, shadowDrawingRect.Size() );
                    }
                else
                    {
                    THuiRealSize relativeShadowTextureSize = iImage.Texture().Size();
                    relativeShadowTextureSize.iWidth *= ((TReal32)shadowDrawingRect.Width())/((TReal32)content.Size().iWidth);
                    relativeShadowTextureSize.iHeight *= ((TReal32)shadowDrawingRect.Height())/((TReal32)content.Size().iHeight);

                    aGc.DrawImage(
                        shadowImage, 
                        relativeShadowTextureSize,
                        THuiRealRect( shadowDrawingRect.iTl, shadowDrawingRect.Size() ));
                       
                    }
                    
                aGc.SetQuality(oldQuality); // restore quality
                }
            }
        }
        
    // Actual color of the image.
    aGc.SetPenAlpha(TInt(effectiveOpacity * 255));
    aGc.SetPenColor(iColor);

    // Apply special color mode.
    if(iColorMode == EColorDimmed)
        {
        aGc.Enable(CHuiGc::EFeatureFog);
        aGc.SetDimmingFog(iColor, iColorParam);
        }

    if(iTopStretch || iBottomStretch)
        {
        aGc.DrawStretchImage(iStretchMode, iImage, content,
                             iTopStretch, iBottomStretch);
        }
    else
        {
        if(aGc.TextureUnits() >= 2 &&
           CHuiStatic::Renderer().Allows(EHuiRenderPluginAllowTextureModeInterpolate) &&
           iImage.HasTexture() && iSecondaryImage.HasTexture() &&
           (iImage.SegmentCount() == iSecondaryImage.SegmentCount() && 
           (iImage.Texture().Size() == iSecondaryImage.Texture().Size() || iImage.SegmentCount() == 1)))
            {
            // Can use multitexturing to interpolate.
            aGc.SetTextureUnits(2);
            aGc.Enable(CHuiGc::EFeatureTexturing);
            aGc.SetTextureMode(CHuiGc::ETextureModeInterpolate,
                               iSecondaryAlpha.Now());
            aGc.SetPenAlpha(TInt(effectiveOpacity * 255));

            if(iScaleMode == EScaleFit)
                {
                // Scale both images to the content rectangle.
                aGc.DrawImages(iImage, iSecondaryImage,
                               content.iTl, content.Size());
                }
            else
                {
                // Draw both images by taking the size of the first one.
                aGc.DrawImages(iImage, iSecondaryImage,
                               iImage.Texture().Size(), content);
                }

            aGc.SetTextureUnits(1);
            aGc.SetTextureMode(CHuiGc::ETextureModeNormal);
            }
        else
            {
            // Must draw using two passes.
            TInt primaryAlpha = 0;
            TInt secondaryAlpha = 0;
            GetDualAlpha(effectiveOpacity, iSecondaryAlpha.Now(),
                         primaryAlpha, secondaryAlpha);

            // Draw both the primary and the secondary images.
            if(primaryAlpha > 0)
                {
                aGc.SetPenAlpha(primaryAlpha);
                if(iScaleMode == EScaleFit)
                    {
                    aGc.DrawImage(iImage, content.iTl, content.Size());
                    }
                else
                    {
                    aGc.DrawImage(iImage, content);
                    }
                }

            if(secondaryAlpha > 0 && iSecondaryImage.HasTexture())
                {
                aGc.SetPenAlpha(secondaryAlpha);
                if(iScaleMode == EScaleFit)
                    {
                    aGc.DrawImage(iSecondaryImage, content.iTl, content.Size());
                    }
                else
                    {
                    aGc.DrawImage(iSecondaryImage, iImage.Texture().Size(), content);
                    }
                }
            else
                {
                // Notify preferred size even if image is not drawn (yet)
                aGc.NotifyImagePreferredSize(iSecondaryImage, content);                    
                }
            }
        }

    // Disable special color modes.
    if(iColorMode == EColorDimmed)
        {
        aGc.Disable(CHuiGc::EFeatureFog);
        }

    // Restore original transformation.
    if(turnAngle != 0 || scale != 1)
        {
        aGc.Pop(EHuiGcMatrixModel);
        }
    }


TBool CHuiImageVisual::Changed() const
    {
    if(CHuiVisual::Changed())
        {
        return ETrue;
        }
        
	if(iImage.Changed())
		{
		return ETrue;
		}
		        
    return (iTurnAngle.Changed() || iOffset.Changed() || iScale.Changed() ||
            iSecondaryAlpha.Changed());
    }


void CHuiImageVisual::ClearChanged()
    {
    CHuiVisual::ClearChanged();
    iImage.ClearChanged();
    iTurnAngle.ClearChanged();
    iOffset.ClearChanged();
    iScale.ClearChanged();
    iSecondaryAlpha.ClearChanged();
    }

void DoRotateRect(THuiRealRect& aRect, TReal32 aAngle)
    {
    // Transform dirty rect and enlarge it if rotated.    
    THuiRealRect transformed(aRect);
    
    THuiRealPoint coords[4];
    coords[0].iX = transformed.iTl.iX;
    coords[0].iY = transformed.iTl.iY;
    coords[1].iX = transformed.iBr.iX;
    coords[1].iY = transformed.iTl.iY;
    coords[2].iX = transformed.iBr.iX;
    coords[2].iY = transformed.iBr.iY;
    coords[3].iX = transformed.iTl.iX;
    coords[3].iY = transformed.iBr.iY;
    

    TInt radians = HuiFixMath::FixMul(HuiFixMath::FloatToFix(aAngle),KFixPi)/180;
    if (aAngle < 0)
        {
        radians = -radians;
        }

    TInt sin = HuiFixMath::FixSin(radians);
    TInt cos = HuiFixMath::FixCos(radians);
        
        
    CHuiMatrixStack::TMatrix rotation;
    rotation.iMatrix[0] = HuiFixMath::FixToFloat(cos);
    rotation.iMatrix[1] = HuiFixMath::FixToFloat(sin);
    rotation.iMatrix[3] = HuiFixMath::FixToFloat(-sin);
    rotation.iMatrix[4] = HuiFixMath::FixToFloat(cos);
    rotation.iMatrix[8] = 1.0f;
    rotation.iIsIdentity = EFalse;

    for (TInt count = 0; count < 4; count++)
        {
        rotation.Multiply(coords[count]);
        }

    // rotate only if there is really rotation, not mirroring
    
    if ( !HuiUtil::RealCompare(coords[2].iY, coords[3].iY) )
        {
        // rotation has been applied
        THuiRealPoint minPoint = coords[0];
        THuiRealPoint maxPoint = coords[0];
        for (TInt count = 1; count < 4; count++)
            {
            if (coords[count].iX < minPoint.iX)
                {
                minPoint.iX = coords[count].iX;
                }
            if (coords[count].iY < minPoint.iY)
                {
                minPoint.iY = coords[count].iY;
                }
            if (coords[count].iX > maxPoint.iX)
                {
                maxPoint.iX = coords[count].iX;
                }
            if (coords[count].iY > maxPoint.iY)
                {
                maxPoint.iY = coords[count].iY;
                }
            }
				
        transformed.iTl = minPoint;
        transformed.iBr = maxPoint;
        }
    else
        {
        transformed.iTl = coords[0];
        transformed.iBr = coords[2];            
        }    
        
    aRect = transformed;
    }


void CHuiImageVisual::ExpandRectWithContent(TRect& aRect) const
    {
    if(!Clipping())
        {
        if( iImage.HasTexture() )
            {
            TSize imageSize = iImage.Texture().Size(); 

            // Handle scale mode
            if (iScaleMode == EScaleNormal && 
                 ( imageSize.iWidth > aRect.Width() || imageSize.iHeight > aRect.Height()))
                {
                const TInt growX = Max( 0, ((imageSize.iWidth - aRect.Width()) + 1) / 2 );
                const TInt growY = Max( 0, ((imageSize.iHeight - aRect.Height()) + 1) / 2 );
                aRect.Grow( growX, growY );
                }
            else if (iScaleMode == EScaleFitWidth)
                {
                TReal32 scale = HuiUtil::CalculateScaleFactorFromScaleMode(
                    aRect.Size(),
                    imageSize,
                    iScaleMode,
                    1.f );
                TInt scaled = (TReal32)imageSize.iHeight*scale+0.5f;
                if (scaled > aRect.Height())
                    {
                    aRect.Grow(0,(((scaled-aRect.Height()) +1)/2));
                    }
                }
            else if (iScaleMode == EScaleFitHeight)
                {
                TReal32 scale = HuiUtil::CalculateScaleFactorFromScaleMode(
                    aRect.Size(),
                    imageSize,
                    iScaleMode,
                    1.f );
                TInt scaled = (TReal32)imageSize.iWidth*scale+0.5f;
                if (scaled > aRect.Width())
                    {
                    aRect.Grow((((scaled-aRect.Width()) +1)/2),0);                    
                    }
                }
            else if (iScaleMode == EScaleCover || iScaleMode == EScaleFitInside)
                {
                TReal32 scale = HuiUtil::CalculateScaleFactorFromScaleMode(
                    aRect.Size(),
                    imageSize,
                    iScaleMode,
                    1.f );
                TInt scaledW = (TReal32)imageSize.iWidth*scale+0.5f;
                TInt scaledH = (TReal32)imageSize.iHeight*scale+0.5f;

                if (scaledW > aRect.Width())
                    {
                    aRect.Grow((((scaledW-aRect.Width()) +1)/2),0);                    
                    }
                if (scaledH > aRect.Height())
                    {
                    aRect.Grow(0,(((scaledH-aRect.Height()) +1)/2));
                    }
                }
            else
                {
                // For PC-lint
                }
            }
               
        // Calculate scaled rect.
        // aRect and scaled rect has same center position. 
        TRect adjusted;
        TReal32 scale = iScale.Now();
        TReal32 offsetX = TReal32(aRect.Width()) * scale / 2.f;
        TReal32 offsetY = TReal32(aRect.Height()) * scale / 2.f;

        adjusted.iTl.iX = HUI_ROUND_FLOAT_TO_INT((aRect.iTl.iX + aRect.Width()/2.f) - offsetX);
        adjusted.iTl.iY = HUI_ROUND_FLOAT_TO_INT((aRect.iTl.iY + aRect.Height()/2.f) - offsetY);
        adjusted.iBr.iX = HUI_ROUND_FLOAT_TO_INT((aRect.iBr.iX - aRect.Width()/2.f) + offsetX);
        adjusted.iBr.iY = HUI_ROUND_FLOAT_TO_INT((aRect.iBr.iY - aRect.Height()/2.f) + offsetY);

        // aRect must not become smaller.
        aRect.BoundingRect( adjusted );
        
        // calculate drop shadow
        CHuiDropShadow* shadowHandler = DropShadowHandler();
        if ( iImage.HasTexture() &&
            shadowHandler &&
            shadowHandler->IsShadowVisible() )
            {
            CHuiTexture* shadowTextureSource = shadowHandler->ShadowedTextureInstance( *iImage.ImageTexture() );
            if ( shadowTextureSource )
                {
                    
                const TInt requestedBlurredSize = HUI_ROUND_FLOAT_TO_INT( 2*shadowHandler->iRadius.Now() );
                
                THuiTextureHandle shadow;
                TBool haveShadowTexture = shadowTextureSource->GetShadowTexture( shadow,requestedBlurredSize );
                
                if ( haveShadowTexture )
                    {
                    const TRect shadowRect = shadowHandler->ShadowDrawingTRect( 
                        aRect.iTl,
                        aRect.Size(),
                        shadow.Size(),
                        *this );
                         
                    // aRect must not become smaller.
                    aRect.BoundingRect( shadowRect );
                    }
                }
            }

        // add the turn angle effect
        if ( iImage.HasTexture() && iTurnAngle.Now() )
            {
            // Take turn angle into accout by rotating aRect.
            THuiRealRect rotated( THuiRealPoint(0.f,0.f), aRect.Size() );
            rotated.Move( -rotated.Center().iX, -rotated.Center().iY );
            DoRotateRect( rotated, iTurnAngle.Now() );
            TSize rotatedSize = rotated.Size();
            // Size rounds down, compensate by increasing by one.
            rotatedSize.iWidth++;
            rotatedSize.iHeight++;
            
            // Define rotatedRect: same center point as aRect, different size
            TRect rotatedRect( rotatedSize );
            rotatedRect.Move( 
                aRect.Center() - 
                TPoint( ( rotatedSize.iWidth + 1 )/2, ( rotatedSize.iHeight + 1 )/2 ) );
            rotatedRect.iBr += TPoint( 1, 1 ); // two possible center
            
            // aRect must not become smaller.
            aRect.BoundingRect( rotatedRect );
            }
        
        // Apparently, there is two pixel error in above 
        // (occurs already before scaling, fit width/fit height cases).
        aRect.Grow( 2, 2 );
        }

    CHuiVisual::ExpandRectWithContent(aRect);
    }
