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
#include "WebViewApiTest.h"

#include "WebViewApiTestContainer.h"
#include <BrCtl.h>


#define RETURN_FAILURE_ON_ERROR     \
    if ( err )                      \
        {                           \
        return KErrNone;            \
        }

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWebViewApiTest::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CWebViewApiTest::Delete()
    {
    }

// -----------------------------------------------------------------------------
// CWebViewApiTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CWebViewApiTest::RunMethodL(CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "CreateWebView",      		CWebViewApiTest::CreateWebViewL ),
        ENTRY( "DestroyWebView",     		CWebViewApiTest::DestroyWebView ),
        ENTRY( "MakeVisible",        		CWebViewApiTest::testMakeVisible ),
        ENTRY( "MakeViewVisible",    		CWebViewApiTest::testMakeViewVisible ),
        ENTRY( "MainFrame",          		CWebViewApiTest::testMainFrame ),
        ENTRY( "MainFrameDocument",  		CWebViewApiTest::testMainFrameDocument ),
        ENTRY( "Page",               		CWebViewApiTest::testPage ),
        ENTRY( "MIMETypeRegisteredAsPlugin",CWebViewApiTest::testMIMETypeRegisteredAsPlugin),
        ENTRY( "PluginForExtension",		CWebViewApiTest::testPluginForExtension ),
        //ENTRY( "LoadBackForwardListFromOtherView",		CWebViewApiTest::testLoadBackForwardListFromOtherView),
        ENTRY( "ShouldClose",				CWebViewApiTest::testShouldClose ),
        ENTRY( "IsLoading",					CWebViewApiTest::testIsLoading ),
        ENTRY( "SyncRepaint",				CWebViewApiTest::testSyncRepaint ),
        ENTRY( "ScheduleRepaint",			CWebViewApiTest::testScheduleRepaint ),
        ENTRY( "DoRepaint",				  	CWebViewApiTest::testDoRepaint ),
        ENTRY( "PageLoadFinished",			CWebViewApiTest::testPageLoadFinished ),
        ENTRY( "UpdatePageScaler",			CWebViewApiTest::testUpdatePageScaler ),
        ENTRY( "DoLayout",			        CWebViewApiTest::testDoLayout ),
        ENTRY( "MaxBidiSize",		        CWebViewApiTest::testMaxBidiSize ),
        ENTRY( "ClearOffScreenBitmap",		CWebViewApiTest::testClearOffScreenBitmap ),
        ENTRY( "ScrollBuffer",		  		CWebViewApiTest::testScrollBuffer ),
        ENTRY( "OffscreenRect",		  		CWebViewApiTest::testOffscreenRect ),
        ENTRY( "PageScaler",		  		CWebViewApiTest::testPageScaler ),
        ENTRY( "ScalingFactor",		  		CWebViewApiTest::testScalingFactor ),
		ENTRY( "OpenUrl",		  			CWebViewApiTest::testOpenUrl ),
		ENTRY( "BrCtl",		  				CWebViewApiTest::testBrCtl ),
		ENTRY( "FepTextEditor",		  		CWebViewApiTest::testFepTextEditor ),
		ENTRY( "SetFormFillPopup",		  	CWebViewApiTest::testSetFormFillPopup ),
		ENTRY( "FormFillPopup",		  		CWebViewApiTest::testFormFillPopup ),
		ENTRY( "SetEditable",		  		CWebViewApiTest::testSetEditable ),
		ENTRY( "IsEditable",		  		CWebViewApiTest::testIsEditable ),
		ENTRY( "OfferKeyEvent",		  		CWebViewApiTest::testOfferKeyEvent ),
		ENTRY( "SetFocusedElementType",		CWebViewApiTest::testSetFocusedElementType ),
		ENTRY( "FocusedElementType",		CWebViewApiTest::testFocusedElementType ),
		ENTRY( "PageScrollHandler",			CWebViewApiTest::testPageScrollHandler ),
		ENTRY( "PageFullScreenHandler",		CWebViewApiTest::testPageFullScreenHandler ),
		ENTRY( "PointerEventHandler",		CWebViewApiTest::testPointerEventHandler ),
		ENTRY( "UpdateScrollbars",			CWebViewApiTest::testUpdateScrollbars ),
		ENTRY( "OpenPageView",				CWebViewApiTest::testOpenPageView ),
		ENTRY( "ClosePageView",				CWebViewApiTest::testClosePageView ),
		ENTRY( "CancelPageView",			CWebViewApiTest::testCancelPageView ),
		ENTRY( "PageView",					CWebViewApiTest::testPageView ),
		ENTRY( "InPageViewMode",			CWebViewApiTest::testInPageViewMode ),
		ENTRY( "SearchFor",					CWebViewApiTest::testSearchFor ),
		ENTRY( "ExitFindState",				CWebViewApiTest::testExitFindState ),
		ENTRY( "InFindState",				CWebViewApiTest::testInFindState ),
		ENTRY( "PageScalerEnabled",			CWebViewApiTest::testPageScalerEnabled ),
		ENTRY( "IsSmallPage",				CWebViewApiTest::testIsSmallPage ),
		ENTRY( "WillSubmitForm",			CWebViewApiTest::testWillSubmitForm )
		
        };

    const TInt count = sizeof( KFunctions ) /
    sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
    }

// -----------------------------------------------------------------------------
// CWebViewApiTest::CreateWebViewL
// Creates a browser control contained within iWebViewTestContainer
// -----------------------------------------------------------------------------
//
TInt CWebViewApiTest::CreateWebViewL(CStifItemParser& /* aItem */)
    {
    TInt err( KErrNone );
    iWebViewTestContainer = CWebViewApiTestContainer::NewL();
    TRAP( err, iWebViewTestContainer->CreateWebViewL());
    // test block executed
    return err;
    }

// -----------------------------------------------------------------------------
// CWebViewApiTest::DestroyWebViewL
// Deletes iView which causes the web view to be deleted
// -----------------------------------------------------------------------------
//
TInt CWebViewApiTest::DestroyWebView(CStifItemParser& /* aItem */)
    {
    delete iWebViewTestContainer;
    iWebViewTestContainer = 0;

    // test block executed
    return KErrNone;
    }

TInt CWebViewApiTest::testMakeVisible(CStifItemParser& /* aItem */)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		TRAP( err, iWebViewTestContainer->MakeWebViewVisible(ETrue) );
    RETURN_FAILURE_ON_ERROR
		TRAP( err, iWebViewTestContainer->MakeWebViewVisible(EFalse) );
    RETURN_FAILURE_ON_ERROR
	}
	// test block executed
	return err;
}

TInt CWebViewApiTest::testMakeViewVisible(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		TRAP( err, iWebViewTestContainer->MakeViewVisible(ETrue) );
    RETURN_FAILURE_ON_ERROR
		TRAP( err, iWebViewTestContainer->MakeViewVisible(EFalse) );
    RETURN_FAILURE_ON_ERROR
	}
	// test block executed
	return err;
}

TInt CWebViewApiTest::testMainFrame(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	TRAP( err,
        if ( iWebViewTestContainer )
            {
            err = iWebViewTestContainer->mainFrame();
            }
        )
    // test block executed
    return err;
}

TInt CWebViewApiTest::testMainFrameDocument(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->mainFrameDocument();
	}
	return err;
}

TInt CWebViewApiTest::testPage(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->page();
	}
	return err;
}

TInt CWebViewApiTest::testMIMETypeRegisteredAsPlugin(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->isMIMETypeRegisteredAsPlugin();
	}
	return err;
}

TInt CWebViewApiTest::testPluginForExtension(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->pluginForExtension();
	}
	return err;
}

TInt CWebViewApiTest::testLoadBackForwardListFromOtherView(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->loadBackForwardListFromOtherView();
	}
	return err;
}

TInt CWebViewApiTest::testShouldClose(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->shouldClose();
	}
	return err;

}

TInt CWebViewApiTest::testIsLoading(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->isLoading();
	}
	return err;

}

TInt CWebViewApiTest::testSyncRepaint(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->syncRepaint();
	}
	return err;
}

TInt CWebViewApiTest::testScheduleRepaint(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->scheduleRepaint();
	}
	return err;
}

TInt CWebViewApiTest::testDoRepaint(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->doRepaint();
	}
	return err;
}

TInt CWebViewApiTest::testPageLoadFinished(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->pageLoadFinished();
	}
	return err;
}

TInt CWebViewApiTest::testUpdatePageScaler(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->updatePageScaler();
	}
	return err;
}

TInt CWebViewApiTest::testDoLayout(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->doLayout();
	}
	return err;
}

TInt CWebViewApiTest::testMaxBidiSize(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->maxBidiSize();
	}
	return err;
}

TInt CWebViewApiTest::testClearOffScreenBitmap(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->clearOffScreenBitmap();
	}
	return err;
}

TInt CWebViewApiTest::testScrollBuffer(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->scrollBuffer();
	}
	return err;
}

TInt CWebViewApiTest::testOffscreenRect(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->offscreenRect();
	}
	return err;
}

TInt CWebViewApiTest::testPageScaler(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->pageScaler();
	}
	return err;
}

TInt CWebViewApiTest::testScalingFactor(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->scalingFactor();
	}
	return err;
}

TInt CWebViewApiTest::testOpenUrl(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->openUrl();
	}
	return err;
}

TInt CWebViewApiTest::testBrCtl(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->brCtl();
	}
	return err;
}

TInt CWebViewApiTest::testFepTextEditor(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->fepTextEditor();
	}
	return err;
}

TInt CWebViewApiTest::testFormFillPopup(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->formFillPopup();
	}
	return err;
}

TInt CWebViewApiTest::testSetFormFillPopup(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->setFormFillPopup();
	}
	return err;
}

TInt CWebViewApiTest::testSetEditable(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->setEditable();
	}
	return err;
}

TInt CWebViewApiTest::testIsEditable(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->isEditable();
	}
	return err;
}

TInt CWebViewApiTest::testOfferKeyEvent(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->OfferKeyEvent();
	}
	return err;
}

TInt CWebViewApiTest::testSetFocusedElementType(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->setFocusedElementType();
	}
	return err;
}

TInt CWebViewApiTest::testFocusedElementType(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->focusedElementType();
	}
	return err;
}

TInt CWebViewApiTest::testPageScrollHandler(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->pageScrollHandler();
	}
	return err;
}

TInt CWebViewApiTest::testPageFullScreenHandler(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->pageFullScreenHandler();
	}
	return err;
}

TInt CWebViewApiTest::testPointerEventHandler(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->pointerEventHandler();
	}
	return err;
}

TInt CWebViewApiTest::testUpdateScrollbars(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->updateScrollbars();
	}
	return err;
}

TInt CWebViewApiTest::testOpenPageView(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->updateScrollbars();
	}
	return err;
}

TInt CWebViewApiTest::testClosePageView(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->closePageView();
	}
	return err;
}

TInt CWebViewApiTest::testCancelPageView(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->cancelPageView();
	}
	return err;
}

TInt CWebViewApiTest::testPageView(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->pageView();
	}
	return err;
}

TInt CWebViewApiTest::testInPageViewMode(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->inPageViewMode();
	}
	return err;
}

TInt CWebViewApiTest::testSearchFor(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->searchFor();
	}
	return err;
}

TInt CWebViewApiTest::testExitFindState(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->exitFindState();
	}
	return err;
	
}

TInt CWebViewApiTest::testInFindState(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->inFindState();
	}
	return err;
}

TInt CWebViewApiTest::testPageScalerEnabled(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->pageScalerEnabled();
	}
	return err;
}

TInt CWebViewApiTest::testIsSmallPage(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->isSmallPage();
	}
	return err;
}

TInt CWebViewApiTest::testWillSubmitForm(CStifItemParser& /*aItem*/)
{
	TInt err( KErrNone );
	if(iWebViewTestContainer){
		err = iWebViewTestContainer->willSubmitForm();
	}
	return err;
}





