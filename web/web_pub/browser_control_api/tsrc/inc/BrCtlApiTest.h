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



#ifndef BRCTLAPITEST_H
#define BRCTLAPITEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#include <coecntrl.h>
#include "BrCtlApiTestContainer.h"

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KBrCtlApiTestLogPath, "\\logs\\testframework\\BrCtlApiTest\\" ); 
// Log file
_LIT( KBrCtlApiTestLogFile, "BrCtlApiTest.txt" ); 
_LIT( KBrCtlApiTestLogFileWithTitle, "BrCtlApiTest_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CBrowserControlApiTest;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CBrowserControlApiTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CBrowserControlApiTest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CBrowserControlApiTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CBrowserControlApiTest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

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
        CBrowserControlApiTest( CTestModuleIf& aTestModuleIf );

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

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ExampleL( CStifItemParser& aItem );
        virtual TInt CreateBrCtlL( CStifItemParser& aItem );
        virtual TInt DestroyBrowserControlL( CStifItemParser& aItem );
        virtual TInt ZoomLevels( CStifItemParser& aItem );
        virtual TInt HandleCommandL( CStifItemParser& aItem );
        virtual TInt LoadData( CStifItemParser& aItem );
        virtual TInt SavedPageL( CStifItemParser& aItem );
        virtual TInt CertInfo( CStifItemParser& aItem );
        virtual TInt SetSelfDownloadContentTypes( CStifItemParser& aItem );
        virtual TInt NavigationAvailable( CStifItemParser& aItem );
        virtual TInt PageInfo( CStifItemParser& aItem );
        virtual TInt EmptyCache( CStifItemParser& aItem );
        virtual TInt GetBitmapData( CStifItemParser& aResult );
        virtual TInt SaveToFile( CStifItemParser& aResult );
        virtual TInt OkToExit( CStifItemParser& aResult );
        virtual TInt OfferKeyEvent( CStifItemParser& aResult );
        virtual TInt MinimumSize( CStifItemParser& aResult );
        virtual TInt FocusedElementType( CStifItemParser& aResult );
        virtual TInt ContentSize( CStifItemParser& aResult );
        virtual TInt BrCtlSetParam( CStifItemParser& aResult );
        virtual TInt BrCtlSubscribeToMenuItemsL( CStifItemParser& aResult );
        virtual TInt FocusedImage( CStifItemParser& aResult );
        virtual TInt BrCtlSettings( CStifItemParser& aResult );
        virtual TInt FindKeyword( CStifItemParser& aResult );
        virtual TInt CommandObserver( CStifItemParser& aResult );
        virtual TInt AddOptionMenuItems( CStifItemParser& aResult );
        virtual TInt BrCtlInitLoadData( CStifItemParser& aResult );        
        virtual TInt HandleDownloadCommand( CStifItemParser& aResult );
        virtual TInt VersionInfo( CStifItemParser& aResult );
        virtual TInt WmlOptionMenuItems( CStifItemParser& aResult );
        virtual TInt ImageCount( CStifItemParser& aResult);
        virtual TInt PostUrlL( CStifItemParser& aResult);
        virtual TInt CreateBrCtlWithWidgetExtensionL( CStifItemParser& aItem ); // Added for Widget Extension Test
	virtual TInt TestHistoryHandlerL( CStifItemParser& aItem );
	virtual TInt TestWMLLoadL( CStifItemParser& aItem );		
		
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

    private:    // Data
        CBrCtlApiTestContainer* iBrCtlTestContainer;
    };

#endif      // BRCTLAPITEST_H

// End of File
