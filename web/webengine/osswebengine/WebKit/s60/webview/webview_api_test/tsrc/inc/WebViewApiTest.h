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



#ifndef WEBVIEWAPITEST_H
#define WEBVIEWAPITEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include "WebViewApiTestContainer.h" //delete the class

// CONSTANTS


// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KWebViewApiTestLogPath, "\\logs\\testframework\\WebViewApiTest\\" );
// Log file
_LIT( KWebViewApiTestLogFile, "WebViewApiTest.txt" );
_LIT( KWebViewApiTestLogFileWithTitle, "WebViewApiTest_[%S].txt" );

// FUNCTION PROTOTYPES



// DATA TYPES


// CLASS DECLARATION
class CWebViewApiTestContainer;
class CPluginHandler;

/**
*  CWebViewApiTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CWebViewApiTest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CWebViewApiTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CWebViewApiTest();

    public: // New functions



    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    private:

        /**
        * C++ default constructor.
        */
        CWebViewApiTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

		/**
        * Test methods are listed below.
        */
        virtual TInt CreateWebViewL( CStifItemParser& aItem );
        virtual TInt DestroyWebView( CStifItemParser& aItem );
		virtual TInt testMakeVisible(CStifItemParser& aItem);
		virtual TInt testMakeViewVisible(CStifItemParser& aItem);
		virtual TInt testMainFrame(CStifItemParser& aItem);
		virtual TInt testMainFrameDocument(CStifItemParser& aItem);
		virtual TInt testPage(CStifItemParser& aItem);
		virtual TInt testMIMETypeRegisteredAsPlugin(CStifItemParser& aItem);
		virtual TInt testPluginForExtension(CStifItemParser& aItem);
		virtual TInt testLoadBackForwardListFromOtherView(CStifItemParser& aItem);
		virtual TInt testShouldClose(CStifItemParser& aItem);
		virtual TInt testIsLoading(CStifItemParser& aItem);
		virtual TInt testSyncRepaint(CStifItemParser& aItem);
		virtual TInt testScheduleRepaint(CStifItemParser& aItem);
		virtual TInt testDoRepaint(CStifItemParser& aItem);
		virtual TInt testPageLoadFinished(CStifItemParser& aItem);
		virtual TInt testUpdatePageScaler(CStifItemParser& aItem);
		virtual TInt testDoLayout(CStifItemParser& aItem);
		virtual TInt testMaxBidiSize(CStifItemParser& aItem);
		virtual TInt testClearOffScreenBitmap(CStifItemParser& aItem);
		virtual TInt testScrollBuffer(CStifItemParser& aItem);
		virtual TInt testOffscreenRect(CStifItemParser& aItem);
		virtual TInt testPageScaler(CStifItemParser& aItem);
		virtual TInt testScalingFactor(CStifItemParser& aItem);
		virtual TInt testOpenUrl(CStifItemParser& aItem);
		virtual TInt testBrCtl(CStifItemParser& aItem);
		virtual TInt testFepTextEditor(CStifItemParser& aItem);
		virtual TInt testFormFillPopup(CStifItemParser& aItem);
		virtual TInt testSetFormFillPopup(CStifItemParser& aItem);
		virtual TInt testSetEditable(CStifItemParser& aItem);
		virtual TInt testIsEditable(CStifItemParser& aItem);
		virtual TInt testOfferKeyEvent(CStifItemParser& aItem);
		virtual TInt testSetFocusedElementType(CStifItemParser& aItem);
		virtual TInt testFocusedElementType(CStifItemParser& aItem);
		virtual TInt testPageScrollHandler(CStifItemParser& aItem);
		virtual TInt testPageFullScreenHandler(CStifItemParser& aItem);
		virtual TInt testPointerEventHandler(CStifItemParser& aItem); 
		virtual TInt testUpdateScrollbars(CStifItemParser& aItem);
		virtual TInt testOpenPageView(CStifItemParser& aItem);
		virtual TInt testClosePageView(CStifItemParser& aItem);
		virtual TInt testCancelPageView(CStifItemParser& aItem);
		virtual TInt testPageView(CStifItemParser& aItem);
		virtual TInt testInPageViewMode(CStifItemParser& aItem);
		virtual TInt testSearchFor(CStifItemParser& aItem);
		virtual TInt testExitFindState(CStifItemParser& aItem);
		virtual TInt testInFindState(CStifItemParser& aItem);
		virtual TInt testPageScalerEnabled(CStifItemParser& aItem);
		virtual TInt testIsSmallPage(CStifItemParser& aItem);
		virtual TInt testWillSubmitForm(CStifItemParser& aItem);
		
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

    private:    // Data
        CWebViewApiTestContainer* iWebViewTestContainer;
    };

#endif      // WEBVIEWAPITEST_H

// End of File
