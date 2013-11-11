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

// INCLUDE FILES
#include <StifTestInterface.h>
#include <SettingServerClient.h>
#include "UsbOtgWatcherTest.h"
#include "testdebug.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUsbOtgWatcherTest::CUsbOtgWatcherTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CtUsbOtgWatcher::CtUsbOtgWatcher( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CtUsbOtgWatcher::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CtUsbOtgWatcher::ConstructL()
    {
    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    RSettingServer settingServer;
    TInt ret = settingServer.Connect();
    if(ret != KErrNone)
        {
        User::Leave(ret);
        }
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings; 
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings( loggerSettings );
    if( ret != KErrNone )
        {
        User::Leave( ret );
        } 
    // Close Setting server session
    settingServer.Close();

    TFileName logFileName;
    
    if( loggerSettings.iAddTestCaseTitle )
        {
        TName title;
        TestModuleIf().GetTestCaseTitleL( title );
        logFileName.Format( KUsbOtgWatcherTestLogFileWithTitle, &title );
        }
    else
        {
        logFileName.Copy( KUsbOtgWatcherTestLogFile );
        }

    iLog = CStifLogger::NewL( KUsbOtgWatcherTestLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    
    SendTestClassVersion();
    TestConstructL();
    }

// -----------------------------------------------------------------------------
// CtUsbOtgWatcher::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CtUsbOtgWatcher* CtUsbOtgWatcher::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CtUsbOtgWatcher* self = new (ELeave) CtUsbOtgWatcher( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CtUsbOtgWatcher::~CtUsbOtgWatcher()
    { 

    // Delete resources allocated from test methods
    TestDelete();

    // Delete logger
    delete iLog; 

    }

// -----------------------------------------------------------------------------
// CtUsbOtgWatcher::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------

TInt CtUsbOtgWatcher::RunMethodL( 
    CStifItemParser& aItem ) 
    {
    return ExecuteTestBlock( aItem );
    }

// -----------------------------------------------------------------------------
// CtUsbOtgWatcher::ExecuteTestBlock
// -----------------------------------------------------------------------------

TInt CtUsbOtgWatcher::ExecuteTestBlock( CStifItemParser& aItem )
    {
    TRACE_FUNC_ENTRY
    iItem = &aItem;    
    TInt res;
    TUsbOtgWatcherTestResult testResult = ETestCaseFailed;
    
    res = iItem -> GetString( _L( "tester" ), iTestBlockParams.iTestBlockName );
    if ( res != KErrNone )
        {
        TRACE_INFO( "Getting test block name failed with: %d", res )
        TRACE_FUNC_EXIT
        return res;
        }
    TRACE_INFO( "Test block name: %S", &iTestBlockParams.iTestBlockName )
    
    TRACE( "Extracting test block parameters..." )
    TRAP( res, TestBlocksInfoL() );
    if ( res != KErrNone )
        {
        TRACE_INFO( "Extracting parameters failed: %d", res )
        TRACE_FUNC_EXIT
        return res;
        }
    if ( !iTestBlockFound )
        {
        TRACE( "Test block not found!" )
        TRACE_FUNC_EXIT
        return KErrNotFound;
        }    
    TRACE( "Extracting parameters successful" )
    
    TRAP( res, ( this ->*iTestBlockFunction )( iTestBlockParams, testResult ) );
    if ( res != KErrNone )
        {
        TRACE_INFO( "ExecuteTestBlockL error: %d", res )
        TRACE_FUNC_EXIT
        return res;
        }
    
    if (ETestCasePassed != testResult) 
        {
        return KErrGeneral;
        }
    TRACE( "Test block passed!" );
    TRACE_FUNC_EXIT 
    return KErrNone;
    } 

// -----------------------------------------------------------------------------
// CtUsbOtgWatcher::GetTestBlockParamL
// -----------------------------------------------------------------------------

void CtUsbOtgWatcher::GetTestBlockParamL( TGetTestBlockParamOperation aOperation, TTestBlockParamName aParamName )
    {    
    switch( aOperation )
        {
        case eGetString:
            {
            switch ( aParamName )
                {
                case eTestOption1:
                    {
                    User::LeaveIfError( iItem -> GetNextString( iTestBlockParams.iTestOption1 ) );
                    break;
                    }
                case eTestOption2:
                    {
                    User::LeaveIfError( iItem -> GetNextString( iTestBlockParams.iTestOption2 ) );
                    break;
                    }
                case eTestOption3:
                    {
                    User::LeaveIfError( iItem -> GetNextString( iTestBlockParams.iTestOption3 ) );
                    break;
                    }
                default:
                    {
                    User::Leave( KErrArgument );
                    break;
                    }
                }
            break;
            }
        case eGetInt:
            {
            switch ( aParamName )
                {
                case eTestIntOption1:
                    {
                    User::LeaveIfError( iItem -> GetNextInt( iTestBlockParams.iTestIntOption1 ) );
                    break;
                    }
                case eTestIntOption2:
                    {
                    User::LeaveIfError( iItem -> GetNextInt( iTestBlockParams.iTestIntOption2 ) );
                    break;
                    }
                case eTestIntOption3:
                    {
                    User::LeaveIfError( iItem -> GetNextInt( iTestBlockParams.iTestIntOption3 ) );
                    break;
                    }
                default:
                    {
                    User::Leave( KErrArgument );
                    break;
                    }
                }
            break;
            }
        case eGetChar:            
            {
            switch ( aParamName )
                {
                case eTestCharOption1:
                    {
                    User::LeaveIfError( iItem -> GetNextChar( iTestBlockParams.iTestCharOption1 ) );
                    break;
                    }
                case eTestCharOption2:
                    {
                    User::LeaveIfError( iItem -> GetNextChar( iTestBlockParams.iTestCharOption2 ) );
                    break;
                    }
                case eTestCharOption3:
                    {
                    User::LeaveIfError( iItem -> GetNextChar( iTestBlockParams.iTestCharOption3 ) );
                    break;
                    }
                default:
                    {
                    User::Leave( KErrArgument );
                    }
                }
            break;
            }
        default:
            {
            User::Leave( KErrArgument );
            }
        }
    }

//-----------------------------------------------------------------------------
// CtUsbOtgWatcher::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CtUsbOtgWatcher::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("UsbOtgWatcherTest.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName, newVersionOfMethod);
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) CtUsbOtgWatcher::NewL( aTestModuleIf );

    }


//  End of File
