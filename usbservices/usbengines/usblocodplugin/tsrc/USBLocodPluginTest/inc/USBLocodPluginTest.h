/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef USBLOCODPLUGINTEST_H
#define USBLOCODPLUGINTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

#include <e32property.h>
#include <usbwatcherinternalpskeys.h>
#include <usbpersonalityids.h>
#include <locodbearerplugin.h>
#include <locodbearer.h>

#include "locodbearerpluginobs.h"

// CONSTANTS
#define KUsbDefaultPersonality  0x00

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KUSBLocodPluginTestLogPath, "\\logs\\testframework\\USBLocodPluginTest\\" );

// Logging path for ATS - for phone builds comment this line
//_LIT( KUSBLocodPluginTestLogPath, "e:\\testing\\stiflogs\\" ); 

// Log file
_LIT( KUSBLocodPluginTestLogFile, "USBLocodPluginTest.txt" );
_LIT( KUSBLocodPluginTestLogFileWithTitle, "USBLocodPluginTest_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CUSBLocodPluginTest;

// DATA TYPES
//enum ?declaration

enum TUSBLocodPluginTestResult
    {
    ETestCasePassed,
    ETestCaseFailed
    };

//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CUSBLocodPluginTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CUSBLocodPluginTest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CUSBLocodPluginTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CUSBLocodPluginTest();

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

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CUSBLocodPluginTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */

        virtual TInt ExecuteApiTest( CStifItemParser& aItem );
        virtual TInt ExecuteModuleTest( CStifItemParser& aItem );
        virtual TInt ExecuteBranchTest( CStifItemParser& aItem );
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

	void DoExecuteApiTestL( TPtrC aApiTestName, TUSBLocodPluginTestResult& aTestResult );

	void InitializationTestL( TUSBLocodPluginTestResult& aTestResult );
	void ImplementatiationIdTestL( TUSBLocodPluginTestResult& aTestResult );
	void USBPCSuitePersonalityTestL( TUSBLocodPluginTestResult& aTestResult );
	void USBDefaultPersonalityTestL( TUSBLocodPluginTestResult& aTestResult );
	
	void DoExecuteModuleTestL( TPtrC aModuleTestName, TUSBLocodPluginTestResult& aTestResult );

    void USBMSPersonalityTestL( TUSBLocodPluginTestResult& aTestResult );
    void USBPTPPersonalityTestL( TUSBLocodPluginTestResult& aTestResult );
    void USBMTPPersonalityTestL( TUSBLocodPluginTestResult& aTestResult );

	void DoExecuteBranchTestL( TPtrC aBranchTestName, TUSBLocodPluginTestResult& aTestResult );

	void USBPCSuiteMTPPersonalityTestL( TUSBLocodPluginTestResult& aTestResult );
	
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        RProperty* iSelectedPersProperty;
        
        CLocodBearerPlugin* iLocodBearerPlugin;
        CLocodBearerPluginObs* iLocodBearerPluginObs;
        CActiveSchedulerWait *iWaiter;
        TBool iDeleteProperty;
        TBool iBearerAvailable;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // USBLOCODPLUGINTEST_H

// End of File
