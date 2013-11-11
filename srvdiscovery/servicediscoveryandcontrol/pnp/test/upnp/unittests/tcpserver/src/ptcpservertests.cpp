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
#include <comms-infras/ss_thread.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include "ptcpservertests.h"
#include "upnp_cf_msgs.h"
#include "upnpflowbase.h"
#include "upnpflowfactory.h"

#include "ctcpservertestupnp1.h"
#include "ctcpservertestupnp2.h"
#include "ctcpservertestupnp3.h"
#include "ctcpservertestupnp4.h"
#include "ctcpservertestupnp5.h"
#include "ctcpservertestupnp6.h"
#include "ctcpservertestupnp7.h"
#include "ctcpservertestupnp8.h"
#include "ctcpservertestupnp9.h"
#include "ctcpservertestupnp10.h"
#include "ctcpservertestupnp11.h"
#include "CTcpServerTestUPnP12.h"
#include "CTcpServerTestUPnP13.h"
#include "CTcpServerTestUPnP14.h"
#include "CTcpServerTestUPnP15.h"
#include "CTcpServerTestUPnP16.h"
#include "CTcpServerTestUPnP17.h"
#include "CTcpServerTestUPnP18.h"
#include "CTcpServerTestUPnP19.h"
#include "CTcpServerTestUPnP20.h"
#include "CTcpServerTestUPnP21.h"
#include "CTcpServerTestUPnP22.h"

#include "CTcpServerTestCase.h"
#include "CTcpServerTestUPnP23.h"
#include "CTcpServerTestUPnP24.h"
#include "CTcpServerTestUPnP25.h"
#include "CTcpServerTestUPnP26.h"
#include "CTcpServerTestUPnP27.h"
#include "CTcpServerTestUPnP28.h"
#include "CTcpServerTestUPnP29.h"
#include "CTcpServerTestUPnP30.h"
#include "CTcpServerTestUPnP31.h"
#include "CTcpServerTestUPnP32.h"
#include "CTcpServerTestUPnP33.h"
#include "CTcpServerTestUPnP34.h"
#include "CTcpServerTestUPnP41.h"

//const TUint16 KActivityNull = 0;
const TUint KHttpDefaultPort = 80;
//Add Blocks of 64
static const TInt KBlockSize1 = 64;
static const TInt KMinGrowth1 = 30;
static const TInt KThresholdGrowth1 = 30;
static const TInt KInitialAllocation1 = 128;

static const TInt KHeapSize	= 1024 * 1024;	//1MB

using namespace Messages;

_LIT8 ( KTestUPnPServiceUri,"http://127.0.0.1:80/testserviceuri1" );
_LIT8 ( KMissingHost, "GET /testserviceuri1 HTTP/1.1\r\nContent-Length: 0\r\n\r\n");
_LIT8 ( KExpectedResponse, "HTTP/1.1 400 Bad request");

//-------------------------------------------------------------------------------------------------
//	CTcpServerTestBase
//-------------------------------------------------------------------------------------------------	

#define BEGIN_TESTLIST

#define ADD_TEST(name) \
	if (aTestName == _L(#name))\
		{\
		return CTcpServer ## name::NewL(aProvd);\
		}
	
#define END_TESTLIST \
	User::Leave(KErrNotSupported);\
	return NULL;	
	
CTcpServerTestBase* CTcpServerTestBase::CreateTestL(const TDesC& aTestName, CProxyProvd* aProvd)
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
		ADD_TEST(TestUPnP18)
		ADD_TEST(TestUPnP19)
		ADD_TEST(TestUPnP20)
		ADD_TEST(TestUPnP21)
		ADD_TEST(TestUPnP22)
		ADD_TEST(TestUPnP23)
		ADD_TEST(TestUPnP24)
		ADD_TEST(TestUPnP25)
		ADD_TEST(TestUPnP26)
		ADD_TEST(TestUPnP27)
		ADD_TEST(TestUPnP28)
		ADD_TEST(TestUPnP29)
		ADD_TEST(TestUPnP30)
		ADD_TEST(TestUPnP31)
		ADD_TEST(TestUPnP32)
		ADD_TEST(TestUPnP33)
		ADD_TEST(TestUPnP34)
		ADD_TEST(TestUPnP35) //TcpServer Recv timeout
		ADD_TEST(TestUPnP36) //TcpServer Recv timeout
		ADD_TEST(TestUPnP37) //TcpServer Recv timeout
		ADD_TEST(TestUPnP38) //Test Case failing
		ADD_TEST(TestUPnP39)
		ADD_TEST(TestUPnP40)
		ADD_TEST(TestUPnP41)
		ADD_TEST(TestUPnP42)
		
	END_TESTLIST
	}


void CTcpServerTestBase::StartTestL(const TDesC& aTestName, CProxyProvd* aProvd)
	{
	CTcpServerTestBase* test = CreateTestL(aTestName,aProvd);
	CleanupStack::PushL(test);
	test->ConstructL();
	CActiveScheduler::Add(test);
	CleanupStack::Pop(test);
	TRequestStatus* stat = &test->iStatus;
	test->SetActive();
	User::RequestComplete(stat,KErrNone);
	}

CTcpServerTestBase::CTcpServerTestBase(CProxyProvd* aProvd) :
	CActive(EPriorityNormal),
	iProvd(aProvd)
	{}
	
void CTcpServerTestBase::ConstructL()
	{
	//Create the Chunk Manager
	CChunkManager* chkmgr = CChunkManager::NewL ( KHeapSize );
	if ( chkmgr )
		{
		chkmgr->AddPoolL ( KBlockSize1, KInitialAllocation1, KMinGrowth1, KThresholdGrowth1 );
		}
	
	iChunkManager = chkmgr;

	SetLoggerL();
	}
	
CTcpServerTestBase::~CTcpServerTestBase()
	{
	iLogger.Close();
	delete iChunkManager;
	}

/**	Override this function if you want cleanup to happen before the completion of
	the test case. This function is called before the test is completed 
 */
void CTcpServerTestBase::Cleanup()
	{}

/** Sets the test case as active again and prevents the object from being destroyed
	when the RunL completes.
	
	Note: The active object is not complete the request again automatically, if this behaviour is
	desired the user must call User::RequestComplete
 */
void CTcpServerTestBase::Reschedule()
	{
	SetActive();
	iReschedule = TRUE;
	}
	
void CTcpServerTestBase::CompleteSelf ( TInt aError )
	{
	TRequestStatus* pStat = &iStatus;
    User::RequestComplete ( pStat, aError );
	}	

/** Attaches the iLogger variable to the TestExecute Log Server. This enable this prt to write to
	the testexecute logs as if it were running directly from TestExecute
 */
// 	SetLoggerL() is taken directly from CTestServer::StartLoggerL() in the TEF src TestServerBase.Cpp


void CTcpServerTestBase::SetLoggerL()
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
void CTcpServerTestBase::RunL()
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
	
void CTcpServerTestBase::FinishTest(TVerdict aResult)
	{
	iProvd->TestComplete(aResult);
	}
	
void CTcpServerTestBase::DoCancel()
	{
	Cleanup();
	}
	
//------------------------------------------------------------//	
/////////////////// CTcpServerTestUPnP /////////////////////////
////// Testcase: Send the test data from the test client/////////
//------------------------------------------------------------//

CTcpServerTestUPnP* CTcpServerTestUPnP::NewL ( CProxyProvd* aProvd )
	{
	CTcpServerTestUPnP* ret = new ( ELeave ) CTcpServerTestUPnP ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CTcpServerTestUPnP::CTcpServerTestUPnP ( CProxyProvd* aProvd ) :
	CTcpServerTestBase ( aProvd ), iSocketHandler ( *this ), iHostAddr ( KInetAddrLoop, KHttpDefaultPort ), iState ( ECreateServer )
	{
	}
	
CTcpServerTestUPnP::~CTcpServerTestUPnP ()
	{
	iServiceUri.Close ();
		
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	
	iTimer.Close ();
	
	TUpnpMessage::DeRegister ();
	}
	
void CTcpServerTestUPnP::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	
	iServiceUri.CreateL ( KTestUPnPServiceUri );
	
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	
	User::LeaveIfError ( err );		
	}

TVerdict CTcpServerTestUPnP::RunTestL ()
	{
	switch ( iState )
		{
		case ECreateServer:
			{
			const TUint8* uriPtr = NULL;
			uriPtr = iServiceUri.Ptr ();
			
			const TUid reqUid = { CUPnPFlowFactory::iUid };		
			THttpServerFlowQuery flowQuery ( uriPtr,
										 iServiceUri.Size (),
										 Id (),
										 EHttpServerFlow, iChunkManager );
			TCFPlayerRole playerRole ( TCFPlayerRole::EDataPlane );
			
			ESock::TCFFactory::TFindOrCreatePeer msg ( TCFPlayerRole::EDataPlane, reqUid, &flowQuery );
			TNodeId factoryContainer = SockManGlobals::Get()->GetPlaneFC( playerRole );
			
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, reqUid, &flowQuery ).CRef () );
			
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
			
		case EStartServer:
			{
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iTcpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocketAndConnect;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
			
		case EOpenClientSocketAndConnect:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockStream, KProtocolInetTcp );
			
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
			
		case ESendData:
			{			
			iSendChain.CreateL ( KMissingHost );
			iSocketHandler.Send ( iSendChain );
			
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{	
			// cleanup tcp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iTcpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iTcpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return iResponse; // Here the final test case is passed back to the RSocket::Ioctl
			}
					
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}
	
void CTcpServerTestUPnP::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );	
	
	iSocketHandler.Connect ( iHostAddr );
	}
	
void CTcpServerTestUPnP::ConnectComplete ()
	{	
	CompleteSelf ( KErrNone );
	}
	
void CTcpServerTestUPnP::SendComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();
	iSocketHandler.Recv ();
	}
	
void CTcpServerTestUPnP::RecvComplete ( RMBufChain& aData )
	{
	RBuf8 responseBuf;
	responseBuf.CreateMax ( aData.Length () );
	aData.CopyOut ( responseBuf );
	aData.Free ();
	
	if ( responseBuf.FindF ( KExpectedResponse ) == KErrNotFound )
		{
		iResponse = EFail; // test case failed
		}
	else
		{
		iResponse = EPass;	// test case passed
		}
	
	responseBuf.Close ();
	CompleteSelf ( KErrNone );
	}
	
void CTcpServerTestUPnP::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iTcpServer = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	}






