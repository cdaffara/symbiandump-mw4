/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef USBOTGWATCHERTEST_H
#define USBOTGWATCHERTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include "stifassertmacros.h"
//  INCLUDES FOR STUBS
#include <stubber.h>
#include <apibehavior.h>
#include <usbmandll_stub.h>
//  INCLUDES FOR TEST
#include <usbstates.h>
#include <usbuinotif.h>
#include "cusbotgwatcher.h"
#include "cusbnotifier.h"
#include "cusbstatehostainitiate.h"
#include "cusbstatehosthandle.h"
#include "cusbservicecontrol.h"


// CONSTANTS
// Logging path
_LIT( KUsbOtgWatcherTestLogPath, "\\logs\\testframework\\UsbOtgWatcherTest\\" );

// Logging path for ATS - for phone builds comment this line
//_LIT( KUsbOtgWatcherTestLogPath, "e:\\testing\\stiflogs\\" ); 

// Log file
_LIT( KUsbOtgWatcherTestLogFile, "UsbOtgWatcherTest.txt" ); 
_LIT( KUsbOtgWatcherTestLogFileWithTitle, "UsbOtgWatcherTest_[%S].txt" );

// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Macros used to extract test block parameters
#define TESTENTRY( A, B )\
    if ( !iTestBlockParams.iTestBlockName.Compare( TPtrC( ( TText* ) L ##A ) ) )\
        { iTestBlockFunction = &B; iTestBlockFound = ETrue; }\
    if ( !iTestBlockParams.iTestBlockName.Compare( TPtrC( ( TText* ) L ##A ) ) )

#define TESTPARAM( A, B )\
    GetTestBlockParamL( ( TGetTestBlockParamOperation ) A, ( TTestBlockParamName ) B );

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CUsbOtgWatcher;
class RUsb;

// DATA TYPES

// Enum type used to determine test block execution result
enum TUsbOtgWatcherTestResult
    {
    ETestCasePassed,
    ETestCaseFailed
    };

// Enum type indicating test block parameter which should be used to store
// data extracted from test script
enum TTestBlockParamName
    {
    eTestOption1,
    eTestOption2,
    eTestOption3,    
    eTestIntOption1,
    eTestIntOption2,
    eTestIntOption3,   
    eTestCharOption1,
    eTestCharOption2,
    eTestCharOption3
    };

// Enum type used to indicate which get parameter function should be used
enum TGetTestBlockParamOperation
    {
    eGetString,
    eGetInt,
    eGetChar
    };

// CLASS DECLARATION
/**
*  TUsbOtgWatcherTestBlockParams test class for storing test block parameters.
*/
NONSHARABLE_CLASS( TUsbOtgWatcherTestBlockParams )
    {
    public:
        TPtrC iTestBlockName;
        
        TPtrC iTestOption1;
        TPtrC iTestOption2;
        TPtrC iTestOption3;
        
        TInt iTestIntOption1;
        TInt iTestIntOption2;
        TInt iTestIntOption3;
        
        TChar iTestCharOption1;
        TChar iTestCharOption2;
        TChar iTestCharOption3;
    };

/**
*  CUsbOtgWatcherTest test class for STIF Test Framework TestScripter.
*/
NONSHARABLE_CLASS( CtUsbOtgWatcher ) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CtUsbOtgWatcher* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CtUsbOtgWatcher();

    public: // New functions
    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions
    protected:  // Functions from base classes
    private:

        /**
        * C++ default constructor.
        */
        CtUsbOtgWatcher( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void TestDelete();
        
        /**
        * Constructs all test resources for test methods.
        */
        void TestConstructL();
        
        /**
        * Extracts test block parameters from script file
        */
        void TestBlocksInfoL();
        
        /**
        * Executes test block
        */
        TInt ExecuteTestBlock( CStifItemParser& aItem );
        
        /**
        * Executes proper get parameter function indicated by its arguments
        */
        void GetTestBlockParamL( TGetTestBlockParamOperation aOperation,
                                TTestBlockParamName aParamName );
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        // ADD NEW METHOD DEC HERE
        // [TestMethods] - Do not remove 
        void ExampleTestL( TUsbOtgWatcherTestBlockParams& aParams, 
                            TUsbOtgWatcherTestResult& aTestResult );
        
        void SetupTearDownL( TUsbOtgWatcherTestBlockParams& aParams,
                            TUsbOtgWatcherTestResult& aTestResult );
        
        void PinOnTest_SuccessL( TUsbOtgWatcherTestBlockParams& aParams,
                            TUsbOtgWatcherTestResult& aTestResult );
        
        void PinOffTest_SuccessL( TUsbOtgWatcherTestBlockParams& aParams,
                            TUsbOtgWatcherTestResult& aTestResult );
        
        void PinOnTest_EmptyCableL( TUsbOtgWatcherTestBlockParams& aParams,
                            TUsbOtgWatcherTestResult& aTestResult );
        
        void PinOnTest_TryStartErrorL( TUsbOtgWatcherTestBlockParams& aParams,
                            TUsbOtgWatcherTestResult& aTestResult );
        
        void PinOnTest_BusRequestErrorL( TUsbOtgWatcherTestBlockParams& aParams,
                            TUsbOtgWatcherTestResult& aTestResult );
        
        void DeviceAttached_BadHubPostionL( TUsbOtgWatcherTestBlockParams& aParams,
                            TUsbOtgWatcherTestResult& aTestResult );
        
        void DeviceAttached_TooMuchRequiredL( TUsbOtgWatcherTestBlockParams& aParams,
                            TUsbOtgWatcherTestResult& aTestResult );
        
        void DeviceAttached_SupportedDeviceL( TUsbOtgWatcherTestBlockParams& aParams,
                            TUsbOtgWatcherTestResult& aTestResult );
        
        void DeviceDetached_SupportedDeviceL( TUsbOtgWatcherTestBlockParams& aParams,
                            TUsbOtgWatcherTestResult& aTestResult );
   
        void IndicatorNotifier_IndicatorActivateL( TUsbOtgWatcherTestBlockParams& aParams,
                            TUsbOtgWatcherTestResult& aTestResult );
        
        //others method
        void SetupL();
        void TearDown();
        
    public:     // Data
    protected:  // Data
    private:    // Data
        
        // Member object used to store test block parameters
        TUsbOtgWatcherTestBlockParams iTestBlockParams;
        TTestResult iResult;
        
        // Used to indicate if test block with specified parameters exist
        TBool iTestBlockFound;
        
        // Function pointer used to call proper test block methods
        void ( CtUsbOtgWatcher::*iTestBlockFunction )
            ( TUsbOtgWatcherTestBlockParams&, TUsbOtgWatcherTestResult& );
        
        // Not own
        CStifItemParser* iItem;
        CStubber * iStubber;
        RUsb* iUsbMan;
        CUsbOtgWatcher* iWatcher;

        // Reserved pointer for future extension
        //TAny* iReserved;

        // ADD NEW DATA DEC HERE

        
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes
        
    };
#endif      // USBOTGWATCHERTEST_H

// End of File
