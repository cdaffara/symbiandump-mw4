/*
* ============================================================================
*  Name:      favouritesitemtestcases.cpp
*  Part of:   FavouritesEngineTest class member functions   
*
*  Description:
*
*  Version:   1.0
*
*  Copyright (C) 2008 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation. All rights are reserved. Copying, 
*  including reproducing, storing,  adapting or translating, any 
*  or all of this material requires the prior written consent of 
*  Nokia Corporation. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia Corporation.
*
* ============================================================================
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

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CTestModuleDemo class 
    member functions that does the actual tests.

-------------------------------------------------------------------------------
*/

// ============================ MEMBER FUNCTIONS ===============================

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemNewLCTestL

    Description: Test creating a new favourites item with the NewLC method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemNewLCTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Creating item with NewLC method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (item)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemNewLTestL

    Description: Test creating a new favourites item with the NewL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemNewLTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Creating item with NewL method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewL();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (item)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    
	    delete item;
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

    Method: ItemDestructorTestL

    Description: Test destroying an item with the ~CFavouritesItem method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemDestructorTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Destroying item with ~CFavouritesItem method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewL();
    
    delete item;

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    _LIT( KDescription , "Test case passed");
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemAssignTestL

    Description: Assigning one item to another with the Assign(=) method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemAssignTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Assigning item with Assign(=) operator");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item1 = CFavouritesItem::NewLC();
    item1->SetNameL( _L("Item1") );
    CFavouritesItem* item2 = CFavouritesItem::NewLC();
    item2->SetNameL( _L("Item2") );

    *item1 = *item2;
    
    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (item1->Name() == item2->Name())
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( 2 );   // item1, item2

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemAssignCompleteTestL

    Description: Assigning one item to another with the Assign(=) method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemAssignCompleteTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Assigning item with Assign(=) operator");
    TestModuleIf().Printf( 0, KDefinition, KData );
    
    TFavouritesWapAp accessPoint1;
    accessPoint1.SetApId( 22 );

    CFavouritesItem* item1 = CFavouritesItem::NewLC();
    item1->SetNameL( _L("Item1") );
    item1->SetUrlL( _L("http://www.nokia.com") );
    item1->SetUserNameL( _L("Name 1"));
    item1->SetPasswordL( _L("Password 1"));
    item1->SetParentFolder(3);
    item1->SetContextId(22);
    item1->SetHidden(1);
    item1->SetWapAp( accessPoint1 );   
    
    
    TFavouritesWapAp accessPoint2;
    accessPoint2.SetApId( 24 );

    CFavouritesItem* item2 = CFavouritesItem::NewLC();
    item2->SetNameL( _L("Item2") );
    item2->SetUrlL( _L("http://www.google.com") );
    item2->SetUserNameL( _L("Name 2"));
    item2->SetPasswordL( _L("Password 2"));
    item2->SetParentFolder(4);
    item2->SetContextId(24);
 //   item2->SetHidden(0);
    item2->SetWapAp( accessPoint2 );   
     
    


    *item1 = *item2;
    
    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (
            (item1->Name() == item2->Name()) &&
            (item1->Url() == item2->Url()) &&
            (item1->UserName() == item2->UserName()) &&
            (item1->Password() == item2->Password()) &&
            (item1->ContextId() == item2->ContextId()) &&
            (item1->ParentFolder() == item2->ParentFolder()) &&
            (item1->Type() == item2->Type()) &&
            (item1->WapAp().ApId() == item2->WapAp().ApId()) &&
            (item1->IsItem() == item2->IsItem()) &&
            (item1->IsFolder() == item2->IsFolder()) &&
            (item1->IsHidden() == item2->IsHidden()) &&
  //          (item1->Uid() == item2->Uid()) &&               
            (item1->Modified() == item2->Modified()) 
           
        )    
        {
        _LIT( KDescription , "Test case passed");
        aResult.SetResult( KErrNone, KDescription );
        }
    else
        {
        _LIT( KDescription , "Test case failed");
        aResult.SetResult( KErrGeneral, KDescription );
        }

    CleanupStack::PopAndDestroy( 2 );   // item1, item2

    // Case was executed
    return KErrNone;
    }


/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemAssignSelfTestL

    Description: Assigning one item to self with the Assign(=) method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemAssignSelfTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Assigning item with Assign(=) operator");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item1 = CFavouritesItem::NewLC();
    item1->SetNameL( _L("Item1") );
  
    *item1 = *item1; //copy to self, should not affect anything
    
    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (item1->Name() == _L("Item1"))
        {
        _LIT( KDescription , "Test case passed");
        aResult.SetResult( KErrNone, KDescription );
        }
    else
        {
        _LIT( KDescription , "Test case failed");
        aResult.SetResult( KErrGeneral, KDescription );
        }

    CleanupStack::PopAndDestroy( item1 );   

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemUidTestL

    Description: Test getting the item's Uid using the Uid method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemUidTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Getting item's Uid with Uid method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    TInt itemUid = item->Uid();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (itemUid == 0)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemParentFolderTestL

    Description: Test getting the item's parent folder using the ParentFolder method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemParentFolderTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Getting item's parent folder with ParentFolder method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    TInt itemParent = item->ParentFolder();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (itemParent == 0)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemTypeTestL

    Description: Test getting the item's type using the Type method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemTypeTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Getting item's type with Type method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    TInt itemType = item->Type();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (itemType == CFavouritesItem::EItem)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemNameTestL

    Description: Test getting the item's name using the Name method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemNameTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Getting item's name with Name method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    TPtrC itemName = item->Name();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    _LIT( KItemName ,"");
    if (itemName == KItemName)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemUrlTestL

    Description: Test getting the item's url using the Url method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemUrlTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Getting item's url with Url method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    TPtrC itemUrl = item->Url();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    _LIT( KItemUrl ,"");
    if (itemUrl == KItemUrl)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemWapApTestL

    Description: Test getting the item's access point using the WapAp method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemWapApTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Getting item's access point with WapAp method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    TFavouritesWapAp itemAP = item->WapAp();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (itemAP.IsDefault())
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemUserNameTestL

    Description: Test getting the item's user name using the UserName method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemUserNameTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Getting item's user name with UserName method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    TPtrC itemUserName = item->UserName();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    _LIT( KItemUserName ,"");
    if (itemUserName == KItemUserName)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemPasswordTestL

    Description: Test getting the item's password using the Password method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemPasswordTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Getting item's password with Password method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    TPtrC itemPassword = item->Password();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    _LIT( KItemPassword ,"");
    if (itemPassword == KItemPassword)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemContextIdTestL

    Description: Test getting the item's context id using the ContextId method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemContextIdTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Getting item's context id with ContextId method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    TInt32 itemContextId = item->ContextId();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (itemContextId == 0)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemIsItemTestL

    Description: Test if the item is an item using the IsItem method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemIsItemTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Checking if the item is an item with IsItem method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    TBool itemIsItem = item->IsItem();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (itemIsItem)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }


/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemIsItemEFolderTestL

    Description: Test if the folder is not an item using the IsItem method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemIsItemEFolderTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Checking if the folder is not an item with IsItem method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    item->SetType(CFavouritesItem::EFolder);

    TBool itemIsItem = item->IsItem();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (!itemIsItem)
        {
        _LIT( KDescription , "Test case passed");
        aResult.SetResult( KErrNone, KDescription );
        }
    else
        {
        _LIT( KDescription , "Test case failed");
        aResult.SetResult( KErrGeneral, KDescription );
        }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemIsFolderTestL

    Description: Test if the item is a folder using the IsFolder method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemIsFolderTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Checking if the item is not a folder with IsFolder method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    TBool itemIsFolder = item->IsFolder();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (!itemIsFolder)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemIsFolderEFolderTestL

    Description: Test if the item of type EFolder is a folder using the IsFolder method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemIsFolderEFolderTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Checking if the folder is a folder with IsFolder method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    /* set item as folder type */
    item->SetType(CFavouritesItem::EFolder);
    TBool itemIsFolder = item->IsFolder();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (itemIsFolder)
        {
        _LIT( KDescription , "Test case passed");
        aResult.SetResult( KErrNone, KDescription );
        }
    else
        {
        _LIT( KDescription , "Test case failed");
        aResult.SetResult( KErrGeneral, KDescription );
        }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemIsFactoryItemTestL

    Description: Test if the item is a factory item using the IsFactoryItem method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemIsFactoryItemTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Checking if the item is a factory item with IsFactory method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    TBool itemIsFactoryItem = item->IsFactoryItem();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (!itemIsFactoryItem)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemIsReadOnlyTestL

    Description: Test if the item is read-only using the IsReadOnly method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemIsReadOnlyTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Checking if the item is read-only item with IsReadOnly method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    TBool itemIsReadOnly = item->IsReadOnly();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (!itemIsReadOnly)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemModifiedTestL

    Description: Test getting the last modified time using the Modified method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemModifiedTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Getting the item's last modified time with Modified method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    TTime itemModified = item->Modified();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (itemModified != NULL)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemClearLTestL

    Description: Test clearing the item using the ClearL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemClearLTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Clearing the item with ClearL method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    item->SetType(CFavouritesItem::EFolder);
    item->SetNameL( _L("Item Name") );
    item->ClearL();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if ((item->Type() == CFavouritesItem::EFolder) || (item->Name() == _L("Item Name")))
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetParentFolderTestL

    Description: Test setting the item's parent folder using the SetParentFolder method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetParentFolderTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's parent folder with SetParentFolder method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    item->SetParentFolder(3);

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (item->ParentFolder() == 3)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetTypeTestL

    Description: Test setting the item's type to EFolder using the SetType method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetTypeTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's type to Folder with SetType method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    item->SetType(CFavouritesItem::EFolder);
    
    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (item->Type() == CFavouritesItem::EFolder)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetTypeNoneTestL

    Description: Test setting the item's type to ENone using the SetType method. Type should not be affected.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetTypeNoneTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's type to None with SetType method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    item->SetType(CFavouritesItem::ENone);
    
    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );
 
    if (item->Type() != CFavouritesItem::ENone)
        {
        _LIT( KDescription , "Test case passed");
        aResult.SetResult( KErrNone, KDescription );
        }
    else
        {
        _LIT( KDescription , "Test case failed");
        aResult.SetResult( KErrGeneral, KDescription );
        }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetNameLTestL

    Description: Test setting the item's name using the SetNameL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetNameLTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's name with SetNameL method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    item->SetNameL( _L("Item Name") );

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (item->Name() == _L("Item Name"))
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }


/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetNameFolderLTestL

    Description: Test setting the item (folder)'s name using the SetNameL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetNameFolderLTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's name with SetNameL method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    item->SetType(CFavouritesItem::EFolder);
    item->SetNameL( _L("Fav Folder") );

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (item->Name() == _L("Fav Folder"))
        {
        _LIT( KDescription , "Test case passed");
        aResult.SetResult( KErrNone, KDescription );
        }
    else
        {
        _LIT( KDescription , "Test case failed");
        aResult.SetResult( KErrGeneral, KDescription );
        }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }


/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetNameMaxLengthTruncateLTestL

    Description: Test setting the item's name and maxlength using the SetNameL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetNameMaxLengthTruncateLTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's name with SetNameL method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    TRAPD(err, item->SetNameL( _L("Item Name longer than KFavouritesMaxName limit which is 50 chars")));

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if ((item->Name().Length() == KFavouritesMaxName)&& (err==KErrOverflow))
        {
        _LIT( KDescription , "Test case passed");
        aResult.SetResult( KErrNone, KDescription );
        }
    else
        {
        _LIT( KDescription , "Test case failed");
        aResult.SetResult( KErrGeneral, KDescription );
        }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }


/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetNameTrimStringLTestL

    Description: Test setting the item's name with trim using the SetNameL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetNameTrimStringLTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's name with SetNameL method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
     item->SetNameL( _L("  some   name    "));

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (item->Name()== _L("some   name"))
        {
        _LIT( KDescription , "Test case passed");
        aResult.SetResult( KErrNone, KDescription );
        }
    else
        {
        _LIT( KDescription , "Test case failed");
        aResult.SetResult( KErrGeneral, KDescription );
        }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetNameRTLMarkLTestL

    Description: Test setting the item's name with trim using the SetNameL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetNameRTLMarkLTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's name with SetNameL method");
   
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
     item->SetNameL( _L("\x200F  some   name   \x200F"));

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (item->Name()== _L("some   name"))
        {
        _LIT( KDescription , "Test case passed");
        aResult.SetResult( KErrNone, KDescription );
        }
    else
        {
        _LIT( KDescription , "Test case failed");
        aResult.SetResult( KErrGeneral, KDescription );
        }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }


/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetNameEmptyStringLTestL

    Description: Test setting the item's name with trim using the SetNameL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetNameEmptyStringLTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's name with SetNameL method");
    TestModuleIf().Printf( 0, KDefinition, KData );
 
    CFavouritesItem* item = CFavouritesItem::NewLC();
    TRAPD(err, item->SetNameL( _L("")));  //returns error

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (err!=KErrNone)
        {
        _LIT( KDescription , "Test case passed");
        aResult.SetResult( KErrNone, KDescription );
        }
    else
        {
        _LIT( KDescription , "Test case failed");
        aResult.SetResult( KErrGeneral, KDescription );
        }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetNameWhiteSpaceLTestL

    Description: Test setting the item's name with trim using the SetNameL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetNameWhiteSpaceLTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's name with SetNameL method");
    TestModuleIf().Printf( 0, KDefinition, KData );
 
    CFavouritesItem* item = CFavouritesItem::NewLC();
    TRAPD(err, item->SetNameL( _L("         ")));  //returns error

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (err!=KErrNone)
        {
        _LIT( KDescription , "Test case passed");
        aResult.SetResult( KErrNone, KDescription );
        }
    else
        {
        _LIT( KDescription , "Test case failed");
        aResult.SetResult( KErrGeneral, KDescription );
        }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }


/*

-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetUrlLTestL

    Description: Test setting the item's url using the SetUrlL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetUrlLTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's url with SetUrlL method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    item->SetUrlL( _L("http://www.nokia.com") );

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (item->Url() == _L("http://www.nokia.com"))
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetWapApTestL

    Description: Test setting the item's access point using the SetWapAp method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetWapApTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's access point with SetWapAp method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();

    TFavouritesWapAp accessPoint;
    accessPoint.SetApId( 22 );
    item->SetWapAp( accessPoint );

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (item->WapAp().ApId() == 22)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetUserNameLTestL

    Description: Test setting the item's user name using the SetUserNameL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetUserNameLTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's user name with SetUserNameL method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    item->SetUserNameL( _L("New User") );

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (item->UserName() == _L("New User"))
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }


/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetUserNameMaxLengthLTestL

    Description: Test setting the item's user name using the SetUserNameL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetUserNameMaxLengthLTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's user name with SetUserNameL method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
  //  TInt err;
    
    TRAPD(err, item->SetUserNameL( _L("New User name longer than allowed by the limit of KFavouritesMaxUserName which is 40 chars at this moment") ));

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (err==KErrOverflow)
        {
        _LIT( KDescription , "Test case passed");
        aResult.SetResult( KErrNone, KDescription );
        }
    else
        {
        _LIT( KDescription , "Test case failed");
        aResult.SetResult( KErrGeneral, KDescription );
        }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }


/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetUserNameTruncateLengthLTestL

    Description: Test setting the item's user name using the SetUserNameL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetUserNameTruncateLengthLTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's user name with SetUserNameL method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    TInt truncLength;
 
    TRAPD(err, item->SetUserNameL( _L("New User name longer than allowed by the limit of KFavouritesMaxUserName which is 40 chars at this moment") ));

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );
    truncLength = item->UserName().Length();
    
    if ((truncLength==KFavouritesMaxUserName)&& (err==KErrOverflow))
        {
        _LIT( KDescription , "Test case passed");
        aResult.SetResult( KErrNone, KDescription );
        }
    else
        {
        _LIT( KDescription , "Test case failed");
        aResult.SetResult( KErrGeneral, KDescription );
        }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }


/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetPasswordLTestL

    Description: Test setting the item's password using the SetPasswordL method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetPasswordLTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's password with SetPasswordL method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    item->SetPasswordL( _L("12345") );

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (item->Password() == _L("12345"))
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetContextIdTestL

    Description: Test setting the item's context id using the SetContextId method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetContextIdTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's context id with SetContextId method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    item->SetContextId(22);

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (item->ContextId() == 22)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemIsHiddenTestL

    Description: Test if the item is hidden using the IsHidden method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/

TInt CFavouritesEngineTest::ItemIsHiddenTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Checking if the item is hidden item with IsHidden method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    TBool itemIsHidden = item->IsHidden();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (!itemIsHidden)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: ItemSetHiddenTestL

    Description: Test setting the item's hidden value using the SetHidden method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::ItemSetHiddenTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Setting the item's hidden value with SetHidden method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CFavouritesItem* item = CFavouritesItem::NewLC();
    
    item->SetHidden(1);

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if (item->IsHidden() == 1)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed");
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy( item );

    // Case was executed
    return KErrNone;
    }

// ================= OTHER EXPORTED FUNCTIONS =================================

// End of File
