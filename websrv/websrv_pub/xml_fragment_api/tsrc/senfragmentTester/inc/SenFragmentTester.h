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









#ifndef SENFRAGMENTTESTER_H
#define SENFRAGMENTTESTER_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KSenFragmentTesterLogPath, "\\logs\\testframework\\SenFragmentTester\\" ); 
// Log file
_LIT( KSenFragmentTesterLogFile, "SenFragmentTester.txt" ); 
_LIT( KSenFragmentTesterLogFileWithTitle, "SenFragmentTester_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CSenFragmentTester;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CSenFragmentTester test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CSenFragmentTester) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSenFragmentTester* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CSenFragmentTester();

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
        CSenFragmentTester( CTestModuleIf& aTestModuleIf );

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

        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
         TInt MT_CSenDomFragmentBase_NewLL();
        
        
         TInt MT_CSenDomFragmentBase_NewL_1L();
        
        
         TInt MT_CSenDomFragmentBase_NewL_2L();
        
        
         TInt MT_CSenDomFragmentBase_NewL_3L();
        
        
         TInt MT_CSenDomFragmentBase_NewL_4L();
        
        
         TInt MT_CSenDomFragmentBase_NewL_5L();
        
        
         TInt MT_CSenDomFragmentBase_NewL_6L();
        
        
         TInt MT_CSenDomFragmentBase_NewL_7L();
        
        
         TInt MT_CSenDomFragmentBase_ExpandLL();
        
        
         TInt MT_CSenDomFragmentBase_OnResumeParsingFromLL();
        
        
         TInt MT_CSenDomFragmentBase_AddAttributesLL();
        
        
         TInt MT_CSenDomFragmentBase_AsXmlLL();
        
        
         TInt MT_CSenDomFragmentBase_OnStartElementLL();
        
        
         TInt MT_CSenDomFragmentBase_OnContentLL();
        
        
         TInt MT_CSenDomFragmentBase_OnWriteStartElementLL();
        
        
         TInt MT_CSenDomFragmentBase_OnWriteEndElementLL();
        
        
         TInt MT_CSenDomFragmentBase_OnEndElementLL();
        
        
         TInt MT_CSenDomFragmentBase_OnStartDocumentLL();
        
        
         TInt MT_CSenDomFragmentBase_OnEndDocumentLL();
        
        
         TInt MT_CSenDomFragmentBase_OnStartPrefixMappingLL();
        
        
         TInt MT_CSenDomFragmentBase_OnEndPrefixMappingLL();
        
        
         TInt MT_CSenDomFragmentBase_OnIgnorableWhiteSpaceLL();
        
        
         TInt MT_CSenDomFragmentBase_OnSkippedEntityLL();
        
        
         TInt MT_CSenDomFragmentBase_OnProcessingInstructionLL();
        
        
         TInt MT_CSenDomFragmentBase_OnErrorL();
        
        
         TInt MT_CSenDomFragmentBase_GetExtendedInterfaceL();
        
        
         TInt MT_CSenDomFragmentBase_ContentLL();
        
        
         TInt MT_CSenDomFragmentBase_NamespaceL();
        
        
         TInt MT_CSenDomFragmentBase_ResetContentLL();
        
        
         TInt MT_CSenDomFragmentBase_LocalNameL();
        
        
         TInt MT_CSenDomFragmentBase_NsUriL();
        
        
         TInt MT_CSenDomFragmentBase_NsPrefixL();
        
        
         TInt MT_CSenDomFragmentBase_AsElementLL();
        
        
         TInt MT_CSenDomFragmentBase_AsDocumentLL();
        
        
         TInt MT_CSenDomFragmentBase_ExtractElementL();
        
        
         TInt MT_CSenDomFragmentBase_OnDelegateParsingLL();
        
        
         TInt MT_CSenDomFragmentBase_OnDelegateParsingL_1L();
        
        
         TInt MT_CSenDomFragmentBase_SetOwnerL();
        
        
         TInt MT_CSenDomFragmentBase_AsXmlUnicodeLL();
        
        
         TInt MT_CSenDomFragmentBase_WriteAsXMLToLL();
        
        
         TInt MT_CSenDomFragmentBase_ConsistsOfLL();
        
        
         TInt MT_CSenDomFragmentBase_SetContentHandlerL();
        
        
         TInt MT_CSenDomFragmentBase_Parsing1L();


         TInt MT_CSenDomFragmentBase_Parsing2L();
         
         
         TInt MT_CSenDomFragmentBase_Parsing3L();
        
        
         TInt MT_CSenDomFragmentBase_Parsing4L();
        
        
         TInt MT_CSenDomFragmentBase_Parsing5L();
        
        
         TInt MT_CSenDomFragmentBase_Parsing6L();
        
        
         TInt MT_CSenDomFragmentBase_Parsing7L();
         
         
         TInt MT_CSenDomFragmentBase_Parsing8L();
        
        
         TInt MT_CSenFragmentBase_NewLL();
        
        
         TInt MT_CSenFragmentBase_NewL_1L();
        
        
         TInt MT_CSenFragmentBase_NewL_2L();
        
        
         TInt MT_CSenFragmentBase_NewL_3L();
        
        
         TInt MT_CSenFragmentBase_NewL_4L();
        
        
         TInt MT_CSenFragmentBase_NewL_5L();
        
        
         TInt MT_CSenFragmentBase_NewL_6L();
        
        
         TInt MT_CSenFragmentBase_NewL_7L();
        
        
         TInt MT_CSenFragmentBase_OnStartElementLL();
        
        
         TInt MT_CSenFragmentBase_OnEndElementLL();
        
        
         TInt MT_CSenFragmentBase_OnStartDocumentLL();
        
        
         TInt MT_CSenFragmentBase_OnEndDocumentLL();
        
        
         TInt MT_CSenFragmentBase_OnContentLL();
        
        
         TInt MT_CSenFragmentBase_OnStartPrefixMappingLL();
        
        
         TInt MT_CSenFragmentBase_OnEndPrefixMappingLL();
        
        
         TInt MT_CSenFragmentBase_OnIgnorableWhiteSpaceLL();
        
        
         TInt MT_CSenFragmentBase_OnSkippedEntityLL();
        
        
         TInt MT_CSenFragmentBase_OnProcessingInstructionLL();
        
        
         TInt MT_CSenFragmentBase_OnErrorL();
        
        
         TInt MT_CSenFragmentBase_GetExtendedInterfaceL();
        
        
         TInt MT_CSenFragmentBase_ContentLL();
        
        
         TInt MT_CSenFragmentBase_NamespaceL();
        
        
         TInt MT_CSenFragmentBase_ResetContentLL();
        
        
         TInt MT_CSenFragmentBase_LocalNameL();
        
        
         TInt MT_CSenFragmentBase_NsUriL();
        
        
         TInt MT_CSenFragmentBase_NsPrefixL();
        
        
         TInt MT_CSenFragmentBase_AsElementLL();
        
        
         TInt MT_CSenFragmentBase_AsDocumentLL();
        
        
         TInt MT_CSenFragmentBase_ExtractElementL();
        
        
         TInt MT_CSenFragmentBase_OnDelegateParsingLL();
        
        
         TInt MT_CSenFragmentBase_OnDelegateParsingL_1L();
        
        
         TInt MT_CSenFragmentBase_SetOwnerL();
        
        
         TInt MT_CSenFragmentBase_OnResumeParsingFromLL();
        
        
         TInt MT_CSenFragmentBase_OnWriteStartElementLL();
        
        
         TInt MT_CSenFragmentBase_OnWriteEndElementLL();
        
        
         TInt MT_CSenFragmentBase_AsXmlUnicodeLL();
        
        
         TInt MT_CSenFragmentBase_AsXmlLL();
        
        
         TInt MT_CSenFragmentBase_WriteAsXMLToLL();
        
        
         TInt MT_CSenFragmentBase_ConsistsOfLL();
        
        
         TInt MT_CSenFragmentBase_SetContentHandlerL();
        
        
         TInt MT_CSenFragmentBase_Parsing1L();


         TInt MT_CSenFragmentBase_Parsing2L();
         
         
         TInt MT_CSenFragmentBase_Parsing3L();


         TInt MT_CSenFragmentBase_Parsing4L();


         TInt MT_CSenFragmentBase_Parsing5L();


         TInt MT_CSenParser_NewLL();
        
        
         TInt MT_CSenParser_NewLCL();
        
        
         TInt MT_CSenParser_NewL_1L();
        
        
         TInt MT_CSenParser_NewLC_1L();
        
        
         TInt MT_CSenParser_NewL_2L();
        
        
         TInt MT_CSenParser_NewLC_2L();
        
#if defined( __SERIES60_31__ ) || (!defined( __SERIES60_30__ ) && defined( SYMBIAN_SWI_POST_INSTALL_REVOCATION) )

         TInt MT_CSenParser_NewL_3L();
         TInt MT_CSenParser_NewLC_3L();

#endif 
        
        
         TInt MT_RSenDocument_NewLL();
        
        
         TInt MT_RSenDocument_NewLCL();
        
        
         TInt MT_RSenDocument_NewL_1L();
        
        
         TInt MT_RSenDocument_NewLC_1L();
        
        
         TInt MT_RSenDocument_CopyL();
        
        
         TInt MT_RSenDocument_CloseL();
        
        
         TInt MT_RSenDocument_DestroyL();
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();


    };

#endif      // SENFRAGMENTTESTER_H

// End of File
