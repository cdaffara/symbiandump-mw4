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
* Description:          ?Description
*
*/









// INCLUDE FILES
#include <e32math.h>
#include "senhostletconnectionbctest.h"

#include <SenFacet.h>


//  EXTERNAL INCLUDES
#include <SenServiceConnection.h>
#include <SenServicePattern.h>



//  INTERNAL INCLUDES
#include <SenHttpTransportProperties.h>
#include "SenHostletConnection.h"
#include "testconsumer.h"
#include "testhostlet.h"

#include <SenTransportProperties.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/


//Macro for Porting to STIF
#define LOCAL_ASSERT(expression)	{if(!(expression)){return KErrArgument;}}
#define LOCAL_VOID_ASSERT(expression)	{if(!(expression)){return ;}}

namespace
    {
    _LIT8(KHostletEndpoint, "hostlet://UnitTestHostlet");
    _LIT8(KHostletEndpoint1, "hostlet://UnitTestHostlet-1");
    _LIT8(KHostletContract, "urn:nokia.com.serene:unit-test-hostlet");
    _LIT8(KHostletContract1, "urn:nokia.com.serene:unit-test-hostlet-1");  
    
    void threadMainL(TAny* aPtr)
        {
        TConsumerTestTask* pTask = (TConsumerTestTask*) aPtr;
        pTask->Execute();
        // Go safely out of scope (will destroy this thread)
        }
    
    TInt threadFunction(TAny* aPtr)
        {
        CTrapCleanup* pCleanup = CTrapCleanup::New();
        CActiveScheduler::Install(NULL); // remove one
        CActiveScheduler* pScheduler = new (ELeave) CActiveScheduler();
        CActiveScheduler::Install(pScheduler);

        TInt leaveCode(KErrNone);
        TRAP(leaveCode, threadMainL(aPtr));

        CActiveScheduler::Install(NULL); // uninstall scheduler
        delete pScheduler;
        delete pCleanup;

        return leaveCode;
        }
        
        
    TInt CreateThreadForConsumerTask(RThread& aThread, const TDesC& aThreadName, TConsumerTestTask& aTask)
        {
        TName threadName(aThreadName);
        // Append a random number to make it unique
        threadName.AppendNum(Math::Random(), EHex);
                
#ifdef EKA2
        RAllocator& heap = User::Allocator(); // RHeap is deprecated in EKA2
        return aThread.Create(threadName, // name
                              threadFunction,
                              KDefaultStackSize,
                              &heap,
                              &aTask);
#else // EKA1
        RHeap& heap = User::Heap();
        return aThread.Create(threadName, // name
                              threadFunction,
                              KDefaultStackSize,
                              &heap,
                              &aTask);
#endif // EKA        
        }      
    }   


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSenServiceManagerBCTest::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases 
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
// 
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// -----------------------------------------------------------------------------
//
const TCaseInfo CSenHostletConnectionBCTest::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    SenServiceManager_stif.cpp file and to SenServiceManager_stif.h 
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case 
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case 
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and 
    * TInt value for last heap memory allocation failure.
    * 
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation 
    * failure and TInt value for last heap memory allocation failure. 
    */ 

    static TCaseInfoInternal const KCases[] =
        {
        // To add new test cases, add new items to this array
        
        // NOTE: When compiled to GCCE, there must be Classname::
        // declaration in front of the method name, e.g. 
        // CSenServiceManagerBCTest::PrintTest. Otherwise the compiler
        // gives errors.
		ENTRY("NewL - CSenHostletConnectionBCTest ", CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL),
    
		ENTRY("NewLC - CSenHostletConnectionBCTest ", CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLCL),
		
		ENTRY("Identifier - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_IdentifierL), 
		
		ENTRY("FrameworkId - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_FrameworkIdL), 
		
		ENTRY("RequestUtf8 - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_RequestUtf8L), 
		
		ENTRY("ThreadId - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_ThreadIdL), 

		ENTRY("ConsumerId - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_ConsumerIdL), 

		ENTRY("RequestId - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_RequestIdL), 

		ENTRY("NewLC - CSenHostletConnectionBCTest ", CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLC_3InvokesL),

		ENTRY("RespondL1 - CSenHostletConnectionBCTest ", CSenHostletConnectionBCTest::UT_CSenHostletConnection_RespondL1L),
   
		ENTRY("RespondL2 - CSenHostletConnectionBCTest ", CSenHostletConnectionBCTest::UT_CSenHostletConnection_RespondL2L),

		ENTRY("RespondL3 - CSenHostletConnectionBCTest ", CSenHostletConnectionBCTest::UT_CSenHostletConnection_RespondL3L),

		ENTRY("RespondL negative response code - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_Negative_ResponseCodeL),
    
		ENTRY("RespondL self def URI - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_HC_SelfDef_URIL),
/*    
		ENTRY("RespondL delete HC after SC is created - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_Delete_HCL),
*/
		ENTRY("NewL leaves with KErrSenEndpointReserved -CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_Two_Equal_UID3L),
         
		ENTRY("RespondL Two HC with the same Endpoints UIDs - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_Two_Equal_EndpointsL),    
   
		ENTRY("RespondL Two HC Two SC diff contracts - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_Two_HC_Two_SC1L),    
     
		ENTRY("RespondL Two HC Two SC diff endpionts - CSenHostletConnectionBCTest",CSenHostletConnectionBCTest::UT_CSenHostletConnection_Two_HC_Two_SC2L),    

		ENTRY("RespondL Two HC Two SC diff endpionts the same contract - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_Two_HC_Two_SC3L),    
      
		ENTRY("RespondL Delete SC before calling HC::RespondL - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_DeleteSC_before_HCRespondLL),    

		ENTRY("RespondL Call RespondL multipe times - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_RespondL_4multipleTimesL),    

		ENTRY("RespondL SC::Cancel after request - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_SC_Cancel_After_RequestL),    
    
		ENTRY("RespondL Set Expose Facet - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_Set_Expose_FacetL),    

		ENTRY("HC responds with TP - ", CSenHostletConnectionBCTest::UT_CSenHostletConnection_RespondWithPropertiesL),    

		//ENTRY("New REST SC + SubmitL Test - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_CreateConnAndSubmit_Rest_MultiThreadL),    
/*
		ENTRY("New WS-I SC + SubmitL Test - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_CreateConnAndSubmit_WSI_MultiThreadL),    


		ENTRY("SendL WS-I Test - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_Send_WSIL),    

		ENTRY("Desc EP + SendL WS-I Test - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_DescEndpoint_Send_WSIL),    
*/

		ENTRY("SendL from 2 SCs to 1 HC - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_2SC1HCL), 

/*
// NOTE: following is used instead:
// --> UT_CSenHostletConnection_CreateConnAndSubmit_Rest_MultiThreadL 
		ENTRY("SubmitL (multit.), Rest Test - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_Submit_Rest_MultiThreadL),    

// NOTE: following is used instead:
// --> UT_CSenHostletConnection_CreateConnAndSubmit_Rest_MultiThreadL 
		ENTRY("SubmitL (active), Rest Test - CSenHostletConnectionBCTest", CSenHostletConnectionBCTest::UT_CSenHostletConnection_Submit_Rest_ActiveL),
*/        
	 	};

	    // Verify that case number is valid
	    if( (TUint) aCaseNumber >= sizeof( KCases ) / 
	                               sizeof( TCaseInfoInternal ) )
	        {
	        // Invalid case, construct empty object
	        TCaseInfo null( (const TText*) L"" );
	        null.iMethod = NULL;
	        null.iIsOOMTest = EFalse;
	        null.iFirstMemoryAllocation = 0;
	        null.iLastMemoryAllocation = 0;
	        return null;
	        } 

	    // Construct TCaseInfo object and return it
	    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
	    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
	    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
	    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
	    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
	    return tmp;

	    }   

//  METHODS

void CSenHostletConnectionBCTest::SetupL ()
    {
    __UHEAP_MARK;
    }

void CSenHostletConnectionBCTest::Teardown ()
    {
//    User::After( 2 * 1000 * 1000 ); // temp tests
    __UHEAP_MARKEND;
    }

void CSenHostletConnectionBCTest::StartActiveScheduler(TInt aNumberOfPendingOps)
    {
    iNumberOfPendingOps = aNumberOfPendingOps;
    CActiveScheduler::Start();
    }
       
void CSenHostletConnectionBCTest::StopActiveScheduler ()
    {
    iNumberOfPendingOps--;
    if ( iNumberOfPendingOps == 0 )
        {
    	CActiveScheduler::Stop();
        }
    }

void TConsumerTestTask::Execute()
    {
    switch(iJob)
        {
        case(ECreateConnAndSubmit): // this is a very oddly orientated, wild case..
            {
            if(ipOwner)
                {
                CTestConsumer* pConsumer = CTestConsumer::NewL(ipOwner); // uses the default implementations for setstatus, handlemessage, handleerror
                CleanupStack::PushL(pConsumer);

                // Note, if NOT set, iEndpoint is typically KNullDesC8 (zero-lenght), which is OK, too.
                CSenServicePattern* pPattern = CSenServicePattern::NewLC(iEndpoint, KHostletContract);
                if(iFrameworkId.Length()>0)
                    {
                    pPattern->SetFrameworkIdL(iFrameworkId);
                    }
                else
                    {
                    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);                        
                    }
                    
                CSenServiceConnection* pConnection = CSenServiceConnection::NewL(*pConsumer, *pPattern);
                CleanupStack::PopAndDestroy(pPattern);
                CleanupStack::PushL(pConnection);
                
                // Construct service connection by starting active scheduler..
                ipOwner->StartActiveScheduler(1);
                
                // Check that NO HandleErrorL callback was received:
                LOCAL_VOID_ASSERT( pConsumer->iErrorCode == KErrNone );
                
                
                 // Check that the Status of the ServiceConnection
                // is KSenConnectionStatusReady
                LOCAL_VOID_ASSERT( pConsumer->iConnectionStatus == KSenConnectionStatusReady); 

                // execution returns here EITHER when HandleMessageL or HandleErrorL has been invoked..
                if(iProperties.Length()==0)
                    {
                    if(ipSoapMsg)
                        {
                        iStatusCode = pConnection->SubmitL(*ipSoapMsg, ipNotOwnedData);
                        }
                    else
                        {
                        iStatusCode = pConnection->SubmitL(iRequest, ipNotOwnedData);
                        }
                    }
                else
                    {
                    if(ipSoapMsg)
                        {
                        iStatusCode = pConnection->SubmitL(*ipSoapMsg, iProperties, ipNotOwnedData);
                        }
                    else
                        {
                        iStatusCode = pConnection->SubmitL(iRequest, iProperties, ipNotOwnedData);
                        }
                    }
                CleanupStack::PopAndDestroy(pConnection);
                CleanupStack::PopAndDestroy(pConsumer);
                }
           }
           break;

        case(ESubmit):
            {
            if(iProperties.Length()==0)
                {
                iStatusCode = iConnection.SubmitL(iRequest, ipNotOwnedData);
                }
            else
                {
                iStatusCode = iConnection.SubmitL(iRequest, iProperties, ipNotOwnedData);
                }
            }
            break;

        case(EActiveSubmit):
            {
            if ( ipActive)
                {
                // Perform blocking (sync) submit:
                if(iProperties.Length()==0)
                    {
                    iStatusCode = iConnection.SubmitL(iRequest, ipNotOwnedData);
                    }
                else
                    {
                    iStatusCode = iConnection.SubmitL(iRequest, iProperties, ipNotOwnedData);
                    }
                    
                // Notify "owner" that blocking operation has completed
                TRequestStatus* status = &ipActive->iStatus;
                RThread owner;
                owner.Open(ipActive->iOwnerThreadId);
                owner.RequestComplete(status, CActiveConsumerTestTask::EDeliverResponse );
                }
            }
            break;
           
        }
    }

/////////////////////////////////////////////////////////////////////////////////////////////////
// HELPER CLASS for SC (active) tasks:
// Public METHODS

CActiveConsumerTestTask* CActiveConsumerTestTask::NewL(TConsumerTestTask& aTask, const TDesC& aThreadName, CSenHostletConnectionBCTest& aOwner)
    {
    CActiveConsumerTestTask* pNew = CActiveConsumerTestTask::NewLC(aTask, aThreadName, aOwner);
    CleanupStack::Pop();
    return pNew;
    }

CActiveConsumerTestTask* CActiveConsumerTestTask::NewLC(TConsumerTestTask& aTask, const TDesC& aThreadName, CSenHostletConnectionBCTest& aOwner)
    {
    CActiveConsumerTestTask* pNew = new (ELeave) CActiveConsumerTestTask(aTask, aOwner);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aThreadName);
    return pNew;
    }

CActiveConsumerTestTask::CActiveConsumerTestTask(TConsumerTestTask& aTask, CSenHostletConnectionBCTest& aOwner)
    :   CActive(EPriorityStandard), 
        iTask(aTask),
        iOwner(aOwner)
    {
    CActiveScheduler::Add(this);    
    }

CActiveConsumerTestTask::~CActiveConsumerTestTask()
    {
    if(IsActive())
        {
        Cancel(); // issues DoCancel();
        }
    iTaskThread.Close();
    }

void CActiveConsumerTestTask::RunL()
    {
    switch(iStatus.Int())
        {
        case(EInvokeService):
            {
            iTaskThread.Resume();
            }
            break;
            
        case(EDeliverResponse):
            {
            iOwner.StopActiveScheduler(); // submit complete..
            }
            break;
         /* case defaul: // unknown */
        }
    }
void CActiveConsumerTestTask::DoCancel()
    {
    iOwner.StopActiveScheduler();
    }
    
// Private METHODS
void CActiveConsumerTestTask::ConstructL(const TDesC& aThreadName)
    {
    RThread thread;
    iOwnerThreadId = thread.Id(); 
    
    TInt createThreadRetVal = CreateThreadForConsumerTask(iTaskThread, aThreadName, iTask);        
    User::LeaveIfError( createThreadRetVal );
    SetActive();
    }
    
    
/////////////////////////////////////////////////////////////////////////////////////////////////

void  CSenHostletConnectionBCTest::UT_CSenHostletConnection_WSI_DescribeServiceL(CSenXmlServiceDescription& aSD)
    {
    aSD.SetFrameworkIdL(KDefaultBasicWebServicesFrameworkID);
    aSD.SetEndPointL(KHostletEndpoint);
    aSD.SetContractL(KHostletContract);
    }

void  CSenHostletConnectionBCTest::UT_CSenHostletConnection_WSI_DescribeService_2L(CSenXmlServiceDescription& aSD)
    {
    aSD.SetFrameworkIdL(KDefaultBasicWebServicesFrameworkID);
    aSD.SetContractL(KHostletContract);
    }


void CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_SetStatus(CSenHostletConnectionBCTest* apTestSuite,
                                                                    TInt aStatus )
   {
    switch( aStatus )
        {
        case KSenConnectionStatusReady:
            {
            apTestSuite->StopActiveScheduler(); // otherwise failed SC construction is reported via HandleErrorL(!)
            }
            break;
        case KSenConnectionStatusNew:
        case KSenConnectionStatusExpired:
        default:            
            break;
        }
    }



// not in use atm:
void CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleMessageL(CSenHostletConnectionBCTest* /* apTestSuite */, const TDesC8& /* aMessage*/ )
    {
    }

//used by UT_CSenHostletConnection_Negative_ResponseCodeL
//used by UT_CSenHostletConnection_Delete_HCL
void CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleErrorL(CSenHostletConnectionBCTest* apTestSuite, const TInt  /*aErrorCode*/ , const TDesC8& /* aMessage */)
    {
    apTestSuite->iWas_HandleErrorL_Accessed = ETrue;
    }
    
//used by the UT_CSenHostletConnection_Negative_ResponseCodeL    
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceL(CSenHostletConnectionBCTest*  apTestSuite , MSenHostletRequest&  /*aRequest*/ , MSenHostletResponse&  aResponse )
    {
    TInt alteredResponseCode = -148000;
    _LIT8(KAlteredResponse, "<Response><ReplyMessage>Response Altered in UT_CSenHostletConnection_NewLL_OnServiceL.</ReplyMessage><Request>%S</Request></Response>");
    apTestSuite->ipHostlet->iResponseCodes[apTestSuite->ipHostlet->iTxnIds.Count()-1] = alteredResponseCode;
    aResponse.SetResponseUtf8L(KAlteredResponse, alteredResponseCode);
    return KErrNone;
    }
    
// not in use atm:    
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceCompleteL(CSenHostletConnectionBCTest* /* apTestSuite */, const TInt /* aTxnId */, TInt /* aCompletionCode */, const TDesC8& /* aDesc */ )
    {
    return KErrNone;
    }    
    
//used by the UT_CSenHostletConnection_DeleteSC_before_HCRespondLL method
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnService1L(CSenHostletConnectionBCTest*  apTestSuite , MSenHostletRequest&  /*aRequest*/ , MSenHostletResponse&  /*aResponse*/ )
    {
    apTestSuite->ipConnection->Cancel();
    delete apTestSuite->ipConnection;
    apTestSuite->ipConnection = NULL;
    return KErrNone;
    }
    
//used by UT_CSenHostletConnection_DeleteSC_before_HCRespondLL
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceComplete1L(CSenHostletConnectionBCTest* apTestSuite, const TInt /* aTxnId */, TInt /*aCompletionCode*/ , const TDesC8& /* aDesc */ )
    {
	apTestSuite->iWas_OnServiceL_Accessed = ETrue;
    return KErrNone;
    }        

// used by the UT_CSenHostletConnection_RespondL_4multipleTimesL method
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnService2L(CSenHostletConnectionBCTest*  apTestSuite , MSenHostletRequest&  /*aRequest*/ , MSenHostletResponse&  aResponse )
    {
    TInt respondRetCode = apTestSuite->ipHostletConnection->RespondL(aResponse);
    TInt respondRetCode1 = apTestSuite->ipHostletConnection->RespondL(aResponse);
	LOCAL_ASSERT(respondRetCode == KErrNone);    
    LOCAL_ASSERT(respondRetCode1 == KErrAlreadyExists);
    return KErrNone;
    }
    
// to be used (currently does nothing) by the UT_CSenHostletConnection_RespondL_4multipleTimesL method    
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceComplete2L(CSenHostletConnectionBCTest* /* apTestSuite */, const TInt /* aTxnId */, TInt /*aCompletionCode*/ , const TDesC8& /* aDesc */ )
    {
    return KErrNone;
    }        
    
//used by the UT_CSenHostletConnection_SC_Cancel_After_RequestL method
void CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleError1L(CSenHostletConnectionBCTest* apTestSuite, const TInt  /* aErrorCode */, const TDesC8& /* aMessage */)
    {
//    TInt a = aErrorCode;
    apTestSuite->iWas_HandleErrorL_Accessed = ETrue;
    }
//used by the UT_CSenHostletConnection_SC_Cancel_After_RequestL method
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnService3L(CSenHostletConnectionBCTest*  apTestSuite , MSenHostletRequest&  /*aRequest*/ , MSenHostletResponse&  /*aResponse*/ )
    {
   // apTestSuite->ipConnection->Cancel();
   //canceling request
    apTestSuite->ipConnection->CancelTransaction(apTestSuite->iTxnID);
    return KErrNone;
    }


//used by the UT_CSenHostletConnection_RespondWithPropertiesL method in order to provide transport properties with the response
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_RespondWithPropertiesL_OnServiceL(CSenHostletConnectionBCTest*  apTestSuite , MSenHostletRequest&  /*aRequest*/ , MSenHostletResponse&  aResponse )
    {
    TInt txn =          apTestSuite->ipHostlet->iTxnIds.Count()-1;
    TPtr8 response =    apTestSuite->ipHostlet->iResponses[txn]->Des();

    TInt responseCode = apTestSuite->ipHostlet->iResponseCodes[txn];

    CSenTransportProperties* pTransportProperties = CSenTransportProperties::NewLC();
    _LIT8(KTestCid, "test:cid");
    _LIT8(KTestFile, "c:\\test.file.txt");
    pTransportProperties->SetFileAttachmentL( KTestCid, KTestFile );
    
    if( apTestSuite->ipHostlet->iResponseProps.Append( pTransportProperties ) )
        {
        // error:
        CleanupStack::PopAndDestroy(pTransportProperties);
        return KErrNoMemory;
        }
    else
        {
        // ok
        CleanupStack::Pop(pTransportProperties);
        aResponse.SetResponseUtf8L(response, responseCode, pTransportProperties); // by default, all requests are "accepted"
        return KErrNone;
        }
    }



//---------------------------------------------------------------
// 							Test Cases
//---------------------------------------------------------------


TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL(TTestResult& aResult)
    {
	SetupL(); 
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror

//    ipHostlet = CTestHostlet::NewL(this,
//                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceL,
//                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceCompleteL);

//    ipConsumer = CTestConsumer::NewL(this, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleMessageL, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleErrorL, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_SetStatus);

    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    // Construct service connection
    StartActiveScheduler(1);
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");

    TInt transactionIdOrError = ipConnection->SendL(KRequest);

    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);

    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    // --- Conditions END ---
    
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    Teardown();
    return KErrNone;
    }

    
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLCL(TTestResult& aResult)
    {
	SetupL(); 
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror

//    ipHostlet = CTestHostlet::NewL(this,
//                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceL,
//                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceCompleteL);

//    ipConsumer = CTestConsumer::NewL(this, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleMessageL, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleErrorL, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_SetStatus);

    ipHostletConnection = CSenHostletConnection::NewLC(*ipHostlet);

    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    // Construct service connection
    StartActiveScheduler(1);
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");

    TInt transactionIdOrError = ipConnection->SendL(KRequest);

    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);

    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    // --- Conditions END ---
    
    delete ipConnection;
    ipConnection = NULL;
    CleanupStack::PopAndDestroy(ipHostletConnection);
    //delete ipHostletConnection; 
    //ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    Teardown();
    return KErrNone;
    }

TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_IdentifierL(TTestResult& aResult)
    {
	SetupL(); 
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror

//    ipHostlet = CTestHostlet::NewL(this,
//                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceL,
//                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceCompleteL);

//    ipConsumer = CTestConsumer::NewL(this, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleMessageL, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleErrorL, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_SetStatus);

    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    TInt id;
    id = ipHostletConnection->Identifier();
    LOCAL_ASSERT( id  != KErrNotReady);
    TPtrC8 framework = ipHostlet->FrameworkId();
    // Construct service connection
    StartActiveScheduler(1);
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");

    TInt transactionIdOrError = ipConnection->SendL(KRequest);

    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);

    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    // --- Conditions END ---
    
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    Teardown();
    return KErrNone;
    }

TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_FrameworkIdL(TTestResult& aResult)
    {
	SetupL(); 
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror

//    ipHostlet = CTestHostlet::NewL(this,
//                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceL,
//                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceCompleteL);

//    ipConsumer = CTestConsumer::NewL(this, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleMessageL, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleErrorL, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_SetStatus);

    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    TInt id;
    id = ipHostletConnection->Identifier();
    LOCAL_ASSERT( id  != KErrNotReady);
    _LIT8(KFramework, "REST");
    TPtrC8 framework = ipHostlet->FrameworkId();
    LOCAL_ASSERT( framework == KFramework);
    // Construct service connection
    StartActiveScheduler(1);
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");

    TInt transactionIdOrError = ipConnection->SendL(KRequest);

    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);

    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    // --- Conditions END ---
    
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    Teardown();
    return KErrNone;
    }

TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_RequestUtf8L(TTestResult& aResult)
    {
	SetupL(); 
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror

//    ipHostlet = CTestHostlet::NewL(this,
//                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceL,
//                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceCompleteL);

//    ipConsumer = CTestConsumer::NewL(this, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleMessageL, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleErrorL, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_SetStatus);

    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    TInt id;
    id = ipHostletConnection->Identifier();
    LOCAL_ASSERT( id  != KErrNotReady);
    TPtrC8 request = ipHostlet->RequestUtf8();
    LOCAL_ASSERT( request == _L8(""));
    // Construct service connection
    StartActiveScheduler(1);
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");

    TInt transactionIdOrError = ipConnection->SendL(KRequest);

    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);

    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    // --- Conditions END ---
    
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    Teardown();
    return KErrNone;
    }

TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_ThreadIdL(TTestResult& aResult)
    {
	SetupL(); 
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror

//    ipHostlet = CTestHostlet::NewL(this,
//                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceL,
//                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceCompleteL);

//    ipConsumer = CTestConsumer::NewL(this, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleMessageL, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleErrorL, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_SetStatus);

    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    TInt id;
    id = ipHostletConnection->Identifier();
    LOCAL_ASSERT( id  != KErrNotReady);
    TPtrC threadId = ipHostlet->ThreadId();
    LOCAL_ASSERT( threadId == _L(""));
    // Construct service connection
    StartActiveScheduler(1);
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");

    TInt transactionIdOrError = ipConnection->SendL(KRequest);

    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);

    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    // --- Conditions END ---
    
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    Teardown();
    return KErrNone;
    }
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_ConsumerIdL(TTestResult& aResult)
	{
	SetupL(); 
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror

//    ipHostlet = CTestHostlet::NewL(this,
//                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceL,
//                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceCompleteL);

//    ipConsumer = CTestConsumer::NewL(this, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleMessageL, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleErrorL, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_SetStatus);

    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    TInt id;
    id = ipHostletConnection->Identifier();
    LOCAL_ASSERT( id  != KErrNotReady);
    TPtrC8 consumerId = ipHostlet->ConsumerId();
    LOCAL_ASSERT( consumerId == _L8(""));
    // Construct service connection
    StartActiveScheduler(1);
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");

    TInt transactionIdOrError = ipConnection->SendL(KRequest);

    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);

    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    // --- Conditions END ---
    
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    Teardown();
    return KErrNone;
    }
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_RequestIdL(TTestResult& aResult)
	{
	SetupL(); 
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror

//    ipHostlet = CTestHostlet::NewL(this,
//                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceL,
//                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceCompleteL);

//    ipConsumer = CTestConsumer::NewL(this, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleMessageL, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleErrorL, 
//                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_SetStatus);

    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    TInt id;
    id = ipHostletConnection->Identifier();
    LOCAL_ASSERT( id  != KErrNotReady);
    TInt requestId = ipHostlet->RequestId();
    LOCAL_ASSERT( requestId == KErrNone);
    // Construct service connection
    StartActiveScheduler(1);
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");

    TInt transactionIdOrError = ipConnection->SendL(KRequest);

    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);

    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    // --- Conditions END ---
    
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    Teardown();
    return KErrNone;		
	}
	
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLC_3InvokesL(TTestResult& aResult)
    {
	SetupL(); 
    // Try to send _two_ messages from ServiceConnection to HostletConnection
    // and check that everything goes OK.

    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Abel\'] or containts[ab:N/ab:GIVEN,\'Abel\'] or containts[ab:TEL,\'Abel\']]</ab:Select></ab:QueryItem></ab:Query>");
    _LIT8(KRequest2, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");
    _LIT8(KRequest3, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Clarice\'] or containts[ab:N/ab:GIVEN,\'Clarice\'] or containts[ab:TEL,\'Clarice\']]</ab:Select></ab:QueryItem></ab:Query>");
    
    // Construct Hostlet
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    // Construct ServiceConnection
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    StartActiveScheduler(1);
    
    // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer->iConnectionStatus == KSenConnectionStatusReady);
    
    // ServiceConnection is ready

    // => Send first request to the Hostlet
    TInt transactionIdOrError = ipConnection->SendL(KRequest);
    
    // Check that SendL returned transaction Id
    LOCAL_ASSERT(transactionIdOrError > KErrNone);

    // => Send second request to the Hostlet
    TInt transactionIdOrError2 = ipConnection->SendL(KRequest2);
    
    // => Send second request to the Hostlet
    TInt transactionIdOrError3 = ipConnection->SendL(KRequest3);

    // Check that SendL returned transaction Id
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  THREE TIMES (one call to ServiceL), respond to consumer THREE TIMES => six async calls:
    StartActiveScheduler(6);

    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
	LOCAL_ASSERT(*ipHostlet->iRequests[0] == KRequest);
	LOCAL_ASSERT(*ipHostlet->iRequests[1] == KRequest2);
	LOCAL_ASSERT(*ipHostlet->iRequests[2] == KRequest3);
	
	// 2. Consumer receives response that hostlet provided
	LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
	LOCAL_ASSERT( *ipConsumer->iResponses[1] == *ipHostlet->iResponses[1]);
	LOCAL_ASSERT( *ipConsumer->iResponses[2] == *ipHostlet->iResponses[2]);

    // 3. Hostlet receives completion code (delivery code)
	LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])
	LOCAL_ASSERT( ipConsumer->iResponseCodes[1] == ipHostlet->iResponseCodes[1])
	LOCAL_ASSERT( ipConsumer->iResponseCodes[2] == ipHostlet->iResponseCodes[2])


	LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError);
	LOCAL_ASSERT( ipHostlet->iTxnIds[1] == transactionIdOrError2);
	LOCAL_ASSERT( ipHostlet->iTxnIds[2] == transactionIdOrError3);

    
    // --- Conditions END ---
    
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    Teardown();
    return KErrNone;
    }


TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_RespondL1L(TTestResult& aResult)
    {
	SetupL(); 
    // Try to send _one_ message from ServiceConnection to HostletConnection
    // and check that everything goes OK.
    
    // Construct Hostlet
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    // Construct ServiceConnection
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    StartActiveScheduler(1);
    
    // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer->iConnectionStatus == KSenConnectionStatusReady);
    
    
	CSenTransportProperties* pProps = CSenTransportProperties::NewLC();
	pProps->SetPropertyL(_L8("KTestProperty"), _L8("KTestPropertyValue"));
	HBufC8* pPropsAsXml = pProps->AsUtf8LC();
    TPtrC8 propertiesAsXml = pPropsAsXml->Des();
    
    // ServiceConnection is ready
    // => Send request to the Hostlet
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");
    TInt transactionIdOrError = ipConnection->SendL(KRequest, propertiesAsXml);
    
    // Check that SendL returned transaction Id
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);

    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    // 4. The properties that were sent via SC were received in HC
    MSenProperties::TSenPropertiesClassType type = ipHostlet->iRequestPropTypes[0];
    TPtrC8 properties = ipHostlet->iRequestProps[0]->Des();

    LOCAL_ASSERT( properties == propertiesAsXml );
    
    // --- Conditions END ---

    CleanupStack::PopAndDestroy( pPropsAsXml );
    CleanupStack::PopAndDestroy( pProps );
    
    
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    Teardown();
    return KErrNone;
    }


TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_RespondL2L(TTestResult& aResult)
    {
	SetupL(); 
    // Try to send _two_ messages from ServiceConnection to HostletConnection
    // and check that everything goes OK.

    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");
    
    // Construct Hostlet
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    // Construct ServiceConnection
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    StartActiveScheduler(1);
    
    // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer->iConnectionStatus == KSenConnectionStatusReady);
    
    // ServiceConnection is ready

    // => Send first request to the Hostlet
    TInt transactionIdOrError = ipConnection->SendL(KRequest);
    
    // Check that SendL returned transaction Id
    LOCAL_ASSERT(transactionIdOrError > KErrNone);

    // => Send second request to the Hostlet
    TInt transactionIdOrError2 = ipConnection->SendL(KRequest);
    
    // Check that SendL returned transaction Id
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  TWICE (one call to ServiceL), respond to consumer TWICE => four calls:
    StartActiveScheduler(4);

    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);
    LOCAL_ASSERT( *ipHostlet->iRequests[1] == KRequest);    

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( *ipConsumer->iResponses[1] == *ipHostlet->iResponses[1]);
    
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])
    LOCAL_ASSERT( ipConsumer->iResponseCodes[1] == ipHostlet->iResponseCodes[1])
    
    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError)
    LOCAL_ASSERT( ipHostlet->iTxnIds[1] == transactionIdOrError2)
    
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[1] == KErrNone ); // true in this "OK" -test case   
    
    // --- Conditions END ---
    
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    Teardown();
    return KErrNone;
    }


TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_RespondL3L(TTestResult& aResult)
    {
    	//Memory leak detection is not supported since Symbian 9.0 (EKA2 kernel) environments.
    	//This this is added to handle thread handle leak in testmodule0
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles);
	SetupL(); 
    // Try to send _one_ message from ServiceConnection to HostletConnection
    // and check that everything goes OK.
    
    // Construct Hostlet
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    // Construct ServiceConnection
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    StartActiveScheduler(1);
    
    // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer->iConnectionStatus == KSenConnectionStatusReady);
    
    // Construct 2. ServiceConnection
    CTestConsumer* pConsumer2 = CTestConsumer::NewLC(this); // uses the default implementations for setstatus, handlemessage, handleerror

    CSenServicePattern* pPattern2 =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern2->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    CSenServiceConnection* pConnection2 = CSenServiceConnection::NewL(*pConsumer2, *pPattern2);
    CleanupStack::PopAndDestroy(pPattern2);
    CleanupStack::PushL(pConnection2);
    StartActiveScheduler(1);
    
    // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( pConsumer2->iConnectionStatus == KSenConnectionStatusReady);
    
	CSenTransportProperties* pProps = CSenTransportProperties::NewLC();
	pProps->SetPropertyL(_L8("KTestProperty"), _L8("KTestPropertyValue"));
	HBufC8* pPropsAsXml = pProps->AsUtf8LC();
    TPtrC8 propertiesAsXml = pPropsAsXml->Des();
    
    // ServiceConnection is ready
    // => Send request to the Hostlet
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");
    TInt transactionIdOrError = ipConnection->SendL(KRequest, propertiesAsXml);
    
    // Check that SendL returned transaction Id
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // 2. ServiceConnection is ready
    // => Send 2. request to the Hostlet
    TInt transactionIdOrError2 = pConnection2->SendL(KRequest, propertiesAsXml);
    
    // Check that SendL returned transaction Id
    LOCAL_ASSERT(transactionIdOrError2 > KErrNone);
    
    // Invoke service  TWICE (two calls to ServiceL), respond to consumer TWICE => four calls
    StartActiveScheduler(4);

    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    // 4. The properties that were sent via SC were received in HC
    MSenProperties::TSenPropertiesClassType type = ipHostlet->iRequestPropTypes[0];
    TPtrC8 properties = ipHostlet->iRequestProps[0]->Des();

    LOCAL_ASSERT( properties == propertiesAsXml );
    
    // --- Conditions END ---

    CleanupStack::PopAndDestroy( pPropsAsXml );
    CleanupStack::PopAndDestroy( pProps );
    
    
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    
    CleanupStack::PopAndDestroy(pConnection2);
    CleanupStack::PopAndDestroy(pConsumer2);
    Teardown();
    return KErrNone;
    }


TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_Negative_ResponseCodeL(TTestResult& aResult)
    {
	SetupL(); 
       
 	iWas_HandleErrorL_Accessed = EFalse;
    ipHostlet = CTestHostlet::NewL(this,
                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceL,
                                   NULL);

    ipConsumer = CTestConsumer::NewL(this, 
                                     NULL, 
                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleErrorL, 
                                     NULL);

    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);
	
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    // Construct service connection
    StartActiveScheduler(1);
    
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");

    TInt transactionIdOrError = ipConnection->SendL(KRequest);
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);
    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])
    
    
 	// 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone );
    
    //4. HandleErrorL accesed after negative iResponseCodes
    LOCAL_ASSERT( iWas_HandleErrorL_Accessed );
    
    // --- Conditions END ---
    
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    Teardown();
    return KErrNone;
    }
  
    
 
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_HC_SelfDef_URIL(TTestResult& aResult)
    {
	SetupL(); 
                                     
  	ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
  	ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror

  
  	ipHostlet->ipHostletDescription->SetEndPointL(KHostletEndpoint);	
  	ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);
    

  	CSenServicePattern* pPattern =
		    CSenServicePattern::NewLC(KHostletEndpoint, KNullDesC8);

    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    // Construct service connection
    StartActiveScheduler(1);

	// Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer->iConnectionStatus == KSenConnectionStatusReady);    
   
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");

    TInt transactionIdOrError = ipConnection->SendL(KRequest);
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);
    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])
    
    
 	// 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    // --- Conditions END ---
    
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    Teardown();
    return KErrNone;
    }
   
/*
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_Delete_HCL(TTestResult& aResult)
    {
	SetupL(); 
     
    iWas_HandleErrorL_Accessed = EFalse;                              
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer = CTestConsumer::NewL(this, 
                                     NULL, 
                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleErrorL, 
                                     NULL);

    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    // Construct service connection
    StartActiveScheduler(1);
    
    //delete Hostlet Connection
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");

    TInt transactionIdOrError = ipConnection->SendL(KRequest);

    // ---- Following conditions MUST be met ----
    LOCAL_ASSERT(transactionIdOrError == KErrSenHostNotAvailable);

    // Start waiting for HandleError Callback which is caused
    // by error in SendL
    StartActiveScheduler(1);
    // Callback was get => Check that callback was HandleError
    LOCAL_ASSERT( iWas_HandleErrorL_Accessed );
    
    // --- Conditions END ---
    
    delete ipConnection;
    ipConnection = NULL;
    
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
  
    Teardown();
    return KErrNone;
    }
    
    
*/
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_Two_Equal_UID3L(TTestResult& aResult)
    {
	SetupL(); 
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel

    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);
 	
 	//Check if CSenHostletConnection::NewL leaves with KErrSenEndpointReserved
/*	EUNIT_ASSERT_SPECIFIC_LEAVE(ipHostletConnection1 = CSenHostletConnection::NewL(*ipHostlet),KErrSenEndpointReserved);*/
	TRAPD(res,ipHostletConnection1 = CSenHostletConnection::NewL(*ipHostlet)) ;
	User::LeaveIfError(res == KErrSenEndpointReserved);

  
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    Teardown();
    return KErrNone;
    }
   
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_Two_Equal_EndpointsL(TTestResult& aResult)
    {
	SetupL(); 
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
	ipHostlet->ipHostletDescription->SetEndPointL(KHostletEndpoint);
    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);
 
 	//Check if CSenHostletConnection::NewL leaves with KErrSenEndpointReserved
	/*EUNIT_ASSERT_SPECIFIC_LEAVE(ipHostletConnection1 = CSenHostletConnection::NewL(*ipHostlet),KErrSenEndpointReserved);*/
	TRAPD(res,ipHostletConnection1 = CSenHostletConnection::NewL(*ipHostlet)) ;
	User::LeaveIfError(res == KErrSenEndpointReserved);
  
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    Teardown();
    return KErrNone;
    }
 

TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_Two_HC_Two_SC1L(TTestResult& aResult)
    {
    	//Memory leak detection is not supported since Symbian 9.0 (EKA2 kernel) environments.
    	//This this is added to handle thread handle leak in testmodule0
     TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles);
	SetupL(); 
   
    //First pair of HC and SC
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror

    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    // Construct service connection
    StartActiveScheduler(1);
    
    // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer->iConnectionStatus == KSenConnectionStatusReady); 
	
	
	//Second pair of HC and SC
    ipHostlet1 = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer1 = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror

	ipHostlet1->ipHostletDescription->SetContractL(KHostletContract1);
    ipHostletConnection1 = CSenHostletConnection::NewL(*ipHostlet1);

    CSenServicePattern* pPattern1 =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract1);
    pPattern1->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection1 = CSenServiceConnection::NewL(*ipConsumer1, *pPattern1);
    CleanupStack::PopAndDestroy(pPattern1);
    
    // Construct service connection
    StartActiveScheduler(1); 
    
    // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer1->iConnectionStatus == KSenConnectionStatusReady); 
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");
    _LIT8(KRequest1, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob1\'] or containts[ab:N/ab:GIVEN,\'Bob1\'] or containts[ab:TEL,\'Bob1\']]</ab:Select></ab:QueryItem></ab:Query>");

	ipUsedHostletConnection = ipHostletConnection;
	
    TInt transactionIdOrError = ipConnection->SendL(KRequest);
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);
    
    ipUsedHostletConnection = ipHostletConnection1;
    transactionIdOrError = ipConnection1->SendL(KRequest1);
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);
 
    // ---- Following conditions MUST be met ----
	
	// First pair of the HC and SC
    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
	
	// Second pair of the HC and SC
	// 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet1->iRequests[0] == KRequest1);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer1->iResponses[0] == *ipHostlet1->iResponses[0]);
    LOCAL_ASSERT( ipConsumer1->iResponseCodes[0] == ipHostlet1->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet1->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet1->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
	
    // --- Conditions END ---
    
	ipUsedHostletConnection = NULL;  
	
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;

    delete ipConnection1;
    ipConnection1 = NULL;
    delete ipHostletConnection1; 
    ipHostletConnection1 = NULL;
    delete ipHostlet1;
    ipHostlet1 = NULL;
    delete ipConsumer1;
    ipConsumer1 = NULL;
    
    Teardown();
    return KErrNone;
    }

      

TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_Two_HC_Two_SC2L(TTestResult& aResult)
    {
    	//Memory leak detection is not supported since Symbian 9.0 (EKA2 kernel) environments.
    	//This this is added to handle thread handle leak in testmodule0
     TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles);
	SetupL(); 
   
    //First pair of HC and SC
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror
    
	ipHostlet->ipHostletDescription->SetEndPointL(KHostletEndpoint);
	ipHostlet->ipHostletDescription->SetContractL(KNullDesC8);
    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KHostletEndpoint, KNullDesC8);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    // Construct service connection
    StartActiveScheduler(1);
    
 	// Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer->iConnectionStatus == KSenConnectionStatusReady); 

	
	//Second pair of HC and SC
    ipHostlet1 = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer1 = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror


	ipHostlet1->ipHostletDescription->SetEndPointL(KHostletEndpoint1);
	ipHostlet1->ipHostletDescription->SetContractL(KNullDesC8);
    ipHostletConnection1 = CSenHostletConnection::NewL(*ipHostlet1);

    CSenServicePattern* pPattern1 =
        CSenServicePattern::NewLC(KHostletEndpoint1, KNullDesC8);
    pPattern1->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection1 = CSenServiceConnection::NewL(*ipConsumer1, *pPattern1);
    CleanupStack::PopAndDestroy(pPattern1);
    
    // Construct service connection
    StartActiveScheduler(1);   
    
    // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer1->iConnectionStatus == KSenConnectionStatusReady); 
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");
    _LIT8(KRequest1, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob1\'] or containts[ab:N/ab:GIVEN,\'Bob1\'] or containts[ab:TEL,\'Bob1\']]</ab:Select></ab:QueryItem></ab:Query>");

	ipUsedHostletConnection = ipHostletConnection;
    TInt transactionIdOrError = ipConnection->SendL(KRequest);
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);
    
    ipUsedHostletConnection = ipHostletConnection1;
    transactionIdOrError = ipConnection1->SendL(KRequest1);
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);
 
    // ---- Following conditions MUST be met ----
	
	// First pair of the HC and SC
    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
	
	// Second pair of the HC and SC
	// 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet1->iRequests[0] == KRequest1);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer1->iResponses[0] == *ipHostlet1->iResponses[0]);
    LOCAL_ASSERT( ipConsumer1->iResponseCodes[0] == ipHostlet1->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet1->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet1->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
	
    // --- Conditions END ---
    
	ipUsedHostletConnection = NULL;  
	
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;

    delete ipConnection1;
    ipConnection1 = NULL;
    delete ipHostletConnection1; 
    ipHostletConnection1 = NULL;
    delete ipHostlet1;
    ipHostlet1 = NULL;
    delete ipConsumer1;
    ipConsumer1 = NULL;
    
    Teardown();
    return KErrNone;
    }


 TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_Two_HC_Two_SC3L(TTestResult& aResult)
    {
    	//Memory leak detection is not supported since Symbian 9.0 (EKA2 kernel) environments.
    	//This this is added to handle thread handle leak in testmodule0
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles);
	SetupL(); 
   
    //First pair of HC and SC
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror
    
	ipHostlet->ipHostletDescription->SetEndPointL(KHostletEndpoint);
	ipHostlet->ipHostletDescription->SetContractL(KNullDesC8);
    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KHostletEndpoint, KNullDesC8);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    // Construct service connection
    StartActiveScheduler(1);
 	// Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer->iConnectionStatus == KSenConnectionStatusReady); 	
    
	//Second pair of HC and SC
    ipHostlet1 = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer1 = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror


	ipHostlet1->ipHostletDescription->SetEndPointL(KHostletEndpoint1);
	ipHostlet1->ipHostletDescription->SetContractL(KNullDesC8);
    ipHostletConnection1 = CSenHostletConnection::NewL(*ipHostlet1);

    CSenServicePattern* pPattern1 =
        CSenServicePattern::NewLC(KHostletEndpoint1, KNullDesC8);
    pPattern1->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection1 = CSenServiceConnection::NewL(*ipConsumer1, *pPattern1);
    CleanupStack::PopAndDestroy(pPattern1);
    
    // Construct service connection
    StartActiveScheduler(1);    
 	// Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady	
    LOCAL_ASSERT( ipConsumer1->iConnectionStatus == KSenConnectionStatusReady);     
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");
    _LIT8(KRequest1, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob1\'] or containts[ab:N/ab:GIVEN,\'Bob1\'] or containts[ab:TEL,\'Bob1\']]</ab:Select></ab:QueryItem></ab:Query>");

	ipUsedHostletConnection = ipHostletConnection;
    TInt transactionIdOrError = ipConnection->SendL(KRequest);
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);
    
    ipUsedHostletConnection = ipHostletConnection1;
    transactionIdOrError = ipConnection1->SendL(KRequest1);
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);
 
    // ---- Following conditions MUST be met ----
	
	// First pair of the HC and SC
    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
	
	// Second pair of the HC and SC
	// 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet1->iRequests[0] == KRequest1);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer1->iResponses[0] == *ipHostlet1->iResponses[0]);
    LOCAL_ASSERT( ipConsumer1->iResponseCodes[0] == ipHostlet1->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet1->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet1->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
	
    // --- Conditions END ---
    
	ipUsedHostletConnection = NULL;  
	
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;

    delete ipConnection1;
    ipConnection1 = NULL;
    delete ipHostletConnection1; 
    ipHostletConnection1 = NULL;
    delete ipHostlet1;
    ipHostlet1 = NULL;
    delete ipConsumer1;
    ipConsumer1 = NULL;
    Teardown();
    return KErrNone;
    }

    

TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_DeleteSC_before_HCRespondLL(TTestResult& aResult)
    {
	SetupL(); 
	iWas_OnServiceL_Accessed = false;     
    ipHostlet = CTestHostlet::NewL(this,
                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnService1L,
                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceComplete1L);

    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror



    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    // Construct service connection
    StartActiveScheduler(1);
    
    // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer->iConnectionStatus == KSenConnectionStatusReady); 
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");

    TInt transactionIdOrError = ipConnection->SendL(KRequest);
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(1);
    

    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);
    
	// 2. onServiceL accesed
    LOCAL_ASSERT( iWas_OnServiceL_Accessed ); 

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] < KErrNone ); 

    // --- Conditions END ---
 
    
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL; 
    Teardown();
    return KErrNone;
    }

      
 TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_RespondL_4multipleTimesL(TTestResult& aResult)
    {
	SetupL(); 
    ipHostlet = CTestHostlet::NewL(this,
                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnService2L,
                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnServiceComplete2L);

    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror



    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    // Construct service connection
    StartActiveScheduler(1);
    
    // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer->iConnectionStatus == KSenConnectionStatusReady); 
    
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");

	
    TInt transactionIdOrError = ipConnection->SendL(KRequest);
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);
   

    // ---- Following conditions MUST be met ----
    
   
    // --- Conditions END ---
  
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL; 
    Teardown();
    return KErrNone;
    }
     



 TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_SC_Cancel_After_RequestL(TTestResult& aResult)
 {
	SetupL(); 
 	
 	iWas_HandleErrorL_Accessed = EFalse;
 	
    
    ipHostlet = CTestHostlet::NewL(this,
                                   &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_OnService3L,
                                   NULL);

    ipConsumer = CTestConsumer::NewL(this, 
                                     NULL, 
                                     &CSenHostletConnectionBCTest::UT_CSenHostletConnection_NewLL_HandleError1L, 
                                     NULL);
                                     
                                     
    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    // Construct service connection
    StartActiveScheduler(1);
    
    // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer->iConnectionStatus == KSenConnectionStatusReady); 
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");

    TInt transactionIdOrError = ipConnection->SendL(KRequest);
    iTxnID=transactionIdOrError;
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    //ipConnection->Cancel(); 
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);
    
	  	
	  	
    //
    // ---- Following conditions MUST be met ----
	
    //1 KErrSenCancelled reported
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == KErrSenCancelled);
    
    //2. HandleErrorL accesed after negative iResponseCodes
    LOCAL_ASSERT( iWas_HandleErrorL_Accessed );
    // --- Conditions END ---
    
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    
    Teardown();
    return KErrNone;
    }

    
 TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_Set_Expose_FacetL(TTestResult& aResult)
 {
	SetupL(); 
    
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    
	CSenFacet* pExposed = CSenFacet::NewL();
    CleanupStack::PushL(pExposed);
    pExposed->SetNameL(KExposeLocalServiceFacet); // from MSenServiceDescription.h
    ipHostlet->ipHostletDescription->SetFacetL(*pExposed);
    CleanupStack::PopAndDestroy(); // pExposed
    
    RFacetArray facets;
	ipHostlet->ipHostletDescription->FacetsL(facets);
	
	LOCAL_ASSERT(facets[0]->Name() == KExposeLocalServiceFacet);
    
    facets.ResetAndDestroy();
    delete ipHostlet;
    ipHostlet = NULL;
      
    Teardown();
    return KErrNone;
    }
    
/*
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_CreateConnAndSubmit_Rest_MultiThreadL(TTestResult& aResult)
    {
	SetupL(); 
    // uses default implementations for servicel and onservicecompletel    
    ipHostlet = CTestHostlet::NewL(this);   
    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);
   
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");
	
	HBufC8* pResponseTo = NULL;

    // Invoke service once. But, due this is performed synchronously within the SAME PROCESS, we need
    // another thread (otherwise ActiveScheduler cannot serve this WS request, ServiceL is never invoked).
    
    TConsumerTestTask task( *ipConnection, 
                            KRequest, 
                            KDefaultRestServicesFrameworkID, 
                            TConsumerTestTask::ECreateConnAndSubmit, 
                            pResponseTo );
    task.ipOwner = this;
    
    RThread localThread;
    _LIT(KThreadName, "tSubmitRestFromThread_");
    
    TInt createThreadRetVal = CreateThreadForConsumerTask(localThread, KThreadName, task);
    
    LOCAL_ASSERT( createThreadRetVal == KErrNone );
    
    localThread.Resume(); // start the thread. 
    
    // Would NOT work: // TInt transactionIdOrError = ipConnection->SubmitL(KRequest, pResponseTo);
                       // LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    
    // Respond to consumer ONCE => only one call..
    StartActiveScheduler(1);
    
    // Push the local pointer to the response descriptor into cleanupstack:
    CleanupStack::PushL(pResponseTo);

    // ---- Following conditions MUST be met ----

    // SubmitL must return KErrNone(!)
    // Note: from Submit, one cannot get TXN ID, but either OK (KErrNone) or an error
 //   LOCAL_ASSERT( task.iStatusCode == KErrNone );


    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *pResponseTo == *ipHostlet->iResponses[0]);
    

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    // --- Conditions END ---
    
    CleanupStack::PopAndDestroy(pResponseTo);
    
    localThread.Close();
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    Teardown();
    return KErrNone;
    }
*/    
    

TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_Submit_Rest_MultiThreadL(TTestResult& aResult)
    {
	SetupL(); 
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror


    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    // Construct service connection
    StartActiveScheduler(1);
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");
	
	HBufC8* pResponseTo = NULL;

    // Invoke service once. But, due this is performed synchronously within the SAME PROCESS, we need
    // another thread (otherwise ActiveScheduler cannot serve this WS request, ServiceL is never invoked).
    
    TConsumerTestTask task( *ipConnection, 
                            KRequest, 
                            KDefaultRestServicesFrameworkID, // not used by this test case..
                            TConsumerTestTask::ESubmit, 
                            pResponseTo);
    
    RThread localThread;
    _LIT(KThreadName, "tSubmitRestFromThread_");
    
    TInt createThreadRetVal = CreateThreadForConsumerTask(localThread, KThreadName, task);
    
    LOCAL_ASSERT( createThreadRetVal == KErrNone );
    
    localThread.Resume(); // start the thread. 
    
    // Would NOT work: // TInt transactionIdOrError = ipConnection->SubmitL(KRequest, pResponseTo);
                       // LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Respond to consumer ONCE => only one call..
    StartActiveScheduler(1);
    
    // Push the local pointer to the response descriptor into cleanupstack:
    CleanupStack::PushL(pResponseTo);
    
    // ---- Following conditions MUST be met ----

    // SubmitL must return KErrNone(!)
    // Note: from Submit, one cannot get TXN ID, but either OK (KErrNone) or an error
    LOCAL_ASSERT( task.iStatusCode == KErrNone ); 


    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *pResponseTo == *ipHostlet->iResponses[0]);
    

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    // --- Conditions END ---

    CleanupStack::PopAndDestroy(pResponseTo);    
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    Teardown();
    return KErrNone;
    }
    

TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_Submit_Rest_ActiveL(TTestResult& aResult)
    {
	SetupL(); 
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror


    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);
    CSenServicePattern* pPattern = CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
  
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    // Construct service connection
    StartActiveScheduler(1);
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");
	
	HBufC8* pResponseTo = NULL;

    // Invoke service once. But, due this is performed synchronously within the SAME PROCESS, we need
    // another thread (otherwise ActiveScheduler cannot serve this WS request, ServiceL is never invoked).
    
    TConsumerTestTask task( *ipConnection, 
                            KRequest, 
                            KDefaultRestServicesFrameworkID, // not used by this test case atm
                            TConsumerTestTask::EActiveSubmit, 
                            pResponseTo);

    _LIT(KThreadName, "tSubmitRestFromAO_");
    CActiveConsumerTestTask* pActiveTask = CActiveConsumerTestTask::NewLC(task, KThreadName(), *this); // push pActiveTask
    task.ipActive = pActiveTask;
    
    if(!pActiveTask->IsActive())
        {
	SetupL(); 
        pActiveTask->SetActive();
        Teardown();
    return KErrNone;
    }

    pActiveTask->iStatus = KRequestPending;
    TRequestStatus* status = &pActiveTask->iStatus;
    User::RequestComplete( status, CActiveConsumerTestTask::EInvokeService );
    
    // Would NOT work: // TInt transactionIdOrError = ipConnection->SubmitL(KRequest, pResponseTo);
                       // LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Submit a single request (via help of an AO!), respond to consumer ONCE,  => two calls
    StartActiveScheduler(2);

    // Push the local pointer to the response descriptor into cleanupstack:
    CleanupStack::PushL(pResponseTo);

    // ---- Following conditions MUST be met ----

    // SubmitL must return KErrNone(!)
    // Note: from Submit, one cannot get TXN ID, but either OK (KErrNone) or an error
    LOCAL_ASSERT( task.iStatusCode == KErrNone );


    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *pResponseTo == *ipHostlet->iResponses[0]);
    

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    // --- Conditions END ---

    CleanupStack::PopAndDestroy(pResponseTo);
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    
    Teardown();
    return KErrNone;
    }
    
/*
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_CreateConnAndSubmit_WSI_MultiThreadL(TTestResult& aResult)
    {
	SetupL(); 
    // This test case uses default implementations for servicel and onservicecompletel, 
    // BUT(!) describeservicel has been overridden!
    ipHostlet = CTestHostlet::NewL(this, NULL, NULL, CSenHostletConnectionBCTest::UT_CSenHostletConnection_WSI_DescribeServiceL);   

// variant point 1:
//    ipHostlet = CTestHostlet::NewL(this);   
    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);
   
	CSenSoapMessage* pSoapMessage = CreateAuthRequestLC();
	HBufC8* pAsXml = pSoapMessage->AsXmlL();
	CleanupStack::PushL(pAsXml);
	TPtrC8 request = pAsXml->Des();
	
	CSenTransportProperties* pProps = CSenTransportProperties::NewLC(); // push pProps
	pProps->SetPropertyL(_L8("KTestProperty"), _L8("KTestPropertyValue"));
	HBufC8* pPropsAsXml = pProps->AsUtf8L();
	CleanupStack::PopAndDestroy(pProps);

	CleanupStack::PushL(pPropsAsXml);
	TPtrC8 properties = pPropsAsXml->Des();

	
	HBufC8* pResponseTo = NULL;

    // Invoke service once. But, due this is performed synchronously within the SAME PROCESS, we need
    // another thread (otherwise ActiveScheduler cannot serve this WS request, ServiceL is never invoked).
    

    // NOTE-NOTE: if WS-I is used instead of REST, timing changes and race-codition occurs
    
    TConsumerTestTask task( *ipConnection, // not used in this test case
                            request,       // not used in this test case
                            KDefaultBasicWebServicesFrameworkID,
                            TConsumerTestTask::ECreateConnAndSubmit, 
                            pResponseTo);
    task.ipOwner = this;

    task.iEndpoint.Set(KHostletEndpoint);
// variant point 2:
    // -- comment out above line: task.iEndpoint.Set(KHostletEndpoint);

    task.ipSoapMsg = pSoapMessage;

    task.iProperties.Set(properties); 
    
    RThread localThread;
    _LIT(KThreadName, "tSubmitWsiFromThread_");
    
    TInt createThreadRetVal = CreateThreadForConsumerTask(localThread, KThreadName, task);
    
    LOCAL_ASSERT( createThreadRetVal == KErrNone );
    
    // Now start the thread that will execute SubmitL:
    localThread.Resume(); // start the thread
    
    // Would NOT work: // TInt transactionIdOrError = ipConnection->SubmitL(KRequest, pResponseTo);
                       // LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Respond to consumer ONCE => only one call..
    StartActiveScheduler(1);

    
    // Push the local pointer to the response descriptor into cleanupstack:
    CleanupStack::PushL(pResponseTo);

    // ---- Following conditions MUST be met ----

    // SubmitL must return KErrNone(!)
    // Note: from Submit, one cannot get TXN ID, but either OK (KErrNone) or an error
    //LOCAL_ASSERT( task.iStatusCode == KErrNone );


    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == request);

    // 2. Consumer receives response that hostlet provided
    //LOCAL_ASSERT( *pResponseTo == *ipHostlet->iResponses[0]);
    

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    // --- Conditions END ---
    
    // Response received, de-alloc response, request properties, request as XML descs and SOAP-MSG obj
    CleanupStack::PopAndDestroy( pResponseTo );

    CleanupStack::PopAndDestroy( pPropsAsXml );

	CleanupStack::PopAndDestroy( pAsXml );

	CleanupStack::PopAndDestroy( pSoapMessage );
    
    localThread.Close();
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    Teardown();
    return KErrNone;
    }
    
*/

    

/*    
// DOES NOT WORK, submit blocks the single ActiveScheduler...    
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_Submit_WSIL(TTestResult& aResult)
    {
	SetupL(); 
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror

    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern =
         CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultBasicWebServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    // Construct service connection
    StartActiveScheduler(1);
    
     // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer->iConnectionStatus == KSenConnectionStatusReady); 
    
	CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();
	HBufC8* pAsXml = pSOAPMessage->AsXmlL(); 
	CleanupStack::PushL(pAsXml);
	
	CSenTransportProperties* pProps = CSenTransportProperties::NewLC();
	pProps->SetPropertyL(_L8("KTestProperty"), _L8("KTestPropertyValue"));
	HBufC8* pPropsAsXml = pProps->AsUtf8LC();
	
	HBufC8* pResponseTo = NULL;
	
    TInt transactionIdOrError = ipConnection->SubmitL(*pSOAPMessage, *pPropsAsXml, pResponseTo);
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);

    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == *pAsXml);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *pResponseTo == *ipHostlet->iResponses[0]);
    
    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    // --- Conditions END ---
    
    CleanupStack::PopAndDestroy(pPropsAsXml);
    CleanupStack::PopAndDestroy(pProps);
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pSOAPMessage);
	delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
    
    Teardown();
    return KErrNone;
    }
    
*/
/*
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_Send_WSIL()
    {
	SetupL(); 
   // This case uses default implementations for servicel and onservicecompletel:
    ipHostlet = CTestHostlet::NewL(this, NULL, NULL, CSenHostletConnectionBCTest::UT_CSenHostletConnection_WSI_DescribeService_2L);

    // This case uses the default implementations for setstatus, handlemessage, handleerror:
    ipConsumer = CTestConsumer::NewL(this); 

    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern = CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    // Choose WS-I service invokation framework
    pPattern->SetFrameworkIdL(KDefaultBasicWebServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    // Construct service connection
    StartActiveScheduler(1);

    // Check that NO HandleErrorL callback was received:
    LOCAL_ASSERT( ipConsumer->iErrorCode == KErrSenHostNotAvailable );

    // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer->iConnectionStatus == KSenConnectionStatusCreateFailed); 
    
/*
    // Check that NO HandleErrorL callback was received:
    LOCAL_ASSERT( ipConsumer->iErrorCode == KErrNone );
    
    
     // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer->iConnectionStatus == KSenConnectionStatusReady); 
    
	CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();
	HBufC8* pAsXml = pSOAPMessage->AsXmlL(); 
	CleanupStack::PushL(pAsXml);
	TPtrC8 request = pAsXml->Des();
	
    TInt transactionIdOrError = ipConnection->SendL(*pSOAPMessage);
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one SendL calling ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);

    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == request);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
   
    
    // --- Conditions END ---

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pSOAPMessage);
     */
     /*
	delete ipConnection;
    ipConnection = NULL;
    
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    
    delete ipConsumer;
    ipConsumer = NULL;
    
    delete ipHostlet;
    ipHostlet = NULL;
    
    Teardown();
    return KErrNone;
    }

    */
/*
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_DescEndpoint_Send_WSIL()
    {
	SetupL(); 
    // This case uses default implementations for servicel and onservicecompletel:
    // NOTE: for ALL OTHER BUT REST hostlets, one must DESCRIBE framework ID
    // via MSenHostlet::DescribeServiceL(!)
    ipHostlet = CTestHostlet::NewL(this, NULL, NULL, CSenHostletConnectionBCTest::UT_CSenHostletConnection_WSI_DescribeServiceL);

    // This case uses the default implementations for setstatus, handlemessage, handleerror:
    ipConsumer = CTestConsumer::NewL(this); 

    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    CSenServicePattern* pPattern = CSenServicePattern::NewLC(KHostletEndpoint, KHostletContract);
    // Choose WS-I service invokation framework
    pPattern->SetFrameworkIdL(KDefaultBasicWebServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    // Construct service connection
    StartActiveScheduler(1);

    // Check that NO HandleErrorL callback was received:
    LOCAL_ASSERT( ipConsumer->iErrorCode == KErrNone );
    
    
     // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer->iConnectionStatus == KSenConnectionStatusReady); 
    
	CSenSoapMessage* pSOAPMessage = CreateAuthRequestLC();
	HBufC8* pAsXml = pSOAPMessage->AsXmlL(); 
	CleanupStack::PushL(pAsXml);
	TPtrC8 request = pAsXml->Des();
	
    TInt transactionIdOrError = ipConnection->SendL(*pSOAPMessage);
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one SendL calling ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);

    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == request);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    
    // --- Conditions END ---

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pSOAPMessage);
    
	delete ipConnection;
    ipConnection = NULL;
    
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    
    delete ipConsumer;
    ipConsumer = NULL;
    
    delete ipHostlet;
    ipHostlet = NULL;
    
    Teardown();
    return KErrNone;
    }


*/

// Leaks memory
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_2SC1HCL(TTestResult& aResult)
    {
    	//Memory leak detection is not supported since Symbian 9.0 (EKA2 kernel) environments.
    	//This this is added to handle thread handle leak in testmodule0
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );
	SetupL(); 
   
    // The only HC
    ipHostlet = CTestHostlet::NewL(this);   // uses default implementations for servicel and onservicecompletel
    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

	
	// First SC
	ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror
	    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    
    // Construct service connection
    StartActiveScheduler(1);
    
    // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer->iConnectionStatus == KSenConnectionStatusReady); 

	// Second SC
    ipConsumer1 = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror
    CSenServicePattern* pPattern1 =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern1->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    ipConnection1 = CSenServiceConnection::NewL(*ipConsumer1, *pPattern1);
    CleanupStack::PopAndDestroy(pPattern1);
    
    // Construct service connection
    StartActiveScheduler(1); 
    
    // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer1->iConnectionStatus == KSenConnectionStatusReady); 
    
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");
    _LIT8(KRequest1, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob1\'] or containts[ab:N/ab:GIVEN,\'Bob1\'] or containts[ab:TEL,\'Bob1\']]</ab:Select></ab:QueryItem></ab:Query>");

	
    TInt transactionIdOrError = ipConnection->SendL(KRequest);
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);
    
    TInt transactionIdOrError1 = ipConnection1->SendL(KRequest1);
    LOCAL_ASSERT(transactionIdOrError1 > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);
 
    // ---- Following conditions MUST be met ----
	
	// First SC
    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. The 1st Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])

    // 3. Hostlet receives completion code from 1st Consumer (delivery code)
    //We should check TxnID as after Dispatcher Implementation VTxnID get return from SendL
    //LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
	
	// Second SC
	// 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[1] == KRequest1);

    // 2. The 2nd Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer1->iResponses[0] == *ipHostlet->iResponses[1]);
    LOCAL_ASSERT( ipConsumer1->iResponseCodes[0] == ipHostlet->iResponseCodes[1])

    // 3. Hostlet receives completion code from 2nd Consumer (delivery code)
   // LOCAL_ASSERT( ipHostlet->iTxnIds[1] == transactionIdOrError1 );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[1] == KErrNone ); // true in this "OK" -test case
	
    // --- Conditions END ---
    delete ipHostletConnection; 
    ipHostletConnection = NULL;

    delete ipHostlet;
    ipHostlet = NULL;

    delete ipConnection;
    ipConnection = NULL;

    delete ipConsumer;
    ipConsumer = NULL;

    delete ipConnection1;
    ipConnection1 = NULL;

    delete ipConsumer1;
    ipConsumer1 = NULL;
    
    Teardown();
    return KErrNone;
    }
 
    
TInt CSenHostletConnectionBCTest::UT_CSenHostletConnection_RespondWithPropertiesL(TTestResult& aResult)
    {
	SetupL(); 
    // Try to send one message  from ServiceConnection to HostletConnection,
    // which provides response (TRANSPORT) properties to the response message
    
    // Construct Hostlet
    
    ipHostlet = CTestHostlet::NewL(this, &CSenHostletConnectionBCTest::UT_CSenHostletConnection_RespondWithPropertiesL_OnServiceL);
    ipHostletConnection = CSenHostletConnection::NewL(*ipHostlet);

    // Construct ServiceConnection
    ipConsumer = CTestConsumer::NewL(this); // uses the default implementations for setstatus, handlemessage, handleerror
    CSenServicePattern* pPattern =
        CSenServicePattern::NewLC(KNullDesC8, KHostletContract);
    pPattern->SetFrameworkIdL(KDefaultRestServicesFrameworkID);
    
    ipConnection = CSenServiceConnection::NewL(*ipConsumer, *pPattern);
    CleanupStack::PopAndDestroy(pPattern);
    StartActiveScheduler(1);
    
    // Check that the Status of the ServiceConnection
    // is KSenConnectionStatusReady
    LOCAL_ASSERT( ipConsumer->iConnectionStatus == KSenConnectionStatusReady);
    
    // ServiceConnection is ready
    // => Send request to the Hostlet
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");
    TInt transactionIdOrError = ipConnection->SendL(KRequest);
    
    // Check that SendL returned transaction Id
    LOCAL_ASSERT(transactionIdOrError > KErrNone);
    
    // Invoke service  ONCE (one call to ServiceL), respond to consumer ONCE => two calls
    StartActiveScheduler(2);

    // ---- Following conditions MUST be met ----

    // 1. Hostlet receives request that consumer made
    LOCAL_ASSERT( *ipHostlet->iRequests[0] == KRequest);

    // 2. Consumer receives response that hostlet provided
    LOCAL_ASSERT( *ipConsumer->iResponses[0] == *ipHostlet->iResponses[0]);
    LOCAL_ASSERT( ipConsumer->iResponseCodes[0] == ipHostlet->iResponseCodes[0])

    // 3. Hostlet receives completion code (delivery code)
    LOCAL_ASSERT( ipHostlet->iTxnIds[0] == transactionIdOrError );
    LOCAL_ASSERT( ipHostlet->iCompletionCodes[0] == KErrNone ); // true in this "OK" -test case
    
    MSenProperties* pHostletProps = ipHostlet->iResponseProps[0];
    
    LOCAL_ASSERT( ipHostlet->iResponseProps[0] != NULL ); // The tp that was sent
    LOCAL_ASSERT( ipConsumer->iResponseProperties[0] != NULL); // The tp that was received
    
    
    
    
    HBufC8* pSerializedPropsSentByHC = ipHostlet->iResponseProps[0]->AsUtf8LC(); // push
    HBufC8* pSerializedPropsReceivedBySC = ipConsumer->iResponseProperties[0]->AsUtf8LC(); // push

    LOCAL_ASSERT( *pSerializedPropsSentByHC == *pSerializedPropsReceivedBySC );
    
    /*EUNIT_PRINT(_L("=> Properties that HC sent:"));*/
    RDebug::Print (_L("=> Properties that HC sent:"));
    RDebug::RawPrint(*pSerializedPropsSentByHC);
    RDebug::Print(_L("=> Properties that SC received:"));
    RDebug::RawPrint(*pSerializedPropsReceivedBySC);
    
    CleanupStack::PopAndDestroy(pSerializedPropsReceivedBySC); // SC's TP AsUtf8LC
    CleanupStack::PopAndDestroy(pSerializedPropsSentByHC); // HC's TP AsUtf8LC
    
    
    // --- Conditions END ---
    
    delete ipConnection;
    ipConnection = NULL;
    delete ipHostletConnection; 
    ipHostletConnection = NULL;
    delete ipHostlet;
    ipHostlet = NULL;
    delete ipConsumer;
    ipConsumer = NULL;
   	Teardown();
    return KErrNone;
    }
    
           
    
CSenSoapMessage* CSenHostletConnectionBCTest::CreateAuthRequestLC()
    {
    _LIT8(KRequest, "<ab:Query xmlns:ab=\"urn:nokia:test:addrbook:2004-09\"><ab:QueryItem><ab:Select>/ab:Card[containts[ab:N/ab:FAMILY,\'Bob\'] or containts[ab:N/ab:GIVEN,\'Bob\'] or containts[ab:TEL,\'Bob\']]</ab:Select></ab:QueryItem></ab:Query>");
    CSenSoapMessage* pSOAPMessage = CSenSoapMessage::NewL();
    CleanupStack::PushL(pSOAPMessage);

    //xxxxxxxx*************** CorrelationHeader starts ***************************
    CSenBaseElement* pElement = CSenBaseElement::NewL(KHostletContract, _L8("Correlation"), _L8("sb:Correlation"));
    CleanupStack::PushL(pElement);

    pElement->AddAttrL(_L8("messageID"),_L8(""));
    pElement->AddAttrL(_L8("refToMessageID"),_L8(""));

    TTime t;
    t.HomeTime();
    TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts;
    SenDateUtils::ToXmlDateTimeUtf8L(ts, t);
    pElement->AddAttrL(_L8("timestamp"), ts);

    pSOAPMessage->AddHeaderL(*pElement); // pElement will be owned by pSOAPMessage
    CleanupStack::Pop(pElement);
    //************************ CorrelationHeader ends ****************************

    pSOAPMessage->SetBodyL(KRequest);
    return pSOAPMessage;    
    }
//  END OF FILE
