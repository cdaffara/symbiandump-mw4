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
* Description:   OpenVG class for canvas graphics context
*
*/

#include <e32math.h>

#include "huivg10canvasgc.h"
#include "huicanvasgc.h"
#include "HuiVg10Gc.h"
#include "uiacceltk/HuiCanvasVisual.h"
#include "uiacceltk/HuiSkin.h"
#include "uiacceltk/HuiS60Skin.h"
#include <uiacceltk/huidisplaybackgrounditem.h>
#include "HuiRenderSurface.h"
#include "uiacceltk/HuiDisplay.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiUtil.h"
#ifndef __SERIES60_30__     
#include "HuiTargetBitmap.h"
#endif

#include <AknsRendererWrapper.h>
#include <AknsDrawUtils.h>
#include <coecntrl.h>
#include "huiskinbackroundlayout.h"
#include "huivg10canvasrenderbuffer.h"
#include "HuiFxVg10.h"

CHuiVg10CanvasGc::CHuiVg10CanvasGc()
    {
    }

CHuiVg10CanvasGc::~CHuiVg10CanvasGc()
    {	
    }

void CHuiVg10CanvasGc::DoDrawPolygon(RArray<THuiRealPoint>& aPoints)
    {
    if(iPolygonDrawMode == EHuiNoFill)
    	{
    	CHuiCanvasGc::DoDrawPolygon(aPoints);
    	}
    else
        {
        CHuiVg10Gc* vg10Gc = (CHuiVg10Gc *)iGc;
        vg10Gc->SetFillRule( iPolygonDrawMode );
        vg10Gc->SetPaintPattern(iPolygonFillTexture, iPolygonFillTextureOrigin);
        vg10Gc->DrawPolygon(aPoints);                
        vg10Gc->SetPaintPattern(NULL, iPolygonFillTextureOrigin);
        }	    
    }

void CHuiVg10CanvasGc::DoDrawArc(const THuiRealRect& aDestinationRect, 
       const THuiRealPoint& aStart, const THuiRealPoint& aEnd) 
   {
   DoDrawPieAndArc( aDestinationRect, aStart, aEnd, EFalse);
   }
   
void CHuiVg10CanvasGc::DoDrawPie(const THuiRealRect& aDestinationRect, 
        const THuiRealPoint& aStart, const THuiRealPoint& aEnd)
    {
    DoDrawPieAndArc( aDestinationRect, aStart, aEnd, ETrue);
    }


void CHuiVg10CanvasGc::DoDrawPieAndArc(const THuiRealRect& aDestinationRect, 
      const THuiRealPoint& aStart, const THuiRealPoint& aEnd, TBool aIsPie)  
    {    
    THuiRealRect destinationRect = aDestinationRect;
    destinationRect.Shrink(0.5f, 0.5f);    
    THuiRealPoint center = destinationRect.Center();

    // Ok, explaining this is not going to be easy. So, please pay attention!
    //
    // Symbian Window Gc DrawArc() method always draws an arc anti-clockwise, from start angle
    // to end angle. As y-axis in Window GC point downwards, the angle grows towards *negative*
    // direction, if we go anti-clockwise. To be able to make calculations later we have to make 
    // sure both the start and end angles are negative.
    // 
    TReal startAngle = 0;
    Math::ATan(startAngle, aStart.iY - center.iY,  aStart.iX - center.iX);
    startAngle = (startAngle*180)/KPi;   
    // make sure this is negative angle
    if (startAngle > - 0.000001) 
        {
        startAngle -= 360.0; 
        }
        
    TReal endAngle = 0;
    Math::ATan(endAngle, aEnd.iY - center.iY, aEnd.iX - center.iX);
    endAngle = (endAngle*180)/KPi;   
    // make sure this is negative angle
    if (endAngle > - 0.000001) 
        {
        endAngle -= 360.0; 
        }

    // Make a full circle if the end and start angles are equal
    if ( Abs(endAngle-startAngle) < 0.000001)
        {
        endAngle -= 360.0;
        }

    // Organize angles so that we go from start to end angle anti-clockwise, i.e. towards negative direction
    if ( endAngle > startAngle )
        {
        endAngle -= 360;
        }

    CHuiVg10Gc* vg10Gc = (CHuiVg10Gc *)iGc;
    vg10Gc->SetPaintPattern(iPolygonFillTexture, iPolygonFillTextureOrigin);
    vg10Gc->DrawArc(aDestinationRect.Round(), iPolygonDrawMode, iPenWidth, startAngle, endAngle, aIsPie);
    vg10Gc->SetPaintPattern(NULL, iPolygonFillTextureOrigin);  
    }

 void CHuiVg10CanvasGc::DoDrawRoundRect(const THuiRealRect& aDestinationRect, const THuiRealSize& aSize)
     {
     CHuiVg10Gc* vg10Gc = (CHuiVg10Gc *)iGc;
     vg10Gc->SetPaintPattern(iPolygonFillTexture, iPolygonFillTextureOrigin);
     vg10Gc->DrawRoundRect(aDestinationRect.Round(), aSize, iPolygonDrawMode, iPenWidth);
     vg10Gc->SetPaintPattern(NULL, iPolygonFillTextureOrigin);
     }
 
void CHuiVg10CanvasGc::ClearWithSkinBackground(const THuiRealRect& /*aRect*/) 
    {
    if (!iGc)
        {
        return;    
        }

    // Acquire background texture
    const CHuiTexture* backgroundTexture = NULL;
    TInt err = CHuiStatic::Env().Skin().GetTexture(EHuiSkinBackgroundTexture, backgroundTexture);
    if(err)
        {
        return;
        }

    // Apply background texture
    THuiImage background(*backgroundTexture);

    TPoint screenOrigin(0, 0);
    CHuiStatic::CurrentRenderSurface()->GetScreenOrigin(screenOrigin);
    // The origin is used to offset the background in the display's
    // rendering surface, so that the background's origin is in the
    // top left screen corner.
    screenOrigin.iX = -screenOrigin.iX;
    screenOrigin.iY = -screenOrigin.iY;

    iGc->SetPenColor(KRgbWhite);
    iGc->SetPenAlpha(255);
    iGc->SetAlign(EHuiAlignHLeft, EHuiAlignVTop);            
    iGc->Disable(CHuiGc::EFeatureBlending);
    iGc->Disable(CHuiGc::EFeatureClipping);            
    iGc->DrawImage(background, screenOrigin, background.Texture().Size()); 
    iGc->Enable(CHuiGc::EFeatureBlending);
    iGc->Enable(CHuiGc::EFeatureClipping);            
    }

void CHuiVg10CanvasGc::ClearWithBackgroundItems(const THuiRealRect& /*aRect*/, const RArray<THuiDisplayBackgroundItem>& aItems)
    {
    if (!iGc)
        return;

    THuiDisplayBackgroundItem item;   
    CHuiS60Skin* s60skin = static_cast<CHuiS60Skin*>(&CHuiStatic::Env().Skin());
    CHuiTexture* backgroundTexture = NULL;

    for (TInt index = 0; index < aItems.Count(); index++)
        {        
        item = aItems[index];    
        switch (item.ClearMode())
            {
            case CHuiDisplay::EClearNone:
                // do nothing...
                break;
            case CHuiDisplay::EClearWithColor:
                iGc->SetPenColor(item.Color());
                iGc->SetPenAlpha(TInt(item.Color().Alpha() * 255));
                iGc->SetAlign(EHuiAlignHLeft, EHuiAlignVTop);            
                iGc->Disable(CHuiGc::EFeatureBlending);
                iGc->Enable(CHuiGc::EFeatureClipping);            
                iGc->PushClip();
                iGc->Clip(item.Rect());  // takes transformations into account                              
                iGc->Clear();
                iGc->PopClip();
                break;
            case CHuiDisplay::EClearWithSkinBackground:
                 backgroundTexture = s60skin->BackgroundTexture(item.SkinBackground());
                 if (backgroundTexture)
                    {
                    TRect skinRect;
                    TRect dummy;
                    GetRectForItem(item.SkinBackground(), dummy, skinRect);

                    THuiImage background(*backgroundTexture);

                    TPoint screenOrigin(0, 0);
                    CHuiStatic::CurrentRenderSurface()->GetScreenOrigin(screenOrigin);
                    screenOrigin.iX = -screenOrigin.iX;
                    screenOrigin.iY = -screenOrigin.iY;
                    screenOrigin+=skinRect.iTl;
                    iGc->SetPenColor(KRgbWhite);
                    iGc->SetPenAlpha(255);
                    iGc->SetAlign(EHuiAlignHLeft, EHuiAlignVTop);            
                    iGc->Disable(CHuiGc::EFeatureBlending);
                    iGc->Enable(CHuiGc::EFeatureClipping);            
                    skinRect.Intersection(item.Rect());
                    iGc->PushClip();
                    iGc->Clip(skinRect); // takes transformations into account
                    iGc->DrawImage(background, screenOrigin, background.Texture().Size());                     
                    iGc->PopClip();
                    }
                 break;                                                    
             }
        }             
    }

TInt CHuiVg10CanvasGc::MaxNumberOfClipRects() const
    {
    // We could ask this number from OpenVg, but spec says that at least 32 must be supported
    return 32;    
    }

CHuiCanvasRenderBuffer* CHuiVg10CanvasGc::CreateRenderBufferL(const TSize& /*aSize*/)
    {
    CHuiVg10CanvasRenderBuffer* buffer = new (ELeave) CHuiVg10CanvasRenderBuffer;
    return buffer;
    }

void CHuiVg10CanvasGc::DoDrawRenderBuffer(const CHuiCanvasRenderBuffer& aImage, const THuiRealPoint& aDestinationPoint)
    {
    const CHuiVg10CanvasRenderBuffer* vg10RenderBuffer = (const CHuiVg10CanvasRenderBuffer*) &aImage;    
    
    CHuiVg10Gc* vg10Gc = (CHuiVg10Gc *)iGc;
    if (vg10RenderBuffer->Image())
        {
        vg10Gc->UpdateColor();
        iGc->Push(EHuiGcMatrixModel);
        vgTranslate(aDestinationPoint.iX, aDestinationPoint.iY);
        vgDrawImage(vg10RenderBuffer->Image());
        iGc->Pop(EHuiGcMatrixModel);
        }
    }

void CHuiVg10CanvasGc::DoClearRenderBuffer( CHuiCanvasRenderBuffer& aImage, const TRect & aRect )
    {
    HUIFX_VG_INVARIANT();   
    const CHuiVg10CanvasRenderBuffer* vg10RenderBuffer = (const CHuiVg10CanvasRenderBuffer*) &aImage;    

    const TInt COLOR_COMPONENTS = 4;
    VGfloat savedColor[COLOR_COMPONENTS];
    vgGetfv(VG_CLEAR_COLOR, COLOR_COMPONENTS, savedColor);
    
    VGfloat color[COLOR_COMPONENTS] = 
                {
                 0.0f, 0.0f, 0.0f, 0.0f
                };
    vgSetfv(VG_CLEAR_COLOR, COLOR_COMPONENTS, color);
    vgClearImage(vg10RenderBuffer->Image(), aRect.iTl.iX, aRect.iTl.iY, aRect.Size().iWidth, aRect.Size().iHeight);
    vgSetfv(VG_CLEAR_COLOR, COLOR_COMPONENTS, savedColor);
    HUIFX_VG_INVARIANT();   
    }


void CHuiVg10CanvasGc::RestoreFlaggedState() const
    {
    if (iGc)
        {
        CHuiVg10Gc* vg10Gc = (CHuiVg10Gc *)iGc;
        vg10Gc->RestoreFlaggedState();
        }
    }

void CHuiVg10CanvasGc::DoDrawRects(RArray<THuiRealRect>& aRects)
    {
    if(iPolygonDrawMode == EHuiNoFill)
        {
        CHuiCanvasGc::DoDrawRects(aRects);
        }
    else
        {
        for(TInt i=0; i<aRects.Count(); i++)
            {
            TRect roundedRect = aRects[i].Round();
            if (roundedRect.Height() > 0 && roundedRect.Width() > 0)
                {                   
                CHuiVg10Gc* vg10Gc = (CHuiVg10Gc *)iGc;
                vg10Gc->SetPaintPattern(iPolygonFillTexture, iPolygonFillTextureOrigin);
                iGc->DrawRect(roundedRect);
                vg10Gc->SetPaintPattern(NULL, iPolygonFillTextureOrigin);
                }           
            }            
        }
    }

void CHuiVg10CanvasGc::DrawEllipse(const THuiRealRect& aDestinationRect)
    {   
    if (!iGc)
       {
       return;    
       }

    Setup();

    EnableDelayedClippingIfNeeded(aDestinationRect);    

    while (ClipNext())
        {
        DoDrawEllipse( aDestinationRect );
        }

    DisableDelayedClippingIfNeeded();    

    Cleanup();
    }

void CHuiVg10CanvasGc::DoDrawEllipse(const THuiRealRect& aDestinationRect)
    {   
    CHuiVg10Gc* vg10Gc = (CHuiVg10Gc *)iGc;
    vg10Gc->SetPaintPattern(iPolygonFillTexture, iPolygonFillTextureOrigin);
    vg10Gc->DrawEllipse(aDestinationRect.Round(), iPolygonDrawMode, iPenWidth);
    vg10Gc->SetPaintPattern(NULL, iPolygonFillTextureOrigin);                
    }
