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
* Description:   Implementation of CHuiEnv, a common working environment for the Toolkit.
*
*/



#include <eikenv.h>
#include <bautils.h>
#include <coemain.h>
#include <w32std.h>
#include <gdi.h>
#include <e32math.h>

#include "uiacceltk/HuiEnv.h"  // Class definition
#include <ecom/implementationinformation.h>
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiStatic.h"
#include "alf/alfconstants.h"
#include "HuiRenderSurface.h"
#include "uiacceltk/HuiDisplay.h"
#include "HuiRosterImpl.h"
#include "uiacceltk/HuiScheduler.h"
#include "uiacceltk/HuiTextureManager.h"
#include "uiacceltk/HuiControlGroup.h"
#include "HuiVisualFactory.h"
#include "uiacceltk/HuiS60Skin.h"
#include "uiacceltk/HuiEvent.h"
#include "uiacceltk/HuiRoster.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/huitextstylemanager.h"
#include "huistatictlsdata.h"
#include "uiacceltk/HuiThemeManager.h"
#include "huicanvastexturecache.h"
#include "HuiFxEngine.h"
#include "appiconcache.h"

#include "huisynchronizationobserver.h"

#define HUI_HIRES_TIMER


/* Constants */

// Until we get refresh interval issues solved (implement hi-res timer), this define keeps fps at respectable enough level
#ifdef SYMBIAN_BUILD_GCE
#define HUI_NGA_MIN_REFRESH_INTERVAL 
#endif

// This is for testing, can be used to force forced refresh
#ifdef SYMBIAN_BUILD_GCE
//#define HUI_NGA_FORCED_REFRESH 
#endif

// This is for testing, prints performance information
//#define HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL

#ifdef SYMBIAN_BUILD_GCE
    #ifdef __WINSCW__
    const TInt KHuiEnvDefaultNormalRefreshIntervalMs = 40;
    const TInt KHuiEnvDefaultBusyRefreshIntervalMs = 40;
    #else
    const TInt KHuiEnvDefaultNormalRefreshIntervalMs = 15;
    const TInt KHuiEnvDefaultBusyRefreshIntervalMs = 15;
    #endif
#else
const TInt KHuiEnvDefaultNormalRefreshIntervalMs = 20;
const TInt KHuiEnvDefaultBusyRefreshIntervalMs = 2 * KHuiEnvDefaultNormalRefreshIntervalMs;
#endif


#ifndef SYMBIAN_BUILD_GCE
const TUint KHuiEnvMaxCpuTime = 100;
#endif
const TUint KRndTexMemCalcFlag = 0x800;

/* If there is idle between frames, this is how much we can use as overdrive max cpu utilisation */
const TUint KHuiEnvMaxCpuTimeOverdriveMaxValue = 100;

/* If max cpu usage has been set below this value, overdrive is not used because there is probaply a good reson 
for a low max cpu usage value */
const TUint KHuiEnvMaxCpuTimeOverdriveLowerThreshold = 50;


/** Threshold number for refreshes that don't have any effect. When exceeded,
    refresh is paused. */
const TInt KIdleRefreshCountThreshold = 3;

const THuiRefreshMode KHuiDefaultRefreshMode = EHuiRefreshModeAutomatic;
class CHighResTimer : public CTimer
	{
public:
	static CHighResTimer* NewL(TCallBack aCallBack, TInt aPriority)
		{
		CHighResTimer* self = new(ELeave) CHighResTimer(aCallBack, aPriority);
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop(self);
		return self;
		}
	
	~CHighResTimer()
		{
		Cancel();
		}

	void CallBack(const TInt aWaitInMilliSecs)
		{
		TInt wait = 1000*aWaitInMilliSecs;
		if (wait < 0)
		    {
		    wait = 15000;
		    }
		else if (wait > 60000)
		    {
		    wait = 60000;
		    }
		
		HighRes( TTimeIntervalMicroSeconds32( wait ) );
		}
	
private:
	CHighResTimer(TCallBack aCallBack, TInt aPriority) 
	: CTimer(aPriority), iCallBack(aCallBack)
		{
		CActiveScheduler::Add(this);
		}

	void RunL()
		{
		iCallBack.CallBack();
		}
	
	TCallBack iCallBack;
	};

EXPORT_C CHuiEnv* CHuiEnv::NewL(THuiRenderer aRenderer, MHuiEnvObserver* aObserver)
    {
    CHuiEnv* self = CHuiEnv::NewLC(aRenderer, aObserver);
    CleanupStack::Pop(self);
    return self;
    }


EXPORT_C CHuiEnv* CHuiEnv::NewLC(THuiRenderer aRenderer, MHuiEnvObserver* aObserver)
    {
    CHuiEnv* self = new (ELeave) CHuiEnv(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aRenderer);

    return self;
    }


CHuiEnv::CHuiEnv(MHuiEnvObserver* aObserver)
        : iState(ENormal),
          iRefreshMode(KHuiDefaultRefreshMode),
          iRefreshInterval(KHuiEnvDefaultNormalRefreshIntervalMs),
          iRefreshIntervalTarget(KHuiEnvDefaultNormalRefreshIntervalMs),
          iEnvObserver(aObserver),
          iIdleThreshold(10),
          iMaxCPUUtilization(0), // disable adaptive scheduling by default
          iRefreshLoopPriority(CActive::EPriorityStandard),
          iRefreshLoopActive(EFalse)
    {
    iIdleCPUValueMonitored = CPUTimeSupported() && OpenHandleToIdleCPUValueThread();
    }

// to get around nasty ownership problem with texture manager interface
void NullTextureManagerPtr(TAny* aPtrToPtr)
    {
    if (aPtrToPtr)
        {
        CHuiTextureManager** ptr = (CHuiTextureManager**)aPtrToPtr;
        *ptr = 0;
        }
    }

void CHuiEnv::ConstructL(THuiRenderer aRenderer)
    {
    HUI_DEBUG(_L("CHuiEnv::ConstructL() - Constructing Hitchcock User Interface Toolkit (HUITK) working environment") );
    HUI_DEBUG1(_L("CHuiEnv::ConstructL() - Free memory in beginning: %i"), HuiUtil::FreeMemory());

    // Static data area.
    iStatic = CHuiStatic::NewL(this);
    HUI_DEBUG1(_L("CHuiEnv::ConstructL() - Free memory after static created: %i"), HuiUtil::FreeMemory());

    // Instantiate the right type of renderer plugin.
    
    
    // The Ecom plugin framework requires the identifier to be a descriptor.
    // Convert the identifier to descriptor, which the ecom uses to determine
    // the correct renderer.

    // -----------RnD-------------------------------------------------------------
    // Disable renderer selection for now !!
    // -----------RnD-------------------------------------------------------------

    // can't currently switch on fly and alf_config has its issues..
    aRenderer = EHuiRendererVg10; 
    

    TBuf<10> renderer;
    renderer.Num(aRenderer);

    // The aRenderer holds the renderer information which is usually gotten from
    // central repository key. It might not be defined though, and in that case,
    // we'll try to load from the 
    switch(aRenderer)
        {
        case EHuiRendererDefault:
            {
            const THuiRenderer rendererTable[] =
                {
                EHuiRendererVg10,
                EHuiRendererBitgdi,
                EHuiRendererGles20,
                EHuiRendererGles11,
                EHuiRendererGles10,
                };
            const TInt KRendererCount = 5;        
            for (TInt index = 0; index <  KRendererCount ; index++)
                {
                renderer.Num(rendererTable[index]);
                TRAPD(error, iRenderer = reinterpret_cast<CHuiRenderPlugin*>(CHuiRenderPluginBase::NewL( renderer )));
                if (error)
                    {
                    HUI_DEBUG1(_L("CHuiEnv::ConstructL() - Renderer construction failed, error code: %i, trying next one...."),error );
                    if (index ==  KRendererCount-1) 
                        {
                        // the last renderer in the table failed to load
                        HUI_DEBUG(_L("CHuiEnv::ConstructL() - ERROR! Failed to create any renderer.. bailing out") );
                        User::Leave(error);
                        }
                    continue;                    
                    }
                // got a renderer, use that...
                iRendererIdentifier = rendererTable[index];
                break;
                }
            break;
            }


        default:
            {
            // Load the renderer plugin with the given identifier. The Ecom framework
            // will take care searching for matching renderer. If not found, will leave with KErrNotFound
            iRenderer = reinterpret_cast<CHuiRenderPlugin*>(CHuiRenderPluginBase::NewL( renderer ));
            iRendererIdentifier = aRenderer;
            
            // @TODO Should we panic, if renderer wasn't found, or is the Leave
            // with KErrNotFound enough?
            //THuiPanic::Panic(THuiPanic::EStaticInvalidRenderer);
            break;
            }
        }

    iStatic->SetRenderer(*iRenderer);

    CHuiStatic::Tic(1);

    // Create the scheduler.
    iScheduler = new (ELeave) CHuiScheduler(*this);

    // Create a texture manager.
    
    CleanupStack::PushL(TCleanupItem(NullTextureManagerPtr,&iTextureManager));
    iTextureManager = iRenderer->CreateTextureManagerL(*this);
    // Register as a state change and texture loaded event observer
    CleanupStack::Pop(); 
    iTextureManager->iStateObservers.AppendL(*this);
    iTextureManager->iLoadObservers.AppendL(*this);

    // Create a font manager.
    iFontManager = iRenderer->CreateFontManagerL();
    
    // Create a text style manager.
    iTextStyleManager = CHuiTextStyleManager::NewL();
    iTextStyleManager->ConstructBuiltInStylesL();
    
    // Create a visual factory.
    iVisualFactory = new (ELeave) CHuiVisualFactory(*this);

    // Create an effects engine
    iEffectsEngine = iRenderer->CreateEffectsEngineL();

    // Cache the last input time.
    iLastInputTime = CHuiStatic::Time();

    // Create a shared roster. This is used by all overlaid displays.
    iSharedRoster = new (ELeave) CHuiRosterImpl(NULL);

    /** @todo Create a skin here. This must be done in a better way.
        The skin can only be created when there is a EGL context because
        the skin will attempt to create textures and load fonts. */

    if(!iSkin)
        {
        if(iEnvObserver)
            {
            iSkin = iEnvObserver->CreateSkinL(*this);
            }
        if(!iSkin)
            {
            // Create an instance of the default skin. The S60 skin uses data
            // and bitmaps from the current S60 skin.
            iSkin = new (ELeave) CHuiS60Skin(*this);
            (static_cast<CHuiS60Skin*>(iSkin))->ConstructL();
            }
        }

    // DEPRECATED: JUST A DUMMY CONSTRUCTION TO PREVENT BREAKS
    //iThemeManager = CHuiThemeManager::NewL();

    iCanvasTextureCache = CHuiCanvasTextureCache::NewL();
    iAppIconCache = CAppIconCache::NewL(128);

#ifndef SYMBIAN_BUILD_GCE
#ifndef __WINSCW__
    // for now, display fps information automatically if errrd installed
    if (eikenv)
        {
        _LIT(KRDSupport, "c:\\resource\\errrd" );
        if (eikenv->FsSession().Handle() && BaflUtils::FileExists( eikenv->FsSession(), KRDSupport ))
            {
            SetFPSCounterThreshold(700);
            }
        }
#endif
#endif //SYMBIAN_BUILD_GCE
    HUI_DEBUG(_L("CHuiEnv::ConstructL - Working environment ready."));
    }


CHuiEnv::~CHuiEnv()
    {
    iSynchObservers.Close();
    iSynchIds.Close();
    
    iActionObservers.Close();

    // Destroy groups in reverse order so that references will be removed
    // in natural order.
    TInt i = 0;
    for(i = iLoadedGroups.Count() - 1; i >= 0; --i)
        {
        CHuiControlGroup* g = iLoadedGroups[i];
        iLoadedGroups.Remove(i);
        delete g;
        }
    iLoadedGroups.Reset();

    for(i = iDisplays.Count() - 1; i >= 0; --i)
        {
        // Deleting a display causes it to be automatically removed from the
        // displays array.
        delete iDisplays[i];
        }

    iDisplays.Reset();
    iOverlaidDisplays.Reset();

	delete iAppIconCache;
    delete iCanvasTextureCache;

    delete iSharedRoster;
    delete iSkin;
    delete iVisualFactory;
    delete iTextureManager;
    delete iEffectsEngine;

    // DEPRECATED: JUST A DUMMY DESTRUCTION
    //delete iThemeManager;	//Theme support

    delete iFontManager;
    delete iTextStyleManager;
    delete iPeriodic;
    delete iScheduler;
    iScheduler = NULL;
    delete iRenderer;
    delete iStatic;
    iEnvObserver = NULL;
    
    if (iIdleCPUValueMonitored)
        {
        CloseHandleToIdleCPUValueThread();            
        }
    
    iLowMemoryObservers.Close();
    iMemoryLevelObservers.Close();
    }


void CHuiEnv::SetTextureManager(CHuiTextureManager& aManager)
    {
    iTextureManager = &aManager;
    }


EXPORT_C THuiRenderer CHuiEnv::Renderer() const
    {
    return iRendererIdentifier;
    }


EXPORT_C void CHuiEnv::SetRefreshMode(THuiRefreshMode aMode)
    {
    switch (aMode)
        {
        case EHuiRefreshModeManual:
               //HUI_DEBUG(_L("CHuiEnv::SetRefreshMode() - Setting manual refresh mode."));
               break;
        case EHuiRefreshModeAutomatic:
               //HUI_DEBUG(_L("CHuiEnv::SetRefreshMode() - Setting automatic refresh mode."));
               break;
        case EHuiRefreshModeForced:
               //HUI_DEBUG(_L("CHuiEnv::SetRefreshMode() - Setting forced refresh mode."));
               break;
        default:
               //HUI_DEBUG(_L("CHuiEnv::SetRefreshMode() - ERROR! Tried to set unknown refresh mode! Ignoring.."));
               return;
        }


#ifdef HUI_NGA_FORCED_REFRESH
    iRefreshMode = EHuiRefreshModeForced;
#else
    iRefreshMode = aMode;
#endif
    
    if(aMode == EHuiRefreshModeManual)
        {
        StopRefresh();
        }
    else
        {
        StartRefresh(iRefreshIntervalTarget);
        }
    }


EXPORT_C THuiRefreshMode CHuiEnv::RefreshMode() const
    {
    return iRefreshMode;
    }


EXPORT_C void CHuiEnv::SetMaxFrameRate(TReal32 aFrameRate) __SOFTFP
    {
#ifdef HUI_NGA_MIN_REFRESH_INTERVAL
    if (aFrameRate < (1000/KHuiEnvDefaultNormalRefreshIntervalMs))
        aFrameRate = 1000/KHuiEnvDefaultNormalRefreshIntervalMs;
#endif
    
    TInt oldIRefreshIntervalTarget = iRefreshIntervalTarget;
    if(aFrameRate != 0.0)
        {
        iRefreshIntervalTarget = TInt(1000 / aFrameRate);
        }
    else
        {
        iRefreshIntervalTarget = KHuiEnvDefaultNormalRefreshIntervalMs;
        }

    if(iRefreshIntervalTarget < 1)
        {
        iRefreshIntervalTarget = 1;
        }

    // If the refresh timer is running, update it.
    if(!iMaxCPUUtilization &&  /* Adaptive scheduling not enabled */ 
       iPeriodic && (iPeriodic->IsActive() || iRefreshLoopActive ) && /* and heart beat already active */
       oldIRefreshIntervalTarget != iRefreshIntervalTarget ) 
        {
        StartRefresh(iRefreshIntervalTarget);
        }
    }


EXPORT_C void CHuiEnv::StartRefresh(TInt aMilliSeconds)
    {
    StartRefresh(aMilliSeconds, aMilliSeconds);
    }

void CHuiEnv::StartRefresh(TInt aIntervalInMilliSeconds, TInt aDelayInMilliseconds)
    {
    // Cancel the previous timer.
    StopRefresh();

    if(iState == EReleased)
        {
        // When environment is released no refresh is done.
        return;
        }
        
    if ( aIntervalInMilliSeconds < 1 )
        {
        HUI_DEBUG1(_L("CHuiEnv::StartRefresh() - Requested %i ms - too low!."), aIntervalInMilliSeconds);
        aIntervalInMilliSeconds = 1;
        }
   
    if (aIntervalInMilliSeconds > 1000)
        {
        HUI_DEBUG1(_L("CHuiEnv::StartRefresh() - Requested %i ms - too high!."), aIntervalInMilliSeconds);
        aIntervalInMilliSeconds = 1000;    
        }

    if ( aDelayInMilliseconds < 1 )
        {
        HUI_DEBUG1(_L("CHuiEnv::StartRefresh() - Requested %i aDelayInMilliseconds ms - too low!."), aDelayInMilliseconds);
        aDelayInMilliseconds = 1;
        }
   
    if (aDelayInMilliseconds > 1000)
        {
        HUI_DEBUG1(_L("CHuiEnv::StartRefresh() - Requested %i aDelayInMilliseconds ms - too high!."), aDelayInMilliseconds);
        aDelayInMilliseconds = 1000;    
        }
    
    
    //HUI_DEBUG1(_L("CHuiEnv::StartRefresh() - Starting at %i ms interval."), aMilliSeconds);

    // Create a new refresh timer.
    
    TInt err(0);
    if (!iPeriodic)
        {
#ifdef HUI_HIRES_TIMER
        TRAP(err, iPeriodic = CHighResTimer::NewL(TCallBack(CHuiEnv::RefreshCallBack, this), iRefreshLoopPriority));
#else
        TRAP( err, iPeriodic = CPeriodic::NewL(iRefreshLoopPriority) );
#endif
        }

    if (!err)
        {
#ifdef HUI_HIRES_TIMER
        iPeriodic->CallBack( aIntervalInMilliSeconds );
#else 
        iPeriodic->Start(TTimeIntervalMicroSeconds32(1000 * aDelayInMilliseconds),
                         TTimeIntervalMicroSeconds32(1000 * aIntervalInMilliSeconds),
                         TCallBack(CHuiEnv::RefreshCallBack, this));  
#endif
        iRefreshLoopActive = ETrue;

        // Store current refresh interval                   
        iRefreshInterval = aIntervalInMilliSeconds; 
        }
    }

void CHuiEnv::StopRefresh()
    {
    //HUI_DEBUG(_L("CHuiEnv::StopRefresh()"));
    if(iPeriodic)
        {
        iPeriodic->Cancel();
        iRefreshLoopActive = EFalse;
        }
    }


EXPORT_C void CHuiEnv::ContinueRefresh()
    {
    ContinueRefresh(EFalse);
    }

void CHuiEnv::ContinueRefresh(TBool aFastFirstRefresh)
    {
    if((iRefreshMode == EHuiRefreshModeManual) ||
       (iState == EReleased))
        {
        // When running in manual refresh mode, this has no effect. Refreshes
        // must be manually requested.
        // When environment is released no refresh is done.
        return;
        }

    if(!iPeriodic || ( !iPeriodic->IsActive() && !iRefreshLoopActive ) )
        {
        HUI_DEBUG1(_L("CHuiEnv::ContinueRefresh() - Continuing normal refresh with %i ms intervals."),
                   iRefreshInterval);
        
        TInt refreshDelay = aFastFirstRefresh ? 1 : iRefreshIntervalTarget;
        StartRefresh(iRefreshIntervalTarget, refreshDelay);

        // When refresh is paused, the frame rate is virtually infinite
        // since nothing needs to be done on each refresh. This means that
        // when refresh continues, the first elapsed step should be zero.
        // A call to UpdateTime() updates the internal absolute clock with
        // the latest time.
        CHuiStatic::UpdateTime();
        }    
    }

EXPORT_C void CHuiEnv::PauseRefresh()
    {
    HUI_DEBUG(_L("CHuiEnv::PauseRefresh() - Refresh paused due to inactivity."));
    HUI_DEBUG1(_L("CHuiEnv::PauseRefresh() - Free memory left: %i"), HuiUtil::FreeMemory());
    StopRefresh();
    }


EXPORT_C CHuiDisplay&
CHuiEnv::NewDisplayL(const TRect& aRect, CCoeControl* aNativeControl, TInt aFlags, 
					 CHuiDisplay* aDisplayToShareRoster, TInt aDisplayType )
    {
    return NewDisplayL(aRect, aNativeControl, aFlags, aDisplayToShareRoster, aDisplayType, KHuiUidBackBufferScreen0);
    }


EXPORT_C CHuiDisplay&
CHuiEnv::NewDisplayL(const TRect& aRect, CCoeControl* aNativeControl, TInt aFlags, 
					 CHuiDisplay* aDisplayToShareRoster, TInt aDisplayType, TUid aBackBufferUid)
    {
    TBool isDisplayTypeTvOut = 
    	((aDisplayType == CHuiDisplay::EDisplayTvOut) || 
   	     (aDisplayType == CHuiDisplay::EDisplayTvOutWide) ||
   	     ((aDisplayType == CHuiDisplay::EDisplayNormal) && 
   	     ((aBackBufferUid == KHuiUidBackBufferTvOutNormal) ||
   	      (aBackBufferUid == KHuiUidBackBufferTvOutWide))));

    if ((iRendererIdentifier != EHuiRendererBitgdi) &&
    	(isDisplayTypeTvOut ||  (aDisplayType == CHuiDisplay::EDisplayOffScreenBuffer)))
    	{
    	User::Leave(KErrNotSupported);
    	}
    
    CHuiRoster* roster = NULL;
    if (aDisplayToShareRoster) 
    	{
    	roster = &aDisplayToShareRoster->Roster();
    	}
    if(aFlags & ENewDisplayOverlaid)
        {
        roster = iSharedRoster;
        }

    CHuiDisplay* display = new (ELeave) CHuiDisplay(*this, aNativeControl, aDisplayType, aBackBufferUid);
    CleanupStack::PushL(display);
    display->ConstructL(aRect, roster);
    User::LeaveIfError(iDisplays.Append(display));
    CleanupStack::Pop(display);

    // Notify the current renderer of the changed number of displays
    iRenderer->NotifyDisplayCountL(iDisplays.Count());

    HUI_DEBUG2(_L("HuiEnv::NewDisplay %x added to array, with flags %x"), display, aFlags);
    
    // Notify observers about new TvOut display
    if(isDisplayTypeTvOut)
        {
        ReportAction(THuiActionCommand(KHuiActionNewTVOutDisplayUid.iUid));
        }

    return *display;
    }

EXPORT_C CHuiDisplay& CHuiEnv::NewDisplayL(RWindow* aNativeWindow,
                                      TInt aFlags,
                                      CHuiDisplay* aDisplayToShareRoster, 
                                      TInt aDisplayType, 
                                      TUid aBackBufferUid)
    {
    CHuiRoster* roster = NULL;
    if (aDisplayToShareRoster) 
    	{
    	roster = &aDisplayToShareRoster->Roster();
    	}
    if(aFlags & ENewDisplayOverlaid)
        {
        roster = iSharedRoster;
        }

    CHuiDisplay* display = new (ELeave) CHuiDisplay(*this, aNativeWindow, aDisplayType, aBackBufferUid);
    CleanupStack::PushL(display);
    display->ConstructL(TRect(TPoint(0,0),aNativeWindow->Size()), roster);
    User::LeaveIfError(iDisplays.Append(display));
    CleanupStack::Pop(display);

    // Notify the current renderer of the changed number of displays
    iRenderer->NotifyDisplayCountL(iDisplays.Count());

    HUI_DEBUG2(_L("HuiEnv::NewDisplay %x added to array, with flags %x"), display, aFlags);
    
    return *display;
    }


EXPORT_C TInt CHuiEnv::DisplayCount() const
    {
    return iDisplays.Count();
    }


void CHuiEnv::RemoveDisplay(CHuiDisplay& aDisplay)
    {
    // Actually this is "DestroyDisplay"

    /** @todo  Make a proper observer. */
    TInt index = iDisplays.Find(&aDisplay);
    if(index >= 0)
        {
        iDisplays.Remove(index);
        }
    else
        {
        HUI_DEBUG1(_L("HuiEnv::DestroyContext: display %x not in array"), &aDisplay);
        }
    index = iOverlaidDisplays.Find(&aDisplay);
    if(index >= 0)
        {
        iOverlaidDisplays.Remove(index);
        }

    // Notify the current renderer of the changed number of displays
    TRAP_IGNORE(iRenderer->NotifyDisplayCountL(iDisplays.Count()))

#ifdef _DEBUG
    HUI_DEBUG(_L("  Contents of iDisplays:"));
    for(TInt i = 0; i < iDisplays.Count(); ++i)
        {
        HUI_DEBUG2(_L("  %i: %x"), i, iDisplays[i]);
        }
#endif
    }


EXPORT_C TInt CHuiEnv::RefreshCallBack(TAny* aInstance)
    {
    ASSERT( aInstance );
    CHuiEnv* self = static_cast<CHuiEnv*>( aInstance );
    
    TReal32 elapsedTime = 0;

    // Investigate whether the environment is released
    if(self->iState == EReleased /*|| self->iPauseDrawing*/ )
        {
        if (!self->iPauseDrawing)
            {    
            HUI_DEBUG(_L("CHuiEnv::RefreshCallBack() - Warning: Refresh callback called while environment is released."));
            }        
        return KErrNone;
        }

    self->SetTimeFromLastUpdate(CHuiStatic::MilliSecondsSinceUpdateTime());
    // Compute the elapsed time (in sec.) between the current and the
    // previous refresh call.
    CHuiStatic::UpdateTime();
    elapsedTime = CHuiStatic::ElapsedSeconds();

    // Time advancements should not happen in too large increments.
    /** @todo The time advancement should be done in steps if the
              elapsed time is too long. The static time in CHuiStatic
              should also be updated using the same steps. This will
              allow scheduled commands to be executed and timed
              correctly (start times of timed transitions).  */

    HUI_DEBUGF1(_L("CHuiEnv::RefreshCallBack() - Elapsed time: %f"), elapsedTime);

    if (elapsedTime < 0)
        {
        elapsedTime = 1;
        }

    self->AdvanceTime(elapsedTime);

    // Report the current frame rate at certain intervals.
    TReal32 nowTime = CHuiStatic::SecondsSinceStart();
    if(nowTime - self->iPreviousFpsTime > 3)
        {
        HUI_DEBUG1(_L("CHuiEnv::RefreshCallBack() - FPS: %f"), CHuiStatic::FrameRate());
        self->iPreviousFpsTime = nowTime;
        }

    return KErrNone;
    }


void CHuiEnv::AdvanceTime(TReal32 aElapsedTime)
    {
    HUI_DEBUGF( _L("CHuiEnv::AdvanceTime() - Started") );
#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
    TTime startTime;
    TTime endTime;
#endif

#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
    RDebug::Print(_L("CHuiEnv::AdvanceTime starting measuring 0."));
    startTime.UniversalTime();
#endif
    
    
    TUint usedMaxCPUUtilization = iMaxCPUUtilization;

    // Calculate cpu values based on null thread cpu usage between frames. 
    // Values will be updated at the end of the frame.
    if (iIdleCPUValueMonitored && usedMaxCPUUtilization)
        {
        TTime currentTime;
        currentTime.HomeTime();

        TTimeIntervalMicroSeconds cputime;
        iIdleCPUValueThread.GetCpuTime(cputime);

        TInt64 cpudelta = cputime.Int64() - iIdleCPUValue;
        TInt64 timedelta = currentTime.Int64() - iIdleCPUValuePreviousTime;

        // If null thread was runnign between frames, we could use more cpu if needed.
        if (cpudelta && timedelta)
            {
            // Calculate how much we want !
            if (iMaxCPUUtilization > KHuiEnvMaxCpuTimeOverdriveLowerThreshold &&
                iMaxCPUUtilization < KHuiEnvMaxCpuTimeOverdriveMaxValue )
                {
                usedMaxCPUUtilization += (KHuiEnvMaxCpuTimeOverdriveMaxValue - iMaxCPUUtilization) * cpudelta/timedelta;
                
                // Sanity check just in case cpu/time measurements are not accurate
                if (usedMaxCPUUtilization > KHuiEnvMaxCpuTimeOverdriveMaxValue)
                    {
                    usedMaxCPUUtilization = KHuiEnvMaxCpuTimeOverdriveMaxValue;   
                    }
                
                if (usedMaxCPUUtilization < iMaxCPUUtilization)
                    {
                    usedMaxCPUUtilization = iMaxCPUUtilization;    
                    }                                    
                }
            }
        }

    if(aElapsedTime > 0)
        {
        // Let the scheduler know that time has passed. It will possible animate
        // visuals and perform actions, causing dirty regions in the display.
        iScheduler->AdvanceTime(aElapsedTime);

        // Scheduled command might have released the environment.
        if(iState == EReleased)
            {
            HUI_DEBUG(_L("CHuiEnv::AdvanceTime() - Environment released when executing scheduled commands. AdvanceTime cancelled."));
            return;
            }

        // Let the texture manager know that time has passed. It will update any
        // animated textures.
        iTextureManager->AdvanceTime(aElapsedTime);                
        // Notify the effects engine to update animated effects
        if (iEffectsEngine)
            {
            iEffectsEngine->AdvanceTime(aElapsedTime);

            // Notify the effects end observers
            iEffectsEngine->NotifyEffectEndObservers();
            }
        }

    // Check for no input for long time.
    TTime now = CHuiStatic::Time();
    TTimeIntervalSeconds seconds = 0;
    now.SecondsFrom(iLastInputTime, seconds);
    if(seconds.Int() >= iIdleThreshold && !iInputIdleIsActive)
        {
        HUI_DEBUG1(_L("CHuiEnv::AdvanceTime() - No input received within %i seconds. Going to idle."), iIdleThreshold);
        iInputIdleIsActive = ETrue;
        
        // Idle state begins.
        TRAPD(err, SendIdleL(ETrue));
        if(err != KErrNone)
            {
            // @todo  Log error?
            }
            
        // Switching to idle state might have released the environment.
        if(iState == EReleased)
            {
            HUI_DEBUG(_L("CHuiEnv::AdvanceTime() - Environment released when switching to idle state. AdvanceTime cancelled."));
            return;
            }            
        }

    TBool somethingUpdated = EFalse;

    // Refresh all displays.
    TInt i;
    const TInt displayCount = iDisplays.Count();
    RArray<TBool> displayRefreshed( displayCount ? displayCount : 1 );
    for(i = 0; i < iDisplays.Count(); ++i)
        {
        displayRefreshed.Append(EFalse);
        if(iRefreshMode == EHuiRefreshModeForced || iDisplays[i]->IsDirty())
            {
            MakeCurrent(*iDisplays[i]);

            HUI_DEBUGF1( _L("CHuiEnv::AdvanceTime() - Refreshing display %i"), i );
            TBool updated = iDisplays[i]->Refresh();
            displayRefreshed[i] = updated;
            if(updated)
                {
                somethingUpdated = ETrue;
                }
            }
        }

#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
    endTime.UniversalTime();
    TInt timeInMs =  endTime.MicroSecondsFrom( startTime ).Int64()/1000;           
    RDebug::Print(_L("CHuiEnv::AdvanceTime first part %i ms"), timeInMs);
#endif
    
    TBool continueRefresh = ETrue;

    if(somethingUpdated)
        {

#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
    RDebug::Print(_L("CHuiEnv::AdvanceTime starting measuring 2."));
    startTime.UniversalTime();
#endif
    
        iIdleRefreshCount = 0;
        // Clear change flags now that the frames are complete.
        for(i = 0; i < iDisplays.Count(); ++i)
            {
            // Clear changed for an off screen display only if the buffer has new content.
            // Index is ok becacause displayRefreshed array was defined using size of iDisplays array
            if (displayRefreshed[i])
            	{
            	iDisplays[i]->ClearChanged();
            	}
            }
        }
    else if(iScheduler->PendingCount() == 0)
        {
        // But if there are scheduled commands, let's make sure they'll get
        // executed at the right time. They might get badly delayed if the
        // refresh wasn't occuring.

        /** @todo  Use a separate timer for the scheduler? */

        // Nothing happened during the display refreshing.
        iIdleRefreshCount++;

        // If this occurs too often, pause refresh automatically.
        if(iIdleRefreshCount > KIdleRefreshCountThreshold)
            {
            if (iFpsCounterThreshold && iMillisecondFromFPSUpdate && iFrames)
                {
                TBuf<16> numBuf;
                TReal fps = 1000*(TReal)iFrames/iMillisecondFromFPSUpdate;
                numBuf.AppendNum(fps, TRealFormat(5,2));
                User::InfoPrint(numBuf);
                iFrames = 0;
                iMillisecondFromFPSUpdate = 0;
                }

            PauseRefresh();
            continueRefresh = EFalse;
            }
        }
    else
        {
        // for PC lint
        }

    if(somethingUpdated)
        {
        HUI_DEBUGF( _L("CHuiEnv::AdvanceTime() - Swap buffers") );

#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
    RDebug::Print(_L("CHuiEnv::AdvanceTime starting measuring 1."));
    startTime.UniversalTime();
#endif

        SwapBuffers(displayRefreshed);

#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
    endTime.UniversalTime();
    TInt timeInMs =  endTime.MicroSecondsFrom( startTime ).Int64()/1000;           
    RDebug::Print(_L("CHuiEnv::AdvanceTime swapbuffers took %i ms"), timeInMs);
#endif
        }

    displayRefreshed.Close(); // Not needed any more
    
#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
    endTime.UniversalTime();
    timeInMs =  endTime.MicroSecondsFrom( startTime ).Int64()/1000;           
    RDebug::Print(_L("CHuiEnv::AdvanceTime iDisplays[i]->ClearChanged() took %i ms"), timeInMs);
#endif
    
#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
    RDebug::Print(_L("CHuiEnv::AdvanceTime starting measuring 3."));
    startTime.UniversalTime();
#endif
	
	
    // Clear change flags of all control groups now that the refresh has
    // been completed for all displays.
    //
    // DEPRECATE:
    // This should be removed when control opacities are deprecated! 
    // Controls shouldn't need change flags because change flags are 
    // only for the refresh.
    //
    for(i = 0; i < iLoadedGroups.Count(); ++i)
        {
        iLoadedGroups[i]->ClearChanged();
        }

    iTextureManager->ClearChangedTextures();
    iCanvasTextureCache->AdvanceTime();

#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
    endTime.UniversalTime();
    timeInMs =  endTime.MicroSecondsFrom( startTime ).Int64()/1000;           
    RDebug::Print(_L("CHuiEnv::AdvanceTime textures ClearChanged() and other stuff took %i ms"), timeInMs);
#endif
    
    if (continueRefresh)
        {
        TUint refreshTime = CHuiStatic::MilliSecondsSinceUpdateTime();
        // Refresh rate adjustment
        if (usedMaxCPUUtilization)
            {
			// Estimated frame, when sleep time is included. 
            TUint estimatedTotalRefreshTime = (refreshTime * 100) / usedMaxCPUUtilization; 
    
            if (estimatedTotalRefreshTime >= iRefreshIntervalTarget) 
                {
                // Rendering slower that fps limit set by user.
                // Start refresh after the sleep period as the frame has already been rendered.
                StartRefresh(estimatedTotalRefreshTime - refreshTime);        
                }
#ifndef HUI_HIRES_TIMER
            else if (iRefreshIntervalTarget != iRefreshInterval)
                {
                StartRefresh(iRefreshIntervalTarget);			
                }
#endif            
            else
                {
                // Rendering faster than the fps limit set by user. Limit fps to target fps.
                // The sleep time is the full frame time less by the render time. 
#ifdef HUI_HIRES_TIMER
                StartRefresh( iRefreshIntervalTarget - refreshTime );
#endif
                }
            }
        else
        	{
            // No CPU utilisation.
            // The sleep time is the full frame time less by the render time. 
#ifdef HUI_HIRES_TIMER
            StartRefresh( iRefreshIntervalTarget - refreshTime );
#endif
        	}

        if (iFpsCounterThreshold && iMillisecondFromFPSUpdate > iFpsCounterThreshold)
            {
            TBuf<16> numBuf;
            TReal fps = 1000*(TReal)iFrames/iMillisecondFromFPSUpdate;
            numBuf.AppendNum(fps, TRealFormat(5,2));
            User::InfoPrint(numBuf);
            iFrames = 0;
            iMillisecondFromFPSUpdate = 0;
            }
        }

    DoSynchronize();
        
    iCurrentDisplay = NULL; // informs the egosystem that the drawing is done.
    CHuiStatic::ReportNewFrame();
    
    // Store cpu value conters of null thread. Values will be used at the start of the next frame.
    if (iIdleCPUValueMonitored)
        {
        TTime currentTime;
        currentTime.HomeTime();

        TTimeIntervalMicroSeconds cputime;
        iIdleCPUValueThread.GetCpuTime(cputime);

        // Store as previous values
        iIdleCPUValue = cputime.Int64();
        iIdleCPUValuePreviousTime = currentTime.Int64();                           
        }
            
    HUI_DEBUGF( _L("CHuiEnv::AdvanceTime() - Exited") );
    }


EXPORT_C TBool CHuiEnv::HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType,
                                        CHuiDisplay* aAssocDisplay)
    {
    if(aAssocDisplay)
        {
        THuiEvent event(aAssocDisplay, aKeyEvent, aType);

        // Notify the environment itself.
        NotifyInputReceivedL(event);

        if(aAssocDisplay->Roster().HandleEventL(event))
            {
            return EKeyWasConsumed;
            }
        return EKeyWasNotConsumed;
        }

    return EKeyWasNotConsumed;
    }


void CHuiEnv::NotifyInputReceivedL(const THuiEvent& aEvent)
    {
    ContinueRefresh();

    if(aEvent.IsKeyEvent() || aEvent.IsPointerEvent())
        {
        if(iInputIdleIsActive)
            {
            HUI_DEBUG(_L("CHuiEnv::NotifyInputReceivedL() - Got key/pointer input! Idle state ends!"));
            // Idle state ends.
            SendIdleL(EFalse);
            }

        iLastInputTime = CHuiStatic::Time();
        iInputIdleIsActive = EFalse;
        }
    }


void CHuiEnv::SendIdleL(TBool aIdleBegins)
    {
    CHuiDisplay* display = NULL;
    
    if(iDisplays.Count() != 0)
        {
        // If we have any displays, pass the first one.
        display = iDisplays[0];
        }
    
    THuiEvent idleEvent(display,
        aIdleBegins ? THuiEvent::ETypeIdleBegin :
                      THuiEvent::ETypeIdleEnd);
    BroadcastEventL(idleEvent);
    }


EXPORT_C void CHuiEnv::SetIdleThreshold(TInt aMilliSeconds)
    {
    iIdleThreshold = aMilliSeconds/1000;
    }


EXPORT_C void CHuiEnv::BroadcastEventL(const THuiEvent& aEvent)
    {
    for(TInt i = 0; i < iDisplays.Count(); ++i)
        {
        THuiEvent event = aEvent;
        event.iDisplay = iDisplays[i];
        /** @todo  Musn't access the roster directly. */
        iDisplays[i]->Roster().HandleEventL(event);
        }
    }


void CHuiEnv::MakeCurrent(const CHuiDisplay& aDisplay) const
    {
    aDisplay.RenderSurface().MakeCurrent();
    iCurrentDisplay = const_cast<CHuiDisplay*>(&aDisplay);
    }


void CHuiEnv::SwapBuffers(const RArray<TBool>& aDisplayRefreshed)
    {
    /** @todo This may not work as expected when multiple displays are
        being used. */

    if (iFpsCounterThreshold)
        {
        iFrames++;
        iMillisecondFromFPSUpdate += iRefreshIntervalReal;
        }    

    /** @todo  Only swap the visible displays. */

    for(TInt i = 0; i < iDisplays.Count(); ++i)
        {          
        // Index should ok becacause displayRefreshed array was defined using size of iDisplays array
        // This function should only be called from inside CHuiEnv even if it is public.
        // At least it is not exported.
        if (aDisplayRefreshed[i] 
            && (iDisplays[i]->DisplayType() != CHuiDisplay::EDisplayOffScreenBuffer)
            && (iDisplays[i]->ScreenBufferObserver() == NULL))
        	{
            MakeCurrent(*iDisplays[i]);
            if (iSwapObserver)
                iSwapObserver->PrepareSwap();
     		iDisplays[i]->RenderSurface().SwapBuffers();
            if (iSwapObserver)
                iSwapObserver->SwapComplete();
        	
        	}
        }
    }


void CHuiEnv::CreateResourceReaderLC(TResourceReader& aReader, TInt aResourceId) const
    {
    CCoeEnv* coe = CCoeEnv::Static();
    if (!coe)
    		{
    		User::Leave(KErrNotSupported);
    	  }
    coe->CreateResourceReaderLC(aReader, aResourceId);
    }


EXPORT_C CHuiControlGroup& CHuiEnv::NewControlGroupL(TInt aId)
    {
    CHuiControlGroup* group = new (ELeave) CHuiControlGroup(aId, *this);
    CleanupStack::PushL(group);
    User::LeaveIfError(iLoadedGroups.Append(group));
    CleanupStack::Pop(group);
    return *group;
    }


EXPORT_C TInt CHuiEnv::DeleteControlGroup(TInt aId)
    {
    TInt i;

    for(i = 0; i < iLoadedGroups.Count(); ++i)
        {
        if(iLoadedGroups[i]->ResourceId() == aId)
            {
            // This is control group to delete.
            CHuiControlGroup* group = iLoadedGroups[i];
            CancelCommands(group);

            for (TInt ii = iDisplays.Count()-1; ii>=0; ii--)
                {
                TInt index = iDisplays[ii]->Roster().Find(group);
                if (index != KErrNotFound)
                    {
                    iDisplays[ii]->Roster().Hide(iDisplays[ii]->Roster().ControlGroup(index));
                    }
                }
            
            iLoadedGroups.Remove(i);
            delete group;
            return KErrNone;
            }
        }

    return KErrNotFound;
    }


EXPORT_C CHuiControlGroup& CHuiEnv::LoadControlGroupL(TInt aResourceId)
    {
    CHuiControlGroup* group = new (ELeave) CHuiControlGroup(aResourceId, *this);
    CleanupStack::PushL(group);

    // Create a resource reader and construct the group and its controls.
    TResourceReader reader;
    CreateResourceReaderLC(reader, aResourceId);
    group->ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy(); // reader

    // Add the control group to the list of all existing groups.
    // The environment will retain ownership of the group.
    User::LeaveIfError( iLoadedGroups.Append(group) );

    CleanupStack::Pop(group);
    return *group;
    }


EXPORT_C void CHuiEnv::LoadBitmapsL(TInt aResourceId)
    {
    TResourceReader reader;
    CreateResourceReaderLC(reader, aResourceId);

    TInt count = reader.ReadInt16();
    HUI_DEBUG2(_L("CHuiEnv::LoadBitmapsL() - Registering %i bitmaps from resource id %i to the texture manager."), count, aResourceId);

    TInt i = 0;
    CHuiTextureManager & manager = TextureManager();
    for(i = 0; i < count; ++i)
        {
        TInt id = reader.ReadInt32();
        TPtrC fileName = reader.ReadTPtrC();
        HUI_DEBUG2(_L("CHuiEnv::LoadBitmapsL() - Bitmap id %i: \"%S\""), id, &fileName);
        manager.DefineFileNameL(id, fileName);
        }

    CleanupStack::PopAndDestroy(); // reader
    }


EXPORT_C CHuiControlGroup& CHuiEnv::ControlGroup(TInt aResourceId)
    {
    for(TInt i = 0; i < iLoadedGroups.Count(); ++i)
        {
        if(iLoadedGroups[i]->ResourceId() == aResourceId)
            {
            return *iLoadedGroups[i];
            }
        }

    // Group hasn't been loaded yet
    /** @todo load automatically? */
    THuiPanic::Panic(THuiPanic::EInternal);
    return *iLoadedGroups[0];
    }


EXPORT_C CHuiControl* CHuiEnv::FindControl(TInt aId) const
    {
    for(TInt i = 0; i < iLoadedGroups.Count(); ++i)
        {
        CHuiControl* control = iLoadedGroups[i]->FindControl(aId);
        if(control)
            {
            return control;
            }
        }
    // The control does not exist.
    return NULL;
    }


EXPORT_C CHuiVisualFactory& CHuiEnv::VisualFactory() const
    {
    return *iVisualFactory;
    }


EXPORT_C CHuiTextureManager& CHuiEnv::TextureManager() const
    {
    return *iTextureManager;
    }


EXPORT_C THuiFontManager& CHuiEnv::FontManager() const
    {
    return *iFontManager;
    }


EXPORT_C CHuiTextStyleManager& CHuiEnv::TextStyleManager() const
    {
    return *iTextStyleManager;
    }


EXPORT_C CHuiSkin& CHuiEnv::Skin() const
    {
    return *iSkin;
    }


EXPORT_C void CHuiEnv::NotifySkinChangedL()
    {
    ReportAction(THuiActionCommand(KAknsMessageSkinChange));
    for(TInt i = 0; i < iDisplays.Count(); ++i)
        {
        iDisplays[i]->RosterImpl().NotifySkinChangedL();
        }
    iTextureManager->NotifySkinChangedL();
    }


EXPORT_C TInt CHuiEnv::Send(const THuiCommand& aCommand, TInt aDelayMilliSeconds)
    {
    TRAPD(err, SendL(aCommand, aDelayMilliSeconds));
    if(err != KErrNone)
        {
        HUI_DEBUG1(_L("CHuiEnv::Send() - Leave %i while posting command."), err);
        }
    return err;
    }


EXPORT_C void CHuiEnv::SendL(const THuiCommand& aCommand, TInt aDelayMilliSeconds)
    {
    ContinueRefresh();
    iScheduler->PostCommandL(aCommand, aDelayMilliSeconds);
    }


EXPORT_C void CHuiEnv::CancelCommands(TAny* aObject)
    {
    if (iScheduler)
        {
        iScheduler->CancelCommands(aObject);
        }
    }


EXPORT_C void CHuiEnv::CancelCommands(MHuiEventHandler* aObject)
    {
    if (iScheduler)
        {
        iScheduler->CancelCommands(aObject);
        }
    }


EXPORT_C void CHuiEnv::CancelCommands(TAny* aObject,
                                      THuiOp aCommandOperation)
    {
    if (iScheduler)
        {
        iScheduler->CancelCommands(aObject, aCommandOperation);
        }
    }


EXPORT_C void CHuiEnv::CancelCommands(TAny* aObject,
                                      THuiCommandType aCommandType,
                                      TInt aParam)
    {
    if (iScheduler)
        {
        iScheduler->CancelCommands(aObject, aCommandType, aParam);
        }
    }


EXPORT_C TInt CHuiEnv::TimeUntilCommand(TAny* aObject,
                                        THuiCommandType aCommandType)
    {
    return iScheduler->TimeUntilCommand(aObject, aCommandType);
    }


void CHuiEnv::TextureLoadingCompleted(CHuiTexture& /*aTexture*/,
                                    TInt /*aTextureId*/,
                                    TInt /*aErrorCode*/)
    {
    // Texture changed flag has been set, visuals should redraw
    // changed textures automatically.
    }


void CHuiEnv::TextureManagerStateChanged(const CHuiTextureManager& aManager)
    {
    if(aManager.State() == CHuiTextureManager::EIdle)
        {
        StartRefresh(iRefreshIntervalTarget);
        }
    else if (!iMaxCPUUtilization)  
        { // only use busy refresh interwall if adaptive scheduling is not enabled
        StartRefresh(KHuiEnvDefaultBusyRefreshIntervalMs);
        }
    else
        {
        // for PC lint
        }
    }


EXPORT_C CHuiDisplay& CHuiEnv::PrimaryDisplay() const
    {
    return *iDisplays[0];
    }


EXPORT_C CHuiDisplay& CHuiEnv::Display(TInt aIndex) const
    {
    return *iDisplays[aIndex];
    }


EXPORT_C void CHuiEnv::Release()
    {
    if(iState == ENormal)
        {
        HUI_DEBUG(_L("CHuiEnv::Release() - Switching to background!"));
        
        ReportAction(THuiActionCommand(KHuiEnvReleasedActionId));
        
        // Pause refresh.
        PauseRefresh();

        // Release the displays.
        for(TInt i = 0; i < iDisplays.Count(); ++i)
            {
            iDisplays[i]->Release();
            }

        // Release the texture manager.
        TBool allTexturesReleased = iTextureManager->Release();

        // Release the rendering plugin only if all the textures are released.
        if ( allTexturesReleased )
            {
            HUI_DEBUG(_L("CHuiEnv::Release() - All textures released - releasing renderer"));
            iRenderer->Release();
            }
        if (iEffectsEngine)
            {
            iEffectsEngine->Release();
            }
        
        // Finally set the new state.
        iState = EReleased;        
        }
    }


EXPORT_C void CHuiEnv::RestoreL()
    {
    if(iState == EReleased)
        {
        HUI_DEBUG(_L("CHuiEnv::RestoreL() - Coming back to foreground!"));

        // Restore the renderer plugin.
        iRenderer->RestoreL();

        // Restore the texture manager.
        iTextureManager->RestoreL();
        if (iEffectsEngine)
            {
            iEffectsEngine->RestoreL();
            }

        HUI_DEBUG(_L("CHuiEnv::RestoreL() - Restoring displays.."));

        // Restore the displays.
        for(TInt i = 0; i < iDisplays.Count(); ++i)
            {
            iDisplays[i]->RestoreL();
            }

        // Set the state to normal
        iState = ENormal;

        ReportAction(THuiActionCommand(KHuiEnvRestoredActionId));

        // Continue refresh.
        ContinueRefresh();

        HUI_DEBUG(_L("CHuiEnv::RestoreL() - Environment restored!"));
        }
    }

EXPORT_C void CHuiEnv::SetMaxCpuTime(TUint aPercent)
    {
    aPercent = aPercent; // to avoid warnings
#ifndef SYMBIAN_BUILD_GCE
    iMaxCPUUtilization = (aPercent < KHuiEnvMaxCpuTime?aPercent:KHuiEnvMaxCpuTime); // Min does not work
#endif
    }

// Forward the respective RnD feature flag to respective components, here to TextureManager
EXPORT_C void CHuiEnv::EnableDebugFlag( TUint aRndFlag )
	{	
	iTextureManager->EnableTexMemoryCalculation( aRndFlag&KRndTexMemCalcFlag );
	}

CHuiScheduler& CHuiEnv::Scheduler()
    {
    return *iScheduler;
    }


TInt CHuiEnv::ReportAction(const THuiActionCommand& aCommand)
    {
    TInt resultError = KErrNone;

    for(TInt i = 0; i < iActionObservers.Count(); ++i)
        {
        TRAPD(err, iActionObservers[i].HandleActionL(aCommand));
        if(err != KErrNone && resultError == KErrNone)
            {
            // The first error code is returned.
            resultError = err;
            }
        }
    return resultError;
    }
    

RPointerArray<CHuiDisplay> CHuiEnv::Displays() const
    {
    return iDisplays;
    }


EXPORT_C TInt& CHuiEnv::GlesRefCounter()
   	{
   	return iGlesRefCounter;
   	}
   	
EXPORT_C CHuiEnv* CHuiEnv::Static()
    {
    return &(CHuiStatic::Env());
    }

EXPORT_C  CHuiThemeManager& CHuiEnv::ThemeManager() const
    {
    return *iThemeManager;   
    }

EXPORT_C void CHuiEnv::SetFPSCounterThreshold(TUint aMilliseconds)
    {
    if (aMilliseconds > 0)
        {
        iFpsCounterThreshold = aMilliseconds;
        }
    else
        {
        iFpsCounterThreshold = 0;
        }
    
    iFrames = 0;
    iMillisecondFromFPSUpdate = 0;
    }

void CHuiEnv::SetTimeFromLastUpdate(TUint aTimeFromLastUpdate)
    {
    iRefreshIntervalReal = aTimeFromLastUpdate;
    }


CHuiDisplay* CHuiEnv::CurrentDisplay() const
    {
    return iCurrentDisplay;    
    }

EXPORT_C void CHuiEnv::ChangeRefreshLoopPriority(CActive::TPriority aPriority)
    {
    iRefreshLoopPriority = aPriority;
    if (iPeriodic)
        {
        TBool wasActive = (iPeriodic->IsActive() || iRefreshLoopActive);
        StopRefresh();
        delete iPeriodic;
        iPeriodic = 0;
        iRefreshLoopActive = EFalse;
        if (wasActive)
            {
            StartRefresh(iRefreshIntervalTarget);
            }
        }
    }

TBool CHuiEnv::CPUTimeSupported()
    {
    TTimeIntervalMicroSeconds time;
    TInt err = RThread().GetCpuTime(time);
    
    if (err == KErrNone && time.Int64() > 0)
        {
        return ETrue;    
        }        
    else
        {
        return EFalse;    
        }        
    }
    
TBool CHuiEnv::OpenHandleToIdleCPUValueThread()
    {
    // find the kernel process and then the null thread
    TFindProcess fp(_L("ekern.exe*"));
    
    TFullName kernelName;
    if (fp.Next(kernelName) == KErrNone)
        {
        // process found, append null thread identifier
        kernelName.Append(_L("::Null"));
        
        // find the thread
        TFindThread ft(kernelName);

        TFullName threadName;
        if (ft.Next(threadName) == KErrNone)
            {
            // open instance to the thread
            if (iIdleCPUValueThread.Open(threadName) != KErrNone)
                {
                return EFalse;                    
                }                
            }
        }        
    else
        {
        // process not found
        return EFalse;    
        }        
    
    // success!
    return ETrue;        
    }

void CHuiEnv::CloseHandleToIdleCPUValueThread()
    {
    iIdleCPUValueThread.Close();
    }

CHuiCanvasTextureCache& CHuiEnv::CanvasTextureCache() const
    {
    return *iCanvasTextureCache;    
    }
    
EXPORT_C CAppIconCache& CHuiEnv::AppIconCache() const
    {
    return *iAppIconCache;
    }

EXPORT_C CHuiFxEngine* CHuiEnv::EffectsEngine() const
    {
    return iEffectsEngine;
    }

EXPORT_C void CHuiEnv::AddLowMemoryObserver(MHuiLowMemoryObserver * aObserver)
    {
    iLowMemoryObservers.Append(aObserver);
    }

EXPORT_C void CHuiEnv::RemoveLowMemoryObserver(MHuiLowMemoryObserver * aObserver)
    {
    TInt index = iLowMemoryObservers.Find(aObserver); 
    if(index != KErrNotFound)
        {
        iLowMemoryObservers.Remove(index);
        }    
    }

EXPORT_C void CHuiEnv::NotifyLowMemory(TBool /*aEnable*/)
    {
    //deprecated
    }


EXPORT_C void CHuiEnv::AddMemoryLevelObserver(MHuiMemoryLevelObserver * aObserver)
    {
    iMemoryLevelObservers.Append(aObserver);
    }

EXPORT_C void CHuiEnv::RemoveMemoryLevelObserver(MHuiMemoryLevelObserver * aObserver)
    {
    TInt index = iMemoryLevelObservers.Find(aObserver); 
    if(index != KErrNotFound)
        {
        iMemoryLevelObservers.Remove(index);
        }    
    }

EXPORT_C void CHuiEnv::NotifyMemoryLevel(THuiMemoryLevel aMemoryLevel)
    {
    iMemoryLevel = aMemoryLevel;
    
    // notify all observers
    for(TInt i=0; i<iMemoryLevelObservers.Count(); i++)
        {
        iMemoryLevelObservers[i]->SetMemoryLevel(aMemoryLevel);
        }
    }

EXPORT_C THuiMemoryLevel CHuiEnv::MemoryLevel()
    {
    return iMemoryLevel;
    }

EXPORT_C void CHuiEnv::Synchronize(TInt aId, MHuiSynchronizationObserver* aObserver)
    {
    TInt err = iSynchObservers.Append(aObserver);
    if(err)
        {
        return;
        }
    err = iSynchIds.Append(aId);
    if(err )
        {
        iSynchObservers.Remove(iSynchObservers.Count()-1);
        return;
        }
    if ( aObserver )
        {
        ContinueRefresh();
        }
    }

void CHuiEnv::DoSynchronize()
    {
    if ( iSynchObservers.Count() == 0)
        {
        return;
        }

    // Synchronize commands before signalling through P&S
    for(TInt i = 0; i < iDisplays.Count(); ++i)
        {          
        if ( ( iDisplays[i]->DisplayType() != CHuiDisplay::EDisplayOffScreenBuffer ) 
             && ( iDisplays[i]->ScreenBufferObserver() == NULL ) )
            {
            MakeCurrent(*iDisplays[i]);
            iDisplays[i]->RenderSurface().Finish();
            }
        }

    TUint observersCount = iSynchObservers.Count()-1;
    for(TInt u = observersCount; u >= 0 ; u-- )
        {
        iSynchObservers[u]->Synchronized( iSynchIds[u] );
        iSynchObservers.Remove(u);
        iSynchIds.Remove(u);
        }
    }

void CHuiEnv::RemoveTheControlGroup(TInt aId)
    {
    TInt i;

    for(i = 0; i < iLoadedGroups.Count(); ++i)
        {
        if(iLoadedGroups[i]->ResourceId() == aId)
            {
            // This is control group to delete.
            CHuiControlGroup* group = iLoadedGroups[i];
            CancelCommands(group);

            for (TInt ii = iDisplays.Count()-1; ii>=0; ii--)
                {
                TInt index = iDisplays[ii]->Roster().Find(group);
                if (index != KErrNotFound)
                    {
                    iDisplays[ii]->Roster().Hide(iDisplays[ii]->Roster().ControlGroup(index));
                    }
                }
            
            iLoadedGroups.Remove(i);
            
            }
        }
    }

