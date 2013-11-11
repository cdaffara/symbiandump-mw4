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



#ifndef CTCPSERVERTESTUPNP17_H_
#define CTCPSERVERTESTUPNP17_H_

//----------------------------------------------------------//
/////////////////// CTcpServerTestUPnP17 /////////////////////////
//////Testcase: Send the test data from the test client/////////
//----------------------------------------------------------//


NONSHARABLE_CLASS(CTcpServerTestUPnP17) : public CTcpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	static CTcpServerTestUPnP17* NewL ( CProxyProvd* aProvd );
	~CTcpServerTestUPnP17 ();
	
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
		
	CTcpServerTestUPnP17 ( CProxyProvd* aProvd );
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


#endif /*CTCPSERVERTESTUPNP17_H_*/
