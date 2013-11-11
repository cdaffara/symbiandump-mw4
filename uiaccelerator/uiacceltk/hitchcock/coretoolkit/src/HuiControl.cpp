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
* Description:   CHuiControl provides a base class for a generic logical 
*                control element in the HUITK UI.
*
*/



#include "uiacceltk/HuiControl.h"  // Class definition
#include "uiacceltk/HuiControlGroup.h"
#include "uiacceltk/huieventhandler.h"
#include "uiacceltk/HuiVisual.h"
#include "uiacceltk/HuiDisplay.h"
#include "HuiRosterImpl.h"
#include "uiacceltk/HuiEnv.h"
#include "HuiVisualFactory.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiProbe.h"


EXPORT_C CHuiControl::CHuiControl(CHuiEnv& aEnv)
        : iEnv(aEnv), iFocusing(EFalse)
    {
    HUI_PROBE_ASSOCIATE_WITH_CURRENT_SESSION
    HUI_PROBE_REPORT_CONSTRUCTED

    // Generate an automatical ID for all controls.
    iId = CHuiStatic::GenerateId();
    }

// virtual second phase construtor
EXPORT_C void CHuiControl::BaseConstructL()
    {
    }


EXPORT_C CHuiControl::~CHuiControl()
    {
    // Cancel scheduled commands for this control.
    Env().CancelCommands(this);
    
    iOwnerGroup = NULL;
    iBoundDisplay = NULL;

    if(iHost)
        {
        iHost->RemoveConnection(this);
        iHost = NULL;
        }

    for(TInt i = iConnections.Count() - 1; i >= 0; --i)
        {
        RemoveConnection(iConnections[i].iControl);
        }
    iConnections.Reset();

    // The visuals are owned by the control.
    iVisuals.ResetAndDestroy();

    HUI_PROBE_REPORT_DESTRUCTED
    
    }


EXPORT_C CHuiEnv& CHuiControl::Env() const
    {
    return iEnv;
    }


EXPORT_C CHuiControlGroup* CHuiControl::ControlGroup() const
    {
    return iOwnerGroup;
    }
    
    
void CHuiControl::SetControlGroup(CHuiControlGroup& aOwnerGroup)
    {
    iOwnerGroup = &aOwnerGroup;
    }
    
    
EXPORT_C CHuiTextureManager& CHuiControl::TextureManager() const
    {
    return Env().TextureManager();
    }
    
    
EXPORT_C CHuiDisplay* CHuiControl::Display() const
    {
    return iBoundDisplay;
    }
    

EXPORT_C void CHuiControl::BindDisplay(CHuiDisplay& aDisplay)
    {
    iBoundDisplay = &aDisplay;
    }    


EXPORT_C void CHuiControl::SetId(TInt aId)
    {
    iId = aId;
    }


EXPORT_C TInt CHuiControl::Id() const
    {
    return iId;
    }


EXPORT_C void CHuiControl::AppendL(CHuiVisual* aVisual)
    {
    User::LeaveIfError(iVisuals.Append(aVisual));
    aVisual->SetOwner(*this);
    TRAPD(err, VisualAddedL(aVisual));
    if(err != KErrNone)
        {
        // Can't leave the visual in the array of our owned visuals.
        iVisuals.Remove(iVisuals.Find(aVisual));
        }
    }


EXPORT_C void CHuiControl::AppendL(CHuiVisual* aVisual, CHuiLayout* aParentLayout)
    {
    TInt err = KErrNone;

    User::LeaveIfError(iVisuals.Append(aVisual));

    // Append to the parent layout before notification.
    if(aParentLayout)
        {
        TRAP(err, aParentLayout->AppendL(aVisual));
        }

    TRAP(err, VisualAddedL(aVisual));
    if(err != KErrNone)
        {
        // Can't leave the visual in the array of our owned visuals.
        iVisuals.Remove(iVisuals.Find(aVisual));
        }
    }


EXPORT_C void CHuiControl::Remove(CHuiVisual* aVisual)
    {
    TInt index = iVisuals.Find(aVisual);
    if(index != KErrNotFound)
        {
        VisualRemoved(aVisual);
        iVisuals.Remove(index);
        }
    }


EXPORT_C CHuiVisual* CHuiControl::AppendVisualL(THuiVisualType aVisualType,
                                                CHuiLayout* aParentLayout)
    {
    CHuiVisual* visual = Env().VisualFactory().NewVisualLC(aVisualType, *this);
    AppendL(visual, aParentLayout);
    // The visual is now owned by this control.
    CleanupStack::Pop(visual);
    return visual;
    }


EXPORT_C CHuiLayout* CHuiControl::AppendLayoutL(THuiLayoutType aLayoutType,
                                                CHuiLayout* aParentLayout)
    {
    CHuiLayout* layout = Env().VisualFactory().NewLayoutLC(aLayoutType, *this);
    AppendL(layout, aParentLayout);
    // The visual is now owned by this control.
    CleanupStack::Pop(layout);
    return layout;
    }


EXPORT_C CHuiVisual& CHuiControl::Visual(TInt aIndex) const
    {
    return *iVisuals[aIndex];
    }


EXPORT_C TInt CHuiControl::VisualCount() const
    {
    return iVisuals.Count();
    }


EXPORT_C CHuiVisual* CHuiControl::FindTag(const TDesC8& aTag) const
    {
    TInt i = 0;
    
    for(i = 0; i < iVisuals.Count(); ++i)
        {
        if(HuiUtil::TagMatches(iVisuals[i]->Tag(), aTag))
            {
            return iVisuals[i];
            }
        }   
        
    return NULL;
    }


EXPORT_C CHuiControl* CHuiControl::Host() const
    {
    return iHost;
    }


EXPORT_C void CHuiControl::SetHost(CHuiControl* aHost)
    {
    // do not call this function directly. This should be only called by the 
    // AddConnectionL and RemoveConnection functions.
    
    if ( aHost )
        {    
        // When adding a host, the host must be aware of this connection first.
        __ASSERT_ALWAYS( aHost->FindConnection(this) != KErrNotFound, USER_INVARIANT() );
        }
    
    TRAPD(err, HostChangingL(aHost));
    if(err != KErrNone)
        {
        if(aHost)
            {
            RemoveVisualsFromHostControl(*aHost);
            }
        return;
        }

    iHost = aHost;
    }


EXPORT_C void CHuiControl::AddConnectionL(CHuiControl* aConnectedControl, TInt aRole)
    {
    // check that the connection does not exist:
    if ( FindConnection( aConnectedControl ) != KErrNotFound )
        {
        User::Leave( KErrAlreadyExists );
        }
    
    SConnection client;
    client.iControl = aConnectedControl;
    client.iRole = aRole;
    User::LeaveIfError(iConnections.Append(client));

    // This control is now the client's host.
    aConnectedControl->SetHost(this);
    
    if ( aConnectedControl->Host() != this )
        {
        // Adding Host failed -> remove connection
        iConnections.Remove( iConnections.Count()-1 );
        User::Leave( KErrGeneral );
        }
    else
        {
        // Host set OK
        ConnectionAddedL(aConnectedControl, aRole);
        }
    }


EXPORT_C void CHuiControl::RemoveConnection(CHuiControl* aConnectedControl)
    {
    for(TInt i = 0; i < iConnections.Count(); ++i)
        {
        if(iConnections[i].iControl == aConnectedControl)
            {
            aConnectedControl->SetHost(NULL);
            const TInt role = iConnections[i].iRole;
            iConnections.Remove(i);
            
            ConnectionRemoved(aConnectedControl, role );
            return;
            }
        }
    // The client must exist.
    __ASSERT_ALWAYS(EFalse, THuiPanic::Panic(THuiPanic::EInternal));
    }


EXPORT_C TInt CHuiControl::ConnectionCount() const
    {
    return iConnections.Count();
    }


EXPORT_C CHuiControl& CHuiControl::Connection(TInt aIndex) const
    {
    return *iConnections[aIndex].iControl;
    }

// @deprecated
EXPORT_C CHuiControl& CHuiControl::ConnectionByOrdinal(TInt aOrdinal) const
    {
    // First look based on role.
    for(TInt i = 0; i < iConnections.Count(); ++i)
        {
        if(iConnections[i].iRole == aOrdinal + 1)
            {
            return *iConnections[i].iControl;
            }
        }

    // Fall back to index.
    return Connection(aOrdinal);
    }


EXPORT_C TInt CHuiControl::FindConnection(const CHuiControl* aConnected) const
    {
    for(TInt i = 0; i < iConnections.Count(); ++i)
        {
        if(iConnections[i].iControl == aConnected)
            {
            return i;
            }
        }
    return KErrNotFound;
    }


EXPORT_C TInt CHuiControl::ConnectionRole(TInt aIndex) const
    {
    return iConnections[aIndex].iRole;
    }


// @deprecated
EXPORT_C TInt CHuiControl::ConnectionOrdinal(TInt aIndex) const
    {
    if(iConnections[aIndex].iRole)
        {
        return iConnections[aIndex].iRole - 1;
        }
    return aIndex;
    }


EXPORT_C TInt CHuiControl::Role() const
    {
    return iRole;
    }


EXPORT_C void CHuiControl::SetRole(TInt aRole)
    {
    iRole = aRole;
    }


EXPORT_C TInt CHuiControl::HostId() const
    {
    return iHostId;
    }


EXPORT_C void CHuiControl::SetHostId(TInt aHostId)
    {
    // If adding automatic visual host, there cannot be already one defined.
    __ASSERT_ALWAYS( !iHost, USER_INVARIANT() );
    
    iHostId = aHostId;
    }


EXPORT_C CHuiLayout* CHuiControl::ContainerLayout(const CHuiControl* /*aConnected*/) const
    {
    // Generic controls aren't able to provide container layouts.
    return NULL;
    }


EXPORT_C void CHuiControl::VisualAddedL(CHuiVisual* aVisual)
    {
    // Add the new visual to the container layout.
    if(iHost && !aVisual->Layout())
        {
        CHuiLayout* container = iHost->ContainerLayout(this);
        if(container)
            {
            container->AppendL(aVisual);
            }
        }
    else
        {
        // If the control has been bound to a display (for example, when
        // the control is shown), new visuals will be automatically shown.
        if(iBoundDisplay && 
           (iOwnerGroup && iBoundDisplay->Roster().Find(iOwnerGroup) != KErrNotFound) && 
           !aVisual->Layout() && !aVisual->Display())
            {
            iBoundDisplay->Roster().ShowVisualL(aVisual);
            }
        }
    }


EXPORT_C void CHuiControl::VisualRemoved(CHuiVisual* aVisual)
    {
    // Add the new visual to the container layout.
    if(iHost)
        {
        CHuiLayout* container = iHost->ContainerLayout(this);
        if(container)
            {
            container->Remove(aVisual);
            }
        }
    }


EXPORT_C void CHuiControl::ConnectionAddedL(CHuiControl* /*aConnectedControl*/, TInt /*aRole*/)
    {
    // Do nothing.
    }


EXPORT_C void CHuiControl::ConnectionRemoved(CHuiControl* /*aConnectedControl*/, TInt /*aRole*/)
    {
    // Do nothing.
    }


void CHuiControl::RemoveVisualsFromHostControl( CHuiControl& aHostControl )
    {
    __ASSERT_ALWAYS( &aHostControl != this, USER_INVARIANT() );
    
    // Remove the visuals.
    for(TInt i = 0; i < iVisuals.Count(); ++i)
        {
        if ( iVisuals[i]->Layout() && &iVisuals[i]->Layout()->Owner() == &aHostControl )
            {
            iVisuals[i]->Layout()->Remove(iVisuals[i]);
            
            // If the own control group is showing still in some roster,
            // we need to add the root visuals over there
            if ( iBoundDisplay )
                {
                TRAP_IGNORE( iBoundDisplay->Roster().ShowVisualL( iVisuals[i] ) )
                }
            }
        }
    }


EXPORT_C void CHuiControl::HostChangingL(CHuiControl* aNewHost)
    {
    CHuiLayout* newContainer = 0;
    TInt i = 0;

    if(aNewHost)
        {
        // The container layout provided by the new host.
        newContainer = aNewHost->ContainerLayout(this);
        }

    if(iHost)
        {
        RemoveVisualsFromHostControl(*iHost);
        }

    if(newContainer)
        {
        for(i = 0; i < iVisuals.Count(); ++i)
            {
            // Only the visuals that aren't already attached to a layout
            // are added to the container layout.
            if(!iVisuals[i]->Layout())
                {
                newContainer->AppendL(iVisuals[i]);
                }
            }
        }
    }


void CHuiControl::ShowL(CHuiDisplay& aDisplay)
    {
    if(HostId())
        {
        CHuiControl* host = Env().FindControl(HostId());

        if(host)
            {
            host->AddConnectionL(this, Role());
            }
        else
            {
            }
        }

    BindDisplay(aDisplay);

    // Show all the visuals on the specified display.
    for(TInt i = 0; i < iVisuals.Count(); ++i)
        {
        // The visuals that are part of a layout will be shown when the
        // layout is shown.
        if(!iVisuals[i]->Layout() && !iVisuals[i]->Display())
            {
            aDisplay.Roster().ShowVisualL(iVisuals[i]);
            }
        }
        
    NotifyControlVisibility(ETrue, aDisplay);
    }


void CHuiControl::Hide(CHuiDisplay& aDisplay)
    {
    NotifyControlVisibility(EFalse, aDisplay);
    
    // Show all the visuals on the specified display.
    for(TInt i = 0; i < iVisuals.Count(); ++i)
        {
        // The visuals that are part of a layout will be shown when the
        // layout is shown.
        if(!iVisuals[i]->Layout() && iVisuals[i]->Display() == &aDisplay)
            {
            aDisplay.RosterImpl().HideVisual(iVisuals[i]);
            }
        }

    // Unbind from display?
    if(iBoundDisplay == &aDisplay)
        {
        iBoundDisplay = NULL;
        }

    // Unlink from parent control if automated host id set.
    if(Host() && HostId() )
        {
        Host()->RemoveConnection(this);
        }
    }


EXPORT_C void CHuiControl::NotifyControlVisibility(TBool /*aIsVisible*/, 
                                                   CHuiDisplay& /*aDisplay*/)
    {
    // Nothing to do by default.
    }


EXPORT_C TPoint CHuiControl::HostToDisplay(const TPoint& aPoint) const
    {
    if(!iHost)
        {
        return aPoint;
        }

    CHuiLayout* container = iHost->ContainerLayout(this);

    return container->LocalToDisplay(aPoint) + container->Pos().Target();
    }


EXPORT_C TPoint CHuiControl::DisplayToHost(const TPoint& aPoint) const
    {
    if(!iHost)
        {
        return aPoint;
        }

    CHuiLayout* container = iHost->ContainerLayout(this);

    return container->DisplayToLocal(aPoint) - container->Pos().Target();
    }


EXPORT_C TRect CHuiControl::Bounds() const
    {
    TPoint min;
    TPoint max;

    min.iX = KMaxTInt;
    min.iY = KMaxTInt;
    max.iX = KMinTInt;
    max.iY = KMinTInt;

    for(TInt i = 0; i < iVisuals.Count(); ++i)
        {
        TRect visualRect = iVisuals[i]->DisplayRectTarget();

        min.iX = Min(min.iX, visualRect.iTl.iX);
        min.iY = Min(min.iY, visualRect.iTl.iY);
        max.iX = Max(max.iX, visualRect.iBr.iX);
        max.iY = Max(max.iY, visualRect.iBr.iY);
        }

    return TRect(min, max);
    }


EXPORT_C TBool CHuiControl::HitTest(const TPoint& aPoint) const
    {
    return Bounds().Contains(aPoint);
    }



EXPORT_C void CHuiControl::AcquireFocus()
    {
    
    /** @todo  Focus should be set separately in each display the control
               has visuals in. */
    
    for(TInt i = 0; i < iVisuals.Count(); ++i)
        {
        if(iVisuals[i]->Display())
            {
            iVisuals[i]->Display()->Roster().SetFocus(*this /*Id()*/);
            //return;
            }
        }
    }   


EXPORT_C TBool CHuiControl::Focus() const
    {
    return iHasFocus;
    }


void CHuiControl::SetFocus(CHuiDisplay& aDisplay, TBool aHasFocus)
    {
    if ( ( iHasFocus && !aHasFocus ) || ( !iHasFocus && aHasFocus ) )
        {
        iHasFocus = aHasFocus;
        FocusChanged(aDisplay, aHasFocus);
        }
    }


EXPORT_C TBool CHuiControl::IsFocusing() const
    {
    return iFocusing;
    }


void CHuiControl::SetFocusing(TBool aFocusing)
    {
    iFocusing = aFocusing;
    }


EXPORT_C void CHuiControl::FocusChanged(CHuiDisplay& /*aDisplay*/, TBool /*iFocused*/)
    {
    // Do nothing by default.
    }


EXPORT_C TBool CHuiControl::AcceptInput() const
    {
    if(iOwnerGroup)
        {
        return iOwnerGroup->AcceptInput();
        }
    return ETrue;    
    }


EXPORT_C TBool CHuiControl::OfferEventL(const THuiEvent& /*aEvent*/)
    {
    return EFalse;
    }


EXPORT_C TRect CHuiControl::DisplayArea() const
    {
    if(iBoundDisplay)
        {
        return iBoundDisplay->VisibleArea();
        }
    
    // The first visual shown on a display determines the display area.            
    for(TInt i = 0; i < iVisuals.Count(); ++i)
        {
        if(iVisuals[i]->Display())
            {
            return iVisuals[i]->Display()->VisibleArea();
            }
        }
        
    if(!Env().DisplayCount())
        {
        // No displays created in the environment yet. Assume device 
        // native resolution.
        return TRect(TPoint(0, 0), HuiUtil::ScreenSize());
        }
        
    // Assume it is the primary display, then.   
    return Env().PrimaryDisplay().VisibleArea();
    }


EXPORT_C THuiRealPoint CHuiControl::DisplayCenter() const __SOFTFP
    {
    TRect area(DisplayArea());
    return area.iTl + THuiRealPoint(area.Width()/2.f, area.Height()/2.f);
    }


EXPORT_C void CHuiControl::CancelAllCommands()
    {
    iEnv.CancelCommands(this);
    
    for(TInt i = 0; i < iVisuals.Count(); ++i)
        {
        iEnv.CancelCommands(iVisuals[i]);
        }
    }
    

void CHuiControl::ClearChanged()
    {
    }


EXPORT_C void CHuiControl::VisualLayoutUpdated(CHuiVisual& /*aVisual*/)
    {
    // Nothing done by default.
    }


EXPORT_C void CHuiControl::VisualDestroyed(CHuiVisual& aVisual)
    {
    Remove(&aVisual);
    }


EXPORT_C void CHuiControl::VisualPrepareDrawFailed(
#ifdef _DEBUG
    #ifdef __WINS__
        #ifdef HUI_NO_DEBUG_OUTPUT_IN_WINS
            CHuiVisual& /*aVisual*/, 
            TInt /*aErrorCode*/
        #else
            CHuiVisual& /*aVisual*/, 
            TInt /*aErrorCode*/
        #endif    
    #else
        CHuiVisual& /*aVisual*/, 
        TInt /*aErrorCode*/
    #endif
#else    
    CHuiVisual& /*aVisual*/, 
    TInt /*aErrorCode*/
#endif
    )
	{
	//HUI_DEBUG2(_L("CHuiControl::VisualPrepareDrawFailed() - ERROR! Visual 0x%x failed to prepare for drawing. Error Code %i. Panicing. (TIP: override this method for custom error handling)."), 
	//           &aVisual, aErrorCode);
	#ifdef _DEBUG           
	HUI_PANIC(THuiPanic::EVisualPrepareDrawFailed)
	#endif
	}


EXPORT_C MHuiEventHandler* CHuiControl::EventHandler()
    {
    return this;
    }


HUI_SESSION_OBJECT_IMPL_EXPORT(CHuiControl, ETypeControl)

EXPORT_C void CHuiControl::ControlExtension(const TUid& /*aExtensionUid*/, TAny** /*aExtensionParams*/)
    {
    
    }


