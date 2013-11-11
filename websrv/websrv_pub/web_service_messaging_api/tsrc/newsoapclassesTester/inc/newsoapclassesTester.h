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









#ifndef NEWSOAPCLASSESTESTER_H
#define NEWSOAPCLASSESTESTER_H

//  INCLUDES
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
_LIT( KnewsoapclassesTesterLogPath, "\\logs\\testframework\\newsoapclassesTester\\" ); 
// Log file
_LIT( KnewsoapclassesTesterLogFile, "newsoapclassesTester.txt" ); 
_LIT( KnewsoapclassesTesterLogFileWithTitle, "newsoapclassesTester_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CSenSoapFault2;
class CSenSoapEnvelope2;
class CSenSoapMessage2;
class CSenWsSecurityHeader2;

class CnewsoapclassesTester;
// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
typedef TInt (CnewsoapclassesTester::* TestFunction)(CStifItemParser&);

// CLASS DECLARATION

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
NONSHARABLE_CLASS(CnewsoapclassesTester) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CnewsoapclassesTester* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CnewsoapclassesTester();

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
        CnewsoapclassesTester( CTestModuleIf& aTestModuleIf );

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
        void SetupL();
        void Teardown();

        /**
        * Test methods are listed below. 
        */

         TInt MT_CSenSoapEnvelope2_NewLL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_NewLCL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_NewL_1L(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_NewLC_1L(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_NewL_2L(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_NewLC_2L(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_TypeL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_DirectionL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_TxnIdL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_CloneL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_SoapVersionL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_SetContextL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_ContextL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_SetPropertiesL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_PropertiesL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_IsSafeToCastL(CStifItemParser& aItem); 
         TInt MT_CSenSoapEnvelope2_SetBodyLL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_SetBodyL_1L(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_BodyLL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_HeaderLL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_AddHeaderLL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_BodyAsStringLL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_IsFaultL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_DetachFaultLL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_FaultLL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_SetSoapActionLL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_SoapActionL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_HasHeaderL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_HasBodyL(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_Parse1L(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_Parse2L(CStifItemParser& aItem);
         TInt MT_CSenSoapEnvelope2_Parse3L(CStifItemParser& aItem);
         TInt MT_CSenSoapFault2_NewLL(CStifItemParser& aItem);
         TInt MT_CSenSoapFault2_NewL_1L(CStifItemParser& aItem);
         TInt MT_CSenSoapFault2_NewL_2L(CStifItemParser& aItem);
         TInt MT_CSenSoapFault2_FaultCodeL(CStifItemParser& aItem);
         TInt MT_CSenSoapFault2_FaultSubcodeL(CStifItemParser& aItem);
         TInt MT_CSenSoapFault2_FaultStringL(CStifItemParser& aItem);
         TInt MT_CSenSoapFault2_FaultActorL(CStifItemParser& aItem);
         TInt MT_CSenSoapFault2_DetailL(CStifItemParser& aItem);
         TInt MT_CSenSoapMessage2_NewLL(CStifItemParser& aItem);
         TInt MT_CSenSoapMessage2_NewLCL(CStifItemParser& aItem);
         TInt MT_CSenSoapMessage2_NewL_1L(CStifItemParser& aItem);
         TInt MT_CSenSoapMessage2_NewLC_1L(CStifItemParser& aItem);
         TInt MT_CSenSoapMessage2_NewL_2L(CStifItemParser& aItem);
         TInt MT_CSenSoapMessage2_NewLC_2L(CStifItemParser& aItem);
         TInt MT_CSenSoapMessage2_NewL_3L(CStifItemParser& aItem);
         TInt MT_CSenSoapMessage2_NewLC_3L(CStifItemParser& aItem);
         TInt MT_CSenSoapMessage2_NewL_4L(CStifItemParser& aItem);
         TInt MT_CSenSoapMessage2_NewLC_4L(CStifItemParser& aItem);
         TInt MT_CSenSoapMessage2_NewL_5L(CStifItemParser& aItem);
         TInt MT_CSenSoapMessage2_NewLC_5L(CStifItemParser& aItem);
         TInt MT_CSenSoapMessage2_NewL_6L(CStifItemParser& aItem);
         TInt MT_CSenSoapMessage2_NewLC_6L(CStifItemParser& aItem);
         TInt MT_CSenSoapMessage2_TypeL(CStifItemParser& aItem);
		 TInt MT_CSenSoapMessage2_CloneL(CStifItemParser& aItem);
		 TInt MT_CSenSoapMessage2_Parse1L(CStifItemParser& aItem);
         TInt MT_CSenSoapMessage2_SetSecurityHeaderLL(CStifItemParser& aItem);
         TInt MT_CSenSoapMessage2_AddSecurityTokenLL(CStifItemParser& aItem);
         TInt MT_CSenWsSecurityHeader2_NewLL(CStifItemParser& aItem);
         TInt MT_CSenWsSecurityHeader2_NewLCL(CStifItemParser& aItem);
         TInt MT_CSenWsSecurityHeader2_NewL_1L(CStifItemParser& aItem);
         TInt MT_CSenWsSecurityHeader2_NewLC_1L(CStifItemParser& aItem);
         TInt MT_CSenWsSecurityHeader2_NewL_2L(CStifItemParser& aItem);
         TInt MT_CSenWsSecurityHeader2_NewLC_2L(CStifItemParser& aItem);
         TInt MT_CSenWsSecurityHeader2_UsernameTokenLL(CStifItemParser& aItem);
         TInt MT_CSenWsSecurityHeader2_UsernameTokenL_1L(CStifItemParser& aItem);
         TInt MT_CSenWsSecurityHeader2_UsernameTokenL_2L(CStifItemParser& aItem);
         TInt MT_CSenWsSecurityHeader2_UsernameTokenL_3L(CStifItemParser& aItem);
         TInt MT_CSenWsSecurityHeader2_UsernameTokenL_4L(CStifItemParser& aItem);
         TInt MT_CSenWsSecurityHeader2_TimestampLL(CStifItemParser& aItem);
         TInt MT_CSenWsSecurityHeader2_TimestampL_1L(CStifItemParser& aItem);
         TInt MT_CSenWsSecurityHeader2_XmlNsL(CStifItemParser& aItem);
         TInt MT_CSenWsSecurityHeader2_XmlNsPrefixL(CStifItemParser& aItem);
         
        
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

#endif      // NEWSOAPCLASSESTESTER_H

// End of File
