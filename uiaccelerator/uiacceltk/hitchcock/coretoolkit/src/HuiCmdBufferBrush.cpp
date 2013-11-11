/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   
*
*/

#include "HuiCmdBufferBrush.h"
#include "HuiFxEffect.h"
#include <uiacceltk/HuiBrushArray.h>
#include "huicanvasgc.h"
#include "huicanvaswsgc.h"
#include "huicanvaswshwgc.h"
#include "huicanvaspainter.h"
#include "huicanvaswspainter.h"
CHuiCmdBufferBrush *CHuiCmdBufferBrush::NewL(TInt aLayerId, CHuiCanvasGc &aGc, CHuiCanvasWsGc &aWsGc, CHuiCanvasVisual &aVisual, CHuiCanvasPainter &aPainter)
    {
    CHuiCmdBufferBrush *brush = new (ELeave) CHuiCmdBufferBrush(aGc, aWsGc);
    CleanupStack::PushL(brush);
    brush->ConstructL(aLayerId, aVisual, aPainter);
    CleanupStack::Pop(brush);
    return brush;
    }
CHuiCmdBufferBrush::CHuiCmdBufferBrush( CHuiCanvasGc &aGc, CHuiCanvasWsGc &aWsGc )
    : iCanvasGc(aGc), iCanvasWsGc(aWsGc)
    {
    }


void CHuiCmdBufferBrush::ConstructL(TInt aLayerId, CHuiCanvasVisual &aVisual, CHuiCanvasPainter &aPainter)
    {
    iLayerId = aLayerId;
    iVisual = &aVisual;
    iCanvasPainter = &aPainter;
    }
CHuiCmdBufferBrush::~CHuiCmdBufferBrush()
    {
    TInt count = iBuffers.Count();
    for(int i=0;i<count;i++)
        {
        CHuiCanvasCommandBuffer *buf = iBuffers[i].iCommandBuffer;
        buf->RemoveDestroyObserver(this);
        }
    }
TInt CHuiCmdBufferBrush::Type2() const { return ECmdBufferBrush; }

void CHuiCmdBufferBrush::AddBufferL(TLayeredBuffer aBuffer)
    {
    iBuffers.AppendL(aBuffer);
    }


void CHuiCmdBufferBrush::Draw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const
    {
    THuiRealRect brushRect = aGuide.BrushRect();
    DrawSelf(aGc, brushRect);
    }
void CHuiCmdBufferBrush::DrawSelf(CHuiGc &aGc, THuiRealRect brushRect) const
{    
    iCanvasGc.SetVisual(*iVisual);
    iCanvasGc.SetGc(aGc);
    iCanvasGc.SetDefaults();   
    //iCanvasPainter->EnableRenderBuffer(ETrue);
    //static_cast<CHuiCanvasWsPainter*>(iCanvasPainter)->EnableRenderBufferIfNeeded(ETrue);

    TRect displayRect = brushRect.Round();
    
    RRegion reg;
    TInt size = iBuffers.Count();
    for(int i=0;i<size;i++)
        {
        TRect rect = iBuffers[i].iBoundingRectangle;

        RDebug::Print(_L("TP: ClearBackground %d"), iLayerId);
        ClearBackground(aGc, rect);

        rect.iTl.iX += brushRect.iTl.iX;
        rect.iTl.iY += brushRect.iTl.iY;
        rect.iBr.iX += brushRect.iTl.iX;
        rect.iBr.iY += brushRect.iTl.iY;
        reg.AddRect(rect);
        }
    iCanvasWsGc.BeginActionL(EDrawBuffer,reg.BoundingRect() /*displayRect*/,*iVisual,EFalse,reg);                   

    for(int i2=0;i2<size;i2++)
        {
        TRect rect = iBuffers[i2].iBoundingRectangle;

        rect.iTl.iX += brushRect.iTl.iX;
        rect.iTl.iY += brushRect.iTl.iY;
        rect.iBr.iX += brushRect.iTl.iX;
        rect.iBr.iY += brushRect.iTl.iY;

        RRegion region;
        region.AddRect(rect);
        for(int ii2 = i2+1;ii2<size; ii2++)
            {
            TRect rect2 = iBuffers[ii2].iBoundingRectangle;

            rect2.iTl.iX += brushRect.iTl.iX;
            rect2.iTl.iY += brushRect.iTl.iY;
            rect2.iBr.iX += brushRect.iTl.iX;
            rect2.iBr.iY += brushRect.iTl.iY;
            if (iBuffers[i2].iCommandBuffer != iBuffers[ii2].iCommandBuffer)
                {
                region.SubRect(rect2);
                }
            }
        region.Intersect(iBuffers[i2].iCommandBuffer->iUpdateRegion);
            
        if (!region.IsEmpty())
            {
            
            //iCanvasWsGc.BeginActionL(EScanBuffer,rect /*displayRect*/,*iVisual,EFalse,region);                   
            //DrawDrawingCommands(EScanBuffer, aGc, rect, iBuffers[i2].iBufferPart, iBuffers[i2].iCommandBuffer, region);
            //iCanvasWsGc.EndActionL(region);                 

            DrawDrawingCommands(EDrawBuffer, aGc, rect, iBuffers[i2].iBufferPart, iBuffers[i2].iCommandBuffer, region);
            }
        region.Close();
        }
    {
    TInt flags = iVisual->Flags();
    iVisual->ClearFlag(EHuiVisualFlagOpaqueHint);
    iCanvasWsGc.EndActionL(reg, ETrue);                     
    iVisual->SetFlags(flags);
    }

    reg.Close();
    
#if 0
    for(int i3=0;i3<size;i3++)
        {
        TRect rect = iBuffers[i3].iBoundingRectangle;
        rect.iTl.iX += brushRect.iTl.iX;
        rect.iTl.iY += brushRect.iTl.iY;
        rect.iBr.iX += brushRect.iTl.iX;
        rect.iBr.iY += brushRect.iTl.iY;
        CopyBuffer(aGc, rect);
        }
#endif

    }

void CHuiCmdBufferBrush::ClearBackground(CHuiGc &aGc, TRect aRectangle) const
    {
    //void DrawRects(RArray<THuiRealRect>& aRects);
#if 0
    THuiRealPoint tl(aRectangle.iTl.iX, aRectangle.iTl.iY);
    THuiRealPoint br(aRectangle.iBr.iX, aRectangle.iBr.iY);
    THuiRealRect rect(tl, br);
    TReal32 opacity = iCanvasGc.Opacity();
    iCanvasGc.SetOpacity(0.0f);
    iCanvasGc.Clear(rect);
    iCanvasGc.SetOpacity(opacity);
#endif

    CHuiCanvasRenderBuffer* renderbuffer = NULL;
    renderbuffer = iVisual->Env().CanvasTextureCache().FindCachedRenderBuffer(*iVisual);
    if (renderbuffer)
        {
        iCanvasGc.ClearRenderBuffer(*renderbuffer, aRectangle /*TRect(0,0,iVisual->DisplayRect().Size().iWidth, iVisual->DisplayRect().Size().iHeight)*/);
        }
    }
void CHuiCmdBufferBrush::DrawDrawingCommands(TInt aAction, CHuiGc &aGc, TRect aClippingRectangle, 
                                             TPtrC8 aCommands, CHuiCanvasCommandBuffer *buf, const TRegion &reg) const
    {
    TRect rect = aClippingRectangle;
    RDebug::Print(_L("TP: DrawDrawingCommands (%d,%d)-(%d,%d)"), rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY);    

    //iCanvasWsGc.EnableUpdateRegion(reg /*buf->iUpdateRegion*/, EFalse /*aClearBeforeHandlingBuffers*/);

    TRAP_IGNORE(iCanvasPainter->HandleBufferL(rect, aAction, *iVisual, &aGc, TPoint(0,0), aCommands, buf));  
    //iCanvasWsGc.DisableUpdateRegion();

    }
void CHuiCmdBufferBrush::CopyBuffer(CHuiGc &aGc, TRect aRectangle) const
    {
    RDebug::Print(_L("TP: CopyBuffer"));    
    }

void CHuiCmdBufferBrush::Flush() const
    {
    
    }

void CHuiCmdBufferBrush::EffectSetEffect(CHuiFxEffect* aEffect)
    {
    if (iEffect)
        {
        iEffect->SetEffectEndObserver( NULL, 0 );
        }
    delete iEffect;
    iEffect = aEffect;
    }

TReal32 CHuiCmdBufferBrush::EffectOpacityTarget() const
    {
    return 1.0;
    }
void CHuiCmdBufferBrush::EffectSetOpacity(TReal32 aOpacity)
    {
    // empty
    }
void CHuiCmdBufferBrush::EffectDrawSelf( CHuiGc &aGc, const TRect & aDisplayRect) const
    {
    DrawSelf(aGc, aDisplayRect);
    }

THuiRealRect CHuiCmdBufferBrush::EffectDisplayRect() const __SOFTFP
    {
    RRegion reg;
    TInt size = iBuffers.Count();
    for(int i=0;i<size;i++)
        {
        TRect rect = iBuffers[i].iBoundingRectangle;
        reg.AddRect(rect);
        }
    
    THuiRealRect r(reg.BoundingRect());
    reg.Close();
    return r;
    }

void CHuiCmdBufferBrush::SetLoadingEffect(TBool /*aLoading*/)
    {
    // TODO
    }
void CHuiCmdBufferBrush::EffectSetSource( TBool /*aIsInput1*/ )
    {
    }
TBool CHuiCmdBufferBrush::EffectGetSource() const
    {
    return EFalse;
    }

void CHuiCmdBufferBrush::DestroyBuffer(CHuiCanvasCommandBuffer *aBuffer)
    {
    TInt count = iBuffers.Count();
    for(int i=count-1;i>=0;i--)
        {
        CHuiCanvasCommandBuffer *buf = iBuffers[i].iCommandBuffer;
        if (buf == aBuffer)
            {
            iBuffers.Remove(i);
            }
        }
    }

struct TIndexTableElement
    {
    TInt32 iOffset;
    TInt32 iSize;
    TInt32 iLayerId;
    //TRect iBoundingRect;
    TInt iTliX;
    TInt iTliY;
    TInt iBriX;
    TInt iBriY;
    //TInt32 iBufferId;
    };

void ReplaceBufferL(RArray<CHuiCmdBufferBrush::TLayeredBuffer> &aBuffers, const CHuiCmdBufferBrush::TLayeredBuffer &buf);
TInt FindBuffer(CHuiBrushArray &aBrushes, TInt aLayerId);


void ParseCmdBufferL(CHuiBrushArray &aBrushes, TPtrC8 aBuffer, TPtrC8 aIndexTable, CHuiCanvasGc &aGc, CHuiCanvasWsGc &aWsGc, CHuiCanvasVisual &aVisual, CHuiCanvasPainter &aPainter)
    {
    TInt32 numIndexes = aIndexTable.Length() / sizeof(TIndexTableElement);
    for(TInt i=0;i<numIndexes;i++)
        {
        TInt32 offset = i * sizeof(TIndexTableElement);
        TInt32 size = sizeof(TIndexTableElement);
        TPtrC8 ptr;
        ptr.Set(aIndexTable.Mid(offset, size));
        TIndexTableElement *elem = (TIndexTableElement*)ptr.Ptr();
        //RDebug::Print(_L("Layer: %d: ID: %d, offset=%d, size = %d"), i, elem->iLayerId, elem->iOffset, elem->iSize);
        //RDebug::Print(_L("aBuffer.Length()=%d"), aBuffer.Length());
        TPtrC8 ptr2(aBuffer.Ptr()+elem->iOffset, elem->iSize) ; // aBuffer.Mid(elem->iOffset, elem->iSize));
        //ptr2.Set();
	
        TInt index = FindBuffer(aBrushes, elem->iLayerId);
        if (index == -1)
            {
            CHuiCmdBufferBrush *brush2 = CHuiCmdBufferBrush::NewL(elem->iLayerId, aGc, aWsGc, aVisual, aPainter);
            CleanupStack::PushL(brush2);
            aBrushes.AppendL(brush2, EHuiHasOwnership);
            CleanupStack::Pop();
            brush2->SetLayer(EHuiBrushLayerForeground);
            index = aBrushes.Count() - 1;
            }
        CHuiCmdBufferBrush &brush = (CHuiCmdBufferBrush&)aBrushes[index];
        CHuiCmdBufferBrush::TLayeredBuffer buf;
        //buf.iBufferId = elem->iBufferId;
        //TODO buf.iUpdateRegion = ...
        TInt count = aPainter.NumBuffers();
        CHuiCanvasCommandBuffer *buffer = count ? aPainter.At(count-1) : NULL;
        if (buffer)
            {
            buffer->AddDestroyObserver(&brush);
            }
        buf.iBoundingRectangle.SetRect(elem->iTliX, elem->iTliY, elem->iBriX, elem->iBriY);
        if (buf.iBoundingRectangle == TRect(0,0,0,0))
            {
            buf.iBoundingRectangle = TRect(0,0,aVisual.BrushRect().Size().iWidth, aVisual.BrushRect().Size().iHeight);
            }
        buf.iBufferPart.Set(ptr2);
        buf.iCommandBuffer = buffer;
        ReplaceBufferL(brush.iBuffers, buf);
        }
    }

void ReplaceBufferL(RArray<CHuiCmdBufferBrush::TLayeredBuffer> &aBuffers, const CHuiCmdBufferBrush::TLayeredBuffer &aBuf)
    {
    aBuffers.AppendL(aBuf);
#if 0
    // TODO: Dunno if iBufferId can be used or whether we need to use regions to
    // identify buffers.
    TInt size = aBuffers.Count();
    TInt index = -1;
    for(int i=0;i<size;i++)
        { // this loop might be able to optimize away if it becomes performance problem.
          // but would need to notice that ParseCmdBuffer always goes layers in order.
          // current code has no such assumption.
        CHuiCmdBufferBrush::TLayeredBuffer &buffer= aBuffers[i];
        if (buffer.iBufferId == buf.iBufferId)
            {
            index = i;
            break;
            }
        }
    if (index != -1)
        {
        aBuffers[index] = buf;
        }
    else
        {
        aBuffers.AppendL(buf);
        }
#endif
    }

TInt FindBuffer(CHuiBrushArray &aBrushes, TInt aLayerId)
    {
    TInt size = aBrushes.Count();
    for(int i=0;i<size;i++)
        {
        CHuiCmdBufferBrush *brush = dynamic_cast<CHuiCmdBufferBrush*>(&aBrushes[i]);
        if (brush && brush->LayerId() == aLayerId)
            {
            return i;
            }
        }
    return -1;
    }

// wspainter needs to be able to skip the index table command
void SkipCommand()
    {
    
    }
