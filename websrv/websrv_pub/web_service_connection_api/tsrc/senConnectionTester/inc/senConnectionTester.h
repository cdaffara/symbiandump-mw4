/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef SENCONNECTIONTESTER_H
#define SENCONNECTIONTESTER_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <MSenServiceConsumer.h>
#include <SenBaseFragment.h>
#include <SenBaseElement.h>
#include <SenXmlReader.h>
#include <SenServiceConnection.h>
#include <f32file.h>
#include <e32base.h> // for CActivef
#include <e32std.h>
#include <unistd.h>
#include <e32def.h>
#include <aknnotewrappers.h> 

//  INTERNAL INCLUDES
namespace
    {
    _LIT16(KSessionsFile,"c:\\private\\101f96f4\\SenSessions.xml");
    _LIT16(KIdentitiesFile,"c:\\private\\101f96f4\\senidentities.xml");
    }

enum TUT_CSenServiceConnection
    {
    ENotInitialized = 1,
    EConnectionNotReady,
    };
    
enum TMSCommands
    {
    EMSNMessengerRegister,
    EMSNMessengerInvite,
    EMSNMessengerSend,
    EMSNMessengerUnRegister,
    };

//  FORWARD DECLARATIONS
class CSenBaseFragment;
class CSenBaseElement;
class CSenSoapEnvelope;
class SenXmlUtils;
class CSenServiceConnection;
class CSenXmlReader;
class CSenDomFragment;
class CSenXmlServiceDescription;
class CSenServicePattern;
class CSenServiceManager;
class CSenSoapMessage;
class CSenSoapMessage2;
class CSenVtcpTransportProperties;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KsenConnectionTesterLogPath, "\\logs\\testframework\\senConnectionTester\\" ); 
// Log file
_LIT( KsenConnectionTesterLogFile, "senConnectionTester.txt" ); 
_LIT( KsenConnectionTesterLogFileWithTitle, "senConnectionTester_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CsenConnectionTester;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
typedef TInt (CsenConnectionTester::* TestFunction)(TTestResult&);
//extern ?data_type;

// CLASS DECLARATION

/**
*  CsenConnectionTester test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/

NONSHARABLE_CLASS(CsenConnectionTester) : public CScriptBase, public MSenServiceConsumer, public MSenAuthenticationProvider, public MSenExtendedConsumerInterface
    {
    public:
        enum TCallback
            {
            ECallbackNone = 0,
            ECallbackHandleMessage,
            ECallbackHandleError
            };
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CsenConnectionTester* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CsenConnectionTester();

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
        CsenConnectionTester( CTestModuleIf& aTestModuleIf );

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
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        void CsenConnectionTester::HandleMessageL(const TDesC8& aMessage);        
        void CsenConnectionTester::HandleErrorL(const int aErrorCode, const TDesC8& aError);
        void CsenConnectionTester::SetStatus(const TInt aStatus);

        void CsenConnectionTester::SetupL();

        void CsenConnectionTester::Teardown();

        void CsenConnectionTester::ErrorL(TInt aCode);

        TInt CsenConnectionTester::UT_CSenServiceConnection_RegisterIdentityProviderLL();

        TInt CsenConnectionTester::UT_CSenServiceConnection_NewL_ContractL();
        TInt CsenConnectionTester::UT_CSenServiceConnection_NewLC_ContractL();
        TInt CsenConnectionTester::UT_CSenServiceConnection_NewL_WS_I_DescriptionL();
        TInt CsenConnectionTester::UT_CSenServiceConnection_NewLC_WS_I_DescriptionL();
        TInt CsenConnectionTester::UT_CSenServiceConnection_NewL_ID_WSF_DescriptionL();
        TInt CsenConnectionTester::UT_CSenServiceConnection_NewLC_ID_WSF_DescriptionL();
        TInt CsenConnectionTester::UT_CSenServiceConnection_NewL_WS_I_PatternL();
        TInt CsenConnectionTester::UT_CSenServiceConnection_NewLC_WS_I_PatternL();
        TInt CsenConnectionTester::UT_CSenServiceConnection_NewL_ID_WSF_PatternL();
        TInt CsenConnectionTester::UT_CSenServiceConnection_NewLC_ID_WSF_PatternL();

        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_NoErrorL();
        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_CancelL();
        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_ConNotInitialL(  );
        
        TInt CsenConnectionTester::UT_CSenServiceConnection_SendMultipleMessagesL(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_Get_Post1L(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_Get_Post2L(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_Proxy1L(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_Proxy2L(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_Pattern_IAP1L(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_Pattern_IAP2L(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_Pattern_IAP3L(  );

        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_ContentType1L(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_ContentType2L(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_UserAgent1L(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_UserAgent2L(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_MessageSoapActionL(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_SoapAction1L(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_SoapAction2L(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_CustomHttpHeaderL(  );

        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_ConBusyL(  );

        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_ConInitializingL(  );

        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_ConExpiredL(  );

        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_ErrSubmitL(  );

        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_ErrNoMemoryL(  );

        TInt CsenConnectionTester::UT_CSenServiceConnection_SubmitLL();

        TInt CsenConnectionTester::UT_CSenServiceConnection_IsReadyL();

        TInt CsenConnectionTester::UT_CSenServiceConnection_HasFacetLL();

        TInt CsenConnectionTester::UT_CSenServiceConnection_CompleteServerMessagesOnOffL();

        TInt CsenConnectionTester::UT_CSenServiceConnection_SendL_1L();

        TInt CsenConnectionTester::UT_CSenServiceConnection_SubmitL_1L();

        TInt CsenConnectionTester::UT_CSenServiceConnection_StartTransactionL();

        TInt CsenConnectionTester::UT_CSenServiceConnection_TransactionCompletedL();
        
        TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries1(  );
        
        TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries2(  );
        
        TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries3(  );
        
        TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries4(  );
        
        TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries5(  );
        
        TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries6(  );
        
        TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries7(  );
        
        TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries8(  );
        
        TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Boundaries9(  );

        TInt CsenConnectionTester::UT_CSenServiceConnection_TP_SecDialog();     
        
        TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Omitted_True();     

        TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Omitted_False();           
        
        TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Removed();     
        
        TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Omit_Rem();
        TInt CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_L();
        TInt CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_with_Properties_L();
        TInt CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_MTOM_20KBL();
        TInt CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_MTOM_200KBL();
        TInt CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_MTOM_Less1MBL();
        TInt CsenConnectionTester::UT_CSenServiceConnection_SubmitL_LibXml2_MTOM_More1MBL();
        TInt CsenConnectionTester::UT_CSenServiceConnection_TestMessengerL(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_TestMessenger2L(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_TestMessenger3L(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_TestMessenger4L(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_TestMessenger5L(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_TestMessenger6L(  );
        TInt CsenConnectionTester::UT_CSenServiceConnection_TestMessenger7L(  );
        
        TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Custom_Header(const TDesC8& KCustomHttpHeader,
                                                       const TDesC8& KCustomHttpHeaderValue);
                                    
        TInt CsenConnectionTester::UT_CSenServiceConnection_TP_Send(const TDesC8& aLogFileName,
                                              const TDesC8& aCustomHttpHeader,
                                              const TDesC8& aCustomHttpHeaderValue,
                                              CSenServiceConnection* iServiceConnection);
                                              
        TInt CsenConnectionTester::UT_CSenServiceConnection_TP_CheckLogs(const TDesC& aLogFilePath,
                                                   const TDesC8& aCustomHttpHeader,
                                                   const TDesC8& aCustomHttpHeaderValue);                                    
                                                   
        TUint32 CsenConnectionTester::SearchIAPIdByNameL( const TDesC& aIAPName );
        
        CSenSoapMessage* CsenConnectionTester::CreateAuthRequestLC();
        
        CSenSoapMessage2* CsenConnectionTester::CreateNewAuthRequestLC();
        
        HBufC8* CsenConnectionTester::ReadFileL(const TDesC& aFilename);
        
        TInt CsenConnectionTester::Find(const TDesC8& aDestination, const TDesC8& aDescToFind, TInt aStartingPos);
        TBool CsenConnectionTester::ReplaceAll(TPtr8 aDestination, const TDesC8& aFrom, const TDesC8& aTo);
        TBool CsenConnectionTester::DecodeHttpCharactersL(const TDesC8& aOriginal, HBufC8*& aDecoded);
        
        HBufC8* CsenConnectionTester::HttpMethodValueFromXmlLC(const TDesC8& aXml);
        
        HBufC8* CsenConnectionTester::HttpPropertyValueFromXmlLC(const TDesC8& aXml,
                                           const TDesC8& aPropertyName);
                                           
        void CsenConnectionTester::StartActiveScheduler(TInt aNumberOfSentMessages);
        void CsenConnectionTester::StopActiveScheduler();
        
        CSenVtcpTransportProperties* CsenConnectionTester::CreateMSTransportPropertiesL(TMSCommands aCommand);
        CSenSoapMessage2* CsenConnectionTester::CreateMSSoapMessage(TMSCommands aCommand);
        void CsenConnectionTester::RegisterSTSL();
        void CsenConnectionTester::RegisterSTSInvalidPasswordL();
        void CsenConnectionTester::UnregisterSTSL();
        void CsenConnectionTester::SetPolicyL(CSenXmlServiceDescription* pServDesc, const TDesC8& pName,
                                                              const TDesC8& pValue);
        void CsenConnectionTester::CreateConnectionToMessengerServiceL();
        void CsenConnectionTester::CreateConnectionToMessengerServiceWithoutCallbackL();
        
        void CsenConnectionTester::AddDeviceOptionsHeader2L(CSenSoapMessage2& aMessage);
        void CsenConnectionTester::AddApplicationHeader2L(CSenSoapMessage2& aMessage);
        TAny* CsenConnectionTester::GetInterfaceByUid( TUid aUID );
        const CSenIdentityProvider* IdentityProviderL() const ;

    private:    // Data
        
        CSenXmlServiceDescription*  iSenXmlServiceDescription;
        CSenXmlServiceDescription*  iSession;
        CSenServiceConnection*      iServiceConnection;
        CSenServiceManager*         iManager;
        CSenXmlReader*              iXmlReader;
        
        TInt                        iTransactionId;
        TInt                        iConnectionState;
        TBool                       iInitialized;
        TBool                       iReady;
        
        TCallback                   iCallback;
        HBufC8*                     ipMessage;
        
        TInt                        iNumberOfPendingOps;
        HBufC8*                     ipFrameworkID;
        
        TInt                        iErrorCode;
  
        CSenIdentityProvider*       ipProviderForAuthProvider;
        TInt                        iAuthInfoAskCounter;
        


    };

#endif      // SENCONNECTIONTESTER_H

// End of File
