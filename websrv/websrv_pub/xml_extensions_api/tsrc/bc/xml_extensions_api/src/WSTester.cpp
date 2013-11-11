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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include <StifTestInterface.h>
#include "WSTester.h"

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

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWSTester::CWSTester
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWSTester::CWSTester( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf ),
		iFrg(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CWSTester::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWSTester::ConstructL()
    {
 //   __UHEAP_MARK;
    TestModuleIf().SetBehavior(CTestModuleIf::ETestLeaksHandles);
    iLog = CStifLogger::NewL( KWSTesterLogPath, 
                          KWSTesterLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    User::LeaveIfError(this->iFsSession.Connect());
	iXmlReader = CSenXmlReader::NewL();
    iErrorsOccured = FALSE;
	iCasePassed = EFalse;
    iLog->Log(_L("                     /------------\\"));
    iLog->Log(_L("--------------------- New testcase -------------"));
    iLog->Log(_L("                     \\------------/"));
	iStringPool.OpenL();
	iElement = NULL;
	iFrg = NULL;
	iProvider = NULL;
	iArgc = 0;
	iRunCase = 0;
	iSaxTester = NULL;
	iSenCredential = NULL;
	iSenServiceConnection = NULL;
	iSenXmlServiceDescription = NULL;
	iServiceManager = NULL;
	iSOAPMessage = NULL;
	SendTestModuleVersion();
    }
//-----------------------------------------------------------------------------
// CWSTester::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CWSTester::SendTestModuleVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_MODULE_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_MODULE_VERSION_MINOR;
	moduleVersion.iBuild = TEST_MODULE_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("WSTester.dll");
	

	TBool newVersionOfMethod = ETrue;
	CTestModuleIf &test=TestModuleIf();
	test.SendTestModuleVersion(moduleVersion, moduleName, newVersionOfMethod);
	test.SetBehavior(CTestModuleIf::ETestLeaksHandles);
	}

// -----------------------------------------------------------------------------
// CWSTester::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWSTester* CWSTester::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CWSTester* self = new (ELeave) CWSTester( aTestModuleIf );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    
    }
    
// Destructor
CWSTester::~CWSTester()
    { 
    
    // Delete resources allocated from test methods
    Delete();
    iWriter.Close();
    iFsSession.Close();    
    // Delete logger
    delete iLog;  

    if ( iSenServiceConnection != NULL ){
        delete iSenServiceConnection ;
	    iSenServiceConnection = NULL;
    }
    if ( iSenXmlServiceDescription!= NULL ){
        delete iSenXmlServiceDescription;
	    iSenXmlServiceDescription = NULL;
    }
    if ( iServiceManager!= NULL ){
        delete iServiceManager;
	    iServiceManager = NULL;
    }
    if ( iProvider!= NULL ){
        delete iProvider;
	    iProvider = NULL;
    }
	if ( iXmlReader != NULL ){
        delete iXmlReader;
	    iXmlReader = NULL;
    }
    
    if ( iSOAPMessage != NULL ){
        delete iSOAPMessage;
	    iSOAPMessage = NULL;
    }
    if ( iSaxTester != NULL ){
        delete iSaxTester;
	    iSaxTester = NULL;
    }
   	if ( iElement != NULL ){
        delete iElement;
        iElement = NULL;
    }

	iStringPool.Close();

	delete iFrg;
	iFrg = NULL;

    //__UHEAP_MARKEND;
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
    
    return ( CScriptBase* ) CWSTester::NewL( aTestModuleIf );
        
    }

// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function.
// Returns: KErrNone
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll(
    TDllReason /*aReason*/) // Reason code
    {
    return(KErrNone);
    
    }
#endif // EKA2
    
//  End of File
