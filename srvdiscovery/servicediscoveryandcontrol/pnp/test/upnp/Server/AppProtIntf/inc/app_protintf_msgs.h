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

#ifndef __APP_PROTINTFMESSAGE_H__
#define __APP_PROTINTFMESSAGE_H__

#include <e32base.h>
#include <comms-infras/ss_nodemessages.h>
#include <elements/factory.h>
#include <comms-infras/eintsock.h>
#include <es_mbuf.h>
#include <in_sock.h>
//#include <appprotintf/app_protintf_base.h>

using namespace Messages;

const TInt KAppProtIntfMessagesImplementationUid = 0x2001DC6B;
class TAppProtAddr
   	{
public:
   	TAppProtAddr ( )
   		{
   		}
   	TAppProtAddr ( TUint32 aAddr,	TUint aPort )
   		:iAddr ( aAddr ), iPort ( aPort  )
   		{
  		}
 	TBool operator== ( const TAppProtAddr& aAppProtAddr ) const
 		{
 		return iAddr == aAppProtAddr.iAddr && iPort == aAppProtAddr.iPort ? ETrue : EFalse;
 		}

   	TUint32 iAddr;
   	TUint 	iPort;
   	};


// Application Protocol Interface custom messages
DECLARE_MESSAGE_SIG_1(SigSockAddr, TAppProtAddr, Addr)
DECLARE_MESSAGE_SIG_2(SigSockAddrMBufChain, TAppProtAddr, Addr, RMBufChain, Data )
DECLARE_MESSAGE_SIG_1(SigMBufChain, RMBufChain, Data)
DECLARE_MESSAGE_SIG_2(SigIntSockMBufChain, RInternalSocket, Socket, RMBufChain, Data)


struct TAppProtIntfQuery : public ESock::TFactoryQueryBase
	{
public:
	inline TAppProtIntfQuery() {}
	explicit TAppProtIntfQuery ( TUint aProtocol, TUint aPort ): iProtocol(aProtocol),iPort(aPort)
			{
			}

public:
	Factories::MFactoryQuery::TMatchResult Match(Factories::TFactoryObjectInfo& /*aObjectInfo*/ )
		{
//		CApplicationProtocolIntfBase* appProtIntfObj = static_cast< CApplicationProtocolIntfBase* >( aObjectInfo.iInfo.iFactoryObject );
//		return ( appProtIntfObj->Protocol() == iProtocol &&  appProtIntfObj->Port() == iPort ) ?
//				MCommsFactoryQuery::EMatch : MCommsFactoryQuery::EContinue;
	return (TMatchResult)0;
		}

public:
	TUint iProtocol;
	TUint iPort;

   DECLARE_MVIP_CTR( TAppProtIntfQuery )
   EXPORT_DATA_VTABLE_AND_FN
	};

NONSHARABLE_CLASS(TAppProtIntfMessage)
	{
	private:

	enum
		{
		EJoin =1,
		EJoinWithMulticastAddress =2,
		EClientLeavingRequest =3,
		ELeaveComplete =4,
		EJoinComplete =5,
		EDataReceived =6,
		ENewConnection =7,
		EAcceptConnection =8,
		ERejectConnection =9,
		EMoreData =10,
		ETransferConnection =11,
		EError =12,
		EFindOrCreateAppProtIntf =13,
		EAppProtIntfCreated =14,
		EDestroy = 15,
		};

	public:
	IMPORT_C static void RegisterL ();	// Register the messages with COMMS transport
	IMPORT_C static void DeRegister ();  // De-register the messages with the COMMS transport

	public:

	enum { ERealmId = 0x2000D055 };

	typedef TMessageSigVoid < EJoin, TAppProtIntfMessage::ERealmId > TJoin;
	typedef TMessageSigSockAddr < EJoinWithMulticastAddress, TAppProtIntfMessage::ERealmId > TJoinWithMulticastAddress;
	typedef TMessageSigVoid < EClientLeavingRequest, TAppProtIntfMessage::ERealmId > TClientLeavingRequest;
    typedef TMessageSigVoid < ELeaveComplete, TAppProtIntfMessage::ERealmId > TLeaveComplete;
	typedef TMessageSigVoid < EJoinComplete, TAppProtIntfMessage::ERealmId > TJoinComplete;
	typedef TMessageSigSockAddrMBufChain < EDataReceived, TAppProtIntfMessage::ERealmId > TDataReceived;
	typedef TMessageSigMBufChain < ENewConnection, TAppProtIntfMessage::ERealmId > TNewConnection;
	typedef TMessageSigVoid < EAcceptConnection, TAppProtIntfMessage::ERealmId > TAcceptConnection;
	typedef TMessageSigMBufChain < ERejectConnection, TAppProtIntfMessage::ERealmId > TRejectConnection;
	typedef TMessageSigVoid < EMoreData, TAppProtIntfMessage::ERealmId > TMoreData;
	typedef TMessageSigIntSockMBufChain < ETransferConnection, TAppProtIntfMessage::ERealmId > TTransferConnection;
	typedef TMessageSigNumber < EError, TAppProtIntfMessage::ERealmId > TError;
	typedef TMessageSigVoid < EDestroy, TAppProtIntfMessage::ERealmId > TDestroy;
	};

#endif // __APP_PROTINTFMESSAGE_H__
