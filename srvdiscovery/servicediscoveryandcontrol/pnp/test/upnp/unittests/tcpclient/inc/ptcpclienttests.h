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


#if !defined(PTCPCLIENTTESTS_H)
#define PTCPCLIENTTESTS_H

#include <e32base.h>
#include <testexecutestepbase.h>
#include "proxyprotocol.h"
#include <comms-infras/eintsock.h>
#include "es_mbuf.h"
#include "testtcpserver.h"
#include <elements/nm_node.h>
#include "mtcpclienttestcase.h"
#include <cchunkmanager.h>
#include <rmemchunk.h>

_LIT8 ( KTestCase1, "testcase1" );
_LIT8 ( KMissingContentLength, "HTTP/1.1 200 OK\r\nST: testcase1\r\nEXT: \r\nSERVER: Apache 9.x, UPnP/1.0\r\nUSN: uuid:ae30fdec-16e3-4d75-ac72-b61c5d6d30d5::urn:schemas-upnp-org:device:BinaryLight:1\r\nCACHE-CONTROL: max-age=900\r\nLOCATION: http://10.192.196.204:54042/\r\n\r\n");


NONSHARABLE_CLASS(CTcpClientTestBase) : protected CActive
	{
public:
	static void StartTestL(const TDesC& aTestName, CProxyProvd* aProvd);
	virtual void RunL();

protected:		
	CTcpClientTestBase(CProxyProvd* aProvd);
	virtual ~CTcpClientTestBase();
	
	//Utilities function to be use in RunTest()
	void Reschedule();
	CTestExecuteLogger iLogger;
	void CompleteSelf ( TInt aError );
	void FinishTest(TVerdict result);
	enum TState
		{
		ECreateTestServer = 0,
		ECreateTestClient,
		ESendRequestData,
		ECleanup,
		EComplete,
		};
	
	RMemoryAllocator 			iAllocator;
	CChunkManager*  iChunkManager;		
	TState				iState;	
private:

	
	//Implemented you own of these		
	virtual TVerdict RunTestL() = 0;
	virtual void Cleanup();
	
	void DoCancel();
	void ConstructL();
	void SetLoggerL();
	CTestExecuteLogger& Logger();	
	static CTcpClientTestBase* CreateTestL(const TDesC& aTestName, CProxyProvd* aProvd);
		
private:
	CProxyProvd* iProvd;
	TBool iReschedule; //Determines whether rescheduling is required
	};
	
inline CTestExecuteLogger& CTcpClientTestBase::Logger()
	{
	return iLogger;
	}


class MTestServerObserver
	{
public:
	/**
	Callback method which returns the responses.
	@param aObserverInfo bundle containg the response.
	*/
	virtual void OnTestServerEventL ( TInt aError ) = 0;
	
	
	virtual MTcpClientTestCase* GetTestCase ( ) = 0;
	
	virtual CTestExecuteLogger& GetLogger ( ) = 0;
	//virtual void OnTestServerEventL ( TBool aFlag) = 0;
	/**
	Callback method which handles the errors of OnPnPEventL.
	@param aError error code.
	*/
	//virtual void OnTestServerError (TInt aError) = 0;
	};

/*class MTestClientObserver
	{
public:
	/**
	Callback method which returns the responses.
	@param aObserverInfo bundle containg the response.
	*
	virtual void OnTestClientEventL ( TObserverBundle& aObserverInfo) = 0;
	/**
	Callback method which handles the errors of OnPnPEventL.
	@param aError error code.
	*
	virtual void OnTestClientError (TInt aError) = 0;
	};
*/
//----------------------------------------------------------//
/////////////////// CTcpClientTestUPnP /////////////////////////
//////Testcase: Send the test data from the test client/////////
//----------------------------------------------------------//

NONSHARABLE_CLASS(CTcpClientTestUPnP) : public CTcpClientTestBase,
										public Messages::ASimpleNodeIdBase,
										public MTestServerObserver,
										public MTcpClientTestCase
	{
public:
	static CTcpClientTestUPnP* NewL ( CProxyProvd* aProvd );

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
	

	~CTcpClientTestUPnP ();

	void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
private:
	
	CTcpClientTestUPnP ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
private:	
	CTestTcpServer*		iTestServer;
	Messages::TNodeId 	iClientId;
	RTimer				iTimer;
	//TState				iState;
	};

#endif //PTCPCLIENTTESTS_H
