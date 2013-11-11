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
* Description:   Implementation of CHuiLayout class. Layouts are visuals that 
*                manage the placement of a set of child visuals.
*
*/



#include <AknUtils.h>

#include "uiacceltk/HuiLayout.h"  // Class definition
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiDrawing.h"
#include "uiacceltk/HuiDisplay.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiUtil.h"
#include "HuiRosterImpl.h"
#include "huilayoutdata.h"

#include "HuiFxEffect.h"
#include "HuiRenderPlugin.h"
#include "huicanvasgc.h"
#include "Matrix.h"
// 
// methods
//

EXPORT_C CHuiLayout* CHuiLayout::AddNewL(CHuiControl& aOwnerControl,
                                         CHuiLayout* aParentLayout)
    {
    CHuiLayout* layout = aOwnerControl.AppendLayoutL(EHuiLayoutTypeLayout,
                                                     aParentLayout);
    return layout;
    }


EXPORT_C CHuiLayout::CHuiLayout(MHuiVisualOwner& aOwner)
        : CHuiVisual(aOwner)
    {
    }


EXPORT_C void CHuiLayout::ConstructL()
    {
    CHuiVisual::ConstructL();
    iHuiLayoutPrivateData = new (ELeave) THuiLayoutPrivateData;
    iHuiLayoutPrivateData->iGc = NULL;
    }


EXPORT_C CHuiLayout::~CHuiLayout()
    {
    if ( iHuiLayoutPrivateData ) // a Fix for OOM situations, tried to dereference NULL pointer
    	{
	    delete iHuiLayoutPrivateData->iScroll;
	
	    // Remove children in reverse order.
	    for(TInt i = Count() - 1; i >= 0; --i)
	        {
	        Remove(iHuiLayoutPrivateData->iChildren[i]);
	        }
	    iHuiLayoutPrivateData->iChildren.Reset();
	    delete iHuiLayoutPrivateData->iGc;
	    iHuiLayoutPrivateData->iGc = NULL;
	    delete iHuiLayoutPrivateData;
    	}
    }


EXPORT_C void CHuiLayout::RemoveAndDestroyAllD()
    {
    // Make sure we do not re-layout during destruction
    SetFlag( EHuiVisualFlagFreezeLayout );
    
    for(TInt i = Count() - 1; i >= 0; --i)
        {
        iHuiLayoutPrivateData->iChildren[i]->RemoveAndDestroyAllD();
        }
    iHuiLayoutPrivateData->iChildren.Reset();

    CHuiVisual::RemoveAndDestroyAllD();
    }


EXPORT_C void CHuiLayout::AppendL(CHuiVisual* aVisual, TInt aLayoutTransitionTime)
    {
    ASSERT(aVisual != NULL);
    __ASSERT_ALWAYS( iHuiLayoutPrivateData->iChildren.Find(aVisual) == KErrNotFound,
                     THuiPanic::Panic(THuiPanic::ELayoutDuplicateVisual) );

    User::LeaveIfError( iHuiLayoutPrivateData->iChildren.Append(aVisual) );
    
    CHuiLayout* oldLayout = aVisual->Layout();
    if (oldLayout != NULL)
    	{
    	// If this visual is already a member of another layout, remove it from the old one first.
    	oldLayout->Remove(aVisual);
	    }
	else
	    {
	    // if the visual is a showing as root visual before, remove it from the roster
	    if ( aVisual->Display() )
	        {
	        aVisual->Display()->Roster().HideVisual( aVisual );
	        }
	    }
    
    aVisual->SetLayout(this);

    // Update the visual's layout immediately.
    if(!(Flags() & EHuiVisualFlagFreezeLayout))
        {
        UpdateChildrenLayout(aLayoutTransitionTime);
        }

    SetChanged();
    }


EXPORT_C void CHuiLayout::InsertL(CHuiVisual* aVisual, TInt aPosition, TInt aLayoutTransitionTime)
    {
    ASSERT(aVisual != NULL);
    __ASSERT_ALWAYS( iHuiLayoutPrivateData->iChildren.Find(aVisual) == KErrNotFound,
                     THuiPanic::Panic(THuiPanic::ELayoutDuplicateVisual) );
    ASSERT((aPosition >= 0) && aPosition <= Count());
    
    iHuiLayoutPrivateData->iChildren.InsertL(aVisual, aPosition);
    
    CHuiLayout* oldLayout = aVisual->Layout();
    if (oldLayout != NULL)
    	{
    	// If this visual is already a member of another layout, remove it from the old one first.
    	oldLayout->Remove(aVisual);
	    }
	else
	    {
	    // if the visual is a showing as root visual before, remove it from the roster
	    if ( aVisual->Display() )
	        {
	        aVisual->Display()->Roster().HideVisual( aVisual );
	        }
	    }

    // Update the visual's layout immediately.
    aVisual->SetLayout(this);
    
    if(!(Flags() & EHuiVisualFlagFreezeLayout))
        {
        UpdateChildrenLayout(aLayoutTransitionTime);
        }

    SetChanged();
    }


EXPORT_C void CHuiLayout::Remove(CHuiVisual* aVisual, TInt aLayoutTransitionTime)
    {
    TInt index = iHuiLayoutPrivateData->iChildren.Find(aVisual);
    /*__ASSERT_DEBUG(index != KErrNotFound,
                   THuiPanic::Panic(THuiPanic::EInternal));*/
    if(index != KErrNotFound)
        {
        iHuiLayoutPrivateData->iChildren.Remove(index);
        aVisual->SetLayout(NULL); // defect. this should check if there is a Host()->ContainerLayout()
        
        // If the control group of the aVisual is showing in a roster,
        // we need to add it as the root visual of that roster
        /*
        for ( TInt d = 0; d < Env().DisplayCount() ; d ++ )
            {
            if ( (Env().Displays())[d]->Roster().Find( aVisual->Owner().ControlGroup() ) != KErrNotFound )
                {
                TRAP_IGNORE( (Env().Displays())[d]->Roster().ShowVisualL( aVisual ) );
                break;
                }
            }*/

        // Update the layout of all the children immediately.
        if(!(Flags() & EHuiVisualFlagFreezeLayout))
            {
            UpdateChildrenLayout(aLayoutTransitionTime);
            }

        SetChanged();
        }
    }


EXPORT_C TInt CHuiLayout::Count() const
    {
    return iHuiLayoutPrivateData->iChildren.Count();
    }


EXPORT_C CHuiVisual& CHuiLayout::Visual(TInt aIndex) const
    {
    return *(iHuiLayoutPrivateData->iChildren[aIndex]);
    }


EXPORT_C TInt CHuiLayout::FindVisual(const CHuiVisual* aVisual) const
    {
    for(TInt i = 0; i < Count(); ++i)
        {
        if(&Visual(i) == aVisual)
            {
            return i;
            }
        }
    return KErrNotFound;
    }


THuiRealRect CHuiLayout::BoundingRect() const
    {
    THuiRealPoint min;
    THuiRealPoint max;
    
    min.iX = 0;
    min.iY = 0;
    max.iX = 0;
    max.iY = 0;
    
    TInt count = Count();
    if(count > 0) 
    	{
    	min = Visual(0).Pos().Target();
    	max = min + Visual(0).Size().Target();    	
    	}

    for(TInt i = 1; i < count; ++i)
        {
        THuiRealPoint tl = Visual(i).Pos().Target();
        THuiRealPoint br = tl + Visual(i).Size().Target();

        min.iX = Min(min.iX, tl.iX);
        min.iY = Min(min.iY, tl.iY);
        max.iX = Max(max.iX, br.iX);
        max.iY = Max(max.iY, br.iY);
        }

    return THuiRealRect(min, max);
    }
    
    
EXPORT_C THuiRealSize CHuiLayout::InnerSize() const __SOFTFP
    {
    // Determine the size of this layout in pixels.
    THuiRealSize innerSizeChild = Size().RealTarget().AsSize(); // not correct units, but will work as fallback value if base unit is in pixels
    THuiRealPoint innerSizePx = LocalPointInPixels(Size().RealTarget(), EHuiReferenceStateTarget);

    // Subtract outer paddings.
    THuiRealRect paddingInPixels = PaddingInPixels(EHuiReferenceStateTarget);
    innerSizePx.iX -= paddingInPixels.iTl.iX + paddingInPixels.iBr.iX;
    innerSizePx.iY -= paddingInPixels.iTl.iY + paddingInPixels.iBr.iY;
    
    // Convert the result into base units of *this* layout.
    CHuiVisual* child = 0;
    if (iHuiLayoutPrivateData->iChildren.Count())
        {
        child = iHuiLayoutPrivateData->iChildren[0]; // get the first child, if there isn't one, then nothing to do anyway
        }
    if(child)
        {
        innerSizeChild = child->PixelPointInUnits(innerSizePx, BaseUnit().Abs(), EHuiReferenceStateTarget).AsSize();
        }

    return innerSizeChild;    
    }


EXPORT_C THuiRealPoint CHuiLayout::InnerTopLeft() const __SOFTFP
    {
    THuiRealPoint topLeftChild(0, 0);
    CHuiVisual* child = 0;
    if (iHuiLayoutPrivateData->iChildren.Count())
        {
        child = iHuiLayoutPrivateData->iChildren[0]; // get the first child, if there isn't one, then nothing to do anyway
        }
    if(child)
        {
        topLeftChild = child->PixelPointInUnits(PaddingInPixels().iTl, BaseUnit().Abs(), EHuiReferenceStateTarget);
        }
    return topLeftChild;
    }


EXPORT_C void CHuiLayout::SetSize(const THuiRealSize& aSize, TInt aTransitionTime)
    {
    CHuiVisual::SetSize(aSize, aTransitionTime);

    // The default layout does not need to update its children.
    }


EXPORT_C void CHuiLayout::UpdateChildrenLayout(TInt aTransitionTime)
    {
    // Update the layout of all children.
    TInt count = Count();
    for(TInt i = 0; i < count; ++i)
        {
        // size and positio changes must go also to inactive visuals
        UpdateChildLayout(i, aTransitionTime); 
        }

    CHuiVisual::UpdateChildrenLayout(aTransitionTime);

    // Report that the layout of this layout visual has been fully updated.
    ReportLayoutUpdate();
    }


EXPORT_C void CHuiLayout::UpdateChildLayout(TInt aIndex, TInt aTransitionTime)
    {
    THuiRealRect rect;
    TSize size;
    TPoint pos;
    TInt ordinal = ChildOrdinal(aIndex);
    CHuiVisual* child = iHuiLayoutPrivateData->iChildren[aIndex];
    TInt flags = child->Flags();

    TInt transitionTime = aTransitionTime;

    if(flags & EHuiVisualFlagManualTransitionTime)
        {
        // override the parent's transition time with our local one.
        transitionTime = TransitionTime();
        }

    // if either flag is missing, call child rect...
    TBool bothManual = (flags & EHuiVisualFlagManualPosition) && (flags & EHuiVisualFlagManualSize);
    if(!bothManual)
        {
        TInt childRectStatus = ChildRect(ordinal, rect);
        if(childRectStatus != THuiLayoutChildRectNotImplemented)
            {
            if(!(flags & EHuiVisualFlagManualPosition) && (childRectStatus & THuiLayoutChildRectPosUpdateNeeded))
                {
                child->SetPos(rect.iTl, transitionTime);
                }
            if(!(flags & EHuiVisualFlagManualSize) && (childRectStatus & THuiLayoutChildRectSizeUpdateNeeded))
                {
                child->SetSize(rect.Size(), transitionTime);
                }
            }
        else
            {
            if(!(flags & EHuiVisualFlagManualPosition) && ChildPos(ordinal, pos))
                {
                child->SetPos(pos, transitionTime);
                }
            if(!(flags & EHuiVisualFlagManualSize) && ChildSize(ordinal, size))
                {
                child->SetSize(size, transitionTime);
                }
            }
        }
    child->ReportLayoutUpdate();
    }


EXPORT_C TInt CHuiLayout::ChildOrdinal(TInt aIndex)
    {
    return aIndex;
    }


EXPORT_C TBool CHuiLayout::ChildSize(TInt /*aOrdinal*/, TSize& /*aSize*/)
    {
    // The default layout doesn't constrain the sizes of the children.
    return EFalse;
    }

EXPORT_C TBool CHuiLayout::ChildPos(TInt /*aOrdinal*/, TPoint& /*aPos*/)
    {
    // The default layout lets the children position themselves.
    return EFalse;
    }

EXPORT_C TInt CHuiLayout::ChildRect(TInt /*aOrdinal*/, THuiRealRect& /*aPos*/)
    {
    // The default layout lets the children position themselves.
    return THuiLayoutChildRectNotImplemented;
    }


EXPORT_C void CHuiLayout::SetInnerPadding(const TPoint& aInnerPadding)
    {
    iHuiLayoutPrivateData->iInnerPadding.iX = THuiMetric(aInnerPadding.iX, EHuiUnitPixel);
    iHuiLayoutPrivateData->iInnerPadding.iY = THuiMetric(aInnerPadding.iY, EHuiUnitPixel);
    SetChanged();
    }


EXPORT_C void CHuiLayout::SetInnerPadding(const THuiXYMetric& aInnerPadding)
    {
    iHuiLayoutPrivateData->iInnerPadding = aInnerPadding;
    SetChanged();
    }


EXPORT_C const THuiXYMetric& CHuiLayout::InnerPadding() const
    {
    return iHuiLayoutPrivateData->iInnerPadding;
    }
    

EXPORT_C THuiRealPoint CHuiLayout::InnerPaddingInBaseUnits() const __SOFTFP
    {
    THuiRealPoint innerPaddingPx = MetricToPixels(iHuiLayoutPrivateData->iInnerPadding, EHuiReferenceStateTarget);
    return PixelPointInUnits(innerPaddingPx, BaseUnit().Abs(), EHuiReferenceStateTarget);    
    }


EXPORT_C TInt CHuiLayout::HorizontalInnerPadding() const
    {
    return 0; //iHuiLayoutPrivateData->iXPadding;
    }


EXPORT_C TInt CHuiLayout::VerticalInnerPadding() const
    {
    return 0; //iHuiLayoutPrivateData->iYPadding;
    }
    

EXPORT_C TBool CHuiLayout::PrepareDrawL() 
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

    // The layout itself is invisible. Only its children can be seen.
    TInt count = Count();
    for(TInt i = 0; i < count; ++i)
        {
        CHuiVisual* visual = iHuiLayoutPrivateData->iChildren[i];
        //Ignore inactive child visuals
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
            return EFalse;
            }
        if (!successfullyPrepared) 
	        {
	        // we can stop preparing the rest of the children
	        // if one prepare failed (we won't be drawing anything 
	        // anyway)
	        
	        // also if the prepare failed but there was not a leave,
	        // we assume it was already handled 
	        return EFalse;
	        }
        }
    return ETrue;
    }


EXPORT_C void CHuiLayout::Draw(CHuiGc& aGc) const
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

    if(iOpacity.Now() <= EPSILON && (Effect() && !Effect()->IsSemitransparent()))
        {
        // This will not be visible due to being completely transparent.
        return;
        }

    if (( IsDelayedEffectSource()||Freezed() ) && StoredRenderBuffer())
        {
        DrawStoredBitmap(aGc);        
        return;
        }
    if (!HasActiveChilderen() && !Brushes() && !Effect())
        {
        // No children, no brushes just return.
        return;            
        }
    
    
    /** @todo  Wouldn't it be possible to just extend CHuiVisual::Draw()
               instead of redefining the entire method? */

    THuiRealRect area = DisplayRect();

    // Apply local transformation.
    EnterLocalProjection(aGc, ETrue, 0.f, &area);
    Transform(aGc, ETrue, &area);

    DrawBrushes(aGc, EHuiBrushLayerBackground);

    if(Clipping())
        {
        // @todo  Clipping not going to work if there is a transformation?
        // Layout clipping is not affected by transformation.
        aGc.Enable(CHuiGc::EFeatureClipping);
        aGc.PushClip();
        aGc.Clip(area);
        }

    // Can we draw effect
    TBool canUseEffectDrawing = Effect() && !LoadingEffect();

    // Flag to know what we did
    TBool didDrawEffect = EFalse;
    
    if (canUseEffectDrawing)
        {
        // Note that EHuiVisualFlagOpaqueHint improves performance a lot in cached effect drawing 
        TBool transparent = !(Flags() & EHuiVisualFlagOpaqueHint) && iOpacity.Now() < 1.0f;
        TBool refreshCache = Changed();                
        didDrawEffect =  Effect()->CachedDraw(aGc, area, refreshCache, !transparent);
        }
    
    if ( !didDrawEffect )
        {
        // huilayout does not draw itself, only children.
        }
    else
        {
        // Effect was drawn, OpenGLES/OpenVG may be in different state than what Gc thinks so we restore it.
        aGc.RestoreState();        
        }
    
    if ( !didDrawEffect || (didDrawEffect && !EffectIsAppliedToChildren()))
        {
        // The layout itself is invisible. Only its children can be seen.
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


    if(Clipping())
        {
        // Restore original clipping rectangle.
        aGc.PopClip();
        }


    DrawBrushes(aGc, EHuiBrushLayerForeground);

    // Restore original transformation.
    Transform(aGc, EFalse);
    EnterLocalProjection(aGc, EFalse);
    }


EXPORT_C void CHuiLayout::ReportChanged()
    {
    // Report changes in the layout itself.
    CHuiVisual::ReportChanged();

    // Notify the display that the visual's area should be redrawn.
    CHuiDisplay* display = NULL;

    // Select active display, fallback to own display.
    // For scaled tv-out cloning the active display takes care of scaling direty areas too. 
    if (iOwner && iOwner->Env().CurrentDisplay())
        {
        display = iOwner->Env().CurrentDisplay();    
        }
    else
        {
        display = Display();    
        }

    if (display && !display->IsDirtyScanNeeded())
        {
        return;    
        }        

    if (display && display->UseTransformedDirtyRegions())
        {
        THuiRealRect displayRect = DisplayRect();
        EnterLocalProjection(*display->Gc(), ETrue, 0.f, &displayRect);
        Transform(*display->Gc(), ETrue, &displayRect);
        }

        
    TInt count = Count();
    for(TInt i = 0; i < count; ++i)
        {        
        if (display && !display->IsDirtyScanNeeded())
            {
            break;    
            }        
        //Ignore inactive child visuals
        if ( iHuiLayoutPrivateData->iChildren[i]->Flags()& EHuiVisualFlagInactive )
        	{
        	continue;
        	}
        iHuiLayoutPrivateData->iChildren[i]->ReportChanged();     
        }
        
    if (display && display->UseTransformedDirtyRegions())
        {
        EnterLocalProjection(*display->Gc(), EFalse);
        Transform(*display->Gc(), EFalse);                
        }
    }


EXPORT_C TBool CHuiLayout::Changed() const
    {
    if(CHuiVisual::Changed())
        {
        return ETrue;
        }
    if(iHuiLayoutPrivateData->iScroll)
        {
        return iHuiLayoutPrivateData->iScroll->iOffset.Changed();
        }
    return EFalse;
    }


EXPORT_C void CHuiLayout::ClearChanged()
    {
    CHuiVisual::ClearChanged();
    
    // Clear the change flags of children as well.
    TInt count = Count();
    for(TInt i = 0; i < count; ++i)
        {
        if (iHuiLayoutPrivateData->iChildren[i]->Flags() & EHuiVisualFlagInactive)
            {
            // No need to clear inactive children
            continue;
            }
        iHuiLayoutPrivateData->iChildren[i]->ClearChanged();
        }
        
    if(iHuiLayoutPrivateData->iScroll)
        {
        iHuiLayoutPrivateData->iScroll->iOffset.ClearChanged();
        }
    }

EXPORT_C void CHuiLayout::SetBaseUnit(const THuiMetric& aBaseUnit)
    {
    iHuiLayoutPrivateData->iBaseUnit.iX = aBaseUnit;
    iHuiLayoutPrivateData->iBaseUnit.iY = aBaseUnit;
    SetChanged();
    }
    
EXPORT_C void CHuiLayout::SetBaseUnit(const THuiXYMetric& aBaseUnit)
    {
    iHuiLayoutPrivateData->iBaseUnit = aBaseUnit;
    SetChanged();
    }


EXPORT_C THuiXYMetric CHuiLayout::BaseUnit() const
    {
    THuiXYMetric metric(iHuiLayoutPrivateData->iBaseUnit);
    if((Flags() & EHuiVisualFlagAutomaticLocaleMirroringEnabled) && CHuiStatic::LayoutMirrored())
        {
        metric.iX.iMagnitude = -metric.iX.iMagnitude;
        }
    return metric;
    }


EXPORT_C void CHuiLayout::EnableScrollingL(TBool aScrolling)
    {
    if(iHuiLayoutPrivateData->iScroll && !aScrolling)
        {
        // Disable scrolling.
        delete iHuiLayoutPrivateData->iScroll;
        iHuiLayoutPrivateData->iScroll = NULL;
        }
    else if(!iHuiLayoutPrivateData->iScroll && aScrolling)
        {
        // Enable scrolling.
        iHuiLayoutPrivateData->iScroll = new (ELeave) TScrollState();
        }
    else
        {
        // for PC lint
        }
    }


EXPORT_C TBool CHuiLayout::Scrolling() const
    {
    return iHuiLayoutPrivateData->iScroll != 0;
    }


EXPORT_C THuiTimedPoint& CHuiLayout::ScrollOffset()
    {
    __ASSERT_ALWAYS(iHuiLayoutPrivateData->iScroll,
                    THuiPanic::Panic(THuiPanic::ELayoutNotScrolling));
    return iHuiLayoutPrivateData->iScroll->iOffset;
    }


EXPORT_C const THuiTimedPoint& CHuiLayout::ScrollOffset() const
    {
    __ASSERT_ALWAYS(iHuiLayoutPrivateData->iScroll,
                    THuiPanic::Panic(THuiPanic::ELayoutNotScrolling));
    return iHuiLayoutPrivateData->iScroll->iOffset;
    }


EXPORT_C void CHuiLayout::SetScrollOffsetInBaseUnits(const THuiRealPoint& aOffset, TInt aTransitionTime)
    {
    // Pass information directly to the scroll offset
    ScrollOffset().Set(aOffset, aTransitionTime);
    }


EXPORT_C TSize CHuiLayout::VirtualSize() const
    {
    __ASSERT_ALWAYS(iHuiLayoutPrivateData->iScroll,
                    THuiPanic::Panic(THuiPanic::ELayoutNotScrolling));
    return iHuiLayoutPrivateData->iScroll->iVirtualSize;
    }


EXPORT_C TInt
CHuiLayout::EffectiveLayoutOrdinal(const CHuiVisual& aVisual) const
    {
    TInt ordinal = 0;

    TInt count = Count();
    for(TInt i = 0; i < count; ++i)
        {
        if(iHuiLayoutPrivateData->iChildren[i] == &aVisual)
            {
            return ordinal;
            }

        // The visuals that are laid out completely manually do not affect
        // the layout ordinal.
        if((iHuiLayoutPrivateData->iChildren[i]->Flags() & EHuiVisualFlagManualLayout) !=
           EHuiVisualFlagManualLayout)
            {
            ++ordinal;
            }
        }

    THuiPanic::Panic(THuiPanic::ELayoutChildVisualNotFound);
    return 0;
    }


EXPORT_C void CHuiLayout::Reorder(CHuiVisual& aVisual, TInt aPosition, TInt aLayoutTransitionTime)
    {
    TInt index = FindVisual(&aVisual);
    if(index != aPosition)
        {
        __ASSERT_ALWAYS( aPosition >= 0 && aPosition < Count(),
                         THuiPanic::Panic(THuiPanic::ELayoutInvalidChildPosition) );
        __ASSERT_ALWAYS( index != KErrNotFound, 
                         THuiPanic::Panic(THuiPanic::ELayoutChildVisualNotFound) );

        // Move the children around in the array, so that aVisual ends up
        // at aPosition.
        TInt dir = (aPosition > index? +1 : -1);
        for(TInt i = index; i != aPosition; i += dir)
            {
            iHuiLayoutPrivateData->iChildren[i] = iHuiLayoutPrivateData->iChildren[i + dir];
            }
        iHuiLayoutPrivateData->iChildren[aPosition] = &aVisual;        
        SetChanged();
        }

    if(!(Flags() & EHuiVisualFlagFreezeLayout))
        {
        UpdateChildrenLayout(aLayoutTransitionTime);
        }    
    }


EXPORT_C void CHuiLayout::MoveVisualToFront(CHuiVisual& aVisual, TInt aLayoutTransitionTime)
    {
    Reorder(aVisual, Count() - 1, aLayoutTransitionTime);
    }


EXPORT_C void CHuiLayout::MoveVisualToBack(CHuiVisual& aVisual, TInt aLayoutTransitionTime)
    {
    Reorder(aVisual, 0, aLayoutTransitionTime);
    }


EXPORT_C void CHuiLayout::DumpTree() const
    {
    // Dump children.
    TBuf<100> myName;
    TBuf<100> childName;

    CHuiVisual::DumpTree();

    GetInstanceName(myName);

    TInt count = Count();
    for(TInt i = 0; i < count; ++i)
        {
        iHuiLayoutPrivateData->iChildren[i]->GetInstanceName(childName);
        CHuiStatic::Printf(_L("%S -> %S;"), &myName, &childName);

        iHuiLayoutPrivateData->iChildren[i]->DumpTree();
        }
    }


EXPORT_C void CHuiLayout::NotifySkinChangedL()
    {
    TInt count = Count();
    for(TInt i = 0; i < count; ++i)
        {
        //Ignore inactive child visuals
        if ( iHuiLayoutPrivateData->iChildren[i]->Flags()& EHuiVisualFlagInactive )
        	{
        	continue; 
        	}
        iHuiLayoutPrivateData->iChildren[i]->NotifySkinChangedL();
        }
    }


EXPORT_C CHuiVisual* CHuiLayout::FindTag(const TDesC8& aTag)
    {
    // Check self first.
    CHuiVisual* result = CHuiVisual::FindTag(aTag);
    if(result)
        {
        return result;
        }

    // Check children.
    TInt i;
    TInt count = Count();
    for(i = 0; i < count; ++i)
        {
        result = iHuiLayoutPrivateData->iChildren[i]->FindTag(aTag);
        if(result)
            {
            return result;
            }
        }

    // Could not find it.
    return NULL;
    }


EXPORT_C void CHuiLayout::SetTransitionTime(TInt aTransitionTime)
    {
    iHuiLayoutPrivateData->iTransitionTime = aTransitionTime;
    }


EXPORT_C TInt CHuiLayout::TransitionTime() const
    {
    return iHuiLayoutPrivateData->iTransitionTime;
    }

EXPORT_C THuiRealPoint CHuiLayout::MetricReferenceForLayoutInPixels(const CHuiLayout* /*aLayout*/, const THuiXYMetric& aMetric) const __SOFTFP
    {
    return MetricReferenceForLayoutInPixels(aMetric);
    }
    
/*
 * can be used by a layout to convert a pixel value into
 * its own units as if from a child visual's frame of reference
 *
 * @todo this needs refactoring with CHuiVisual::MetricReferenceInPixels
 */
EXPORT_C THuiRealPoint CHuiLayout::MetricReferenceForLayoutInPixels(const THuiXYMetric& aMetric) const __SOFTFP
    {
    THuiRealPoint refSizePx(0.f, 0.f);
    
    // if weights are used in this context, treat them as normalized units
    TInt xUnit = (aMetric.iX.iUnit == EHuiUnitWeight) ? EHuiUnitNormalized : aMetric.iX.iUnit;
    TInt yUnit = (aMetric.iY.iUnit == EHuiUnitWeight) ? EHuiUnitNormalized : aMetric.iY.iUnit;
    
    // For normalized units, the reference is the parent size in pixels.
    if(xUnit == EHuiUnitNormalized || yUnit == EHuiUnitNormalized ||
       xUnit == EHuiUnitParentWidth || yUnit == EHuiUnitParentWidth ||
       xUnit == EHuiUnitParentHeight || yUnit == EHuiUnitParentHeight)        
        {
        THuiRealPoint sizePx(0.f, 0.f);
        
        // Determine the size of this layout in pixels.
        if(Layout())
            {
            // Recursive fun. We don't want to do this more than once.
            // (LocalPointInPixels => MetricsToPixels => MetricReferenceInPixels => LocalPointInPixels)
            sizePx = LocalPointInPixels(Size().RealTarget(), EHuiReferenceStateTarget);
            }
        else if(Display())
            {
            // @todo Jaakko please code review this change!
//            sizePx = Display()->Size().AsPoint();
            sizePx = Display()->VisibleArea().Size().AsPoint();
            }
        else
            {
            // For PC-lint
            }
            
        if(xUnit == EHuiUnitNormalized || xUnit == EHuiUnitParentWidth)
            {
            refSizePx.iX = sizePx.iX;
            }
        else if(xUnit == EHuiUnitParentHeight)
            {
            refSizePx.iX = sizePx.iY;
            }
        else
            {
            // For PC-lint
            }

        if(yUnit == EHuiUnitNormalized || yUnit == EHuiUnitParentHeight)
            {
            refSizePx.iY = sizePx.iY;
            }
        else if(yUnit == EHuiUnitParentWidth)
            {
            refSizePx.iY = sizePx.iX;
            }
        else
            {
            // For PC-lint
            }
        }

    // Display-relative units are always relative to the display where the visual is on.
    if(xUnit == EHuiUnitRelativeToDisplay && Display())
        {
        refSizePx.iX = Display()->VisibleArea().Width();
        }
    if(yUnit == EHuiUnitRelativeToDisplay && Display())
        {
        refSizePx.iY = Display()->VisibleArea().Height();
        }        
        
    // For relative to my size units, the reference is the size of this visual in pixels.
    if(xUnit == EHuiUnitRelativeToMySize || yUnit == EHuiUnitRelativeToMySize ||
        xUnit == EHuiUnitMyWidth || yUnit == EHuiUnitMyWidth ||
        xUnit == EHuiUnitMyHeight || yUnit == EHuiUnitMyHeight ||
        xUnit == EHuiUnitMyDimensionAverage || yUnit == EHuiUnitMyDimensionAverage)
        {
        THuiRealPoint sizePx(0.f, 0.f);
        if(Layout())
            {
            // Recursive fun. We don't want to do this more than once.
            // (MetricsToPixels => MetricReferenceInPixels)
            sizePx = MetricToPixels(LocalBaseUnit().Abs() * Size().RealTarget(), EHuiReferenceStateTarget);
            }
        else if(Display())
            {
            sizePx = Display()->VisibleArea().Size().AsPoint();
            }
        else
            {
            // For PC-lint
            }
        
        if(xUnit == EHuiUnitRelativeToMySize || xUnit == EHuiUnitMyWidth)
            {
            refSizePx.iX = sizePx.iX;
            }
        else if(xUnit == EHuiUnitMyHeight)
            {
            refSizePx.iX = sizePx.iY;
            }
        else
            {
            // For PC-lint
            }
        
        if(yUnit == EHuiUnitRelativeToMySize || yUnit == EHuiUnitMyHeight)
            {
            refSizePx.iY = sizePx.iY;
            }
        else if(yUnit == EHuiUnitMyWidth)
            {
            refSizePx.iY = sizePx.iX;
            }
        else
            {
            // For PC-lint
            }

        if( xUnit == EHuiUnitMyDimensionAverage ||
      	    yUnit == EHuiUnitMyDimensionAverage )
            {
            TReal32 average = (sizePx.iX + sizePx.iY) / 2;
            if(xUnit == EHuiUnitMyDimensionAverage)
      	        {
                refSizePx.iX = average;
                }
            if(yUnit == EHuiUnitMyDimensionAverage)
                {           
                refSizePx.iY = average;
                }
            }
        }
                
    // S60 platform units are specific to the display where the visual is on.
    if(xUnit == EHuiUnitS60 && Display())
        {
        refSizePx.iX = Display()->UnitValue();
        }
    if(yUnit == EHuiUnitS60 && Display())
        {
        refSizePx.iY = Display()->UnitValue();
        }

    return refSizePx;        
    }
   
EXPORT_C void CHuiLayout::ExpandRectWithContent(TRect& aRect) const
    {
    // Expand with scroll offset
    if ( iHuiLayoutPrivateData->iScroll )
        {
        const TPoint scrollOffset = LocalPointInPixels(iHuiLayoutPrivateData->iScroll->iOffset.RealNow(), EHuiReferenceStateNow );
        if ( scrollOffset.iX > 0 )
            {
            aRect.iTl.iX -= scrollOffset.iX;
            }
        if ( scrollOffset.iX < 0 )
            {
            aRect.iBr.iX -= scrollOffset.iX;
            }
        if ( scrollOffset.iY > 0 )
            {
            aRect.iTl.iY -= scrollOffset.iY;
            }
        if ( scrollOffset.iY < 0 )
            {
            aRect.iBr.iY -= scrollOffset.iY;
            }
        }
    
    CHuiVisual::ExpandRectWithContent( aRect );
    }

EXPORT_C void CHuiLayout::VisualExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CHuiVisual::VisualExtension(aExtensionUid, aExtensionParams);
    }

EXPORT_C void CHuiLayout::SetPos(const THuiRealPoint& aPos, TInt aTransitionTime)
    {
    CHuiVisual::SetPos(aPos,aTransitionTime);
    }

void CHuiLayout::DrawStoredBitmap(CHuiGc &aGc) const
    {
    if (!Display()) return;
    
    if (!iHuiLayoutPrivateData->iGc)
        {
        CHuiRenderPlugin& renderplugin = CHuiStatic::Renderer();
		// deleted in destructor or CHuiCanvasVisual::FreeRenderBuffer when not needed anymore
        iHuiLayoutPrivateData->iGc = renderplugin.CreateCanvasGcL(); 
        }
    CHuiCanvasGc& gc = *iHuiLayoutPrivateData->iGc;
    gc.SetGc(aGc);
    gc.SetDefaults();
    gc.PushTransformationMatrix();
    
    //TInt w = displayArea.Width();
    TRect displayArea = Display()->VisibleArea();
    TInt h = displayArea.Height();

    // Pixels are upside down in OpenVG canvas render buffer
    if (CHuiStatic::Env().Renderer() == EHuiRendererVg10)
        {
        TInt height = h;
        //if (relativeOrientation == CHuiGc::EOrientationCW90 || relativeOrientation == CHuiGc::EOrientationCCW90)
        //    {
        //    height = w;    
        //    }            
        gc.Translate(0.0f, height, 0.0f);            
        gc.Scale(1.f, -1.f, 1.f);
        }
    
    THuiRealPoint dest_point = DisplayRect().iTl;
    CHuiCanvasRenderBuffer *stored = StoredRenderBuffer();
    gc.DrawImage(*stored, dest_point); 

    gc.PopTransformationMatrix();

    }

EXPORT_C void CHuiLayout::DrawSelf(CHuiGc& aGc, const TRect& aDisplayRect) const
    {
    if (( IsDelayedEffectSource()||Freezed() ) && StoredRenderBuffer())
        {
        DrawStoredBitmap(aGc);        
        return;
        }

    if( Effect() && EffectIsAppliedToChildren() )
        {
        // The layout itself is invisible. Only its children can be seen.
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
    else
        {
        CHuiVisual::DrawSelf(aGc, aDisplayRect);
        }
    }

EXPORT_C TReal32 CHuiLayout::BrushOpacity() const __SOFTFP
    {
    return CHuiVisual::BrushOpacity();
    }

EXPORT_C THuiRealRect CHuiLayout::BrushRect() const __SOFTFP
    {
    return CHuiVisual::BrushRect();
    }


EXPORT_C CHuiSkin& CHuiLayout::BrushSkin() const
    {
    return CHuiVisual::BrushSkin();
    }

HUI_SESSION_OBJECT_IMPL_EXPORT(CHuiLayout, ETypeVisual)

EXPORT_C void CHuiLayout::GetInstanceName(TDes& aName) const
    {
    CHuiVisual::GetInstanceName(aName);
    }


EXPORT_C void CHuiLayout::GetClassName(TDes& aName) const
    {
    aName = _L("CHuiLayout");
    }

TBool CHuiLayout::HasActiveChilderen() const
    {
    TBool hasActiveChildren = EFalse;
    const TInt count = Count();
    for(TInt i = 0; i < count; ++i)
        {
        CHuiVisual* visual = iHuiLayoutPrivateData->iChildren[i];
        if (!(visual->Flags()& EHuiVisualFlagInactive ))
            {
            hasActiveChildren = ETrue;
            break;
            }
        }    
    return hasActiveChildren;    
    }

TBool CHuiLayout::EffectIsAppliedToChildren() const
    {
    if (Effect() && !LoadingEffect() && !(Effect()->EffectFlags() & KHuiFxEffectExcludeChildrenFlag))
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }    
    }
