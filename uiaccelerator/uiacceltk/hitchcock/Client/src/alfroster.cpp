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
* Description:   Roster.
*
*/



#include "alf/alfroster.h"
#include "alf/alfdisplay.h"
#include "alf/alfcontrolgroup.h"
#include "alf/alfcontrol.h"
#include "alf/alfenv.h"
#include "alfclient.h"
#include "alf/alfevent.h"
#include "alflogger.h"
#include "alfpanic.h"

#include <uiacceltk/HuiUtil.h>

// Private data
    
struct CAlfRoster::TPrivateData
    {
    CAlfDisplay* iDisplay;                      // Not owned.
    RPointerArray<CAlfControlGroup> iEntries;    // Not owned. Kept in order.
    CAlfControl* iInputFocus;                   // Not owned.
    RPointerArray<MAlfControlGroupOrderChangedObserver> iControlGroupOrderChangedObservers; // Not owned.
    };


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfRoster::CAlfRoster()
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfRoster::ConstructL(CAlfDisplay* aDisplay)
    {
    iData = new (ELeave) TPrivateData;
    iData->iDisplay = aDisplay;
    iData->iInputFocus = NULL;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfRoster::~CAlfRoster()
    {
    if ( iData )
        {
        iData->iEntries.Close();
        iData->iControlGroupOrderChangedObservers.Close();
        }
    
    delete iData;
    iData = NULL;
    }


// ---------------------------------------------------------------------------
// Shows given control group.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfRoster::ShowL(CAlfControlGroup& aGroup, TInt aWhere )
    {
    // If the group is already shown on the display, just adjust its 
    // position according to the parameters.
    if(Find(aGroup) != KErrNotFound)
        {
        // It is already shown. Only adjust position.
        TInt newPos = aWhere;
        if(newPos == KAlfRosterShowAtTop)
            {
            newPos = Count() - 1;
            }
        else if(newPos == KAlfRosterShowAtBottom)
            {
            newPos = 0;
            }
        else
            {
            // for PC lint
            }
            
        // Move the control group to a new position in the roster.
        const TBool lWasMoved = Move(aGroup, newPos);
        
        // Move on the server side
        iData->iDisplay->Env().Client().RosterShow( 
              aGroup.Identifier(), 
                aWhere, 
                iData->iDisplay->ServerHandle() );                 
                
        //fix for bug  "ESMA-7L3H7F". Notification is moved from Move() to here.          
        // Notify control group order changed observers about changed control group order
        
         if(lWasMoved)
        	{
        	NotifyControlGroupOrderChangedObservers();		
        	}
                              
        return;
        }

    if(aWhere == KAlfRosterShowAtTop)
        {
        // Put the group on top of the roster.
        AppendL(aGroup);
        }
    else if(aWhere == KAlfRosterShowAtBottom)
        {
        InsertL(aGroup, 0);
        }
    else
        {
        InsertL(aGroup, aWhere);
        }


    // The group will automatically accept input events once shown.
    aGroup.SetAcceptInput(ETrue);

    // Show all the controls of the group.
    for(TInt i = 0; i < aGroup.Count(); ++i)
        {
        aGroup.Control(i).ShowL(*iData->iDisplay);
        }
    
    // Bind the control group to the correct display.
    aGroup.BindDisplay(iData->iDisplay);
        
    // Show on the server side
    TInt err = 
        iData->iDisplay->Env().Client().RosterShow( 
            aGroup.Identifier(), 
            aWhere, 
            iData->iDisplay->ServerHandle() );
            
    // hide on error
    if ( err != KErrNone )
        {
        // There is no window group ID if we are not running this from a 
        // Symbian application. Therefore the roster::show returns an error.
        // -> only concider this to be an error if the CCoeEnv is present.
        if ( err == KErrNotFound && !CCoeEnv::Static() )
            {
            __ALFLOGSTRING1( "CAlfRoster::ShowL ignore error %d (no CCoeEnv)", err )
            return;
            }
        
        // Hide all the controls of the group.
        for(TInt i = 0; i < aGroup.Count(); ++i)
            {
            aGroup.Control(i).Hide(*iData->iDisplay);
            }
        
        // Unbind the control group from the display.
        aGroup.BindDisplay(NULL);
               
        __ALFLOGSTRING1( "CAlfRoster::ShowL leave error %d", err )
        User::Leave( err );
        }
        
    // Notify control group order changed observers about changed control group order
    NotifyControlGroupOrderChangedObservers();
    }
    
// ---------------------------------------------------------------------------
// Hides given control group
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfRoster::Hide(CAlfControlGroup& aGroup)
    {
    // Hide on the server side
    TInt err = 
        iData->iDisplay->Env().Client().RosterHide( 
            aGroup.Identifier(), 
            iData->iDisplay->ServerHandle() );
            
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfRoster::Hide panic error %d", err )
        USER_INVARIANT();
        }
            
    // Hide all the controls of the group.
    for(TInt i = 0; i < aGroup.Count(); ++i)
        {
        aGroup.Control(i).Hide(*iData->iDisplay);
        }

    // Unbind the control group from the display.
    aGroup.BindDisplay(NULL);

    // Investigate whether control group was shown
    TBool wasShown = (Find(aGroup) == KErrNotFound) ? EFalse : ETrue;

    Remove(&aGroup);
    
    // Notify control group order changed observers about changed control group order
    // if control group was shown before.
    if(wasShown)
        {
        NotifyControlGroupOrderChangedObservers();
        }    
    }

// ---------------------------------------------------------------------------
// Returns index of the given group
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfRoster::Find(const CAlfControlGroup& aGroup) const
    {
    return iData->iEntries.Find( &aGroup );
    }

// ---------------------------------------------------------------------------
// Removes group
// ---------------------------------------------------------------------------
//    
void CAlfRoster::Remove(CAlfControlGroup* aGroup)
    {
    if ( aGroup )
        {
        TInt index = Find(*aGroup);
        if(index != KErrNotFound)
            {
            iData->iEntries.Remove(index);
            }
        }
    }
    
// ---------------------------------------------------------------------------
// Number of control groups
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfRoster::Count() const
    {
    return iData->iEntries.Count();
    }
   
// ---------------------------------------------------------------------------
// Moves group.
// ---------------------------------------------------------------------------
// 
TBool CAlfRoster::Move(CAlfControlGroup& aGroup, TInt aPos)
    {
    TInt index = Find(aGroup);
    TInt k = 0;
    
    if(index != KErrNotFound && index != aPos)
        {
        CAlfControlGroup* moving = iData->iEntries[index];
        
        if(aPos > index)
            {
            for(k = index; k < aPos; ++k)
                {
                iData->iEntries[k] = iData->iEntries[k + 1];
                }
            }
        else 
            {
            for(k = index; k > aPos; --k)
                {
                iData->iEntries[k] = iData->iEntries[k - 1];
                }
            }
        iData->iEntries[aPos] = moving;
        return ETrue;
        }
    return EFalse;    
    }

// ---------------------------------------------------------------------------
// Appends new group
// ---------------------------------------------------------------------------
//
void CAlfRoster::AppendL(CAlfControlGroup& aGroup)
    {
    User::LeaveIfError(iData->iEntries.Append(&aGroup));
    }
    
// ---------------------------------------------------------------------------
// Inserts new group
// ---------------------------------------------------------------------------
//
void CAlfRoster::InsertL(CAlfControlGroup& aGroup, TInt aPos)
    {
    if(Find(aGroup) != KErrNotFound)
        {
        User::Leave(KErrAlreadyExists);
        }

    User::LeaveIfError(iData->iEntries.Insert(&aGroup, aPos));
    }    
    
// ---------------------------------------------------------------------------
// Handles received event
// ---------------------------------------------------------------------------
//
TBool CAlfRoster::HandleEventL(const TAlfEvent& aEvent)
    {
    TInt i = 0;
    TInt k = 0;

    if(iData->iDisplay)
        {
        iData->iDisplay->Env().ContinueRefresh();
        }

    if(aEvent.IsPointerEvent())
        {
        // The order pointer events are offered in depends on the visuals.

        for(i = iData->iEntries.Count() - 1; i >= 0; --i)
            {
            CAlfControlGroup& group = *iData->iEntries[i];
            
            if(!group.AcceptInput())
                {
                // The group will not be receiving input events.
                continue;
                }
            
            for(k = group.Count() - 1; k >= 0; --k)
                {
                CAlfControl& control = group.Control(k);
                if(control.HitTest(aEvent.PointerEvent().iPosition))
                    {
                    if(control.OfferEventL(aEvent))
                        {
                        // The event was consumed, now it can't be handled by
                        // anyone else.
                        return ETrue;
                        }
                    }
                }
            }
        }
    else 
        {
        // First offer to the input control.
        if(aEvent.IsKeyEvent() && iData->iInputFocus && iData->iInputFocus->AcceptInput())
            {
            // check if the focus control has a host
            CAlfControl* focusRoot = iData->iInputFocus;
            while ( focusRoot->Host() )
                {
                focusRoot = focusRoot->Host();
                }
                
            if(focusRoot->OfferEventL(aEvent))
                {
                return ETrue;
                }
            }

        // Iterate through the groups and controls in priority order.
        for(i = iData->iEntries.Count() - 1; i >= 0; --i)
            {
            CAlfControlGroup& group = *iData->iEntries[i];
            
            if(!group.AcceptInput())
                {
                // The group will not be receiving input events.
                continue;
                }
            
            for(k = 0 ; k < group.Count() ; k++ )
                {
                CAlfControl& control = group.Control(k);
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
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfControlGroup& CAlfRoster::ControlGroup(TInt aIndex) const
    {
    return *iData->iEntries[aIndex];
    }
  
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfControl* CAlfRoster::FindControl(TInt aControlId) const
    {
    for(TInt i = 0; i < iData->iEntries.Count(); ++i)
        {
        CAlfControl* control = 
            iData->iEntries[i]->FindControl(aControlId);
        if(control)
            {
            return control;
            }
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfRoster::SetFocus(CAlfControl& aControl)
    {
    TRAP_IGNORE(ShowL( *aControl.ControlGroup(), KAlfRosterShowAtTop))
        
    // Something needs to change if these are not the same
    if ( &aControl != iData->iInputFocus ) 
        {
        ChangeInputFocus( &aControl );
        }
    }    

// ---------------------------------------------------------------------------
// Clears the input focus so that no control has focus.
// No change is implied for the visibility of the control group
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfRoster::ClearFocus()
    { 
    // Only do something if not cleared already
    if ( iData->iInputFocus != NULL ) 
        {
        ChangeInputFocus(NULL);
        }
    }
    
// ---------------------------------------------------------------------------
// The main job of this routine is to change iData->iInputFocus, and perform the 
// necessary co-ordinated activities.  This should be the only place to change this
// member datum. 
// ---------------------------------------------------------------------------
//      
void CAlfRoster::ChangeInputFocus( CAlfControl* aControl )
    {
    // This method will always do something except when both old value and new are NULL
    // If they are non-null and identical, then focus will be removed and then re-applied.
    // That is, any optimizations need to be done by the caller.
    
    // The policy of whether focus is altered in the AlfControl is maintained uniformly by storing the cone focus 
    // state at the beginning of the routine
    // Current implementation is for one display/roster.  If multiple displays added then this will have to be reconsidered
    TBool displayIsInitiallyFocused = iData->iDisplay ? iData->iDisplay->IsFocused() : EFalse;
    
    // There can be a latent focus only if display is unfocused.
    TBool latentFocus = ((iData->iInputFocus != NULL) && (!iData->iInputFocus->Focus()));    
    __ASSERT_DEBUG((latentFocus ? !displayIsInitiallyFocused : ETrue), User::Panic(_L("UIAcceltkClient"), EAlfRosterPanicFocusAnomaly));
    
    CAlfControl* oldFocusedControl = iData->iInputFocus;
    
    // Remove focus from the currently focused control
    if ( iData->iInputFocus )
        {
        // Remove focus from the control
        // Only need to change actual focus if the roster is focused
        if ( displayIsInitiallyFocused )
            {
            // This control should be focused at the moment or then there is a latent focus that we are manipulating.
            __ASSERT_DEBUG( iData->iInputFocus->Focus(), User::Panic(_L("UIAcceltkClient"),EAlfRosterPanicFocusAnomaly ));

            CAlfDisplay* display = iData->iInputFocus->Display();
            if ( display )
                {
               // SetFocus should cause the control to perform any required changes its visuals.
                iData->iInputFocus->SetFocus( *display, EFalse);
                }
            }

        // Still change the iInputFocus even if actual focus has not been removed
        if ( !aControl )
            {
            iData->iInputFocus = NULL;
            }
        }
    
    if ( aControl )
        {
        // Check that the control switched to is in this Roster. Ignore it if not.
        CAlfControl* control = FindControl(aControl->Id());
        if( control == aControl )
            {
            iData->iInputFocus = aControl;
            // This control should not be focused at the moment
            __ASSERT_DEBUG( !(iData->iInputFocus->Focus()), User::Panic(_L("UIAcceltkClient"),EAlfRosterPanicFocusAnomaly ));
            
            // Only manipulate actual focus if the roster is focused
            if ( displayIsInitiallyFocused )
                {               
                CAlfDisplay* display2 =  iData->iInputFocus->Display();
                if ( display2 )
                    {
                    iData->iInputFocus->SetFocus( *display2, ETrue);
                    }
                }
            }
        }
       
    ReportHostsAboutChangingInputFocus( oldFocusedControl, aControl );
    }    

// ---------------------------------------------------------------------------
// Inform host containers for the change 
// ---------------------------------------------------------------------------
//    
void CAlfRoster::ReportHostsAboutChangingInputFocus( 
    CAlfControl* aOldFocusControl, 
    CAlfControl* aNewFocusControl )
    {
    // collect new focused control host tree
    RPointerArray<CAlfControl> gainingHostControls;
    if ( aNewFocusControl ) 
        {
        CAlfControl* hostTmp = aNewFocusControl->Host();
        while ( hostTmp )
            {
            (void)gainingHostControls.Append( hostTmp );
            hostTmp = hostTmp->Host();
            }
        }
        
    // Go through the hosts 
    // 1. starting from the losing leaf host
    // 2. continue to the losing root or combined host
    // 3. start from the gaining root or combined host
    // 4. end to the gaining leaf host
    
    TInt combinedIndex = KErrNotFound;
    // 1
    if ( aOldFocusControl )
        {
        CAlfControl* losingHostTmp = aOldFocusControl->Host();
        while ( losingHostTmp && (combinedIndex == KErrNotFound ) ) // 2
            {
            combinedIndex = gainingHostControls.Find( losingHostTmp );
            if ( combinedIndex == KErrNotFound )
                {
                losingHostTmp->FocusChainChanged( EFalse );
                losingHostTmp = losingHostTmp->Host();
                }
            }
        }
        
    // 3
    TInt gainingStartIndex = gainingHostControls.Count() - 1;
    if ( combinedIndex != KErrNotFound )
        {
        gainingStartIndex = combinedIndex - 1; // If we want to inform the combined host, remove the '-1'
        }
        
    if ( gainingStartIndex >= 0 )
        {
        for ( TInt gainingHostIndex = gainingStartIndex ; gainingHostIndex >= 0 ; gainingHostIndex-- ) // 4
            {
            gainingHostControls[gainingHostIndex]->FocusChainChanged( ETrue );
            }
        }

    gainingHostControls.Close();
    }
    
// ---------------------------------------------------------------------------
// Notifies registered control group order changed observers about changed
// order in control group relative order.
// ---------------------------------------------------------------------------
//    
void CAlfRoster::NotifyControlGroupOrderChangedObservers()
    {
    for(TInt i = 0; i < iData->iControlGroupOrderChangedObservers.Count(); i++)
        {
        iData->iControlGroupOrderChangedObservers[i]->NotifyControlGroupOrderChanged();
        }    
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfRoster::ShowVisualL(CAlfVisual& aVisual)
    {
    TInt err = 
        iData->iDisplay->Env().Client().RosterShowVisual( 
            aVisual.Identifier(), 
            iData->iDisplay->ServerHandle() );
            
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfRoster::ShowVisualL leave error %d", err )
        User::Leave( err );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfRoster::HideVisual(CAlfVisual& aVisual)
    {
    TInt err = 
        iData->iDisplay->Env().Client().RosterHideVisual( 
            aVisual.Identifier(), 
            iData->iDisplay->ServerHandle() );
            
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfRoster::HideVisual ignore error %d", err )
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
EXPORT_C void CAlfRoster::MoveVisualToFront(CAlfVisual& aRootVisual)
    {
    TInt err =
        iData->iDisplay->Env().Client().RosterMoveVisualToFront( 
            aRootVisual.Identifier(), 
            iData->iDisplay->ServerHandle() );
            
    if ( err != KErrNone )
        {
        __ALFLOGSTRING1( "CAlfRoster::MoveVisualToFront ignore error %d", err )
        }
    }

// ---------------------------------------------------------------------------
// Adds a pointer event observer
// ---------------------------------------------------------------------------
//  
EXPORT_C TInt CAlfRoster::AddPointerEventObserver( 
        TAlfPointerEventFlags aObserver, 
        CAlfControl& aControl )
    {
    switch ( aObserver )
        {
        case EAlfPointerEventReportDrag:
        case EAlfPointerEventReportLongTap:
        case EAlfPointerEventReportUnhandled:
            break;
        default:
            USER_INVARIANT();
            break;
        }
    
    TInt err =
        iData->iDisplay->Env().Client().RosterAddPointerEventObserver( 
            aObserver, 
            aControl.Identifier(), 
            iData->iDisplay->ServerHandle()  );
            
    return err;
    }

// ---------------------------------------------------------------------------
// Removes a pointer event observer
// ---------------------------------------------------------------------------
//  
EXPORT_C TInt CAlfRoster::RemovePointerEventObserver( 
        TAlfPointerEventFlags aObserver, 
        CAlfControl& aControl )
    {
    switch ( aObserver )
        {
        case EAlfPointerEventReportDrag:
        case EAlfPointerEventReportLongTap:
        case EAlfPointerEventReportUnhandled:
            break;
        default:
            USER_INVARIANT();
            break;
        }
        
    TInt err =
        iData->iDisplay->Env().Client().RosterRemovePointerEventObserver( 
            aObserver, 
            aControl.Identifier(), 
            iData->iDisplay->ServerHandle()  );
            
    return err;
    }

// ---------------------------------------------------------------------------
// Sets pointer event observer flags.
// ---------------------------------------------------------------------------
//     
EXPORT_C TInt CAlfRoster::SetPointerEventObservers( 
    TUint aPointerEventFlags, 
    CAlfControl& aControl )
    {
    TInt err =
        iData->iDisplay->Env().Client().RosterSetPointerEventObservers( 
            aPointerEventFlags, 
            aControl.Identifier(), 
            iData->iDisplay->ServerHandle()  );
            
     return err;
    }
    
void CAlfRoster::DisplayFocusChanged( CAlfDisplay& aDisplay, TBool aNewFocus )
    {
    // This method should only be called during a real transition of display focus. If there is a 
    // roster focused control then alter it's focus state.
    if ( iData->iInputFocus )
        {
        iData->iInputFocus->SetFocus( aDisplay, aNewFocus );
        
        if (aNewFocus)
            {
            ReportHostsAboutChangingInputFocus( NULL, iData->iInputFocus );
            }
        else
            {
            ReportHostsAboutChangingInputFocus( iData->iInputFocus, NULL );
            }
            
        }
    }
    
CAlfControl* CAlfRoster::FocusedControl() const
    {
    return iData->iInputFocus;
    }

EXPORT_C TInt CAlfRoster::SetPointerDragThreshold(const CAlfControl& aControl, const TAlfXYMetric& aXYMetric)
	{
    TInt err =
        iData->iDisplay->Env().Client().RosterSetPointerDragTreshold( 
            aControl.Identifier(), 
            aXYMetric,
            iData->iDisplay->ServerHandle()  );
            
    return err;	
	}
	
EXPORT_C TInt CAlfRoster::DisableLongTapEventsWhenDragging(const CAlfControl& aControl, TBool aDisable)
	{
    TInt err =
        iData->iDisplay->Env().Client().RosterDisableLongTapEventsWhenDragging( 
            aControl.Identifier(), 
            aDisable,
            iData->iDisplay->ServerHandle()  );
            
    return err;	
	}

// ---------------------------------------------------------------------------
// Adds a control group order change observer into the observers array.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfRoster::AddControlGroupOrderChangedObserverL(MAlfControlGroupOrderChangedObserver& aObserver)
    {
	if(iData->iControlGroupOrderChangedObservers.Find(&aObserver) != KErrNotFound)
	    {
	    return;
	    }
	iData->iControlGroupOrderChangedObservers.AppendL(&aObserver);    
    }

// ---------------------------------------------------------------------------
// Removes a control group order change observer from the observers array.
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfRoster::RemoveControlGroupOrderChangedObserver(MAlfControlGroupOrderChangedObserver& aObserver)
    {
	TInt index = iData->iControlGroupOrderChangedObservers.Find(&aObserver);
	if(index != KErrNotFound)
	    {
	    iData->iControlGroupOrderChangedObservers.Remove(index);
	    }
    }
