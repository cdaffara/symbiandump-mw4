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
* Description:        SenFragment test module.
*
*/


#ifndef __SENSERVICECONNECTION_H__
#define __SENSERVICECONNECTION_H__

//#define __ENABLE_ALR__

//  EXTERNAL INCLUDES
#include <StifTestModule.h>
#include <StifLogger.h>

#include <MSenServiceConsumer.h>
#include <SenBaseFragment.h>
#include <SenBaseElement.h>
#include <SenXmlReader.h>
#include <SenServiceConnection.h>

#include <f32file.h>
#include <e32base.h> // for CActive
#include <e32std.h>
#include <unistd.h>
#include <aknnotewrappers.h> 
#include <SenIdentityProvider.h>

// MACROS
//#define ?macro ?macro_def
#define TEST_MODULE_VERSION_MAJOR 50
#define TEST_MODULE_VERSION_MINOR 9
#define TEST_MODULE_VERSION_BUILD 6

#ifdef __ENABLE_ALR__
	#include <comms-infras/cs_mobility_apiext.h>
	#include <senserviceconnectioninterfaces.h>
#endif //__ENABLE_ALR__

#include <msencobrandinginterface.h>

// Logging path
_LIT( KSenServiceConnectionLogPath, "\\logs\\testframework\\SenServiceConnectionBCTest\\" ); 
// Log file
_LIT( KSenServiceConnectionLogFile, "SenServiceConnectionBCTest.txt" ); 

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

/*
#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif
*/
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
class CSenServiceConnectionBCTest;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
// A typedef for function that does the actual testing,
// function is a type 
// TInt CSenFragmentBCTest::<NameOfFunction> ( TTestResult& aResult )
typedef TInt (CSenServiceConnectionBCTest::* TestFunction)(TTestResult&);

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



//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( CSenServiceConnectionBCTest )
#ifdef __ENABLE_ALR__
     : public CTestModuleBase, public MSenServiceConsumer, public MSenAuthenticationProvider, public MSenExtendedConsumerInterface, public MSenCoBrandingObserver, MSenHostletConsumer, public MMobilityProtocolResp
#else
     : public CTestModuleBase, public MSenServiceConsumer, public MSenAuthenticationProvider, public MSenExtendedConsumerInterface, public MSenCoBrandingObserver, MSenHostletConsumer
#endif //__ENABLE_ALR__     
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
        static CSenServiceConnectionBCTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CSenServiceConnectionBCTest();

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

    private:

        /**
        * C++ default constructor.
        */
        CSenServiceConnectionBCTest();

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
        
           // MService Consumer implementation
        void HandleMessageL(const TDesC8& aMessage);        
        void HandleErrorL(const int aErrorCode, const TDesC8& aError);
        void SetStatus(const TInt aStatus);

        void SetupL();
        void Teardown();
        void ErrorL(TInt aCode);

        TInt UT_CSenServiceConnection_RegisterIdentityProviderLL( TTestResult& aResult );

        TInt UT_CSenServiceConnection_NewL_ContractL( TTestResult& aResult );
        TInt UT_CSenServiceConnection_NewLC_ContractL( TTestResult& aResult );
        TInt UT_CSenServiceConnection_NewL_WS_I_DescriptionL( TTestResult& aResult );
        TInt UT_CSenServiceConnection_NewLC_WS_I_DescriptionL( TTestResult& aResult );
        TInt UT_CSenServiceConnection_NewL_ID_WSF_DescriptionL( TTestResult& aResult );
        TInt UT_CSenServiceConnection_NewLC_ID_WSF_DescriptionL( TTestResult& aResult );
        TInt UT_CSenServiceConnection_NewL_WS_I_PatternL( TTestResult& aResult );
        TInt UT_CSenServiceConnection_NewLC_WS_I_PatternL( TTestResult& aResult );
        TInt UT_CSenServiceConnection_NewL_ID_WSF_PatternL( TTestResult& aResult );
        TInt UT_CSenServiceConnection_NewLC_ID_WSF_PatternL( TTestResult& aResult );

        TInt UT_CSenServiceConnection_NewL_ContractL1( TTestResult& aResult );
        TInt UT_CSenServiceConnection_NewLC_ContractL1( TTestResult& aResult );
        TInt UT_CSenServiceConnection_NewL_WS_I_DescriptionL1( TTestResult& aResult );
        TInt UT_CSenServiceConnection_NewLC_WS_I_DescriptionL1( TTestResult& aResult );
        TInt UT_CSenServiceConnection_NewL_WS_I_PatternL1( TTestResult& aResult );
        TInt UT_CSenServiceConnection_NewLC_WS_I_PatternL1( TTestResult& aResult );
        TInt UT_CSenServiceConnectionSnapIdL( TTestResult& aResult );
        TInt UT_CSenServiceConnectionSetSnapIdL( TTestResult& aResult );

    	TInt UT_CSenServiceConnection_SendL_RFileL( TTestResult& aResult );
    	TInt UT_CSenServiceConnection_SubmitL_RFileL( TTestResult& aResult );
    	TInt UT_CSenServiceConnection_SendL_RFileL_IPCBigrequest( TTestResult& aResult );
    	TInt UT_CSenServiceConnection_SubmitL_RFileL_IPCBigrequest( TTestResult& aResult );
    	TInt UT_CSenServiceConnection_SubmitL_RFileL_MTOM( TTestResult& aResult );
    
        TInt UT_CSenServiceConnection_SendL_NoErrorL( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SendL_CancelL( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SendL_ConNotInitialL( TTestResult& aResult );
        
        TInt UT_CSenServiceConnection_SendMultipleMessagesL( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SendL_Get_Post1L( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SendL_Get_Post2L( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SendL_Proxy1L( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SendL_Proxy2L( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SendL_Pattern_IAP1L( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SendL_Pattern_IAP2L( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SendL_Pattern_IAP3L( TTestResult& aResult );

        TInt UT_CSenServiceConnection_SendL_ContentType1L( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SendL_ContentType2L( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SendL_UserAgent1L( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SendL_UserAgent2L( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SendL_MessageSoapActionL( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SendL_SoapAction1L( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SendL_SoapAction2L( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SendL_CustomHttpHeaderL( TTestResult& aResult );

        TInt UT_CSenServiceConnection_SendL_ConBusyL( TTestResult& aResult );

        TInt UT_CSenServiceConnection_SendL_ConInitializingL( TTestResult& aResult );

        TInt UT_CSenServiceConnection_SendL_ConExpiredL( TTestResult& aResult );

        TInt UT_CSenServiceConnection_SendL_ErrSubmitL( TTestResult& aResult );

        TInt UT_CSenServiceConnection_SendL_ErrNoMemoryL( TTestResult& aResult );

        TInt UT_CSenServiceConnection_SubmitLL( TTestResult& aResult );

        TInt UT_CSenServiceConnection_IsReadyL( TTestResult& aResult );

        TInt UT_CSenServiceConnection_HasFacetLL( TTestResult& aResult );

        TInt UT_CSenServiceConnection_CompleteServerMessagesOnOffL( TTestResult& aResult );

        TInt UT_CSenServiceConnection_SendL_1L( TTestResult& aResult );

        TInt UT_CSenServiceConnection_SubmitL_1L( TTestResult& aResult );

        TInt UT_CSenServiceConnection_StartTransactionL( TTestResult& aResult );

        TInt UT_CSenServiceConnection_TransactionCompletedL( TTestResult& aResult );
        
        TInt UT_CSenServiceConnection_TP_Boundaries1( TTestResult& aResult );
        
        TInt UT_CSenServiceConnection_TP_Boundaries2( TTestResult& aResult );
        
        TInt UT_CSenServiceConnection_TP_Boundaries3( TTestResult& aResult );
        
        TInt UT_CSenServiceConnection_TP_Boundaries4( TTestResult& aResult );
        
        TInt UT_CSenServiceConnection_TP_Boundaries5( TTestResult& aResult );
        
        TInt UT_CSenServiceConnection_TP_Boundaries6( TTestResult& aResult );
        
        TInt UT_CSenServiceConnection_TP_Boundaries7( TTestResult& aResult );
        
        TInt UT_CSenServiceConnection_TP_Boundaries8( TTestResult& aResult );
        
        TInt UT_CSenServiceConnection_TP_Boundaries9( TTestResult& aResult );

        TInt UT_CSenServiceConnection_TP_SecDialog( TTestResult& aResult );     
        
        TInt UT_CSenServiceConnection_TP_Omitted_True( TTestResult& aResult );     

        TInt UT_CSenServiceConnection_TP_Omitted_False( TTestResult& aResult );           
        
        TInt UT_CSenServiceConnection_TP_Removed( TTestResult& aResult );     
        
        TInt UT_CSenServiceConnection_TP_Omit_Rem( TTestResult& aResult );
        
        
        TInt UT_CSenServiceConnection_SubmitL_LibXml2_L( TTestResult& aResult );
        
        TInt UT_CSenServiceConnection_SubmitL_LibXml2_with_Properties_L( TTestResult& aResult );
        
        TInt UT_CSenServiceConnection_SubmitL_LibXml2_MTOM_20KBL( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SubmitL_LibXml2_MTOM_200KBL( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SubmitL_LibXml2_MTOM_Less1MBL( TTestResult& aResult );
        TInt UT_CSenServiceConnection_SubmitL_LibXml2_MTOM_More1MBL( TTestResult& aResult );

        TInt UT_CSenServiceConnection_TestMessengerL( TTestResult& aResult );
        TInt UT_CSenServiceConnection_TestMessenger2L( TTestResult& aResult );
        TInt UT_CSenServiceConnection_TestMessenger3L( TTestResult& aResult );
        TInt UT_CSenServiceConnection_TestMessenger4L( TTestResult& aResult );
        TInt UT_CSenServiceConnection_TestMessenger5L( TTestResult& aResult );
        TInt UT_CSenServiceConnection_TestMessenger6L( TTestResult& aResult );
        TInt UT_CSenServiceConnection_TestMessenger7L( TTestResult& aResult );
        
        TInt UT_CSenServiceConnection_TP_Custom_Header(const TDesC8& KCustomHttpHeader,
                                                       const TDesC8& KCustomHttpHeaderValue);
                                    
        TInt UT_CSenServiceConnection_TP_Send(const TDesC8& aLogFileName,
                                              const TDesC8& aCustomHttpHeader,
                                              const TDesC8& aCustomHttpHeaderValue,
                                              CSenServiceConnection* iServiceConnection);
                                              
        TInt UT_CSenServiceConnection_TP_CheckLogs(const TDesC& aLogFilePath,
                                                   const TDesC8& aCustomHttpHeader,
                                                   const TDesC8& aCustomHttpHeaderValue);                                    
                                                   
        TUint32 SearchIAPIdByNameL( const TDesC& aIAPName );
        
        CSenSoapMessage* CreateAuthRequestLC();
        
        CSenSoapMessage2* CreateNewAuthRequestLC();
        
        HBufC8* ReadFileL(const TDesC& aFilename);
        
        TInt Find(const TDesC8& aDestination, const TDesC8& aDescToFind, TInt aStartingPos);
        TBool ReplaceAll(TPtr8 aDestination, const TDesC8& aFrom, const TDesC8& aTo);
        TBool DecodeHttpCharactersL(const TDesC8& aOriginal, HBufC8*& aDecoded);
        
        HBufC8* HttpMethodValueFromXmlLC(const TDesC8& aXml);
        
        HBufC8* HttpPropertyValueFromXmlLC(const TDesC8& aXml,
                                           const TDesC8& aPropertyName);
                                           
        TInt StartActiveScheduler(TInt aNumberOfSentMessages);
        TInt StopActiveScheduler();
        
        CSenVtcpTransportProperties* CreateMSTransportPropertiesL(TMSCommands aCommand);
        CSenSoapMessage2* CreateMSSoapMessage(TMSCommands aCommand);
        TInt RegisterSTSL();
        TInt RegisterSTSInvalidPasswordL();
        TInt UnregisterSTSL( );
        TInt SetPolicyL(CSenXmlServiceDescription* pServDesc, const TDesC8& pName,
                                                              const TDesC8& pValue);
		const CSenIdentityProvider* IdentityProviderL() const ;
        TInt CreateConnectionToMessengerServiceL();
        TInt CreateConnectionToMessengerServiceWithoutCallbackL();
        
        TInt AddDeviceOptionsHeader2L(CSenSoapMessage2& aMessage);
        TInt AddApplicationHeader2L(CSenSoapMessage2& aMessage);
        
        TInt SendL( TTestResult& aResult );                                                                   
        TInt SubmitL( TTestResult& aResult );                                                                         
        TInt IsReady( TTestResult& aResult ) ;                                                                        
        TInt HasFacetL( TTestResult& aResult );                                                                       
                                                             
        TInt CompleteServerMessagesOnOff( TTestResult& aResult ) ;                                                    
        TInt Send2L( TTestResult& aResult ) ;                                                                         
        TInt Submit2L( TTestResult& aResult ) ;                                                                       
        TInt ServiceDescriptionL( TTestResult& aResult ) ;                                                            
        TInt StartTransaction( TTestResult& aResult ) ;                                                               
        TInt TransactionCompleted( TTestResult& aResult ) ;                                                           
        TInt TxnId( TTestResult& aResult ) ;                                                                          
        TInt SetTransportPropertiesL( TTestResult& aResult ) ;                                                        
        TInt TransportPropertiesL( TTestResult& aResult ) ;                                                           
        TInt Send3L( TTestResult& aResult ) ;                                                                         
        TInt Submit3L( TTestResult& aResult ) ;                                                                       
        TInt Send4L( TTestResult& aResult ) ;                                                                         
        TInt Submit4L( TTestResult& aResult ) ;                                                                       
        TInt Send5L( TTestResult& aResult ) ;                                                                         
        TInt Submit5L( TTestResult& aResult ) ;                                                                       
        TInt Response( TTestResult& aResult ) ;                                                                       
        TInt CancelTransaction( TTestResult& aResult ) ;                                                              
        TInt IdentityProvider1L( TTestResult& aResult ) ;                                                              
        TInt Identifier( TTestResult& aResult ) ;                                                                     
        TInt Name( TTestResult& aResult );
        TInt Value( TTestResult& aResult );
        TInt IntValue( TTestResult& aResult );
        TInt BoolValue( TTestResult& aResult );
        TInt Type( TTestResult& aResult );
        TInt ValueTokensL( TTestResult& aResult );
        TInt IdentityProvider2L( TTestResult& aResult ) ;
        TInt Username1L( TTestResult& aResult ) ;
        TInt Password1L( TTestResult& aResult ) ;
        TInt ExtendedInterface1L( TTestResult& aResult ) ;                                  
        TInt TransferProgress(TTestResult& aResult );
        TInt HandleMeassage(TTestResult& aResult);
        TInt HandleErrorL(TTestResult& aResult);
        TInt SetStatus(TTestResult& aResult);
        TAny* GetInterfaceByUid( TUid aUID );
        		
        #ifdef __ENABLE_ALR__
            TInt ALRL(TTestResult& aResult);
           
            //mobility methods
            virtual void PreferredCarrierAvailable( TAccessPointInfo aOldAPInfo,
                                                         TAccessPointInfo aNewAPInfo,
                                                         TBool aIsUpgrade,
                                                         TBool aIsSeamless ) ;
            virtual void NewCarrierActive( TAccessPointInfo aNewAPInfo, TBool aIsSeamless ) ;
            
            virtual void Error(TInt aError) ;
        #endif //__ENABLE_ALR__
            TInt CoBrandL(TTestResult& aResult);
            virtual TBool OnGetBrandIdL(  const TDesC8& aBrandIdListAsXmlSnippet, RBuf8& aSelectedBrandId);
            TInt HostletConsumerL(TTestResult& aResult);
            virtual void SetConnectionId( TInt aConnectionId );
            TInt DataTrafficDetailsL(TTestResult& aResult);
    private:    // Data
        
        CSenXmlServiceDescription*  iSenXmlServiceDescription;
        CSenXmlServiceDescription*  iSession;
        CSenServiceConnection*      iServiceConnection;
        CSenSoapMessage* iSOAPMessage;
        #ifdef __ENABLE_ALR__       
            MSenAlrServiceConnection* iALRSC;
        #endif //__ENABLE_ALR__
        TBool iUseALR;
        TInt iAlrCallBackRetryCount;
        TBool iCoBrandingCallbackOption;
        TBool iHostletConsumerOption;
        TBool iGetDataTrafficDetails;
        TBool isNewCarrierAccepted;
        TBool isMigratedToPrefferedCarrier;
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
        RFs							iFsSessionHandle;
        RFile 						iRFileHandle;
        
    private:    // Data
        // Pointer to test (function) to be executed
        TestFunction iMethod;

        // Pointer to logger
        CStifLogger * iLog; 
		CActiveScheduler* iActiveScheduler;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;
        
        
    };

#endif      //  __SENSERVICECONNECTION_H__

// End of file
