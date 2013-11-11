/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include <ss_glob.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
//#include <elements/nm_messages_child.h>

#include "pudpclienttests.h"	
#include "upnp_cf_msgs.h"
#include "upnpflowbase.h"
#include "upnpflowfactory.h"
#include "httpudpflow.h"

//const TUint16 KActivityNull = 0;

//Add Blocks of 64
static const TInt KBlockSize1 = 64;
static const TInt KMinGrowth1 = 30;
static const TInt KThresholdGrowth1 = 30;
static const TInt KInitialAllocation1 = 128;

static const TInt KHeapSize	= 1024 * 1024;	//1MB

using namespace Messages;


//-------------------------------------------------------------------------------------------------
//	CUdpClientTestBase
//-------------------------------------------------------------------------------------------------	
 
#define BEGIN_TESTLIST

#define ADD_TEST(name) \
	if (aTestName == _L(#name))\
		{\
		return CUdpClient ## name::NewL(aProvd);\
		}
	
#define END_TESTLIST \
	User::Leave(KErrNotSupported);\
	return NULL;	
	
CUdpClientTestBase* CUdpClientTestBase::CreateTestL(const TDesC& aTestName, CProxyProvd* aProvd)
	{
	BEGIN_TESTLIST
		ADD_TEST(TestUPnP)		
		ADD_TEST(TestUPnP1)
		ADD_TEST(TestUPnP2)
		ADD_TEST(TestUPnP3)
		ADD_TEST(TestUPnP4)
		ADD_TEST(TestUPnP5)
		ADD_TEST(TestUPnP6)
		ADD_TEST(TestUPnP7)
		ADD_TEST(TestUPnP8)
		ADD_TEST(TestUPnP9)
		ADD_TEST(TestUPnP10)
		ADD_TEST(TestUPnP11)
		ADD_TEST(TestUPnP12)
		
	END_TESTLIST
	}


void CUdpClientTestBase::StartTestL(const TDesC& aTestName, CProxyProvd* aProvd)
	{
	CUdpClientTestBase* test = CreateTestL(aTestName,aProvd);
	CleanupStack::PushL(test);
	test->CUdpClientTestBase::ConstructL();
	CActiveScheduler::Add(test);
	CleanupStack::Pop(test);
	TRequestStatus* stat = &test->iStatus;
	test->SetActive();
	User::RequestComplete(stat,KErrNone);
	}

CUdpClientTestBase::CUdpClientTestBase(CProxyProvd* aProvd) :
	CActive(EPriorityNormal),
	iProvd(aProvd)
	{}
	
void CUdpClientTestBase::ConstructL()
	{
	//Create the Chunk Manager
	CChunkManager* chkmgr = CChunkManager::NewL ( KHeapSize );
	if ( chkmgr )
		{
		chkmgr->AddPoolL ( KBlockSize1, KInitialAllocation1, KMinGrowth1, KThresholdGrowth1 );
		}
	
	iChunkManager = chkmgr;
	iAllocator.SetChunkManager(iChunkManager);
	SetLoggerL();
	}
	
CUdpClientTestBase::~CUdpClientTestBase()
	{
	iLogger.Close();
	delete iChunkManager;
	}

/**	Override this function if you want cleanup to happen before the completion of
	the test case. This function is called before the test is completed 
 */
void CUdpClientTestBase::Cleanup()
	{}

/** Sets the test case as active again and prevents the object from being destroyed
	when the RunL completes.
	
	Note: The active object is not complete the request again automatically, if this behaviour is
	desired the user must call User::RequestComplete
 */
void CUdpClientTestBase::Reschedule()
	{
	SetActive();
	iReschedule = TRUE;
	}
	
void CUdpClientTestBase::CompleteSelf ( TInt aError )
	{
	TRequestStatus* pStat = &iStatus;
    User::RequestComplete ( pStat, aError );
	}	

/** Attaches the iLogger variable to the TestExecute Log Server. This enable this prt to write to
	the testexecute logs as if it were running directly from TestExecute
 */
// 	SetLoggerL() is taken directly from CTestServer::StartLoggerL() in the TEF src TestServerBase.Cpp


void CUdpClientTestBase::SetLoggerL()
	{
    	// Create a cinidata object for parsing the testexecute.ini
	CTestExecuteIniData* parseTestExecuteIni = NULL;
	TBuf<KMaxTestExecuteNameLength> resultFilePath;
	TBuf<KMaxTestExecuteNameLength> xmlFilePath;
	TInt logMode;
	TInt logLevel;
	
	TRAPD(err,parseTestExecuteIni = CTestExecuteIniData::NewL());
	if (err == KErrNone)
		{
		CleanupStack::PushL(parseTestExecuteIni);
		parseTestExecuteIni->ExtractValuesFromIni();
		parseTestExecuteIni->GetKeyValueFromIni(KTEFHtmlKey, resultFilePath);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFXmlKey, xmlFilePath);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFLogMode, logMode);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFLogSeverityKey, logLevel);
		}
	else
		{
		resultFilePath.Copy(KTestExecuteLogPath);
		xmlFilePath.Copy(KTestExecuteLogPath);
		logMode = TLoggerOptions(ELogHTMLOnly);
		logLevel = RFileFlogger::TLogSeverity(ESevrAll);
		}
	Logger().SetLoggerOptions(logMode);
		
	// Initialise a handle to the file logger
	User::LeaveIfError(Logger().Connect());
	RFs fS;
	User::LeaveIfError(fS.Connect());
	CleanupClosePushL(fS);
	RFile file;
	TBuf<KMaxTestExecuteNameLength> xmlLogFile(xmlFilePath);
	TBuf<KMaxTestExecuteNameLength> logFile;
	TBuf<KMaxTestExecuteNameLength> logFileNameFile(resultFilePath);
	logFileNameFile.Append(KTestExecuteScheduleTestLogCompatibilityNameFile);
	if(file.Open(fS,logFileNameFile,EFileRead | EFileShareAny) != KErrNone)
		{
		// For the old flogger we have to create an individual file
		_LIT(KTxtLog,"TEIntSock.txt");
		logFile.Copy(KTxtLog);
		logMode = TLoggerOptions(0);
		Logger().SetLoggerOptions(logMode);
		}
	else
		{
		CleanupClosePushL(file);
		TBuf8<KMaxTestExecuteNameLength> logFile8;
		TInt fileSize;
		User::LeaveIfError(file.Size(fileSize));
		User::LeaveIfError(file.Read(logFile8,fileSize));
		logFile.Copy(logFile8);
		xmlLogFile.Append(logFile);
		_LIT(KXmlExtension,".xml");
		xmlLogFile.Append(KXmlExtension);
		_LIT(KHtmExtension,".htm");
		logFile.Append(KHtmExtension);
		CleanupStack::Pop(&file);
		file.Close();
		}
	TBuf<KMaxTestExecuteLogFilePath> logFilePath(resultFilePath);
	logFilePath.Append(logFile);
	CleanupStack::Pop(&fS);
	fS.Close();
	
	if (logMode == 0 || logMode == 2)
		{
		User::LeaveIfError(Logger().HtmlLogger().CreateLog(logFilePath,	RTestExecuteLogServ::ELogModeAppend));
		}
	if (logMode == 1 || logMode == 2)
		{
		User::LeaveIfError(Logger().XmlLogger().CreateLog(xmlLogFile,RFileFlogger::ELogModeAppend));
		Logger().XmlLogger().SetLogLevel(RFileFlogger::TLogSeverity(logLevel));
		}
	if (parseTestExecuteIni != NULL)
		{
		CleanupStack::PopAndDestroy(parseTestExecuteIni);
		}
	}
	

	
/** Called by the active scheduler in order to run the test 
 */
void CUdpClientTestBase::RunL()
	{
	iReschedule = EFalse;
	TVerdict res = RunTestL();
	if (!iReschedule)
		{
		Cleanup();
		FinishTest(res);
		delete this;
		return;
		}
	}
	
void CUdpClientTestBase::FinishTest(TVerdict aResult)
	{
	iProvd->TestComplete(aResult);
	}
	
void CUdpClientTestBase::DoCancel()
	{
	Cleanup();
	}
	
//------------------------------------------------------------//	
/////////////////// CUdpClientTestUPnP /////////////////////////
////// Testcase: Send the test data from the test client/////////
//------------------------------------------------------------//

CUdpClientTestUPnP* CUdpClientTestUPnP::NewL ( CProxyProvd* aProvd )
	{
	CUdpClientTestUPnP* ret = new ( ELeave ) CUdpClientTestUPnP ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpClientTestUPnP::CUdpClientTestUPnP ( CProxyProvd* aProvd ) :
	CUdpClientTestBase(aProvd), iTestServer(NULL), iState ( ECreateTestServer )
	{
	}

CUdpClientTestUPnP::~CUdpClientTestUPnP ()
	{
	iTimer.Close ();
	}

void CUdpClientTestUPnP::ConstructL()
	{
	iTimer.CreateLocal ();
	}

TVerdict CUdpClientTestUPnP::RunTestL()
	{//This part of the class creates a dummy server and client
	//The client sends a m-search request to the server
	//and the server responds with a Discovery Responses...this is the point where the test cases come into play
	//every test case send an invalid request to the client using udpflow
	//the client validates these responses
	const TUint16 KActivityNull = 0;
	switch ( iState )
		{
		case ECreateTestServer:
			{
			iLogger.WriteFormat(_L("<i>Creating TestServer..... </i>"));		
			
			iTestServer = CTestMUdpServer::NewL ( *this );
			
			iState  = ECreateTestClient;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		
		case ECreateTestClient:
			{
			iLogger.WriteFormat(_L("<i>TestServer is created..... </i>"));			
			iLogger.WriteFormat(_L("<i>Creating TestClient..... </i>"));
			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpUdpFlow, iChunkManager);
			const TUid requestedUid = { CUPnPFlowFactory::iUid };	
			
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
						
			RClientInterface::OpenPostMessageClose ( NodeId (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = ESendRequestData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		
		case ESendRequestData:
			{
			iLogger.WriteFormat(_L("<i>Client is Created</i>"));
			iLogger.WriteFormat(_L("<i>Send data..... </i>"));
			
			RMemChunk st;
			st.CreateL ( KTestCase1, iAllocator );	//KTestCase1 = "testcase1"
			TSsdpInfo searchInfo ( 5, st ); // MX , ST
			TUpnpMessage::TUPnPSearchRequest msg ( searchInfo );
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg );
			
			iTimer.After ( iStatus, 5000000 ); // 5secs
			
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}

		case ECleanup:		
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);
			return EPass;
			}
			
		case EFailed:	
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			FinishTest ( EFail );
			delete this;
			return EFail;
			}

		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
void CUdpClientTestUPnP::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}
	
void CUdpClientTestUPnP::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iClientId = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchResponse > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}
	
	
CUdpClientTestUPnP1* CUdpClientTestUPnP1::NewL ( CProxyProvd* aProvd )
	{
	CUdpClientTestUPnP1* ret = new ( ELeave ) CUdpClientTestUPnP1 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpClientTestUPnP1::CUdpClientTestUPnP1 ( CProxyProvd* aProvd ) :
	CUdpClientTestBase(aProvd), iTestServer(NULL), iState ( ECreateTestServer )
	{
	}

CUdpClientTestUPnP1::~CUdpClientTestUPnP1 ()
	{
	iTimer.Close ();
	}

void CUdpClientTestUPnP1::ConstructL()
	{
	iTimer.CreateLocal ();
	}

TVerdict CUdpClientTestUPnP1::RunTestL()
	{
	const TUint16 KActivityNull = 0;
	switch ( iState )
		{
		case ECreateTestServer:
			{
			iLogger.WriteFormat(_L("<i>Creating TestServer..... </i>"));
			
			iTestServer = CTestMUdpServer::NewL ( *this );
			
			iState  = ECreateTestClient;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		
		case ECreateTestClient:
			{
			iLogger.WriteFormat(_L("<i>TestServer is created..... </i>"));			
			iLogger.WriteFormat(_L("<i>Creating TestClient..... </i>"));
			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpUdpFlow, iChunkManager);
			const TUid requestedUid = { CUPnPFlowFactory::iUid };	
			
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
						
			RClientInterface::OpenPostMessageClose ( NodeId (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = ESendRequestData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		
		case ESendRequestData:
			{
			iLogger.WriteFormat(_L("<i>Client is Created</i>"));
			iLogger.WriteFormat(_L("<i>Send data..... </i>"));
			
			RMemChunk st;
			st.CreateL ( KTestCase2, iAllocator );	//KTestCase2 = "testcase2"
			TSsdpInfo searchInfo ( 5, st ); // MX , ST
			TUpnpMessage::TUPnPSearchRequest msg ( searchInfo );
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg );
			
			iTimer.After ( iStatus, 5000000 ); // 5secs
			
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}

		case ECleanup:		
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			return EPass;
			}
			
		case EFailed:	
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
void CUdpClientTestUPnP1::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}
	
void CUdpClientTestUPnP1::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iClientId = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchResponse > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}	



//---------------------------------------------------------------------------------------------------------------------------------



CUdpClientTestUPnP2* CUdpClientTestUPnP2::NewL ( CProxyProvd* aProvd )
	{
	CUdpClientTestUPnP2* ret = new ( ELeave ) CUdpClientTestUPnP2 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpClientTestUPnP2::CUdpClientTestUPnP2 ( CProxyProvd* aProvd ) :
	CUdpClientTestBase(aProvd), iTestServer(NULL), iState ( ECreateTestServer )
	{
	}

CUdpClientTestUPnP2::~CUdpClientTestUPnP2 ()
	{
	iTimer.Close ();
	}

void CUdpClientTestUPnP2::ConstructL()
	{
	iTimer.CreateLocal ();
	}

TVerdict CUdpClientTestUPnP2::RunTestL()
	{
	const TUint16 KActivityNull = 0;
	switch ( iState )
		{
		case ECreateTestServer:
			{
			iLogger.WriteFormat(_L("<i>Creating TestServer..... </i>"));
			
			iTestServer = CTestMUdpServer::NewL ( *this );
			
			iState  = ECreateTestClient;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		
		case ECreateTestClient:
			{
			iLogger.WriteFormat(_L("<i>TestServer is created..... </i>"));			
			iLogger.WriteFormat(_L("<i>Creating TestClient..... </i>"));
			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpUdpFlow, iChunkManager);
			const TUid requestedUid = { CUPnPFlowFactory::iUid };	
			
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
						
			RClientInterface::OpenPostMessageClose ( NodeId (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = ESendRequestData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		
		case ESendRequestData:
			{
			iLogger.WriteFormat(_L("<i>Client is Created</i>"));
			iLogger.WriteFormat(_L("<i>Send data..... </i>"));
			
			RMemChunk st;
			st.CreateL ( KTestCase3, iAllocator );	//KTestCase2 = "testcase2"
			TSsdpInfo searchInfo ( 5, st ); // MX , ST
			TUpnpMessage::TUPnPSearchRequest msg ( searchInfo );
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg );
			
			iTimer.After ( iStatus, 5000000 ); // 5secs
			
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}

		case ECleanup:		
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			return EPass;
			}
			
		case EFailed:	
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
void CUdpClientTestUPnP2::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}
	
void CUdpClientTestUPnP2::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iClientId = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchResponse > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}


//---------------------------------------------------------------------------------------------------------------------------------




CUdpClientTestUPnP3* CUdpClientTestUPnP3::NewL ( CProxyProvd* aProvd )
	{
	CUdpClientTestUPnP3* ret = new ( ELeave ) CUdpClientTestUPnP3 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpClientTestUPnP3::CUdpClientTestUPnP3 ( CProxyProvd* aProvd ) :
	CUdpClientTestBase(aProvd), iTestServer(NULL), iState ( ECreateTestServer )
	{
	}

CUdpClientTestUPnP3::~CUdpClientTestUPnP3 ()
	{
	iTimer.Close ();
	}

void CUdpClientTestUPnP3::ConstructL()
	{
	iTimer.CreateLocal ();
	}

TVerdict CUdpClientTestUPnP3::RunTestL()
	{
	const TUint16 KActivityNull = 0;
	switch ( iState )
		{
		case ECreateTestServer:
			{
			iLogger.WriteFormat(_L("<i>Creating TestServer..... </i>"));
			
			iTestServer = CTestMUdpServer::NewL ( *this );
			
			iState  = ECreateTestClient;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		
		case ECreateTestClient:
			{
			iLogger.WriteFormat(_L("<i>TestServer is created..... </i>"));			
			iLogger.WriteFormat(_L("<i>Creating TestClient..... </i>"));
			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpUdpFlow, iChunkManager);
			const TUid requestedUid = { CUPnPFlowFactory::iUid };	
			
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
						
			RClientInterface::OpenPostMessageClose ( NodeId (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = ESendRequestData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		
		case ESendRequestData:
			{
			iLogger.WriteFormat(_L("<i>Client is Created</i>"));
			iLogger.WriteFormat(_L("<i>Send data..... </i>"));
			
			RMemChunk st;
			st.CreateL ( KTestCase4, iAllocator );	//KTestCase2 = "testcase2"
			TSsdpInfo searchInfo ( 5, st ); // MX , ST
			TUpnpMessage::TUPnPSearchRequest msg ( searchInfo );
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg );
			
			iTimer.After ( iStatus, 5000000 ); // 5secs
			
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}

		case ECleanup:		
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			return EPass;
			}
			
		case EFailed:	
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
void CUdpClientTestUPnP3::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}
	
void CUdpClientTestUPnP3::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iClientId = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchResponse > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}


//----------------------------------------------------------------------------------------------------------------------------





CUdpClientTestUPnP4* CUdpClientTestUPnP4::NewL ( CProxyProvd* aProvd )
	{
	CUdpClientTestUPnP4* ret = new ( ELeave ) CUdpClientTestUPnP4 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpClientTestUPnP4::CUdpClientTestUPnP4 ( CProxyProvd* aProvd ) :
	CUdpClientTestBase(aProvd), iTestServer(NULL), iState ( ECreateTestServer )
	{
	}

CUdpClientTestUPnP4::~CUdpClientTestUPnP4 ()
	{
	iTimer.Close ();
	}

void CUdpClientTestUPnP4::ConstructL()
	{
	iTimer.CreateLocal ();
	}

TVerdict CUdpClientTestUPnP4::RunTestL()
	{
	const TUint16 KActivityNull = 0;
	switch ( iState )
		{
		case ECreateTestServer:
			{
			iLogger.WriteFormat(_L("<i>Creating TestServer..... </i>"));
			
			iTestServer = CTestMUdpServer::NewL ( *this );
			
			iState  = ECreateTestClient;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		
		case ECreateTestClient:
			{
			iLogger.WriteFormat(_L("<i>TestServer is created..... </i>"));			
			iLogger.WriteFormat(_L("<i>Creating TestClient..... </i>"));
			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpUdpFlow, iChunkManager);
			const TUid requestedUid = { CUPnPFlowFactory::iUid };	
			
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
						
			RClientInterface::OpenPostMessageClose ( NodeId (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = ESendRequestData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		
		case ESendRequestData:
			{
			iLogger.WriteFormat(_L("<i>Client is Created</i>"));
			iLogger.WriteFormat(_L("<i>Send data..... </i>"));
			
			RMemChunk st;
			st.CreateL ( KTestCase5, iAllocator );	
			TSsdpInfo searchInfo ( 5, st ); // MX , ST
			TUpnpMessage::TUPnPSearchRequest msg ( searchInfo );
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg );
			
			iTimer.After ( iStatus, 5000000 ); // 5secs
			
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}

		case ECleanup:		
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			return EPass;
			}
			
		case EFailed:	
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
void CUdpClientTestUPnP4::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}
	
void CUdpClientTestUPnP4::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iClientId = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchResponse > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}
	
	
	//----------------------------------------------------------------------------------------------------------------------------
	
	


CUdpClientTestUPnP5* CUdpClientTestUPnP5::NewL ( CProxyProvd* aProvd )
	{
	CUdpClientTestUPnP5* ret = new ( ELeave ) CUdpClientTestUPnP5 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpClientTestUPnP5::CUdpClientTestUPnP5 ( CProxyProvd* aProvd ) :
	CUdpClientTestBase(aProvd), iTestServer(NULL), iState ( ECreateTestServer )
	{
	}

CUdpClientTestUPnP5::~CUdpClientTestUPnP5 ()
	{
	iTimer.Close ();
	}

void CUdpClientTestUPnP5::ConstructL()
	{
	iTimer.CreateLocal ();
	}

TVerdict CUdpClientTestUPnP5::RunTestL()
	{
	const TUint16 KActivityNull = 0;
	switch ( iState )
		{
		case ECreateTestServer:
			{
			iLogger.WriteFormat(_L("<i>Creating TestServer..... </i>"));
			
			iTestServer = CTestMUdpServer::NewL ( *this );
			
			iState  = ECreateTestClient;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		
		case ECreateTestClient:
			{
			iLogger.WriteFormat(_L("<i>TestServer is created..... </i>"));			
			iLogger.WriteFormat(_L("<i>Creating TestClient..... </i>"));
			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpUdpFlow, iChunkManager);
			const TUid requestedUid = { CUPnPFlowFactory::iUid };	
			
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
						
			RClientInterface::OpenPostMessageClose ( NodeId (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = ESendRequestData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		
		case ESendRequestData:
			{
			iLogger.WriteFormat(_L("<i>Client is Created</i>"));
			iLogger.WriteFormat(_L("<i>Send data..... </i>"));
			
			RMemChunk st;
			st.CreateL ( KTestCase6, iAllocator );	
			TSsdpInfo searchInfo ( 5, st ); // MX , ST
			TUpnpMessage::TUPnPSearchRequest msg ( searchInfo );
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg );
			
			iTimer.After ( iStatus, 5000000 ); // 5secs
			
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}

		case ECleanup:		
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			return EPass;
			}
			
		case EFailed:	
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
void CUdpClientTestUPnP5::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}
	
void CUdpClientTestUPnP5::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iClientId = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	/*
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchResponse > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	*/
	}


//-------------------------------------------------------------------------------------------------------------------------------




CUdpClientTestUPnP6* CUdpClientTestUPnP6::NewL ( CProxyProvd* aProvd )
	{
	CUdpClientTestUPnP6* ret = new ( ELeave ) CUdpClientTestUPnP6 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpClientTestUPnP6::CUdpClientTestUPnP6 ( CProxyProvd* aProvd ) :
	CUdpClientTestBase(aProvd), iTestServer(NULL), iState ( ECreateTestServer )
	{
	}

CUdpClientTestUPnP6::~CUdpClientTestUPnP6 ()
	{
	iTimer.Close ();
	}

void CUdpClientTestUPnP6::ConstructL()
	{
	iTimer.CreateLocal ();
	}

TVerdict CUdpClientTestUPnP6::RunTestL()
	{
	const TUint16 KActivityNull = 0;
	switch ( iState )
		{
		case ECreateTestServer:
			{
			iLogger.WriteFormat(_L("<i>Creating TestServer..... </i>"));
			
			iTestServer = CTestMUdpServer::NewL ( *this );
			
			iState  = ECreateTestClient;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		
		case ECreateTestClient:
			{
			iLogger.WriteFormat(_L("<i>TestServer is created..... </i>"));			
			iLogger.WriteFormat(_L("<i>Creating TestClient..... </i>"));
			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpUdpFlow, iChunkManager);
			const TUid requestedUid = { CUPnPFlowFactory::iUid };	
			
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
						
			RClientInterface::OpenPostMessageClose ( NodeId (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = ESendRequestData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		
		case ESendRequestData:
			{
			iLogger.WriteFormat(_L("<i>Client is Created</i>"));
			iLogger.WriteFormat(_L("<i>Send data..... </i>"));
			
			RMemChunk st;
			st.CreateL ( KTestCase7, iAllocator );	
			TSsdpInfo searchInfo ( 5, st ); // MX , ST
			TUpnpMessage::TUPnPSearchRequest msg ( searchInfo );
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg );
			
			iTimer.After ( iStatus, 5000000 ); // 5secs
			
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}

		case ECleanup:		
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			return EPass;
			}
			
		case EFailed:	
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
void CUdpClientTestUPnP6::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}
	
void CUdpClientTestUPnP6::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iClientId = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchResponse > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}



//-------------------------------------------------------------------------------------------------------------------------------




CUdpClientTestUPnP7* CUdpClientTestUPnP7::NewL ( CProxyProvd* aProvd )
	{
	CUdpClientTestUPnP7* ret = new ( ELeave ) CUdpClientTestUPnP7 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpClientTestUPnP7::CUdpClientTestUPnP7 ( CProxyProvd* aProvd ) :
	CUdpClientTestBase(aProvd), iTestServer(NULL), iState ( ECreateTestServer )
	{
	}

CUdpClientTestUPnP7::~CUdpClientTestUPnP7 ()
	{
	iTimer.Close ();
	}

void CUdpClientTestUPnP7::ConstructL()
	{
	iTimer.CreateLocal ();
	}

TVerdict CUdpClientTestUPnP7::RunTestL()
	{
	const TUint16 KActivityNull = 0;
	switch ( iState )
		{
		case ECreateTestServer:
			{
			iLogger.WriteFormat(_L("<i>Creating TestServer..... </i>"));
			
			iTestServer = CTestMUdpServer::NewL ( *this );
			
			iState  = ECreateTestClient;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		
		case ECreateTestClient:
			{
			iLogger.WriteFormat(_L("<i>TestServer is created..... </i>"));			
			iLogger.WriteFormat(_L("<i>Creating TestClient..... </i>"));
			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpUdpFlow, iChunkManager);
			const TUid requestedUid = { CUPnPFlowFactory::iUid };	
			
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
						
			RClientInterface::OpenPostMessageClose ( NodeId (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = ESendRequestData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		
		case ESendRequestData:
			{
			iLogger.WriteFormat(_L("<i>Client is Created</i>"));
			iLogger.WriteFormat(_L("<i>Send data..... </i>"));
			
			RMemChunk st;
			st.CreateL ( KTestCase8, iAllocator );	
			TSsdpInfo searchInfo ( 5, st ); // MX , ST
			TUpnpMessage::TUPnPSearchRequest msg ( searchInfo );
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg );

			iTimer.After ( iStatus, 5000000 ); // 5secs
			
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}

		case ECleanup:		
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			return EPass;
			}
			
		case EFailed:	
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
void CUdpClientTestUPnP7::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}
	
void CUdpClientTestUPnP7::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iClientId = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchResponse > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}



//-------------------------------------------------------------------------------------------------------------------------------




CUdpClientTestUPnP8* CUdpClientTestUPnP8::NewL ( CProxyProvd* aProvd )
	{
	CUdpClientTestUPnP8* ret = new ( ELeave ) CUdpClientTestUPnP8 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpClientTestUPnP8::CUdpClientTestUPnP8 ( CProxyProvd* aProvd ) :
	CUdpClientTestBase(aProvd), iTestServer(NULL), iState ( ECreateTestServer )
	{
	}

CUdpClientTestUPnP8::~CUdpClientTestUPnP8 ()
	{
	iTimer.Close ();
	}

void CUdpClientTestUPnP8::ConstructL()
	{
	iTimer.CreateLocal ();
	}

TVerdict CUdpClientTestUPnP8::RunTestL()
	{
	const TUint16 KActivityNull = 0;
	switch ( iState )
		{
		case ECreateTestServer:
			{
			iLogger.WriteFormat(_L("<i>Creating TestServer..... </i>"));
			
			iTestServer = CTestMUdpServer::NewL ( *this );
			
			iState  = ECreateTestClient;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		
		case ECreateTestClient:
			{
			iLogger.WriteFormat(_L("<i>TestServer is created..... </i>"));			
			iLogger.WriteFormat(_L("<i>Creating TestClient..... </i>"));
			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpUdpFlow, iChunkManager);
			const TUid requestedUid = { CUPnPFlowFactory::iUid };	
			
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
						
			RClientInterface::OpenPostMessageClose ( NodeId (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = ESendRequestData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		
		case ESendRequestData:
			{
			iLogger.WriteFormat(_L("<i>Client is Created</i>"));
			iLogger.WriteFormat(_L("<i>Send data..... </i>"));
			
			RMemChunk st;
			st.CreateL ( KTestCase9, iAllocator );	
			TSsdpInfo searchInfo ( 5, st ); // MX , ST
			TUpnpMessage::TUPnPSearchRequest msg ( searchInfo );
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg );

			iTimer.After ( iStatus, 5000000 ); // 5secs
			
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}

		case ECleanup:		
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			return EPass;
			}
			
		case EFailed:	
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
void CUdpClientTestUPnP8::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}
	
void CUdpClientTestUPnP8::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iClientId = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchResponse > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}



//-------------------------------------------------------------------------------------------------------------------------------




CUdpClientTestUPnP9* CUdpClientTestUPnP9::NewL ( CProxyProvd* aProvd )
	{
	CUdpClientTestUPnP9* ret = new ( ELeave ) CUdpClientTestUPnP9 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpClientTestUPnP9::CUdpClientTestUPnP9 ( CProxyProvd* aProvd ) :
	CUdpClientTestBase(aProvd), iTestServer(NULL), iState ( ECreateTestServer )
	{
	}

CUdpClientTestUPnP9::~CUdpClientTestUPnP9 ()
	{
	iTimer.Close ();
	}

void CUdpClientTestUPnP9::ConstructL()
	{
	iTimer.CreateLocal ();
	}

TVerdict CUdpClientTestUPnP9::RunTestL()
	{
	const TUint16 KActivityNull = 0;
	switch ( iState )
		{
		case ECreateTestServer:
			{
			iLogger.WriteFormat(_L("<i>Creating TestServer..... </i>"));
			
			iTestServer = CTestMUdpServer::NewL ( *this );
			
			iState  = ECreateTestClient;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		
		case ECreateTestClient:
			{
			iLogger.WriteFormat(_L("<i>TestServer is created..... </i>"));			
			iLogger.WriteFormat(_L("<i>Creating TestClient..... </i>"));
			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpUdpFlow, iChunkManager);
			const TUid requestedUid = { CUPnPFlowFactory::iUid };	
			
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
						
			RClientInterface::OpenPostMessageClose ( NodeId (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = ESendRequestData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		
		case ESendRequestData:
			{
			iLogger.WriteFormat(_L("<i>Client is Created</i>"));
			iLogger.WriteFormat(_L("<i>Send data..... </i>"));
			
			RMemChunk st;
			st.CreateL ( KTestCase10, iAllocator );	
			TSsdpInfo searchInfo ( 5, st ); // MX , ST
			TUpnpMessage::TUPnPSearchRequest msg ( searchInfo );
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg );

			iTimer.After ( iStatus, 5000000 ); // 5secs
			
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}

		case ECleanup:		
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			return EPass;
			}
			
		case EFailed:	
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
void CUdpClientTestUPnP9::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}
	
void CUdpClientTestUPnP9::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iClientId = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchResponse > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}


//-------------------------------------------------------------------------------------------------------------------------------




CUdpClientTestUPnP10* CUdpClientTestUPnP10::NewL ( CProxyProvd* aProvd )
	{
	CUdpClientTestUPnP10* ret = new ( ELeave ) CUdpClientTestUPnP10 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpClientTestUPnP10::CUdpClientTestUPnP10 ( CProxyProvd* aProvd ) :
	CUdpClientTestBase(aProvd), iTestServer(NULL), iState ( ECreateTestServer )
	{
	}

CUdpClientTestUPnP10::~CUdpClientTestUPnP10 ()
	{
	iTimer.Close ();
	}

void CUdpClientTestUPnP10::ConstructL()
	{
	iTimer.CreateLocal ();
	}

TVerdict CUdpClientTestUPnP10::RunTestL()
	{
	const TUint16 KActivityNull = 0;
	switch ( iState )
		{
		case ECreateTestServer:
			{
			iLogger.WriteFormat(_L("<i>Creating TestServer..... </i>"));
			
			iTestServer = CTestMUdpServer::NewL ( *this );
			
			iState  = ECreateTestClient;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		
		case ECreateTestClient:
			{
			iLogger.WriteFormat(_L("<i>TestServer is created..... </i>"));			
			iLogger.WriteFormat(_L("<i>Creating TestClient..... </i>"));
			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpUdpFlow, iChunkManager);
			const TUid requestedUid = { CUPnPFlowFactory::iUid };	
			
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
						
			RClientInterface::OpenPostMessageClose ( NodeId (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = ESendRequestData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		
		case ESendRequestData:
			{
			iLogger.WriteFormat(_L("<i>Client is Created</i>"));
			iLogger.WriteFormat(_L("<i>Send data..... </i>"));
			
			RMemChunk st;
			st.CreateL ( KTestCase11, iAllocator );	
			TSsdpInfo searchInfo ( 5, st ); // MX , ST
			TUpnpMessage::TUPnPSearchRequest msg ( searchInfo );
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg );

			iTimer.After ( iStatus, 5000000 ); // 5secs
			
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}

		case ECleanup:		
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			return EPass;
			}
			
		case EFailed:	
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
void CUdpClientTestUPnP10::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}
	
void CUdpClientTestUPnP10::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iClientId = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchResponse > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}


//-------------------------------------------------------------------------------------------------------------------------------




CUdpClientTestUPnP11* CUdpClientTestUPnP11::NewL ( CProxyProvd* aProvd )
	{
	CUdpClientTestUPnP11* ret = new ( ELeave ) CUdpClientTestUPnP11 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpClientTestUPnP11::CUdpClientTestUPnP11 ( CProxyProvd* aProvd ) :
	CUdpClientTestBase(aProvd), iTestServer(NULL), iState ( ECreateTestServer )
	{
	}

CUdpClientTestUPnP11::~CUdpClientTestUPnP11 ()
	{
	iTimer.Close ();
	}

void CUdpClientTestUPnP11::ConstructL()
	{
	iTimer.CreateLocal ();
	}

TVerdict CUdpClientTestUPnP11::RunTestL()
	{
	const TUint16 KActivityNull = 0;
	switch ( iState )
		{
		case ECreateTestServer:
			{
			iLogger.WriteFormat(_L("<i>Creating TestServer..... </i>"));
			
			iTestServer = CTestMUdpServer::NewL ( *this );
			
			iState  = ECreateTestClient;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		
		case ECreateTestClient:
			{
			iLogger.WriteFormat(_L("<i>TestServer is created..... </i>"));			
			iLogger.WriteFormat(_L("<i>Creating TestClient..... </i>"));
			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpUdpFlow, iChunkManager);
			const TUid requestedUid = { CUPnPFlowFactory::iUid };	
			
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
						
			RClientInterface::OpenPostMessageClose ( NodeId (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = ESendRequestData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		
		case ESendRequestData:
			{
			iLogger.WriteFormat(_L("<i>Client is Created</i>"));
			iLogger.WriteFormat(_L("<i>Send data..... </i>"));
			
			RMemChunk st;
			st.CreateL ( KTestCase12, iAllocator );	
			TSsdpInfo searchInfo ( 5, st ); // MX , ST
			TUpnpMessage::TUPnPSearchRequest msg ( searchInfo );
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg );

			iTimer.After ( iStatus, 5000000 ); // 5secs
			
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}

		case ECleanup:		
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			return EPass;
			}
			
		case EFailed:	
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
void CUdpClientTestUPnP11::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}
	
void CUdpClientTestUPnP11::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iClientId = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchResponse > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}


//-------------------------------------------------------------------------------------------------------------------------------




CUdpClientTestUPnP12* CUdpClientTestUPnP12::NewL ( CProxyProvd* aProvd )
	{
	CUdpClientTestUPnP12* ret = new ( ELeave ) CUdpClientTestUPnP12 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpClientTestUPnP12::CUdpClientTestUPnP12 ( CProxyProvd* aProvd ) :
	CUdpClientTestBase(aProvd), iTestServer(NULL),iState ( ECreateTestServer ), iSocketHandler ( *this ), iPort(1025)
	{
	}

CUdpClientTestUPnP12::~CUdpClientTestUPnP12 ()
	{
	iTimer.Close ();
	
	delete iSocketOpener;
	iSocketHandler.CancelAll ();
	
	iSocketArray.Close();
	}

void CUdpClientTestUPnP12::ConstructL()
	{
	iTimer.CreateLocal ();
	
	}

TVerdict CUdpClientTestUPnP12::RunTestL()
	{
	const TUint16 KActivityNull = 0;
	switch ( iState )
		{
		case ECreateTestServer:
			{
			iLogger.WriteFormat(_L("<i>Creating TestServer..... </i>"));
			
			iTestServer = CTestMUdpServer::NewL ( *this );
			
			iState  = EOpenTestSocket;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		case EOpenTestSocket:
			{
			iSocketOpener = CSocketOpener::NewL ( *this );
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );

			iState  = ECreateTestClient;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		case ECreateTestClient:
			{
			iLogger.WriteFormat(_L("<i>TestServer is created..... </i>"));			
			iLogger.WriteFormat(_L("<i>Creating TestClient..... </i>"));

			THttpUdpFlowQuery flowQuery ( Id (), EHttpUdpFlow, iChunkManager);
			const TUid requestedUid = { CUPnPFlowFactory::iUid };	
			
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
						
			RClientInterface::OpenPostMessageClose ( NodeId (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			iState  = ESendRequestData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		
		case ESendRequestData:
			{
			iLogger.WriteFormat(_L("<i>Client is Created</i>"));
			iLogger.WriteFormat(_L("<i>Send data..... </i>"));
			
			RMemChunk st;
			st.CreateL ( KTestCase12, iAllocator );	
			TSsdpInfo searchInfo ( 5, st ); // MX , ST
			TUpnpMessage::TUPnPSearchRequest msg ( searchInfo );
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg );

			iTimer.After ( iStatus, 5000000 ); // 5secs
			
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}

		case ECleanup:		
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			return EPass;
			}
			
		case EFailed:	
			{
			delete iTestServer;
			// cleanup udp client flow
			TEChild::TDestroy msg;
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg);

			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
void CUdpClientTestUPnP12::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}
	
void CUdpClientTestUPnP12::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iClientId = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchResponse > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}

void CUdpClientTestUPnP12::OpenComplete ( RInternalSocket& aSocket )
	{
	
	TInetAddr addr ( iPort );
	addr.SetAddress ( KInetAddrAny );

	aSocket.Bind( addr );
	iSocketHandler.Attach ( aSocket );
	iSocketArray.Append(&aSocket);
	iPort++;
	
	CompleteSelf ( KErrNone );
	}




