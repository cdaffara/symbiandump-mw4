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

#ifndef HUICMDBUFFERBRUSH_H_
#define HUICMDBUFFERBRUSH_H_

#include <uiacceltk/HuiBrush.h>
#include "huicanvaspainter.h"
#include "huieffectable.h"

// TASK: 1) Avkon provides markers in buffers to mark highlight
// TASK: 2) fxml provides names for visual buffers -- snapshot of buffers in different times
// TASK: 3) 

// TODO: Support for effects in the brush
// TODO: Support for parsing buffers in visual and passing it to cmdbufferbrushes
// TODO: Support for drawing CmdBufferBrush
// TODO: HuiCanvasPainter stores commandbuffers -- create links to those buffers.
// 
class CHuiFxEffect;
class CHuiBrushArray;
class CHuiCanvasGc;
class CHuiCanvasWsGc;
class CHuiCanvasVisual;
class CHuiCanvasPainter;
class CHuiGc;


class CHuiCmdBufferBrush : public CHuiBrush, public MHuiEffectable, public MHuiCmdBufDestroyObserver
{
public:
   struct TLayeredBuffer
       {
       //TInt iBufferId; // which visual's buffer this is.
       TRect iBoundingRectangle;
       //TRegion iUpdateRegion;
       TPtrC8 iBufferPart; // range from huicanvaspainter's buffer
       CHuiCanvasCommandBuffer *iCommandBuffer; // reference to painter's command buffer
       TLayeredBuffer &operator=(const TLayeredBuffer &aVal)
           {
           //iBufferId = aVal.iBufferId;
           iBoundingRectangle = aVal.iBoundingRectangle;
           // iUpdateRegion = aVal.iUpdateRegion;
           iBufferPart.Set(aVal.iBufferPart);
           iCommandBuffer = aVal.iCommandBuffer;
           return *this;
           }
       };

public: // construction
    static CHuiCmdBufferBrush *NewL(TInt aLayerId, CHuiCanvasGc &aGc, CHuiCanvasWsGc &aWsGc, CHuiCanvasVisual &aVisual, CHuiCanvasPainter &aPainter);
    ~CHuiCmdBufferBrush();
public: // modifying buffers
   void AddBufferL(TLayeredBuffer aBuffer);

public: // Accessors
    TInt LayerId() const { return iLayerId; }

public: // from CHuiBrush
    virtual void Draw(CHuiGc& aGc, const MHuiBrushGuide& aGuide) const;
    void DrawSelf(CHuiGc &aGc, THuiRealRect aRect) const;
    virtual TInt Type2() const;
    void Flush() const;
    void ClearBackground(CHuiGc &aGc, TRect aRectangle) const;
    void DrawDrawingCommands(TInt aAction, CHuiGc &aGc, TRect aClippingRectangle, 
                             TPtrC8 aCommands, CHuiCanvasCommandBuffer *buf, const TRegion &aRegion) const;
    void CopyBuffer(CHuiGc &aGc, TRect aRectangle) const;


public: // from MHuiEffectable
   //void SetEffect(CHuiFxEffect* aEffect);
   void EffectSetEffect(CHuiFxEffect* aEffect);
   TReal32 EffectOpacityTarget() const;
   void EffectSetOpacity(TReal32 aOpacity);
   void EffectDrawSelf( CHuiGc &aGc, const TRect & aDisplayRect) const;
   THuiRealRect EffectDisplayRect() const __SOFTFP;
   void SetLoadingEffect(TBool aLoading);
   void EffectSetSource( TBool aIsInput1 );
   TBool EffectGetSource() const;

   
public: // from MHuiCmdBufDestroyObserver
    void DestroyBuffer(CHuiCanvasCommandBuffer *aBuffer);
private:
    void ConstructL(TInt aLayerId, CHuiCanvasVisual &aVisual, CHuiCanvasPainter &aPainter);
    CHuiCmdBufferBrush( CHuiCanvasGc &aGc, CHuiCanvasWsGc &aWsGc);
private:
    TInt iLayerId; // layer id of this brush
    RArray<TLayeredBuffer> iBuffers; // pointers to buffer subareas
private:
    CHuiFxEffect *iEffect;
    CHuiCanvasGc &iCanvasGc;
    CHuiCanvasWsGc &iCanvasWsGc;
    CHuiCanvasVisual *iVisual;
    CHuiCanvasPainter *iCanvasPainter;
    friend void ParseCmdBufferL(CHuiBrushArray &aBrushes, TPtrC8 aBuffer, TPtrC8 aIndexTable, CHuiCanvasGc &aGc, CHuiCanvasWsGc &aWsGc, CHuiCanvasVisual &aVisual,CHuiCanvasPainter &aPainter);

};
void ParseCmdBufferL(CHuiBrushArray &aBrushes, TPtrC8 aBuffer, TPtrC8 aIndexTable, CHuiCanvasGc &aGc, CHuiCanvasWsGc &aWsGc, CHuiCanvasVisual &aVisual, CHuiCanvasPainter &aPainter);

#endif
