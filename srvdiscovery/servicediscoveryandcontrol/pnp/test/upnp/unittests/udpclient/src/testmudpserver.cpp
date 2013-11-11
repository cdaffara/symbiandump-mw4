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
#include "testmudpserver.h"
#include "pudpclienttests.h"
#include "upnp_cf_msgs.h"


const TUint32 KMulticastAddrIp = INET_ADDR ( 239, 255, 255, 250 );
const TUint KMulticastAddrPort = 1900;

CTestMUdpServer* CTestMUdpServer::NewL ( MTestServerObserver& aObserver )
	{
	CTestMUdpServer* self = new ( ELeave ) CTestMUdpServer ( aObserver );
	CleanupStack::PushL ( self );
	self->ConstructL ();
	CleanupStack::Pop ();
	return self;		
	}

CTestMUdpServer::CTestMUdpServer ( MTestServerObserver& aObserver ) 
	: iObserver ( aObserver ), iSocketHandler ( *this )
	{	
	}
	
void CTestMUdpServer::ConstructL ()
	{
	User::LeaveIfError ( OpenSocket () );	
	TUpnpMessage::RegisterL ( SockManGlobals::Get ()->SelfWorker ());
	}

CTestMUdpServer::~CTestMUdpServer ()
	{
	Shutdown ();
	iSocketHandler.CancelAll ();
	iSocket.Close ();
	delete iSocketOpener;
	TUpnpMessage::DeRegister ();
	}
	
TInt CTestMUdpServer::OpenSocket ()
	{
	TInt err = KErrNone;
	if ( NULL == iSocketOpener )
		{
		TRAP ( err, iSocketOpener = CSocketOpener::NewL ( *this ) );
		}

	if ( err == KErrNone )
		iSocketOpener->MakeSocket ( KAfInet, KSockDatagram, KProtocolInetUdp );

	return err;
	}
	
void CTestMUdpServer::Startup ()
	{//start listening to the mutlicast addresss
	//TInt ret = KErrNone;	
	TInetAddr addr;
  	addr.SetAddress ( KMulticastAddrIp );
  	addr.SetPort ( KMulticastAddrPort );

	if ( addr.Family() != KAfInet6 )
		{
		addr.ConvertToV4Mapped();				
		}
	TPckgBuf<TIp6Mreq> mReqBuf;
	mReqBuf().iAddr = addr.Ip6Address();
	mReqBuf().iInterface = 0;
		
	iSocket.SetOpt( KSoIp6JoinGroup, KSolInetIp, mReqBuf ); // Join the multicast group
	
	iSocketHandler.RecvFrom (); // Start the receive
	}
	
void CTestMUdpServer::Shutdown ()
	{
	TInetAddr addr;
  	addr.SetAddress ( KMulticastAddrIp );
  	addr.SetPort ( KMulticastAddrPort );

	if ( addr.Family() != KAfInet6 )
		{
		addr.ConvertToV4Mapped();				
		}
	TPckgBuf<TIp6Mreq> mReqBuf;
	mReqBuf().iAddr = addr.Ip6Address();
	mReqBuf().iInterface = 0;
		
	iSocket.SetOpt( KSoIp6LeaveGroup, KSolInetIp, mReqBuf ); // leave the multicast group		
	}
	
void CTestMUdpServer::OpenComplete ( RInternalSocket& aSocket )
	{
	iSocket = aSocket;
	iSocketHandler.Attach ( aSocket );
	TInetAddr addr ( KMulticastAddrPort );

	addr.SetAddress ( KInetAddrAny );
	TInt err = aSocket.Bind ( addr );

	if ( err == KErrNone )
		Startup ();
	TRAP_IGNORE(iObserver.OnTestServerEventL ( err ));
	}	
	
void CTestMUdpServer::RecvFromComplete ( RMBufChain& aData, const TSockAddr& aAddr )
	{//data/request completely recd
	iSendToAddr = aAddr;
	TBool recvAgain = EFalse;
	TRAP_IGNORE ( DoSendL ( aData, recvAgain ) );	
	aData.Free ();
	
	if ( recvAgain )
		{
		iSocketHandler.RecvFrom (); // Start the receive from the  multicast address
		}
	}
	
void CTestMUdpServer::DoSendL ( RMBufChain& aData, TBool& aRecvAgain )
	{//send the packet to the multicast address
	RBuf8 dataRcvd;
	dataRcvd.CreateMaxL ( aData.Length () );	//aData is the m-search request received..its a dummy request
	aData.CopyOut ( dataRcvd );
	
	if ( dataRcvd.FindF ( KTestCase1 ) != KErrNotFound )	//receives the requests
		{//if request is "TestCase1" then execute test case1 and send a response with missing content length
		//this response is captured by the client CTestUpnpClient# object & validated
		iSendChain.CreateL ( KInvalidCL );		//send responses
		iSocketHandler.SendTo ( iSendChain, iSendToAddr );			//send to the client
	    }
	else if ( dataRcvd.FindF ( KTestCase2 ) != KErrNotFound )
		{
		iSendChain.CreateL ( KMissingST );					//send invalid response
		iSocketHandler.SendTo ( iSendChain, iSendToAddr );        
        }
    else if ( dataRcvd.FindF ( KTestCase3 ) != KErrNotFound )
		{
		iSendChain.CreateL ( KMissingRespLocn );					//send invalid response
		iSocketHandler.SendTo ( iSendChain, iSendToAddr );        
        }
    else if ( dataRcvd.FindF ( KTestCase4 ) != KErrNotFound )
		{
		iSendChain.CreateL ( KMissingContentLength );					//send invalid response
		iSocketHandler.SendTo ( iSendChain, iSendToAddr );        
        }
    else if ( dataRcvd.FindF ( KTestCase5 ) != KErrNotFound )
		{
		iSendChain.CreateL ( KMissingRespUSN );					//send invalid response
		iSocketHandler.SendTo ( iSendChain, iSendToAddr );        
        }
     else if ( dataRcvd.FindF ( KTestCase6 ) != KErrNotFound )
		{
		iSendChain.CreateL ( KInvalidCacheControl );					//send invalid response
		iSocketHandler.SendTo ( iSendChain, iSendToAddr );        
        }        
      else if ( dataRcvd.FindF ( KTestCase7 ) != KErrNotFound )
		{
		iSendChain.CreateL ( KMissingExtn );					//send invalid response
		iSocketHandler.SendTo ( iSendChain, iSendToAddr );        
        }
      else if ( dataRcvd.FindF ( KTestCase8 ) != KErrNotFound )
      	{
      	iSendChain.CreateL ( KInvalidRes8 );					//send invalid response
      	iSocketHandler.SendTo ( iSendChain, iSendToAddr );        
        }
      else if ( dataRcvd.FindF ( KTestCase9 ) != KErrNotFound )
      	{
      	iSendChain.CreateL ( KInvalidRes9 );					//send invalid response
      	iSocketHandler.SendTo ( iSendChain, iSendToAddr );        
        }
      else if ( dataRcvd.FindF ( KTestCase10 ) != KErrNotFound )
      	{
      	iSendChain.CreateL ( KInvalidRes10 );					//send invalid response
      	iSocketHandler.SendTo ( iSendChain, iSendToAddr );        
        }
      else if ( dataRcvd.FindF ( KTestCase11 ) != KErrNotFound )
      	{
      	iSendChain.CreateL ( KInvalidRes11 );					//send invalid response
      	iSocketHandler.SendTo ( iSendChain, iSendToAddr );        
        }
      else if ( dataRcvd.FindF ( KTestCase12 ) != KErrNotFound )
      	{
      	iSendChain.CreateL ( KInvalidRes12 );					//send invalid response
      	iSocketHandler.SendTo ( iSendChain, iSendToAddr );        
        }
    else
    	{
    	aRecvAgain = ETrue;
    	}
	
	dataRcvd.Close ();	
	}
	
void CTestMUdpServer::SendToComplete ( TInt /*aLength*/ )
	{	
	iSendChain.Init ();
	iSocketHandler.RecvFrom (); // Start the receive
	}

