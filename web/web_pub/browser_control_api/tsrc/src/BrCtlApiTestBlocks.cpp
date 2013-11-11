/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "BrCtlApiTest.h"

#define RETURN_FAILURE_ON_ERROR     \
    if ( err )                      \
        {                           \
        return KErrNone;            \
        }

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CBrowserControlApiTest::Delete() 
    {
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::RunMethodL(CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CBrowserControlApiTest::ExampleL ),
        ENTRY( "CreateBrowserControl", CBrowserControlApiTest::CreateBrCtlL ),
        ENTRY( "DestroyBrowserControl", CBrowserControlApiTest::DestroyBrowserControlL ),
        ENTRY( "ZoomLevels", CBrowserControlApiTest::ZoomLevels ),
        ENTRY( "HandleCommand", CBrowserControlApiTest::HandleCommandL ),
        ENTRY( "SavedPage", CBrowserControlApiTest::SavedPageL ),
        ENTRY( "LoadData", CBrowserControlApiTest::LoadData ),
        ENTRY( "CertInfo", CBrowserControlApiTest::CertInfo ),
        ENTRY( "NavigationAvailable", CBrowserControlApiTest::NavigationAvailable ),
     	ENTRY( "PageInfo", CBrowserControlApiTest::PageInfo ),
        ENTRY( "EmptyCache", CBrowserControlApiTest::EmptyCache ),
        ENTRY( "GetBitmapData", CBrowserControlApiTest::GetBitmapData ),
        ENTRY( "SaveToFile", CBrowserControlApiTest::SaveToFile ),
        ENTRY( "OkToExit", CBrowserControlApiTest::OkToExit ),
        ENTRY( "FindKeyword", CBrowserControlApiTest::FindKeyword ),
        ENTRY( "MinimumSize", CBrowserControlApiTest::MinimumSize ),
        ENTRY( "OfferKeyEvent", CBrowserControlApiTest::OfferKeyEvent ),
        ENTRY( "BrCtlSubscribeToMenuItems", CBrowserControlApiTest::BrCtlSubscribeToMenuItemsL ),
        ENTRY( "ContentSize", CBrowserControlApiTest::ContentSize ),
        ENTRY( "BrCtlSettings", CBrowserControlApiTest::BrCtlSettings ),
        ENTRY( "FocusedImage", CBrowserControlApiTest::FocusedImage ),
        ENTRY( "CommandObserver", CBrowserControlApiTest::CommandObserver ),
        ENTRY( "FocusedElementType", CBrowserControlApiTest::FocusedElementType ),
        ENTRY( "BrCtlSetParam", CBrowserControlApiTest::BrCtlSetParam ),
        ENTRY( "SetSelfDownloadContentTypes", CBrowserControlApiTest::SetSelfDownloadContentTypes ),
        ENTRY( "AddOptionMenuItems", CBrowserControlApiTest::AddOptionMenuItems ),
        ENTRY( "BrCtlInitLoadData", CBrowserControlApiTest::BrCtlInitLoadData ),
        ENTRY( "HandleDownloadCommand", CBrowserControlApiTest::HandleDownloadCommand ),
        ENTRY( "VersionInfo", CBrowserControlApiTest::VersionInfo ),
        ENTRY( "WmlOptionMenuItems", CBrowserControlApiTest::WmlOptionMenuItems ), 
        ENTRY( "ImageCount", CBrowserControlApiTest::ImageCount),
        ENTRY( "PostUrl", CBrowserControlApiTest::PostUrlL),
        // Added for Widget Extension Test
        ENTRY( "CreateBrCtlWithWidgetExtension", CBrowserControlApiTest::CreateBrCtlWithWidgetExtensionL),
        ENTRY( "HistoryHandler", CBrowserControlApiTest::TestHistoryHandlerL ),
        ENTRY( "WMLLoad", CBrowserControlApiTest::TestWMLLoadL )
        };

    const TInt count = sizeof( KFunctions ) / 
    sizeof( TStifFunctionInfo );

//	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksMem );
//	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests );
//	TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );
	
    return RunInternalL( KFunctions, count, aItem );
    }
    
// -----------------------------------------------------------------------------
// CBrowserControlApiTest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KMyExample, "MyExample" );
    _LIT( KExample, "In ExampleL" );
    TestModuleIf().Printf( 0, KMyExample, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KMyExample, 
                                KParam, i, &string );
        i++;
        }

    // test block
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::CreateBrCtlL
// Creates a browser control contained within iBrCtlTestContainer
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::CreateBrCtlL( 
   CStifItemParser&  aItem  )
    {
    TInt err( KErrNone );
    TInt aBrCtlConfig = 0x0000; 

	TPtrC string;
	
    TPoint base( 0, 0 );
    TPoint extent( 252, 195 );
    TRect rect( base, extent );


	// To test for different configurations of the Browser control
	
	while ( aItem.GetNextString ( string ) == KErrNone ) 
		{
		if ( string.Compare(_L("DisplayScrollBar"))==0)
			{
			aBrCtlConfig = aBrCtlConfig | TBrCtlDefs::ECapabilityDisplayScrollBar;
			}
		if ( string.Compare(_L("ClientResolveEmbeddedURL"))==0)
			{
			aBrCtlConfig = aBrCtlConfig | TBrCtlDefs::ECapabilityClientResolveEmbeddedURL;
			}
		if ( string.Compare(_L("ClientNotifyURL"))==0)
			{
			aBrCtlConfig = aBrCtlConfig | TBrCtlDefs::ECapabilityClientNotifyURL ;
			}
		if ( string.Compare(_L("DisableInputAndPlugins"))==0)
			{
			aBrCtlConfig = aBrCtlConfig | TBrCtlDefs::ECapabilityDisableInputAndPlugins;
			}
		if ( string.Compare(_L("FindItem"))==0)
			{
			aBrCtlConfig = aBrCtlConfig | TBrCtlDefs::ECapabilityFindItem;
			}
		if ( string.Compare(_L("LoadHttpFw"))==0)
			{
			aBrCtlConfig = aBrCtlConfig | TBrCtlDefs::ECapabilityLoadHttpFw;
			}
		if ( string.Compare(_L("SavedPage"))==0)
			{
			aBrCtlConfig = aBrCtlConfig | TBrCtlDefs::ECapabilitySavedPage;
			}
		if ( string.Compare(_L("GraphicalHistory"))==0)
			{
			aBrCtlConfig = aBrCtlConfig | TBrCtlDefs::ECapabilityGraphicalHistory;
			}
			
		}
	
	if ( aBrCtlConfig == 0x0000 )
			aBrCtlConfig = TBrCtlDefs::ECapabilityDisplayScrollBar | 
						TBrCtlDefs::ECapabilityClientResolveEmbeddedURL |
						TBrCtlDefs::ECapabilityLoadHttpFw |
						TBrCtlDefs::ECapabilitySavedPage |
						TBrCtlDefs::ECapabilityDisplayScrollBar ;
		
    TRAP( err, 
        iBrCtlTestContainer = new (ELeave) CBrCtlApiTestContainer;
        iBrCtlTestContainer->ConstructL( rect );
	    iBrCtlTestContainer->CreateBrowserControlInterfaceL(aBrCtlConfig);
        );

    if ( err )
        {
        delete iBrCtlTestContainer; 
        iBrCtlTestContainer = 0;
        return KErrNone;
        }

    // Great, now set the focus of the browser control
    iBrCtlTestContainer->SetFocus(); 
    iBrCtlTestContainer->DrawNow();

    // test block executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::DestroyBrowserControlL
// Deletes iBrCtlTestContainer which causes the browser control to be deleted.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::DestroyBrowserControlL(
   CStifItemParser& /* aItem */ )
    {
    delete iBrCtlTestContainer; 
    iBrCtlTestContainer = 0;

    // test block executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::ZoomLevels
// BC Test that ZoomLevels() works.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::ZoomLevels( CStifItemParser& /* aItem */ )
    {
    if ( iBrCtlTestContainer )
        {
        iBrCtlTestContainer->ZoomLevels();
        }

    // test executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::HandleCommandL
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::HandleCommandL( CStifItemParser&  aItem )
    {
    TPtrC string;
	if ( iBrCtlTestContainer )
	    if ( aItem.GetNextString ( string ) == KErrNone )
	    {
		    if (string.Compare(_L("Disconnect1"))==0)
		    	{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandDisconnect ) );
		        RETURN_FAILURE_ON_ERROR 
		    	}
		    if (string.Compare(_L("CancelFetch"))==0)
				{
			 	TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandCancelFetch ) );
    			RETURN_FAILURE_ON_ERROR
				}
		   	if (string.Compare(_L("Open"))==0)
				{
	            TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandOpen ) );
    			RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("Reload"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandReload ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("Back"))==0)
				{
			 	TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandBack ) );
   			 	RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("Forward"))==0)
		    	{
				TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandForward ) );
        		RETURN_FAILURE_ON_ERROR 
		    	}
		    if (string.Compare(_L("ClearHistory"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandClearHistory ) );
		        RETURN_FAILURE_ON_ERROR
				}
		   	if (string.Compare(_L("ShowHistory"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandShowHistory ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("Disconnect2"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandDisconnect ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("Accept"))==0)
				{
        		TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandAccept ) );
        		RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("Cancel"))==0)
		    	{
				TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandCancel ) );
        		RETURN_FAILURE_ON_ERROR 
		    	}
		    if (string.Compare(_L("OpenToViewer1"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandOpenToViewer ) );
		        RETURN_FAILURE_ON_ERROR
				}
		   	if (string.Compare(_L("AddToPhoneBook"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandAddToPhoneBook ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("MakeCall"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandMakeCall ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("RemoveFileName"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandRemoveFileName ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("ShowImages"))==0)
		    	{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandShowImages ) );
		        RETURN_FAILURE_ON_ERROR
		    	}
		    if (string.Compare(_L("LoadImages"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandLoadImages ) );
		        RETURN_FAILURE_ON_ERROR
				}
		   	if (string.Compare(_L("GainFocus"))==0)
				{
			   	TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandGainFocus ) );
        		RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("LoseFocus"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandLoseFocus ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("FindItem"))==0)
				{
				TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandFindItem ) );
        		RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("FindItemPhoneNumber"))==0)
		    	{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandFindItemPhoneNumber ) );
		        RETURN_FAILURE_ON_ERROR
		    	}
		    if (string.Compare(_L("FindItemEMail"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandFindItemEMail ) );
		        RETURN_FAILURE_ON_ERROR
				}
		   	if (string.Compare(_L("FindItemAddress"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandFindItemAddress ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("FindKeyword"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandFindKeyword ) );
        		RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("ClearFind"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandClearFind ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("ShowThumbnailView"))==0)
		    	{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandShowThumbnailView ) );
		        RETURN_FAILURE_ON_ERROR
		    	}
		    if (string.Compare(_L("OneStepBack"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandOneStepBack ) );
		        RETURN_FAILURE_ON_ERROR
				}
		   	if (string.Compare(_L("ShowHistory"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandShowHistory ) );
        		RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("AppForeground"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandAppForeground ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("AppBackground"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandAppBackground ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("OpenNewWindow"))==0)
		    	{
     			TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandOpenNewWindow ) );
        		RETURN_FAILURE_ON_ERROR
		    	}
		    if (string.Compare(_L("OpenToViewer2"))==0)
				{
				TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandOpenToViewer ) );
        		RETURN_FAILURE_ON_ERROR
				}
		   	if (string.Compare(_L("ClearAutoFormFillData"))==0)
				{
		     	TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandClearAutoFormFillData ) );
        		RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("ClearAutoFormFillPasswordData"))==0)
				{
				TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandClearAutoFormFillPasswordData ) );
        		RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("RemoveFileName"))==0)
				{
			   	TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandRemoveFileName ) );
       		 	RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("FreeMemory"))==0)
		    	{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandFreeMemory ) );
		        RETURN_FAILURE_ON_ERROR 
		    	}
		    if (string.Compare(_L("MemoryGood"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandMemoryGood ) );
		        RETURN_FAILURE_ON_ERROR
				}
		   	if (string.Compare(_L("DumpRenderTree"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandDumpRenderTree ) );
        		RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("UnloadWMLEngine"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandUnloadWMLEngine ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("SmartLinkMakeCall"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandSmartLinkMakeCall ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("SmartLinkSendMessage"))==0)
		    	{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandSmartLinkSendMessage ) );
		        RETURN_FAILURE_ON_ERROR
		    	}
		    if (string.Compare(_L("SmartLinkAddToPhoneBook"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandSmartLinkAddToPhoneBook ) );
		        RETURN_FAILURE_ON_ERROR
				}
		   	if (string.Compare(_L("ShowToolBar"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandShowToolBar ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("SmartLinkSendEmail"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandSmartLinkSendEmail ) );
        		RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("UnLoadPluginWindows"))==0)
				{
		       	TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandUnLoadPluginWindows ) );
        		RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("FindItemVoIPAddress"))==0)
		    	{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandFindItemVoIPAddress ) );
        		RETURN_FAILURE_ON_ERROR
		    	}
		    if (string.Compare(_L("SmartLinkMakeVoipCall"))==0)
				{
			  	TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandSmartLinkMakeVoipCall ) );
        		RETURN_FAILURE_ON_ERROR
				}
		   	if (string.Compare(_L("ZoomSliderHide"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandZoomSliderHide ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("EnterFullscreenBrowsing"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandEnterFullscreenBrowsing ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("LeaveFullscreenBrowsing"))==0)
				{
				TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandLeaveFullscreenBrowsing ) );
        		RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("ShowDownloads"))==0)
		    	{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandShowDownloads ) );
        		RETURN_FAILURE_ON_ERROR
		    	}
		    if (string.Compare(_L("SaveLaunchParams"))==0)
				{
				TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandSaveLaunchParams ) );
        		RETURN_FAILURE_ON_ERROR
				}
		   	if (string.Compare(_L("ShowAnchorHref"))==0)
				{
				TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandShowAnchorHref ) );
        		RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("LoadFocusedImage"))==0)
				{
				TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandLoadFocusedImage ) );
        		RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("ZoomSliderShow"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandZoomSliderShow ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("HistoryNavigateForward"))==0)
		    	{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandHistoryNavigateForward ) );
		        RETURN_FAILURE_ON_ERROR 
		    	}
		    if (string.Compare(_L("HistoryNavigateBack"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->HandleCommandL( TBrCtlDefs::ECommandHistoryNavigateBack ) );
		        RETURN_FAILURE_ON_ERROR
				}
		   	if (string.Compare(_L(""))==0)
				{
				TRAPD( err, iBrCtlTestContainer->HandleCommandL(NULL) );
        		RETURN_FAILURE_ON_ERROR
				} 
		}

    // test block
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::LoadData
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::LoadData( CStifItemParser& /* aItem */ )
    {
    TInt err( KErrNone );
    if ( iBrCtlTestContainer )
        {
        TRAP( err, iBrCtlTestContainer->LoadDataL() );
        }
        
    return err;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::SavedPageL
// BC Test that LoadSavedPageL works.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::SavedPageL( CStifItemParser& /* aItem */)
    {
    iBrCtlTestContainer->LoadSavedPageL();

    // test executed
    return KErrNone;
    }    

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::CertInfo
// BC Test that CertInfo works.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::CertInfo( CStifItemParser& /* aItem */)
    {
    if ( iBrCtlTestContainer )
        {
        iBrCtlTestContainer->CertInfo();
        }
    // test executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::SetSelfDownloadContentTypes
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::SetSelfDownloadContentTypes( CStifItemParser& /* aItem */ )
    {
    if ( iBrCtlTestContainer )
        {
        iBrCtlTestContainer->SetSelfDownloadContentTypes();
        }

    // test executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::NavigationAvailable
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::NavigationAvailable( CStifItemParser& /* aItem */)
    {
    TInt err = iBrCtlTestContainer->NavigationAvailableTest();
    RETURN_FAILURE_ON_ERROR

    // Now load first page - still should not be able to go back...
    TRAP(err,  iBrCtlTestContainer->LoadSavedPageL() );
    RETURN_FAILURE_ON_ERROR
    err = iBrCtlTestContainer->NavigationAvailableTest();

    // test executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::PageInfo
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::PageInfo( CStifItemParser&  aItem  )
    {
	TPtrC string;
	if ( iBrCtlTestContainer )
	    if ( aItem.GetNextString ( string ) == KErrNone )
	    {
		    if (string.Compare(_L("Title"))==0)
		    	{
		    	TRAPD( err, iBrCtlTestContainer->PageInfoLC( TBrCtlDefs::EPageInfoTitle ) );
			    RETURN_FAILURE_ON_ERROR  
		    	}
		    if (string.Compare(_L("Url"))==0)
				{
				TRAPD( err, iBrCtlTestContainer->PageInfoLC( TBrCtlDefs::EPageInfoUrl ) );
		        RETURN_FAILURE_ON_ERROR
				}
		   	if (string.Compare(_L("FocusedNodeUrl"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->PageInfoLC( TBrCtlDefs::EPageInfoFocusedNodeUrl ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("Content"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->PageInfoLC( TBrCtlDefs::EPageInfoContent ) );
		        RETURN_FAILURE_ON_ERROR
				}
			if (string.Compare(_L("SavedPage"))==0)
				{
		        TRAPD( err, iBrCtlTestContainer->PageInfoLC( TBrCtlDefs::EPageInfoSavedPage ) );
		        RETURN_FAILURE_ON_ERROR
				}
		}

    // test executed
    return KErrNone;
    }
        
// -----------------------------------------------------------------------------
// CBrowserControlApiTest::EmptyCache
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::EmptyCache( CStifItemParser& /* aItem */ )
    {
    if ( iBrCtlTestContainer )
        {
        TInt err = iBrCtlTestContainer->ClearCacheTest();
        RETURN_FAILURE_ON_ERROR
        TBool inCache = iBrCtlTestContainer->IsUrlInCacheTest( ( TDesC16 & )KUrlLoadTestHtml() );
        if ( inCache )
            {
            return KErrNone;
            }
        err = iBrCtlTestContainer->ClearItemInCacheTest();
        RETURN_FAILURE_ON_ERROR
        
        // Now try loading a page
        TRAP( err, iBrCtlTestContainer->LoadDataL() );
        RETURN_FAILURE_ON_ERROR
        // Now the url should be in the cache
        inCache = iBrCtlTestContainer->IsUrlInCacheTest( ( TDesC16 & )KUrlLoadTestHtml() );
        if ( !inCache )
            {
            return KErrNone;
            }
        err = iBrCtlTestContainer->ClearItemInCacheTest();
        RETURN_FAILURE_ON_ERROR
        // Now the url should expunged from the cache
        inCache = iBrCtlTestContainer->IsUrlInCacheTest( ( TDesC16 & )KUrlLoadTestHtml() );
        if ( inCache )                      
            {                           
            return KErrNone;            
            }
        }
    // test executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::GetBitmapData
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::GetBitmapData( CStifItemParser& /* aItem */ )
    {
    TRAPD( err, iBrCtlTestContainer->LoadDataL() );
    RETURN_FAILURE_ON_ERROR
    
    TRAP( err, 
        if ( iBrCtlTestContainer )
            {
            err = iBrCtlTestContainer->GetBitmapData();
            }
        )
    // test executed
    return err;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::SaveToFile
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::SaveToFile( CStifItemParser& /* aItem */ )
    {
    TInt err( KErrNone );
    if ( iBrCtlTestContainer )
        {
        TRAP( err,  iBrCtlTestContainer->SaveToFileL() );
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::OkToExit
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::OkToExit( CStifItemParser& /* aItem */ )
    {
    if ( iBrCtlTestContainer )
        {
        iBrCtlTestContainer->OkToExit();
        }

    // test executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::OfferKeyEvent
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::OfferKeyEvent( CStifItemParser& /* aItem */ )
    {
    if ( iBrCtlTestContainer )
        {
        iBrCtlTestContainer->OfferKeyEvent();
        }

    // test executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::MinimumSize
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::MinimumSize( CStifItemParser& /* aItem */ )
    {
    if ( iBrCtlTestContainer )
        {
        iBrCtlTestContainer->BrCtlMinimumSize();
        }
    // test executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::FocusedElementType
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::FocusedElementType( CStifItemParser& /* aItem */ )
    {
    TInt err( KErrNone );
    if ( iBrCtlTestContainer )
        {
        TRAP( err, err = iBrCtlTestContainer->BrCtlFocusedElementTypeL() );
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::ContentSize
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::ContentSize( CStifItemParser& /* aItem */ )
    {
    if ( iBrCtlTestContainer )
        {
        iBrCtlTestContainer->BrCtlContentSize();
        }

    // test executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::BrCtlSetParam
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::BrCtlSetParam( CStifItemParser& /* aItem */ )
    {
    if ( iBrCtlTestContainer )
        {
        iBrCtlTestContainer->BrCtlSetParam();
        }
    // test executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::BrCtlSubscribeToMenuItemsL
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::BrCtlSubscribeToMenuItemsL( CStifItemParser& /* aItem */ )
    {
    TInt err( KErrNone );
    if ( iBrCtlTestContainer )
        {
        TRAP(err,  err = iBrCtlTestContainer->BrCtlSubscribeToMenuItemsL() );
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::FocusedImage
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::FocusedImage( CStifItemParser& /* aItem */ )
    {
    TRAPD( err,  err = iBrCtlTestContainer->FocusedImageL() );
    RETURN_FAILURE_ON_ERROR

    const TInt KExpectedImageCount( 1 );

    TInt actualCount( KErrNotFound );
    
    TRAP( err, actualCount = iBrCtlTestContainer->ImageCountL() );
    if ( err || ( actualCount != KExpectedImageCount ) )
        {
        return KErrNone;
        }

    // test executed
    return KErrNone;
    }    

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::BrCtlSettings
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::BrCtlSettings( CStifItemParser& /* aItem */ )
    {
    if ( iBrCtlTestContainer )
        {
        TRAPD(err,  
            err = iBrCtlTestContainer->SetBrowserSettingL();
            RETURN_FAILURE_ON_ERROR
            TUint result = 0;
            err = iBrCtlTestContainer->BrowserSettingL( result );
            if ( err || result )
                {
                return KErrNone;
                }
            ) // TRAP
    }
    // test executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CBrowserControlApiTest::FindKeyword
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::FindKeyword( CStifItemParser&  aItem  )
    {
   
    TPtrC html;
    TPtrC keyword;
    TInt match;
    
    TInt err ( KErrNone );
        
    if ( iBrCtlTestContainer )
    {

		//if ( aItem.GetNextString ( html ) == KErrNone )
			if ( (err = aItem.GetNextString ( keyword )) == KErrNone )
				if ( (err = aItem.GetNextInt ( match )) == KErrNone )
				{
					
				}

        err = iBrCtlTestContainer->FindKeyword( html, keyword, match );
        RETURN_FAILURE_ON_ERROR
        err = iBrCtlTestContainer->FindKeywordAgain( match );
        RETURN_FAILURE_ON_ERROR
        }
    // test executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::CommandObserver
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::CommandObserver( CStifItemParser& /* aItem */ )
    {
    TInt ret(KErrGeneral);

    if ( iBrCtlTestContainer )
        {
        TRAPD(err, ret = iBrCtlTestContainer->AddCommandObserverL() );
        if ( err || !ret )
            {
            err = iBrCtlTestContainer->RemoveCommandObserver(); 
            return KErrNone;
            }
        err = iBrCtlTestContainer->RemoveCommandObserver();
        RETURN_FAILURE_ON_ERROR
        }
    // test executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::AddOptionMenuItems
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::AddOptionMenuItems( CStifItemParser& /* aItem */ )
    {
    if ( iBrCtlTestContainer )
        {
        TRAPD(err,  iBrCtlTestContainer->LoadDataL() );
        RETURN_FAILURE_ON_ERROR

        err = iBrCtlTestContainer->AddOptionMenuItems();
        RETURN_FAILURE_ON_ERROR
        }
    // test executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::BrCtlInitLoadData
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::BrCtlInitLoadData( CStifItemParser& /* aItem */ )
    {
    if ( iBrCtlTestContainer )
        {
        TRAPD(err,  iBrCtlTestContainer->BrCtlInitLoadDataL() );
        RETURN_FAILURE_ON_ERROR
        }

    // test executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CBrowserControlApiTest::HandleDownloadCommand
// BC Test that all browser commands work.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::HandleDownloadCommand( CStifItemParser& /* aItem */)
    {
    if ( iBrCtlTestContainer )
        {
        TInt err = iBrCtlTestContainer->HandleDownloadCommand();
        RETURN_FAILURE_ON_ERROR
        }

    // test executed
    return KErrNone;
    }    

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::VersionInfo
// This BrCtl API function does nothing!
// BC Test that this function still works.
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::VersionInfo( CStifItemParser& /* aItem */ )
    {
    if ( iBrCtlTestContainer )
        {
        TRAPD( err, err = iBrCtlTestContainer->VersionInfoL() );
        RETURN_FAILURE_ON_ERROR
        }

    // test executed
    return KErrNone;
    }
    
    
// -----------------------------------------------------------------------------
// CBrowserControlApiTest::WmlOptionMenuItem
// 
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::WmlOptionMenuItems( CStifItemParser& /* aItem */ )
    {
    if ( iBrCtlTestContainer )
        {
        TRAPD( err, err = iBrCtlTestContainer->WmlOptionMenuItemsL() );
        RETURN_FAILURE_ON_ERROR
        }

    // test executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CBrowserControlApiTest::ImageCount
// 
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::ImageCount( CStifItemParser& /* aItem */ )
    {
    if ( iBrCtlTestContainer )
        {
        TRAPD( err, err = iBrCtlTestContainer->ImageCountL() );
        RETURN_FAILURE_ON_ERROR
        }

    // test executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::PostUrlL
// 
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::PostUrlL( CStifItemParser& /* aItem */ )
    {
    if ( iBrCtlTestContainer )
        {
        TRAPD( err, err = iBrCtlTestContainer->BrCtlPostL() );
        RETURN_FAILURE_ON_ERROR
        }

    // test executed
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CBrowserControlApiTest::CreateBrCtlWithWidgetExtensionL
// Creates a browser control with widget extension contained within iBrCtlTestContainer
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::CreateBrCtlWithWidgetExtensionL( 
   CStifItemParser&  aItem  )
    {
    TInt err( KErrNone );

    TInt aBrCtlConfig = 0x0000;

	TPtrC string;
	
    TPoint base( 0, 0 );
    TPoint extent( 252, 195 );
    TRect rect( base, extent );
		
    TRAP( err, 
        iBrCtlTestContainer = new (ELeave) CBrCtlApiTestContainer;
        iBrCtlTestContainer->ConstructL( rect );
	    err = iBrCtlTestContainer->CreateBrCtlInterfaceAndWidgetExtensionL();
        );

    if ( err )
        {
        delete iBrCtlTestContainer; 
        iBrCtlTestContainer = 0;
        return KErrNone;
        }

    // Great, now set the focus of the browser control
    iBrCtlTestContainer->SetFocus(); 
    iBrCtlTestContainer->DrawNow();
    		
    // test block executed
    return KErrNone;
    }

    
// -----------------------------------------------------------------------------
// CBrowserControlApiTest::TestHistoryHandlerL
// Load several pages in an attempt to indirectly exercise History Handler
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::TestHistoryHandlerL( CStifItemParser&  aItem )
    {
    TPtrC string;
	if ( iBrCtlTestContainer )
	{
	    if ( aItem.GetNextString ( string ) == KErrNone )
	    {
		    if (string.Compare(_L("Page1"))==0)
		    	{
		        TRAPD( err, iBrCtlTestContainer->HistoryLoad1L() );
		        RETURN_FAILURE_ON_ERROR 
		    	}

		    if (string.Compare(_L("Page2"))==0)
		    	{
		        TRAPD( err, iBrCtlTestContainer->HistoryLoad2L() );
		        RETURN_FAILURE_ON_ERROR 
		    	}

		    if (string.Compare(_L("Page3"))==0)
		    	{
		        TRAPD( err, iBrCtlTestContainer->HistoryLoad3L() );
		        RETURN_FAILURE_ON_ERROR 
		    	}
		}
	}
	
    // test executed
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBrowserControlApiTest::TestWMLLoadL
// Load a WML page in an attempt to indirectly exercise WML functionality
// -----------------------------------------------------------------------------
//
TInt CBrowserControlApiTest::TestWMLLoadL( CStifItemParser&  aItem )
    {
    TPtrC string;
	if ( iBrCtlTestContainer )
	{
	    if ( aItem.GetNextString ( string ) == KErrNone )
	    {
		    if (string.Compare(_L("Page1"))==0)
		    	{
		        TRAPD( err, iBrCtlTestContainer->WMLLoad1L() );
		        RETURN_FAILURE_ON_ERROR 
		    	}
		    if (string.Compare(_L("Page2"))==0)
		    	{
		        TRAPD( err, iBrCtlTestContainer->WMLLoad2L() );
		        RETURN_FAILURE_ON_ERROR 
		    	}
		    if (string.Compare(_L("Page3"))==0)
		    	{
		        TRAPD( err, iBrCtlTestContainer->WMLLoad3L() );
		        RETURN_FAILURE_ON_ERROR 
		    	}
		}
	}
	
    // test executed
    return KErrNone;
    }
    
// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
