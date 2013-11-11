/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of member functions of CAlfExAnalogDialerAppUi.
*
*/



#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <aknutils.h>
#include <akndef.h>                 // KAknFullOrPartialForegroundGained/Lost

#include <alf/alfcontrolgroup.h>
#include <alf/alfdisplay.h>
#include <uiacceltk/huidisplay.h>
#include <uiacceltk/huiutil.h>
#include <alf/alfcontrol.h>
#include <alf/alfanchorlayout.h>
#include <alf/alftextvisual.h>
#include <alf/alfroster.h>
#include <alf/alfevent.h>
#include <alf/alfclientbase.h>
#include <alf/alftransformation.h>

#include "alfexanalogdialer.pan"
#include "alfexanalogdialerappui.h"
#include "alfexanalogdialer.hrh"

#include "alfexanalogdialercontrol.h"

// Identifier for the primary control group.
const TInt KExAnalogControlPrimaryGroup = 1;

// Location of image files of analog dialer
_LIT(KAnalogDialerImagePath, "c:\\Data\\Images\\Pictures\\");

// ---------------------------------------------------------------------------
// CAlfExAnalogDialerAppUi
// ---------------------------------------------------------------------------
//
CAlfExAnalogDialerAppUi::CAlfExAnalogDialerAppUi()
    {
	// no implementation required
    }

// ---------------------------------------------------------------------------
// ConstructL
// ConstructL is called by the application framework
// ---------------------------------------------------------------------------
//
void CAlfExAnalogDialerAppUi::ConstructL()
    {
    // Enable Avkon skins.
    BaseConstructL(EAknEnableSkin);

    // create ALF environment
    iEnv = CAlfEnv::NewL();    

    // set location of images
    iEnv->TextureManager().SetImagePathL(KAnalogDialerImagePath);

    // get the application screen area. Neither status pane nor softkeys exist
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EApplicationWindow, rect);
    
    // create display
    CAlfDisplay& disp = iEnv->NewDisplayL(rect, CAlfEnv::ENewDisplayAsCoeControl);
    disp.SetClearBackgroundL(CAlfDisplay::EClearWithSkinBackground);

    // Create a control group.
    CAlfControlGroup& group = iEnv->NewControlGroupL(KExAnalogControlPrimaryGroup);

    // Create control for analog dialer and add it into control group
    CAlfExAnalogDialerControl* control = CAlfExAnalogDialerControl::NewLC(*iEnv);
    group.AppendL(control);
    CleanupStack::Pop(control);
    
    // Show the control group on the display.
    disp.Roster().ShowL( group );
    }

// ---------------------------------------------------------------------------
// ~CAlfExAnalogDialerAppUi()
// ---------------------------------------------------------------------------
//
CAlfExAnalogDialerAppUi::~CAlfExAnalogDialerAppUi()
    {
    delete iEnv;
    }

// ---------------------------------------------------------------------------
// HandleCommandL
// Handle commands from framework
// ---------------------------------------------------------------------------
//
void CAlfExAnalogDialerAppUi::HandleCommandL(TInt aCommand)
    {
    switch(aCommand)
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;
        default:
            CAknAppUi::HandleCommandL(aCommand);
            break;
        }
    }

// ----------------------------------------------------------
// HandleResourceChangeL
// Handle Resource Change Events: Display orientation switch
// ----------------------------------------------------------
//
void CAlfExAnalogDialerAppUi::HandleResourceChangeL(TInt aType)
    {
    CAknAppUi::HandleResourceChangeL(aType);

    if(aType == KEikDynamicLayoutVariantSwitch)
        {
        CAlfControlGroup& group = iEnv->ControlGroup(KExAnalogControlPrimaryGroup);
        CAlfExAnalogDialerControl* control = static_cast<CAlfExAnalogDialerControl*> 
                                    ( group.FindControl(KAlfExAnalogDialerControlId));
        if (control)
            {
            // TODO: experimental code
            control->PrepareForLayoutSwitchL();
            }

        TRect rect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EApplicationWindow, rect);
        iEnv->PrimaryDisplay().SetVisibleArea(rect);    
        if (control)
            {
            control->SwitchLayoutL();
            }
        }
    }

// ----------------------------------------------------------
// HandleWsEventL
// Handle Resource Change Events: Display orientation switch
// ----------------------------------------------------------
//
void CAlfExAnalogDialerAppUi::HandleWsEventL(   const TWsEvent& aEvent,
	                                            CCoeControl*    aDestination)
    { 
    // When the application is put to the background, release resources.
    // This is not mandatory.
	switch(aEvent.Type()) 
    	{ 
    	case KAknFullOrPartialForegroundLost:
        	//iEnv->Release();
        	break;

    	case KAknFullOrPartialForegroundGained:
        	//iEnv->RestoreL();
        	break;
    
      	default:
        	break;
    	}

    CAknAppUi::HandleWsEventL(aEvent, aDestination);
    }


