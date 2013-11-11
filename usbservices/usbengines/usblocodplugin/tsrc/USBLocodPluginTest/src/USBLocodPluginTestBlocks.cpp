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




// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include "USBLocodPluginTest.h"

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
// CUSBLocodPluginTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CUSBLocodPluginTest::Delete() 
    {
    delete iLocodBearerPluginObs;
    iLocodBearerPluginObs = NULL;
    
    delete iLocodBearerPlugin;
    iLocodBearerPlugin = NULL;
    if ( iDeleteProperty )
        RProperty::Delete( KPSUidUsbWatcher, KUsbWatcherSelectedPersonality );
    
    iSelectedPersProperty -> Close();
    delete iSelectedPersProperty;
    iSelectedPersProperty = NULL;    
    REComSession::FinalClose();
    }

// -----------------------------------------------------------------------------
// CUSBLocodPluginTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CUSBLocodPluginTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
		ENTRY( "ExecuteApiTest", CUSBLocodPluginTest::ExecuteApiTest ),
        ENTRY( "ExecuteModuleTest", CUSBLocodPluginTest::ExecuteModuleTest ),
        ENTRY( "ExecuteBranchTest", CUSBLocodPluginTest::ExecuteBranchTest ),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CUSBLocodPluginTest::ExecuteApiTest
// -----------------------------------------------------------------------------

TInt CUSBLocodPluginTest::ExecuteApiTest( CStifItemParser& aItem )
    {
	STIF_LOG( "[STIF_LOG] >>>ExecuteApiTest" );
	
	TInt res;
    TUSBLocodPluginTestResult testResult;
    TPtrC apiTestName( KNullDesC );
	
	res = aItem.GetString( _L( "ExecuteApiTest" ), apiTestName );   
    if ( res != KErrNone )
        {
        iLog -> Log( _L( "GetString failed with value: %d" ), res );
        return res;
        }
		
	TRAP( res, DoExecuteApiTestL( apiTestName, testResult ) );
    if ( res != KErrNone )
        {
        iLog -> Log( _L( "DoExecuteApiTestL error: %d"), res );
        return res;
        }
    
    STIF_ASSERT_EQUALS( ETestCasePassed, testResult );
    STIF_LOG( "[STIF_LOG] Test case passed" );
	STIF_LOG( "[STIF_LOG] <<<ExecuteApiTest" );
    return KErrNone;
    }
	
	
void CUSBLocodPluginTest::DoExecuteApiTestL( TPtrC aApiTestName, TUSBLocodPluginTestResult& aTestResult )
    {
	STIF_LOG( "[STIF_LOG] >>>DoExecuteApiTestL" );
    if ( !aApiTestName.Compare( _L( "InitializationTestL" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Api test type: InitializationTestL" );
        InitializationTestL( aTestResult );
        }
    else if ( !aApiTestName.Compare( _L( "ImplementatiationIdTestL" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Api test type: ImplementatiationIdTestL" );
        ImplementatiationIdTestL( aTestResult );
        }
    else if ( !aApiTestName.Compare( _L( "USBPCSuitePersonalityTestL" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Api test type: USBPCSuitePersonalityTestL" );
        USBPCSuitePersonalityTestL( aTestResult );
        }
    else if ( !aApiTestName.Compare( _L( "USBDefaultPersonalityTestL" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Api test type: USBDefaultPersonalityTestL" );
        USBDefaultPersonalityTestL( aTestResult );
        }
    else
        {
        STIF_LOG( "[STIF_LOG] Api test type: not found" );
        User::Leave( KErrNotFound );
        }
	STIF_LOG( "[STIF_LOG] <<<DoExecuteApiTestL" );
    }

	
void CUSBLocodPluginTest::InitializationTestL( TUSBLocodPluginTestResult& aTestResult )
    {
	STIF_LOG( "[STIF_LOG] >>>InitializationTestL" );
	
	if( !iLocodBearerPlugin )
	    {
	    aTestResult = ETestCaseFailed;
	    return;
	    }
	
    aTestResult = ETestCasePassed;	
	STIF_LOG( "[STIF_LOG] <<<InitializationTestL" );
    }


void CUSBLocodPluginTest::ImplementatiationIdTestL( TUSBLocodPluginTestResult& aTestResult )
    {
    STIF_LOG( "[STIF_LOG] >>>ImplementatiationIdTestL" );
    
    User::LeaveIfNull( iLocodBearerPlugin );
    
    TUid implementationUid = TUid::Uid( KFeatureIdUsb );
    iLog -> Log( _L( "[STIF_LOG] Implementation Uid: %d, expected Uid: %d" ), implementationUid.iUid, iLocodBearerPlugin -> ImplementationUid().iUid );
    if ( iLocodBearerPlugin -> ImplementationUid() != implementationUid )
        {
        aTestResult = ETestCaseFailed;
        return;
        }
    
    aTestResult = ETestCasePassed;    
    STIF_LOG( "[STIF_LOG] <<<ImplementatiationIdTestL" );
    }


void CUSBLocodPluginTest::USBPCSuitePersonalityTestL( TUSBLocodPluginTestResult& aTestResult )
    {
    STIF_LOG( "[STIF_LOG] >>>USBPCSuitePersonalityTestL" );
    
    User::LeaveIfNull( iLocodBearerPlugin );
    TBool expectedLocodBearerStatus = ETrue;
    TBool returnedLocodBearerStatus;
    TInt res = iSelectedPersProperty -> Set( KUsbPersonalityIdPCSuite );
    iLog -> Log( _L( "[STIF_LOG] Set selected personality property result: %d" ), res );
    if ( res != KErrNone )
        {
        aTestResult = ETestCaseFailed;
        return;
        }
    iLocodBearerPluginObs -> ActivateObserver( returnedLocodBearerStatus );
    
    iLog -> Log( _L( "[STIF_LOG] Expected locod bearer status: %d, returned status: %d" ), 
                                        expectedLocodBearerStatus, returnedLocodBearerStatus );
    if ( returnedLocodBearerStatus != expectedLocodBearerStatus )
        {
        aTestResult = ETestCaseFailed;
        return;
        }
    
    aTestResult = ETestCasePassed;    
    STIF_LOG( "[STIF_LOG] <<<USBPCSuitePersonalityTestL" );
    }


void CUSBLocodPluginTest::USBDefaultPersonalityTestL( TUSBLocodPluginTestResult& aTestResult )
    {
    STIF_LOG( "[STIF_LOG] >>>USBDefaultPersonalityTestL" );
    
    User::LeaveIfNull( iLocodBearerPlugin );
    TBool expectedLocodBearerStatus = EFalse;
    TBool returnedLocodBearerStatus;
    TInt res = iSelectedPersProperty -> Set( KUsbDefaultPersonality );
    iLog -> Log( _L( "[STIF_LOG] Set selected personality property result: %d" ), res );
    if ( res != KErrNone )
        {
        aTestResult = ETestCaseFailed;
        return;
        }
    iLocodBearerPluginObs -> ActivateObserver( returnedLocodBearerStatus ); 
    
    iLog -> Log( _L( "[STIF_LOG] Expected locod bearer status: %d, returned status: %d" ), 
                                        expectedLocodBearerStatus, returnedLocodBearerStatus );
    if ( returnedLocodBearerStatus != expectedLocodBearerStatus )
        {
        aTestResult = ETestCaseFailed;
        return;
        }
    
    aTestResult = ETestCasePassed;    
    STIF_LOG( "[STIF_LOG] <<<USBDefaultPersonalityTestL" );
    }
	
// -----------------------------------------------------------------------------
// CUSBLocodPluginTest::ExecuteModuleTest
// -----------------------------------------------------------------------------	

TInt CUSBLocodPluginTest::ExecuteModuleTest( CStifItemParser& aItem )
    {
	STIF_LOG( "[STIF_LOG] >>>ExecuteModuleTest" );
	
    TInt res;
    TUSBLocodPluginTestResult testResult;
    TPtrC moduleTestName( KNullDesC );

    res = aItem.GetString( _L( "ExecuteModuleTest" ), moduleTestName );   
    if ( res != KErrNone )
        {
        iLog -> Log( _L( "GetString failed with value: %d" ), res );
        return res;
        }

    TRAP( res, DoExecuteModuleTestL( moduleTestName, testResult ) );
    if ( res != KErrNone )
        {
        iLog -> Log( _L( "DoExecuteModuleTestL error: %d"), res );
        return res;
        }
    
    STIF_ASSERT_EQUALS( ETestCasePassed, testResult );
    STIF_LOG( "[STIF_LOG] Test case passed" );
	STIF_LOG( "[STIF_LOG] <<<ExecuteModuleTest" );
    return KErrNone;
    }	
	
	
void CUSBLocodPluginTest::DoExecuteModuleTestL( TPtrC aModuleTestName, TUSBLocodPluginTestResult& aTestResult )
    {
	STIF_LOG( "[STIF_LOG] >>>DoExecuteModuleTestL" );
    if ( !aModuleTestName.Compare( _L( "USBMSPersonalityTestL" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Module test type: USBMSPersonalityTestL" );
        USBMSPersonalityTestL( aTestResult );
        }
    else if ( !aModuleTestName.Compare( _L( "USBPTPPersonalityTestL" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Module test type: USBPTPPersonalityTestL" );
        USBPTPPersonalityTestL( aTestResult );
        }
    else if ( !aModuleTestName.Compare( _L( "USBMTPPersonalityTestL" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Module test type: USBMTPPersonalityTestL" );
        USBMTPPersonalityTestL( aTestResult );
        }
    else
        {
        STIF_LOG( "[STIF_LOG] Module test type: not found" );
        User::Leave( KErrNotFound );
        }
	STIF_LOG( "[STIF_LOG] <<<DoExecuteModuleTestL" );
    }
	

void CUSBLocodPluginTest::USBMSPersonalityTestL( TUSBLocodPluginTestResult& aTestResult )
    {
    STIF_LOG( "[STIF_LOG] >>>USBMSPersonalityTestL" );
    
    User::LeaveIfNull( iLocodBearerPlugin );
    TBool expectedLocodBearerStatus = EFalse;
    TBool returnedLocodBearerStatus;
    TInt res = iSelectedPersProperty -> Set( KUsbPersonalityIdMS );
    iLog -> Log( _L( "[STIF_LOG] Set selected personality property result: %d" ), res );
    if ( res != KErrNone )
        {
        aTestResult = ETestCaseFailed;
        return;
        }
    iLocodBearerPluginObs -> ActivateObserver( returnedLocodBearerStatus ); 
    
    iLog -> Log( _L( "[STIF_LOG] Expected locod bearer status: %d, returned status: %d" ), 
                                        expectedLocodBearerStatus, returnedLocodBearerStatus );
    if ( returnedLocodBearerStatus != expectedLocodBearerStatus )
        {
        aTestResult = ETestCaseFailed;
        return;
        }
    
    aTestResult = ETestCasePassed;    
    STIF_LOG( "[STIF_LOG] <<<USBMSPersonalityTestL" );
    }


void CUSBLocodPluginTest::USBPTPPersonalityTestL( TUSBLocodPluginTestResult& aTestResult )
    {
    STIF_LOG( "[STIF_LOG] >>>USBPTPPersonalityTestL" );
    
    User::LeaveIfNull( iLocodBearerPlugin );
    TBool expectedLocodBearerStatus = EFalse;
    TBool returnedLocodBearerStatus;
    TInt res = iSelectedPersProperty -> Set( KUsbPersonalityIdPTP );
    iLog -> Log( _L( "[STIF_LOG] Set selected personality property result: %d" ), res );
    if ( res != KErrNone )
        {
        aTestResult = ETestCaseFailed;
        return;
        }
    iLocodBearerPluginObs -> ActivateObserver( returnedLocodBearerStatus ); 
    
    iLog -> Log( _L( "[STIF_LOG] Expected locod bearer status: %d, returned status: %d" ), 
                                        expectedLocodBearerStatus, returnedLocodBearerStatus );
    if ( returnedLocodBearerStatus != expectedLocodBearerStatus )
        {
        aTestResult = ETestCaseFailed;
        return;
        }
    
    aTestResult = ETestCasePassed;   
    STIF_LOG( "[STIF_LOG] <<<USBPTPPersonalityTestL" );
    }


void CUSBLocodPluginTest::USBMTPPersonalityTestL( TUSBLocodPluginTestResult& aTestResult )
    {
    STIF_LOG( "[STIF_LOG] >>>USBMTPPersonalityTestL" );
    
    User::LeaveIfNull( iLocodBearerPlugin );
    TBool expectedLocodBearerStatus = EFalse;
    TBool returnedLocodBearerStatus;
    TInt res = iSelectedPersProperty -> Set( KUsbPersonalityIdMTP );
    iLog -> Log( _L( "[STIF_LOG] Set selected personality property result: %d" ), res );
    if ( res != KErrNone )
        {
        aTestResult = ETestCaseFailed;
        return;
        }
    iLocodBearerPluginObs -> ActivateObserver( returnedLocodBearerStatus ); 
    
    iLog -> Log( _L( "[STIF_LOG] Expected locod bearer status: %d, returned status: %d" ), 
                                        expectedLocodBearerStatus, returnedLocodBearerStatus );
    if ( returnedLocodBearerStatus != expectedLocodBearerStatus )
        {
        aTestResult = ETestCaseFailed;
        return;
        }
    
    aTestResult = ETestCasePassed;  
    STIF_LOG( "[STIF_LOG] <<<USBMTPPersonalityTestL" );
    }
	

// -----------------------------------------------------------------------------
// CUSBLocodPluginTest::ExecuteBranchTest
// -----------------------------------------------------------------------------
	
TInt CUSBLocodPluginTest::ExecuteBranchTest( CStifItemParser& aItem )
    {
	STIF_LOG( "[STIF_LOG] >>>ExecuteBranchTest" );
	
    TInt res;
    TUSBLocodPluginTestResult testResult;
    TPtrC branchTestName( KNullDesC );

    res = aItem.GetString( _L( "ExecuteBranchTest" ), branchTestName );   
    if ( res != KErrNone )
        {
        iLog -> Log( _L( "GetString failed with value: %d" ), res );
        return res;
        }

    TRAP( res, DoExecuteBranchTestL( branchTestName, testResult ) );
    if ( res != KErrNone )
        {
        iLog -> Log( _L( "DoExecuteBranchTestL error: %d"), res );
        return res;
        }
    
    STIF_ASSERT_EQUALS( ETestCasePassed, testResult );
    STIF_LOG( "[STIF_LOG] Test case passed" );
	STIF_LOG( "[STIF_LOG] <<<ExecuteBranchTest" );
    return KErrNone;
    }

	
void CUSBLocodPluginTest::DoExecuteBranchTestL( TPtrC aBranchTestName, TUSBLocodPluginTestResult& aTestResult )
    {
	STIF_LOG( "[STIF_LOG] >>>DoExecuteBranchTestL" );
    if ( !aBranchTestName.Compare( _L( "USBPCSuiteMTPPersonalityTestL" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Branch test type: USBPCSuiteMTPPersonalityTestL" );
        USBPCSuiteMTPPersonalityTestL( aTestResult );
        }
    else
        {
        STIF_LOG( "[STIF_LOG] Branch test type: not found" );
        User::Leave( KErrNotFound );
        }
	STIF_LOG( "[STIF_LOG] <<<DoExecuteBranchTestL" );
    }
	
	
void CUSBLocodPluginTest::USBPCSuiteMTPPersonalityTestL( TUSBLocodPluginTestResult& aTestResult )
    {
    STIF_LOG( "[STIF_LOG] >>>USBPCSuiteMTPPersonalityTestL" );
    
    User::LeaveIfNull( iLocodBearerPlugin );
    TBool expectedLocodBearerStatus = ETrue;
    TBool returnedLocodBearerStatus;
    TInt res = iSelectedPersProperty -> Set( KUsbPersonalityIdPCSuiteMTP );
    iLog -> Log( _L( "[STIF_LOG] Set selected personality property result: %d" ), res );
    if ( res != KErrNone )
        {
        aTestResult = ETestCaseFailed;
        return;
        }
    iLocodBearerPluginObs -> ActivateObserver( returnedLocodBearerStatus ); 
    
    iLog -> Log( _L( "[STIF_LOG] Expected locod bearer status: %d, returned status: %d" ), 
                                        expectedLocodBearerStatus, returnedLocodBearerStatus );
    if ( returnedLocodBearerStatus != expectedLocodBearerStatus )
        {
        aTestResult = ETestCaseFailed;
        return;
        }

    aTestResult = ETestCasePassed;    
    STIF_LOG( "[STIF_LOG] <<<USBPCSuiteMTPPersonalityTestL" );
    }
	
// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
