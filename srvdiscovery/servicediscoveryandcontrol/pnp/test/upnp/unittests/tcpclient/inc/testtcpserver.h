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



#ifndef __TESTTCPSERVER_H_
#define __TESTTCPSERVER_H_

// System Includes
#include <comms-infras/eintsock.h>
#include <es_mbuf.h>
#include <comms-infras/mbufchain.h>

#include <rsockethandler.h>
#include <csocketopener.h>

class MTestServerObserver;
class CTestServerSocketHandler;
class MTcpClientTestCase;

//----------------------------------------------------------//
/////////////////// CTestTcpServer /////////////////////////
//----------------------------------------------------------//

class CTestTcpServer : public CBase,
						public MSocketHandlerObserver
	{
public:	
	static CTestTcpServer* NewL ( MTestServerObserver& aObserver );
	~CTestTcpServer ();
	
private:
	TInt OpenSocket ();
	TInt Startup ();
	void Shutdown ();
	void DoSendL ( RMBufChain& aData, TBool& aRecvAgain );
	void DoAccept ();
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket ) ;
	void ConnectComplete () 
	{	}
	void AcceptComplete ( RInternalSocket& aSocket );
	void SendComplete ( TInt /*aLength*/ )
	{	}
	void SendToComplete ( TInt /*aLength*/ )
	{	}
	void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	CTestTcpServer ( MTestServerObserver& aObserver );
	void ConstructL ();		

private:
	MTestServerObserver& iObserver;
	RSocketHandler		iSocketHandler;
	RInternalSocket 	iSocket;
	CSocketOpener*		iSocketOpener;
	
	MTcpClientTestCase*        iTestCase;      // Not owned
	RPointerArray<CTestServerSocketHandler> iIncomingHandlers;
	};

#endif /*__TESTTCPSERVER_H_*/
