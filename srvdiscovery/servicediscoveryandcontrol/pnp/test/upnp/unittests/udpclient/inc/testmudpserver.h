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



#ifndef __TESTMUDPSERVER_H_
#define __TESTMUDPSERVER_H_

// System Includes
#include <comms-infras/eintsock.h>
#include <es_mbuf.h>
#include <comms-infras/mbufchain.h>

#include <rsockethandler.h>
#include <csocketopener.h>
class MTestServerObserver;

//----------------------------------------------------------//
/////////////////// CTestMUdpServer /////////////////////////
//----------------------------------------------------------//

class CTestMUdpServer : public CBase,
						public MSocketHandlerObserver
	{
public:	
	static CTestMUdpServer* NewL ( MTestServerObserver& aObserver );
	~CTestMUdpServer ();
	
private:
	TInt OpenSocket ();
	void Startup ();
	void Shutdown ();
	void CTestMUdpServer::DoSendL ( RMBufChain& aData, TBool& aRecvAgain );
	
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
	void RecvFromComplete ( RMBufChain& aData, const TSockAddr& aAddr );	
	void IoctlComplete ()
	{	}
	void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
private:
	CTestMUdpServer ( MTestServerObserver& aObserver );
	void ConstructL ();		

private:
	MTestServerObserver& iObserver;
	RSocketHandler		iSocketHandler;
	RInternalSocket 	iSocket;
	CSocketOpener*		iSocketOpener;
	RMBufChain 			iSendChain;
	TSockAddr			iSendToAddr;
	};

#endif /*__TESTMUDPSERVER_H_*/
