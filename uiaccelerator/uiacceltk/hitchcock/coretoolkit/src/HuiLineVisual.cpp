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
* Description:   Implementation of CHuiLineVisual, Visual that is able to draw 
*                lines onto a curve path.
*
*/



#include "uiacceltk/HuiLineVisual.h"  // Class definition
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiDrawing.h"
#include "uiacceltk/HuiUtil.h"


EXPORT_C CHuiLineVisual* CHuiLineVisual::AddNewL(CHuiControl& aOwnerControl,
                                                 CHuiLayout* aParentLayout)
    {
    CHuiLineVisual* line = STATIC_CAST(CHuiLineVisual*,
        aOwnerControl.AppendVisualL(EHuiVisualTypeLine, aParentLayout));
    return line;
    }


CHuiLineVisual::CHuiLineVisual(MHuiVisualOwner& aOwner)
        : CHuiVisual(aOwner), iColor(KRgbWhite)
    {
    iThickness.Set(12.f);
    iEndPos.Set(1.f);
    }


void CHuiLineVisual::ConstructL()
    {
    CHuiVisual::ConstructL();
    
    // Use the default line texture by default.
    iImage = THuiImage(Env().Skin().TextureL(EHuiSkinLineTexture));
    }


CHuiLineVisual::~CHuiLineVisual()
    {
    iAlphaFunction = NULL;
    iWidthFunction = NULL;
    }


/*EXPORT_C void CHuiLineVisual::LayoutToPath()
    {
    TRect bounds(TPoint(0, 0), TSize(1, 1));

    for(TInt i = 0; i < iPath.Count(); ++i)
        {
        bounds.BoundingRect(TRect(iPath[i].iPoint, TSize(1, 1)));
        }

    SetSize(bounds.Size());
    }*/


EXPORT_C void CHuiLineVisual::SetPath(CHuiCurvePath* aPath,
                                      THuiOwnership aOwnership)
    {
    iPath.Set(aPath, aOwnership);
    SetChanged();
    }


EXPORT_C CHuiCurvePath& CHuiLineVisual::Path()
    {
    SetChanged();
    return iPath.Ref();
    }


void CHuiLineVisual::DrawSelf(CHuiGc& aGc, const TRect& aDisplayRect) const
    {
    if(!iPath.Ptr())
        {
        // If the path is empty, ie has no data, return without drawing anything.
        return;
        }

    aGc.Enable(CHuiGc::EFeatureBlending);
    aGc.SetPenAlpha(TInt(EffectiveOpacity() * 255));
    aGc.SetPenColor(iColor);

    // By default, use the current thickness for the entire path.
    if ( !HuiUtil::RealCompare( iDefaultWidthFunction.iValue, iThickness.Now() ) )
        {
        iPath.NonConstRef().SetNeedsUpdate();
        iDefaultWidthFunction.iValue = iThickness.Now();
        }

	aGc.Enable(CHuiGc::EFeatureTexturing);
	if(iImage.HasTexture())
		{
		iImage.Texture().Bind();
		}

    iPath->SetVisual(this);
    aGc.DrawPath(iPath.Ref(), aDisplayRect.iTl,
                 iStartPos.Now(), iEndPos.Now(),
                 iAlphaFunction,
                 iWidthFunction == 0? &iDefaultWidthFunction : 0);
    iPath->SetVisual(NULL);
    }


TBool CHuiLineVisual::Changed() const
    {
    if(CHuiVisual::Changed())
        {
        return ETrue;
        }

    return iThickness.Changed() || iShadowThickness.Changed() ||
           iStartPos.Changed() || iEndPos.Changed() || iImage.Changed();
    }


void CHuiLineVisual::ClearChanged()
    {
    CHuiVisual::ClearChanged();
    iThickness.ClearChanged();
    iShadowThickness.ClearChanged();
    iStartPos.ClearChanged();
    iEndPos.ClearChanged();
    iImage.ClearChanged();
    }


EXPORT_C void CHuiLineVisual::SetImage(const THuiImage& aImage)
    {
    iImage = aImage;
    SetChanged();
    }


EXPORT_C void CHuiLineVisual::SetAlphaFunction(MHuiMappingFunction* aFunction)
    {
    iAlphaFunction = aFunction;
    SetChanged();
    }


EXPORT_C void CHuiLineVisual::SetWidthFunction(MHuiMappingFunction* aFunction)
    {
    iWidthFunction = aFunction;
    SetChanged();
    }
    
EXPORT_C void CHuiLineVisual::SetColor(const TRgb& aColor)
    {
	iColor = aColor;
    SetChanged();
    }
    

void CHuiLineVisual::ExpandRectWithContent(TRect& aRect) const
    {
    if(!iPath.Ptr())
        {
        // If the path is empty, ie has no data, return directly
        return;
        }
    
    const TInt segmentCount = iPath->SegmentCount();    
    if(!Clipping() && segmentCount)
        {
            
        // calculate each segment
        THuiRealRect segmentsCoverRectInMetrics;
        for ( TInt i = 0 ; i < segmentCount ; i++ )
            {
            const CHuiCurvePath::TSegment segment = iPath->Segment(i);
            THuiRealRect segmentAreaInMetrics;
            
            if ( segment.iType == CHuiCurvePath::ESegmentTypeLine )
                {
                segmentAreaInMetrics = THuiRealRect( 
                    THuiRealPoint( segment.iOrigin.iX,segment.iOrigin.iY ),
                    THuiRealSize( segment.iDelta.AsSize().iWidth, segment.iDelta.AsSize().iHeight ) );
                }
            else // ESegmentTypeArc
                {
                segmentAreaInMetrics = THuiRealRect( 
                    THuiRealPoint( 
                        segment.iOrigin.iX - segment.iDelta.AsSize().iWidth,
                        segment.iOrigin.iY - segment.iDelta.AsSize().iHeight ),
                    THuiRealPoint( 
                        segment.iOrigin.iX + segment.iDelta.AsSize().iWidth,
                        segment.iOrigin.iY + segment.iDelta.AsSize().iHeight ));        
                }
               
            if ( i == 0 )
                {
                // first rect
                segmentsCoverRectInMetrics = segmentAreaInMetrics;
                }
            else
                {
                // append to previous rect combination
                segmentsCoverRectInMetrics.BoundingRect( segmentAreaInMetrics );
                }
            }
        
        // Change metrics to pixels
        THuiRealRect segmentsCoverRectInPixels( 
            LocalPointInPixels( segmentsCoverRectInMetrics.iTl ),
            LocalPointInPixels( segmentsCoverRectInMetrics.iBr ) );
        
        // check that both height and width != zero    
        if ( !HuiUtil::RealCompare( segmentsCoverRectInPixels.Height(), 0.f ) && 
             !HuiUtil::RealCompare( segmentsCoverRectInPixels.Width(), 0.f ) )
            {
            // move by offset: assume that iOffset is in pixels
            const THuiRealPoint offsetNowInPixels = iPath->iOffset.Now();
            segmentsCoverRectInPixels.Move( offsetNowInPixels.iX,offsetNowInPixels.iY);
            
            // move into relation of the visual area (aRect in pixels)
            segmentsCoverRectInPixels.Move( aRect.iTl.iX, aRect.iTl.iY );
            
            // add the thickness
            THuiRealSize thicknessInPixels = LocalPointInPixels(THuiRealPoint( iThickness.Now()/2.f, iThickness.Now()/2.f )).AsSize();
            
            // Take the average of the x and y dimensions - what would be the correct behaviour?
            const TInt averageThincknessInPixels = 
                HUI_ROUND_FLOAT_TO_INT( (
                    thicknessInPixels.iWidth + 
                    thicknessInPixels.iHeight
                    )/2.f );
  
            segmentsCoverRectInPixels.Grow( averageThincknessInPixels, averageThincknessInPixels );
            
            // Finally round to TRect
            const TRect finalSegmentsCoverArea(
                HUI_ROUND_FLOAT_TO_INT(segmentsCoverRectInPixels.iTl.iX),
                HUI_ROUND_FLOAT_TO_INT(segmentsCoverRectInPixels.iTl.iY),
                HUI_ROUND_FLOAT_TO_INT(segmentsCoverRectInPixels.iBr.iX),
                HUI_ROUND_FLOAT_TO_INT(segmentsCoverRectInPixels.iBr.iY) );
                
            // combine the calculated segment area and the 
            aRect.BoundingRect(finalSegmentsCoverArea);
            }
        }
        
    CHuiVisual::ExpandRectWithContent(aRect);
    }
    
