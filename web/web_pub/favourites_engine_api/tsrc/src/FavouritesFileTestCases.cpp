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

    Method: FileCloseTestL

    Description: Test the RFavouritesFile Close method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::FileCloseTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State" );
    _LIT( KData ,"Test the RFavouritesFile Close method" );
    TestModuleIf().Printf( 0, KDefinition, KData );

    iFavouritesDb.Open( iFavouritesSession, KTestDbName );
    CleanupClosePushL<RFavouritesDb>( iFavouritesDb );
    
    CreatePopulatedDbL();

    RFavouritesFile file;
	file.Open( iFavouritesDb, iItem1->Uid() );
	
	file.Close();  // Close returns void, so we have to assume success

    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );

    _LIT( KDescription , "Test case passed" );
    aResult.SetResult( KErrNone, KDescription );

    CleanupStack::PopAndDestroy(); // iFavouritesDb
    iFavouritesSession.DeleteDatabase( KTestDbName );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: FileOpenTestL

    Description: Test the RFavouritesFile Open method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::FileOpenTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State" );
    _LIT( KData ,"Test the RFavouritesFile Open method" );
    TestModuleIf().Printf( 0, KDefinition, KData );

    iFavouritesDb.Open( iFavouritesSession, KTestDbName );
    CleanupClosePushL<RFavouritesDb>( iFavouritesDb );
    
    CreatePopulatedDbL();

    RFavouritesFile file;
	TInt result = file.Open( iFavouritesDb, iItem1->Uid() );

    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );
	
	file.Close();

    if( result == KErrNotFound )
	    {
	    _LIT( KDescription , "Test case passed" );
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed" );
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    CleanupStack::PopAndDestroy(); // iFavouritesDb
    iFavouritesSession.DeleteDatabase( KTestDbName );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: FileReplaceTestL

    Description: Test the RFavouritesFile Replace method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::FileReplaceTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State" );
    _LIT( KData ,"Test the RFavouritesFile Replace method" );
    TestModuleIf().Printf( 0, KDefinition, KData );

    iFavouritesDb.Open( iFavouritesSession, KTestDbName );
    CleanupClosePushL<RFavouritesDb>( iFavouritesDb );
    
    CreatePopulatedDbL();

    RFavouritesFile file;
	TInt result = file.Replace( iFavouritesDb, KFavouritesRootUid );

    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );
	
	file.Close();

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

    CleanupStack::PopAndDestroy(); // iFavouritesDb
    iFavouritesSession.DeleteDatabase( KTestDbName );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: FileReadTestL

    Description: Test the RFavouritesFile Read method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::FileReadTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State" );
    _LIT( KData ,"Test the RFavouritesFile Read method" );
    TestModuleIf().Printf( 0, KDefinition, KData );

    iFavouritesDb.Open( iFavouritesSession, KTestDbName );
    CleanupClosePushL<RFavouritesDb>( iFavouritesDb );
    
    CreatePopulatedDbL();

    RFavouritesFile fileToWrite;
    TPtrC8 testString = _L8("Once upon a time");
        
    fileToWrite.Replace( iFavouritesDb, iItem1->Uid() );
        
    fileToWrite.Write(testString);
    
    fileToWrite.Close();

    RFavouritesFile fileToRead;
	HBufC8 *readBuf = HBufC8::NewLC(testString.Length());
	TPtr8 readPtr(readBuf->Des());
	
    fileToRead.Open( iFavouritesDb, iItem1->Uid() );

	TInt result = fileToRead.Read( readPtr );

    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );
	
	fileToRead.Close();

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

    CleanupStack::PopAndDestroy(2); // iFavouritesDb, readBuf
    iFavouritesSession.DeleteDatabase( KTestDbName );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: FileWriteTestL

    Description: Test the RFavouritesFile Write method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::FileWriteTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State" );
    _LIT( KData ,"Test the RFavouritesFile Write method" );
    TestModuleIf().Printf( 0, KDefinition, KData );

    iFavouritesDb.Open( iFavouritesSession, KTestDbName );
    CleanupClosePushL<RFavouritesDb>( iFavouritesDb );
    
    CreatePopulatedDbL();

    RFavouritesFile fileToWrite;
    TPtrC8 testString = _L8("Once upon a time");
        
    fileToWrite.Replace( iFavouritesDb, iItem1->Uid() );
        
    TInt result = fileToWrite.Write(testString);
    
    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );
	
    fileToWrite.Close();

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

    CleanupStack::PopAndDestroy(); // iFavouritesDb
    iFavouritesSession.DeleteDatabase( KTestDbName );

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: FileSizeTestL

    Description: Test the RFavouritesFile Size method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFavouritesEngineTest::FileSizeTestL( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State" );
    _LIT( KData ,"Test the RFavouritesFile Size method" );
    TestModuleIf().Printf( 0, KDefinition, KData );

    iFavouritesDb.Open( iFavouritesSession, KTestDbName );
    CleanupClosePushL<RFavouritesDb>( iFavouritesDb );
    
    CreatePopulatedDbL();

    RFavouritesFile file;

    TPtrC8 testString = _L8("Once upon a time");
        
    file.Replace( iFavouritesDb, iItem1->Uid() );
        
    file.Write(testString);

    TInt dbSize;
    
    TInt result = file.Size( dbSize );

    _LIT( KData2 ,"Finished" );
    TestModuleIf().Printf( 0, KDefinition, KData2 );
	
	file.Close();
	
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

    CleanupStack::PopAndDestroy(); // iFavouritesDb
    iFavouritesSession.DeleteDatabase( KTestDbName );

    // Case was executed
    return KErrNone;
    }

// ================= OTHER EXPORTED FUNCTIONS =================================

// End of File
