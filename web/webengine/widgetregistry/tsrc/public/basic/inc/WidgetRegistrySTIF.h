/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Widget Registry test module.
*
*/



#ifndef WIDGETREGISTRYSTIF_H
#define WIDGETREGISTRYSTIF_H

// INCLUDES
#include <WidgetRegistryClient.h>
#include <StifTestModule.h>
#include <StifLogger.h>
#include <f32file.h>


// CONSTANTS
_LIT( KTestDbName, "Test" );

// MACROS

// Logging path
_LIT( KWidgetRegistrySTIFLogPath, "\\logs\\testframework\\" );
// Log file
_LIT( KWidgetRegistrySTIFLogFile, "WidgetRegistrySTIF.txt" );

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CWidgetRegistrySTIF;

// A typedef for function that does the actual testing,
// function is a type
// TInt CDemoModule::<NameOfFunction> ( TTestResult& aResult )
typedef TInt (CWidgetRegistrySTIF::* TestFunction)(TTestResult&);

// CLASS DECLARATION

/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*
*  @since 5.0
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
*  @since 5.0
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
*  This a WidgetRegistrySTIF class.
*
*  @since 5.0
*/
NONSHARABLE_CLASS(CWidgetRegistrySTIF) : public CTestModuleBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CWidgetRegistrySTIF* NewL();

        /**
        * Destructor.
        */
        virtual ~CWidgetRegistrySTIF();

    public: // Functions from base classes

        /**
        * From CTestModuleBase InitL is used to initialize the WidgetRegistrySTIF.
        * It is called once for every instance of WidgetRegistrySTIF after
        * its creation.
        * @since 5.0
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first
        *               created instance of WidgetRegistrySTIF.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases
        *   from WidgetRegistrySTIF.
        * @since 5.0
        * @param aTestCaseFile Test case file (optional)
        * @param aTestCases  Array of TestCases returned to test framework
        * @return Symbian OS error code
        */
        TInt GetTestCasesL( const TFileName& aTestCaseFile,
                            RPointerArray<TTestCaseInfo>& aTestCases );

        /**
        * From CTestModuleBase RunTestCaseL is used to run an individual
        *   test case.
        * @since 5.0
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
        CWidgetRegistrySTIF();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Function returning test case name and pointer to test case function.
        * @since ?Series60_version
        * @param aCaseNumber test case number
        * @return TCaseInfo
        */
        const TCaseInfo Case( const TInt aCaseNumber ) const;

    private:   // tests

        // client session tests
        TInt ForceFailedL( TTestResult& aResult );
        TInt ClientSessionCreateL( TTestResult& aResult );
        TInt ClientSessionDisconnectFirstL( TTestResult& aResult );
        TInt ClientSessionConnectL( TTestResult& aResult );
        TInt ClientSessionTwoConnectL( TTestResult& aResult );
        TInt ClientSessionDisconnectL( TTestResult& aResult );
        TInt ClientSessionVersionL( TTestResult& aResult );
        TInt ClientSessionIsWidgetNullL( TTestResult& aResult );
        TInt ClientSessionIsWidgetInvalidL( TTestResult& aResult );
        TInt ClientSessionIsWidgetValidL( TTestResult& aResult );
        TInt ClientSessionWidgetExistsL( TTestResult& aResult );
        TInt ClientSessionIsWidgetRunningNullL( TTestResult& aResult );
        TInt ClientSessionIsWidgetRunningValidL( TTestResult& aResult );
        TInt ClientSessionIsWidgetRunningInvalidL( TTestResult& aResult );
        TInt ClientSessionWidgetCountL( TTestResult& aResult );

        TInt ClientSessionWidgetExistsOverflowL(TTestResult& aResult);
        TInt ClientSessionWidgetExistsValidL(TTestResult& aResult);
        TInt ClientSessionGetWidgetPathL( TTestResult& aResult );
        TInt ClientSessionGetWidgetUidL( TTestResult& aResult );
        TInt ClientSessionGetWidgetUidValidL( TTestResult& aResult );        
        TInt ClientSessionGetWidgetUidForUrlL( TTestResult& aResult );
        TInt ClientSessionGetAvailableUidL( TTestResult& aResult );
        TInt ClientSessionGetAvailableUidNullL( TTestResult& aResult );
        TInt ClientSessionGetWidgetBundleIdL( TTestResult& aResult );
        TInt ClientSessionGetWidgetBundleNameL( TTestResult& aResult );
        TInt ClientSessionGetWidgetPropertyValueL( TTestResult& aResult );
        TInt ClientSessionInstalledWidgetsL( TTestResult& aResult );
        TInt ClientSessionRunningWidgetsL( TTestResult& aResult );
        TInt ClientSessionRegisterWidgetL( TTestResult& aResult );
        TInt ClientSessionDeRegisterWidgetL( TTestResult& aResult );
        TInt ClientSessionSetActiveL( TTestResult& aResult );
        TInt ClientSessionGetLprojNameL( TTestResult& aResult );
        TInt ClientSessionSecurityPolicyId( TTestResult& aResult );
        
    private: // Data
		
		TUid iValidUid;
        CStifLogger* iLog;
        RFs iFileSystem;
    };

#endif // WIDGETREGISTRYSTIF_H

// End of File
