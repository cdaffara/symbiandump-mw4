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
* Description:  TestHarness view
*
*/


#include <aknviewappui.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <EikMenup.h>

#include <FeedsApp.Rsg>
#include "FeedsApp.hrh"

#include "FeedsAppUi.h"
#include "LeakTracker.h"
#include "TestHarness.h"
#include "TestHarnessView.h"
#include "TestHarnessContainer.h"


// -----------------------------------------------------------------------------
// CTestHarnessView::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTestHarnessView* CTestHarnessView::NewL()
	{
    CTestHarnessView* self = new (ELeave) CTestHarnessView();

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
	}


// -----------------------------------------------------------------------------
// CTestHarnessView::CTestHarnessView
//
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CTestHarnessView::CTestHarnessView()
	{
    LEAK_TRACKER_INCREMENT(CLeakTracker::ETestHarnessView);
	}


// -----------------------------------------------------------------------------
// CTestHarnessView::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTestHarnessView::ConstructL()
    {
    BaseConstructL(R_FEEDS_TEST_HARNESS_VIEW);

    delete iHarness;
    iHarness = NULL;
    
    iHarness = CTestHarness::NewL(*this);            
    }


// -----------------------------------------------------------------------------
// CTestHarnessView::~CTestHarnessView
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
CTestHarnessView::~CTestHarnessView()
    {
    DoDeactivate();

    delete iHarness;
    delete iContainer;

    LEAK_TRACKER_DECREMENT(CLeakTracker::ETestHarnessView);
    }


// -----------------------------------------------------------------------------
// CTestHarnessView::ShowL
//
// Shows the settings view.
// -----------------------------------------------------------------------------
//
void CTestHarnessView::ShowL(TUid /*aRestoreToId*/)
    {
    CFeedsAppUi*    appUi = NULL;
    TUid            viewId;

    // Activate the view.
    appUi = static_cast<CFeedsAppUi*>(CCoeEnv::Static()->AppUi());

    viewId.iUid = EFeedsTestHarnessViewUid;
    appUi->ActivateLocalViewL(viewId);
    }


// -----------------------------------------------------------------------------
// CTestHarnessView::Id
//
// Returns the id of the view.
// -----------------------------------------------------------------------------
//
TUid CTestHarnessView::Id() const
    {
    return TUid::Uid(EFeedsTestHarnessViewUid);
    }


// -----------------------------------------------------------------------------
// CTestHarnessView::HandleCommandL
//
// Processes commands.
// -----------------------------------------------------------------------------
//
void CTestHarnessView::HandleCommandL(TInt aCommand)
    {   
    switch (aCommand)
        {
        case EFeedsAppCmd0:
            iHarness->Start(CTestHarness::ECanned);
            break;
            
        case EFeedsAppCmd1:
            iHarness->Start(CTestHarness::ECannedAtom);
            break;

        case EFeedsAppCmd2:
            break;

		case EFeedsAppCmd3:    	        
            iHarness->FetchRootFolderItemL();
            break;

		case EFeedsAppCmd4:    	        
            iHarness->WatchFolderListL();
            break;

		case EFeedsAppCmd5:    	        
            // make sure to extract the file to \epoc32\winscw\c
            iHarness->ImportOPMLL( _L("c:/test.opml") );
            break;

		case EFeedsAppCmd6:    	        
            iHarness->UpdateAllL();
            break;

		case EFeedsAppCmd7:    	        
            iHarness->AddFolderItemL( ETrue );
            break;

		case EFeedsAppCmd8:    	        
            iHarness->AddFolderItemL( EFalse );
            break;

		case EFeedsAppCmd9:    	        
            iHarness->ChangeFolderItemL();
            break;

		case EFeedsAppCmd10:    	        
            iHarness->DeleteFolderItemsL();
            break;

		case EFeedsAppCmd11:    	        
            iHarness->DeleteAllFolderItemsL();
            break;

		case EFeedsAppCmd12:    	        
            iHarness->MoveFolderItemsToL();
            break;

		case EFeedsAppCmd13:    	        
            iHarness->MoveFolderItemsL();
            break;

		case EFeedsAppCmd14:    	        
            iHarness->UpdateFolderItemsL();
            break;

		case EFeedsAppCmd15:    	        
            iHarness->FetchFeedL();
            break;

		case EFeedsAppCmd16:    	        
            iHarness->UpdateFeedItemStatusL();
            break;

		case EFeedsAppCmd17:    	        
            iHarness->FetchServerSettingsSyncL();
            break;

		case EFeedsAppCmd18:    	        
            iHarness->UpdateSettingsL( ETrue );
            break;

		case EFeedsAppCmd19:    	        
            iHarness->UpdateSettingsL( EFalse );
            break;

		case EFeedsAppCmd20:    	        
            iHarness->DisconnectL();
            break;

		case EFeedsAppCmd21:    	        
            // make sure to extract the file to \epoc32\winscw\c
            iHarness->ImportOPMLL( _L("c:/test2.opml"), ETrue );
            break;

		case EFeedsAppCmd22:    	        
            iHarness->DebugPrintTablesL();
            break;

		case EFeedsAppCmd23:    	        
            iHarness->FetchServerSettingsL();
            break;

        default:
        	{
            AppUi()->HandleCommandL(aCommand);
            break;
        	}
        }
    }


// -----------------------------------------------------------------------------
// CTestHarnessView::DoActivateL
//
// Called when the view is activated.
// -----------------------------------------------------------------------------
//
void CTestHarnessView::DoActivateL(const TVwsViewId& /*aPrevViewId*/, 
        TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
		iContainer = CTestHarnessContainer::NewL(*this, ClientRect());
        iContainer->SetMopParent(this);
        }

    if (!iContainerOnStack)
        {
        AppUi()->AddToStackL(*this, iContainer);
        iContainer->MakeVisible(ETrue);
        iContainerOnStack = ETrue;
        }
    }


// -----------------------------------------------------------------------------
// CTestHarnessView::DoDeactivate
//
// Called when the view is deactivated.
// -----------------------------------------------------------------------------
//
void CTestHarnessView::DoDeactivate()
    {
    if (iContainerOnStack)
        {
        AppUi()->RemoveFromViewStack(*this, iContainer);
        iContainer->MakeVisible(EFalse);
        iContainerOnStack = EFalse;
        }
    }


// -----------------------------------------------------------------------------
// CTestHarnessView::Completed
//
// Notifies the observer that the test harness is completed.
// -----------------------------------------------------------------------------
//
void CTestHarnessView::Completed()
    {
    }


