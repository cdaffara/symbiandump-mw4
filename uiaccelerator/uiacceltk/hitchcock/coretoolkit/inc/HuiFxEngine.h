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



#ifndef HUIFXENGINE_H_
#define HUIFXENGINE_H_

#include "HuiFxConstants.h"
#include "HuiFxParameter.h"
#include "huieffectable.h"
#include <uiacceltk/huilowmemoryobserver.h>
#include <e32std.h>


#define HUIFX_RBCACHE_ENABLED
#define HUIFX_EFFECTCACHE_ENABLED

//FORWARD DECLARATIONS
class CHuiFxRenderbuffer;
class CHuiFxFilter;
class CHuiFxEffect;
class CHuiVisual;
class CHuiFxEffectCache;
class MAlfGfxEffectObserver;
class CHuiGc;

class MHuiEffectObserver
    {
    public:
    virtual void EffectAdded(CHuiFxEffect* aEffect) = 0;
    virtual void EffectComplete(CHuiFxEffect* aEffect) = 0;
    };

class CHuiFxEngine : public CBase, public MHuiLowMemoryObserver, public MHuiMemoryLevelObserver
    {
    // important constant! Affects memory fragmentation in backend too small 
    // number forces lots of acquires/releases resulting in fragmented memory 
    // and finally panic.

    // CACHE_SIZE set to 20 to avoid buffer creation at worst case scenario (fading).
    // Emulator cannot currently handle that much (out of memory) 
#ifdef __WINS__ 
    static const TInt CACHE_SIZE = 1;
#else
    static const TInt CACHE_SIZE = 20;
#endif    
    friend class CHuiFxEffect;
protected:
    IMPORT_C void ConstructL(THuiFxEngineType aEngineType);
public:
    IMPORT_C virtual ~CHuiFxEngine();
    // Engine management
    IMPORT_C virtual void Release();
    IMPORT_C virtual void RestoreL();

    // Render buffer management
    IMPORT_C virtual CHuiFxRenderbuffer* AcquireRenderbuffer(const TSize& aDesiredSize);
    CHuiFxRenderbuffer* AcquireRenderbuffer(const TSize& aDesiredSize, TBool aAlllowLarger);

    IMPORT_C virtual void ReleaseRenderbuffer(CHuiFxRenderbuffer* aBuffer);
    virtual CHuiFxRenderbuffer* DefaultRenderbuffer() = 0;

    // Filter construction
    virtual CHuiFxFilter* CreateFilterL(THuiFxFilterType aType) = 0;

    // Composition
    // TODO: Coordinates
    virtual void Composite(CHuiFxRenderbuffer& aTarget, CHuiFxRenderbuffer& aSource,
                                    const TRect& aTargetRect, const TRect& aSourceRect,
                                    THuiFxBlendingMode aMode, TInt aAlpha) = 0;

    // Composition variant to compose directly to the screen. Uses always over blending with constant alpha.
    IMPORT_C virtual void Composite(CHuiGc& aGc, CHuiFxRenderbuffer& aSource, const TPoint& aTargetPoint, TBool aOpaque, TInt aAlpha);
    
    // Effect construction
    // TODO: Other needed effect parameters?
    // Demarcation rectangele added.
    
    // Effect end observer and handle added
    IMPORT_C void RegisterEffectL(const TDesC &aFileName);
    IMPORT_C void UnRegisterEffectL(const TDesC &aFileName);
    IMPORT_C void LoadEffectL(
        const TDesC& aFileName,
        CHuiFxEffect*& aEffect,
        MHuiEffectable* aVisual = 0,
        TRect* extRect = 0,
        MAlfGfxEffectObserver* aEffectEndObserver = 0,
        TInt aHandle = 0, 
        TInt aFlags = 0 );
    
    IMPORT_C void LoadGroupEffectL(
            const TDesC& aFileName,
            CHuiFxEffect*& aEffect,
            MHuiEffectable* aVisual = 0,
            TInt aGroupHandle = 0,
            TRect* extRect = 0,
            MAlfGfxEffectObserver* aEffectEndObserver = 0,
            TInt aHandle = 0,
            TInt aFlags = 0 );
    
    IMPORT_C TBool FxmlUsesInput1(const TDesC &aFileName);
    IMPORT_C TBool FxmlUsesOpaqueHint(const TDesC &aFileName);
    
    IMPORT_C void AdvanceTime(TReal32 aElapsedTime);
    
    IMPORT_C TReal32 GetReferenceValue(THuiFxReferencePoint aPoint);
    
    IMPORT_C THuiFxEngineType EngineType();
    
    //from MHuiLowMemoryObserver
    // deprecated
    IMPORT_C void EnableLowMemoryState(TBool aEnable);
    
    /*
	 * BeginGroupEffect
	 *
	 * Begins new group. start collecting effects, that should be set to motion at the same 
	 * time.
	 */
	IMPORT_C void BeginGroupEffect(TInt aGroup);
    
    IMPORT_C TInt ActiveGroupEffect();
    
    IMPORT_C TBool AddEffectToGroup(TInt aGroup);
    /*
	 * StartGroupEffect
	 *
	 * Starts syncronously group of effects that have the same aGroup id. 
	 */
    IMPORT_C void StartGroupEffect(TInt aGroup);
    
    TInt LowMemoryState();
    
    TBool HasActiveEffects() const;
    TBool HasActiveFadeEffect() const;
    
    void ClearCache();
    
    void NotifyEffectEndObservers();
    
    IMPORT_C void SetMemoryLevel(THuiMemoryLevel aLevel);
    
    void SetObserver(MHuiEffectObserver* aObserver)
        {
        iEffectObserver = aObserver;    
        }
	
	/**
	 * Group effects wait until each effect has been drawn once. 
	 *
	 * Group effects are set into motion by NotifyEffectReady, when all effects in
	 * the group have been drawn at least once.
	 */
    void NotifyEffectReady(TInt aGroupId);
    
protected:
    IMPORT_C void AddEffectL(CHuiFxEffect* aEffect);
    IMPORT_C void RemoveEffect(CHuiFxEffect* aEffect);
    
    TRequestStatus   iParserStatus;
    TInt             iBuffersInUse;
    
private:
    
    TBool FxmlUsesInput1(CHuiFxEffect& aEffect);
    TInt FindEffectGroup(TInt aGroup);
    
private:
    // Render buffer management --- native implementations
    virtual CHuiFxRenderbuffer* AcquireNativeRenderbuffer(const TSize& aDesiredSize) = 0;
    virtual void ReleaseNativeRenderbuffer(CHuiFxRenderbuffer* aBuffer) = 0;

    // Render buffer cache
#ifdef HUIFX_RBCACHE_ENABLED
    CHuiFxRenderbuffer* LookupFreeRenderbuffer(const TSize& aDesiredSize, TBool aAlllowLarger);
    void InsertFreeRenderbuffer(CHuiFxRenderbuffer *aBuffer);
    inline TBool IsCacheEmpty() const;
    inline TBool IsCacheFull() const;

    // simple general renderbuffer cache
    TInt                iBuffersInCache;
    CHuiFxRenderbuffer* iBufferCache[CACHE_SIZE];
#endif
    
#ifdef HUIFX_PERF_COUNTERS
    // TODO! counters for performance evaluation?!
    TInt iMaxAcquiredBuffers;
    TInt iMaxFreeBuffers;
    TSize iMaxBufferSize;
    TInt iHit;
    TInt iMiss;
#endif

    RPointerArray<CHuiFxEffect>  iActiveEffects;
    THuiFxEngineType             iType;
    TRect                        iExtRect;
    CHuiFxEffectCache *iCache;
    TInt iLowGraphicsMemoryMode;
    
    NONSHARABLE_STRUCT(TEffectGroupStruct)
        {
    public:
            
        TEffectGroupStruct(TInt aGroup) : iGroup(aGroup), iWaiting(0), iEndCalled(EFalse){};

        TInt iGroup;
        TInt iWaiting;
        TBool iEndCalled;
        };
    
	/**
	 * Only one simultanious effect group is supported
	 */
    RArray<TEffectGroupStruct> iActiveEffectGroups;
    
    MHuiEffectObserver* iEffectObserver;
    };

#endif /*HUIFXENGINE_H_*/
