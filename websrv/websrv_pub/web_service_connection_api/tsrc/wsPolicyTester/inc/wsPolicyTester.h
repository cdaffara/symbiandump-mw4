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









#ifndef WSPOLICYTESTER_H
#define WSPOLICYTESTER_H

//  EXTERNAL INCLUDES
#include "SenBaseFragment.h"
#include "SenBaseElement.h"
#include "SenDomFragment.h"
#include<flogger.h>
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
_LIT( KwsPolicyTesterLogPath, "\\logs\\testframework\\wsPolicyTester\\" ); 
// Log file
_LIT( KwsPolicyTesterLogFile, "wsPolicyTester.txt" ); 
_LIT( KwsPolicyTesterLogFileWithTitle, "wsPolicyTester_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CwsPolicyTester;
class CPolicyRegistry;
class CWSPolicy;

#include <e32def.h>
#include <e32std.h>
#include <f32file.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CwsPolicyTester test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CwsPolicyTester) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CwsPolicyTester* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CwsPolicyTester();

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
        CwsPolicyTester( CTestModuleIf& aTestModuleIf );

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
    private:    // New methods

         void SetupL();
        
         void Teardown();

         TInt MT_CWSPolicyUtils_PolicyIdLL(CStifItemParser& aItem);

         TInt MT_CWSPolicyUtils_PolicyNameLL(CStifItemParser& aItem );
         TInt MT_CWSPolicyUtils_PolicyUriLL(CStifItemParser& aItem );

         TInt MT_CWSPolicyUtils_IsExpiredPolicyLL(CStifItemParser& aItem );
         
                                    
         TInt MT_CPolicyRegistry_NewLL(CStifItemParser& aItem );
        
        
         TInt MT_CPolicyRegistry_NewLCL(CStifItemParser& aItem );
        
        
         TInt MT_CPolicyRegistry_SetParentL(CStifItemParser& aItem );
        
        
         TInt MT_CPolicyRegistry_ParentL(CStifItemParser& aItem );
        
        
         TInt MT_CPolicyRegistry_LookupPolicyL(CStifItemParser& aItem );
        
        
         TInt MT_CPolicyRegistry_LookupPolicyByNameL(CStifItemParser& aItem );
        
        
         TInt MT_CPolicyRegistry_RegisterPolicyL(CStifItemParser& aItem );
        
        
         TInt MT_CPolicyRegistry_UnregisterPolicyL(CStifItemParser& aItem );
        
        
         TInt MT_CPolicyRegistry_PopulateRegistryL(CStifItemParser& aItem );
        
        
         TInt MT_CPolicyRegistry_ResetRegistryL(CStifItemParser& aItem );
        
        
         TInt MT_CWSPolicy_NewLL(CStifItemParser& aItem );
        
        
         TInt MT_CWSPolicy_NewLCL(CStifItemParser& aItem );
        
        
         TInt MT_CWSPolicy_NewL_1L(CStifItemParser& aItem );
        
        
         TInt MT_CWSPolicy_NewLC_1L(CStifItemParser& aItem );
        
        
         TInt MT_CWSPolicy_NormalizePolicyLL(CStifItemParser& aItem );
        
        
         TInt MT_CWSPolicy_IntersectPolicyLL(CStifItemParser& aItem );
        
        
         TInt MT_CWSPolicy_MergePolicyLL(CStifItemParser& aItem );
        
        
         TInt MT_CWSPolicy_NormalizedL(CStifItemParser& aItem );
        
        
         TInt MT_CWSPolicy_PolicyNormalizerL(CStifItemParser& aItem );
        
        
         TInt MT_CWSPolicy_PolicyAsXmlLL(CStifItemParser& aItem );
        
        
         TInt MT_CWSPolicy_Policy(CStifItemParser& aItem );
        
        
         TInt MT_CWSPolicy_NormalizedAsXmlLL(CStifItemParser& aItem );
        
        
         TInt MT_CWSPolicy_NormalizedPolicy(CStifItemParser& aItem );
        
        
         TInt MT_CWSPolicy_IsExpiredPolicyL(CStifItemParser& aItem );
         
         TInt MT_CSenInternalWsPolicy_NewLL(CStifItemParser& aItem );
         TInt MT_CSenInternalWsPolicy_NewLCL(CStifItemParser& aItem );
         TInt MT_CSenInternalWsPolicy_NewL_1L(CStifItemParser& aItem );
         TInt MT_CSenInternalWsPolicy_NewLC_1L(CStifItemParser& aItem );
         TInt MT_CSenInternalWsPolicy_NewL_2L(CStifItemParser& aItem );
         TInt MT_CSenInternalWsPolicy_NewLC_2L(CStifItemParser& aItem );
         TInt MT_CSenInternalWsPolicy_IsApplicableLL(CStifItemParser& aItem );
         TInt MT_CSenInternalWsPolicy_IsApplicableL_1L(CStifItemParser& aItem );
         TInt MT_CSenInternalWsPolicy_IsApplicableL_2L(CStifItemParser& aItem );
         TInt MT_CSenInternalWsPolicy_UpdateMetadataEndpointLL(CStifItemParser& aItem );
         TInt MT_CSenInternalWsPolicy_AddMetadataPolicyLL(CStifItemParser& aItem );
         TInt MT_CSenInternalWsPolicy_RemoveMetadataPolicyLL(CStifItemParser& aItem );
         TInt MT_CSenInternalWsPolicy_WsPolicyByUriLL(CStifItemParser& aItem );
         TInt MT_CSenInternalWsPolicy_WsPolicyByNameL(CStifItemParser& aItem );
         TInt MT_CSenInternalWsPolicy_ValidUntilLL(CStifItemParser& aItem );
         TInt MT_CSenWSPolicyIdentifier_NewLL(CStifItemParser& aItem );
         TInt MT_CSenWSPolicyIdentifier_NewLCL(CStifItemParser& aItem );
         TInt MT_CSenWSPolicyIdentifier_NewL_1L(CStifItemParser& aItem );
         TInt MT_CSenWSPolicyIdentifier_NewLC_1L(CStifItemParser& aItem );
         TInt MT_CSenWSPolicyIdentifier_NewL_2L(CStifItemParser& aItem );
         TInt MT_CSenWSPolicyIdentifier_NewLC_2L(CStifItemParser& aItem );
         TInt MT_CSenWSPolicyIdentifier_SetPropertyLL(CStifItemParser& aItem );         
         TInt MT_CSenWSPolicyIdentifier_PropertyLL(CStifItemParser& aItem );         
         TInt MT_CSenWSPolicyIdentifier_SetIdLL(CStifItemParser& aItem );         
         TInt MT_CSenWSPolicyIdentifier_IdLL(CStifItemParser& aItem );         
         TInt MT_CSenWSPolicyIdentifier_AddMetadataEndpointLL(CStifItemParser& aItem );         
         TInt MT_CSenWSPolicyIdentifier_MetadataEndpointLL(CStifItemParser& aItem );         
         TInt MT_CSenWSPolicyIdentifier_MetadataEndpointElementLL(CStifItemParser& aItem );    
         TInt MT_CSenWSPolicyIdentifier_ElementAttribValueLL(CStifItemParser& aItem );         
         TInt MT_CSenWSPolicyIdentifier_HasAttributeLL(CStifItemParser& aItem ); 
         TInt MT_CWSPolicyWriter_NewLL(CStifItemParser& aItem );
         TInt MT_CWSPolicyWriter_NewLCL(CStifItemParser& aItem );
         TInt MT_CWSPolicyWriter_WriteToFileAllLL(CStifItemParser& aItem );
         TInt MT_CWSPolicyWriter_WriteToFileAllL_1L(CStifItemParser& aItem );
         TInt MT_CWSPolicyWriter_WriteAllAsXmlLL(CStifItemParser& aItem );
         
         
         TInt MT_CWSPolicyReader_NewLL(CStifItemParser& aItem );
         TInt MT_CWSPolicyReader_NewLCL(CStifItemParser& aItem );
         TInt MT_CWSPolicyReader_GetPolicyLL(CStifItemParser& aItem );
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove
        void DoNormalizePolicyL(TFileName aFile, TInt count);
        void TestIntersectPolicyL(TInt i, TInt j);
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
//        RFs iFss;
//		RFileLogger fL;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // WSPOLICYTESTER_H

// End of File
