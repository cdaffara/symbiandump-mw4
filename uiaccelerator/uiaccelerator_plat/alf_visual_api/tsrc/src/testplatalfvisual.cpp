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
* Description:  functions implement of CTestPlatAlfVisual
*
*/



// INCLUDE FILES
#include <stiftestinterface.h>
#include <settingserverclient.h>
#include <screensaverinternalpskeys.h>
#include <e32property.h>


#include "testplatalfvisual.h"

// CONSTANTS
_LIT( KModuleName, "testplatalfvisual.dll" );

const TInt KControlGroupId = 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::CTestPlatAlfVisual
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTestPlatAlfVisual::CTestPlatAlfVisual( CTestModuleIf& aTestModuleIf ):
    CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::~CTestPlatAlfVisual
// Destructor.
// -----------------------------------------------------------------------------
//
CTestPlatAlfVisual::~CTestPlatAlfVisual()
    {
    // Delete logger
    delete iLog; 
    
    // Delete CAlfEnv
    delete iAlfEnv;
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTestPlatAlfVisual::ConstructL()
    {
    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    RSettingServer settingServer;
    CleanupClosePushL( settingServer );
    TInt ret = settingServer.Connect();
    if ( ret != KErrNone )
        {
        User::Leave(ret);
        }
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings; 
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings( loggerSettings );
    if ( ret != KErrNone )
        {
        User::Leave( ret );
        } 
    // Close Setting server session
    settingServer.Close();
    CleanupStack::PopAndDestroy( &settingServer );

    TFileName logFileName;
    
    if ( loggerSettings.iAddTestCaseTitle )
        {
        TName title;
        TestModuleIf().GetTestCaseTitleL( title );
        logFileName.Format( KtestplatalfvisualLogFileWithTitle, &title );
        }
    else
        {
        logFileName.Copy( KtestplatalfvisualLogFile );
        }

    iLog = CStifLogger::NewL( KtestplatalfvisualLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    
    SendTestClassVersion();
    
    TurnOffScreenSaver();
    
    // Create CAlfEnv instance
    iAlfEnv = CAlfEnv::NewL();
    
    TRect rect = CEikonEnv::Static()->AppUiFactory()->ClientRect();
    iAlfDisplay = &( iAlfEnv->NewDisplayL( rect ) );
    iAlfCtrlGroup = &( iAlfEnv->NewControlGroupL( KControlGroupId ) );
    
    // Create CAlfControl instance
    iAlfCtl = new ( ELeave ) CAlfControl;
    iAlfCtl->ConstructL( *iAlfEnv );
    
    iAlfCtrlGroup->AppendL( iAlfCtl );
    
    iAlfCtl->BindDisplay( *iAlfDisplay );
    
    }

// -----------------------------------------------------------------------------
// CTestPlatAlfVisual::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTestPlatAlfVisual* CTestPlatAlfVisual::NewL( CTestModuleIf& aTestModuleIf )
    {
    CTestPlatAlfVisual* self = new( ELeave ) CTestPlatAlfVisual( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;

    }

//-----------------------------------------------------------------------------
// CTestPlatAlfVisual::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CTestPlatAlfVisual::SendTestClassVersion()
    {
    TVersion moduleVersion;
    moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
    moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
    moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;

    TFileName moduleName;
    moduleName = KModuleName;

    TBool newVersionOfMethod = ETrue;
    TestModuleIf().SendTestModuleVersion( moduleVersion, moduleName, 
        newVersionOfMethod );
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
    return ( CScriptBase* ) CTestPlatAlfVisual::NewL( aTestModuleIf );
    }

// -----------------------------------------------------------------------------
// Turn off ScreenSaver
// -----------------------------------------------------------------------------
//
void CTestPlatAlfVisual::TurnOffScreenSaver()
    {
    TInt err1 = RProperty::Get( KPSUidScreenSaver, KScreenSaverAllowScreenSaver, 
        iOldScreenSaverProperty );
    TInt err2 = RProperty::Set( KPSUidScreenSaver, KScreenSaverAllowScreenSaver, 
        KScreenSaverAllowScreenSaver );    
    RDebug::Printf( "screensaver property=%d err1=%d err2=%d\n", 
        iOldScreenSaverProperty, err1, err2 );
    }

// -----------------------------------------------------------------------------
// Restore ScreenSaver
// -----------------------------------------------------------------------------
//
void CTestPlatAlfVisual::RestoreScreenSaver()
    {
    RProperty::Set( KPSUidScreenSaver, KScreenSaverAllowScreenSaver, 
        iOldScreenSaverProperty );
    User::ResetInactivityTime();
    }


//  End of File
