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


#ifndef CTCPCLIENTTESTUPNP5_H_
#define CTCPCLIENTTESTUPNP5_H_

//----------------------------------------------------------//
/////////////////// CTcpClientTestUPnP5 /////////////////////////
//////Testcase: Send the test data from the test client/////////
//----------------------------------------------------------//

NONSHARABLE_CLASS(CTcpClientTestUPnP5) : public CTcpClientTestBase,
										public Messages::ASimpleNodeIdBase,
										public MTestServerObserver,
										public MTcpClientTestCase
	{
public:
	static CTcpClientTestUPnP5* NewL ( CProxyProvd* aProvd );

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
	

	~CTcpClientTestUPnP5 ();

	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
private:
		
	CTcpClientTestUPnP5 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	CTestTcpServer*		iTestServer;
	Messages::TNodeId 	iClientId;	
	RTimer				iTimer;
	};


#endif /*CTCPCLIENTTESTUPNP5_H_*/
