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
* Description: Header declaration
*
*/









#ifndef SENXMLTESTER_H
#define SENXMLTESTER_H

//  INCLUDES
#include <StifLogger.h>
#include <SenBaseAttribute.h>
#include <SenBaseElement.h>
#include <SenBaseFragment.h>
#include <SenDomFragment.h>
#include <SenElement.h>
#include <SenNameSpace.h>
#include <SenXmlElement.h>
#include <SenXmlReader.h>
#include <SenXmlUtils.h>
#include <f32file.h>
#include <e32debug.h>
#include <e32std.h>
#include <e32def.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;
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

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KsenxmlTesterLogPath, "\\logs\\testframework\\senxmlTester\\" ); 
// Log file
_LIT( KsenxmlTesterLogFile, "senxmlTester.txt" ); 
_LIT( KsenxmlTesterLogFileWithTitle, "senxmlTester_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CsenxmlTester;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
typedef TInt (CsenxmlTester::* TestFunction)(TTestResult&);

// CLASS DECLARATION

/**
*  CsenxmlTester test class for STIF Test Framework TestScripter.
*  ?other_description_lines
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

NONSHARABLE_CLASS(CsenxmlTester) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CsenxmlTester* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CsenxmlTester();

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
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

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
        CsenxmlTester( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();
        const TCaseInfo Case ( const TInt aCaseNumber ) const;
        
        void SetupL();
        void Teardown();

        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt CSenBaseAttribute_NewLL( CStifItemParser& aItem );
        
        
         TInt CSenBaseAttribute_NewL_1L( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_NewLL( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_NewL_1L( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_NewL_2L( CStifItemParser& aItem );
         
         
         TInt CSenBaseElement_NewL_3L( CStifItemParser& aItem );
         
         
         TInt CSenBaseElement_NewL_4L( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_SetAttributesLL( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_LocalNameL( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_NamespaceURIL( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_NsPrefixL( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_SetPrefixLL( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_HasContentTL( CStifItemParser& aItem );
         
         TInt CSenBaseElement_HasContentFL( CStifItemParser& aItem );        
        
         TInt CSenBaseElement_ContentL( CStifItemParser& aItem );
         
         TInt CSenBaseElement_Content_NullL( CStifItemParser& aItem );        
        
         TInt CSenBaseElement_ContentUnicodeLL( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_SetContentLL( CStifItemParser& aItem );
         
         TInt CSenBaseElement_SetContentL_NullL( CStifItemParser& aItem );        
         
         TInt CSenBaseElement_SetContentL_OverRightL( CStifItemParser& aItem );
         
         TInt CSenBaseElement_SetContentL_WithEscCharsL( CStifItemParser& aItem );
        
         TInt CSenBaseElement_ConsistsOfLL( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_SetNamespaceLL( CStifItemParser& aItem );
         
         TInt CSenBaseElement_SetNamespaceL_NullL( CStifItemParser& aItem );        
        
         TInt CSenBaseElement_SetNamespaceL_1L( CStifItemParser& aItem );
        
         TInt CSenBaseElement_SetNamespaceL_2same1L( CStifItemParser& aItem );
         
         TInt CSenBaseElement_SetNamespaceL_2Diff1L( CStifItemParser& aItem );
       
         TInt CSenBaseElement_SetNamespaceL_DPSU1L( CStifItemParser& aItem );
        
         TInt CSenBaseElement_SetNamespaceL_SNsPC1L( CStifItemParser& aItem );
        
         TInt CSenBaseElement_AddNamespaceL_FL( CStifItemParser& aItem );        
         
         TInt CSenBaseElement_AddNamespaceL_TL( CStifItemParser& aItem );
         
         TInt CSenBaseElement_AddNamespaceL_TSPL( CStifItemParser& aItem ); 
         
         TInt CSenBaseElement_AddNamespaceL_TDPL( CStifItemParser& aItem );        
        
         TInt CSenBaseElement_AddNamespaceL_1L( CStifItemParser& aItem );
         
         TInt CSenBaseElement_AddNamespaceL_2same1L( CStifItemParser& aItem );
         
         TInt CSenBaseElement_AddNamespaceL_2Diff1L( CStifItemParser& aItem );        
        
         TInt CSenBaseElement_NamespaceL( CStifItemParser& aItem );        
        
         TInt CSenBaseElement_Namespace_1L( CStifItemParser& aItem );
         
         TInt CSenBaseElement_Namespace_WP1L( CStifItemParser& aItem );        
        
         TInt CSenBaseElement_Namespace_2L( CStifItemParser& aItem );
        
		 	        
        
         TInt CSenBaseElement_Namespace_3L( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_ElementsLL( CStifItemParser& aItem );
        
        
         
        
         TInt CSenBaseElement_AttributesLL( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_NamespacesLL( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_AttrValueL( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_AddAttrLL( CStifItemParser& aItem );
         TInt CSenBaseElement_AddAttributeLL( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_ParentL( CStifItemParser& aItem );
         
         TInt CSenBaseElement_SetParentL( CStifItemParser& aItem );
        
         TInt CSenBaseElement_RootL( CStifItemParser& aItem );
        
         TInt CSenBaseElement_ElementL( CStifItemParser& aItem );
		TInt CSenBaseElement_ElementsL1( CStifItemParser& aItem );
         TInt CSenBaseElement_ElementsL2( CStifItemParser& aItem );
         
         TInt CSenBaseElement_CreateElementLL( CStifItemParser& aItem );
     
         TInt CSenBaseElement_InsertElementLL( CStifItemParser& aItem );
         
		 TInt CSenBaseElement_InsertElementL_NoBeforeL( CStifItemParser& aItem );                 
        
         TInt CSenBaseElement_AddElementLL( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_AddElementL_1L( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_AddElementL_2L( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_AddElementL_3L( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_RemoveElementL( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_RemoveElement_1L( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_RemoveElement_2L( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_ReplaceElementLL( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_AsXmlLL( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_AsXmlUnicodeLL( CStifItemParser& aItem );
        
         TInt CSenBaseElement_CopyFromLL( CStifItemParser& aItem );
        
         TInt CSenBaseElement_DetachLL( CStifItemParser& aItem );
        
         TInt CSenBaseElement_ChildL( CStifItemParser& aItem );
        
        
         TInt CSenBaseElement_SetL( CStifItemParser& aItem );
         
         
         TInt CSenBaseElement_AddAttributesLL( CStifItemParser& aItem );
         
         TInt CSenBaseElement_AsElementL( CStifItemParser& aItem );
         TInt CSenBaseElement_ContentWriteStreamLL( CStifItemParser& aItem );
		 TInt CSenBaseElement_RemoveAttributeLL( CStifItemParser& aItem );
         TInt CSenXmlElement_NewLL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_NewL_1L( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_NewL_2L( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_SetAttributesLL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_LocalNameL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_NamespaceURIL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_NsPrefixL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_SetPrefixLL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_HasContentL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_ContentL( CStifItemParser& aItem );
         
         TInt CSenXmlElement_ContentWriteStreamLL( CStifItemParser& aItem );
         
        
         TInt CSenXmlElement_ContentUnicodeLL( CStifItemParser& aItem );
                
         TInt CSenXmlElement_SetContentLL( CStifItemParser& aItem );        
        
         TInt CSenXmlElement_ConsistsOfLL( CStifItemParser& aItem );
         
         TInt CSenXmlElement_ConsistsOfL1L( CStifItemParser& aItem );
         
         TInt CSenXmlElement_ConsistsOfL2L( CStifItemParser& aItem );        
        
         TInt CSenXmlElement_SetNamespaceLL( CStifItemParser& aItem );
         
         TInt CSenXmlElement_SetNamespaceL_1L( CStifItemParser& aItem );
         
         TInt CSenXmlElement_SetNamespaceL_2same1L( CStifItemParser& aItem );
         
         TInt CSenXmlElement_SetNamespaceL_2Diff1L( CStifItemParser& aItem ) ;      
        
         TInt CSenXmlElement_AddNamespaceLL( CStifItemParser& aItem );
         
         TInt CSenXmlElement_AddNamespaceLFL( CStifItemParser& aItem );
        
         TInt CSenXmlElement_AddNamespaceL_1L( CStifItemParser& aItem );
      
        
         TInt CSenXmlElement_NamespaceL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_Namespace_1L( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_Namespace_2L( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_Namespace_3L( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_ElementsLL( CStifItemParser& aItem );
                
         TInt CSenXmlElement_AttributesLL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_NamespacesLL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_AttrValueL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_AddAttrLL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_ParentL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_SetParentL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_RootL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_ElementL( CStifItemParser& aItem );
        
         TInt CSenXmlElement_CreateElementLL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_InsertElementLL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_AddElementLL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_AddElementL_1L( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_AddElementL_2L( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_AddElementL_3L( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_RemoveElementL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_RemoveElement_1L( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_RemoveElement_2L( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_ReplaceElementLL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_AsXmlLL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_AsXmlUnicodeLL( CStifItemParser& aItem );
        
         TInt CSenXmlElement_CopyFromLL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_DetachLL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_ChildL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_SetL( CStifItemParser& aItem );
        
        
         TInt CSenXmlElement_AddAttributesLL( CStifItemParser& aItem );
         TInt CSenXmlElement_AddAttributeLL( CStifItemParser& aItem );
         TInt CSenXmlElement_RemoveAttributeLL( CStifItemParser& aItem );
         TInt CSenXmlElement_AsElementL( CStifItemParser& aItem );
         TInt CSenBaseFragment_AsXmlUnicodeLL( CStifItemParser& aItem );
         
         TInt CSenBaseFragment_NewL_L( CStifItemParser& aItem );
         TInt CSenBaseFragment_NewL_1L( CStifItemParser& aItem );
         TInt CSenBaseFragment_NewL_2L( CStifItemParser& aItem );
         TInt CSenBaseFragment_NewL_3L( CStifItemParser& aItem );
         TInt CSenBaseFragment_NewL_4L( CStifItemParser& aItem );
         TInt CSenBaseFragment_NewL_5L( CStifItemParser& aItem );
         TInt CSenBaseFragment_WriteAsXMLToLL( CStifItemParser& aItem );
         TInt CSenBaseFragment_ContentL( CStifItemParser& aItem );
         TInt CSenBaseFragment_NamespaceL( CStifItemParser& aItem );
         TInt CSenBaseFragment_EnsureNamespaceL( CStifItemParser& aItem );
         TInt CSenBaseFragment_DetachLL( CStifItemParser& aItem );
         TInt CSenBaseFragment_SaveNamespacesLL( CStifItemParser& aItem );
         TInt CSenBaseFragment_ResetContentLL( CStifItemParser& aItem );
         TInt CSenBaseFragment_LocalNameL( CStifItemParser& aItem );
         TInt CSenBaseFragment_NsUriL( CStifItemParser& aItem );
         TInt CSenBaseFragment_NsPrefixL( CStifItemParser& aItem );
         TInt CSenBaseFragment_ExtractElementL( CStifItemParser& aItem );
         TInt CSenBaseFragment_SetAttributesLL( CStifItemParser& aItem );
         TInt CSenBaseFragment_ConsistsOfLL( CStifItemParser& aItem );
         TInt CSenBaseFragment_AllocContentSaverLL( CStifItemParser& aItem );
         TInt CSenBaseFragment_ParseLL( CStifItemParser& aItem );
         TInt CSenBaseFragment_ParseL1L( CStifItemParser& aItem );
         TInt CSenBaseFragment_SetContentOfLL( CStifItemParser& aItem );
         TInt CSenBaseFragment_ContentOfL( CStifItemParser& aItem );
         TInt CSenXMLReader_ParserFeatureL( CStifItemParser& aItem );
		 TInt CSenXMLReader_ParseLL( CStifItemParser& aItem );
         TInt CSenDomFragment_ConsistsOfLL( CStifItemParser& aItem );
         
         TInt CSenDomFragment_NewL_L( CStifItemParser& aItem );
         TInt CSenDomFragment_NewL_1L( CStifItemParser& aItem );
         TInt CSenDomFragment_NewL_2L( CStifItemParser& aItem );
         TInt CSenDomFragment_NewL_3L( CStifItemParser& aItem );
         TInt CSenDomFragment_NewL_4L( CStifItemParser& aItem );
         TInt CSenDomFragment_NewL_5L( CStifItemParser& aItem );
         TInt CSenDomFragment_NewL_6L( CStifItemParser& aItem );
         
         //TInt CSenDomFragment_ExpandLL( CStifItemParser& aItem );
         //TInt CSenDomFragment_ResumeParsingFromLL( CStifItemParser& aItem );
         TInt CSenDomFragment_ParseWithLL( CStifItemParser& aItem );
         TInt CSenDomFragment_SetAttributesLL( CStifItemParser& aItem );
         TInt CSenDomFragment_ParseL1L( CStifItemParser& aItem );
         
         TInt CSenNamespace_NewL_L( CStifItemParser& aItem );
         TInt CSenNamespace_NewLC_L( CStifItemParser& aItem );
         TInt CSenNamespace_NewL_1L( CStifItemParser& aItem );
         TInt CSenNamespace_NewLC_1L( CStifItemParser& aItem );
         TInt CSenNamespace_MatchesL_L( CStifItemParser& aItem );
         TInt CSenNamespace_InsertLL( CStifItemParser& aItem );
         TInt CSenXmlReader_NewL_L( CStifItemParser& aItem );
         TInt CSenXmlReader_NewLC_L( CStifItemParser& aItem );
         TInt CSenXmlReader_NewL_1L( CStifItemParser& aItem );
         TInt CSenXmlReader_NewLC_1L( CStifItemParser& aItem );
         TInt CSenXmlReader_NewL_2L( CStifItemParser& aItem );
         TInt CSenXmlReader_NewLC_2L( CStifItemParser& aItem );
         TInt CSenXmlReader_NewL_3L( CStifItemParser& aItem );
         TInt CSenXmlReader_NewLC_3L( CStifItemParser& aItem );

         TInt CSenXmlReader_EnabledParserFeature_L( CStifItemParser& aItem );        
         
         TInt CSenXmlUtils_ToUtf8LCL( CStifItemParser& aItem );        
         TInt CSenXmlUtils_StartsWithL( CStifItemParser& aItem );        
         TInt CSenXmlUtils_EndsWithL( CStifItemParser& aItem );        
         TInt CSenXmlUtils_AttrValueL( CStifItemParser& aItem );        
         TInt CSenXmlUtils_AllocAttrValueLL( CStifItemParser& aItem );        
         TInt CSenXmlUtils_DecodeHttpCharactersLL( CStifItemParser& aItem );        
         TInt CSenXmlUtils_DecodeHttpCharactersLCL( CStifItemParser& aItem );        
         TInt CSenXmlUtils_NsPrefixL( CStifItemParser& aItem );        
         TInt CSenXmlUtils_LocalNameL( CStifItemParser& aItem );        
         TInt CSenXmlUtils_RemoveAttributeLL( CStifItemParser& aItem );        
         TInt CSenXmlUtils_RemoveAttributeL1L( CStifItemParser& aItem );        
         TInt CSenXmlUtils_AddAttributeLL( CStifItemParser& aItem );        
         TInt CSenXmlUtils_AddAttributeL1L( CStifItemParser& aItem );        
         TInt CSenXmlUtils_AddAttributeL2L( CStifItemParser& aItem );        
         TInt CSenXmlUtils_EncodeHttpCharactersLCL( CStifItemParser& aItem ); 
         TInt CSenDomFragment_BaseConstructL( CStifItemParser& aItem );
         TInt CSenBaseFragment_BaseConstructL( CStifItemParser& aItem );
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

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

#endif      // SENXMLTESTER_H

// End of File
