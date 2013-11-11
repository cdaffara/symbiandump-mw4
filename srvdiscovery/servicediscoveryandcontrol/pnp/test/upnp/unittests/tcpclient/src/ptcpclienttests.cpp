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
#include <httpstringconstants.h>

#include "ptcpclienttests.h"	
#include "upnp_cf_msgs.h"
#include "upnpflowbase.h"
#include "httpclientflow.h"
#include "upnpflowfactory.h"

#include "ctcpclienttestupnp1.h"
#include "ctcpclienttestupnp2.h"
#include "ctcpclienttestupnp3.h"
#include "ctcpclienttestupnp4.h"
#include "ctcpclienttestupnp5.h"
#include "ctcpclienttestupnp6.h"
#include "ctcpclienttestupnp7.h"
#include "ctcpclienttestupnp8.h"
#include "ctcpclienttestupnp9.h"
#include "ctcpclienttestupnp10.h"
#include "ctcpclienttestupnp11.h"
#include "ctcpclienttestupnp12.h"
#include "ctcpclienttestupnp13.h"
#include "ctcpclienttestupnp14.h"

const TUint16 KActivityNull = 0;
const TUint KHttpDefaultPort = 80;

//Add Blocks of 64
static const TInt KBlockSize1 = 64;
static const TInt KMinGrowth1 = 30;
static const TInt KThresholdGrowth1 = 30;
static const TInt KInitialAllocation1 = 128;

static const TInt KHeapSize	= 1024 * 1024;	//1MB

using namespace Messages;


//-------------------------------------------------------------------------------------------------
//	CTcpClientTestBase
//-------------------------------------------------------------------------------------------------	

#define BEGIN_TESTLIST

#define ADD_TEST(name) \
	if (aTestName == _L(#name))\
		{\
		return CTcpClient ## name::NewL(aProvd);\
		}
	
#define END_TESTLIST \
	User::Leave(KErrNotSupported);\
	return NULL;	
	
CTcpClientTestBase* CTcpClientTestBase::CreateTestL(const TDesC& aTestName, CProxyProvd* aProvd)
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
		ADD_TEST(TestUPnP13)
		ADD_TEST(TestUPnP14)
		ADD_TEST(TestUPnP15)
		ADD_TEST(TestUPnP16)
		ADD_TEST(TestUPnP17)
	END_TESTLIST
	}


void CTcpClientTestBase::StartTestL(const TDesC& aTestName, CProxyProvd* aProvd)
	{
	CTcpClientTestBase* test = CreateTestL(aTestName,aProvd);
	CleanupStack::PushL(test);
	test->CTcpClientTestBase::ConstructL();
	CActiveScheduler::Add(test);
	CleanupStack::Pop(test);
	TRequestStatus* stat = &test->iStatus;
	test->SetActive();
	User::RequestComplete(stat,KErrNone);
	}

CTcpClientTestBase::CTcpClientTestBase(CProxyProvd* aProvd) :
	CActive(EPriorityNormal), iState ( ECreateTestServer ),
	iProvd(aProvd)
	{}
	
void CTcpClientTestBase::ConstructL()
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
	
CTcpClientTestBase::~CTcpClientTestBase()
	{
	Cancel();
	iLogger.Close();
	delete iChunkManager;
	}

/**	Override this function if you want cleanup to happen before the completion of
	the test case. This function is called before the test is completed 
 */
void CTcpClientTestBase::Cleanup()
	{}

/** Sets the test case as active again and prevents the object from being destroyed
	when the RunL completes.
	
	Note: The active object is not complete the request again automatically, if this behaviour is
	desired the user must call User::RequestComplete
 */
void CTcpClientTestBase::Reschedule()
	{
	SetActive();
	iReschedule = TRUE;
	}
	
void CTcpClientTestBase::CompleteSelf ( TInt aError )
	{
	TRequestStatus* pStat = &iStatus;
    User::RequestComplete ( pStat, aError );
	}	

/** Attaches the iLogger variable to the TestExecute Log Server. This enable this prt to write to
	the testexecute logs as if it were running directly from TestExecute
 */
// 	SetLoggerL() is taken directly from CTestServer::StartLoggerL() in the TEF src TestServerBase.Cpp


void CTcpClientTestBase::SetLoggerL()
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
void CTcpClientTestBase::RunL()
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
	
void CTcpClientTestBase::FinishTest(TVerdict aResult)
	{
	iProvd->TestComplete(aResult);
	}
	
void CTcpClientTestBase::DoCancel()
	{
	Cleanup();
	}
	
//------------------------------------------------------------//	
/////////////////// CTcpClientTestUPnP /////////////////////////
////// Testcase: Send the test data from the test client/////////
//------------------------------------------------------------//
const TUint KTransactionCount = 1;
_LIT8 ( KTxtRawRequest, "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\nContent-Length: 0\r\n\r\n" );
_LIT8 ( KTxtRawResponse1, "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n" );


CTcpClientTestUPnP* CTcpClientTestUPnP::NewL ( CProxyProvd* aProvd )
	{
	CTcpClientTestUPnP* ret = new ( ELeave ) CTcpClientTestUPnP ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpClientTestUPnP::CTcpClientTestUPnP ( CProxyProvd* aProvd ) :
	CTcpClientTestBase(aProvd), iTestServer(NULL)
	{
	}

CTcpClientTestUPnP::~CTcpClientTestUPnP ()
	{
	iTimer.Close ();
	}

void CTcpClientTestUPnP::ConstructL()
	{
	iTimer.CreateLocal ();
	}

TVerdict CTcpClientTestUPnP::RunTestL()
	{
	switch ( iState )
		{
		case ECreateTestServer:
			{
			iLogger.WriteFormat(_L("<i>Creating TestServer..... </i>"));
			
			iTestServer = CTestTcpServer::NewL ( *this );
			
			iState  = ECreateTestClient;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;
			}
		
		case ECreateTestClient:
			{
			iLogger.WriteFormat(_L("<i>TestServer is created..... </i>"));			
			iLogger.WriteFormat(_L("<i>Creating TestClient..... </i>"));
			
			THttpClientFlowQuery flowQuery ( TAppProtAddr ( KInetAddrLoop, KHttpDefaultPort ), Id (), EHttpClientFlow, THttpClientFlowQuery::ECreateNew, iChunkManager );
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
			
			RMemChunk bodyBuf;
			RMemChunk uriBuf;
			uriBuf.CreateL ( _L8 ("http://127.0.0.1:9999"), iAllocator );
			TUpnpMessage::TUPnPClientRequestInfo msg ( HTTP::EGET, uriBuf, bodyBuf, NULL );
			iClientId.Node ().ReceivedL ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iClientId ), msg );
			
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}

		case ECleanup:
			{
			delete iTestServer;
			// cleanup tcp client flow
			delete reinterpret_cast<CHttpClientFlow*> ( iClientId.Ptr () );
			iTimer.After ( iStatus, 60000000 ); //10 secs
			iState = EComplete;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		case EComplete:
			{
			return EPass;
			}
		default:
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
		}
	}
	
void CTcpClientTestUPnP::OnTestServerEventL ( TInt aError )
	{
	CompleteSelf ( aError );
	}

MTcpClientTestCase* CTcpClientTestUPnP::GetTestCase ()
	{
	return this;
	}

CTestExecuteLogger& CTcpClientTestUPnP::GetLogger ( )
	{
	return iLogger;
	}

	
void CTcpClientTestUPnP::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iClientId = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TResponse > () )
		{		
		CompleteSelf ( EPass );
		}
	}
	
const TDesC& CTcpClientTestUPnP::TestCaseName () const
	{
	_LIT ( KTxtTitle, "Defect Fix CINC077703More" );
	return KTxtTitle ();
	}
	
TInt CTcpClientTestUPnP::TotalTransactionCount () const
	{
	return KTransactionCount;
	}

const TDesC8& CTcpClientTestUPnP::GetRawRequest ( TInt aTransIndex )
	{
	__ASSERT_ALWAYS ( aTransIndex < KTransactionCount, User::Invariant () );
	return KTxtRawRequest ();
	}
	
const TDesC8& CTcpClientTestUPnP::GetRawResponse ( TInt aTransIndex )
	{
	__ASSERT_ALWAYS( aTransIndex < KTransactionCount, User::Invariant () );
	return KTxtRawResponse1 ();
	}
TInt CTcpClientTestUPnP::TransactionCount () const
	{ 
	return KTransactionCount; 
	}









	

