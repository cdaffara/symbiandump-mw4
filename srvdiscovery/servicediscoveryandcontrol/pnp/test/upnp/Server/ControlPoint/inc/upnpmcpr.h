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
// @file
// @internalComponent
// 
//

#ifndef __UPNPMCPR_H
#define __UPNPMCPR_H

#include <comms-infras/coremcpr.h>
#include <rsockethandler.h>
#include <csocketopener.h>
#include <in_sock.h>
#include <cchunkmanager.h>

class CUPnPMetaConnectionProvider : public CCoreMetaConnectionProvider,
								public MSocketHandlerObserver
	{
public:
	static CUPnPMetaConnectionProvider* NewL (
		ESock::CMetaConnectionProviderFactoryBase& aFactory, 
		const ESock::TProviderInfo& aProviderInfo );
	virtual ~CUPnPMetaConnectionProvider ();
		
	const TDesC8& DeviceLocationUrl ();	
	CChunkManager* ChunkManager();
private:
	CUPnPMetaConnectionProvider (
		ESock::CMetaConnectionProviderFactoryBase& aFactory, 
		const ESock::TProviderInfo& aProviderInfo, 
		const MeshMachine::TNodeActivityMap& aActivityMap );
	void ConstructL ();

private:
    virtual void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );    
	void FindLocalHost ();
	TInt GenerateDeviceLocation ( const TInetAddr& aAddr );
	
	//From MSocketHandlerObserver
	void OpenComplete ( RInternalSocket& aSocket );
	inline void ConnectComplete ();
	inline void AcceptComplete ( RInternalSocket& aSocket );
	inline void SendComplete ( TInt aLength );
	inline void SendToComplete ( TInt aLength );
	inline void RecvComplete ( RMBufChain& aData );
	inline void RecvFromComplete ( RMBufChain& aData, const TSockAddr& aAddr );
	void Error ( TOperation aOperation, TInt aError  );
	
private:
	CSocketOpener*	iSocketOpener;
	RInternalSocket iSocket;
	RBuf8			iDeviceLocation;
	CChunkManager*  iChunkManager;
	};


void CUPnPMetaConnectionProvider::ConnectComplete ()
	{ }
	
void CUPnPMetaConnectionProvider::AcceptComplete ( RInternalSocket& /*aSocket*/ )
	{ }

void CUPnPMetaConnectionProvider::SendComplete ( TInt /*aLength*/ )
	{ }
	
void CUPnPMetaConnectionProvider::SendToComplete ( TInt /*aLength*/ )
	{ }

void CUPnPMetaConnectionProvider::RecvComplete ( RMBufChain& /*aData*/ )
	{ }
	
void CUPnPMetaConnectionProvider::RecvFromComplete ( RMBufChain& /*aData*/, const TSockAddr& /*aAddr*/ )
	{ }
#endif // UPNPMCPR_H
