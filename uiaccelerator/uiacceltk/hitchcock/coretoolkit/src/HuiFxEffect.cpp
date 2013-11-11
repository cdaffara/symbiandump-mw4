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



#include "HuiFxEffect.h"
#include "HuiFxGroupLayer.h"
#include "HuiRenderPlugin.h"
#include "HuiCmdBufferBrush.h" // MHuiEffectable

#include "alfmoduletestconf.h"
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
    // Provides TLS object data for test cases.
    // This is used only if module test hooks are set on.
    #include "huistatictlsdata.h"
#endif // USE_MODULE_TEST_HOOKS_FOR_ALF
// Provides module test hook defines.
#include "alfmoduletestdefines.h"


EXPORT_C CHuiFxEffect* CHuiFxEffect::NewL(CHuiFxEngine& aEngine)
    {
    CHuiFxEffect* e = new (ELeave) CHuiFxEffect( aEngine );
    CleanupStack::PushL(e);
    e->ConstructL();
    CleanupStack::Pop(e);
    return e;
    }

CHuiFxEffect::CHuiFxEffect( CHuiFxEngine& aEngine ) :
    iEngine( &aEngine )
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxEffect::CHuiFxEffect - 0x%x"), this);
#endif
    }
EXPORT_C CHuiFxEffect *CHuiFxEffect::CloneL() const
{
   CHuiFxEffect *effect = new (ELeave)CHuiFxEffect(*iEngine);
   effect->iRoot = iRoot->CloneL();
   effect->iEffectEndObserver = iEffectEndObserver;
   effect->iHandle = iHandle;
   return effect;
}
EXPORT_C void CHuiFxEffect::SetExtRect( TRect *aExtRect )
    {
    iRoot->SetExtRect(aExtRect);
    }
EXPORT_C void CHuiFxEffect::SetVisual( CHuiVisual *aVisual )
    {
    iRoot->SetVisual(aVisual);
    }
EXPORT_C void CHuiFxEffect::SetVisual( MHuiEffectable *aEffectable )
    {
    iRoot->SetVisual(aEffectable);
    }
EXPORT_C void CHuiFxEffect::SetEngine( CHuiFxEngine *aEngine )
    {
    iEngine = aEngine;
    iEngine->AddEffectL(this);
    }

EXPORT_C void CHuiFxEffect::ConstructL( )
    {
    iRoot = CHuiFxGroupLayer::NewL(ETrue);
#ifndef HUIFX_EFFECTCACHE_ENABLED    
    iEngine->AddEffectL(this);
#endif
    }

EXPORT_C CHuiFxEffect::~CHuiFxEffect()
    {
    delete iRoot;
    iRoot = NULL;
    NotifyEffectEndObserver();
    
    ReleaseCachedRenderTarget();
    
    iEngine->RemoveEffect(this);
    if (iEngine && iGroupId != KErrNotFound && !(iFlags & KHuiReadyToDrawNotified))
        {
        // if effect was deleted before it was drawn, the group must be notified. If this was the last effect in the group
        // the group will be removed by the EffectReadyToStart
        // effect group does not not know, which effects have notified about themselves.
        SetEffectFlag(KHuiReadyToDrawNotified);
        iEngine->NotifyEffectReady(iGroupId);
        }
    
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxEffect::~CHuiFxEffect - 0x%x"), this);
#endif
    }

TBool CHuiFxEffect::NotifyEffectEndObserver()
    {
    if (iFlags & KHuiEffectObserverNotified)
        {
        return ETrue;
        }
    SetEffectFlag(KHuiEffectObserverNotified); // prevent extra notifier calls calls
	// fade effect should not have observers
    if (iFlags & KHuiFadeEffectFlag)
        {
        return ETrue; // fade effect does not have observer that would need notification
        }
    
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
    TTime endTime;
    endTime.UniversalTime();
    
	// There might be several BeginFullScreen for single effects. We want to calculate
	// reaction time from the first BeginFullScreen event to this point
    TInt timeStamps = 0;
    AMT_GET_TIME_POINT_COUNT(iHandle, timeStamps);
    
    TInt64 temp;
    TBool effects(EFalse); // dummy, 1 if effects were on for this time stamp
    AMT_GET_TIME(temp, iHandle, 0, effects);
    TTime startTime(temp);
    
    AMT_GET_TIME(temp, iHandle, timeStamps - 1, effects);
    TTime effectStartTime(temp);
    
    TInt64 effectTime = endTime.MicroSecondsFrom(effectStartTime).Int64();
    TReal fps = (TReal)iFramesDrawn / ((TReal)effectTime / 1000000.0f) ; 

    TInt64 totalEffectTime = endTime.MicroSecondsFrom(startTime).Int64();
    TInt64 reactionTime = effectStartTime.MicroSecondsFrom(startTime).Int64();
        
    RDebug::Printf("CHuiFxEffect::NotifyEffectEndObserver - Reaction time \t0x%x\t%f\tVisible effect time:\t%f\ts. (%f FPS). Total effect time:\t%f", 
            iHandle,
            (TReal)reactionTime / 1000000.0f,
            ((TReal)effectTime)/1000000,
            fps,
            ((TReal)totalEffectTime)/1000000.0f 
            );
    AMT_RESET_TIME(iHandle);
#endif
    
    if (iEffectEndObserver)
        {
        // The callback can be called only once when the effect finishes
        MAlfGfxEffectObserver* effectEndObserver = iEffectEndObserver;
        iEffectEndObserver = NULL;
        // Note: The call below may synchronously delete me (CHuiFxEffect instance)
        effectEndObserver->AlfGfxEffectEndCallBack( iHandle );
        return ETrue; // end observer notified
        }
    else
        {
		// must be notified without destroying the effect first. gives alf apps chance
		// to do their own cleanup
        return EFalse; 
        }
    }

EXPORT_C void CHuiFxEffect::AddLayerL(const CHuiFxLayer* aLayer)
    {
    iRoot->AddLayerL(aLayer);
    }

void CHuiFxEffect::ReleaseCachedRenderTarget()
    {
    if (iCachedRenderTarget)
        {
        iEngine->ReleaseRenderbuffer(iCachedRenderTarget);
        iCachedRenderTarget = NULL;
        }                
    }

void CHuiFxEffect::PrepareCachedRenderTarget(const TPoint& aPosition, const TSize& aSize, TBool aClear, TBool aEnableBackground)
    {
    // If size has chnaged, we must delete old one
    if (iCachedRenderTarget && 
        iCachedRenderTarget->Size() != aSize)
        {
        ReleaseCachedRenderTarget();
        }            
    
    // Accure new buffer
    if (!iCachedRenderTarget)
        {
        iCachedRenderTarget = iEngine->AcquireRenderbuffer(aSize, EFalse);                    
        }    
    
    if (iCachedRenderTarget)
        {
        // Set render buffer position in screen coordinates
        iCachedRenderTarget->SetPosition(aPosition);                 
        
        // Enable background if needed
       iCachedRenderTarget->EnableBackground(aEnableBackground);

       // Clear if requested
        if (aClear)
            {
            iCachedRenderTarget->PrepareForReuse(iCachedRenderTarget->Size());
            }        
        }    
    }

void CHuiFxEffect::ForceCachedRenderTargetUsage(TBool aUseCachedRenderTarget)
    {
    iForcedUseCachedRenderTarget = aUseCachedRenderTarget;
    }

TBool CHuiFxEffect::IsCachedRenderTargetPreferred() const
    {
    // Prefer use cached render target if 
    //
    // 1. I am filter effect. 
    //    Note that this does not provide good performance if effect 
    //    is animated, but noncached version does not work visually correctly 
    //    in rotated display if effect is applied to background as well.
    //    -> TODO: Return EFalse if effect has animated filter which does not use background pixels 
    // 
    // 2. User has set force flag
    //
    if (iForcedUseCachedRenderTarget || iRoot->IsFiltered())
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

TBool CHuiFxEffect::IsCachedRenderTargetSupported() const
    {
    // Can use cached render target if
    //
    // 1. Render supports it (openvg for now).
    // 2. Engine is not in low memory mode.
    //
    if (iEngine->EngineType() == EHuiFxEngineVg10 && 
        !iEngine->LowMemoryState()) // normal
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

TBool CHuiFxEffect::CachedDraw(CHuiGc& aGc, const TRect& aDisplayRect, TBool aRefreshCachedRenderTarget, TBool aOpaque)
    {
    RRegion dummy;
    TBool ret = CachedDraw(aGc, aDisplayRect, aRefreshCachedRenderTarget, aOpaque, dummy,EFalse);
    dummy.Close();
    return ret;
    }

// TODO: effect area should be reduced if aClipRegion is smaller than aDisplayRect.
TBool CHuiFxEffect::CachedDraw(CHuiGc& aGc, const TRect& aDisplayRect, TBool aRefreshCachedRenderTarget, TBool aOpaque, const TRegion& aClipRegion, TBool aHasSurface, TInt aAlpha)
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxEffect::CachedDraw - 0x%x"), this);
#endif    
    iFramesDrawn++;
    if (!CHuiStatic::Renderer().Allows(EHuiRenderPluginAllowVisualPBufferSurfaces))
        {
        return EFalse;
        }
            
    
    CHuiFxRenderbuffer* target = NULL;
    
    // Prepare all layers
    TRect displayArea = aGc.DisplayArea();
    TRect targetArea = aDisplayRect;
    targetArea.Intersection(displayArea);

    if (targetArea.Width() <= 0 || targetArea.Height() <= 0)
        {
        // Not visible
        return ETrue;
        }

    if (!iEngine || !iRoot)
        {
        return EFalse;
        }

    if (iEngine->LowMemoryState())
        {
        // No memory, no effects.
        return EFalse;
        }
    
    // Check if margins are allowed to be used for this effect
    if (EffectFlags() & KHuiFxEffectDisableMarginsFlag)
        {
        iRoot->EnableMargin(EFalse);
        }

    // Check if surface pixels are to be used for this effect in all layers.
    if (EffectFlags() & KHuiFxEnableBackgroundInAllLayers)
        {
        iRoot->SetAlwaysReadSurfacePixels(ETrue);
        }
    
    iRoot->SetTargetRect(targetArea);
    iRoot->SetSourceRect(targetArea);        
    iRoot->SetDisplayArea(displayArea);
    
    TRAPD(err, iRoot->PrepareDrawL(*iEngine));
    
    if (err != KErrNone)
        {
        return EFalse;
        }

    if (IsCachedRenderTargetSupported() && IsCachedRenderTargetPreferred())
        {
        // Background needs to be captured from surface if effect uses background AND 
        // Visual is transparent or margin is enabled AND
        // Background has not been disabled with a effect specific flag
        TBool enableBackground = IsAppliedToBackground() && (!aOpaque || iRoot->IsMarginEnabled()) && !(EffectFlags() & KHuiFxDisableBackground);
        
        if (EffectFlags() & KHuiFxEnableBackgroundInAllLayers)
            {
            enableBackground = ETrue;
            }
        
        TBool useFrozenBackground = (EffectFlags() & KHuiFxFrozenBackground);
        
        // Check if cache is up-to date or does it need to be refreshed
        TBool cachedRenderTargetNeedsRefresh = (iRoot->Changed() || aRefreshCachedRenderTarget || (enableBackground && !useFrozenBackground));
        if (!iCachedRenderTarget || (iCachedRenderTarget && iCachedRenderTarget->Size() !=  iRoot->VisualRect().Size())) 
            {
            cachedRenderTargetNeedsRefresh = ETrue;
            }

        // Try to apply also margins, we cannot just use aDisplayRect directly
        TRect targetRect = iRoot->VisualRect();
        
        // Size is always same as target rect. It contains margins if those are enabled.
        TSize cachedRenderTargetSize = targetRect.Size();        
                
        // Prepare cached offscreen render target
        PrepareCachedRenderTarget(targetRect.iTl, cachedRenderTargetSize, cachedRenderTargetNeedsRefresh, enableBackground);
        
        // If it is available, then lets do it 
        if (iCachedRenderTarget)
            {
            // Disable clipping, otherwise strange things happen.
            aGc.Disable(CHuiGc::EFeatureClipping);             
            if (cachedRenderTargetNeedsRefresh)
                {
                // Render to cached render target
                iRoot->Draw(*iEngine, aGc, *iCachedRenderTarget, *iCachedRenderTarget, aHasSurface);                
#ifdef HUIFX_TRACE    
                RDebug::Print(_L("CHuiFxEffect::CachedDraw - refreshed cached render buffer 0x%x"), this);
#endif
                }            

            if (aClipRegion.Count())
                {
                aGc.Enable(CHuiGc::EFeatureClipping);
                aGc.PushClip();
                aGc.Clip(aClipRegion);
                }
            
            // Write cached buffer to the display
           
	       iEngine->Composite(aGc, *iCachedRenderTarget, targetRect.iTl, aOpaque && !(EffectFlags() & KHuiFxAlwaysBlend), aAlpha);
           

            if (aClipRegion.Count())
                {
                aGc.PopClip();
                }
            
            aGc.Enable(CHuiGc::EFeatureClipping);
#ifdef HUIFX_TRACE    
            RDebug::Print(_L("CHuiFxEffect::CachedDraw - Cached render buffer drawn 0x%x"), this);
            RDebug::Print(_L("CHuiFxEffect::CachedDraw - displayrect: %i,%i, %i,%i "), 
                    aDisplayRect.iTl.iX,
                    aDisplayRect.iTl.iY,
                    aDisplayRect.iBr.iX,
                    aDisplayRect.iBr.iY);
#endif            
            }
        else
            {
            // It might not be available e.g. in low memory situations, just indiacte that we could not draw.
            return EFalse;
            }
        }
    else
        {
        // Release cached render target just in case it is reserved for some reason
        ReleaseCachedRenderTarget();

        // Use default onscreen render target
        if (!target)
            {
            target = iEngine->DefaultRenderbuffer();
            }
        
        if (!target)
            {
            return EFalse;
            }
        
        // Normal drawing
        iRoot->Draw(*iEngine, aGc, *target, *target, aHasSurface);
        }
                
    return ETrue;    
    }

EXPORT_C TBool CHuiFxEffect::Draw(CHuiGc& aGc, const TRect& aDisplayRect, TBool aHasSurface)
    {
    // Prepare all layers
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxEffect::Draw - 0x%x"), this);
#endif
    iFramesDrawn++;
    if (!CHuiStatic::Renderer().Allows(EHuiRenderPluginAllowVisualPBufferSurfaces))
        {
        return EFalse;
        }
            
    TRect displayArea = aGc.DisplayArea();
    TRect targetArea = aDisplayRect;
    targetArea.Intersection(displayArea);

    if (targetArea.Width() <= 0 || targetArea.Height() <= 0)
        {
        // Not visible
        return ETrue;
        }

    if (!iEngine || !iRoot)
        {
        return EFalse;
        }

    if (iEngine->LowMemoryState())
        {
        // No memory, no effects.
        return EFalse;
        }
    
    // Check if margins are allowed to be used for this effect
    if (EffectFlags() & KHuiFxEffectDisableMarginsFlag)
        {
        iRoot->EnableMargin(EFalse);
        }
    
    iRoot->SetDisplayArea(displayArea);
    iRoot->SetTargetRect(targetArea);
    iRoot->SetSourceRect(targetArea);
    
    TRAPD(err, iRoot->PrepareDrawL(*iEngine));
    
    if (err != KErrNone)
        {
        return EFalse;
        }
    
    CHuiFxRenderbuffer* target = iEngine->DefaultRenderbuffer();
    
    if (!target)
        {
        return EFalse;
        }

    iRoot->Draw(*iEngine, aGc, *target, *target, aHasSurface);
    return ETrue;
    }

EXPORT_C TBool CHuiFxEffect::VisualArea(TRect& aRect) const
    {
    return iRoot->VisualArea(aRect);
    }

EXPORT_C CHuiFxEngine& CHuiFxEffect::Engine() const
    {
    return *iEngine;
    }

EXPORT_C TBool CHuiFxEffect::Changed()
    {
    TBool changed = iRoot->Changed();
    return changed;
    }
    
EXPORT_C void CHuiFxEffect::SetEffectEndObserver( MAlfGfxEffectObserver* aEffectEndObserver, TInt aHandle )
    {
    iEffectEndObserver = aEffectEndObserver;
    
    if (aHandle != 0) // override handle only if someone is interested
        {
        iHandle = aHandle;
        }    
    }

EXPORT_C void CHuiFxEffect::SetEffectFlags( TInt aFlags )
    {
    iFlags = aFlags;
    }

void CHuiFxEffect::SetEffectFlag( TInt aFlag )
    {
#ifdef HUIFX_TRACE
    RDebug::Printf("CHuiFxEffect::SetEffectFlag - Setting flag 0x%x for 0x%x, before: iFlags: 0x%x", aFlag, this, iFlags);
#endif
    iFlags |= aFlag;
    }

void CHuiFxEffect::ClearEffectFlag( TInt aFlag )
    {
#ifdef HUIFX_TRACE
    RDebug::Printf("CHuiFxEffect::ClearEffectFlag - Clearing flag 0x%x for 0x%x, before: iFlags: 0x%x", aFlag, this, iFlags);
#endif    
    iFlags &= ~aFlag;
    }

EXPORT_C void CHuiFxEffect::SetEffectGroup(TInt aGroupId)
    {
    iGroupId = aGroupId;
    }

EXPORT_C TInt CHuiFxEffect::EffectFlags()
    {
    return iFlags;
    }

EXPORT_C TInt CHuiFxEffect::GroupId()
    {
    return iGroupId;
    }

EXPORT_C void CHuiFxEffect::AdvanceTime(TReal32 aElapsedTime)
    {
#ifdef HUIFX_TRACE
    RDebug::Printf("CHuiFxEffect::AdvanceTime 0x%x, aElapsed time: %f, Total elapsed time: %f, Frames drawn: %d, iFlags: 0x%x, iGroup %d, iHandle: %d", 
            this, 
            aElapsedTime, 
            iElapsedTime, 
            iFramesDrawn, 
            iFlags,
            iGroupId,
            iHandle);
#endif
    // KHuiFxDelayRunUntilFirstFrameHasBeenDrawn flag is for giving effect chance to run
    // its whole timeline by starting the time only when first frame has been drawn.
    if (iFlags & KHuiFxDelayRunUntilFirstFrameHasBeenDrawn)
        {
        // Sometimes the effect does not get any frames. Force the time to start, because
        // otherwise will jam itself and possible the group, where the effect is.
        if (iElapsedTime > 0.2 && iFramesDrawn == 0)
            {
            iFramesDrawn = 1;
#ifdef HUIFX_TRACE            
            RDebug::Printf("CHuiFxEffect::AdvanceTime - Not drawn, but cannot wait. release 0x%x in time %f", this, iElapsedTime);
#endif
            }
        
        if (iFramesDrawn)
            { 
            if (iFlags & KHuiFxReadyAndWaitingGroupToStartSyncronized)
                {
	            // this has drawn atleast once, but all the effect in this group have not drawn. Must hang on little more.
                return;
                }
        
            if (iFlags & KHuiFxWaitGroupToStartSyncronized)
                {
				// Group has been started, waiting the others in the group to be drawn
                ClearEffectFlag(KHuiFxWaitGroupToStartSyncronized);
                SetEffectFlag(KHuiFxReadyAndWaitingGroupToStartSyncronized);
				// NotifyEffectReady will clear KHuiFxReadyAndWaitingGroupToStartSyncronized flag
				// if all items in the group are ready.
                iEngine->NotifyEffectReady(iGroupId);
                SetEffectFlag(KHuiReadyToDrawNotified);
                return;
                }

            if (iFramesDrawn == 1)
                {
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
				// This is about the time when first frame from the effect is on screen
                TTime endTime;
                endTime.UniversalTime();
                
                AMT_ADD_TIME(iHandle, endTime.Int64(), ETrue);
#endif
                aElapsedTime = 0;
                iFramesDrawn++;
                }
                iRoot->AdvanceTime(aElapsedTime);
            }
        }
    else
        {
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
        if (iFramesDrawn == 1)
            {
            // This is about the time when first frame from the effect is on screen
            TTime endTime;
            endTime.UniversalTime();
            AMT_ADD_TIME(iHandle, endTime.Int64(), ETrue);
            }
#endif
        iRoot->AdvanceTime(aElapsedTime);
        }
    iElapsedTime += aElapsedTime;
    }
    
EXPORT_C TBool CHuiFxEffect::IsAnimated() const
    {
    if (iRoot && iRoot->IsAnimated())
        {
        return ETrue;        
        }
    else
        {
        return EFalse;
        }    
    }

EXPORT_C TBool CHuiFxEffect::IsTransformed() const
    {
    if (iRoot && iRoot->IsTransformed())
        {
        return ETrue;        
        }
    else
        {
        return EFalse;
        }            
    }

TBool CHuiFxEffect::IsFiltered() const
    {
    if (iRoot && iRoot->IsFiltered())
        {
        return ETrue;        
        }
    else
        {
        return EFalse;
        }            
    }

TBool CHuiFxEffect::IsAppliedToBackground()
    {
    if (iRoot && iRoot->LayerCount())
        {
        if (iRoot->Layer(0).Type() == ELayerTypeGroup)
            {
            // If first layer after root is group, then effect is not applied to background (?)
            return EFalse;    
            }
        else
            {
            if (IsFiltered())
                {
                return ETrue;
                }
            else
                {
                // Not filter effect -> cannot be applied to background.
                return EFalse;
                }
            } 
        }            
    return EFalse;    
    }

TBool CHuiFxEffect::IsSemitransparent() const
    {
    if (iRoot && iRoot->IsSemitransparent())
        {
        return ETrue;        
        }
    else
        {
        return EFalse;
        }            
    }

void CHuiFxEffect::FxmlVisualInputs(RArray<THuiFxVisualSrcType> &aArray)
    {
    iRoot->FxmlVisualInputs(aArray);
    }

TBool CHuiFxEffect::FxmlUsesOpaqueHint() const
    {
    return iRoot->FxmlUsesOpaqueHint();
    }
