/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*
*/



// INCLUDE FILES
#include <e32math.h>
#include "DownloadMgrBCTest.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================

// ============================ MEMBER FUNCTIONS ===============================

/*
-------------------------------------------------------------------------------

    Class: CDownloadMgrClientApiTest

    Method: RegistryNewLTest

    Description: Test the CDownloadMgrUiLibRegistry NewL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::RegistryNewLTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the CDownloadMgrUiRegistry NewL method");
    TestModuleIf().Printf( 0, KDefinition, KData );
    
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    CDownloadMgrUiLibRegistry* registry = CDownloadMgrUiLibRegistry::NewL( iDownloadManager );
    
    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );
    
    if( registry )
	    {
	    _LIT( KDescription , "Test case passed" );
	    aResult.SetResult( KErrNone, KDescription );
	    
	    delete registry;
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed" );
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy(); // scheduler

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CDownloadMgrClientApiTest

    Method: RegistryRegisterUserInteractionsLTest

    Description: Test the CDownloadMgrUiLibRegistry RegisterUserInteractionsL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::RegistryRegisterUserInteractionsLTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the CDownloadMgrUiRegistry RegisterUserInteractionsL method");
    TestModuleIf().Printf( 0, KDefinition, KData );
    
    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );
    
    _LIT( KDescription , "Test case failed" );
    aResult.SetResult( KErrGeneral, KDescription );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CDownloadMgrClientApiTest

    Method: RegistryRegisterDownloadsListLTest

    Description: Test the CDownloadMgrUiLibRegistry RegisterDownloadsListL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::RegistryRegisterDownloadsListLTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the CDownloadMgrUiRegistry RegisterDownloadsListL method");
    TestModuleIf().Printf( 0, KDefinition, KData );
    
    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );
    
    _LIT( KDescription , "Test case failed" );
    aResult.SetResult( KErrGeneral, KDescription );

    // Case was executed
    return KErrNone;
    }

// ================= OTHER EXPORTED FUNCTIONS =================================

// End of File
