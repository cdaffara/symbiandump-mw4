/*
* ============================================================================
*  Name:      favouriteswapaptestcases.cpp
*  Part of:   FavouritesEngineTest class member functions   
*
*  Description:
*
*  Version:   0.5
*
*  Copyright (C) 2002 Nokia Corporation.
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


// ============================ MEMBER FUNCTIONS ===============================

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: WapApConstructorTest

    Description: Test the WapAp constructor method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::WapApConstructorTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the WapAp constructor method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    TFavouritesWapAp accessPoint;
    
    accessPoint = 222;
    
    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if(accessPoint.ApId() == 222)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
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

    Method: WapApAssignWithApTest

    Description: Test the WapAp assign(=) operater using a WapAp as the argument.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::WapApAssignWithApTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the WapAp assign(=) operater using a WapAp as the argument");
    TestModuleIf().Printf( 0, KDefinition, KData );

    TFavouritesWapAp accessPoint1, accessPoint2;
    
    accessPoint1 = 222;
    
    accessPoint2 = accessPoint1;
    
    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if(accessPoint2.ApId() == 222)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
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

    Method: WapApAssignSelfWithApTest

    Description: Test the WapAp assign(=) operater using a WapAp as the argument.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::WapApAssignSelfWithApTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the WapAp assign(=) operater using the same WapAp as the argument");
    TestModuleIf().Printf( 0, KDefinition, KData );

    TFavouritesWapAp accessPoint1, accessPoint2;
    
    accessPoint1 = 222;
    
    accessPoint1 = accessPoint1;
    
    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if(accessPoint1.ApId() == 222)
        {
        _LIT( KDescription , "Test case passed");
        aResult.SetResult( KErrNone, KDescription );
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

    Method: WapApAssignWithApIdTest

    Description: Test the WapAp assign(=) operater using an ApId as the argument.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::WapApAssignWithApIdTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the WapAp assign(=) operater using an ApId as the argument");
    TestModuleIf().Printf( 0, KDefinition, KData );

    TFavouritesWapAp accessPoint;
    
    accessPoint = 222;
    
    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if(accessPoint.ApId() == 222)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
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

    Method: WapApSetNullTest

    Description: Test the WapAp SetNull method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::WapApSetNullTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the WapAp SetNull method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    TFavouritesWapAp accessPoint;
    
    accessPoint.SetNull();
    
    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if(accessPoint.IsNull())
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
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

    Method: WapApSetDefaultTest

    Description: Test the WapAp SetDefault method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::WapApSetDefaultTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the WapAp SetDefault method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    TFavouritesWapAp accessPoint;
    
    accessPoint.SetDefault();
    
    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if(accessPoint.IsDefault())
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
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

    Method: WapApSetApIdTest

    Description: Test the WapAp SetApId method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::WapApSetApIdTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the WapAp SetApId method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    TFavouritesWapAp accessPoint;
    
    accessPoint.SetApId(222);
    
    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if(accessPoint.ApId() == 222)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
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

    Method: WapApIsNullTest

    Description: Test the WapAp IsNull method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::WapApIsNullTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the WapAp IsNull method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    TFavouritesWapAp accessPoint;
    
    accessPoint.SetNull();
    
    TBool isNull = accessPoint.IsNull();
    
    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if(isNull)
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
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

    Method: WapApIsDefaultTest

    Description: Test the WapAp IsDefault method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::WapApIsDefaultTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State" );
    _LIT( KData ,"Test the WapAp IsDefault method" );
    TestModuleIf().Printf( 0, KDefinition, KData );

    TFavouritesWapAp accessPoint;
    
    accessPoint.SetDefault();
    
    TBool isDefault = accessPoint.IsDefault();
    
    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if( isDefault )
	    {
	    _LIT( KDescription , "Test case passed");
	    aResult.SetResult( KErrNone, KDescription );
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

    Method: WapApApIdTest

    Description: Test the WapAp ApId method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::WapApApIdTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State" );
    _LIT( KData ,"Test the WapAp ApId method" );
    TestModuleIf().Printf( 0, KDefinition, KData );

    TFavouritesWapAp accessPoint;
    
    accessPoint.SetApId( 222 );
    
    TUint32 apId = accessPoint.ApId();
    
    TBool isDefault = accessPoint.IsDefault();
    
    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    if( apId == 222 )
	    {
	    _LIT( KDescription , "Test case passed" );
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed" );
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    // Case was executed
    return KErrNone;
    }

// ================= OTHER EXPORTED FUNCTIONS =================================

// End of File
