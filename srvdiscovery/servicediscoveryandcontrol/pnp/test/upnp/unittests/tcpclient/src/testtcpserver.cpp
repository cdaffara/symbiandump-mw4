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

#include <ss_glob.h>
#include <comms-infras/ss_thread.h>

#include "testtcpserver.h"
#include "ctestserversockethandler.h"
#include "ptcpclienttests.h"
#include "upnp_cf_msgs.h"

const TUint KListenQSize = 5;
const TUint KListenPort = 80;

CTestTcpServer* CTestTcpServer::NewL ( MTestServerObserver& aObserver )
	{
	CTestTcpServer* self = new ( ELeave ) CTestTcpServer ( aObserver );
	CleanupStack::PushL ( self );
	self->ConstructL ();
	CleanupStack::Pop ();
	return self;		
	}

CTestTcpServer::CTestTcpServer ( MTestServerObserver& aObserver ) 
	: iObserver ( aObserver ), iSocketHandler ( *this )
	{	
	}
	
void CTestTcpServer::ConstructL ()
	{
	User::LeaveIfError ( OpenSocket () );	
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	}

CTestTcpServer::~CTestTcpServer ()
	{
	iIncomingHandlers.ResetAndDestroy ();
	iSocketHandler.CancelAll ();
	iSocket.Close ();
	delete iSocketOpener;
		
	TUpnpMessage::DeRegister ();
	}
	
TInt CTestTcpServer::OpenSocket ()
	{
	TInt err = KErrNone;
	if ( NULL == iSocketOpener )
		{
		TRAP ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );
		}
	
	if ( err == KErrNone )
		iSocketOpener->MakeSocket ( KAfInet, KSockStream, KProtocolInetTcp );
	
	return err;
	}
	
void CTestTcpServer::DoAccept ()
	{
	iSocketHandler.Accept();		
	}

TInt CTestTcpServer::Startup ()
	{
	// Start listening and accepting for new connections
	TInt err = iSocket.Listen ( KListenQSize ); 
	if ( err == KErrNone )
		DoAccept ();
	return err;		
	}

void CTestTcpServer::OpenComplete ( RInternalSocket& aSocket )
	{
	iSocket = aSocket;
	iSocketHandler.Attach ( iSocket );
	
	TInetAddr addr ( KListenPort );
	addr.SetAddress ( KInetAddrAny );
	TInt err = iSocket.Bind ( addr );

	if ( err == KErrNone )
		{
		err = Startup ();
		}
		
	TRAP_IGNORE(iObserver.OnTestServerEventL ( err ));
	}	
	
void CTestTcpServer::AcceptComplete ( RInternalSocket& aSocket )
	{


	//iTestCase = reinterpret_cast<MTcpClientTestCase*> ( &iObserver );
	iTestCase = iObserver.GetTestCase();
	CTestServerSocketHandler* newSrvSockHandler = NULL;
	TRAP_IGNORE(newSrvSockHandler = CTestServerSocketHandler::NewL ( iTestCase, aSocket, iObserver.GetLogger()));
    iIncomingHandlers.Append ( newSrvSockHandler );

    DoAccept ();
	}
	

