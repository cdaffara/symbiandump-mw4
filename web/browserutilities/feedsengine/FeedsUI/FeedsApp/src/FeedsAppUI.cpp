/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Feeds application UI instance.
*
*/

#include <e32std.h>

#include "FeedsApp.hrh"
#include "FeedsAppUi.h"
#include "LeakTracker.h"
#include "TestHarnessView.h"

#include "Logger.h"


// -----------------------------------------------------------------------------
// CFeedsAppUi::ConstructL()
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFeedsAppUi::ConstructL()
    {
    // Initialise app UI
    BaseConstructL(EAknEnableSkin);

    // Init the engine and UI
    InitEngineL();
    InitUIL();

    LEAK_TRACKER_INCREMENT(CLeakTracker::EFeedsAppUi);
    }


// -----------------------------------------------------------------------------
// CFeed::~CFeed
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
CFeedsAppUi::~CFeedsAppUi()
    {
    LEAK_TRACKER_DECREMENT(CLeakTracker::EFeedsAppUi);
    }

// -----------------------------------------------------------------------------
// CFeedsAppUi::HandleCommandL()
//
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CFeedsAppUi::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
        {
        case EShowTestHarnessView:
            ActivateLocalViewL(TUid::Uid(EFeedsTestHarnessViewUid));
            break;
            
		case EAknCmdHelp:    	        
            break;

        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        default:
            break;
        }
    }


// -----------------------------------------------------------------------------
// CFeedsAppUi::InitEngineL.
//
// Inits the Engine.
// -----------------------------------------------------------------------------
//
void CFeedsAppUi::InitEngineL()
    {
    }


// -----------------------------------------------------------------------------
// CFeedsAppUi::InitUIL.
//
// Inits the UI.
// -----------------------------------------------------------------------------
//
void CFeedsAppUi::InitUIL()
    {
    CAknView*  view = NULL;

    // Add the Folder View.
    view = CTestHarnessView::NewL();
    CleanupStack::PushL(view);
    AddViewL(view);
    CleanupStack::Pop(view);

    SetDefaultViewL(*view);
    }
