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

#ifndef __APP_PROTINF_BASE_H__
#define __APP_PROTINF_BASE_H__

#include <comms-infras/ss_subconnflow.h>
#include <rsockethandler.h>
#include <csocketopener.h>
#include <appprotintf/genericcontainer.h>

_LIT ( KAppProtIntfInvalidRealmId, " RealmId is invalid " );

enum TAppProtIntfPanic
	{
	KInvalidReamId
	};
	
// Forward declarations
class CApplicationProtocolIntfFactory;

class CApplicationProtocolIntfBase : public CFactoryObject,
									public MSocketHandlerObserver
	{
public:

	inline TUint Port () const;
	virtual TUint Protocol () const =0;
	virtual ~CApplicationProtocolIntfBase ();

protected:

	enum
		{
		ERecvFrom = 1
		};

	typedef Messages::TMessageSigVoid < ERecvFrom, ESock::TCFMessage::ERealmId > TRecvFrom;

	CApplicationProtocolIntfBase ( TUint aPort, TInt aProtocolType );

	void JoinCompleted ();
	void PostToLink ( const Messages::TNodeCtxId& aRecipient, const Messages::TSignatureBase& aMessage );
	void PostToLink ( const Messages::RNodeInterface& aRecipient, const Messages::TSignatureBase& aMessage );
	void PostToAllLinks ( const Messages::TSignatureBase& aMessage );

	void InitiateLinkL ( const Messages::TNodeCtxId& aSender, TUint aSockType, TUint aProtocol );
	void HandleClientLeavingRequest ( const Messages::TNodeCtxId&  aSender );
	void CloseLink ( const Messages::TNodeId& aSender );

	TInt OpenSocket ( TUint aAddrFamily, TUint aSockType, TUint aProtocol );

	virtual TInt Startup () =0;
	virtual void Shutdown ();

	// From ANodeIdBase
	virtual void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	// From MSocketHandlerObserver
	virtual void OpenComplete ( RInternalSocket& aSocket );
	inline virtual void ConnectComplete ();
	inline virtual void AcceptComplete ( RInternalSocket& aSocket );
	inline virtual void SendComplete ( TInt aLength );
	inline virtual void SendToComplete ( TInt aLength );
	inline virtual void RecvComplete ( RMBufChain& aData );
	inline virtual void RecvFromComplete ( RMBufChain& aData, const TSockAddr& aAddr );
//	virtual void IoctlComplete ();
	virtual void Error ( TOperation aOperation, TInt aError  );

protected:
	TInt AddLink ( const Messages::TNodeId& aSender );
	TInt RemoveLink ( const Messages::TNodeId& aSender );
	TInt FindLink ( const Messages::TNodeId& aSender );

protected:
	RArray<Messages::RNodeInterface>	iLinks;
	RSocketHandler		iSocketHandler;
	RInternalSocket 	iSocket;
private:
	TUint 				iPort;
	CSocketOpener*		iSocketOpener;
	};


TUint CApplicationProtocolIntfBase::Port () const
	{
	return iPort;
	}

void CApplicationProtocolIntfBase::ConnectComplete ()
	{

	}

void CApplicationProtocolIntfBase::AcceptComplete ( RInternalSocket& /* aSocket */ )
	{
	}

/*void CApplicationProtocolIntfBase::IoctlComplete ()
	{
	}
*/
void CApplicationProtocolIntfBase::RecvComplete ( RMBufChain& /* aData */ )
	{

	}

void CApplicationProtocolIntfBase::RecvFromComplete ( RMBufChain& /* aData */, const TSockAddr& /* aAddr */ )
	{

	}

void CApplicationProtocolIntfBase::SendComplete ( TInt /* aLength */ )
	{

	}

void CApplicationProtocolIntfBase::SendToComplete ( TInt /* aLength */ )
	{

	}

#endif // __APP_PROTINF_BASE_H__
