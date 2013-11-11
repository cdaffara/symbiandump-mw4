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
* Description:  DownloadMgrClientApiTest class member functions
*
*/



// INCLUDE FILES
#include <Stiftestinterface.h>
#include "DownloadMgrClientApiTest.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS

// MACROS

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

// ---------------------------------------------------------
// DeleteFileL
// ---------------------------------------------------------
//
void DeleteFileL( const TDesC& aFile, RFs& aFs )
    {
    TParse file;
    User::LeaveIfError( aFs.Parse( aFile, file ) );
    TUint att;
    TInt res = aFs.Att( file.FullName(), att );
    if ( res == KErrNone )
        {
        User::LeaveIfError( aFs.Delete( file.FullName() ) );
        }
    }

// ---------------------------------------------------------
// CopyFileL
// ---------------------------------------------------------
//
void CopyFileL( const TDesC& aSrc, const TDesC& aDst, RFs& aFs )
    {
    RFile src;
    RFile dst;
    TBuf8<256> buf;

    User::LeaveIfError( src.Open
        ( aFs, aSrc, EFileRead | EFileShareReadersOnly | EFileStream ) );
    CleanupClosePushL<RFile>( src );
    (void)aFs.MkDirAll( aDst );    
    (void)aFs.SetAtt( aDst, 0, KEntryAttReadOnly ); // Overwrite read-only.
    User::LeaveIfError( dst.Replace
        ( aFs, aDst, EFileWrite | EFileShareExclusive | EFileStream ) );
    CleanupClosePushL<RFile>( dst );
    FOREVER
        {
        User::LeaveIfError( src.Read( buf ) );
        if ( !buf.Length() )
            {
            break;
            }
        User::LeaveIfError( dst.Write( buf ) );
        }
    CleanupStack::PopAndDestroy( 2 );   // close dst, src
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDownloadMgrClientApiTest::CDownloadMgrClientApiTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDownloadMgrClientApiTest::CDownloadMgrClientApiTest()
    {

    }

// -----------------------------------------------------------------------------
// CDownloadMgrClientApiTest::ConstructL
// Symbian 2nd phase constructor can leave.
// Note: If OOM test case uses STIF Logger, then STIF Logger must be created
// with static buffer size parameter (aStaticBufferSize). Otherwise Logger 
// allocates memory from heap and therefore causes error situations with OOM 
// testing. For more information about STIF Logger construction, see STIF Users 
// Guide.
// -----------------------------------------------------------------------------
//
void CDownloadMgrClientApiTest::ConstructL()
    {
    iLog = CStifLogger::NewL( KDownloadMgrClientApiTestLogPath, 
                          KDownloadMgrClientApiTestLogFile);
                          
    User::LeaveIfError( iFileSystem.Connect() );

    iDownloadManager.ConnectL( TUid::Uid( KThisAppUid ), *this, EFalse );
    }

// -----------------------------------------------------------------------------
// CDownloadMgrClientApiTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDownloadMgrClientApiTest* CDownloadMgrClientApiTest::NewL()
    {
    CDownloadMgrClientApiTest* self = new (ELeave) CDownloadMgrClientApiTest;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CDownloadMgrClientApiTest::~CDownloadMgrClientApiTest()
    {
    iDownloadManager.Close();
    iFileSystem.Close();
    delete iLog;
    }

// -----------------------------------------------------------------------------
// CDownloadMgrClientApiTest::InitL
// InitL is used to initialize the Test Module.
// -----------------------------------------------------------------------------
//
TInt CDownloadMgrClientApiTest::InitL( 
    TFileName& /*aIniFile*/, 
    TBool /*aFirstTime*/ )
    {
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CDemoModule

    Method: Case

    Description: Returns a test case by number.

    This function contains an array of all available test cases 
    i.e pair of case name and test function. If case specified by parameter
    aCaseNumber is found from array, then that item is returned.

    The reason for this rather complicated function is to specify all the
    test cases only in one place. It is not necessary to understand how
    function pointers to class member functions works when adding new test
    cases. See function body for instructions how to add new test case.
    
    Parameters:    const TInt aCaseNumber :in:      Test case number

    Return Values: const TCaseInfo Struct containing case name & function

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/ 
const TCaseInfo CDownloadMgrClientApiTest::Case ( 
    const TInt aCaseNumber ) const 
     {

    /*
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    *
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    OOMHard.cpp file and to OOMHard.h 
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case 
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case 
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and 
    * TInt value for last heap memory allocation failure.
    * 
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation 
    * failure and TInt value for last heap memory allocation failure. 
    */ 

    static TCaseInfoInternal const KCases[] =
        {
        // To add new test cases, add new items to this array
        
        // NOTE: When compiled to GCCE, there must be Classname::
        // declaration in front of the method name, e.g. 
        // CDemoModule::PrintTest. Otherwise the compiler
        // gives errors.
                
        ENTRY( "Download Start test", CDownloadMgrClientApiTest::DownloadStartTest ),
        ENTRY( "Download Pause test", CDownloadMgrClientApiTest::DownloadPauseTest ),
        ENTRY( "Download Reset test", CDownloadMgrClientApiTest::DownloadResetTest ),
        ENTRY( "Download Delete test", CDownloadMgrClientApiTest::DownloadDeleteTest ),
        ENTRY( "Download Move test", CDownloadMgrClientApiTest::DownloadMoveTest ),
        ENTRY( "Download GetIntAttribute test", CDownloadMgrClientApiTest::DownloadGetIntAttributeTest ),
        ENTRY( "Download GetBoolAttribute test", CDownloadMgrClientApiTest::DownloadGetBoolAttributeTest ),
        ENTRY( "Download GetStringAttribute test with TDes16 argument", CDownloadMgrClientApiTest::DownloadGetStringAttribute16Test ),
        ENTRY( "Download GetStringAttribute test with TDes8 argument", CDownloadMgrClientApiTest::DownloadGetStringAttribute8Test ),
        ENTRY( "Download GetFileHandleAttribute test", CDownloadMgrClientApiTest::DownloadGetFileHandleAttributeTest ),
        ENTRY( "Download SetIntAttribute test", CDownloadMgrClientApiTest::DownloadSetIntAttributeTest ),
        ENTRY( "Download SetBoolAttribute test", CDownloadMgrClientApiTest::DownloadSetBoolAttributeTest ),
        ENTRY( "Download SetStringAttribute test with TDes16 argument", CDownloadMgrClientApiTest::DownloadSetStringAttribute16Test ),
        ENTRY( "Download SetStringAttribute test with TDes8 argument", CDownloadMgrClientApiTest::DownloadSetStringAttribute8Test ),
        ENTRY( "Download SetFileHandleAttribute test with TDes8 argument", CDownloadMgrClientApiTest::DownloadSetFileHandleAttributeTest ),
        ENTRY( "Manager ConnectL test", CDownloadMgrClientApiTest::ManagerConnectLTest ),
        ENTRY( "Manager Version test", CDownloadMgrClientApiTest::ManagerVersionTest ),
        ENTRY( "Manager Close test", CDownloadMgrClientApiTest::ManagerCloseTest ),
        ENTRY( "Manager CurrentDownloads test", CDownloadMgrClientApiTest::ManagerCurrentDownloadsTest ),
        ENTRY( "Manager CreateDownloadL test with Bool argument", CDownloadMgrClientApiTest::ManagerCreateDownloadLWithBoolTest ),
        ENTRY( "Manager CreateDownloadL test", CDownloadMgrClientApiTest::ManagerCreateDownloadLTest ),
        ENTRY( "Manager CreateCodDownloadL test", CDownloadMgrClientApiTest::ManagerCreateCodDownloadLTest ),
        ENTRY( "Manager FindDownload test", CDownloadMgrClientApiTest::ManagerFindDownloadTest ),
        ENTRY( "Manager PauseAll test", CDownloadMgrClientApiTest::ManagerPauseAllTest ),
        ENTRY( "Manager StartAll test", CDownloadMgrClientApiTest::ManagerStartAllTest ),
        ENTRY( "Manager ResetAll test", CDownloadMgrClientApiTest::ManagerResetAllTest ),
        ENTRY( "Manager DeleteAll test", CDownloadMgrClientApiTest::ManagerDeleteAllTest ),
        ENTRY( "Manager Disconnect test", CDownloadMgrClientApiTest::ManagerDisconnectTest ),
        ENTRY( "Manager GetIntAttribute test", CDownloadMgrClientApiTest::ManagerGetIntAttributeTest ),
        ENTRY( "Manager GetBoolAttribute test", CDownloadMgrClientApiTest::ManagerGetBoolAttributeTest ),
        ENTRY( "Manager GetStringAttribute test with TDesC16 argument", CDownloadMgrClientApiTest::ManagerGetStringAttribute16Test ),
        ENTRY( "Manager SetIntAttribute test", CDownloadMgrClientApiTest::ManagerSetIntAttributeTest ),
        ENTRY( "Manager SetBoolAttribute test", CDownloadMgrClientApiTest::ManagerSetBoolAttributeTest ),
        ENTRY( "Manager SetStringAttribute test with TDesC16 argument", CDownloadMgrClientApiTest::ManagerSetStringAttribute16Test ),
        ENTRY( "Manager SetDefaultIntAttribute test", CDownloadMgrClientApiTest::ManagerSetDefaultIntAttributeTest ),
        ENTRY( "Manager SetDefaultBoolAttribute test", CDownloadMgrClientApiTest::ManagerSetDefaultBoolAttributeTest ),
        ENTRY( "Manager SetDefaultStringAttribute test with TDesc16 argument", CDownloadMgrClientApiTest::ManagerSetDefaultStringAttribute16Test ),
        ENTRY( "Manager SetDefaultStringAttribute test with TDesc8 argument", CDownloadMgrClientApiTest::ManagerSetDefaultStringAttribute8Test ),
        ENTRY( "Manager AddObserverL test", CDownloadMgrClientApiTest::ManagerAddObserverLTest ),
        ENTRY( "Manager RemoveObserver test", CDownloadMgrClientApiTest::ManagerRemoveObserverTest ),
        ENTRY( "Manager SetNextUriObserver test", CDownloadMgrClientApiTest::ManagerSetNextUriObserverTest ),
        };

    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) / 
                               sizeof( TCaseInfoInternal ) )
        {

        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;

        } 

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;

    }

void CDownloadMgrClientApiTest::HandleDMgrEventL( RHttpDownload& /*aDownload*/, THttpDownloadEvent /*aEvent*/ )
    {

    }

void CDownloadMgrClientApiTest::NextUriL( RHttpDownload& /*aDownload*/, const TDesC8& /*aUri*/ )
	{

	}


// -----------------------------------------------------------------------------
// CDownloadMgrClientApiTest::GetTestCasesL
// GetTestCases is used to inquire test cases from the Test Module. Test
// cases are stored to array of test cases. The Test Framework will be 
// the owner of the data in the RPointerArray after GetTestCases return
// and it does the memory deallocation. 
// -----------------------------------------------------------------------------
//
TInt CDownloadMgrClientApiTest::GetTestCasesL( 
    const TFileName& /*aConfig*/, 
    RPointerArray<TTestCaseInfo>& aTestCases )
    {

    // Loop through all test cases and create new
    // TTestCaseInfo items and append items to aTestCase array    
    for( TInt i = 0; Case(i).iMethod != NULL; i++ )
        {
       // Allocate new TTestCaseInfo from heap for a testcase definition.
        TTestCaseInfo* newCase = new( ELeave ) TTestCaseInfo();
    
        // PushL TTestCaseInfo to CleanupStack.    
        CleanupStack::PushL( newCase );

        // Set number for the testcase.
        // When the testcase is run, this comes as a parameter to RunTestCaseL.
        newCase->iCaseNumber = i;

        // Set title for the test case. This is shown in UI to user.
        newCase->iTitle.Copy( Case(i).iCaseName );

        // Append TTestCaseInfo to the testcase array. After appended 
        // successfully the TTestCaseInfo object is owned (and freed) 
        // by the TestServer. 
        User::LeaveIfError(aTestCases.Append ( newCase ) );
        
        // Pop TTestCaseInfo from the CleanupStack.
        CleanupStack::Pop( newCase );
        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CDownloadMgrClientApiTest::RunTestCaseL
// RunTestCaseL is used to run an individual test case specified 
// by aTestCase. Test cases that can be run may be requested from 
// Test Module by GetTestCases method before calling RunTestCase.
// -----------------------------------------------------------------------------
//
TInt CDownloadMgrClientApiTest::RunTestCaseL(
    const TInt aCaseNumber,
    const TFileName& /*aConfig*/,
    TTestResult& aResult )
    {
    // Return value
    TInt execStatus = KErrNone;

    // Get the pointer to test case function
    TCaseInfo tmp = Case ( aCaseNumber );

    _LIT( KLogInfo, "Starting testcase [%S]" );
    iLog->Log( KLogInfo, &tmp.iCaseName);

    // Check that case number was valid
    if ( tmp.iMethod != NULL )
        {
        // Valid case was found, call it via function pointer
        iMethod = tmp.iMethod;        
        execStatus  = ( this->*iMethod )( aResult );
        }
    else
        {
        // Valid case was not found, return error.
        execStatus = KErrNotFound;
        }

    // Return case execution status (not the result of the case execution)
    return execStatus;

    }

// -----------------------------------------------------------------------------
// CDownloadMgrClientApiTest::OOMTestQueryL
// Used to check if a particular test case should be run in OOM conditions and 
// which memory allocations should fail.    
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.  
// -----------------------------------------------------------------------------
//
TBool CDownloadMgrClientApiTest::OOMTestQueryL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */, 
                                TOOMFailureType& /* aFailureType */, 
                                TInt& /* aFirstMemFailure */, 
                                TInt& /* aLastMemFailure */ ) 
    {
    _LIT( KOOMTestQueryL, "CDownloadMgrClientApiTest::OOMTestQueryL" );
    iLog->Log( KOOMTestQueryL ); 

    return EFalse;

    }

// -----------------------------------------------------------------------------
// CDownloadMgrClientApiTest::OOMTestInitializeL
// Used to perform the test environment setup for a particular OOM test case. 
// Test Modules may use the initialization file to read parameters for Test 
// Module initialization but they can also have their own configure file or 
// some other routine to initialize themselves.  
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.  
// -----------------------------------------------------------------------------
//
void CDownloadMgrClientApiTest::OOMTestInitializeL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */ )
    {
    }

// -----------------------------------------------------------------------------
// CDownloadMgrClientApiTest::OOMHandleWarningL
// In some cases the heap memory allocation should be skipped, either due to
// problems in the OS code or components used by the code being tested, or even 
// inside the tested components which are implemented this way on purpose (by 
// design), so it is important to give the tester a way to bypass allocation 
// failures.
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.  
// -----------------------------------------------------------------------------
//
void CDownloadMgrClientApiTest::OOMHandleWarningL( 
                                const TFileName& /* aTestCaseFile */,
                                const TInt /* aCaseNumber */, 
                                TInt& /* aFailNextValue */ )
    {
    }

// -----------------------------------------------------------------------------
// CDownloadMgrClientApiTest::OOMTestFinalizeL
// Used to perform the test environment cleanup for a particular OOM test case.
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.  
// -----------------------------------------------------------------------------
//                  
void CDownloadMgrClientApiTest::OOMTestFinalizeL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */ )
    {
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point
// Returns: CTestModuleBase*: Pointer to Test Module object
// -----------------------------------------------------------------------------
//
EXPORT_C CTestModuleBase* LibEntryL()
    {
    return CDownloadMgrClientApiTest::NewL();

    }

// -----------------------------------------------------------------------------
// SetRequirements handles test module parameters(implements evolution
// version 1 for test module's heap and stack sizes configuring).
// Returns: TInt: Symbian error code.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SetRequirements( CTestModuleParam*& /*aTestModuleParam*/, 
                                TUint32& /*aParameterValid*/ )
    {

    /* --------------------------------- NOTE ---------------------------------
    USER PANICS occurs in test thread creation when:
    1) "The panic occurs when the value of the stack size is negative."
    2) "The panic occurs if the minimum heap size specified is less
       than KMinHeapSize".
       KMinHeapSize: "Functions that require a new heap to be allocated will
       either panic, or will reset the required heap size to this value if
       a smaller heap size is specified".
    3) "The panic occurs if the minimum heap size specified is greater than
       the maximum size to which the heap can grow".
    Other:
    1) Make sure that your hardware or Symbian OS is supporting given sizes.
       e.g. Hardware might support only sizes that are divisible by four.
    ------------------------------- NOTE end ------------------------------- */

    // Normally STIF uses default heap and stack sizes for test thread, see:
    // KTestThreadMinHeap, KTestThreadMinHeap and KStackSize.
    // If needed heap and stack sizes can be configured here by user. Remove
    // comments and define sizes.

/*
    aParameterValid = KStifTestModuleParameterChanged;

    CTestModuleParamVer01* param = CTestModuleParamVer01::NewL();
    // Stack size
    param->iTestThreadStackSize= 16384; // 16K stack
    // Heap sizes
    param->iTestThreadMinHeap = 4096;   // 4K heap min
    param->iTestThreadMaxHeap = 1048576;// 1M heap max

    aTestModuleParam = param;
*/
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function
// Returns: KErrNone: No error
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll(
    TDllReason /*aReason*/) // Reason
    {
    return(KErrNone);

    }
#endif // EKA2

//  End of File
