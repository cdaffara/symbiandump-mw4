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
* Description:   Implementation of CHuiRosterImpl. CHuiRosterImpl implements 
*                the interface defined by CHuiRoster.
*
*/



#include "HuiRosterImpl.h"  // Class definition
#include "HuiRosterEntry.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiSkin.h"
#include "uiacceltk/HuiControlGroup.h"
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiDisplay.h"
#include "uiacceltk/HuiEvent.h"
#include "uiacceltk/HuiTransformation.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiProbe.h"
#include "uiacceltk/HuiCanvasVisual.h"
#include "huilongtapdetector.h"
#include "HuiFxEffect.h"
#include "huieffectable.h"
#include "huicanvasrenderbuffer.h"
#include "huicanvasgc.h"
#include "HuiRenderPlugin.h"

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_TACTILE_FEEDBACK

const TReal32 KHuiVisualOutlineWidth = 1.f;

struct THuiPointerEventDetails 
	{
	MHuiEventHandler* iEventHandler;
	THuiXYMetric iXYMetric;
	TBool iDisableLongTapWhenDragging;
	TBool iDragging;
	};



CHuiRosterImpl::CHuiRosterImpl(CHuiDisplay* aDisplay)
        : iDisplay(aDisplay), iInitialDownEvent(NULL), iEffectOpacity(1.f)
    {
    }

CHuiRosterImpl::~CHuiRosterImpl()
    {
    if (iDisplay)
        {
        iDisplay->Env().RemoveMemoryLevelObserver(this);
        }

    FreezeVisibleContentL(EFalse);
    
    delete iLongTapDetector;
    iEntries.ResetAndDestroy();
    iPointerDragObservers.Close();
    iDisplay = NULL;
    iInputFocus = NULL;
    iUnhandledPointerObservers.Close();
    iLongTapObservers.Close();
    while(iPointerEventDetails.Count()>0)
    	{
    	delete iPointerEventDetails[0];
		iPointerEventDetails.Remove(0);    	
    	}
    iPointerEventDetails.ResetAndDestroy();
    }


void CHuiRosterImpl::SetDisplay(CHuiDisplay* aDisplay)
    {
    iDisplay = aDisplay;    
    }
    
    
void CHuiRosterImpl::SetRect(const TRect& aRect)
    {
    iRect = TRect(0, 0, aRect.Width(), aRect.Height());
    UpdateLayout();

    SetChanged();
    }
    
 
TRect CHuiRosterImpl::Rect() const
	{
	return(iRect);
	}
   
    
void CHuiRosterImpl::ShowL(CHuiControlGroup& aGroup, TInt aWhere)
    {
    
    // The group will automatically accept input events once shown.
       aGroup.SetAcceptInput(ETrue);
    // If the group is already shown on the display, just adjust its position
    // according to the parameters.
    if(Find(&aGroup) != KErrNotFound)
        {
        // It is already shown. Only adjust position.
        TInt newPos = aWhere;
        if(newPos == KHuiRosterShowAtTop)
            {
            newPos = Count() - 1;
            }
        else if(newPos == KHuiRosterShowAtBottom)
            {
            newPos = 0;
            }
        else
            {
            // for PC lint
            }
        Move(&aGroup, newPos);            
        return;
        }

    if(aWhere == KHuiRosterShowAtTop)
        {
        // Put the group on top of the roster.
        AppendL(&aGroup);
        }
    else if(aWhere == KHuiRosterShowAtBottom)
        {
        InsertL(&aGroup, 0);
        }
    else
        {
        InsertL(&aGroup, aWhere);
        }

   

    // Show all the controls of the group.
    for(TInt i = 0; i < aGroup.Count(); ++i)
        {
        aGroup.Control(i).ShowL(*iDisplay);
        }
    }


void CHuiRosterImpl::Hide(CHuiControlGroup& aGroup)
    {
    // Hide all the controls of the group.
    for(TInt i = 0; i < aGroup.Count(); ++i)
        {
        aGroup.Control(i).Hide(*iDisplay);
        }

    Remove(&aGroup);
    }


void CHuiRosterImpl::AppendL(CHuiControlGroup* aGroup)
    {
    ASSERT( aGroup );
    CHuiRosterEntry* entry = new (ELeave) CHuiRosterEntry(*aGroup);
    CleanupStack::PushL(entry);
    User::LeaveIfError(iEntries.Append(entry));
    CleanupStack::Pop(entry);

    if(iDisplay)
        {
        iDisplay->Env().ContinueRefresh();
        }
        
    SetChanged();
    }


void CHuiRosterImpl::InsertL(CHuiControlGroup* aGroup, TInt aPos)
    {
    ASSERT( aGroup );
    if(Find(aGroup) != KErrNotFound)
        {
        User::Leave(KErrAlreadyExists);
        }

    CHuiRosterEntry* entry = new (ELeave) CHuiRosterEntry(*aGroup);
    CleanupStack::PushL(entry);
    User::LeaveIfError(iEntries.Insert(entry, aPos));
    CleanupStack::Pop(entry);
    
    if(iDisplay)
        {
        iDisplay->Env().ContinueRefresh();
        }

    SetChanged();
    }

void CHuiRosterImpl::Move(CHuiVisual* aVisual, TInt aPos)
    {
    CHuiRosterEntry* entry = EntryForVisual(aVisual);
    if (entry)
        {
        Move(&entry->ControlGroup(), aPos);        
        }    
    }

void CHuiRosterImpl::Move(CHuiControlGroup* aGroup, TInt aPos)
    {
    TInt index = Find(aGroup);
    TInt k = 0;
    
    if(index != KErrNotFound && index != aPos)
        {
        CHuiRosterEntry* moving = iEntries[index];
        
        if(aPos > index)
            {
            for(k = index; k < aPos; ++k)
                {
                iEntries[k] = iEntries[k + 1];
                }
            }
        else 
            {
            for(k = index; k > aPos; --k)
                {
                iEntries[k] = iEntries[k - 1];
                }
            }
        iEntries[aPos] = moving;                
        }
        
    if(iDisplay)
        {
        iDisplay->Env().ContinueRefresh();        
        }

    SetChanged();
    }


TInt CHuiRosterImpl::Find(CHuiControlGroup* aGroup) const
    {
    for(TInt i = 0; i < iEntries.Count(); ++i)
        {
        if(&iEntries[i]->ControlGroup() == aGroup)
            {
            return i;
            }
        }
    return KErrNotFound;
    }


void CHuiRosterImpl::Remove(CHuiControlGroup* aGroup)
    {
    TInt index = Find(aGroup);
    if(index != KErrNotFound)
        {
        delete iEntries[index];
        iEntries.Remove(index);

        if(iDisplay)
            {
            iDisplay->Env().ContinueRefresh();
            }

        SetChanged();
        }
    }


TInt CHuiRosterImpl::Count() const
    {
    return iEntries.Count();
    }


CHuiControlGroup& CHuiRosterImpl::ControlGroup(TInt aIndex) const
    {
    return iEntries[aIndex]->ControlGroup();
    }


CHuiControl* CHuiRosterImpl::FindControl(TInt aControlId) const
    {
    CHuiControl* control = 0;
    for(TInt i = 0; i < iEntries.Count(); ++i)
        {
        control = iEntries[i]->ControlGroup().FindControl(aControlId);
        if(control)
            {
            break;
            }
        }
    return control;
    }


CHuiRosterEntry* CHuiRosterImpl::EntryForVisual(const CHuiVisual* aVisual)
    {
    ASSERT( aVisual != NULL );
    CHuiControlGroup* group = aVisual->Owner().ControlGroup();
    TInt index = Find(group);
    if(index == KErrNotFound)
        {
        // The group is not in this roster.
        return NULL;
        }
    return iEntries[index];
    }


void CHuiRosterImpl::ShowVisualL(CHuiVisual* aVisual)
    {
    CHuiRosterEntry* entry = EntryForVisual(aVisual);

    if(!entry)
        {
        User::Leave(KErrNotFound);
        }
    
    if(entry->iRootVisuals.Find(aVisual) != KErrNotFound)
        {
        User::Leave(KErrAlreadyExists);
        }
    
    User::LeaveIfError( entry->iRootVisuals.Append(aVisual) );

    SetVisualToFullArea(aVisual);    
    }


void CHuiRosterImpl::HideVisual(CHuiVisual* aVisual)
    {
    ASSERT( aVisual );
    aVisual->SetDisplay(NULL);
    
    CHuiRosterEntry* entry = EntryForVisual(aVisual);
    if(entry)
        {
        TInt index = entry->iRootVisuals.Find(aVisual);
        if(index != KErrNotFound) // do nothing if visual was not found
            {
            entry->iRootVisuals.Remove(index);
    
            if(iDisplay)
                {
                iDisplay->Env().ContinueRefresh();
                }
            }
        }
    }


void CHuiRosterImpl::SetFocus(CHuiControl& aControl)
    {
    ChangeInputFocus(&aControl);
    }


void CHuiRosterImpl::ClearFocus()
    {
    ChangeInputFocus(NULL);
    }


void CHuiRosterImpl::ChangeInputFocus(CHuiControl* aControl)
    {
    if(aControl)
        {
        // The control must be in one of groups of the roster.
        CHuiControl* control = FindControl(aControl->Id());
        if(control != aControl)
            {
            // Illegal focus.
            return;
            }
        }

    iInputFocus = aControl;
    }

TBool CHuiRosterImpl::PrepareDraw() const
    {
    HUI_PROBE_PROGRAMFLOW_ENTER(MHuiProbe::EProgramFlowPointRefreshPrepareDraw)
    
    TInt i = 0;
    TInt j = 0;

    TInt visualCount = 0;
    TInt entryCount = iEntries.Count();
    // Prepare the visuals tree in the display.
    for(j = 0; j < entryCount; ++j)
        {
        CHuiRosterEntry& entry = *iEntries[j]; 
        
        visualCount = entry.iRootVisuals.Count();
        for(i = 0; i < visualCount; ++i)
            {
            CHuiVisual* visual = entry.iRootVisuals[i];
            //Ignore inactive visuals
            if ( visual->Flags()& EHuiVisualFlagInactive )
            	{
            	continue; 
            	}
            TBool successfullyPrepared = ETrue;
            TRAPD(err, successfullyPrepared = visual->PrepareDrawL());
            if (err != KErrNone)
	            {
	            // Handle the error by calling the visual owner
	            // errorhandling callback
                visual->Owner().VisualPrepareDrawFailed(*visual, err);

                HUI_PROBE_PROGRAMFLOW_EXIT(MHuiProbe::EProgramFlowPointRefreshPrepareDraw)
                return EFalse;
	            }
	        if (!successfullyPrepared) 
		        {
		        // we can stop preparing the rest of the root visuals
		        // if one prepare failed (we won't be drawing 
		        // anything anyway)

		        // also if the prepare failed but it didn't leave,
		        // we assume it was already handled 

                HUI_PROBE_PROGRAMFLOW_EXIT(MHuiProbe::EProgramFlowPointRefreshPrepareDraw)
		        return EFalse;
		        }
            }
        }

    HUI_PROBE_PROGRAMFLOW_EXIT(MHuiProbe::EProgramFlowPointRefreshPrepareDraw)
    return ETrue;
    }


void CHuiRosterImpl::Draw(CHuiGc& aGc) const
    {
    Draw(aGc, NULL);
    }

void CHuiRosterImpl::Draw(CHuiGc& aGc, CHuiDisplay* aDisplay) const
    {
    CHuiDisplay* display = aDisplay ? aDisplay : iDisplay;
    THuiRealRect displayRect = display->VisibleArea();
    TBool refreshCache = ETrue; // TODO: Could this be optimized ?
    TBool opaque = EFalse;  

    // Init projection matrix
    aGc.LoadIdentity(EHuiGcMatrixProjection);

    // Init model matrix 
    aGc.LoadIdentity(EHuiGcMatrixModel);        
    
    // Set up display-specifc window transformations e.g. scaling for tv out        
    aGc.SetWindowTransformation(&(display->WindowTransformation()));
    aGc.UpdateProjection();                            

    TBool didDrawEffect = EFalse;
    if (iEffect)
        {
        //RDebug::Print(_L("CHuiRosterImpl::Draw trying to use effect"));
        TRgb oldColor = aGc.PenColor(); 
        TInt oldAlpha = aGc.PenAlpha(); 

        // Clear
        aGc.SetPenColor(KRgbBlack);
        aGc.SetPenAlpha(0);
        aGc.Disable(CHuiGc::EFeatureBlending);
        aGc.Disable(CHuiGc::EFeatureClipping);
        aGc.Clear();

        // Restore Gc values
        aGc.SetPenColor(oldColor);
        aGc.SetPenAlpha(oldAlpha);
        aGc.Enable(CHuiGc::EFeatureBlending);
        aGc.Enable(CHuiGc::EFeatureClipping);

        // Note: Opacity implementation in effect system  does not really work visually 
        // correct if there are semitransparent child windows. Child window opacity becomes too
        // large, because opacity is applied during visual tree drawing and thus blending child
        // visuals on top of the parent produces wrong transparency level.
        //
        // We workaround this issue here so that we force rendering to cached render
        // target (with opacity 255) and give the real opacity as a parameter to effect
        // drawing so the whole operation gets correct opacity. However there is possibility
        // that complex effect fxml would assume transparency to be handled *before*
        // some other part of the effect, so this workaround won't work correctly in those cases. 
        // To get those working, propably whole opacity effect handling should be re-written 
        // differently.
        if (iEffect->IsSemitransparent() || iEffectOpacity < 1.f)
            {
            iEffect->ForceCachedRenderTargetUsage(ETrue);
            }        
        RRegion dummy;
        didDrawEffect = iEffect->CachedDraw(aGc, displayRect, refreshCache, opaque, dummy, EFalse, iEffectOpacity*255);        
        dummy.Close();
        }
    
    if (!didDrawEffect)
        {
        DrawSelf(aGc, aDisplay);
        }
    else
        {
        // Effect was drawn, OpenGLES/OpenVG may be in different state than what Gc thinks so we restore it.
        aGc.RestoreState();                
        }

    // Undo display-specifc window transformations                          
    aGc.SetWindowTransformation(NULL);
    aGc.UpdateProjection();            

    // Undo changes to model matrix
    aGc.LoadIdentity(EHuiGcMatrixModel);    
    }

void CHuiRosterImpl::DrawSelf(CHuiGc& aGc, CHuiDisplay* aDisplay) const
    {
    HUI_PROBE_PROGRAMFLOW_ENTER(MHuiProbe::EProgramFlowPointRefreshDraw)

    TInt i = 0;
    TInt j = 0;

    CHuiDisplay* display = aDisplay ? aDisplay : iDisplay;
    
    ASSERT(display!=NULL);

    if (IsVisibleContentFrozen())
        {
        DrawSelfFrozen(aGc, display);
        return;
        }
    
    
    TInt visualCount = 0;
    TInt entryCount = iEntries.Count();
    // Draw the visuals tree in the display.
    for(j = 0; j < entryCount; ++j)
        {
        CHuiRosterEntry& entry = *iEntries[j]; 
        CHuiControlGroup& group = entry.ControlGroup();
        CHuiControl& control = group.Control(0);
        
        // skip all  alf client session drawing
        if(control.Role() == EHuiSessionContainer )
            {
            continue;
            }
        
        // Init model matrix always for every group
        //aGc.LoadIdentity(EHuiGcMatrixModel);        
        aGc.Push(EHuiGcMatrixModel);
        
        // Set up display-specifc transformations i.e. camera transformations    	
        display->Transformation().Execute(EHuiGcMatrixModel, aGc);
        
        // Set up a group-specific transformation.
        if(group.IsTransformed())
            {
   	        group.Transformation().Execute(EHuiGcMatrixModel, aGc);
            }
        
        // Draw visuals
        visualCount = entry.iRootVisuals.Count();
        for(i = 0; i < visualCount; ++i)
            {
            CHuiVisual* visual = entry.iRootVisuals[i];
            //Ignore inactive visuals
            if ( visual->Flags()& EHuiVisualFlagInactive || visual->LoadingEffect() )
            	{
            	continue; 
            	}
            visual->Draw(aGc);
            }       

        if( display->DrawVisualOutline() != CHuiDisplay::EDrawVisualOutlineNone )
            {        	
            for(i = 0; i < visualCount; ++i)
                {	
                // Draw Visual Outline depending on central repository setting
                // and visual flag value
                const TBool drawOutline =
                    ( entry.iRootVisuals[i]->Flags() & EHuiVisualFlagEnableDebugMode ) ||  
                    ( display->DrawVisualOutline() == CHuiDisplay::EDrawVisualOutlineAllVisuals );
                
                DrawBoundaries( aGc, entry.iRootVisuals[i], drawOutline );        
                }
            }
        
        aGc.Pop(EHuiGcMatrixModel);        
        }
    
    HUI_PROBE_PROGRAMFLOW_EXIT(MHuiProbe::EProgramFlowPointRefreshDraw)
    }

void CHuiRosterImpl::DrawSelfFrozen(CHuiGc& aGc, CHuiDisplay* aDisplay) const
    {
    if (iCanvasGc && iCanvasRenderBuffer)
        {
        iCanvasGc->SetGc(aGc);
        iCanvasGc->SetDefaults();
        iCanvasGc->PushTransformationMatrix();
                
        // Calculate rotation as relative to the current display rotation.
        CHuiGc::TOrientation relativeOrientation = CHuiGc::EOrientationNormal;    
        CHuiGc::TOrientation displayOrientation = aDisplay->Orientation(); 
        CHuiGc::TOrientation renderBufferOrientation = iCanvasRenderBufferOrientation; 
        
        TInt rotationAngle[] =
            {
            0, 
            270, 
            90, 
            180
            };
            
        TInt displayOrientationAngle = rotationAngle[displayOrientation];
        TInt renderBufferOrientationAngle = rotationAngle[renderBufferOrientation];    
        TInt relativeOrientationAngle = renderBufferOrientationAngle - displayOrientationAngle;
        
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
        
        TRect displayArea = aDisplay->VisibleArea();
        TInt w = displayArea.Width();
        TInt h = displayArea.Height();

       
        // ...select right rotation...
        if (relativeOrientation == CHuiGc::EOrientationCW90)
            {
            // Rotate around origo and move back to displayarea
            iCanvasGc->Rotate(-90, 0, 0, 1.f);
            iCanvasGc->Translate(-h, 0, 0);
            }
        else if (relativeOrientation == CHuiGc::EOrientationCCW90)
            {
            // Rotate around origo and move back to displayarea
            iCanvasGc->Rotate(90, 0, 0, 1.f);
            iCanvasGc->Translate(0, -w, 0);
            }
        else if (relativeOrientation == CHuiGc::EOrientation180)
            {
            // Rotate around origo and move back to displayarea
            iCanvasGc->Rotate(180, 0, 0, 1.f);
            iCanvasGc->Translate(-w, -h, 0);            
            }
        else
            {
            // Nothing
            }            

        // Pixels are upside down in OpenVG canvas render buffer
        if (CHuiStatic::Env().Renderer() == EHuiRendererVg10)
            {
            TInt height = h;
            if (relativeOrientation == CHuiGc::EOrientationCW90 || relativeOrientation == CHuiGc::EOrientationCCW90)
                {
                height = w;    
                }            
            iCanvasGc->Translate(0.0f, height, 0.0f);            
            iCanvasGc->Scale(1.f, -1.f, 1.f);
            }
        
        iCanvasGc->DrawImage(*iCanvasRenderBuffer, TPoint(0,0));        
        iCanvasGc->PopTransformationMatrix();
        }
    }

void CHuiRosterImpl::SetAlfEventWindow(CHuiVisual* aVisual)
    {
    iAlfEventWindowVisual = aVisual;
    }

const CHuiVisual* CHuiRosterImpl::AlfEventWindow()
    {
    return iAlfEventWindowVisual;
    }

TBool CHuiRosterImpl::NativeAppsContentChanged()
    {
    // if we have multiple frames where alf content was changed, skip every other frame
	// this should be only used if whole alf content has effect (is faded)
    TBool alfContentChanged = iDisplay->AlfContentChanged();
    if(!iPreviousAlfContentDrawn && alfContentChanged)
        {
        iPreviousAlfContentDrawn = ETrue;
        }
    else
        {
        iPreviousAlfContentDrawn = EFalse;
        }
    return iPreviousAlfContentDrawn;
    }


void CHuiRosterImpl::DrawNativeAppsContent(CHuiGc &aGc, CHuiDisplay* aDisplay) const
    {
    TInt i = 0;
    TInt j = 0;

    CHuiDisplay* display = aDisplay ? aDisplay : iDisplay;
    
    ASSERT(display!=NULL);

    TBool clearDone = EFalse;
    
    TInt visualCount = 0;
    TInt entryCount = iEntries.Count();
    // Draw the visuals tree in the display.
    for(j = 0; j < entryCount; ++j)
        {
        CHuiRosterEntry& entry = *iEntries[j]; 
        CHuiControlGroup& group = entry.ControlGroup();
        CHuiControl& control = group.Control(0);
        // skip all but alf client session drawing
        if(control.Role() != EHuiSessionContainer )
            {
            continue;
            }
        
        // skip inactive
        CHuiLayout* hostContainer = control.ContainerLayout( NULL );
        if(hostContainer->Flags() & EHuiVisualFlagInactive )
            {
            continue;
            }
        
        // Init model matrix always for every group
        //aGc.LoadIdentity(EHuiGcMatrixModel);        
        aGc.Push(EHuiGcMatrixModel);
        
        // Set up display-specifc transformations i.e. camera transformations       
        display->Transformation().Execute(EHuiGcMatrixModel, aGc);
        
        // Set up a group-specific transformation.
        if(group.IsTransformed())
            {
            group.Transformation().Execute(EHuiGcMatrixModel, aGc);
            }
        
        // Draw visuals
        visualCount = entry.iRootVisuals.Count();
        for(i = 0; i < visualCount; ++i)
            {
            CHuiVisual* visual = entry.iRootVisuals[i];
            //Ignore inactive visuals
            if ( visual->Flags()& EHuiVisualFlagInactive || visual->LoadingEffect() )
                {
                continue; 
                }
            
            if(!clearDone)
                {
                // also check if display size change is pending or not
                if(display->QueryAndResetSkinSizeChangePendingStatus())
                    {
                    display->Env().Skin().NotifyDisplaySizeChangedL();
                    }
            
                // only do clear if we really draw some alf native content
                display->DoBackgroundClear();
                clearDone = ETrue;
                }
            
            visual->Draw(aGc);
            
            }       

        if( display->DrawVisualOutline() != CHuiDisplay::EDrawVisualOutlineNone )
            {           
            for(i = 0; i < visualCount; ++i)
                {   
                // Draw Visual Outline depending on central repository setting
                // and visual flag value
                const TBool drawOutline =
                    ( entry.iRootVisuals[i]->Flags() & EHuiVisualFlagEnableDebugMode ) ||  
                    ( display->DrawVisualOutline() == CHuiDisplay::EDrawVisualOutlineAllVisuals );
                
                DrawBoundaries( aGc, entry.iRootVisuals[i], drawOutline );        
                }
            }
        
        aGc.Pop(EHuiGcMatrixModel);        
        }
    }




void CHuiRosterImpl::DrawBoundaries( CHuiGc& aGc, CHuiVisual* aVisual, TBool aDrawOutline ) const
	{
    if( aDrawOutline )
        {
        const THuiRealRect content = aVisual->DisplayRect();
        
        if (aVisual->Flags()& EHuiVisualFlagInactive)
            {
            aGc.SetPenColor( KRgbDarkGray );           
            }
        else
            {
            aGc.SetPenColor( TUint32(aVisual)&0x00ffffff/*KRgbBlack*/ );  // Use "random" color
            }
        aGc.DrawBorders(content.Round(), 
            KHuiVisualOutlineWidth, KHuiVisualOutlineWidth,
            KHuiVisualOutlineWidth, KHuiVisualOutlineWidth,
            CHuiGc::EBorderFixedCorners);
        }
        
    for( TInt k = 0; k < aVisual->Count(); ++k )
        {
        DrawBoundaries( 
            aGc, 
            &aVisual->Visual( k ), 
            ( (  aVisual->Visual( k ).Flags() & EHuiVisualFlagEnableDebugMode ) || 
                aDrawOutline ) );
        }
    }


TBool CHuiRosterImpl::HasTransformedControlGroups() const
    {
    TInt j = 0;
    for(j = 0; j < iEntries.Count(); ++j)
        {
        CHuiRosterEntry& entry = *iEntries[j]; 
        CHuiControlGroup& group = entry.ControlGroup();
        if(group.IsTransformed())
            {
            return ETrue;    
            }
        }
    return EFalse;    
    }


TBool CHuiRosterImpl::HandleEventL(const THuiEvent& aEvent)
    {
    TInt i = 0;
    TInt k = 0;

    if(iDisplay)
        {
        iDisplay->Env().ContinueRefresh();
        }

    if(aEvent.IsPointerEvent())
        {
        iTactileFeedbackHandled = EFalse;
        
        // Put event details array up to date
        CleanEventDetails();
        
        if ( !iLongTapDetector )
            {
            iLongTapDetector = CHuiLongTapDetector::NewL( *this );
            }
        iLongTapDetector->OfferPointerEvent( aEvent );
        
        if ( aEvent.PointerDown() )
        	{
        	iInitialDownEvent = aEvent;
        	CleanDraggingFlags();
        	}
		
        
        // Restricted distribution?
        if(aEvent.iPointerEvent.iType == TPointerEvent::EDrag)
            {
            // Only send these if there are observers.
            for(i = 0; i < iPointerDragObservers.Count(); ++i)
                {
                if( DragEventOutOfRangeL( &iPointerDragObservers[i], aEvent) )
                	{
	                if(iPointerDragObservers[i].OfferEventL(aEvent))
	                    {
	                    return ETrue;
	                    }                                    	
                	}
                }
            return EFalse;
            }
            
        // The event is generated by the long tap detector
        if(aEvent.iPointerEvent.iType == TPointerEvent::EButtonRepeat)
            {
            // Only send these if there are observers.
            for(i = 0; i < iLongTapObservers.Count(); ++i)
                {
                if(IsLongTapEventAllowed(&iLongTapObservers[i]))
                	{
	                if(iLongTapObservers[i].OfferEventL(aEvent))
	                    {
	                    return ETrue;
	                    }                	
                	}
                }
            return EFalse;
            }  
        
        // The other pointer events are offered depending on the visuals.
        for(i = iEntries.Count() - 1; i >= 0; --i)
            {
            CHuiRosterEntry& entry = *iEntries[i];
            CHuiControlGroup& group = entry.ControlGroup();
            
            if(!group.AcceptInput())
                {
                // The group will not be receiving input events.
                continue;
                }
           
            for(k = entry.iRootVisuals.Count() - 1; k >= 0; --k)
                {
                if(OfferPointerEventWithinTreeL(*entry.iRootVisuals[k], aEvent))
                    {
                    return ETrue;
                    }
                }    
                
            // Offer to the fallback handlers of the group.
            for(k = 0; k < group.iUnhandledEventObservers.Count(); ++k)
                {
                if(group.iUnhandledEventObservers[k].OfferEventL(aEvent))
                    {
                    return ETrue;
                    }
                }
            }
            
        // No one handled the event.
        for(i = 0; i < iUnhandledPointerObservers.Count(); ++i)
            {
            if(iUnhandledPointerObservers[i].OfferEventL(aEvent))
                {
                return ETrue;
                }
            }
        }
    else 
        {
        // First offer to the input control.
        if(aEvent.IsKeyEvent() && iInputFocus && iInputFocus->AcceptInput())
            {
            if(iInputFocus->OfferEventL(aEvent))
                {
                return ETrue;
                }
            }

        // Iterate through the groups and controls in priority order.
        for(i = iEntries.Count() - 1; i >= 0; --i)
            {
            CHuiControlGroup& group = iEntries[i]->ControlGroup();
            
            if(!group.AcceptInput())
                {
                // The group will not be receiving input events.
                continue;
                }
            
            for(k = group.Count() - 1; k >= 0; --k)
                {
                CHuiControl& control = group.Control(k);
                if(control.OfferEventL(aEvent))
                    {
                    // The event was consumed, now it can't be handled by
                    // anyone else.
                    return ETrue;
                    }
                }
            // Notify observer that group didn't handle event?
            }
        }

    return EFalse;
    }
    
    
TBool CHuiRosterImpl::OfferPointerEventWithinTreeL(CHuiVisual& aVisual, const THuiEvent& aEvent, TBool aUseDispRect)
    {
    //Ignore inactive visuals
    if( ( aVisual.Flags() & EHuiVisualFlagIgnorePointer ) || 
    		( aVisual.Flags() & EHuiVisualFlagInactive ) )
        {
        // Discard the entire branch of visuals.
        return EFalse;
        }

    const TPoint& pos = aEvent.iPointerEvent.iPosition;
    
    TBool useDispRect = EFalse;
    // If it has children, offer to those first.
    for(TInt i = aVisual.Count() - 1; i >= 0; --i)
        {
        // set useDispRect, if scroll is enabled on a layout
        useDispRect = static_cast<CHuiLayout&>(aVisual).Scrolling();
        
        if(OfferPointerEventWithinTreeL(aVisual.Visual(i), aEvent, useDispRect))
            {
            return ETrue;
            }
        }
            
    TRect visualRect;
    if(aUseDispRect)
        {
        visualRect = TRect(aVisual.DisplayRect());
        }
    else
        {
        // use cashed display rect
        visualRect = TRect(aVisual.CachedDisplayRect());
        }
    
    // Offer to the visual itself.
    if(visualRect.Contains(pos))
        {
        HUI_DEBUG3(_L("CHuiRosterImpl::OfferPointerEventWithinTreeL: aVisual=%x pos=%i,%i"), 
            &aVisual, pos.iX, pos.iY);

        MHuiEventHandler* handler = aVisual.Owner().EventHandler();
        if(handler)
            {
            THuiEvent eventWithVisual = aEvent;
            eventWithVisual.SetVisual(&aVisual);
            HandleTactileFeedback( eventWithVisual );
            if(handler->OfferEventL(eventWithVisual))
                {
                return ETrue;
                }
            }
        }
        
    // Not consumed.
    return EFalse;        
    }
    
    
TBool CHuiRosterImpl::Changed() const
    {
    if(iChanged)
        {
        return ETrue;
        }
    
    TInt entryCount = iEntries.Count();
    for(TInt i = 0; i < entryCount; ++i)
        {
        if(iEntries[i]->ControlGroup().Changed())
            {
            return ETrue;
            }
        }

    if (iEffect && iEffect->Changed())
        {
        return ETrue;
        }
                        
    return EFalse;        
    }
    
    
void CHuiRosterImpl::ScanDirty()
    {
    if(Changed())
        {
        // If the entire roster has changed, there is no need to query 
        // individual visuals.
        if(iDisplay)
            {
            iDisplay->AddDirtyRegion(iRect);
            iDisplay->SetAlfContentChanged(ETrue);
            }
        SetChanged(EFalse);
        return;
        }
    
    if (iDisplay && !iDisplay->IsDirtyScanNeeded())
        {
        // Dirty scan is not needed, we can return directly.
        return;    
        }
    
    TInt entryCount = iEntries.Count();
    for(TInt j = 0; j < entryCount; ++j)
        {
        CHuiRosterEntry& entry = *iEntries[j]; 
        TInt count = entry.iRootVisuals.Count();
        for(TInt i = 0; i < count; ++i)
            {
            //Ignore inactive visuals
            if ( entry.iRootVisuals[i]->Flags()& EHuiVisualFlagInactive )
           		{
           		continue; 
           		}
            
             CHuiControlGroup& group = entry.ControlGroup();
             CHuiControl& control = group.Control(0);
             
             if(control.Role() == EHuiSessionContainer )
                 {
                 if (iDisplay)
                     {
                     iDisplay->ScanningAlfContent(ETrue);
                     }
                 
                 }
             
            entry.iRootVisuals[i]->ReportChanged();
            
            if(control.Role() == EHuiSessionContainer )
                {
                if(iDisplay)
                    {
                    iDisplay->ScanningAlfContent(EFalse);
                    }
                }
            }
        }
    }


// This ScanDirty-method supports transformed dirty rects
void CHuiRosterImpl::ScanTransformedDirty(CHuiGc& aGc, CHuiDisplay* aDisplay)
    {
    CHuiDisplay* display = aDisplay ? aDisplay : iDisplay;

    // Init projection matrix
    aGc.LoadIdentity(EHuiGcMatrixProjection);

    // Set up display-specifc window transformations e.g. scaling for tv out    	
    aGc.SetWindowTransformation(&(display->WindowTransformation()));
    aGc.UpdateProjection();                            

    if(Changed()) 
        {
        // If the entire roster has changed, there is no need to query 
        // individual visuals.
        if(display)
            {
            display->AddDirtyRegion(iRect);
            }
        
        SetChanged(EFalse);
        }
    
    for(TInt j = 0; j < iEntries.Count(); ++j)
    	{
        if (display && !display->IsDirtyScanNeeded())
        	{
            break;    
            }

        CHuiRosterEntry& entry = *iEntries[j]; 
        CHuiControlGroup& group = entry.ControlGroup();

        // Init model matrix always for every group
        aGc.LoadIdentity(EHuiGcMatrixModel);        
            
        // Set up display-specifc transformations i.e. camera transformations    	
        __ASSERT_DEBUG(display, USER_INVARIANT());
        display->Transformation().Execute(EHuiGcMatrixModel, aGc);
            
        // Set up a group-specific transformation.
        if(group.IsTransformed())
            {
            group.Transformation().Execute(EHuiGcMatrixModel, aGc);
            }

        for(TInt i = 0; i < entry.iRootVisuals.Count(); ++i)
            {
            if (display && !display->IsDirtyScanNeeded())
                {
                break;    
                }
            //Ignore inactive visuals     
            if ( entry.iRootVisuals[i]->Flags()& EHuiVisualFlagInactive )
           		{
                continue;
           		}
            entry.iRootVisuals[i]->ReportChanged();
            }
        }

    // Undo display-specifc window transformations                          
    aGc.SetWindowTransformation(NULL);
    aGc.UpdateProjection();            

    // Undo changes to model matrix
    aGc.LoadIdentity(EHuiGcMatrixModel);
    }


void CHuiRosterImpl::MoveVisualToFront(CHuiVisual* aRootVisual)
    {
    ASSERT( aRootVisual );
    MoveVisualToFront(*aRootVisual);
    }


void CHuiRosterImpl::MoveVisualToFront(const CHuiVisual& aRootVisual)
    {
    CHuiRosterEntry* entry = EntryForVisual(&aRootVisual);
    __ASSERT_ALWAYS( entry != NULL, THuiPanic::Panic(THuiPanic::ERosterGroupNotFound) );
    TInt index = entry->iRootVisuals.Find(&aRootVisual);
    if(index == KErrNotFound)
        {
        /** @todo Panic? */
        return;
        }
        
    entry->iRootVisuals.Remove(index);
    entry->iRootVisuals.Append(&aRootVisual);        
    
    UpdateLayout();
    }
    
     
void CHuiRosterImpl::UpdateLayout()
    {
    SetChanged();
    
    TInt visualCount = 0;
    TInt entryCount = iEntries.Count();
    for(TInt i = 0; i < entryCount; ++i)
        {
        visualCount = iEntries[i]->iRootVisuals.Count();
        for(TInt j = 0; j < visualCount; ++j)
            {
            //Ignore inactive visuals
            if ( iEntries[i]->iRootVisuals[j]->Flags()& EHuiVisualFlagInactive )
            	{
            	continue;
            	}
            SetVisualToFullArea(iEntries[i]->iRootVisuals[j]);
            }
        }
    }    


void CHuiRosterImpl::SetVisualToFullArea(CHuiVisual* aVisual)
    {
    ASSERT( aVisual );
    
    TInt globalTransitionTime = CHuiStatic::LayoutTransitionTime();
    
    // Root visuals are automatically full size.
    /** @todo  Needs a better way to set the visual's display. */
    if(iDisplay)
        {
        aVisual->SetDisplay(iDisplay);
        iDisplay->Env().ContinueRefresh();
        }
    
    if(!(aVisual->Flags() & EHuiVisualFlagManualPosition))
        {
        aVisual->SetPos(iRect.iTl, globalTransitionTime);
        }

    if(!(aVisual->Flags() & EHuiVisualFlagManualSize))
        {
        aVisual->SetSize(iRect.Size(), globalTransitionTime);
        }
    else
        {
        aVisual->UpdateChildrenLayout(globalTransitionTime);
        }
        
    aVisual->ReportLayoutUpdate();        
    }


void CHuiRosterImpl::NotifySkinChangedL()
    {
    TInt visualCount = 0;
    TInt entryCount = iEntries.Count();
    for(TInt j = 0; j < entryCount; ++j)
        {
        CHuiRosterEntry& entry = *iEntries[j];
        visualCount = entry.iRootVisuals.Count();
        for(TInt i = 0; i < visualCount; ++i)
            {
            //Ignore inactive visuals
            if ( entry.iRootVisuals[i]->Flags()& EHuiVisualFlagInactive )
            	{
            	continue;	
            	}
            entry.iRootVisuals[i]->NotifySkinChangedL();
            }
        }
    }


void CHuiRosterImpl::DumpRootLevelTrees() const
    {
    TInt visualCount = 0;
    TInt entryCount = iEntries.Count();
    for(TInt j = 0; j < entryCount; ++j)
        {
        CHuiRosterEntry& entry = *iEntries[j];
        visualCount = entry.iRootVisuals.Count();
        for(TInt i = 0; i < visualCount; ++i)
            {
            CHuiVisual* visual = entry.iRootVisuals[i];
            visual->DumpTree();
            }
        }
    }


void CHuiRosterImpl::ClearChanged()
    {
    HUI_DEBUGF(_L("CHuiRosterImpl::ClearChanged() - Clearing all change flags."));
    
    TInt visualCount = 0;
    TInt entryCount = iEntries.Count();
    for(TInt j = 0; j < entryCount; ++j)
        {
        CHuiRosterEntry& entry = *iEntries[j];
        visualCount = entry.iRootVisuals.Count();
        for(TInt i = 0; i < visualCount; ++i)
            {
            // Ignore inactive visuals
            if ( !(entry.iRootVisuals[i]->Flags()& EHuiVisualFlagInactive) )
                {
                // This will clear the change flags of the entire tree of visuals.
                entry.iRootVisuals[i]->ClearChanged(); 
                }            
            }
        }
    }


void CHuiRosterImpl::SetChanged(TBool aIsChanged)
    {
    if(aIsChanged && !iChanged)
        {
        HUI_DEBUG(_L("CHuiRosterImpl::SetChanged() - Roster flagged as changed."));
        }
    else if(!aIsChanged && iChanged)
        {
        HUI_DEBUG(_L("CHuiRosterImpl::SetChanged() - Roster change flag cleared."));
        }
    else
        {
        // for PC lint
        }
        
    iChanged = aIsChanged;
    }


RHuiObserverArray<MHuiEventHandler>& CHuiRosterImpl::Observers(THuiInputType aInputType)
    {
    switch(aInputType)
        {
        case EHuiInputPointerDrag:
            return iPointerDragObservers;
            
        case EHuiInputPointerLongTap:
            return iLongTapObservers;

        case EHuiInputPointerUnhandled:
            return iUnhandledPointerObservers;
            
        default:
            HUI_PANIC(THuiPanic::ERosterUnknownInputObserverType)
            return iPointerDragObservers; // never reached
        }
    }
    
void CHuiRosterImpl::HandleTactileFeedback( const THuiEvent& aEvent )
    {
    // If tactile feedback has been already deciede for this event....
    if ( iTactileFeedbackHandled )
        {
        return;
        }
    
    // The tactile feedback is defined on visual basis
    if ( !aEvent.Visual() )
        {
        // no visual, no feedback
        return;
        }
        
    // Only pointer events
    if ( !aEvent.IsPointerEvent() )
        {
        return;
        }
        
    // Currently support only pointer down events
    if ( !aEvent.PointerDown() )
        {
        return;
        }
        
    const TInt eventType = 0; // = ETouchEventStylusDown
    
    // Check if the viusal has a tactile feedback specified
    const TInt feedbackType = aEvent.Visual()->TactileFeedback( eventType );
    
    if ( feedbackType != KErrNotFound )
        {
        iTactileFeedbackHandled = ETrue; // Handling it now.
        
#ifdef RD_TACTILE_FEEDBACK
        MTouchFeedback* feedback = MTouchFeedback::Instance();
        if ( feedback )
            {
            feedback->InstantFeedback( (TTouchLogicalFeedback)feedbackType );
            }   
#else

#ifdef _DEBUG
        // for RnD
        switch( feedbackType )
            {
            case 0: // none
                User::InfoPrint( _L("None") );
                break;
            case 1: // basic
                User::InfoPrint( _L("Basic") );
                break;
            case 2: // sensitive
                User::InfoPrint( _L("Sensitive") );
                break;
            default:
                break;
            }
#endif // _DEBUG

#endif
        }
    }

// Adds new detail entry to iPointerEventDetails
void CHuiRosterImpl::AddEventDetailsL(MHuiEventHandler* aEventHandler, const THuiXYMetric& aXYMetric, TBool aDisable)
	{
	THuiPointerEventDetails* newDetails = new (ELeave) THuiPointerEventDetails;
	newDetails->iEventHandler = aEventHandler;
	newDetails->iXYMetric = aXYMetric;
	newDetails->iDisableLongTapWhenDragging = aDisable;
	newDetails->iDragging = EFalse;	
	TInt err = iPointerEventDetails.Append(newDetails);
	if ( err != KErrNone )
	    {
	    delete newDetails;
	    User::Leave( err );
	    }
	}
	
// Cleans entries from iPointerEventDetails if there isn't observer for 
// that event handler
void CHuiRosterImpl::CleanEventDetails()
	{
	TInt i = 0;
	
	while(i < iPointerEventDetails.Count())
		{
		TBool used = EFalse;
		
		// Check in drag observers
		for(TInt j = 0; j < iPointerDragObservers.Count(); ++j)
			{
			if(iPointerEventDetails[i]->iEventHandler == &iPointerDragObservers[j])
				{
				used = ETrue;
				break;
				}
			}

		// Check in long tap observers
		if(!used) 
			{
			for(TInt j = 0; j < iLongTapObservers.Count(); ++j)
				{
				if(iPointerEventDetails[i]->iEventHandler == &iLongTapObservers[j])
					{
					used = ETrue;
					break;
					}
				}			
			}
			
		// Check in unhandled observers
		if(!used) 
			{
			for(TInt j = 0; j < iUnhandledPointerObservers.Count(); ++j)
				{
				if(iPointerEventDetails[i]->iEventHandler == &iUnhandledPointerObservers[j])
					{
					used = ETrue;
					break;
					}
				}
			}
		
		// If event handler isn't found in observers, we remove details for that
		// event handler from iPointerEventDetails
		if(!used)
			{
			delete iPointerEventDetails[i];
			iPointerEventDetails.Remove(i);
			}
		else
			{
			i++;
			}
		}	
	}

// Cleans dragging flags in iPointerEventDetails
void CHuiRosterImpl::CleanDraggingFlags()
	{
	for(TInt i = 0; i < iPointerEventDetails.Count(); ++i)
		{
		iPointerEventDetails[i]->iDragging = EFalse;
		}
	}

// Sets treshold which pointer needs to be moved before starting to send drag events.
void CHuiRosterImpl::SetPointerDragThresholdL(const MHuiEventHandler& aEventHandler, const THuiXYMetric& aXYMetric)
	{		
	MHuiEventHandler *handler = (MHuiEventHandler*)&aEventHandler;

	// Check if there is already detail entry in iPointerEventDetails where to update
	// details
	for(TInt i = 0; i < iPointerEventDetails.Count(); ++i)
		{
		if(iPointerEventDetails[i]->iEventHandler == handler)
			{
			iPointerEventDetails[i]->iXYMetric = aXYMetric;
			return;
			}					
		}
	
	// If event handler wasn't found, we add new detail entry
	AddEventDetailsL(handler, aXYMetric, EFalse);
	}
	
// Disables long tap events when dragging is going on. 
void  CHuiRosterImpl::DisableLongTapEventsWhenDraggingL(const MHuiEventHandler& aEventHandler, TBool aDisable)
	{
	MHuiEventHandler* handler = const_cast<MHuiEventHandler*>( &aEventHandler );
	
	// Check if there is already detail entry in iPointerEventDetails where to update
	// details
	for(TInt i = 0; i < iPointerEventDetails.Count(); ++i)
		{
		if(iPointerEventDetails[i]->iEventHandler == handler)
			{
			iPointerEventDetails[i]->iDisableLongTapWhenDragging = aDisable;
			return;
			}
		}	

	// If event handler wasn't found, we add new detail entry
	AddEventDetailsL(handler, THuiXYMetric(KDefaultDragTresholdInPixelsX, KDefaultDragTresholdInPixelsY), aDisable);
	}

// Checks if event handler is within threshold range.
TBool CHuiRosterImpl::DragEventOutOfRangeL(MHuiEventHandler* aEventHandler, const THuiEvent& aEvent)
	{	
 	if ( aEvent.IsPointerEvent() && iInitialDownEvent.IsPointerEvent())
        {
		TBool handlerFound = EFalse;
		
		// Find event handler from detail array
		for(TInt i = 0; i < iPointerEventDetails.Count(); ++i)
			{
			if(iPointerEventDetails[i]->iEventHandler == aEventHandler)
				{
				
				// If we are already sending dragging events to this handler
				// we can return immediately
				if(iPointerEventDetails[i]->iDragging) 
					{
					return ETrue;
					}
					
				handlerFound = ETrue;
				
				// calculate moved distance in pixels
				TPoint result = iInitialDownEvent.iPointerEvent.iPosition-aEvent.iPointerEvent.iPosition;
								
				// Do metric calculations
			    THuiRealPoint tresholdInPx(iPointerEventDetails[i]->iXYMetric.iX.iMagnitude, iPointerEventDetails[i]->iXYMetric.iY.iMagnitude);
			    if(iPointerEventDetails[i]->iXYMetric.iX.iUnit != EHuiUnitPixel) 
			    	{
			    	TReal32 reference = 0;
			    	if (iPointerEventDetails[i]->iXYMetric.iX.iUnit == EHuiUnitRelativeToDisplay && iDisplay ) 
			    		{
			    		reference = iDisplay->VisibleArea().Width();
			    		}
			    	if (iPointerEventDetails[i]->iXYMetric.iX.iUnit == EHuiUnitS60  && iDisplay ) 
			    		{
			    		reference = iDisplay->UnitValue();
			    		}			    		
			    	tresholdInPx.iX = tresholdInPx.iX * reference;
			    	}
			    if(iPointerEventDetails[i]->iXYMetric.iY.iUnit != EHuiUnitPixel) 
			    	{
			    	TReal32 reference = 0;
			    	if (iPointerEventDetails[i]->iXYMetric.iY.iUnit == EHuiUnitRelativeToDisplay && iDisplay ) 
			    		{
			    		reference = iDisplay->VisibleArea().Width();
			    		}
			    	if (iPointerEventDetails[i]->iXYMetric.iY.iUnit == EHuiUnitS60  && iDisplay ) 
			    		{
			    		reference = iDisplay->UnitValue();
			    		}			    		
			    	tresholdInPx.iY = tresholdInPx.iY * reference;
			    	}
			    
			    // Check if we are outside of the threshold
				if (result.iX < -tresholdInPx.iX || result.iX > tresholdInPx.iX||
					result.iY < -tresholdInPx.iY ||	result.iY > tresholdInPx.iY	) 
					{
					iPointerEventDetails[i]->iDragging = ETrue;					
					return ETrue;						
					}
				else
					{
					iPointerEventDetails[i]->iDragging = EFalse;
					}
				}
			}
			
		// If no event handler found in iPointerEventDetails array,
		// check with default threshold.
		if(!handlerFound) 
			{
			TPoint result = iInitialDownEvent.iPointerEvent.iPosition-aEvent.iPointerEvent.iPosition;
								
			if (result.iX < -KDefaultDragTresholdInPixelsX || result.iX > KDefaultDragTresholdInPixelsX||
				result.iY < -KDefaultDragTresholdInPixelsY || result.iY > KDefaultDragTresholdInPixelsY) 
				{
				// We need to add new event detail to store dragging flag
				AddEventDetailsL(aEventHandler, THuiXYMetric(KDefaultDragTresholdInPixelsX, KDefaultDragTresholdInPixelsY), EFalse);
				return DragEventOutOfRangeL(aEventHandler, aEvent);
				}			
			}		
        }	
	return EFalse;
	}

// Checks if long tap event is allowed to send.
TBool CHuiRosterImpl::IsLongTapEventAllowed(MHuiEventHandler* aEventHandler)
	{
	for(TInt i = 0; i < iPointerEventDetails.Count(); ++i)
		{
		if(iPointerEventDetails[i]->iEventHandler == aEventHandler)
			{
			if ( iPointerEventDetails[i]->iDragging ) 
				{
				return !(iPointerEventDetails[i]->iDisableLongTapWhenDragging);				
				}
			}	
        }	
	return ETrue;
	}


void CHuiRosterImpl::SetDrawingExternalContent(TBool aEnable)
	{
	iDrawEvenIfDisabledDrawInRoster = aEnable;
	}

	
TBool CHuiRosterImpl::IsDrawingExternalContent() const
	{
	return iDrawEvenIfDisabledDrawInRoster;
	}

void CHuiRosterImpl::RemoveExternalContentVisualFromParentL(CHuiVisual* aExternalContentVisual)
    {
    // External content is only used with canvas visuals, and the layouts of canvas visuals are always in the roster.
    // Browse all layouts in the roster and check if they have a canvas visual as a child visual.
    // Note: this can be slow operation, so it should only be done rarely.
   
    TInt visualCount = 0;
    TInt entryCount = iEntries.Count();    
    for(TInt j = 0; j < entryCount; ++j)
    	{
    	CHuiRosterEntry& entry = *iEntries[j]; // control group
    	visualCount = entry.iRootVisuals.Count();
        for(TInt i = 0; i < visualCount; ++i)
        	{
        	CHuiLayout* layout = dynamic_cast<CHuiLayout*>(entry.iRootVisuals[i]); // layout
        	
        	if (layout)
        		{
        		RemoveExternalContentVisualFromParentL(aExternalContentVisual,layout);
        		}
        	} 	
    	}
    }

void CHuiRosterImpl::RemoveExternalContentVisualFromParentL(CHuiVisual* aExternalContentVisual, CHuiLayout* aLayout)
	{
	for(TInt k = 0; aLayout && k < aLayout->Count(); ++k)
		{
		CHuiCanvasVisual* canvasVisual = dynamic_cast<CHuiCanvasVisual*>(&(aLayout->Visual(k)));

		if (canvasVisual)
			{
			if (canvasVisual->ExternalContent() == aExternalContentVisual)
				{
				// Remove aExternalContentVisual from the parent visual
				canvasVisual->SetExternalContentL(NULL);
				}
			}
		else
			{
			CHuiLayout* layoutVisual = dynamic_cast<CHuiLayout*>(&(aLayout->Visual(k)));
			if ( layoutVisual)
				{
				RemoveExternalContentVisualFromParentL(aExternalContentVisual,layoutVisual);
				}
			}
		}
	}


void CHuiRosterImpl::FreezeVisibleContentL(TBool aEnable)
    {
    iRosterIsFrozen = aEnable;
    
    if (aEnable && !iMonitorMemoryLevel && iDisplay)
        {
        iDisplay->Env().AddMemoryLevelObserver(this);
        iMonitorMemoryLevel = ETrue;
        }
    
    if (aEnable)
        {    
        if (UseRenderBufferForFreeze())
            {
            if (!iCanvasGc)
                {
                CHuiRenderPlugin& renderplugin = CHuiStatic::Renderer();
                iCanvasGc = renderplugin.CreateCanvasGcL();
                }    
            if (!iCanvasRenderBuffer)
                {
                iCanvasRenderBuffer = iCanvasGc->CreateRenderBufferL(TSize(0,0));
                iCanvasRenderBuffer->InitializeL(CHuiStatic::Env().Display(0).VisibleArea().Size());
                iCanvasRenderBuffer->Copy(TPoint(0,0));    
                iCanvasRenderBufferOrientation = CHuiStatic::Env().Display(0).Orientation();
                }
            }
        }
    else
        {
        delete iCanvasGc;
        iCanvasGc = NULL;        
        delete iCanvasRenderBuffer;
        iCanvasRenderBuffer = NULL;
        SetChanged();
        }    
    }

TBool CHuiRosterImpl::IsVisibleContentFrozen() const
    {
    return iRosterIsFrozen;
    }
    
void CHuiRosterImpl::EffectSetEffect(CHuiFxEffect* aEffect)
    {
    if (iEffect)
        {
        delete iEffect;
        iEffect = NULL;
        }
    iEffect = aEffect;
    SetChanged();
    }

TReal32 CHuiRosterImpl::EffectOpacityTarget() const
    {
    return iEffectOpacity; 
    }

void CHuiRosterImpl::EffectSetOpacity(TReal32 aOpacity)
    {
    iEffectOpacity = aOpacity;
    }

void CHuiRosterImpl::EffectDrawSelf( CHuiGc &aGc, const TRect & aDisplayRect) const
    {
    DrawSelf(aGc, iDisplay);
    }

THuiRealRect CHuiRosterImpl::EffectDisplayRect() const __SOFTFP
    {
    if (iDisplay)
        {
        return iDisplay->VisibleArea();
        }
    else
        {
        return THuiRealRect(TRect(0,0,0,0));
        }    
    }

MHuiEffectable* CHuiRosterImpl::Effectable()
    {
    return this;
    }

void CHuiRosterImpl::SetLoadingEffect(TBool /*aLoading*/)
    {
    // not needed
    }
void CHuiRosterImpl::EffectSetSource( TBool aIsInput1 )
    {
    iIsInput1 = aIsInput1;
    }

TBool CHuiRosterImpl::EffectGetSource() const
    {
    return iIsInput1;
    }

void CHuiRosterImpl::SetMemoryLevel(THuiMemoryLevel /*aLevel*/)
    {
    if ( IsVisibleContentFrozen() && !UseRenderBufferForFreeze() )
        {
        // Release render buffer if we are not supposed to use.
        delete iCanvasGc;
        iCanvasGc = NULL;        
        delete iCanvasRenderBuffer;
        iCanvasRenderBuffer = NULL;        
        }
    }

TBool CHuiRosterImpl::UseRenderBufferForFreeze() const
    {
    return !iDisplay || !( iDisplay->Env().MemoryLevel() <= EHuiMemoryLevelLowest );
    }
