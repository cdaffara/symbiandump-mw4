/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include <eikenv.h>
#include <bautils.h>
#include <coemain.h>
#include <w32std.h>
#include <gdi.h>
#include <e32math.h>

#include "uiacceltk/huiEnv.h"  // Class definition
#include <ecom/implementationinformation.h>
#include "huirenderplugin.h"
#include "uiacceltk/huiStatic.h"
#include "alf/alfconstants.h"
#include "huirendersurface.h"
#include "uiacceltk/huiDisplay.h"
#include "huirosterimpl.h"
#include "uiacceltk/huiScheduler.h"
#include "uiacceltk/huiTextureManager.h"
#include "uiacceltk/huiControlGroup.h"
#include "huivisualfactory.h"
#include "uiacceltk/huiS60Skin.h"
#include "uiacceltk/huiEvent.h"
#include "uiacceltk/huiRoster.h"
#include "uiacceltk/huiUtil.h"
#include "uiacceltk/huipanic.h"
#include "uiacceltk/huiTextStyleManager.h"
#include "huistatictlsdata.h"
#include "uiacceltk/HuiThemeManager.h"


/* Constants */
const TInt KHuiEnvDefaultNormalRefreshIntervalMs = 40;

/* If there is idle between frames, this is how much we can use as overdrive max cpu utilisation */
const TUint KHuiEnvMaxCpuTimeOverdriveMaxValue = 100;

/* If max cpu usage has been set below this value, overdrive is not used because there is probaply a good reson 
for a low max cpu usage value */
const TUint KHuiEnvMaxCpuTimeOverdriveLowerThreshold = 50;

// @todo: this should be obsolete once scheduling can adapt to CPU load
const TInt KHuiEnvDefaultBusyRefreshIntervalMs = 2 * KHuiEnvDefaultNormalRefreshIntervalMs;

/** Threshold number for refreshes that don't have any effect. When exceeded,
    refresh is paused. */
const TInt KIdleRefreshCountThreshold = 3;


// to get around nasty ownership problem with texture manager interface
void NullTextureManagerPtr(TAny* aPtrToPtr)
    {
    if (aPtrToPtr)
        {
        CHuiTextureManager** ptr = (CHuiTextureManager**)aPtrToPtr;
        *ptr = 0;
        }
    }


void CHuiEnv::SetTextureManager(CHuiTextureManager& aManager)
    {
    iTextureManager = &aManager;
    }



void CHuiEnv::StopRefresh()
    {
    HUI_DEBUG(_L("CHuiEnv::StopRefresh()"));
    if(iPeriodic)
        {
        iPeriodic->Cancel();
        }
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


void CHuiEnv::AdvanceTime(TReal32 aElapsedTime)
    {
    HUI_DEBUGF( _L("CHuiEnv::AdvanceTime() - Started") )

    TUint usedMaxCPUUtilization = iMaxCPUUtilization;

    // Calculate cpu values based on null thread cpu usage between frames. 
    // Values will be updated at the end of the frame.
    if (iIdleCPUValueMonitored)
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

            HUI_DEBUGF1( _L("CHuiEnv::AdvanceTime() - Refreshing display %i"), i )
            TBool updated = iDisplays[i]->Refresh();
            displayRefreshed[i] = updated;
            if(updated)
                {
                somethingUpdated = ETrue;
                }
            }
        }
    
    TBool continueRefresh = ETrue;

    if(somethingUpdated)
        {
        HUI_DEBUGF( _L("CHuiEnv::AdvanceTime() - Swap buffers") )

        SwapBuffers(displayRefreshed);
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

	displayRefreshed.Close(); // Not needed any more

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

    if (continueRefresh)
        {
        // Refresh rate adjustment
        if (usedMaxCPUUtilization)
            {
            TUint millisecondsUsedInRefresh = CHuiStatic::MilliSecondsSinceUpdateTime();

            TUint totalLoopTime = (millisecondsUsedInRefresh * 100) / usedMaxCPUUtilization;
    
            if (totalLoopTime >= iRefreshIntervalTarget)
                {
                iRefreshInterval = (millisecondsUsedInRefresh*(100-usedMaxCPUUtilization))/usedMaxCPUUtilization;
                StartRefresh(iRefreshInterval);        
                }
            else if (iRefreshIntervalTarget != iRefreshInterval)
                {
                iRefreshInterval = iRefreshIntervalTarget; 
                StartRefresh(iRefreshInterval);
                }
            else
                {
                // otherwise just let the periodic run as it already has good interval set
                }
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
     		iDisplays[i]->RenderSurface().SwapBuffers();
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



void CHuiEnv::SetTimeFromLastUpdate(TUint aTimeFromLastUpdate)
    {
    iRefreshIntervalReal = aTimeFromLastUpdate;
    }


CHuiDisplay* CHuiEnv::CurrentDisplay() const
    {
    return iCurrentDisplay;    
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
