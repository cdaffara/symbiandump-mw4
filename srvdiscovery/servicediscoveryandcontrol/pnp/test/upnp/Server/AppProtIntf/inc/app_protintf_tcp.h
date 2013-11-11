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

#ifndef __APP_PROTINTF_TCP_H__
#define __APP_PROTINTF_TCP_H__

#include <in_sock.h>
#include <es_mbuf.h>
#include "app_protintf_base.h"

class RMBufChain;

class CApplicationProtocolIntfTcp : public CApplicationProtocolIntfBase
	{
	public:
	static CApplicationProtocolIntfBase* NewL ( TInt aPort );
	inline virtual TUint Protocol () const;
	~CApplicationProtocolIntfTcp ();
	
	protected:
	virtual void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	
	private:
	CApplicationProtocolIntfTcp ( TInt aPort );
	virtual TInt Startup ();
	virtual void Shutdown ();
	
	void DoAccept ();	

	virtual void AcceptComplete ( RInternalSocket& aSocket );
	virtual void RecvComplete ( RMBufChain& aData );
	virtual void SendComplete ( TInt aLength );
	virtual void Error ( TOperation aOperation, TInt aError );
	private:
	
	RInternalSocket iAcceptedSocket;
	RSocketHandler	iAcceptedSocketHandler;	
	RMBufChain 		iReceivedData;
	};
	
TUint CApplicationProtocolIntfTcp::Protocol () const
	{
	return KProtocolInetTcp;
	}
	
#endif // __APP_PROTINTF_TCP_H__
