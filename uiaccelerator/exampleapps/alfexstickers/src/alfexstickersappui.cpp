/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   
*
*/


#include <avkon.hrh>
#include <aknnotewrappers.h> 

#include <alf/alfdisplay.h> 
#include <alf/alfenv.h> 
#include <alf/alfcontrolgroup.h>
#include <alf/alfroster.h>


#include "AlfExStickers.pan"
#include "AlfExStickersAppUi.h"
#include "AlfExStickers.hrh"
#include "AlfExStickersControl.h"

const TInt KAlfExStickersControlGroupId = 1;

// ConstructL is called by the application framework
void CAlfExStickersAppUi::ConstructL()
    {
    BaseConstructL(EAknEnableSkin);
    iEnv = CAlfEnv::NewL();
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, rect);
    CAlfDisplay& display = 
        iEnv->NewDisplayL(rect, CAlfEnv::ENewDisplayAsCoeControl);

    display.SetClearBackgroundL(CAlfDisplay::EClearWithSkinBackground);
    
    CAlfControlGroup& group = 
        iEnv->NewControlGroupL(KAlfExStickersControlGroupId);

    CAlfExStickersControl* control = CAlfExStickersControl::NewLC(*iEnv, &iCoeEnv->FsSession());
    group.AppendL(control);
    CleanupStack::Pop(control);
    
    iCoeEnv->FsSession();
    
    
    display.Roster().ShowL(group);
    
    }


CAlfExStickersAppUi::CAlfExStickersAppUi()                              
    {
	// no implementation required
    }


CAlfExStickersAppUi::~CAlfExStickersAppUi()
    {
    delete iEnv;
    }


// handle any menu commands
void CAlfExStickersAppUi::HandleCommandL(TInt aCommand)
    {
    switch(aCommand)
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;
          
        /*
        case EAlfExStickersCommand1:
            {
            _LIT(message,"Hello!");
            CAknInformationNote* informationNote = new (ELeave) CAknInformationNote;
            informationNote->ExecuteLD(message);
            }
            break;
		*/
        default:
            Panic(EAlfExStickersBasicUi);
            break;
        }
    }

void CAlfExStickersAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );

    if ( iEnv && aType == KEikDynamicLayoutVariantSwitch )
        {
        iEnv->NotifyLayoutChangedL();
        }
    }
    
void CAlfExStickersAppUi::HandleWsEventL(const TWsEvent &aEvent, CCoeControl* aDestination)
    {
    switch(aEvent.Type())
        {
        case KAknFullOrPartialForegroundLost:
            iEnv->Release();
            break;
        case KAknFullOrPartialForegroundGained:
            iEnv->RestoreL();
            break;
        default:
            break;
        }
    CAknAppUi::HandleWsEventL(aEvent, aDestination);
    }
