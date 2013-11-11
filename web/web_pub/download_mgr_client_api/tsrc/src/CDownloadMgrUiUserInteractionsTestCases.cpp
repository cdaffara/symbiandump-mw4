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

    Method: InteractionsOkToExitLTest

    Description: Test the CDownloadMgrUiUserInteractions OkToExitL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::InteractionsOkToExitLTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the CDownloadMgrUiUserInteractions OkToExitL method");
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

    Method: InteractionsPrepareToExitWithViewIdTest

    Description: Test the CDownloadMgrUiUserInteractions PrepareToExit method with ViewId argument.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::InteractionsPrepareToExitWithViewIdTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the CDownloadMgrUiUserInteractions PrepareToExit method with ViewId argument");
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

    Method: InteractionsPrepareToExitTest

    Description: Test the CDownloadMgrUiUserInteractions PrepareToExit method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::InteractionsPrepareToExitTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the CDownloadMgrUiUserInteractions PrepareToExit method");
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

    Method: InteractionsHandleDownloadLTest

    Description: Test the CDownloadMgrUiUserInteractions HandleDownloadL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::InteractionsHandleDownloadLTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the CDownloadMgrUiUserInteractions HandleDownloadL method");
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
