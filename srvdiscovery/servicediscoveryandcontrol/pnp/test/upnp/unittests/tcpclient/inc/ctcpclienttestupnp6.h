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

#ifndef CTCPCLIENTTESTUPNP6_H_
#define CTCPCLIENTTESTUPNP6_H_

//----------------------------------------------------------//
/////////////////// CTcpClientTestUPnP6 /////////////////////////
//////Testcase: Send the test data from the test client/////////
//----------------------------------------------------------//
#include "atestsocket.h"

NONSHARABLE_CLASS(CTcpClientTestUPnP6) : public CTcpClientTestBase,
										public Messages::ASimpleNodeIdBase,
										public MTestServerObserver,
										public ATestSocket,
										public MTcpClientTestCase
	{
public:
	static CTcpClientTestUPnP6* NewL ( CProxyProvd* aProvd );

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
	
	void NewData(TUint aCount);
				
	~CTcpClientTestUPnP6 ();

	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
private:
		
	CTcpClientTestUPnP6 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	CTestTcpServer*				iTestServer;
	Messages::TNodeId 			iClientId;	
	RTimer						iTimer;
		
	CSubConnectionFlowBase* 	iFlow;
	MSessionData* 				iSSPData;
	MFlowBinderControl* 		iFlowBinder;
	MSessionControl* 			iSSP;
	
	};

#endif /*CTCPCLIENTTESTUPNP6_H_*/


