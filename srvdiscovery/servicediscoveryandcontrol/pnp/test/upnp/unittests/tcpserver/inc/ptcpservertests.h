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


#if !defined(PTCPSERVERTESTS_H)
#define PTCPSERVERTESTS_H

#include <e32base.h>
#include <testexecutestepbase.h>
#include "proxyprotocol.h"
#include <comms-infras/eintsock.h>
#include "es_mbuf.h"
#include <elements/nm_node.h>

#include <rsockethandler.h>
#include <csocketopener.h>
#include <cchunkmanager.h>
#include <rmemchunk.h>

NONSHARABLE_CLASS(CTcpServerTestBase) : public CActive
	{
public:
	static void StartTestL(const TDesC& aTestName, CProxyProvd* aProvd);
	virtual void RunL();

protected:		
	CTcpServerTestBase(CProxyProvd* aProvd);
	virtual ~CTcpServerTestBase();
	
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
	static CTcpServerTestBase* CreateTestL(const TDesC& aTestName, CProxyProvd* aProvd);
		
private:
	CProxyProvd* iProvd;
	TBool iReschedule; //Determines whether rescheduling is required
	};
	
inline CTestExecuteLogger& CTcpServerTestBase::Logger()
	{
	return iLogger;
	}

//----------------------------------------------------------//
/////////////////// CTcpServerTestUPnP /////////////////////////
//////Testcase: Send the test data from the test client/////////
//----------------------------------------------------------//


NONSHARABLE_CLASS(CTcpServerTestUPnP) : public CTcpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CTcpServerTestUPnP* NewL ( CProxyProvd* aProvd );
	~CTcpServerTestUPnP ();
	
	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket );
	void ConnectComplete ();
	void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	void SendComplete ( TInt aLength );	
	void SendToComplete ( TInt /*aLength*/ )
	{	}
	void RecvComplete ( RMBufChain& aData );
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
		EOpenClientSocketAndConnect,
		ESendData,
		ESendStopServer,
		EServerStopped,		
		ECleanup,
		};
		
	CTcpServerTestUPnP ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	Messages::TNodeId 	iTcpServer;
	RBuf8				iServiceUri;
	
	RInternalSocket		iClientSocket;
	RSocketHandler		iSocketHandler;
	CSocketOpener*		iSocketOpener;
	
	RMBufChain			iSendChain;
	TInetAddr			iHostAddr;
	TVerdict			iResponse;
	
	RTimer				iTimer;
	TState				iState;
	};

#endif //PTCPSERVERTESTS_H
