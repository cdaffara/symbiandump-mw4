/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition of CHuiCanvasWsPainter.
*
*/



#include "huicanvaswspainter.h"

#include "uiacceltk/HuiCanvasVisual.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiGc.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiVisual.h"
#include "uiacceltk/HuiTextMesh.h"
#include "uiacceltk/HuiCanvasCmdBufferReader.h"
#include "huicanvasgc.h"
#include "huicanvaswsgc.h"
#include "HuiRenderSurface.h"
#include "huicanvastexturecache.h"
#include "huicanvasbackground.h"
#include "huicanvaswsswgc.h"
#include "huicanvasdebugwsgc.h"
#include "huicanvaswsbitgc.h"
#include "../../CommonInc/huiwscanvascommands.h"
#include <graphics/wsgraphicscontext.h>

#ifdef HUI_DEBUG_TRACK_DRAWING
#include "alfloggingconfiguration.h"
#include "alfcommanddebug.h"
#include "huistatictlsdata.h"
#endif
#include <e32cmn.h>
#include <AknLayoutFont.h>
#include <graphics/lookuptable.h>

#include "HuiCmdBufferBrush.h"

#include "alfmoduletestconf.h"
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
    // Provides TLS object data for test cases.
    // This is used only if module test hooks are set on.
    #include "huistatictlsdata.h"
#endif // USE_MODULE_TEST_HOOKS_FOR_ALF
// Provides module test hook defines.
#include "alfmoduletestdefines.h"

/** 
 * Constants to define enabling/disabling render buffer automatically 
 *  incase there are frequently new buffers posted. 
 * 
 *  This may improve performance in some HW platforms.
 * 
 */
const TBool KHuiCanvasAutomaticRenderBufferUsage = EFalse; // Disbaled until all UI isses has been solved  
const TInt KHuiCanvasDisableRenderBufferHandleInterval = 100; // Milliseconds 
const TInt KHuiCanvasEnableRenderBufferHandleInterval = 500;  // Milliseconds

/** ETrue - use SelectGcL to select gc implementation, 
    EFalse - use KHuiDefaultCanvasWsGc. */
const TBool KHuiUseSelectGc = ETrue;

/** Change this to select default rendering ws gc implementation.
    Only used when KHuiUseSelectGc does not hold. */
const THuiCanvasWsGcType KHuiDefaultCanvasWsGc = EHuiCanvasWsHw;

//#define HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL

CHuiCanvasWsPainter* CHuiCanvasWsPainter::NewL()
	{
	CHuiCanvasWsPainter* self    = new ( ELeave ) CHuiCanvasWsPainter;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
	return self;
    }


CHuiCanvasWsPainter::CHuiCanvasWsPainter()
    {
    }

void CHuiCanvasWsPainter::ConstructL()
    {
    CHuiStatic::Env().AddMemoryLevelObserver(this);
    iWsCommandBufferReader = NULL;
    iPartialCommandBuffer = NULL;
    iCacheCleared = EFalse;

    if ( KHuiUseSelectGc )
        {
        SelectGcL();
        }
    else if (KHuiDefaultCanvasWsGc == EHuiCanvasWsHw)
        {
        iCanvasWsHwGc = CHuiCanvasWsHwGc::NewL();
        iCanvasWsGc = iCanvasWsHwGc;        
        }
    else if (KHuiDefaultCanvasWsGc == EHuiCanvasWsSw)
        {
        iCanvasWsSwGc = CHuiCanvasWsSwGc::NewL();
        iCanvasWsGc = iCanvasWsSwGc;            
        }    
    else if (KHuiDefaultCanvasWsGc == EHuiCanvasWsBit)
        {
        iCanvasWsBitGc = CHuiCanvasWsBitGc::NewL();
        iCanvasWsGc = iCanvasWsBitGc;            
        }    
    else
        {
        // Never happens, right.
        }
    
    iWsCommandBufferReader = new (ELeave) CHuiCanvasCmdBufferReader(); 
    
    #ifdef HUI_DEBUG_TRACK_DRAWING
    if (!iCanvasDebugWsGc)
        {
        iCanvasDebugWsGc = CHuiCanvasDebugWsGc::NewL( iCanvasWsGc, *TTlsData::CommandDebugger() );
        iCanvasWsGc = iCanvasDebugWsGc;
        }
    
    #endif
    }


CHuiCanvasWsPainter::~CHuiCanvasWsPainter()
    {
    if (iCanvasVisual)
        {
        iCanvasVisual->Env().RemoveMemoryLevelObserver(this);
        }
    else
        {
        CHuiStatic::Env().RemoveMemoryLevelObserver(this);
        }
    
    ClearCommandSet();	
    delete iWsCommandBufferReader;	
    delete iCanvasWsHwGc;
    delete iCanvasWsSwGc;
    delete iCanvasWsBitGc;

    iTempRegion.Close();
    iTempCurrentSubRegion.Close();
    iTempIntersectingRegion.Close();
#ifdef HUI_DEBUG_TRACK_DRAWING
    delete iCanvasDebugWsGc;
#endif
    iFullUpdateRegion.Close();
    iShapeRegion.Close();
    }
    

CHuiGc::TOrientation CHuiCanvasWsPainter::CalculateRelativeOrientation()
    {
    // Calculate drawing command rotation as relative to the current display rotation.
    // It may be that drawing commands has been sent when display was in different rotation.
    TInt last = iCommandBuffers.Count() - 1;
    CHuiGc::TOrientation relativeOrientation = CHuiGc::EOrientationNormal;    
    CHuiGc::TOrientation displayOrientation = CHuiStatic::Env().PrimaryDisplay().Orientation(); 
    CHuiGc::TOrientation drawingCommandOrientation = iCommandBuffers[last]->iOrientation; 
    
    TInt rotationAngle[] =
        {
        0, 
        270, 
        90, 
        180
        };
        
    TInt displayOrientationAngle = rotationAngle[displayOrientation];
    TInt drawingCommandOrientationAngle = rotationAngle[drawingCommandOrientation];    
    TInt relativeOrientationAngle = drawingCommandOrientationAngle - displayOrientationAngle;
    
    // Check over/underflow
    if (relativeOrientationAngle > 360)
        {
        relativeOrientationAngle -= 360;
        }
    else if (relativeOrientationAngle < 0)
        {
        relativeOrientationAngle += 360;
        }
    
    switch (relativeOrientationAngle)
        {
        case 0:
            {
            relativeOrientation = CHuiGc::EOrientationNormal;
            break;
            }
        case 90:
            {
            relativeOrientation = CHuiGc::EOrientationCW90;            
            break;
            }
        case 270:
            {
            relativeOrientation = CHuiGc::EOrientationCCW90;            
            break;
            }
        case 180:
            {
            relativeOrientation = CHuiGc::EOrientation180;            
            break;
            }
        default:
            {
            // As usual, this should not happen...but just in case it does use normal rotation
            relativeOrientation = CHuiGc::EOrientationNormal;
            break;
            }        
        }
    return relativeOrientation;
    }

void CHuiCanvasWsPainter::HandleBufferL(TRect& aDisplayRect, TInt aAction, const CHuiCanvasVisual& aUser, CHuiGc* aGc, TPoint aPos ) 
    {   
	#ifdef __ALF_SYMBIAN_RWINDOW_CLEARING_BUG_WORKAROUND__
    iTestFullDisplayRect = aDisplayRect;
	#endif
    
    // Store user so that we can clear cache later
    iCanvasVisual = (CHuiCanvasVisual*)&aUser;

    // Let canvas know about user and gc
    CHuiCanvasGc& canvasGc = iCanvasWsGc->CanvasGc();
    canvasGc.SetVisual(aUser);
    if (aGc)
        {
        canvasGc.SetGc(*aGc);
        }
    
    if (iCommandBuffers.Count() != 0)
        {
#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        RDebug::Print(_L(">> CHuiCanvasWsPainter::HandleBufferL this = %i, Action = %i"), this, aAction );
        TTime startTime;
        startTime.UniversalTime();
#endif

        // Initialize canvas
        canvasGc.SetDefaults();   

        // Ensure that all regions have been shifted to correct origin (before calculating iFullUpdateRegion).
        UpdateBufferUpdateRegions(aPos);
        
        // Make sure we got up to date update reagion
        iFullUpdateRegion.Clear();                
        TInt bufferCount = iCommandBuffers.Count();
        for (TInt cb = 0; cb < bufferCount; cb++)
            {        
            iFullUpdateRegion.Union(iCommandBuffers[cb]->iUpdateRegion);
            }
        iFullUpdateRegion.Tidy();
        
#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        const TRect updateRegionBoundingRect = iFullUpdateRegion.BoundingRect();        
        RDebug::Print(_L(">> CHuiCanvasWsPainter::HandleBufferL iFullUpdateRegion.BoundingRect() = (%i,%i) (%i,%i)"), 
                updateRegionBoundingRect.iTl.iX,
                updateRegionBoundingRect.iTl.iY,
                updateRegionBoundingRect.iBr.iX,
                updateRegionBoundingRect.iBr.iY
                );

        if (iCanvasWsGc->Type() == EHuiCanvasWsSw)
            {
            RDebug::Print(_L(">> CHuiCanvasWsPainter::HandleBufferL - SW mode is used."));
            }
        else if (iCanvasWsGc->Type() == EHuiCanvasWsBit)
            {
            RDebug::Print(_L(">> CHuiCanvasWsPainter::HandleBufferL - BITGC mode is used."));
            }
        
        if (aUser.Effect())
            {
            RDebug::Print(_L(">> CHuiCanvasWsPainter::HandleBufferL - visual has EFFECT"));
            }
        
        
#endif
        
        if (aAction == EDrawBuffer)
            {
            /* Check and enable render buffer if needed */
            EnableRenderBufferIfNeeded(ETrue);

            // Compenstate possible orientation mismatch between drawing commands and display orientation
            // by letting iCanvasWsGc know about it. Mismatch usually happens when screen is rotated and
            // it may cause visually unpleasent things at the screen.
            CHuiGc::TOrientation relativeOrientation = CalculateRelativeOrientation();
            iCanvasWsGc->SetRelativeOrientation(relativeOrientation);
            #ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
            RDebug::Print(_L(">> CHuiCanvasWsPainter::HandleBufferL - relativeOrientation = %i "), relativeOrientation);    
            #endif
            
            /* If window shape region has been changed, we should clear the window to get rid of old content. 
             * We also set all command buffers to "not drawn" state so that everything will be redrawn.
             * Actual clearing is done after BeginActionL, because drawing phase will start only after calling it.
             */
            if(iShapeRegionClearingPending)
                {
                // Cause a full redraw for the canvas visual
                SetAllBuffersChanged(ETrue);    
                ClearAllBufferStatusFlags(EHuiCanvasBufferStatusDrawn);
                }
            
            
            /* Check if we have buffers that are not yet been drawn (or we render buffer is disabled) */
            TBool cachePrepared = !HasCommandBuffersWithoutFlags(EHuiCanvasBufferStatusDrawn) || !iCanvasWsGc->IsRenderBufferEnabled();

            /* Begin draw. If render buffer is used this sets up the render buffer if needed */
            iCanvasWsGc->BeginActionL(aAction,aDisplayRect,aUser,cachePrepared,iFullUpdateRegion);                   

            if(iShapeRegionClearingPending)
                {
                #ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
                    RDebug::Print(_L(">> CHuiCanvasWsPainter::HandleBufferL - Clearing fullupdateregion"));    
                #endif
                // Clear the window update region area
                TBool doClear = ETrue;
                iCanvasWsGc->EnableUpdateRegion(iFullUpdateRegion, doClear);
                iCanvasWsGc->DisableUpdateRegion(); 
                iShapeRegionClearingPending = EFalse;
                }

            #ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
            if (iCanvasWsGc->IsRenderBufferEnabled())
                {
                RDebug::Print(_L(">> CHuiCanvasWsPainter::HandleBufferL renderbuffer is ENABLED"));
                }
            else
                {
                RDebug::Print(_L(">> CHuiCanvasWsPainter::HandleBufferL renderbuffer is DISABLED"));
                }
            #endif
            
            /* We do not ignore unchanged buffers currently */
            TBool ignoreUnChangedBuffers = EFalse; 

            /* We can ignore buffers that only update areas which get clipped */
            TBool ignoreClippedBuffers = ETrue;            
            if (relativeOrientation != CHuiGc::EOrientationNormal)
                {
                ignoreClippedBuffers = EFalse;
                }
            
            /* We can ignore buffers that has previously already been drawn into render buffer */
            TBool ignoreHandledBuffers = iCanvasWsGc->IsRenderBufferEnabled() && iCanvasWsGc->IsRenderBufferInitialized();                    

            
            /* 
             * We need to clear area if render buffer is used because otherwise it will cause dimming for semitransparent areas.
             * 
             * NOTE1: We do not clear if the window is opaque. This is a bit questionable, but Avkon seems to rely
             * quite much on luck with its drawing and there are areas in the windows which are not always covered
             * by drawing commands, thus random garabage from previous frames would be visible if such window
             * update area would always be cleared.
             * 
             * NOTE2: We handle window that has background clearing enabled similarly as transparent window
             *        because their wanted behaviour is similar to transparent window (user wants to see the stuff
             *        behind window drawing commands and may want to draw commands translucently). 
             *        
             * -> So always clear if drawing vie render buffer and window is transparent or has backround.       
             *        
             */            
            TBool clearBeforeHandlingBuffer = iCanvasWsGc->IsRenderBufferEnabled() 
                && (!(aUser.Flags() & EHuiVisualFlagOpaqueHint) || aUser.IsBackgroundDrawingEnabled());

                        
            // TODO: Should avoid trap, but EndActionL must always be called
            TRAPD(err, DoHandleAllBuffersL( aDisplayRect, aAction, aUser, aGc, aPos, 
                    ignoreUnChangedBuffers, 
                    ignoreClippedBuffers,
                    ignoreHandledBuffers,
                    clearBeforeHandlingBuffer));
            
            if (err != KErrNone)
                {
                RDebug::Print(_L("CHuiCanvasWsPainter::HandleBufferL(): draw error = %i "), err);
                RenewAllBuffers();
                }
            
            // Workaround for fact that WSERV sends command buffers that do not contain real drawing commands.
            // If there are no real drawing commnads, just avoid display update at the EndAction so that
            // we don't write empty pixels to screen and cause flicker. 
            TBool updateDisplay = HasCommandBuffers(EHuiCanvasBufferContainsDrawing);
            
            /* End draw. If render buffer is in use, this will update the screen */
            iCanvasWsGc->EndActionL(iFullUpdateRegion, updateDisplay);                 
            
            /* Mark that cache is not more clear */
            iCacheCleared = EFalse;
            }
        else if (aAction == EScanBuffer)
            {
            /* Check and enable render buffer if needed */
            EnableRenderBufferIfNeeded(ETrue);
            
            /* Check if we have buffers that are not yet prepared (or we render buffer is disabled) */
            TBool cachePrepared = !HasCommandBuffersWithoutFlags(EHuiCanvasBufferStatusScanned) || !iCanvasWsGc->IsRenderBufferEnabled();

            /* Begin scan. If render buffer is used this sets up the render buffer if needed */
            iCanvasWsGc->BeginActionL(aAction,aDisplayRect,aUser,cachePrepared,iFullUpdateRegion);    

            #ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
            if (iCanvasWsGc->IsRenderBufferEnabled())
                {
                RDebug::Print(_L(">> CHuiCanvasWsPainter::HandleBufferL renderbuffer is ENABLED"));
                }
            else
                {
                RDebug::Print(_L(">> CHuiCanvasWsPainter::HandleBufferL renderbuffer is DISABLED"));
                }
            #endif
            
            /* We do not ignore unchanged buffers currently */
            TBool ignoreUnChangedBuffers = EFalse;
            
            /* We do not ignore clipped buffers as they possibly contain items that may soon appear on the screen */
            TBool ignoreClippedBuffers = EFalse;                    
            
            /* We can ignore buffers that has previously already been scanned */
            TBool ignoreHandledBuffers = ETrue;     

            /* No need to clear area if scanning */
            TBool clearBeforeHandlingBuffer = EFalse;
            
            // TODO: Should avoid trap, but EndActionL must always be called
            TRAPD(err, DoHandleAllBuffersL( aDisplayRect, aAction, aUser, aGc, aPos, 
                    ignoreUnChangedBuffers, 
                    ignoreClippedBuffers,
                    ignoreHandledBuffers,
                    clearBeforeHandlingBuffer));    
            
            if (err != KErrNone)
                {
                RDebug::Print(_L("CHuiCanvasWsPainter::HandleBufferL(): scan error = %i "), err);
                RenewAllBuffers();
                }
            
            /* End draw. If render buffer is in use, this will update the screen */
            iCanvasWsGc->EndActionL(iFullUpdateRegion, EFalse);

            /* Mark that cache is not more clear */
            iCacheCleared = EFalse;
            
            /* Update dirty area to aDisplayRect in case this action is originated from dirty area scan */
            aDisplayRect = DirtyRect();
            }
            
                        
#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        TTime endTime;
        endTime.UniversalTime();
        TInt timeInMs =  endTime.MicroSecondsFrom( startTime ).Int64()/1000;           
        RDebug::Print(_L(">> CHuiCanvasWsPainter::HandleBufferL took %i ms"), timeInMs);
#endif
        }
        
    }

void CHuiCanvasWsPainter::DoHandleAllBuffersL( TRect& aDisplayRect, 
                                               TInt aAction, 
                                               const CHuiCanvasVisual& aUser, 
                                               CHuiGc* aGc, 
                                               TPoint& aPos, 
                                               TBool aIgnoreUnChangedBuffers, 
                                               TBool aIgnoreClippedBuffers, 
                                               TBool aIgnoreHandledBuffers,
                                               TBool aClearBeforeHandlingBuffers) 
    {
    TInt bufferCount = iCommandBuffers.Count();
    for (TInt cb = 0; cb < bufferCount; cb++)
        {
        CHuiCanvasCommandBuffer* cmdbuffer = iCommandBuffers[cb];

        #ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        RDebug::Print(_L(">> CHuiCanvasWsPainter::DoHandleAllBuffersL Buffer size: %i "), iCommandBuffers[cb]->iCommands->Size());                
        for (TInt k=0; k < cmdbuffer->iUpdateRegion.Count();k++)
            {
            RDebug::Print(_L(">> CHuiCanvasWsPainter::DoHandleAllBuffersL Buffer region rect: %i,%i, %i,%i "), 
                cmdbuffer->iUpdateRegion[k].iTl.iX,
                cmdbuffer->iUpdateRegion[k].iTl.iY,
                cmdbuffer->iUpdateRegion[k].iBr.iX,
                cmdbuffer->iUpdateRegion[k].iBr.iY);                
            }
        #endif

        if (aIgnoreUnChangedBuffers && !cmdbuffer->iChanged)
            {
            #ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
            RDebug::Print(_L(">> CHuiCanvasWsPainter::DoHandleAllBuffersL Skipping buffer, it has not changed"));                
            #endif            
            continue;            
            }

        if (aIgnoreHandledBuffers)
            {
            if (aAction == EDrawBuffer && (cmdbuffer->iStatusFlags & EHuiCanvasBufferStatusDrawn))
                {
                #ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
                RDebug::Print(_L(">> CHuiCanvasWsPainter::DoHandleAllBuffersL Skipping buffer, it has already been drawn."));                
                #endif                            
                continue;
                }
            else if (aAction == EScanBuffer && (cmdbuffer->iStatusFlags & EHuiCanvasBufferStatusScanned))
                {
                #ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
                RDebug::Print(_L(">> CHuiCanvasWsPainter::DoHandleAllBuffersL Skipping buffer, it has already been scanned."));                
                #endif            
                continue;            
                }
            else
                {
                // No, we dont ignore.                                
                }
            }
            
        if (aIgnoreClippedBuffers && IsBufferCompletelyOutisideClippingRegion(*cmdbuffer))
            {
            #ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
            RDebug::Print(_L(">> CHuiCanvasWsPainter::DoHandleAllBuffersL: Skipping buffer %i, as it is completely clipped"), cb);
            #endif                    
            continue;
            }
              
        if (aGc)
            {
            #ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
            RDebug::Print(_L(">> CHuiCanvasWsPainter::DoHandleAllBuffersL ClipRegion before clipping to updateregion:"));                
            const TRegion& originalclipregion = aGc->ClipRegion(); 
            for (TInt k=0; k < originalclipregion.Count();k++)
                {
                
                RDebug::Print(_L(">> CHuiCanvasWsPainter::DoHandleAllBuffersL original clip region rect: %i,%i, %i,%i "), 
                        originalclipregion[k].iTl.iX,
                        originalclipregion[k].iTl.iY,
                        originalclipregion[k].iBr.iX,
                        originalclipregion[k].iBr.iY);                
                }
            #endif

            
            // If buffers has been scanned, but it does not seem to contain any real drawing commands we discard it            
            if (!(cmdbuffer->iStatusFlags & EHuiCanvasBufferContainsDrawing) && (cmdbuffer->iStatusFlags & EHuiCanvasBufferStatusScanned))
                {
                #ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
                RDebug::Print(_L("CHuiCanvasWsPainter::DoHandleAllBuffersL: Hmmm, WSERV has sent buffer without real drawing commands."), cb);
                #endif
                continue;
                }
            
            // Clip drawing to update area. This seems to improve performance although
            // it may depend on used HW.

            // Set position delta to zero, otherwise updateregion is adjusted incorrectly
            // in SW mode. No effect on HW mode as it does not adjust the updateregion in
            // EnableUpdateRegion call...
            TPoint delta(0,0);
            iCanvasWsGc->SetPositionDelta(delta);
            iCanvasWsGc->EnableUpdateRegion(iCommandBuffers[cb]->iUpdateRegion, aClearBeforeHandlingBuffers);

            #ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
            RDebug::Print(_L(">> CHuiCanvasWsPainter::DoHandleAllBuffersL ClipRegion after clipping to updateregion:"));                
            const TRegion& clipregion = aGc->ClipRegion(); 
            for (TInt k=0; k < originalclipregion.Count();k++)
                {
                
                RDebug::Print(_L(">> CHuiCanvasWsPainter::DoHandleAllBuffersL current clip region rect: %i,%i, %i,%i "), 
                        clipregion[k].iTl.iX,
                        clipregion[k].iTl.iY,
                        clipregion[k].iBr.iX,
                        clipregion[k].iBr.iY);                
                }
            #endif
            
            TRAPD(error, DoHandleBufferL(cb, aDisplayRect, aAction, aUser, aGc, aPos ));
            
            if (error)
                {
                #ifdef _DEBUG
                RDebug::Print(_L("CHuiCanvasWsPainter::DoHandleAllBuffersL: Error in buffer %i, continuing."), cb);
                #endif
                }
            
            iCanvasWsGc->DisableUpdateRegion();
            }
        else
            {
            DoHandleBufferL(cb, aDisplayRect, aAction, aUser, aGc, aPos);
            }
        
        // Update buffer status
        if (aAction == EDrawBuffer)
            {
            cmdbuffer->SetStatusFlags(EHuiCanvasBufferStatusDrawn);
            }
        else if (aAction == EScanBuffer)
            {
            cmdbuffer->SetStatusFlags(EHuiCanvasBufferStatusScanned);            
            }
        else
            {
            // This should not happen
            }
        }         
    }


void CHuiCanvasWsPainter::HandleBufferL(TRect& aDisplayRect, TInt aAction, const CHuiCanvasVisual& aUser, CHuiGc* aGc, TPoint aPos, TPtrC8 aCommands, CHuiCanvasCommandBuffer * /*buf*/)
    {
    TUint8 *ptr = const_cast<TUint8*>(aCommands.Ptr());
    TInt length = aCommands.Length();
    TPtr8 cmds(ptr, length, length);
    CHuiCanvasCommandBuffer *buf = new (ELeave)CHuiCanvasCommandBuffer;
    CleanupStack::PushL(buf);
    buf->iUpdateRegion.AddRect(aDisplayRect);
    if (iCanvasVisual && iCanvasVisual->Display())
        {
        buf->iOrientation = iCanvasVisual->Display()->Orientation();
        }
    DoHandleBufferStringL(-1, aDisplayRect, aAction, aUser, aGc, aPos, cmds, buf);
    CleanupStack::PopAndDestroy();
    }

void CHuiCanvasWsPainter::DoPeekBufferAndUpdateShapeRegionL()
    {
    TBool hadShapeRegion = iShapeRegionAvailable;
    iShapeRegionAvailable = EFalse;
    
    TInt latest = iCommandBuffers.Count() - 1;    
    DoPeekBufferL(latest);
    
    if (hadShapeRegion && !iShapeRegionAvailable)
        {
        // Previously we had shape region, but not any more.
        // So refresh update regions.
        iShapeRegionClearingPending = ETrue;
        iShapeRegionClippingPending = ETrue;        
        }
    }

void CHuiCanvasWsPainter::DoPeekBufferL(TInt aIndex) 
    {   
    RMemReadStream& readerStream = iWsCommandBufferReader->Stream();
    CHuiCanvasCommandBuffer& commandBuffer = *iCommandBuffers[aIndex];
    HBufC8* cmdBuf = commandBuffer.iCommands;
    TInt bufLength =  cmdBuf->Length();
    
    TPtr8 des = cmdBuf->Des();
    iWsCommandBufferReader->Init( des, bufLength );
    TInt offset = 0; 
    TInt command = 0;
    TBool donePeeking = EFalse;
    do
        {
        command = readerStream.ReadUint8L();
        
        switch ( command )
            {
            case EAlfFrameContainsUnsupportedCommands:
                {
                TUint8 containsUnsupportedCommands = iWsCommandBufferReader->Stream().ReadUint8L();
                if ( containsUnsupportedCommands )
                    {
                    commandBuffer.iContainsUnsupportedCommands = ETrue;
                    }
                break;
                }
            case EAlfFrameFlags:
                {
                TInt flags;
                iWsCommandBufferReader->ReadInt32L( flags );
                if (flags & EAlfTransparentContent)
                    {
                    EnableRenderBuffer(ETrue);
                    }                
                break;
                }
            case EAlfFrameOrientation:
                {
                TInt orientation;
                iWsCommandBufferReader->ReadInt32L(orientation);
                commandBuffer.iOrientation = (CHuiGc::TOrientation)orientation;
                break;
                }
                           
            case EAlfSetUpdateRegion: 
                {
                WsSetUpdateRegionL(aIndex);
				commandBuffer.iPositionForUpdateRegion = Visual()->DisplayRect().iTl.Round();
                break;
                }
            case EAlfCommandIndexArrayHeader:
                {
                TInt tmp;
                iWsCommandBufferReader->ReadInt8L(); // padding
                iWsCommandBufferReader->ReadInt32L( tmp );
                iWsCommandBufferReader->ReadInt32L( tmp );
                break;
                }
            case EAlfPacketPadding:
                {
                DoDigestPaddingL();
				break;
                }
            case EAlfSetShapeRegion: 
                {
                WsSetShapeRegionL( aIndex );
                break;
                }
            default:
                {
                donePeeking = ETrue;
                break;    
                }     
            }
        
   		offset = readerStream.Source()->TellL( MStreamBuf::ERead ).Offset();
       	
        } while( offset < bufLength && !donePeeking);
    
    iCommandBuffers[aIndex]->iOriginalDisplayRect = iCanvasVisual->DisplayRect();
    }


#ifdef HUI_DEBUG_TRACK_DRAWING
void CHuiCanvasWsPainter::DoHandleBufferL(TInt aIndex, TRect& aDisplayRect, TInt aAction, const CHuiCanvasVisual& aUser, CHuiGc* aGc, TPoint& aPos) 
#else
void CHuiCanvasWsPainter::DoHandleBufferL(TInt aIndex, TRect& aDisplayRect, TInt aAction, const CHuiCanvasVisual& aUser, CHuiGc* aGc, TPoint& aPos) 
#endif
    {
    CHuiCanvasCommandBuffer* commandBuffer = iCommandBuffers[aIndex];
    HBufC8* cmdBuf = commandBuffer->iCommands;

#ifdef HUI_DEBUG_TRACK_DRAWING
    if ( commandBuffer->iTrackedBuffer )
        {
        RDebug::Print( _L("CHuiCanvasWsPainter::DoHandleBufferL - Tracked buffer found"));
        switch ( aAction )
            {
            case EDrawBuffer:
                {
                RDebug::Print( _L("CHuiCanvasWsPainter::DoHandleBufferL - draw buffer"));
                break;
                }
            case EScanBuffer:
                {
                RDebug::Print( _L("CHuiCanvasWsPainter::DoHandleBufferL - scan buffer"));
                break;
                }
            default:;
            }
        }

    TTlsData::CommandDebugger()->StartFrame();
    if ( commandBuffer->iProcessName )
        {
        // UNCOMMENT THIS IF YOU WANT TO PRINT EVERYTHING
        // iCommandDebugger->SetPrint( ETrue );
        // UNCOMMENT THIS IF YOU WANT PRINTS ONLY FOR CERTAIN PROCESS!
        // DEFINE PROCESS NAME IN alfcommanddebug.cpp
        // iCommandDebugger->SetPrint( iCommandDebugger->TrackProcess( commandBuffer->iProcessName ) );
        RDebug::Print(_L("CHuiCanvasWsPainter::DoHandleBufferL - Process [%S]"), commandBuffer->iProcessName );
        }
    else
        {
//        RDebug::Print(_L("CHuiCanvasWsPainter::DoHandleBufferL - Process [already dead or not found]"));
        }
#endif
  
    TPtr8 des = cmdBuf->Des();
    DoHandleBufferStringL(aIndex, aDisplayRect, aAction, aUser, aGc, aPos, des, commandBuffer);
    }

void CHuiCanvasWsPainter::DoHandleBufferStringL(TInt aIndex, TRect& /*aDisplayRect*/, TInt /*aAction*/, const CHuiCanvasVisual& /*aUser*/, CHuiGc* /*aGc*/, TPoint& aPos, TPtr8 aCmds, CHuiCanvasCommandBuffer *aCommandBuffer)
    {
    RMemReadStream& readerStream = iWsCommandBufferReader->Stream();    
    TInt bufLength =  aCmds.Length();
    TPtr8 des = aCmds;
    CHuiCanvasCommandBuffer *commandBuffer = aCommandBuffer;
    TPoint deltaPos = aPos - commandBuffer->iOriginalDisplayRect.iTl;
    
    iCanvasWsGc->SetPositionDelta( deltaPos );
    iWsCommandBufferReader->Init( des, bufLength );
    TInt offset = 0; 
    TInt command = 0;
    do
        {
        command = readerStream.ReadUint8L();
#ifdef HUI_DEBUG_TRACK_DRAWING
        TTlsData::CommandDebugger()->SetDescription( command );
#endif

		// Command cases are ordered approximately in the order so that most common ones are at first
        switch ( command )
            {
            case EAlfFrameContainsUnsupportedCommands:
                {
                TUint8 containsUnsupportedCommands = iWsCommandBufferReader->Stream().ReadUint8L();
                if (containsUnsupportedCommands )
                    {
                    // RDebug::Print(_L("Unsupported commands in this frame"));
                    }
                break;
                }
            case EAlfFrameFlags:
                {
                TInt flags;
                iWsCommandBufferReader->ReadInt32L( flags );
                if ( flags != 0 )
                    {
                    RDebug::Print(_L("FLAGS: %d"), flags );
                    }
                break;
                }
            case EAlfFrameOrientation:
                {
                TInt orientation;
                iWsCommandBufferReader->ReadInt32L( orientation );
                break;
                }

            case EAlfSetUpdateRegion: 
                {
                WsSetUpdateRegionL(aIndex);                    
                break;
                }               
            case EAlfSetShapeRegion: 
                {
                iTempRegion.Clear();                
                iWsCommandBufferReader->ReadRegionL( iTempRegion );
                break;
                }               
            case EAlfSetBrushStyle:
                {
                WsSetBrushStyleL();                  
                break;
                }
            case EAlfSetOrigin:
                {
                WsSetOriginL();
                break;
                }
            case EAlfSetPenStyle:
                {
                WsSetPenStyleL();
                break;
                }
            case EAlfSetClippingRegion:
                {        
                WsSetClippingRegionL();
                break;
                }
            case EAlfResetClippingRegion:
                {
                WsResetClippingRegionL();
                break;
                }
            case EAlfSetBrushColor:
                {
                WsSetBrushColorL();
                break;
                }
            case EAlfBitBltMasked:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsBitBltMaskedL();
                break;
                }
            case EAlfCombinedBitBlitMasked:
				{
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
				WsCombinedBitBlitMaskedL();
				break;
				}
            case EAlfReset:
                {
                WsResetL();
                break;
                }
            case EAlfSetPenColor:
                {
                WsSetPenColorL();
                break;
                }
            case EAlfBitBltRect:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsBitBltRectL();
                break;
                }
            case EAlfSetDrawMode:
                {
                WsSetDrawModeL();
                break;
                }
            case EAlfSetCharJustification:
                {
                WsSetCharJustificationL();
                break;
                }
            case EAlfDrawBitmapMasked:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawBitmapMaskedL();
                break;
                }
            case EAlfSetUnderlineStyle:
                {
                WsSetUnderlineStyleL();
                break;
                }
            case EAlfResetFont:
                {
                WsResetFontL();
                break;
                }
            case EAlfSetStrikethroughStyle:
                {
                WsSetStrikethroughStyleL();
                break;
                }
            case EAlfBitBlt:
              	{
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
              	WsBitBltL();
                break;
              	}
            case EAlfBitBltMaskedPoint:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsBitBltMaskedPointL();
                break;
                }
            case EAlfClear:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsClearL();
                break;
                }
            case EAlfClearRect:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                TRgb transpclearcolor(KRgbWhite);
                transpclearcolor.SetAlpha(0);
                if (iBrushColor == transpclearcolor)
                    {
                    commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsTransparentClear);                    
                    }
                WsClearRectL();
                break;
                }
            case EAlfResetBrushPattern:
                {
                WsResetBrushPatternL();
                break;
                }
            case EAlfDrawText1:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawText1L();
                break;
                }
            case EAlfDrawText2:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawText2L();
                break;
                }
            case EAlfDrawText3:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawText3L();
                break;
                }
            case EAlfDrawText4:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawText4L();
                break;
                }                
            case EAlfDrawText5:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawText5L();
                break;
                 }
            case EAlfDrawArc:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawArcL();
                break;
                }
            case EAlfDrawPie:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawPieL();                    
                break;
                }
            case EAlfDrawBitmap1:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawBitmap1L();                    
                break;
                }
            case EAlfDrawBitmap2:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawBitmap2L();
                break;
                }
            case EAlfDrawBitmap3:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawBitmap3L();
                break;
                }
            case EAlfDrawRoundRect:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawRoundRectL();
                break;
                }
            case EAlfDrawPolyLine:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawPolyLineL();
                break;
                }
            case EAlfDrawPolyLineNoEndPoint:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawPolyLineNoEndPointL();
                break;
                }
            case EAlfDrawPolygon:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawPolygonL();                                     
                break;
                }
            case EAlfDrawEllipse:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawEllipseL();
                break;
                }
            case EAlfDrawLine:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawLineL();
                break;
                }
            case EAlfDrawLineTo:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawLineToL();
                break;
                }
            case EAlfDrawLineBy:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawLineByL();
                break;
                }
            case EAlfDrawRect:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawRectL();
                break;
                }
            case EAlfDrawTextVertical1:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawTextVertical1L();
                break;
                }
            case EAlfDrawTextVertical2:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawTextVertical2L();
                break;
                }                
            case EAlfDrawTextVertical3:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawTextVertical3L();                          
                break;
                } 
            case EAlfDrawTextVertical4: 
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawTextVertical4L();
                break;
                }
            case EAlfDrawTextVertical5:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsDrawTextVertical5L();
                break;
                }
            case EAlfMoveTo:
                {
                WsMoveToL();
                break;
                }
            case EAlfMoveBy:
                {
                WsMoveByL();
                break;
                }
            case EAlfPlot:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsPlotL();
                break;
                }
            case EAlfSetBrushOrigin:
                {
                WsSetBrushOriginL();
                break;
                }
            case EAlfSetPenSize:
                WsSetPenSizeL();                
                break;
            case EAlfSetTextShadowColor:
                {
                WsSetTextShadowColorL();
                break;
                }
            case EAlfSetWordJustification:
                {
                WsSetWordJustificationL();
                break;
                }                
            case EAlfSetBrushPattern:
                {
                WsSetBrushPatternL();
                break;
                }
            case EAlfSetBrushPattern2:
                {
                WsSetBrushPatternL();
                break;
                }
            case EAlfSetFont:
                {
                WsSetFontL();
                break;
                }
            case EAlfContentTag:
                {
                WsContentTagL();
                break;
                }
            case EAlfCopyRect:
                WsCopyRectL();
                break;
            case EAlfUpdateJustification:
                {
                WsUpdateJustificationL();                  
                break;
                }
            case EAlfUpdateJustificationVertical:
                {
                WsUpdateJustificationVerticalL();
                break;
                }
            case EAlfSetFontNoDuplicate:
                {
                WsSetFontNoDuplicateL();
                break;
                }
            case EAlfHasBrushPattern:
            case EAlfHasFont:
            case EAlfBrushColor:
            case EAlfPenColor:
            case EAlfTextShadowColor:
                {
                // these values are cached on client side, we dont do anything.
                break;
                }
            case EAlfCopySettings:
                {
                WsCopySettingsL();                    
                break;    
                }
            case EAlfSetClippingRect:
                {
                WsSetClippingRectL();
                break;
                }
            case EAlfCancelClippingRect:
                {
                WsCancelClippingRectL();
                break;
                }
            case EAlfSetFaded:
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsSetFadedL();
                }
                break;
            case EAlfSetFadingParameters:
                {
                WsSetFadingParametersL();
                break;
                }
            case EAlfFadeArea: 
                {
                commandBuffer->SetStatusFlags(EHuiCanvasBufferContainsDrawing);
                WsFadeAreaL();
                break;
                }
            case EAlfMapColors:
                // MapColors is not supported. Client should never supply this command.
                User::Leave( KErrNotSupported );
                break;
            case EAlfSetUserDisplayMode:
                {
                WsSetUserDisplayModeL();
                break;
                }
            case EAlfUseFont:
                {
                WsUseFontL();                    
                break;
                }
            case EAlfSendSyncDataBlock:
                {
                break;
                }
            case EAlfWindowInactivateWindow:
                {
                // Nothing to do
                break;
                }
            case EAlfPacketPadding:
                {
                DoDigestPaddingL();
				break;
                }
            case EAlfCommandIndexArrayHeader:
                {
                TInt tmp;
                iWsCommandBufferReader->ReadInt8L(); // padding
                iWsCommandBufferReader->ReadInt32L( tmp );
                iWsCommandBufferReader->ReadInt32L( tmp );
                break;
                }
            case EAlfCommandIndexArray:
                {
                TUint8 a = iWsCommandBufferReader->ReadInt8L();
                iWsCommandBufferReader->ReadInt8L();
                iWsCommandBufferReader->ReadInt8L();
                TInt tmp;
                TRect tmp2;
                for(TUint8 i=0;i<a;i++)
                    {
                    iWsCommandBufferReader->ReadInt32L(tmp);
                    iWsCommandBufferReader->ReadInt32L(tmp);
                    iWsCommandBufferReader->ReadInt32L(tmp);
                    iWsCommandBufferReader->ReadRectL(tmp2);
                    }
                break;
                }
              
                
            default:
                {
                // We should never ever get here
                    HUI_DEBUG1(_L("CHuiCanvasVisual::HandleWsBufferL. Error: Unknown command=%d"), command);
                break;
                }
            }// end switch
    		offset = readerStream.Source()->TellL( MStreamBuf::ERead ).Offset();
       	
        } while( offset < bufLength );
#ifdef HUI_DEBUG_TRACK_DRAWING
    TTlsData::CommandDebugger()->EndFrame();
#endif    
    }
    
    
void CHuiCanvasWsPainter::SetCommandSetL( const TDesC8& aCommands )
    {
	CHuiCanvasPainter::SetCommandSetL(aCommands);  

    DoPeekBufferAndUpdateShapeRegionL();
    SelectGcL();
       
    // If shape region has changed recalculate all update regions and remove redundant buffers
    if (iShapeRegionClippingPending)
        {
        ApplyShapeRegion();       
        RemoveRedundantBuffers();   
        }    
    
	// Release currently cached images. 
    // They may still be taken into use after next HandleBuffferL call.
    if (iCanvasVisual)
        {
        iCanvasVisual->Env().CanvasTextureCache().ReleaseAllCachedEntries(*iCanvasVisual);         
        }    
    }
    

void CHuiCanvasWsPainter::ClearCommandSet()
    {
#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        RDebug::Print(_L(">> CHuiCanvasWsPainter::ClearCommandSet this = %i"), this );
        TTime startTime;
        startTime.UniversalTime();
#endif

    CHuiCanvasPainter::ClearCommandSet();
    
    iShapeRegionAvailable = EFalse;
    
    // Release currently cached images. 
    // They may still be taken into use after next HandleBufferL call.
    if (iCanvasVisual)
        {
        iCanvasVisual->Env().CanvasTextureCache().ReleaseAllCachedEntries(*iCanvasVisual); 
        }

#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        TTime endTime;
        endTime.UniversalTime();
        TInt timeInMs =  endTime.MicroSecondsFrom( startTime ).Int64()/1000;           
        RDebug::Print(_L(">> CHuiCanvasWsPainter::ClearCommandSet took %i ms"), timeInMs);
#endif
    
    }

struct TArrayHeader
    {
    TInt32 iOffset;
    TInt32 iSize;
    };

void CHuiCanvasWsPainter::AddCommandSetL( const TDesC8& aMoreCommands )
    {    
#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        RDebug::Print(_L(">> CHuiCanvasWsPainter::AddCommandSetL this = %i"), this );
        TTime startTime;
        startTime.UniversalTime();
#endif

    CHuiCanvasPainter::AddCommandSetL(aMoreCommands);
    DoPeekBufferAndUpdateShapeRegionL();
    SelectGcL();
    
    ApplyShapeRegion();
    
    RemoveRedundantBuffers();   
    
    // New command set may contain BitBlt commands
    // and those cached images must be refreshed
    if (iCanvasVisual)
        {
        // Release currently cached images. 
        // They may still be taken into use after next HandleBufferL call.
        iCanvasVisual->Env().CanvasTextureCache().ReleaseAllCachedEntries(*iCanvasVisual); 
        }

#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        TTime endTime;
        endTime.UniversalTime();
        TInt timeInMs =  endTime.MicroSecondsFrom( startTime ).Int64()/1000;           
        RDebug::Print(_L(">> CHuiCanvasWsPainter::AddCommandSetL took %i ms"), timeInMs);
#endif

#ifdef HUI_ENABLE_SUBWINDOW_FX        
    TInt count = NumBuffers();
    if (!count) return;
    const TDesC8 &more = At(count-1)->iCommands->Des();
    //RDebug::Print(_L("MoreCommands: %x : %d"), aMoreCommands.Ptr(), aMoreCommands.Length());
    TPtrC8 header = more.Mid(4, sizeof(TArrayHeader)); // the magic comes from CAlfRsSendBuffer::WriteFollowingFrameOffsetL.
    TArrayHeader *ptr = (TArrayHeader*)header.Ptr();
    //RDebug::Print(_L("Ptr: %d : %d"), ptr->iOffset, ptr->iSize);
    TPtrC8 indexTable = more.Mid(ptr->iOffset, ptr->iSize);

    if (iCanvasVisual->Brushes() && iCanvasVisual)
        {
        ParseCmdBufferL(*iCanvasVisual->Brushes(), more, indexTable, CanvasGc(), *CanvasWsGc(), *iCanvasVisual, *this);
        }
#endif
    
#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        TTime endTime2;
        endTime2.UniversalTime();
        TInt time2InMs =  endTime2.MicroSecondsFrom( startTime ).Int64()/1000;           
        RDebug::Print(_L(">> CHuiCanvasWsPainter::AddCommandSetL including brush creation took %i ms"), time2InMs);
#endif

    }

void CHuiCanvasWsPainter::AddPartialCommandSetL( const TDesC8& aMoreCommands, TBool aLastPart )
    {
    CHuiCanvasPainter::AddPartialCommandSetL(aMoreCommands,aLastPart); 
    }

#ifdef HUI_DEBUG_TRACK_DRAWING
void CHuiCanvasWsPainter::SetTrackCommandSet( TFileName& aFileName, TBool aTrack )
    {
    CHuiCanvasPainter::SetTrackCommandSet( aFileName, aTrack );
    }
#endif

void CHuiCanvasWsPainter::ClearCache()
    {
    if (iCanvasVisual && !iCacheCleared)
        {
#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        RDebug::Print(_L(">> CHuiCanvasWsPainter::ClearCache this = %i"), this );
        TTime startTime;
        startTime.UniversalTime();
#endif
        TRAP_IGNORE(SelectGcL());
        RenewAllBuffers();         
        SetAllBuffersChanged(ETrue);
        iCanvasWsGc->ClearCache();
        iCanvasVisual->Env().CanvasTextureCache().ReleaseAllCachedEntries(*iCanvasVisual); 
        iCacheCleared = ETrue;
        
#ifdef HUI_DEBUG_PRINT_PERFORMANCE_INTERVAL
        TTime endTime;
        endTime.UniversalTime();
        TInt timeInMs =  endTime.MicroSecondsFrom( startTime ).Int64()/1000;           
        RDebug::Print(_L(">> CHuiCanvasWsPainter::ClearCache took %i ms"), timeInMs);
#endif

        }
    }


void CHuiCanvasWsPainter::WsBitBltL()
    {
    TPoint point(0,0);
    TInt bitmapHandle = 0;
    iWsCommandBufferReader->ReadPointL( point );
    iWsCommandBufferReader->ReadBitmapHandleL( bitmapHandle );
   	iCanvasWsGc->WsBitBltL(bitmapHandle,point);         
    }
    
void CHuiCanvasWsPainter::WsBitBltRectL()
    {
    iWsCommandBufferReader->ReadL( (TUint8*)&iStructBitBlitRect, sizeof( TStructBitBlitRect ) );
    iCanvasWsGc->WsBitBltRectL( 
            iStructBitBlitRect.iBitmapHandle,
            iStructBitBlitRect.iPoint,
            iStructBitBlitRect.iRect);
    }
    
void CHuiCanvasWsPainter::WsBitBltMaskedL()
    {
    iWsCommandBufferReader->ReadL( (TUint8*)&iStructBitBlitMasked, sizeof( TStructBitBlitMasked ) );
	iCanvasWsGc->WsBitBltMaskedL( 
	        iStructBitBlitMasked.iBitmapHandle,
	        iStructBitBlitMasked.iMaskHandle,
	        iStructBitBlitMasked.iInvertMask,
	        iStructBitBlitMasked.iPoint,
	        iStructBitBlitMasked.iRect);         
    }


void  CHuiCanvasWsPainter::WsCombinedBitBlitMaskedL()
	{
	TRect clipRect(0,0,0,0);
	TRect blitRect(0,0,0,0);
	TInt itemCount = iWsCommandBufferReader->ReadInt8L();
	iWsCommandBufferReader->ReadL( (TUint8*)&clipRect, sizeof( TRect ) );
	iWsCommandBufferReader->ReadL( (TUint8*)&blitRect, sizeof( TRect ) );
	    
	TInt index(0);
	
	RArray<THuiCachedCombinedImageParams> combinedBlitItems;
	THuiCachedCombinedImageParams combinedBlitItem;
	while( index++ < itemCount )
		{
		iWsCommandBufferReader->ReadL( (TUint8*)&combinedBlitItem, sizeof( THuiCachedCombinedImageParams ) );
		combinedBlitItems.Append(combinedBlitItem);
		}
	// clipping rects were optimized away by renderstage. it must be set now.
	// We use SetClippingRegion instead of SetClippingRect as they handle ws origin differently.
	iTempRegion.Clear();
    iTempRegion.AddRect(clipRect);
    iCanvasWsGc->WsSetClippingRegionL(iTempRegion);  
	iCanvasWsGc->WsCombinedBitBltMaskedL(blitRect, combinedBlitItems);
    iCanvasWsGc->WsResetClippingRegionL();  
	combinedBlitItems.Close();
	}

void CHuiCanvasWsPainter::WsBitBltMaskedPointL()
    { 
    TPoint point1(0,0);
    TPoint point2(0,0);
    TRect rect(0,0,0,0);
    TInt bitmapHandle = 0;
    TInt maskHandle = 0;
    iWsCommandBufferReader->ReadPointL( point1 );
    iWsCommandBufferReader->ReadBitmapHandleL( bitmapHandle );
    iWsCommandBufferReader->ReadRectL( rect );
    iWsCommandBufferReader->ReadBitmapHandleL( maskHandle );
    iWsCommandBufferReader->ReadPointL( point2 );
    iCanvasWsGc->WsBitBltMaskedPointL(bitmapHandle,maskHandle,point1,point2,rect);
    }
    
void CHuiCanvasWsPainter::WsResetClippingRegionL()
    {    
    iCanvasWsGc->WsResetClippingRegionL();
    }
    
void CHuiCanvasWsPainter::WsClearL()
    {
   	iCanvasWsGc->WsClearL();
    }

void CHuiCanvasWsPainter::WsClearRectL()
    {
    TRect tmpRect1(0,0,0,0);
    iWsCommandBufferReader->ReadRectL( tmpRect1 );
	iCanvasWsGc->WsClearRectL(tmpRect1);
    }

void CHuiCanvasWsPainter::WsResetBrushPatternL()
    {
            
    }

void CHuiCanvasWsPainter::WsResetFontL()
    {    
    iCanvasWsGc->WsResetFontL();
    }

void CHuiCanvasWsPainter::WsDrawArcL()
    {
    TPoint point1(0,0);
    TPoint point2(0,0);
    TRect rect(0,0,0,0);
    iWsCommandBufferReader->ReadRectL( rect );
    iWsCommandBufferReader->ReadPointL( point1 );
    iWsCommandBufferReader->ReadPointL( point2 );
   	iCanvasWsGc->WsDrawArcL(point1,point2,rect);   
    }

void CHuiCanvasWsPainter::WsDrawPieL()
    {
    TPoint point1(0,0);
    TPoint point2(0,0);
    TRect rect(0,0,0,0);
    iWsCommandBufferReader->ReadRectL( rect );
    iWsCommandBufferReader->ReadPointL( point1 );
    iWsCommandBufferReader->ReadPointL( point2 );
    iCanvasWsGc->WsDrawPieL(point1,point2,rect);
    }

void CHuiCanvasWsPainter::WsDrawBitmap1L()
    {
    TRect rect(0,0,0,0);
    TInt bitmapHandle = 0;
    iWsCommandBufferReader->ReadRectL( rect );
    iWsCommandBufferReader->ReadBitmapHandleL( bitmapHandle );
    iCanvasWsGc->WsDrawBitmap1L(bitmapHandle,rect);
    }

void CHuiCanvasWsPainter::WsDrawBitmap2L()
    {
    TRect rect1(0,0,0,0);
    TRect rect2(0,0,0,0);
    TInt bitmapHandle = 0;
    iWsCommandBufferReader->ReadRectL( rect1);
    iWsCommandBufferReader->ReadBitmapHandleL( bitmapHandle );
    iWsCommandBufferReader->ReadRectL( rect2);
    iCanvasWsGc->WsDrawBitmap2L(bitmapHandle,rect1,rect2);
    }

void CHuiCanvasWsPainter::WsDrawBitmap3L()
    {
    TPoint point(0,0);
    TInt bitmapHandle = 0;
    iWsCommandBufferReader->ReadPointL( point );
    iWsCommandBufferReader->ReadBitmapHandleL( bitmapHandle );   
	iCanvasWsGc->WsDrawBitmap3L(bitmapHandle,point);
    }

void CHuiCanvasWsPainter::WsDrawBitmapMaskedL()
    {
    iWsCommandBufferReader->ReadL( (TUint8*)&iStructDrawBitmapMasked, sizeof( TStructDrawBitmapMasked ) );
    iCanvasWsGc->WsDrawBitmapMaskedL(
            iStructDrawBitmapMasked.iBitmapHandle,
            iStructDrawBitmapMasked.iMaskHandle,
            iStructDrawBitmapMasked.iInvertMask,
            iStructDrawBitmapMasked.iRect1,
            iStructDrawBitmapMasked.iRect2);
    }

void CHuiCanvasWsPainter::WsDrawRoundRectL()
    {
    TPoint point(0,0);
    TRect rect(0,0,0,0);        
    iWsCommandBufferReader->ReadRectL( rect );
    iWsCommandBufferReader->ReadPointL( point );    
    iCanvasWsGc->WsDrawRoundRectL(point,rect);    
    }

void CHuiCanvasWsPainter::WsDrawPolyLineL()
    {
    CArrayFix<TPoint>* pointerArray = NULL;    
    iWsCommandBufferReader->ReadPointArrayL( pointerArray );    
    iCanvasWsGc->WsDrawPolyLineL(pointerArray);    
    delete pointerArray;
    }

void CHuiCanvasWsPainter::WsDrawPolyLineNoEndPointL()
    {
    TPoint* pointerArray = NULL;
    TInt count = 0;
    iWsCommandBufferReader->ReadPointArrayL( pointerArray, count );
    iCanvasWsGc->WsDrawPolyLineNoEndPointL(pointerArray,count);
    pointerArray = NULL;
    }

void CHuiCanvasWsPainter::WsDrawPolygonL()
    {
    CArrayFix<TPoint>* pointerArray;
    TInt fillRule = 0;
    iWsCommandBufferReader->ReadPointArrayL( pointerArray );
    iWsCommandBufferReader->ReadInt8L(); // command
    iWsCommandBufferReader->ReadInt32L( fillRule ); 
    iCanvasWsGc->WsDrawPolygonL(pointerArray,fillRule);    
    delete pointerArray;
    }

void CHuiCanvasWsPainter::WsDrawEllipseL()
    {
    TRect rect(0,0,0,0);                        
    iWsCommandBufferReader->ReadRectL( rect );    
    iCanvasWsGc->WsDrawEllipseL(rect);    
    }

void CHuiCanvasWsPainter::WsDrawLineL()
    {
    TPoint start(0,0);
    TPoint end(0,0);
    iWsCommandBufferReader->ReadPointL( start );
    iWsCommandBufferReader->ReadPointL( end );   
    iCanvasWsGc->WsDrawLineL(start,end);    
    }

void CHuiCanvasWsPainter::WsDrawLineToL()
    {
    TPoint point(0,0);
    iWsCommandBufferReader->ReadPointL( point );    
    iCanvasWsGc->WsDrawLineToL(point);    
    }

void CHuiCanvasWsPainter::WsDrawLineByL()
    {
    TPoint point(0,0);
    iWsCommandBufferReader->ReadPointL( point );    
    iCanvasWsGc->WsDrawLineByL(point);    
    }

void CHuiCanvasWsPainter::WsDrawRectL()
    {
    TRect rect(0,0,0,0);                        
    iWsCommandBufferReader->ReadRectL( rect );

	// There is a bug that CScreen::DisplayMode() returns non-alpha display mode (TSW error: ??)
	// That is why WServ clears the window with the color that is opaque (0xFF000400) instead of transparent (0x00000000) 
	#ifdef __ALF_SYMBIAN_RWINDOW_CLEARING_BUG_WORKAROUND__
//	RDebug::Print(_L("HV: CHuiCanvasWsPainter::WsDrawRectL(). iTestBrushColor.Internal()=%x, iTestFullDisplayRect=(%d %d %d %d), rect=(%d %d %d %d) "),
//		iTestBrushColor.Internal(), iTestFullDisplayRect.iTl.iX, iTestFullDisplayRect.iTl.iY,iTestFullDisplayRect.iBr.iX, iTestFullDisplayRect.iBr.iY, 
//		rect.iTl.iX, rect.iTl.iY,rect.iBr.iX, rect.iBr.iY);
	if ( (rect ==  iTestFullDisplayRect) &&  	// Is full screen rect?
	     (iTestBrushColor.Internal() == 0xff000400))  	// Last set color is opaque dark green?
		{
	//	RDebug::Print(_L("HV: CHuiCanvasWsPainter::WsDrawRectL(). Change brush color to 0x00000000. "));
		iCanvasWsGc->WsSetBrushColorL(TRgb(0,0,0,0)); // Replace brush color with transparent black          	  	                            	
		}
	#endif
    
    iCanvasWsGc->WsDrawRectL(rect);    
    }

void CHuiCanvasWsPainter::WsDrawText1L()
    {
    TPtr textValue(0,0);
    TBool success = iWsCommandBufferReader->ReadTextL( textValue, iTextParametersGeneral );
    
    if (textValue.Length() == 0 || !success )
        {
        return;   
        }
    
    iCanvasWsGc->WsDrawText1L(textValue,iTextParametersGeneral);    
    }
    
void CHuiCanvasWsPainter::WsDrawText2L()
    {
    TPoint point(0,0);
    TPtr textValue(0,0);
    TBool success = iWsCommandBufferReader->ReadTextL( textValue, iTextParametersGeneral );
    iWsCommandBufferReader->ReadPointL( point );
    
    if (textValue.Length() == 0 ||!success)
        {
        return;   
        }
	
    iCanvasWsGc->WsDrawText2L(textValue,point,iTextParametersGeneral);    
    }

void CHuiCanvasWsPainter::WsDrawText3L()
    {
    TRect rect(0,0,0,0);
    TPtr textValue(0,0);
    TBool success = iWsCommandBufferReader->ReadTextL( textValue, iTextParametersGeneral );
    iWsCommandBufferReader->ReadRectL( rect );

    if (textValue.Length() == 0 || !success )
        {
        return;   
        }
	
    iCanvasWsGc->WsDrawText3L(textValue,rect,iTextParametersGeneral);    
    }

void CHuiCanvasWsPainter::WsDrawText4L()
    {
    TPtr textValue(0,0);
    TBool success = iWsCommandBufferReader->ReadTextL( textValue, iTextParametersGeneral );
    iWsCommandBufferReader->ReadL( (TUint8*)&iStructDrawTextGeneral, sizeof( TStructDrawTextGeneral ) - sizeof(TInt) );
    
    if (textValue.Length() == 0 || !success )
            {
            return;   
            }

    iCanvasWsGc->WsDrawText4L(textValue,
            iStructDrawTextGeneral.iRect,
            iStructDrawTextGeneral.iValue1,  // textBaselineOffset
            iStructDrawTextGeneral.iValue2, // textAlign
            iStructDrawTextGeneral.iValue3, // textMargin,
            iTextParametersGeneral);    
    }

void CHuiCanvasWsPainter::WsDrawText5L()
    {
    TPtr textValue(0,0);
    TBool success = iWsCommandBufferReader->ReadTextL( textValue, iTextParametersGeneral );
    iWsCommandBufferReader->ReadL( (TUint8*)&iStructDrawTextGeneral, sizeof( TStructDrawTextGeneral ) );
    
    if (textValue.Length() == 0 || !success )
        {
        return;   
        }

	iCanvasWsGc->WsDrawText5L(textValue,
	        iStructDrawTextGeneral.iRect,
	        iStructDrawTextGeneral.iValue1,  // textBaselineOffset
	        iStructDrawTextGeneral.iValue3, // textAlign
	        iStructDrawTextGeneral.iValue4, // textMargin,
	        iStructDrawTextGeneral.iValue2, // textWidth
	        iTextParametersGeneral);    
    }

void CHuiCanvasWsPainter::WsDrawTextVertical1L()
    {
    TInt textUp = 0;
    TPtr textValue(0,0);
    TBool success = iWsCommandBufferReader->ReadTextL( textValue, iTextParametersGeneral );
    iWsCommandBufferReader->ReadInt32L( textUp );  
    
    if (textValue.Length() == 0 || !success )
        {
        return;   
        }

	iCanvasWsGc->WsDrawTextVertical1L(textValue,textUp,iTextParametersGeneral);    
    }

void CHuiCanvasWsPainter::WsDrawTextVertical2L()
    {
    TPoint point(0,0);
    TInt textUp = 0;
    TPtr textValue(0,0);
    TBool success = iWsCommandBufferReader->ReadTextL( textValue, iTextParametersGeneral );
    iWsCommandBufferReader->ReadPointL( point );
    iWsCommandBufferReader->ReadInt32L( textUp );  

    if (textValue.Length() == 0 || !success )
            {
            return;   
            }
        
	iCanvasWsGc->WsDrawTextVertical2L(textValue,textUp,point,iTextParametersGeneral);    
    }


void CHuiCanvasWsPainter::WsDrawTextVertical3L()
    {
    TRect rect(0,0,0,0);
    TInt textUp = 0;
    TPtr textValue(0,0);
    TBool success = iWsCommandBufferReader->ReadTextL( textValue, iTextParametersGeneral);
    iWsCommandBufferReader->ReadRectL( rect );
    iWsCommandBufferReader->ReadInt32L( textUp ); 

    if (textValue.Length() == 0 || !success )
            {
            return;   
            }
    
    iCanvasWsGc->WsDrawTextVertical3L(textValue,textUp,rect,iTextParametersGeneral);    
    }

void CHuiCanvasWsPainter::WsDrawTextVertical4L()
    {
    TRect rect(0,0,0,0);
    TInt textBaselineOffset = 0;
    TInt textUp = 0;
    TInt textAlign = 0;
    TInt textMargin = 0;
    TPtr textValue(0,0);
    
    TBool success = iWsCommandBufferReader->ReadTextL( textValue, iTextParametersGeneral );
    iWsCommandBufferReader->ReadRectL( rect );
    iWsCommandBufferReader->ReadInt32L( textBaselineOffset );
    iWsCommandBufferReader->ReadInt32L( textUp );
    iWsCommandBufferReader->ReadInt32L( textAlign );
    iWsCommandBufferReader->ReadInt32L( textMargin );

    if (textValue.Length() == 0 || !success )
            {
            return;   
            }

    iCanvasWsGc->WsDrawTextVertical4L(textValue,rect,textBaselineOffset,textUp,textAlign,textMargin,iTextParametersGeneral);    
    }

void CHuiCanvasWsPainter::WsDrawTextVertical5L()
    {
    TRect rect(0,0,0,0);
    TInt textBaselineOffset = 0;
    TInt textWidth = 0;
    TInt textUp = 0;
    TInt textAlign = 0;
    TInt textMargin = 0;
    TPtr textValue(0,0);
    
    iWsCommandBufferReader->ReadTextL( textValue, iTextParametersGeneral );
    iWsCommandBufferReader->ReadRectL( rect );
    iWsCommandBufferReader->ReadInt32L( textBaselineOffset );
    iWsCommandBufferReader->ReadInt32L( textWidth );
    iWsCommandBufferReader->ReadInt32L( textUp );
    iWsCommandBufferReader->ReadInt32L( textAlign );
    iWsCommandBufferReader->ReadInt32L( textMargin );

    if (textValue.Length() == 0)
            {
            return;   
            }

    iCanvasWsGc->WsDrawTextVertical5L(textValue,rect,textBaselineOffset,textUp,textAlign,textMargin,textWidth,iTextParametersGeneral);    
    }

void CHuiCanvasWsPainter::WsMoveToL()
    {
    TPoint point(0,0);        
    iWsCommandBufferReader->ReadPointL( point );
    iCanvasWsGc->WsMoveToL(point);
    }

void CHuiCanvasWsPainter::WsMoveByL()
    {
    TPoint point(0,0);        
    iWsCommandBufferReader->ReadPointL( point );    
    iCanvasWsGc->WsMoveByL(point);
    }

void CHuiCanvasWsPainter::WsPlotL()
    {
    TPoint point(0,0);        
    iWsCommandBufferReader->ReadPointL( point );    
    iCanvasWsGc->WsPlotL(point);               	  	                        
    }

void CHuiCanvasWsPainter::WsResetL()
    {
    iCanvasWsGc->WsResetL();               	  	                            

	#ifdef __ALF_SYMBIAN_RWINDOW_CLEARING_BUG_WORKAROUND__
	iTestBrushColor.SetInternal(0);
	#endif	
	iBrushColor.SetInternal(0);
    }

void CHuiCanvasWsPainter::WsSetBrushColorL()
    {
    TRgb color = 0;        
    iWsCommandBufferReader->ReadColorL( color );
	iCanvasWsGc->WsSetBrushColorL(color);           	  	                            

	#ifdef __ALF_SYMBIAN_RWINDOW_CLEARING_BUG_WORKAROUND__
	iTestBrushColor = color;    	  	                            	
	#endif
	iBrushColor = color;
    }

void CHuiCanvasWsPainter::WsSetBrushOriginL()
    {
    TPoint point(0,0);        
    iWsCommandBufferReader->ReadPointL( point );
	iCanvasWsGc->WsSetBrushOriginL(point);	
    }

void CHuiCanvasWsPainter::WsSetBrushStyleL()
    {
    TInt style = 0;
    iWsCommandBufferReader->ReadInt32L( style );
	iCanvasWsGc->WsSetBrushStyleL(style);        
    }

void CHuiCanvasWsPainter::WsSetClippingRegionL()
    {
    iTempRegion.Clear();
    iWsCommandBufferReader->ReadRegionL( iTempRegion );
    iCanvasWsGc->WsSetClippingRegionL(iTempRegion);
    }

void CHuiCanvasWsPainter::WsSetDrawModeL()
    {
    TInt drawMode = 0;    
    iWsCommandBufferReader->ReadInt32L( drawMode );
	iCanvasWsGc->WsSetDrawModeL(drawMode);
    }

void CHuiCanvasWsPainter::WsSetOriginL()
    {
    TPoint origin(0,0);        
    iWsCommandBufferReader->ReadPointL(origin);
   	iCanvasWsGc->WsSetOriginL(origin);
    }

void CHuiCanvasWsPainter::WsSetPenColorL()
    {
    TRgb color = 0;        
    iWsCommandBufferReader->ReadColorL( color );
	iCanvasWsGc->WsSetPenColorL(color);    
    }

void CHuiCanvasWsPainter::WsSetPenStyleL()
    {
    TInt style = 0;    
    iWsCommandBufferReader->ReadInt32L( style );
	iCanvasWsGc->WsSetPenStyleL(style);
    }

void CHuiCanvasWsPainter::WsSetPenSizeL()
    {
    TInt width = 0;    
    TInt height = 0;    
    iWsCommandBufferReader->ReadInt32L( width ); 
    iWsCommandBufferReader->ReadInt32L( height ); 
    iCanvasWsGc->WsSetPenSizeL(width,height);
    //\todo: Separate pen width and height are not yet supported 	
    }

void CHuiCanvasWsPainter::WsSetTextShadowColorL()
    {
    TRgb color = 0;        
    iWsCommandBufferReader->ReadColorL( color );
    iCanvasWsGc->WsSetTextShadowColorL(color);
    }

void CHuiCanvasWsPainter::WsSetCharJustificationL()
    {
    TInt excessWidth = 0;
    TInt numGaps = 0;
    iWsCommandBufferReader->ReadInt32L( excessWidth );
    iWsCommandBufferReader->ReadInt32L( numGaps ); // NumChars
	iCanvasWsGc->WsSetCharJustificationL(excessWidth,numGaps);
    }

void CHuiCanvasWsPainter::WsSetWordJustificationL()
    {
    TInt excessWidth = 0;
    TInt numGaps = 0;
    iWsCommandBufferReader->ReadInt32L( excessWidth );
    iWsCommandBufferReader->ReadInt32L( numGaps ); // NumChars
	iCanvasWsGc->WsSetWordJustificationL(excessWidth,numGaps);        
    }

void CHuiCanvasWsPainter::WsSetUnderlineStyleL()
    {
    TInt value = 0;
    iWsCommandBufferReader->ReadInt32L( value );
    iCanvasWsGc->WsSetUnderlineStyleL(value);
    }

void CHuiCanvasWsPainter::WsSetStrikethroughStyleL()
    {
    TInt value = 0;
    iWsCommandBufferReader->ReadInt32L( value );
    // todo: is this magic 1000 safe offset?
    iCanvasWsGc->WsSetUnderlineStyleL(value+1000);
    }


void CHuiCanvasWsPainter::WsSetBrushPatternL()
    {
    TInt value = 0;        
    iWsCommandBufferReader->ReadInt32L( value );
	iCanvasWsGc->WsSetBrushPatternL(value);
    }

void CHuiCanvasWsPainter::WsSetFontL()
    {
    TInt fontId = 0;
    iWsCommandBufferReader->ReadInt32L( fontId );// Font type Uid
    iCanvasWsGc->WsSetFontL(fontId);    
    }

void CHuiCanvasWsPainter::WsContentTagL()
    {
    TInt tag;
    iWsCommandBufferReader->ReadInt32L( tag );
    }

void CHuiCanvasWsPainter::WsCopyRectL()
    {
    TPoint point(0,0);
    TRect rect(0,0,0,0);
    iWsCommandBufferReader->ReadPointL( point );
    iWsCommandBufferReader->ReadRectL( rect );
	iCanvasWsGc->WsCopyRectL(point,rect);
    }

void CHuiCanvasWsPainter::WsUpdateJustificationL()
    {
    TPtr textValue(0,0);
    iWsCommandBufferReader->ReadTextL( textValue, iTextParametersGeneral );	
	iCanvasWsGc->WsUpdateJustificationL(textValue);
    }

void CHuiCanvasWsPainter::WsUpdateJustificationVerticalL()
    {
    TPtr textValue(0,0);
    TInt value = 0;

    iWsCommandBufferReader->ReadTextL( textValue, iTextParametersGeneral );
    iWsCommandBufferReader->ReadInt32L( value );
	iCanvasWsGc->WsUpdateJustificationVerticalL(value,textValue);
   }

void CHuiCanvasWsPainter::WsSetFontNoDuplicateL()
    {
    TInt tempFont = 0;
    iWsCommandBufferReader->ReadInt32L( tempFont );
	iCanvasWsGc->WsSetFontNoDuplicateL(tempFont);	    
    }


void CHuiCanvasWsPainter::WsCopySettingsL()
    {
    iCanvasWsGc->WsCopySettingsL();	
    }

void CHuiCanvasWsPainter::WsSetClippingRectL()
    {
    TRect rect(0,0,0,0);
    iWsCommandBufferReader->ReadRectL( rect );
	iCanvasWsGc->WsSetClippingRectL(rect);	                        
    }

void CHuiCanvasWsPainter::WsCancelClippingRectL()
    {
    iCanvasWsGc->WsCancelClippingRectL();
    }

void CHuiCanvasWsPainter::WsSetFadedL()
    {
    TInt faded = 0;
    iWsCommandBufferReader->ReadInt32L( faded );
	iCanvasWsGc->WsSetFadedL(faded);	                        
    }

void CHuiCanvasWsPainter::WsSetFadingParametersL()
    {
    TInt blackMap = 0;        
    TInt whiteMap = 0;        
    iWsCommandBufferReader->ReadInt32L( blackMap );
    iWsCommandBufferReader->ReadInt32L( whiteMap );
	iCanvasWsGc->WsSetFadingParametersL(blackMap,whiteMap);	                        
    }

void CHuiCanvasWsPainter::WsFadeAreaL()
    {
    iTempRegion.Clear();
    iWsCommandBufferReader->ReadRegionL( iTempRegion );    
	iCanvasWsGc->WsFadeAreaL(iTempRegion);	                        
    }

void CHuiCanvasWsPainter::WsMapColorsL()
    {
    iCanvasWsGc->WsMapColorsL();	                        
    }

void CHuiCanvasWsPainter::WsSetUserDisplayModeL()
    {
    TInt mode = 0;
    iWsCommandBufferReader->ReadInt32L( mode );
	iCanvasWsGc->WsSetUserDisplayModeL(mode);	                        
        
    }

void CHuiCanvasWsPainter::WsUseFontL()
    {
	TInt tempFont = 0;
    iWsCommandBufferReader->ReadInt32L( tempFont );
    iCanvasWsGc->WsUseFontL(tempFont);	                                
    }       

void CHuiCanvasWsPainter::WsSetUpdateRegionL(TInt aIndex)
    {
    iTempRegion.Clear();                
    iWsCommandBufferReader->ReadRegionL( iTempRegion );
    if (aIndex==-1) return; //TP
    // Update region is modified when new buffers are added after this buffer, 
    // so store the region only at first read. 
    if (iCommandBuffers[aIndex]->iChanged && iCommandBuffers[aIndex]->iOriginalUpdateRegion.Count() == 0)
        {
        // Check first that the coordiates are not insane (sometimes thay are)
        TInt count = iTempRegion.Count();                    
        TRect displayRect = iCanvasVisual->DisplayRect();
        for (TInt i=count-1; i>=0; i--)
            {
            TRect rect = iTempRegion[i];

            // - Sometimes updateregion is outiside window (WSERV...why?!) 
            //   We clip it here to avoid ui probs.
            // - Lets remove this for now, this seems to break scrollbars again
            //   when window parent-child relations work..
            // - Hmm, should we add this anyway because now the idle softkeys are lost again in landscape mode ?
            // - Added again after WSERV fix in the window parent-child relations
            // - Rect may be outside the screen and that's fine (Visual may be moved later).
#ifndef HUI_DISABLE_CANVAS_VISUAL_CLIPPING
            rect.Intersection(displayRect); 
#endif                
            iCommandBuffers[aIndex]->iUpdateRegion.AddRect(rect);    
            iCommandBuffers[aIndex]->iOriginalUpdateRegion.AddRect(rect);    
            }                
        
        // Combine areas that are next to each others
        iCommandBuffers[aIndex]->iUpdateRegion.Tidy();                    
        iCommandBuffers[aIndex]->iOriginalUpdateRegion.Tidy();                    

        // Subtract this buffers update area from previous buffers update areas to avoid overdraw

 // TODO: Subtraction is for now commented out because it seems to cause UI problems, especially
 //       with lists with scrollbar. Try e.g. opening image from filebrowser, then press back and
 //       scrollbar area is often corrupted. This happens (probaply) because first command buffer contains
 //       real background drawing under scrollbar, but partial command buffers do not contain it although 
 //       their update region extends below scrollbar.
 //       
 //       This issue should be fixed because leaving subration undone causes performance to be worse.
 //       
        
 /*
        if (aIndex > 0)
            {
            for (TInt i=aIndex; i>0; i--)
                {
                TInt previous = aIndex - 1;
                iCommandBuffers[previous]->iUpdateRegion.SubRegion(iCommandBuffers[aIndex]->iUpdateRegion);
                iCommandBuffers[previous]->iUpdateRegion.Tidy();                            
                }            
            }
*/            
        }
    }

void CHuiCanvasWsPainter::WsSetShapeRegionL( TInt aIndex )
    {
    iTempRegion.Clear();                
    iWsCommandBufferReader->ReadRegionL( iTempRegion );
    
	
    // If there is a shape region, get it only from the newest buffer. 
    // This is the case when the buffer has just been added to the canvas visual.
    
    if(aIndex == iCommandBuffers.Count() - 1)
        {
        // Note: The "set shape" command looks to be used only if it really is different from
        // the window rect. So there is no need to check if the shape region is relevant. 
        iShapeRegion.Copy(iTempRegion);

        // note: iUpdateRegion will be updated later, set flags to indicate pending
        iShapeRegionAvailable = ETrue;
        iShapeRegionOrigin = iCanvasVisual->DisplayRect().Round().iTl;
        
        iShapeRegionClearingPending = ETrue;
        iShapeRegionClippingPending = ETrue;
        }
    
	
    #ifdef _DEBUG
    if (iShapeRegion.Count() == 0 && iTempRegion.Count() > 0)
        {
        HUI_DEBUG(_L("CHuiCanvasWsPainter::WsSetShapeRegionL. Error: iShapeRegion not set by any command buffer! However, there exists at least one command buffer that has shape region command."));
        }
    #endif
    }

void CHuiCanvasWsPainter::DoDigestPaddingL()
    {
    TUint8 padding = iWsCommandBufferReader->ReadInt8L();
    while( padding-- )
        {
#ifdef _DEBUG
        if ( iWsCommandBufferReader->ReadInt8L() != 0 )
            {
            RDebug::Print(_L("CHuiCanvasWsPainter::DigestPaddingL - Padding error"));
            USER_INVARIANT();
            }
#else
        iWsCommandBufferReader->ReadInt8L();
#endif        
        }
    }

void CHuiCanvasWsPainter::DestroyBuffer(TInt aIndex)
    {
    CHuiCanvasCommandBuffer* buffer = iCommandBuffers[aIndex];

    TTime now;
    now.UniversalTime();    
    TInt lifetimeInMs =  now.MicroSecondsFrom( buffer->iCreationTime ).Int64()/1000;                               
    if (lifetimeInMs > 0 && lifetimeInMs < KHuiCanvasDisableRenderBufferHandleInterval)
        {
        /* Check and disable render buffer if needed */
        if (iCommandBuffers.Count() == 2)
            {
            EnableRenderBufferIfNeeded(EFalse);
            }
        }
    
    iCommandBuffers.Remove(aIndex);
    delete buffer;
    }


TBool CHuiCanvasWsPainter::RemoveRedundantBuffers()
    {
    TBool didRemoveBuffers = EFalse;
    TInt originalBufferCount = iCommandBuffers.Count();
    
    RemoveBuffersWithoutRealDrawing();

    // Don't remove command buffers with modified display rect.
	// Instead, apply clipping.
    //RemoveBuffersWithMovedDisplayRect();
    ModifyBuffersWithChangedDisplayRect();
    //RemoveBuffersWithOldDisplayRect();
    
    RemoveBuffersWithEmptyUpdateRegion();
    RemoveBuffersWithOverlappingUpdateRegion();         
    
    didRemoveBuffers = (originalBufferCount != iCommandBuffers.Count());    
    return didRemoveBuffers;
    }

void CHuiCanvasWsPainter::RemoveBuffersWithMovedDisplayRect()
    {
    TInt bufferCount = iCommandBuffers.Count();
    
    TRect canvas = iCanvasVisual->DisplayRect().Round();
    for (TInt cb = bufferCount - 1; cb >= 0; cb--)
        {
        CHuiCanvasCommandBuffer* buffer = iCommandBuffers[cb];
        TRect bufRect = buffer->iOriginalDisplayRect.Round();
        
        // If the visual has moved, delete the old buffer
        if (bufRect.iTl != canvas.iTl)
            {
            DestroyBuffer(cb);
            }
        }
    }

void CHuiCanvasWsPainter::ModifyBuffersWithChangedDisplayRect()
    {
    TInt bufferCount = iCommandBuffers.Count();
    TRect canvasRect = iCanvasVisual->DisplayRect().Round();
    TRegionFix<1> region(canvasRect);

    // We must first offset to canvasRect.iTl, otherwise region clipping doesn't work.
    UpdateBufferUpdateRegions(canvasRect.iTl);
    
    // If the buffers have different update region than CanvasVisual, clip
    // the drawing to canvas visual's & cmdbuffer's updateregions' intersection.
    for (TInt cb = 0; cb < bufferCount; cb++)
        {
        CHuiCanvasCommandBuffer* cmdbuffer = iCommandBuffers[cb];
        TRect bufRect = cmdbuffer->iOriginalDisplayRect.Round();
                
        if (bufRect != canvasRect)
            {
            cmdbuffer->iUpdateRegion.Copy(cmdbuffer->iOriginalUpdateRegion);
            
            TPoint deltaPos = cmdbuffer->iPositionForUpdateRegion - cmdbuffer->iOriginalDisplayRect.Round().iTl;
            cmdbuffer->iUpdateRegion.Offset(deltaPos);
            
            cmdbuffer->iUpdateRegion.Intersect(region);
            
            if ( iShapeRegionAvailable )
                {
                TranslateShapeRegion(cmdbuffer->iPositionForUpdateRegion);
                cmdbuffer->iUpdateRegion.Intersect(iShapeRegion);
                }
            
            cmdbuffer->iUpdateRegion.Tidy();
            }
        }
    }

void CHuiCanvasWsPainter::RemoveBuffersWithoutRealDrawing()
    {
    TInt bufferCount = iCommandBuffers.Count();
    for (TInt cb = bufferCount - 1; cb >= 0; cb--)
        {
        CHuiCanvasCommandBuffer* buffer = iCommandBuffers[cb];
        TInt flags = buffer->iStatusFlags;
        if ((flags & EHuiCanvasBufferStatusScanned) && !(flags & EHuiCanvasBufferContainsDrawing))
            {
            DestroyBuffer(cb);
            }
        }        
    }


void CHuiCanvasWsPainter::RemoveBuffersWithOldDisplayRect() 
    {
    TInt bufferCount = iCommandBuffers.Count();
    for (TInt cb = bufferCount - 1; cb >= 0; cb--)
        {
        CHuiCanvasCommandBuffer* buffer = iCommandBuffers[cb];
        if (buffer->iOriginalDisplayRect.Round() != iCanvasVisual->DisplayRect().Round())
            {
            DestroyBuffer(cb);
            }
        }    
    }

void CHuiCanvasWsPainter::RemoveBuffersWithEmptyUpdateRegion() 
    {
    TInt bufferCount = iCommandBuffers.Count();
    for (TInt cb = bufferCount - 1; cb >= 0; cb--)
        {
        CHuiCanvasCommandBuffer* buffer = iCommandBuffers[cb];
        if (buffer->iOriginalUpdateRegion.Count() == 0)
            {        
            DestroyBuffer(cb);
            }
        }    
    }

void CHuiCanvasWsPainter::RemoveBuffersWithOverlappingUpdateRegion() 
    {
    // Check possibly overlapping update regions
    TBool arraysChanged = ETrue;  
    while (arraysChanged)
        {
        arraysChanged = EFalse;
        iTempRegion.Clear();
        
        // Loop all buffers                
        TInt bufferCount = iCommandBuffers.Count();        
        for (TInt cb = bufferCount - 1; cb >= 0; cb--)
            {            
            if (arraysChanged)
                {
                break;    
                }
            
            CHuiCanvasCommandBuffer* cmdbuffer = iCommandBuffers[cb];
            // Translate region to be relative to visual top left corner.
            TPoint offset = -cmdbuffer->iOriginalDisplayRect.Round().iTl;

            for (TInt j=0; j < cmdbuffer->iOriginalUpdateRegion.Count();j++)
                {
                TRect rect = cmdbuffer->iOriginalUpdateRegion[j];
                rect.Move(offset);
                iTempRegion.AddRect(rect);
                }
                        
            // Check older buffers for overlapping regions against current buffer
            if (cb > 0)
                {                   
                for (TInt i = cb - 1; i >= 0; i--)
                    {
                    if (arraysChanged)
                        {
                        break;    
                        }

                    CHuiCanvasCommandBuffer* previousCommands = iCommandBuffers[i];
                    TPoint previousCommandsOffset = previousCommands->iOriginalDisplayRect.Round().iTl;
                    
                    // Keep count how many subregions of the prevous command are contained inside current command buffer region
                    TInt coveredRegionCount = 0;                    
                    for (TInt k=0; k < previousCommands->iOriginalUpdateRegion.Count();k++)
                        {
                        iTempCurrentSubRegion.Clear();
                        iTempIntersectingRegion.Clear();                                               
                        iTempCurrentSubRegion.AddRect(previousCommands->iOriginalUpdateRegion[k]);
                                                
                        iTempCurrentSubRegion.Offset(-previousCommandsOffset);
                        
                        iTempIntersectingRegion.Intersection(iTempRegion, iTempCurrentSubRegion);
                        iTempIntersectingRegion.Tidy();
                        
                        if (iTempIntersectingRegion.Count() == 1)
                            {
                            iTempIntersectingRegion.Offset(previousCommandsOffset);
                        
                            if (iTempIntersectingRegion[0] == previousCommands->iOriginalUpdateRegion[k])
                                {
                                coveredRegionCount++;
                                // If all regions were overlapping agains current buffer, we can delete this.
                                if (coveredRegionCount == previousCommands->iOriginalUpdateRegion.Count())
                                    {
                                    DestroyBuffer(i);
                                    arraysChanged = ETrue;
                                    break;                                                                            
                                    }
                                }
                            else
                                {
                                // There is no reason to continue with this one if even one of its subregions
                                // is  outside overlapping region.
                                break;                                    
                                }
                            }                        
                        }
                    }
                }
            }
        }
    }

void CHuiCanvasWsPainter::ApplyShapeRegion()
    {
    // If shape region has changed recalculate all update regions
    if (iShapeRegionClippingPending)
        {
        TRect canvasRect = iCanvasVisual->DisplayRect().Round();

        // We must first offset to canvasRect.iTl, otherwise region clipping doesn't work.
        UpdateBufferUpdateRegions(canvasRect.iTl);

        TInt bufferCount = iCommandBuffers.Count();
        for (TInt cb = 0; cb < bufferCount; cb++)
            {        
            CHuiCanvasCommandBuffer* cmdbuffer = iCommandBuffers[cb];
            cmdbuffer->iUpdateRegion.Copy(cmdbuffer->iOriginalUpdateRegion);                

            TPoint deltaPos = cmdbuffer->iPositionForUpdateRegion - cmdbuffer->iOriginalDisplayRect.Round().iTl;
            cmdbuffer->iUpdateRegion.Offset(deltaPos);

            if ( iShapeRegionAvailable )
                {
                TranslateShapeRegion(cmdbuffer->iPositionForUpdateRegion);                 
                cmdbuffer->iUpdateRegion.Intersect(iShapeRegion);
                }
            cmdbuffer->iUpdateRegion.Tidy();
            }
        iShapeRegionClippingPending = EFalse;       
        }    
    }
    
void CHuiCanvasWsPainter::TranslateShapeRegion(const TPoint& aNewOrigin)
    {
    if (iShapeRegionOrigin != aNewOrigin)
        {
        iShapeRegion.Offset(aNewOrigin - iShapeRegionOrigin);
        iShapeRegionOrigin = aNewOrigin;
        }
    }

CHuiCanvasGc& CHuiCanvasWsPainter::CanvasGc() const
	{
	return iCanvasWsGc->CanvasGc();
	}

TInt CHuiCanvasWsPainter::PaintedAreaCount() const
    {
    TInt paintedAreaCount = 0;
    TInt bufferCount = iCommandBuffers.Count();
    for (TInt cb = 0; cb < bufferCount; cb++)
        {        
        paintedAreaCount += iCommandBuffers[cb]->iUpdateRegion.Count();
        }
    return paintedAreaCount;
    }
    
THuiCanvasPaintedArea CHuiCanvasWsPainter::PaintedArea(TInt aIndex)
    {
    THuiCanvasPaintedArea area;
    area.iPaintType = EHuiCanvasPaintTypeOpaque;
    area.iFlags = 0;    
    
    TInt firstIndexInCurrentBuffer = 0;
    TInt lastIndexInCurrentBuffer = 0;
    TInt indexCounter = 0;
    
    TInt bufferCount = iCommandBuffers.Count();
    for (TInt cb = 0; cb < bufferCount; cb++)
        {
        TInt updateRegionCount = iCommandBuffers[cb]->iUpdateRegion.Count();
        if (updateRegionCount)
            {        
            firstIndexInCurrentBuffer = indexCounter;
            lastIndexInCurrentBuffer = firstIndexInCurrentBuffer + updateRegionCount - 1;
            
            if ((firstIndexInCurrentBuffer <= aIndex) && (aIndex <= lastIndexInCurrentBuffer))
                {
                TInt localIndex = aIndex - firstIndexInCurrentBuffer;
                area.iPaintedRect = iCommandBuffers[cb]->iUpdateRegion[localIndex];
                break;
                }        
            indexCounter += updateRegionCount;        
            }
       }   
    return area;
    }

TInt CHuiCanvasWsPainter::SetCapturingBufferL(CFbsBitmap* aTarget)
    {
    TInt result = iCanvasWsGc->SetCapturingBufferL(aTarget);
        
    if ( !aTarget )
        {
        // We may have switched to another GC - so to ensure that 
        // all are released, we will send this request to all.
        CHuiCanvasWsGc* gcs[] = 
            { 
            iCanvasWsBitGc, 
            iCanvasWsSwGc, 
            iCanvasWsHwGc 
            };
        TInt count = sizeof(gcs) / sizeof(CHuiCanvasWsGc*);
        for ( TInt i = count - 1; i >= 0; --i )
            {
            if ( gcs[i] && gcs[i] != iCanvasWsGc )
                {
                gcs[i]->SetCapturingBufferL(NULL);
                }
            }
        }

    return result;
    }

void CHuiCanvasWsPainter::ClearCapturingBufferArea(const TRect& aRect)
    {
    iCanvasWsGc->ClearCapturingBufferArea(aRect);
    }

TRect CHuiCanvasWsPainter::DirtyRect() const
    {
    iTempRegion.Clear();                
    TInt bufferCount = iCommandBuffers.Count();
    for (TInt cb = 0; cb < bufferCount; cb++)
        {        
        const CHuiCanvasCommandBuffer& cmdbuffer = *iCommandBuffers[cb];
        
        // Report only changed buffers. This should be ok as long as
        // this method is only used when scanning visual tree and
        // thus visual (and buffer) changed flags are in correct states.
        if (cmdbuffer.iChanged)
            {
            iTempRegion.Union(cmdbuffer.iUpdateRegion);
            }
        }
    return iTempRegion.BoundingRect();
    }

TBool CHuiCanvasWsPainter::IsBufferCompletelyOutisideClippingRegion(const CHuiCanvasCommandBuffer& aCommandbuffer) const
    {
    CHuiGc* gc = iCanvasWsGc->CanvasGc().Gc();
    if (gc && gc->ClipRegion().Count())
        {
        // This is a bit tricky part, gc clipregion is in transformed coordinates but our update region is not.
        // So we must transform our update region before we can compare it to gc clipping region and
        // know if it falls comletely outside it or not.        
        iTempRegion.Clear();
        for (TInt i=0; i<aCommandbuffer.iUpdateRegion.Count();i++)
            {
            THuiRealRect transformed = aCommandbuffer.iUpdateRegion[i];
            gc->TransformDirtyRect(transformed);
            iTempRegion.AddRect(transformed.Round());            
            }
        
        CHuiCanvasGc::TClipRectVisibility visibility = iCanvasWsGc->CanvasGc().IsClipped(iTempRegion, gc->ClipRegion());
        
        if (visibility == CHuiCanvasGc::EFullyOutside)
            {
            return ETrue;
            }
        }

    return EFalse;
    }

TInt CHuiCanvasWsPainter::EnableRenderBuffer(TBool aEnable)
    {
    iEnableRenderBuffer = aEnable;
    
    if (iCanvasWsGc && ( aEnable != iCanvasWsGc->IsRenderBufferEnabled() ) )
        {
        if (aEnable)
            {
            // Must redraw all buffers to render buffer at next frame
            RenewAllBuffers();         
            }        
        return iCanvasWsGc->EnableRenderbuffer(aEnable);
        }
    else
        {
        return KErrNone;
        }    
    }

void CHuiCanvasWsPainter::EnableRenderBufferIfNeeded(TBool aEnable)
    {
    if (!KHuiCanvasAutomaticRenderBufferUsage || !CHuiStatic::Renderer().Allows(EHuiRenderPluginAllowVisualPBufferSurfaces))
        {
        return;
        }
    
    if (aEnable)
        {
        if (iAutomaticRenderBufferUsage && !iCanvasWsGc->IsRenderBufferEnabled() && HasOldCommandBuffers(KHuiCanvasEnableRenderBufferHandleInterval))
            {
            RDebug::Print(_L("CHuiCanvasWsPainter::EnableRenderBufferIfNeeded: Enabling renderbuffer"));
            EnableRenderBuffer(ETrue);
            iAutomaticRenderBufferUsage = ETrue;
            }        
        }
    else
        {
        if (iCanvasWsGc->IsRenderBufferEnabled())
            {
            RDebug::Print(_L("CHuiCanvasWsPainter::EnableRenderBufferIfNeeded: Disabling renderbuffer"));
            EnableRenderBuffer(EFalse);
            iAutomaticRenderBufferUsage = ETrue;
            }
        }    
    }

void CHuiCanvasWsPainter::SelectGcL()
    {
    TBool hasUnsupportedCommands = EFalse;
    TInt bufferCount = iCommandBuffers.Count();
    for (TInt cb = 0; cb < bufferCount; cb++)
        {        
        const CHuiCanvasCommandBuffer& cmdbuffer = *iCommandBuffers[cb];
        if (cmdbuffer.iContainsUnsupportedCommands)
            {
            hasUnsupportedCommands = ETrue;
            break;
            }
        }
        
    THuiMemoryLevel memoryLevel;
    if ( iCanvasVisual ) // in constructor, iCanvasVisual == NULL
        {
        memoryLevel = iCanvasVisual->Env().MemoryLevel();
        }
    else
        {
        memoryLevel = CHuiStatic::Env().MemoryLevel();
        }

    CHuiCanvasWsGc* oldGc = iCanvasWsGc;

    if (memoryLevel <= EHuiMemoryLevelLowest)
        {
        if (!iCanvasWsGc || iCanvasWsGc != iCanvasWsBitGc)
            {
            // Fallback to the BIT GDI mode
            if (!iCanvasWsBitGc)
                {
                iCanvasWsBitGc = CHuiCanvasWsBitGc::NewL();
                }        
            iCanvasWsGc = iCanvasWsBitGc;
            // Must redraw all buffers to render buffer at next frame
            RenewAllBuffers();             
            }
        }
    else if (hasUnsupportedCommands && (!iCanvasWsGc || 
             iCanvasWsGc != iCanvasWsSwGc))
        {
        // Fallback to the SW mode
        if (!iCanvasWsSwGc)
            {
            iCanvasWsSwGc = CHuiCanvasWsSwGc::NewL();     
            }        
        iCanvasWsGc = iCanvasWsSwGc;
        // Must redraw all buffers to render buffer at next frame
        RenewAllBuffers();         
        }
    else if (!hasUnsupportedCommands && (!iCanvasWsGc || 
             iCanvasWsGc != iCanvasWsHwGc))
        {
        // Return to the HW accelerated mode
        if (!iCanvasWsHwGc)
            {
            iCanvasWsHwGc = CHuiCanvasWsHwGc::NewL();     
            }                
        iCanvasWsGc = iCanvasWsHwGc;
        // Must redraw all buffers to render buffer at next frame
        RenewAllBuffers();         
        }
    else
        {
        // We are good
        }        
        
    // Clear caches of non-used GCs
    if ( oldGc && oldGc != iCanvasWsGc )
        {
        oldGc->ClearCache();
        }
    if ( iCanvasWsGc )
        {
        // Forward 'enable render buffer' setting to new GC.
        iCanvasWsGc->EnableRenderbuffer( iEnableRenderBuffer );
        }

    #ifdef HUI_DEBUG_TRACK_DRAWING
    if (!iCanvasDebugWsGc)
        {
        iCanvasDebugWsGc = CHuiCanvasDebugWsGc::NewL( iCanvasWsGc, *TTlsData::CommandDebugger() );
        }
    else if(iCanvasWsGc!=iCanvasDebugWsGc) 
        {
        iCanvasDebugWsGc->SetInternalGc(iCanvasWsGc);
        }
    
    iCanvasWsGc = iCanvasDebugWsGc;
    #endif
    
    AMT_MAP_CANVAS_WS_PAINTER_SELECT_GC();
    }

void CHuiCanvasWsPainter::RenewAllBuffers()
    {
    ClearAllBufferStatusFlags(EHuiCanvasBufferStatusAll);
    SetAllBufferStatusFlags(EHuiCanvasBufferStatusNew);                
    
    if (iCanvasVisual)
        {
        iCanvasVisual->SetChanged();
        }    
    }
	
void CHuiCanvasWsPainter::UpdateBufferUpdateRegions(TPoint aPos)
	{
	// iUpdateRegion is in screen coordinates. If visual moves position, iUpdateRegion 
	// should be updated as well. Otherwise visual will encounter clipping.
    TBool updated = EFalse;
	
	for (TInt cb = 0; cb < iCommandBuffers.Count(); cb++)
        {
		if (iCommandBuffers[cb]->iPositionForUpdateRegion != aPos && !iCommandBuffers[cb]->iUpdateRegion.IsEmpty() )
			{
			updated = ETrue;
			
			TPoint deltaPos = aPos - iCommandBuffers[cb]->iPositionForUpdateRegion;
			iCommandBuffers[cb]->iUpdateRegion.Offset(deltaPos);
			iCommandBuffers[cb]->iPositionForUpdateRegion = aPos;
			}
		
		if (updated)
		    {
            // If one command buffer is drawn, then all following command
            // buffers must be redrawn as well.
            iCommandBuffers[cb]->ClearStatusFlags(EHuiCanvasBufferStatusAll);
            iCommandBuffers[cb]->SetStatusFlags(EHuiCanvasBufferStatusNew);			
            }
        }
    }

void CHuiCanvasWsPainter::SetMemoryLevel(THuiMemoryLevel /*aLevel*/)
    {
    TRAP_IGNORE(SelectGcL());
    }

