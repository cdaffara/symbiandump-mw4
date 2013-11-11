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


#ifndef __CTESTSERVERSOCKETHANDLER_H__
#define __CTESTSERVERSOCKETHANDLER_H__

#include <rsockethandler.h>
#include <es_mbuf.h>
#include <comms-infras/mbufchain.h>
#include <testexecutestepbase.h>

class MTcpClientTestCase;

class CTestServerSocketHandler: public CTimer,
								public MSocketHandlerObserver
	{
public:
	static CTestServerSocketHandler* NewL ( MTcpClientTestCase* aTestCase, RInternalSocket& aIncomingConnection, CTestExecuteLogger& aLogger );
	virtual ~CTestServerSocketHandler ();

private:
	CTestServerSocketHandler ( MTcpClientTestCase* aTestCase, RInternalSocket& aIncomingConnection, CTestExecuteLogger&	aLogger );
	void ConstructL ();
	TBool ProcessRequestL ();
	TBool IsConnectionCloseInData ( const TDesC8& aRequest, const TDesC8& aResponse ) const;
	TBool Is100ContinueInRequestData ( const TDesC8& aRequest ) const;
	void SendDataL ();
	
	// From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& /*Socket*/ )
	{	}
	void ConnectComplete () 
	{	}
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
		
	// From CTimer
	void RunL ();
	
private:
	RInternalSocket 		iIncomingConnection;
	RSocketHandler			iSocketHandler;
	HBufC8*					iDataStore;
	HBufC8*					iDataToSend;
	RMBufChain				iSendChain;
	MTcpClientTestCase*		iTestCase;		// Not owned
	TInt					iTransCount;
	TInt					iCurrentTrans;
	TInt					iDataPos;
	TBool					iCloseConnection;
	TBool					iPost;
	TBool					iSend100Continue;
	CTestExecuteLogger&		iLogger;
	};

#endif // __CTESTSERVERSOCKETHANDLER_H__
