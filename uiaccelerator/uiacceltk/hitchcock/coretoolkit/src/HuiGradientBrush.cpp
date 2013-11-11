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
* Description:   Implementation of CHuiLinearGradientBrush - a Brush that is 
*                able to draw linear color gradients.
*
*/



#include "uiacceltk/HuiGradientBrush.h"  // Class definition
#include "uiacceltk/HuiDrawing.h"
#include "uiacceltk/HuiVisual.h"
#include "uiacceltk/HuiUtil.h"


EXPORT_C CHuiGradientBrush* CHuiGradientBrush::NewL()
    {
    CHuiGradientBrush* self = NewLC();
    CleanupStack::Pop();
    return self;
    }


EXPORT_C CHuiGradientBrush* CHuiGradientBrush::NewLC()
    {
    CHuiGradientBrush* self = new (ELeave) CHuiGradientBrush();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


CHuiGradientBrush::CHuiGradientBrush()
        : CHuiBrush(),
          iDirection(EDirectionRight)
    {
    }


void CHuiGradientBrush::ConstructL()
    {
    SColor defaultColor;
    defaultColor.iPosition = 0;
    defaultColor.iColor = KRgbWhite;
    defaultColor.iOpacity = 1.0;
    User::LeaveIfError(iColors.Append(defaultColor));
    }


CHuiGradientBrush::~CHuiGradientBrush()
    {
    iColors.Reset();
    }


EXPORT_C void CHuiGradientBrush::SetDirection(TDirection aDirection)
    {
    iDirection = aDirection;
    SetChanged();
    }
    

void CHuiGradientBrush::Draw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const
    {
    TRect content = aGuide.BrushRect();
    TReal32 opacity = aGuide.BrushOpacity() * iOpacity.Now();

    if(opacity <= 0)
        {
        return;
        }

    TBool useTexture = EFalse;
    TReal32 texCoords[8];
    THuiRealRect texCoordRect;

    aGc.SetPenAlpha(TInt(opacity * 255));

    if(iImage.HasTexture())
        {
        useTexture = ETrue;
        iImage.GetTexCoords(texCoords);
        aGc.Enable(CHuiGc::EFeatureTexturing);
        iImage.Texture().Bind();
        }
    else
        {
        aGc.Disable(CHuiGc::EFeatureTexturing);
        }

    // Optimization for gradientbrush (MNIA-7G79GD). Draws a simple rect with one color
    // if there really isn't anything to gradient. 
    // Updated optimization to work with single semi-transparent color
    if(iColors.Count() == 1 && !iImage.HasTexture() && (iColors[0].iOpacity * opacity) >= 1 )
        {
        aGc.SetPenColor(iColors[0].iColor);
        aGc.DrawRect(content);
        return;
        }

    aGc.Enable(CHuiGc::EFeatureBlending);

    TInt segments = iColors.Count() - 1;
    if(segments <= 0)
        {
        segments = 1;
        }

    TInt length = content.Width();
    TBool isHoriz = ETrue;
    TBool invert = EFalse;
    CHuiGc::TGradientType gradientType;

    switch(iDirection)
        {
        default:
        case EDirectionUp:
            gradientType = CHuiGc::EGradientLinearUp;
            isHoriz = EFalse;
            length = content.Height();
            invert = ETrue;
            break;

        case EDirectionDown:
            gradientType = CHuiGc::EGradientLinearDown;
            isHoriz = EFalse;
            length = content.Height();
            break;

        case EDirectionRight:
            gradientType = CHuiGc::EGradientLinearRight;
            break;

        case EDirectionLeft:
            gradientType = CHuiGc::EGradientLinearLeft;
            invert = ETrue;
            break;
        }

    for(TInt start = 0; start < segments; ++start)
        {
        TReal32 startPosition = iColors[start].iPosition;

        TInt end = start + 1;
        TReal32 endPosition;
        if(end >= iColors.Count())
            {
            end = iColors.Count() - 1;
            endPosition = 1;
            }
        else
            {
            endPosition = iColors[end].iPosition;
            }

        if(iColors[start].iOpacity <= 0 && iColors[end].iOpacity <= 0)
            {
            // Won't be visible.
            continue;
            }

        if(invert)
            {
            // Inverted direction.
            startPosition = 1 - startPosition;
            endPosition = 1 - endPosition;
            }

        TInt beginCoord = (isHoriz? content.iTl.iX : content.iTl.iY);
        TInt startCoord = beginCoord + TInt(startPosition * length);
        TInt endCoord = beginCoord + TInt(endPosition * length);

        TRect segmentRect;

        if(isHoriz)
            {
            segmentRect.iTl = TPoint(startCoord, content.iTl.iY);
            segmentRect.iBr = TPoint(endCoord, content.iBr.iY);
            }
        else
            {
            segmentRect.iTl = TPoint(content.iTl.iX, startCoord);
            segmentRect.iBr = TPoint(content.iBr.iX, endCoord);
            }

        // Calculate texture coordinates, if necessary.
        if(useTexture)
            {
            if(isHoriz)
                {
                texCoordRect.iTl.iX = HuiUtil::Interpolate(startPosition, texCoords[0], texCoords[2]);
                texCoordRect.iTl.iY = texCoords[3];
                texCoordRect.iBr.iX = HuiUtil::Interpolate(endPosition, texCoords[0], texCoords[2]);
                texCoordRect.iBr.iY = texCoords[5];
                }
            else
                {
                texCoordRect.iTl.iX = texCoords[0];
                texCoordRect.iTl.iY = HuiUtil::Interpolate(startPosition, texCoords[3], texCoords[5]);
                texCoordRect.iBr.iX = texCoords[2];
                texCoordRect.iBr.iY = HuiUtil::Interpolate(endPosition, texCoords[3], texCoords[5]);
                }
            }

        aGc.DrawGradient(gradientType, segmentRect,
                         iColors[start].iColor, iColors[end].iColor,
                         iColors[start].iOpacity * opacity,
                         iColors[end].iOpacity * opacity,
                         useTexture? &texCoordRect : NULL);                     
        }
    aGc.Disable(CHuiGc::EFeatureBlending);        
    }


EXPORT_C void CHuiGradientBrush::SetColor(const TRgb& aColor, TReal32 aOpacity) __SOFTFP
    {
    while(iColors.Count() > 1)
        {
        iColors.Remove(iColors.Count() - 1);
        }

    SColor color;
    color.iPosition = 0;
    color.iColor = aColor;
    color.iOpacity = aOpacity;

    iColors[0] = color;

    SetChanged();
    }


EXPORT_C TRgb CHuiGradientBrush::Color(TInt aIndex) const
    {
    // make sure aIndex is not out of bounds
    if ( aIndex < iColors.Count() )
        {
        return iColors[aIndex].iColor;
        }
    else
        {
        // default color always exists
        return iColors[0].iColor;
        }
    }


EXPORT_C TReal32 CHuiGradientBrush::Opacity(TInt aIndex) const __SOFTFP
    {
    // make sure aIndex is not out of bounds
    // make sure aIndex is not out of bounds
    if ( aIndex < iColors.Count() )
        {
        return iColors[aIndex].iOpacity;
        }
    else
        {
        // default color always exists
        return iColors[0].iOpacity;
        }
    }


EXPORT_C void CHuiGradientBrush::AppendColorL(TReal32 aPosition,
                                               const TRgb& aColor,
                                               TReal32 aOpacity) __SOFTFP
    {
    SColor color;

    color.iPosition = aPosition;
    color.iColor = aColor;
    color.iOpacity = aOpacity;
    User::LeaveIfError(iColors.Append(color));

    SetChanged();
    }


EXPORT_C void CHuiGradientBrush::SetImage(const THuiImage& aImage)
    {
    iImage = aImage;
    SetChanged();
    }


EXPORT_C const THuiImage& CHuiGradientBrush::Image() const
    {
    return iImage;
    }
