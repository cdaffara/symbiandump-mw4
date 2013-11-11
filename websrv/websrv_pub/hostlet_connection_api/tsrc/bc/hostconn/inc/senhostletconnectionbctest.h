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
* Description:          SenHostletConnection test module.
*
*/








#ifndef __SENHOSTLETCONNECTION_H__
#define __SENHOSTLETCONNECTION_H__

//  EXTERNAL INCLUDES
// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>
#include <MSenHostlet.h>
#include <f32file.h>


#include <SenXmlServiceDescription.h>
#include <SenHostletConnection.h>


#include <SenBaseElement.h>
#include <SenSoapMessage.h>
#include <SenDateUtils.h>

//  INTERNAL INCLUDES

//#include <e32def.h>
//#ifndef NONSHARABLE_CLASS
//    #define NONSHARABLE_CLASS(x) class x
//#endif
// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_MODULE_VERSION_MAJOR 50
#define TEST_MODULE_VERSION_MINOR 9
#define TEST_MODULE_VERSION_BUILD 6

// Logging path
_LIT( KSenHostletConnection_stifLogPath, "\\logs\\testframework\\SenHostletConnection_stif\\" ); 
// Log file
_LIT( KSenHostletConnection_stifLogFile, "SenHostletConnection_stif.txt" ); 


#define GETPTR &
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}
#define OOM_ENTRY(str,func,a,b,c) {_S(str), GETPTR func,a,b,c}
#define OOM_FUNCENTRY(func,a,b,c) {_S(#func), GETPTR func,a,b,c}

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CSenHostletConnection;
class CSenHostletConnectionBCTest;
class CTestConsumer;
class CTestHostlet;


// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
// A typedef for function that does the actual testing,
// function is a type 
// TInt CSenHostletConnectionBCTest::<NameOfFunction> ( TTestResult& aResult )
typedef TInt (CSenHostletConnectionBCTest::* TestFunction)(TTestResult&);

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
//  CLASS DEFINITIONS

// FORWARD DECLARATIONS
class CActiveConsumerTestTask;
//class CSenHostletConnectionBCTest;

NONSHARABLE_CLASS( TConsumerTestTask )
    {
    public:
        enum TJob
            {
            ECreateConnAndSubmit = 0,
            ESubmit,
            EActiveSubmit
            };
    
    
        inline TConsumerTestTask(CSenServiceConnection& aConnection, const TDesC8& aWsRequest, const TDesC8& aFrameworkId, TJob aJob, HBufC8*& apResult)
            : iConnection(aConnection),
            iRequest(aWsRequest),
            iFrameworkId(aFrameworkId),
            iJob(aJob),
            iStatusCode(KErrNone),
            ipNotOwnedData(apResult),
            ipSoapMsg(NULL)
            {
            }
            
        void Execute();


    public: // Data (which is NOT OWNED, as this is a T-class)...      
        CSenServiceConnection&      iConnection;
        TPtrC8                      iRequest;
        TPtrC8                      iFrameworkId;
        TJob                        iJob;
        TInt                        iStatusCode;
        HBufC8*&                    ipNotOwnedData;
        CActiveConsumerTestTask*    ipActive;
        CSenHostletConnectionBCTest*       ipOwner;
        TPtrC8                      iProperties;
        TPtrC8                      iEndpoint;
        CSenSoapMessage*            ipSoapMsg; // ... never & none owned.
    };


NONSHARABLE_CLASS( CActiveConsumerTestTask ) : public CActive
    {
    friend class CSenHostletConnectionBCTest;
    
    public:
        enum TStatus
            {
            EInvokeService = 1,
            EDeliverResponse
            };    

        static CActiveConsumerTestTask* NewL(TConsumerTestTask& aTask, const TDesC& aThreadName, CSenHostletConnectionBCTest& aOwner);
        static CActiveConsumerTestTask* NewLC(TConsumerTestTask& aTask, const TDesC& aThreadName, CSenHostletConnectionBCTest& aOwner);
        
        ~CActiveConsumerTestTask();
        
        void RunL();
        void DoCancel();

    private:
        void ConstructL(const TDesC& aThreadName);
        CActiveConsumerTestTask(TConsumerTestTask& aTask, CSenHostletConnectionBCTest& aOwner);

    public: // Data
        TThreadId               iOwnerThreadId;


    private: // Data
        TConsumerTestTask&      iTask;
        RThread                 iTaskThread;
        CSenHostletConnectionBCTest&   iOwner;
    };

// CLASS DECLARATION

/**
*  This a SenServiceManager_stif class.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CSenHostletConnectionBCTest) : public CTestModuleBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSenHostletConnectionBCTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CSenHostletConnectionBCTest();

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
        *       SenServiceManager_stif. It is called once for every instance of 
        *       TestModuleSenServiceManager_stif after its creation.
        * @since ?Series60_version
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first 
        *               created instance of SenServiceManager_stif.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
        *   from SenServiceManager_stif. 
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
         void StopActiveScheduler();
         void StartActiveScheduler(TInt aNumberOfExpectedOperationsPriorStop);
          				/**
         * Method used to log version of test module
         */
        void SendTestModuleVersion();
                              

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
        CSenHostletConnectionBCTest();

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


	    friend class CTestConsumer;
    	friend class CTestHostlet;
    

    public: // Public data => OK in testers, no benefit gained from getters => in real apps, these should be private:
        CSenServiceConnection*      ipConnection;
        CSenServiceConnection*      ipConnection1;
        CSenHostletConnection*      ipHostletConnection;
        CSenHostletConnection*      ipHostletConnection1;
        CSenHostletConnection*		ipUsedHostletConnection;
        

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         static void UT_CSenHostletConnection_WSI_DescribeServiceL(CSenXmlServiceDescription&);
         static void UT_CSenHostletConnection_WSI_DescribeService_2L(CSenXmlServiceDescription&);
         static void UT_CSenHostletConnection_NewLL_SetStatus(CSenHostletConnectionBCTest* apTestSuite, TInt aStatus);
         static void UT_CSenHostletConnection_NewLL_HandleMessageL(CSenHostletConnectionBCTest* apTestSuite, const TDesC8& aMessage );
         static void UT_CSenHostletConnection_NewLL_HandleErrorL(CSenHostletConnectionBCTest* apTestSuite, const TInt aErrorCode, const TDesC8& aMessage );
         static TInt UT_CSenHostletConnection_NewLL_OnServiceL(CSenHostletConnectionBCTest* apTestSuite, MSenHostletRequest& aRequest, MSenHostletResponse& aResponse);
         static TInt UT_CSenHostletConnection_NewLL_OnServiceCompleteL(CSenHostletConnectionBCTest* apTestSuite, const TInt aTxnId, TInt aCompletionCode, const TDesC8& aDesc );
		 static TInt UT_CSenHostletConnection_NewLL_OnService1L(CSenHostletConnectionBCTest* apTestSuite, MSenHostletRequest& aRequest, MSenHostletResponse& aResponse);
         static TInt UT_CSenHostletConnection_NewLL_OnServiceComplete1L(CSenHostletConnectionBCTest* apTestSuite, const TInt aTxnId, TInt aCompletionCode, const TDesC8& aDesc );
		 static TInt UT_CSenHostletConnection_NewLL_OnService2L(CSenHostletConnectionBCTest* apTestSuite, MSenHostletRequest& aRequest, MSenHostletResponse& aResponse);
		 static TInt UT_CSenHostletConnection_NewLL_OnService3L(CSenHostletConnectionBCTest* apTestSuite, MSenHostletRequest& aRequest, MSenHostletResponse& aResponse);
         static TInt UT_CSenHostletConnection_NewLL_OnServiceComplete2L(CSenHostletConnectionBCTest* apTestSuite, const TInt aTxnId, TInt aCompletionCode, const TDesC8& aDesc );                        
         static void UT_CSenHostletConnection_NewLL_HandleError1L(CSenHostletConnectionBCTest* apTestSuite, const TInt aErrorCode, const TDesC8& aMessage );
         static TInt UT_CSenHostletConnection_RespondWithPropertiesL_OnServiceL(CSenHostletConnectionBCTest* apTestSuite, MSenHostletRequest& aRequest, MSenHostletResponse& aResponse);
		 CSenSoapMessage* CreateAuthRequestLC();
		         
         TInt UT_CSenHostletConnection_NewLL(TTestResult& aResult);
         
         TInt UT_CSenHostletConnection_NewLCL(TTestResult& aResult);
        
         TInt UT_CSenHostletConnection_NewLC_3InvokesL(TTestResult& aResult);
        
         TInt UT_CSenHostletConnection_RespondL1L(TTestResult& aResult);

         TInt UT_CSenHostletConnection_RespondL2L(TTestResult& aResult);
         
         TInt UT_CSenHostletConnection_RespondL3L(TTestResult& aResult);
         
         TInt UT_CSenHostletConnection_Negative_ResponseCodeL(TTestResult& aResult);
         
         TInt UT_CSenHostletConnection_HC_SelfDef_URIL(TTestResult& aResult);
         
         TInt UT_CSenHostletConnection_Delete_HCL(TTestResult& aResult);
         
         TInt UT_CSenHostletConnection_Two_Equal_UID3L(TTestResult& aResult);
         
         TInt UT_CSenHostletConnection_Two_Equal_EndpointsL(TTestResult& aResult);
         
         TInt UT_CSenHostletConnection_Two_HC_Two_SC1L(TTestResult& aResult);
         
         TInt UT_CSenHostletConnection_Two_HC_Two_SC2L(TTestResult& aResult);
         
         TInt UT_CSenHostletConnection_Two_HC_Two_SC3L(TTestResult& aResult);
         
         TInt UT_CSenHostletConnection_DeleteSC_before_HCRespondLL(TTestResult& aResult);
         
         TInt UT_CSenHostletConnection_RespondL_4multipleTimesL(TTestResult& aResult);
         
         TInt UT_CSenHostletConnection_SC_Cancel_After_RequestL(TTestResult& aResult);
         
         TInt UT_CSenHostletConnection_Set_Expose_FacetL(TTestResult& aResult);
         
         TInt UT_CSenHostletConnection_RespondWithPropertiesL(TTestResult& aResult);

         //TInt UT_CSenHostletConnection_CreateConnAndSubmit_Rest_MultiThreadL(TTestResult& aResult);

         TInt UT_CSenHostletConnection_CreateConnAndSubmit_WSI_MultiThreadL(TTestResult& aResult);

         TInt UT_CSenHostletConnection_Send_WSIL(TTestResult& aResult);

         TInt UT_CSenHostletConnection_DescEndpoint_Send_WSIL(TTestResult& aResult);

         TInt UT_CSenHostletConnection_2SC1HCL(TTestResult& aResult);

         TInt UT_CSenHostletConnection_Submit_Rest_MultiThreadL(TTestResult& aResult); // Just for debugging, won't pass

         TInt UT_CSenHostletConnection_Submit_Rest_ActiveL(TTestResult& aResult); // Just for debugging, won't pass
         
         // TInt UT_CSenHostletConnection_Submit_WSIL(TTestResult& aResult); // non-working version, just to keep note
         TInt UT_CSenHostletConnection_IdentifierL(TTestResult& aResult);
		 TInt UT_CSenHostletConnection_FrameworkIdL(TTestResult& aResult);
		 TInt UT_CSenHostletConnection_RequestUtf8L(TTestResult& aResult);
		 TInt UT_CSenHostletConnection_ThreadIdL(TTestResult& aResult);
		 TInt UT_CSenHostletConnection_ConsumerIdL(TTestResult& aResult);
		 TInt UT_CSenHostletConnection_RequestIdL(TTestResult& aResult);
         

    private:    // Data
    
        TInt                        iNumberOfPendingOps;
        TInt                        iRespondRetCode;
        TInt                        iNumOfReqs;
        TBool                       iWas_HandleErrorL_Accessed;
        TBool                       iWas_OnServiceL_Accessed;        
        TInt                        iTxnID;       
        
        CTestConsumer*              ipConsumer;        
        CTestConsumer*              ipConsumer1;        
        CTestHostlet*               ipHostlet;
		CTestHostlet*               ipHostlet1;   
		
		CActiveScheduler* iActiveScheduler;
		
        // Pointer to test (function) to be executed
        TestFunction iMethod;

        // Pointer to logger
        CStifLogger * iLog; 

        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

		// RThread                     iThread;
    };

#endif      //  __SENHOSTLETCONNECTION_H__

// End of file
