/*
* ==============================================================================
*  Name        : favouritesenginetest.cpp
*  Part of     : ?Subsystem_name / FavouritesEngineTest
*
*  Description : FavouritesBCTest class member functions
*  Version:   0.5
*
*  Copyright (C) 2006 Nokia Corporation.
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
#include <Stiftestinterface.h>
#include "favouritesenginetest.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

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
// CFavouritesEngineTest::CFavouritesEngineTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFavouritesEngineTest::CFavouritesEngineTest()
    {

    }

// -----------------------------------------------------------------------------
// CFavouritesEngineTest::ConstructL
// Symbian 2nd phase constructor can leave.
// Note: If OOM test case uses STIF Logger, then STIF Logger must be created
// with static buffer size parameter (aStaticBufferSize). Otherwise Logger 
// allocates memory from heap and therefore causes error situations with OOM 
// testing. For more information about STIF Logger construction, see STIF Users 
// Guide.
// -----------------------------------------------------------------------------
//
void CFavouritesEngineTest::ConstructL()
    {
    iLog = CStifLogger::NewL( KFavouritesEngineTestLogPath, 
                          KFavouritesEngineTestLogFile);
                          
    User::LeaveIfError( iFileSystem.Connect() );

    User::LeaveIfError( iFavouritesSession.Connect() );
    }

// -----------------------------------------------------------------------------
// CFavouritesEngineTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFavouritesEngineTest* CFavouritesEngineTest::NewL()
    {
    CFavouritesEngineTest* self = new (ELeave) CFavouritesEngineTest;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CFavouritesEngineTest::~CFavouritesEngineTest()
    {
    iFavouritesDb.Close();
    iFavouritesSession.Close();
    iFileSystem.Close();
    delete iLog;
    }

// ----------------------------------------------------------
// CFavouritesDbTester::MapAccessPoint()
// ----------------------------------------------------------
//
TBool CFavouritesEngineTest::MapAccessPoint
( TUint aOldUid, TUint& aNewUid )
    {
    aNewUid = aOldUid;
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CFavouritesEngineTest::InitL
// InitL is used to initialize the Test Module.
// -----------------------------------------------------------------------------
//
TInt CFavouritesEngineTest::InitL( 
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
const TCaseInfo CFavouritesEngineTest::Case ( 
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
                
        ENTRY( "Item NewLC test", CFavouritesEngineTest::ItemNewLCTestL ),
        ENTRY( "Item NewL test", CFavouritesEngineTest::ItemNewLTestL ),
        ENTRY( "Item Destructor test", CFavouritesEngineTest::ItemDestructorTestL ),
        ENTRY( "Item Assign(=) operator basic test", CFavouritesEngineTest::ItemAssignTestL ),
        ENTRY( "Item Assign(=) to Self test", CFavouritesEngineTest::ItemAssignSelfTestL ),        
        ENTRY( "Item Assign(=) Complete test", CFavouritesEngineTest::ItemAssignCompleteTestL ),        
        ENTRY( "Item Uid test", CFavouritesEngineTest::ItemUidTestL ),
        ENTRY( "Item ParentFolder test", CFavouritesEngineTest::ItemParentFolderTestL ),
        ENTRY( "Item Type test", CFavouritesEngineTest::ItemTypeTestL ),
        ENTRY( "Item Name test", CFavouritesEngineTest::ItemNameTestL ),
        ENTRY( "Item Url test", CFavouritesEngineTest::ItemUrlTestL ),
        ENTRY( "Item WapAp test", CFavouritesEngineTest::ItemWapApTestL ),
        ENTRY( "Item UserName test", CFavouritesEngineTest::ItemUserNameTestL ),
        ENTRY( "Item Password test", CFavouritesEngineTest::ItemPasswordTestL ),
        ENTRY( "Item ContextId test", CFavouritesEngineTest::ItemContextIdTestL ),
        ENTRY( "Item IsItem Default test", CFavouritesEngineTest::ItemIsItemTestL ),
        ENTRY( "Item IsItem Folder test", CFavouritesEngineTest::ItemIsItemEFolderTestL ),        
        ENTRY( "Item IsFolder Default test", CFavouritesEngineTest::ItemIsFolderTestL ),
        ENTRY( "Item IsFolder Folder test", CFavouritesEngineTest::ItemIsFolderEFolderTestL ),      
        ENTRY( "Item IsFactoryItem test", CFavouritesEngineTest::ItemIsFactoryItemTestL ),
        ENTRY( "Item IsReadOnly test", CFavouritesEngineTest::ItemIsReadOnlyTestL ),
        ENTRY( "Item Modified test", CFavouritesEngineTest::ItemModifiedTestL ),
        ENTRY( "Item ClearL test", CFavouritesEngineTest::ItemClearLTestL ),
        ENTRY( "Item SetParentFolder test", CFavouritesEngineTest::ItemSetParentFolderTestL ),
        ENTRY( "Item SetType Default test", CFavouritesEngineTest::ItemSetTypeTestL ),
        ENTRY( "Item SetType None test", CFavouritesEngineTest::ItemSetTypeNoneTestL ),       
        ENTRY( "Item SetName Default test", CFavouritesEngineTest::ItemSetNameLTestL ),
        ENTRY( "Item SetName Truncate test", CFavouritesEngineTest::ItemSetNameMaxLengthTruncateLTestL ),
        ENTRY( "Item SetName TrimString test", CFavouritesEngineTest::ItemSetNameTrimStringLTestL ),
        ENTRY( "Item SetName RTLMark test", CFavouritesEngineTest::ItemSetNameRTLMarkLTestL ),
        ENTRY( "Item SetName EmptyString test", CFavouritesEngineTest::ItemSetNameEmptyStringLTestL ),
        ENTRY( "Item SetName WhiteSpace test", CFavouritesEngineTest::ItemSetNameWhiteSpaceLTestL ),        
        ENTRY( "Item SetName ForFolder test", CFavouritesEngineTest::ItemSetNameFolderLTestL ),
        ENTRY( "Item SetUrl test", CFavouritesEngineTest::ItemSetUrlLTestL ),
        ENTRY( "Item SetWapAp test", CFavouritesEngineTest::ItemSetWapApTestL ),
        ENTRY( "Item SetUserName test", CFavouritesEngineTest::ItemSetUserNameLTestL ),
        ENTRY( "Item SetUserName MaxLengthErr test", CFavouritesEngineTest::ItemSetUserNameMaxLengthLTestL ),        
        ENTRY( "Item SetUserName Truncate test", CFavouritesEngineTest::ItemSetUserNameTruncateLengthLTestL ),        
        ENTRY( "Item SetPassword test", CFavouritesEngineTest::ItemSetPasswordLTestL ),
        ENTRY( "Item SetContextId test", CFavouritesEngineTest::ItemSetContextIdTestL ),
        ENTRY( "List Constructor test", CFavouritesEngineTest::ListConstructorTestL ),
        ENTRY( "List Destructor test", CFavouritesEngineTest::ListDestructorTestL ),
        ENTRY( "List Delete test", CFavouritesEngineTest::ListDeleteTestL ),
        ENTRY( "List Delete Range test", CFavouritesEngineTest::ListDeleteRangeTestL ),
        ENTRY( "List Sort test", CFavouritesEngineTest::ListSortTestL ),
        ENTRY( "List UidToIndex test", CFavouritesEngineTest::ListUidToIndexTestL ),
        ENTRY( "List IndexToUid test", CFavouritesEngineTest::ListIndexToUidTestL ),
        ENTRY( "List ItemByUid test", CFavouritesEngineTest::ListItemByUidTestL ),
        ENTRY( "Db Open test", CFavouritesEngineTest::DbOpenTestL ),
        ENTRY( "Db Version test", CFavouritesEngineTest::DbVersionTestL ),
        ENTRY( "Db IsDamaged test", CFavouritesEngineTest::DbIsDamagedTestL ),
        ENTRY( "Db Recover test", CFavouritesEngineTest::DbRecoverTestL ),
        ENTRY( "Db Compact test", CFavouritesEngineTest::DbCompactTestL ),
        ENTRY( "Db Size test", CFavouritesEngineTest::DbSizeTestL ),
        ENTRY( "Db UpdateStats test", CFavouritesEngineTest::DbUpdateStatsTestL ),
        ENTRY( "Db Begin test", CFavouritesEngineTest::DbBeginTestL ),
        ENTRY( "Db Commit test", CFavouritesEngineTest::DbCommitTestL ),
        ENTRY( "Db Rollback test", CFavouritesEngineTest::DbRollbackTestL ),
        ENTRY( "Db CleanupRollbackPushL test", CFavouritesEngineTest::DbCleanupRollbackPushLTestL ),
        ENTRY( "Db Get test", CFavouritesEngineTest::DbGetTestL ),
        ENTRY( "Db GetAll test", CFavouritesEngineTest::DbGetAllTestL ),
        ENTRY( "Db GetUids test", CFavouritesEngineTest::DbGetUidsTestL ),
        ENTRY( "Db PreferredUid test", CFavouritesEngineTest::DbPreferredUidTestL ),
        ENTRY( "Db Delete test", CFavouritesEngineTest::DbDeleteTestL ),
        ENTRY( "Db Update test", CFavouritesEngineTest::DbUpdateTestL ),
        ENTRY( "Db Add test", CFavouritesEngineTest::DbAddTestL ),
        ENTRY( "Db SetHomepage test", CFavouritesEngineTest::DbSetHomepageTestL ),
        ENTRY( "Db SetLastVisited test", CFavouritesEngineTest::DbSetLastVisitedTestL ),
        ENTRY( "Db SetFactoryItem test", CFavouritesEngineTest::DbSetFactoryItemTestL ),
        ENTRY( "Db SetReadOnly test", CFavouritesEngineTest::DbSetReadOnlyTestL ),
        ENTRY( "Db SetModified test", CFavouritesEngineTest::DbSetModifiedTestL ),
        ENTRY( "Db SetPreferredUid test", CFavouritesEngineTest::DbSetPreferredUidTestL ),
        ENTRY( "Db ItemExists test", CFavouritesEngineTest::DbItemExistsTestL ),
        ENTRY( "Db FolderExists test", CFavouritesEngineTest::DbFolderExistsTestL ),
        ENTRY( "Db Count test", CFavouritesEngineTest::DbCountTestL ),
        ENTRY( "Db SetData test", CFavouritesEngineTest::DbSetDataTestL ),
        ENTRY( "Db GetData test", CFavouritesEngineTest::DbGetDataTestL ),
        ENTRY( "Db SetBrowserData test", CFavouritesEngineTest::DbSetBrowserDataTestL ),
        ENTRY( "Db GetBrowserData test", CFavouritesEngineTest::DbGetBrowserDataTestL ),
        ENTRY( "Db MakeUniqueName with folder test", CFavouritesEngineTest::DbMakeUniqueNameWithFolderTestL ),
        ENTRY( "Db MakeUniqueName test", CFavouritesEngineTest::DbMakeUniqueNameTestL ),
        ENTRY( "Db CreateStartPageItemL test", CFavouritesEngineTest::DbCreateStartPageItemLTestL ),
        ENTRY( "Db CreateAdaptiveItemsFolderL test", CFavouritesEngineTest::DbCreateAdaptiveItemsFolderLTestL ),
        ENTRY( "Db DeleteFile test", CFavouritesEngineTest::DbDeleteFileTestL ),
//        ENTRY( "Db RestoreFactorySettingsL test", CFavouritesEngineTest::DbRestoreFactorySettingsLTestL ),
        ENTRY( "WapAp Constructor test", CFavouritesEngineTest::WapApConstructorTest ),
        ENTRY( "WapAp Assign with Ap test", CFavouritesEngineTest::WapApAssignWithApTest ),
        ENTRY( "WapAp Assign with ApId test", CFavouritesEngineTest::WapApAssignWithApIdTest ),
        ENTRY( "WapAp Self-Assign with ApId test", CFavouritesEngineTest::WapApAssignSelfWithApTest ),        
        ENTRY( "WapAp SetNull test", CFavouritesEngineTest::WapApSetNullTest ),
        ENTRY( "WapAp SetDefault test", CFavouritesEngineTest::WapApSetDefaultTest ),
        ENTRY( "WapAp SetApId test", CFavouritesEngineTest::WapApSetApIdTest ),
        ENTRY( "WapAp IsNull test", CFavouritesEngineTest::WapApIsNullTest ),
        ENTRY( "WapAp IsDefault test", CFavouritesEngineTest::WapApIsDefaultTest ),
        ENTRY( "WapAp ApId test", CFavouritesEngineTest::WapApApIdTest ),
        ENTRY( "DbInc Recover test", CFavouritesEngineTest::DbIncRecoverTestL ),
        ENTRY( "DbInc Compact test", CFavouritesEngineTest::DbIncCompactTestL ),
        ENTRY( "DbInc Next test", CFavouritesEngineTest::DbIncNextTestL ),
        ENTRY( "File Close test", CFavouritesEngineTest::FileCloseTestL ),
        ENTRY( "File Open test", CFavouritesEngineTest::FileOpenTestL ),
        ENTRY( "File Replace test", CFavouritesEngineTest::FileReplaceTestL ),
        ENTRY( "File Read test", CFavouritesEngineTest::FileReadTestL ),
        ENTRY( "File Write test", CFavouritesEngineTest::FileWriteTestL ),
        ENTRY( "File Size test", CFavouritesEngineTest::FileSizeTestL ),
        ENTRY( "Session Version test", CFavouritesEngineTest::SessionVersionTest ),
        ENTRY( "Session Connect test", CFavouritesEngineTest::SessionConnectTest ),
        ENTRY( "Session ResourceMark test", CFavouritesEngineTest::SessionResourceMarkTest ),
        ENTRY( "Session ResourceCheck test", CFavouritesEngineTest::SessionResourceCheckTest ),
        ENTRY( "Session ResourceCount test", CFavouritesEngineTest::SessionResourceCountTest ),
        ENTRY( "Session DeleteDatabase test", CFavouritesEngineTest::SessionDeleteDatabaseTestL ),
        ENTRY( "Session __DbgSetAllocFail test", CFavouritesEngineTest::SessionDbgSetAllocFailTest ),
        ENTRY( "Notifier constructor test", CFavouritesEngineTest::NotifierConstructorTestL ),
        ENTRY( "Notifier destructor test", CFavouritesEngineTest::NotifierDestructorTestL ),
        ENTRY( "Notifier Start test", CFavouritesEngineTest::NotifierStartTestL ),
        ENTRY( "Db DeleteFolder test", CFavouritesEngineTest::DbDeleteFolderTestL ),
        ENTRY( "Item IsHidden test", CFavouritesEngineTest::ItemIsHiddenTestL ),
        ENTRY( "Item SetHidden test", CFavouritesEngineTest::ItemSetHiddenTestL )
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

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: CreatePopulatedDb

    Description: Populate a test db with items.
  
    Parameters:  None   

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CFavouritesEngineTest::CreatePopulatedDbL()
    {
    DestroyDbItems();

    // Create reference items in inst vars.
    CreateFolderL( iItemFolder1 );
    CreateFolderL( iItemFolder2 );
    CreateItemL( iItem1 );
    CreateItemL( iItem2 );
    CreateItemL( iItem3 );
    CreateItemL( iItem4 );
    CreateItemL( iItem5 );
    CreateItemL( iItem6 );

    // Add items to database.
    iFavouritesDb.Add( *iItemFolder1, EFalse );
    iFavouritesDb.Add( *iItemFolder2, EFalse );

    iItem3->SetParentFolder( iItemFolder1->Uid() );
    iItem4->SetParentFolder( iItemFolder1->Uid() );
    iItem5->SetParentFolder( iItemFolder2->Uid() );
    iItem6->SetParentFolder( iItemFolder2->Uid() );

    // Insert out of order for sort test
    iFavouritesDb.Add( *iItem5, EFalse );
    iFavouritesDb.Add( *iItem1, EFalse );
    iFavouritesDb.Add( *iItem6, EFalse );
    iFavouritesDb.Add( *iItem3, EFalse );
    iFavouritesDb.Add( *iItem2, EFalse );
    iFavouritesDb.Add( *iItem4, EFalse );

    }

/*
-------------------------------------------------------------------------------

    Class: CFavouritesEngineTest

    Method: DestroyDb

    Description: Delete the test database and all items in it.
  
    Parameters:  None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CFavouritesEngineTest::DestroyDbItems()
    {
    delete iItemFolder1;
    delete iItemFolder2;
    delete iItem1;
    delete iItem2;
    delete iItem3;
    delete iItem4;
    delete iItem5;
    delete iItem6;

    iItemFolder1 = NULL;
    iItemFolder2 = NULL;
    iItem1 = NULL;
    iItem2 = NULL;
    iItem3 = NULL;
    iItem4 = NULL;
    iItem5 = NULL;
    iItem6 = NULL;
    }

// -----------------------------------------------------------------------------
// CFavouritesEngineTest::GetTestCasesL
// GetTestCases is used to inquire test cases from the Test Module. Test
// cases are stored to array of test cases. The Test Framework will be 
// the owner of the data in the RPointerArray after GetTestCases return
// and it does the memory deallocation. 
// -----------------------------------------------------------------------------
//
TInt CFavouritesEngineTest::GetTestCasesL( 
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
// CFavouritesEngineTest::RunTestCaseL
// RunTestCaseL is used to run an individual test case specified 
// by aTestCase. Test cases that can be run may be requested from 
// Test Module by GetTestCases method before calling RunTestCase.
// -----------------------------------------------------------------------------
//
TInt CFavouritesEngineTest::RunTestCaseL(
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
// CFavouritesEngineTest::OOMTestQueryL
// Used to check if a particular test case should be run in OOM conditions and 
// which memory allocations should fail.    
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.  
// -----------------------------------------------------------------------------
//
TBool CFavouritesEngineTest::OOMTestQueryL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */, 
                                TOOMFailureType& /* aFailureType */, 
                                TInt& /* aFirstMemFailure */, 
                                TInt& /* aLastMemFailure */ ) 
    {
    _LIT( KOOMTestQueryL, "CFavouritesEngineTest::OOMTestQueryL" );
    iLog->Log( KOOMTestQueryL ); 

    return EFalse;

    }

// -----------------------------------------------------------------------------
// CFavouritesEngineTest::OOMTestInitializeL
// Used to perform the test environment setup for a particular OOM test case. 
// Test Modules may use the initialization file to read parameters for Test 
// Module initialization but they can also have their own configure file or 
// some other routine to initialize themselves.  
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.  
// -----------------------------------------------------------------------------
//
void CFavouritesEngineTest::OOMTestInitializeL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */ )
    {
    }

// -----------------------------------------------------------------------------
// CFavouritesEngineTest::OOMHandleWarningL
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
void CFavouritesEngineTest::OOMHandleWarningL( 
                                const TFileName& /* aTestCaseFile */,
                                const TInt /* aCaseNumber */, 
                                TInt& /* aFailNextValue */ )
    {
    }

// -----------------------------------------------------------------------------
// CFavouritesEngineTest::OOMTestFinalizeL
// Used to perform the test environment cleanup for a particular OOM test case.
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.  
// -----------------------------------------------------------------------------
//                  
void CFavouritesEngineTest::OOMTestFinalizeL( 
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
    return CFavouritesEngineTest::NewL();

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
