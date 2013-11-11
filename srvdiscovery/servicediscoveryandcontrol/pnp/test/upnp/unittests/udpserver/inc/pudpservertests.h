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


#if !defined(PUDPSERVERTESTS_H)
#define PUDPSERVERTESTS_H

#include <e32base.h>
#include <testexecutestepbase.h>
#include "proxyprotocol.h"
#include <comms-infras/eintsock.h>
#include <cchunkmanager.h>
#include <rmemchunk.h>

#include "es_mbuf.h"
#include <elements/nm_node.h>
#include <rsockethandler.h>
#include <csocketopener.h>

NONSHARABLE_CLASS(CUdpServerTestBase) : protected CActive
	{
public:
	static void StartTestL(const TDesC& aTestName, CProxyProvd* aProvd);
	virtual void RunL();

protected:		
	CUdpServerTestBase(CProxyProvd* aProvd);
	virtual ~CUdpServerTestBase();
	
	//Utilities function to be use in RunTest()
	void Reschedule();
	CTestExecuteLogger iLogger;
	void CompleteSelf ( TInt aError );
	void FinishTest(TVerdict result);

	CChunkManager*  iChunkManager;
private:
	//Implemented you own of these		
	virtual TVerdict RunTestL() = 0;
	virtual void Cleanup();
	
	void DoCancel();
	void ConstructL();
	void SetLoggerL();
	CTestExecuteLogger& Logger();	
	static CUdpServerTestBase* CreateTestL(const TDesC& aTestName, CProxyProvd* aProvd);
		
private:
	CProxyProvd* iProvd;
	TBool iReschedule; //Determines whether rescheduling is required
	};
	
inline CTestExecuteLogger& CUdpServerTestBase::Logger()
	{
	return iLogger;
	}

//----------------------------------------------------------//
/////////////////// CUdpServerTestUPnP /////////////////////////
//////Testcase: Send the test data from the test client/////////
//----------------------------------------------------------//
_LIT8 ( KMissingMX, "M-SEARCH * HTTP/1.1\r\nMAN: \"ssdp:discover\"\r\nHOST: 239.255.255.250:1900\r\nST: ssdp:all\r\n\r\n");

NONSHARABLE_CLASS(CUdpServerTestUPnP) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}

private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
		
private:	
	Messages::TNodeId 	iMudpServer;
	RInternalSocket		iClientSocket;
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;		
	};


_LIT8 ( KMSearch, "M-SEARCH * HTTP/1.1\r\nMAN: \"ssdp:discover\"\r\nHOST: 239.255.255.250:1900\r\nMX: 120\r\nST: ssdp:all\r\n\r\n");
NONSHARABLE_CLASS(CUdpServerTestUPnP1) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP1* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP1 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP1 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	Messages::TNodeId 	iMudpServer;//produciton server module
	
	RInternalSocket		iClientSocket;//client socket
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;	
	};


//----------------------------------------------------------//
/////////////////// CUdpServerTestUPnP2 /////////////////////////
//////Testcase: Send the test data from the test client/////////
//----------------------------------------------------------//

_LIT8 ( KInvalidMX, "M-SEARCH * HTTP/1.1\r\nMAN: \"ssdp:discover\"\r\nHOST: 239.255.255.250:1900\r\nMX: 200\r\nST: ssdp:all\r\n\r\n");
NONSHARABLE_CLASS(CUdpServerTestUPnP2) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP2* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP2 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP2 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	Messages::TNodeId 	iMudpServer;//produciton server module
	
	RInternalSocket		iClientSocket;//client socket
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;	
	};
	
	
	
//---------------------------------------------------------------------------------------------------------
//----------------------------------- CUdpServerTestUPnP3--------------------------------------------------
//------------------------test case for M-search packet with invalid MAN header----------------------------
//---------------------------------------------------------------------------------------------------------

_LIT8 ( KInvalidMAN, "M-SEARCH * HTTP/1.1\r\nMAN: \"ssdp:hello\"\r\nHOST: 239.255.255.250:1900\r\nMX: 200\r\nST: ssdp:all\r\n\r\n");
NONSHARABLE_CLASS(CUdpServerTestUPnP3) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP3* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP3 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP3 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	Messages::TNodeId 	iMudpServer;//produciton server module
	
	RInternalSocket		iClientSocket;//client socket
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;	
	};

//-------------------------------------------------------------------------------------------------------------
_LIT8 ( KInvalidMethod, "GET / HTTP/1.1 \r\r\r\r\r\r\r\r\r\r\n\r\n");

NONSHARABLE_CLASS(CUdpServerTestUPnP4) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP4* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP4 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP4 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	Messages::TNodeId 	iMudpServer;//produciton server module
	
	RInternalSocket		iClientSocket;//client socket
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;	
	};


//-------------------------------------------------------------------------------------------------------------
_LIT8 ( KValidNotify, "NOTIFY * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nCACHE-CONTROL: max-age=1800\r\nLOCATION: http://10.192.196.204:54042/\r\nNT:upnp:rootdevice\r\nNTS:ssdp:alive\r\nSERVER: Apache 9.x, UPnP/1.0\r\nUSN: uuid:ae30fdec-16e3-4d75-ac72-b61c5d6d30d5::urn:schemas-upnp-org:device:BinaryLight:1\r\n\r\n");

NONSHARABLE_CLASS(CUdpServerTestUPnP5) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP5* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP5 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP5 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	Messages::TNodeId 	iMudpServer;//produciton server module
	
	RInternalSocket		iClientSocket;//client socket
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;	
	};

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------- CUdpServerTestUPnP6 --------------------------------------------------------------
//-------------------------------------Missing NTS header in notify packet-------------------------------------------------------

_LIT8 ( KMissingNTS, "NOTIFY * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nCACHE-CONTROL: max-age=1800\r\nLOCATION: http://10.192.196.204:54042/\r\nNT:upnp:rootdevice\r\nSERVER: Apache 9.x, UPnP/1.0\r\nUSN: uuid:ae30fdec-16e3-4d75-ac72-b61c5d6d30d5::urn:schemas-upnp-org:device:BinaryLight:1\r\n\r\n");

NONSHARABLE_CLASS(CUdpServerTestUPnP6) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP6* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP6 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP6 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	Messages::TNodeId 	iMudpServer;
	RInternalSocket		iClientSocket;
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;	
	};

//------------------------------------------------------------------------------------------------------------------------------

_LIT8 ( KInvalidNotifyByeBye, "NOTIFY * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nNTS:ssdp:alive\r\nUSN: uuid:ae30fdec-16e3-4d75-ac72-b61c5d6d30d5::urn:schemas-upnp-org:device:BinaryLight:1\r\n\r\n");
NONSHARABLE_CLASS(CUdpServerTestUPnP7) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP7* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP7 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP7 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	Messages::TNodeId 	iMudpServer;
	RInternalSocket		iClientSocket;
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;	
	};


//--------------------------------------------------------------------------------------------------------------------
_LIT8 ( KMissingNotifyLocn, "NOTIFY * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nCACHE-CONTROL: max-age=1800\r\nNT:upnp:rootdevice\r\nNTS:ssdp:alive\r\nSERVER: Apache 9.x, UPnP/1.0\r\nUSN: uuid:ae30fdec-16e3-4d75-ac72-b61c5d6d30d5::urn:schemas-upnp-org:device:BinaryLight:1\r\n\r\n");
NONSHARABLE_CLASS(CUdpServerTestUPnP8) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP8* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP8 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP8 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	Messages::TNodeId 	iMudpServer;
	RInternalSocket		iClientSocket;
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;	
	};

//-------------------------------------------------------------------------------------------------------------------------	
	
_LIT8 ( KMissingST, "M-SEARCH * HTTP/1.1\r\nMAN: \"ssdp:discover\"\r\nHOST: 239.255.255.250:1900\r\nMX: 120\r\n\r\n");

NONSHARABLE_CLASS(CUdpServerTestUPnP9) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP9* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP9 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP9 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	Messages::TNodeId 	iMudpServer;
	RInternalSocket		iClientSocket;
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;	
	};
	
	
	
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------


_LIT8 ( KInvalidHdr, "M-SEARCH * HTTP/1.1\r\nMAN: \"ssdp:discover\"\r\nMCADDR: 239.255.255.250:1900\r\nMX: 120\r\nST: ssdp:all\r\n\r\n");

NONSHARABLE_CLASS(CUdpServerTestUPnP10) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP10* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP10 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP10 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	Messages::TNodeId 	iMudpServer;
	RInternalSocket		iClientSocket;
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;	
	};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

_LIT8 ( KInvalidMaxAge, "NOTIFY * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nCACHE-CONTROL: \r\nLOCATION: http://10.192.196.204:54042/\r\nNT:upnp:rootdevice\r\nNTS:ssdp:alive\r\nSERVER: Apache 9.x, UPnP/1.0\r\nUSN: uuid:ae30fdec-16e3-4d75-ac72-b61c5d6d30d5::urn:schemas-upnp-org:device:BinaryLight:1\r\n\r\n");

NONSHARABLE_CLASS(CUdpServerTestUPnP11) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP11* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP11 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP11 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	Messages::TNodeId 	iMudpServer;//produciton server module
	
	RInternalSocket		iClientSocket;//client socket
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;	
	};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

_LIT8 ( KMissingNT, "NOTIFY * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nCACHE-CONTROL: max-age=1800\r\nLOCATION: http://10.192.196.204:54042/\r\nNTS:ssdp:alive\r\nSERVER: Apache 9.x, UPnP/1.0\r\nUSN: uuid:ae30fdec-16e3-4d75-ac72-b61c5d6d30d5::urn:schemas-upnp-org:device:BinaryLight:1\r\n\r\n");

NONSHARABLE_CLASS(CUdpServerTestUPnP12) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP12* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP12 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP12 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	Messages::TNodeId 	iMudpServer;//produciton server module
	
	RInternalSocket		iClientSocket;//client socket
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;	
	};
	
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

_LIT8 ( KMan, "MAN:\"ssdp:discover\"\r\n");
NONSHARABLE_CLASS(CUdpServerTestUPnP13) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver				

						
	{
public:
	static CUdpServerTestUPnP13* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP13 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ ){	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP13 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	Messages::TNodeId 	iMudpServer;//produciton server module
	
	RInternalSocket		iClientSocket;//client socket
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;	
	};
	
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------

NONSHARABLE_CLASS(CUdpServerTestUPnP14) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase							
					
	{
public:
	static CUdpServerTestUPnP14* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP14 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,	
		ESendErrorMsg,
		ESendMsg,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP14 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	Messages::TNodeId 	iMudpServer;//produciton server module
	TState				iState;
	RTimer				iTimer;
	};
/////////////////////////////////////////////////////////////////////////////////////////////
_LIT8 ( KInvalidSsdpPort, "M-SEARCH * HTTP/1.1\r\nMAN: \"ssdp:discover\"\r\nHOST: 239.255.255.250:1700\r\nST: ssdp:all\r\n\r\n");

NONSHARABLE_CLASS(CUdpServerTestUPnP15) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP15* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP15 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP15 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
		
private:	
	Messages::TNodeId 	iMudpServer;
	RInternalSocket		iClientSocket;
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;		
	};

/////////////////////////////////////////////////////////////////////////////////////////////
_LIT8 ( KMissingHost, "M-SEARCH * HTTP/1.1\r\nMAN: \"ssdp:discover\"\r\nST: ssdp:all\r\n\r\n");

NONSHARABLE_CLASS(CUdpServerTestUPnP16) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP16* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP16 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP16 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
		
private:	
	Messages::TNodeId 	iMudpServer;
	RInternalSocket		iClientSocket;
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;		
	};
/////////////////////////////////////////////////////////////////////////////////////////////
_LIT8 ( KMissingSsdpPort, "M-SEARCH * HTTP/1.1\r\nMAN: \"ssdp:discover\"\r\nHOST: 239.255.255.250:\r\nST: ssdp:all\r\n\r\n");

NONSHARABLE_CLASS(CUdpServerTestUPnP17) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP17* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP17 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP17 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
		
private:	
	Messages::TNodeId 	iMudpServer;
	RInternalSocket		iClientSocket;
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;		
	};
/////////////////////////////////////////////////////////////////////////////////////////////
_LIT8 ( KMissingSsdpPort1, "M-SEARCH * HTTP/1.1\r\nMAN: \"ssdp:discover\"\r\nHOST: 239.255.255.250\r\nST: ssdp:all\r\n\r\n");

NONSHARABLE_CLASS(CUdpServerTestUPnP18) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP18* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP18 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP18 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
		
private:	
	Messages::TNodeId 	iMudpServer;
	RInternalSocket		iClientSocket;
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;		
	};
/////////////////////////////////////////////////////////////////////////////////////////////
_LIT8 ( KInvalidSsdpHost, "M-SEARCH * HTTP/1.1\r\nMAN: \"ssdp:discover\"\r\nHOST: 239.255.250.250:1900\r\nST: ssdp:all\r\n\r\n");

NONSHARABLE_CLASS(CUdpServerTestUPnP19) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP19* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP19 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP19 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
		
private:	
	Messages::TNodeId 	iMudpServer;
	RInternalSocket		iClientSocket;
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;		
	};
/////////////////////////////////////////////////////////////////////////////////////////////
_LIT8 ( KInvalidHostMissingSsdpPort, "M-SEARCH * HTTP/1.1\r\nMAN: \"ssdp:discover\"\r\nHOST: 239.255.250.250\r\nST: ssdp:all\r\n\r\n");

NONSHARABLE_CLASS(CUdpServerTestUPnP20) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP20* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP20 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP20 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
		
private:	
	Messages::TNodeId 	iMudpServer;
	RInternalSocket		iClientSocket;
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;		
	};

//////////////////////////////////////////////////////////////////////////////////////////////
_LIT8 ( KMissingMan, "M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nST: ssdp:all\r\n\r\n");

NONSHARABLE_CLASS(CUdpServerTestUPnP21) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP21* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP21 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP21 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
		
private:	
	Messages::TNodeId 	iMudpServer;
	RInternalSocket		iClientSocket;
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;		
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////
NONSHARABLE_CLASS(CUdpServerTestUPnP22) : public CUdpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CUdpServerTestUPnP22* NewL ( CProxyProvd* aProvd );
	~CUdpServerTestUPnP22 ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocket,
		ESendData,
		ESendStopServer,
		EServerStopped,
		EFailed,
		ECleanup
		};
		
	CUdpServerTestUPnP22 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	Messages::TNodeId 	iMudpServer;//produciton server module
	
	RInternalSocket		iClientSocket;//client socket
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	RTimer				iTimer;
	TState				iState;
	RMBufChain			iSendChain;
	TInetAddr			iSendToAddr;	
	};



#endif //PUDPSERVERTESTS_H
