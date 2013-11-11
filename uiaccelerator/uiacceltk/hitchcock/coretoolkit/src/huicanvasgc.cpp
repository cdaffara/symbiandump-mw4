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
* Description:   base class for cnaves graphics context
*
*/



#include "huicanvasgc.h"
#include "uiacceltk/HuiGc.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiImage.h"
#include "uiacceltk/HuiCanvasVisual.h"
#include "uiacceltk/HuiCurvePath.h"
#include "uiacceltk/HuiMappingFunctions.h"
#include "HuiRenderSurface.h"
#include "uiacceltk/HuiStatic.h"
#include "huicanvasrenderbuffer.h"

#ifndef __SERIES60_30__     
#include "HuiTargetBitmap.h"
#endif

#include <e32math.h>

#include "alfmoduletestconf.h"
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
    // Provides TLS object data for test cases.
    // This is used only if module test hooks are set on.
    #include "huistatictlsdata.h"
#endif // USE_MODULE_TEST_HOOKS_FOR_ALF
// Provides module test hook defines.
#include "alfmoduletestdefines.h"


EXPORT_C CHuiCanvasGc::CHuiCanvasGc():
    iPenColor(KRgbBlack),
    iPenWidth(1.f),
    iOpacity(1.f),
    iPolygonDrawMode(EHuiNoFill),
    iVerticalTextAlign(EHuiAlignVTop),
    iHorizontalTextAlign(EHuiAlignHLeft),
    iTextStyleId(EHuiTextStyleNormal),
    iEffectiveOpacityEnabled(ETrue)
    {
    }

EXPORT_C CHuiCanvasGc::~CHuiCanvasGc()
    {
    iClippingRegion.Close();
    iDelayedClipRegion.Close();	

    iTempRegion.Close();
    iTempRegion2.Close();
    }

EXPORT_C void CHuiCanvasGc::SetGc(CHuiGc& aGc)
    {
    iGc = &aGc;        
    }
    
EXPORT_C void CHuiCanvasGc::SetVisual(const CHuiCanvasVisual& aVisual)
    {
    iVisual = (CHuiCanvasVisual*)&aVisual;    
    }

EXPORT_C void CHuiCanvasGc::Clip(const TRect& aClipRect)
    {
    if (!iGc)
       {
       return;    
       }
    
    switch (iClipMode)
        {
        case EHuiCanvasClipModeNormal:
        case EHuiCanvasClipModeDelayed:
            {
            if (iClippingRegion.Count())
                {
                CancelClipping();    
                }

            if (!aClipRect.IsEmpty())
                {
                iTempRegion.Clear();
                iTempRegion.AddRect(aClipRect);
                ClipRegion(iTempRegion);
                }
            break;    
            }
        case EHuiCanvasClipModeNone:
        default:
            {
            // Do nothing            
            break;    
            }
        }    
    }

EXPORT_C void CHuiCanvasGc::ClipRegion(const TRegion& aClipRegion)
    {
    if (!iGc)
       {
       return;    
       }

    switch (iClipMode)
        {
        case EHuiCanvasClipModeNormal:
            {
            // If previous clipping region set, cancel it first.
            if (iClippingRegion.Count())
                {
                CancelClipping();    
                }
                
            // Set new region, ignore empty rects if any
            for (TInt i=0; i < aClipRegion.Count(); i++)
                {
                if (!aClipRegion[i].IsEmpty())
                    {
                    iClippingRegion.AddRect(aClipRegion[i]);    
                    }                                        
                }
            
            iClippingRegion.Tidy();
            
            if (iClippingRegion.Count() == 1)
                {
                // If only one rect, then do simple clipping...                    
                iGc->Enable(CHuiGc::EFeatureClipping);
                iGc->PushClip();
                iGc->Clip(iClippingRegion[0]);                
                }
            else if (iClippingRegion.Count() > 1)
                {
                // ...otherewise must do region clipping.
                iGc->Enable(CHuiGc::EFeatureClipping);
                iGc->PushClip();
                if (MaxNumberOfClipRects() > 1)
                    {
                    iGc->Clip(iClippingRegion);                    
                    }
                else
                    {
                    // region clipping is not available, try boundingrect
                    iGc->Clip(iClippingRegion.BoundingRect());                                        
                    }
                }
            else
                {
                // No clip rects set, do nothing here.    
                }                                
            break;
            }
        case EHuiCanvasClipModeDelayed:
            {
            // If previous clipping region set, cancel it first.
            if (iClippingRegion.Count())
                {
                CancelClipping();    
                }
                
            // Set new region, ignore empty rects if any
            for (TInt i=0; i < aClipRegion.Count(); i++)
                {
                if (!aClipRegion[i].IsEmpty())
                    {
                    iClippingRegion.AddRect(aClipRegion[i]);    
                    }                    
                }

            iClippingRegion.Tidy();
            break;    
            }
        case EHuiCanvasClipModeNone:
        default:
            {
            // Do nothing            
            break;    
            }
        }    
    }  

EXPORT_C void CHuiCanvasGc::CancelClipping()
    {
    if (!iGc)
       {
       return;    
       }


    switch (iClipMode)
        {
        case EHuiCanvasClipModeNormal:
            {
            if (iClippingRegion.Count())
                {
                iClippingRegion.Clear();
                iGc->Enable(CHuiGc::EFeatureClipping);
                iGc->PopClip();                            
                }
            break;
            }
        case EHuiCanvasClipModeDelayed:
            {
            iDelayedClipRegion.Clear();
            iClippingRegion.Clear();
            break;    
            }
        case EHuiCanvasClipModeNone:
        default:
            {
            // Do nothing            
            break;    
            }
        }    
    }

EXPORT_C void CHuiCanvasGc::DrawImage(const CHuiTexture& aImage, const THuiRealRect& aDestinationRect)
    {
    if (!iGc)
        {
        return;    
        }

    THuiRealRect srcRect = THuiRealRect(TPoint(0,0), aImage.Size() );
    DrawImage(aImage, aDestinationRect, srcRect);
    }
        
EXPORT_C void CHuiCanvasGc::DrawText(const CHuiTextMesh& aTextMesh, const THuiRealRect& aDestinationRect)
    {
    if (!iGc)
        {
        return;    
        }

    Setup();
    
    EnableBlendingIfNeeded();
    
    THuiTextStyle* textStyle = CHuiStatic::Env().TextStyleManager().TextStyle(iTextStyleId);
    if (textStyle)
        {
        iGc->SetFont(textStyle->Font());        
        }    
    iGc->SetAlign(iHorizontalTextAlign, iVerticalTextAlign);

    EnableDelayedClippingIfNeeded(aDestinationRect);	
    
    while (ClipNext())
        {
        iGc->DrawText(aTextMesh, aDestinationRect, 0.f);                              
        }
    
    DisableDelayedClippingIfNeeded();       
    
    Cleanup();
    }
    
    
EXPORT_C void CHuiCanvasGc::DrawLines(RArray<THuiRealLine>& aLines)
	{
	if (!iGc)
        {
        return;    
        }

    Setup();

    EnableBlendingIfNeeded();

    EnableDelayedClippingIfNeeded(aLines);
	
    while (ClipNext())
        {
        DoDrawLines(aLines);
        }

    DisableDelayedClippingIfNeeded();    

    Cleanup();
	}

EXPORT_C void CHuiCanvasGc::DoDrawLines(RArray<THuiRealLine>& aLines)
	{
    for(TInt i=0; i<aLines.Count(); i++)
 	   	{
 	   	TPoint start = aLines[i].iStart; 
 	   	TPoint end = aLines[i].iEnd;
    	iGc->DrawLine(start, end, iPenWidth);
    	}        
	}
	

EXPORT_C void CHuiCanvasGc::DrawEllipse(const THuiRealRect& aDestinationRect)
	{
	if (!iGc)
	    {
	    return;    
	    }
	double startx = aDestinationRect.iTl.iX + aDestinationRect.Width();
    double starty = aDestinationRect.iTl.iY + aDestinationRect.Height()/2;   
  
    DrawArc(aDestinationRect, THuiRealPoint(startx,starty), THuiRealPoint(startx,starty));	
	}

EXPORT_C void CHuiCanvasGc::DrawRoundRect(const THuiRealRect& aDestinationRect, 
        const THuiRealSize& aSize)
    {   
    if (!iGc)
        {
        return;    
        }

    Setup();

    EnableDelayedClippingIfNeeded(aDestinationRect);    

    while (ClipNext())
        {
        DoDrawRoundRect(aDestinationRect, aSize);
        }
        
    DisableDelayedClippingIfNeeded();    

    Cleanup();
    }

EXPORT_C void CHuiCanvasGc::DoDrawRoundRect(const THuiRealRect& aDestinationRect, 
        const THuiRealSize& aSize)
    {   
    THuiRealSize cornerRectSize = THuiRealSize( 2*aSize.iWidth, 2*aSize.iHeight);
    
    THuiRealPoint topLeftCornerRectTL=aDestinationRect.iTl;
    THuiRealRect topLeftCornerRect = THuiRealRect(topLeftCornerRectTL,cornerRectSize);
    
    THuiRealPoint topRightCornerRectTL=THuiRealPoint
    (aDestinationRect.iTl.iX + aDestinationRect.Width()-2*(aSize.iWidth), aDestinationRect.iTl.iY);
    THuiRealRect topRightCornerRect = THuiRealRect(topRightCornerRectTL, cornerRectSize);
    
    THuiRealPoint bottomRightCornerRectTL = THuiRealPoint
    (aDestinationRect.iBr.iX-(2*aSize.iWidth),aDestinationRect.iBr.iY-(2*aSize.iHeight));
    THuiRealRect bottomRightCornerRect = THuiRealRect(bottomRightCornerRectTL, cornerRectSize);
    
    THuiRealPoint bottomLeftCornerRectTL = THuiRealPoint(aDestinationRect.iTl.iX, 
            aDestinationRect.iTl.iY + aDestinationRect.Height() -2*(aSize.iHeight));   
    THuiRealRect bottomLeftCornerRect = THuiRealRect(bottomLeftCornerRectTL,cornerRectSize);
 
    THuiRealPoint topLeftArcStart = THuiRealPoint(topLeftCornerRectTL.iX + aSize.iWidth, topLeftCornerRectTL.iY);
    THuiRealPoint topLeftArcEnd = THuiRealPoint(topLeftCornerRectTL.iX, topLeftCornerRectTL.iY + aSize.iHeight);
    DoDrawArc(topLeftCornerRect, topLeftArcStart,topLeftArcEnd);
    
    THuiRealPoint topRightArcStart = THuiRealPoint(topRightCornerRectTL.iX + 2*aSize.iWidth, 
            topRightCornerRectTL.iY + aSize.iHeight);
    THuiRealPoint topRightArcEnd = THuiRealPoint(topRightCornerRectTL.iX + aSize.iWidth, topRightCornerRectTL.iY);
    DoDrawArc(topRightCornerRect, topRightArcStart, topRightArcEnd);
    
    THuiRealPoint bottomRightArcStart = THuiRealPoint(bottomRightCornerRectTL.iX + aSize.iWidth, 
            bottomRightCornerRectTL.iY + 2*aSize.iHeight);
    THuiRealPoint bottomRightArcEnd = THuiRealPoint(bottomRightCornerRectTL.iX + 2*aSize.iWidth, 
            bottomRightCornerRectTL.iY + aSize.iHeight);
    DoDrawArc(bottomRightCornerRect,bottomRightArcStart ,bottomRightArcEnd);
    
    THuiRealPoint bottomLeftArcStart = THuiRealPoint(bottomLeftCornerRectTL.iX, bottomLeftCornerRectTL.iY +
            aSize.iHeight);
    THuiRealPoint bottomLeftArcEnd = THuiRealPoint(bottomLeftCornerRectTL.iX + 
            aSize.iWidth, bottomLeftCornerRectTL.iY + 2*aSize.iHeight);
    DoDrawArc(bottomLeftCornerRect, bottomLeftArcStart, bottomLeftArcEnd);
    if(iPolygonDrawMode)
        {
        RArray<THuiRealRect> rects;
        rects.Append(THuiRealRect(topLeftArcStart,bottomRightArcStart));        
        rects.Append(THuiRealRect(topLeftArcEnd, THuiRealSize(aSize.iWidth,aDestinationRect.Height()-(2*aSize.iHeight))));
        rects.Append(THuiRealRect(THuiRealPoint(topRightArcStart.iX - aSize.iWidth,topRightArcStart.iY),
                 THuiRealSize(aSize.iWidth,aDestinationRect.Height()-(2*aSize.iHeight))));
        
        DoDrawRects(rects);
        rects.Close();
        }
    else
        {
        RArray<THuiRealLine> lines;
        lines.Append(THuiRealLine(topLeftArcStart,topRightArcEnd));        
        lines.Append(THuiRealLine(topLeftArcEnd, bottomLeftArcStart));
        lines.Append(THuiRealLine(bottomLeftArcEnd, bottomRightArcStart));
        lines.Append(THuiRealLine(bottomRightArcEnd, topRightArcStart));
        DoDrawLines(lines);
        lines.Close();
        }
    }


EXPORT_C void CHuiCanvasGc::DrawPie(const THuiRealRect& aDestinationRect, 
        const THuiRealPoint& aStart, const THuiRealPoint& aEnd)
    {   
    if (!iGc)
       {
       return;    
       }

    Setup();

    EnableDelayedClippingIfNeeded(aDestinationRect);    

    while (ClipNext())
        {
        DoDrawPie( aDestinationRect, aStart, aEnd);
        }

    DisableDelayedClippingIfNeeded();    

    Cleanup();
    }

EXPORT_C void CHuiCanvasGc::DoDrawPie(const THuiRealRect& aDestinationRect, 
        const THuiRealPoint& aStart, const THuiRealPoint& aEnd)
    {
    double startx = aDestinationRect.iTl.iX + aDestinationRect.Width()/2;
    double starty = aDestinationRect.iTl.iY + aDestinationRect.Height()/2;      
    DoDrawPie( aDestinationRect, aStart, aEnd);
    }

EXPORT_C void CHuiCanvasGc::DrawArc(const THuiRealRect& aDestinationRect, 
        const THuiRealPoint& aStart, const THuiRealPoint& aEnd)
    {    
	if (!iGc)
        {
        return;    
        }
    
    Setup();

    EnableBlendingIfNeeded();
    
    EnableDelayedClippingIfNeeded(aDestinationRect);    

    while (ClipNext())
        {
        DoDrawArc(aDestinationRect, aStart, aEnd);
        }
    
    DisableDelayedClippingIfNeeded();    
    
    Cleanup();
	}

EXPORT_C void CHuiCanvasGc::DoDrawArc(const THuiRealRect& aDestinationRect, 
        const THuiRealPoint& aStart, const THuiRealPoint& aEnd)
    {    
    TPoint start = aStart;
    TPoint end = aEnd;
    TReal startx = aDestinationRect.iTl.iX;
    TReal starty = aDestinationRect.iTl.iY;
    TReal rx = aDestinationRect.Width()/2;
    TReal ry = aDestinationRect.Height()/2; 
    startx += rx;
    starty += ry;
    //Check if given start and end point lie on ellipse, if not, 
    //Find the point of intersection of ellipse with line joing to ellipse origin and start point
    //and ellipse origin and end point
    if ( ((aStart.iX-startx)*(aStart.iX-startx)/(rx*rx) + (aStart.iY-starty)*(aStart.iY-starty)/(ry*ry))!=1 )
       {
       TReal m = (aStart.iY-starty)/(aStart.iX - startx);
       TReal intersectionPointX;
       TReal src = (rx*rx*ry*ry)/((ry*ry)+(rx*rx*m*m));
       Math::Sqrt(intersectionPointX,src);
       TReal intersectionPointY = starty + (m*intersectionPointX);
       intersectionPointX = intersectionPointX + startx;       
       start.iX = intersectionPointX;
       start.iY = intersectionPointY;
       }
    if ( ((aEnd.iX-startx)*(aEnd.iX-startx)/(rx*rx) + (aEnd.iY-starty)*(aEnd.iY-starty)/(ry*ry))!=1 )
       {
       TReal m = (aEnd.iY-starty)/(aEnd.iX - startx);
       TReal intersectionPointX;
       TReal src = (rx*rx*ry*ry)/((ry*ry)+(rx*rx*m*m));
       Math::Sqrt(intersectionPointX,src);
       TReal intersectionPointY = (m*intersectionPointX) + starty;
       intersectionPointX = intersectionPointX + startx;       
       end.iX = intersectionPointX;
       end.iY = intersectionPointY;
       }  
    TReal startAngle = 0;
    Math::ATan(startAngle,(starty-start.iY),(start.iX-startx));
    startAngle = (startAngle*180)/KPi;
   
    TReal endAngle = 0;
    Math::ATan(endAngle,(starty-end.iY),(end.iX-startx));
    endAngle = (endAngle*180)/KPi;
   
    if ( endAngle <= startAngle )
        endAngle = 360 + endAngle;
   
    // this is very slow, needs to be re-implemented    
    if(iPolygonDrawMode)
       {     
       TReal x,y,newx,newy;
       x = startx;
       y = starty;
       newx = x;
       newy = y;
       RArray<THuiRealPoint> points;
       for(TInt i = -endAngle; i <-startAngle ; i++)
	        {
	        double a = double(i) * KPi / 180.0;        
	        double sin, cos;
	        
	        Math::Sin( sin, a );
	        Math::Cos( cos, a );

	        newx = startx + cos * rx;
	        newy = starty + sin * ry;        
	        
	        points.Append(THuiRealPoint(x,y));
	        points.Append(THuiRealPoint(newx, newy));
				        
	        x = newx;
	        y = newy;
	        }
	    DoDrawPolygon(points);
	    points.Close();
	    }
    else
	    {
	    CHuiCurvePath* path = NULL;
	    TRAPD(error, path = CHuiCurvePath::NewL());
	    if(error ==KErrNone)
	    	{
	    	TRAP(error,path->AppendArcL(THuiRealPoint(startx, starty), THuiRealSize(rx, ry), -startAngle, -endAngle, 1));
	    	if(error ==KErrNone)
	    		{
				THuiLinearMappingFunction alphaFunc;
				alphaFunc.iOffset = iOpacity;
				THuiLinearMappingFunction widthFunc;
				widthFunc.iOffset = iPenWidth;
				
				const TPoint pt(0,0);
				RHuiOwnedPointer<CHuiCurvePath> iPath;
				iPath.Set(path, EHuiHasOwnership);
            
  				iGc->DrawPath(iPath.Ref(), pt, 0.0, 1.0, &alphaFunc, &widthFunc);
	    		}
	    	}
		}    
	}

		
EXPORT_C void CHuiCanvasGc::DrawPoints(RArray<THuiRealPoint>& aPoints)
	{
	if (!iGc)
        {
        return;    
        }

    Setup();
      
    EnableBlendingIfNeeded();

    EnableDelayedClippingIfNeeded(aPoints);    

    while (ClipNext())
        {            
        DoDrawPoints(aPoints);
        }

    DisableDelayedClippingIfNeeded(); 

    Cleanup(); 
    }

EXPORT_C void CHuiCanvasGc::DoDrawPoints(RArray<THuiRealPoint>& aPoints)
	{
    for(TInt i=0; i< aPoints.Count(); i++)
 	   	{
 	   	TInt TlOffset = TInt(iPenWidth/2);
		TInt BrOffset = TInt(TReal(iPenWidth)/2.f + 0.5f); 	   	
 	   	TPoint Tl = TPoint(aPoints[i].iX - TlOffset, aPoints[i].iY - TlOffset);
		TPoint Br = TPoint(aPoints[i].iX + BrOffset, aPoints[i].iY + BrOffset);
		iGc->DrawRect(TRect(Tl, Br));
    	}        
    }

	
EXPORT_C void CHuiCanvasGc::DrawPolygon(RArray<THuiRealPoint>& aPoints)
	{
	if (!iGc)
        {
        return;    
        }
        
    Setup();
  
    EnableBlendingIfNeeded();

    EnableDelayedClippingIfNeeded(aPoints);    

    while (ClipNext())
        {                    
        DoDrawPolygon(aPoints);
        }

    DisableDelayedClippingIfNeeded(); 

    Cleanup();       
	}

EXPORT_C void CHuiCanvasGc::DoDrawPolygon(RArray<THuiRealPoint>& aPoints)
	{
    TInt i=0;
    for(; i<(aPoints.Count()-1); i++)
 	   	{
 	   	iGc->DrawLine(aPoints[i], aPoints[i+1], iPenWidth);
    	}
    iGc->DrawLine(aPoints[i], aPoints[0], iPenWidth);        
	}

	
EXPORT_C void CHuiCanvasGc::DrawRects(RArray<THuiRealRect>& aRects)
	{
	if (!iGc)
        {
        return;    
        }

    Setup();
  
    EnableBlendingIfNeeded();
        
    EnableDelayedClippingIfNeeded(aRects);

    while (ClipNext())
        {
        DoDrawRects(aRects);
        }

    DisableDelayedClippingIfNeeded();                

    Cleanup();
    
    AMT_MAP_SET_VALUE_IF( iVisual && aRects.Count() > 0, iRectMap, 
                          AMT_MAP_CPTR_TO_KEY_CAST( iVisual ), aRects[ aRects.Count() - 1 ].Round(),
                          EAlfModuleTestTypeCoreToolkitDrawWindow );
	}

EXPORT_C void CHuiCanvasGc::DoDrawRects(RArray<THuiRealRect>& aRects)
	{
	for(TInt i=0; i<aRects.Count(); i++)
		{
		TRect rect = aRects[i].Round();
		if (rect.Height() > 0 && rect.Width() > 0)
		    {		        	
        	if(iPolygonDrawMode == 0)
        	    {
        	    // By definition, symbian TRect Br coordinates are outside of the rect so subract those here first
        	    rect.iBr.iX--;
        	    rect.iBr.iY--;
        	    
        	    // TODO: following does overlapping drawing at the corners, it could be fixed but might not be 
        	    // trivial if pensize is bigger than 1
                if (rect.Width())
                    iGc->DrawLine(TPoint(rect.iTl.iX, rect.iTl.iY), TPoint(rect.iBr.iX, rect.iTl.iY), iPenWidth); // Top
                
                if (rect.Height() > 2 && rect.Width() > 1)
                    iGc->DrawLine(TPoint(rect.iBr.iX, rect.iTl.iY), TPoint(rect.iBr.iX, rect.iBr.iY), iPenWidth); // Right
                
                if (rect.Width() && rect.Height() > 1)
                    iGc->DrawLine(TPoint(rect.iTl.iX, rect.iBr.iY), TPoint(rect.iBr.iX, rect.iBr.iY), iPenWidth); // Bottom
                
                if (rect.Height() > 2)
                    iGc->DrawLine(TPoint(rect.iTl.iX, rect.iTl.iY), TPoint(rect.iTl.iX,rect.iBr.iY), iPenWidth); // Left        	    
        	    }
        	else
        	    {
      		  	iGc->DrawRect(rect);
        	    }
		    }    	    
		}            
	}

EXPORT_C void CHuiCanvasGc::SetTextStyle(TInt aStyleId)
    {
    iTextStyleId = aStyleId;    
    }

EXPORT_C TInt CHuiCanvasGc::TextStyle() const
    {
    return iTextStyleId;    
    }
    
EXPORT_C void CHuiCanvasGc::SetPenColor(const TRgb& aColor)
	{
	iPenColor = aColor;
	}
	
EXPORT_C void CHuiCanvasGc::SetPenWidth(const TReal32& aWidth)
	{
	iPenWidth = aWidth;
	}
	
EXPORT_C void CHuiCanvasGc::SetOpacity(const TReal32& aOpacity)
	{
	iOpacity = aOpacity;
	}

EXPORT_C void CHuiCanvasGc::SetPolygonDrawMode(THuiFillMode aPolygonDrawMode)
	{
	iPolygonDrawMode = aPolygonDrawMode;
	}
	
EXPORT_C void CHuiCanvasGc::SetTextAlign(THuiAlignHorizontal aAlignHorizontal, THuiAlignVertical aAlignVertical)
	{
	iVerticalTextAlign = aAlignVertical;
    iHorizontalTextAlign = aAlignHorizontal;
    }
    
EXPORT_C void CHuiCanvasGc::SetDefaults()
	{
	SetTextStyle(0);
    SetPenColor(TRgb(255,255,255));
	SetPenWidth(1.0);
	SetOpacity(1.0);
	SetPolygonDrawMode(EHuiNoFill);
	SetTextAlign(EHuiAlignHLeft, EHuiAlignVTop);
    SetDrawMode(EHuiCanvasDrawModeBlend);
    EnableEffectiveOpacity(ETrue);    
    SetPolygonFillTexture(NULL);
    SetPolygonFillTextureOrigin(TPoint(0,0));
	CancelClipping();
	}
    
    // Transformations
EXPORT_C void CHuiCanvasGc::LoadIdentity()
	{
    iGc->Pop(EHuiGcMatrixModel);
    iGc->Push(EHuiGcMatrixModel);	
	}
	
EXPORT_C void CHuiCanvasGc::Translate(const TReal32& aX, const TReal32& aY, const TReal32& aZ)
	{
	if (!iGc)
        {
        return;    
        }
        
    iGc->SetAlign(EHuiAlignHLeft, EHuiAlignVTop);        
    iGc->Translate(EHuiGcMatrixModel, aX, aY, aZ);    
    }

EXPORT_C void CHuiCanvasGc::Scale(const TReal32& aX, const TReal32& aY, const TReal32& aZ)
	{
	if (!iGc)
        {
        return;    
        }
        
    iGc->SetAlign(EHuiAlignHLeft, EHuiAlignVTop);        
    iGc->Scale(EHuiGcMatrixModel, aX, aY, aZ);
	}

EXPORT_C void CHuiCanvasGc::Rotate(const TReal32& aAngle, const TReal32& aX, const TReal32& aY, const TReal32& aZ)    
	{
	if (!iGc)
        {
        return;    
        }
        
    iGc->SetAlign(EHuiAlignHLeft, EHuiAlignVTop);        
    iGc->Rotate(EHuiGcMatrixModel, aAngle, aX, aY, aZ);
	}

EXPORT_C TInt CHuiCanvasGc::TextVerticalAlign() const
    {
    return iVerticalTextAlign;    
    }

EXPORT_C TInt CHuiCanvasGc::TextHorizontalAlign() const
    {
    return iHorizontalTextAlign;    
    }
    
EXPORT_C TReal32 CHuiCanvasGc::PenWidth() const
    {
    return iPenWidth;    
    }

EXPORT_C THuiFillMode CHuiCanvasGc::PolygonDrawMode() const
    {
    return iPolygonDrawMode;    
    }
    
EXPORT_C void CHuiCanvasGc::DrawImage(const CHuiTexture& aImage, const THuiRealRect& aDestinationRect, const THuiRealRect& aSourceRect)
    {
    DrawImage(aImage, aDestinationRect, aSourceRect, CHuiGc::EStretchFull);        
    }

EXPORT_C void CHuiCanvasGc::DrawImage(const CHuiTexture& aImage, 
    const THuiRealRect& aDestinationRect, 
    const THuiRealRect& aSourceRect,
    CHuiGc::TStretchMode aStretchMode)
    {
    if (!iGc)
        {
        return;    
        }

    Setup();

    THuiImage image = THuiImage(aImage);           

    // If renderer supports texture coordinates, we can use those to specify subset of image
    EnableBlendingIfNeeded(&aImage);

    TSize textureSize = aImage.Size();        

    if  (textureSize.iWidth > 0 && textureSize.iHeight > 0)
        {
        THuiRealRect fullTextureRect = THuiRealRect(TPoint(0,0), aImage.Size());
        THuiRealRect partialTextureRect = fullTextureRect;
        
        // Calculate intersection
       	if (partialTextureRect.iTl.iX<aSourceRect.iTl.iX)
    		partialTextureRect.iTl.iX=aSourceRect.iTl.iX;
	    if (partialTextureRect.iTl.iY<aSourceRect.iTl.iY)
		    partialTextureRect.iTl.iY=aSourceRect.iTl.iY;
	    if (partialTextureRect.iBr.iX>aSourceRect.iBr.iX)
		    partialTextureRect.iBr.iX=aSourceRect.iBr.iX;
    	if (partialTextureRect.iBr.iY>aSourceRect.iBr.iY)
	        partialTextureRect.iBr.iY=aSourceRect.iBr.iY;
    	
        TReal32 tlX = TReal32(partialTextureRect.iTl.iX)/TReal32(textureSize.iWidth);
        TReal32 tlY = TReal32(partialTextureRect.iTl.iY)/TReal32(textureSize.iHeight);
        TReal32 brX = TReal32(partialTextureRect.iBr.iX)/TReal32(textureSize.iWidth);
        TReal32 brY = TReal32(partialTextureRect.iBr.iY)/TReal32(textureSize.iHeight);        
        
        image.SetTexCoords(tlX,tlY,brX, brY);          

        TSize usedSize = aDestinationRect.Size().Round();
                
        if(aStretchMode != CHuiGc::EStretchFull && 
           aStretchMode != CHuiGc::EStretchHorizontal && 
           partialTextureRect.Height() < usedSize.iHeight)
            {
            usedSize.iHeight = partialTextureRect.Height();  
            }
        
        if(aStretchMode != CHuiGc::EStretchFull &&
           aStretchMode != CHuiGc::EStretchVertical && 
           partialTextureRect.Width() < usedSize.iWidth)
            {
            usedSize.iWidth = partialTextureRect.Width();  
            }
        
        TRect updatedArea = TRect(aDestinationRect.iTl.Round(), usedSize);

        if (usedSize.iHeight > 0 && usedSize.iWidth > 0)
            {                
            EnableDelayedClippingIfNeeded(updatedArea);
			
            while (ClipNext())
                {
                iGc->DrawImage(image, aDestinationRect.iTl, usedSize);                                
                } 
            DisableDelayedClippingIfNeeded();    
            }
        }    

    Cleanup();        
    }
    
EXPORT_C void CHuiCanvasGc::Clear(const THuiRealRect& aRect)
    {
    if (!iGc)
        {
        return;    
        }

    TReal32 effectiveOpacity = 1.f;
    if (iVisual && iEffectiveOpacityEnabled)
        {
        effectiveOpacity = iVisual->EffectiveOpacity();    
        }
    iGc->SetPenAlpha(TInt(iOpacity * effectiveOpacity * 255));
    iGc->SetPenColor(iPenColor);
    iGc->SetAlign(EHuiAlignHLeft, EHuiAlignVTop);            
    iGc->Disable(CHuiGc::EFeatureBlending);
    iGc->Enable(CHuiGc::EFeatureClipping);            
    iGc->PushClip();
    iGc->Clip(aRect.Round()); // takes transformations into account
    iGc->Clear(); 
    iGc->PopClip();
    iGc->Enable(CHuiGc::EFeatureBlending);
    }

EXPORT_C void CHuiCanvasGc::ClearWithSkinBackground(const THuiRealRect& /*aRect*/)
    {
    // Implementation is renderspecific        
    }

    
EXPORT_C void CHuiCanvasGc::ClearWithBackgroundItems(const THuiRealRect& /*aRect*/,
        const RArray<THuiDisplayBackgroundItem>& /*aItems*/)
    {        
    // Implementation is renderspecific   
    }
    
EXPORT_C void CHuiCanvasGc::EnableBlendingIfNeeded(const CHuiTexture* aTexture)
    {
    if (iDrawMode == EHuiCanvasDrawModeBlend)
        {
        iGc->Enable(CHuiGc::EFeatureBlending);    

        // Optimization, enable blending only if really needed, on some HW blending slows performance.
        /*
        if ((iGc->PenColor().Alpha() == 255) && (iGc->PenAlpha() == 255) && (!aTexture || !aTexture->HasAlpha()))
            {
            iGc->Disable(CHuiGc::EFeatureBlending);   
            }
        else
            {
            iGc->Enable(CHuiGc::EFeatureBlending);    
            }
        */                
        }
    else if (iDrawMode == EHuiCanvasDrawModeNormal)
        {
        iGc->Disable(CHuiGc::EFeatureBlending);        
        }
    else
        {
        // Unknown mode !
        }
    }

THuiCanvasDrawMode CHuiCanvasGc::DrawMode() const
    {
    return iDrawMode;
    }


    
CHuiCanvasVisual* CHuiCanvasGc::Visual() const
    {
    return iVisual;    
    }

TRgb CHuiCanvasGc::PenColor() const
    {
    return iPenColor;    
    }
    
TReal32 CHuiCanvasGc::Opacity() const
    {
    return iOpacity;
    
    }
    
EXPORT_C CHuiCanvasGc::TClipRectVisibility CHuiCanvasGc::IsClipped(const TRect& aRect) const
    {
    TClipRectVisibility isClipped = EFullyOutside;
    if (!iGc)
        {
        return isClipped;
        }

    // First check the active clipping region from GC...
    
    // aRect is untransformed, activeClippingRegion is transformed
    // so we must transform aRect first when we comapre it to activeClippingRegion...
    THuiRealRect transformed = aRect;
    iGc->TransformDirtyRect(transformed );
    const TRegion& activeClippingRegion = iGc->ClipRegion();

    isClipped = IsClipped(transformed.Round(), activeClippingRegion);
    //...fully outside of GC so it would not be drawn regardless of
    // the iClipRegion (if it is trying to do clipping), we can return immediately.
    if (isClipped == EFullyOutside && activeClippingRegion.Count())
        {        
        return isClipped;
        }            
    //...not clipped by active clip region, check our own iClipRegion
    if (iClippingRegion.Count())
        {
        isClipped = IsClipped(aRect, iClippingRegion);
        }
    
    return isClipped;
    }

CHuiCanvasGc::TClipRectVisibility CHuiCanvasGc::IsClipped(const TRect& aRect, const TRegion& aClippingRegion) const
    {
    TClipRectVisibility isClipped = EFullyOutside;
    if (!aClippingRegion.Count())
        {
        return EFullyInside;
        }
    
    if (!aRect.IsEmpty() && aClippingRegion.Count())
        {    
#if 0            
        TBool test = EFalse;        
        if (test)
            {
            for (TInt i=0; i<aClippingRegion.Count(); i++)
                {
#ifdef _DEBUG
                RDebug::Print(_L("aClippingRegion Rect: %d %d %d %d"), 
                        aClippingRegion[i].iTl.iX, 
                        aClippingRegion[i].iTl.iY,
                        aClippingRegion[i].iBr.iX,
                        aClippingRegion[i].iBr.iY);
#endif
                }                            
            }
#endif        
        
        if (aClippingRegion.Intersects(aRect))
            {            
            iTempRegion.Clear();
            iTempRegion2.Clear();

            iTempRegion.AddRect(aRect);
            
            iTempRegion2.Intersection(aClippingRegion, iTempRegion);
            iTempRegion2.Tidy();
            
            // Assume it is only partially inside region -> Clipped
            isClipped = EPartialOverlap;
            
            if (iTempRegion2.Count() == 1)
                {
                if (iTempRegion2[0] == aRect)
                    {
                    // Fully inside region -> Not clipped
                    isClipped = EFullyInside;    
                    }                
                }
            
            }
        else
            {
            // No overlap -> aRect is completely outside region -> Clipped
            isClipped = EFullyOutside;    
            }                    
        }
    return isClipped;
    }


EXPORT_C CHuiCanvasGc::TClipRectVisibility CHuiCanvasGc::IsClipped(const TPoint& aPoint) const
    {
    return IsClipped(aPoint, iClippingRegion);
    }

CHuiCanvasGc::TClipRectVisibility CHuiCanvasGc::IsClipped(const TPoint& aPoint, const TRegion& aClippingRegion) const
    {
    if (iClippingRegion.Count() && !aClippingRegion.Contains(aPoint))
        {
        return EFullyOutside;
        }
    else 
        {
        return EFullyInside;
        }
    }

CHuiCanvasGc::TClipRectVisibility CHuiCanvasGc::IsClipped(const TRegion& aTestedRegion, const TRegion& aClippingRegion) const
    {
    TClipRectVisibility isWholeRegionClipped = EUnknown;
    TInt count = aTestedRegion.Count(); 
    
    if (count == 0)
        {
        isWholeRegionClipped = EFullyOutside;
        }
    
    for (TInt i=0; i<count; i++)
        {
        TClipRectVisibility isRectClipped = IsClipped(aTestedRegion[i], aClippingRegion);
        if (isWholeRegionClipped == EFullyInside)
            {
            if (isRectClipped == EFullyOutside ||
                isRectClipped == EPartialOverlap )
                {
                isWholeRegionClipped = EPartialOverlap;
                }            
            }
        else if (isWholeRegionClipped == EFullyOutside)
            {
            if (isRectClipped == EFullyInside ||
                isRectClipped == EPartialOverlap )
                {
                isWholeRegionClipped = EPartialOverlap;
                }                        
            }
        else if (isWholeRegionClipped == EPartialOverlap)
            {
            // No changes to isWholeRegionClipped in this case and no need to look further
            break;
            }
        else
            {
            isWholeRegionClipped = isRectClipped;
            }                   
        }
    return isWholeRegionClipped;
    }


EXPORT_C void CHuiCanvasGc::SetClippingMode(THuiCanvasClipMode aClipMode)
    {
    iClipMode = aClipMode;    
    }

EXPORT_C void CHuiCanvasGc::SetDrawMode(THuiCanvasDrawMode aDrawMode)
    {
    iDrawMode = aDrawMode;
    }

EXPORT_C CHuiCanvasGc::TClipRectVisibility CHuiCanvasGc::EnableDelayedClippingIfNeeded(const THuiRealRect& aUpdatedRect)
    {
    iDelayedClipCount = 0;
    iDelayedClipVisibility = EPartialOverlap;
    if (iClipMode == EHuiCanvasClipModeDelayed)
        {            
        iDelayedClipVisibility = IsClipped(aUpdatedRect.Round());
        if ( iDelayedClipVisibility ==  EPartialOverlap )
            {
            // Could copy only needed rects to avoid excessive clipping
            iDelayedClipRegion.Copy(iClippingRegion);
            }
        }                        
    return iDelayedClipVisibility;
    }

EXPORT_C CHuiCanvasGc::TClipRectVisibility CHuiCanvasGc::EnableDelayedClippingIfNeeded(const RArray<THuiRealRect>& aUpdatedRects)
    {
    iDelayedClipCount = 0;
    iDelayedClipVisibility = EPartialOverlap;
    if (iClipMode == EHuiCanvasClipModeDelayed)
        {            
        for (TInt i=0; i<aUpdatedRects.Count(); i++)
            {
            iDelayedClipVisibility = IsClipped(aUpdatedRects[i].Round());            
            if (iDelayedClipVisibility ==  EPartialOverlap)
                {
                // Could copy only needed rects to avoid excessive clipping
                iDelayedClipRegion.Copy(iClippingRegion);
                break;
                }                                                
            }        
        }
    return iDelayedClipVisibility;
    }
    
    
EXPORT_C CHuiCanvasGc::TClipRectVisibility CHuiCanvasGc::EnableDelayedClippingIfNeeded(const RArray<THuiRealPoint>& aUpdatedPoints)
    {
    iDelayedClipCount = 0;
    iDelayedClipVisibility = EPartialOverlap;

    if (iClipMode == EHuiCanvasClipModeDelayed)
        {
        for (TInt i=0; i<aUpdatedPoints.Count();i++)
            {
            iDelayedClipVisibility = IsClipped(aUpdatedPoints[i].Round());
            if (iDelayedClipVisibility != EFullyOutside)
                {
                // Could copy only needed rects to avoid excessive clipping
                iDelayedClipRegion.Copy(iClippingRegion);
                break;
                }                                
            }                    
        }
    return iDelayedClipVisibility;
    }

EXPORT_C CHuiCanvasGc::TClipRectVisibility CHuiCanvasGc::EnableDelayedClippingIfNeeded(const RArray<THuiRealLine>& aUpdatedLines)
    {
    iDelayedClipCount = 0;
    TClipRectVisibility vis1 = EPartialOverlap;
    TClipRectVisibility vis2 = EPartialOverlap;

    if (iClipMode == EHuiCanvasClipModeDelayed)
        {
        for (TInt i=0; i<aUpdatedLines.Count();i++)
            {
            TPoint start = aUpdatedLines[i].iStart.Round();
            TPoint end = aUpdatedLines[i].iEnd.Round();            
            vis1 = IsClipped(start);
            vis2 = IsClipped(end);
            if ((vis1 != EFullyOutside) || (vis2 != EFullyOutside))
                {
                // Could copy only needed rects to avoid excessive clipping
                iDelayedClipRegion.Copy(iClippingRegion);
                break;
                }

		    // both points lie outside - but the line may still intersect the region, 
            // so represent the line as a rect and try an intersection test...
            TRect lineBoundingRect(start, end);
            lineBoundingRect.Normalize();
            lineBoundingRect.iBr += TPoint(1, 1);
            if (IsClipped(lineBoundingRect, iClippingRegion) != EFullyOutside)
                {
                iDelayedClipRegion.Copy(iClippingRegion);
                vis1 = EPartialOverlap;
                break;
                }                    
            }                    
        }        
    if (vis1 == EFullyOutside && vis2 == EFullyOutside)
        {
        iDelayedClipVisibility = EFullyOutside;
        }
    else
        {
        iDelayedClipVisibility = EPartialOverlap;
        }
    return iDelayedClipVisibility;
    }

    
    
EXPORT_C TBool CHuiCanvasGc::ClipNext()
    {
    if (iDelayedClipVisibility == EFullyOutside)
        {
        // If drawing would fall completely outside the clipping region, we are done. 
        return EFalse;
        }

    const TBool clipOneByOne = iDelayedClipRegion.Count() > MaxNumberOfClipRects();
    if (iDelayedClipVisibility != EFullyOutside && 
        iDelayedClipRegion.Count() && 
        iDelayedClipCount < iDelayedClipRegion.Count())        
        {
        if (iDelayedClipRectPushed)
            {
            iGc->PopClip();    
            }                
      	
      	iGc->Enable(CHuiGc::EFeatureClipping);
      	iGc->PushClip();
        iDelayedClipRectPushed = ETrue;
        
        if (clipOneByOne)
            {
            iGc->Clip(iDelayedClipRegion[iDelayedClipCount]);                            
            }
        else
            {
            iGc->Clip(iDelayedClipRegion);                                
            }                            
        }    

    TBool continueDrawing = EFalse;
    if (clipOneByOne)
        {
        // Clip one by one.            
        iDelayedClipCount++;
        continueDrawing = iDelayedClipCount <= iDelayedClipRegion.Count();
        }
    else
        {
        // Drawing once is sufficient - all clipping can be done.
        continueDrawing = !iDelayedClipCount;
        iDelayedClipCount++;
        }

    return continueDrawing;    
    }

EXPORT_C void CHuiCanvasGc::DisableDelayedClippingIfNeeded()
    {
    iDelayedClipVisibility = EPartialOverlap;
    iDelayedClipCount = 0;
    if (iDelayedClipRegion.Count())
        {
        if (iDelayedClipRectPushed)
            {
            iGc->PopClip();
            iDelayedClipRectPushed = EFalse;    
            }                

        iDelayedClipRegion.Clear();
        }
    }

EXPORT_C void CHuiCanvasGc::Setup()
    {
    iOldPencolor = iGc->PenColor();
    iOldPenAlpha = iGc->PenAlpha();
    iGc->SetAlign(EHuiAlignHLeft, EHuiAlignVTop);    
    TReal32 effectiveOpacity = 1.f;
    if (iVisual && iEffectiveOpacityEnabled)
        {
        effectiveOpacity = iVisual->EffectiveOpacity();    
        }
    iGc->SetPenAlpha(TInt(iOpacity * effectiveOpacity * 255));
    iGc->SetPenColor(iPenColor);        
    }

EXPORT_C void CHuiCanvasGc::Cleanup()
    {
	iGc->SetPenAlpha(iOldPenAlpha);  
	iGc->SetPenColor(iOldPencolor);	
    }

EXPORT_C TInt CHuiCanvasGc::MaxNumberOfClipRects() const
    {
    // By default only one at the time, but most renders override this
    return 1;    
    }

CHuiGc* CHuiCanvasGc::Gc() const
    {
    return iGc;
    }

EXPORT_C CHuiCanvasRenderBuffer* CHuiCanvasGc::CreateRenderBufferL(const TSize& /*aSize*/)
    {
    // Only deriving classes really create render targets
    return NULL;
    }

EXPORT_C void CHuiCanvasGc::DrawImage(const CHuiCanvasRenderBuffer& aImage, const THuiRealPoint& aDestinationPoint)
    {
    if (!iGc)
        {
        return;    
        }
        
    THuiRealSize destinationSize = aImage.Size();
    THuiRealRect destinationRect(aDestinationPoint, destinationSize);
    
    Setup();
  
    EnableBlendingIfNeeded();

    EnableDelayedClippingIfNeeded(destinationRect.Round());    

    while (ClipNext())
        {                    
        DoDrawRenderBuffer(aImage, aDestinationPoint);
        }

    DisableDelayedClippingIfNeeded(); 

    Cleanup();
    
    AMT_MAP_SET_VALUE_IF( iVisual, iRectMap,
                          AMT_MAP_CPTR_TO_KEY_CAST( iVisual ),
                          destinationRect.Round(), 
                          EAlfModuleTestTypeCoreToolkitDrawFromRenderBuffer );
    }

EXPORT_C void CHuiCanvasGc::ClearRenderBuffer( CHuiCanvasRenderBuffer& aImage, const TRect & aRect )
    {
    DoClearRenderBuffer(aImage, aRect);
    }
EXPORT_C void CHuiCanvasGc::DoClearRenderBuffer( CHuiCanvasRenderBuffer& /*aImage*/, const TRect & /*aRect*/ )
    {
    // only deriving classes implement this
    }


EXPORT_C void CHuiCanvasGc::DoDrawRenderBuffer(const CHuiCanvasRenderBuffer& /*aImage*/, const THuiRealPoint& /*aDestinationPoint*/)
    {
    // Only deriving classes really draw render targets
    }

EXPORT_C void CHuiCanvasGc::PushTransformationMatrix()
    {
    if (!iGc)
        {
        return;    
        }
    
    iGc->Push(EHuiGcMatrixModel);
    }

EXPORT_C void CHuiCanvasGc::PopTransformationMatrix()
    {
    if (!iGc)
        {
        return;    
        }
    
    iGc->Pop(EHuiGcMatrixModel);    
    }

void CHuiCanvasGc::RestoreState() const
    {
    if (!iGc)
        {
        return;    
        }

    iGc->RestoreState();    
    }

EXPORT_C void CHuiCanvasGc::RestoreFlaggedState() const
    {
    // Default implementation does not support flagged states in gc
    RestoreState();
    }

void CHuiCanvasGc::EnableEffectiveOpacity(TBool aEnable)
    {
    iEffectiveOpacityEnabled = aEnable;
    }

EXPORT_C void CHuiCanvasGc::SetPolygonFillTexture(CHuiTexture* aTexture)
    {
    iPolygonFillTexture = aTexture;
    }

EXPORT_C CHuiTexture* CHuiCanvasGc::PolygonFillTexture() const 
    {
    return iPolygonFillTexture;
    }

EXPORT_C void CHuiCanvasGc::SetPolygonFillTextureOrigin(const TPoint& aOrigin)
    {
    iPolygonFillTextureOrigin = aOrigin;
    }

EXPORT_C TPoint CHuiCanvasGc::PolygonFillTextureOrigin() const
    {
    return iPolygonFillTextureOrigin;
    }

