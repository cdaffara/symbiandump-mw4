/*
* ==============================================================================
*  Name        : favouritesenginetest.h
*  Part of     : ?Subsystem_name / FavouritesEngineTest
*
*  Description : FavouritesEngineTest test module.
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


#ifndef FAVOURITESENGINETEST_H
#define FAVOURITESENGINETEST_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>
#include <f32file.h>
#include <favouritessession.h>
#include <favouritesitem.h>
#include <favouritesitemlist.h>
#include <favouritesdb.h>
#include <favouritesfile.h>
//#include <RfsApMapper.h>
#include <favouritesdbIncremental.h>
#include <favouritesdbobserver.h>
#include <activefavouritesdbnotifier.h >
#include "favouritesitemtestdata.h"

// CONSTANTS
_LIT( KTestDbName, "Test" );

// MACROS
/// Cosmetic purpose macro, to initialize reference item.
#define InitItemL(name)                             \
    name->SetType( CFavouritesItem::EItem );        \
    name->SetNameL( _L( #name ) );                  \
    name->SetUrlL( _L( #name ) );                   \
    name->SetUserNameL( _L( #name ) );              \
    name->SetPasswordL( _L( #name ) );              \
    name->SetParentFolder( KFavouritesRootUid );

/// Cosmetic purpose macro, to initialize reference folder.
#define InitFolderL(name)                           \
    name->SetType( CFavouritesItem::EFolder );      \
    name->SetNameL( _L( #name ) );                  \
    name->SetUrlL( _L( "" ) );                      \
    name->SetUserNameL( _L( "" ) );                 \
    name->SetPasswordL( _L( "" ) );                 \
    name->SetParentFolder( KFavouritesRootUid );

/// Cosmetic purpose macro, to create and initialize reference item.
#define CreateItemL(name)                           \
    name = CFavouritesItem::NewL();                 \
    InitItemL( name );

/// Cosmetic purpose macro, to create and initialize reference folder.
#define CreateFolderL(name)                         \
    name = CFavouritesItem::NewL();                 \
    InitFolderL( name );

// Logging path
_LIT( KFavouritesEngineTestLogPath, "\\logs\\testframework\\FavouritesEngineTest\\" ); 
// Log file
_LIT( KFavouritesEngineTestLogFile, "FavouritesEngineTest.txt" ); 

// Function pointer related internal definitions
// Rounding known bug in GCC

#ifdef __VC32__
#define GETPTR 
#else
#define GETPTR &
#endif  
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}
#define OOM_ENTRY(str,func,a,b,c) {_S(str), GETPTR func,a,b,c}
#define OOM_FUNCENTRY(func,a,b,c) {_S(#func), GETPTR func,a,b,c}

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
class CFavouritesEngineTest;
class CActiveFavouritesDbNotifier;

// A typedef for function that does the actual testing,
// function is a type 
// TInt CDemoModule::<NameOfFunction> ( TTestResult& aResult )
typedef TInt (CFavouritesEngineTest::* TestFunction)(TTestResult&);    

// CLASS DECLARATION

/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*
*  \b Library: ?library
*
*  @since ?Series60_version
*/
class TCaseInfoInternal
    {
    public:
        const TText*    iCaseName;
        TestFunction    iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;
    };

// CLASS DECLARATION

/**
*  A structure containing a test case name and
*  the pointer to function doing the test
*
*  \b Library: ?library
*
*  @since ?Series60_version
*/
class TCaseInfo
    {
    public:
        TPtrC           iCaseName;    
        TestFunction    iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;

    TCaseInfo( const TText* a ) : iCaseName( (TText*) a )
        {        
        };

    };

// CLASS DECLARATION

/**
*  This a FavouritesBCTest class.
*  ?other_description_lines
*
*  \b Library: ?library
*
*  @since ?Series60_version
*/
//NONSHARABLE_CLASS(CFavouritesEngineTest) : public CTestModuleBase,
//                                       public MRfsApMapper
NONSHARABLE_CLASS(CFavouritesEngineTest) : public CTestModuleBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CFavouritesEngineTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CFavouritesEngineTest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    private:    // from MRfsApMApper

        /**
        * Dummy AP mapping to test RFS. Original value is returned always.
        * @return ETrue.
        */
        TBool MapAccessPoint( TUint aOldUid, TUint& aNewUid );

    public: // Functions from base classes

        /**
        * From CTestModuleBase InitL is used to initialize the FavouritesBCTest. 
        *       It is called once for every instance of FavouritesBCTest after
        *       its creation.
        * @since ?Series60_version
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first 
        *               created instance of FavouritesBCTest.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
        *   from FavouritesBCTest. 
        * @since ?Series60_version
        * @param aTestCaseFile Test case file (optional)
        * @param aTestCases  Array of TestCases returned to test framework
        * @return Symbian OS error code
        */
        TInt GetTestCasesL( const TFileName& aTestCaseFile, 
                            RPointerArray<TTestCaseInfo>& aTestCases );

        /**
        * From CTestModuleBase RunTestCaseL is used to run an individual 
        *   test case. 
        * @since ?Series60_version
        * @param aCaseNumber Test case number
        * @param aTestCaseFile Test case file (optional)
        * @param aResult Test case result returned to test framework (PASS/FAIL)
        * @return Symbian OS error code (test case execution error, which is 
        *           not reported in aResult parameter as test case failure).
        */   
        TInt RunTestCaseL( const TInt aCaseNumber, 
                           const TFileName& aTestCaseFile,
                           TTestResult& aResult );

        /**
        * From CTestModuleBase; OOMTestQueryL is used to specify is particular
        * test case going to be executed using OOM conditions
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailureType OOM failure type (optional)
        * @param aFirstMemFailure The first heap memory allocation failure value (optional)
        * @param aLastMemFailure The last heap memory allocation failure value (optional)
        * @return TBool
        */
        virtual TBool OOMTestQueryL( const TFileName& /* aTestCaseFile */, 
                                     const TInt /* aCaseNumber */, 
                                     TOOMFailureType& /* aFailureType */,
                                     TInt& /* aFirstMemFailure */, 
                                     TInt& /* aLastMemFailure */ );

        /**
        * From CTestModuleBase; User may add implementation for OOM test 
        * environment initialization. Usually no implementation is required.
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        virtual void OOMTestInitializeL( const TFileName& /* aTestCaseFile */, 
                                    const TInt /* aCaseNumber */ ); 

        /**
        * From CTestModuleBase; OOMHandleWarningL. User may add implementation 
        * for OOM test warning handling. Usually no implementation is required.
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailNextValue FailNextValue for OOM test execution (optional)
        * @return None
        */
        virtual void OOMHandleWarningL( const TFileName& /* aTestCaseFile */,
                                        const TInt /* aCaseNumber */, 
                                        TInt& /* aFailNextValue */);

        /**
        * From CTestModuleBase; OOMTestFinalizeL may be used to finalize OOM
        * test environment. Usually no implementation is required.
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        virtual void OOMTestFinalizeL( const TFileName& /* aTestCaseFile */, 
                                       const TInt /* aCaseNumber */ );

    private:

        /**
        * C++ default constructor.
        */
        CFavouritesEngineTest();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Function returning test case name and pointer to test case function.
        * @since ?Series60_version
        * @param aCaseNumber test case number
        * @return TCaseInfo 
        */
        const TCaseInfo Case ( const TInt aCaseNumber ) const;

    private:   // Functions to test FavouritesItem interface

        /**
        * FavouritesItem NewLC test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemNewLCTestL( TTestResult& aResult );

        /**
        * FavouritesItem NewL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemNewLTestL( TTestResult& aResult );

        /**
        * FavouritesItem destructor test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemDestructorTestL( TTestResult& aResult );

        /**
        * FavouritesItem Assign(=) operator test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemAssignTestL( TTestResult& aResult );

        /**
        * FavouritesItem Assign(=) operator test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemAssignSelfTestL( TTestResult& aResult );

        /**
        * FavouritesItem Assign(=) operator test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemAssignCompleteTestL( TTestResult& aResult );
        
        /**
        * FavouritesItem Uid test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemUidTestL( TTestResult& aResult );

        /**
        * FavouritesItem ParentFolder test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemParentFolderTestL( TTestResult& aResult );

        /**
        * FavouritesItem Type test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemTypeTestL( TTestResult& aResult );

        /**
        * FavouritesItem Name test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemNameTestL( TTestResult& aResult );

        /**
        * FavouritesItem Url test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemUrlTestL( TTestResult& aResult );

        /**
        * FavouritesItem WapAp test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemWapApTestL( TTestResult& aResult );

        /**
        * FavouritesItem UserName test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemUserNameTestL( TTestResult& aResult );

        /**
        * FavouritesItem Password test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemPasswordTestL( TTestResult& aResult );

        /**
        * FavouritesItem ContextId test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemContextIdTestL( TTestResult& aResult );

        /**
        * FavouritesItem IsItem test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemIsItemTestL( TTestResult& aResult );

        /**
        * FavouritesItem IsItemEFolder test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemIsItemEFolderTestL( TTestResult& aResult );

        /**
        * FavouritesItem IsFolder test case for type EItem.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        
        TInt ItemIsFolderTestL( TTestResult& aResult );
        /**
         * FavouritesItem IsFolder test case for type EFolder.
         * @since ?Series60_version
         * @param aResult Test case result (PASS/FAIL)
         * @return Symbian OS error code (test case execution error 
         *   that is not returned as test case result in aResult)
         */
        
        TInt ItemIsFolderEFolderTestL( TTestResult& aResult );

        /**
        * FavouritesItem IsFactoryItem test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemIsFactoryItemTestL( TTestResult& aResult );

        /**
        * FavouritesItem IsReadOnly test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemIsReadOnlyTestL( TTestResult& aResult );

        /**
        * FavouritesItem Modified test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemModifiedTestL( TTestResult& aResult );

        /**
        * FavouritesItem ClearL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemClearLTestL( TTestResult& aResult );

        /**
        * FavouritesItem SetParentFolder test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemSetParentFolderTestL( TTestResult& aResult );

        /**
        * FavouritesItem SetType test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemSetTypeTestL( TTestResult& aResult );

        /**
        * FavouritesItem SetTypeNone test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemSetTypeNoneTestL( TTestResult& aResult );        

        /**
        * FavouritesItem SetNameL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemSetNameLTestL( TTestResult& aResult );

        /**
        * FavouritesItem ItemSetNameMaxLengthTruncateLTestL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemSetNameMaxLengthTruncateLTestL( TTestResult& aResult );

        /**
        * FavouritesItem ItemSetNameTrimStringLTestL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemSetNameTrimStringLTestL( TTestResult& aResult );

        /**
        * FavouritesItem ItemSetNameRTLMarkLTestL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemSetNameRTLMarkLTestL( TTestResult& aResult );

        /**
        * FavouritesItem ItemSetNameEmptyStringLTestL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemSetNameEmptyStringLTestL( TTestResult& aResult );   
        
        /**
        * FavouritesItem ItemSetNameWhiteSpaceLTestL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemSetNameWhiteSpaceLTestL( TTestResult& aResult );   
        
        /**
        * FavouritesItem SetNameFolderL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */   
        TInt ItemSetNameFolderLTestL( TTestResult& aResult );

        /**
        * FavouritesItem SetUrlL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemSetUrlLTestL( TTestResult& aResult );

        /**
        * FavouritesItem SetWapAp test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemSetWapApTestL( TTestResult& aResult );

        /**
        * FavouritesItem SetUserNameL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemSetUserNameLTestL( TTestResult& aResult );
     
  
        /**
        * FavouritesItem ItemSetUserNameMaxLengthLTestL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemSetUserNameMaxLengthLTestL( TTestResult& aResult );
          
        /**
        * FavouritesItem ItemSetUserNameTruncateLengthLTestL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemSetUserNameTruncateLengthLTestL( TTestResult& aResult );

        /**
        * FavouritesItem SetPasswordL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemSetPasswordLTestL( TTestResult& aResult );

        /**
        * FavouritesItem SetContextId test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemSetContextIdTestL( TTestResult& aResult );

    private:     // Functions to test FavouritesItemList interface

        /**
        * FavouritesItemList constructor (CFavouritesItemList) test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ListConstructorTestL( TTestResult& aResult );

        /**
        * FavouritesItemList destructor (~CFavouritesItemList) test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ListDestructorTestL( TTestResult& aResult );

        /**
        * FavouritesItemList Delete single item test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ListDeleteTestL( TTestResult& aResult );

        /**
        * FavouritesItemList Delete range of items test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ListDeleteRangeTestL( TTestResult& aResult );

        /**
        * FavouritesItemList SortL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ListSortTestL( TTestResult& aResult );

        /**
        * FavouritesItemList UidToIndex test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ListUidToIndexTestL( TTestResult& aResult );

        /**
        * FavouritesItemList IndexToUid test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ListIndexToUidTestL( TTestResult& aResult );
        
        /**
        * FavouritesItemList ItemByUid test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ListItemByUidTestL( TTestResult& aResult );
        
    private:
        /**
        * CreatePopulatedDbL is used to create a Favourites Db
        * populated with test data.
        * @since ?Series60_version
        * @param None
        * @return None
        */
        void CreatePopulatedDbL();
        
        /**
        * DestroyDbItems deletes the items used by CreatePopulatedDbL.
        * @since ?Series60_version
        * @param None
        * @return None
        */
        void DestroyDbItems();
        
    private:   // Functions to test RFavouritesDb interface

        /**
        * RFavouritesDb Open test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbOpenTestL( TTestResult& aResult );

        /**
        * RFavouritesDb Version test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbVersionTestL( TTestResult& aResult );

        /**
        * RFavouritesDb IsDamaged test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbIsDamagedTestL( TTestResult& aResult );

        /**
        * RFavouritesDb Recover test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbRecoverTestL( TTestResult& aResult );

        /**
        * RFavouritesDb Compact test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbCompactTestL( TTestResult& aResult );

        /**
        * RFavouritesDb Size test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbSizeTestL( TTestResult& aResult );

        /**
        * RFavouritesDb UpdateStats test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbUpdateStatsTestL( TTestResult& aResult );

        /**
        * RFavouritesDb Begin test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbBeginTestL( TTestResult& aResult );

        /**
        * RFavouritesDb Commit test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbCommitTestL( TTestResult& aResult );

        /**
        * RFavouritesDb Rollback test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbRollbackTestL( TTestResult& aResult );

        /**
        * RFavouritesDb CleanupRollbackPushL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbCleanupRollbackPushLTestL( TTestResult& aResult );

        /**
        * RFavouritesDb Get test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbGetTestL( TTestResult& aResult );

        /**
        * RFavouritesDb GetAll test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbGetAllTestL( TTestResult& aResult );

        /**
        * RFavouritesDb GetUids test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbGetUidsTestL( TTestResult& aResult );

        /**
        * RFavouritesDb PreferredUid test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbPreferredUidTestL( TTestResult& aResult );

        /**
        * RFavouritesDb Delete test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbDeleteTestL( TTestResult& aResult );

        /**
        * RFavouritesDb Update test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbUpdateTestL( TTestResult& aResult );

        /**
        * RFavouritesDb Add test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbAddTestL( TTestResult& aResult );

        /**
        * RFavouritesDb SetHomepage test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbSetHomepageTestL( TTestResult& aResult );

        /**
        * RFavouritesDb SetLastVisited test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbSetLastVisitedTestL( TTestResult& aResult );

        /**
        * RFavouritesDb SetFactoryItem test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbSetFactoryItemTestL( TTestResult& aResult );

        /**
        * RFavouritesDb SetReadOnly test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbSetReadOnlyTestL( TTestResult& aResult );

        /**
        * RFavouritesDb SetModified test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbSetModifiedTestL( TTestResult& aResult );

        /**
        * RFavouritesDb SetPreferredUid test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbSetPreferredUidTestL( TTestResult& aResult );

        /**
        * RFavouritesDb ItemExists test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbItemExistsTestL( TTestResult& aResult );

        /**
        * RFavouritesDb FolderExists test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbFolderExistsTestL( TTestResult& aResult );

        /**
        * RFavouritesDb Count test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbCountTestL( TTestResult& aResult );

        /**
        * RFavouritesDb SetData test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbSetDataTestL( TTestResult& aResult );

        /**
        * RFavouritesDb GetData test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbGetDataTestL( TTestResult& aResult );

        /**
        * RFavouritesDb SetBrowserData test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbSetBrowserDataTestL( TTestResult& aResult );

        /**
        * RFavouritesDb SetBrowserData test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbGetBrowserDataTestL( TTestResult& aResult );

        /**
        * RFavouritesDb MakeUniqueName with folder argument test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbMakeUniqueNameWithFolderTestL( TTestResult& aResult );

        /**
        * RFavouritesDb MakeUniqueName test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbMakeUniqueNameTestL( TTestResult& aResult );

        /**
        * RFavouritesDb CreateStartPageItemL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbCreateStartPageItemLTestL( TTestResult& aResult );

        /**
        * RFavouritesDb CreateAdaptiveItemsFolderL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbCreateAdaptiveItemsFolderLTestL( TTestResult& aResult );

        /**
        * RFavouritesDb DeleteFile test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbDeleteFileTestL( TTestResult& aResult );

        /**
        * RFavouritesDb RestoreFactorySettingsL test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbRestoreFactorySettingsLTestL( TTestResult& aResult );

        /**
        * RFavouritesDb DeleteFile test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbDeleteFolderTestL( TTestResult& aResult );

        /**
        * FavouritesItem IsHidden test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemIsHiddenTestL( TTestResult& aResult );
        
        /**
        * FavouritesItem SetHidden test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt ItemSetHiddenTestL( TTestResult& aResult );

    private:   // Functions to test TFavouritesWapAp interface

        /**
        * TFavouritesWapAp constructor test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt WapApConstructorTest( TTestResult& aResult );

        /**
        * TFavouritesWapAp assign operator using TFavouritesWapAp test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt WapApAssignWithApTest( TTestResult& aResult );

        /**
        * TFavouritesWapAp assign operator using ApId test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt WapApAssignWithApIdTest( TTestResult& aResult );

        /**
        * TFavouritesWapAp assign operator using self ApId test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt WapApAssignSelfWithApTest( TTestResult& aResult );

        /**
        * TFavouritesWapAp SetNull test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt WapApSetNullTest( TTestResult& aResult );

        /**
        * TFavouritesWapAp SetDefault test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt WapApSetDefaultTest( TTestResult& aResult );

        /**
        * TFavouritesWapAp SetApId test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt WapApSetApIdTest( TTestResult& aResult );

        /**
        * TFavouritesWapAp IsNull test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt WapApIsNullTest( TTestResult& aResult );

        /**
        * TFavouritesWapAp IsDefault test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt WapApIsDefaultTest( TTestResult& aResult );

        /**
        * TFavouritesWapAp ApId test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt WapApApIdTest( TTestResult& aResult );

    private:   // Functions to test RFavouritesDbIncremental interface
    
        /**
        * RFavouritesDbIncremental Recover test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbIncRecoverTestL( TTestResult& aResult );

        /**
        * RFavouritesDbIncremental Compact test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbIncCompactTestL( TTestResult& aResult );

        /**
        * RFavouritesDbIncremental Next test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt DbIncNextTestL( TTestResult& aResult );

    private:   // Functions to test RFavouritesFile interface
    
        /**
        * RFavouritesFile Close test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt FileCloseTestL( TTestResult& aResult );
    
        /**
        * RFavouritesFile Open test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt FileOpenTestL( TTestResult& aResult );
    
        /**
        * RFavouritesFile Replace test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt FileReplaceTestL( TTestResult& aResult );
    
        /**
        * RFavouritesFile Read test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt FileReadTestL( TTestResult& aResult );
    
        /**
        * RFavouritesFile Write test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt FileWriteTestL( TTestResult& aResult );
    
        /**
        * RFavouritesFile Size test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt FileSizeTestL( TTestResult& aResult );

    private:   // Functions to test RFavouritesSession interface
    
        /**
        * RFavouritesSession Version test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt SessionVersionTest( TTestResult& aResult );
    
        /**
        * RFavouritesSession Connect test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt SessionConnectTest( TTestResult& aResult );
    
        /**
        * RFavouritesSession ResourceMark test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt SessionResourceMarkTest( TTestResult& aResult );
    
        /**
        * RFavouritesSession ResourceCheck test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt SessionResourceCheckTest( TTestResult& aResult );
    
        /**
        * RFavouritesSession ResourceCount test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt SessionResourceCountTest( TTestResult& aResult );
    
        /**
        * RFavouritesSession DeleteDatabase test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt SessionDeleteDatabaseTestL( TTestResult& aResult );
    
        /**
        * RFavouritesSession __DbgSetAllocFail test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt SessionDbgSetAllocFailTest( TTestResult& aResult );

    private:   // Functions to test CActiveFavouritesDbNotifier interface
    
        /**
        * CActiveFavouritesDbNotifier constructor test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt NotifierConstructorTestL( TTestResult& aResult );
    
        /**
        * CActiveFavouritesDbNotifier destructor test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt NotifierDestructorTestL( TTestResult& aResult );
    
        /**
        * CActiveFavouritesDbNotifier Start test case.
        * @since ?Series60_version
        * @param aResult Test case result (PASS/FAIL)
        * @return Symbian OS error code (test case execution error 
        *   that is not returned as test case result in aResult)
        */
        TInt NotifierStartTestL( TTestResult& aResult );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // Pointer to test (function) to be executed
        TestFunction iMethod;

        CStifLogger * iLog;
        
        RFavouritesSession iFavouritesSession;
        RFs iFileSystem;
        RFavouritesDb iFavouritesDb;
        CFavouritesItem* iItemFolder1;
        CFavouritesItem* iItemFolder2;
        CFavouritesItem* iItem1;
        CFavouritesItem* iItem2;
        CFavouritesItem* iItem3;
        CFavouritesItem* iItem4;
        CFavouritesItem* iItem5;
        CFavouritesItem* iItem6;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // FAVOURITESENGINETEST_H

// End of File
