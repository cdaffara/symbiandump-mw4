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
* Description:   Implements a brush that is able to draw images.
*
*/



#include "uiacceltk/HuiImageBrush.h"  // Class definition
#include "uiacceltk/HuiDrawing.h"
#include "uiacceltk/HuiVisual.h"
#include "uiacceltk/HuiUtil.h"


EXPORT_C CHuiImageBrush*
CHuiImageBrush::NewL(const THuiImage & aImage)
    {
    CHuiImageBrush* self = NewLC(aImage);
    CleanupStack::Pop();
    return self;
    }


EXPORT_C CHuiImageBrush* CHuiImageBrush::NewLC(const THuiImage & aImage)
    {
    CHuiImageBrush* self = new (ELeave) CHuiImageBrush(aImage);
    CleanupStack::PushL(self);
    return self;
    }


CHuiImageBrush::CHuiImageBrush(const THuiImage & aImage)
        : iImage(aImage),
          iScaleMode(CHuiImageVisual::EScaleFit),
          iLeftBorderWidth(0),
          iRightBorderWidth(0),
		  iTopBorderWidth(0),
    	  iBottomBorderWidth(0)
    	  
    {
    SetClipToVisual(EFalse);
    iScale.Set(1.f);
    iImageOffset.iX.Set(0.f);
    iImageOffset.iY.Set(0.f);
    }

EXPORT_C CHuiImageBrush::~CHuiImageBrush()
    {
    }
    
EXPORT_C void CHuiImageBrush::SetScaleMode(
    CHuiImageVisual::TScaleMode aScaleMode)
    {
    iScaleMode = aScaleMode;
    SetChanged();
    }

EXPORT_C void CHuiImageBrush::SetImage(const THuiImage& aImage)
    {
    iImage = aImage;
    SetChanged();
    }

EXPORT_C const THuiImage& CHuiImageBrush::Image() const
    {
    return iImage;    
    }


EXPORT_C void CHuiImageBrush::SetBorders(TInt aLeftBorderPixels,
                         TInt aRightBorderPixels,
                         TInt aTopBorderPixels,
                         TInt aBottomBorderPixels)
	{
    iLeftBorderWidth = aLeftBorderPixels;
    iRightBorderWidth = aRightBorderPixels;
    iTopBorderWidth = aTopBorderPixels;
    iBottomBorderWidth = aBottomBorderPixels;
    SetChanged();
	}                         

EXPORT_C TBool CHuiImageBrush::BorderDrawingEnabled() const
	{
	return ((iLeftBorderWidth!=0) || (iRightBorderWidth!=0)		
		    || (iTopBorderWidth!=0) || (iBottomBorderWidth!=0));
	}

EXPORT_C TBool CHuiImageBrush::CenterDrawingEnabled() const
	{
	if(!iImage.HasTexture()) 
		{
		return EFalse;
		}				
	// determine if there are center pixels available inside borders
	// if not, we can't draw the center..
	TSize imageSize = iImage.Texture().Size();	
	if(((imageSize.iWidth - iLeftBorderWidth) - iRightBorderWidth <= 0)
	   || ((imageSize.iHeight - iTopBorderWidth) - iBottomBorderWidth <= 0))
		{
		return EFalse;
		}	  
		
	return ETrue;
	}

EXPORT_C void CHuiImageBrush::ExpandVisualRect(TRect& aRect) const
    {
    if ( CenterDrawingEnabled() )
        {
        THuiRealRect contentArea(aRect);
         
        // Get the scaled size
        THuiRealSize imageSize( iImage.Texture().Size() );
        if ( iScaleMode == CHuiImageVisual::EScaleFit )
            {
            imageSize = contentArea.Size() * iScale.Now();
            }
        else
            {
            TReal32 scale = HuiUtil::CalculateScaleFactorFromScaleMode(
                contentArea.Size(),
                iImage.Texture().Size(),
                iScaleMode,
                iScale.Now() );
            imageSize = imageSize * scale;
            }

        THuiRealPoint mid = contentArea.Center();
        
        // Move mid point based on the offset
        TReal32 xOffset = 0;
        TReal32 yOffset = 0;

        xOffset = contentArea.Width() * iImageOffset.iX.Now();
        yOffset = contentArea.Height() * iImageOffset.iY.Now();
        
        mid += THuiRealPoint( xOffset, yOffset );
        
        // expand the dirty area rect if needed
        aRect.iTl.iX = Min( aRect.iTl.iX, mid.iX-imageSize.iWidth/2.f );
        aRect.iTl.iY = Min( aRect.iTl.iY, mid.iY-imageSize.iHeight/2.f );
        aRect.iBr.iX = Max( aRect.iBr.iX, mid.iX+imageSize.iWidth/2.f );
        aRect.iBr.iY = Max( aRect.iBr.iY, mid.iY+imageSize.iHeight/2.f );
        }
    
	if(BorderDrawingEnabled())
		{
        // expand the area of the content to include the border:
        aRect.Grow((Max(iLeftBorderWidth, 0) + Max(iRightBorderWidth, 0)) / 2, 
                   (Max(iTopBorderWidth, 0) + Max(iBottomBorderWidth, 0)) / 2);
        // move the area to justify the border width differencies
        aRect.Move((Max(iRightBorderWidth, 0)-Max(iLeftBorderWidth, 0)) / 2,
                   (Max(iTopBorderWidth, 0)-Max(iBottomBorderWidth, 0)) / 2);
		}	
    }
    
TBool CHuiImageBrush::Changed() const
    {
    if ( CHuiBrush::Changed() )
        {
        return ETrue;
        }
    return iImageOffset.Changed() || iScale.Changed() || iImage.Changed();
    }

void CHuiImageBrush::ClearChanged()
    {
    CHuiBrush::ClearChanged();
    iImageOffset.ClearChanged();
    iScale.ClearChanged();
    iImage.ClearChanged();
    }

EXPORT_C void CHuiImageBrush::Draw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const
    {
    
	TBool borderDrawing = BorderDrawingEnabled();
	TBool centerDrawing = CenterDrawingEnabled();
    
    TReal32 opacity = aGuide.BrushOpacity() * iOpacity.Now();    
    aGc.SetPenColor(KRgbWhite);
    aGc.SetPenAlpha(TInt(opacity * 255.f));
    
    if(centerDrawing) 
	    {
        THuiRealRect contentArea = aGuide.BrushRect();
    
        aGc.Enable(CHuiGc::EFeatureBlending);
    
	    if(borderDrawing)
		    {
	        // contract the content area if the borders
	        // have been defined to be inside
		    if(iLeftBorderWidth < 0)
			    {
			    contentArea.iTl.iX -= iLeftBorderWidth;
			    }
		    if(iRightBorderWidth < 0)
			    {
			    contentArea.iBr.iX += iRightBorderWidth;
			    }
		    if(iTopBorderWidth < 0)
			    {
			    contentArea.iTl.iY -= iTopBorderWidth;
			    }
		    if(iBottomBorderWidth < 0)
			    {
			    contentArea.iBr.iY += iBottomBorderWidth;
			    }
		    	
		    // Read in the original texture coordinates (NOTE: not affected
		    // by any renderer-specific texcoord manipulations, like the Gles POT trick).
		    TSize imageSize(iImage.Texture().Size());
		    THuiRealPoint tl = iImage.TopLeft(), br = iImage.BottomRight();
            TReal32 widthU = br.iX - tl.iX;
            TReal32 heightV = br.iY - tl.iY;

			// Remove borders by adjusting texture coordinates.
			// But don't divide by 0
			if ( imageSize.iWidth != 0 && imageSize.iHeight != 0 )
			    {
    			tl.iX += ((TReal32)Abs(iLeftBorderWidth) / (TReal32)imageSize.iWidth) * widthU;
    			br.iX -= ((TReal32)Abs(iRightBorderWidth) / (TReal32)imageSize.iWidth) * widthU;
    			tl.iY += ((TReal32)Abs(iTopBorderWidth) / (TReal32)imageSize.iHeight) * heightV;
    			br.iY -= ((TReal32)Abs(iBottomBorderWidth) / (TReal32)imageSize.iHeight) * heightV;
			    }
								    	    		    	    
			// Create temporary Image with adjusted texcoords
		    THuiImage imageBordersRemoved;
		    imageBordersRemoved.SetTexture(iImage.TextureIf());
		    imageBordersRemoved.SetTexCoords(tl.iX, tl.iY, br.iX, br.iY);
		    
		    // Draw the image without the borders.
	        aGc.DrawStretchImage(CHuiGc::EStretchFull,
	                             imageBordersRemoved,
	                             contentArea);		    			
		    }
	    else
		    {	  

            TReal32 scale = HuiUtil::CalculateScaleFactorFromScaleMode(
                contentArea.Size(),
                iImage.Texture().Size(),
                iScaleMode,
                iScale.Now() );
             
            // Determine offset.
            TReal32 xOffset = 0;
            TReal32 yOffset = 0;

            xOffset = aGuide.BrushRect().Width() * iImageOffset.iX.Now();
            yOffset = aGuide.BrushRect().Height() * iImageOffset.iY.Now();
             
            
            // Do scaling
            TBool doTranslate = !HuiUtil::RealCompare( scale, 1.f ) || !HuiUtil::RealCompare(xOffset,0.f) || !HuiUtil::RealCompare(yOffset,0.f);
            if( doTranslate )
                {
                /** @todo  GC must provide some support for transformations. */
                aGc.Push(EHuiGcMatrixModel);

                // Rotate around the midpoint of the visual.
                THuiRealPoint mid = contentArea.Center();

                aGc.Translate(EHuiGcMatrixModel, mid.iX + xOffset, mid.iY + yOffset, 0.f);
                if( !HuiUtil::RealCompare( scale, 1.f ) )
                    {
                    aGc.Scale(EHuiGcMatrixModel, scale, scale, 1.f);
                    }
                aGc.Translate(EHuiGcMatrixModel, -mid.iX, -mid.iY, 0.f);
                }
            
            // Use the appropriate alignment in the graphics context.
            if(iScaleMode == CHuiImageVisual::EScaleFit)
                {
                aGc.SetAlign(EHuiAlignHLeft, EHuiAlignVTop);
                }
            else
                {
                aGc.SetAlign(EHuiAlignHCenter, EHuiAlignVCenter);
                }   
            
            // draw the image
            if(iScaleMode == CHuiImageVisual::EScaleFit)
                {
                aGc.DrawImage(iImage, contentArea.iTl, contentArea.Size());
                }
            else
                {
                aGc.DrawImage(iImage, contentArea);
                }
                
            // Restore original transformation.
            if( doTranslate )
                {
                aGc.Pop(EHuiGcMatrixModel);
                }
        		
		    } 

        }

	if(borderDrawing)
		{
        THuiRealRect contentArea = aGuide.BrushRect();
		
        // expand the area of the content to include the border:
        contentArea.Grow((Max(iLeftBorderWidth, 0) + Max(iRightBorderWidth, 0)) / 2, 
                   (Max(iTopBorderWidth, 0) + Max(iBottomBorderWidth, 0)) / 2);
        // move the area to justify the border width differencies
        contentArea.Move((Max(iRightBorderWidth, 0)-Max(iLeftBorderWidth, 0)) / 2,
                   (Max(iTopBorderWidth, 0)-Max(iBottomBorderWidth, 0)) / 2);
                                                          
		aGc.DrawBorders(contentArea.Round(),
						Abs(iLeftBorderWidth),
						Abs(iRightBorderWidth),
						Abs(iTopBorderWidth),
						Abs(iBottomBorderWidth),
						CHuiGc::EBorderFixedCorners,
						&iImage);
		}

    }
