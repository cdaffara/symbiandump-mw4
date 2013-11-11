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
//

#include "app_protintf_tcp.h"
#include <in_sock.h>
#include "app_protintf_msgs.h"
const TInt KListenQSize = 5;

using namespace Messages;

CApplicationProtocolIntfBase* CApplicationProtocolIntfTcp::NewL ( TInt aPort )
	{
	CApplicationProtocolIntfTcp* self = new (ELeave)CApplicationProtocolIntfTcp( aPort );
	return self;	
	}

CApplicationProtocolIntfTcp::CApplicationProtocolIntfTcp ( TInt aPort )
: CApplicationProtocolIntfBase ( aPort, KProtocolInetTcp ),iAcceptedSocketHandler (*this)
	{
	LOG_NODE_CREATE ( KESockFlowTag, CApplicationProtocolIntfTcp );	
	}

CApplicationProtocolIntfTcp::~CApplicationProtocolIntfTcp ()
	{
	Shutdown();
	LOG_NODE_DESTROY(KESockFlowTag, CApplicationProtocolIntfTcp);
	}

void CApplicationProtocolIntfTcp::ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage )
	{
	__ASSERT_DEBUG ( aMessage.MessageId ().Realm () == TAppProtIntfMessage::ERealmId, User::Panic ( KAppProtIntfInvalidRealmId, KInvalidReamId ) );
	
	CApplicationProtocolIntfBase::ReceivedL ( aSender, aRecipient, aMessage );
	switch ( aMessage.MessageId ().MessageId () )
		{
		case TAppProtIntfMessage::TJoin::EId:
			{
			InitiateLinkL ( address_cast<TNodeCtxId> ( aSender ), KSockStream, KProtocolInetTcp );
			}
			break;
		
		case TAppProtIntfMessage::TAcceptConnection::EId:
			{
			// Transfer the connection
			TAppProtIntfMessage::TTransferConnection msg ( iAcceptedSocket, iReceivedData );
			PostToLink ( address_cast<TNodeCtxId> ( aSender ), msg );
			
			RInternalSocket sock;
			iAcceptedSocket = sock;	 // Empty the accepted socket, as ownership is transferred
			iReceivedData.Init (); // Empty the received data
			DoAccept ();
			__ASSERT_DEBUG ( iReceivedData.IsEmpty(), User::Invariant() );
			}
			break;
			
		case TAppProtIntfMessage::TRejectConnection::EId:
			{
			// Note: If there is a rejection e should send the data to next links till last, one by one.
			// Currently we have only one link. Noone is there to accept this connection. So close the accepted socket
			// and keep in accept modefor new connections
			
			// The link should sent rejected data. If the rejected data is sent and we don't have any more links
			// we will send the rejected data and go into an accepting mode. 
			// If there is no rejected data we will close the accepted socket && directly go into an accepting mode 
			
			// For example: In the case of HTTP the link will send an internal error 500 or not found 400 response
						
			TAppProtIntfMessage::TRejectConnection& msg = message_cast < TAppProtIntfMessage::TRejectConnection > ( aMessage );
			iReceivedData.Free ();
			if ( msg.iData.Length() > 0 )
				{
				iAcceptedSocketHandler.Send ( msg.iData );
				}
			else
				{						
				iAcceptedSocket.Close();
				DoAccept ();
				}
			}
			break;
			
		case TAppProtIntfMessage::TMoreData::EId:
			{
			iReceivedData.Init();
			iAcceptedSocketHandler.Recv();
			break;
			}
		}
	}

void CApplicationProtocolIntfTcp::DoAccept ()
	{
	iSocketHandler.Accept();		
	}

TInt CApplicationProtocolIntfTcp::Startup ()
	{
	// Start listening and accepting for new connections
	TInt err = iSocket.Listen( KListenQSize ); 
	if ( err == KErrNone )
		DoAccept ();
	return err;		
	}

void CApplicationProtocolIntfTcp::Shutdown ()
	{	
	iAcceptedSocketHandler.CancelAll ();
	iAcceptedSocket.Close ();
	CApplicationProtocolIntfBase::Shutdown ();
	}

void CApplicationProtocolIntfTcp::AcceptComplete ( RInternalSocket& aSocket )
	{
	iAcceptedSocket = aSocket;
	iAcceptedSocketHandler.Attach ( aSocket );
	
	iAcceptedSocketHandler.Recv(); // Start receiving the data
	}

void CApplicationProtocolIntfTcp::RecvComplete ( RMBufChain& aData )
	{
	// Post the data to the link. We should do this one by one. But for the timebeing we
	// are keeping it simple. ie; one link is attached with us.
	__ASSERT_DEBUG ( iLinks.Count() > 0, User::Invariant() );
	
	if ( iLinks[0].Flags() != TClientType::ELeaving )
		{
		iReceivedData.Append ( aData );
		
		TAppProtIntfMessage::TNewConnection msg ( iReceivedData );
		PostToLink ( iLinks[0], msg );
		}
	else
		{
		iAcceptedSocket.Close ();
		aData.Free ();
		}
	}

void CApplicationProtocolIntfTcp::SendComplete ( TInt /* aLength */ )
	{
	// we will send data only for the accepted socket. Send complete means, we don't have anyone to accept
	// our connection and typically a error response ( protocol dependent ) will be send to the client and 
	// we will close the connection.
	// Send completed. Close the accepted socket and move into an accepting mode
	iAcceptedSocket.Close ();
	DoAccept ();
	}

void CApplicationProtocolIntfTcp::Error ( TOperation aOperation, TInt aError )
	{
	// If we have a recv/send failure we should handle it here. otherwise allow the base class to handle the error
	if ( ( aOperation == SockHandler::ERecv ) || ( aOperation == SockHandler::EMBufSend ) )
		{
		DoAccept ();
		return;
		}
	CApplicationProtocolIntfBase::Error ( aOperation, aError );
	}
