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
* Description:   Implementation of CHuiDisplay, a drawing buffer into which a 
*                the toolkit contents (visuals) are drawn.
*
*/



#include "uiacceltk/HuiDisplay.h"  // Class definition
#include "uiacceltk/HuiStatic.h"
#include "HuiRenderPlugin.h"
#include "HuiRenderSurface.h"
#include "uiacceltk/HuiEnv.h"
#include "HuiRosterImpl.h"
#include "uiacceltk/HuiSkin.h"
#include "uiacceltk/HuiTransformation.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiTexture.h"
#include "uiacceltk/HuiProbe.h"
#include "uiacceltk/HuiS60Skin.h"
#include "uiacceltk/HuiGc.h"
#ifndef __SERIES60_30__ 
#include "HuiTargetBitmap.h"
#endif
#include "uiacceltk/HuiControlGroup.h"
#include "alf/alfconstants.h" 

#ifdef RD_ALF_IN_PLATFORM            
#include <displaylayoutmetrics.cdl.h>
#endif // RD_ALF_IN_PLATFORM            

#include <AknsRendererWrapper.h>
#include <AknsDrawUtils.h>

#include <coecntrl.h>
#include <uiacceltk/huidisplaybackgrounditem.h>
#include "huiskinbackroundlayout.h"
#include "HuiFxEngine.h"
#include "huiextension.h"

#include <e32math.h>

//#define DEBUG_SW_MODE_DIRTY_AREAS

const TUid KHuiInternalFbsBitmapBufferGcUid = {0x2000e5a3}; 

//#define HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL


// *** Class CHuiFbsBitmapBufferGc

class CHuiFbsBitmapBufferGc: public CBase, public MHuiFbsBitmapBufferGc
    {
    public:
    	CHuiFbsBitmapBufferGc(CFbsBitmap* aBitmap) :iBitmap(aBitmap) {};
    
    private: // From MHuiFbsBitmapBufferGc
    	void ReleaseGc(); 
    	CFbsBitmap* Bitmap(); 

    private:
    	CFbsBitmap* iBitmap; // not owned  
    };


void CHuiFbsBitmapBufferGc::ReleaseGc()
	{
	// Line "delete(this)" should always be the last line in this method.
	// If you change this method behaviour make sure you know what caveats "delete(this)" has.
	delete(this); 
	}

CFbsBitmap* CHuiFbsBitmapBufferGc::Bitmap()
	{
	return iBitmap; 
	}

// *** Class CHuiDisplay

CHuiDisplay::CHuiDisplay(CHuiEnv& aEnv, RWindow* aNativeWindow, TInt aDisplayType, TUid aBackBufferUid )
          : iEnv(aEnv),
          iDisplayType(aDisplayType),
          iActive(ETrue),
          iBackgroundColor(KRgbBlack),
          iUpdateRenderState(ETrue),
          iUsageHint(EHuiDisplayUsageGeneric),
          iOnForeground(ETrue),
          iScreenBufferUid(aBackBufferUid),
          iUseTransformedDirtyRegions(EFalse),
          iDrawVisualOutline( EDrawVisualOutlineNone ),
          iNativeWindow(aNativeWindow)
    {
    }

CHuiDisplay::CHuiDisplay(CHuiEnv& aEnv, CCoeControl* aNativeControl, TInt aDisplayType, TUid aBackBufferUid )
        : iEnv(aEnv),
          iDisplayType(aDisplayType),
          iNativeControl(aNativeControl),
          iSize(0, 0),
          iActive(ETrue),
          iBackgroundColor(KRgbBlack),
          iUpdateRenderState(ETrue),
          iUsageHint(EHuiDisplayUsageGeneric),
          iOnForeground(ETrue),
          iScreenBufferUid(aBackBufferUid),
          iUseTransformedDirtyRegions(EFalse),
          iDrawDirtyRegions( EFalse ),
          iDrawVisualOutline( EDrawVisualOutlineNone )
    {
    }


void CHuiDisplay::ConstructL(const TRect& aRect, CHuiRoster* aSharedRoster)
    {
    HUI_DEBUG1(_L("CHuiDisplay::ConstructL - Free memory in beginning: %i"), HuiUtil::FreeMemory());
    
    // Register this display to receive foreground lost / gained events.
/*    CCoeEnv* coeEnv = CCoeEnv::Static();
    if(coeEnv && NativeWindow())
        {
        coeEnv->AddMessageMonitorObserverL(*this);
        NativeWindow()->EnableVisibilityChangeEvents();
        }
*/    

    iSize = aRect.Size();

    // Create a rendering surface.
    iRenderSurface = CHuiStatic::Renderer().CreateRenderSurfaceL(*this);
    
    iCurrentDirtyRegions = &iDirtyRegions;
    
    // Preserve previous dirty rect info only if the surface uses both dirty rects and double 
    // buffering
    TUint flags = MHuiRenderSurface::EFlagUseDirtyRects | MHuiRenderSurface::EFlagUseDoubleBuffering;
    TBool preservePreviousDirtyRectInfo = ((iRenderSurface->Flags() & flags) == flags);

    if (preservePreviousDirtyRectInfo)
   		{
    	iPreviousDirtyRegions = &iDirtyRegions2;
   		}
    		 
    // Create a control roster.
    if(aSharedRoster)
        {
        iRoster.Set(aSharedRoster, EHuiDoesNotHaveOwnership);
        }
    else
        {
        // Construct a private roster.
        CHuiRosterImpl* roster = new (ELeave) CHuiRosterImpl(this);
        iRoster.Set(roster, EHuiHasOwnership);
        }

    // Transformation for the entire display.
    iTransform = CHuiTransformation::NewL();
    // Window transformation for the entire display.
    iWindowTransform = CHuiTransformation::NewL();

	SetScreenBufferLock(EFalse);

    // Create a graphics context.
    iGc = CHuiStatic::Renderer().CreateGcL();

    HUI_DEBUG1(_L("CHuiDisplay::ConstructL - Free memory after GC created: %i"), HuiUtil::FreeMemory());

    SetVisibleArea(TRect(TPoint(0, 0), iSize));

    HUI_DEBUG1(_L("CHuiDisplay::ConstructL - Free memory after surface created: %i"), HuiUtil::FreeMemory());
    
    if ( !CHuiStatic::Renderer().Allows( EHuiRenderPluginAllowPBufferSurfaces ) )
    // There is no default render surface, so make this one current 
    // immediately to enable texture loading.
        {
        iRenderSurface->MakeCurrent();
        }
    
    // When dirty regions are transformed it is possible to avoid
    // full display refresh in many cases -> better performance.
    iGc->EnableTransformedClippingRects(iUseTransformedDirtyRegions);

    iDisplaySizeChangePending = EFalse;
    
    HUI_DEBUG1(_L("CHuiDisplay::ConstructL - Free memory when exiting ConstructL: %i"), HuiUtil::FreeMemory());
    }


EXPORT_C CHuiDisplay::~CHuiDisplay()
    {
    HUI_DEBUG1(_L("CHuiDisplay %x destructor"), this);

    // Unregister this display from foreground gained / lost events.
//    CCoeEnv* coeEnv = CCoeEnv::Static();
//    if(coeEnv && NativeWindow())
//        {
//        coeEnv->RemoveMessageMonitorObserver(*this);
//        }

    // Notify observers of deletion.
    
   	if(iScreenBufferObserver)
		{
		iScreenBufferObserver->HandleScreenBufferEvent(iScreenBufferUid, MHuiScreenBufferObserver::EDeleted);
		}
 
    for(TInt i = 0; i < iDeletionObservers.Count(); ++i)
        {
        iDeletionObservers[i].NotifyDisplayDeletion(*this);
        }
    iDeletionObservers.Reset();
    iRefreshObservers.Reset();
    iVisibleAreaObservers.Reset();
    iRosterObservers.Reset();

    iTempDirtyRegions.Close();
    
    iDirtyRegions.Close();
    iDirtyRegions2.Close();
    if ( iCurrentDirtyRegions )
        {
        iCurrentDirtyRegions->Close();
        }
    iCurrentDirtyRegions = NULL;
    
    if ( iPreviousDirtyRegions )
        {
        iPreviousDirtyRegions->Close();
        }
    iPreviousDirtyRegions = NULL;

    delete iRenderSurface;
    iRenderSurface = NULL;

    delete iGc;
    iGc = NULL;

    delete iTransform; 
    iTransform = NULL;

    delete iWindowTransform;
    iWindowTransform = NULL;
    
    iEnv.RemoveDisplay(*this);
    
    iNativeControl = NULL;
    
    iBackgroundItems.Close();
    
    delete iForegroundTexture;

    delete iForegroundBitmapGc;
    delete iForegroundBitmapDevice;
    iForegroundBitmap = NULL;
    }

TReal32 CHuiDisplay::UnitValue() const
    {
#ifdef RD_ALF_IN_PLATFORM            
    // this will be correct according to the currently loaded layout pack for the current display
    if (CCoeEnv::Static())
        {
        return Display_Layout_Metrics::UnitValue();
        }
    else
        {
        return 1.0f;
        }
#else // RD_ALF_IN_PLATFORM            
    // not supported
    return 1.0f;
#endif // RD_ALF_IN_PLATFORM            
    }

void CHuiDisplay::MonitorWsMessage(const TWsEvent& aEvent)
    {
	switch(aEvent.Type())
	    {
        case EEventWindowVisibilityChanged:
            {
            const TBool wasOnForeground = iOnForeground;
            
//            iOnForeground = !(aEvent.VisibilityChanged()->iFlags&TWsVisibilityChangedEvent::ENotVisible);
            
            if ( !wasOnForeground && iOnForeground )
                {
                // Becomes visible, force refresh. 
                // This is needed for example in case of resolution changes.
                SetDirty();
                }
            break;
            }
            
      	default:
        	break;              
	    }
    }


CHuiGc* CHuiDisplay::Gc()
    {
    ASSERT(iGc != NULL);
    
    return iGc;
    }
    
EXPORT_C CCoeControl* CHuiDisplay::NativeControl()
    {
    return iNativeControl;
    }


EXPORT_C RDrawableWindow* CHuiDisplay::NativeWindow()
    {
    if (iNativeWindow)
        {
        return iNativeWindow;
        }
                
    else if(iNativeControl)
        {
        return iNativeControl->DrawableWindow();
        }
        
    return 0;
    }


EXPORT_C CHuiEnv& CHuiDisplay::Env()
    {
    return iEnv;
    }


EXPORT_C MHuiRenderSurface& CHuiDisplay::RenderSurface() const
    {
    return *iRenderSurface;
    }


EXPORT_C CHuiRoster& CHuiDisplay::Roster()
    {
    return iRoster.Ref();
    }


EXPORT_C const CHuiRoster& CHuiDisplay::Roster() const
    {
    return iRoster.Ref();
    }


EXPORT_C void CHuiDisplay::Show(TBool aShow)
    {
    iRenderSurface->HandleVisibilityEvent(aShow);

    // Update projection and GC state on the next refresh.
    iUpdateRenderState = ETrue;
    }


CHuiRosterImpl& CHuiDisplay::RosterImpl() const
    {
    return STATIC_CAST(CHuiRosterImpl&, iRoster.NonConstRef());
    }


EXPORT_C void CHuiDisplay::SetSizeL(const TSize& aSize)
    {
    iSize = aSize;

    // Update projection and GC state on the next refresh.
    iUpdateRenderState = ETrue;

    if(iRenderSurface)
        {
        iRenderSurface->SetSizeL(iSize);
        }
    }


EXPORT_C TSize CHuiDisplay::Size() const
    {
    return iSize;
    }


EXPORT_C TRect CHuiDisplay::VisibleArea() const
    {
    return iGc->DisplayArea();
    }


EXPORT_C void CHuiDisplay::SetVisibleArea(const TRect& aArea)
    {
    if(iGc)
        {
        // If clipping has been defined for visible are, it is now invalid so disable.
        SetVisibleAreaClippingRect(TRect(0,0,0,0));
        
        iGc->SetDisplayArea(aArea);
        // Make sure everything gets redrawn.
        SetDirty();
        
  		// Can change the roster rect only if the roster is own
   		// \todo This should be prevented also for other shared roster types.
       	RosterImpl().SetRect(aArea);

        // Update projection and GC state on the next refresh.
        iUpdateRenderState = ETrue;

        // Inform observers.
        for(TInt i = 0; i < iVisibleAreaObservers.Count(); ++i)
            {
            iVisibleAreaObservers[i].NotifyDisplayVisibleAreaChanged(*this);
            }
        }
    }


EXPORT_C void CHuiDisplay::Activate(TBool aActivate)
    {
    iActive = aActivate;

    if(iActive)
        {
        // This is the display now using the roster. Only meaningful when the
        // roster is shared between many displays.
        RosterImpl().SetDisplay(this);
        }
    }


EXPORT_C TBool CHuiDisplay::Active() const
    {
    return iActive;
    }


EXPORT_C void CHuiDisplay::SetOrientation(CHuiGc::TOrientation aOrientation)
    {
    if (iGc->Orientation() == aOrientation)
        {
        return;
        }
    
    iGc->SetOrientation(aOrientation);

    // The effective visible area, as seen by the UI, changes.
    RosterImpl().SetRect(VisibleArea());
    for(TInt i = 0; i < iVisibleAreaObservers.Count(); ++i)
        {
        iVisibleAreaObservers[i].NotifyDisplayVisibleAreaChanged(*this);
        }

    SetDirty();

    // Update projection and GC state on the next refresh.
    iUpdateRenderState = ETrue;
    
    // Clear effect engine cache as it most likely now contains buffers which 
    // sizes are not very well reusable. It would be better to have "fifo" or
    // something in the cache implementation but until that just release everything here
    // when display layout changes.
    CHuiFxEngine* fxEngine = Env().EffectsEngine();  
    if (fxEngine)
        {
        fxEngine->ClearCache();
        }
    } 


EXPORT_C CHuiGc::TOrientation CHuiDisplay::Orientation() const
    {
    return iGc->Orientation();
    }


EXPORT_C void CHuiDisplay::SetDirty()
    {
    // Mark the entire display dirty.
    HUI_DEBUG(_L("CHuiDisplay::SetDirty() - called."));
   
    if ( iWholeDisplayAreaIsDirty )
        {
        return;
        }
   
    TRect displayArea = iGc->DisplayArea();
    AddDirtyRegion(displayArea, *iCurrentDirtyRegions);
    if (iPreviousDirtyRegions)
    	{
		AddDirtyRegion(displayArea, *iPreviousDirtyRegions);
    	}
    iEnv.ContinueRefresh();
    
    iWholeDisplayAreaIsDirty = ETrue;
    }


TBool CHuiDisplay::IsDirty() const
    {
    //iRoster->ScanDirty();
    //return iDirtyRegions.Count() > 0;

    /** @todo  Determine this better. There should be a way to quickly
               determine if the display needs to be updated; although
               a visual change scan is also a possiblity if it is done
               once per frame. */

    return iActive;
    }


void CHuiDisplay::AddDirtyRegion(const TRect& aDirtyRegion)
    {        
    // Whole screen has been set dirty, no need to add new areas.
    if (iWholeDisplayAreaIsDirty && iGotDirtyReports)
        {
        return;    
        }

    TRect clippedDirtyRect = aDirtyRegion;            
    
    if (UseTransformedDirtyRegions())
        {
        TransformDirtyRect(clippedDirtyRect);
        }
    
    ClipDirtyRect(clippedDirtyRect);
        
    // If whole screen is set dirty, update flag which can be used to avoid 
    // further unnecessary dirty area calculations
    if (iGc && (iGc->DisplayArea() ==  clippedDirtyRect))
        {
        iWholeDisplayAreaIsDirty = ETrue;    
        }        

    // At least one dirty region was reported.
    iGotDirtyReports = ETrue;

    AddDirtyRegion(clippedDirtyRect, *iCurrentDirtyRegions);
    }


void CHuiDisplay::CombineAndAddDirtyRegion(const TRect& aPrevDirtyRegion, TRect& aDirtyRegion)
    {   
    // aPrevDirtyRegion has already been transformed, but aDirtyRegion is not (yet)
    // transformed.

    TRect dirtyRect = aDirtyRegion;            
    
    if (!dirtyRect.IsEmpty() && UseTransformedDirtyRegions())
        {
        TransformDirtyRect(dirtyRect);
     	}

    TRect transformedNewDirtyRect = dirtyRect;            
              
    // If the whole display is not dirty, update current combined dirty region     
    if (!iWholeDisplayAreaIsDirty)
    	{
    	dirtyRect.BoundingRect(aPrevDirtyRegion);	
    
    	ClipDirtyRect(dirtyRect);
    	
     	if (iGc && (iGc->DisplayArea() ==  dirtyRect))
        	{
    		// If whole screen is set dirty, update flag which can be used to avoid 
    		// further unnecessary dirty area calculations
        	iWholeDisplayAreaIsDirty = ETrue;    
        	}        

    	// At least one dirty region was reported.
    	iGotDirtyReports = ETrue;         	
   		AddDirtyRegion(dirtyRect, *iCurrentDirtyRegions);
    	}
    // We must only transform the (new) dirtyregion, not combine or anything else here
    aDirtyRegion = transformedNewDirtyRect;
    
    if(iScanningAlfContent)
        {
		SetAlfContentChanged(ETrue);
        }
    }


void CHuiDisplay::AddDirtyRegion(const TRect& aDirtyRegion, RDirtyRegions& aRegions, TBool /*aGrow*/)
    {
    TBool combined = ETrue;
    TInt i = 0;
    TInt k = 0;

    aRegions.Append(aDirtyRegion);

    // Combine overlapping regions.
    while(combined && aRegions.Count() > 1)
        {
        combined = EFalse;

        // Check that none of the regions overlap each other.
        /** @todo Performance? */
        for(i = 0; i < aRegions.Count(); ++i)
            {
            TRect expanded = aRegions[i];
            expanded.Grow(20, 20); //(8, 8);

            for(k = i + 1; k < aRegions.Count(); ++k)
                {
                // E: comment out for lazy merging
                if(aRegions[k].Intersects(expanded))
                    {
                    // Remove the latter one.
                    aRegions[i].BoundingRect(aRegions[k]);
                    aRegions.Remove(k);
                    --k;
                    combined = ETrue;
                    }
                }
            }
        }
    }


TBool CHuiDisplay::Refresh()
    {
    HUI_DEBUGF(_L("CHuiDisplay::Refresh() - Entering."));
    HUI_PROBE_PROGRAMFLOW_ENTER(MHuiProbe::EProgramFlowPointRefresh)
    
#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
    RDebug::Print(_L("CHuiDisplay::Refresh: starting measuring."));
    TTime startTime;
    startTime.UniversalTime();
#endif
    
    SetAlfContentChanged(EFalse);
    
    // Prevent display refresh, if the display is on background.
    if(!iOnForeground || IsScreenBufferLocked())
        {
        return EFalse;
        }
    
    if (!iForegroundTextureTransparency && iForegroundTexture)
        {
        // When we can blit foreground texture, we shouldn't need any textures in skin side.
        iEnv.Skin().ReleaseCachedTextures();
        }
    
    TBool useDirtyRects = (RenderSurface().Flags() & MHuiRenderSurface::EFlagUseDirtyRects) 
    					  == MHuiRenderSurface::EFlagUseDirtyRects;
 	
    iWholeDisplayAreaIsDirty = EFalse;    
    
    // Restore state to defaults. Alf client may have done some NVG drawing outside refresh loop and main drawing context may be wrong
    iGc->RestoreState();
    
    if(iUpdateRenderState)
        {
        // Set state when requested.
        iGc->InitState();
        iGc->SetProjection();

        iUpdateRenderState = EFalse;
        }

    // Let the skin know that we're now drawing in this context.
    iEnv.Skin().SetContext(*iGc);

    TRect displayArea = iGc->DisplayArea();

    // Notify observers of the display refresh.
    TInt i = 0;
    for(i = 0; i < iRefreshObservers.Count(); ++i)
        {
        iRefreshObservers[i].NotifyDisplayRefreshStarted(*this);
        }
#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
    TTime afterNotifiedTime;
    afterNotifiedTime.UniversalTime();
#endif


	if (IsDisplayTypeTvOut())
		{
		// Scale shared roster to Tv-out screen				
		TReal32 scaleFactorX = (TReal32)Size().iWidth / (TReal32)RosterImpl().Rect().Width();
		TReal32 scaleFactorY = (TReal32)Size().iHeight / (TReal32)RosterImpl().Rect().Height();					
		// Calculate window transformations		
    	iWindowTransform->LoadIdentity();
    	iWindowTransform->Scale(scaleFactorX, scaleFactorY, 1.f);
    	}

//	THuiRenderer renderer = iEnv.Renderer();
	TBool hasTransformations = iTransform->Count() != 0 || iWindowTransform->Count() != 0 || RosterImpl().HasTransformedControlGroups();
	TBool cantUseDirtyRectsInThisFrame = hasTransformations && !UseTransformedDirtyRegions();
	    
    // When refresh is forced, there is no need to scan dirty areas.
    if (iEnv.RefreshMode() != EHuiRefreshModeForced) 
        {
        // Flag that tells us if any dirty regions were reported for
        // this frame.
        iGotDirtyReports = (iCurrentDirtyRegions->Count() > 0);
        
        // Tell all visuals to post their dirty regions (for the current state).
        if (UseTransformedDirtyRegions())
            {
            RosterImpl().ScanTransformedDirty(*iGc, this);	         	        
            }
         else
            {
            RosterImpl().ScanDirty();	         	      
            }   

        if (!useDirtyRects || cantUseDirtyRectsInThisFrame || iEnv.EffectsEngine()->HasActiveEffects() || RosterImpl().IsVisibleContentFrozen()) 
         	{         	
	        // Just redraw everything
	        iCurrentDirtyRegions->Reset();
	        AddDirtyRegion(displayArea, *iCurrentDirtyRegions, EFalse);
         	} 
        }
    else
        {
        iGotDirtyReports = ETrue;

        // Just redraw everything since determining dirty regions is
        // complicated with a transformed display.
        iCurrentDirtyRegions->Reset();
        AddDirtyRegion(displayArea);
        }

	// We can return now if there are no new or old dirty areas
	if (!iGotDirtyReports && (!iPreviousDirtyRegions || (iPreviousDirtyRegions->Count() == 0)))
		{
		iCurrentDirtyRegions->Reset();
		return EFalse;
		}

    // Form the real list of dirty regions by combining the dirty regions of
    // this frame and the previous frame (this is needed when buffer swapping
    // is used; with buffer copying a single list of dirty regions would
    // suffice).


    iTempDirtyRegions.Reset();
    for(i = 0; i < iCurrentDirtyRegions->Count(); ++i)
        {
        iTempDirtyRegions.Append((*iCurrentDirtyRegions)[i]);
        }
    if (iPreviousDirtyRegions)
    	{
    	for(i = 0; i < iPreviousDirtyRegions->Count(); ++i)
        	{
        	AddDirtyRegion((*iPreviousDirtyRegions)[i], iTempDirtyRegions, EFalse);
        	}
    	}

	// Do initializations for new frame
	// initnewframe may clear the screen, and it may be that
	// the clipping rectangle is not set up correctly
	// atleast if the dirty area tracking is on..
	iGc->SetPenColor(iBackgroundColor);
	iGc->InitNewFrame();

	
	// if there is a fade effect in progress, we
	// need to clear the screen as fade effect uses
	// always blending. If we do not clear here
	// fade leaves trails in certain situations.
	if (iEnv.EffectsEngine()->HasActiveEffects() 
	        || RosterImpl().IsVisibleContentFrozen() // guaranteen, that transparent pixels of the UI surface are drawn correctly during layout switch.
	        )
	    {
        iGc->SetPenColor(KRgbBlack);
        iGc->SetPenAlpha(0);
        iGc->Disable(CHuiGc::EFeatureClipping);
        iGc->Disable(CHuiGc::EFeatureBlending);
        iGc->Clear();
	    }
	
	
    if ( iDrawDirtyRegions )
        {
        // Show dirty. 
        // This will draw the whole background with black.
        iGc->SetPenColor(KRgbBlack);
        iGc->Disable(CHuiGc::EFeatureClipping);
        iGc->Clear();
        }

		
    // Set up clipping.
    TInt markedClipStackCount = iGc->ClipStackCount();
    if(useDirtyRects)
    	{
	    iGc->Enable(CHuiGc::EFeatureClipping);
    	}

	// Assume that we succeed to clean the dirty regions
    TBool dirtyRegionsCleaned = ETrue;

	// Set dirty rect in render surface to minimize screen update
	// Only implemented for BitGdi renderer for now
    TRect mergedDirtyRect;
    if (iTempDirtyRegions.Count() > 0)
    	{
    	mergedDirtyRect = iTempDirtyRegions[0];
    	}
    	
	if (useDirtyRects)
	    {
		// When Bitgdi renderer used set dirty rect in render surface
		// to minimize screen update in CHuiBitgdiRenderSurface::SwapBuffers
	    if (iTempDirtyRegions.Count() == 1)
		    {
            ClipDirtyRect(mergedDirtyRect, VisibleAreaClippingRect());
            iRenderSurface->SetDirtyRect(mergedDirtyRect);
		    }
		else if (iTempDirtyRegions.Count() > 1) 
		    {
			for(i = 1; i < iTempDirtyRegions.Count(); ++i)
				{
				TRect r(iTempDirtyRegions[i]);
				// check top left corner to expand or not
				if (r.iTl.iX < mergedDirtyRect.iTl.iX)
				    {
				    mergedDirtyRect.iTl.iX = r.iTl.iX;
				    }
				if (r.iTl.iY < mergedDirtyRect.iTl.iY)
				    {
				    mergedDirtyRect.iTl.iY = r.iTl.iY;
				    }
				// check bottom right corner to expand or not
				if (mergedDirtyRect.iBr.iX < r.iBr.iX)
				    {
				    mergedDirtyRect.iBr.iX = r.iBr.iX;
				    }
				if (mergedDirtyRect.iBr.iY < r.iBr.iY)
				    {
				    mergedDirtyRect.iBr.iY = r.iBr.iY;
				    }
				}
				
            ClipDirtyRect(mergedDirtyRect, VisibleAreaClippingRect());
            iRenderSurface->SetDirtyRect(mergedDirtyRect);
		    }
        else
            {
            // for PC lint
            }
            
        if ( iDrawDirtyRegions )
	        {
            // This will disable the clipping of the frame buffer when 
            // blitting on the screen.
	        iRenderSurface->SetDirtyRect(displayArea);
            }
	    }	   

	// Merge into max one dirty area when HW accelrated drawing is used
	if (useDirtyRects && IsRendererHWAccelerated())
	    {
        iTempDirtyRegions.Reset();
        iTempDirtyRegions.Append(mergedDirtyRect);
	    }
	
#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
    RDebug::Print(_L("CHuiDisplay::Refresh: Merged dirty rect: %d %d %d %d"), 
            mergedDirtyRect.iTl.iX,  
            mergedDirtyRect.iTl.iY,  
            mergedDirtyRect.iBr.iX,  
            mergedDirtyRect.iBr.iY);  
#endif	
	
    // Usually there is only one dirty region (if any).
    iCurrentDirtyIndx = 0;
    for(; iCurrentDirtyIndx < iTempDirtyRegions.Count(); ++iCurrentDirtyIndx)
        {
        // Set up the clipping rectangle.
        TRect dirtyRect = iTempDirtyRegions[iCurrentDirtyIndx];
        ClipDirtyRect(dirtyRect, VisibleAreaClippingRect());
        iCurrentDirtyRect = dirtyRect;
        iGc->PushClip();
        iGc->Clip(dirtyRect);        
        
        if ( iForegroundBitmapGc && !RosterImpl().IsVisibleContentFrozen() )
            {
            // If we are in SW rendering mode, then SW bitmap may be blended.
            // However, it's possible that nothing is drawn below, so clear
            // background.
            const TRgb oldBgColor = iBackgroundColor;
            iBackgroundColor = KRgbBlack;
            iBackgroundColor.SetAlpha(0);
            ClearWithColor( dirtyRect );
            iBackgroundColor = oldBgColor;
            }
                
         
        if ( iForegroundBitmapGc && iForegroundTextureTransparency )
            {
            // There is ALF content in the background, we have to
            // clear foreground bitmap.
            
            iForegroundBitmapGc->Reset();
            TRgb clearColor = KRgbBlack;
            clearColor.SetAlpha(0x00);
                    
	        iForegroundBitmapGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
            iForegroundBitmapGc->SetPenColor(clearColor);
            iForegroundBitmapGc->SetBrushColor(clearColor);
            iForegroundBitmapGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
  	        iForegroundBitmapGc->Clear(dirtyRect);
  	        iForegroundBitmapGc->Reset();
            }
        		
        for(TInt i = 0; i < iRosterObservers.Count(); i++)
            {
            iRosterObservers[i].NotifyRosterDrawStart(*this);
            }
        
		// Prepare dirty visuals for drawing
		// Draw only if the prepare was successful
		if (RosterImpl().PrepareDraw())
			{
	        // Draw all the dirty visuals (that are touching dirty regions).
	        iGc->Enable(CHuiGc::EFeatureBlending);
            RosterImpl().Draw(*iGc, this);                    
			}
		else 
			{
		    HUI_DEBUGF(_L("CHuiDisplay::Refresh() - Failed to prepare drawing!"));
	        dirtyRegionsCleaned = EFalse;			
			}

        for(TInt i = 0; i < iRosterObservers.Count(); i++)
            {
            iRosterObservers[i].NotifyRosterDrawEnd(*this);
            }
		
		if (iForegroundTexture)
		    {
		    UpdateForegroundTexture(dirtyRect);
		    DrawForegroundTexture();
		    }
		
        // Return to the clipping rectangle that was in use previously.
        iGc->PopClip();
        }

    iTempDirtyRegions.Reset();

    // There must be no disparity in the number of pushed clipping rectangles.
    // (equivalent to __ASSERT_ALWAYS)
    if(iGc->ClipStackCount() > markedClipStackCount)
        {
        THuiPanic::Panic(THuiPanic::EDisplayClipStackUnpopped);
        }
    if(iGc->ClipStackCount() < markedClipStackCount)
        {
        THuiPanic::Panic(THuiPanic::EDisplayClipStackEarlyPop);
        }

    // Reset and flip if we managed to clean (draw) everything
	if (iPreviousDirtyRegions && dirtyRegionsCleaned && iCurrentDirtyRegions->Count() > 0) 
		{
    	RDirtyRegions* tmp = iPreviousDirtyRegions;
    	iPreviousDirtyRegions = iCurrentDirtyRegions;
    	iCurrentDirtyRegions = tmp;
		}
		
	// Clear current dirty regions	
	iCurrentDirtyRegions->Reset();
		    	
    iWholeDisplayAreaIsDirty = EFalse;    
	
    HUI_PROBE_PROGRAMFLOW_EXIT(MHuiProbe::EProgramFlowPointRefresh)
    HUI_DEBUGF(_L("CHuiDisplay::Refresh() - Exiting."));

#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
    TTime endTime;
    endTime.UniversalTime();
    TInt timeInMs =  endTime.MicroSecondsFrom( startTime ).Int64()/1000;           
    TInt notifyTimeInMs =  afterNotifiedTime.MicroSecondsFrom( startTime ).Int64()/1000;           
    TInt restTimeInMs =  endTime.MicroSecondsFrom( afterNotifiedTime ).Int64()/1000;           

    RDebug::Print(_L("CHuiDisplay::Refresh: took %i ms"), timeInMs);
    RDebug::Print(_L("CHuiDisplay::Refresh: notify refresh observers took %i ms"), notifyTimeInMs);
    RDebug::Print(_L("CHuiDisplay::Refresh: rest took %i ms"), restTimeInMs);

#endif


    // If something was reported (even if eventually clipped), we'll say that
    // there was some activity. If it was clipped and did not show up on the
    // display, it may soon (re)appear, so let's be cautious.
    return iGotDirtyReports;
    }


EXPORT_C CHuiTransformation& CHuiDisplay::Transformation() const
    {
    return *iTransform;
    }


EXPORT_C void CHuiDisplay::SetClearBackgroundL(TClearMode aClearBackground)
    {
    // Make sure that background resource allocation is succeeded.
    if(aClearBackground == EClearWithSkinBackground)
        {
        iEnv.Skin().TextureL(EHuiSkinBackgroundTexture);
        }

    iClearBackground = aClearBackground;
    iBackgroundItems.Reset();
    
    // release background texture if clearing is set to None
    if(aClearBackground == EClearNone)
        {
        iEnv.Skin().ReleaseTexture(EHuiSkinBackgroundTexture);
        }
    }


EXPORT_C void CHuiDisplay::SetBackgroundColor(const TRgb& aBackgroundColor)
    {
    iBackgroundColor = aBackgroundColor;
    }


EXPORT_C void CHuiDisplay::SetUseDepth(TBool aUseDepth)
    {
    iUseDepth = aUseDepth;
    }


EXPORT_C TUint8* CHuiDisplay::CaptureLC(TSize& aSize) const
    {
    return iGc->CaptureLC(aSize);
    }


EXPORT_C void CHuiDisplay::SetQuality(THuiQuality aRenderingQuality)
    {
    if(iGc)
        {
        iGc->SetQuality(aRenderingQuality);
        SetDirty();
        }
    }


EXPORT_C THuiQuality CHuiDisplay::Quality() const
    {
    if(iGc)
        {
        return iGc->Quality();
        }
    return EHuiQualityAccurate;
    }


EXPORT_C void CHuiDisplay::Release()
    {      
    // Free the render surface.
    ASSERT(iRenderSurface != NULL);
    HUI_DEBUG(_L("CHuiDisplay::Release() - Releasing render surface."));    
    iRenderSurface->Release();
        
    HUI_DEBUG(_L("CHuiDisplay::Release() - Display released."));            
    }


EXPORT_C void CHuiDisplay::RestoreL()
    {
    HUI_DEBUG(_L("CHuiDisplay::RestoreL() - Restoring display resources."));    
    
    // Reconfigure graphics context when display is refreshed.
    iUpdateRenderState = ETrue;
    
    // Create a rendering surface.
    iRenderSurface->RestoreL();
    }


EXPORT_C TInt CHuiDisplay::DisplayType()
	{
	return iDisplayType;
	}
	
EXPORT_C TUid CHuiDisplay::ScreenBufferUid()
	{
	return iScreenBufferUid;
	}
	

EXPORT_C TBool CHuiDisplay::IsScreenBufferLocked() const
	{
	return iIsScreenBufferLocked;
	}
    
    
EXPORT_C void CHuiDisplay::SetScreenBufferLock(TBool aLock)
	{
	iIsScreenBufferLocked = aLock;
	}


EXPORT_C MHuiBufferDrawer* CHuiDisplay::GetDrawingInterface(const TUid& /*aInterfaceUid*/)
	{
	return (MHuiBufferDrawer*)NULL;
	}
	
	
EXPORT_C MHuiGc* CHuiDisplay::GetGraphicsContext(const TUid& aInterfaceUid)
	{
	MHuiGc* gc = NULL;
	if (aInterfaceUid == KHuiInternalFbsBitmapBufferGcUid)
		{
    	if (iEnv.Renderer() == EHuiRendererBitgdi) 
			{
			CFbsBitmap* backBuffer = iRenderSurface->BackBuffer()->FbsBitmap();
			CHuiFbsBitmapBufferGc* bitmapGc = new CHuiFbsBitmapBufferGc(backBuffer);
			gc =  (MHuiFbsBitmapBufferGc*) bitmapGc;
			}
        else
            {
            if ( DisplayType() != CHuiDisplay::EDisplayOffScreenBuffer )
			    {
			    gc =(MHuiFbsBitmapBufferGc*) new CHuiFbsBitmapBufferGc(
			        CHuiStatic::Renderer().OffScreenBitmap());
			    }
            }
		}
	return gc;     	
	}	
	
EXPORT_C void CHuiDisplay::AddScreenBufferObserverL(MHuiScreenBufferObserver* aObserver)
	{
	if (DisplayType() != CHuiDisplay::EDisplayOffScreenBuffer)
		{
		CHuiStatic::Renderer().EnableOffScreenBitmapL(0);
		}
								
	iScreenBufferObserver = aObserver;
	SetScreenBufferLock(EFalse); 
	}
	
	
EXPORT_C void CHuiDisplay::RemoveScreenBufferObserver()
	{
	iScreenBufferObserver = NULL;	
	
	if (DisplayType() != CHuiDisplay::EDisplayOffScreenBuffer)
		{
		CHuiStatic::Renderer().DisableOffScreenBitmap();
		}
	
	SetScreenBufferLock(EFalse); 
	}
     
     
EXPORT_C MHuiScreenBufferObserver*  CHuiDisplay::ScreenBufferObserver()
	{
	return iScreenBufferObserver;
	}
	

EXPORT_C void CHuiDisplay::DrawScreenBuffer() 
	{
	if (DisplayType() != CHuiDisplay::EDisplayOffScreenBuffer)
		{
		TRAP_IGNORE(CHuiStatic::Renderer().DrawOffScreenBitmapToWindowL(*this));			
		}					
	}


void CHuiDisplay::ClearChanged()
    {
    RosterImpl().ClearChanged();
    }
        
EXPORT_C void CHuiDisplay::SetUsageL(TUint aUsageHint)
    {
    iUsageHint = aUsageHint;    
    if (iRenderSurface)
        {
        iRenderSurface->HandleDisplayUsageChangeL();    
        }
    }

EXPORT_C TUint CHuiDisplay::Usage() const
    {
    return iUsageHint;     
    }
  
    
EXPORT_C TInt CHuiDisplay::GetPreferredTextureFormats(RPointerArray<CHuiDisplay::CTextureBitmapFormat>& aTextureFormats)
	{
    // Note: It would be better if preferred display mode was asked from the renderer itself
	TInt err = KErrNone;
	CHuiDisplay::CTextureBitmapFormat* format = NULL;
    if (!IsRendererHWAccelerated())
        {
        format = new CHuiDisplay::CTextureBitmapFormat(EColor64K, EGray256);
        if (format != NULL)
        	{
        	err = aTextureFormats.Append(format);
        	}
        }
    else
        {
        format = new CHuiDisplay::CTextureBitmapFormat(EColor16MA, ENone);
        if (format != NULL)
        	{
        	err = aTextureFormats.Append(format);
        	if (!err)
        		{
        		format = new CHuiDisplay::CTextureBitmapFormat(EColor64K, EGray256);
        		if (format != NULL)
        			{
        			err = aTextureFormats.Append(format);
        			}
        		}
        	}
        }
        			
	if (err)
    	{
        delete(format);
        }
        
    return (format == NULL) ? KErrNoMemory : err;	   
	}
  
    
CHuiTransformation& CHuiDisplay::WindowTransformation() const
    {
    return *iWindowTransform;        
    }

EXPORT_C TBool CHuiDisplay::IsDisplayTypeTvOut() const
   	{
   	return((iDisplayType == CHuiDisplay::EDisplayTvOut) || 
   	       (iDisplayType == CHuiDisplay::EDisplayTvOutWide) ||
   	       ((iDisplayType == CHuiDisplay::EDisplayNormal) && 
   	       ((iScreenBufferUid == KHuiUidBackBufferTvOutNormal) ||
   	        (iScreenBufferUid == KHuiUidBackBufferTvOutWide))));
   	}

TBool CHuiDisplay::IsRendererBitgdi() const
    {
    return (iEnv.Renderer() == EHuiRendererBitgdi);        
    }

TBool CHuiDisplay::IsRendererHWAccelerated() const
    {
    return (iEnv.Renderer() != EHuiRendererBitgdi);
    }

void CHuiDisplay::ClipDirtyRect(TRect& aRect)
    {
    // Check if dirty regions partially outside display are reported, limit
    // inside display area. 
    if(iGc)
        {            
        TRect displayRect = iGc->DisplayArea();        
        // If partially outside display area, we use intersection instead.
        if (aRect.Intersects(displayRect))
            {
            aRect.Intersection(displayRect);  
            }        
        }        
    }

void CHuiDisplay::TransformDirtyRect(TRect& aRect)
    {
    if ( iGc )
        {
        THuiRealRect realDirtyRect(aRect);
        iGc->TransformDirtyRect(realDirtyRect);
        aRect = TRect(HUI_ROUND_FLOAT_TO_INT(realDirtyRect.iTl.iX), HUI_ROUND_FLOAT_TO_INT(realDirtyRect.iTl.iY),
                      HUI_ROUND_FLOAT_TO_INT(realDirtyRect.iBr.iX), HUI_ROUND_FLOAT_TO_INT(realDirtyRect.iBr.iY));
        }
    }
    
TBool CHuiDisplay::UseTransformedDirtyRegions() const
    {
    return iUseTransformedDirtyRegions;    
    }
    
EXPORT_C void CHuiDisplay::ShowDirtyRegions( TBool aShow )
    {
    iDrawDirtyRegions = aShow;
    }

EXPORT_C void CHuiDisplay::SetDrawVisualOutline( TUint aDrawVisualOutline )
	{
	iDrawVisualOutline = aDrawVisualOutline;
	}
	 
TUint CHuiDisplay::DrawVisualOutline() const
	{
	return iDrawVisualOutline;
	}

TBool CHuiDisplay::IsDirtyScanNeeded() const
    {
    TUint flag = MHuiRenderSurface::EFlagUseDirtyRects;
	TBool useDirtyRects = (RenderSurface().Flags() & flag) == flag;
	return (useDirtyRects || !iGotDirtyReports);
    }
    
EXPORT_C void CHuiDisplay::SetBackgroundItemsL(const RArray<THuiDisplayBackgroundItem>& aItems)
    {
    iClearBackground = EClearNone;
    iBackgroundItems.Reset();
    for(TInt i = 0; i < aItems.Count(); ++i)
        {
        iBackgroundItems.AppendL(aItems[i]);
        }
    if (IsRendererHWAccelerated())
        {
        CHuiS60Skin* s60skin = static_cast<CHuiS60Skin*>(&iEnv.Skin());
        s60skin->UpdateBackgroundsL(iBackgroundItems);
        }
    SetDirty();    
    }

void CHuiDisplay::ClearWithColor(TRect aRect)
    {
    if (!IsRendererHWAccelerated())
        {
        BitgdiClearWithColor(aRect);    
        }
    else
        {
        HWAcceleratedClearWithColor(aRect);    
        }
    }

void CHuiDisplay::ClearWithSkinBackground(TRect aRect)
    {    
    if (!IsRendererHWAccelerated())
        {
        BitgdiClearWithSkinBackground(aRect);    
        }
    else
        {
        HWAcceleratedClearWithSkinBackground(aRect);    
        }
    }

void CHuiDisplay::ClearWithBackgroundItems(TRect aRect)
    {
    if (!IsRendererHWAccelerated())
        {
        BitgdiClearWithBackgroundItems(aRect);    
        }
    else
        {
        HWAcceleratedClearWithBackgroundItems(aRect);    
        }
    }

    
void CHuiDisplay::BitgdiClearWithColor(TRect /*aRect*/) 
    {
    iGc->Enable(CHuiGc::EFeatureClipping);
    iGc->Clear();        
    }
     
void CHuiDisplay::BitgdiClearWithSkinBackground(TRect aRect) 
    {
	CCoeControl* nativeControl = NativeControl();
   	CHuiS60Skin* s60skin = static_cast<CHuiS60Skin*>(&iEnv.Skin());
    CAknsBasicBackgroundControlContext* bgcontext = (CAknsBasicBackgroundControlContext*)s60skin->SkinControlContext();

    CFbsBitGc* bitGc = iRenderSurface->BackBuffer()->Gc();
	if (nativeControl)
		{
		bgcontext->SetRect(TRect(TPoint(0, -nativeControl->Position().iY), HuiUtil::ScreenSize()));
		}
	else
		{
		bgcontext->SetRect(TRect(TPoint(0, 0), HuiUtil::ScreenSize()));
		}

    // Draw the background onto the framebuffer
	MAknsSkinInstance* skin = CHuiStatic::SkinInstance();
	TPoint tl = aRect.iTl;
	TRect rect = aRect;
    bgcontext->SetBitmap(KAknsIIDQsnBgScreen);       

    AknsDrawUtils::DrawBackground(skin, bgcontext, NULL, 
    								*bitGc, tl, rect, 
                                  	KAknsDrawParamDefault);
    }

void CHuiDisplay::BitgdiClearWithBackgroundItems(TRect aRect)
    {
   	CHuiS60Skin* s60skin = static_cast<CHuiS60Skin*>(&iEnv.Skin());
    CAknsBasicBackgroundControlContext* bgcontext = (CAknsBasicBackgroundControlContext*)s60skin->SkinControlContext();

    CFbsBitGc* bitGc = iRenderSurface->BackBuffer()->Gc();
    THuiDisplayBackgroundItem item;    
    MAknsSkinInstance* skin = CHuiStatic::SkinInstance();

    for (TInt index = 0; index < iBackgroundItems.Count(); index++)
        {
        TRect clearRect = aRect;
        TRect skinRect = TRect(0,0,0,0);
        TRect dummy = TRect(0,0,0,0);

        item = iBackgroundItems[index];
        switch (item.ClearMode())
            {
            case EClearWithSkinBackground:
                if (skin)
                    {
                    GetRectForItem(item.SkinBackground(), dummy, skinRect);
                    bgcontext->SetRect(skinRect);
                    bgcontext->SetBitmap(item.SkinBackground());       
                    skinRect.Intersection(aRect);
                    skinRect.Intersection(item.Rect());
  		            AknsDrawUtils::DrawBackground(skin, bgcontext, NULL, 
							*bitGc, skinRect.iTl, skinRect, 
                          	KAknsDrawParamDefault);
                    break;
                    } // else fall through
            case EClearWithColor:
                clearRect.Intersection(item.Rect());
                if (!clearRect.IsEmpty())
                    {
                    bitGc->SetBrushColor(item.Color());
                    bitGc->Clear(clearRect);                                    
                    }
                break;
            case EClearNone:
            default: 
                // do nothing...
                break;
            }
        }
        
    }
    
void CHuiDisplay::HWAcceleratedClearWithColor(TRect /*aRect*/)
    {
    iGc->SetPenColor(iBackgroundColor);
    iGc->SetPenAlpha(iBackgroundColor.Alpha());
    iGc->Disable(CHuiGc::EFeatureBlending);
    iGc->Enable(CHuiGc::EFeatureClipping);
    iGc->Clear();
    }

void CHuiDisplay::HWAcceleratedClearWithSkinBackground(TRect /*aRect*/)
    {
    // Acquire background texture
    const CHuiTexture* backgroundTexture = NULL;
    TInt err = iEnv.Skin().GetTexture(EHuiSkinBackgroundTexture, backgroundTexture);
    if( err )
        {
        return;
        }

    // Apply background texture
    THuiImage background(*backgroundTexture);

    TPoint screenOrigin(0, 0);
    iRenderSurface->GetScreenOrigin(screenOrigin);
    // The origin is used to offset the background in the display's
    // rendering surface, so that the background's origin is in the
    // top left screen corner.
    screenOrigin.iX = -screenOrigin.iX;
    screenOrigin.iY = -screenOrigin.iY;

    iGc->SetPenColor(KRgbWhite);
    iGc->SetPenAlpha(255);
    iGc->SetAlign(EHuiAlignHLeft, EHuiAlignVTop);            
    iGc->Disable(CHuiGc::EFeatureBlending);
    iGc->Enable(CHuiGc::EFeatureClipping);            
    iGc->DrawImage(background, screenOrigin, background.Texture().Size());        
    }

void CHuiDisplay::HWAcceleratedClearWithBackgroundItems(TRect aRect)
    {
    THuiDisplayBackgroundItem item;
	CHuiS60Skin* s60skin = static_cast<CHuiS60Skin*>(&iEnv.Skin());
    CHuiTexture* backgroundTexture = NULL;
    for (TInt index = 0; index < iBackgroundItems.Count(); index++)
        {
        item = iBackgroundItems[index];
        switch (item.ClearMode())
            {
            case EClearNone:
                // do nothing...
                break;
            case EClearWithColor:
                // do not draw anything if background item rect does not intersect with dirty/ current clip rect
                if( item.Rect().Intersects(aRect)) 
                    {
                    iGc->SetPenColor(item.Color());
                    iGc->SetPenAlpha(255);
                    iGc->SetAlign(EHuiAlignHLeft, EHuiAlignVTop);            
                    iGc->Disable(CHuiGc::EFeatureBlending);
                    iGc->Enable(CHuiGc::EFeatureClipping);            
                    iGc->PushClip();
                    TRect temp = item.Rect();
                    temp.Intersection(aRect);
                    iGc->Clip(temp);
                    iGc->Clear();
                    iGc->PopClip();
                    }
                break;
            case EClearWithSkinBackground:
                 backgroundTexture = s60skin->BackgroundTexture(item.SkinBackground());
                 if (backgroundTexture)
                    {
                    TRect skinRect = s60skin->SkinRect(item.SkinBackground());
                    THuiImage background(*backgroundTexture);

                    TPoint screenOrigin(0, 0);
                    iRenderSurface->GetScreenOrigin(screenOrigin);
                    screenOrigin.iX = -screenOrigin.iX;
                    screenOrigin.iY = -screenOrigin.iY;
                    screenOrigin+=skinRect.iTl;
                    
                    // do not draw anything if background item rects does not intersect with dirty/ current clip rect
                    skinRect.Intersection(item.Rect());
                    if( skinRect.Intersects(aRect) )
                        {
                        iGc->SetPenColor(KRgbWhite);
                        iGc->SetPenAlpha(255);
                        iGc->SetAlign(EHuiAlignHLeft, EHuiAlignVTop);            
                        iGc->Disable(CHuiGc::EFeatureBlending);
                        iGc->Enable(CHuiGc::EFeatureClipping);            
                        iGc->PushClip();
                        skinRect.Intersection(aRect);
                        iGc->Clip(skinRect);
                        iGc->DrawImage(background, screenOrigin, background.Texture().Size());                     
                        iGc->PopClip();
                        }
                    }
                 break;                                                    
             }
        }             
    }
    
EXPORT_C void CHuiDisplay::SetVisibleAreaClippingRect(const TRect& 
        #ifndef SYMBIAN_BUILD_GCE  
        aVisibleAreaClippingRect 
        #endif  
        )
    {
// Visible area clipping is disabled in NGA    
#ifndef SYMBIAN_BUILD_GCE    
    iVisibleAreaClippingRect = aVisibleAreaClippingRect;   
#endif
    } 
    
TRect CHuiDisplay::VisibleAreaClippingRect() const
    {
// Visible area clipping is disabled in NGA    
#ifndef SYMBIAN_BUILD_GCE    
    if (IsVisibleAreaClippingEnabled())
        {
        return iVisibleAreaClippingRect;             
        }
    else
        { 
        return VisibleArea();             
        }        
#else
    return VisibleArea();                 
#endif    
    }
    
TBool CHuiDisplay::IsVisibleAreaClippingEnabled() const
    {
// Visible area clipping is disabled in NGA    
#ifndef SYMBIAN_BUILD_GCE    
    return iVisibleAreaClippingRect != TRect(0,0,0,0);            
#else
    return EFalse;
#endif
    }

void CHuiDisplay::ScanningAlfContent(TBool aScanning )
    {
    iScanningAlfContent = aScanning;
    }

void CHuiDisplay::SetAlfContentChanged(TBool aChanged)
    {
    iDisplayContainsChangedAlfContent = aChanged;
    }

TBool CHuiDisplay::AlfContentChanged()
    {
    return iDisplayContainsChangedAlfContent;
    }

EXPORT_C void CHuiDisplay::SetSkinSizeChangePending()
    {
    iDisplaySizeChangePending = ETrue;
    }

TBool CHuiDisplay::QueryAndResetSkinSizeChangePendingStatus()
    {
    TBool ret = iDisplaySizeChangePending;
    iDisplaySizeChangePending = EFalse;
    return ret;
    }

void CHuiDisplay::ClipDirtyRect(TRect& aRect, TRect aClippingRect)
    {
    if (aRect.Intersects(aClippingRect))
        {
        aRect.Intersection(aClippingRect);  
        }                
    }
    
EXPORT_C void CHuiDisplay::SetForegroundTexture(CHuiTexture* aTexture)
    {
    if (iForegroundTexture != aTexture)
        {
        delete iForegroundTexture;
        iForegroundTexture = aTexture;
        SetDirty();
        }
    }

EXPORT_C CHuiTexture* CHuiDisplay::ForegroundTexture() const
    {
    return iForegroundTexture;
    }

EXPORT_C void CHuiDisplay::SetForegroundTextureOptions(TBool aTransparency)
    {
    iForegroundTextureTransparency = aTransparency;

    if (!iForegroundTextureTransparency && iForegroundTexture)
        {
        // When we can blit foreground texture, we shouldn't need any textures in skin side.
        iEnv.Skin().ReleaseCachedTextures();
        }
    }

void CHuiDisplay::UpdateForegroundTexture(const TRect& aRect)
    {
    if (iForegroundTexture && iForegroundBitmap && !aRect.IsEmpty())
        {
#if defined(DEBUG_SW_MODE_DIRTY_AREAS)
        TRgb penColor = TRgb(Math::Random());
        penColor.SetAlpha(128);
        iForegroundBitmapGc->SetPenStyle(CGraphicsContext::ESolidPen);
        iForegroundBitmapGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
        iForegroundBitmapGc->SetPenColor(penColor);
        iForegroundBitmapGc->SetBrushColor(penColor);
        iForegroundBitmapGc->DrawRect(aRect);
#endif
        TRAP_IGNORE(DoUpdateForegroundTextureL(aRect));
        }
    }

void CHuiDisplay::DoUpdateForegroundTextureL(const TRect& aRect)
    {
    // First try to upload with faster mechanism
    THuiTexturePartialBitmapUploadParams params;
    params.iErrorCode = KErrNotSupported;
    params.iRect = aRect;
    params.iBitmap = iForegroundBitmap;
    TAny* ptr = &params;
    
    MHuiTexture* texture = iForegroundTexture;
    texture->TextureExtension(KHuiTexturePartialBitmapUploadUid, &ptr);
    if ( params.iErrorCode != KErrNone )
        {    
        iForegroundTexture->UploadL(*iForegroundBitmap, NULL);
        }
    }

void CHuiDisplay::DrawForegroundTexture()
    {
    if (iForegroundTexture && !RosterImpl().IsVisibleContentFrozen() )
        {
        THuiQuality originalQuality = iGc->Quality();
        if ( originalQuality != EHuiQualityFast )
            {
            // Enforce fast quality to be used - we want one to one mapping
            // & no antialiasing.
            iGc->SetQuality( EHuiQualityFast );
            }            
    
        // The following is similar to HWAcceleratedClearWithSkinBackground,
        // except blending is enabled.
        
        TPoint screenOrigin(0, 0);
        iRenderSurface->GetScreenOrigin(screenOrigin);
        screenOrigin.iX = -screenOrigin.iX;
        screenOrigin.iY = -screenOrigin.iY;

        THuiImage image = THuiImage(*iForegroundTexture);
        iGc->SetPenColor(KRgbWhite);
        iGc->SetPenAlpha(255);
        iGc->SetAlign(EHuiAlignHLeft, EHuiAlignVTop);            
        
        if ( iForegroundTextureTransparency )
            {
            iGc->Enable(CHuiGc::EFeatureBlending);
            }
        else
            {
            iGc->Disable(CHuiGc::EFeatureBlending);
            }
               
        iGc->Enable(CHuiGc::EFeatureClipping);            
        iGc->DrawImage(image, screenOrigin, image.Texture().Size());        

        if ( originalQuality != EHuiQualityFast )
            {
            // Restore original quality.
            iGc->SetQuality( originalQuality );
            }        
        }
    }
    
EXPORT_C void CHuiDisplay::SetForegroundBitmapL(CFbsBitmap* aBitmap)
    {
    // Note this method may be called with same bitmap, e.g.
    // when emulator display orientation is changed. Then we recreate
    // device & context in order to avoid panics.
    
    if ( !aBitmap )
        {
        delete iForegroundBitmapGc;
        iForegroundBitmapGc = NULL;
        delete iForegroundBitmapDevice;
        iForegroundBitmapDevice = NULL;
        iForegroundBitmap = NULL;
        }
    else
        {
        CFbsBitmapDevice* device = CFbsBitmapDevice::NewL( aBitmap );
        CleanupStack::PushL( device );
        
        CFbsBitGc* gc = NULL;
        User::LeaveIfError( device->CreateContext( gc ) );
        User::LeaveIfNull( gc );
        
        CleanupStack::Pop( device );
        
        iForegroundBitmap = aBitmap;
        delete iForegroundBitmapDevice;
        iForegroundBitmapDevice = device;
        delete iForegroundBitmapGc;
        iForegroundBitmapGc = gc;
        }
    }
    
EXPORT_C CFbsBitmap* CHuiDisplay::ForegroundBitmap() const
    {
    return iForegroundBitmap;
    }

EXPORT_C void CHuiDisplay::CopyScreenToBitmapL(CFbsBitmap* aBitmap)
    {
    MHuiRenderSurface* oldSurface = CHuiStatic::CurrentRenderSurface();
    RenderSurface().MakeCurrent();
    
    TInt err = iGc->CopyScreenToBitmap( aBitmap );
        
    if (oldSurface && oldSurface != &RenderSurface())
        {
        oldSurface->MakeCurrent();
        }
        
    User::LeaveIfError( err );
    }

void CHuiDisplay::DoBackgroundClear()
    {
    // Clear background for the dirty area
    TRect dirtyRect = iTempDirtyRegions[iCurrentDirtyIndx];
    if (iBackgroundItems.Count() != 0)
        {
        ClearWithBackgroundItems(dirtyRect);    
        }
    else
        {
        switch (iClearBackground)
            {
            case EClearWithColor:
                {
                ClearWithColor(dirtyRect);                            
                break;    
                }
            case EClearWithSkinBackground:
                {
                ClearWithSkinBackground(dirtyRect);                                                    
                break;    
                }
            case EClearNone:
            default:
                {
                // Don't do anything
                break;    
                }                                                
            }                                    
        }
    }

TRect CHuiDisplay::CurrentDirtyRect()
    {
    return iCurrentDirtyRect;
    }
