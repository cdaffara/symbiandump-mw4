/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition of CHuiCanvasWsHwGc.
*
*/



#include "huicanvaswshwgc.h"

#include "uiacceltk/HuiCanvasVisual.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiGc.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiVisual.h"
#include "uiacceltk/HuiTextMesh.h"
#include "uiacceltk/HuiCanvasCmdBufferReader.h"
#include "huicanvasgc.h"
#include "HuiRenderSurface.h"
#include "huicanvastexturecache.h"
#include "huicanvasbackground.h"
#include <graphics/wsgraphicscontext.h>
#include <e32cmn.h>
#include <AknLayoutFont.h>
#include <graphics/lookuptable.h>

//#define HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL

CHuiCanvasWsHwGc* CHuiCanvasWsHwGc::NewL()
	{
	CHuiCanvasWsHwGc* self    = new ( ELeave ) CHuiCanvasWsHwGc;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

void CHuiCanvasWsHwGc::AdjustCoordinates(TPoint& aPoint, TBool aUseWsOrigin )
    {
    aPoint +=  iPosDelta;
    if ( aUseWsOrigin ) 
        {
        aPoint += iWsOrigin;
        }
    }

void CHuiCanvasWsHwGc::AdjustCoordinates(TRect& aRect, TBool aUseWsOrigin )
    {
    aRect.Move( iPosDelta );
    if ( aUseWsOrigin )
        {
        aRect.Move( iWsOrigin );
        }
    }

void CHuiCanvasWsHwGc::DrawTextBrush( THuiRealRect& aRect )
    {
    if( iWsBrushStyle == CGraphicsContext::ESolidBrush )
        {
        TRgb oldPenColor = iCanvasGc->PenColor();
        TReal32 oldOpacity = iCanvasGc->Opacity();
        THuiFillMode oldFillMode = iCanvasGc->PolygonDrawMode();
        
        iCanvasGc->SetPenColor(iWsBrushColor);
        iCanvasGc->SetOpacity(TReal32(iWsBrushColor.Alpha() / 255.f));   
        iCanvasGc->SetPolygonDrawMode(EHuiFillEvenOdd);
        
        RArray<THuiRealRect> rects;
        rects.Append(aRect);
        iCanvasGc->DrawRects(rects);
        rects.Close();
        
        iCanvasGc->SetPolygonDrawMode(oldFillMode);
        iCanvasGc->SetOpacity(oldOpacity);
        iCanvasGc->SetPenColor(oldPenColor);
        }
    }

CHuiCanvasWsHwGc::CHuiCanvasWsHwGc()
    {
    }

void CHuiCanvasWsHwGc::ConstructL()
	{
    iCanvasGc = CHuiStatic::Renderer().CreateCanvasGcL();
	
	// Set delayed clipping mode, this is supposed to reduce unnecessary
	// clipping which may cause bad performance on hw renderers 
	iCanvasGc->SetClippingMode(EHuiCanvasClipModeDelayed);
	WsResetL();
#ifdef __WINS__	
	iPushCount = 0;
#endif
	}

CHuiCanvasWsHwGc::~CHuiCanvasWsHwGc()
    {
    if (iVisual)
        {
        iVisual->Env().CanvasTextureCache().ReleaseCachedRenderBuffer(*iVisual);
        }
                
    iTempRegion.Close();
    iWsClipRegion.Close();
    delete iCanvasGc;
    iCanvasGc = NULL;
    }
    
void CHuiCanvasWsHwGc::WsBitBltL(TInt aBitmapHandle, TPoint aPoint)
	{
	THuiCachedImageParams cachedImageParams;
    cachedImageParams.iBitmapHandle = aBitmapHandle;
    cachedImageParams.iGcParams = CachedGcParams();
     
    const CHuiCanvasGraphicImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedImageL(cachedImageParams,*iVisual);

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            AdjustCoordinates( aPoint );
            THuiRealRect destinationRect = TRect(aPoint, texture->Size());          
    
            THuiCanvasDrawMode oldDrawMode = iCanvasGc->DrawMode();
            iCanvasGc->SetDrawMode(SelectCanvasDrawMode(iWsDrawMode));        
            // Draw            
            iCanvasGc->DrawImage(*texture, destinationRect, TRect(TPoint(0,0), texture->Size()), CHuiGc::EStretchNone);
            iCanvasGc->SetDrawMode(oldDrawMode);
            }
        }
	}




void CHuiCanvasWsHwGc::WsBitBltRectL(TInt aBitmapHandle, TPoint aPoint, TRect aRect)
	{
	THuiCachedImageParams cachedImageParams;
    cachedImageParams.iBitmapHandle = aBitmapHandle;
    cachedImageParams.iGcParams = CachedGcParams();
     
    const CHuiCanvasGraphicImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedImageL(cachedImageParams,*iVisual);

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            AdjustCoordinates( aPoint );
            THuiRealRect destinationRect = TRect(aPoint, texture->Size());                

            THuiCanvasDrawMode oldDrawMode = iCanvasGc->DrawMode();
            iCanvasGc->SetDrawMode(SelectCanvasDrawMode(iWsDrawMode));        

            // Draw            
            iCanvasGc->DrawImage(*texture, destinationRect, aRect, CHuiGc::EStretchNone);
            iCanvasGc->SetDrawMode(oldDrawMode);
            }        
        }
	}



void CHuiCanvasWsHwGc::WsBitBltMaskedL(TInt aBitmapHandle, TInt aMaskHandle, TInt aInvertMask, TPoint aPoint, TRect aRect)
	{
	THuiCachedImageParams cachedImageParams;
    cachedImageParams.iBitmapHandle = aBitmapHandle;
    cachedImageParams.iMaskHandle = aMaskHandle;
    cachedImageParams.iInvertedMask = aInvertMask;
    cachedImageParams.iGcParams = CachedGcParams();
     
     
    const CHuiCanvasGraphicImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedImageL(cachedImageParams,*iVisual);

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            AdjustCoordinates( aPoint );
            THuiRealRect destinationRect = TRect(aPoint, texture->Size());                
            
            // Draw            
            iCanvasGc->DrawImage(*texture, destinationRect, aRect, CHuiGc::EStretchNone);
            }
        }
	}


void CHuiCanvasWsHwGc::WsCombinedBitBltMaskedL(TRect aDestinationRect, const RArray<THuiCachedCombinedImageParams>& aBlits )
	{
	THuiCachedCombinedImageParams cachedCombinedImageParams;	    
    THuiCachedGcParams gcParams = CachedGcParams();          
    TSize imageSize = aDestinationRect.Size();
    
    // Must convert points relative to the image itself
    RArray<THuiCachedCombinedImageParams> convertedBlits;
    for (TInt i=0; i<aBlits.Count();i++)
        {
        THuiCachedCombinedImageParams convertedParams;
        convertedParams = aBlits[i];
        convertedParams.iCombinedBitmapPoint -= aDestinationRect.iTl;
        convertedBlits.Append(convertedParams);
        }    
    
    const CHuiCanvasGraphicImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCombinedCachedImageL(
        convertedBlits, 
        gcParams,
        imageSize, 
        *iVisual);

    convertedBlits.Close();

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();        
        if (texture)
            {
            TPoint point = aDestinationRect.iTl;
            AdjustCoordinates( point );
            
            THuiRealRect destinationRect = TRect(point, texture->Size());                        
    
            // Draw            
            iCanvasGc->DrawImage(*texture, destinationRect, TRect(TPoint(0,0), texture->Size()),  CHuiGc::EStretchNone);
            }
        }
	}


void CHuiCanvasWsHwGc::WsBitBltMaskedPointL(TInt aBitmapHandle, TInt aMaskHandle, TPoint aPoint1, TPoint aPoint2, TRect aRect)
	{
	THuiCachedImageParams cachedImageParams;
    cachedImageParams.iBitmapHandle = aBitmapHandle;
    cachedImageParams.iMaskHandle = aMaskHandle;
    cachedImageParams.iMaskOriginPoint = aPoint2;
    cachedImageParams.iGcParams = CachedGcParams();

    const CHuiCanvasGraphicImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedImageL(cachedImageParams,*iVisual);

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            AdjustCoordinates( aPoint1 );
            THuiRealRect destinationRect = TRect(aPoint1, texture->Size());                
            
            // Draw            
            iCanvasGc->DrawImage(*texture, destinationRect, aRect, CHuiGc::EStretchNone);
            }             
        }
	}

void CHuiCanvasWsHwGc::WsResetClippingRegionL()
	{
    if (IsRenderingEnabled())
        {
        // Reset clip region
        iWsClipRegion.Clear();
        iCanvasGc->CancelClipping();
        }
	}

void CHuiCanvasWsHwGc::WsClearL()
	{		
    if (IsRenderingEnabled())
        {
        THuiRealRect rect(iVisual->DisplayRect());     
        DoWsClearRectL(rect);
        }
	}

void CHuiCanvasWsHwGc::WsClearRectL(TRect& aRect)
	{		
    if (IsRenderingEnabled())
        {
        AdjustCoordinates( aRect );
        DoWsClearRectL(aRect);
        }
	}


void CHuiCanvasWsHwGc::DoWsClearRectL(const THuiRealRect& aRect )
	{
    if (IsRenderingEnabled())
        {
        RArray<THuiRealRect> rects;
        rects.Append(aRect);
    
        // Store current gc params
        UseWsState();
        
        CHuiTexture* oldPolygonFillTexture = iCanvasGc->PolygonFillTexture(); 
        
        // Clear
        iCanvasGc->SetPolygonFillTexture(NULL);
        iCanvasGc->SetPolygonDrawMode(EHuiFillNonZero);
        iCanvasGc->SetPenWidth(1);           	  	        
        iCanvasGc->SetPenColor(iWsBrushColor);            
        iCanvasGc->SetOpacity(TReal32(iWsBrushColor.Alpha() / 255.f));   
        iCanvasGc->SetDrawMode(SelectCanvasDrawMode(iWsDrawMode));        
        iCanvasGc->DrawRects(rects);
    
        // Restore gc params etc.
        rects.Close();
        UseCanvasState();
        iCanvasGc->SetPolygonFillTexture(oldPolygonFillTexture);
        }
    }

void CHuiCanvasWsHwGc::WsResetBrushPatternL()
	{
	iWsBrushPattern = 0;
	}

void CHuiCanvasWsHwGc::WsResetFontL()
	{
    iWsCurrentFont = 0;
	}

void CHuiCanvasWsHwGc::WsDrawArcL(TPoint aPoint1, TPoint aPoint2, TRect aRect)
	{
	// If penstyle is ENullPen, then nothign is drawn
    if (!iWsPenStyle)
        {
        return;    
        }

    if (IsRenderingEnabled())
        {
        UseWsState();        

        AdjustCoordinates( aRect );
        AdjustCoordinates( aPoint1 );
        AdjustCoordinates( aPoint2 );

        iCanvasGc->DrawArc( THuiRealRect(aRect), THuiRealPoint(aPoint1), THuiRealPoint(aPoint2) );
        
        UseCanvasState();
        }
    }

void CHuiCanvasWsHwGc::WsDrawPieL(TPoint aPoint1, TPoint aPoint2, TRect aRect)
	{
	// If penstyle is ENullPen, then nothign is drawn
    if (!iWsPenStyle && !iWsBrushStyle)
        {
        return;    
        }

    if (IsRenderingEnabled())
        {    
        UseWsState();

        AdjustCoordinates( aRect );
        AdjustCoordinates( aPoint1 );
        AdjustCoordinates( aPoint2 );
            
        if (iWsBrushStyle == MWsGraphicsContext::ENullBrush)
            {
            iCanvasGc->SetPolygonDrawMode( EHuiNoFill );
            iCanvasGc->SetPenColor(iWsPenColor);            
            iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));       
            iCanvasGc->DrawPie(THuiRealRect(aRect),THuiRealPoint(aPoint1),THuiRealPoint(aPoint2));  
            }    
        else if (iWsBrushStyle == MWsGraphicsContext::ESolidBrush)
            {
            iCanvasGc->SetPolygonDrawMode( EHuiFillEvenOdd );
            iCanvasGc->SetPenColor(iWsBrushColor);            
            iCanvasGc->SetOpacity(TReal32(iWsBrushColor.Alpha() / 255.f));            
            iCanvasGc->DrawPie(THuiRealRect(aRect),THuiRealPoint(aPoint1),THuiRealPoint(aPoint2));  

            // Border should be drawn if pencolor is different than solid fill
            if (IsDifferent(iWsBrushColor, iWsPenColor, ETrue) && iWsPenStyle && iWsPenSize.iHeight >= 1 && iWsPenSize.iWidth >= 1)
                {
                iCanvasGc->SetPolygonDrawMode( EHuiNoFill );
                iCanvasGc->SetPenColor(iWsPenColor);                    
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));       
                iCanvasGc->DrawPie(THuiRealRect(aRect),THuiRealPoint(aPoint1),THuiRealPoint(aPoint2));  
                }            
            }
        else if (iWsBrushStyle == MWsGraphicsContext::EPatternedBrush)
            {
            iCanvasGc->SetPolygonDrawMode( EHuiFillEvenOdd );
            iCanvasGc->SetPenColor(iWsPenColor);                    
            iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));       
            UseBrushPattern();
            iCanvasGc->SetDrawMode(SelectCanvasDrawMode(iWsDrawMode));        
            iCanvasGc->DrawPie(THuiRealRect(aRect),THuiRealPoint(aPoint1),THuiRealPoint(aPoint2));  
            DiscardBrushPattern();

            if (iWsPenStyle && iWsPenSize.iHeight >= 1 && iWsPenSize.iWidth >= 1)
                {
                iCanvasGc->SetPolygonDrawMode( EHuiNoFill );
                iCanvasGc->SetPenColor(iWsPenColor);                    
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));       
                iCanvasGc->DrawPie(THuiRealRect(aRect),THuiRealPoint(aPoint1),THuiRealPoint(aPoint2));  
                }
            }
        else
            {
            // We should never get here because fallbackmode should be used in this case !
            RDebug::Print(_L("CHuiCanvasWsHwGc::WsDrawPieL - Unsupported brush mode %i"), iWsBrushStyle);
            }
                
        UseCanvasState();
        }
	}

void CHuiCanvasWsHwGc::WsDrawBitmap1L(TInt aBitmapHandle, TRect aRect)
	{
	THuiCachedImageParams cachedImageParams;
    cachedImageParams.iBitmapHandle = aBitmapHandle;
    cachedImageParams.iGcParams = CachedGcParams();

    const CHuiCanvasGraphicImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedImageL(cachedImageParams,*iVisual);
    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            AdjustCoordinates( aRect );
            THuiRealRect destinationRect = aRect;                
            
            // Draw            
            // This should stretch !
            iCanvasGc->DrawImage(*texture, destinationRect, TRect(TPoint(0,0), texture->Size()), CHuiGc::EStretchFull); 
            }        	
        }
	}

void CHuiCanvasWsHwGc::WsDrawBitmap2L(TInt aBitmapHandle, TRect aRect1, TRect aRect2)
	{
	THuiCachedImageParams cachedImageParams;
    cachedImageParams.iBitmapHandle = aBitmapHandle;
    cachedImageParams.iGcParams = CachedGcParams();

    const CHuiCanvasGraphicImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedImageL(cachedImageParams,*iVisual);

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            AdjustCoordinates( aRect1 );
            THuiRealRect destinationRect = aRect1;                
            
            // Draw            
            iCanvasGc->DrawImage(*texture, destinationRect, aRect2, CHuiGc::EStretchFull); 
            }        
        }
	}

void CHuiCanvasWsHwGc::WsDrawBitmap3L(TInt aBitmapHandle, TPoint aPoint)
	{
	 THuiCachedImageParams cachedImageParams;
    cachedImageParams.iBitmapHandle = aBitmapHandle;
    cachedImageParams.iGcParams = CachedGcParams();

    const CHuiCanvasGraphicImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedImageL(cachedImageParams,*iVisual);

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            AdjustCoordinates( aPoint );
            THuiRealRect destinationRect = TRect(aPoint, texture->Size());                
       
            // Draw
            // TODO: Actually we should check the bitmap size in twips here are decide destinationRect based on that !            
            iCanvasGc->DrawImage(*texture, destinationRect, TRect(TPoint(0,0), texture->Size()), CHuiGc::EStretchNone);
            }        
        }
	}

void CHuiCanvasWsHwGc::WsDrawBitmapMaskedL(TInt aBitmapHandle, TInt aMaskHandle, TInt aInvertMask, TRect& aRect1, TRect& aRect2)
	{
    THuiCachedImageParams cachedImageParams;
    cachedImageParams.iBitmapHandle = aBitmapHandle;
    cachedImageParams.iMaskHandle = aMaskHandle;
    cachedImageParams.iInvertedMask = aInvertMask;
    cachedImageParams.iGcParams = CachedGcParams();
    
    const CHuiCanvasGraphicImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedImageL(cachedImageParams,*iVisual);

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            AdjustCoordinates( aRect1 );
            THuiRealRect destinationRect = aRect1;              
    
            // Draw            
            iCanvasGc->DrawImage(*texture, destinationRect, aRect2, CHuiGc::EStretchFull);
            }        
        }
	}

void CHuiCanvasWsHwGc::WsDrawRoundRectL(TPoint aPoint, TRect& aRect)
	{
	// If penstyle is ENullPen, then nothign is drawn
    if (!iWsPenStyle && !iWsBrushStyle)
        {
        return;    
        }

    if (IsRenderingEnabled())
        {    
        UseWsState();
        
        AdjustCoordinates( aRect );
        iCanvasGc->SetDrawMode(SelectCanvasDrawMode(iWsDrawMode));        
        
        if (iWsBrushStyle == MWsGraphicsContext::ENullBrush)
            {
            iCanvasGc->SetPolygonDrawMode( EHuiNoFill );
            iCanvasGc->SetPenColor(iWsPenColor);            
            iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));       
            iCanvasGc->DrawRoundRect( THuiRealRect(aRect), THuiRealSize( aPoint.iX*2, aPoint.iY*2 ));
            }    
        else if (iWsBrushStyle == MWsGraphicsContext::ESolidBrush)
            {
            iCanvasGc->SetPolygonDrawMode( EHuiFillEvenOdd );
            iCanvasGc->SetPenColor(iWsBrushColor);            
            iCanvasGc->SetOpacity(TReal32(iWsBrushColor.Alpha() / 255.f));            
            iCanvasGc->DrawRoundRect( THuiRealRect(aRect), THuiRealSize( aPoint.iX*2, aPoint.iY*2 ));

            // Border should be drawn if pencolor is different than solid fill
            if (IsDifferent(iWsBrushColor,iWsPenColor, ETrue) && iWsPenStyle && iWsPenSize.iHeight >= 1 && iWsPenSize.iWidth >= 1)
                {
                iCanvasGc->SetPolygonDrawMode( EHuiNoFill );
                iCanvasGc->SetPenColor(iWsPenColor);                    
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));       
                iCanvasGc->DrawRoundRect( THuiRealRect(aRect), THuiRealSize( aPoint.iX*2, aPoint.iY*2 ));
                }            
            }
        else if (iWsBrushStyle == MWsGraphicsContext::EPatternedBrush)
            {
            iCanvasGc->SetPolygonDrawMode( EHuiFillEvenOdd );
            iCanvasGc->SetPenColor(iWsBrushColor);                    
            iCanvasGc->SetOpacity(TReal32(iWsBrushColor.Alpha() / 255.f));       
            UseBrushPattern();
            iCanvasGc->SetDrawMode(SelectCanvasDrawMode(iWsDrawMode));        
            iCanvasGc->DrawRoundRect( THuiRealRect(aRect), THuiRealSize( aPoint.iX*2, aPoint.iY*2 ));
            DiscardBrushPattern();

            if (iWsPenStyle && iWsPenSize.iHeight >= 1 && iWsPenSize.iWidth >= 1)
                {
                iCanvasGc->SetPolygonDrawMode( EHuiNoFill );
                iCanvasGc->SetPenColor(iWsPenColor);                    
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));       
                iCanvasGc->DrawRoundRect( THuiRealRect(aRect), THuiRealSize( aPoint.iX*2, aPoint.iY*2 ));
                }
            }
        else
            {
            // We should never get here because fallbackmode should be used in this case !
            RDebug::Print(_L("CHuiCanvasWsHwGc::WsDrawRoundRectL - Unsupported brush mode %i"), iWsBrushStyle);
            }
        UseCanvasState();        
        }
    }

void CHuiCanvasWsHwGc::WsDrawPolyLineL(CArrayFix<TPoint>* aPointerArray)
	{
	// If penstyle is ENullPen, then nothign is drawn
    if (!iWsPenStyle)
        {
        return;    
        }
    
    if (IsRenderingEnabled())
        {    
        if ( aPointerArray )
            {
            UseWsState();
                
            RArray<THuiRealLine> lines;
            TPoint finalDeltaPos = iWsOrigin + iPosDelta; 
            for(TInt j=0; j<aPointerArray->Count()-1; j++)
                {    
                THuiRealLine line(aPointerArray->At(j) + finalDeltaPos, aPointerArray->At(j+1) + finalDeltaPos);
                lines.Append(line);        
                }
            iCanvasGc->DrawLines(lines);
            lines.Close();  
            
            UseCanvasState();
            }
        }
    }

void CHuiCanvasWsHwGc::WsDrawPolyLineNoEndPointL(TPoint* aPointerArray, TInt aCount)
	{
	// If penstyle is ENullPen, then nothign is drawn
    if (!iWsPenStyle)
        {
        return;    
        }
	
    if (IsRenderingEnabled())
        {    
        if ( aPointerArray )
           {
           UseWsState();
           
           RArray<THuiRealLine> lines;
           TPoint finalDeltaPos = iWsOrigin + iPosDelta; 
           for(TInt j=0; j< aCount-1; j++)
               {          
               THuiRealLine line(aPointerArray[j] + finalDeltaPos, aPointerArray[j+1] + finalDeltaPos );// TODO: TEST
               lines.Append(line);         
               }
           iCanvasGc->DrawLines(lines);
    
           RArray<THuiRealRect> rects;
           THuiRealRect pointRect = THuiRealRect(aPointerArray[aCount],aPointerArray[aCount]);
           rects.AppendL(pointRect);
           iCanvasGc->SetPenWidth(0);
           iCanvasGc->DrawRects(rects);
           rects.Close();
           lines.Close();       
           
           UseCanvasState();
           }
       }      
    }

void CHuiCanvasWsHwGc::WsDrawPolygonL(CArrayFix<TPoint>* aPointerArray, TInt aFillRule)
	{
    if (IsRenderingEnabled())
        {    	
        RArray<THuiRealPoint> points;
        for(TInt j=0; j<aPointerArray->Count(); j++)
           {      
           points.Append( THuiRealPoint( aPointerArray->At(j) + iWsOrigin + iPosDelta) );        
           }
        UseWsState();
        
        if (iWsBrushStyle == MWsGraphicsContext::ENullBrush)
            {
            // nullbrush, but pen style set
            // -> draw non filled polygon and 
            // use pencolor
            iCanvasGc->SetPolygonDrawMode( EHuiNoFill );
            iCanvasGc->SetPenColor(iWsPenColor);            
            iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));       

            }
        else
            {
            if ( aFillRule == MWsGraphicsContext::EAlternate )
                {
                iCanvasGc->SetPolygonDrawMode( EHuiFillEvenOdd );
                }
            if ( aFillRule == MWsGraphicsContext::EWinding )
                {        
                iCanvasGc->SetPolygonDrawMode( EHuiFillNonZero );
                }
            iCanvasGc->SetPenColor(iWsBrushColor);            
            iCanvasGc->SetOpacity(TReal32(iWsBrushColor.Alpha() / 255.f));       
            }
            
        iCanvasGc->SetDrawMode(SelectCanvasDrawMode(iWsDrawMode));        
            
        iCanvasGc->DrawPolygon( points );
    
        UseCanvasState();
    
        points.Close();        
        }	
	}

void CHuiCanvasWsHwGc::WsDrawEllipseL(TRect& aRect)
	{
	// If penstyle is ENullPen, then nothign is drawn
    if (!iWsPenStyle && !iWsBrushStyle)
        {
        return;    
        }

    if (IsRenderingEnabled())
        {        
        AdjustCoordinates( aRect );
        THuiRealRect destRect = aRect;
        UseWsState();
        
        if (iWsBrushStyle == MWsGraphicsContext::ENullBrush)
            {
            iCanvasGc->SetPolygonDrawMode( EHuiNoFill );
            iCanvasGc->SetPenColor(iWsPenColor);            
            iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));       
            iCanvasGc->DrawEllipse(destRect);
            }    
        else if (iWsBrushStyle == MWsGraphicsContext::ESolidBrush)
            {
            iCanvasGc->SetPolygonDrawMode( EHuiFillEvenOdd );
            iCanvasGc->SetPenColor(iWsBrushColor);            
            iCanvasGc->SetOpacity(TReal32(iWsBrushColor.Alpha() / 255.f));            
            iCanvasGc->DrawEllipse(destRect);

            // Border should be drawn if pencolor is different than solid fill
            if (IsDifferent(iWsBrushColor,iWsPenColor, ETrue) && iWsPenStyle && iWsPenSize.iHeight >= 1 && iWsPenSize.iWidth >= 1)
                {
                iCanvasGc->SetPolygonDrawMode( EHuiNoFill );
                iCanvasGc->SetPenColor(iWsPenColor);                    
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));       
                iCanvasGc->DrawEllipse(destRect);
                }            
            }
        else if (iWsBrushStyle == MWsGraphicsContext::EPatternedBrush)
            {
            iCanvasGc->SetPolygonDrawMode( EHuiFillEvenOdd );
            iCanvasGc->SetPenColor(iWsBrushColor);                    
            iCanvasGc->SetOpacity(TReal32(iWsBrushColor.Alpha() / 255.f));       
            UseBrushPattern();
            iCanvasGc->SetDrawMode(SelectCanvasDrawMode(iWsDrawMode));        
            iCanvasGc->DrawEllipse(destRect);
            DiscardBrushPattern();

            if (iWsPenStyle && iWsPenSize.iHeight >= 1 && iWsPenSize.iWidth >= 1)
                {
                iCanvasGc->SetPolygonDrawMode( EHuiNoFill );
                iCanvasGc->SetPenColor(iWsPenColor);                    
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));       
                iCanvasGc->DrawEllipse(destRect);
                }
            }
        else
            {
            // We should never get here because fallbackmode should be used in this case !
            RDebug::Print(_L("CHuiCanvasWsHwGc::WsDrawEllipseL - Unsupported brush mode %i"), iWsBrushStyle);
            }
    
        UseCanvasState();
        }
	}

void CHuiCanvasWsHwGc::WsDrawLineL(TPoint& aStart, TPoint& aEnd)
	{
	// If penstyle is ENullPen, then nothign is drawn
    if (!iWsPenStyle)
        {
        return;    
        }

    if (IsRenderingEnabled())
        {            
        AdjustCoordinates( aStart );
        AdjustCoordinates( aEnd );
        
        THuiRealLine line(aStart, aEnd);
        RArray<THuiRealLine> lines;
        lines.Append(line);        
    
        UseWsState();
    
        iCanvasGc->DrawLines( lines );
        lines.Close();
    
        UseCanvasState();
        }
    }

void CHuiCanvasWsHwGc::WsDrawLineToL(TPoint& aPoint)
	{
	// If penstyle is ENullPen, then nothign is drawn
    if (!iWsPenStyle)
        {
        return;    
        }
	
    if (IsRenderingEnabled())
        {            
        AdjustCoordinates( aPoint );
        TPoint currentPosition = iWsPenPosition;	
        THuiRealLine line(currentPosition, aPoint);
        RArray<THuiRealLine> lines;
        lines.Append(line);        
    
        UseWsState();
    
        iCanvasGc->DrawLines( lines );
            
        iWsPenPosition = aPoint;       
        lines.Close();        
    
        UseCanvasState();
        }
	}
    
void CHuiCanvasWsHwGc::WsDrawLineByL(TPoint& aPoint)
	{
	// If penstyle is ENullPen, then nothign is drawn
    if (!iWsPenStyle)
        {
        return;    
        }
	
    if (IsRenderingEnabled())
        {            
        AdjustCoordinates( aPoint );
        TPoint currentPosition = iWsPenPosition;
        TPoint endPoint = currentPosition + aPoint;
        THuiRealLine line(currentPosition, endPoint);    
        RArray<THuiRealLine> lines;
        lines.Append(line);        
    
        UseWsState();
    
        iCanvasGc->DrawLines( lines );
    
        iWsPenPosition = endPoint;
        lines.Close();    
    
        UseCanvasState();
        }
	}
    
void CHuiCanvasWsHwGc::WsDrawRectL(TRect& aRect)
	{
	// If penstyle is ENullPen, then nothign is drawn
    if (!iWsPenStyle && !iWsBrushStyle)
        {
        return;    
        }

    if (IsRenderingEnabled())
        {                
        AdjustCoordinates( aRect );
        RArray<THuiRealRect> rects;
        rects.AppendL(aRect);        
        
        UseWsState();
   
        if (iWsBrushStyle == MWsGraphicsContext::ENullBrush)
            {
            iCanvasGc->SetPenColor(iWsPenColor);                    
            iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));       
            iCanvasGc->SetDrawMode(SelectCanvasDrawMode(iWsDrawMode));        
            iCanvasGc->DrawRects(rects);
            }    
        else if (iWsBrushStyle == MWsGraphicsContext::ESolidBrush)
            {
            iCanvasGc->SetPolygonDrawMode( EHuiFillEvenOdd );
            iCanvasGc->SetPenColor(iWsBrushColor);            
            iCanvasGc->SetOpacity(TReal32(iWsBrushColor.Alpha() / 255.f));            
            iCanvasGc->SetDrawMode(SelectCanvasDrawMode(iWsDrawMode));        
            iCanvasGc->DrawRects(rects);

            
            // Border should be drawn if pencolor is different than solid fill
            if (IsDifferent(iWsBrushColor,iWsPenColor, ETrue) && iWsPenStyle && iWsPenSize.iHeight >= 1 && iWsPenSize.iWidth >= 1)
                {
                iCanvasGc->SetPolygonDrawMode( EHuiNoFill );
                iCanvasGc->SetPenColor(iWsPenColor);                    
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));       
                iCanvasGc->DrawRects(rects);                
                }            
            }
        else if (iWsBrushStyle == MWsGraphicsContext::EPatternedBrush)
            {
            iCanvasGc->SetPolygonDrawMode( EHuiFillEvenOdd );
            iCanvasGc->SetPenColor(iWsBrushColor);                    
            iCanvasGc->SetOpacity(TReal32(iWsBrushColor.Alpha() / 255.f));       
            UseBrushPattern();
            iCanvasGc->SetDrawMode(SelectCanvasDrawMode(iWsDrawMode));        
            iCanvasGc->DrawRects(rects);
            DiscardBrushPattern();

            if (iWsPenStyle && iWsPenSize.iHeight >= 1 && iWsPenSize.iWidth >= 1)
                {
                iCanvasGc->SetPolygonDrawMode( EHuiNoFill );
                iCanvasGc->SetPenColor(iWsPenColor);                    
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));       
                iCanvasGc->DrawRects(rects);                                
                }
            }
        else
            {
            // We should never get here because fallbackmode should be used in this case !
            RDebug::Print(_L("CHuiCanvasWsHwGc::WsDrawRectL - Unsupported brush mode %i"), iWsBrushStyle);
            }
        
        rects.Close();
        
        UseCanvasState();
        }
    }
	
const TUint8 color_s_to_lin[256] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
   0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
   0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
   0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04,
   0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05,
   0x05, 0x06, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07,
   0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x0a,
   0x0a, 0x0a, 0x0b, 0x0b, 0x0c, 0x0c, 0x0c, 0x0d,
   0x0d, 0x0e, 0x0e, 0x0e, 0x0f, 0x0f, 0x10, 0x10,
   0x11, 0x11, 0x12, 0x12, 0x12, 0x13, 0x13, 0x14,
   0x15, 0x15, 0x16, 0x16, 0x17, 0x17, 0x18, 0x18,
   0x19, 0x1a, 0x1a, 0x1b, 0x1b, 0x1c, 0x1d, 0x1d,
   0x1e, 0x1f, 0x1f, 0x20, 0x21, 0x21, 0x22, 0x23,
   0x23, 0x24, 0x25, 0x26, 0x26, 0x27, 0x28, 0x29,
   0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2d, 0x2e, 0x2f,
   0x30, 0x31, 0x32, 0x33, 0x33, 0x34, 0x35, 0x36,
   0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e,
   0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
   0x47, 0x48, 0x49, 0x4a, 0x4c, 0x4d, 0x4e, 0x4f,
   0x50, 0x51, 0x52, 0x54, 0x55, 0x56, 0x57, 0x58,
   0x5a, 0x5b, 0x5c, 0x5d, 0x5f, 0x60, 0x61, 0x63,
   0x64, 0x65, 0x67, 0x68, 0x69, 0x6b, 0x6c, 0x6d,
   0x6f, 0x70, 0x72, 0x73, 0x74, 0x76, 0x77, 0x79,
   0x7a, 0x7c, 0x7d, 0x7f, 0x80, 0x82, 0x83, 0x85,
   0x86, 0x88, 0x8a, 0x8b, 0x8d, 0x8e, 0x90, 0x92,
   0x93, 0x95, 0x97, 0x98, 0x9a, 0x9c, 0x9d, 0x9f,
   0xa1, 0xa3, 0xa4, 0xa6, 0xa8, 0xaa, 0xac, 0xad,
   0xaf, 0xb1, 0xb3, 0xb5, 0xb7, 0xb8, 0xba, 0xbc,
   0xbe, 0xc0, 0xc2, 0xc4, 0xc6, 0xc8, 0xca, 0xcc,
   0xce, 0xd0, 0xd2, 0xd4, 0xd6, 0xd8, 0xda, 0xdc,
   0xde, 0xe0, 0xe2, 0xe5, 0xe7, 0xe9, 0xeb, 0xed,
   0xef, 0xf2, 0xf4, 0xf6, 0xf8, 0xfa, 0xfd, 0xff };

inline TRgb ConvertToLinear(TRgb aColor)
    {
    // perform sRGB->linear color conversion if the renderer is
    // openvg
    // NOTE: For emulator depending on the OpenVG SW version mapping may
    // be needed or not. Use/unuse ifdefs below if text colors are too dark/light.
//#ifndef __WINSCW__
    if (CHuiStatic::Env().Renderer() == EHuiRendererVg10)
        {
        TUint32 color = aColor.Internal();
        return
            ((TUint32)color_s_to_lin[(color >> 0) & 0xff] << 16) |
            ((TUint32)color_s_to_lin[(color >> 8) & 0xff] << 8) |
            ((TUint32)color_s_to_lin[(color >> 16) & 0xff] << 0) |
            (color & 0xff000000);
        }
//#endif    
    return aColor;
    }

void CHuiCanvasWsHwGc::WsDrawText1L(TPtr& aTextValue, THuiCanvasTextParameters& aTextParameters)
	{		
	TPoint point = iWsPenPosition;
    AdjustCoordinates( point );
	THuiCachedTextParams cachedtextParams;
    cachedtextParams.iFindTextPtr = &aTextValue;
    cachedtextParams.iFontHandle = iWsCurrentFont;
    cachedtextParams.iTextParams = aTextParameters;
    cachedtextParams.iGcParams = CachedGcParams();

    const CHuiCanvasTextImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedTextL(cachedtextParams,*iVisual); 

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            THuiRealRect destinationRect = TRect(point, texture->Size());                
            
            // Adjust destination rect according to the point where text was rasterised in the image
            TInt dx = cachedImage->iRasterizationOffset.iX;
            TInt dy = cachedImage->iRasterizationOffset.iY;        
            destinationRect.Move(-dx,-dy);
            
            TRgb oldPenColor = iCanvasGc->PenColor();
            TReal32 oldOpacity = iCanvasGc->Opacity();
            if (cachedImage->iUseColorModulation)
                {
                iCanvasGc->SetPenColor(ConvertToLinear(iWsPenColor));    
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));
                }
            
            // Draw            
            iCanvasGc->DrawImage(*texture, destinationRect, TRect(TPoint(0,0), texture->Size()), CHuiGc::EStretchNone);
    
            if (cachedImage->iUseColorModulation)
                {
                iCanvasGc->SetPenColor(oldPenColor);                        
                iCanvasGc->SetOpacity(oldOpacity);
                }
            }        
        }
	}

void CHuiCanvasWsHwGc::WsDrawText2L(TPtr& aTextValue, TPoint& aPoint,THuiCanvasTextParameters& aTextParameters)
	{
	THuiCachedTextParams cachedtextParams;
    cachedtextParams.iFindTextPtr = &aTextValue;
    cachedtextParams.iFontHandle = iWsCurrentFont;
    cachedtextParams.iTextParams = aTextParameters;
    cachedtextParams.iGcParams = CachedGcParams();

    const CHuiCanvasTextImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedTextL(cachedtextParams,*iVisual); 

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            AdjustCoordinates( aPoint );
            THuiRealRect destinationRect = TRect(aPoint, texture->Size());                
            
            // Adjust destination rect according to the point where text was rasterised in the image
            TInt dx = cachedImage->iRasterizationOffset.iX;
            TInt dy = cachedImage->iRasterizationOffset.iY;        
            destinationRect.Move(-dx,-dy);
            
            TRgb oldPenColor = iCanvasGc->PenColor();
            TReal32 oldOpacity = iCanvasGc->Opacity();
            if (cachedImage->iUseColorModulation)
                {
                iCanvasGc->SetPenColor(ConvertToLinear(iWsPenColor));
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));    
                }
            
            // Draw            
            iCanvasGc->DrawImage(*texture, destinationRect, TRect(TPoint(0,0), texture->Size()), CHuiGc::EStretchNone);
    
            if (cachedImage->iUseColorModulation)
                {
                iCanvasGc->SetPenColor(oldPenColor);                        
                iCanvasGc->SetOpacity(oldOpacity);
                }
            }
        }
	}

void CHuiCanvasWsHwGc::WsDrawText3L(TPtr& aTextValue, TRect& aRect, THuiCanvasTextParameters& aTextParameters)
	{
	THuiCachedTextParams cachedtextParams;
	cachedtextParams.iFindTextPtr = &aTextValue;
	cachedtextParams.iFontHandle = iWsCurrentFont;
    cachedtextParams.iTextParams = aTextParameters;
    cachedtextParams.iTextBoxMaxSize = aRect.Size();
    cachedtextParams.iGcParams = CachedGcParams();

    const CHuiCanvasTextImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedTextL(cachedtextParams,*iVisual); 

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            AdjustCoordinates( aRect );
            THuiRealRect destinationRect = aRect;                
            
            // Adjust destination rect according to the point where text was rasterised in the image
            TInt dx = cachedImage->iRasterizationOffset.iX;
            TInt dy = cachedImage->iRasterizationOffset.iY;        
            destinationRect.Move(-dx,-dy);
            
            TRgb oldPenColor = iCanvasGc->PenColor();
            TReal32 oldOpacity = iCanvasGc->Opacity();
            if (cachedImage->iUseColorModulation)
                {
                DrawTextBrush( destinationRect );
                iCanvasGc->SetPenColor(ConvertToLinear(iWsPenColor));
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));    
                }
    
            // Draw            
            iCanvasGc->DrawImage(*texture, destinationRect, TRect(TPoint(0,0), texture->Size()), CHuiGc::EStretchNone);
    
            if (cachedImage->iUseColorModulation)
                {
                iCanvasGc->SetPenColor(oldPenColor);
                iCanvasGc->SetOpacity(oldOpacity);                        
                }
            }
        }
	}


void CHuiCanvasWsHwGc::WsDrawText4L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextAlign, TInt aTextMargin,THuiCanvasTextParameters& aTextParameters)
    {
	THuiCachedTextParams cachedtextParams;
	cachedtextParams.iFindTextPtr = &aTextValue;
    cachedtextParams.iFontHandle = iWsCurrentFont;
    cachedtextParams.iTextParams = aTextParameters;
    cachedtextParams.iTextBoxMaxSize = aRect.Size();
    cachedtextParams.iBaseLineOffset = aBaselineOffset;
    cachedtextParams.iTextAlign = aTextAlign;
    cachedtextParams.iMargin = aTextMargin;
    cachedtextParams.iGcParams = CachedGcParams();
    
    const CHuiCanvasTextImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedTextL(cachedtextParams,*iVisual); 

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            AdjustCoordinates( aRect );
            THuiRealRect destinationRect = aRect;                
            
            // Adjust destination rect according to the point where text was rasterised in the image
            TInt dx = cachedImage->iRasterizationOffset.iX;
            TInt dy = cachedImage->iRasterizationOffset.iY;        
            destinationRect.Move(-dx,-dy);
            
            TRgb oldPenColor = iCanvasGc->PenColor();
            TReal32 oldOpacity = iCanvasGc->Opacity();
            if (cachedImage->iUseColorModulation)
                {
                DrawTextBrush( destinationRect );
                iCanvasGc->SetPenColor(ConvertToLinear(iWsPenColor));
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));    
                }
    
            // Draw            
            iCanvasGc->DrawImage(*texture, destinationRect, TRect(TPoint(0,0), texture->Size()), CHuiGc::EStretchNone);
    
            if (cachedImage->iUseColorModulation)
                {
                iCanvasGc->SetPenColor(oldPenColor);
                iCanvasGc->SetOpacity(oldOpacity);                        
                }
            }    
        }
    }

void CHuiCanvasWsHwGc::WsDrawText5L( TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextAlign, TInt aTextMargin,TInt aTextWidth, THuiCanvasTextParameters& aTextParameters)
	{	
	THuiCachedTextParams cachedtextParams;
	cachedtextParams.iFindTextPtr = &aTextValue;
    cachedtextParams.iFontHandle = iWsCurrentFont;
    cachedtextParams.iTextParams = aTextParameters;
    cachedtextParams.iTextBoxMaxSize = aRect.Size();
    cachedtextParams.iBaseLineOffset = aBaselineOffset;
    cachedtextParams.iTextAlign = aTextAlign;
    cachedtextParams.iMargin = aTextMargin;
    cachedtextParams.iTextWidth = aTextWidth;
    cachedtextParams.iGcParams = CachedGcParams();
    
    const CHuiCanvasTextImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedTextL(cachedtextParams,*iVisual); 

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            AdjustCoordinates( aRect );
            THuiRealRect destinationRect = aRect;                
            
            // Adjust destination rect according to the point where text was rasterised in the image
            TInt dx = cachedImage->iRasterizationOffset.iX;
            TInt dy = cachedImage->iRasterizationOffset.iY;        
            destinationRect.Move(-dx,-dy);
            
            TRgb oldPenColor = iCanvasGc->PenColor();
            TReal32 oldOpacity = iCanvasGc->Opacity();
            if (cachedImage->iUseColorModulation)
                {
                DrawTextBrush( destinationRect );
                iCanvasGc->SetPenColor(ConvertToLinear(iWsPenColor));
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));    
                }
    
            // Draw            
            iCanvasGc->DrawImage(*texture, destinationRect, TRect(TPoint(0,0), texture->Size()), CHuiGc::EStretchNone);
    
            if (cachedImage->iUseColorModulation)
                {
                iCanvasGc->SetPenColor(oldPenColor);
                iCanvasGc->SetOpacity(oldOpacity);                        
                }
            }    
        }
	}
	
void CHuiCanvasWsHwGc::WsDrawTextVertical1L(TPtr& aTextValue, TInt aTextUp, THuiCanvasTextParameters& aTextParameters)	
    {
	TPoint point = iWsPenPosition;
    TInt angle = aTextUp ? -90 : 90;

    THuiCachedTextParams cachedtextParams;
    cachedtextParams.iFindTextPtr = &aTextValue;
    cachedtextParams.iFontHandle = iWsCurrentFont;
    cachedtextParams.iTextParams = aTextParameters;
    cachedtextParams.iAngle = angle;
    cachedtextParams.iGcParams = CachedGcParams();

    const CHuiCanvasTextImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedTextL(cachedtextParams,*iVisual); 

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            AdjustCoordinates( point );
            THuiRealRect destinationRect = TRect(point, texture->Size());                
            
            // Adjust destination rect according to the point where text was rasterised in the image
            TInt dx = cachedImage->iRasterizationOffset.iX;
            TInt dy = cachedImage->iRasterizationOffset.iY;        
            destinationRect.Move(-dx,-dy);
            
            TRgb oldPenColor = iCanvasGc->PenColor();
            TReal32 oldOpacity = iCanvasGc->Opacity();
            if (cachedImage->iUseColorModulation)
                {
                iCanvasGc->SetPenColor(ConvertToLinear(iWsPenColor));    
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));
                }
    
            // Draw            
            iCanvasGc->DrawImage(*texture, destinationRect, TRect(TPoint(0,0), texture->Size()), CHuiGc::EStretchNone);
    
            if (cachedImage->iUseColorModulation)
                {
                iCanvasGc->SetPenColor(oldPenColor);
                iCanvasGc->SetOpacity(oldOpacity);                        
                }
            }    
        }
    }

void CHuiCanvasWsHwGc::WsDrawTextVertical2L(TPtr& aTextValue, TInt aTextUp,TPoint& aPoint, THuiCanvasTextParameters& aTextParameters)	
    {	 	                                       	
    TInt angle = aTextUp ? -90 : 90;

    THuiCachedTextParams cachedtextParams;
    cachedtextParams.iFindTextPtr = &aTextValue;
    cachedtextParams.iFontHandle = iWsCurrentFont;
    cachedtextParams.iTextParams = aTextParameters;
    cachedtextParams.iAngle = angle;
    cachedtextParams.iGcParams = CachedGcParams();
	
    const CHuiCanvasTextImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedTextL(cachedtextParams,*iVisual); 

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            AdjustCoordinates( aPoint );
            THuiRealRect destinationRect = TRect(aPoint, texture->Size());                
            
            // Adjust destination rect according to the point where text was rasterised in the image
            TInt dx = cachedImage->iRasterizationOffset.iX;
            TInt dy = cachedImage->iRasterizationOffset.iY;        
            destinationRect.Move(-dx,-dy);
            
            TRgb oldPenColor = iCanvasGc->PenColor();
            TReal32 oldOpacity = iCanvasGc->Opacity();
            if (cachedImage->iUseColorModulation)
                {
                iCanvasGc->SetPenColor(ConvertToLinear(iWsPenColor));
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));    
                }
    
            // Draw            
            iCanvasGc->DrawImage(*texture, destinationRect, TRect(TPoint(0,0), texture->Size()), CHuiGc::EStretchNone);
    
            if (cachedImage->iUseColorModulation)
                {
                iCanvasGc->SetPenColor(oldPenColor); 
                iCanvasGc->SetOpacity(oldOpacity);                       
                }
            }
        }
    }

void CHuiCanvasWsHwGc::WsDrawTextVertical3L(TPtr& aTextValue, TInt aTextUp, TRect& aRect, THuiCanvasTextParameters& aTextParameters)	
    {
	TInt angle = aTextUp ? -90 : 90;

    THuiCachedTextParams cachedtextParams;
    cachedtextParams.iFindTextPtr = &aTextValue;
    cachedtextParams.iFontHandle = iWsCurrentFont;
    cachedtextParams.iTextParams = aTextParameters;
    cachedtextParams.iTextBoxMaxSize = aRect.Size();
    cachedtextParams.iAngle = angle;
    cachedtextParams.iGcParams = CachedGcParams();

    const CHuiCanvasTextImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedTextL(cachedtextParams,*iVisual); 

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            AdjustCoordinates( aRect );
            THuiRealRect destinationRect = aRect;                
            
            // Adjust destination rect according to the point where text was rasterised in the image
            TInt dx = cachedImage->iRasterizationOffset.iX;
            TInt dy = cachedImage->iRasterizationOffset.iY;        
            destinationRect.Move(-dx,-dy);
            
            TRgb oldPenColor = iCanvasGc->PenColor();
            TReal32 oldOpacity = iCanvasGc->Opacity();
            if (cachedImage->iUseColorModulation)
                {
                DrawTextBrush( destinationRect );
                iCanvasGc->SetPenColor(ConvertToLinear(iWsPenColor));  
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));  
                }
    
            // Draw            
            iCanvasGc->DrawImage(*texture, destinationRect, TRect(TPoint(0,0), texture->Size()), CHuiGc::EStretchNone);
    
            if (cachedImage->iUseColorModulation)
                {
                iCanvasGc->SetPenColor(oldPenColor); 
                iCanvasGc->SetOpacity(oldOpacity);                       
                }
            }
        }
    }

void CHuiCanvasWsHwGc::WsDrawTextVertical4L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextUp, TInt aTextAlign, TInt aTextMargin,THuiCanvasTextParameters& aTextParameters)
    {
    TInt angle = aTextUp ? -90 : 90;


    THuiCachedTextParams cachedtextParams;
    cachedtextParams.iFindTextPtr = &aTextValue;
    cachedtextParams.iFontHandle = iWsCurrentFont;
    cachedtextParams.iTextParams = aTextParameters;
    cachedtextParams.iTextBoxMaxSize = aRect.Size();
    cachedtextParams.iAngle = angle;
    cachedtextParams.iBaseLineOffset = aBaselineOffset;
    cachedtextParams.iTextAlign = aTextAlign;
    cachedtextParams.iMargin = aTextMargin;
    cachedtextParams.iGcParams = CachedGcParams();

    const CHuiCanvasTextImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedTextL(cachedtextParams,*iVisual); 

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            AdjustCoordinates( aRect );
            THuiRealRect destinationRect = aRect;                
            
            // Adjust destination rect according to the point where text was rasterised in the image
            TInt dx = cachedImage->iRasterizationOffset.iX;
            TInt dy = cachedImage->iRasterizationOffset.iY;        
            destinationRect.Move(-dx,-dy);
            
            TRgb oldPenColor = iCanvasGc->PenColor();
            TReal32 oldOpacity = iCanvasGc->Opacity();
            if (cachedImage->iUseColorModulation)
                {
                DrawTextBrush( destinationRect );
                iCanvasGc->SetPenColor(ConvertToLinear(iWsPenColor)); 
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));   
                }
    
            // Draw            
            iCanvasGc->DrawImage(*texture, destinationRect, TRect(TPoint(0,0), texture->Size()), CHuiGc::EStretchNone);
    
            if (cachedImage->iUseColorModulation)
                {
                iCanvasGc->SetPenColor(oldPenColor);
                iCanvasGc->SetOpacity(oldOpacity);                        
                }
            }
        }
    }

void CHuiCanvasWsHwGc::WsDrawTextVertical5L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextUp, TInt aTextAlign, TInt aTextMargin, TInt aTextWidth, THuiCanvasTextParameters& aTextParameters)
    {
	TInt angle = aTextUp ? -90 : 90;

    THuiCachedTextParams cachedtextParams;
    cachedtextParams.iFindTextPtr = &aTextValue;
    cachedtextParams.iFontHandle = iWsCurrentFont;
    cachedtextParams.iTextParams = aTextParameters;
    cachedtextParams.iTextBoxMaxSize = aRect.Size();
    cachedtextParams.iAngle = angle;
    cachedtextParams.iTextWidth = aTextWidth;
    cachedtextParams.iBaseLineOffset = aBaselineOffset;
    cachedtextParams.iTextAlign = aTextAlign;
    cachedtextParams.iMargin = aTextMargin;
    cachedtextParams.iGcParams = CachedGcParams();

    const CHuiCanvasTextImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedTextL(cachedtextParams,*iVisual); 

    if (IsRenderingEnabled())
        {           
        const CHuiTexture* texture = cachedImage->Texture();
        if (texture)
            {
            AdjustCoordinates( aRect );
            THuiRealRect destinationRect = aRect;                
            
            // Adjust destination rect according to the point where text was rasterised in the image
            TInt dx = cachedImage->iRasterizationOffset.iX;
            TInt dy = cachedImage->iRasterizationOffset.iY;        
            destinationRect.Move(-dx,-dy);
            
            TRgb oldPenColor = iCanvasGc->PenColor();
            TReal32 oldOpacity = iCanvasGc->Opacity();
            if (cachedImage->iUseColorModulation)
                {
                DrawTextBrush( destinationRect );
                iCanvasGc->SetPenColor(ConvertToLinear(iWsPenColor));  
                iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));  
                }
    
            // Draw            
            iCanvasGc->DrawImage(*texture, destinationRect, TRect(TPoint(0,0), texture->Size()), CHuiGc::EStretchNone);
    
            if (cachedImage->iUseColorModulation)
                {
                iCanvasGc->SetPenColor(oldPenColor);
                iCanvasGc->SetOpacity(oldOpacity);                        
                }
            }
        }
    }

void CHuiCanvasWsHwGc::WsMoveToL( TPoint& aPoint)
    {
    iWsPenPosition = aPoint;
    }

void CHuiCanvasWsHwGc::WsMoveByL( TPoint& aPoint)
    {
    iWsPenPosition += aPoint;
    }
    
void CHuiCanvasWsHwGc::WsPlotL(TPoint& aPoint)
	{
    if (IsRenderingEnabled())
    	{
        AdjustCoordinates( aPoint );
	    UseWsState();        
        // Draw  
        RArray<THuiRealPoint> points;
        CleanupClosePushL(points);  
        points.AppendL(THuiRealPoint(aPoint));       
        iCanvasGc->DrawPoints(points);
        CleanupStack::PopAndDestroy();
        UseCanvasState();
        }
    }

void CHuiCanvasWsHwGc::WsResetL()
	{
	iWsClipRegion.Clear();
	iWsCurrentFont = 0;
	iWsOrigin = KHuiWsDefaultOrigin;
    iWsPenSize = KHuiWsDefaultPenSize;

	iWsPenPosition = TPoint(0,0);
	iWsFadeColor = KHuiWsDefaultFadeColor;

    iWsPenColor = KHuiWsDefaultPenColor;
    iWsBrushColor = KHuiWsDefaultBrushColor;
    iWsPenStyle = KHuiWsDefaultPenStyle;
    iWsDrawMode = KHuiWsDefaultDrawMode;
    iWsBrushStyle = KHuiWsDefaultBrushStyle;
    iWsShadowMode = KHuiWsDefaultShadowMode;
    iWsStrikethrough = KHuiWsDefaultStrikethrough;
    iWsUnderline = KHuiWsDefaultUnderline; 
    iWsUserDisplayMode = KHuiWsDefaultUserDisplayMode;
    iWsShadowColor = KHuiWsDefaultShadowColor;
    iWsBrushPattern = 0;
    iWsBrushOrigin = TPoint(0,0);


  	iCanvasGc->CancelClipping();
  	iCanvasGc->SetPenColor(KRgbWhite); 
    iCanvasGc->SetOpacity(1.0);
	iCanvasGc->SetPenWidth(1); 
	}



void CHuiCanvasWsHwGc::WsSetBrushColorL( TRgb aColor)
    {
    iWsBrushColor = aColor;
    }

void CHuiCanvasWsHwGc::WsSetBrushOriginL( TPoint& aPoint)
    {
    iWsBrushOrigin = aPoint;
    }

void CHuiCanvasWsHwGc::WsSetBrushStyleL( TInt aStyle)
    {
    iWsBrushStyle = aStyle;        
    }

void CHuiCanvasWsHwGc::WsSetClippingRegionL( RRegion& aRegion)
    {
    if (IsRenderingEnabled())
        {
        iWsClipRegion.Clear();    
        for (TInt i=0;i<aRegion.Count();i++)
            {
            TRect rect = aRegion[i];
            AdjustCoordinates( rect, EFalse );
            iWsClipRegion.AddRect(rect);
            }    
        
        iCanvasGc->ClipRegion(iWsClipRegion);                
        }
    }
    
void CHuiCanvasWsHwGc::WsSetDrawModeL( TInt aDrawMode)
	{
	iWsDrawMode = aDrawMode;	
	}

void CHuiCanvasWsHwGc::WsSetOriginL( TPoint& aOrigin)
	{
 	iWsOrigin = aOrigin;
	}

void CHuiCanvasWsHwGc::WsSetPenColorL( TRgb aColor)
	{
	iWsPenColor = aColor;
	}

void CHuiCanvasWsHwGc::WsSetPenStyleL( TInt aStyle)
	{
    iWsPenStyle = aStyle;
	}

void CHuiCanvasWsHwGc::WsSetPenSizeL( TInt aWidth, TInt aHeight)
	{
    iWsPenSize.iWidth = aWidth;
    // Note, that only width is supported by HW renderer. Canvas should be draw in fallback mode, if width and height are not the same
    iWsPenSize.iHeight = aHeight; 
	}

void CHuiCanvasWsHwGc::WsSetTextShadowColorL( TRgb aColor)
	{
 	iWsShadowColor = aColor;
	}

void CHuiCanvasWsHwGc::WsSetCharJustificationL( TInt /*aExcessiveWidth*/, TInt /*aNumGap*/)
	{
    // This is not supported by this CHuiCanvasWsGc implementation
	iAllCommandsSupported = EFalse;
	}

void CHuiCanvasWsHwGc::WsSetWordJustificationL( TInt /*aExcessiveWidth*/, TInt /*aNumGap*/)
	{
    // This is not supported by this CHuiCanvasWsGc implementation
	iAllCommandsSupported = EFalse;
	}

void CHuiCanvasWsHwGc::WsSetUnderlineStyleL( TInt aValue )
	{
    if (aValue < 1000) // magic, we recycle the same function for both underline and strike through
        {
        iWsUnderline = aValue;
        }
    else
        {
        iWsStrikethrough=aValue-1000;
        }
	}

void CHuiCanvasWsHwGc::WsSetBrushPatternL( TInt aValue )
	{
	iWsBrushPattern = aValue;
	}

void CHuiCanvasWsHwGc::WsSetFontL( TInt aFontId)
	{
	iWsCurrentFont = aFontId;
	}

void CHuiCanvasWsHwGc::WsCopyRectL( TPoint& /*aPoint*/, TRect& /*aRect*/)
	{
    // This is not supported by this CHuiCanvasWsGc implementation
	iAllCommandsSupported = EFalse;
	}

void CHuiCanvasWsHwGc::WsUpdateJustificationL( TPtr& /*aTextValue*/)
	{
    // This is not supported by this CHuiCanvasWsGc implementation
	iAllCommandsSupported = EFalse;
	}

void CHuiCanvasWsHwGc::WsUpdateJustificationVerticalL( TInt /*aValue*/, TPtr& /*aTextValue*/)
	{
    // This is not supported by this CHuiCanvasWsGc implementation
	iAllCommandsSupported = EFalse;
	}

void CHuiCanvasWsHwGc::WsSetFontNoDuplicateL( TInt aCurrentFont)
	{
	iWsCurrentFont = aCurrentFont;
	}

void CHuiCanvasWsHwGc::WsCopySettingsL()
	{
    // This is not supported by this CHuiCanvasWsGc implementation
	iAllCommandsSupported = EFalse;
	}

void CHuiCanvasWsHwGc::WsSetClippingRectL( TRect& aRect)
    {    
    if (IsRenderingEnabled())
        {
        // Symbian SetClippingRect takes ws origin into account, 
        // but setClippingRegion does not.
        AdjustCoordinates( aRect, ETrue );
        iTempRegion.Clear();
        iTempRegion.AddRect( aRect );
        iCanvasGc->ClipRegion( iTempRegion );
        }
    }

void CHuiCanvasWsHwGc::WsCancelClippingRectL()
    {
    if (IsRenderingEnabled())
        {
        WsResetClippingRegionL();
        iCanvasGc->CancelClipping();
        }
    }

void CHuiCanvasWsHwGc::WsSetFadedL( TInt& /*aFaded*/)
    {
	// Nothing to do, we should get FadeArea commands if window is faded.
    }

void CHuiCanvasWsHwGc::WsSetFadingParametersL( TInt aBlackMap, TInt aWhiteMap)
    {
    if (IsRenderingEnabled())
        {
		if (aBlackMap > aWhiteMap)
            {
            TInt oldMap = aBlackMap;
            aBlackMap = aWhiteMap;
            aWhiteMap = oldMap;
            }

        const TUint16* lut = PtrTo16BitNormalisationTable();
        
        //CFbsBitGc::FadeArea() does the following per color component:
        //   dst = dst * (aWhiteMap - aBlackMap) + aBlackMap;
    
        //To achieve the same effect using MWsGraphicsContext we draw a rectangle
        //with specific intensity and alpha values:
        //   dst = dst * (1 - alpha) + intensity * alpha;
        //Thus:
        //   alpha = 1 - aWhiteMap + aBlackMap;
        //   intensity = aBlackMap / alpha;
    
        // alpha = 1 - aWhiteMap + aBlackMap;
        TInt alpha = 255 - aWhiteMap + aBlackMap;
        // intensity = aBlackMap / alpha;
        TInt i = (aBlackMap * lut[alpha]) >> 8;
    
        iWsFadeColor.SetInternal(i << 16 | i << 8 | i | alpha << 24); 
        }
    }

void CHuiCanvasWsHwGc::WsFadeAreaL( RRegion& aRegion)
    {
    if (IsRenderingEnabled())
        {
        RArray<THuiRealRect> realRects;
        for (TInt i=0; i<aRegion.Count();i++)
            {
            realRects.Append(aRegion[i]);
            }
        UseWsState();
        // Get real color & opacity from fading params
        TRgb fadeColor = iWsFadeColor;
        // Opacity is given separately, so set alpha of the color to 255.   
        fadeColor.SetAlpha(255); 
        TReal32 opacity = TReal32(TReal32(iWsFadeColor.Alpha() / 255.f));
        
        // Draw semi transparent rect(s) over the visual faded areas
        iCanvasGc->SetPenColor(fadeColor);
        iCanvasGc->SetOpacity(opacity);    
        iCanvasGc->SetPolygonDrawMode(EHuiFillNonZero);    
        iCanvasGc->SetDrawMode(SelectCanvasDrawMode(iWsDrawMode));        

        iCanvasGc->DrawRects(realRects);
        
        UseCanvasState();
            
        realRects.Close();    
        aRegion.Close();    
        }
    }

void CHuiCanvasWsHwGc::WsMapColorsL()
    {
    // This is not supported by this CHuiCanvasWsGc implementation
	iAllCommandsSupported = EFalse;
    }

void CHuiCanvasWsHwGc::WsSetUserDisplayModeL( TInt aMode )
    {
    iWsUserDisplayMode = aMode;
    }

void CHuiCanvasWsHwGc::WsUseFontL(TInt aCurrentFont)
    {
    iWsCurrentFont = aCurrentFont;
    }       

void CHuiCanvasWsHwGc::BeginActionL(TInt aAction, TRect aDisplayRect, const CHuiCanvasVisual& aUser, TBool aIsCachePrepared, const TRegion& aUpdateRegion)
	{
    iAction = aAction;
    iVisual = (CHuiCanvasVisual*)&aUser;
    iAllCommandsSupported = ETrue;

    if (aAction == EDrawBuffer)
	    {
	    BeginDrawL(aDisplayRect, aUser, aIsCachePrepared, aUpdateRegion);
	    }
	else if (aAction == EScanBuffer)
	    {
        BeginScanL(aDisplayRect, aUser, aIsCachePrepared, aUpdateRegion);	    
	    }
	else
	    {
	    // Should not happen
	    }
	}

void CHuiCanvasWsHwGc::EndActionL(const TRegion& aUpdateRegion, TBool aUpdateDisplay)
	{
    if (iAction == EDrawBuffer)
        {
        EndDrawL(aUpdateRegion, aUpdateDisplay);
        }
    else if (iAction == EScanBuffer)
        {
        EndScanL(aUpdateRegion, aUpdateDisplay);
        }
    else
        {
        // Should not happen
        }
	}

void CHuiCanvasWsHwGc::BeginDrawL(TRect aDisplayRect,const CHuiCanvasVisual& aUser, TBool aIsCachePrepared, const TRegion& aUpdateRegion)
    {
    CHuiCanvasRenderBuffer* renderbuffer = NULL;
    TRect updatedRect = aUpdateRegion.BoundingRect();
    TBool renderBufferCreated = EFalse;
    
    // Try to get render buffer if it has been enabled
    if (iRenderBufferEnabled && updatedRect.Size() != TSize(0,0))
        {
        renderbuffer = iVisual->Env().CanvasTextureCache().FindCachedRenderBuffer(aUser);
        if (!renderbuffer)
            {        
            renderbuffer = iVisual->Env().CanvasTextureCache().CreateCachedRenderBufferL(aUser, updatedRect.Size());
            renderBufferCreated = ETrue;
            }
        }
    
    if (renderbuffer)
        {
        // If renderbuffer is not initialized, set the internal flag to EFalse. It will be set
        // to ETrue after drawing has been done.
        if (!renderbuffer->IsInitialized() || renderbuffer->Size() != updatedRect.Size() || renderBufferCreated)
            {
            iRenderBufferInitialized = EFalse;
            }
        
        // We can access render buffer, mark it to be available.
        iRenderBufferAvailable = ETrue;
        
        // Render buffer is prepared if caller says that cache is prepared and renderbuffer has been initialized.
        iRenderBufferPrepared = aIsCachePrepared && iRenderBufferInitialized;
        }
    else
        {
        // We could not access render buffer, mark it to be unavailable.
        iRenderBufferAvailable = EFalse;
        }
        
    // If render buffer is not prepared, we must prepare it. We will set the flag at EndActionL when
    // we really have prepared it.
    if (IsRenderBufferUpdateOngoing())
        {        
        // Init render buffer (it only really initializes if needed)
        renderbuffer->InitializeL(updatedRect.Size());

        // Bind render buffer
        BindRenderBuffer(renderbuffer, aUpdateRegion);
        }
    iCanvasGc->RestoreFlaggedState();
    }

void CHuiCanvasWsHwGc::BeginScanL(TRect aDisplayRect,const CHuiCanvasVisual& aUser, TBool aIsCachePrepared, const TRegion& aUpdateRegion)
    {
    CHuiCanvasRenderBuffer* renderbuffer = NULL;
    TRect updatedRect = aUpdateRegion.BoundingRect();
    TBool renderBufferCreated = EFalse;
    
    // Try to get render buffer if it has been enabled
    if (iRenderBufferEnabled && updatedRect.Size() != TSize(0,0))
        {
        renderbuffer = iVisual->Env().CanvasTextureCache().FindCachedRenderBuffer(aUser);
        if (!renderbuffer)
            {        
            renderbuffer = iVisual->Env().CanvasTextureCache().CreateCachedRenderBufferL(aUser, updatedRect.Size());
            renderBufferCreated = ETrue;
            }
        }
    
    if (renderbuffer)
        {
        // If renderbuffer is not initialized, set the internal flag to EFalse. It will be set
        // to ETrue after drawing has been done.
        if (!renderbuffer->IsInitialized() || renderbuffer->Size() != updatedRect.Size() || renderBufferCreated)
            {
            iRenderBufferInitialized = EFalse;
            }
        
        // We can access render buffer, mark it to be available.
        iRenderBufferAvailable = ETrue;
        
        // Render buffer is prepared if caller says that cache is prepared and renderbuffer has been initialized.
        iRenderBufferPrepared = aIsCachePrepared && iRenderBufferInitialized;
        }
    else
        {
        // We could not access render buffer, mark it to be unavailable.
        iRenderBufferAvailable = EFalse;
        }
        
    // If render buffer is not prepared, we must prepare it. We will set the flag at EndActionL when
    // we really have prepared it.
    if (IsRenderBufferUpdateOngoing())
        {        
        // Init render buffer (it only really initializes if needed)
        renderbuffer->InitializeL(updatedRect.Size());
        }

    iCanvasGc->RestoreFlaggedState();
	}

void CHuiCanvasWsHwGc::EndDrawL(const TRegion& aUpdateRegion, TBool aUpdateDisplay)
    {
    CHuiCanvasRenderBuffer* renderbuffer = NULL;

    // Check if we have cached render buffer available
    if (IsRenderBufferEnabled())
        {
        renderbuffer = iVisual->Env().CanvasTextureCache().FindCachedRenderBuffer(*iVisual);
        }
    
    if (renderbuffer)
        {
        // We can access render buffer, mark it to be available.
        iRenderBufferAvailable = ETrue;
        }
    else
        {
        // We could not access render buffer, mark it to be unavailable.
        iRenderBufferAvailable = EFalse;        
        }

    // Are we updating into render buffer
    if (IsRenderBufferUpdateOngoing())
        {            
        // Are we rendering
        UnBindRenderBuffer(renderbuffer);
        // If we really did draw to the renderbuffer, then set it "prepared".
        if (iCanvasGc->Gc())
            {
            iRenderBufferInitialized = ETrue;
            iRenderBufferPrepared = ETrue;
            }    
        }

    iCanvasGc->RestoreFlaggedState();
    
    // Cancel clipping if still active etc...
    iCanvasGc->SetDefaults();    

    // If render buffer is used, draw its content to display here
    if (aUpdateDisplay && renderbuffer && iCanvasGc->Gc())
        {
        THuiRealRect updatedRect = aUpdateRegion.BoundingRect();
        TInt w = 0; 
		TInt h = 0;
        // Handle relative rotation
        if (iVisual->Display())
            {
            TRect displayArea = iVisual->Display()->VisibleArea();
            w = displayArea.Width();
            h = displayArea.Height();
            }
        // ...select right rotation...
        if (iRelativeOrientation == CHuiGc::EOrientationCW90)
            {
            // Rotate around origo and move back to displayarea
            iCanvasGc->Rotate(-90, 0, 0, 1.f);
            iCanvasGc->Translate(-h, 0, 0);
            }
        else if (iRelativeOrientation == CHuiGc::EOrientationCCW90)
            {
            // Rotate around origo and move back to displayarea
            iCanvasGc->Rotate(90, 0, 0, 1.f);
            iCanvasGc->Translate(0, -w, 0);
            }
        else if (iRelativeOrientation == CHuiGc::EOrientation180)
            {
            // Rotate around origo and move back to displayarea
            iCanvasGc->Rotate(180, 0, 0, 1.f);
            iCanvasGc->Translate(-w, -h, 0);            
            }
        else
            {
            // Nothing
            }            
        
        iCanvasGc->ClipRegion(aUpdateRegion);
        THuiCanvasDrawMode dmode = iCanvasGc->DrawMode();
        // blend only transparent windows
        if ((iVisual->Flags() & EHuiVisualFlagOpaqueHint) && !(iVisual->EffectiveOpacity() < 1.0f))
            {
            iCanvasGc->SetDrawMode(EHuiCanvasDrawModeNormal);
            }
        else
            {
            iCanvasGc->SetDrawMode(EHuiCanvasDrawModeBlend);
            }
        
        iCanvasGc->DrawImage(*renderbuffer, updatedRect.iTl);
        iCanvasGc->CancelClipping();
        iCanvasGc->SetDrawMode(dmode);
        }    
    }

void CHuiCanvasWsHwGc::EndScanL(const TRegion& aUpdateRegion, TBool aUpdateDisplay)
    {
    CHuiCanvasRenderBuffer* renderbuffer = NULL;

    // Check if we have cached render buffer available
    if (IsRenderBufferEnabled())
        {
        renderbuffer = iVisual->Env().CanvasTextureCache().FindCachedRenderBuffer(*iVisual);
        }
    
    if (renderbuffer)
        {
        // We can access render buffer, mark it to be available.
        iRenderBufferAvailable = ETrue;
        }
    else
        {
        // We could not access render buffer, mark it to be unavailable.
        iRenderBufferAvailable = EFalse;        
        }
    
    // Cancel clipping if still active etc...
    iCanvasGc->RestoreFlaggedState();
    iCanvasGc->SetDefaults();    
    }


CHuiCanvasGc& CHuiCanvasWsHwGc::CanvasGc() const
	{
	return *iCanvasGc;	
	}
	
THuiCachedGcParams CHuiCanvasWsHwGc::CachedGcParams()
    {
    THuiCachedGcParams params;
    params.iPenColor = iWsPenColor;    
    params.iBrushColor = iWsBrushColor;
    params.iPenStyle = iWsPenStyle;
    params.iDrawMode = iWsDrawMode;
    params.iBrushColor = iWsBrushColor;
    params.iBrushStyle = iWsBrushStyle;
    params.iShadowMode = iWsShadowMode;
    params.iStrikethrough = iWsStrikethrough;
    params.iUnderline = iWsUnderline;
    params.iUserDisplayMode = iWsUserDisplayMode;
    params.iShadowColor = iWsShadowColor;
    return params;        
    }

TInt CHuiCanvasWsHwGc::SetCapturingBufferL(CFbsBitmap* /*aTarget*/)
    {
    // This implementation does not support capturing
    return KErrNotSupported;    
    }

void CHuiCanvasWsHwGc::SetPositionDelta( TPoint& aPoint )
    {
    iPosDelta = aPoint;
    }

TRect CHuiCanvasWsHwGc::RoundedDisplayRect()
    {
    if (iVisual)
        {
        return iVisual->DisplayRect().Round();    
        }
    else
        {
        return TRect(0,0,0,0);    
        }            
    }

void CHuiCanvasWsHwGc::ClearCache()
    {
    if (iVisual)
        {
        iVisual->Env().CanvasTextureCache().ReleaseCachedRenderBuffer(*iVisual);
        }
    }    

TBool CHuiCanvasWsHwGc::IsRenderBufferEnabled() const
    {
    return iRenderBufferEnabled && iRenderBufferAvailable;
    }

TBool CHuiCanvasWsHwGc::IsRenderBufferPrepared() const
    {
    return iRenderBufferEnabled && iRenderBufferAvailable && iRenderBufferPrepared;    
    }

TBool CHuiCanvasWsHwGc::IsRenderBufferInitialized() const
    {
    return iRenderBufferEnabled && iRenderBufferAvailable && iRenderBufferInitialized;    
    }


TBool CHuiCanvasWsHwGc::IsRenderingEnabled() const
    {
    if (iAction == EDrawBuffer)
        {
        return ETrue;
        }
    else if (iAction == EScanBuffer)
        {
        return EFalse;
        }
    else
        {
        return EFalse;
        }
    }

TBool CHuiCanvasWsHwGc::IsRenderBufferUpdateOngoing() const
    {
    return iRenderBufferEnabled && iRenderBufferAvailable && !iRenderBufferPrepared;
    }


TInt CHuiCanvasWsHwGc::EnableRenderbuffer(TBool aEnable)
    {
    // TODO: Some renderers should return KErrNotSupported
    TInt retVal = KErrNone;
    iRenderBufferEnabled = aEnable;
    return retVal;
    }

THuiCanvasWsGcType CHuiCanvasWsHwGc::Type() const
    {
    return EHuiCanvasWsHw;
    }

void CHuiCanvasWsHwGc::ClearCapturingBufferArea(const TRect& /*aRect*/)
    {
    // Not supported by HW GC.
    }

THuiCanvasDrawMode CHuiCanvasWsHwGc::SelectCanvasDrawMode(TInt aWsDrawMode)
    {
    if (aWsDrawMode == MWsGraphicsContext::EDrawModeWriteAlpha)
        {
        return EHuiCanvasDrawModeNormal;            
        }
    else
        {
         return EHuiCanvasDrawModeBlend;
        }    
    }

void CHuiCanvasWsHwGc::EnableUpdateRegion(const TRegion& aUpdateRegion, TBool aClear)
    {
    CHuiGc* gc = iCanvasGc->Gc(); 
    if (gc && IsRenderingEnabled())
        {
        gc->Enable(CHuiGc::EFeatureClipping);
        gc->PushClip();
        gc->Clip(aUpdateRegion);
        if (aClear)
            {
            TInt oldAlpha = gc->PenAlpha();
            TRgb oldColor = gc->PenColor();            
            gc->SetPenAlpha(0);
            gc->SetPenColor(KRgbBlue);
            TRect rect = aUpdateRegion.BoundingRect();
            gc->Clear(rect); 
            gc->SetPenAlpha(oldAlpha);
            gc->SetPenColor(oldColor);
            }
        }
    }

void CHuiCanvasWsHwGc::DisableUpdateRegion()
    {
    CHuiGc* gc = iCanvasGc->Gc(); 
    if (gc && IsRenderingEnabled())
        {    
        gc->PopClip();
        }
    }

static TRect CalculateClipRect(CHuiCanvasRenderBuffer* aRenderbuffer)
    {
    const TInt KHuiDefaultSize = 2048;
    return 
        aRenderbuffer && aRenderbuffer->IsInitialized() ? 
        TRect( aRenderbuffer->Size() ) :
        TRect( TSize( KHuiDefaultSize, KHuiDefaultSize ) );
    }

void CHuiCanvasWsHwGc::BindRenderBuffer(CHuiCanvasRenderBuffer* aRenderbuffer, const TRegion& aUpdateRegion)
    {
    // Disable effective opacity when rendering to a buffer
    iCanvasGc->EnableEffectiveOpacity(EFalse);
    
    THuiRealRect updatedRect = aUpdateRegion.BoundingRect();

    // Bind render buffer as rendering target (further drawing will be redirected to render buffer)
    aRenderbuffer->Bind();

    // Restore gc state after render target bind/unbind ops
    iCanvasGc->RestoreState();

    // Translation values
    TReal32 x = -updatedRect.iTl.iX;
    TReal32 y = -updatedRect.iTl.iY;  
    
    // Push clipping so that we can restore it after we have done drawing into render buffer.
    iCanvasGc->Gc()->PushClip();
    
    // Set new clipping region which does not clip anything. 
    // We want always draw aUpdateRegion fully to the aRenderbuffer. 
            
    iCanvasGc->Gc()->SetClip(CalculateClipRect(aRenderbuffer)); // this call does not transform region anymore
    
    // We use translation to get screen coordinates to match render buffer coordinates
    iCanvasGc->PushTransformationMatrix();
    iCanvasGc->Translate(x, y, 0.f);
        
    // Handle relative rotation
    TRect displayArea = iCanvasGc->Gc()->DisplayArea();
    TInt w = displayArea.Width();
    TInt h = displayArea.Height();
    
    // ...select right rotation...
    if (iRelativeOrientation == CHuiGc::EOrientationCW90)
        {
        // Rotate around origo and move back to displayarea
        iCanvasGc->Rotate(-90, 0, 0, 1.f);
        iCanvasGc->Translate(-h, 0, 0);
        }
    else if (iRelativeOrientation == CHuiGc::EOrientationCCW90)
        {
        // Rotate around origo and move back to displayarea
        iCanvasGc->Rotate(90, 0, 0, 1.f);
        iCanvasGc->Translate(0, -w, 0);
        }
    else if (iRelativeOrientation == CHuiGc::EOrientation180)
        {
        // Rotate around origo and move back to displayarea
        iCanvasGc->Rotate(180, 0, 0, 1.f);
        iCanvasGc->Translate(-w, -h, 0);            
        }
    else
        {
        // Nothing
        }            
        
    // Set needed clip region
    iCanvasGc->Gc()->Clip(aUpdateRegion);
    }

void CHuiCanvasWsHwGc::UnBindRenderBuffer(CHuiCanvasRenderBuffer* aRenderbuffer)
    {
    // Enable effective opacity again when no more rendering to buffer
    iCanvasGc->EnableEffectiveOpacity(ETrue);

    // Restore translation (screen coordinates to match render buffer coordinates)
    iCanvasGc->PopTransformationMatrix();

    // Restore clipping 
    iCanvasGc->Gc()->PopClip();
            
    // Unbind render buffer as rendering target (further drawing will be done to display normally) 
    aRenderbuffer->UnBind();
    
    // Restore gc state after render target bind/unbind ops
    iCanvasGc->RestoreState();
    }

void CHuiCanvasWsHwGc::SetRelativeOrientation(CHuiGc::TOrientation aOrientation)
    {
    iRelativeOrientation = aOrientation;
    }


void CHuiCanvasWsHwGc::UseBrushPattern()
    {
    THuiCachedImageParams cachedImageParams;
    cachedImageParams.iBitmapHandle = iWsBrushPattern;
    cachedImageParams.iGcParams = CachedGcParams();    

    const CHuiCanvasGraphicImage* cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedImageL(cachedImageParams,*iVisual);    
    if (IsRenderingEnabled())
        {
        iCanvasGc->SetPolygonFillTexture(cachedImage->Texture());
        // Symbian implementation for CWindowGc is different from CFbsBitGc. Latter
        // adds iWsOrigin to iWsBrushOrigin, but because we are implementing
        // CWindowGc, we do not take iWsOrigin into account here.
        iCanvasGc->SetPolygonFillTextureOrigin(iWsBrushOrigin); 
        }
    }

void CHuiCanvasWsHwGc::DiscardBrushPattern()
    {
    if (IsRenderingEnabled())
        {
        iCanvasGc->SetPolygonFillTexture(NULL);
        iCanvasGc->SetPolygonFillTextureOrigin(TPoint(0,0));
        }
    }

TBool CHuiCanvasWsHwGc::IsDifferent(const TRgb& aFirst, const TRgb& aSecond, TBool aIgnoreAlpha)
    {
    if (aIgnoreAlpha)
        {
        const TUint32 mask = 0x00FFFFFF;
        return ((aFirst.Internal() & mask) != (aSecond.Internal() & mask));
        }
    else
        {
        return (aFirst.Internal() != aSecond.Internal());
        }    
    }

void CHuiCanvasWsHwGc::UseWsState()
    {
#ifdef __WINS__
    iPushCount++;
    if(iPushCount>1)
        {
        // This function cannot be called recursively. Fix your code.
        USER_INVARIANT();
        }
#endif    
    iOldColor = iCanvasGc->PenColor();            
    iOldOpacity = iCanvasGc->Opacity();
    iOldDrawMode = iCanvasGc->DrawMode();
    iOldPenWidth = iCanvasGc->PenWidth(); 
    iOldPolygonDrawMode = iCanvasGc->PolygonDrawMode();
    
    iCanvasGc->SetPenColor(iWsPenColor);
    iCanvasGc->SetOpacity(TReal32(iWsPenColor.Alpha() / 255.f));
    iCanvasGc->SetDrawMode(SelectCanvasDrawMode(iWsDrawMode));
    iCanvasGc->SetPenWidth(iWsPenSize.iWidth);
    iCanvasGc->PolygonDrawMode();
    }

void CHuiCanvasWsHwGc::UseCanvasState()
    {
#ifdef __WINS__
    iPushCount--;
    if(iPushCount!=0)
        {
        // This function cannot be called recursively. Fix your code.
        USER_INVARIANT();
        }
#endif
    iCanvasGc->SetPenColor(iOldColor);            
    iCanvasGc->SetOpacity(iOldOpacity);
    iCanvasGc->SetDrawMode(iOldDrawMode);
    iCanvasGc->SetPenWidth(iOldPenWidth);
    iCanvasGc->SetPolygonDrawMode(iOldPolygonDrawMode);
    }
