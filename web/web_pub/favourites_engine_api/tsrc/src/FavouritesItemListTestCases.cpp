/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

// -----------------------------------------------------------------------------
// CompareItemNamesL
// Compare the names of two favourite list items.
// Returns: negative, if aItem1.Name is less than aItem2.Name
//          0, if aItem1.Name is the same as aItem2.Name 
//          positive, if aItem1.Name is greater than aItem2.Name
// -----------------------------------------------------------------------------
//
LOCAL_C TInt CompareItemNamesL(const CFavouritesItem& aItem1, const CFavouritesItem& aItem2)
    {
    return aItem1.Name().CompareF(aItem2.Name());
    }

// ============================ MEMBER FUNCTIONS ===============================

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ListConstructorTestL

    Description: Test the item list constructor method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ListConstructorTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the item list constructor method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItemList* list = new (ELeave) CFavouritesItemList();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (list)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    
	    delete list;
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ListDestructorTestL

    Description: Test the item list destructor method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ListDestructorTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the item list destructor method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItemList* list = new (ELeave) CFavouritesItemList();

    list->~CFavouritesItemList();
    
    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (list)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    
	    delete list;
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ListDeleteTestL

    Description: Test the item list Delete method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ListDeleteTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the item list Delete method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    iFavouritesDb.Open( iFavouritesSession, KTestDbName );
    CleanupClosePushL<RFavouritesDb>( iFavouritesDb );

    CreatePopulatedDbL();
    
    CFavouritesItemList* list = new (ELeave) CFavouritesItemList();

    iFavouritesDb.GetAll( *list, KFavouritesRootUid );

    TInt countBeforeDelete = list->Count();
    
    list->Delete( 1 );
    
    TInt countAfterDelete = list->Count();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (countAfterDelete == countBeforeDelete - 1)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    delete list;
    CleanupStack::PopAndDestroy(); // iFavouritesDb
    iFavouritesSession.DeleteDatabase( KTestDbName );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ListDeleteRangeTestL

    Description: Test deleting a range of items using the item list Delete method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ListDeleteRangeTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the item list Delete method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    iFavouritesDb.Open( iFavouritesSession, KTestDbName );
    CleanupClosePushL<RFavouritesDb>( iFavouritesDb );

    CreatePopulatedDbL();
    
    CFavouritesItemList* list = new (ELeave) CFavouritesItemList();

    iFavouritesDb.GetAll( *list, KFavouritesRootUid );

    TInt countBeforeDelete = list->Count();
    
    list->Delete( 1, 3 );
    
    TInt countAfterDelete = list->Count();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (countAfterDelete == countBeforeDelete - 3)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    delete list;
    CleanupStack::PopAndDestroy(); // iFavouritesDb
    iFavouritesSession.DeleteDatabase( KTestDbName );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ListSortTestL

    Description: Test sorting the list using the Sort method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ListSortTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the item list Delete method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    iFavouritesDb.Open( iFavouritesSession, KTestDbName );
    CleanupClosePushL<RFavouritesDb>( iFavouritesDb );

    CreatePopulatedDbL();
    
    CFavouritesItemList* list = new (ELeave) CFavouritesItemList();

    iFavouritesDb.GetAll( *list, KFavouritesRootUid );

    list->SortL( &CompareItemNamesL );

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );
    
    const CFavouritesItem* firstItem = list->ItemByUid( list->IndexToUid( 0 ) );

    if (firstItem->Name() == _L("iItem1"))
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    delete list;
    CleanupStack::PopAndDestroy(); // iFavouritesDb
    iFavouritesSession.DeleteDatabase( KTestDbName );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ListUidToIndexTestL

    Description: Test uid to index conversion using the UidToIndex method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ListUidToIndexTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the item list Delete method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    iFavouritesDb.Open( iFavouritesSession, KTestDbName );
    CleanupClosePushL<RFavouritesDb>( iFavouritesDb );

    CreatePopulatedDbL();
    
    CFavouritesItemList* list = new (ELeave) CFavouritesItemList();

    iFavouritesDb.GetAll( *list, KFavouritesRootUid );
    
    list->SortL( &CompareItemNamesL );

    const CFavouritesItem* firstItem = list->ItemByUid( list->IndexToUid( 1 ) );

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );
    
    if (firstItem->Uid() == list->IndexToUid( 1 ))
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    delete list;
    CleanupStack::PopAndDestroy(); // iFavouritesDb
    iFavouritesSession.DeleteDatabase( KTestDbName );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ListIndexToUidTestL

    Description: Test index to uid conversion using the UidToIndex method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ListIndexToUidTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the item list Delete method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    iFavouritesDb.Open( iFavouritesSession, KTestDbName );
    CleanupClosePushL<RFavouritesDb>( iFavouritesDb );

    CreatePopulatedDbL();
    
    CFavouritesItemList* list = new (ELeave) CFavouritesItemList();

    iFavouritesDb.GetAll( *list, KFavouritesRootUid );
    
    list->SortL( &CompareItemNamesL );
    
    TInt itemUid = list->IndexToUid(1);

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );
    
    if (itemUid == list->At(1)->Uid())
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    delete list;
    CleanupStack::PopAndDestroy(); // iFavouritesDb
    iFavouritesSession.DeleteDatabase( KTestDbName );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ListItemByUidTestL

    Description: Test retrieveing an item by Id using the ItemByUid method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ListItemByUidTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the item list Delete method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    iFavouritesDb.Open( iFavouritesSession, KTestDbName );
    CleanupClosePushL<RFavouritesDb>( iFavouritesDb );

    CreatePopulatedDbL();
    
    CFavouritesItemList* list = new (ELeave) CFavouritesItemList();

    iFavouritesDb.GetAll( *list, KFavouritesRootUid );
    
    list->SortL( &CompareItemNamesL );
    
    const CFavouritesItem* firstItem = list->ItemByUid( list->IndexToUid( 0 ) );

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );
    
    if (firstItem->Name() == _L("iItem1"))
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    delete list;
    CleanupStack::PopAndDestroy(); // iFavouritesDb
    iFavouritesSession.DeleteDatabase( KTestDbName );

    // Case was executed
    return KErrNone;
    }

// ================= OTHER EXPORTED FUNCTIONS =================================

// End of File
