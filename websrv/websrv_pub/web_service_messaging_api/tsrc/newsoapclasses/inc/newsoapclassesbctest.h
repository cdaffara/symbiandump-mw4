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
* Description:          NewSoapClasses test module.
*
*/











#ifndef NEWSOAPCLASSES_H
#define NEWSOAPCLASSES_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>

#include <SenParser.h>
#include <SenSoapEnvelope2.h>
#include <SenSoapFault2.h>
#include <SenSoapMessage2.h>
#include <SenSoapConstants.h>
//#include <SenSoapMessageDom2.h>
#include <SenWsSecurityHeader2.h>
#include <SenIdentityProvider.h>
//#include <SenMessageContext.h>
#include <SenXmlProperties.h>
#include <e32debug.h>
#include <e32std.h>
#include <e32def.h>
#include <SenXmlConstants.h>





//  FORWARD DECLARATIONS
class CSenSoapFault2;
class CSenSoapEnvelope2;
class CSenSoapMessage2;
class CSenWsSecurityHeader2;



// Logging path
_LIT( KNewSoapClassesLogPath, "\\logs\\testframework\\NewSoapClassesBCTest\\" ); 
// Log file
_LIT( KNewSoapClassesLogFile, "NewSoapClassesBCTest.txt" ); 

// Function pointer related internal definitions
// Visual studio 6.0 (__VC32__) needs different type of handling 
#ifdef __VC32__
#define GETPTR
#else
#define GETPTR &
#endif  
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}
#define OOM_ENTRY(str,func,a,b,c) {_S(str), GETPTR func,a,b,c}
#define OOM_FUNCENTRY(func,a,b,c) {_S(#func), GETPTR func,a,b,c}

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CNewSoapClassesBCTest;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
// A typedef for function that does the actual testing,
// function is a type 
// TInt CNewSoapClassesBCTest::<NameOfFunction> ( TTestResult& aResult )
typedef TInt (CNewSoapClassesBCTest::* TestFunction)(TTestResult&);

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
*  This a NewSoapClasses class.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CNewSoapClassesBCTest) : public CTestModuleBase
    {
    public:  // Constructors and destructor


        /**
        * Two-phased constructor.
        */
        static CNewSoapClassesBCTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CNewSoapClassesBCTest();

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
        *       NewSoapClasses. It is called once for every instance of 
        *       TestModuleNewSoapClasses after its creation.
        * @since ?Series60_version
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first 
        *               created instance of NewSoapClasses.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
        *   from NewSoapClasses. 
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

    private:

        /**
        * C++ default constructor.
        */
        CNewSoapClassesBCTest();

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
        
        void SetupL();
        void Teardown();


        /**
        * Actual Hardcoded test case functions are listed below.
        */
         TInt MT_CSenSoapEnvelope2_NewLL(TTestResult& aResult);
         
         TInt MT_CSenSoapEnvelope2_NewLCL(TTestResult& aResult);
        
         TInt MT_CSenSoapEnvelope2_NewL_1L(TTestResult& aResult);
         
         TInt MT_CSenSoapEnvelope2_NewLC_1L(TTestResult& aResult);
         
         TInt MT_CSenSoapEnvelope2_NewL_2L(TTestResult& aResult);
         
         TInt MT_CSenSoapEnvelope2_NewLC_2L(TTestResult& aResult);
         
         TInt MT_CSenSoapEnvelope2_TypeL(TTestResult& aResult);
         
         TInt MT_CSenSoapEnvelope2_DirectionL(TTestResult& aResult);
         
         TInt MT_CSenSoapEnvelope2_TxnIdL(TTestResult& aResult);
         
         TInt MT_CSenSoapEnvelope2_CloneL(TTestResult& aResult);
         
         TInt MT_CSenSoapEnvelope2_SoapVersionL(TTestResult& aResult);
         
         TInt MT_CSenSoapEnvelope2_SetContextL(TTestResult& aResult);
         
         TInt MT_CSenSoapEnvelope2_ContextL(TTestResult& aResult);
         
         TInt MT_CSenSoapEnvelope2_SetPropertiesL(TTestResult& aResult);

         TInt MT_CSenSoapEnvelope2_PropertiesL(TTestResult& aResult);
         
         TInt MT_CSenSoapEnvelope2_IsSafeToCastL(TTestResult& aResult); 


         TInt MT_CSenSoapEnvelope2_SetBodyLL(TTestResult& aResult);
         
         
         TInt MT_CSenSoapEnvelope2_SetBodyL_1L(TTestResult& aResult);
        
        
         TInt MT_CSenSoapEnvelope2_BodyLL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapEnvelope2_HeaderLL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapEnvelope2_AddHeaderLL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapEnvelope2_BodyAsStringLL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapEnvelope2_IsFaultL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapEnvelope2_DetachFaultLL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapEnvelope2_FaultLL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapEnvelope2_SetSoapActionLL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapEnvelope2_SoapActionL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapEnvelope2_HasHeaderL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapEnvelope2_HasBodyL(TTestResult& aResult);
         
         
         TInt MT_CSenSoapEnvelope2_Parse1L(TTestResult& aResult);
        
        
         TInt MT_CSenSoapEnvelope2_Parse2L(TTestResult& aResult);
        
        
         TInt MT_CSenSoapEnvelope2_Parse3L(TTestResult& aResult);
        
        
         TInt MT_CSenSoapFault2_NewLL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapFault2_NewL_1L(TTestResult& aResult);
        
        
         TInt MT_CSenSoapFault2_NewL_2L(TTestResult& aResult);
        
        
         TInt MT_CSenSoapFault2_FaultCodeL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapFault2_FaultSubcodeL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapFault2_FaultStringL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapFault2_FaultActorL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapFault2_DetailL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapMessage2_NewLL(TTestResult& aResult);
         
         TInt MT_CSenSoapMessage2_NewLCL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapMessage2_NewL_1L(TTestResult& aResult);
         
         TInt MT_CSenSoapMessage2_NewLC_1L(TTestResult& aResult);
        
        
         TInt MT_CSenSoapMessage2_NewL_2L(TTestResult& aResult);
         
         TInt MT_CSenSoapMessage2_NewLC_2L(TTestResult& aResult);
         
         TInt MT_CSenSoapMessage2_NewL_3L(TTestResult& aResult);
         
         TInt MT_CSenSoapMessage2_NewLC_3L(TTestResult& aResult);
         
         TInt MT_CSenSoapMessage2_NewL_4L(TTestResult& aResult);
         
         TInt MT_CSenSoapMessage2_NewLC_4L(TTestResult& aResult);
         
         TInt MT_CSenSoapMessage2_NewL_5L(TTestResult& aResult);
         
         TInt MT_CSenSoapMessage2_NewLC_5L(TTestResult& aResult);
         
         TInt MT_CSenSoapMessage2_NewL_6L(TTestResult& aResult);
         
         TInt MT_CSenSoapMessage2_NewLC_6L(TTestResult& aResult);
         
         TInt MT_CSenSoapMessage2_TypeL(TTestResult& aResult);
         
		 TInt MT_CSenSoapMessage2_CloneL(TTestResult& aResult);
		 
		 TInt MT_CSenSoapMessage2_Parse1L(TTestResult& aResult);
		 
        
        
         TInt MT_CSenSoapMessage2_SetSecurityHeaderLL(TTestResult& aResult);
        
        
         TInt MT_CSenSoapMessage2_AddSecurityTokenLL(TTestResult& aResult);
         
         
         TInt MT_CSenWsSecurityHeader2_NewLL(TTestResult& aResult);
        
        
         TInt MT_CSenWsSecurityHeader2_NewLCL(TTestResult& aResult);
        
        
         TInt MT_CSenWsSecurityHeader2_NewL_1L(TTestResult& aResult);
        
        
         TInt MT_CSenWsSecurityHeader2_NewLC_1L(TTestResult& aResult);
        
        
         TInt MT_CSenWsSecurityHeader2_NewL_2L(TTestResult& aResult);
        
        
         TInt MT_CSenWsSecurityHeader2_NewLC_2L(TTestResult& aResult);
        
        
         TInt MT_CSenWsSecurityHeader2_UsernameTokenLL(TTestResult& aResult);
        
        
         TInt MT_CSenWsSecurityHeader2_UsernameTokenL_1L(TTestResult& aResult);
        
        
         TInt MT_CSenWsSecurityHeader2_UsernameTokenL_2L(TTestResult& aResult);
        
        
         TInt MT_CSenWsSecurityHeader2_UsernameTokenL_3L(TTestResult& aResult);
        
        
         TInt MT_CSenWsSecurityHeader2_UsernameTokenL_4L(TTestResult& aResult);
        
        
         TInt MT_CSenWsSecurityHeader2_TimestampLL(TTestResult& aResult);
        
        
         TInt MT_CSenWsSecurityHeader2_TimestampL_1L(TTestResult& aResult);
        
        
         TInt MT_CSenWsSecurityHeader2_XmlNsL(TTestResult& aResult);
        
        
         TInt MT_CSenWsSecurityHeader2_XmlNsPrefixL(TTestResult& aResult);
         
		 TInt MT_CTestMSenMessage_Type(TTestResult& aResult);
		 TInt MT_CTestMSenMessage_Direction(TTestResult& aResult);
		 TInt MT_CTestMSenMessage_Context(TTestResult& aResult);
		 TInt MT_CTestMSenMessage_SetContext(TTestResult& aResult);
		 TInt MT_CTestMSenMessage_Properties(TTestResult& aResult);
		 TInt MT_CTestMSenMessage_SetProperties(TTestResult& aResult);
		 TInt MT_CTestMSenMessage_IsSafeToCast(TTestResult& aResult);
		 TInt MT_CTestMSenMessage_TxnId(TTestResult& aResult);
		 TInt MT_CTestMSenMessage_CloneL(TTestResult& aResult);
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

		CSenXmlReader* iXmlReader;
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

#endif      // NewSoapClasses_H

// End of File