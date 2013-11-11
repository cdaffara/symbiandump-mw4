/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Control base class.
*
*/



#include "alf/alfcontrol.h"
#include "alf/alfcontrolgroup.h"
#include "alf/alfdisplay.h"
#include "alf/alfroster.h"
#include "alf/alfenv.h"
#include "alf/alfvisualfactory.h"
#include "alfclient.h"
#include "alf/alfgencomponent.h"
#include "alf/alfconstants.h"
#include "alf/alftimedvalue.h"
#include "alflogger.h"

#include <uiacceltk/HuiUtil.h>

#include <eikappui.h>

// Private structure
struct CAlfControl::TPrivateData
    {
    CAlfEnv* iEnv;                      // Not owned.
    CAlfControlGroup* iOwnerGroup;      // Not owned.
    CAlfDisplay* iBoundDisplay;         // Not owned.
    TInt iId;                           // Owned.
    TInt iHostId;                       // Owned.
    TInt iRole;                         // Owned.
    RPointerArray<CAlfVisual> iVisuals; // Not owned.
    CAlfControl* iHost;                 // Not owned.
    struct SConnection
        {
        CAlfControl* iControl;          // Not owned.
        TInt iRole;                     // Owned.
        };
    RArray<SConnection> iConnections;   // Owned.
    TBool iFocusing;                    // Owned.
    TBool iHasFocus;                    // Owned.
    CAlfGenComponent* iComms;           // Owned
    };


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfControl::CAlfControl()
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::ConstructL(CAlfEnv& aEnv)
    {
    iData = new (ELeave) TPrivateData;
    
    iData->iEnv = &aEnv;
    iData->iFocusing = EFalse;
    //iData->iId = CAlfStatic::GenerateId();  
        
    // NULL data
    iData->iOwnerGroup = NULL;
    iData->iBoundDisplay = NULL;
    iData->iId = 0;
    iData->iHostId = 0;
    iData->iRole = 0;
    iData->iVisuals.Reset();
    iData->iHost = NULL;
    iData->iConnections.Reset();
    iData->iHasFocus = EFalse;
    iData->iComms = 0;
    iData->iComms = CAlfGenComponent::NewL(aEnv,
            EAlfCntrlCreate, 
            0, 
            KNullDesC8);

    iData->iId = Identifier(); // to have unique value 
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfControl::~CAlfControl()
    {
    if ( iData )
        { 
        // Cancel scheduled commands for this control.
        Env().CancelCustomCommands(this);
        Env().CancelCommands(this);

        if(iData->iHost)
            {
            iData->iHost->RemoveConnection(this);
            iData->iHost = NULL;
            }

        for(TInt i = iData->iConnections.Count() - 1; i >= 0; --i)
            {
            RemoveConnection(iData->iConnections[i].iControl);
            }
        iData->iConnections.Reset();

        // The visuals are owned by the control.
        iData->iVisuals.ResetAndDestroy();
        delete iData->iComms;
        }
    delete iData;
    iData = NULL;
    }

// ---------------------------------------------------------------------------
// Returns server handle
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfControl::Identifier() const
    {
    return iData->iComms->Identifier();
    }

// ---------------------------------------------------------------------------
// Returns the env
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfEnv& CAlfControl::Env() const
    {
    return *iData->iEnv;
    }


// ---------------------------------------------------------------------------
// Returns control group that this control is in.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfControlGroup* CAlfControl::ControlGroup() const
    {
    return iData->iOwnerGroup;
    }
    
// ---------------------------------------------------------------------------
// Sets the control group
// ---------------------------------------------------------------------------
//   
void CAlfControl::SetControlGroup(CAlfControlGroup& aOwnerGroup)
    {
    iData->iOwnerGroup = &aOwnerGroup;
    }
    
// ---------------------------------------------------------------------------
// Returns bound display
// ---------------------------------------------------------------------------
//     
EXPORT_C CAlfDisplay* CAlfControl::Display() const
    {
    return iData->iBoundDisplay;
    }
    
// ---------------------------------------------------------------------------
// Sets bound display
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAlfControl::BindDisplay(CAlfDisplay& aDisplay)
    {
    iData->iBoundDisplay = &aDisplay;
    }    

// ---------------------------------------------------------------------------
// Sets control ID.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::SetId(TInt aId)
    {
    iData->iId = aId;
    }

// ---------------------------------------------------------------------------
// Returns control ID.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfControl::Id() const
    {
    return iData->iId;
    }

// ---------------------------------------------------------------------------
// Appends new visual
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfControl::Append(CAlfVisual* aVisual, CAlfLayout* aParentLayout)
    {
    TInt err = KErrNone;
    
    ASSERT(aVisual); // must not be null
    
    if(&aVisual->Owner() != this) // ownership cannot be altered here
        {
        return KErrNotSupported;
        }
    
    // prevent adding visual several times
    for (TInt i = iData->iVisuals.Count()-1;i>=0;i--)
        {
        if (iData->iVisuals[i] == aVisual)
            {
            return KErrAlreadyExists;
            }
        }

    // Append the visual to the visuals array.
    err = iData->iVisuals.Append(aVisual);
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfControl::Append return error %d", err )
        return err;
        }

    if(aParentLayout)
        { // once parent layout has been set, the visual has already relation in shape 
          // server side, tell the layout that theres no need to synch that info to server 
          // anymore (latter param)
        err = aParentLayout->Append(aVisual, ETrue);  
        if ( err != KErrNone )
            {
            __ALFLOGSTRING1( "CAlfControl::Append return error %d", err )
            // Do cleanup
            iData->iVisuals.Remove(iData->iVisuals.Find(aVisual));
            return err;
            }
        }
     /*   
    if ( iData->iInformServerAboutAppendRemove )
        {
        TInt2 params(aVisual->Identifier(), aParentLayout ? aParentLayout ->Identifier() : 0 );
        TPckgC<TInt2> inPckg(params);
        TBuf8<1> dum;
        TInt err = iData->iComms->DoSynchronousCmd( EAlfCntrlAppendVisual, inPckg , dum );
        if ( err != KErrNone )
            {
            __ALFLOGSTRING1( "CAlfControl::Append return error %d", err )
            // Do cleanup
            if ( aParentLayout )
                {
                aParentLayout->Remove(aVisual);  
                }
            iData->iVisuals.Remove(iData->iVisuals.Find(aVisual));
            return err;
            }
        }*/

    TRAP(err, VisualAddedL(aVisual));
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfControl::Append return error %d", err )
        // Do cleanup
        if ( aParentLayout )
            {
            aParentLayout->Remove(aVisual);
            }
        Remove( aVisual );
        }
    return err;
    }

// ---------------------------------------------------------------------------
// Removes visual
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::Remove(CAlfVisual* aVisual)
    {
    /*
    if ( iData->iInformServerAboutAppendRemove && aVisual)
        {
        TPckgC<TInt> inPckg(aVisual->Identifier());
        TBuf8<1> dum;
        iData->iComms->DoSynchronousCmd( EAlfCntrlRemoveVisual, inPckg , dum );
        }*/
    
    TInt index = iData->iVisuals.Find(aVisual);
    if(index != KErrNotFound)
        {
        VisualRemoved(aVisual);
        iData->iVisuals.Remove(index);
        }
    }

// --------------------------------------------------------------------------
// Creates and appends new visual
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfVisual* CAlfControl::AppendVisualL(TAlfVisualType aVisualType,
                                                CAlfLayout* aParentLayout,
                                                TInt aImplementationUid )
    {
    CAlfVisual* visual = AlfVisualFactory::NewVisualLC(aVisualType, 
                                        aParentLayout, *this, *iData->iEnv, aImplementationUid);
                                                             
    TInt err = Append(visual, aParentLayout);
    User::LeaveIfError( err );
    CleanupStack::Pop(visual);
    return visual;
    }

// ---------------------------------------------------------------------------
// Creates and appends new layout
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfLayout* CAlfControl::AppendLayoutL(TAlfLayoutType aLayoutType,
                                                CAlfLayout* aParentLayout,
                                                TInt aImplementationUid)
    {
    CAlfLayout* layout = AlfVisualFactory::NewLayoutLC(aLayoutType,
                        aParentLayout, *this, *iData->iEnv, aImplementationUid);
     
    TInt err = Append(layout, aParentLayout);
    User::LeaveIfError( err );
    CleanupStack::Pop(layout);
    return layout;
    }

// ---------------------------------------------------------------------------
// Returns indexed visual
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfVisual& CAlfControl::Visual(TInt aIndex) const
    {
    return *iData->iVisuals[aIndex];
    }

// ---------------------------------------------------------------------------
// Returns visual count.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfControl::VisualCount() const
    {
    return iData->iVisuals.Count();
    }

// ---------------------------------------------------------------------------
// Finds visual with given tag.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfVisual* CAlfControl::FindTag(const TDesC8& aTag) const
    {
    ASSERT(iData);

    for(TInt i = 0; i < iData->iVisuals.Count(); ++i)
        {
        if(iData->iVisuals[i]->TagMatches(aTag))
            {
            return iData->iVisuals[i];
            }
        }   
        
    return NULL;
    }


// ---------------------------------------------------------------------------
// Returns host.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfControl* CAlfControl::Host() const
    {
    return iData->iHost;
    }

// ---------------------------------------------------------------------------
// Sets host.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::SetHost(CAlfControl* aHost)
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

    iData->iHost = aHost;
    }

// ---------------------------------------------------------------------------
// Adds control connection.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::AddConnectionL(CAlfControl* aConnectedControl, TInt aRole)
    {
    // check that the connection does not exist:
    if ( FindConnection( aConnectedControl ) != KErrNotFound )
        {
        User::Leave( KErrAlreadyExists );
        }
    
    TPrivateData::SConnection client;
    client.iControl = aConnectedControl;
    client.iRole = aRole;
    User::LeaveIfError(iData->iConnections.Append(client));

    // This control is now the client's host.
    aConnectedControl->SetHost(this);

    ConnectionAddedL(aConnectedControl, aRole);
    }

// ---------------------------------------------------------------------------
// Removes control connection
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::RemoveConnection(CAlfControl* aConnectedControl)
    {
    for(TInt i = 0; i < iData->iConnections.Count(); ++i)
        {
        if(iData->iConnections[i].iControl == aConnectedControl)
            {
            aConnectedControl->SetHost(NULL);
            const TInt role = iData->iConnections[i].iRole;
            iData->iConnections.Remove(i);
            
            ConnectionRemoved(aConnectedControl, role);
            return;
            }
        }
    // The client must exist.
    USER_INVARIANT();
    }

// ---------------------------------------------------------------------------
// Returns connection count
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfControl::ConnectionCount() const
    {
    return iData->iConnections.Count();
    }

// ---------------------------------------------------------------------------
// Returns indexed connection.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfControl& CAlfControl::Connection(TInt aIndex) const
    {
    return *iData->iConnections[aIndex].iControl;
    }

// ---------------------------------------------------------------------------
// Returns ordinal-indexed connection.
// @deprecated
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfControl& CAlfControl::ConnectionByOrdinal(TInt aOrdinal) const
    {
    // First look based on role.
    for(TInt i = 0; i < iData->iConnections.Count(); ++i)
        {
        if(iData->iConnections[i].iRole == aOrdinal + 1)
            {
            return *iData->iConnections[i].iControl;
            }
        }

    // Fall back to index.
    return Connection(aOrdinal);
    }

// ---------------------------------------------------------------------------
// Returns given connection index.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfControl::FindConnection(const CAlfControl* aConnected) const
    {
    for(TInt i = 0; i < iData->iConnections.Count(); ++i)
        {
        if(iData->iConnections[i].iControl == aConnected)
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// Returns connection role
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfControl::ConnectionRole(TInt aIndex) const
    {
    return iData->iConnections[aIndex].iRole;
    }

// ---------------------------------------------------------------------------
// Returns connection ordinal
// @deprecated
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfControl::ConnectionOrdinal(TInt aIndex) const
    {
    if(iData->iConnections[aIndex].iRole)
        {
        return iData->iConnections[aIndex].iRole - 1;
        }
    return aIndex;
    }

// ---------------------------------------------------------------------------
// Returns role
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfControl::Role() const
    {
    /*if(iHost)
        {
        return iHost->ClientRole(iHost->FindClient(this));
        }
    // Zero is the default role.
    return 0;*/
    return iData->iRole;
    }

// ---------------------------------------------------------------------------
// Sets role
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::SetRole(TInt aRole)
    {
    iData->iRole = aRole;
    }

// ---------------------------------------------------------------------------
// Returns host ID.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfControl::HostId() const
    {
    return iData->iHostId;
    }

// ---------------------------------------------------------------------------
// Sets host ID
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::SetHostId(TInt aHostId)
    {
    // If adding automatic visual host, there cannot be already one defined.
    __ASSERT_ALWAYS( !iData->iHost, USER_INVARIANT() );
    
    iData->iHostId = aHostId;
    }

// ---------------------------------------------------------------------------
// Returns container layout if set.
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfLayout* CAlfControl::ContainerLayout(const CAlfControl* /*aConnected*/) const
    {
    // Generic controls aren't able to provide container layouts.
    return NULL;
    }

// ---------------------------------------------------------------------------
// Called when new visual is added.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::VisualAddedL(CAlfVisual* aVisual)
    {
    // Add the new visual to the container layout.
    if(iData->iHost && !aVisual->Layout())
        {
        CAlfLayout* container = iData->iHost->ContainerLayout(this);
        if(container)
            {
            User::LeaveIfError( container->Append(aVisual) );
            }
        }
    else
        {
        // @todo Calling ShowVisualL panics the server, becuase it has
        // been called already on the construction.
        /*
        // If the control has been bound to a display (for example, when
        // the control is shown), new visuals will be automatically shown.
        if( iData->iBoundDisplay && 
                (iData->iOwnerGroup && 
                 iData->iBoundDisplay->Roster().Find(
                    *iData->iOwnerGroup) != KErrNotFound) && 
           !aVisual->Layout() && !aVisual->Display())
            {
            iData->iBoundDisplay->Roster().ShowVisualL(*aVisual);
            }
            */
        }
    }

// ---------------------------------------------------------------------------
// Called when visual is removed.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::VisualRemoved(CAlfVisual* aVisual)
    {
    // Add the new visual to the container layout.
    if(iData->iHost)
        {
        CAlfLayout* container = iData->iHost->ContainerLayout(this);
        if(container && aVisual->Layout() == container )
            {
            container->Remove(aVisual);
            }
        }
    }

// ---------------------------------------------------------------------------
// Called when connection is added.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::ConnectionAddedL(CAlfControl* /*aConnectedControl*/, TInt /*aRole*/)
    {
    // Do nothing.
    }

// ---------------------------------------------------------------------------
// Called when connection is removed.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::ConnectionRemoved(CAlfControl* /*aConnectedControl*/, TInt /*aRole*/)
    {
    // Do nothing.
    }

// ---------------------------------------------------------------------------
// Removes visual from layout.
// ---------------------------------------------------------------------------
//
void CAlfControl::RemoveVisualsFromHostControl( CAlfControl& aHostControl )
    {
    __ASSERT_ALWAYS( &aHostControl != this, USER_INVARIANT() );
    
    // Remove the visuals.
    for(TInt i = 0; i < iData->iVisuals.Count(); ++i)
        {
        if ( iData->iVisuals[i]->Layout() && &iData->iVisuals[i]->Layout()->Owner() == &aHostControl )
            {
            iData->iVisuals[i]->Layout()->Remove(iData->iVisuals[i]);
            
            // If the own control group is showing still in some roster,
            // we need to add the root visuals over there
            /* // do we need this?
            if ( iBoundDisplay )
                {
                TRAP_IGNORE( iBoundDisplay->Roster().ShowVisualL( iVisuals[i] ) );
                }*/
            }
        }
    }

// ---------------------------------------------------------------------------
// Called when the host is about to change
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::HostChangingL(CAlfControl* aNewHost)
    {
    CAlfLayout* newContainer = 0;
    TInt i = 0;

    if(aNewHost)
        {
        // The container layout provided by the new host.
        newContainer = aNewHost->ContainerLayout(this);
        }

    if(iData->iHost)
        {
        // There is a previous host.
        // Remove the visuals.
        RemoveVisualsFromHostControl(*iData->iHost);
        }

    if(newContainer)
        {
        for(i = 0; i < iData->iVisuals.Count(); ++i)
            {
            // Only the visuals that aren't already attached to a layout
            // are added to the container layout.
            if(!iData->iVisuals[i]->Layout())
                {
                User::LeaveIfError( newContainer->Append(iData->iVisuals[i]) );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Shows conttol on the given display
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::ShowL(CAlfDisplay& aDisplay)
    {
    if(HostId())
        {
        CAlfControl* host = Env().FindControl(HostId());

        if(host)
            {
            host->AddConnectionL(this, Role());
            }
        else
            {
            /** @todo Leave? */
            }
        }

    BindDisplay(aDisplay);

    // @todo: do we need this? the roster::show is already passed
    // to the server side, which call the same thing.
    
    // Show all the visuals on the specified display.
    for(TInt i = 0; i < iData->iVisuals.Count(); ++i)
        {

        // Set the display of the root visual
        if( !iData->iVisuals[i]->Layout() )
            {
            iData->iVisuals[i]->SetDisplay( &aDisplay );
            }
        }
        
    NotifyControlVisibility(ETrue, aDisplay);
    }

// ---------------------------------------------------------------------------
// Hides control from the given display
// ---------------------------------------------------------------------------
//
void CAlfControl::Hide(CAlfDisplay& aDisplay)
    {
    NotifyControlVisibility(EFalse, aDisplay);
    
    // Hide all the visuals on the specified display.
    for(TInt i = 0; i < iData->iVisuals.Count(); ++i)
        {
        // The visuals that are part of a layout will be shown when the
        // layout is shown.
        if(!iData->iVisuals[i]->Layout() && iData->iVisuals[i]->Display() == &aDisplay)
            {
            aDisplay.Roster().HideVisual(*iData->iVisuals[i]);
            }
        }

    // Relinquish focus on this display
    CAlfRoster& roster = aDisplay.Roster();
    if ( roster.FocusedControl() == this )
        {
        roster.ClearFocus();
        }
    
    // Unbind from display?
    if(iData->iBoundDisplay == &aDisplay)
        {
        iData->iBoundDisplay = NULL;
        }

    // Unlink from parent control.
    if(Host() && HostId() )
        {
        Host()->RemoveConnection(this);
        }
    }

// ---------------------------------------------------------------------------
// Called when visibility changes.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::NotifyControlVisibility(TBool /*aIsVisible*/, 
                                                   CAlfDisplay& /*aDisplay*/)
    {
    // Nothing to do by default.
    }

// ---------------------------------------------------------------------------
// Converts point from relative to absolute coordinates
// ---------------------------------------------------------------------------
//
EXPORT_C TPoint CAlfControl::HostToDisplay(const TPoint& aPoint) const
    {
    if(!iData->iHost)
        {
        return aPoint;
        }

    CAlfLayout* container = iData->iHost->ContainerLayout(this);

    return container->LocalToDisplay(aPoint) + container->Pos().Target();
    }

// ---------------------------------------------------------------------------
// Converts point from absolute to relative coordinates
// ---------------------------------------------------------------------------
//
EXPORT_C TPoint CAlfControl::DisplayToHost(const TPoint& aPoint) const
    {
    if(!iData->iHost)
        {
        return aPoint;
        }

    CAlfLayout* container = iData->iHost->ContainerLayout(this);

    return container->DisplayToLocal(aPoint) - container->Pos().Target();
    }

// ---------------------------------------------------------------------------
// Returns bounding rect.
// ---------------------------------------------------------------------------
//
EXPORT_C TRect CAlfControl::Bounds() const
    {
    TPoint min;
    TPoint max;

    min.iX = KMaxTInt;
    min.iY = KMaxTInt;
    max.iX = KMinTInt;
    max.iY = KMinTInt;

    for(TInt i = 0; i < iData->iVisuals.Count(); ++i)
        {
        TRect visualRect = iData->iVisuals[i]->DisplayRectTarget();

        min.iX = Min(min.iX, visualRect.iTl.iX);
        min.iY = Min(min.iY, visualRect.iTl.iY);
        max.iX = Max(max.iX, visualRect.iBr.iX);
        max.iY = Max(max.iY, visualRect.iBr.iY);
        }

    return TRect(min, max);
    }

// ---------------------------------------------------------------------------
// Is the given point inside the control area?
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfControl::HitTest(const TPoint& aPoint) const
    {
    return Bounds().Contains(aPoint);
    }

    
// ---------------------------------------------------------------------------
// Tries to get the focus.
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfControl::AcquireFocus()
    {
    if ( !AcceptInput() ) // Cannot take focus to this control
        {
        return;
        }

    // Focus is set set separately in each display the control has visuals in.
    // Find all the displays
    RPointerArray<CAlfDisplay> displays;
    for(TInt i = 0; i < iData->iVisuals.Count(); ++i)
        {
        if(iData->iVisuals[i]->Display())
            {
            // Do not allow duplicates. Ignore error.
            (void)displays.InsertInAddressOrder( iData->iVisuals[i]->Display() );
            }
        }
    
    // go through the displays
    for ( TInt d = 0 ; d < displays.Count() ; d++ )
        {
        if ( displays[d]->Roster().FocusedControl() != this )
            {
            displays[d]->Roster().SetFocus(*this );
            }
        }
    
    displays.Close();
    }   

// ---------------------------------------------------------------------------
// Attempt to remove the focus from this control
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfControl::RelinquishFocus()
    {
    // Focus needs to be removed from all displays that the control has visuals in
    for(TInt i = 0; i < iData->iVisuals.Count(); ++i)
        {
        if(iData->iVisuals[i]->Display())
            {
            // This method should have no effect on rosters where this does not match the currently focused control
            CAlfRoster& roster = iData->iVisuals[i]->Display()->Roster();
            if ( roster.FocusedControl() == this )
                {
                roster.ClearFocus(); // Potentially called repeatedly for each visual. Optimization is in CAlfRoster
                }
            }
        }
    }   
// ---------------------------------------------------------------------------
// Does this control have focus?
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfControl::Focus() const
    {
    return iData->iHasFocus;
    }

// ---------------------------------------------------------------------------
// Is one of the child focused?
// ---------------------------------------------------------------------------
//   
EXPORT_C CAlfControl* CAlfControl::FocusedConnection() const
    {
    CAlfControl* focusChild = NULL;
    
    const TInt connectionCount = ConnectionCount();
    for ( TInt c = 0 ; c < connectionCount && !focusChild ; c++ )
        {
        CAlfControl& childConnection =  Connection(c);
        if ( childConnection.Focus() || childConnection.FocusedConnection() )
            {
            focusChild = &childConnection;
            }
        }
        
    return focusChild;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//      
void CAlfControl::FocusChainChanged( TBool aInFocusChain )
    {
    TAny* pInFocusChain = &aInFocusChain;
    PropertyOwnerExtension( KUidAlfPropOwnerExtControlFocusChainChanged, &pInFocusChain );
    }

// ---------------------------------------------------------------------------
// Sets focus
// ---------------------------------------------------------------------------
//
void CAlfControl::SetFocus(CAlfDisplay& aDisplay, TBool aHasFocus)
    {
    if ( ( iData->iHasFocus && !aHasFocus ) || 
         ( !iData->iHasFocus && aHasFocus ) )
        {
        iData->iHasFocus = aHasFocus;
        FocusChanged(aDisplay, aHasFocus);
        }
    }

// ---------------------------------------------------------------------------
// Is focusing control?
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfControl::IsFocusing() const
    {
    return iData->iFocusing;
    }

// ---------------------------------------------------------------------------
// Set is focusing control.
// ---------------------------------------------------------------------------
//
void CAlfControl::SetFocusing(TBool aFocusing)
    {
    iData->iFocusing = aFocusing;
    }

// ---------------------------------------------------------------------------
// Called when focus changes.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::FocusChanged(CAlfDisplay& /*aDisplay*/, TBool /*aFocused*/)
    {
    // Do nothing by default.
    }

// ---------------------------------------------------------------------------
// Does control group accept input?
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfControl::AcceptInput() const
    {
    if(iData->iOwnerGroup)
        {
        return iData->iOwnerGroup->AcceptInput();
        }
    return ETrue;    
    }


// ---------------------------------------------------------------------------
// Called when event is received.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfControl::OfferEventL(const TAlfEvent& /*aEvent*/)
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Returns control area.
// ---------------------------------------------------------------------------
//
EXPORT_C TRect CAlfControl::DisplayArea() const
    {
    if(iData->iBoundDisplay)
        {
        return iData->iBoundDisplay->VisibleArea();
        }
    
    // The first visual shown on a display determines the display area.            
    for(TInt i = 0; i < iData->iVisuals.Count(); ++i)
        {
        if(iData->iVisuals[i]->Display())
            {
            return iData->iVisuals[i]->Display()->VisibleArea();
            }
        }
        
    if(!Env().DisplayCount())
        {
        // No displays created in the environment yet. Assume device 
        // native resolution.
        //return TRect(TPoint(0, 0), AlfUtil::ScreenSize());
        CCoeEnv* coe = CCoeEnv::Static();
        if ( coe )
            {
            return static_cast<CEikAppUi*>(coe->AppUi())->ClientRect(); 
            }
        else
            {
            return TRect( 0,0,0,0 );
            }
        }
        
    // Assume it is the primary display, then.   
    return Env().PrimaryDisplay().VisibleArea();
    }

// ---------------------------------------------------------------------------
// Returns the center point of the area.
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfRealPoint CAlfControl::DisplayCenter() const
    {
    TRect area(DisplayArea());
    return area.iTl + TAlfRealPoint(area.Width()/2.f, area.Height()/2.f);
    }

// ---------------------------------------------------------------------------
// Cancels all commands
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::CancelAllCommands()
    {
    iData->iEnv->CancelCustomCommands(this);
    iData->iEnv->CancelCommands(this);
    
    for(TInt i = 0; i < iData->iVisuals.Count(); ++i)
        {
        iData->iEnv->CancelCommands(iData->iVisuals[i]);
        }
    }
    
// ---------------------------------------------------------------------------
// Clear flag.
// ---------------------------------------------------------------------------
//
void CAlfControl::ClearChanged()
    { // todo: does client need to access this information
    }

// ---------------------------------------------------------------------------
// Called when layout changes.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::VisualDestroyed(CAlfVisual& aVisual)
    {
    // If called from CAlfVisual::RemoveAndDestroyAllD the server
    // side object is removed also automatically. Otherwise not..
    Remove(&aVisual);
    }

// ---------------------------------------------------------------------------
// Called when visual layout is updated (see visual flags)
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfControl::VisualLayoutUpdated(CAlfVisual& /*aVisual*/)
    {
    
    }
  
// ---------------------------------------------------------------------------
// Called when draw preparation failed (not implemented currenltly)
// ---------------------------------------------------------------------------
// 
EXPORT_C void CAlfControl::VisualPrepareDrawFailed(CAlfVisual& /*aVisual*/, TInt /*aErrorCode*/)
    {
    
    }

// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfControl::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfPropertyOwner::PropertyOwnerExtension(aExtensionUid, aExtensionParams);
    }

