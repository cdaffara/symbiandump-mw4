/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
#include "favouritesenginetest.h"
#include "favouritesdbtestobserver.h"

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

    Class: CFavouritesEngineTest

    Method: NotifierConstructorTestL

    Description: Test the CActiveFavouritesDbNotifier constructor method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::NotifierConstructorTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State" );
    _LIT( KData ,"Test the CActiveFavouritesDbNotifier constructor method" );
    TestModuleIf().Printf( 0, KDefinition, KData );
    
    iFavouritesDb.Open( iFavouritesSession, KTestDbName );
    CleanupClosePushL<RFavouritesDb>( iFavouritesDb );
    
    CreatePopulatedDbL();

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    CFavouritesDbTestObserver* observer = new (ELeave) CFavouritesDbTestObserver( *this );
    CleanupStack::PushL( observer );

    observer->Start();
    CActiveScheduler::Start();

    CActiveFavouritesDbNotifier* notifier = new (ELeave) CActiveFavouritesDbNotifier( iFavouritesDb, (MFavouritesDbObserver &)*this );
    CleanupStack::PushL( notifier );

    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );
	
    if( notifier )
	    {
	    _LIT( KDescription , "Test case passed" );
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed" );
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy(4); // iFavouritesDb, observer, notifier
    iFavouritesSession.DeleteDatabase( KTestDbName );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: NotifierDestructorTestL

    Description: Test the CActiveFavouritesDbNotifier destructor method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::NotifierDestructorTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State" );
    _LIT( KData ,"Test the CActiveFavouritesDbNotifier destructor method" );
    TestModuleIf().Printf( 0, KDefinition, KData );
    
    iFavouritesDb.Open( iFavouritesSession, KTestDbName );
    CleanupClosePushL<RFavouritesDb>( iFavouritesDb );
    
    CreatePopulatedDbL();

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    CFavouritesDbTestObserver* observer = new (ELeave) CFavouritesDbTestObserver( *this );
    CleanupStack::PushL( observer );

    observer->Start();
    CActiveScheduler::Start();

    CActiveFavouritesDbNotifier* notifier = new (ELeave) CActiveFavouritesDbNotifier( iFavouritesDb, (MFavouritesDbObserver &)*this );
    CleanupStack::PushL( notifier );
    
    notifier->~CActiveFavouritesDbNotifier();

    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );
	
    if( notifier )
	    {
	    _LIT( KDescription , "Test case passed" );
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed" );
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy(4); // iFavouritesDb, observer, notifier
    iFavouritesSession.DeleteDatabase( KTestDbName );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: NotifierStartTestL

    Description: Test the CActiveFavouritesDbNotifier Start method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::NotifierStartTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State" );
    _LIT( KData ,"Test the CActiveFavouritesDbNotifier Start method" );
    TestModuleIf().Printf( 0, KDefinition, KData );
    
    iFavouritesDb.Open( iFavouritesSession, KTestDbName );
    CleanupClosePushL<RFavouritesDb>( iFavouritesDb );
    
    CreatePopulatedDbL();

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    CFavouritesDbTestObserver* observer = new (ELeave) CFavouritesDbTestObserver( *this );
    CleanupStack::PushL( observer );

    observer->Start();
    CActiveScheduler::Start();

    CActiveFavouritesDbNotifier* notifier = new (ELeave) CActiveFavouritesDbNotifier( iFavouritesDb, (MFavouritesDbObserver &)*this );
    CleanupStack::PushL( notifier );
    
    TInt result = notifier->Start();

    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );
	
    if( result == KErrNone )
	    {
	    _LIT( KDescription , "Test case passed" );
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed" );
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy(4); // iFavouritesDb, observer, notifier
    iFavouritesSession.DeleteDatabase( KTestDbName );

    // Case was executed
    return KErrNone;
    }

// ================= OTHER EXPORTED FUNCTIONS =================================

// End of File
