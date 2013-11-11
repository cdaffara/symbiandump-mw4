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

#include "pudpservertests.h"	
#include "upnp_cf_msgs.h"
#include "upnpflowbase.h"
#include "upnpflowfactory.h"
#include "httpmudpflow.h"
#include "upnp_cf_msgs.h"

const TUint32 KMulticastAddrIp = INET_ADDR ( 239, 255, 255, 250 );
const TUint KMulticastAddrPort = 1900;

//Add Blocks of 64
static const TInt KBlockSize1 = 64;
static const TInt KMinGrowth1 = 30;
static const TInt KThresholdGrowth1 = 30;
static const TInt KInitialAllocation1 = 128;

static const TInt KHeapSize	= 1024 * 1024;	//1MB

using namespace Messages;


//-------------------------------------------------------------------------------------------------
//	CUdpServerTestBase
//-------------------------------------------------------------------------------------------------	
	
/**	This is a static method that is called to get a given test running
	@param aTestName The name of the test to be run
	@param aProvd The name of the provider starting the test
 */
 
#define BEGIN_TESTLIST

#define ADD_TEST(name) \
	if (aTestName == _L(#name))\
		{\
		return CUdpServer ## name::NewL(aProvd);\
		}
	
#define END_TESTLIST \
	User::Leave(KErrNotSupported);\
	return NULL;	
	
CUdpServerTestBase* CUdpServerTestBase::CreateTestL(const TDesC& aTestName, CProxyProvd* aProvd)
	{
	BEGIN_TESTLIST
		ADD_TEST(TestUPnP15)
		ADD_TEST(TestUPnP16)
		ADD_TEST(TestUPnP17)
		ADD_TEST(TestUPnP18)
		ADD_TEST(TestUPnP19)
		ADD_TEST(TestUPnP20)
		ADD_TEST(TestUPnP21)
		ADD_TEST(TestUPnP22)
		ADD_TEST(TestUPnP13)
		ADD_TEST(TestUPnP)
		ADD_TEST(TestUPnP10)
		ADD_TEST(TestUPnP11)
		ADD_TEST(TestUPnP9)
		ADD_TEST(TestUPnP1)	
		ADD_TEST(TestUPnP2)	
		ADD_TEST(TestUPnP3)	
		ADD_TEST(TestUPnP4)
		ADD_TEST(TestUPnP5)	
		ADD_TEST(TestUPnP6)
		ADD_TEST(TestUPnP7)
		ADD_TEST(TestUPnP8)
		ADD_TEST(TestUPnP14)
		ADD_TEST(TestUPnP12)
	END_TESTLIST
	}


void CUdpServerTestBase::StartTestL(const TDesC& aTestName, CProxyProvd* aProvd)
	{
	CUdpServerTestBase* test = CreateTestL(aTestName,aProvd);
	CleanupStack::PushL(test);
	test->ConstructL();
	CActiveScheduler::Add(test);
	CleanupStack::Pop(test);
	TRequestStatus* stat = &test->iStatus;
	test->SetActive();
	User::RequestComplete(stat,KErrNone);
	}

CUdpServerTestBase::CUdpServerTestBase(CProxyProvd* aProvd) :
	CActive(EPriorityNormal),
	iProvd(aProvd)
	{}
	
void CUdpServerTestBase::ConstructL()
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
	
CUdpServerTestBase::~CUdpServerTestBase()
	{
	iLogger.Close();
	delete iChunkManager;
	}

/**	Override this function if you want cleanup to happen before the completion of
	the test case. This function is called before the test is completed 
 */
void CUdpServerTestBase::Cleanup()
	{}

/** Sets the test case as active again and prevents the object from being destroyed
	when the RunL completes.
	
	Note: The active object is not complete the request again automatically, if this behaviour is
	desired the user must call User::RequestComplete
 */
void CUdpServerTestBase::Reschedule()
	{
	SetActive();
	iReschedule = TRUE;
	}
	
void CUdpServerTestBase::CompleteSelf ( TInt aError )
	{
	TRequestStatus* pStat = &iStatus;
    User::RequestComplete ( pStat, aError );
	}	

/** Attaches the iLogger variable to the TestExecute Log Server. This enable this prt to write to
	the testexecute logs as if it were running directly from TestExecute
 */
// 	SetLoggerL() is taken directly from CTestServer::StartLoggerL() in the TEF src TestServerBase.Cpp


void CUdpServerTestBase::SetLoggerL()
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
void CUdpServerTestBase::RunL()
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
	
void CUdpServerTestBase::FinishTest(TVerdict aResult)
	{
	iProvd->TestComplete(aResult);
	}
	
void CUdpServerTestBase::DoCancel()
	{
	Cleanup();
	}
	
//------------------------------------------------------------//	
/////////////////// CUdpServerTestUPnP /////////////////////////
////// Testcase: Send the test data from the test client/////////
//------------------------------------------------------------//

CUdpServerTestUPnP* CUdpServerTestUPnP::NewL ( CProxyProvd* aProvd )
	{
	CUdpServerTestUPnP* ret = new ( ELeave ) CUdpServerTestUPnP ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP::CUdpServerTestUPnP ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP::~CUdpServerTestUPnP ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	}
	
void CUdpServerTestUPnP::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	
	iSendToAddr.SetAddress ( KMulticastAddrIp );
  	iSendToAddr.SetPort ( KMulticastAddrPort );	
	}

TVerdict CUdpServerTestUPnP::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
	
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			
			iSendChain.CreateL ( KMissingMX );
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );
		
			
			iTimer.After ( iStatus, 2000000 );
			iState = ESendStopServer;
			
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}

void CUdpServerTestUPnP::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP::SendToComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();
	}
	
void CUdpServerTestUPnP::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		TUpnpMessage::TUPnPSearchRequest& msg = message_cast < TUpnpMessage::TUPnPSearchRequest > ( aMessage );
		msg.iSsdpInfo.iLocation.Free ();
		msg.iSsdpInfo.iSearchTarget.Free ();
		msg.iSsdpInfo.iUsn.Free ();
		
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}	
	
//------------------------------------------------------------//	
/////////////////// CUdpServerTestUPnP1 /////////////////////////
////// Testcase: Send the test data from the test client/////////
//------------------------------------------------------------//

CUdpServerTestUPnP1* CUdpServerTestUPnP1::NewL ( CProxyProvd* aProvd )
	{
	CUdpServerTestUPnP1* ret = new ( ELeave ) CUdpServerTestUPnP1 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP1::CUdpServerTestUPnP1 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP1::~CUdpServerTestUPnP1 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	// delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () ); No need to do this, already it is cleaned-up
	}
	
void CUdpServerTestUPnP1::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	}

TVerdict CUdpServerTestUPnP1::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
	
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );	//created the client socket
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			//sends valid m-search packet
			iSendChain.CreateL ( KMSearch );			
			iSendToAddr.SetAddress ( KMulticastAddrIp );	
  			iSendToAddr.SetPort ( KMulticastAddrPort );
			
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );	//sending m-search packet to the multicast addr
			
			iTimer.After ( iStatus, 3000000 ); //300secs or 5minutes
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}
	
void CUdpServerTestUPnP1::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP1::SendToComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();	
	}
	
void CUdpServerTestUPnP1::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;	//Id of node found or created
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}	
	
	
	
//------------------------------------------------------------//	
/////////////////// CUdpServerTestUPnP2 /////////////////////////
////// Testcase: Send the test data from the test client/////////
//------------------------------------------------------------//

//all test classes are listeners...hence
//events like client socket opening, data sending thro socket etc ....the completion notification for all these
///events are sent to the respective test classes
CUdpServerTestUPnP2* CUdpServerTestUPnP2::NewL ( CProxyProvd* aProvd )
	{
	CUdpServerTestUPnP2* ret = new ( ELeave ) CUdpServerTestUPnP2 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP2::CUdpServerTestUPnP2 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP2::~CUdpServerTestUPnP2 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	// delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () ); No need to do this, already it is cleaned-up
	}
	
void CUdpServerTestUPnP2::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	}

TVerdict CUdpServerTestUPnP2::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
	
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );	//created the client socket
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			//sends m-search with MX > 120 
			iSendChain.CreateL ( KInvalidMX );			
			iSendToAddr.SetAddress ( KMulticastAddrIp );	
  			iSendToAddr.SetPort ( KMulticastAddrPort );
			
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );	//sending m-search packet to the multicast addr
			
			iTimer.After ( iStatus, 3000000 ); //3 sec
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}
	
void CUdpServerTestUPnP2::OpenComplete ( RInternalSocket& aSocket )
	{//indicates that client socket has been opened
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP2::SendToComplete ( TInt /*aLength*/ )
	{//indicates that data has been sent thro socket
	iSendChain.Init ();	
	}
	
void CUdpServerTestUPnP2::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;	//Id of node found or created
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}
	
	
	
//------------------------------------------------------------//	
/////////////////// CUdpServerTestUPnP3 /////////////////////////
////// Testcase: Send the test data from the test client/////////
//------------------------------------------------------------//

CUdpServerTestUPnP3* CUdpServerTestUPnP3::NewL ( CProxyProvd* aProvd )

	{
	CUdpServerTestUPnP3* ret = new ( ELeave ) CUdpServerTestUPnP3 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP3::CUdpServerTestUPnP3 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP3::~CUdpServerTestUPnP3 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	// delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () ); No need to do this, already it is cleaned-up
	}
	
void CUdpServerTestUPnP3::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	}

TVerdict CUdpServerTestUPnP3::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
	
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			//void OpenPostMessageClose(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TSignalBase& aMessage);
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData://so here the sender is client and receiver (servicepoint) will be the server
			{			//send m-search request with invalid MAN
			iSendChain.CreateL ( KInvalidMAN );			
			iSendToAddr.SetAddress ( KMulticastAddrIp );
  			iSendToAddr.SetPort ( KMulticastAddrPort );
			
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );
			
			iTimer.After ( iStatus, 3000000 ); //3secs
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}
	
void CUdpServerTestUPnP3::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP3::SendToComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();	
	}
	
void CUdpServerTestUPnP3::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;	//iMudpServer is of type TNodeId which is a subclass of TRuntimeCtxId
		/*TRuntimeCtxId is the baseclass intended for addressing message
		(TSignalBase) destinations, i.e.: As implied by the respective API,
		the sender of a TSignalBase must supply a TRuntimeCtxId object.*/
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}


//-----------------------------------------------------------------------------------------------------------------

CUdpServerTestUPnP4* CUdpServerTestUPnP4::NewL ( CProxyProvd* aProvd )

	{
	CUdpServerTestUPnP4* ret = new ( ELeave ) CUdpServerTestUPnP4 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP4::CUdpServerTestUPnP4 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP4::~CUdpServerTestUPnP4 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	// delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () ); No need to do this, already it is cleaned-up
	}
	
void CUdpServerTestUPnP4::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	}

TVerdict CUdpServerTestUPnP4::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
	
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			//void OpenPostMessageClose(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TSignalBase& aMessage);
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			//send invalid request method
			iSendChain.CreateL ( KInvalidMethod );			
			iSendToAddr.SetAddress ( KMulticastAddrIp );
  			iSendToAddr.SetPort ( KMulticastAddrPort );
			
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );
			
			iTimer.After ( iStatus, 3000000 ); //3secs
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}
	
void CUdpServerTestUPnP4::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP4::SendToComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();	
	}
	
void CUdpServerTestUPnP4::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;	//iMudpServer is of type TNodeId which is a subclass of TRuntimeCtxId
		/*TRuntimeCtxId is the baseclass intended for addressing message
		(TSignalBase) destinations, i.e.: As implied by the respective API,
		the sender of a TSignalBase must supply a TRuntimeCtxId object.*/
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}


//------------------------------------------------------------------------

CUdpServerTestUPnP5* CUdpServerTestUPnP5::NewL ( CProxyProvd* aProvd )

	{
	CUdpServerTestUPnP5* ret = new ( ELeave ) CUdpServerTestUPnP5 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP5::CUdpServerTestUPnP5 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP5::~CUdpServerTestUPnP5 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	// delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () ); No need to do this, already it is cleaned-up
	}
	
void CUdpServerTestUPnP5::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	}

TVerdict CUdpServerTestUPnP5::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
	
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			//void OpenPostMessageClose(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TSignalBase& aMessage);
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			//send valid notify method
			iSendChain.CreateL ( KValidNotify );			
			iSendToAddr.SetAddress ( KMulticastAddrIp );
  			iSendToAddr.SetPort ( KMulticastAddrPort );
			
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );
			
			iTimer.After ( iStatus, 3000000 ); //3secs
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}
	
void CUdpServerTestUPnP5::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;	//client socket open
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP5::SendToComplete ( TInt /*aLength*/ )//data sent to udp socket
	{
	iSendChain.Init ();	
	}
	
void CUdpServerTestUPnP5::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;	//iMudpServer is of type TNodeId which is a subclass of TRuntimeCtxId
		/*TRuntimeCtxId is the baseclass intended for addressing message
		(TSignalBase) destinations, i.e.: As implied by the respective API,
		the sender of a TSignalBase must supply a TRuntimeCtxId object.*/
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}


//------------------------------------------------------------------------

CUdpServerTestUPnP6* CUdpServerTestUPnP6::NewL ( CProxyProvd* aProvd )

	{
	CUdpServerTestUPnP6* ret = new ( ELeave ) CUdpServerTestUPnP6 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP6::CUdpServerTestUPnP6 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP6::~CUdpServerTestUPnP6 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	// delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () ); No need to do this, already it is cleaned-up
	}
	
void CUdpServerTestUPnP6::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	}

TVerdict CUdpServerTestUPnP6::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
				
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			//void OpenPostMessageClose(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TSignalBase& aMessage);
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			//send valid notify method
			iSendChain.CreateL ( KMissingNTS );			
			iSendToAddr.SetAddress ( KMulticastAddrIp );
  			iSendToAddr.SetPort ( KMulticastAddrPort );
			
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );
			
			iTimer.After ( iStatus, 3000000 ); //3secs
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}
	
void CUdpServerTestUPnP6::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;	//client socket open
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP6::SendToComplete ( TInt /*aLength*/ )//data sent to udp socket
	{
	iSendChain.Init ();	
	}
	
void CUdpServerTestUPnP6::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;	//iMudpServer is of type TNodeId which is a subclass of TRuntimeCtxId
		/*TRuntimeCtxId is the baseclass intended for addressing message
		(TSignalBase) destinations, i.e.: As implied by the respective API,
		the sender of a TSignalBase must supply a TRuntimeCtxId object.*/
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}

//------------------------------------------------------------------------
//------ Sends a notify packet with missing NT header --------------------
//------------------------------------------------------------------------

CUdpServerTestUPnP7* CUdpServerTestUPnP7::NewL ( CProxyProvd* aProvd )

	{
	CUdpServerTestUPnP7* ret = new ( ELeave ) CUdpServerTestUPnP7 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP7::CUdpServerTestUPnP7 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP7::~CUdpServerTestUPnP7 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	// delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () ); No need to do this, already it is cleaned-up
	}
	
void CUdpServerTestUPnP7::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	}

TVerdict CUdpServerTestUPnP7::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
				
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			//void OpenPostMessageClose(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TSignalBase& aMessage);
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			//send valid notify method
			iSendChain.CreateL ( KInvalidNotifyByeBye );			
			iSendToAddr.SetAddress ( KMulticastAddrIp );
  			iSendToAddr.SetPort ( KMulticastAddrPort );
			
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );
			
			iTimer.After ( iStatus, 3000000 ); //3secs
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}
	
void CUdpServerTestUPnP7::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;	//client socket open
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP7::SendToComplete ( TInt /*aLength*/ )//data sent to udp socket
	{
	iSendChain.Init ();	
	}
	
void CUdpServerTestUPnP7::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;	//iMudpServer is of type TNodeId which is a subclass of TRuntimeCtxId
		/*TRuntimeCtxId is the baseclass intended for addressing message
		(TSignalBase) destinations, i.e.: As implied by the respective API,
		the sender of a TSignalBase must supply a TRuntimeCtxId object.*/
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------Sends notify packet with missing location header -----------------------------------
//--------------------------------------------------------------------------------------------------------------------

CUdpServerTestUPnP8* CUdpServerTestUPnP8::NewL ( CProxyProvd* aProvd )

	{
	CUdpServerTestUPnP8* ret = new ( ELeave ) CUdpServerTestUPnP8 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP8::CUdpServerTestUPnP8 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP8::~CUdpServerTestUPnP8 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	// delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () ); No need to do this, already it is cleaned-up
	}
	
void CUdpServerTestUPnP8::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	}

TVerdict CUdpServerTestUPnP8::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
				
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			//void OpenPostMessageClose(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TSignalBase& aMessage);
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			//send valid notify method
			iSendChain.CreateL ( KMissingNotifyLocn );			
			iSendToAddr.SetAddress ( KMulticastAddrIp );
  			iSendToAddr.SetPort ( KMulticastAddrPort );
			
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );
			
			iTimer.After ( iStatus, 3000000 ); //3secs
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}
	
void CUdpServerTestUPnP8::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;	//client socket open
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP8::SendToComplete ( TInt /*aLength*/ )//data sent to udp socket
	{
	iSendChain.Init ();	
	}
	
void CUdpServerTestUPnP8::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;	//iMudpServer is of type TNodeId which is a subclass of TRuntimeCtxId
		/*TRuntimeCtxId is the baseclass intended for addressing message
		(TSignalBase) destinations, i.e.: As implied by the respective API,
		the sender of a TSignalBase must supply a TRuntimeCtxId object.*/
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}

//------------------------------------------------------------------------------------------------------------------

CUdpServerTestUPnP9* CUdpServerTestUPnP9::NewL ( CProxyProvd* aProvd )

	{
	CUdpServerTestUPnP9* ret = new ( ELeave ) CUdpServerTestUPnP9 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP9::CUdpServerTestUPnP9 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP9::~CUdpServerTestUPnP9 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	// delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () ); No need to do this, already it is cleaned-up
	}
	
void CUdpServerTestUPnP9::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	}

TVerdict CUdpServerTestUPnP9::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
				
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			//void OpenPostMessageClose(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TSignalBase& aMessage);
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			//send valid notify method
			iSendChain.CreateL ( KMissingST );			
			iSendToAddr.SetAddress ( KMulticastAddrIp );
  			iSendToAddr.SetPort ( KMulticastAddrPort );
			
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );
			
			iTimer.After ( iStatus, 3000000 ); //3secs
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}
	
void CUdpServerTestUPnP9::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;	//client socket open
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP9::SendToComplete ( TInt /*aLength*/ )//data sent to udp socket
	{
	iSendChain.Init ();	
	}
	
void CUdpServerTestUPnP9::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;	//iMudpServer is of type TNodeId which is a subclass of TRuntimeCtxId
		/*TRuntimeCtxId is the baseclass intended for addressing message
		(TSignalBase) destinations, i.e.: As implied by the respective API,
		the sender of a TSignalBase must supply a TRuntimeCtxId object.*/
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}
	
	
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//			Sends a m-search packet with invalid header
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

CUdpServerTestUPnP10* CUdpServerTestUPnP10::NewL ( CProxyProvd* aProvd )

	{
	CUdpServerTestUPnP10* ret = new ( ELeave ) CUdpServerTestUPnP10 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP10::CUdpServerTestUPnP10 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP10::~CUdpServerTestUPnP10 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	// delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () ); No need to do this, already it is cleaned-up
	}
	
void CUdpServerTestUPnP10::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	}

TVerdict CUdpServerTestUPnP10::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
				
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			//void OpenPostMessageClose(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TSignalBase& aMessage);
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			//send valid notify method
			iSendChain.CreateL ( KInvalidHdr );			
			iSendToAddr.SetAddress ( KMulticastAddrIp );
  			iSendToAddr.SetPort ( KMulticastAddrPort );
			
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );
			
			iTimer.After ( iStatus, 3000000 ); //3secs
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}
	
void CUdpServerTestUPnP10::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;	//client socket open
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP10::SendToComplete ( TInt /*aLength*/ )//data sent to udp socket
	{
	iSendChain.Init ();	
	}
	
void CUdpServerTestUPnP10::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;	//iMudpServer is of type TNodeId which is a subclass of TRuntimeCtxId
		/*TRuntimeCtxId is the baseclass intended for addressing message
		(TSignalBase) destinations, i.e.: As implied by the respective API,
		the sender of a TSignalBase must supply a TRuntimeCtxId object.*/
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//			Sends notify packet with invalid max-age directive
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


CUdpServerTestUPnP11* CUdpServerTestUPnP11::NewL ( CProxyProvd* aProvd )
	{
	CUdpServerTestUPnP11* ret = new ( ELeave ) CUdpServerTestUPnP11 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP11::CUdpServerTestUPnP11 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP11::~CUdpServerTestUPnP11 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	// delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () ); No need to do this, already it is cleaned-up
	}
	
void CUdpServerTestUPnP11::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	}

TVerdict CUdpServerTestUPnP11::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
				
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			
			iSendChain.CreateL ( KInvalidMaxAge );			
			iSendToAddr.SetAddress ( KMulticastAddrIp );
  			iSendToAddr.SetPort ( KMulticastAddrPort );
			
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );
			
			iTimer.After ( iStatus, 3000000 ); //3secs
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}
	
void CUdpServerTestUPnP11::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP11::SendToComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();	
	}
	
void CUdpServerTestUPnP11::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//			Sends notify packet  with missing nt header
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//




CUdpServerTestUPnP12* CUdpServerTestUPnP12::NewL ( CProxyProvd* aProvd )
	{
	CUdpServerTestUPnP12* ret = new ( ELeave ) CUdpServerTestUPnP12 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP12::CUdpServerTestUPnP12 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP12::~CUdpServerTestUPnP12 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	// delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () ); No need to do this, already it is cleaned-up
	}
	
void CUdpServerTestUPnP12::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	}

TVerdict CUdpServerTestUPnP12::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
				
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			
			iSendChain.CreateL ( KMissingNT );			
			iSendToAddr.SetAddress ( KMulticastAddrIp );
  			iSendToAddr.SetPort ( KMulticastAddrPort );
			
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );
			
			iTimer.After ( iStatus, 3000000 ); //3secs
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}
	
void CUdpServerTestUPnP12::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP12::SendToComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();	
	}
	
void CUdpServerTestUPnP12::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}
	
	
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//					sends an invalid upnp packet
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


CUdpServerTestUPnP13* CUdpServerTestUPnP13::NewL ( CProxyProvd* aProvd )
	{
	CUdpServerTestUPnP13* ret = new ( ELeave ) CUdpServerTestUPnP13 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP13::CUdpServerTestUPnP13 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP13::~CUdpServerTestUPnP13 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	// delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () ); No need to do this, already it is cleaned-up
	}
	
void CUdpServerTestUPnP13::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	}

TVerdict CUdpServerTestUPnP13::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
			//this  calls httpmudpflow::ReceivedL() else case  (aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated>)
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{//this  calls httpmudpflow::ReceivedL() switch case  TAppProtIntfMessage::TJoinComplete::EId:			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );			
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			
			iSendChain.CreateL ( KMan);			
			iSendToAddr.SetAddress ( KMulticastAddrIp );
  			iSendToAddr.SetPort ( KMulticastAddrPort );
			
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );//this invokes httpmudpflow::receivedL switch case TAppProtIntfMessage::TDataReceived::EId:
			
			iTimer.After ( iStatus, 3000000 ); //3secs
			iState = ESendStopServer;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ),TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}
	
void CUdpServerTestUPnP13::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP13::SendToComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();	
	}
	
void CUdpServerTestUPnP13::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}
	
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CUdpServerTestUPnP14* CUdpServerTestUPnP14::NewL ( CProxyProvd* aProvd )
	{
	CUdpServerTestUPnP14* ret = new ( ELeave ) CUdpServerTestUPnP14 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP14::CUdpServerTestUPnP14 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP14::~CUdpServerTestUPnP14 ()
	{	
	TUpnpMessage::DeRegister ();
	// delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () ); No need to do this, already it is cleaned-up
	}
	
void CUdpServerTestUPnP14::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());	
	iTimer.CreateLocal ();
	}

TVerdict CUdpServerTestUPnP14::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
			//this  calls httpmudpflow::ReceivedL() else case  (aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated>)
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case EStartServer:
			{//this  calls httpmudpflow::ReceivedL() switch case  TAppProtIntfMessage::TJoinComplete::EId:			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = ESendErrorMsg;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}	
		
		case ESendErrorMsg:
			{
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ),TAppProtIntfMessage::TError(0).CRef() );
			
			iTimer.After ( iStatus, 1000000 ); //1sec and this takes the iStatus value and sets it to KRequestPending
			//on completion it will call CompleteSelf() and sets iStatus to KNone
			iState = ESendStopServer;
			//iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}	
			
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ),TCFDataClient::TStop ( KErrNone ).CRef () );
			
		//	iTimer.After ( iStatus, 500000 );	//no need because it does not need to wait for any response for the stop message sent
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 );		//waits for a response after sending the destro message
			iState = ECleanup;
		//	iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}

	
void CUdpServerTestUPnP14::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		iState = EFailed;
		RunTestL ();
		}
	}
	
////////////////////////////////////////////////////////////////////////////////////////
CUdpServerTestUPnP15* CUdpServerTestUPnP15::NewL ( CProxyProvd* aProvd )
	{
	CUdpServerTestUPnP15* ret = new ( ELeave ) CUdpServerTestUPnP15 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP15::CUdpServerTestUPnP15 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP15::~CUdpServerTestUPnP15 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	}
	
void CUdpServerTestUPnP15::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	
	iSendToAddr.SetAddress ( KMulticastAddrIp );
  	iSendToAddr.SetPort ( KMulticastAddrPort );	
	}

TVerdict CUdpServerTestUPnP15::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
				
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TAppProtIntfMessage::TJoin().CRef () );
			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TUpnpMessage::TRequestLocalHost ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			
			iSendChain.CreateL ( KInvalidSsdpPort );			
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );
		
			
			iTimer.After ( iStatus, 2000000 );
			iState = ESendStopServer;
			
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}

void CUdpServerTestUPnP15::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP15::SendToComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();
	}
	
void CUdpServerTestUPnP15::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		TUpnpMessage::TUPnPSearchRequest& msg = message_cast < TUpnpMessage::TUPnPSearchRequest > ( aMessage );
		msg.iSsdpInfo.iLocation.Free ();
		msg.iSsdpInfo.iSearchTarget.Free ();
		msg.iSsdpInfo.iUsn.Free ();
		
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}

////////////////////////////////////////////////////////////////////////////////////////
CUdpServerTestUPnP16* CUdpServerTestUPnP16::NewL ( CProxyProvd* aProvd )
	{
	CUdpServerTestUPnP16* ret = new ( ELeave ) CUdpServerTestUPnP16 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP16::CUdpServerTestUPnP16 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP16::~CUdpServerTestUPnP16 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	}
	
void CUdpServerTestUPnP16::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	
	iSendToAddr.SetAddress ( KMulticastAddrIp );
  	iSendToAddr.SetPort ( KMulticastAddrPort );	
	}

TVerdict CUdpServerTestUPnP16::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
				
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			
			iSendChain.CreateL ( KMissingHost );
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );
		
			
			iTimer.After ( iStatus, 2000000 );
			iState = ESendStopServer;
			
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}

void CUdpServerTestUPnP16::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP16::SendToComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();
	}
	
void CUdpServerTestUPnP16::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		TUpnpMessage::TUPnPSearchRequest& msg = message_cast < TUpnpMessage::TUPnPSearchRequest > ( aMessage );
		msg.iSsdpInfo.iLocation.Free ();
		msg.iSsdpInfo.iSearchTarget.Free ();
		msg.iSsdpInfo.iUsn.Free ();
		
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}
////////////////////////////////////////////////////////////////////////////////////////
CUdpServerTestUPnP17* CUdpServerTestUPnP17::NewL ( CProxyProvd* aProvd )
	{
	CUdpServerTestUPnP17* ret = new ( ELeave ) CUdpServerTestUPnP17 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP17::CUdpServerTestUPnP17 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP17::~CUdpServerTestUPnP17 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	}
	
void CUdpServerTestUPnP17::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	
	iSendToAddr.SetAddress ( KMulticastAddrIp );
  	iSendToAddr.SetPort ( KMulticastAddrPort );	
	}

TVerdict CUdpServerTestUPnP17::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
				
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			
			iSendChain.CreateL ( KMissingSsdpPort );
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );
		
			
			iTimer.After ( iStatus, 2000000 );
			iState = ESendStopServer;
			
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}

void CUdpServerTestUPnP17::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP17::SendToComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();
	}
	
void CUdpServerTestUPnP17::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		TUpnpMessage::TUPnPSearchRequest& msg = message_cast < TUpnpMessage::TUPnPSearchRequest > ( aMessage );
		msg.iSsdpInfo.iLocation.Free ();
		msg.iSsdpInfo.iSearchTarget.Free ();
		msg.iSsdpInfo.iUsn.Free ();
		
		iTimer.Cancel ();
		RunTestL ();
		}
	}
////////////////////////////////////////////////////////////////////////////////////////
CUdpServerTestUPnP18* CUdpServerTestUPnP18::NewL ( CProxyProvd* aProvd )
	{
	CUdpServerTestUPnP18* ret = new ( ELeave ) CUdpServerTestUPnP18 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP18::CUdpServerTestUPnP18 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP18::~CUdpServerTestUPnP18 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	}
	
void CUdpServerTestUPnP18::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	
	iSendToAddr.SetAddress ( KMulticastAddrIp );
  	iSendToAddr.SetPort ( KMulticastAddrPort );	
	}

TVerdict CUdpServerTestUPnP18::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
				
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			
			iSendChain.CreateL ( KMissingSsdpPort1 );
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );
		
			
			iTimer.After ( iStatus, 2000000 );
			iState = ESendStopServer;
			
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}

void CUdpServerTestUPnP18::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP18::SendToComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();
	}
	
void CUdpServerTestUPnP18::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		TUpnpMessage::TUPnPSearchRequest& msg = message_cast < TUpnpMessage::TUPnPSearchRequest > ( aMessage );
		msg.iSsdpInfo.iLocation.Free ();
		msg.iSsdpInfo.iSearchTarget.Free ();
		msg.iSsdpInfo.iUsn.Free ();
		
		iTimer.Cancel ();
		RunTestL ();
		}
	}
////////////////////////////////////////////////////////////////////////////////////////
CUdpServerTestUPnP19* CUdpServerTestUPnP19::NewL ( CProxyProvd* aProvd )
	{
	CUdpServerTestUPnP19* ret = new ( ELeave ) CUdpServerTestUPnP19 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP19::CUdpServerTestUPnP19 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP19::~CUdpServerTestUPnP19 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	}
	
void CUdpServerTestUPnP19::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	
	iSendToAddr.SetAddress ( KMulticastAddrIp );
  	iSendToAddr.SetPort ( KMulticastAddrPort );	
	}

TVerdict CUdpServerTestUPnP19::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
				
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			
			iSendChain.CreateL ( KInvalidSsdpHost );
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );
		
			
			iTimer.After ( iStatus, 2000000 );
			iState = ESendStopServer;
			
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}

void CUdpServerTestUPnP19::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP19::SendToComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();
	}
	
void CUdpServerTestUPnP19::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		TUpnpMessage::TUPnPSearchRequest& msg = message_cast < TUpnpMessage::TUPnPSearchRequest > ( aMessage );
		msg.iSsdpInfo.iLocation.Free ();
		msg.iSsdpInfo.iSearchTarget.Free ();
		msg.iSsdpInfo.iUsn.Free ();
		
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}

////////////////////////////////////////////////////////////////////////////////////////
CUdpServerTestUPnP20* CUdpServerTestUPnP20::NewL ( CProxyProvd* aProvd )
	{
	CUdpServerTestUPnP20* ret = new ( ELeave ) CUdpServerTestUPnP20 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP20::CUdpServerTestUPnP20 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP20::~CUdpServerTestUPnP20 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	}
	
void CUdpServerTestUPnP20::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	
	iSendToAddr.SetAddress ( KMulticastAddrIp );
  	iSendToAddr.SetPort ( KMulticastAddrPort );	
	}

TVerdict CUdpServerTestUPnP20::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
				
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );
			
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{			
			iSendChain.CreateL ( KInvalidHostMissingSsdpPort );
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );
		
			
			iTimer.After ( iStatus, 2000000 );
			iState = ESendStopServer;
			
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}

void CUdpServerTestUPnP20::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP20::SendToComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();
	}
	
void CUdpServerTestUPnP20::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		TUpnpMessage::TUPnPSearchRequest& msg = message_cast < TUpnpMessage::TUPnPSearchRequest > ( aMessage );
		msg.iSsdpInfo.iLocation.Free ();
		msg.iSsdpInfo.iSearchTarget.Free ();
		msg.iSsdpInfo.iUsn.Free ();
		
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}

////////////////////////////////////////////////////////////////////////////////////////
CUdpServerTestUPnP21* CUdpServerTestUPnP21::NewL ( CProxyProvd* aProvd )
	{
	CUdpServerTestUPnP21* ret = new ( ELeave ) CUdpServerTestUPnP21 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP21::CUdpServerTestUPnP21 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP21::~CUdpServerTestUPnP21 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	}
	
void CUdpServerTestUPnP21::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	
	iSendToAddr.SetAddress ( KMulticastAddrIp );
  	iSendToAddr.SetPort ( KMulticastAddrPort );	
	}

TVerdict CUdpServerTestUPnP21::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
				
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );						
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendData;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
			
		case ESendData:
			{						
			iSendChain.CreateL ( KMissingMan );
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );		
			
			iTimer.After ( iStatus, 2000000 );
			iState = ESendStopServer;
			
			Reschedule ();
			return EPass;
			}
		
		// just to make sure production objects are cleaned-up properly.
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = EServerStopped;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}

void CUdpServerTestUPnP21::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP21::SendToComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();
	}
	
void CUdpServerTestUPnP21::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		TUpnpMessage::TUPnPSearchRequest& msg = message_cast < TUpnpMessage::TUPnPSearchRequest > ( aMessage );
		msg.iSsdpInfo.iLocation.Free ();
		msg.iSsdpInfo.iSearchTarget.Free ();
		msg.iSsdpInfo.iUsn.Free ();
		
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}

////////////////////////////////////////////////////////////////////////////////////////
CUdpServerTestUPnP22* CUdpServerTestUPnP22::NewL ( CProxyProvd* aProvd )
	{
	CUdpServerTestUPnP22* ret = new ( ELeave ) CUdpServerTestUPnP22 ( aProvd );
	CleanupStack::PushL ( ret );
	ret->ConstructL ();
	CleanupStack::Pop ( ret );
	return ret;
	}

CUdpServerTestUPnP22::CUdpServerTestUPnP22 ( CProxyProvd* aProvd ) :
	CUdpServerTestBase ( aProvd ), iSocketHandler ( *this ), iState ( ECreateServer )
	{
	}
	
CUdpServerTestUPnP22::~CUdpServerTestUPnP22 ()
	{
	iSocketHandler.CancelAll ();
	iClientSocket.Close ();
	delete iSocketOpener;
	iTimer.Close ();
	TUpnpMessage::DeRegister ();
	}
	
void CUdpServerTestUPnP22::ConstructL ()
	{
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	iTimer.CreateLocal ();
	TRAPD ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );			
	User::LeaveIfError ( err );		
	
	iSendToAddr.SetAddress ( KMulticastAddrIp );
  	iSendToAddr.SetPort ( KMulticastAddrPort );	
	}

TVerdict CUdpServerTestUPnP22::RunTestL()
	{
	switch ( iState )
		{
		case ECreateServer:
			{			
			THttpUdpFlowQuery flowQuery ( Id (), EHttpMUdpFlow, iChunkManager );
			const TUid requestedUid = { CUPnPFlowFactory::iUid };
			TNodeId factoryContainer = SockManGlobals::Get( )->GetPlaneFC( TCFPlayerRole ( TCFPlayerRole::EDataPlane ) );
				
			RClientInterface::OpenPostMessageClose ( Id (), TNodeCtxId ( KActivityNull, factoryContainer ), TCFFactory::TFindOrCreatePeer ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery ).CRef () );
			
			iState  = EStartServer;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;
			}
		case EStartServer:
			{			
			RClientInterface::OpenPostMessageClose (TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStart ().CRef () );						
			iState  = EOpenClientSocket;
			iStatus = KRequestPending;
			Reschedule();
			return EPass;			
			}
				
		case EOpenClientSocket:
			{
			iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );
			iState  = ESendStopServer;
			iStatus = KRequestPending;			
			Reschedule();
			return EPass;		
			}
			
		case ESendStopServer:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TCFDataClient::TStop ( KErrNone ).CRef () );
			
			iState = ESendData;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
					
		case ESendData:
			{						
			iSendChain.CreateL ( KMSearch );
			iSocketHandler.SendTo ( iSendChain, iSendToAddr );		
			
			iTimer.After ( iStatus, 2000000 );
			iState = EServerStopped;
			
			Reschedule ();
			return EPass;
			}
		
		
		case EServerStopped:
			{
			// cleanup udp server flow
			RClientInterface::OpenPostMessageClose ( TNodeCtxId ( KActivityNull, Id () ), TNodeCtxId ( KActivityNull, iMudpServer ), TEChild::TDestroy ().CRef () );
			
			iTimer.After ( iStatus, 1000000 ); //1secs
			iState = ECleanup;
			iStatus = KRequestPending;
			Reschedule ();
			return EPass;
			}
		
		case ECleanup:
			{
			return EPass;
			}
			
		case EFailed:
			{
			// cleanup udp server flow
			delete reinterpret_cast<CHttpMUdpFlow*> ( iMudpServer.Ptr () );
			FinishTest ( EFail );
			delete this;
			return EFail;
			}
			
		default:
			{
			iLogger.WriteFormat(_L("<i> Failed: TestCase:..... </i>"));
			ASSERT(0);
			return EFail;
			}
		}
	}

void CUdpServerTestUPnP22::OpenComplete ( RInternalSocket& aSocket )
	{
	iClientSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	CompleteSelf ( KErrNone );
	}	
	
void CUdpServerTestUPnP22::SendToComplete ( TInt /*aLength*/ )
	{
	iSendChain.Init ();
	}
	
void CUdpServerTestUPnP22::ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage )
	{
	if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated> () )
		{
		const TCFFactory::TPeerFoundOrCreated& msg = message_cast < const TCFFactory::TPeerFoundOrCreated > ( aMessage );
		iMudpServer = msg.iNodeId;
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TCFDataClient::TStarted> () 
				|| aMessage.IsMessage<TCFDataClient::TStopped> () )
		{
		CompleteSelf ( KErrNone );
		}
	else if ( aMessage.IsMessage<TUpnpMessage::TUPnPSearchRequest > () )
		{
		TUpnpMessage::TUPnPSearchRequest& msg = message_cast < TUpnpMessage::TUPnPSearchRequest > ( aMessage );
		msg.iSsdpInfo.iLocation.Free ();
		msg.iSsdpInfo.iSearchTarget.Free ();
		msg.iSsdpInfo.iUsn.Free ();
		
		iTimer.Cancel ();
		iState = EFailed;
		RunTestL ();
		}
	}
