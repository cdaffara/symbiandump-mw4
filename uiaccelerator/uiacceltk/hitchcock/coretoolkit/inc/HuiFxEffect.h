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



#ifndef HUIFXEFFECT_H_
#define HUIFXEFFECT_H_

#include "HuiFxEngine.h"
#include <uiacceltk/HuiGc.h>

//FORWARD DECLARATIONS
class CHuiFxLayer;
class CHuiFxGroupLayer;
class MHuiEffectable;
// Hui Effect Flags
// The flags must be 1 bit flags to allow several to be set simultaneously (if needed)
const TInt KHuiFlagsNone = 0x0;

/** Effect is system fade effect */
const TInt KHuiFadeEffectFlag = 0x1;

/** Effect does not use margins. It is clipped inside its area. */
const TInt KHuiFxEffectDisableMarginsFlag = 0x2;

/** Effect is not applied to children visuals. */
const TInt KHuiFxEffectExcludeChildrenFlag = 0x4;

/** Effect is grouped and syncronized with other effect(s)
 * 
 *  Indicates that the EndGroup event has not yet been received.
 */
const TInt KHuiFxWaitGroupSyncronization = 0x8;

/** Effect duration is started after it has first time been drawn. */
const TInt KHuiFxDelayRunUntilFirstFrameHasBeenDrawn = 0x10;

/** Background pixels are not read even if effect would be applied to background. */
const TInt KHuiFxDisableBackground = 0x20;

/** Effect content is composited using blending even if normally it would require other mode */ 
const TInt KHuiFxAlwaysBlend = 0x40;

/** Effect content is marked to produce opaque content on the screen. This can be used to avoid drawing unnecessarely under the effect area. */
const TInt KHuiFxOpaqueHint = 0x80;

/** Background pixels are read for every layer. This is used for fading legacy ALF content only. */
const TInt KHuiFxEnableBackgroundInAllLayers = 0x100;

/** Background pixels are not read for every frame (only once in a while), thus background content looks frozen if it is visible. */
const TInt KHuiFxFrozenBackground = 0x200;

/** 
 * EndGroup request has been received, but this effect has not yet drawn itself.
 */
const TInt KHuiFxWaitGroupToStartSyncronized = 0x400;

/** EndGroup request has been received and this effect has drawn itself once and it is
 *  now waiting for the others in the group to be drawn.
 */
const TInt KHuiFxReadyAndWaitingGroupToStartSyncronized = 0x800;

/**
 * This applies only to group effects. Visual has drawn one frame after the 
 * effect was added to the visual.
 */
const TInt KHuiReadyToDrawNotified = 0x1000;

/**
 * Effect observer has been called once. 
 */
const TInt KHuiEffectObserverNotified = 0x2000;

class MAlfGfxEffectObserver
    {
    public:
    	/**
     	 * Function to be called when CHuiFxEffect class effect has ended
     	 * @param aHandle handle of the effect, given when the effect is started
     	 */
        virtual void AlfGfxEffectEndCallBack( TInt aHandle ) = 0;
    };

class CHuiFxEffect : public CBase
    {
public:
    IMPORT_C virtual ~CHuiFxEffect();
    IMPORT_C static CHuiFxEffect* NewL(CHuiFxEngine& aEngine);
    IMPORT_C void AddLayerL(const CHuiFxLayer* aLayer); // takes ownership
    IMPORT_C void AdvanceTime(TReal32 aElapsedTime);
    IMPORT_C TBool Draw(CHuiGc& aGc, const TRect& aDisplayRect, TBool aHasSurface);
    IMPORT_C CHuiFxEngine& Engine() const;
    IMPORT_C TBool VisualArea(TRect& aRect) const;
    IMPORT_C TBool Changed();
    IMPORT_C void SetEffectEndObserver( MAlfGfxEffectObserver* aEffectEndObserver, TInt aHandle );

    /**
	 * NotifyEffectEndObserver
	 *
	 * @return	ETrue, if effect observer was notified.
	 *			EFalse, if effect does not have observer, but other notification is required (alf application
	 *                  effects.
	 */
    TBool NotifyEffectEndObserver();

    IMPORT_C TBool IsAnimated() const;
    IMPORT_C TBool IsTransformed() const;
    TBool IsSemitransparent() const;
    

public: // effect cache methods
    IMPORT_C CHuiFxEffect *CloneL() const;
    IMPORT_C void SetExtRect( TRect *aExtRect );
    IMPORT_C void SetVisual( CHuiVisual *aVisual );
    IMPORT_C void SetVisual( MHuiEffectable *aVisual );
    IMPORT_C void SetEngine( CHuiFxEngine *aEngine );

    IMPORT_C void SetEffectFlags( TInt aFlags );
    void SetEffectFlag( TInt aFlag );
    void ClearEffectFlag( TInt aFlag );
    
    IMPORT_C void SetEffectGroup(TInt aGroupId);
    IMPORT_C TInt EffectFlags();
    IMPORT_C TInt GroupId();

    // Effect cached render target methods    
    TBool IsCachedRenderTargetSupported() const;
    TBool IsCachedRenderTargetPreferred() const;
    
    void ForceCachedRenderTargetUsage(TBool aUseCachedRenderTarget);
    
    void PrepareCachedRenderTarget(const TPoint& aPosition, const TSize& aSize, TBool aClear, TBool aEnableBackground);
    void ReleaseCachedRenderTarget();
    
    /*
     * CachedDraw is typically much faster than normal draw as it keeps effect result in pre-rendered format
     * instead of doing it on-the-fly as normal draw. There are however few issues that must be noted:
     * 
     * - If visual content has changed, caller is then responsible for setting param aRefreshCachedRenderTarget=ETrue
     *   so that effect gets refreshed.
     * - Caching does not make sense for animated effects, this method automatically defaults to normal
     *   drawing if the effect is animated
     * - All renderers do not (yet) support cached drawing
     * - If memory is low, cached drawing may not succed.    
     *   
     *  @param aGc
     *  @param aDisplayRect
     *  @param aRefreshCachedRenderTarget ETrue makes sure that effect is rendered, EFalse may use cached
     *         result if it is otherwise possible.
     *  @param  aOpaque, can be used for optimization so that when set to ETrue content behind visual is 
     *          not read from drawing surface.
     *  @param  aClipRegion If region contains rects, final composition to display is clipped according to
     *          defined region.          
     *                   
     *  @return ETrue if drawing was done, otherwise EFalse   
     */
    TBool CachedDraw(CHuiGc& aGc, const TRect& aDisplayRect, TBool aRefreshCachedRenderTarget, TBool aOpaque, const TRegion& aClipRegion, TBool aHasSurface, TInt aAlpha = 255);
    TBool CachedDraw(CHuiGc& aGc, const TRect& aDisplayRect, TBool aRefreshCachedRenderTarget, TBool aOpaque);

    void FxmlVisualInputs(RArray<THuiFxVisualSrcType> &aArray);
    
	TBool FxmlUsesOpaqueHint() const;
	
    TInt Handle() const
        {
        return iHandle;    
        }
private:

    TBool IsAppliedToBackground();
    TBool IsFiltered() const;
    
    
protected:
    CHuiFxEffect( CHuiFxEngine& aEngine );
    void ConstructL();
        
    CHuiFxGroupLayer*        iRoot;
    CHuiFxEngine*            iEngine;
    // The observer that wants to be called when the effect finishes
    // If no observer has been set, the pointer is NULL
    // This pointer is not owned, so it must not be deleted
    MAlfGfxEffectObserver*   iEffectEndObserver;
    // The effect handle that will be returned to the observer when effect finishes
    TInt                     iHandle;
    // The flags will indicate special cases that will need special handling (like fade)
    TInt                     iFlags;
    // Cached render target which holds the effect result in pre-rendered format
    CHuiFxRenderbuffer* iCachedRenderTarget;
    
    TBool iForcedUseCachedRenderTarget;
    
    // Used for syncronizing effect start of effects with same group id. 
    // See also KHuiFxWaitGroupSyncronization
    TInt iGroupId;
	
    TInt iFramesDrawn;
    TReal32 iElapsedTime;
    };

#endif /*HUIFXEFFECT_H_*/
