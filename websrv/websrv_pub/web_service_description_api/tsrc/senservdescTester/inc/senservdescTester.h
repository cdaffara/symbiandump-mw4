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









#ifndef SENSERVDESCTESTER_H
#define SENSERVDESCTESTER_H

//  INCLUDES
#include <StifTestModule.h>
#include <StifLogger.h>
#include "TestServiceDescription.h"
#include <SenBaseAttribute.h>
#include <SenBaseElement.h>
#include <SenBaseFragment.h>
#include <TestScripterInternal.h>
#include <SenCredential.h>
#include <SenCredential2.h>
#include <RSenDocument.h>
#include <SenDomFragment.h>
#include <SenFacet.h>
#include <SenIdentityProvider.h>
#include <SenIdentityProviderIdArray8.h>
#include <SenNameSpace.h>
#include <SenServicePattern.h>
#include <SenSoapEnvelope.h>
#include <SenXmlReader.h>
#include <SenXmlServiceDescription.h>
#include <MSenIdentityProviderIdArray.h>
#include <SenDateUtils.h>
#include <SenXmlUtils.h>
#include <SenXmlConstants.h>
#include <e32debug.h>
#include <TestclassAssert.h>
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
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0
namespace{
    _LIT8(KText,"text");
    _LIT8(KText2,"text2");
    _LIT8(KXmlSpecific, "\"&<>");
}

// Logging path
_LIT( KsenservdescTesterLogPath, "\\logs\\testframework\\senservdescTester\\" ); 
// Log file
_LIT( KsenservdescTesterLogFile, "senservdescTester.txt" ); 
_LIT( KsenservdescTesterLogFileWithTitle, "senservdescTester_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CsenservdescTester;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
typedef TInt (CsenservdescTester::* TestFunction)(CStifItemParser&);

// CLASS DECLARATION

/**
*  CsenservdescTester test class for STIF Test Framework TestScripter.
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

NONSHARABLE_CLASS(CsenservdescTester) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CsenservdescTester* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CsenservdescTester();

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
        CsenservdescTester( CTestModuleIf& aTestModuleIf );

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
        const TCaseInfo Case ( const TInt aCaseNumber ) const;
        
     private:    // New methods

         void SetupL();
        
         void Teardown();
        
         
         TPtr16 ConvertToPtr16LC(CSenBaseFragment &fragment);
         TPtr16 ConvertToPtr16LC(CSenBaseElement &element);
                 
         TInt MT_CSenConsumerPolicy_NewLL( CStifItemParser& aItem );
         TInt MT_CSenConsumerPolicy_NewLCL(CStifItemParser& aItem);
         TInt MT_CSenConsumerPolicy_NewL_1L(CStifItemParser& aItem);
         TInt MT_CSenConsumerPolicy_NewLC_1L(CStifItemParser& aItem);
         void MT_CSenConsumerPolicy_RebuildFromL();
         void MT_CSenConsumerPolicy_AcceptsL();

         TInt MT_CSenCredential_NewLL(CStifItemParser& aItem);
         TInt MT_CSenCredential_NewLCL(CStifItemParser& aItem);
         TInt MT_CSenCredential_NewL_1L(CStifItemParser& aItem);
         TInt MT_CSenCredential_NewLC_1L(CStifItemParser& aItem);
         TInt MT_CSenCredential_NewL_2L(CStifItemParser& aItem);
         TInt MT_CSenCredential_NewLC_2L(CStifItemParser& aItem);
         TInt MT_CSenCredential_IdL(CStifItemParser& aItem);
         TInt MT_CSenCredential_ValidUntilL(CStifItemParser& aItem);
         TInt MT_CSenCredential_SetValidUntilL(CStifItemParser& aItem);
         TInt MT_CSenCredential_StartElementLL(CStifItemParser& aItem);
         TInt MT_CSenCredential2_NewLL(CStifItemParser& aItem);
         TInt MT_CSenCredential2_NewLCL(CStifItemParser& aItem);
         TInt MT_CSenCredential2_NewL_1L(CStifItemParser& aItem);
         TInt MT_CSenCredential2_NewLC_1L(CStifItemParser& aItem);
         TInt MT_CSenCredential2_NewL_2L(CStifItemParser& aItem);
         TInt MT_CSenCredential2_NewLC_2L(CStifItemParser& aItem);
         TInt MT_CSenCredential2_NewL_3L(CStifItemParser& aItem);
         TInt MT_CSenCredential2_NewLC_3L(CStifItemParser& aItem);
         TInt MT_CSenCredential2_NewL_4L(CStifItemParser& aItem);
         TInt MT_CSenCredential2_NewLC_4L(CStifItemParser& aItem);
         TInt MT_CSenCredential2_IdL(CStifItemParser& aItem);
         TInt MT_CSenCredential2_ValidUntilL(CStifItemParser& aItem);
         TInt MT_CSenCredential2_SetValidUntilL(CStifItemParser& aItem);
         
         TInt MT_CSenFacet_NewLL( CStifItemParser& aItem );
         TInt MT_CSenFacet_NewL_1L(CStifItemParser& aItem);
         TInt MT_CSenFacet_NewL_2L(CStifItemParser& aItem);
         TInt MT_CSenFacet_SetNameLL( CStifItemParser& aItem );
         TInt MT_CSenFacet_SetTypeLL(CStifItemParser& aItem);
         TInt MT_CSenFacet_SetValueLL(CStifItemParser& aItem);
         TInt MT_CSenFacet_NameL(CStifItemParser& aItem);
         TInt MT_CSenFacet_TypeL(CStifItemParser& aItem);
         TInt MT_CSenFacet_ValueL(CStifItemParser& aItem);

         TInt MT_CSenIdentityProvider_NewL_L(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_NewLCL( CStifItemParser& aItem );
         TInt MT_CSenIdentityProvider_NewL_1L(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_NewLC_1L(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_NewL_2L(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_NewLC_2L(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_NewL_3L(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_NewLC_3L(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_AuthzIDL(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_AdvisoryAuthnIDL(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_ProviderIDL(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_PasswordL(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_IMEIL(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_UserNameL(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_SetProviderIDL(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_SetServiceIDL(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_IsTrustedByLL(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_IsTrustedByL1L(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_IsDefaultL(CStifItemParser& aItem);
         void MT_CSenIdentityProvider_SetUserInfoLL();
         TInt MT_CSenIdentityProvider_HttpCredentialsLL(CStifItemParser& aItem);
         TInt MT_CSenIdentityProvider_NewElementNameL(CStifItemParser& aItem);

         TInt MT_CSenIdentityProviderIdArray8_NewLL(CStifItemParser& aItem);
         TInt MT_CSenIdentityProviderIdArray8_NewLCL(CStifItemParser& aItem);
         TInt MT_CSenIdentityProviderIdArray8_NewL_1L(CStifItemParser& aItem);
         TInt MT_CSenIdentityProviderIdArray8_NewLC_1L(CStifItemParser& aItem);
         TInt MT_CSenIdentityProviderIdArray8_IsStrictL(CStifItemParser& aItem);
         TInt MT_CSenIdentityProviderIdArray8_SetStrictL(CStifItemParser& aItem);

         void MT_CSenPolicy_NewLL();
         void MT_CSenPolicy_NewLCL();
         void MT_CSenPolicy_NewL_1L();
         void MT_CSenPolicy_NewLC_1L();
         void MT_CSenPolicy_SetIapIdLL();
         void MT_CSenPolicy_IapIdL();
         void MT_CSenPolicy_SetIdentityProviderIdsLL( CStifItemParser& aItem );
         void MT_CSenPolicy_AddIdentityProviderIdLL( CStifItemParser& aItem );
         void MT_CSenPolicy_RebuildFromL();
         void MT_CSenPolicy_IdentityProviderIds8LL();
         void MT_CSenPolicy_AcceptsL();

         void MT_CSenProviderPolicy_NewLL();
         void MT_CSenProviderPolicy_NewLCL();
         void MT_CSenProviderPolicy_NewL_1L();
         void MT_CSenProviderPolicy_NewLC_1L();
         void MT_CSenProviderPolicy_AcceptsL();
         void MT_CSenProviderPolicy_RebuildFromL();

         TInt MT_CSenServicePattern_NewLL(CStifItemParser& aItem);
         TInt MT_CSenServicePattern_NewLCL(CStifItemParser& aItem);
         TInt MT_CSenServicePattern_NewL_1L(CStifItemParser& aItem);
         TInt MT_CSenServicePattern_NewLC_1L(CStifItemParser& aItem);
         TInt MT_CSenServicePattern_NewL_2L(CStifItemParser& aItem);
         TInt MT_CSenServicePattern_NewLC_2L(CStifItemParser& aItem);
         TInt MT_CSenServicePattern_ConsumerPolicyAsXmlLL(CStifItemParser& aItem);
         TInt MT_CSenServicePattern_MatchesL(CStifItemParser& aItem);
//         void MT_CSenServicePattern_StartElementLL();//need parsing some xml file
         TInt MT_CSenServicePattern_SetConsumerIapIdLL(CStifItemParser& aItem);
         TInt MT_CSenServicePattern_ConsumerIapIdL(CStifItemParser& aItem);
         TInt MT_CSenServicePattern_SetConsumerIdentityProviderIdsLL(CStifItemParser& aItem);
         TInt MT_CSenServicePattern_AddConsumerIdentityProviderIdLL(CStifItemParser& aItem);
         //TInt MT_CSenServicePattern_ConsumerPolicyAsXmlLL(CStifItemParser& aItem);
         
         TInt MT_CSenServicePattern_ConsumerIdentityProviderIds8LL(CStifItemParser& aItem);
         TInt MT_CSenServicePattern_AcceptsConsumerPolicyL(CStifItemParser& aItem);
         TInt MT_CSenServicePattern_RebuildFromConsumerPolicyL(CStifItemParser& aItem);
         TInt MT_CSenServicePattern_StartElementLL(CStifItemParser& aItem);


         TInt MT_CSenXmlServiceDescription_NewLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_NewLCL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_NewL_1L(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_NewLC_1L(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_NewL_2L(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_NewLC_2L(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_SetFrameworkIdLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_NewElementNameL(CStifItemParser& aItem);
//         void MT_CSenXmlServiceDescription_CredentialsL();//need parsing some xml file
         TInt MT_CSenXmlServiceDescription_SetIapIdLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_IapIdL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_SetIdentityProviderIdsLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_AddIdentityProviderIdLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_RebuildFromL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_IdentityProviderIds8LL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_AcceptsL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_DescriptionClassTypeL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_MatchesL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_SetProviderIDL( CStifItemParser& aItem ) ;
         TInt MT_CSenXmlServiceDescription_SetPolicyLL( CStifItemParser& aItem ) ;
         TInt MT_CSenXmlServiceDescription_SetPolicyL_1L( CStifItemParser& aItem ) ;
         TInt MT_CSenXmlServiceDescription_SetPolicyL_2L( CStifItemParser& aItem ) ;
         TInt MT_CSenXmlServiceDescription_ContractL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_HasFacetLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_FacetValueL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_AddFacetLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_SetFacetLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_RemoveFacetL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_FacetsLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_ScoreMatchLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_EndpointL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_FrameworkIdL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_FrameworkVersionL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_SetContractLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_SetEndPointLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_AsXmlLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_WriteAsXMLToLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_SetAttributesLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_CredentialsL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_AsXmlUnicodeLL(CStifItemParser& aItem);
         
         
         TInt MT_CSenXmlServiceDescription_HasEqualPrimaryKeysLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_IsLocalLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_SetTransportCueLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_TransportCueL(CStifItemParser& aItem);
         //TInt MT_CSenXmlServiceDescription_ServicePolicyL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_ProviderIdL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_SetPromptUserInfoLL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_PromptUserInfoL(CStifItemParser& aItem);
         TInt MT_CSenXmlServiceDescription_StartEndElementLL(CStifItemParser& aItem);
         
//         void MT_CSenXmlServiceDescription_ResumeParsingFromLL();//need parsing some xml file
        
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

#endif      // SENSERVDESCTESTER_H

// End of File
