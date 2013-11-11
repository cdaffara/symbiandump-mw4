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
* Description:        SenServDesc test module.
*
*/


#ifndef SENSERVDESCBCTEST_H
#define SENSERVDESCBCTEST_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>


#include "TestServiceDescription.h"
#include <SenBaseAttribute.h>
#include <SenBaseElement.h>
#include <SenBaseFragment.h>
//#include <SenConsumerPolicy.h>
#include <SenCredential.h>
#include <SenCredential2.h>
#include <RSenDocument.h>
#include <SenDomFragment.h>
#include <SenFacet.h>
#include <SenIdentityProvider.h>
#include <SenIdentityProviderIdArray8.h>
#include <SenNamespace.h>
//#include <SenPolicy.h>
//#include <SenProviderPolicy.h>
#include <SenServicePattern.h>
#include <SenSoapEnvelope.h>
#include <SenXmlReader.h>
#include <SenXmlServiceDescription.h>
#include <MSenIdentityProviderIdArray.h>
#include <SenDateUtils.h>
#include <SenXmlUtils.h>
#include <SenXmlConstants.h>
#include <e32debug.h>



class CSenConsumerPolicy;
class CSenBaseElement;
class CSenBaseFragment;
class CSenIdentityProvider;
class CSenIdentityProviderIdArray8;
class CSenSoapEnvelope;
class CSenPolicy;
class CSenCredential;
class CSenCredential2;

class CSenBaseAttribute;
class CSenServicePattern;
class CSenDomFragment;
class CSenNamespace;
class CSenFacet;
class MSenIdentityProviderIdArray;
class CSenXmlReader;
class CSenXmlServiceDescription;
class CSenProviderPolicy;
class SenXmlUtils;
class SenDateUtils;
class TXmlEngElement;
class RSenDocument;

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def


namespace{
    _LIT8(KText,"text");
    _LIT8(KText2,"text2");
    _LIT8(KXmlSpecific, "\"&<>");
}

// Logging path
_LIT( KSenServDescLogPath, "\\logs\\testframework\\SenServDesc\\" ); 
// Log file
_LIT( KSenServDescLogFile, "SenServDesc.txt" ); 


#define GETPTR &
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}
#define OOM_ENTRY(str,func,a,b,c) {_S(str), GETPTR func,a,b,c}
#define OOM_FUNCENTRY(func,a,b,c) {_S(#func), GETPTR func,a,b,c}

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CSenServDesc;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
// A typedef for function that does the actual testing,
// function is a type 
// TInt CSenServDesc::<NameOfFunction> ( TTestResult& aResult )
typedef TInt (CSenServDesc::* TestFunction)(TTestResult&);

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
*  This a SenServDesc class.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CSenServDesc) : public CTestModuleBase
    {
    public:  // Constructors and destructor


        /**
        * Two-phased constructor.
        */
        static CSenServDesc* NewL();

        /**
        * Destructor.
        */
        virtual ~CSenServDesc();

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
        *       SenServDesc. It is called once for every instance of 
        *       TestModuleSenServDesc after its creation.
        * @since ?Series60_version
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first 
        *               created instance of SenServDesc.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
        *   from SenServDesc. 
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
        CSenServDesc();

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
        
     private:    // New methods

         void SetupL();
        
         void Teardown();
        
         
         TPtr16 ConvertToPtr16LC(CSenBaseFragment &fragment);
         TPtr16 ConvertToPtr16LC(CSenBaseElement &element);
                 
        
         TInt MT_CSenConsumerPolicy_NewLL( TTestResult& aResult );
         TInt MT_CSenConsumerPolicy_NewLCL(TTestResult& aResult);
         TInt MT_CSenConsumerPolicy_NewL_1L(TTestResult& aResult);
         TInt MT_CSenConsumerPolicy_NewLC_1L(TTestResult& aResult);
         void MT_CSenConsumerPolicy_RebuildFromL();
         void MT_CSenConsumerPolicy_AcceptsL();

         TInt MT_CSenCredential_NewLL(TTestResult& aResult);
         TInt MT_CSenCredential_NewLCL(TTestResult& aResults);
         TInt MT_CSenCredential_NewL_1L(TTestResult& aResult);
         TInt MT_CSenCredential_NewLC_1L(TTestResult& aResult);
         TInt MT_CSenCredential_NewL_2L(TTestResult& aResult);
         TInt MT_CSenCredential_NewLC_2L(TTestResult& aResult);
         TInt MT_CSenCredential_IdL(TTestResult& aResult);
         TInt MT_CSenCredential_ValidUntilL(TTestResult& aResult);
         TInt MT_CSenCredential_SetValidUntilL(TTestResult& aResult);
         TInt MT_CSenCredential_StartElementLL(TTestResult& aResult);
         TInt MT_CSenCredential2_NewLL(TTestResult& aResult);
         TInt MT_CSenCredential2_NewLCL(TTestResult& aResults);
         TInt MT_CSenCredential2_NewL_1L(TTestResult& aResult);
         TInt MT_CSenCredential2_NewLC_1L(TTestResult& aResult);
         TInt MT_CSenCredential2_NewL_2L(TTestResult& aResult);
         TInt MT_CSenCredential2_NewLC_2L(TTestResult& aResult);
         TInt MT_CSenCredential2_NewL_3L(TTestResult& aResult);
         TInt MT_CSenCredential2_NewLC_3L(TTestResult& aResult);
         TInt MT_CSenCredential2_NewL_4L(TTestResult& aResult);
         TInt MT_CSenCredential2_NewLC_4L(TTestResult& aResult);
         TInt MT_CSenCredential2_IdL(TTestResult& aResult);
         TInt MT_CSenCredential2_ValidUntilL(TTestResult& aResult);
         TInt MT_CSenCredential2_SetValidUntilL(TTestResult& aResult);
         
         TInt MT_CSenFacet_NewLL( TTestResult& aResult );
         TInt MT_CSenFacet_NewL_1L(TTestResult& aResult);
         TInt MT_CSenFacet_NewL_2L(TTestResult& aResult);
         TInt MT_CSenFacet_SetNameLL( TTestResult& aResult );
         TInt MT_CSenFacet_SetTypeLL(TTestResult& aResult);
         TInt MT_CSenFacet_SetValueLL(TTestResult& aResult);
         TInt MT_CSenFacet_NameL(TTestResult& aResult);
         TInt MT_CSenFacet_TypeL(TTestResult& aResult);
         TInt MT_CSenFacet_ValueL(TTestResult& aResult);

         TInt MT_CSenIdentityProvider_NewL_L(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_NewLCL( TTestResult& aResult );
         TInt MT_CSenIdentityProvider_NewL_1L(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_NewLC_1L(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_NewL_2L(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_NewLC_2L(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_NewL_3L(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_NewLC_3L(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_AuthzIDL(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_AdvisoryAuthnIDL(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_ProviderIDL(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_PasswordL(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_IMEIL(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_UserNameL(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_SetProviderIDL(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_SetServiceIDL(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_IsTrustedByLL(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_IsTrustedByL1L(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_IsDefaultL(TTestResult& aResult);
         void MT_CSenIdentityProvider_SetUserInfoLL();
         TInt MT_CSenIdentityProvider_HttpCredentialsLL(TTestResult& aResult);
         TInt MT_CSenIdentityProvider_NewElementNameL(TTestResult& aResult);

         TInt MT_CSenIdentityProviderIdArray8_NewLL(TTestResult& aResult);
         TInt MT_CSenIdentityProviderIdArray8_NewLCL(TTestResult& aResult);
         TInt MT_CSenIdentityProviderIdArray8_NewL_1L(TTestResult& aResult);
         TInt MT_CSenIdentityProviderIdArray8_NewLC_1L(TTestResult& aResult);
         TInt MT_CSenIdentityProviderIdArray8_IsStrictL(TTestResult& aResult);
         TInt MT_CSenIdentityProviderIdArray8_SetStrictL(TTestResult& aResult);

         void MT_CSenPolicy_NewLL();
         void MT_CSenPolicy_NewLCL();
         void MT_CSenPolicy_NewL_1L();
         void MT_CSenPolicy_NewLC_1L();
         void MT_CSenPolicy_SetIapIdLL();
         void MT_CSenPolicy_IapIdL();
         void MT_CSenPolicy_SetIdentityProviderIdsLL( TTestResult& aResult );
         void MT_CSenPolicy_AddIdentityProviderIdLL( TTestResult& aResult );
         void MT_CSenPolicy_RebuildFromL();
         void MT_CSenPolicy_IdentityProviderIds8LL();
         void MT_CSenPolicy_AcceptsL();

         void MT_CSenProviderPolicy_NewLL();
         void MT_CSenProviderPolicy_NewLCL();
         void MT_CSenProviderPolicy_NewL_1L();
         void MT_CSenProviderPolicy_NewLC_1L();
         void MT_CSenProviderPolicy_AcceptsL();
         void MT_CSenProviderPolicy_RebuildFromL();

         TInt MT_CSenServicePattern_NewLL(TTestResult& aResult);
         TInt MT_CSenServicePattern_NewLCL(TTestResult& aResult);
         TInt MT_CSenServicePattern_NewL_1L(TTestResult& aResult);
         TInt MT_CSenServicePattern_NewLC_1L(TTestResult& aResult);
         TInt MT_CSenServicePattern_NewL_2L(TTestResult& aResult);
         TInt MT_CSenServicePattern_NewLC_2L(TTestResult& aResult);
         TInt MT_CSenServicePattern_ConsumerPolicyAsXmlLL(TTestResult& aResult);
         TInt MT_CSenServicePattern_MatchesL(TTestResult& aResult);
//         void MT_CSenServicePattern_StartElementLL();//need parsing some xml file
         TInt MT_CSenServicePattern_SetConsumerIapIdLL(TTestResult& aResult);
         TInt MT_CSenServicePattern_ConsumerIapIdL(TTestResult& aResult);
         TInt MT_CSenServicePattern_SetConsumerSnapIdLL(TTestResult& aResult);
         TInt MT_CSenServicePattern_ConsumerSnapIdL(TTestResult& aResult);
         TInt MT_CSenServicePattern_SetConsumerIdentityProviderIdsLL(TTestResult& aResult);
         TInt MT_CSenServicePattern_AddConsumerIdentityProviderIdLL(TTestResult& aResult);
         //TInt MT_CSenServicePattern_ConsumerPolicyAsXmlLL(TTestResult& aResult);
         
         TInt MT_CSenServicePattern_ConsumerIdentityProviderIds8LL(TTestResult& aResult);
         TInt MT_CSenServicePattern_AcceptsConsumerPolicyL(TTestResult& aResult);
         TInt MT_CSenServicePattern_RebuildFromConsumerPolicyL(TTestResult& aResult);
         TInt MT_CSenServicePattern_StartElementLL(TTestResult& aResult);


         TInt MT_CSenXmlServiceDescription_NewLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_NewLCL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_NewL_1L(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_NewLC_1L(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_NewL_2L(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_NewLC_2L(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_SetFrameworkIdLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_NewElementNameL(TTestResult& aResult);
//         void MT_CSenXmlServiceDescription_CredentialsL();//need parsing some xml file
         TInt MT_CSenXmlServiceDescription_SetIapIdLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_IapIdL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_SetSnapIdLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_SnapIdL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_SetIdentityProviderIdsLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_AddIdentityProviderIdLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_RebuildFromL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_IdentityProviderIds8LL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_AcceptsL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_DescriptionClassTypeL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_MatchesL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_SetProviderIDL( TTestResult& aResult ) ;
         TInt MT_CSenXmlServiceDescription_SetPolicyLL( TTestResult& aResult ) ;
         TInt MT_CSenXmlServiceDescription_SetPolicyL_1L( TTestResult& aResult ) ;
         TInt MT_CSenXmlServiceDescription_SetPolicyL_2L( TTestResult& aResult ) ;
         TInt MT_CSenXmlServiceDescription_ContractL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_HasFacetLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_FacetValueL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_AddFacetLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_SetFacetLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_RemoveFacetL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_FacetsLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_ScoreMatchLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_EndpointL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_FrameworkIdL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_FrameworkVersionL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_SetContractLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_SetEndPointLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_AsXmlLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_WriteAsXMLToLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_SetAttributesLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_CredentialsL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_AsXmlUnicodeLL(TTestResult& aResult);
         
         
         TInt MT_CSenXmlServiceDescription_HasEqualPrimaryKeysLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_IsLocalLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_SetTransportCueLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_TransportCueL(TTestResult& aResult);
         //TInt MT_CSenXmlServiceDescription_ServicePolicyL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_ProviderIdL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_SetPromptUserInfoLL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_PromptUserInfoL(TTestResult& aResult);
         TInt MT_CSenXmlServiceDescription_StartEndElementLL(TTestResult& aResult);
         
//       void MT_CSenXmlServiceDescription_ResumeParsingFromLL();//need parsing some xml file
         TInt MT_CSenXmlServiceDescription_CopyFromLL( TTestResult& aResult );
        

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

#endif      // SENSERVDESCBCTEST_H

// End of File