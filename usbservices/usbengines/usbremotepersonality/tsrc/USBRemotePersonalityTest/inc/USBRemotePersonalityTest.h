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




#ifndef USBREMOTEPERSONALITYTEST_H
#define USBREMOTEPERSONALITYTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

#include <d32usbc.h>
#include <usbman.h>
#include <usbwatcher.h>
#include <usbpersonalityids.h>

#include "cremotepersonalityhandler.h"

// CONSTANTS
//const ?type ?constant_var = ?constant;
const TUint KRemotePersonalityPluginImpUid = 0x10283308;
const TUint KSetupPacketLength = 8;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KUSBRemotePersonalityTestLogPath, "\\logs\\testframework\\USBRemotePersonalityTest\\" );

// Logging path for ATS - for phone builds comment this line
//_LIT( KUSBRemotePersonalityTestLogPath, "e:\\testing\\stiflogs\\" ); 

// Log file
_LIT( KUSBRemotePersonalityTestLogFile, "USBRemotePersonalityTest.txt" ); 
_LIT( KUSBRemotePersonalityTestLogFileWithTitle, "USBRemotePersonalityTest_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CUSBRemotePersonalityTest;

// DATA TYPES
//enum ?declaration

enum TUSBRemotePersonalityTestResult
    {
    ETestCasePassed,
    ETestCaseFailed
    };

//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CUSBRemotePersonalityTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CUSBRemotePersonalityTest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CUSBRemotePersonalityTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CUSBRemotePersonalityTest();

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
        CUSBRemotePersonalityTest( CTestModuleIf& aTestModuleIf );

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

	void DoExecuteApiTestL( TPtrC aApiTestName, TUSBRemotePersonalityTestResult& aTestResult );
	
	void CreatePluginImplementationTestL( TUSBRemotePersonalityTestResult& aTestResult );
	void InitializeTestL( TUSBRemotePersonalityTestResult& aTestResult );
	void CheckLastResultTestL( TUSBRemotePersonalityTestResult& aTestResult );
	void GetAllPersonalitiesTestL( TUSBRemotePersonalityTestResult& aTestResult );
	void SetPersonalityTestL( TUSBRemotePersonalityTestResult& aTestResult );
	void GetPersonalityTestL( TUSBRemotePersonalityTestResult& aTestResult );
	void GetPersonalityDesTestL( TUSBRemotePersonalityTestResult& aTestResult );
	void GetLockStateTestL( TUSBRemotePersonalityTestResult& aTestResult );
	void SetLockStateTestL( TUSBRemotePersonalityTestResult& aTestResult );	
	
	void DoExecuteModuleTestL( TPtrC aModuleTestName, TUSBRemotePersonalityTestResult& aTestResult );
	
	void ExampleModuleTestL( TUSBRemotePersonalityTestResult& aTestResult );

	void DoExecuteBranchTestL( TPtrC aBranchTestName, TUSBRemotePersonalityTestResult& aTestResult );
	
	void ExampleBranchTestL( TUSBRemotePersonalityTestResult& aTestResult );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        RUsb* iUsbMan;
        RDevUsbcClient* iDevUsbClient;
        RUsbWatcher* iUsbWatcher;
        RBuf8 iSetupBuff;
        RBuf8 iDataBuff;        
        
        CRemotePersonalityHandler *iPlugin;
        
        TUid iPluginDtorIDKey;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // USBREMOTEPERSONALITYTEST_H

// End of File
