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



#include "HuiFxGroupLayer.h"

EXPORT_C CHuiFxGroupLayer::CHuiFxGroupLayer()
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxGroupLayer::CHuiFxGroupLayer - 0x%x "), this);
#endif
    }

EXPORT_C CHuiFxGroupLayer::~CHuiFxGroupLayer()
    {
    /*for (TInt i = 0; i < iLayers.Count(); i++)
        {
        delete iLayers[i];
        }
    iLayers.Close();*/
    iLayers.ResetAndDestroy();
    iLayers.Close();
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxGroupLayer::~CHuiFxGroupLayer - 0x%x "), this);
#endif
    }

EXPORT_C CHuiFxGroupLayer* CHuiFxGroupLayer::NewL(TBool aIsRoot)
    {
    CHuiFxGroupLayer* e = new (ELeave) CHuiFxGroupLayer();
    CleanupStack::PushL(e);
    e->ConstructL(aIsRoot);
    CleanupStack::Pop(e);
    return e;
    }

EXPORT_C CHuiFxGroupLayer *CHuiFxGroupLayer::CloneL() const
{
   CHuiFxGroupLayer *gl = new (ELeave)CHuiFxGroupLayer;
   gl->CHuiFxLayer::CopyFromL(this);
   for(int i=0;i<iLayers.Count();i++)
       {
       CHuiFxLayer *layer = iLayers[i];
       CHuiFxLayer *layer2 = layer->CloneL();
       gl->iLayers.AppendL(layer2);
       }
   gl->iIsRoot = iIsRoot;
   gl->iBackbufferRect = iBackbufferRect;
   return gl;
}
EXPORT_C void CHuiFxGroupLayer::SetExtRect( TRect *aExtRect )
    {
    for(int i=0;i<iLayers.Count();i++)
        {
        CHuiFxLayer *layer = iLayers[i];
        layer->SetExtRect( aExtRect );
        }
    }
EXPORT_C void CHuiFxGroupLayer::SetVisual( CHuiVisual *aVisual )
    {
    for(int i=0;i<iLayers.Count();i++)
        {
        CHuiFxLayer *layer = iLayers[i];
        layer->SetVisual(aVisual);
        }
    }
EXPORT_C void CHuiFxGroupLayer::SetVisual( MHuiEffectable *aVisual )
    {
    for(int i=0;i<iLayers.Count();i++)
        {
        CHuiFxLayer *layer = iLayers[i];
        layer->SetVisual(aVisual);
        }    
    }
EXPORT_C void CHuiFxGroupLayer::ConstructL(TBool aIsRoot)
    {
    CHuiFxLayer::ConstructL( ELayerTypeGroup );
    iIsRoot = aIsRoot;
    }

EXPORT_C TBool CHuiFxGroupLayer::PrepareDrawL(CHuiFxEngine& aEngine)
    {
    // TODO: fast path
    if (!VisualArea(iBackbufferRect))
        {
        ApplyMargin( iBackbufferRect );

        // Clip to display area (if set) 
        iBackbufferRect.Intersection(iDisplayArea);

        SetVisualRect(iBackbufferRect);
        //iBackbufferRect = TargetRect();
        }
    else
        {
        ApplyMargin( iBackbufferRect );
        // Restrict rendering to the output area

        // Clip to display area (if set) 
        iBackbufferRect.Intersection(iDisplayArea);
        
        SetVisualRect(iBackbufferRect);
        //iBackbufferRect.Intersection(TargetRect());
        }

    TRect targetRect;
    TRect sourceRect;

    if (!iIsRoot)
        {
        sourceRect = targetRect = TRect(TPoint(0, 0), iBackbufferRect.Size());
        }
    else
        {
        sourceRect = targetRect = iBackbufferRect;
        }

    TRect newBackbufferRect(0,0,0,0);
    for (TInt i = 0; i < iLayers.Count(); i++)
        {
        iLayers[i]->SetDisplayArea(iDisplayArea);
        
        // Should we shrink the visual area?
        if (i > 0 && iLayers[i]->VisualArea(newBackbufferRect))
            {
            ApplyMargin( newBackbufferRect );
            SetVisualRect(newBackbufferRect);
            //newBackbufferRect.Intersection(TargetRect());
            
            // Clip to display area (if set) 
            newBackbufferRect.Intersection(iDisplayArea);
            
            ASSERT(newBackbufferRect.Size().iWidth  <= iBackbufferRect.Size().iWidth &&
                   newBackbufferRect.Size().iHeight <= iBackbufferRect.Size().iHeight);
            if (iIsRoot)
                {
                sourceRect = targetRect = newBackbufferRect;
                }
            else
                {
                targetRect.iTl.iX = newBackbufferRect.iTl.iX - iBackbufferRect.iTl.iX;
                targetRect.iTl.iY = newBackbufferRect.iTl.iY - iBackbufferRect.iTl.iY;
                targetRect.iBr.iX = newBackbufferRect.iBr.iX - iBackbufferRect.iTl.iX;
                targetRect.iBr.iY = newBackbufferRect.iBr.iY - iBackbufferRect.iTl.iY;
                sourceRect = targetRect;
                }
            }
        iLayers[i]->SetSourceRect(sourceRect);
        iLayers[i]->SetTargetRect(targetRect);
        iLayers[i]->SetVisualRect(VisualRect());
        if (!iLayers[i]->PrepareDrawL(aEngine))
            {
            return EFalse;
            }
        }
    return ETrue;
    }

EXPORT_C void CHuiFxGroupLayer::Draw(CHuiFxEngine& aEngine, CHuiGc& aGc, CHuiFxRenderbuffer& aTarget, 
                                     CHuiFxRenderbuffer& aSource, TBool aHasSurface)
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxGroupLayer::Draw - 0x%x "), this);
#endif
    // TODO: fast path
    CHuiFxRenderbuffer* backBuffer = &aTarget;
    CHuiFxRenderbuffer* sourceBuffer = &aSource;
    THuiFxEngineType engineType = aEngine.EngineType();

    // The root group does not require a back buffer
    if (!iIsRoot)
        {
        backBuffer = aEngine.AcquireRenderbuffer(iBackbufferRect.Size());
        if (!backBuffer)
            {
            return;
            }

        // Make sure background is enabled if needed.
        if (AlwaysReadSurfacePixels())
            {
            backBuffer->EnableBackground(ETrue);
            backBuffer->PrepareForReuse(backBuffer->Size());
            }
        
        sourceBuffer = backBuffer;
        
        // Translate the graphics context so that the content appears in the correct place
        backBuffer->BindAsRenderTarget();        
        aGc.Push(EHuiGcMatrixModel);
        if(engineType == EHuiFxEngineVg10)
            {
            aGc.Scale(EHuiGcMatrixModel, 1.0f, -1.0f, 1.0f);
            aGc.Translate(EHuiGcMatrixModel, 0.0f, -iBackbufferRect.Size().iHeight, 0.0f);
            }
        
        aGc.Translate(EHuiGcMatrixModel, -iBackbufferRect.iTl.iX, -iBackbufferRect.iTl.iY, 0.0f);
        backBuffer->UnbindAsRenderTarget();
        }
    else if (iIsRoot && (&aTarget != aEngine.DefaultRenderbuffer()))
        {
        // We are rendering to offscreen buffer
        // Translate the graphics context so that the content appears in the correct place
        backBuffer->BindAsRenderTarget();        
        aGc.Push(EHuiGcMatrixModel);
        if(engineType == EHuiFxEngineVg10)
            {
            aGc.Scale(EHuiGcMatrixModel, 1.0f, -1.0f, 1.0f);
            aGc.Translate(EHuiGcMatrixModel, 0.0f, -iBackbufferRect.Size().iHeight, 0.0f);
            }
        
        aGc.Translate(EHuiGcMatrixModel, -iBackbufferRect.iTl.iX, -iBackbufferRect.iTl.iY, 0.0f);
        backBuffer->UnbindAsRenderTarget();        
        }
    
    for (TInt i = 0; i < iLayers.Count(); i++)
        {
        iLayers[i]->Draw(aEngine, aGc, *backBuffer, *sourceBuffer, aHasSurface);
        }

    // The root group does not support composition
    if (!iIsRoot)
        {
        TRect compSourceRect(TPoint(0, 0), iBackbufferRect.Size());
        TInt alpha = 0xff; // TODO
        if(engineType == EHuiFxEngineVg10)
            {
            backBuffer->BindAsRenderTarget();
            aGc.Pop(EHuiGcMatrixModel);
            backBuffer->UnbindAsRenderTarget();
            }
        else
            {
            aGc.Pop(EHuiGcMatrixModel);
            }                    
        
        // Composite the result
        TRect compositionTargetRect(TargetRect());        
        compositionTargetRect.Move(-aTarget.Position());

        aEngine.Composite(aTarget, *backBuffer, compositionTargetRect, compSourceRect, BlendingMode(), alpha);
        aEngine.ReleaseRenderbuffer(backBuffer);
        }
    else if (iIsRoot && (&aTarget != aEngine.DefaultRenderbuffer()))
        {
        // We did rendering to offscreen buffer
        if(engineType == EHuiFxEngineVg10)
            {
            backBuffer->BindAsRenderTarget();
            aGc.Pop(EHuiGcMatrixModel);
            backBuffer->UnbindAsRenderTarget();
            }
        else
            {
            aGc.Pop(EHuiGcMatrixModel);
            }                            
        }
    }

EXPORT_C void CHuiFxGroupLayer::AddLayerL(const CHuiFxLayer* aLayer)
    {
    iLayers.AppendL(aLayer);
    }

EXPORT_C TBool CHuiFxGroupLayer::VisualArea( TRect& aRect )
    {
    for( TInt i=0 ; i < iLayers.Count() ; i++ )
        {
        if( iLayers[i]->Type() == ELayerTypeFilter )
            {
            // found filter layer before visual => should apply to whole screen
            return EFalse;
            }
        else if(iLayers[i]->VisualArea( aRect ))
            {
            return ETrue;
            }
        }
    return EFalse;
    }

EXPORT_C void CHuiFxGroupLayer::ApplyMargin( TRect &aRect )
    {
    if (IsMarginEnabled())       
        {
        TMargins m;
        Margin(m);
        aRect.iTl.iX -= m.iLeft;
        aRect.iTl.iY -= m.iTop;
        aRect.iBr.iX += m.iRight;
        aRect.iBr.iY += m.iBottom;
        }
    }

EXPORT_C TBool CHuiFxGroupLayer::Margin( TMargins &aMargin )
     {
     TMargins m;
     m.iLeft = 0;
     m.iRight = 0;
     m.iTop = 0;
     m.iBottom = 0;
     for( TInt i=0; i < iLayers.Count() ; i++)
         {
         TMargins m2;
         if ( iLayers[i]->Margin(m2) )
             {
             m.iLeft += m2.iLeft;
             m.iRight += m2.iRight;
             m.iTop += m2.iTop;
             m.iBottom += m2.iBottom;
             }
         }
     aMargin = m;
     return ETrue;
     }
void CHuiFxGroupLayer::EnableMarginApplyChildren(TBool aEnable)
    {
    for( TInt i=0 ; i < iLayers.Count() ; i++ )
        {
        iLayers[i]->EnableMargin(aEnable);
        }
    }

EXPORT_C void CHuiFxGroupLayer::AdvanceTime(TReal32 aElapsedTime)
    {
    for( TInt i=0 ; i < iLayers.Count() ; i++ )
        {
        iLayers[i]->AdvanceTime(aElapsedTime);
        }
    }

EXPORT_C TBool CHuiFxGroupLayer::Changed() const
    {
    for( TInt i=0 ; i < iLayers.Count() ; i++ )
        {
        if (iLayers[i]->Changed())
            {
#ifdef HUIFX_TRACE    
            RDebug::Print(_L("CHuiFxGroupLayer::Changed true - 0x%x,"), this);
#endif
            return ETrue;
            }
        }
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxGroupLayer::Changed false- 0x%x,"), this);
#endif
    return EFalse;
    }
	
TBool CHuiFxGroupLayer::IsAnimated() const
    {
    if (CHuiFxLayer::IsAnimated())
        {
        return ETrue;
        }
    else
        {
        for( TInt i=0 ; i < iLayers.Count() ; i++ )
            {
            if (iLayers[i]->IsAnimated())
                {
                return ETrue;
                }
            }
        }
    return EFalse;
    }
	
TBool CHuiFxGroupLayer::IsTransformed() const
    {
    if (CHuiFxLayer::IsTransformed())
        {
        return ETrue;
        }
    else
        {
        for( TInt i=0 ; i < iLayers.Count() ; i++ )
            {
            if (iLayers[i]->IsTransformed())
                {
                return ETrue;
                }
            }
        }
    return EFalse;
    }

TBool CHuiFxGroupLayer::IsFiltered() const
    {
    if (CHuiFxLayer::IsFiltered())
        {
        return ETrue;
        }
    else
        {
        for( TInt i=0 ; i < iLayers.Count() ; i++ )
            {
            if (iLayers[i]->IsFiltered())
                {
                return ETrue;
                }
            }
        }
    return EFalse;
    }

TBool CHuiFxGroupLayer::IsSemitransparent() const
    {
    if (CHuiFxLayer::IsSemitransparent())
        {
        return ETrue;
        }
    else
        {
        for( TInt i=0 ; i < iLayers.Count() ; i++ )
            {
            if (iLayers[i]->IsSemitransparent())
                {
                return ETrue;
                }
            }
        }
    return EFalse;
    }


TInt CHuiFxGroupLayer::LayerCount() const
    {
    return iLayers.Count();
    }

CHuiFxLayer& CHuiFxGroupLayer::Layer(TInt aIndex) const
    {
    return *iLayers[aIndex];
    }
void CHuiFxGroupLayer::FxmlVisualInputs(RArray<THuiFxVisualSrcType> &aArray)
    {
    for( TInt i=0 ; i < iLayers.Count() ; i++ )
        {
        iLayers[i]->FxmlVisualInputs(aArray);
        }
    }

TBool CHuiFxGroupLayer::FxmlUsesOpaqueHint() const
    {
    TBool b = EFalse;
    for( TInt i=0 ; i < iLayers.Count() ; i++ )
        {
        b |= iLayers[i]->FxmlUsesOpaqueHint();
        }
    return b;
    }

void CHuiFxGroupLayer::SetAlwaysReadSurfacePixels(TBool aAlwaysReadSurfacePixels)
    {
    CHuiFxLayer::SetAlwaysReadSurfacePixels(aAlwaysReadSurfacePixels);
    for( TInt i=0 ; i < iLayers.Count() ; i++ )
        {
        iLayers[i]->SetAlwaysReadSurfacePixels(aAlwaysReadSurfacePixels);
        }
    }
