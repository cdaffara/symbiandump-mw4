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


#if !defined(PUDPCLIENTTESTS_H)
#define PUDPCLIENTTESTS_H

#include <e32base.h>
#include <testexecutestepbase.h>
#include "proxyprotocol.h"
#include <comms-infras/eintsock.h>
#include "es_mbuf.h"
#include "testmudpserver.h"
#include <elements/nm_node.h>

#include <cchunkmanager.h>
#include <rmemchunk.h>

_LIT8 ( KTestCase1, "testcase1" );
_LIT8 ( KMissingContentLength, "HTTP/1.1 200 OK\r\nST: testcase1\r\nEXT: \r\nSERVER: Apache 9.x, UPnP/1.0\r\nUSN: uuid:ae30fdec-16e3-4d75-ac72-b61c5d6d30d5::urn:schemas-upnp-org:device:BinaryLight:1\r\nCACHE-CONTROL: max-age=900\r\nLOCATION: http://10.192.196.204:54042/\r\n\r\n");
_LIT8 ( KMissingST, "HTTP/1.1 200 OK\r\nEXT: \r\nSERVER: Apache 9.x, UPnP/1.0\r\nDATE: 27\11\08\r\nUSN: uuid:ae30fdec-16e3-4d75-ac72-b61c5d6d30d5::urn:schemas-upnp-org:device:BinaryLight:1\r\nCACHE-CONTROL: max-age=900\r\nLOCATION: http://10.192.196.204:54042/\r\nContent-Length: 0\r\n\r\n");
_LIT8 ( KTestCase2, "testcase2" );
_LIT8 ( KTestCase3, "testcase3" );

NONSHARABLE_CLASS(CUdpClientTestBase) : protected CActive
	{
public:
	static void StartTestL(const TDesC& aTestName, CProxyProvd* aProvd);
	virtual void RunL();

protected:		
	CUdpClientTestBase(CProxyProvd* aProvd);
	virtual ~CUdpClientTestBase();
	
	//Utilities function to be use in RunTest()
	void Reschedule();
	CTestExecuteLogger iLogger;
	void CompleteSelf ( TInt aError );
	void FinishTest(TVerdict result);
	RMemoryAllocator 			iAllocator;
	CChunkManager*  iChunkManager;			
private:
	//Implemented you own of these		
	virtual TVerdict RunTestL() = 0;
	virtual void Cleanup();
	
	void DoCancel();
	void ConstructL();
	void SetLoggerL();
	CTestExecuteLogger& Logger();	
	static CUdpClientTestBase* CreateTestL(const TDesC& aTestName, CProxyProvd* aProvd);
		
private:
	CProxyProvd* iProvd;
	TBool iReschedule; //Determines whether rescheduling is required
	
	};
	
inline CTestExecuteLogger& CUdpClientTestBase::Logger()
	{
	return iLogger;
	}


/*struct TObserverBundle 
	{
	enum TObserverState {EState1,EState2,EState3};
	TObserverState iObserverState;
	};
*/
class MTestServerObserver
	{
public:
	/**
	Callback method which returns the responses.
	@param aObserverInfo bundle containg the response.
	*/
	virtual void OnTestServerEventL ( TInt aError ) = 0;
	//virtual void OnTestServerEventL ( TBool aFlag) = 0;
	/**
	Callback method which handles the errors of OnPnPEventL.
	@param aError error code.
	*/
	//virtual void OnTestServerError (TInt aError) = 0;
	};

/*class MTestClientObserver
	{
public:
	/**
	Callback method which returns the responses.
	@param aObserverInfo bundle containg the response.
	*
	virtual void OnTestClientEventL ( TObserverBundle& aObserverInfo) = 0;
	/**
	Callback method which handles the errors of OnPnPEventL.
	@param aError error code.
	*
	virtual void OnTestClientError (TInt aError) = 0;
	};
*/
//----------------------------------------------------------//
/////////////////// CUdpClientTestUPnP /////////////////////////
//////Testcase: Send the test data from the test client/////////
//----------------------------------------------------------//

NONSHARABLE_CLASS(CUdpClientTestUPnP) : public CUdpClientTestBase,
										public Messages::ASimpleNodeIdBase,
										public MTestServerObserver
	{
public:
	static CUdpClientTestUPnP* NewL ( CProxyProvd* aProvd );

	//MTestServerObserver
	void OnTestServerEventL ( TInt aError );

	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
private:
	enum TState
		{
		ECreateTestServer = 0,
		ECreateTestClient,
		ESendRequestData,
		EFailed,
		ECleanup,
		};
		
	CUdpClientTestUPnP ( CProxyProvd* aProvd );
	~CUdpClientTestUPnP();
	void ConstructL ();
	virtual TVerdict RunTestL ();
	//void Cleanup();

private:	
	CTestMUdpServer*	iTestServer;	
	Messages::TNodeId 	iClientId;
	RTimer				iTimer;
	TState				iState;
	};

//---------------------------------------------------------------------------------------------------------------------------------------

NONSHARABLE_CLASS(CUdpClientTestUPnP1) : public CUdpClientTestBase,
										public Messages::ASimpleNodeIdBase,
										public MTestServerObserver
	{
public:
	static CUdpClientTestUPnP1* NewL ( CProxyProvd* aProvd );

	//MTestServerObserver
	void OnTestServerEventL ( TInt aError );

	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
private:
	enum TState
		{
		ECreateTestServer = 0,
		ECreateTestClient,
		ESendRequestData,
		EFailed,
		ECleanup,
		};
		
	CUdpClientTestUPnP1 ( CProxyProvd* aProvd );
	~CUdpClientTestUPnP1();
	void ConstructL ();
	virtual TVerdict RunTestL ();
	//void Cleanup();

private:	
	CTestMUdpServer*	iTestServer;	
	Messages::TNodeId 	iClientId;
	RTimer				iTimer;
	TState				iState;
	};
	


//----------------------------------------------------------------------------------------------------------------------------------



_LIT8 ( KMissingRespLocn, "HTTP/1.1 200 OK\r\nEXT: \r\nST: testcase3\r\nSERVER: Apache 9.x, UPnP/1.0\r\nDATE: 27\11\08\r\nUSN: uuid:ae30fdec-16e3-4d75-ac72-b61c5d6d30d5::urn:schemas-upnp-org:device:BinaryLight:1\r\nCACHE-CONTROL: max-age=900\r\nContent-Length: 0\r\n\r\n");


NONSHARABLE_CLASS(CUdpClientTestUPnP2) : public CUdpClientTestBase,public Messages::ASimpleNodeIdBase,
										public MTestServerObserver
	{
public:
	static CUdpClientTestUPnP2* NewL ( CProxyProvd* aProvd );

	//MTestServerObserver
	void OnTestServerEventL ( TInt aError );

	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient,Messages::TSignatureBase& aMessage );
	
private:
	enum TState
		{
		ECreateTestServer = 0,
		ECreateTestClient,
		ESendRequestData,
		EFailed,
		ECleanup,
		};
		
	CUdpClientTestUPnP2 ( CProxyProvd* aProvd );
	~CUdpClientTestUPnP2();
	void ConstructL ();
	virtual TVerdict RunTestL ();
	//void Cleanup();

private:	
	CTestMUdpServer*	iTestServer;	
	Messages::TNodeId 	iClientId;
	RTimer				iTimer;
	TState				iState;
	};
	
	
	
_LIT8 ( KTestCase4, "testcase4" );

//----------------------------------------------------------------------------------------------------------------------------------



_LIT8 ( KMissingRespUSN, "HTTP/1.1 200 OK\r\nEXT: \r\nST: testcase4\r\nSERVER: Apache 9.x, UPnP/1.0\r\nDATE: 27\11\08\r\nCACHE-CONTROL: max-age=900\r\nLOCATION: http://10.192.196.204:54042/\r\nContent-Length: 0\r\n\r\n");


NONSHARABLE_CLASS(CUdpClientTestUPnP3) : public CUdpClientTestBase,public Messages::ASimpleNodeIdBase,
										public MTestServerObserver
	{
public:
	static CUdpClientTestUPnP3* NewL ( CProxyProvd* aProvd );

	//MTestServerObserver
	void OnTestServerEventL ( TInt aError );

	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
private:
	enum TState
		{
		ECreateTestServer = 0,
		ECreateTestClient,
		ESendRequestData,
		EFailed,
		ECleanup,
		};
		
	CUdpClientTestUPnP3 ( CProxyProvd* aProvd );
	~CUdpClientTestUPnP3();
	void ConstructL ();
	virtual TVerdict RunTestL ();
	//void Cleanup();

private:	
	CTestMUdpServer*	iTestServer;	
	Messages::TNodeId 	iClientId;
	RTimer				iTimer;
	TState				iState;
	};


_LIT8 ( KTestCase5, "testcase5" );

//----------------------------------------------------------------------------------------------------------------------------------



_LIT8 ( KInvalidCacheControl, "HTTP/1.1 200 OK\r\nST: testcase5\r\nEXT: \r\nSERVER: Apache 9.x, UPnP/1.0\r\nDATE: 27\11\08\r\nUSN: uuid:ae30fdec-16e3-4d75-ac72-b61c5d6d30d5::urn:schemas-upnp-org:device:BinaryLight:1\r\nCACHE-CONTROL: \r\nLOCATION: http://10.192.196.204:54042/\r\nContent-Length: 0\r\n\r\n");


NONSHARABLE_CLASS(CUdpClientTestUPnP4) : public CUdpClientTestBase,
										public Messages::ASimpleNodeIdBase,
										public MTestServerObserver
	{
public:
	static CUdpClientTestUPnP4* NewL ( CProxyProvd* aProvd );

	//MTestServerObserver
	void OnTestServerEventL ( TInt aError );

	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
private:
	enum TState
		{
		ECreateTestServer = 0,
		ECreateTestClient,
		ESendRequestData,
		EFailed,
		ECleanup,
		};
		
	CUdpClientTestUPnP4 ( CProxyProvd* aProvd );
	~CUdpClientTestUPnP4();
	void ConstructL ();
	virtual TVerdict RunTestL ();
	//void Cleanup();

private:	
	CTestMUdpServer*	iTestServer;	
	Messages::TNodeId 	iClientId;
	RTimer				iTimer;
	TState				iState;
	};




_LIT8 ( KTestCase6, "testcase6" );

//----------------------------------------------------------------------------------------------------------------------------------




_LIT8 ( KInvalidCL, "HTTP/1.1 200 OK\r\nST: testcase1\r\nEXT: \r\nSERVER: Apache 9.x, UPnP/1.0\r\nUSN: uuid:ae30fdec-16e3-4d75-ac72-b61c5d6d30d5::urn:schemas-upnp-org:device:BinaryLight:1\r\nCACHE-CONTROL: max-age=900\r\nLOCATION: http://10.192.196.204:54042/\r\nContent-Length: 100\r\n\r\n");


NONSHARABLE_CLASS(CUdpClientTestUPnP5) : public CUdpClientTestBase,
										public Messages::ASimpleNodeIdBase,
										public MTestServerObserver
	{
public:
	static CUdpClientTestUPnP5* NewL ( CProxyProvd* aProvd );

	//MTestServerObserver
	void OnTestServerEventL ( TInt aError );

	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient,Messages::TSignatureBase& aMessage );
	
private:
	enum TState
		{
		ECreateTestServer = 0,
		ECreateTestClient,
		ESendRequestData,
		EFailed,
		ECleanup,
		};
		
	CUdpClientTestUPnP5 ( CProxyProvd* aProvd );
	~CUdpClientTestUPnP5();
	void ConstructL ();
	virtual TVerdict RunTestL ();
	//void Cleanup();

private:	
	CTestMUdpServer*	iTestServer;	
	Messages::TNodeId 	iClientId;
	RTimer				iTimer;
	TState				iState;
	};



_LIT8 ( KTestCase7, "testcase7" );

//----------------------------------------------------------------------------------------------------------------------------------




_LIT8 ( KMissingExtn, "HTTP/1.1 200 OK\r\nST: testcase1\r\nSERVER: Apache 9.x, UPnP/1.0\r\nUSN: uuid:ae30fdec-16e3-4d75-ac72-b61c5d6d30d5::urn:schemas-upnp-org:device:BinaryLight:1\r\nCACHE-CONTROL: max-age=900\r\nLOCATION:http://10.192.196.204:54042/\r\nContent-Length: 0\r\n\r\n");


NONSHARABLE_CLASS(CUdpClientTestUPnP6) : public CUdpClientTestBase,
										public Messages::ASimpleNodeIdBase,
										public MTestServerObserver
	{
public:
	static CUdpClientTestUPnP6* NewL ( CProxyProvd* aProvd );

	//MTestServerObserver
	void OnTestServerEventL ( TInt aError );

	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
private:
	enum TState
		{
		ECreateTestServer = 0,
		ECreateTestClient,
		ESendRequestData,
		EFailed,
		ECleanup,
		};
		
	CUdpClientTestUPnP6 ( CProxyProvd* aProvd );
	~CUdpClientTestUPnP6();
	void ConstructL ();
	virtual TVerdict RunTestL ();
	//void Cleanup();

private:	
	CTestMUdpServer*	iTestServer;	
	Messages::TNodeId 	iClientId;
	RTimer				iTimer;
	TState				iState;
	};
	


_LIT8 ( KTestCase8, "testcase8" );
_LIT8 ( KTestCase9, "testcase9" );
_LIT8 ( KTestCase10, "testcase10" );
_LIT8 ( KTestCase11, "testcase11" );
_LIT8 ( KTestCase12, "testcase12" );


//----------------------------------------------------------------------------------------------------------------------------------




_LIT8 ( KInvalidRes8, "HTTP/1.0 200 OK\r\nCACHE-CONTROL: max-age=900\r\nContent-Length: 0\r\n\r\n");


NONSHARABLE_CLASS(CUdpClientTestUPnP7) : public CUdpClientTestBase,
										public Messages::ASimpleNodeIdBase,
										public MTestServerObserver
	{
public:
	static CUdpClientTestUPnP7* NewL ( CProxyProvd* aProvd );

	//MTestServerObserver
	void OnTestServerEventL ( TInt aError );

	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
private:
	enum TState
		{
		ECreateTestServer = 0,
		ECreateTestClient,
		ESendRequestData,
		EFailed,
		ECleanup,
		};
		
	CUdpClientTestUPnP7 ( CProxyProvd* aProvd );
	~CUdpClientTestUPnP7();
	void ConstructL ();
	virtual TVerdict RunTestL ();
	//void Cleanup();

private:	
	CTestMUdpServer*	iTestServer;	
	Messages::TNodeId 	iClientId;
	RTimer				iTimer;
	TState				iState;
	};


//----------------------------------------------------------------------------------------------------------------------------------




_LIT8 ( KInvalidRes9, "HTTP/1.1 200 OK\r\nCACHE-CONTROL: max-age=900\r\nContent-Length: 0\r\n\r\n");


NONSHARABLE_CLASS(CUdpClientTestUPnP8) : public CUdpClientTestBase,
										public Messages::ASimpleNodeIdBase,
										public MTestServerObserver
	{
public:
	static CUdpClientTestUPnP8* NewL ( CProxyProvd* aProvd );

	//MTestServerObserver
	void OnTestServerEventL ( TInt aError );

	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
private:
	enum TState
		{
		ECreateTestServer = 0,
		ECreateTestClient,
		ESendRequestData,
		EFailed,
		ECleanup,
		};
		
	CUdpClientTestUPnP8 ( CProxyProvd* aProvd );
	~CUdpClientTestUPnP8();
	void ConstructL ();
	virtual TVerdict RunTestL ();
	//void Cleanup();

private:	
	CTestMUdpServer*	iTestServer;	
	Messages::TNodeId 	iClientId;
	RTimer				iTimer;
	TState				iState;
	};

//----------------------------------------------------------------------------------------------------------------------------------




_LIT8 ( KInvalidRes10, "HTTP/1.1 200 OK\r\nCACHE-CONTROL: max-age=900\r\nEXT: \r\nContent-Length: 0\r\n\r\n");


NONSHARABLE_CLASS(CUdpClientTestUPnP9) : public CUdpClientTestBase,
										public Messages::ASimpleNodeIdBase,
										public MTestServerObserver
	{
public:
	static CUdpClientTestUPnP9* NewL ( CProxyProvd* aProvd );

	//MTestServerObserver
	void OnTestServerEventL ( TInt aError );

	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
private:
	enum TState
		{
		ECreateTestServer = 0,
		ECreateTestClient,
		ESendRequestData,
		EFailed,
		ECleanup,
		};
		
	CUdpClientTestUPnP9 ( CProxyProvd* aProvd );
	~CUdpClientTestUPnP9();
	void ConstructL ();
	virtual TVerdict RunTestL ();
	//void Cleanup();

private:	
	CTestMUdpServer*	iTestServer;	
	Messages::TNodeId 	iClientId;
	RTimer				iTimer;
	TState				iState;
	};


//----------------------------------------------------------------------------------------------------------------------------------




_LIT8 ( KInvalidRes11, "HTTP/1.1 200 OK\r\nCACHE-CONTROL: max-age=900\r\nEXT: \r\nLOCATION:http://10.192.196.204:54042/\r\nContent-Length: 0\r\n\r\n");


NONSHARABLE_CLASS(CUdpClientTestUPnP10) : public CUdpClientTestBase,
										public Messages::ASimpleNodeIdBase,
										public MTestServerObserver
	{
public:
	static CUdpClientTestUPnP10* NewL ( CProxyProvd* aProvd );

	//MTestServerObserver
	void OnTestServerEventL ( TInt aError );

	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
private:
	enum TState
		{
		ECreateTestServer = 0,
		ECreateTestClient,
		ESendRequestData,
		EFailed,
		ECleanup,
		};
		
	CUdpClientTestUPnP10 ( CProxyProvd* aProvd );
	~CUdpClientTestUPnP10();
	void ConstructL ();
	virtual TVerdict RunTestL ();
	//void Cleanup();

private:	
	CTestMUdpServer*	iTestServer;	
	Messages::TNodeId 	iClientId;
	RTimer				iTimer;
	TState				iState;
	};

_LIT8 ( KInvalidRes12, "HTTP/1.1 200 OK\r\nCACHE-CONTROL: max-age=900\r\nEXT: \r\nLOCATION:http://10.192.196.204:54042/\r\nST: testcase1\r\nContent-Length: 0\r\n\r\n");


NONSHARABLE_CLASS(CUdpClientTestUPnP11) : public CUdpClientTestBase,
										public Messages::ASimpleNodeIdBase,
										public MTestServerObserver
	{
public:
	static CUdpClientTestUPnP11* NewL ( CProxyProvd* aProvd );

	//MTestServerObserver
	void OnTestServerEventL ( TInt aError );

	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
private:
	enum TState
		{
		ECreateTestServer = 0,
		ECreateTestClient,
		ESendRequestData,
		EFailed,
		ECleanup,
		};
		
	CUdpClientTestUPnP11 ( CProxyProvd* aProvd );
	~CUdpClientTestUPnP11();
	void ConstructL ();
	virtual TVerdict RunTestL ();
	//void Cleanup();

private:	
	CTestMUdpServer*	iTestServer;	
	Messages::TNodeId 	iClientId;
	RTimer				iTimer;
	TState				iState;
	};


//----------------------------------------------------------------------------------------------------------------------------------
_LIT8 ( KInvalidRes13, "HTTP/1.1 200 OK\r\nCACHE-CONTROL: max-age=900\r\nEXT: \r\nLOCATION:http://10.192.196.204:54042/\r\nST: testcase1\r\nContent-Length: 0\r\n\r\n");


NONSHARABLE_CLASS(CUdpClientTestUPnP12) : public CUdpClientTestBase,
										public MSocketHandlerObserver,
										public Messages::ASimpleNodeIdBase,
										public MTestServerObserver
	{
public:
	static CUdpClientTestUPnP12* NewL ( CProxyProvd* aProvd );

	//MTestServerObserver
	void OnTestServerEventL ( TInt aError );

	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	virtual void OpenComplete ( RInternalSocket& aSocket );
	inline virtual void ConnectComplete ();
	inline virtual void AcceptComplete ( RInternalSocket&  );
	inline virtual void SendComplete ( TInt  );
	inline virtual void SendToComplete ( TInt  );
	inline virtual void RecvComplete ( RMBufChain&  );
	inline virtual void RecvFromComplete ( RMBufChain& , const TSockAddr&  );
	inline virtual void Error ( TOperation , TInt   );

private:
	enum TState
		{
		ECreateTestServer = 0,
		ECreateTestClient,
		EOpenTestSocket,
		ESendRequestData,
		EFailed,
		ECleanup,
		};
		
	CUdpClientTestUPnP12 ( CProxyProvd* aProvd );
	~CUdpClientTestUPnP12();
	void ConstructL ();
	virtual TVerdict RunTestL ();
	//void Cleanup();

private:	
	CTestMUdpServer*	iTestServer;	
	Messages::TNodeId 	iClientId;
	RTimer				iTimer;
	TState				iState;
	
	RPointerArray <RInternalSocket>			iSocketArray;
	RSocketHandler			iSocketHandler;
	CSocketOpener* 			iSocketOpener;
	TUint 					iPort;
	};

void CUdpClientTestUPnP12::ConnectComplete ()
	{
	
	}
void CUdpClientTestUPnP12::AcceptComplete ( RInternalSocket&  )
	{
	
	}
void CUdpClientTestUPnP12::SendComplete ( TInt  )
	{
	
	}
void CUdpClientTestUPnP12::SendToComplete ( TInt  )
	{
	
	}
void CUdpClientTestUPnP12::RecvComplete ( RMBufChain&  )
	{
	
	}
void CUdpClientTestUPnP12::RecvFromComplete ( RMBufChain& , const TSockAddr&  )
	{
	
	}
void CUdpClientTestUPnP12::Error ( TOperation , TInt   )
	{
	
	}


#endif //PUDPCLIENTTESTS_H



