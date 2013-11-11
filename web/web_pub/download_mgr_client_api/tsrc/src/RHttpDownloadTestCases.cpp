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
#include "DownloadMgrClientApiTest.h"

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

    Method: DownloadStartTest

    Description: Test the RHttpDownload Start method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::DownloadStartTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the RHttpDownload Start method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TBuf8<128> url = _L8("http://waplabdc.nokia-boston.com/browser/users/s60_devtests/nightly_ss_on.xml");

    TBool isNewDl = ETrue;
    RHttpDownload& download = iDownloadManager.CreateDownloadL( url, isNewDl );
	
	// Added To avoid user Input while Automation in is Progress
	// 7 - SONERA GPRS Access Point
	TInt dlresult = iDownloadManager.SetIntAttribute( EDlMgrIap, 7 );
    
    TInt result = download.Start();

    _LIT( KData2 ,"Finished");
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

    iDownloadManager.DeleteAll();
    CleanupStack::PopAndDestroy(); // scheduler

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CDownloadMgrClientApiTest

    Method: DownloadPauseTest

    Description: Test the RHttpDownload Pause method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::DownloadPauseTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the RHttpDownload Pause method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TBuf8<128> url = _L8("http://waplabdc.nokia-boston.com/browser/users/s60_devtests/nightly_ss_on.xml");

    TBool isNewDl = ETrue;
    RHttpDownload& download = iDownloadManager.CreateDownloadL( url, isNewDl );
	
	// Added To avoid user Input while Automation in is Progress
	// 7 - SONERA GPRS Access Point
	TInt dlresult = iDownloadManager.SetIntAttribute( EDlMgrIap, 7 );
	
    download.Start();
    
    TInt result = download.Pause();

    _LIT( KData2 ,"Finished");
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

    iDownloadManager.DeleteAll();
    CleanupStack::PopAndDestroy(); // scheduler

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CDownloadMgrClientApiTest

    Method: DownloadResetTest

    Description: Test the RHttpDownload Reset method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::DownloadResetTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the RHttpDownload Reset method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TBuf8<128> url = _L8("http://waplabdc.nokia-boston.com/browser/users/s60_devtests/nightly_ss_on.xml");

    TBool isNewDl = ETrue;
    RHttpDownload& download = iDownloadManager.CreateDownloadL( url, isNewDl );

	// Added To avoid user Input while Automation in is Progress
	// 7 - SONERA GPRS Access Point
	TInt dlresult = iDownloadManager.SetIntAttribute( EDlMgrIap, 7 );
	
    download.Start();
    
    TInt result = download.Reset();

    _LIT( KData2 ,"Finished");
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

    iDownloadManager.DeleteAll();
    CleanupStack::PopAndDestroy(); // scheduler

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CDownloadMgrClientApiTest

    Method: DownloadDeleteTest

    Description: Test the RHttpDownload Delete method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::DownloadDeleteTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the RHttpDownload Delete method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TBuf8<128> url = _L8("http://waplabdc.nokia-boston.com/browser/users/s60_devtests/nightly_ss_on.xml");

    TBool isNewDl = ETrue;
    RHttpDownload& download = iDownloadManager.CreateDownloadL( url, isNewDl );

	// Added To avoid user Input while Automation in is Progress
	// 7 - SONERA GPRS Access Point
	TInt dlresult = iDownloadManager.SetIntAttribute( EDlMgrIap, 7 );
	
    download.Start();
    
    TInt result = download.Delete();

    _LIT( KData2 ,"Finished");
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

    iDownloadManager.DeleteAll();
    CleanupStack::PopAndDestroy(); // scheduler

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CDownloadMgrClientApiTest

    Method: DownloadMoveTest

    Description: Test the RHttpDownload Move method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::DownloadMoveTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the RHttpDownload Move method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TBuf8<128> url = _L8("http://waplabdc.nokia-boston.com/browser/users/s60_devtests/nightly_ss_on.xml");

    TBool isNewDl = ETrue;
    RHttpDownload& download = iDownloadManager.CreateDownloadL( url, isNewDl );

	// Added To avoid user Input while Automation in is Progress
	// 7 - SONERA GPRS Access Point
	TInt dlresult = iDownloadManager.SetIntAttribute( EDlMgrIap, 7 );
	
    download.Start();
    
    TInt result = download.Move();

    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );
    
    if( result == KErrNotReady )
	    {
	    _LIT( KDescription , "Test case passed" );
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed" );
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    iDownloadManager.DeleteAll();
    CleanupStack::PopAndDestroy(); // scheduler

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CDownloadMgrClientApiTest

    Method: DownloadGetIntAttributeTest

    Description: Test the RHttpDownload GetIntAttribute method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::DownloadGetIntAttributeTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the RHttpDownload GetIntAttribute method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TBuf8<128> url = _L8("http://waplabdc.nokia-boston.com/browser/users/s60_devtests/nightly_ss_on.xml");

    TBool isNewDl = ETrue;
    RHttpDownload& download = iDownloadManager.CreateDownloadL( url, isNewDl );

    TInt32 value;
    TInt result = download.GetIntAttribute( EDlAttrId, value );
    
    _LIT( KData2 ,"Finished");
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

    iDownloadManager.DeleteAll();
    CleanupStack::PopAndDestroy(); // scheduler

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CDownloadMgrClientApiTest

    Method: DownloadGetBoolAttributeTest

    Description: Test the RHttpDownload GetBoolAttribute method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::DownloadGetBoolAttributeTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the RHttpDownload GetBoolAttribute method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TBuf8<128> url = _L8("http://waplabdc.nokia-boston.com/browser/users/s60_devtests/nightly_ss_on.xml");

    TBool isNewDl = ETrue;
    RHttpDownload& download = iDownloadManager.CreateDownloadL( url, isNewDl );

    TBool value;
    TInt result = download.GetBoolAttribute( EDlAttrDestRemovable, value );
    
    _LIT( KData2 ,"Finished");
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

    iDownloadManager.DeleteAll();
    CleanupStack::PopAndDestroy(); // scheduler

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CDownloadMgrClientApiTest

    Method: DownloadGetStringAttribute16Test

    Description: Test the RHttpDownload GetStringAttribute method using TDes16 argument.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::DownloadGetStringAttribute16Test( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the RHttpDownload GetStringAttribute method using TDes16 argument");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TBuf8<128> url = _L8("http://waplabdc.nokia-boston.com/browser/users/s60_devtests/nightly_ss_on.xml");

    TBool isNewDl = ETrue;
    RHttpDownload& download = iDownloadManager.CreateDownloadL( url, isNewDl );

    HBufC16* stringAttr = HBufC16::NewLC( KMaxUrlLength );
    TPtr16 stringAttrPtr = stringAttr->Des();
    TInt result = download.GetStringAttribute( EDlAttrCurrentUrl, stringAttrPtr );
    
    _LIT( KData2 ,"Finished");
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

    iDownloadManager.DeleteAll();
    CleanupStack::PopAndDestroy( 2 ); // scheduler, stringAttr

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CDownloadMgrClientApiTest

    Method: DownloadGetStringAttribute8Test

    Description: Test the RHttpDownload GetStringAttribute method using TDes8 argument.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::DownloadGetStringAttribute8Test( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the RHttpDownload GetStringAttribute method using TDes8 argument");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TBuf8<128> url = _L8("http://waplabdc.nokia-boston.com/browser/users/s60_devtests/nightly_ss_on.xml");

    TBool isNewDl = ETrue;
    RHttpDownload& download = iDownloadManager.CreateDownloadL( url, isNewDl );

    HBufC8* stringAttr = HBufC8::NewLC( KMaxUrlLength );
    TPtr8 stringAttrPtr = stringAttr->Des();
    TInt result = download.GetStringAttribute( EDlAttrCurrentUrl, stringAttrPtr );
    
    _LIT( KData2 ,"Finished");
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

    iDownloadManager.DeleteAll();
    CleanupStack::PopAndDestroy( 2 ); // scheduler, stringAttr

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CDownloadMgrClientApiTest

    Method: DownloadGetFileHandleAttributeTest

    Description: Test the RHttpDownload GetFileHandleAttribute method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::DownloadGetFileHandleAttributeTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the RHttpDownload GetFileHandleAttribute method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TBuf8<128> url = _L8("http://waplabdc.nokia-boston.com/browser/users/s60_devtests/nightly_ss_on.xml");

    TBool isNewDl = ETrue;
    RHttpDownload& download = iDownloadManager.CreateDownloadL( url, isNewDl );

	// Added To avoid user Input while Automation in is Progress
	// 7 - SONERA GPRS Access Point
	TInt dlresult = iDownloadManager.SetIntAttribute( EDlMgrIap, 7 );
	
    download.Start();
    
    RFile file;
    TInt result = download.GetFileHandleAttribute( file );
    
    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );
    
    if( result == KErrArgument )
	    {
	    _LIT( KDescription , "Test case passed" );
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed" );
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    iDownloadManager.DeleteAll();
    CleanupStack::PopAndDestroy(); // scheduler

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CDownloadMgrClientApiTest

    Method: DownloadSetIntAttributeTest

    Description: Test the RHttpDownload SetIntAttribute method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::DownloadSetIntAttributeTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the RHttpDownload SetIntAttribute method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TBuf8<128> url = _L8("http://waplabdc.nokia-boston.com/browser/users/s60_devtests/nightly_ss_on.xml");

    TBool isNewDl = ETrue;
    RHttpDownload& download = iDownloadManager.CreateDownloadL( url, isNewDl );

    TInt result = download.SetIntAttribute( EDlAttrFotaPckgId, KDefaultFotaPckgId );
    
    _LIT( KData2 ,"Finished");
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

    iDownloadManager.DeleteAll();
    CleanupStack::PopAndDestroy(); // scheduler

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CDownloadMgrClientApiTest

    Method: DownloadSetBoolAttributeTest

    Description: Test the RHttpDownload SetBoolAttribute method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::DownloadSetBoolAttributeTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the RHttpDownload SetBoolAttribute method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TBuf8<128> url = _L8("http://waplabdc.nokia-boston.com/browser/users/s60_devtests/nightly_ss_on.xml");

    TBool isNewDl = ETrue;
    RHttpDownload& download = iDownloadManager.CreateDownloadL( url, isNewDl );

    TInt result = download.SetBoolAttribute( EDlAttrProgressive, EFalse );
    
    _LIT( KData2 ,"Finished");
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

    iDownloadManager.DeleteAll();
    CleanupStack::PopAndDestroy(); // scheduler

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CDownloadMgrClientApiTest

    Method: DownloadSetStringAttribute16Test

    Description: Test the RHttpDownload SetStringAttribute method using TDes16 argument.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::DownloadSetStringAttribute16Test( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the RHttpDownload SetStringAttribute method using TDes16 argument");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TBuf8<128> url = _L8("http://waplabdc.nokia-boston.com/browser/users/s60_devtests/nightly_ss_on.xml");

    TBool isNewDl = ETrue;
    RHttpDownload& download = iDownloadManager.CreateDownloadL( url, isNewDl );

    _LIT( KDestFileName, "c:\\fakefile\\");
    TInt result = download.SetStringAttribute( EDlAttrDestFilename, KDestFileName() );
    
    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );
    
    if( result == KErrArgument )
	    {
	    _LIT( KDescription , "Test case passed" );
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed" );
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    iDownloadManager.DeleteAll();
    CleanupStack::PopAndDestroy(); // scheduler

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CDownloadMgrClientApiTest

    Method: DownloadSetStringAttribute8Test

    Description: Test the RHttpDownload SetStringAttribute method using TDes8 argument.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::DownloadSetStringAttribute8Test( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the RHttpDownload SetStringAttribute method using TDes8 argument");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TBuf8<128> url = _L8("http://waplabdc.nokia-boston.com/browser/users/s60_devtests/nightly_ss_on.xml");

    TBool isNewDl = ETrue;
    RHttpDownload& download = iDownloadManager.CreateDownloadL( url, isNewDl );

    _LIT8( KUserName, "username");
    TInt result = download.SetStringAttribute( EDlAttrUsername, KUserName );
    
    _LIT( KData2 ,"Finished");
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

    iDownloadManager.DeleteAll();
    CleanupStack::PopAndDestroy(); // scheduler

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CDownloadMgrClientApiTest

    Method: DownloadSetFileHandleAttributeTest

    Description: Test the RHttpDownload SetFileHandleAttribute method.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDownloadMgrClientApiTest::DownloadSetFileHandleAttributeTest( TTestResult& aResult )
    {
    /* Simple server connect */
    _LIT( KDefinition ,"State");
    _LIT( KData ,"Test the RHttpDownload SetFileHandleAttribute method");
    TestModuleIf().Printf( 0, KDefinition, KData );

    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TBuf8<128> url = _L8("http://waplabdc.nokia-boston.com/browser/users/s60_devtests/nightly_ss_on.xml");

    TBool isNewDl = ETrue;
    RHttpDownload& download = iDownloadManager.CreateDownloadL( url, isNewDl );

	// Added To avoid user Input while Automation in is Progress
	// 7 - SONERA GPRS Access Point
	TInt dlresult = iDownloadManager.SetIntAttribute( EDlMgrIap, 7 );
	
    download.Start();
    
    RFile file;
    TInt result = download.SetFileHandleAttribute( file );
    
    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinition, KData2 );
    
    if( result == KErrBadHandle )
	    {
	    _LIT( KDescription , "Test case passed" );
	    aResult.SetResult( KErrNone, KDescription );
	    }
    else
	    {
	    _LIT( KDescription , "Test case failed" );
	    aResult.SetResult( KErrGeneral, KDescription );
	    }

    iDownloadManager.DeleteAll();
    CleanupStack::PopAndDestroy(); // scheduler

    // Case was executed
    return KErrNone;
    }

// ================= OTHER EXPORTED FUNCTIONS =================================

// End of File
