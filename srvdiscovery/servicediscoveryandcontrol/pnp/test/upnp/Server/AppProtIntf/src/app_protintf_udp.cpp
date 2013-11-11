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

#include "app_protintf_udp.h"
#include <in_sock.h>

using namespace Messages;


CApplicationProtocolIntfBase* CApplicationProtocolIntfUdp::NewL ( TInt aPort )
	{
	CApplicationProtocolIntfUdp* self = new (ELeave)CApplicationProtocolIntfUdp( aPort );
	return self;			
	}

CApplicationProtocolIntfUdp::CApplicationProtocolIntfUdp ( TInt aPort )
: CApplicationProtocolIntfBase ( aPort, KProtocolInetUdp )
	{
	LOG_NODE_CREATE ( KESockFlowTag, CApplicationProtocolIntfUdp );	
	}
CApplicationProtocolIntfUdp::~CApplicationProtocolIntfUdp()
	{
	Shutdown();
	LOG_NODE_DESTROY(KESockFlowTag, CApplicationProtocolIntfUdp);
	}
	
void CApplicationProtocolIntfUdp::ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage )
	{
	if ( aMessage.MessageId().Realm() == TAppProtIntfMessage::ERealmId )
		{
		CApplicationProtocolIntfBase::ReceivedL ( aSender, aRecipient, aMessage );
		switch ( aMessage.MessageId().MessageId() )
			{
			case TAppProtIntfMessage::TJoinWithMulticastAddress::EId:
				{
				const TAppProtIntfMessage::TJoinWithMulticastAddress& msg = message_cast<const TAppProtIntfMessage::TJoinWithMulticastAddress> ( aMessage );
				iMulticastAddr = msg.iAddr;				
				InitiateLinkL ( address_cast<TNodeCtxId> ( aSender ), KSockDatagram, KProtocolInetUdp );
				}
				break;
			}
		}
	
	// The below message are self posted. Just to keep the operation to continue.
	if ( aMessage.MessageId().Realm() == ESock::TCFMessage::ERealmId )
		{
		switch ( aMessage.MessageId ().MessageId () )
			{
			case CApplicationProtocolIntfBase::TRecvFrom::EId:
				{
				__ASSERT_DEBUG ( iLinks.Count() > 0, User::Invariant() );
				if ( iLinks[0].Flags () != TClientType::ELeaving )
					{
					iSocketHandler.RecvFrom ();
					}
				}
			break;
			
			}
		}
	}

TInt CApplicationProtocolIntfUdp::Startup ()
	{
	TInt ret = KErrNone;	
	TInetAddr addr;
  	addr.SetAddress ( iMulticastAddr.iAddr );
  	addr.SetPort ( iMulticastAddr.iPort );

	if ( addr.IsMulticast() )
		{
		if ( addr.Family() != KAfInet6 )
			{
			addr.ConvertToV4Mapped();				
			}
		TPckgBuf<TIp6Mreq> mReqBuf;
		mReqBuf().iAddr = addr.Ip6Address();
		mReqBuf().iInterface = 0;
		
		ret = iSocket.SetOpt( KSoIp6JoinGroup, KSolInetIp, mReqBuf ); // Join the multicast group
		}
	iSocketHandler.RecvFrom(); // Start the receive
	return ret;	
	}
	
void CApplicationProtocolIntfUdp::Shutdown ()
	{
	TInetAddr addr;
  	addr.SetAddress ( iMulticastAddr.iAddr );
  	addr.SetPort ( iMulticastAddr.iPort );

	if ( addr.IsMulticast() )
		{
		if ( addr.Family() != KAfInet6 )
			{
			addr.ConvertToV4Mapped();				
			}
		TPckgBuf<TIp6Mreq> mReqBuf;
		mReqBuf().iAddr = addr.Ip6Address();
		mReqBuf().iInterface = 0;
		
		TInt ret = iSocket.SetOpt( KSoIp6LeaveGroup, KSolInetIp, mReqBuf ); // leave the multicast group
		ASSERT ( ret == KErrNone );
		}
		
	CApplicationProtocolIntfBase::Shutdown ();	
	}
	
void CApplicationProtocolIntfUdp::RecvFromComplete ( RMBufChain& aData, const TSockAddr& aAddr )
	{
	__ASSERT_DEBUG ( iLinks.Count() > 0, User::Invariant() );
	
	// if client is in leaving state, no need of any posts
	if ( iLinks[0].Flags() != TClientType::ELeaving )
		{
		TInetAddr inetAddr ( aAddr );
  		TAppProtAddr protAddr ( inetAddr.Address(), inetAddr.Port() );
		
		TAppProtIntfMessage::TDataReceived msg ( protAddr, aData );
		
		PostToLink ( iLinks[0], msg ); // We do assume that we have only one link ( atleast for the timebeing ).
		// Note: If we have more link then we need to make copies of RMBufChain before posting. Otherwise the ownership
		// of the RMBufChain will be an issue?			
		TNodeCtxId ctx ( MeshMachine::KActivityNull, NodeId () );
		PostToLink ( ctx, CApplicationProtocolIntfBase::TRecvFrom ().CRef () ); // Self post to keep continue with the receiving
		}
	else
		{
		aData.Free ();
		}
	}
