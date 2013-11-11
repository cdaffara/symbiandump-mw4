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









#ifndef SENSERVICEMANAGERTESTER_H
#define SENSERVICEMANAGERTESTER_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <SenServiceManager.h>
#include <SenIdentityProvider.h>
#include <SenXmlServiceDescription.h>
#include <SenServiceConnection.h>
#include <f32file.h>
#include <TestclassAssert.h>
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
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KsenservicemanagerTesterLogPath, "\\logs\\testframework\\senservicemanagerTester\\" ); 
// Log file
_LIT( KsenservicemanagerTesterLogFile, "senservicemanagerTester.txt" ); 
_LIT( KsenservicemanagerTesterLogFileWithTitle, "senservicemanagerTester_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CsenservicemanagerTester;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
typedef TInt (CsenservicemanagerTester::* TestFunction)(CStifItemParser&);

// CLASS DECLARATION

/**
*  CsenservicemanagerTester test class for STIF Test Framework TestScripter.
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

NONSHARABLE_CLASS(CsenservicemanagerTester) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CsenservicemanagerTester* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CsenservicemanagerTester();

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
        CsenservicemanagerTester( CTestModuleIf& aTestModuleIf );

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
        const TCaseInfo Case ( const TInt aCaseNumber ) const;
        void Delete();

        /**
        * Test methods are listed below. 
        */

    private:    // methods
         void Empty();

         void SetupL();
        
         void Teardown();
         void DeleteDBL();
         
         
         TInt UT_CSenServiceManager_NewLL(CStifItemParser& aItem);
        
         TInt UT_CSenServiceManager_NewLCL(CStifItemParser& aItem);
         
         TInt UT_CSenServiceManager_NewL_1L(CStifItemParser& aItem);
        
         TInt UT_CSenServiceManager_NewL_1CL(CStifItemParser& aItem);
         
        
        	  
         TInt UT_CSenServiceManager_ServiceDescriptionsLL_normalL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_ServiceDescriptionsLL_notFoundL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_ServiceDescriptionsLL_badDescriptorL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_ServiceDescriptionsLL_nullL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_ServiceDescriptionsLL_randomPtrL(CStifItemParser& aItem);
        
         TInt UT_CSenServiceManager_ServiceDescriptionsL_1L_normalL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_ServiceDescriptionsL_1L_notFoundL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_ServiceDescriptionsL_1L_badDescriptorL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_ServiceDescriptionsL_1L_XMLpatternL(CStifItemParser& aItem);
         
         TInt UT_CSenServiceManager_RegisterServiceDescriptionLL_normalL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_RegisterServiceDescriptionLL_badDescriptorL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_RegisterServiceDescriptionLL_noEndpointL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_RegisterServiceDescriptionLL_noContractL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_RegisterServiceDescriptionLL_badFrameworkL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_RegisterServiceDescriptionLL_randomPtrL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_RegisterServiceDescriptionLL_nullL(CStifItemParser& aItem);
         
         TInt UT_CSenServiceManager_UnregisterServiceDescriptionLL_normalL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_UnregisterServiceDescriptionLL_badDescriptorL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_UnregisterServiceDescriptionLL_noEndpointL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_UnregisterServiceDescriptionLL_noContractL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_UnregisterServiceDescriptionLL_badFrameworkL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_UnregisterServiceDescriptionLL_randomPtrL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_UnregisterServiceDescriptionLL_nullL(CStifItemParser& aItem);
        
        
         TInt UT_CSenServiceManager_RegisterIdentityProviderLL_normalL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_RegisterIdentityProviderLL_badDescriptorL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_RegisterIdentityProviderLL_notReadyL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_RegisterIdentityProviderLL_providerIDInUseL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_RegisterIdentityProviderLL_nullL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_RegisterIdentityProviderLL_randomPtrL(CStifItemParser& aItem);

        
         TInt UT_CSenServiceManager_UnregisterIdentityProviderLL_normalL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_UnregisterIdentityProviderLL_badDescriptorL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_UnregisterIdentityProviderLL_notFoundL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_UnregisterIdentityProviderLL_notReadyL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_UnregisterIdentityProviderLL_nullL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_UnregisterIdentityProviderLL_randomPtrL(CStifItemParser& aItem);
        
        
         TInt UT_CSenServiceManager_AssociateServiceLL_normalL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_AssociateServiceLL_argumentL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_AssociateServiceLL_notReadyL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_AssociateServiceLL_notFoundL(CStifItemParser& aItem);
        
        
         TInt UT_CSenServiceManager_DissociateServiceLL_normalL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_DissociateServiceLL_argumentL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_DissociateServiceLL_notReadyL(CStifItemParser& aItem);
         TInt UT_CSenServiceManager_DissociateServiceLL_notFoundL(CStifItemParser& aItem);
        

    private:    // Data
        CSenServiceManager*         iServiceManager;
        CSenXmlServiceDescription*  iSenXmlServiceDescription;
        CSenIdentityProvider*       iProvider;
        RFs                         iFsSession;

		CActiveScheduler* iActiveScheduler;
        
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

#endif      // SENSERVICEMANAGERTESTER_H

// End of File
