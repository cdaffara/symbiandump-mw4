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
#include "USBRemotePersonalityTest.h"

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
// CUSBRemotePersonalityTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CUSBRemotePersonalityTest::Delete() 
    {    
    REComSession::DestroyedImplementation( iPluginDtorIDKey );        
    if ( iPlugin )
        {
        delete iPlugin;
        iPlugin = NULL;
        }
    REComSession::FinalClose();
    
    iSetupBuff.Close();
    iDataBuff.Close();
    
    if ( iUsbMan )
        {
        iUsbMan -> Close();
        delete iUsbMan;
        iUsbMan = NULL;
        }
    
    if ( iUsbWatcher )
        {
        iUsbWatcher -> Close();
        delete iUsbWatcher;
        iUsbWatcher = NULL;
        }
    
    if ( iDevUsbClient )
        {
        iDevUsbClient -> Close();
        delete iDevUsbClient;
        iDevUsbClient = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CUSBRemotePersonalityTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CUSBRemotePersonalityTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
		ENTRY( "ExecuteApiTest", CUSBRemotePersonalityTest::ExecuteApiTest ),
        ENTRY( "ExecuteModuleTest", CUSBRemotePersonalityTest::ExecuteModuleTest ),
        ENTRY( "ExecuteBranchTest", CUSBRemotePersonalityTest::ExecuteBranchTest ),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CUSBRemotePersonalityTest::ExecuteApiTest
// -----------------------------------------------------------------------------

TInt CUSBRemotePersonalityTest::ExecuteApiTest( CStifItemParser& aItem )
    {
	STIF_LOG( "[STIF_LOG] >>>ExecuteApiTest" );
	
	TInt res;
    TUSBRemotePersonalityTestResult testResult;
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
	
	
void CUSBRemotePersonalityTest::DoExecuteApiTestL( TPtrC aApiTestName, TUSBRemotePersonalityTestResult& aTestResult )
    {
	STIF_LOG( "[STIF_LOG] >>>DoExecuteApiTestL" );
    if ( !aApiTestName.Compare( _L( "CreatePluginImplementationL" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Api test type: CreatePluginImplementationL" );
        CreatePluginImplementationTestL( aTestResult );
        }
    else if ( !aApiTestName.Compare( _L( "InitializeTestL" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Api test type: InitializeTestL" );
        InitializeTestL( aTestResult );
        }
    else if ( !aApiTestName.Compare( _L( "CheckLastResultTestL" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Api test type: CheckLastResultTestL" );
        CheckLastResultTestL( aTestResult );
        }
    else if ( !aApiTestName.Compare( _L( "GetAllPersonalitiesTestL" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Api test type: GetAllPersonalitiesTestL" );
        GetAllPersonalitiesTestL( aTestResult );
        }
    else if ( !aApiTestName.Compare( _L( "SetPersonalityTestL" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Api test type: SetPersonalityTestL" );
        SetPersonalityTestL( aTestResult );
        }
    else if ( !aApiTestName.Compare( _L( "GetPersonalityTestL" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Api test type: GetPersonalityTestL" );
        GetPersonalityTestL( aTestResult );
        }
    else if ( !aApiTestName.Compare( _L( "GetPersonalityDesTestL" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Api test type: GetPersonalityDesTestL" );
        GetPersonalityDesTestL( aTestResult );
        }
    else if ( !aApiTestName.Compare( _L( "GetLockStateTestL" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Api test type: GetLockStateTestL" );
        GetLockStateTestL( aTestResult );
        }
    else if ( !aApiTestName.Compare( _L( "SetLockStateTestL" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Api test type: SetLockStateTestL" );
        SetLockStateTestL( aTestResult );
        }
    else
        {
        STIF_LOG( "[STIF_LOG] Api test type: not found" );
        User::Leave( KErrNotFound );
        }
	STIF_LOG( "[STIF_LOG] <<<DoExecuteApiTestL" );
    }


void CUSBRemotePersonalityTest::CreatePluginImplementationTestL( TUSBRemotePersonalityTestResult& aTestResult )
    {
    STIF_LOG( "[STIF_LOG] >>>CreatePluginImplementationTestL" );
    
    if ( !iPlugin )
        {
        aTestResult = ETestCaseFailed;
        return;
        }
    
    aTestResult = ETestCasePassed;
    STIF_LOG( "[STIF_LOG] <<<CreatePluginImplementationTestL" );
    }


void CUSBRemotePersonalityTest::InitializeTestL( TUSBRemotePersonalityTestResult& aTestResult )
    {
    STIF_LOG( "[STIF_LOG] >>>InitializeTestL" );
    
    iPlugin -> Initialize( *iDevUsbClient, *iUsbWatcher, *iUsbMan );
    if ( !iPlugin )
        {
        aTestResult = ETestCaseFailed;
        return;
        }
    
    aTestResult = ETestCasePassed;
    STIF_LOG( "[STIF_LOG] <<<InitializeTestL" );
    }


void CUSBRemotePersonalityTest::CheckLastResultTestL( TUSBRemotePersonalityTestResult& aTestResult )
    {
    STIF_LOG( "[STIF_LOG] >>>CheckLastResultTestL" );
    TInt res;
    TUint8 getLastResult;
    TInt getLastResultLenght;
    CRemotePersonalityHandler::TLastResult lastResult;
    
    User::LeaveIfNull( iPlugin );
    iPlugin -> Initialize( *iDevUsbClient, *iUsbWatcher, *iUsbMan );
    STIF_LOG( "[STIF_LOG] CheckLastResultTestL::initialization completed" );
    
    getLastResult = CRemotePersonalityHandler::EGetLastResult;
    getLastResultLenght = sizeof( getLastResult );
    TPtrC8 const getLastResultString( &getLastResult, getLastResultLenght );
    
    iSetupBuff.FillZ( KSetupPacketLength );
    iSetupBuff.Replace( 1, 1, getLastResultString );
    
    res = iPlugin->Handle( iSetupBuff, iDataBuff );
    if ( res != KErrNone )
        {
        iLog -> Log( _L( "Handle failed with value: %d"), res );
        aTestResult = ETestCaseFailed;
        return;
        }    
    STIF_LOG( "[STIF_LOG] Handle request completed" );
    
    lastResult = static_cast< CRemotePersonalityHandler::TLastResult >( iDataBuff[0] );
    iLog -> Log( _L( "Last result value: %d, expected %d"), lastResult, CRemotePersonalityHandler::EUndefinedError );   
    if ( lastResult != CRemotePersonalityHandler::EUndefinedError )
        {
        aTestResult = ETestCaseFailed;
        return;
        }

    aTestResult = ETestCasePassed;
    STIF_LOG( "[STIF_LOG] <<<CheckLastResultTestL" );
    }
	
void CUSBRemotePersonalityTest::GetAllPersonalitiesTestL( TUSBRemotePersonalityTestResult& aTestResult )
    {
    STIF_LOG( "[STIF_LOG] >>>GetAllPersonalitiesTestL" );
    TInt res;
    TUint8 getAll;
    TInt getAllLenght;
    
    User::LeaveIfNull( iPlugin );
    iPlugin -> Initialize( *iDevUsbClient, *iUsbWatcher, *iUsbMan );
    STIF_LOG( "[STIF_LOG] CheckLastResultTestL::initialization completed" );
        
    getAll = CRemotePersonalityHandler::EGetAllPersonalities;
    getAllLenght = sizeof( getAll );
    TPtrC8 const getAllString( &getAll, getAllLenght );
    
    iSetupBuff.FillZ( KSetupPacketLength );
    iSetupBuff.Replace( 1, 1, getAllString );
  
    res = iPlugin->Handle( iSetupBuff, iDataBuff );
    if ( res != KErrNone )
        {
        iLog -> Log( _L( "Handle failed with value: %d"), res );
        aTestResult = ETestCaseFailed;
        return;
        }    
    STIF_LOG( "[STIF_LOG] Handle request completed" );

    iLog -> Log( _L( "Data length: %d, expected != 0"), iDataBuff.Length() ); 
    if ( !iDataBuff.Length() )
        {
        aTestResult = ETestCaseFailed;
        return;
        }

    aTestResult = ETestCasePassed;
    STIF_LOG( "[STIF_LOG] <<<GetAllPersonalitiesTestL" );
    }


void CUSBRemotePersonalityTest::SetPersonalityTestL( TUSBRemotePersonalityTestResult& aTestResult )
    {
    STIF_LOG( "[STIF_LOG] >>>SetPersonalityTestL" );
    TInt res;
    TUint8 setPersonality;
    TInt setPersonalityLenght;
    TUint8 personalityType;
    TInt personalityTypeLenght;
    TUint8 getLastResult;
    TInt getLastResultLenght;
    CRemotePersonalityHandler::TLastResult lastResult;
    
    User::LeaveIfNull( iPlugin );
    iPlugin -> Initialize( *iDevUsbClient, *iUsbWatcher, *iUsbMan );
    STIF_LOG( "[STIF_LOG] CheckLastResultTestL::initialization completed" );
    
    // Request type should be set in second byte
    // Personality type should be set in third byte
    setPersonality = CRemotePersonalityHandler::ESetPersonality;
    setPersonalityLenght = sizeof( setPersonality );
    TPtrC8 const setPersonalityString( &setPersonality, setPersonalityLenght );
    personalityType = KUsbPersonalityIdMTP;
    personalityTypeLenght = sizeof( personalityType );
    TPtrC8 const personalityTypeString( &personalityType, personalityTypeLenght );
    
    iSetupBuff.FillZ( KSetupPacketLength );   
    iSetupBuff.Replace( 1, 1, setPersonalityString );
    iSetupBuff.Replace( 2, 1, personalityTypeString );

    res = iPlugin->Handle( iSetupBuff, iDataBuff );
    if ( res != KErrNone )
        {
        iLog -> Log( _L( "Handle failed with value: %d"), res );
        aTestResult = ETestCaseFailed;
        return;
        }    
    STIF_LOG( "[STIF_LOG] Handle request completed" );

    getLastResult = CRemotePersonalityHandler::EGetLastResult;
    getLastResultLenght = sizeof( getLastResult );
    TPtrC8 const getLastResultString( &getLastResult, getLastResultLenght );
    
    iSetupBuff.FillZ( KSetupPacketLength );
    iDataBuff.FillZ();
    iSetupBuff.Replace( 1, 1, getLastResultString );

    res = iPlugin -> Handle( iSetupBuff, iDataBuff );
    if ( res != KErrNone )
        {
        iLog -> Log( _L( "Handle failed with value: %d"), res );
        aTestResult = ETestCaseFailed;
        return;
        }    
    STIF_LOG( "[STIF_LOG] Handle request completed" );
    
    lastResult = static_cast< CRemotePersonalityHandler::TLastResult >( iDataBuff[0] );
    iLog -> Log( _L( "Last result value: %d, expected %d"), lastResult, CRemotePersonalityHandler::EDataTransferInProgress );   
    if ( lastResult != CRemotePersonalityHandler::EDataTransferInProgress )
        {
        aTestResult = ETestCaseFailed;
        return;
        }

    aTestResult = ETestCasePassed;
    STIF_LOG( "[STIF_LOG] <<<SetPersonalityTestL" );
    }


void CUSBRemotePersonalityTest::GetPersonalityTestL( TUSBRemotePersonalityTestResult& aTestResult )
    {
    STIF_LOG( "[STIF_LOG] >>>GetPersonalityTestL" );
    TInt res;
    TUint8 getPersonality;
    TInt getPersonalityLenght;
    
    User::LeaveIfNull( iPlugin );
    iPlugin -> Initialize( *iDevUsbClient, *iUsbWatcher, *iUsbMan );
    STIF_LOG( "[STIF_LOG] CheckLastResultTestL::initialization completed" );
    
    // Request type should be set in second byte
    getPersonality = CRemotePersonalityHandler::EGetPersonality;
    getPersonalityLenght = sizeof( getPersonality );
    TPtrC8 const getPersonalityString( &getPersonality, getPersonalityLenght );
    
    iSetupBuff.FillZ( KSetupPacketLength );
    iSetupBuff.Replace( 1, 1, getPersonalityString );

    res = iPlugin->Handle( iSetupBuff, iDataBuff );
    if ( res != KErrNotSupported )
        {
        iLog -> Log( _L( "Handle completion value: %d"), res );
        aTestResult = ETestCaseFailed;
        return;
        }    
    STIF_LOG( "[STIF_LOG] Handle request completed" );
    
    aTestResult = ETestCasePassed;
    STIF_LOG( "[STIF_LOG] <<<GetPersonalityTestL" );
    }


void CUSBRemotePersonalityTest::GetPersonalityDesTestL( TUSBRemotePersonalityTestResult& aTestResult )
    {
    STIF_LOG( "[STIF_LOG] >>>GetPersonalityDesTestL" );
    TInt res;
    TUint8 getPersonalityDes;
    TInt getPersonalityDesLenght;
    
    User::LeaveIfNull( iPlugin );
    iPlugin -> Initialize( *iDevUsbClient, *iUsbWatcher, *iUsbMan );
    STIF_LOG( "[STIF_LOG] CheckLastResultTestL::initialization completed" );
    
    // Request type should be set in second byte
    getPersonalityDes = CRemotePersonalityHandler::EGetPersonalityDescriptor;
    getPersonalityDesLenght = sizeof( getPersonalityDes );
    TPtrC8 const getPersonalityDesString( &getPersonalityDes, getPersonalityDesLenght );
    
    iSetupBuff.FillZ( KSetupPacketLength );
    iSetupBuff.Replace( 1, 1, getPersonalityDesString );

    res = iPlugin->Handle( iSetupBuff, iDataBuff );
    if ( res != KErrNotSupported )
        {
        iLog -> Log( _L( "Handle completion value: %d"), res );
        aTestResult = ETestCaseFailed;
        return;
        }    
    STIF_LOG( "[STIF_LOG] Handle request completed" );
    
    aTestResult = ETestCasePassed;
    STIF_LOG( "[STIF_LOG] <<<GetPersonalityDesTestL" );
    }


void CUSBRemotePersonalityTest::GetLockStateTestL( TUSBRemotePersonalityTestResult& aTestResult )
    {
    STIF_LOG( "[STIF_LOG] >>>GetLockStateTestL" );
    TInt res;
    TUint8 getLockState;
    TInt getLockStateLenght;
    
    User::LeaveIfNull( iPlugin );
    iPlugin -> Initialize( *iDevUsbClient, *iUsbWatcher, *iUsbMan );
    STIF_LOG( "[STIF_LOG] CheckLastResultTestL::initialization completed" );
    
    // Request type should be set in second byte
    getLockState = CRemotePersonalityHandler::EGetLockState;
    getLockStateLenght = sizeof( getLockState );
    TPtrC8 const getLockStateString( &getLockState, getLockStateLenght );
    
    iSetupBuff.FillZ( KSetupPacketLength );
    iSetupBuff.Replace( 1, 1, getLockStateString );

    res = iPlugin->Handle( iSetupBuff, iDataBuff );
    if ( res != KErrNotSupported )
        {
        iLog -> Log( _L( "Handle completion value: %d"), res );
        aTestResult = ETestCaseFailed;
        return;
        }    
    STIF_LOG( "[STIF_LOG] Handle request completed" );
    
    aTestResult = ETestCasePassed;
    STIF_LOG( "[STIF_LOG] <<<GetLockStateTestL" );
    }


void CUSBRemotePersonalityTest::SetLockStateTestL( TUSBRemotePersonalityTestResult& aTestResult )
    {
    STIF_LOG( "[STIF_LOG] >>>SetLockStateTestL" );
    TInt res;
    TUint8 setLockState;
    TInt setLockStateLenght;
    
    User::LeaveIfNull( iPlugin );
    iPlugin -> Initialize( *iDevUsbClient, *iUsbWatcher, *iUsbMan );
    STIF_LOG( "[STIF_LOG] CheckLastResultTestL::initialization completed" );
    
    // Request type should be set in second byte
    setLockState = CRemotePersonalityHandler::ESetLockState;
    setLockStateLenght = sizeof( setLockState );
    TPtrC8 const setLockStateString( &setLockState, setLockStateLenght );
    
    iSetupBuff.FillZ( KSetupPacketLength );
    iSetupBuff.Replace( 1, 1, setLockStateString );

    res = iPlugin->Handle( iSetupBuff, iDataBuff );
    if ( res != KErrNotSupported )
        {
        iLog -> Log( _L( "Handle completion value: %d"), res );
        aTestResult = ETestCaseFailed;
        return;
        }    
    STIF_LOG( "[STIF_LOG] Handle request completed" );
 
    aTestResult = ETestCasePassed;
    STIF_LOG( "[STIF_LOG] <<<SetLockStateTestL" );
    }


// -----------------------------------------------------------------------------
// CUSBRemotePersonalityTest::ExecuteModuleTest
// -----------------------------------------------------------------------------	

TInt CUSBRemotePersonalityTest::ExecuteModuleTest( CStifItemParser& aItem )
    {
	STIF_LOG( "[STIF_LOG] >>>ExecuteModuleTest" );
	
    TInt res;
    TUSBRemotePersonalityTestResult testResult;
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
	
	
void CUSBRemotePersonalityTest::DoExecuteModuleTestL( TPtrC aModuleTestName, TUSBRemotePersonalityTestResult& aTestResult )
    {
	STIF_LOG( "[STIF_LOG] >>>DoExecuteModuleTestL" );
    if ( !aModuleTestName.Compare( _L( "ExampleModuleTest" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Module test type: ExampleModuleTest" );
        ExampleModuleTestL( aTestResult );
        }
    else
        {
        STIF_LOG( "[STIF_LOG] Module test type: not found" );
        User::Leave( KErrNotFound );
        }
	STIF_LOG( "[STIF_LOG] <<<DoExecuteModuleTestL" );
    }
	
	
void CUSBRemotePersonalityTest::ExampleModuleTestL( TUSBRemotePersonalityTestResult& aTestResult )
    {
	STIF_LOG( "[STIF_LOG] >>>ExampleModuleTestL" );
	
    aTestResult = ETestCasePassed;
	
	STIF_LOG( "[STIF_LOG] <<<ExampleModuleTestL" );
    }
	
// -----------------------------------------------------------------------------
// CUSBRemotePersonalityTest::ExecuteBranchTest
// -----------------------------------------------------------------------------
	
TInt CUSBRemotePersonalityTest::ExecuteBranchTest( CStifItemParser& aItem )
    {
	STIF_LOG( "[STIF_LOG] >>>ExecuteBranchTest" );
	
    TInt res;
    TUSBRemotePersonalityTestResult testResult;
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

	
void CUSBRemotePersonalityTest::DoExecuteBranchTestL( TPtrC aBranchTestName, TUSBRemotePersonalityTestResult& aTestResult )
    {
	STIF_LOG( "[STIF_LOG] >>>DoExecuteBranchTestL" );
    if ( !aBranchTestName.Compare( _L( "ExampleBranchTest" ) ) )
        {
        STIF_LOG( "[STIF_LOG] Branch test type: ExampleBranchTest" );
        ExampleBranchTestL( aTestResult );
        }
    else
        {
        STIF_LOG( "[STIF_LOG] Branch test type: not found" );
        User::Leave( KErrNotFound );
        }
	STIF_LOG( "[STIF_LOG] <<<DoExecuteBranchTestL" );
    }
	
	
void CUSBRemotePersonalityTest::ExampleBranchTestL( TUSBRemotePersonalityTestResult& aTestResult )
    {
	STIF_LOG( "[STIF_LOG] >>>ExampleBranchTestL" );
	
    aTestResult = ETestCasePassed;
    
	STIF_LOG( "[STIF_LOG] <<<ExampleBranchTestL" );
	}
	
// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
