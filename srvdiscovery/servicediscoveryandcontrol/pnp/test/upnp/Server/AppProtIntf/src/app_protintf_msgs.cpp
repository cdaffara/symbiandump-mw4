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

#include <comms-infras/metatype.h>
#include <elements/vc.h>

#include "app_protintf_msgs.h"

using namespace Messages;


// Application Protocol Signatures
enum 
	{
	ESigSockAddr					=0,
	ESigSockAddrMBufChain			=1,
	ESigMBufChain					=2, 
	ESigIntSockMBufChain			=3,
	EAppProtIntfQuery				=4,
	};
	
EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TSigSockAddr, KAppProtIntfMessagesImplementationUid, ESigSockAddr)
	REGISTER_ATTRIBUTE(TSigSockAddr, iAddr, TMeta<TAppProtAddr>)
END_ATTRIBUTE_TABLE_BASE(TSignatureBase, 0)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TSigSockAddrMBufChain, KAppProtIntfMessagesImplementationUid, ESigSockAddrMBufChain)
	REGISTER_ATTRIBUTE(TSigSockAddrMBufChain, iAddr, TMeta<TAppProtAddr>)	
	REGISTER_ATTRIBUTE(TSigSockAddrMBufChain, iData, TMeta<RMBufChain>)
END_ATTRIBUTE_TABLE_BASE(TSignatureBase, 0)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TSigMBufChain, KAppProtIntfMessagesImplementationUid, ESigMBufChain)
	REGISTER_ATTRIBUTE(TSigMBufChain, iData, TMeta<RMBufChain>)
END_ATTRIBUTE_TABLE_BASE(TSignatureBase, 0)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TSigIntSockMBufChain, KAppProtIntfMessagesImplementationUid, ESigIntSockMBufChain)
	REGISTER_ATTRIBUTE(TSigIntSockMBufChain, iSocket, TMeta<RInternalSocket>)	
	REGISTER_ATTRIBUTE(TSigIntSockMBufChain, iData, TMeta<RMBufChain>)	
END_ATTRIBUTE_TABLE_BASE(TSignatureBase, 0)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TAppProtIntfQuery, KAppProtIntfMessagesImplementationUid, EAppProtIntfQuery)
	REGISTER_ATTRIBUTE (TAppProtIntfQuery, iProtocol, TMetaNumber)
	REGISTER_ATTRIBUTE(TAppProtIntfQuery, iPort, TMetaNumber)
END_ATTRIBUTE_TABLE()


DEFINE_MVIP_CTR(TSigSockAddr);
DEFINE_MVIP_CTR(TSigSockAddrMBufChain);
DEFINE_MVIP_CTR(TSigMBufChain);
DEFINE_MVIP_CTR(TSigIntSockMBufChain);
DEFINE_MVIP_CTR(TAppProtIntfQuery);

const TImplementationProxy SignatureImplementationTable[] = 
	{
	MVIP_CTR_ENTRY(ESigSockAddr, TSigSockAddr),
	MVIP_CTR_ENTRY(ESigSockAddrMBufChain, TSigSockAddrMBufChain),
	MVIP_CTR_ENTRY(ESigMBufChain, TSigMBufChain),
	MVIP_CTR_ENTRY(ESigIntSockMBufChain, TSigIntSockMBufChain),
	MVIP_CTR_ENTRY(EAppProtIntfQuery, TAppProtIntfQuery),
	};

EXPORT_C void TAppProtIntfMessage::RegisterL()
	{
	TlsGlobals::Get().RegisterInterfaceL(TUid::Uid(KAppProtIntfMessagesImplementationUid), sizeof(SignatureImplementationTable) / sizeof(SignatureImplementationTable[0]), SignatureImplementationTable);
	}

EXPORT_C void TAppProtIntfMessage::DeRegister()
	{
	TlsGlobals::Get().DeregisterInterface(TUid::Uid(KAppProtIntfMessagesImplementationUid));
	}
