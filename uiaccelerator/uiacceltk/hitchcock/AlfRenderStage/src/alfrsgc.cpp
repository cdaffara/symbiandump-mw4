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
* Description:   AlfRenderStage provides a way to redirect avkon drawing to a graphics accelerator
*
*/

/*
 * @note: There are a lot of L functions called by non-leaving GC functions and thus possibly causing 
 *        WServ and phone to boot in leave situation. That has been done on purpose, because 
 *        it is extreamly fatal case if those functions leave. We cannot skip critical commands.
 *        In practise, those functions never leave.
 * 
 */

#include <gdi.h>
#include <uiacceltk/HuiCanvasVisual.h>
#include <huiwscanvascommands.h>
#include "alfrssendbuffer.h"
#include "alfrsgc.h"

const TUint8 KNoSupportInAnyRenderer = 63; 

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfGraphicsContext* CAlfGraphicsContext::NewL( CAlfRsSendBuffer& aAlfSendBuffer )
	{
	CAlfGraphicsContext* self = new(ELeave) CAlfGraphicsContext( aAlfSendBuffer );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CAlfGraphicsContext::CAlfGraphicsContext( CAlfRsSendBuffer& aAlfSendBuffer) :
    iFontHandle(0),
    iTextShadowColor( TRgb(KRgbBlack) ),
    iBrushPatternSet( EFalse ),
    iPenColor( TRgb( KRgbBlack ) ),
    iBrushColor( TRgb( KRgbBlack ) ),
    iSendBuffer( aAlfSendBuffer )
        {}

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::ConstructL( )
	{
	iFontHandle = 0;
	iBrushPatternSet = EFalse;
	DoReset();
	}

// ---------------------------------------------------------------------------
// C++ destructor
// ---------------------------------------------------------------------------
//
CAlfGraphicsContext::~CAlfGraphicsContext()
	{
	}

// ---------------------------------------------------------------------------
// BitBlt
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::BitBlt(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap)
    {
    
#ifdef _OPTIMIZE_WS_COMMANDS_ADVANCED_
    if ( !CheckDelayedClipping( TRect( aDestPos, aSourceBitmap.SizeInPixels()))) 
        {
        // outside clipping region
        return;
        }
    FlushStateL( iRollback );
#endif    
    
    iSendBuffer.WriteIntsL(EAlfBitBlt,
            3,
            aDestPos.iX,
            aDestPos.iY,
            aSourceBitmap.Handle());

	const TRect rect = aSourceBitmap.SizeInPixels() + aDestPos;
	iSendBuffer.AppendPatternSearchCache( aSourceBitmap, NULL, rect, aDestPos, 1 );
	iDrawCommandCount++;
	}

// ---------------------------------------------------------------------------
// BitBlt
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::BitBlt(
        const TPoint& aDestPos, 
        const CFbsBitmap& aSourceBitmap, 
        const TRect& aSourceRect )
    {
#ifdef _OPTIMIZE_WS_COMMANDS_ADVANCED_    
    if ( !CheckDelayedClipping( TRect( aDestPos, aSourceRect.Size()))) 
        {
        // outside clipping region
        return;
        }
    FlushStateL( iRollback );
#endif
    iSendBuffer.WriteIntsL(EAlfBitBltRect,
            7,
            aDestPos.iX,
            aDestPos.iY,
            aSourceBitmap.Handle(),
            aSourceRect.iTl.iX,
	        aSourceRect.iTl.iY,
	        aSourceRect.iBr.iX,
	        aSourceRect.iBr.iY );
	iSendBuffer.AppendPatternSearchCache( aSourceBitmap, NULL, aSourceRect, aDestPos, 1 );
	iDrawCommandCount++;
	}

// ---------------------------------------------------------------------------
// BitBltMasked
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::BitBltMasked(
        const TPoint& aDestPos, 
        const CFbsBitmap& aSourceBitmap, 
        const TRect& aSourceRect, 
        const CFbsBitmap& aMaskBitmap, 
        TBool aInvertMask )
    {
#ifdef _OPTIMIZE_WS_COMMANDS_ADVANCED_    
    if ( !CheckDelayedClipping( TRect( aDestPos, aSourceRect.Size()))) 
        {
        // outside clipping region
        return;
        }

    FlushStateL( iRollback );
#endif    
    iSendBuffer.WriteIntsL(EAlfBitBltMasked,
            9,
            aDestPos.iX,
            aDestPos.iY,
            aSourceBitmap.Handle(),
            aSourceRect.iTl.iX,
	        aSourceRect.iTl.iY,
	        aSourceRect.iBr.iX,
	        aSourceRect.iBr.iY,
	        aMaskBitmap.Handle(),
	        aInvertMask );
	
	iSendBuffer.AppendPatternSearchCache( 
	        aSourceBitmap, 
	        &aMaskBitmap, 
	        aSourceRect, 
	        aDestPos, 
	        aInvertMask );
    iDrawCommandCount++;
	}

// ---------------------------------------------------------------------------
// BitBltMasked
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::BitBltMasked(
        const TPoint& aDestPos, 
        const CFbsBitmap& aSourceBitmap, 
        const TRect& aSourceRect, 
        const CFbsBitmap& aMaskBitmap, 
        const TPoint& aMaskPos )
    {
#ifdef _OPTIMIZE_WS_COMMANDS_ADVANCED_
    if ( !CheckDelayedClipping( TRect( aDestPos, aSourceRect.Size()))) 
        {
        // outside clipping region
        return;
        }
    FlushStateL( iRollback );	
#endif
    iSendBuffer.WriteIntsL( EAlfBitBltMaskedPoint,
            10,
            aDestPos.iX,
            aDestPos.iY,
            aSourceBitmap.Handle(),
            aSourceRect.iTl.iX,
            aSourceRect.iTl.iY,
            aSourceRect.iBr.iX,
            aSourceRect.iBr.iY,
            aMaskBitmap.Handle(),
            aMaskPos.iX,
            aMaskPos.iY );
    iDrawCommandCount++;
	}

// ---------------------------------------------------------------------------
// Reioion
// calls CancelClippingRegion
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::ResetClippingRegion()
    {
    if ( iClippingRegion.Count() == 0 )
        {
        return;
        }
    iClippingRegion.Clear();
#ifdef _OPTIMIZE_WS_COMMANDS_ADVANCED_	
    iDelayedContextState |= EModifiedClipReset;
    iDelayedContextState &= ~EModifiedClipRect;
    iPreviousClippingRegion.Clear();
#else
    iSendBuffer.WriteCommandL( EAlfResetClippingRegion );
#endif
    }

// ---------------------------------------------------------------------------
// Clear
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::Clear()
    {
#ifdef _OPTIMIZE_WS_COMMANDS_ADVANCED_  
    FlushStateL( iRollback );
#endif
    iSendBuffer.WriteCommandL( EAlfClear );
    iDrawCommandCount++;
    }

// ---------------------------------------------------------------------------
// Clear
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::Clear(const TRect& aRect)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_ADVANCED_    
    // This will result skipping the preceding clipping rect setting, if text is fully inside the clip
    CheckDelayedClipping( aRect );
    FlushStateL( iRollback );
#endif
    iSendBuffer.WriteIntsL( EAlfClearRect,
            4,
            aRect.iTl.iX,
            aRect.iTl.iY,
            aRect.iBr.iX,
            aRect.iBr.iY );
    iDrawCommandCount++;
	}

// ---------------------------------------------------------------------------
// ResetBrushPattern
// calls DiscardBrushPattern on the other side
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::ResetBrushPattern()
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    if (!iBrushPatternSet)
        return;
#endif
    iBrushPatternSet = EFalse;
    iSendBuffer.WriteCommandL( EAlfResetBrushPattern );
    }

// ---------------------------------------------------------------------------
// ResetFont
// calls DiscardFont on the other side
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::ResetFont()
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    if (iFontHandle == 0)
        return;
#endif
    iFontHandle = 0;
#ifdef  _OPTIMIZE_WS_COMMANDS_FONT_
    iDelayedContextState |= EModifiedResetFont;
    iDelayedContextState &= ~EModifiedSetFontNoDuplicate;
#else    
    iSendBuffer.WriteCommandL( EAlfResetFont );
#endif
    }

// ---------------------------------------------------------------------------
// DrawArc
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawArc(
        const TRect& aRect, 
        const TPoint& aStart, 
        const TPoint& aEnd )
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif    
    iSendBuffer.WriteIntsL( EAlfDrawArc,
            8,
            aRect.iTl.iX,
            aRect.iTl.iY,
            aRect.iBr.iX,
            aRect.iBr.iY,
	        aStart.iX,
	        aStart.iY,
	        aEnd.iX,
	        aEnd.iY );
    iDrawCommandCount++;
	}

// ---------------------------------------------------------------------------
// DrawPie
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawPie(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif
    iSendBuffer.WriteIntsL( EAlfDrawPie,
            8,
            aRect.iTl.iX,
            aRect.iTl.iY,
            aRect.iBr.iX,
            aRect.iBr.iY,
            aStart.iX,
            aStart.iY,
            aEnd.iX,
            aEnd.iY );
    iDrawCommandCount++;
    }

// ---------------------------------------------------------------------------
// DrawBitmap
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif    
    iSendBuffer.WriteIntsL( EAlfDrawBitmap1,
            5,
            aDestRect.iTl.iX,
            aDestRect.iTl.iY,
            aDestRect.iBr.iX,
            aDestRect.iBr.iY,
	        aSourceBitmap.Handle() );
    iDrawCommandCount++;
	}

// ---------------------------------------------------------------------------
// DrawBitmap
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawBitmap(
        const TRect& aDestRect, 
        const CFbsBitmap& aSourceBitmap, 
        const TRect& aSourceRect )
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif
    iSendBuffer.WriteIntsL( EAlfDrawBitmap2,
            9,
            aDestRect.iTl.iX,
            aDestRect.iTl.iY,
            aDestRect.iBr.iX,
            aDestRect.iBr.iY,
	        aSourceBitmap.Handle(),
	        aSourceRect.iTl.iX,
	        aSourceRect.iTl.iY,
	        aSourceRect.iBr.iX,
	        aSourceRect.iBr.iY );
    iDrawCommandCount++;
	}

// ---------------------------------------------------------------------------
// DrawBitmap
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawBitmap(const TPoint &aTopLeft, const CFbsBitmap &aSource)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif
    iSendBuffer.WriteIntsL( EAlfDrawBitmap3,
            3,
            aTopLeft.iX,
            aTopLeft.iY,
            aSource.Handle() );
    iDrawCommandCount++; 
	}

// ---------------------------------------------------------------------------
// DrawBitmapMasked
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawBitmapMasked(
        const TRect& aDestRect,
        const CFbsBitmap& aSourceBitmap,
        const TRect& aSourceRect,
        const CFbsBitmap& aMaskBitmap,
        TBool aInvertMask )
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    FlushStateL( iRollback );
#endif
    iSendBuffer.WriteIntsL( EAlfDrawBitmapMasked,
            11,
            aDestRect.iTl.iX,
            aDestRect.iTl.iY,
            aDestRect.iBr.iX,
            aDestRect.iBr.iY,
            aSourceBitmap.Handle(),
            aSourceRect.iTl.iX,
            aSourceRect.iTl.iY,
            aSourceRect.iBr.iX,
            aSourceRect.iBr.iY,
            aMaskBitmap.Handle(),
            aInvertMask  );
    iDrawCommandCount++;
	}

// ---------------------------------------------------------------------------
// DrawRoundRect
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawRoundRect(const TRect& aRect, const TSize& aEllipse)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif    
    iSendBuffer.WriteIntsL( EAlfDrawRoundRect,
            6,
            aRect.iTl.iX,
            aRect.iTl.iY,
            aRect.iBr.iX,
            aRect.iBr.iY,
	        aEllipse.iWidth,
	        aEllipse.iHeight );
    iDrawCommandCount++;
	}

// ---------------------------------------------------------------------------
// DrawPolyLine
// Funnel both DrawPolyline to the same
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawPolyLine(const CArrayFix<TPoint>* aPoWriteIntList)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    FlushStateL( iRollback );
#endif
    iSendBuffer.WritePointArrayL( EAlfDrawPolyLine, aPoWriteIntList );
    iDrawCommandCount++; 
    }

// ---------------------------------------------------------------------------
// DrawPolyLine
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawPolyLine(const TArray<TPoint>& aPointList)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif
    iSendBuffer.WritePointArrayL( EAlfDrawPolyLine, &aPointList );
    iDrawCommandCount++; 
    };
// ---------------------------------------------------------------------------
// DrawPolyLineNoEndPoint
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawPolyLineNoEndPoint(
        const TPoint *aPoWriteIntList, 
        TInt aNumPoints )
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif    
    iSendBuffer.WritePointArrayL( EAlfDrawPolyLineNoEndPoint, aPoWriteIntList, aNumPoints );
    iDrawCommandCount++; 
    }


// ---------------------------------------------------------------------------
// DrawPolyLine
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawPolyLine(const TPoint *aPoWriteIntList, TInt aNumPoints)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif
    iSendBuffer.WritePointArrayL( EAlfDrawPolyLine, aPoWriteIntList, aNumPoints );
    iDrawCommandCount++; 
    }

// ---------------------------------------------------------------------------
// DrawPolyLineNoEndPoint
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawPolyLineNoEndPoint(const TArray<TPoint>& aPointList)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif
    iSendBuffer.WritePointArrayL( EAlfDrawPolyLineNoEndPoint, &aPointList );
    iDrawCommandCount++; 
    }

// ---------------------------------------------------------------------------
// DrawPolyLineNoEndPoint
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawPolyLineNoEndPoint(const CArrayFix<TPoint>* aPoWriteIntList)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif
    iSendBuffer.WritePointArrayL( EAlfDrawPolyLineNoEndPoint, aPoWriteIntList );
    iDrawCommandCount++; 
    }

// ---------------------------------------------------------------------------
// DrawPolygon
// Funnel all polygons into one. The ouput structure is the same in all of these
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawPolygon(
        const CArrayFix<TPoint>* aPoWriteIntList, 
        TFillRule aFillRule )
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif
    iSendBuffer.WritePointArrayL( EAlfDrawPolygon, aPoWriteIntList );
    iSendBuffer.WriteIntL( EAlfDrawPolygon, aFillRule );
    iDrawCommandCount++; 
    }

// ---------------------------------------------------------------------------
// DrawPolygon
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawPolygon(const TArray<TPoint>& aPointList, TFillRule aFillRule)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif    
    iSendBuffer.WritePointArrayL( EAlfDrawPolygon, &aPointList );
    iSendBuffer.WriteIntL( EAlfDrawPolygon, aFillRule );
    iDrawCommandCount++;
    };

// ---------------------------------------------------------------------------
// DrawPolygon
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawPolygon(
        const TPoint *aPoWriteIntList, 
        TInt aNumPoints, 
        TFillRule aFillRule )
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif    
    iSendBuffer.WritePointArrayL( EAlfDrawPolygon, aPoWriteIntList, aNumPoints );
    iSendBuffer.WriteIntL( EAlfDrawPolygon, aFillRule );
    iDrawCommandCount++; 
    }

// ---------------------------------------------------------------------------
// DrawEllipse
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawEllipse(const TRect& aRect)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    FlushStateL( iRollback );
#endif
    iSendBuffer.WriteIntsL( EAlfDrawEllipse,
            4,
            aRect.iTl.iX,
            aRect.iTl.iY,
            aRect.iBr.iX,
            aRect.iBr.iY );
    iDrawCommandCount++;
	}

// ---------------------------------------------------------------------------
// DrawLine
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawLine(const TPoint& aStart, const TPoint& aEnd)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif
    iSendBuffer.WriteIntsL( EAlfDrawLine,
            4,
            aStart.iX,
            aStart.iY,
            aEnd.iX,
            aEnd.iY );
    iDrawCommandCount++;
	}

// ---------------------------------------------------------------------------
// DrawLineTo
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawLineTo(const TPoint& aPoint)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    FlushStateL( iRollback );
#endif
    iSendBuffer.WriteIntsL( EAlfDrawLineTo, 2, aPoint.iX, aPoint.iY );
    iDrawCommandCount++; 

    }

// ---------------------------------------------------------------------------
// DrawLineBy
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawLineBy(const TPoint& aVector)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    FlushStateL( iRollback );
#endif
    iSendBuffer.WriteIntsL( EAlfDrawLineBy, 2, aVector.iX, aVector.iY );
    iDrawCommandCount++; 

    }

// ---------------------------------------------------------------------------
// DrawRect
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawRect(const TRect& aRect)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    FlushStateL( iRollback );
#endif
    iSendBuffer.WriteIntsL( EAlfDrawRect,
            4,
            aRect.iTl.iX,
            aRect.iTl.iY,
            aRect.iBr.iX,
            aRect.iBr.iY );
    iDrawCommandCount++;
	}


// ---------------------------------------------------------------------------
// DrawText
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawText(const TDesC& aText,const TTextParameters* aParam)
    {
    TInt tmp;
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    FlushStateL( iRollback );
#endif    
    iSendBuffer.WriteDescriptorAndIntsL( EAlfDrawText1, aText,
            (CGraphicsContext::TTextParameters*)aParam->iStart,0, tmp );

    iDrawCommandCount++;
    }

// ---------------------------------------------------------------------------
// DrawText
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawText(
        const TDesC& aText,
        const TTextParameters* aParam,
        const TPoint& aPosition )
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    FlushStateL( iRollback );
#endif    
    iSendBuffer.WriteDescriptorAndIntsL( EAlfDrawText2, aText,
            (CGraphicsContext::TTextParameters*)aParam,
            2,
            aPosition.iX,
            aPosition.iY );
    iDrawCommandCount++; 
    }

// ---------------------------------------------------------------------------
// DrawText
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawText(
        const TDesC& aText,
        const TTextParameters* aParam,
        const TRect& aClipRect )
    {
#ifdef _OPTIMIZE_WS_COMMANDS_ADVANCED_   
    CheckDelayedClipping( aClipRect );
#endif
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif    
    iSendBuffer.WriteDescriptorAndIntsL( EAlfDrawText3, aText,
            (CGraphicsContext::TTextParameters*)aParam,
            4,
            aClipRect.iTl.iX,
            aClipRect.iTl.iY,
            aClipRect.iBr.iX,
            aClipRect.iBr.iY
            );
    iDrawCommandCount++;
    }

// ---------------------------------------------------------------------------
// DrawText
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawText(
        const TDesC& aText,
        const TTextParameters* aParam,
        const TRect& aClipFillRect,
        TInt aBaselineOffset,
        TTextAlign aHrz,
        TInt aMargin )
    {
#ifdef _OPTIMIZE_WS_COMMANDS_ADVANCED_
    // This will result skipping the preceding clipping rect setting, if text is fully inside the clip
    CheckDelayedClipping( aClipFillRect );
#endif
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif
    iSendBuffer.WriteDescriptorAndIntsL( EAlfDrawText4, aText,
            (CGraphicsContext::TTextParameters*)aParam,
            7,
            aClipFillRect.iTl.iX,
            aClipFillRect.iTl.iY,
            aClipFillRect.iBr.iX,
            aClipFillRect.iBr.iY,
            aBaselineOffset,
            aHrz,
            aMargin
            );
    iDrawCommandCount++;
    }

// ---------------------------------------------------------------------------
// DrawTextVertical
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawTextVertical(
        const TDesC& aText,
        const TTextParameters* aParam,
        TBool aUp )
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    FlushStateL( iRollback );
#endif    
    iSendBuffer.WriteDescriptorAndIntsL( EAlfDrawTextVertical1, aText,
            (CGraphicsContext::TTextParameters*)aParam,
            1,
            aUp
    );
    iDrawCommandCount++; 
    }

// ---------------------------------------------------------------------------
// DrawTextVertical
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawTextVertical(
        const TDesC& aText,
        const TTextParameters* aParam,
        const TPoint& aPosition,
        TBool aUp )
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    FlushStateL( iRollback );
#endif    
    iSendBuffer.WriteDescriptorAndIntsL( EAlfDrawTextVertical2, aText,
            (CGraphicsContext::TTextParameters*)aParam,
            3,
            aPosition.iX,
            aPosition.iY,
            aUp);
    iDrawCommandCount++;
    }

// ---------------------------------------------------------------------------
// DrawTextVertical
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawTextVertical(
        const TDesC& aText,
        const TTextParameters* aParam,
        const TRect& aClipRect,
        TBool aUp )
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    FlushStateL( iRollback );
#endif    
    iSendBuffer.WriteDescriptorAndIntsL( EAlfDrawTextVertical3, aText,
            (CGraphicsContext::TTextParameters*)aParam,
            5,
            aClipRect.iTl.iX,
            aClipRect.iTl.iY,
            aClipRect.iBr.iX,
            aClipRect.iBr.iY,
            aUp
            );
    iDrawCommandCount++;
    }
// ---------------------------------------------------------------------------
// DrawTextVertical
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawTextVertical(
        const TDesC& aText,
        const TTextParameters* aParam,
        const TRect& aClipRect,
        TInt aBaselineOffset,
        TBool aUp,
        TTextAlign aVert,
        TInt aMargin )
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    FlushStateL( iRollback );
#endif    
    iSendBuffer.WriteDescriptorAndIntsL( EAlfDrawTextVertical4, aText,
            (CGraphicsContext::TTextParameters*)aParam,
            8,
            aClipRect.iTl.iX,
            aClipRect.iTl.iY,
            aClipRect.iBr.iX,
             aClipRect.iBr.iY,
             aBaselineOffset,
             aUp,
             aVert,
             aMargin
             );
    iDrawCommandCount++;
    }

// ---------------------------------------------------------------------------
// DrawTextVertical
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DrawTextVertical(
        const TDesC& aText,
        const TTextParameters* aParam,
        const TRect& aClipRect,
        TInt aBaselineOffset,
        TInt aTextWidth,
        TBool aUp,
        TTextAlign aVert,
        TInt aMargin )
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    FlushStateL( iRollback );
#endif    
    iSendBuffer.WriteDescriptorAndIntsL( EAlfDrawTextVertical5, aText,
            (CGraphicsContext::TTextParameters*)aParam,
            9,
            aClipRect.iTl.iX,
            aClipRect.iTl.iY,
            aClipRect.iBr.iX,
             aClipRect.iBr.iY,
             aBaselineOffset,
             aTextWidth,
             aUp,
             aVert,
             aMargin
             );
    iDrawCommandCount++;
    }

// ---------------------------------------------------------------------------
// MoveTo
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::MoveTo(const TPoint& aPoint)
	{
	iSendBuffer.WriteIntsL( EAlfMoveTo, 2, aPoint.iX, aPoint.iY );
	}

// ---------------------------------------------------------------------------
// MoveBy
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::MoveBy(const TPoint& aVector)
	{
    iSendBuffer.WriteIntsL( EAlfMoveBy, 2, aVector.iX, aVector.iY );
	}

// ---------------------------------------------------------------------------
// Plot
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::Plot(const TPoint& aPoint)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    FlushStateL( iRollback );
#endif
    iSendBuffer.WriteIntsL( EAlfPlot,  2, aPoint.iX, aPoint.iY );
    iDrawCommandCount++; 
    }

// ---------------------------------------------------------------------------
// Reset
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::Reset()
    {
    // filter away sequential reset commands
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    if ( iSendBuffer.PreviousCommand() == EAlfReset ) 
        {
        return;
        }
#endif
    DoReset();
    iSendBuffer.WriteCommandL( EAlfReset );
    }

// ---------------------------------------------------------------------------
// DoReset
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::DoReset()
    {
    iFontHandle = 0;
    iOrigin = KHuiWsDefaultOrigin;
    iClippingRegion.Clear();
    
    iClippingRect = KHuiWsDefaultUserClipRect;
    iTextShadowColor = KHuiWsDefaultShadowColor;
    iBrushPatternSet = EFalse;
    iPenColor = KHuiWsDefaultPenColor;
    // SetPenColor( KHuiWsDefaultPenColor );
    iPenSize = KHuiWsDefaultPenSize;
    iDrawMode = KHuiWsDefaultDrawMode;
    iLinePosition = KHuiWsDefaultLinePosition;


    iBrushOrigin = KHuiWsDefaultBrushOrigin;
    iCharJustExcess = KHuiWsDefaultCharJustExcess;
    iCharJustNum = KHuiWsDefaultCharJustNum;
    iWordJustExcess = KHuiWsDefaultWordJustExcess;
    iWordJustNum = KHuiWsDefaultWordJustNum;
    iDitherOrigin = KHuiWsDefaultDitherOrigin;
    iDotLength = KHuiWsDefaultDotLength;
    iDotMask = KHuiWsDefaultDotMask;
    iDotParam = KHuiWsDefaultDotParam;
    iDotDirection = KHuiWsDefaultDotDirection;
    iShadowMode = KHuiWsDefaultShadowMode;
    iStrikethrough = KHuiWsDefaultStrikethrough;
    iUnderline = KHuiWsDefaultUnderline;
    iUserDisplayMode = KHuiWsDefaultUserDisplayMode;
    iPenStyle = KHuiWsDefaultPenStyle;
    //SetPenColor( KHuiWsDefaultPenStyle );

        
#ifdef  _OPTIMIZE_WS_COMMANDS_ADVANCED_
    iPreviousClippingRegion.Clear();
    iPreviousFontHandle = 0;
#endif
    
#ifdef  _OPTIMIZE_WS_COMMANDS_BASIC_
    iDelayedContextState = 0;
    
        
    iPreviousPenStyle = KHuiWsDefaultPenStyle;
    iPreviousPenColor = KHuiWsDefaultPenColor;

    iPreviousBrushStyle = KHuiWsDefaultBrushStyle;
    iPreviousBrushColor = KHuiWsDefaultBrushColor;

    
    SetBrushStyle( ENullBrush  ); // KHuiWsDefaultBrushStyle
    SetBrushColor( KHuiWsDefaultBrushColor );
#else
    iBrushColor = KHuiWsDefaultBrushColor;
    iBrushStyle = KHuiWsDefaultBrushStyle;
        
    
#endif
    }

// ---------------------------------------------------------------------------
// SetBrushColor
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetBrushColor(const TRgb& aColor)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    if (aColor == iBrushColor)
        return;
#endif
    iBrushColor = aColor;
#ifdef	_OPTIMIZE_WS_COMMANDS_BASIC_
    iDelayedContextState |= EModifiedBrushColor;
#else
    iSendBuffer.WriteIntsL( EAlfSetBrushColor, 1, aColor.Internal());
#endif
    }

// ---------------------------------------------------------------------------
// SetBrushOrigin
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetBrushOrigin(const TPoint& aOrigin)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    if (aOrigin == iBrushOrigin)
        return;
#endif    
    iBrushOrigin = aOrigin;
    iSendBuffer.WriteIntsL( EAlfSetBrushOrigin, 2, aOrigin.iX, aOrigin.iY );
    }

// ---------------------------------------------------------------------------
// SetBrushStyle
// calls SetBrushStyle(MWsGraphicsContextToBitGdiMappings::Convert(
//  (MWsGraphicsContext::TBrushStyle)value))
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetBrushStyle(TBrushStyle aBrushStyle)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    if (aBrushStyle == iBrushStyle)
        return;
#endif
    iBrushStyle = aBrushStyle;
    
    if ( aBrushStyle == ENullBrush || aBrushStyle == ESolidBrush || aBrushStyle == EPatternedBrush) 
        {
        // this is supported only by bitgdi
        iSendBuffer.SetSupportedCommand( EAlfSetBrushStyle, 0 ); // All renderer support this
        }
    else
        {
        // 63 = binary 0b0111111, not supported by any renderer
        iSendBuffer.SetSupportedCommand( EAlfSetBrushStyle, KNoSupportInAnyRenderer );
        }
    
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    iDelayedContextState |= EModifiedBrushStyle;
#else
    iSendBuffer.WriteIntL( EAlfSetBrushStyle, aBrushStyle );
#endif
    }

// ---------------------------------------------------------------------------
// SetClippingRegion
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetClippingRegion(const TRegion& aRegion)
    {
    iClippingRegion.Copy( aRegion );
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    iDelayedContextState |= EModifiedClipRect;
    iDelayedContextState &= ~EModifiedClipReset;
#else
    iSendBuffer.WriteRegionL( EAlfSetClippingRegion, aRegion );
#endif
    }

// ---------------------------------------------------------------------------
// SetDrawMode
// calls SetDrawMode(MWsGraphicsContextToBitGdiMappings::LossyConvert(
//  (MWsGraphicsContext::TDrawMode)value))
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetDrawMode(TDrawMode aDrawMode)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    if (aDrawMode == iDrawMode)
        return;
#endif
    iDrawMode = aDrawMode;
    iSendBuffer.WriteIntL( EAlfSetDrawMode, aDrawMode);
    }

// ---------------------------------------------------------------------------
// SetOrigin
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetOrigin(const TPoint& aPoint)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    if (iOrigin == aPoint)
        return;
#endif
    iOrigin = aPoint;
    iSendBuffer.WriteIntsL( EAlfSetOrigin,  2, aPoint.iX, aPoint.iY);
    }

// ---------------------------------------------------------------------------
// SetPenColor
// @todo Init to KRgbBlack
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetPenColor(const TRgb& aColor)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    if (aColor == iPenColor)
        return;
#endif
    iPenColor = aColor;
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    iDelayedContextState |= EModifiedPenColor;
#else
    iSendBuffer.WriteIntsL( EAlfSetPenColor, 1, aColor.Internal());
#endif
    }

// ---------------------------------------------------------------------------
// SetPenStyle
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetPenStyle(TPenStyle aPenStyle)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    if (aPenStyle == iPenStyle)
        return;
#endif
    iPenStyle = aPenStyle;
    
    if ( aPenStyle == ENullPen || aPenStyle == ESolidPen) 
        {
        // this is supported only by bitgdi
        iSendBuffer.SetSupportedCommand( EAlfSetPenStyle, 0 ); // All renderer support this
        }
    else
        {
        // 63 = binary 0b0111111, not supported by any renderer
        iSendBuffer.SetSupportedCommand( EAlfSetPenStyle, KNoSupportInAnyRenderer );
        }
    
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    iDelayedContextState |= EModifiedPenStyle;
#else
    iSendBuffer.WriteIntL( EAlfSetPenStyle, aPenStyle );
#endif
    }

// ---------------------------------------------------------------------------
// SetPenSize
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetPenSize(const TSize& aSize)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    if (aSize == iPenSize)
        return;
#endif    
    if ( aSize.iWidth == aSize.iHeight ) 
        {
        // this is supported only by bitgdi
        iSendBuffer.SetSupportedCommand( EAlfSetPenSize, 0 ); // All renderer support this
        }
    else
        {
        // 63 = binary 0b0111111, not supported by any renderer
        iSendBuffer.SetSupportedCommand( EAlfSetPenSize, KNoSupportInAnyRenderer );
        }
    iPenSize = aSize;
    iSendBuffer.WriteIntsL( EAlfSetPenSize, 2, aSize.iWidth, aSize.iHeight );
	}

// ---------------------------------------------------------------------------
// SetTextShadowColor
// call to SetShadowColor
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetTextShadowColor(const TRgb& aColor)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    if (iTextShadowColor == aColor)
        return;
#endif
    iTextShadowColor = aColor;
    iSendBuffer.WriteIntsL( EAlfSetTextShadowColor, 1 , aColor.Internal());
    }

// ---------------------------------------------------------------------------
// SetCharJustification
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetCharJustification(TInt aExcessWidth, TInt aNumChars)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    if (aExcessWidth == iCharJustExcess && aNumChars == iCharJustNum)
        return;
#endif
    iCharJustExcess = aExcessWidth;
    iCharJustNum = aNumChars;
    iSendBuffer.WriteIntsL( EAlfSetCharJustification, 2, aExcessWidth, aNumChars );
    }

// ---------------------------------------------------------------------------
// SetWordJustification
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetWordJustification(TInt aExcessWidth, TInt aNumGaps)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    if (aExcessWidth == iWordJustExcess && aNumGaps == iWordJustNum)
        return;
#endif
    iWordJustExcess = aExcessWidth;
    iWordJustNum = aNumGaps;
    iSendBuffer.WriteIntsL( EAlfSetCharJustification, 2, aExcessWidth, aNumGaps );
    }

// ---------------------------------------------------------------------------
// SetUnderlineStyle
// SetUnderlineStyle(MWsGraphicsContextToBitGdiMappings::Convert(
//    (MWsGraphicsContext::TFontUnderline)value))
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetUnderlineStyle(TFontUnderline aUnderlineStyle)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    if (aUnderlineStyle == iUnderline)
        return;
#endif
    iUnderline = aUnderlineStyle;
    iSendBuffer.WriteIntsL( EAlfSetUnderlineStyle, 1, aUnderlineStyle );
    }

// ---------------------------------------------------------------------------
//  SetStrikethroughStyle
//  calls SetStrikethroughStyle(MWsGraphicsContextToBitGdiMappings::Convert(
//  (MWsGraphicsContext::TFontStrikethrough)value));
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    if (aStrikethroughStyle == iStrikethrough)
        return;
#endif
    iStrikethrough = aStrikethroughStyle;	
    iSendBuffer.WriteIntsL( EAlfSetStrikethroughStyle, 1, aStrikethroughStyle );
    }

// ---------------------------------------------------------------------------
// SetBrushPattern
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetBrushPattern(const CFbsBitmap& aBitmap)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    if ( iBrushPattern == iPreviousBrushPattern )
        {
        return;
        }
#endif    
    iBrushPatternSet = ETrue;
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    iDelayedContextState |= EModifiedBrushPattern1;
    iBrushPattern = aBitmap.Handle();
#else
    iSendBuffer.WriteIntsL( EAlfSetBrushPattern, 1, aBitmap.Handle() );
#endif
    }

// ---------------------------------------------------------------------------
// SetBrushPattern
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetBrushPattern(TInt aFbsBitmapHandle)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    if ( iBrushPattern == iPreviousBrushPattern )
        {
        return;
        }
#endif    
    iBrushPatternSet = ETrue;
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    iDelayedContextState |= EModifiedBrushPattern2;
    iBrushPattern = aFbsBitmapHandle;
#else
    iSendBuffer.WriteIntsL( EAlfSetBrushPattern, 1, aFbsBitmapHandle );
#endif
    }

// ---------------------------------------------------------------------------
// SetFont
// calls UseFont
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetFont( const CFont* aFont)
    {
    const CFbsFont* font = static_cast<const CFbsFont*>(aFont);
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    if (iFontHandle == font->Handle())
        return;	
#endif
    iFontHandle = font->Handle();

#ifdef _OPTIMIZE_WS_COMMANDS_FONT_
    iDelayedContextState &= ~EModifiedResetFont;
    iDelayedContextState |= EModifiedSetFontNoDuplicate;
#else        
    iSendBuffer.WriteIntsL( EAlfSetFont, 1, font->Handle() );
#endif
    }

// ---------------------------------------------------------------------------
// CopyRect
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::CopyRect(
        const TPoint& aOffset, 
        const TRect& aRect )
	{
	iSendBuffer.WriteIntsL( EAlfCopyRect, 6,
	        aOffset.iX,
	        aOffset.iY,
	        aRect.iTl.iX,
	        aRect.iTl.iY,
	        aRect.iBr.iX,
	        aRect.iBr.iY );
	}

// ---------------------------------------------------------------------------
// UpdateJustification
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::UpdateJustification(
        const TDesC& aText,
        const TTextParameters* aParam)
    {
    // Temp is not written to the stream, because 0 parameter is given to WriteDescriptorAndIntsL as TInt count.
    // This is simply to avoid creating yet another WriteDescriptorL method. 
    TInt notUsed; 
    iSendBuffer.WriteDescriptorAndIntsL( EAlfUpdateJustification, aText, (CGraphicsContext::TTextParameters*)aParam, 0, notUsed );
    }

// ---------------------------------------------------------------------------
// UpdateJustificationVertical
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::UpdateJustificationVertical(
        const TDesC& aText,
        const TTextParameters* aParam,
        TBool aUp )
    {
    iSendBuffer.WriteDescriptorAndIntsL( EAlfUpdateJustificationVertical, aText, (CGraphicsContext::TTextParameters*)aParam, 1, aUp );
    }

// ---------------------------------------------------------------------------
// SetFontNoDuplicate
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetFontNoDuplicate(const CFont* aFont)
    {
    const CFbsFont* font = static_cast<const CFbsFont*>(aFont);
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_    
    if (iFontHandle == font->Handle())
        return;	
#endif
    iFontHandle = font->Handle();

#ifdef  _OPTIMIZE_WS_COMMANDS_FONT_
    iDelayedContextState &= ~EModifiedResetFont;
    iDelayedContextState |= EModifiedSetFontNoDuplicate;
#else    
    iSendBuffer.WriteIntsL( EAlfSetFontNoDuplicate, 1, font->Handle() );
#endif

    }

// ---------------------------------------------------------------------------
// HasBrushPattern
// ---------------------------------------------------------------------------
//
TBool CAlfGraphicsContext::HasBrushPattern() const
	{
	return iBrushPatternSet;
	}

// ---------------------------------------------------------------------------
// HasFont
// ---------------------------------------------------------------------------
//
TBool CAlfGraphicsContext::HasFont() const
	{
	return iFontHandle;
	}

// ---------------------------------------------------------------------------
// InternalizeL
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::InternalizeL(RReadStream& /*aReadStream*/)
	{
	//iContext->InternalizeL(aReadStream);
	}

// ---------------------------------------------------------------------------
// ExternalizeL
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::ExternalizeL(RWriteStream& /*aWriteStream*/)
	{
	//iContext->ExternalizeL(aWriteStream);
	}

// ---------------------------------------------------------------------------
// BrushColor
// ---------------------------------------------------------------------------
//
TRgb CAlfGraphicsContext::BrushColor() const
	{
	return iBrushColor;
	}

// ---------------------------------------------------------------------------
// PenColor
// ---------------------------------------------------------------------------
//
TRgb CAlfGraphicsContext::PenColor() const
	{
	return iPenColor;
	}

// ---------------------------------------------------------------------------
// TextShadowColor
// ---------------------------------------------------------------------------
//
TRgb CAlfGraphicsContext::TextShadowColor() const
	{
    return iTextShadowColor;
	}

// ---------------------------------------------------------------------------
// CopySettings
// Copies all settings from the specified bitmap graphics context. 
// const CFbsBitGc &aGc The bitmap graphics context whose settings are to be copied.
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::CopySettings(const MWsGraphicsContext& /*aGc*/)
	{
	// NOP
	}

// ---------------------------------------------------------------------------
//  SetFaded
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetFaded(TBool /*aFaded*/)
	{
	}

// ---------------------------------------------------------------------------
// SetFadingParameters
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetFadingParameters(TUint8 /*aBlackMap*/, TUint8 /*aWhiteMap*/)
	{
	}

// ---------------------------------------------------------------------------
// FadeArea
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::FadeArea(const TRegion* /*aRegion*/ )
    {
	}

// ---------------------------------------------------------------------------
// This is not implemented by hitchcock
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::MapColors(    
        const TRect &/*aRect*/, 
        const TRgb */*aColors*/, 
        TInt /*aNumPairs*/, 
        TBool /*aMapForwards*/)
	{
	}

// ---------------------------------------------------------------------------
// SetUserDisplayMode
// ---------------------------------------------------------------------------
//
void CAlfGraphicsContext::SetUserDisplayMode(TDisplayMode aDisplayMode)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
    if (aDisplayMode == iUserDisplayMode)
        return;
#endif
    iUserDisplayMode = aDisplayMode;
    iSendBuffer.WriteIntsL( EAlfSetUserDisplayMode, 1, aDisplayMode );
    }

// ---------------------------------------------------------------------------
// UseFont
// hes value locally
// @todo Check Symbian reference imnplementation for return value
// ---------------------------------------------------------------------------
//
TInt CAlfGraphicsContext::UseFont(TInt aFontHandle)
    {
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_   
    if (iFontHandle == aFontHandle)
        return KErrNone;	
#endif
    iFontHandle = aFontHandle;
    iSendBuffer.WriteIntsL( EAlfUseFont, 1, aFontHandle );
    // TODO: DANGER!! What should this in REALITY return?
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Origin
// ---------------------------------------------------------------------------
//
TPoint CAlfGraphicsContext::Origin() const
    {
    return iOrigin;
    }

#ifdef _OPTIMIZE_WS_COMMANDS_ADVANCED_
// ---------------------------------------------------------------------------
// CheckDelayedClipping
// ---------------------------------------------------------------------------
//
TBool CAlfGraphicsContext::CheckDelayedClipping( TRect aDrawRect )
    {
    iDelayedClippingRegion.Copy( iClippingRegion );
    aDrawRect.Move( iOrigin );
    iDelayedDrawRegion.Clear();
    iDelayedDrawRegion.AddRect( aDrawRect );
    iDelayedDrawRegion.Intersect( iDelayedClippingRegion );

    if ( iDelayedContextState & EModifiedClipRect 
            && iDelayedDrawRegion.Count() == 1 && iDelayedDrawRegion[0] == aDrawRect )
        {
        iDelayedContextState &= ~EModifiedClipRect;
        iRollback |= ERollbackClippingState;
        return ETrue;
        }

    if ( iDelayedDrawRegion.IsEmpty() && iClippingRegion.Count() )
        {
        return EFalse;
        }
    return ETrue;
    }
#endif

// ---------------------------------------------------------------------------
// FlushStateL
// ---------------------------------------------------------------------------
//
#ifdef _OPTIMIZE_WS_COMMANDS_BASIC_
void CAlfGraphicsContext::FlushStateL( TInt& aRollbackClippingState )
    {
    /// Pen
    if ( iDelayedContextState & EModifiedPenStyle 
            && iPenStyle != iPreviousPenStyle )
        {
        iSendBuffer.WriteIntL( EAlfSetPenStyle, iPenStyle );
        iPreviousPenStyle = iPenStyle;
        }
    if ( iDelayedContextState & EModifiedPenColor 
            && iPenColor != iPreviousPenColor )
        {
        iSendBuffer.WriteIntsL( EAlfSetPenColor, 1, iPenColor.Internal());
        iPreviousPenColor = iPenColor;
        }

    // Brush - Brush style must be set before any other brush related settings.
    if ( iDelayedContextState & EModifiedBrushStyle 
            && iBrushStyle != iPreviousBrushStyle )
        {
        iSendBuffer.WriteIntL( EAlfSetBrushStyle, iBrushStyle );
        iPreviousBrushStyle = iBrushStyle;
        }    

    if ( iDelayedContextState & EModifiedBrushPattern1 
            && iBrushStyle != iPreviousBrushStyle )
        {
        iSendBuffer.WriteIntL( EAlfSetBrushPattern, iBrushPattern );
        iPreviousBrushPattern = iBrushPattern;
        }

    if ( iDelayedContextState & EModifiedBrushPattern2 
            && iBrushStyle != iPreviousBrushStyle )
        {
        iSendBuffer.WriteIntL( EAlfSetBrushPattern2, iBrushPattern );
        iPreviousBrushPattern = iBrushPattern;
        }

    if ( iDelayedContextState & EModifiedBrushColor 
            && iBrushColor != iPreviousBrushColor )
        {
        iSendBuffer.WriteIntsL( EAlfSetBrushColor, 1, iBrushColor.Internal());
        iPreviousBrushColor = iBrushColor;               
        }
#ifdef  _OPTIMIZE_WS_COMMANDS_ADVANCED_
    // Clipping rect
    if ( iDelayedContextState & EModifiedClipRect 
            && !( iPreviousClippingRegion.Count() == 1 && iClippingRegion.Count() == 1 
                    && iPreviousClippingRegion[0] == iClippingRegion[0] ))
        {
        iSendBuffer.WriteRegionL( EAlfSetClippingRegion, iClippingRegion );
        iPreviousClippingRegion.Copy( iClippingRegion );
        }
    if ( iDelayedContextState & EModifiedClipReset )
        {
        iSendBuffer.WriteCommandL( EAlfResetClippingRegion );
        } 
#endif
#ifdef  _OPTIMIZE_WS_COMMANDS_FONT_
    // font
    if ( iDelayedContextState & EModifiedSetFontNoDuplicate 
            && iFontHandle != iPreviousFontHandle )
        {
        iSendBuffer.WriteIntsL( EAlfSetFontNoDuplicate, 1, iFontHandle );
        iPreviousFontHandle = iFontHandle;
        }
    if ( iDelayedContextState & EModifiedResetFont )
        {
        iSendBuffer.WriteCommandL( EAlfResetFont );
        }
#endif
    iDelayedContextState = 0;

#ifdef  _OPTIMIZE_WS_COMMANDS_ADVANCED_
    if ( aRollbackClippingState & ERollbackClippingState )
        {
        iDelayedContextState |= EModifiedClipRect;
        aRollbackClippingState &= ~ERollbackClippingState;
        }
#endif    
    }
#endif
