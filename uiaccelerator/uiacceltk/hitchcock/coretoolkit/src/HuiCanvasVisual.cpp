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
* Description:   Implementation of CHuiCanvasVisual.
*
*/



#include "uiacceltk/HuiCanvasVisual.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiGc.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiVisual.h"
#include "uiacceltk/HuiLayout.h"
#include "uiacceltk/HuiTextMesh.h"
#include "uiacceltk/HuiCanvasCmdBufferReader.h"
#include "uiacceltk/HuiTransformation.h"
#include "huicanvasgc.h"
#include "HuiRenderSurface.h"
#include "huicanvastexturecache.h"
#include "huicanvasbackground.h"
#include <graphics/wsgraphicscontext.h>
#include <e32cmn.h>
#include <AknLayoutFont.h>
#include "huicanvasalfpainter.h"
#include "huicanvaswspainter.h"
#include "HuiRosterImpl.h"
#include "huilayoutdata.h"
#include "Matrix.h"


#include "HuiFxEffect.h"
#include "HuiCmdBufferBrush.h"
#include "huiextension.h"

#include "huiwscanvascommands.h"

/** Flag to convienintly disable/enable canvas render buffer usage if needed */
/* Turned to EFalse as DFS77 was having booting problems in wk26 */
/* Now checked dynamically from CHuiRenderer to allow different behaviour in DFS77 and DFS79 */
//const TBool KHuiCanvasVisualUseRenderBuffer = EFalse; 

NONSHARABLE_STRUCT( CHuiCanvasVisual::THuiCanvasVisualData )
    {
    THuiCanvasVisualData(){};
    TInt iCommandSetType;
    TInt iCanvasFlags;
    
    // Canvas background
    CHuiCanvasBackground* iBackground;    
    CHuiCanvasPainter* iCanvasPainter;

	// External draw content. Can be layout visual that have child visuals.
    CHuiVisual* iExternalContentVisual; // Not owned
    
    CHuiCanvasVisual* iParentCanvas; // Not owned
    RPointerArray<CHuiCanvasVisual> iChildCanvasList; // Does not own contained visuals 
    RRegionBuf<5> iClipRegion;
    RRegionBuf<20> iPaintedRegion;
    CHuiCanvasRenderBuffer* iStoredRenderBuffer;
    
    // For handling possible background surface that window may have
    TBool iLayerUsesAlphaFlag;
    TRect iLayerExtent;

    RRegionBuf<5> iShapeRegion;
    TPoint iShapeOrigin;

    // Flag to indicate if this has received commands while inactive
    TBool iCommandsReceivedWhileNoCache;
    };

EXPORT_C CHuiCanvasVisual* CHuiCanvasVisual::AddNewL(CHuiControl& aOwnerControl,
                                                 CHuiLayout* aParentLayout)
    {
    CHuiCanvasVisual* canvas = STATIC_CAST(CHuiCanvasVisual*,
        aOwnerControl.AppendVisualL(EHuiVisualTypeCanvas, aParentLayout));
    return canvas;
    }


CHuiCanvasVisual::CHuiCanvasVisual(MHuiVisualOwner& aOwner)
        : CHuiLayout(aOwner)
    {
    }

void CHuiCanvasVisual::ConstructL()
    {
    CHuiLayout::ConstructL();

    SetFlags(EHuiVisualFlagWserv);
    
    iCanvasVisualData = new (ELeave) THuiCanvasVisualData;
    iCanvasVisualData->iCommandSetType = ECommandBufferWs;
    iCanvasVisualData->iCanvasFlags = 0;   
    iCanvasVisualData->iExternalContentVisual = NULL;    
    iCanvasVisualData->iParentCanvas = NULL;
    iCanvasVisualData->iBackground = NULL;
    iCanvasVisualData->iCanvasPainter = NULL;
    iCanvasVisualData->iStoredRenderBuffer = 0;
    
    iCanvasVisualData->iBackground = CHuiCanvasBackground::NewL();
    iCanvasVisualData->iBackground->SetVisual(this);
    
    iCanvasVisualData->iCanvasPainter = CHuiCanvasWsPainter::NewL();
    iCanvasVisualData->iCanvasPainter->SetVisual(this);
    
    TBool useCanvasRenderBuffer = CHuiStatic::Renderer().Allows(EHuiRenderPluginAllowVisualPBufferSurfaces);    
    iCanvasVisualData->iCanvasPainter->EnableRenderBuffer(useCanvasRenderBuffer); 

    iCanvasVisualData->iLayerUsesAlphaFlag = EFalse;
    iCanvasVisualData->iLayerExtent = TRect();
    
    iCanvasVisualData->iShapeOrigin = TPoint();
    
    iCanvasVisualData->iCommandsReceivedWhileNoCache = EFalse;
    
    // subwindow effects
    //EnableBrushesL(ETrue);
    }


CHuiCanvasVisual::~CHuiCanvasVisual()
    {
   
    FreeRenderBuffer();	
    if (iCanvasVisualData)
        {
        if(iCanvasVisualData->iExternalContentVisual)
            {
            // Allow the external content visual to be drawn normally
            iCanvasVisualData->iExternalContentVisual->ClearFlag(EHuiVisualFlagDrawOnlyAsExternalContent);
            }
        ClearCommandSet();
        if ( iCanvasVisualData->iBackground )
            {
            delete iCanvasVisualData->iBackground;
            iCanvasVisualData->iBackground = NULL;
            }
        if ( iCanvasVisualData->iCanvasPainter )
            {
            delete iCanvasVisualData->iCanvasPainter;
            iCanvasVisualData->iCanvasPainter = NULL;
            }
        
    	
    	iCanvasVisualData->iExternalContentVisual = NULL;
    	
    	// Tell parent that we are destructed
    	if (iCanvasVisualData->iParentCanvas)
    	    {
    	    iCanvasVisualData->iParentCanvas->RemoveChildCanvas(this);
    	    }
    	
    	// Tell children
        for (TInt i=0; i<iCanvasVisualData->iChildCanvasList.Count(); i++)
            {
            iCanvasVisualData->iChildCanvasList[i]->SetParentCanvas(NULL);
            }
    	
        iCanvasVisualData->iPaintedRegion.Close();
        iCanvasVisualData->iClipRegion.Close();
        iCanvasVisualData->iChildCanvasList.Close();
        iCanvasVisualData->iShapeRegion.Close();
        }
    
    delete iCanvasVisualData;
    iCanvasVisualData = NULL;
    
    }

CHuiCanvasGc& CHuiCanvasVisual::CanvasGc() const
    {
    return iCanvasVisualData->iCanvasPainter->CanvasGc();
    }

EXPORT_C void CHuiCanvasVisual::SetClearBackground(CHuiDisplay::TClearMode aClearBackground)
    {
    if (iCanvasVisualData->iBackground->ClearBackground() != aClearBackground)
        {
        iCanvasVisualData->iBackground->SetClearBackground(aClearBackground);    
        SetChanged();
        }
    }

EXPORT_C void CHuiCanvasVisual::SetBackgroundColor(const TRgb& aBackgroundColor)
    {
    if (iCanvasVisualData->iBackground->BackgroundColor() != aBackgroundColor)
        {
        iCanvasVisualData->iBackground->SetBackgroundColor(aBackgroundColor);    
        SetChanged();
        }
    }

EXPORT_C void CHuiCanvasVisual::SetBackgroundItems(const RArray<THuiDisplayBackgroundItem>& aItems)
    {
    iCanvasVisualData->iBackground->SetBackgroundItems(aItems);    
    }


EXPORT_C void CHuiCanvasVisual::SetExternalContentL(CHuiVisual* aVisual)
	{
	if (aVisual)
		{
		// Clip external content to canvas visual rect
		SetClipping(ETrue);
		}
	else
		{
		SetClipping(EFalse);
		}
		
	iCanvasVisualData->iExternalContentVisual = aVisual;
	}

EXPORT_C CHuiVisual* CHuiCanvasVisual::ExternalContent()
	{
	return iCanvasVisualData->iExternalContentVisual;
	}


void CHuiCanvasVisual::ExpandRectWithContent(TRect& aRect) const
    {
    TRect scannedRect(0,0,0,0);
    
    // HandleBuffer tries to return area where the drawing actually happens, area
    // may be smaller than the visuals displayarea. In that case we dont expand
    // rect but make it smaller.
    HandleBuffer(scannedRect, EScanBuffer, NULL); 
    
    // Note: this could be improved to take care of dirty rects 
    if (Effect() && Display() && CHuiEnv::Static()->MemoryLevel() != EHuiMemoryLevelLowest )
        {        
        // If there is effect enabled, we cannot say anything about dirty area so
        // we set whole screen dirty.
        scannedRect = TRect(TPoint(0,0), Display()->VisibleArea().Size());
        }
    else if (IsExternalContentDrawingEnabled())
        {
        // If the visual has external content or draws background, 
        // set always the whole window area as a content.
        scannedRect = aRect; 
        }
    
    if (scannedRect != TRect(0,0,0,0))
        {        
        if (!Brushes())
            {
            aRect = scannedRect;                
            }
        else
            {
            // However if there are brushes they need to know the visual displayrect
            // so incase scanned rect is smaller we use bounding rect as the expanded rect
            if (!Clipping())
                {
                aRect.BoundingRect(scannedRect);                        
                }            
            }    
        }

    CHuiLayout::ExpandRectWithContent(aRect);
    }

void CHuiCanvasVisual::HandleBuffer(TRect& aDisplayRect, TInt aAction, CHuiGc* aGc) const
    {
    TBool touchCountWasEnabled = EFalse;
    if (iCanvasVisualData->iCommandsReceivedWhileNoCache)
        {
        // Commands were received while this visual didn't keep cache. 
        // So it's possible that some images are still in texture cache but haven't been updated.
        touchCountWasEnabled = Env().CanvasTextureCache().IsTouchCountCheckEnabled();
        Env().CanvasTextureCache().EnableTouchCountCheck();
        }
        
    if (iCanvasVisualData->iCommandSetType == ECommandBufferWs || iCanvasVisualData->iCommandSetType == ECommandBufferAlf)
        {
        TRAPD(err, iCanvasVisualData->iCanvasPainter->HandleBufferL(aDisplayRect, aAction, *this, aGc, DisplayRect().iTl.Round()));  
        if (err)
        	{
        	HUI_DEBUG2(_L("CHuiCanvasVisual::HandleBufferL returned:%d command type was: %d"), err,iCanvasVisualData->iCommandSetType);
        	}
        }
    else
        {
        // do nothing    
        }                
    
    if (iCanvasVisualData->iCommandsReceivedWhileNoCache)
        {
        Env().CanvasTextureCache().EnableTouchCountCheck( touchCountWasEnabled );
        if ( !KeepNoCache() && aAction == EDrawBuffer )
            {
            iCanvasVisualData->iCommandsReceivedWhileNoCache = EFalse;
            }   
        }
    }


TBool CHuiCanvasVisual::PrepareDrawL() 
    {
	if (Flags() & EHuiVisualFlagDrawOnlyAsExternalContent)
	   	{
	    // This is used only as external content visual. Return now if we are not currently drawing
	    // external content.
	   	if (!Display() || !Display()->RosterImpl().IsDrawingExternalContent())
	   		{
			return ETrue;
	   		}
	   	}

    TBool ret = CHuiLayout::PrepareDrawL();
    
    if (ret && IsExternalContentDrawingEnabled())
    	{
    	if (Display())
    		{
    		// Enable drawing external visual tree
	    	Display()->RosterImpl().SetDrawingExternalContent(ETrue);
    		ret =  iCanvasVisualData->iExternalContentVisual->PrepareDrawL();
	    	Display()->RosterImpl().SetDrawingExternalContent(EFalse);
    		}
    	}
    return ret;	
    }
    
    
TBool CHuiCanvasVisual::CanSkipDrawing() const
    {
    if(Display()->RosterImpl().AlfEventWindow() == this)
         {
         return EFalse;
         }
    
    if (Effect())
        {
        TBool hasStoredContent = (IsDelayedEffectSource() || Freezed()) && (StoredRenderBuffer() ||  iCanvasVisualData->iStoredRenderBuffer);
    
        TBool semiTranparentEffectActive = Effect()->IsSemitransparent();
        if ((iOpacity.Now() <= EPSILON && !semiTranparentEffectActive))
            {
            return ETrue;
            }
        TBool childWindowEffectActive = EffectIsAppliedToChildren();
        TBool effectAppliedToSurfacePixels = (Effect()->EffectFlags() & KHuiFxEnableBackgroundInAllLayers);
        if( !childWindowEffectActive &&
            !effectAppliedToSurfacePixels &&
            !HasCommandBuffers(ETrue /*include children*/) && 
            !IsBackgroundDrawingEnabled() &&
            !IsExternalContentDrawingEnabled()&&
            !IsExternalContentDrawingEnabled(ETrue /*include children*/) &&
            !hasStoredContent )
            {
            return ETrue;
            }
        }
    else
        {
        if (iOpacity.Now() <= EPSILON ||
            (!HasCommandBuffers(ETrue /*include children*/) && 
            !IsBackgroundDrawingEnabled() &&
            !IsExternalContentDrawingEnabled() &&
            !IsExternalContentDrawingEnabled(ETrue /*include children*/)) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

void CHuiCanvasVisual::Draw(CHuiGc& aGc) const
    {
    if (Flags() & EHuiVisualFlagDrawOnlyAsExternalContent)
    	{
   	    // This is used only as external content visual. Return now if we are not currently drawing
   	    // external content.
    	if (!Display() || !Display()->RosterImpl().IsDrawingExternalContent())
    		{
			return;
    		}
    	}
    
    // Optimization
    if (CanSkipDrawing())
        {
        return;
        }

    if ((IsDelayedEffectSource() || Freezed()))
        {
        // Select right draw mode
        THuiCanvasDrawMode drawMode = (Flags() & EHuiVisualFlagOpaqueHint) ? EHuiCanvasDrawModeNormal : EHuiCanvasDrawModeBlend;

        if (StoredRenderBuffer())
            {
            DrawStoredFullScreenRenderBuffer(drawMode, aGc);
            return;
            }
        else if (iCanvasVisualData->iStoredRenderBuffer)
            {
            DrawStoredVisualRenderBuffer(drawMode);
            return;
			}
        }  
    
    // If this visual is completely under another visual, then this visual 
    // shouldn't draw anything, but of course there might still be children 
    // that should be drawn.
    // For example, consider the case where you have two visuals:
    // - visual B is child of window A.
    // - visual A and B have exactly same position and size.
    // - both are opaque.
    // Thus, window A can have 'under opaque' hint, but not window B.
    const TBool drawVisualContent = !( Flags() & EHuiVisualFlagUnderOpaqueHint );
    
    THuiRealRect area = DisplayRect();

    // Apply local transformation.
    EnterLocalProjection(aGc, ETrue, 0.f, &area);
    Transform(aGc, ETrue, &area);

    if (drawVisualContent)
        {
        DrawBrushes(aGc, EHuiBrushLayerBackground);
        }

    if(Clipping())
        {
        // @todo  Clipping not going to work if there is a transformation?
        // Layout clipping is not affected by transformation.
        aGc.Enable(CHuiGc::EFeatureClipping);
        aGc.PushClip();
        aGc.Clip(area);
        }

    // Flag to know what we did
    TBool didDrawEffect = EFalse;
    
    if (EffectActive())
        {
        // Note that EHuiVisualFlagOpaqueHint improves performance a lot in cached effect drawing 
        TBool transparent = EFalse; 
        transparent |= (!(Flags() & EHuiVisualFlagOpaqueHint)); // Does not have opaque hint -> always transparent
        transparent |= iOpacity.Now() < 1.0f; // Opacity less than 1.0f -> always transparent
                
        TBool refreshCache = EFalse;        
        TBool layer =  EFalse;
        if (EffectIsAppliedToChildren())
            {
            refreshCache |= ChildTreeChanged(EHuiCanvasFlagExcludeFromParentEffect);

            iCanvasVisualData->iPaintedRegion.Clear();
            layer = CollectRecursivePaintedRegion(iCanvasVisualData->iPaintedRegion, EHuiCanvasFlagExcludeFromParentEffect);
            }
        else
            {
            refreshCache |= Changed();            

            iCanvasVisualData->iPaintedRegion.Clear();
            layer = CollectPaintedRegion(iCanvasVisualData->iPaintedRegion, 0);
            }

        if(Display()->RosterImpl().AlfEventWindow() == this)
            {
            refreshCache |= Display()->RosterImpl().NativeAppsContentChanged();
            }
        didDrawEffect = Effect()->CachedDraw(aGc, area, refreshCache, !transparent, iCanvasVisualData->iPaintedRegion,layer );
        
        }
    
    if ( !didDrawEffect )
        {
        DrawSelf(aGc, area);
        }
    else
        {
        // Effect was drawn, OpenGLES/OpenVG may be in different state than what Gc thinks so we restore it.
        aGc.RestoreState();        
        }
    
    if (!didDrawEffect)
        {
        // Draw all children
        DrawCanvasChildren(aGc, 0, 0);
        }
    else if (didDrawEffect && !EffectIsAppliedToChildren())
        {
        // Draw all children
        DrawCanvasChildren(aGc, 0, 0);    
        }
    else if (didDrawEffect && EffectIsAppliedToChildren())
        {
        // Draw only children which do not want to have parent effect applied to them
        DrawCanvasChildren(aGc, EHuiCanvasFlagExcludeFromParentEffect, 0);        
        }

    if (drawVisualContent && IsContentDrawingEnabled() && IsExternalContentDrawingEnabled())
    	{
    	if (Display())
    		{
    		// Enable drawing external visual tree
    		Display()->RosterImpl().SetDrawingExternalContent(ETrue);
    		// The possible effect has been attached to the layout of the visual itself
    		// If the external content needs an effect, too, it should probably be defined
    		// separately, or alternatively the drawing of an effect should be able to 
    		// handle the external content, too.
	   		iCanvasVisualData->iExternalContentVisual->Draw(aGc);
    		Display()->RosterImpl().SetDrawingExternalContent(EFalse);
    		}
    	}

    if(Clipping())
        {
        // Restore original clipping rectangle.
        aGc.PopClip();
        }

    if (drawVisualContent)
        {
        DrawBrushes(aGc, EHuiBrushLayerForeground);
        }

    // Restore original transformation.
    Transform(aGc, EFalse);
    EnterLocalProjection(aGc, EFalse);
    }


void CHuiCanvasVisual::DrawSelf(CHuiGc& aGc, const TRect& aDisplayRect) const
    {
#ifdef HUI_DEBUG_TRACK_DRAWING	
    if ( Tracking() )
        {
        RDebug::Print(_L("CHuiCanvasVisual::DrawSelf - tracked visual"));
        }
#endif		
    TBool alfEventWindow = (Display()->RosterImpl().AlfEventWindow() == this);
    TReal32 effectiveOpacity = EffectiveOpacity();
    TBool hasStoredContent = (IsContentDrawingEnabled() && (IsDelayedEffectSource() || Freezed())) && (StoredRenderBuffer() || iCanvasVisualData->iStoredRenderBuffer);        
    if ((effectiveOpacity <= 0 || !HasCommandBuffers(ETrue)) && !alfEventWindow && !hasStoredContent)
        {
        // In case background drawing is enabled, and even if we don't have command buffers we still
        // want to issue clear. If the background drawing is enabled here, it means that the
        // window bound to this visual has a background surface. If we dont do the clear, the
        // surface will be covered by the Alf surface, ie. the underlying surface is not visible
        // It's quite common for applications using Khronos APIs not to issue any drawing commands, 
        // as the drawing goes directly via Khronos APIs.
        // In that case, the background clearing should only be skipped if the window is not 
        // visible (ie. the visual is inactive), and that should already be handled elsewhere.
        if (!IsBackgroundDrawingEnabled())
            {
            return;
            }
        }

    if (hasStoredContent)
        {
        // Select right draw mode
        THuiCanvasDrawMode drawMode = (Flags() & EHuiVisualFlagOpaqueHint) ? EHuiCanvasDrawModeNormal : EHuiCanvasDrawModeBlend;
        
        if (StoredRenderBuffer())
            {
            DrawStoredFullScreenRenderBuffer(drawMode, aGc);
			return;
            }
        else if (iCanvasVisualData->iStoredRenderBuffer)
            {
            DrawStoredVisualRenderBuffer(drawMode);
            return;
            }
        }    

    if(alfEventWindow)
         {
         Display()->RosterImpl().DrawNativeAppsContent(aGc, Display());
         return;
         }
    
    // Use 'under opaque' hint to optimize drawing.
    // See comment from CHuiCanvasVisual::Draw for further details. 
    const TBool drawVisualContent = !( Flags() & EHuiVisualFlagUnderOpaqueHint );

    // Canvas clipping is done in DrawSelf instead of Draw so that canvas clipping does not clip effects 
    if (IsCanvasClippingEnabled())
        {
        aGc.Enable(CHuiGc::EFeatureClipping);
        aGc.PushClip();
        aGc.Clip(ClipRegion());        
        }
    
    // Draws background if it has been defined
    if (drawVisualContent && IsBackgroundDrawingEnabled())
        {
        if (iCanvasVisualData->iCanvasPainter && 
            !iCanvasVisualData->iLayerExtent.IsEmpty())
            {
            // In SW rendering case, we support only clearing with transparent.
            iCanvasVisualData->iCanvasPainter->ClearCapturingBufferArea(aDisplayRect);
            }
            
        iCanvasVisualData->iBackground->DrawSelf(aGc, aDisplayRect); 
        }

    aGc.Push(EHuiGcMatrixModel);	

    TRect displayRect = aDisplayRect;
    
    if (IsContentDrawingEnabled())
        {
        // Handle buffer normally unless layered subwindow effects are active
        if (drawVisualContent && !IsContentDrawingLayered())
            {        
            HandleBuffer(displayRect, EDrawBuffer, &aGc);
            }
        
        if (EffectActive() && EffectIsAppliedToChildren())
            {
            DrawCanvasChildren(aGc, 0, EHuiCanvasFlagExcludeFromParentEffect);        
            }
        }
	
    aGc.Pop(EHuiGcMatrixModel);
    
    if(IsCanvasClippingEnabled())
        {
        // Restore original clipping rectangle.
        aGc.PopClip();
        }
    }

void CHuiCanvasVisual::DrawChildren(CHuiGc& aGc) const
    {
    // Draw child windows, begin
    TBool afterOthers = EFalse;
    const TInt count = Count();
    for(TInt i = 0; i < count; ++i)
        {
        CHuiVisual* visual = iHuiLayoutPrivateData->iChildren[i];
        //Ignore inactive child visuals
        if ( visual->Flags()& EHuiVisualFlagInactive )
            {
            continue;
            }
        if(visual->Flags() & EHuiVisualFlagDrawAfterOthers)
            {
            afterOthers = ETrue;
            continue;
            }

        // @todo  No need to draw children that won't be visible.
        visual->Draw(aGc);
        }
    if(afterOthers)
        {
        for(TInt i = 0; i < count; ++i)
            {
            if(iHuiLayoutPrivateData->iChildren[i]->Flags() & EHuiVisualFlagDrawAfterOthers)
                {
                iHuiLayoutPrivateData->iChildren[i]->Draw(aGc);
                }
            }
        }            
    }

void CHuiCanvasVisual::DrawCanvasChildren(CHuiGc& aGc, TInt aIncludeCanvasFlags, TInt aExcludeCanvasFlags) const
    {
    // Draw child windows, begin
    TBool afterOthers = EFalse;
    const TInt count = Count();
    for(TInt i = 0; i < count; ++i)
        {
        CHuiVisual* visual = iHuiLayoutPrivateData->iChildren[i];
        
        //Ignore inactive child visuals
        if ((visual->Flags()& EHuiVisualFlagInactive))
            {
            continue;
            }
        
        TInt canvasFlags = 0;
        if (aIncludeCanvasFlags || aExcludeCanvasFlags)
            {
            canvasFlags = visual->QueryCanvasFlags();
            }
        
        if (aIncludeCanvasFlags && !(canvasFlags & aIncludeCanvasFlags))
            {
            continue;
            }

        if (aExcludeCanvasFlags && (canvasFlags & aExcludeCanvasFlags))
            {
            continue;
            }
        
        if(visual->Flags() & EHuiVisualFlagDrawAfterOthers)
            {
            afterOthers = ETrue;
            continue;
            }

        // @todo  No need to draw children that won't be visible.
        visual->Draw(aGc);
        }
    if(afterOthers)
        {
        for(TInt i = 0; i < count; ++i)
            {
            if(iHuiLayoutPrivateData->iChildren[i]->Flags() & EHuiVisualFlagDrawAfterOthers)
                {
                iHuiLayoutPrivateData->iChildren[i]->Draw(aGc);
                }
            }
        }                
    }

TBool CHuiCanvasVisual::Changed() const
    {
    if (iCanvasVisualData->iCanvasPainter->BuffersChanged())
        {
        return ETrue;        
        }
    else if(CHuiLayout::Changed())
        {
        return ETrue;
        } 
        
    TBool hasChanged = EFalse;     
    if (IsExternalContentDrawingEnabled())
    	{
		hasChanged = iCanvasVisualData->iExternalContentVisual->Changed();
    	}

   	return hasChanged;
    }

// Goes through visual hierarchy and checks if any visual has changed.
// This method does not modify any visuals. Parameter is not const 
// because of CanvasVisual method.
TBool CHuiCanvasVisual::RecursiveChildTreeChanged(CHuiVisual* aVisual, TInt aExcludeCanvasFlags)
    {
    TBool changed = EFalse;

    // Check wheter we should include this visual or igonre it.
    if (aExcludeCanvasFlags)
        {
        if ( !(aVisual->Flags() & EHuiVisualFlagInactive) )
            {
            TInt canvasFlags = aVisual->QueryCanvasFlags();
                
            if ( !(canvasFlags & aExcludeCanvasFlags) )
                {
                changed |= aVisual->Changed();
                }
            }        
        }
    
    if ( !changed )
        {
        const TInt count = aVisual->Count();
        for(TInt i = 0; i < count; ++i)
            {
            CHuiVisual* visual = &aVisual->Visual(i);
            changed |= RecursiveChildTreeChanged( visual, aExcludeCanvasFlags );
            if ( changed )
                {
                break;
                }
            }
        }
    return changed;    
    }

TBool CHuiCanvasVisual::ChildTreeChanged(TInt aExcludeCanvasFlags) const
    {
    // CHuiLayout::Changed() does not check children.  
    // This utility method checks whole child tree below this visual.
    return RecursiveChildTreeChanged(
        const_cast<CHuiCanvasVisual*>(this), aExcludeCanvasFlags);
    }

EXPORT_C void CHuiCanvasVisual::ClearChanged()
    {
    iCanvasVisualData->iCanvasPainter->ClearAllBuffersChanged();
    CHuiLayout::ClearChanged();
    }

EXPORT_C void CHuiCanvasVisual::SetChanged()
    {
    iCanvasVisualData->iCanvasPainter->SetAllBuffersChanged(ETrue);
    CHuiLayout::SetChanged();    
    }

EXPORT_C void CHuiCanvasVisual::SetCommandSetL( const TDesC8& aCommands )
    {
    Env().CanvasTextureCache().EnableTouchCountCheck();    
    ClearCommandSet();
	TRAP_IGNORE(iCanvasVisualData->iCanvasPainter->SetCommandSetL(aCommands));	

	// Memory optimization. Do not prepare cache if visual is inactive.
    if (KeepNoCache())
        {
        iCanvasVisualData->iCommandsReceivedWhileNoCache = ETrue;
        ClearCache();
        }
    else
        {
        PrepareCache();
        }
    
    SetChanged();
    Env().CanvasTextureCache().EnableTouchCountCheck(EFalse);    
    }

THuiRealPoint CHuiCanvasVisual::ConvertPoint(const THuiRealPoint& aPointInUnits) const
    {
    // Convert first from units to pixels        
    THuiRealPoint converted = LocalPointInPixels(aPointInUnits);
    
    // Pixels are relative to the canvas visual itself, change to display relative
    THuiRealRect displayRect = DisplayRect();
    converted.iX += displayRect.iTl.iX;
    converted.iY += displayRect.iTl.iY;

    return converted;        
    }

void CHuiCanvasVisual::ExpandRect(TRect& aRectToExpand, const TRect& aNewRect) const
    {
    if (aRectToExpand == TRect(0,0,0,0))
        {
        aRectToExpand = aNewRect;    
        }
    else
        {
        aRectToExpand.BoundingRect(aNewRect);            
        }            
    }
    
EXPORT_C void CHuiCanvasVisual::SetCommandType( TInt aCommandType )
    {
    TRAP_IGNORE(SetCommandTypeL(aCommandType));
    }

EXPORT_C void CHuiCanvasVisual::ClearCommandSet()
    {
    if ( iCanvasVisualData->iCanvasPainter )
        {
        iCanvasVisualData->iCanvasPainter->ClearCommandSet();
        SetChanged();
        }
    }

#ifdef HUI_DEBUG_TRACK_DRAWING  
EXPORT_C void CHuiCanvasVisual::SetTrackCommandSet( TFileName& aFileName, TBool aTrack )
    {
    iCanvasVisualData->iCanvasPainter->SetTrackCommandSet( aFileName, aTrack || iTrackVisual );
    }
#else
EXPORT_C void CHuiCanvasVisual::SetTrackCommandSet( TFileName&, TBool)
    {
    }
#endif


EXPORT_C void CHuiCanvasVisual::AddCommandSetL( const TDesC8& aMoreCommands )
    {
    Env().CanvasTextureCache().EnableTouchCountCheck();    
    TRAP_IGNORE(iCanvasVisualData->iCanvasPainter->AddCommandSetL(aMoreCommands));
    
    // Memory optimization. Do not prepare cache if visual is inactive.
    if (KeepNoCache())
        {
        iCanvasVisualData->iCommandsReceivedWhileNoCache = ETrue;
        ClearCache();
        }
    else
        {
        PrepareCache();
        }
    
    // don't call setchanged directly here, as it changes all
    // the command buffers dirty. We don't want to do that
    // as the drawing area might be significantly smaller in
    // some cases
    CHuiLayout::SetChanged();
    Env().ContinueRefresh(ETrue);
    Env().CanvasTextureCache().EnableTouchCountCheck(EFalse);    
    }

EXPORT_C void CHuiCanvasVisual::AddPartialCommandSetL( const TDesC8& aMoreCommands, TBool aLastPart )
    {
    iCanvasVisualData->iCanvasPainter->AddPartialCommandSetL(aMoreCommands, aLastPart); 
    }

    
EXPORT_C void CHuiCanvasVisual::ClearCache()
    {
    iCanvasVisualData->iCanvasPainter->ClearCache();
    }

EXPORT_C void CHuiCanvasVisual::PrepareCache()
    {
    if (iCanvasVisualData->iCanvasPainter->HasCommandBuffers())
        {
        TRect displayRect = DisplayRect();
        HandleBuffer(displayRect, EScanBuffer, NULL);                    
        }        
    }

void CHuiCanvasVisual::SetCommandTypeL(TInt aCommandType)
    {
    if (iCanvasVisualData->iCommandSetType == aCommandType)
        {
        return;    
        }
    
   	if (aCommandType == ECommandBufferAlf)
   	   	{
   	   	ClearFlags(EHuiVisualFlagWserv);
   	   	delete iCanvasVisualData->iCanvasPainter;
   	   	iCanvasVisualData->iCanvasPainter = NULL;
        iCanvasVisualData->iCommandSetType = ECommandBufferAlf;    
   	   	iCanvasVisualData->iCanvasPainter = CHuiCanvasAlfPainter::NewL();   
        iCanvasVisualData->iCanvasPainter->SetVisual(this);
   		}	     
    else if (aCommandType == ECommandBufferWs)
    	{
    	SetFlags(EHuiVisualFlagWserv);
    	delete iCanvasVisualData->iCanvasPainter;
   	   	iCanvasVisualData->iCanvasPainter = NULL;
        iCanvasVisualData->iCommandSetType = ECommandBufferWs;    
   	   	iCanvasVisualData->iCanvasPainter = CHuiCanvasWsPainter::NewL(); 
        iCanvasVisualData->iCanvasPainter->SetVisual(this);
        TBool useCanvasRenderBuffer = CHuiStatic::Renderer().Allows(EHuiRenderPluginAllowVisualPBufferSurfaces);            
        iCanvasVisualData->iCanvasPainter->EnableRenderBuffer(useCanvasRenderBuffer); 
    	}
    else
        {
        // For pc lint    
        }	        
    }

EXPORT_C TInt CHuiCanvasVisual::PaintedAreaCount() const
    {
    TInt count = 0;    

    count = iCanvasVisualData->iCanvasPainter->PaintedAreaCount();

    // Check if there is background clearing, it may increase painted area count
    if (IsBackgroundDrawingEnabled())
        {
        TInt backgroundItemCount = iCanvasVisualData->iBackground->BackgroundItemsCount();
        
        if (backgroundItemCount > 0)
            {
            // Background items                
            count += backgroundItemCount; 
            }
        else
            {
            // If there are no background items, then color clearing is used.                
            count += 1; 
            }                         
        }
    
    return count;    
    }
    
EXPORT_C THuiCanvasPaintedArea CHuiCanvasVisual::PaintedArea(TInt aIndex)
    {
    return CanvasPaintedArea(aIndex);            
    }

THuiCanvasPaintedArea CHuiCanvasVisual::CanvasPaintedArea(TInt aIndex) const
    {
    if (IsBackgroundDrawingEnabled())
        {
        // Check if there is background clearing, it may increase painted area count
        THuiCanvasPaintedArea background;
        TInt backgroundItemCount = iCanvasVisualData->iBackground->BackgroundItemsCount();
        
        if (backgroundItemCount > 0)
            {
            // Background items
            if (aIndex < backgroundItemCount)
                {
                RArray<THuiDisplayBackgroundItem> backgroundItems;
                iCanvasVisualData->iBackground->GetBackgroundItems(backgroundItems);

                if (backgroundItems[aIndex].ClearMode() == CHuiDisplay::EClearNone)
                    {
                    // No clearing -> transparent
                    background.iPaintType = EHuiCanvasPaintTypeTransparent;                                             
                    }
                else if (backgroundItems[aIndex].ClearMode() == CHuiDisplay::EClearWithColor)
                    {
                    // Clearing with color, transparency does NOT depend on alpha but is always opaque
                    // because clearing is done with "fast" clear and it just writes the alpha
                    // instead of blending
                    background.iPaintType = EHuiCanvasPaintTypeOpaque;                                                                                                     
                    }
                else
                    {
                    // By default assume opaque
                    background.iPaintType = EHuiCanvasPaintTypeOpaque;                                                 
                    }                    

                background.iPaintedRect = backgroundItems[aIndex].Rect();
                backgroundItems.Close();
                return background;
                }
            else
                {
                return iCanvasVisualData->iCanvasPainter->PaintedArea(aIndex - backgroundItemCount);
                }
            }
        else
            {
            // If there are no background items, then color clearing is used.                
            if (aIndex == 0)
                {
                background.iPaintedRect = DisplayRect();
                background.iPaintType = /*(iCanvasVisualData->iBackground->BackgroundColor().Alpha() == 255) ?*/ EHuiCanvasPaintTypeOpaque;// : EHuiCanvasPaintTypeTransparent; 
                return background;
                }          
            else 
                {
                aIndex--;
                }
            }                                            
        }

    return iCanvasVisualData->iCanvasPainter->PaintedArea(aIndex);            
    }

EXPORT_C TInt CHuiCanvasVisual::SetCapturingBufferL(CFbsBitmap* aTargetBuffer)
    {
    return iCanvasVisualData->iCanvasPainter->SetCapturingBufferL(aTargetBuffer);                 
    }

EXPORT_C void CHuiCanvasVisual::SetCanvasFlags(TInt aCanvasFlags)
    {
    TInt oldFlags = iCanvasVisualData->iCanvasFlags;           
    iCanvasVisualData->iCanvasFlags |= aCanvasFlags;

    // Set visual changed if flags were really changed
    if (oldFlags != iCanvasVisualData->iCanvasFlags)
        {
        SetChanged();
        }
    }

EXPORT_C void CHuiCanvasVisual::ClearCanvasFlags(TInt aCanvasFlags)
    {
    TInt oldFlags = iCanvasVisualData->iCanvasFlags;           
    iCanvasVisualData->iCanvasFlags &= ~aCanvasFlags;

    // Set visual changed if flags were really changed
    if (oldFlags != iCanvasVisualData->iCanvasFlags)
        {
        SetChanged();
        }
    }

EXPORT_C TInt CHuiCanvasVisual::CanvasFlags()
    {
    return iCanvasVisualData->iCanvasFlags;
    }


EXPORT_C TBool CHuiCanvasVisual::IsBackgroundDrawingEnabled() const
    {
    TBool drawBackground = EFalse;
    drawBackground |= (iCanvasVisualData->iBackground->ClearBackground() != CHuiDisplay::EClearNone);
    drawBackground |= (iCanvasVisualData->iBackground->BackgroundItemsCount() > 0);
    
    if (iCanvasVisualData->iCanvasFlags & EHuiCanvasFlagDisableCanvasBackground)
        {
        drawBackground = EFalse;    
        }
    return drawBackground;        
    }

TBool CHuiCanvasVisual::IsContentDrawingEnabled() const
    {
    TBool drawContent = ETrue;
    if (iCanvasVisualData->iCanvasFlags & EHuiCanvasFlagDisableCanvasContent)
        {
        drawContent = EFalse;    
        }
    return drawContent;       
    }

// Goes through visual hierarchy and checks if any visual has external content drawing enabled.
TBool CHuiCanvasVisual::RecursiveIsExternalContentDrawingEnabled(CHuiVisual* aVisual)
    {
    TBool drawExternalContent = EFalse;
    
    const TInt count = aVisual->Count();
    for(TInt i = 0; i < count; ++i)
        {
        CHuiVisual* visual = &aVisual->Visual(i);
        drawExternalContent |= visual->QueryExternalContentDrawingEnabled();
        if ( drawExternalContent )
            {
            break;
            }
        
        drawExternalContent |= RecursiveIsExternalContentDrawingEnabled( visual );
        if ( drawExternalContent )
            {
            break;
            }
        }
    
    return drawExternalContent;    
    }

TBool CHuiCanvasVisual::IsExternalContentDrawingEnabled(TBool aIncludeChildren) const
    {
    TBool drawExternalContent = EFalse;
    if (iCanvasVisualData->iExternalContentVisual)
        {
        drawExternalContent = ETrue;    
        }
    if (aIncludeChildren && !drawExternalContent)
        {
        drawExternalContent |= RecursiveIsExternalContentDrawingEnabled(
            const_cast<CHuiCanvasVisual*>(this));        
        }
    return drawExternalContent;       
    }

EXPORT_C void CHuiCanvasVisual::SetParentCanvas(CHuiCanvasVisual* aParent)
    {
    iCanvasVisualData->iParentCanvas = aParent;
    if (aParent)
        {
        aParent->AddChildCanvas(this);
        }
    }

const TRegion& CHuiCanvasVisual::ClipRegion() const
    {
    iCanvasVisualData->iClipRegion.Clear();
    // TODO: This should be region in cases when canvas shape is non-rectangular, 
    // for now just use display rect. Window Shape is currently only known by the wspainter, 
    // so new interface is needed to query it.
    iCanvasVisualData->iClipRegion.AddRect(DisplayRect().Round());

    CHuiCanvasVisual* parent = iCanvasVisualData->iParentCanvas;    
    if (parent)
        {
        const TRegion& parentClipRegion = parent->ClipRegion(); 
        iCanvasVisualData->iClipRegion.Intersect(parentClipRegion);
        }
    
    return iCanvasVisualData->iClipRegion;
    }

void CHuiCanvasVisual::AddChildCanvas(CHuiCanvasVisual* aChild)
    {
    if (iCanvasVisualData->iChildCanvasList.Find(aChild) == KErrNotFound)
        {
        iCanvasVisualData->iChildCanvasList.Append(aChild);
        }
    }

void CHuiCanvasVisual::RemoveChildCanvas(CHuiCanvasVisual* aChild)
    {
    for (TInt i=0; i<iCanvasVisualData->iChildCanvasList.Count(); i++)
        {
        if (iCanvasVisualData->iChildCanvasList[i] == aChild)
            {
            iCanvasVisualData->iChildCanvasList.Remove(i);
            break;
            }        
        }
    }

EXPORT_C TBool CHuiCanvasVisual::LayerUsesAlphaFlag()
    {
    return iCanvasVisualData->iLayerUsesAlphaFlag;
    }

EXPORT_C void CHuiCanvasVisual::SetLayerUsesAlphaFlag(TBool aEnabled)
    {
    if (aEnabled == KWindowIsDSAHost)
        {
        iCanvasVisualData->iCanvasFlags |= EHuiCanvasFlagDisableCanvasContent;        
        ClearCommandSet();
        return;    
        }    
    iCanvasVisualData->iLayerUsesAlphaFlag = aEnabled;
    }

EXPORT_C TRect& CHuiCanvasVisual::LayerExtent()
    {
    return iCanvasVisualData->iLayerExtent;
    }

EXPORT_C void CHuiCanvasVisual::SetLayerExtent(TRect& aExtent)
    {
    iCanvasVisualData->iLayerExtent = aExtent;
    if(0)// aExtent != TRect() ) // background surface was set or moved to new place
        {
        EnableTransformationL();
        CHuiTransformation& transformation = Transformation();
        
        TPoint extentPos = aExtent.iTl;
        TPoint visualPos = Pos().Now();

        TReal32 xDif = (TReal32)(extentPos.iX - visualPos.iX);
        TReal32 yDif = (TReal32)(extentPos.iY - visualPos.iY);
        
        TSize extentSize = aExtent.Size(); 
        TPoint visualSize = Size().Now();

        TReal32 hVisualWidth = (TReal32)visualSize.iX/2;
        TReal32 hVisualHeigth = (TReal32)visualSize.iY/2;
        
        
        TReal32 xScale = (TReal32)extentSize.iWidth/visualSize.iX;
        TReal32 yScale = (TReal32)extentSize.iHeight/visualSize.iY;
        
		// create transformation when applying first time
        if(transformation.Count() == 0)
            {
            transformation.Translate(xDif,yDif);
            // for moving the visual origo to visual's top left corner
            transformation.Translate(-hVisualWidth,-hVisualHeigth);
            transformation.Scale(xScale,yScale);    
            // move back
            transformation.Translate(hVisualWidth,hVisualHeigth);
            }
        else // update existing transformations
            {
            CHuiTransformation::TTransform& translate = transformation[0];
            translate.iParams[EHuiTransformParamTranslateX] = xDif;
            translate.iParams[EHuiTransformParamTranslateY] = yDif;
            
            CHuiTransformation::TTransform& translateforscale = transformation[1];
            translateforscale.iParams[EHuiTransformParamTranslateX] = -hVisualWidth;
            translateforscale.iParams[EHuiTransformParamTranslateY] = -hVisualHeigth;
            
            CHuiTransformation::TTransform& scale = transformation[2];
            scale.iParams[EHuiTransformParamScaleX] = xScale;
            scale.iParams[EHuiTransformParamScaleY] = yScale;

            CHuiTransformation::TTransform& translateback = transformation[3];
            translateback.iParams[EHuiTransformParamTranslateX] = hVisualWidth;
            translateback.iParams[EHuiTransformParamTranslateY] = hVisualHeigth;
            }
        }
    else // background surface was removed
        {
        iCanvasVisualData->iCanvasFlags &= (~EHuiCanvasFlagDisableCanvasContent);    
        //EnableTransformationL(EFalse);
        }
    }


EXPORT_C void CHuiCanvasVisual::SetShape(const TPoint& aOrigin, const TRegion& aRegion)
    {
    iCanvasVisualData->iShapeOrigin = aOrigin;
    iCanvasVisualData->iShapeRegion.Copy(aRegion);
    if ( iCanvasVisualData->iShapeRegion.CheckError() )
        {
        // fallback to full
        iCanvasVisualData->iShapeRegion.Clear();
        }
    }
    
EXPORT_C TBool CHuiCanvasVisual::HasCustomShape() const
    {
    return iCanvasVisualData->iShapeRegion.Count();
    }
    
EXPORT_C TPoint CHuiCanvasVisual::ShapeOrigin() const
    {
    return iCanvasVisualData->iShapeOrigin;
    }
    
EXPORT_C const TRegion& CHuiCanvasVisual::ShapeRegion() const
    {
    return iCanvasVisualData->iShapeRegion;
    }

TBool CHuiCanvasVisual::IsCanvasClippingEnabled() const
    {
    return iCanvasVisualData->iCanvasFlags & EHuiCanvasFlagEnableCanvasClipping; 
    }

TBool CHuiCanvasVisual::EffectActive() const
    {
    return Effect() && !LoadingEffect();
    }
    
TBool CHuiCanvasVisual::IsContentDrawingLayered() const
    {
    // TODO: real flag here to check if drawing normally or layered with brushes
    return (Brushes() && (Brushes()->Count() > 0));
    }

EXPORT_C void CHuiCanvasVisual::StoreRenderBufferL()
    {
    CHuiCanvasGc& gc = CanvasGc();
    CHuiCanvasRenderBuffer *content_rb = Env().CanvasTextureCache().FindCachedRenderBuffer(*this);
    if (content_rb)
        {
        if (!iCanvasVisualData->iStoredRenderBuffer)
            {
            iCanvasVisualData->iStoredRenderBuffer = gc.CreateRenderBufferL(TSize(0,0));
            }
        iCanvasVisualData->iStoredRenderBuffer->InitializeL(content_rb->Size());
        iCanvasVisualData->iStoredRenderBuffer->Copy(*content_rb);    
        }
    }

void CHuiCanvasVisual::DrawStoredVisualRenderBuffer(TInt aCanvasDrawMode) const
    {
    CHuiCanvasGc& gc = CanvasGc();
    CHuiCanvasVisual* visual = NULL; 
    TBool transparent = EffectiveOpacity() < 1.0f;
    gc.SetDrawMode((THuiCanvasDrawMode)aCanvasDrawMode);
    if (transparent)
        {
        gc.EnableEffectiveOpacity(ETrue);  
        visual = gc.Visual();
        gc.SetVisual(*this);
        gc.SetDrawMode(EHuiCanvasDrawModeBlend);
        }
    THuiRealPoint dest_point = DisplayRect().iTl;
    CHuiCanvasRenderBuffer *stored = iCanvasVisualData->iStoredRenderBuffer;

    gc.DrawImage(*stored, dest_point);
    if (transparent)
        {
        gc.SetVisual(*visual);
        gc.EnableEffectiveOpacity(EFalse);
        }
    }

void CHuiCanvasVisual::DrawStoredFullScreenRenderBuffer(TInt aCanvasDrawMode, CHuiGc& aGc) const
    {
    if (!Display()) return;
    if (!iHuiLayoutPrivateData->iGc)
        {
        CHuiRenderPlugin& renderplugin = CHuiStatic::Renderer();
		// deleted in CHuiLayout destructor or CHuiCanvasVisual::FreeRenderBuffer when not needed anymore
        iHuiLayoutPrivateData->iGc = renderplugin.CreateCanvasGcL();
        }
    CHuiCanvasGc& gc = *iHuiLayoutPrivateData->iGc;
    gc.SetGc(aGc);
    gc.SetDefaults();
    gc.PushTransformationMatrix();
    
    TRect displayArea = Display()->VisibleArea();
    TInt h = displayArea.Height();

    // Pixels are upside down in OpenVG canvas render buffer
    if (CHuiStatic::Env().Renderer() == EHuiRendererVg10)
        {
        gc.Translate(0.0f, h, 0.0f);            
        gc.Scale(1.f, -1.f, 1.f);
        }
    
    THuiRealPoint dest_point = DisplayRect().iTl;
    CHuiCanvasRenderBuffer *stored = StoredRenderBuffer();
    TBool transparent = EffectiveOpacity() < 1.0f;
    CHuiCanvasVisual* visual = NULL; 
	gc.SetDrawMode((THuiCanvasDrawMode)aCanvasDrawMode);
    if (transparent)
        {
        
        gc.EnableEffectiveOpacity(ETrue);  
        visual = gc.Visual();
        gc.SetVisual(*this);
        gc.SetDrawMode(EHuiCanvasDrawModeBlend);
        }
    gc.DrawImage(*stored, dest_point);
    if (transparent)
        {
        gc.SetVisual(*visual);
        gc.EnableEffectiveOpacity(EFalse);
        }
    gc.PopTransformationMatrix();
    }

EXPORT_C void CHuiCanvasVisual::FreeRenderBuffer()
    {
    if (iCanvasVisualData)
        {
        if (iCanvasVisualData->iStoredRenderBuffer)
            {
            delete iCanvasVisualData->iStoredRenderBuffer;
            iCanvasVisualData->iStoredRenderBuffer = NULL;
            delete iHuiLayoutPrivateData->iGc;
            iHuiLayoutPrivateData->iGc = NULL;
            }
        }
    }

// Goes through visual hierarchy and checks if any visual has command buffers
// or other drawable content.
TBool CHuiCanvasVisual::RecursiveHasCommandBuffers(CHuiVisual* aVisual)
    {
    TBool hasCommandBuffers = EFalse;
    
    const TInt count = aVisual->Count();
    for(TInt i = 0; i < count; ++i)
        {
        CHuiVisual* visual = &aVisual->Visual(i);
        // If visual is a canvas one, then QueryHasDrawableContent returns 
        // HasCommandBuffers(EFalse)
        hasCommandBuffers |= visual->QueryHasDrawableContent();
        if ( hasCommandBuffers )
            {
            break;
            }
        
        hasCommandBuffers |= RecursiveHasCommandBuffers( visual );
        if ( hasCommandBuffers )
            {
            break;
            }
        }
    
    return hasCommandBuffers;    
    }

EXPORT_C TBool CHuiCanvasVisual::HasCommandBuffers(TBool aIncludeChildren) const
    {
    TBool hasCommandBuffers = iCanvasVisualData->iCanvasPainter->HasCommandBuffers();
    if (aIncludeChildren && !hasCommandBuffers)
        {
        // Include children branch - just check if there is something to draw.
        hasCommandBuffers |= RecursiveHasCommandBuffers(const_cast<CHuiCanvasVisual*>(this));
        }
    return hasCommandBuffers;
    }
    

void CHuiCanvasVisual::SetSize(const THuiRealSize& aSize, TInt aTransitionTime)
    {
    CHuiLayout::SetSize(aSize, aTransitionTime);
    UpdateChildrenLayout(aTransitionTime);    
    }

TBool CHuiCanvasVisual::ChildSize(TInt aOrdinal, TSize& aSize)
    {
    TBool result(EFalse);
    THuiRealRect rect;
    TInt childRectStatus(THuiLayoutChildRectUpdateNotNeeded);
    childRectStatus = ChildRect(aOrdinal, rect);
    if(childRectStatus != THuiLayoutChildRectNotImplemented)
        {
        result = (childRectStatus & THuiLayoutChildRectSizeUpdateNeeded);
        if(result)
            {
            THuiRealPoint size(rect.Width(), rect.Height());
            aSize = LocalPointInPixels(size).AsSize();
            }
        }
    return result;
    }

TBool CHuiCanvasVisual::ChildPos(TInt aOrdinal, TPoint& aPos)
    {
    TBool result(EFalse);
    THuiRealRect rect;
    TInt childRectStatus(THuiLayoutChildRectUpdateNotNeeded);
    childRectStatus = ChildRect(aOrdinal, rect);
    if(childRectStatus != THuiLayoutChildRectNotImplemented)
        {
        result = (childRectStatus & THuiLayoutChildRectPosUpdateNeeded);
        if(result)
            {
            aPos = LocalPointInPixels(rect.iTl);
            }
        }
    return result;
    }

TInt CHuiCanvasVisual::ChildRect(TInt /*aOrdinal*/, THuiRealRect& aPos)
    {
    THuiRealSize innerSize = InnerSize();
    aPos.iTl = InnerTopLeft(); 
    aPos.iBr = aPos.iTl + THuiRealPoint(innerSize.iWidth, innerSize.iHeight) ; 
    return THuiLayoutChildRectLayoutUpdateNeeded;
    }

void CHuiCanvasVisual::VisualExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    if (aExtensionUid == KHuiVisualQueryUid && aExtensionParams && *aExtensionParams)
        {
        THuiVisualQueryParams* params = static_cast<THuiVisualQueryParams*>(*aExtensionParams);
        switch (params->iQueryType)
            {
        case THuiVisualQueryParams::EQueryCanvasFlags:
            params->iValue = CanvasFlags();
            params->iResult = KErrNone;
            break;
        case THuiVisualQueryParams::EQueryExternalContentDrawingEnabled:
            params->iValue = IsExternalContentDrawingEnabled(EFalse);
            params->iResult = KErrNone;
            break;
        case THuiVisualQueryParams::EQueryHasDrawableContent:
            params->iValue = HasCommandBuffers(EFalse) || IsBackgroundDrawingEnabled();
            params->iResult = KErrNone;
            break;
        default:
            break;
            }
        }
    else
	    {
		CHuiVisual::VisualExtension(aExtensionUid, aExtensionParams);
		}
    }

TBool CHuiCanvasVisual::CollectPaintedRegion(TRegion& aPaintRegion, TInt aExcludeCanvasFlags) const
    {
    // Only our own painted areas.
    TInt paintedAreaCount = PaintedAreaCount();  
    for (TInt i=0; i<paintedAreaCount; i++ )
        {
        aPaintRegion.AddRect( CanvasPaintedArea(i).iPaintedRect.Round() );
        }
    aPaintRegion.Tidy();
    return !iCanvasVisualData->iLayerExtent.IsEmpty();
    }

TBool CHuiCanvasVisual::CollectRecursivePaintedRegion(TRegion& aRecursivePaintRegion, TInt aExcludeCanvasFlags) const
    {
    // First our own painted areas...
    TBool layer = CollectPaintedRegion(aRecursivePaintRegion, aExcludeCanvasFlags);
            
    // ...then children (and their children).
    const TInt count = Count();
    for(TInt i = 0; i < count; ++i)
        {
        // Check wheter we should include this child visual or ignore it.
        if (aExcludeCanvasFlags)
            {
            CHuiVisual* visual = (CHuiCanvasVisual*)&Visual(i);
            if ( !(visual->Flags() & EHuiVisualFlagInactive) )
                {
                TInt canvasFlags = visual->QueryCanvasFlags();
                    
                if ( !(canvasFlags & aExcludeCanvasFlags) )
                    {
                    // If this is marked as Wserv visual, it should be safe to cast.
                    if (visual->Flags() & EHuiVisualFlagWserv)
                        {
                        CHuiCanvasVisual* canvasVisual = (CHuiCanvasVisual*)visual;
                        layer |= canvasVisual->CollectRecursivePaintedRegion(aRecursivePaintRegion, aExcludeCanvasFlags);
                        }
                    }
                }        
            }
        }    
    aRecursivePaintRegion.Tidy();
    return layer;
    }

EXPORT_C TRect CHuiCanvasVisual::CommandBufferCoverage(TInt aOrientation)
    {
    return iCanvasVisualData->iCanvasPainter->CommandBufferCoverage(aOrientation); 
    }

EXPORT_C TBool CHuiCanvasVisual::HasTransParentClear() const
    {
    return iCanvasVisualData->iCanvasPainter->HasCommandBuffers(EHuiCanvasBufferContainsTransparentClear);
    }

TBool CHuiCanvasVisual::KeepNoCache() const
    {
    TBool rosterFrozen = Display() && Display()->Roster().IsVisibleContentFrozen();
    TBool inactive = EFalse; 
    inactive |= Flags() & EHuiVisualFlagInactive;
    inactive |= Flags() & EHuiVisualFlagUnderOpaqueHint;
    
    return rosterFrozen || inactive;
    }
