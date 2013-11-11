/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        ?Description
*
*/









// INCLUDE FILES
#include <Stiftestinterface.h>
#include "hostconnTester.h"
#include <SettingServerClient.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChostconnTester::ChostconnTester
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChostconnTester::ChostconnTester( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// ChostconnTester::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChostconnTester::ConstructL()
    {
    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    iLog = CStifLogger::NewL( KSenHostletConnection_stifLogPath, 
                          KSenHostletConnection_stifLogFile);

    // Sample how to use logging
    _LIT( KLogStart, "KSenHostletConnection_stif logging starts!" );
    iLog->Log( KLogStart );
    
//    iActiveScheduler = new CActiveScheduler;
//	  CActiveScheduler::Install( iActiveScheduler );
    }

// -----------------------------------------------------------------------------
// ChostconnTester::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChostconnTester* ChostconnTester::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    ChostconnTester* self = new (ELeave) ChostconnTester( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
ChostconnTester::~ChostconnTester()
    {
    delete iLog;
    
    delete iActiveScheduler;
   
    }

//-----------------------------------------------------------------------------
// ChostconnTester::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void ChostconnTester::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("hostconnTester.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName, newVersionOfMethod);
	}
// -----------------------------------------------------------------------------
// ChostconnTester::RunTestCaseL
// RunTestCaseL is used to run an individual test case specified 
// by aTestCase. Test cases that can be run may be requested from 
// Test Module by GetTestCases method before calling RunTestCase.
// -----------------------------------------------------------------------------
//
//TInt ChostconnTester::RunTestCaseL( 
//    const TInt aCaseNumber,   
//    const TFileName& /*aConfig*/,
//    TTestResult& aResult )
//    {

    // Return value
//    TInt execStatus = KErrNone;

    // Get the pointer to test case function
//    TCaseInfo tmp = Case ( aCaseNumber );

//    _LIT( KLogStartTC, "Starting testcase [%S]" );
//    iLog->Log( KLogStartTC, &tmp.iCaseName);

    // Check that case number was valid
//    if ( tmp.iMethod != NULL )
//        {
        // Valid case was found, call it via function pointer
//        iMethod = tmp.iMethod;        
//        execStatus  = ( this->*iMethod )( aResult );
//        }
//    else
//        {
        // Valid case was not found, return error.
//        execStatus = KErrNotFound;
//        }

    // Return case execution status (not the result of the case execution)
//    return execStatus;

//    }
// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) ChostconnTester::NewL( aTestModuleIf );

    }


//  End of File
