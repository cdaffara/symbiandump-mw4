/*
* Copyright  (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:          SenFragment test module.
*
*/











#ifndef SENFRAGMENT_H
#define SENFRAGMENT_H

// INCLUDES
#include <StifTestModule.h>
#include <StifLogger.h>
#include "TestFragment.h"

#include <SenDomFragmentBase.h>
#include <SenFragmentBase.h>
#include <SenParser.h>
#include <RSenDocument.h>
//#include <XmlEngDomUtils.h>
#include <SenBaseAttribute.h>
#include <SenBaseElement.h>
#include <SenBaseFragment.h>
#include <SenDomFragment.h>
#include <SenIdentityProvider.h>
#include <SenNameSpace.h>
#include <SenSoapEnvelope.h>
#include <SenSoapFault.h>
#include <SenSoapMessage.h>
#include <SenWsSecurityHeader.h>
#include <SenXmlReader.h>
#include <SenXmlServiceDescription.h>
//#include <SenDateFragment.h>
//#include <SenXmlFragment.h>
#include <SenXmlConstants.h>
#include <f32file.h>
//#include <S32FILE.H>
//#include <e32std.h>

#include <xml/parserfeature.h>           // for TParserFeature enumeration
//#include <XmlEngDSerializationOptions.h> // needed for TSerializationOptions
//#include <XmlEngDBinaryContainer.h>
#include <s32mem.h>

// INCLUDES
#include "SenFragmentBase.h"
#include "delegatefragment.h"



//  FORWARD DECLARATIONS
class CSenBaseAttribute;
class CSenBaseFragment;
class CSenSoapMessage;
class CSenIdentityProvider;
class CSenGuidGen;
class CSenDomFragment;
class CSenSoapFault;
class CSenXmlServiceDescription;
class CSenSoapEnvelope;
class CSenWsSecurityHeader;
class SenXmlFragment;
class CSenBaseElement;
class MSenElement;
class CSenXmlReader;
class SenDateFragment;
class CSenNamespace;
class CSenParser;
class CSenDomFragmentBase;
class RSenDocument;
class CSenFragmentBase;
class CTestFragmentBase;
class CTestDomFragmentBase;

// MACROS
//#define ?macro ?macro_def
#define TEST_MODULE_VERSION_MAJOR 50
#define TEST_MODULE_VERSION_MINOR 8
#define TEST_MODULE_VERSION_BUILD 48

// Logging path
_LIT( KSenFragmentLogPath, "\\logs\\testframework\\SenFragmentBCTest\\" ); 
// Log file
_LIT( KSenFragmentLogFile, "SenFragmentBCTest.txt" ); 

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
class CSenFragmentBCTest;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
// A typedef for function that does the actual testing,
// function is a type 
// TInt CSenFragmentBCTest::<NameOfFunction> ( TTestResult& aResult )
typedef TInt (CSenFragmentBCTest::* TestFunction)(TTestResult&);

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
*  This a SenFragment class.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CSenFragmentBCTest) : public CTestModuleBase
    {
    public:  // Constructors and destructor


        /**
        * Two-phased constructor.
        */
        static CSenFragmentBCTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CSenFragmentBCTest();

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
        *       SenFragment. It is called once for every instance of 
        *       TestModuleSenFragment after its creation.
        * @since ?Series60_version
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first 
        *               created instance of SenFragment.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
        *   from SenFragment. 
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
 				/**
         * Method used to log version of test module
         */
        void SendTestModuleVersion();

    private:

        /**
        * C++ default constructor.
        */
        CSenFragmentBCTest();

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
		TInt MT_CSenDomFragmentBase_NewLL(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_NewL_1L(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_NewL_2L(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_NewL_3L(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_NewL_4L(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_NewL_5L(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_NewL_6L(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_NewL_7L(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_AddAttributesLL(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_AsXmlLL(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_ContentLL(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_NamespaceL(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_ResetContentLL(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_LocalNameL(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_NsUriL(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_NsPrefixL(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_AsElementLL(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_AsDocumentLL(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_ExtractElementL(TTestResult& aResult);
        
         TInt MT_CSenDomFragmentBase_AsXmlUnicodeLL(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_WriteAsXMLToLL(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_ConsistsOfLL(TTestResult& aResult);
        
        
         
        
        
         TInt MT_CSenDomFragmentBase_Parsing1L(TTestResult& aResult);


         TInt MT_CSenDomFragmentBase_Parsing2L(TTestResult& aResult);
         
         
         TInt MT_CSenDomFragmentBase_Parsing3L(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_Parsing4L(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_Parsing5L(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_Parsing6L(TTestResult& aResult);
        
        
         TInt MT_CSenDomFragmentBase_Parsing7L(TTestResult& aResult);
         
         
         TInt MT_CSenDomFragmentBase_Parsing8L(TTestResult& aResult);
         
         TInt MT_CSenDomFragmentBase_OnWriteStartElementLL(TTestResult& aResult);
         
         TInt MT_CSenDomFragmentBase_OnWriteEndElementLL(TTestResult& aResult);

         TInt MT_CSenDomFragmentBase_OnIgnorableWhiteSpaceLL(TTestResult& aResult);
         
         TInt MT_CSenDomFragmentBase_OnSkippedEntityLL(TTestResult& aResult);

         TInt MT_CSenDomFragmentBase_OnProcessingInstructionLL(TTestResult& aResult);

         TInt MT_CSenDomFragmentBase_OnErrorL(TTestResult& aResult);

         TInt MT_CSenDomFragmentBase_GetExtendedInterfaceL(TTestResult& aResult);
         
        
         TInt MT_CSenFragmentBase_NewLL(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_NewL_1L(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_NewL_2L(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_NewL_3L(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_NewL_4L(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_NewL_5L(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_NewL_6L(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_NewL_7L(TTestResult& aResult);
         
         TInt MT_CSenFragmentBase_SetDocumentL(TTestResult& aResult);
         
   
        
         TInt MT_CSenFragmentBase_ContentLL(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_NamespaceL(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_ResetContentLL(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_LocalNameL(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_NsUriL(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_NsPrefixL(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_AsElementLL(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_AsDocumentLL(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_ExtractElementL(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_AsXmlUnicodeLL(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_AsXmlLL(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_WriteAsXMLToLL(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_ConsistsOfLL(TTestResult& aResult);
        
        
         TInt MT_CSenFragmentBase_Parsing1L(TTestResult& aResult);


         TInt MT_CSenFragmentBase_Parsing2L(TTestResult& aResult);
         
         
         TInt MT_CSenFragmentBase_Parsing3L(TTestResult& aResult);


         TInt MT_CSenFragmentBase_Parsing4L(TTestResult& aResult);


         TInt MT_CSenFragmentBase_Parsing5L(TTestResult& aResult);


         TInt MT_CSenParser_NewLL(TTestResult& aResult);
        
        
         TInt MT_CSenParser_NewLCL(TTestResult& aResult);
        
        
         TInt MT_CSenParser_NewL_1L(TTestResult& aResult);
        
        
         TInt MT_CSenParser_NewLC_1L(TTestResult& aResult);
        
        
         TInt MT_CSenParser_NewL_2L(TTestResult& aResult);
        
        
         TInt MT_CSenParser_NewLC_2L(TTestResult& aResult);
/*        
#if defined( __SERIES60_31__ ) || (!defined( __SERIES60_30__ ) && defined( SYMBIAN_SWI_POST_INSTALL_REVOCATION) )

         TInt MT_CSenParser_NewL_3L(TTestResult& aResult);
         TInt MT_CSenParser_NewLC_3L(TTestResult& aResult);

#endif 
*/        
        
         TInt MT_RSenDocument_NewLL(TTestResult& aResult);
        
        
         TInt MT_RSenDocument_NewLCL(TTestResult& aResult);
         
         TInt MT_RSenDocument_NewL_1L(TTestResult& aResult);
         
         TInt MT_RSenDocument_NewLC_1L(TTestResult& aResult);
        
         TInt MT_RSenDocument_CopyL(TTestResult& aResult);
                
         TInt MT_RSenDocument_CloseL(TTestResult& aResult);
                
         TInt MT_RSenDocument_DestroyL(TTestResult& aResult);
         
         TInt MT_RSenDocument_ManualXmlEngineTlsAttachLL(TTestResult& aResult);
         
         TInt MT_RSenDocument_ManualXmlEngineTlsCleanupL(TTestResult& aResult);
         
         TInt MT_TestFragmentBase_SetContentLL(TTestResult& aResult);

         TInt MT_TestFragmentBase_AddContentLL(TTestResult& aResult);
         
         TInt MT_TestDomFragmentBase_SetContentOfLL(TTestResult& aResult);

         TInt MT_TestDomFragmentBase_ContentOfL(TTestResult& aResult);
         
         

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

#endif      // SENFragment_H

// End of File