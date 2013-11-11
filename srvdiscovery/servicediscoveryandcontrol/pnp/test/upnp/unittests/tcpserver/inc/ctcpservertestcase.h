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


#ifndef CTCPSERVERTESTCASE_H_
#define CTCPSERVERTESTCASE_H_

//----------------------------------------------------------//
/////////////////// CTcpServerTestCase /////////////////////////
//////Testcase: Send the test data from the test client/////////
//----------------------------------------------------------//
const TUint16 KActivityNull = 0;

NONSHARABLE_CLASS(CTcpServerTestCase) : public CTcpServerTestBase,
										public Messages::ASimpleNodeIdBase,
										public MSocketHandlerObserver										
	{
public:
	//static CTcpServerTestCase* NewL ( CProxyProvd* aProvd );
	virtual ~CTcpServerTestCase ();
	
	virtual void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	virtual void OpenComplete ( RInternalSocket& aSocket );
	virtual void ConnectComplete ();
	virtual void AcceptComplete ( RInternalSocket& /*aSocket*/ ) 
	{	}
	virtual void SendComplete ( TInt aLength );	
	virtual void SendToComplete ( TInt /*aLength*/ )
	{	}
	virtual void RecvComplete ( RMBufChain& /*aData*/ )
	{	}
	virtual void RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{	}
	virtual void IoctlComplete ()
	{	}
	virtual void Error ( TOperation /*aOperation*/, TInt /*aError*/ )
	{	}
	
protected:
	enum TState
		{
		ECreateServer = 0,
		EStartServer,
		EOpenClientSocketAndConnect,
		ESendData,
		ESendStopServer,
		EServerStopped,		
		ECleanup,		
		EStartTimer,
		ETimerStopped,
		};
		
	CTcpServerTestCase ( CProxyProvd* aProvd );
	void ConstructL (const TDesC8& aDes);
	virtual TVerdict RunTestL ();
	
	
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



#endif /*CTCPSERVERTESTCASE_H_*/
