/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition of CHuiCanvasAlfPainter.
*
*/




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
#include "huicanvasalfpainter.h"




template <class T> 
void HuiCanavasGcInternalizeL( const TDesC8& aDes, RArray<T>& aArray )
    {
    aArray.Reset();
    if ( !aDes.Length() )
        {
        return;
        }
        
    TInt itemCount = 0;
    memcpy(&itemCount, &aDes[0], sizeof(TInt));

    TPtrC8 buffer( &aDes[4], itemCount*sizeof(T) );
    
    for ( TInt i = 0 ; i < itemCount ;  i++ )
        {
        T* itemPtr = (T*)&buffer[i*sizeof(T)];
        T item = *itemPtr;
        User::LeaveIfError( aArray.Append( item ) );
        }
    }


CHuiCanvasAlfPainter* CHuiCanvasAlfPainter::NewL()
	{
	CHuiCanvasAlfPainter* self    = new ( ELeave ) CHuiCanvasAlfPainter;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


CHuiCanvasAlfPainter::CHuiCanvasAlfPainter()
    {
    }

void CHuiCanvasAlfPainter::ConstructL()
    {
    iPartialCommandBuffer = NULL;
    iCanvasGc = CHuiStatic::Renderer().CreateCanvasGcL();    
    }


CHuiCanvasAlfPainter::~CHuiCanvasAlfPainter()
    {
    iCachedTexts.Close();
    iPointCords.Close();	
    delete iCanvasGc;
    iCanvasGc = NULL;
    }
    
    
void CHuiCanvasAlfPainter::HandleBufferL(TRect& aDisplayRect, TInt aAction, const CHuiCanvasVisual& aUser, CHuiGc* aGc, TPoint/* aPos*/ ) 
    {        
    TInt commandNumber = 0;
    TInt currentText = -1;
    TInt bufferCount = iCommandBuffers.Count();
    iCanvasGc->SetVisual(aUser);
    if ( aGc )
		{
		iCanvasGc->SetGc(*aGc);
		}
	iCanvasGc->SetDefaults();
	
    for (TInt cb = 0; cb < bufferCount; cb++)
        {            
        TPtrC8 ptr = *iCommandBuffers[cb]->iCommands; 
        while ( ptr.Length() )    
            {
            TInt command = 0;
            memcpy(&command, &ptr[0], sizeof(TInt));
            
            ptr.Set( ptr.Right( ptr.Length() - 4 ) );        
            
            TInt bufferLength = 0;
            memcpy(&bufferLength, &ptr[0], sizeof(TInt));

            // Make sure we are aligned by 4 bytes 
            TInt padding = 0;
            if (bufferLength % 4)
                {
                padding = 4 - (bufferLength % 4);    
                }        
            bufferLength += padding;
            
            ptr.Set( ptr.Right( ptr.Length() - 4 ) );
            
            TPtrC8 buffer( ptr.Left( bufferLength ).Ptr(), bufferLength );

            ptr.Set( ptr.Right( ptr.Length() - bufferLength) );        

            commandNumber++;
            switch( command )
                {
                case EHuiCanvasDrawImage:
                    {            
                    const THuiCanvasDrawImageParams* const params = (THuiCanvasDrawImageParams*)buffer.Ptr();
                    
                    CHuiTexture* texture = (CHuiTexture*)(params->iTexture);
                                    
                    THuiRealPoint topLeft = THuiRealPoint(params->iDestinationRectTopLeftX,                
                        params->iDestinationRectTopLeftY);
                        
                    THuiRealPoint bottomRight = THuiRealPoint(params->iDestinationRectBottomRightX,                
                        params->iDestinationRectBottomRightY);
                    
                    // Conversion to pixels (if needed) and to screen coordinates from visual coordinates
                    THuiRealRect destRect = THuiRealRect(aUser.ConvertPoint(topLeft), aUser.ConvertPoint(bottomRight));
                    
                    if (aAction == EDrawBuffer)
                        {
                        iCanvasGc->DrawImage(*texture, destRect);            
                        }
                    else
                        {
                        aUser.ExpandRect(aDisplayRect, destRect);    
                        }    
                    break;
                    }
                case EHuiCanvasPrepareDrawText:
                    {
                    TPtrC text((const TUint16*)buffer.Ptr(), buffer.Length()/2);          
                    
                    // Check if we already have created a rasterized version of text into cache...
                    TInt cachedEntry = -1;
                    for (TInt i=0; i < iCachedTexts.Count(); i++)
                        {
                        if (iCachedTexts[i].iId == commandNumber)
                            {                        
                            cachedEntry = i;
                            }
                        }
                    
                    // ...no, this is first drawing time so we need to rasterize text
                    if (cachedEntry == -1)
                        {
                        // Create new text mesh
                        CHuiTextMesh* mesh = CHuiStatic::Renderer().CreateTextMeshL();
                        CleanupStack::PushL(mesh);
                        mesh->SetTextStyle(iCanvasGc->TextStyle());
                        mesh->SetTextL(text, ETrue);

                        THuiCanvasDrawTextCachedTextEntry newEntry;
                        newEntry.iId = commandNumber;
                        newEntry.iTextMesh = mesh;
                        iCachedTexts.AppendL(newEntry);
                        CleanupStack::Pop(mesh);
                        cachedEntry = iCachedTexts.Count() - 1;                    
                        }
                    
                    currentText = commandNumber;                
                    break;
                    }
                case EHuiCanvasDrawText:
                    {          
                    const THuiCanvasDrawTextParams* const params = (THuiCanvasDrawTextParams*)buffer.Ptr();
                                 
                    THuiRealPoint topLeft = THuiRealPoint(params->iDestinationRectTopLeftX,                
                        params->iDestinationRectTopLeftY);
                        
                    THuiRealPoint bottomRight = THuiRealPoint(params->iDestinationRectBottomRightX,                
                        params->iDestinationRectBottomRightY);
                    
                    // Conversion to pixels (if needed) and to screen coordinates from visual coordinates
                    THuiRealRect destRect = THuiRealRect(aUser.ConvertPoint(topLeft), aUser.ConvertPoint(bottomRight));
                    
                    // Check if we already have created a rasterized version of text into cache...
                    TInt cachedEntry = -1;
                    for (TInt i=0; i < iCachedTexts.Count(); i++)
                        {
                        if (iCachedTexts[i].iId == currentText)
                            {                        
                            cachedEntry = i;
                            }
                        }
                                    
                    if (cachedEntry != -1 && iCachedTexts[cachedEntry].iTextMesh)
                        {
                        if (aAction == EDrawBuffer)
                            {
                            iCanvasGc->DrawText(*iCachedTexts[cachedEntry].iTextMesh, destRect);      
                            }
                        else if (aAction == EScanBuffer)
                            {
                            TSize extents = iCachedTexts[cachedEntry].iTextMesh->Extents();
                            TRect destinationRect = destRect;
                            TPoint tl = destinationRect.Center();
                            switch( iCanvasGc->TextHorizontalAlign() )
                                {
                                case EHuiAlignHLeft:
                                    tl.iX = destinationRect.iTl.iX;
                                    break;
                                case EHuiAlignHCenter:
                                    tl.iX -= extents.iWidth/2;
                                    break;
                                case EHuiAlignHRight:
                                    tl.iX = destinationRect.iBr.iX - extents.iWidth;
                                    break;
                                default:
                                    break;
                                }
                            
                            // check top left Y
                            switch( iCanvasGc->TextVerticalAlign() )
                                {
                                case EHuiAlignVTop:
                                    tl.iY = destinationRect.iTl.iY;
                                    break;
                                case EHuiAlignVCenter:
                                    tl.iY -= extents.iHeight/2;
                                    break;
                                case EHuiAlignVBottom:
                                    tl.iY = destinationRect.iBr.iY - extents.iHeight;
                                    break;
                                default:
                                    break;
                                }
                        
                            TRect textExtendsRect(tl, extents);
                            aUser.ExpandRect(aDisplayRect, textExtendsRect);
                            }    
                        }

                    break;
                    }
                case EHuiCanvasDrawLines:
                	{
                    iPointCords.Reset();
                    TRAP_IGNORE(HuiCanavasGcInternalizeL(buffer, iPointCords))
                    TInt linecount = iPointCords.Count()/4;
            	  	RArray<THuiRealLine> lines;
            	  	for(TInt i=0, j=0; i<linecount; i++)
                	  	{
            	  		THuiRealPoint start(iPointCords[j], iPointCords[j+1]);
            	  		THuiRealPoint end(iPointCords[j+2], iPointCords[j+3]);
            	  		THuiRealLine line(aUser.ConvertPoint(start), aUser.ConvertPoint(end));
            	  		lines.Append(line);
            	  		j += 4;
                	  	}

                    if (aAction == EDrawBuffer)
                        {
                	  	iCanvasGc->DrawLines(lines);
                        }
                    else
                        {
                        for (TInt i=0;i<lines.Count();i++)
                            {
                            TRect pointRect = TRect(lines[i].iStart,lines[i].iStart );
                            pointRect.Grow(iCanvasGc->PenWidth(), iCanvasGc->PenWidth());                                               
                            aUser.ExpandRect(aDisplayRect, pointRect);
                                                    
                            pointRect = TRect(lines[i].iEnd,lines[i].iEnd );
                            pointRect.Grow(iCanvasGc->PenWidth(), iCanvasGc->PenWidth());                       
                            aUser.ExpandRect(aDisplayRect, pointRect);
                            }                        
                        }    
            	  	lines.Close();
                	break;	
                	}
    			
               	case EHuiCanvasDrawEllipse:
               		{
           			const THuiCanvasDrawEllipseParams* const params = (THuiCanvasDrawEllipseParams*)buffer.Ptr();
            	  	
            	  	THuiRealPoint topLeft = THuiRealPoint(params->iDestinationRectTopLeftX, params->iDestinationRectTopLeftY);
                    
                	THuiRealPoint bottomRight = THuiRealPoint(params->iDestinationRectBottomRightX, params->iDestinationRectBottomRightY);
                
                	// Conversion to pixels (if needed) and to screen coordinates from visual coordinates
                	THuiRealRect destRect = THuiRealRect(aUser.ConvertPoint(topLeft), aUser.ConvertPoint(bottomRight));
            	  	
                    if (aAction == EDrawBuffer)
                        {
                	  	iCanvasGc->DrawEllipse(destRect);
                        }
                    else
                        {
                        aUser.ExpandRect(aDisplayRect, destRect);    
                        }    
            		break;	
                	}
    			case EHuiCanvasDrawPoints:
    				{
                    iPointCords.Reset();
                    TRAP_IGNORE(HuiCanavasGcInternalizeL(buffer, iPointCords))
                    TInt pointcount = iPointCords.Count()/2;
              	  	RArray<THuiRealPoint> points;
            	  	for(TInt i=0, j=0; i<pointcount; i++)
                	  	{
              	  		THuiRealPoint point(iPointCords[j], iPointCords[j+1]);
            	  		points.Append(aUser.ConvertPoint(point));
            	  		j += 2;
                	  	}
                	  	
                    if (aAction == EDrawBuffer)
                        {
                   	  	iCanvasGc->DrawPoints(points);
                        }
                    else
                        {
                        for (TInt i=0;i<points.Count();i++)
                            {
                            TRect pointRect = TRect(points[i],points[i]);
                            pointRect.Grow(iCanvasGc->PenWidth(), iCanvasGc->PenWidth());                       
                            aUser.ExpandRect(aDisplayRect, pointRect);
                            }                        
                        }    
               	  	points.Close();
               		break;	
                	}
    			case EHuiCanvasDrawPolygon:
    				{
                    iPointCords.Reset();
                    TRAP_IGNORE(HuiCanavasGcInternalizeL(buffer, iPointCords))
                    TInt pointcount = iPointCords.Count()/2;
              	  	RArray<THuiRealPoint> points;
            	  	for(TInt i=0, j=0; i<pointcount; i++)
                	  	{
              	  		THuiRealPoint point(iPointCords[j], iPointCords[j+1]);
            	  		points.Append(aUser.ConvertPoint(point));
            	  		j += 2;
                	  	}

                    if (aAction == EDrawBuffer)
                        {
                   	  	iCanvasGc->DrawPolygon(points);
                        }
                    else
                        {
                        for (TInt i=0;i<points.Count();i++)
                            {
                            TRect pointRect = TRect(points[i],points[i]);
                            pointRect.Grow(iCanvasGc->PenWidth(), iCanvasGc->PenWidth());                       
                            aUser.ExpandRect(aDisplayRect, pointRect);
                            }                        
                        }    

               	  	points.Close();
                	break;	
                	}
    			case EHuiCanvasDrawRects:
    				{
                    iPointCords.Reset();
                    TRAP_IGNORE(HuiCanavasGcInternalizeL(buffer, iPointCords))
                    TInt rectcount = iPointCords.Count()/4;
            	  	RArray<THuiRealRect> rects;
            	  	for(TInt i=0, j=0; i<rectcount; i++)
                	  	{
            	  		THuiRealPoint topleft(iPointCords[j], iPointCords[j+1]);
            	  		THuiRealPoint bottomright(iPointCords[j+2], iPointCords[j+3]);
            	  		THuiRealRect rect(aUser.ConvertPoint(topleft), aUser.ConvertPoint(bottomright));
            	  		rects.Append(rect);
            	  		j += 4;
                	  	}

           	  	     if (aAction == EDrawBuffer)
           	  	        {
                        iCanvasGc->DrawRects(rects);
           	  	        }
                     else
                        {
                        for (TInt i=0;i<rects.Count();i++)
                            {
                            TRect pointRect = rects[i];
                            pointRect.Grow(iCanvasGc->PenWidth(), iCanvasGc->PenWidth());                       
                            aUser.ExpandRect(aDisplayRect, pointRect);
                            }                        
                        }    
           	  	        
            	  	rects.Close();
               		break;	
                	}

               	case EHuiCanvasSetTextStyle:
                    {          
                    const THuiCanvasSetTextStyleParams* const params = (THuiCanvasSetTextStyleParams*)buffer.Ptr();
                    iCanvasGc->SetTextStyle(params->iTextStyleId);
                    break;    
                    }

    			case EHuiCanvasSetPenColor:
    				{
                    const THuiCanvasSetPenColorParams* const params = (THuiCanvasSetPenColorParams*)buffer.Ptr();
                    iCanvasGc->SetPenColor(params->iPenColor);
                    break;    
                	}
    			case EHuiCanvasSetPenWidth:
    				{
                    const THuiCanvasSetPenWidthParams* const params = (THuiCanvasSetPenWidthParams*)buffer.Ptr();
                    iCanvasGc->SetPenWidth(params->iPenWidth);
                    break;    
                	}
    			case EHuiCanvasSetOpacity:
    				{
                    const THuiCanvasSetOpacityParams* const params = (THuiCanvasSetOpacityParams*)buffer.Ptr();
                    iCanvasGc->SetOpacity(params->iOpacity);
                    break;    
                	}
    			case EHuiCanvasSetPolygonDrawMode:
    				{
    				const THuiCanvasSetPolygonDrawModeParams* const params = (THuiCanvasSetPolygonDrawModeParams*)buffer.Ptr();
                    iCanvasGc->SetPolygonDrawMode(params->iPolygonDrawMode);
                    break;	
                	}
    			case EHuiCanvasSetTextAlign:
    				{
    				const THuiCanvasSetTextAlignParams* const params = (THuiCanvasSetTextAlignParams*)buffer.Ptr();
                    iCanvasGc->SetTextAlign(THuiAlignHorizontal(params->iTextAlignHorizontal), THuiAlignVertical(params->iTextAlignVertical));
                	break;	
                	}

        		case EHuiCanvasLoadIdentity:
    				{
                    if (aAction == EDrawBuffer)
                        {                
        				iCanvasGc->LoadIdentity();
                        }
                	break;	
                	}
    			case EHuiCanvasTranslate:
    				{
    				const THuiCanvasTranslateParams* const params = (THuiCanvasTranslateParams*)buffer.Ptr();
                    if (aAction == EDrawBuffer)
                        {                
                        iCanvasGc->Translate(aUser.ConvertPoint(THuiRealPoint(params->iX,0)).iX,
                             aUser.ConvertPoint(THuiRealPoint(params->iY,0)).iX, 
                             aUser.ConvertPoint(THuiRealPoint(params->iZ,0)).iX);
                        }
                    else
                        {
                        // Calculating transformations may be too difficult, so just set largest possible rect
                        if (aUser.Clipping() || !aUser.Display())
                            {
                            aUser.ExpandRect(aDisplayRect, aUser.DisplayRect());                                        
                            }
                        else
                            {
                            aUser.ExpandRect(aDisplayRect, aUser.Display()->VisibleArea());            
                            }    
                        }    
                    break;	
                	}
    			case EHuiCanvasScale:
    				{
    				const THuiCanvasScaleParams* const params = (THuiCanvasScaleParams*)buffer.Ptr();
                    if (aAction == EDrawBuffer)
                        {                
                        iCanvasGc->Scale(params->iX, params->iY, params->iZ);
                        }
                    else
                        {
                        // Calculating transformations may be too difficult, so just set largest possible rect
                        if (aUser.Clipping() || !aUser.Display())
                            {
                            aUser.ExpandRect(aDisplayRect, aUser.DisplayRect());                                        
                            }
                        else
                            {
                            aUser.ExpandRect(aDisplayRect, aUser.Display()->VisibleArea());            
                            }    
                        }      
                    break;	
                	}
    			case EHuiCanvasRotate:
    				{
    				const THuiCanvasRotateParams* const params = (THuiCanvasRotateParams*)buffer.Ptr();
                    if (aAction == EDrawBuffer)
                        {                
        				iCanvasGc->Rotate(params->iAngle, params->iX, params->iY, params->iZ);
                        }
                    else
                        {
                        // Calculating transformations may be too difficult, so just set largest possible rect
                        if (aUser.Clipping() || !aUser.Display())
                            {
                            aUser.ExpandRect(aDisplayRect, aUser.DisplayRect());                                        
                            }
                        else
                            {
                            aUser.ExpandRect(aDisplayRect, aUser.Display()->VisibleArea());            
                            }    
                        }        
                    break;	
                	}
               default:
                    {
                    __ASSERT_DEBUG(EFalse, THuiPanic::Panic(THuiPanic::EUnknown));
                    break;
                    } 	            	
                }                
            }
        }     
    iPaintedRect = aUser.DisplayRect();    
    }


    
void CHuiCanvasAlfPainter::SetCommandSetL( const TDesC8& aCommands )
    {
	CHuiCanvasPainter::SetCommandSetL(aCommands);    
    }
    
void CHuiCanvasAlfPainter::ClearCommandSet()
    {
   	CHuiCanvasPainter::ClearCommandSet();
    }

void CHuiCanvasAlfPainter::AddCommandSetL( const TDesC8& aMoreCommands )
    {
    CHuiCanvasPainter::AddCommandSetL(aMoreCommands); 
    }

void CHuiCanvasAlfPainter::AddPartialCommandSetL( const TDesC8& aMoreCommands, TBool aLastPart )
    {
    CHuiCanvasPainter::AddPartialCommandSetL(aMoreCommands,aLastPart); 
    }

#ifdef HUI_DEBUG_TRACK_DRAWING
void CHuiCanvasAlfPainter::SetTrackCommandSet( TFileName& aFileName, TBool aTrack )
    {
    CHuiCanvasPainter::SetTrackCommandSet( aFileName, aTrack ); 
    }
#endif

void CHuiCanvasAlfPainter::ClearCache()
    {
    ClearHuiTextCache();	
    }

void CHuiCanvasAlfPainter::ClearHuiTextCache()
    {
    // Hui text meshes
    for (TInt i=0; i < iCachedTexts.Count(); i++)
        {
        delete iCachedTexts[i].iTextMesh;
        iCachedTexts[i].iTextMesh = NULL;    
        }                 
    iCachedTexts.Reset();           	
    }       
 
CHuiCanvasGc& CHuiCanvasAlfPainter::CanvasGc() const
	{
		return *iCanvasGc;
	}

TInt CHuiCanvasAlfPainter::PaintedAreaCount() const
    {
    return 1;    
    }
    
THuiCanvasPaintedArea CHuiCanvasAlfPainter::PaintedArea(TInt /*aIndex*/)
    {
    THuiCanvasPaintedArea area;
    area.iPaintedRect = iPaintedRect;
    area.iPaintType = EHuiCanvasPaintTypeTransparent;
    area.iFlags = 0;    
    return area;
    }

TInt CHuiCanvasAlfPainter::SetCapturingBufferL(CFbsBitmap* /*aTarget*/)
    {
    return KErrNotSupported;
    }

TInt CHuiCanvasAlfPainter::EnableRenderBuffer(TBool /*aEnable*/)
    {
    return KErrNotSupported;
    }
