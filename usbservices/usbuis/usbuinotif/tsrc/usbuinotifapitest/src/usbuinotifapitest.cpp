/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "usbuinotifapitest.h"
#include <SettingServerClient.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUSBUiNotifApiTest::CUSBUiNotifApiTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUSBUiNotifApiTest::CUSBUiNotifApiTest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf ),
        iRes(0)
        
    {
    }

// -----------------------------------------------------------------------------
// CUSBUiNotifApiTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUSBUiNotifApiTest::ConstructL()
    {
    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    
    RSettingServer settingServer;
    CleanupClosePushL(settingServer); 
    TInt ret = settingServer.Connect();
    if(ret != KErrNone)
        {
        User::Leave(ret);
        }
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings; 
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings(loggerSettings);
    if(ret != KErrNone)
        {
        User::Leave(ret);
        } 
    // Close Setting server session
    CleanupStack::PopAndDestroy(&settingServer);

    TFileName logFileName;
    
    if(loggerSettings.iAddTestCaseTitle)
        {
        TName title;
        TestModuleIf().GetTestCaseTitleL(title);
        logFileName.Format(KUSBUiNotifApiTestLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KUSBUiNotifApiTestLogFile);
        }

    iLog = CStifLogger::NewL( KUSBUiNotifApiTestLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    
    SendTestClassVersion();
   
    iNotifier = new ( ELeave ) RNotifier();
    /*
    iUsbWatcher = new ( ELeave ) RUsbWatcher();
    iRepository = CRepository::NewL( KCRUidUsbWatcher );
    */
    }

// -----------------------------------------------------------------------------
// CUSBUiNotifApiTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUSBUiNotifApiTest* CUSBUiNotifApiTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CUSBUiNotifApiTest* self = new (ELeave) CUSBUiNotifApiTest( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;

    }

// Destructor
CUSBUiNotifApiTest::~CUSBUiNotifApiTest()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 

    }

//-----------------------------------------------------------------------------
// CUSBUiNotifApiTest::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CUSBUiNotifApiTest::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("USBUiNotifApiTest.dll");

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

    return ( CScriptBase* ) CUSBUiNotifApiTest::NewL( aTestModuleIf );

    }


//  End of File
