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

#ifndef __APP_PROTINTF_UDP_H__
#define __APP_PROTINTF_UDP_H__

#include "app_protintf_base.h"
#include <appprotintf/app_protintf_msgs.h>

class CApplicationProtocolIntfUdp : public CApplicationProtocolIntfBase
	{
	public:
	static CApplicationProtocolIntfBase* NewL ( TInt aPort );
	~CApplicationProtocolIntfUdp();
	inline virtual TUint Protocol () const;

	protected:
	virtual void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );	

	private:
	CApplicationProtocolIntfUdp ( TInt aPort );
	
	virtual TInt Startup ();
	virtual void Shutdown ();
		
	virtual void RecvFromComplete ( RMBufChain& aData, const TSockAddr& aAddr );

	private:	
	TAppProtAddr	iMulticastAddr;	
	};
	
TUint CApplicationProtocolIntfUdp::Protocol () const
	{
	return KProtocolInetUdp;
	}
	
#endif // __APP_PROTINTF_UDP_H__
