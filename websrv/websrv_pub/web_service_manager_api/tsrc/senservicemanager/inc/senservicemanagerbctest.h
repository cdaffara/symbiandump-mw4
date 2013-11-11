/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:          SenServiceManager_stif test module.
*
*/









#ifndef SENSERVICEMANAGERBCTEST_H
#define SENSERVICEMANAGERBCTEST_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>


#include <SenServiceManager.h>
#include <SenIdentityProvider.h>
#include <SenXmlServiceDescription.h>
#include <SenServiceConnection.h>
#include <f32file.h>


//  INTERNAL INCLUDES
namespace{
    _LIT16(KSessionsFile,"c:\\private\\101f96f4\\SenSessions.xml");
    _LIT16(KIdentitiesFile,"c:\\private\\101f96f4\\senidentities.xml");
    _LIT8(KText,"text");
    _LIT8(KText2,"text2");
}

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// Logging path
_LIT( KSenServiceManager_stifLogPath, "\\logs\\testframework\\SenServiceManager_stif\\" ); 
// Log file
_LIT( KSenServiceManager_stifLogFile, "SenServiceManager_stif.txt" ); 


#define GETPTR &
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}
#define OOM_ENTRY(str,func,a,b,c) {_S(str), GETPTR func,a,b,c}
#define OOM_FUNCENTRY(func,a,b,c) {_S(#func), GETPTR func,a,b,c}

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CSenServiceManagerBCTest;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
// A typedef for function that does the actual testing,
// function is a type 
// TInt CSenServiceManagerBCTest::<NameOfFunction> ( TTestResult& aResult )
typedef TInt (CSenServiceManagerBCTest::* TestFunction)(TTestResult&);

// CLASS DECLARATION

/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
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
*  @lib ?library
*  @since ?Series60_version
*/
class TCaseInfo
    {
    public:
        TPtrC iCaseName;
        TestFunction iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;

    TCaseInfo( const TText* a ) : iCaseName( (TText*) a )
        {
        };

    };


// CLASS DECLARATION

/**
*  This a SenServiceManager_stif class.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CSenServiceManagerBCTest) : public CTestModuleBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSenServiceManagerBCTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CSenServiceManagerBCTest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CTestModuleBase InitL is used to initialize the 
        *       SenServiceManager_stif. It is called once for every instance of 
        *       TestModuleSenServiceManager_stif after its creation.
        * @since ?Series60_version
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first 
        *               created instance of SenServiceManager_stif.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
        *   from SenServiceManager_stif. 
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
                                     TOOMFailureType& aFailureType, 
                                     TInt& /* aFirstMemFailure */, 
                                     TInt& /* aLastMemFailure */ );

        /**
        * From CTestModuleBase; OOMTestInitializeL may be used to initialize OOM
        * test environment
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        virtual void OOMTestInitializeL( const TFileName& /* aTestCaseFile */, 
                                    const TInt /* aCaseNumber */ ); 

        /**
        * From CTestModuleBase; OOMHandleWarningL
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailNextValue FailNextValue for OOM test execution (optional)
        * @return None
        *
        * User may add implementation for OOM test warning handling. Usually no
        * implementation is required.           
        */
        virtual void OOMHandleWarningL( const TFileName& /* aTestCaseFile */,
                                        const TInt /* aCaseNumber */, 
                                        TInt& /* aFailNextValue */); 

        /**
        * From CTestModuleBase; OOMTestFinalizeL may be used to finalize OOM
        * test environment
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        *
        */
        virtual void OOMTestFinalizeL( const TFileName& /* aTestCaseFile */, 
                                       const TInt /* aCaseNumber */ );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CSenServiceManagerBCTest();

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


    private:    // methods
    			
         void Empty();

         void SetupL();
        
         void Teardown();
         void DeleteDBL();
         
         
         TInt UT_CSenServiceManager_NewLL(TTestResult& aResult);
        
         TInt UT_CSenServiceManager_NewLCL(TTestResult& aResult);
        
        	  
         TInt UT_CSenServiceManager_ServiceDescriptionsLL_normalL(TTestResult& aResult);
         TInt UT_CSenServiceManager_ServiceDescriptionsLL_notFoundL(TTestResult& aResult);
         TInt UT_CSenServiceManager_ServiceDescriptionsLL_badDescriptorL(TTestResult& aResult);
         TInt UT_CSenServiceManager_ServiceDescriptionsLL_nullL(TTestResult& aResult);
         TInt UT_CSenServiceManager_ServiceDescriptionsLL_randomPtrL(TTestResult& aResult);
        
         TInt UT_CSenServiceManager_ServiceDescriptionsL_1L_normalL(TTestResult& aResult);
         TInt UT_CSenServiceManager_ServiceDescriptionsL_1L_notFoundL(TTestResult& aResult);
         TInt UT_CSenServiceManager_ServiceDescriptionsL_1L_badDescriptorL(TTestResult& aResult);
         TInt UT_CSenServiceManager_ServiceDescriptionsL_1L_XMLpatternL(TTestResult& aResult);
         
         TInt UT_CSenServiceManager_RegisterServiceDescriptionLL_normalL(TTestResult& aResult);
         TInt UT_CSenServiceManager_RegisterServiceDescriptionLL_badDescriptorL(TTestResult& aResult);
         TInt UT_CSenServiceManager_RegisterServiceDescriptionLL_noEndpointL(TTestResult& aResult);
         TInt UT_CSenServiceManager_RegisterServiceDescriptionLL_noContractL(TTestResult& aResult);
         TInt UT_CSenServiceManager_RegisterServiceDescriptionLL_badFrameworkL(TTestResult& aResult);
         TInt UT_CSenServiceManager_RegisterServiceDescriptionLL_randomPtrL(TTestResult& aResult);
         TInt UT_CSenServiceManager_RegisterServiceDescriptionLL_nullL(TTestResult& aResult);
         
         TInt UT_CSenServiceManager_UnregisterServiceDescriptionLL_normalL(TTestResult& aResult);
         TInt UT_CSenServiceManager_UnregisterServiceDescriptionLL_badDescriptorL(TTestResult& aResult);
         TInt UT_CSenServiceManager_UnregisterServiceDescriptionLL_noEndpointL(TTestResult& aResult);
         TInt UT_CSenServiceManager_UnregisterServiceDescriptionLL_noContractL(TTestResult& aResult);
         TInt UT_CSenServiceManager_UnregisterServiceDescriptionLL_badFrameworkL(TTestResult& aResult);
         TInt UT_CSenServiceManager_UnregisterServiceDescriptionLL_randomPtrL(TTestResult& aResult);
         TInt UT_CSenServiceManager_UnregisterServiceDescriptionLL_nullL(TTestResult& aResult);
        
        
         TInt UT_CSenServiceManager_RegisterIdentityProviderLL_normalL(TTestResult& aResult);
         TInt UT_CSenServiceManager_RegisterIdentityProviderLL_badDescriptorL(TTestResult& aResult);
         TInt UT_CSenServiceManager_RegisterIdentityProviderLL_notReadyL(TTestResult& aResult);
         TInt UT_CSenServiceManager_RegisterIdentityProviderLL_providerIDInUseL(TTestResult& aResult);
         TInt UT_CSenServiceManager_RegisterIdentityProviderLL_nullL(TTestResult& aResult);
         TInt UT_CSenServiceManager_RegisterIdentityProviderLL_randomPtrL(TTestResult& aResult);

        
         TInt UT_CSenServiceManager_UnregisterIdentityProviderLL_normalL(TTestResult& aResult);
         TInt UT_CSenServiceManager_UnregisterIdentityProviderLL_badDescriptorL(TTestResult& aResult);
         TInt UT_CSenServiceManager_UnregisterIdentityProviderLL_notFoundL(TTestResult& aResult);
         TInt UT_CSenServiceManager_UnregisterIdentityProviderLL_notReadyL(TTestResult& aResult);
         TInt UT_CSenServiceManager_UnregisterIdentityProviderLL_nullL(TTestResult& aResult);
         TInt UT_CSenServiceManager_UnregisterIdentityProviderLL_randomPtrL(TTestResult& aResult);
        
        
         TInt UT_CSenServiceManager_AssociateServiceLL_normalL(TTestResult& aResult);
         TInt UT_CSenServiceManager_AssociateServiceLL_argumentL(TTestResult& aResult);
         TInt UT_CSenServiceManager_AssociateServiceLL_notReadyL(TTestResult& aResult);
         TInt UT_CSenServiceManager_AssociateServiceLL_notFoundL(TTestResult& aResult);
        
        
         TInt UT_CSenServiceManager_DissociateServiceLL_normalL(TTestResult& aResult);
         TInt UT_CSenServiceManager_DissociateServiceLL_argumentL(TTestResult& aResult);
         TInt UT_CSenServiceManager_DissociateServiceLL_notReadyL(TTestResult& aResult);
         TInt UT_CSenServiceManager_DissociateServiceLL_notFoundL(TTestResult& aResult);
        

    private:    // Data
        CSenServiceManager*         iServiceManager;
        CSenXmlServiceDescription*  iSenXmlServiceDescription;
        CSenIdentityProvider*       iProvider;
        RFs                         iFsSession;

		CActiveScheduler* iActiveScheduler;

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // Pointer to test (function) to be executed
        TestFunction iMethod;

        // Pointer to logger
        CStifLogger * iLog; 

        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // SENSERVICEMANAGERBCTEST_H

// End of File