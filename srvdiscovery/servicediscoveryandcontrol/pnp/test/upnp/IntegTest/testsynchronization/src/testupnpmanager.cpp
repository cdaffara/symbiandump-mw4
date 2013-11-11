// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Contains implementation of CTestUPnPManager class
// 
//

/**
 @file
 @internalTechnology
*/

#include "testupnpmanager.h"


/*
  Constructor:
  @internalTechnology
  @test
*/
CTestUPnPManager::CTestUPnPManager()
:	CTestStep()
	{
	}

/*
   Initializes all member vaiables.
   @param		None.
   @return		None.
*/
void CTestUPnPManager::InitializeL()
	{
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	}

/*
   Destructor
   @internalTechnology
   @test
*/
CTestUPnPManager::~CTestUPnPManager()
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	}

/*
   Implementation of CTestStep base class virtual and this is used for doing all
   initialisation common
    to derived classes here.
   @param  None
   @return TVerdict
 */
TVerdict CTestUPnPManager::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

/*
   Implementation of CTestStep base class virtual and it is used for doing all
   after test treatment common to derived classes in here.
   @return TVerdict
 */
TVerdict CTestUPnPManager::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}


TVerdict CTestUPnPManager::doTestStepL()
	{
	return TestStepResult();	
	}


void CTestUPnPManager::ConnectClientL()
    {

	
    RSocketServ 	iSockServ;
    RConnection 	iConnection;
    RSubConnection 	iSubConnection;

	TInt error = KErrNone;
	error = iSockServ.Connect();
	error = iConnection.Open(iSockServ);

	TCommDbConnPref prefs;
	prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	prefs.SetDirection(ECommDbConnectionDirectionOutgoing);

	error = iConnection.Start();
	error = iSubConnection.Open(iSockServ, RSubConnection::EAttachToDefault, iConnection);

	User::LeaveIfError(error);

	_LIT(KServerIPAddress, "10.192.192.138");
	TInt serverPort = 7890;
	TInetAddr destAddr;
	destAddr.Input(KServerIPAddress);
	destAddr.SetPort(serverPort);



	FOREVER
		{
		TRequestStatus	status;
		RSocket	socket;
		TInt error = KErrNone;
		TInt64 numOfRetries = 0;
		const TInt64 KMaxNumOfRetries = 1152921504606846976;


		error = socket.Open(iSockServ, KAfInet, KSockStream, KProtocolInetTcp, iSubConnection);
		User::LeaveIfError(error);
		socket.Connect(destAddr, status);
		User::WaitForRequest(status);

		if(KErrNone == status.Int())
			{
			_LIT(KHostReachable, "Host reachable \n");
			INFO_PRINTF1(KHostReachable);
			socket.Close();
			iSubConnection.Close();
			iConnection.Close();
			iSockServ.Close();
			return;
			}
		else
			{
			_LIT(KHostNotReachable, "Host NOT reachable \n");
			INFO_PRINTF1(KHostNotReachable);
			numOfRetries++;
			socket.Close();
			if(numOfRetries > KMaxNumOfRetries)
				{
				iSubConnection.Close();
				iConnection.Close();
				iSockServ.Close();
				return;
				}
			}
		}
    }


void CTestUPnPManager::ConnectServerL()
    {
	RSocketServ 		iSockServ;
	RConnection 		iConnection;
	RSubConnection 		iSubConnection;
	RSocket 			iSocket;
	RSocket				iConnectedSocket;
	TRequestStatus 		status;



	TInt serverPort = 7890;

	TInt error = KErrNone;
	error = iSockServ.Connect();
	error = iConnection.Open(iSockServ);

	error = iConnection.Start();
	error = iSubConnection.Open(iSockServ, RSubConnection::EAttachToDefault, iConnection);

	TInetAddr anyAddrOnPort(KInetAddrAny,serverPort);
	error = iSocket.Open(iSockServ, KAfInet, KSockStream, KProtocolInetTcp, iSubConnection);
	error = iSocket.Bind(anyAddrOnPort);
	error = iConnectedSocket.Open(iSockServ);
	User::LeaveIfError(error);


	const TInt KQueueSize = 3;
	iSocket.Listen(KQueueSize);
	iSocket.Accept(iConnectedSocket,status);
	User::WaitForRequest(status);

	_LIT(KConnectionEstablished, "connection established \n");
	INFO_PRINTF1(KConnectionEstablished);

	iConnectedSocket.Close();
	iSocket.Close();
	iSubConnection.Close();
	iConnection.Close();
	iSockServ.Close();
    }


