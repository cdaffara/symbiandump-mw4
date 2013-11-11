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
* Description:   Definition of CHuiCanvasWsSwGc.
*
*/



#include "huicanvaswsswgc.h"
#include "uiacceltk/HuiCanvasVisual.h"
#include <graphics/wsgraphicscontext.h>
#include <e32cmn.h>
#include <AknLayoutFont.h>
#include <gdi.h>



CHuiCanvasWsSwGc* CHuiCanvasWsSwGc::NewL()
	{
	CHuiCanvasWsSwGc* self    = new ( ELeave ) CHuiCanvasWsSwGc;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


CHuiCanvasWsSwGc::CHuiCanvasWsSwGc()
    {
    
    }

void CHuiCanvasWsSwGc::ConstructL()
	{
    iCanvasGc = CHuiStatic::Renderer().CreateCanvasGcL();

	// There is no need to do clipping at canvas gc so lets disable it
	iCanvasGc->SetClippingMode(EHuiCanvasClipModeNone);
	}

CHuiCanvasWsSwGc::~CHuiCanvasWsSwGc()
    {
    iTempRegion.Close();
    iWsClipRegion.Close();
    iDefaultClipRegion.Close();
    
    delete iCanvasGc;
    iCanvasGc = NULL;
    
    delete iBitBitmap;
    iBitBitmap = NULL;
    
	delete iBitDevice;
	iBitDevice = NULL;
	
	delete iBitGc;
	iBitGc = NULL;
	
    delete iCapturingBitGc;
    iCapturingBitGc = NULL;
    
    delete iCapturingBitBitmap;
    iCapturingBitBitmap = NULL;
    
    delete iCapturingBitDevice;
    iCapturingBitDevice = NULL;
    }
    
void CHuiCanvasWsSwGc::WsBitBltL(TInt aBitmapHandle, TPoint aPoint)
	{
	iIsReallyDirty = ETrue;
			
	if (!IsRenderingEnabled())
	    {
	    return;
	    }
	
    TPoint point = AdjustCoordinates(aPoint);	
    // Draw            
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    
	TInt error = bitmap->Duplicate(aBitmapHandle);
	if ( error == KErrNone  )
	    {
	    iBitGc->BitBlt(point,bitmap);
	    }
	    CleanupStack::PopAndDestroy();
	}




void CHuiCanvasWsSwGc::WsBitBltRectL(TInt aBitmapHandle, TPoint aPoint, TRect aRect)
	{
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
	TPoint point = AdjustCoordinates(aPoint);	
    // Draw            
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    
	TInt error = bitmap->Duplicate(aBitmapHandle);
	
	if ( error == KErrNone  )
        {
        iBitGc->BitBlt(point,bitmap,aRect);
        }
        CleanupStack::PopAndDestroy();

	}



void CHuiCanvasWsSwGc::WsBitBltMaskedL(TInt aBitmapHandle, TInt aMaskHandle, TInt aInvertMask, TPoint aPoint, TRect aRect)
	{
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
	TPoint point = AdjustCoordinates(aPoint);	
    // Draw            
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    TInt error = bitmap->Duplicate(aBitmapHandle);
	if (error == KErrNone)
	    {
	    CFbsBitmap* bitmapMask = new (ELeave) CFbsBitmap;
	    CleanupStack::PushL(bitmapMask);
	    error = bitmapMask->Duplicate(aMaskHandle);
	    if (error == KErrNone)
	        {
	        iBitGc->BitBltMasked(point,bitmap,aRect,bitmapMask,aInvertMask);
	        }
	    CleanupStack::PopAndDestroy(2);
	    }
	else
        {
        CleanupStack::PopAndDestroy();
        }
	
	}

void CHuiCanvasWsSwGc::WsBitBltMaskedPointL(TInt aBitmapHandle, TInt aMaskHandle, TPoint aPoint1, TPoint aPoint2, TRect aRect)
	{
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
	TPoint point1 = AdjustCoordinates(aPoint1);	

     // Draw            
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    TInt error = bitmap->Duplicate(aBitmapHandle);
    if (error == KErrNone)
        {
        CFbsBitmap* bitmapMask = new (ELeave) CFbsBitmap;
        CleanupStack::PushL(bitmapMask);
        error = bitmapMask->Duplicate(aMaskHandle);
        if (error == KErrNone)
            {
            if (aPoint2 != TPoint(0,0))
                {
                CFbsBitmap* movedMask = CreateMovedMaskL(*bitmapMask, aPoint2);
                iBitGc->BitBltMasked(point1,bitmap,aRect, movedMask ,ETrue);    
                delete movedMask;    
                }
            else
                {
                iBitGc->BitBltMasked(point1,bitmap,aRect, bitmapMask ,ETrue);            
                }        
            }
        CleanupStack::PopAndDestroy(2);
        }
    else
        {
        CleanupStack::PopAndDestroy();
        }    
	}

void CHuiCanvasWsSwGc::WsCombinedBitBltMaskedL(TRect aDestinationRect, const RArray<THuiCachedCombinedImageParams>& aBlits)
    {
    if (!IsRenderingEnabled())
        {
        return;
        }
    
    TRect adjustedDestinationRect(aDestinationRect);
           
    for (TInt i=0; i<aBlits.Count(); i++)
        {
      	CFbsBitmap* bitmap = NULL;
    	CFbsBitmap* bitmapMask = NULL;
      	TPoint point = aBlits[i].iCombinedBitmapPoint;
      	TPoint adjustedPoint =AdjustCoordinates(point);
        
        if (aBlits[i].iBitmapHandle)
            {
        	bitmap = new (ELeave) CFbsBitmap;
            CleanupStack::PushL(bitmap);
            bitmap->Duplicate(aBlits[i].iBitmapHandle);                                        
            }
        
        if (aBlits[i].iMaskHandle)
            {
    	    bitmapMask = new (ELeave) CFbsBitmap;
            CleanupStack::PushL(bitmapMask);
            bitmapMask->Duplicate(aBlits[i].iMaskHandle);                
            }        
        
        TRect sourceRect = TRect(TPoint(0,0), bitmap->SizeInPixels());
        if (bitmapMask)
            {
            TBool invertMask = (bitmapMask->DisplayMode() == EGray256);                       
            iBitGc->BitBltMasked(adjustedPoint, bitmap, sourceRect, bitmapMask, invertMask);                       
            }
        else
            {
            iBitGc->BitBlt(adjustedPoint, bitmap);                                           
            }                                                    
        
        if (bitmapMask)
            {
            CleanupStack::PopAndDestroy(bitmapMask);    
            bitmapMask = NULL;
            }
        
        if (bitmap)
            {
            CleanupStack::PopAndDestroy(bitmap);                    
            bitmap = NULL;
            }        
        }
    }

void CHuiCanvasWsSwGc::WsResetClippingRegionL()
	{
    if (!IsRenderingEnabled())
        {
        return;
        }

    // Reset clip region in our own region
    iWsClipRegion.Clear();

	// Reset clip region in iBitGc to default one
    if (iDefaultClipRegion.Count())
        {
        iBitGc->SetClippingRegion(iDefaultClipRegion);
        }
    else
        {
        iBitGc->CancelClippingRegion();
        }
	}

void CHuiCanvasWsSwGc::WsClearL()
	{		
	iIsReallyDirty = ETrue;
	
	if (!IsRenderingEnabled())
        {
        return;
        }

   	iBitGc->Clear();
	}

void CHuiCanvasWsSwGc::WsClearRectL(TRect& aRect)
	{		
	iIsReallyDirty = ETrue;
	
	if (!IsRenderingEnabled())
        {
        return;
        }

    iBitGc->Clear(AdjustCoordinates(aRect));
	}

void CHuiCanvasWsSwGc::WsResetBrushPatternL()
	{
    if (!IsRenderingEnabled())
        {
        return;
        }

    iBitGc->DiscardBrushPattern();
	}

void CHuiCanvasWsSwGc::WsResetFontL()
	{
    if (!IsRenderingEnabled())
        {
        return;
        }
    
	iBitGc->DiscardFont();
	}

void CHuiCanvasWsSwGc::WsDrawArcL(TPoint aPoint1, TPoint aPoint2, TRect aRect)
	{
	iIsReallyDirty = ETrue;

	if (!IsRenderingEnabled())
        {
        return;
        }

	TPoint point1 = AdjustCoordinates(aPoint1);
	TPoint point2 = AdjustCoordinates(aPoint2);
	TRect rect = AdjustCoordinates(aRect);
	iBitGc->DrawArc(rect,point1,point2);
	}

void CHuiCanvasWsSwGc::WsDrawPieL(TPoint aPoint1, TPoint aPoint2, TRect aRect)
	{
	iIsReallyDirty = ETrue;

	if (!IsRenderingEnabled())
        {
        return;
        }

	TPoint point1 = AdjustCoordinates(aPoint1);
	TPoint point2 = AdjustCoordinates(aPoint2);
	TRect rect = AdjustCoordinates( aRect );
	iBitGc->DrawPie(rect,point1,point2);
	}

void CHuiCanvasWsSwGc::WsDrawBitmap1L(TInt aBitmapHandle, TRect aRect)
	{
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
    // Draw            
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    
	TInt error = bitmap->Duplicate(aBitmapHandle);
	if ( error == KErrNone )
	    {
	    iBitGc->DrawBitmap(AdjustCoordinates(aRect),bitmap);
	    }
	CleanupStack::PopAndDestroy();
	}

void CHuiCanvasWsSwGc::WsDrawBitmap2L(TInt aBitmapHandle, TRect aRect1, TRect aRect2)
	{
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
     // Draw            
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    
	TInt error = bitmap->Duplicate(aBitmapHandle);
	if ( error == KErrNone )
	    {
	    iBitGc->DrawBitmap(AdjustCoordinates(aRect1),bitmap,aRect2);
	    }
	CleanupStack::PopAndDestroy();   
	}

void CHuiCanvasWsSwGc::WsDrawBitmap3L(TInt aBitmapHandle, TPoint aPoint)
	{
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
	TPoint point = AdjustCoordinates(aPoint);
    // Draw            
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    
	TInt error = bitmap->Duplicate(aBitmapHandle);
	if ( error == KErrNone )
	    {
	    iBitGc->DrawBitmap(point,bitmap);
	    }
	CleanupStack::PopAndDestroy();
	}

void CHuiCanvasWsSwGc::WsDrawBitmapMaskedL(TInt aBitmapHandle, TInt aMaskHandle, TInt aInvertMask, TRect& aRect1, TRect& aRect2)
	{
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
    // Draw            
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    TInt error = bitmap->Duplicate(aBitmapHandle);
    if ( error == KErrNone )
        {
        // Draw            
        CFbsBitmap* bitmapMask = new (ELeave) CFbsBitmap;
        CleanupStack::PushL(bitmapMask);
        error = bitmapMask->Duplicate(aMaskHandle);
        if ( error == KErrNone )
            {
            iBitGc->DrawBitmapMasked(AdjustCoordinates(aRect1),bitmap,aRect2,bitmapMask,aInvertMask);    
            }
        CleanupStack::PopAndDestroy(2);
        }
    else
        {
        CleanupStack::PopAndDestroy();
        }
          
	}

void CHuiCanvasWsSwGc::WsDrawRoundRectL(TPoint aPoint, TRect& aRect)
	{
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }

    iBitGc->DrawRoundRect(AdjustCoordinates(aRect),TSize(aPoint.iX,aPoint.iY));
	}

void CHuiCanvasWsSwGc::WsDrawPolyLineL(CArrayFix<TPoint>* aPointerArray)
	{
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
	if(aPointerArray)
		{
		for(int i=0; i<aPointerArray->Count();i++)
			{
			aPointerArray->At(i) = AdjustCoordinates(aPointerArray->At(i));
			}
		iBitGc->DrawPolyLine(aPointerArray);
		}
	}

void CHuiCanvasWsSwGc::WsDrawPolyLineNoEndPointL(TPoint* aPointerArray, TInt aCount)
	{
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
	if ( aPointerArray )
        {
        for(int i=0; i<aCount;i++)
			{
			aPointerArray[i] = AdjustCoordinates(aPointerArray[i]);
			}
		iBitGc->DrawPolyLineNoEndPoint(aPointerArray,aCount);
        } 
    }

void CHuiCanvasWsSwGc::WsDrawPolygonL(CArrayFix<TPoint>* aPointerArray, TInt aFillRule)
	{
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
	if ( aPointerArray )
        {
        for(int i=0; i<aPointerArray->Count();i++)
        	{
        	aPointerArray->At(i) = AdjustCoordinates(aPointerArray->At(i));	
        	}
        iBitGc->DrawPolygon( aPointerArray,(CFbsBitGc ::TFillRule)aFillRule );	
        }
	}	

void CHuiCanvasWsSwGc::WsDrawEllipseL(TRect& aRect)
	{
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
    iBitGc->DrawEllipse(AdjustCoordinates(aRect));
	}

void CHuiCanvasWsSwGc::WsDrawLineL(TPoint& aStart, TPoint& aEnd)
	{
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
	TPoint startPoint = AdjustCoordinates(aStart);
	TPoint endPoint = AdjustCoordinates(aEnd);
    iBitGc->DrawLine( startPoint,endPoint );
	}

void CHuiCanvasWsSwGc::WsDrawLineToL(TPoint& aPoint)
	{
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
	TPoint point = AdjustCoordinates(aPoint);
    iBitGc->DrawLineTo(point);
	}

void CHuiCanvasWsSwGc::WsDrawLineByL(TPoint& aPoint)
	{
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
	TPoint point = AdjustCoordinates(aPoint);
    iBitGc->DrawLineBy(point);
	}

void CHuiCanvasWsSwGc::WsDrawRectL(TRect& aRect)
	{
	iIsReallyDirty = ETrue;

	if (!IsRenderingEnabled())
        {
        return;
        }

    iBitGc->DrawRect(AdjustCoordinates(aRect));
	}

void CHuiCanvasWsSwGc::WsDrawText1L(TPtr& aTextValue, THuiCanvasTextParameters& aTextParameters)
	{
	if (iBitGc && !iBitGc->IsFontUsed())
	    {
	    return;    
	    }

	iIsReallyDirty = ETrue;
	
    if (!IsRenderingEnabled())
        {
        return;
        }
	
#ifdef SYMBIAN_BUILD_GCE            
    CGraphicsContext::TTextParameters gcTextParams = *(CGraphicsContext::TTextParameters*)&aTextParameters;
	iBitGc->DrawText(aTextValue, &gcTextParams);
#else			
	iBitGc->DrawText(aTextValue);
#endif            
    }

void CHuiCanvasWsSwGc::WsDrawText2L(TPtr& aTextValue, TPoint& aPoint,THuiCanvasTextParameters& aTextParameters)
	{
	if (iBitGc && !iBitGc->IsFontUsed())
	    {
	    return;    
	    }

	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
	TPoint point = AdjustCoordinates(aPoint);

#ifdef SYMBIAN_BUILD_GCE            
    CGraphicsContext::TTextParameters gcTextParams = *(CGraphicsContext::TTextParameters*)&aTextParameters;
	iBitGc->DrawText(aTextValue, &gcTextParams, point);
#else			
    iBitGc->DrawText(aTextValue, point);
#endif
    }

void CHuiCanvasWsSwGc::WsDrawText3L(TPtr& aTextValue, TRect& aRect, THuiCanvasTextParameters& aTextParameters)
	{
	if (iBitGc && !iBitGc->IsFontUsed())
	    {
	    return;    
	    }
	
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
#ifdef SYMBIAN_BUILD_GCE            
    CGraphicsContext::TTextParameters gcTextParams = *(CGraphicsContext::TTextParameters*)&aTextParameters;
	iBitGc->DrawText(aTextValue, &gcTextParams, AdjustCoordinates(aRect));
#else			
	iBitGc->DrawText(aTextValue, AdjustCoordinates(aRect));
#endif
    }


void CHuiCanvasWsSwGc::WsDrawText4L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextAlign, TInt aTextMargin,THuiCanvasTextParameters& aTextParameters)
	{
	if (iBitGc && !iBitGc->IsFontUsed())
	    {
	    return;    
	    }
	
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
		
#ifdef SYMBIAN_BUILD_GCE            
    CGraphicsContext::TTextParameters gcTextParams = *(CGraphicsContext::TTextParameters*)&aTextParameters;
	iBitGc->DrawText(aTextValue, &gcTextParams, AdjustCoordinates(aRect), aBaselineOffset,(CFbsBitGc::TTextAlign )aTextAlign,aTextMargin);
#else			
    iBitGc->DrawText(aTextValue, AdjustCoordinates(aRect), aBaselineOffset,(CFbsBitGc::TTextAlign )aTextAlign,aTextMargin);
#endif
    }

void CHuiCanvasWsSwGc::WsDrawText5L( TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextAlign, TInt aTextMargin,TInt aTextWidth, THuiCanvasTextParameters& aTextParameters)
	{
	if (iBitGc && !iBitGc->IsFontUsed())
	    {
	    return;    
	    }
	
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
#ifdef SYMBIAN_BUILD_GCE            
    CGraphicsContext::TTextParameters gcTextParams = *(CGraphicsContext::TTextParameters*)&aTextParameters;
    iBitGc->DrawText(aTextValue, &gcTextParams, AdjustCoordinates(aRect), aBaselineOffset,aTextWidth,(CFbsBitGc::TTextAlign )aTextAlign,aTextMargin);
#else
    iBitGc->DrawText(aTextValue, AdjustCoordinates(aRect), aBaselineOffset,aTextWidth,(CFbsBitGc::TTextAlign )aTextAlign,aTextMargin);
#endif
    }
	
void CHuiCanvasWsSwGc::WsDrawTextVertical1L(TPtr& aTextValue, TInt aTextUp, THuiCanvasTextParameters& aTextParameters)	
    {
	if (iBitGc && !iBitGc->IsFontUsed())
	    {
	    return;    
	    }
    
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
#ifdef SYMBIAN_BUILD_GCE            
    CGraphicsContext::TTextParameters gcTextParams = *(CGraphicsContext::TTextParameters*)&aTextParameters;
    iBitGc->DrawTextVertical(aTextValue, &gcTextParams, aTextUp);
#else
    iBitGc->DrawTextVertical(aTextValue, aTextUp);
#endif
    }

void CHuiCanvasWsSwGc::WsDrawTextVertical2L(TPtr& aTextValue, TInt aTextUp,TPoint& aPoint, THuiCanvasTextParameters& aTextParameters)	
    {
	if (iBitGc && !iBitGc->IsFontUsed())
	    {
	    return;    
	    }
    
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
    TPoint point = AdjustCoordinates(aPoint);	 	                                       	
#ifdef SYMBIAN_BUILD_GCE            
    CGraphicsContext::TTextParameters gcTextParams = *(CGraphicsContext::TTextParameters*)&aTextParameters;
    iBitGc->DrawTextVertical(aTextValue, &gcTextParams, point,aTextUp);
#else
    iBitGc->DrawTextVertical(aTextValue,point,aTextUp);
#endif
    }

void CHuiCanvasWsSwGc::WsDrawTextVertical3L(TPtr& aTextValue, TInt aTextUp, TRect& aRect, THuiCanvasTextParameters& aTextParameters)	
    {
	if (iBitGc && !iBitGc->IsFontUsed())
	    {
	    return;    
	    }
    
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
#ifdef SYMBIAN_BUILD_GCE            
    CGraphicsContext::TTextParameters gcTextParams = *(CGraphicsContext::TTextParameters*)&aTextParameters;
    iBitGc->DrawTextVertical(aTextValue, &gcTextParams, AdjustCoordinates(aRect),aTextUp);
#else
    iBitGc->DrawTextVertical(aTextValue,AdjustCoordinates(aRect),aTextUp);
#endif
    }

void CHuiCanvasWsSwGc::WsDrawTextVertical4L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextUp, TInt aTextAlign, TInt aTextMargin,THuiCanvasTextParameters& aTextParameters)
    {
	if (iBitGc && !iBitGc->IsFontUsed())
	    {
	    return;    
	    }
    
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
#ifdef SYMBIAN_BUILD_GCE            
    CGraphicsContext::TTextParameters gcTextParams = *(CGraphicsContext::TTextParameters*)&aTextParameters;
    iBitGc->DrawTextVertical(aTextValue, &gcTextParams, AdjustCoordinates(aRect),aBaselineOffset,aTextUp,(CFbsBitGc::TTextAlign )aTextAlign,aTextMargin);
#else
    iBitGc->DrawTextVertical(aTextValue,AdjustCoordinates(aRect),aBaselineOffset,aTextUp,(CFbsBitGc::TTextAlign )aTextAlign,aTextMargin);
#endif
    }

void CHuiCanvasWsSwGc::WsDrawTextVertical5L(TPtr& aTextValue, TRect& aRect, TInt aBaselineOffset, TInt aTextUp, TInt aTextAlign, TInt aTextMargin, TInt aTextWidth, THuiCanvasTextParameters& aTextParameters)
    {
	if (iBitGc && !iBitGc->IsFontUsed())
	    {
	    return;    
	    }
    
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
#ifdef SYMBIAN_BUILD_GCE            
    CGraphicsContext::TTextParameters gcTextParams = *(CGraphicsContext::TTextParameters*)&aTextParameters;
    iBitGc->DrawTextVertical(aTextValue, &gcTextParams, AdjustCoordinates(aRect),aBaselineOffset,aTextWidth,aTextUp,(CFbsBitGc::TTextAlign )aTextAlign,aTextMargin);
#else
    iBitGc->DrawTextVertical(aTextValue,AdjustCoordinates(aRect),aBaselineOffset,aTextWidth,aTextUp,(CFbsBitGc::TTextAlign )aTextAlign,aTextMargin);
#endif
    }

void CHuiCanvasWsSwGc::WsMoveToL( TPoint& aPoint)
    {
    if (!IsRenderingEnabled())
        {
        return;
        }

    TPoint point = AdjustCoordinates(aPoint);
    iBitGc->MoveTo(point);
    }

void CHuiCanvasWsSwGc::WsMoveByL( TPoint& aPoint)
    {
    if (!IsRenderingEnabled())
        {
        return;
        }

    TPoint point = AdjustCoordinates(aPoint);
    iBitGc->MoveBy(point);
    }
    
void CHuiCanvasWsSwGc::WsPlotL(TPoint& aPoint)
	{
	iIsReallyDirty = ETrue;

    if (!IsRenderingEnabled())
        {
        return;
        }
	
	TPoint point = AdjustCoordinates(aPoint);
    // Draw  
    iBitGc->Plot(point);
	}

void CHuiCanvasWsSwGc::WsResetL()
	{
    if (!IsRenderingEnabled())
        {
        return;
        }

    iWsClipRect = RoundedDisplayRect();            
	iWsOrigin = TPoint(0,0);	
	iBitGc->Reset();
    WsResetClippingRegionL();   
	}

void CHuiCanvasWsSwGc::WsSetBrushColorL( TRgb aColor)
    {
    if (!IsRenderingEnabled())
        {
        return;
        }

    iBitGc->SetBrushColor(aColor);
    }

void CHuiCanvasWsSwGc::WsSetBrushOriginL( TPoint& aPoint)
    {
    if (!IsRenderingEnabled())
        {
        return;
        }

    TPoint point = AdjustCoordinates(aPoint);
    iBitGc->SetBrushOrigin(point);
    }

void CHuiCanvasWsSwGc::WsSetBrushStyleL( TInt aStyle)
    {
    if (!IsRenderingEnabled())
        {
        return;
        }

    iBitGc->SetBrushStyle((CFbsBitGc::TBrushStyle)aStyle);
    }

void CHuiCanvasWsSwGc::WsSetClippingRegionL( RRegion& aRegion)
    {        
    if (!IsRenderingEnabled())
        {
        return;
        }

    // Store first
    iWsClipRegion.Clear();    
    for (TInt i=0;i<aRegion.Count();i++)
        {
        iWsClipRegion.AddRect(aRegion[i]);
        }    

    // Area is in screen coordinates so we must convert it 
    iTempRegion.Clear();
    for (TInt i=0; i < aRegion.Count(); i++)
        {
        iTempRegion.AddRect(AdjustCoordinates(aRegion[i]));   
        }
    
    iTempRegion.Intersect(iDefaultClipRegion); // MNA: Koe
    
    iBitGc->SetClippingRegion(iTempRegion);
    }
    
void CHuiCanvasWsSwGc::WsSetDrawModeL( TInt aDrawMode)
	{
    if (!IsRenderingEnabled())
        {
        return;
        }

    iBitGc->SetDrawMode((CFbsBitGc::TDrawMode)aDrawMode);
	}

void CHuiCanvasWsSwGc::WsSetOriginL( TPoint& aOrigin)
	{
    if (!IsRenderingEnabled())
        {
        return;
        }
	
	iWsOrigin = aOrigin;
 	iBitGc->SetOrigin(aOrigin);
	}

void CHuiCanvasWsSwGc::WsSetPenColorL( TRgb aColor)
	{
    if (!IsRenderingEnabled())
        {
        return;
        }

    iBitGc->SetPenColor(aColor);
	}

void CHuiCanvasWsSwGc::WsSetPenStyleL( TInt aStyle)
	{
    if (!IsRenderingEnabled())
        {
        return;
        }

	iBitGc->SetPenStyle((CFbsBitGc::TPenStyle)aStyle);
	}

void CHuiCanvasWsSwGc::WsSetPenSizeL( TInt aWidth, TInt aHeight)
	{
    if (!IsRenderingEnabled())
        {
        return;
        }

	TSize lSize(aWidth,aHeight);
	iBitGc->SetPenSize(lSize);
	}

void CHuiCanvasWsSwGc::WsSetTextShadowColorL( TRgb /*aColor*/)
	{
 	// N/A
	}

void CHuiCanvasWsSwGc::WsSetCharJustificationL( TInt aExcessiveWidth, TInt aNumGap)
	{
    if (!IsRenderingEnabled())
        {
        return;
        }

	iBitGc->SetCharJustification(aExcessiveWidth,aNumGap);
	}

void CHuiCanvasWsSwGc::WsSetWordJustificationL( TInt aExcessiveWidth, TInt aNumGap)
	{
    if (!IsRenderingEnabled())
        {
        return;
        }

    iBitGc->SetWordJustification(aExcessiveWidth,aNumGap);
	}

void CHuiCanvasWsSwGc::WsSetUnderlineStyleL( TInt aValue )
	{
    if (!IsRenderingEnabled())
        {
        return;
        }
	
	if (aValue < 1000) // magic, we recycle the same function for both underline and strike through
	    {
	    iBitGc->SetUnderlineStyle((TFontUnderline)aValue);	
	    }
	else
	    {
	    iBitGc->SetStrikethroughStyle((TFontStrikethrough)(aValue-1000));
	    }
	}

void CHuiCanvasWsSwGc::WsSetBrushPatternL( TInt aValue)
	{
    if (!IsRenderingEnabled())
        {
        return;
        }

	iBitGc->UseBrushPattern(aValue);
	}

void CHuiCanvasWsSwGc::WsSetFontL( TInt aFontId)
	{
    if (!IsRenderingEnabled())
        {
        return;
        }

	iBitGc->UseFont(aFontId);
	}

void CHuiCanvasWsSwGc::WsCopyRectL( TPoint& aPoint, TRect& aRect)
	{
    if (!IsRenderingEnabled())
        {
        return;
        }

	iBitGc->CopyRect(AdjustCoordinates(aPoint),AdjustCoordinates(aRect));
	}

void CHuiCanvasWsSwGc::WsUpdateJustificationL( TPtr& aTextValue)
	{
    if (!IsRenderingEnabled())
        {
        return;
        }

	iBitGc->UpdateJustification(aTextValue);
	}

void CHuiCanvasWsSwGc::WsUpdateJustificationVerticalL( TInt aValue, TPtr& aTextValue )
	{
    if (!IsRenderingEnabled())
        {
        return;
        }

	if(aValue)
    	iBitGc->UpdateJustificationVertical(aTextValue,ETrue);
	else
		iBitGc->UpdateJustificationVertical(aTextValue,EFalse);
	}

void CHuiCanvasWsSwGc::WsSetFontNoDuplicateL( TInt aCurrentFont)
	{
    if (!IsRenderingEnabled())
        {
        return;
        }

	iBitGc->UseFont(aCurrentFont);	
	}

void CHuiCanvasWsSwGc::WsCopySettingsL()
	{
    }

void CHuiCanvasWsSwGc::WsSetClippingRectL( TRect& aRect)
    {
    if (!IsRenderingEnabled())
        {
        return;
        }

    // Symbian SetClippingRect takes ws origin into account, 
    // but SetClippingRegion does not.
 	iWsClipRect = aRect;
    iBitGc->SetClippingRect(AdjustCoordinates(aRect));                
    return;
    }

void CHuiCanvasWsSwGc::WsCancelClippingRectL()
    {
    if (!IsRenderingEnabled())
        {
        return;
        }

    iWsClipRect = AdjustCoordinates(RoundedDisplayRect());        
    iBitGc->CancelClippingRect();
    }

void CHuiCanvasWsSwGc::WsSetFadedL( TInt& aFaded)
    {
    if (!IsRenderingEnabled())
        {
        return;
        }

    iBitGc->SetFaded(aFaded);
    }

void CHuiCanvasWsSwGc::WsSetFadingParametersL( TInt aBlackMap, TInt aWhiteMap)
    {
    if (!IsRenderingEnabled())
        {
        return;
        }

    iBitGc->SetFadingParameters(aBlackMap, aWhiteMap);    
	}

void CHuiCanvasWsSwGc::WsFadeAreaL( RRegion& /*aRegion */)
    {
    if (!IsRenderingEnabled())
        {
        return;
        }

    return; // TODO: Fading does not work, somebody fix it please
    
/*    
	iIsReallyDirty = ETrue;

    // Area is in screen coordinates so we must convert it ?
    RRegion adjustedRegion;

    for (TInt i=0; i < aRegion.Count(); i++)
        {
        adjustedRegion.AddRect(AdjustCoordinates(aRegion[i]));   
        }
        
   	iBitGc->FadeArea(&adjustedRegion);    
    adjustedRegion.Close();    
*/
    }

void CHuiCanvasWsSwGc::WsMapColorsL()
    {
    // N/A
    }

void CHuiCanvasWsSwGc::WsSetUserDisplayModeL( TInt /*aMode*/ )
    {
    // N/A
    }

void CHuiCanvasWsSwGc::WsUseFontL(TInt aCurrentFont)
    {
    if (!IsRenderingEnabled())
        {
        return;
        }

    iBitGc->UseFont(aCurrentFont);    
    }       

void CHuiCanvasWsSwGc::BeginActionL(TInt aAction, TRect /*aDisplayRect*/, const CHuiCanvasVisual& aUser, TBool aIsCachePrepared, const TRegion& aUpdateRegion)
	{
	iAction = aAction;
	iVisual = &aUser;

    if (!iVisual)
        {
        return;    
        }        

    // Update area is in screen coordinates so we must convert it 
    iDefaultClipRegion.Clear();
    for (TInt i=0; i < aUpdateRegion.Count(); i++)
        {
        iDefaultClipRegion.AddRect(AdjustCoordinates(aUpdateRegion[i]));   
        }
    
    TSize size = RoundedDisplayRect().Size();	

    TBool recreateBackbuffer = (!iBitGc || (iBitBitmap && iBitBitmap->SizeInPixels() != size));
    
    // If this is first draw or size has changed, re-create backbuffer
	if (recreateBackbuffer)
		{				    	
        // Release old cached backbuffer image
        iVisual->Env().CanvasTextureCache().ReleaseAllCachedEntries(*iVisual);		        

        iBitmapInitialized = EFalse;
		
        if (!iBitBitmap)
            {
            iBitBitmap = new (ELeave) CFbsBitmap;
            iBitBitmap->Create(size, SelectDisplayMode());  
            }
        else
            {
            iBitBitmap->Resize(size);    
            }    
	    
        if (!iBitDevice)
            {
            iBitDevice = CFbsBitmapDevice::NewL(iBitBitmap);        
            }
        else
            {
            iBitDevice->Resize(size);    
            }    

	    delete iBitGc;
	    iBitGc = NULL;
	    User::LeaveIfError(iBitDevice->CreateContext(iBitGc));             
        
        TRgb clearColor = KRgbWhite;
        clearColor.SetAlpha(0x00);
        
	    iBitGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
        iBitGc->SetPenColor(clearColor);
        iBitGc->SetBrushColor(clearColor);
        iBitGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
  	    iBitGc->Clear();
  	    iBitGc->Reset();    	  
  	    
  	    iIsReallyDirty = EFalse;
		}
	
	if (iBitGc)
	    {
	    iBitGc->Reset();
	    }
	
	iBitmapPrepared = aIsCachePrepared && iBitmapInitialized;
	}

// Note that for perfomance reasons we keep iBitBitmap alive in this method   
void CHuiCanvasWsSwGc::EndActionL(const TRegion& /*aUpdateRegion*/, TBool aUpdateDisplay)
	{
    if (!iBitBitmap || !iVisual)
        {
        return;    
        }        
        
    if ( iAction == EDrawBuffer )
        {
        iBitmapInitialized = ETrue;
        iBitmapPrepared = ETrue;
        }
        
    // If capturing is used, then copy content from bitmap.
    // TODO: We could use capturing bitmap directly to get better performance if the window content changes a lot 
    // TODO: Is this actually a bug ? We should not copy to iCapturingBitGc EVERYTIME !??!
    if (iCapturingBitGc)
        {
        iCapturingBitGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
        iCapturingBitGc->BitBlt(TPoint(0,0), iBitBitmap);    
        }

    if (iAction == EDrawBuffer && aUpdateDisplay && iIsReallyDirty && RoundedDisplayRect().Size() != TSize(0,0))
        {
        // Texture cache creates new cached (hw accelerated) image from backbuffer bitmap if needed
        // However, we must enable touch count check for the duration of cached image creation.
        // In addition, we must perform CreateCachedImageL only after we have really updated bitmap,
        // so we want touch count check to be used.
        
        THuiCachedImageParams cachedImageParams;
        cachedImageParams.iBitmapHandle = iBitBitmap->Handle();
         
        TBool wasTouchCountEnabled = iVisual->Env().CanvasTextureCache().IsTouchCountCheckEnabled();
        iVisual->Env().CanvasTextureCache().EnableTouchCountCheck(ETrue);
        iVisual->Env().CanvasTextureCache().ReleaseAllCachedEntries(*iVisual);
        
        const CHuiCanvasGraphicImage* cachedImage = NULL;
        TRAPD(err, cachedImage = iVisual->Env().CanvasTextureCache().CreateCachedImageL(cachedImageParams, *iVisual));
        
        iVisual->Env().CanvasTextureCache().EnableTouchCountCheck(wasTouchCountEnabled);
        if (err)
            {
            User::Leave(err);
            }

        const CHuiTexture* texture = NULL;
        if (cachedImage)
            {
            texture = cachedImage->Texture();
            }
        if (texture)
            {
            // Draw backbuffer to screen            
            THuiRealRect destinationRect(iVisual->DisplayRect().iTl, texture->Size());                
            CanvasGc().DrawImage(*texture, destinationRect);        
            }
        else
            {
    #ifdef _DEBUG
            RDebug::Print(_L("CHuiCanvasWsSwGc::EndActionL: No cached image !"));
    #endif
            }
        }
	}

CHuiCanvasGc& CHuiCanvasWsSwGc::CanvasGc() const
	{
	return *iCanvasGc;
	}	  	
    
    
TPoint CHuiCanvasWsSwGc::AdjustCoordinates(TPoint aPoint)
	{
	TPoint adjustment = RoundedDisplayRect().iTl;
	TPoint lPoint(aPoint.iX-adjustment.iX + iPosDelta.iX,aPoint.iY-adjustment.iY + iPosDelta.iY );	
	return lPoint;
	}

TRect CHuiCanvasWsSwGc::AdjustCoordinates(TRect aRect)
	{
	TRect rect = aRect;
	TPoint adjustment = -RoundedDisplayRect().iTl;
	    
	rect.Move(adjustment.iX + iPosDelta.iX, adjustment.iY  + iPosDelta.iY);
	return rect;
	}
	
TDisplayMode CHuiCanvasWsSwGc::SelectDisplayMode()
    {
    TDisplayMode mode = EColor16MAP;
    
    // Note 64K mode cannot be used until separate mask is supported. It might be
    // possible to generate according to dirty region ?
    
    /*
    // NOTE: EHuiRendererGles10 and EHuiRendererGles11 temporarely use EColor64K
    // beacuse it looks as there are some problems with uploading 16MA 
    // mode bitmaps into multisegmented textures.
    //
    if (CHuiStatic::Env().Renderer() == EHuiRendererBitgdi || 
        CHuiStatic::Env().Renderer() == EHuiRendererGles10 || 
        CHuiStatic::Env().Renderer() == EHuiRendererGles11)
        {        
        mode = EColor64K;           
        }
    */    
    return mode;    
    }
	
TInt CHuiCanvasWsSwGc::SetCapturingBufferL(CFbsBitmap* aTarget)
    {
    if (iCapturingBitBitmap)
        {
        delete iCapturingBitBitmap;
        iCapturingBitBitmap = NULL;    
        }
    
    if (iCapturingBitGc)
        {
        delete iCapturingBitGc;
        iCapturingBitGc = NULL;    
        }

    if (iCapturingBitDevice)
        {
        delete iCapturingBitDevice;
        iCapturingBitDevice = NULL;    
        }

    if (aTarget)
        {
        iCapturingBitBitmap = new (ELeave) CFbsBitmap;
        iCapturingBitBitmap->Duplicate(aTarget->Handle());                        
        }
                   
    if (iCapturingBitBitmap)
        {
	    iCapturingBitDevice = CFbsBitmapDevice::NewL(iCapturingBitBitmap);
	    User::LeaveIfError(iCapturingBitDevice->CreateContext(iCapturingBitGc));                         
        }    
    
    return KErrNone;
    }

TRect CHuiCanvasWsSwGc::RoundedDisplayRect() const
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

void CHuiCanvasWsSwGc::SetPositionDelta( TPoint& aPoint )
    {
    iPosDelta = aPoint;
    }

CFbsBitmap* CHuiCanvasWsSwGc::CreateMovedMaskL(CFbsBitmap& aOriginalMask, TPoint aNewTopLeftCorner)
    {
    TSize size = aOriginalMask.SizeInPixels();
    
    CFbsBitmap* newMask = new(ELeave)CFbsBitmap;                       
    CleanupStack::PushL(newMask);
            
    newMask->Create(size, aOriginalMask.DisplayMode());       
    
    CFbsDevice* newMaskdevice = NULL;
    CFbsBitGc* newMaskGc = NULL;

    newMaskdevice = CFbsBitmapDevice::NewL(newMask);
    CleanupStack::PushL(newMaskdevice);
    User::LeaveIfError(newMaskdevice->CreateContext(newMaskGc));

    newMaskGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    newMaskGc->SetPenStyle(CGraphicsContext::ESolidPen);
    newMaskGc->SetPenColor(KRgbWhite);
    newMaskGc->SetBrushColor(KRgbWhite);
    newMaskGc->DrawRect(TRect(TPoint(0,0), size));
    newMaskGc->BitBlt(TPoint(0,0), &aOriginalMask, TRect(aNewTopLeftCorner, size));
    newMaskGc->BitBlt(aNewTopLeftCorner, &aOriginalMask, TRect(TPoint(0,0), size));
    
    delete newMaskGc;    
    CleanupStack::PopAndDestroy(newMaskdevice);
    CleanupStack::Pop(newMask);
    
    return newMask;
    }

void CHuiCanvasWsSwGc::ClearCache()
    {
    // Nothing to do..
    }

TBool CHuiCanvasWsSwGc::IsRenderBufferEnabled() const
    {
    return (iBitBitmap != NULL);
    }

TBool CHuiCanvasWsSwGc::IsRenderBufferPrepared() const
    {
    return (iBitBitmap != NULL) && iIsReallyDirty && 
           (RoundedDisplayRect().Size() != TSize(0,0)) && iBitmapPrepared;       
    }

TBool CHuiCanvasWsSwGc::IsRenderBufferInitialized() const
    {
    // always initialized
    return (iBitBitmap != NULL) && iBitmapInitialized;
    }

TInt CHuiCanvasWsSwGc::EnableRenderbuffer(TBool /*aEnable*/)
    {
    // Enabled by default
    return KErrNone;
    }

THuiCanvasWsGcType CHuiCanvasWsSwGc::Type() const
    {
    return EHuiCanvasWsSw;
    }


void CHuiCanvasWsSwGc::EnableUpdateRegion(const TRegion& aUpdateRegion, TBool aClear)
    {
    // Area is in screen coordinates so we must convert it 
    iDefaultClipRegion.Clear();
    for (TInt i=0; i < aUpdateRegion.Count(); i++)
        {
        iDefaultClipRegion.AddRect(AdjustCoordinates(aUpdateRegion[i]));   
        }

    if (!iBitGc)
        {
        return;
        }
    
    iBitGc->Reset();
    
    if (aClear)
        {
        if (iDefaultClipRegion.Count())
            {
            iBitGc->SetClippingRegion(iDefaultClipRegion);
            }

        TRgb clearColor = KRgbWhite;
        clearColor.SetAlpha(0x0);
        iBitGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
        iBitGc->SetPenColor(clearColor);
        iBitGc->SetBrushColor(clearColor);
        iBitGc->SetBrushStyle(CGraphicsContext::ESolidBrush);        
        iBitGc->Clear(); 
        iBitGc->Reset();
        }    

    if (iDefaultClipRegion.Count())
        {
        iBitGc->SetClippingRegion(iDefaultClipRegion);
        }
    }

void CHuiCanvasWsSwGc::DisableUpdateRegion()
    {
    if (iBitGc)
        {
        iBitGc->CancelClippingRegion();
        }
    }

void CHuiCanvasWsSwGc::SetRelativeOrientation(CHuiGc::TOrientation /*aOrientation*/)
    {
    // Not supported yet
    }

TBool CHuiCanvasWsSwGc::IsRenderingEnabled() const
    {
    return iAction == EDrawBuffer;
    }

void CHuiCanvasWsSwGc::ClearCapturingBufferArea(const TRect& /*aRect*/)
    {
    // Not supported by fallback GC.
    }

