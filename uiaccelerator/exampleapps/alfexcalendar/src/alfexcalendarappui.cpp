/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ?Description
*
*/



#include <avkon.hrh>
#include <aknnotewrappers.h> 
#include <aknutils.h>
#include <eikmenup.h>

#include <alf/alfenv.h> 
#include <alf/alfdisplay.h> 
#include <alf/alfcontrolgroup.h> 
#include <alf/alfroster.h> 
#include <alf/alftransformation.h>

#include "alfexcalendar.pan"
#include "alfexcalendarappui.h"
#include "alfexcalendar.hrh"
#include "alfexcalendardeckcontrol.h"
#include "alfexcalendarengine.h"
#include "alfexcalendar.rsg"

// Control group ID
const TInt KAlfExCalendarControlGroupDeckId = 1;

// --------------------------------------------------------------------------
// CAlfExCalendarAppUi::ConstructL
// --------------------------------------------------------------------------
// 
void CAlfExCalendarAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin );

    // Create Toolkit Environment
    iEnv = CAlfEnv::NewL();
    
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );

    // Create Display
    CAlfDisplay& display = 
        iEnv->NewDisplayL( rect, CAlfEnv::ENewDisplayAsCoeControl );
    display.SetClearBackgroundL( CAlfDisplay::EClearWithSkinBackground );

    // instantiate CalendarEngine
    // This is dummy engine which generates and give hardcoded calendar events
    iCalendarEngine = CAlfExCalendarEngine::NewL();
    
    // Create Control Group for Deck Layout
    CAlfControlGroup& deckGroup =
        iEnv->NewControlGroupL( KAlfExCalendarControlGroupDeckId );

    // Create Control and Append to Control Group2
    CAlfExCalendarDeckControl* deckControl =
        CAlfExCalendarDeckControl::NewLC( *iEnv, *iCalendarEngine );
    deckGroup.AppendL( deckControl );
    CleanupStack::Pop( deckControl );

    // Activate/Show the Control group on the Display
    display.Roster().ShowL( deckGroup );
    
    }

// --------------------------------------------------------------------------
// CAlfExCalendarAppUi::CAlfExCalendarAppUi
// --------------------------------------------------------------------------
// 
CAlfExCalendarAppUi::CAlfExCalendarAppUi()
    {
	// no implementation required
    }

// --------------------------------------------------------------------------
// CAlfExCalendarAppUi::~CAlfExCalendarAppUi
// --------------------------------------------------------------------------
// 
CAlfExCalendarAppUi::~CAlfExCalendarAppUi()
    {
    // delete Toolkit Environment
    delete iEnv;
    
    delete iCalendarEngine;
    }

// --------------------------------------------------------------------------
// CAlfExCalendarAppUi::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CAlfExCalendarAppUi::DynInitMenuPaneL( 
        TInt aResourceId,
        CEikMenuPane* aMenuPane )
    {
    // Not used currently. May be used if necessary in future
    switch ( aResourceId )
        {
        case R_ALFEXCALENDAR_MENU:
            {

            }
            break;
        }
    
    CAknAppUi::DynInitMenuPaneL( aResourceId, aMenuPane );
    }

// --------------------------------------------------------------------------
// CAlfExCalendarAppUi::HandleCommandL
// --------------------------------------------------------------------------
// 
void CAlfExCalendarAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        case EAlfExCalendarCmdHelp:
            {
            _LIT(message,"Alf Calendar");
            CAknInformationNote* informationNote =
                new (ELeave) CAknInformationNote;
            informationNote->ExecuteLD(message);
            }
            break;
            
        default:
            Panic( EAlfExCalendarBasicUi );
            break;
        }
    }

// --------------------------------------------------------------------------
// CAlfExCalendarAppUi::HandleResourceChangeL
// --------------------------------------------------------------------------
//
void CAlfExCalendarAppUi::HandleResourceChangeL( TInt aType )
    {
    // In order to handle resolution and orientation changes properly,
    // the HandleResourceChangeL method needs to be implemented

    CAknAppUi::HandleResourceChangeL( aType );

    if ( iEnv && aType == KEikDynamicLayoutVariantSwitch )
        {
        iEnv->NotifyLayoutChangedL();
        }
    }

// --------------------------------------------------------------------------
// CAlfExCalendarAppUi::HandleWsEventL
// Calls HUI environment to release the resources when KAknFullOrPartialForegroundLost
// is received and to restore the resources when KAknFullOrPartialForegroundGained.
// Resources that are released/restored are memory loaded bitmaps.
// --------------------------------------------------------------------------
//
void CAlfExCalendarAppUi::HandleWsEventL(
        const TWsEvent& aEvent,
        CCoeControl* aDestination )
    {
    switch( aEvent.Type() )
        {
        // When the application is moved out of the foreground,
        // release all resources in the environment with CAlfEnv::Release()
        case KAknFullOrPartialForegroundLost:
            {
            iEnv->Release();
            break;
            }

        // When the application is brought to foreground,
        // restore all resources in the environment with CAlfEnv::RestoreL()        
        case KAknFullOrPartialForegroundGained:
            {
            iEnv->RestoreL();
            break;
            }
        default:
            {
            break;
            }
        }

    CAknAppUi::HandleWsEventL( aEvent, aDestination );
    }

// END OF FILE

