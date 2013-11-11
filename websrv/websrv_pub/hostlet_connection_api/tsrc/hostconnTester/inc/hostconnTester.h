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









#ifndef HOSTCONNTESTER_H
#define HOSTCONNTESTER_H

//  INCLUDES
#include <StifLogger.h>
#include <MSenHostlet.h>
#include <f32file.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <SenXmlServiceDescription.h>
#include <SenHostletConnection.h>


#include <SenBaseElement.h>
#include <SenSoapMessage.h>
#include <SenDateUtils.h>
#include <TestclassAssert.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KSenHostletConnection_stifLogPath, "\\logs\\testframework\\SenHostletConnection_stif\\" ); 
// Log file
_LIT( KSenHostletConnection_stifLogFile, "SenHostletConnection_stif.txt" ); 



// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CSenHostletConnection;
class CTestConsumer;
class CTestHostlet;
class ChostconnTester;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
typedef TInt (ChostconnTester::* TestFunction)(CStifItemParser&);

// CLASS DECLARATION

/**
*  ChostconnTester test class for STIF Test Framework TestScripter.
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
//  CLASS DEFINITIONS

// FORWARD DECLARATIONS
class CActiveConsumerTestTask;
//class ChostconnTester;

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
        ChostconnTester*       ipOwner;
        TPtrC8                      iProperties;
        TPtrC8                      iEndpoint;
        CSenSoapMessage*            ipSoapMsg; // ... never & none owned.
    };
NONSHARABLE_CLASS( CActiveConsumerTestTask ) : public CActive
    {
    friend class ChostconnTester;
    
    public:
        enum TStatus
            {
            EInvokeService = 1,
            EDeliverResponse
            };    

        static CActiveConsumerTestTask* NewL(TConsumerTestTask& aTask, const TDesC& aThreadName, ChostconnTester& aOwner);
        static CActiveConsumerTestTask* NewLC(TConsumerTestTask& aTask, const TDesC& aThreadName, ChostconnTester& aOwner);
        
        ~CActiveConsumerTestTask();
        
        void RunL();
        void DoCancel();

    private:
        void ConstructL(const TDesC& aThreadName);
        CActiveConsumerTestTask(TConsumerTestTask& aTask, ChostconnTester& aOwner);

    public: // Data
        TThreadId               iOwnerThreadId;


    private: // Data
        TConsumerTestTask&      iTask;
        RThread                 iTaskThread;
        ChostconnTester&   iOwner;
    };
NONSHARABLE_CLASS(ChostconnTester) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static ChostconnTester* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~ChostconnTester();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
        void StopActiveScheduler();
         void StartActiveScheduler(TInt aNumberOfExpectedOperationsPriorStop);

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
        ChostconnTester( CTestModuleIf& aTestModuleIf );

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
        

	    friend class CTestConsumer;
    	friend class CTestHostlet;
    public: // Public data => OK in testers, no benefit gained from getters => in real apps, these should be private:
        CSenServiceConnection*      ipConnection;
        CSenServiceConnection*      ipConnection1;
        CSenHostletConnection*      ipHostletConnection;
        CSenHostletConnection*      ipHostletConnection1;
        CSenHostletConnection*		ipUsedHostletConnection;
    private:    // New methods

         static void UT_CSenHostletConnection_WSI_DescribeServiceL(CSenXmlServiceDescription&);
         static void UT_CSenHostletConnection_WSI_DescribeService_2L(CSenXmlServiceDescription&);
         static void UT_CSenHostletConnection_NewLL_SetStatus(ChostconnTester* apTestSuite, TInt aStatus);
         static void UT_CSenHostletConnection_NewLL_HandleMessageL(ChostconnTester* apTestSuite, const TDesC8& aMessage );
         static void UT_CSenHostletConnection_NewLL_HandleErrorL(ChostconnTester* apTestSuite, const TInt aErrorCode, const TDesC8& aMessage );
         static TInt UT_CSenHostletConnection_NewLL_OnServiceL(ChostconnTester* apTestSuite, MSenHostletRequest& aRequest, MSenHostletResponse& aResponse);
         static TInt UT_CSenHostletConnection_NewLL_OnServiceCompleteL(ChostconnTester* apTestSuite, const TInt aTxnId, TInt aCompletionCode, const TDesC8& aDesc );
				 static TInt UT_CSenHostletConnection_NewLL_OnService1L(ChostconnTester* apTestSuite, MSenHostletRequest& aRequest, MSenHostletResponse& aResponse);
         static TInt UT_CSenHostletConnection_NewLL_OnServiceComplete1L(ChostconnTester* apTestSuite, const TInt aTxnId, TInt aCompletionCode, const TDesC8& aDesc );
				 static TInt UT_CSenHostletConnection_NewLL_OnService2L(ChostconnTester* apTestSuite, MSenHostletRequest& aRequest, MSenHostletResponse& aResponse);
		 		 static TInt UT_CSenHostletConnection_NewLL_OnService3L(ChostconnTester* apTestSuite, MSenHostletRequest& aRequest, MSenHostletResponse& aResponse);
         static TInt UT_CSenHostletConnection_NewLL_OnServiceComplete2L(ChostconnTester* apTestSuite, const TInt aTxnId, TInt aCompletionCode, const TDesC8& aDesc );                        
         static void UT_CSenHostletConnection_NewLL_HandleError1L(ChostconnTester* apTestSuite, const TInt aErrorCode, const TDesC8& aMessage );
         static TInt UT_CSenHostletConnection_RespondWithPropertiesL_OnServiceL(ChostconnTester* apTestSuite, MSenHostletRequest& aRequest, MSenHostletResponse& aResponse);
		 		 CSenSoapMessage* CreateAuthRequestLC();

         TInt UT_CSenHostletConnection_NewLL(CStifItemParser& aItem);
         
         TInt UT_CSenHostletConnection_NewLCL(CStifItemParser& aItem);
        
         TInt UT_CSenHostletConnection_NewLC_3InvokesL(CStifItemParser& aItem);
        
         TInt UT_CSenHostletConnection_RespondL1L(CStifItemParser& aItem);

         TInt UT_CSenHostletConnection_RespondL2L(CStifItemParser& aItem);
         
         TInt UT_CSenHostletConnection_RespondL3L(CStifItemParser& aItem);
         
         TInt UT_CSenHostletConnection_Negative_ResponseCodeL(CStifItemParser& aItem);
         
         TInt UT_CSenHostletConnection_HC_SelfDef_URIL(CStifItemParser& aItem);
         
         TInt UT_CSenHostletConnection_Delete_HCL(CStifItemParser& aItem);
         
         TInt UT_CSenHostletConnection_Two_Equal_UID3L(CStifItemParser& aItem);
         
         TInt UT_CSenHostletConnection_Two_Equal_EndpointsL(CStifItemParser& aItem);
         
         TInt UT_CSenHostletConnection_Two_HC_Two_SC1L(CStifItemParser& aItem);
         
         TInt UT_CSenHostletConnection_Two_HC_Two_SC2L(CStifItemParser& aItem);
         
         TInt UT_CSenHostletConnection_Two_HC_Two_SC3L(CStifItemParser& aItem);
         
         TInt UT_CSenHostletConnection_DeleteSC_before_HCRespondLL(CStifItemParser& aItem);
         
         TInt UT_CSenHostletConnection_RespondL_4multipleTimesL(CStifItemParser& aItem);
         
         TInt UT_CSenHostletConnection_SC_Cancel_After_RequestL(CStifItemParser& aItem);
         
         TInt UT_CSenHostletConnection_Set_Expose_FacetL(CStifItemParser& aItem);
         
         TInt UT_CSenHostletConnection_RespondWithPropertiesL(CStifItemParser& aItem);

         //TInt UT_CSenHostletConnection_CreateConnAndSubmit_Rest_MultiThreadL(CStifItemParser& aItem);

         TInt UT_CSenHostletConnection_CreateConnAndSubmit_WSI_MultiThreadL(CStifItemParser& aItem);

         TInt UT_CSenHostletConnection_Send_WSIL(CStifItemParser& aItem);

         TInt UT_CSenHostletConnection_DescEndpoint_Send_WSIL(CStifItemParser& aItem);

         TInt UT_CSenHostletConnection_2SC1HCL(CStifItemParser& aItem);

         TInt UT_CSenHostletConnection_Submit_Rest_MultiThreadL(CStifItemParser& aItem); // Just for debugging, won't pass

         TInt UT_CSenHostletConnection_Submit_Rest_ActiveL(CStifItemParser& aItem); // Just for debugging, won't pass
         
         // TInt UT_CSenHostletConnection_Submit_WSIL(CStifItemParser& aItem); // non-working version, just to keep note
         TInt UT_CSenHostletConnection_IdentifierL(CStifItemParser& aItem );
		 TInt UT_CSenHostletConnection_FrameworkIdL(CStifItemParser& aItem );
		 TInt UT_CSenHostletConnection_RequestUtf8L(CStifItemParser& aItem );
		 TInt UT_CSenHostletConnection_ThreadIdL(CStifItemParser& aItem );
		 TInt UT_CSenHostletConnection_ConsumerIdL(CStifItemParser& aItem );
		 TInt UT_CSenHostletConnection_RequestIdL(CStifItemParser& aItem );
        /**
        * Test methods are listed below. 
        */

        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

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

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // HOSTCONNTESTER_H

// End of File
