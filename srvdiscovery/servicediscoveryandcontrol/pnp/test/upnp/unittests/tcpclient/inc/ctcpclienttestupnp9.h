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

#ifndef CTCPCLIENTTESTUPNP9_H_
#define CTCPCLIENTTESTUPNP9_H_

//----------------------------------------------------------//
/////////////////// CTcpClientTestUPnP9 /////////////////////////
//////Testcase: Send the test data from the test client/////////
//----------------------------------------------------------//
//#include "atestsocket.h"

NONSHARABLE_CLASS(CTcpClientTestUPnP9) : public CTcpClientTestBase,
										public MTestServerObserver,
										public MTcpClientTestCase,
										public MHttpEventObserver
	{
public:
	static CTcpClientTestUPnP9* NewL ( CProxyProvd* aProvd );

	//MTestServerObserver
	void OnTestServerEventL ( TInt aError );
	MTcpClientTestCase* GetTestCase ();
	CTestExecuteLogger& GetLogger ();
	//MTcpClientTestCase
	const TDesC& TestCaseName () const;
	TInt TotalTransactionCount () const;	
	TInt TransactionCount () const;
	
	const TDesC8& GetRawRequest ( TInt aTransIndex );
	const TDesC8& GetRawResponse ( TInt aTransIndex );
	
	//void NewData(TUint aCount);
				
	~CTcpClientTestUPnP9 ();
	
	// From MHttpEventObserver
	TInt OnHttpEvent ( CTransaction* aTransaction, THTTPEvent& aEvent );
	
	//void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
private:
		
	CTcpClientTestUPnP9 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
	CHTTPClientTransaction* PrepareTransactionL ();
	
private:	
	CTestTcpServer*				iTestServer;
	RTimer						iTimer;
	CHTTPClientHandler* 		iClientHandler;
	RStringPool					iStringPool;
	CHeaderCodec*				iCodec;
	CHTTPClientTransaction* 	iTransaction;
	};

///////////////////////////////////////////////////////////////////////////////
NONSHARABLE_CLASS(CTcpClientTestUPnP16) : public CTcpClientTestBase,
										public MTestServerObserver,
										public MTcpClientTestCase,
										public MHttpEventObserver
	{
public:
	static CTcpClientTestUPnP16* NewL ( CProxyProvd* aProvd );

	//MTestServerObserver
	void OnTestServerEventL ( TInt aError );
	MTcpClientTestCase* GetTestCase ();
	CTestExecuteLogger& GetLogger ();
	//MTcpClientTestCase
	const TDesC& TestCaseName () const;
	TInt TotalTransactionCount () const;	
	TInt TransactionCount () const;
	
	const TDesC8& GetRawRequest ( TInt aTransIndex );
	const TDesC8& GetRawResponse ( TInt aTransIndex );
	
	//void NewData(TUint aCount);
				
	~CTcpClientTestUPnP16 ();
	
	// From MHttpEventObserver
	TInt OnHttpEvent ( CTransaction* aTransaction, THTTPEvent& aEvent );
	
	//void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
private:
		
	CTcpClientTestUPnP16 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
	CHTTPClientTransaction* PrepareTransactionL ();
	
private:	
	CTestTcpServer*				iTestServer;
	RTimer						iTimer;
	CHTTPClientHandler* 		iClientHandler;
	RStringPool					iStringPool;
	CHeaderCodec*				iCodec;
	CHTTPClientTransaction* 	iTransaction;
	};

///////////////////////////////////////////////////////////////////////////////
NONSHARABLE_CLASS(CTcpClientTestUPnP17) : public CTcpClientTestBase,
										public MTestServerObserver,
										public MTcpClientTestCase,
										public MHttpEventObserver
	{
public:
	static CTcpClientTestUPnP17* NewL ( CProxyProvd* aProvd );

	//MTestServerObserver
	void OnTestServerEventL ( TInt aError );
	MTcpClientTestCase* GetTestCase ();
	CTestExecuteLogger& GetLogger ();
	//MTcpClientTestCase
	const TDesC& TestCaseName () const;
	TInt TotalTransactionCount () const;	
	TInt TransactionCount () const;
	
	const TDesC8& GetRawRequest ( TInt aTransIndex );
	const TDesC8& GetRawResponse ( TInt aTransIndex );
	
	//void NewData(TUint aCount);
				
	~CTcpClientTestUPnP17 ();
	
	// From MHttpEventObserver
	TInt OnHttpEvent ( CTransaction* aTransaction, THTTPEvent& aEvent );
	
	//void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
private:
		
	CTcpClientTestUPnP17 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
	CHTTPClientTransaction* PrepareTransactionL ();
	
private:	
	CTestTcpServer*				iTestServer;
	RTimer						iTimer;
	CHTTPClientHandler* 		iClientHandler;
	RStringPool					iStringPool;
	CHeaderCodec*				iCodec;
	CHTTPClientTransaction* 	iTransaction;
	};


#endif /*CTCPCLIENTTESTUPNP9_H_*/
