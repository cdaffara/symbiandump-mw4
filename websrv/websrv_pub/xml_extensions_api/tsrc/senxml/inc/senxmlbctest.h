/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        SenXml test module.
*
*/








#ifndef __SENXML_H__
#define __SENXML_H__

//  EXTERNAL INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>

#include <SenBaseAttribute.h>
#include <SenBaseElement.h>
#include <SenBaseFragment.h>
#include <SenDomFragment.h>
#include <SenElement.h>
#include <SenNamespace.h>
#include <SenXmlElement.h>
#include <SenXmlReader.h>
#include <SenXmlUtils.h>
#include <f32file.h>
#include <e32debug.h>
#include <e32std.h>
#include <e32def.h>



//  FORWARD DECLARATIONS

class CSenBaseAttribute;
class CSenElement;
class CSenBaseFragment;
class CSenNamespace;
class SenXmlUtils;
class CSenDomFragment;
class CSenBaseElement;
class CSenXmlReader;
class CSenXmlElement;

//#include <e32def.h>
//#ifndef NONSHARABLE_CLASS
//    #define NONSHARABLE_CLASS(x) class x
//#endif

// Logging path
_LIT( KSenXmlLogPath, "\\logs\\testframework\\SenXmlBCTest\\" ); 
// Log file
_LIT( KSenXmlLogFile, "SenXmlBCTest.txt" );

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
class CSenXmlBCTest;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
// A typedef for function that does the actual testing,
// function is a type 
// TInt CSenXmlBCTest::<NameOfFunction> ( TTestResult& aResult )
typedef TInt (CSenXmlBCTest::* TestFunction)(TTestResult&);

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
NONSHARABLE_CLASS(CSenXmlBCTest) : public CTestModuleBase
    {
    public:  // Constructors and destructor


        /**
        * Two-phased constructor.
        */
        static CSenXmlBCTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CSenXmlBCTest();

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
        CSenXmlBCTest();

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
        */         TInt CSenBaseAttribute_NewLL( TTestResult& aResult );
        
        
         TInt CSenBaseAttribute_NewL_1L( TTestResult& aResult );
        
        
         TInt CSenBaseElement_NewLL( TTestResult& aResult );
        
        
         TInt CSenBaseElement_NewL_1L( TTestResult& aResult );
        
        
         TInt CSenBaseElement_NewL_2L( TTestResult& aResult );
         
         
         TInt CSenBaseElement_NewL_3L( TTestResult& aResult );
         
         
         TInt CSenBaseElement_NewL_4L( TTestResult& aResult );
        
        
         TInt CSenBaseElement_SetAttributesLL( TTestResult& aResult );
        
        
         TInt CSenBaseElement_LocalNameL( TTestResult& aResult );
        
        
         TInt CSenBaseElement_NamespaceURIL( TTestResult& aResult );
        
        
         TInt CSenBaseElement_NsPrefixL( TTestResult& aResult );
        
        
         TInt CSenBaseElement_SetPrefixLL( TTestResult& aResult );
        
        
         TInt CSenBaseElement_HasContentTL( TTestResult& aResult );
         
         TInt CSenBaseElement_HasContentFL( TTestResult& aResult );        
        
         TInt CSenBaseElement_ContentL( TTestResult& aResult );
         
         TInt CSenBaseElement_Content_NullL( TTestResult& aResult );        
        
         TInt CSenBaseElement_ContentUnicodeLL( TTestResult& aResult );
        
        
         TInt CSenBaseElement_SetContentLL( TTestResult& aResult );
         
         TInt CSenBaseElement_SetContentL_NullL( TTestResult& aResult );        
         
         TInt CSenBaseElement_SetContentL_OverRightL( TTestResult& aResult );
         
         TInt CSenBaseElement_SetContentL_WithEscCharsL( TTestResult& aResult );
        
         TInt CSenBaseElement_ConsistsOfLL( TTestResult& aResult );
        
        
         TInt CSenBaseElement_SetNamespaceLL( TTestResult& aResult );
         
         TInt CSenBaseElement_SetNamespaceL_NullL( TTestResult& aResult );        
        
         TInt CSenBaseElement_SetNamespaceL_1L( TTestResult& aResult );
        
         TInt CSenBaseElement_SetNamespaceL_2same1L( TTestResult& aResult );
         
         TInt CSenBaseElement_SetNamespaceL_2Diff1L( TTestResult& aResult );
       
         TInt CSenBaseElement_SetNamespaceL_DPSU1L( TTestResult& aResult );
        
         TInt CSenBaseElement_SetNamespaceL_SNsPC1L( TTestResult& aResult );
        
         TInt CSenBaseElement_AddNamespaceL_FL( TTestResult& aResult );        
         
         TInt CSenBaseElement_AddNamespaceL_TL( TTestResult& aResult );
         
         TInt CSenBaseElement_AddNamespaceL_TSPL( TTestResult& aResult ); 
         
         TInt CSenBaseElement_AddNamespaceL_TDPL( TTestResult& aResult );        
        
         TInt CSenBaseElement_AddNamespaceL_1L( TTestResult& aResult );
         
         TInt CSenBaseElement_AddNamespaceL_2same1L( TTestResult& aResult );
         
         TInt CSenBaseElement_AddNamespaceL_2Diff1L( TTestResult& aResult );        
        
         TInt CSenBaseElement_NamespaceL( TTestResult& aResult );        
        
         TInt CSenBaseElement_Namespace_1L( TTestResult& aResult );
         
         TInt CSenBaseElement_Namespace_WP1L( TTestResult& aResult );        
        
         TInt CSenBaseElement_Namespace_2L( TTestResult& aResult );
        
		 	        
        
         TInt CSenBaseElement_Namespace_3L( TTestResult& aResult );
        
        
         TInt CSenBaseElement_ElementsLL( TTestResult& aResult );
        
        
         
        
         TInt CSenBaseElement_AttributesLL( TTestResult& aResult );
        
        
         TInt CSenBaseElement_NamespacesLL( TTestResult& aResult );
        
        
         TInt CSenBaseElement_AttrValueL( TTestResult& aResult );
        
        
         TInt CSenBaseElement_AddAttrLL( TTestResult& aResult );
         TInt CSenBaseElement_AddAttributeLL( TTestResult& aResult );
        
        
         TInt CSenBaseElement_ParentL( TTestResult& aResult );
         
         TInt CSenBaseElement_SetParentL( TTestResult& aResult );
        
         TInt CSenBaseElement_RootL( TTestResult& aResult );
        
         TInt CSenBaseElement_ElementL( TTestResult& aResult );
		TInt CSenBaseElement_ElementsL1( TTestResult& aResult );
         TInt CSenBaseElement_ElementsL2( TTestResult& aResult );
         
         TInt CSenBaseElement_CreateElementLL( TTestResult& aResult );
     
         TInt CSenBaseElement_InsertElementLL( TTestResult& aResult );
         
		 TInt CSenBaseElement_InsertElementL_NoBeforeL( TTestResult& aResult );                 
        
         TInt CSenBaseElement_AddElementLL( TTestResult& aResult );
        
        
         TInt CSenBaseElement_AddElementL_1L( TTestResult& aResult );
        
        
         TInt CSenBaseElement_AddElementL_2L( TTestResult& aResult );
        
        
         TInt CSenBaseElement_AddElementL_3L( TTestResult& aResult );
        
        
         TInt CSenBaseElement_RemoveElementL( TTestResult& aResult );
        
        
         TInt CSenBaseElement_RemoveElement_1L( TTestResult& aResult );
        
        
         TInt CSenBaseElement_RemoveElement_2L( TTestResult& aResult );
        
        
         TInt CSenBaseElement_ReplaceElementLL( TTestResult& aResult );
        
        
         TInt CSenBaseElement_AsXmlLL( TTestResult& aResult );
        
        
         TInt CSenBaseElement_AsXmlUnicodeLL( TTestResult& aResult );
        
         TInt CSenBaseElement_CopyFromLL( TTestResult& aResult );
        
         TInt CSenBaseElement_DetachLL( TTestResult& aResult );
        
         TInt CSenBaseElement_ChildL( TTestResult& aResult );
        
        
         TInt CSenBaseElement_SetL( TTestResult& aResult );
         
         
         TInt CSenBaseElement_AddAttributesLL( TTestResult& aResult );
         
         TInt CSenBaseElement_AsElementL( TTestResult& aResult );
         TInt CSenBaseElement_ContentWriteStreamLL( TTestResult& aResult );
		 TInt CSenBaseElement_RemoveAttributeLL( TTestResult& aResult );
         TInt CSenXmlElement_NewLL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_NewL_1L( TTestResult& aResult );
        
        
         TInt CSenXmlElement_NewL_2L( TTestResult& aResult );
        
        
         TInt CSenXmlElement_SetAttributesLL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_LocalNameL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_NamespaceURIL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_NsPrefixL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_SetPrefixLL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_HasContentL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_ContentL( TTestResult& aResult );
         
         TInt CSenXmlElement_ContentWriteStreamLL( TTestResult& aResult );
         
        
         TInt CSenXmlElement_ContentUnicodeLL( TTestResult& aResult );
                
         TInt CSenXmlElement_SetContentLL( TTestResult& aResult );        
        
         TInt CSenXmlElement_ConsistsOfLL( TTestResult& aResult );
         
         TInt CSenXmlElement_ConsistsOfL1L( TTestResult& aResult );
         
         TInt CSenXmlElement_ConsistsOfL2L( TTestResult& aResult );        
        
         TInt CSenXmlElement_SetNamespaceLL( TTestResult& aResult );
         
         TInt CSenXmlElement_SetNamespaceL_1L( TTestResult& aResult );
         
         TInt CSenXmlElement_SetNamespaceL_2same1L( TTestResult& aResult );
         
         TInt CSenXmlElement_SetNamespaceL_2Diff1L( TTestResult& aResult ) ;      
        
         TInt CSenXmlElement_AddNamespaceLL( TTestResult& aResult );
         
         TInt CSenXmlElement_AddNamespaceLFL( TTestResult& aResult );
        
         TInt CSenXmlElement_AddNamespaceL_1L( TTestResult& aResult );
      
        
         TInt CSenXmlElement_NamespaceL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_Namespace_1L( TTestResult& aResult );
        
        
         TInt CSenXmlElement_Namespace_2L( TTestResult& aResult );
        
        
         TInt CSenXmlElement_Namespace_3L( TTestResult& aResult );
        
        
         TInt CSenXmlElement_ElementsLL( TTestResult& aResult );
                
         TInt CSenXmlElement_AttributesLL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_NamespacesLL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_AttrValueL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_AddAttrLL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_ParentL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_SetParentL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_RootL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_ElementL( TTestResult& aResult );
        
         TInt CSenXmlElement_CreateElementLL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_InsertElementLL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_AddElementLL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_AddElementL_1L( TTestResult& aResult );
        
        
         TInt CSenXmlElement_AddElementL_2L( TTestResult& aResult );
        
        
         TInt CSenXmlElement_AddElementL_3L( TTestResult& aResult );
        
        
         TInt CSenXmlElement_RemoveElementL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_RemoveElement_1L( TTestResult& aResult );
        
        
         TInt CSenXmlElement_RemoveElement_2L( TTestResult& aResult );
        
        
         TInt CSenXmlElement_ReplaceElementLL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_AsXmlLL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_AsXmlUnicodeLL( TTestResult& aResult );
        
         TInt CSenXmlElement_CopyFromLL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_DetachLL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_ChildL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_SetL( TTestResult& aResult );
        
        
         TInt CSenXmlElement_AddAttributesLL( TTestResult& aResult );
         TInt CSenXmlElement_AddAttributeLL( TTestResult& aResult );
         TInt CSenXmlElement_RemoveAttributeLL( TTestResult& aResult );
         TInt CSenXmlElement_AsElementL( TTestResult& aResult );
         TInt CSenBaseFragment_AsXmlUnicodeLL( TTestResult& aResult );
         
         TInt CSenBaseFragment_NewL_L( TTestResult& aResult );
         TInt CSenBaseFragment_NewL_1L( TTestResult& aResult );
         TInt CSenBaseFragment_NewL_2L( TTestResult& aResult );
         TInt CSenBaseFragment_NewL_3L( TTestResult& aResult );
         TInt CSenBaseFragment_NewL_4L( TTestResult& aResult );
         TInt CSenBaseFragment_NewL_5L( TTestResult& aResult );
         TInt CSenBaseFragment_WriteAsXMLToLL( TTestResult& aResult );
         TInt CSenBaseFragment_ContentL( TTestResult& aResult );
         TInt CSenBaseFragment_NamespaceL( TTestResult& aResult );
         TInt CSenBaseFragment_EnsureNamespaceL( TTestResult& aResult );
         TInt CSenBaseFragment_DetachLL( TTestResult& aResult );
         TInt CSenBaseFragment_SaveNamespacesLL( TTestResult& aResult );
         TInt CSenBaseFragment_ResetContentLL( TTestResult& aResult );
         TInt CSenBaseFragment_LocalNameL( TTestResult& aResult );
         TInt CSenBaseFragment_NsUriL( TTestResult& aResult );
         TInt CSenBaseFragment_NsPrefixL( TTestResult& aResult );
         TInt CSenBaseFragment_ExtractElementL( TTestResult& aResult );
         TInt CSenBaseFragment_SetAttributesLL( TTestResult& aResult );
         TInt CSenBaseFragment_ConsistsOfLL( TTestResult& aResult );
         TInt CSenBaseFragment_AllocContentSaverLL( TTestResult& aResult );
         TInt CSenBaseFragment_ParseLL( TTestResult& aResult );
         TInt CSenBaseFragment_ParseL1L( TTestResult& aResult );
         TInt CSenBaseFragment_SetContentOfLL( TTestResult& aResult );
         TInt CSenBaseFragment_ContentOfL( TTestResult& aResult );
         TInt CSenXMLReader_ParserFeatureL( TTestResult& aResult );
		 TInt CSenXMLReader_ParseLL( TTestResult& aResult );
         TInt CSenDomFragment_ConsistsOfLL( TTestResult& aResult );
         
         TInt CSenDomFragment_NewL_L( TTestResult& aResult );
         TInt CSenDomFragment_NewL_1L( TTestResult& aResult );
         TInt CSenDomFragment_NewL_2L( TTestResult& aResult );
         TInt CSenDomFragment_NewL_3L( TTestResult& aResult );
         TInt CSenDomFragment_NewL_4L( TTestResult& aResult );
         TInt CSenDomFragment_NewL_5L( TTestResult& aResult );
         TInt CSenDomFragment_NewL_6L( TTestResult& aResult );
         
         //TInt CSenDomFragment_ExpandLL( TTestResult& aResult );
         //TInt CSenDomFragment_ResumeParsingFromLL( TTestResult& aResult );
         TInt CSenDomFragment_ParseWithLL( TTestResult& aResult );
         TInt CSenDomFragment_SetAttributesLL( TTestResult& aResult );
         TInt CSenDomFragment_ParseL1L( TTestResult& aResult );
         
         TInt CSenNamespace_NewL_L( TTestResult& aResult );
         TInt CSenNamespace_NewLC_L( TTestResult& aResult );
         TInt CSenNamespace_NewL_1L( TTestResult& aResult );
         TInt CSenNamespace_NewLC_1L( TTestResult& aResult );
         TInt CSenNamespace_MatchesL_L( TTestResult& aResult );
         TInt CSenNamespace_InsertLL( TTestResult& aResult );
         TInt CSenXmlReader_NewL_L( TTestResult& aResult );
         TInt CSenXmlReader_NewLC_L( TTestResult& aResult );
         TInt CSenXmlReader_NewL_1L( TTestResult& aResult );
         TInt CSenXmlReader_NewLC_1L( TTestResult& aResult );
         TInt CSenXmlReader_NewL_2L( TTestResult& aResult );
         TInt CSenXmlReader_NewLC_2L( TTestResult& aResult );
         TInt CSenXmlReader_NewL_3L( TTestResult& aResult );
         TInt CSenXmlReader_NewLC_3L( TTestResult& aResult );

         TInt CSenXmlReader_EnabledParserFeature_L( TTestResult& aResult );        
         
         TInt CSenXmlUtils_ToUtf8LCL( TTestResult& aResult );        
         TInt CSenXmlUtils_StartsWithL( TTestResult& aResult );        
         TInt CSenXmlUtils_EndsWithL( TTestResult& aResult );        
         TInt CSenXmlUtils_AttrValueL( TTestResult& aResult );        
         TInt CSenXmlUtils_AllocAttrValueLL( TTestResult& aResult );        
         TInt CSenXmlUtils_DecodeHttpCharactersLL( TTestResult& aResult );        
         TInt CSenXmlUtils_DecodeHttpCharactersLCL( TTestResult& aResult );        
         TInt CSenXmlUtils_NsPrefixL( TTestResult& aResult );        
         TInt CSenXmlUtils_LocalNameL( TTestResult& aResult );        
         TInt CSenXmlUtils_RemoveAttributeLL( TTestResult& aResult );        
         TInt CSenXmlUtils_RemoveAttributeL1L( TTestResult& aResult );        
         TInt CSenXmlUtils_AddAttributeLL( TTestResult& aResult );        
         TInt CSenXmlUtils_AddAttributeL1L( TTestResult& aResult );        
         TInt CSenXmlUtils_AddAttributeL2L( TTestResult& aResult );        
         TInt CSenXmlUtils_EncodeHttpCharactersLCL( TTestResult& aResult ); 
         TInt CSenDomFragment_BaseConstructL( TTestResult& aResult );
         TInt CSenBaseFragment_BaseConstructL( TTestResult& aResult );



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