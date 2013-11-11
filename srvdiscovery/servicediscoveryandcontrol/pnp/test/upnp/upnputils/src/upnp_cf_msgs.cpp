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
// UPnP-proprietary messages
// 
//

/**
 @file
 @internalTechnology
*/
//System Includes 
#include <ss_glob.h>
#include <comms-infras/metatype.h>
#include <comms-infras/metabuffer.h>
#include <comms-infras/ss_thread.h>


//Local Includes
#include "upnp_cf_msgs.h"
#include "upnpflowbase.h"
#include "httpserverflow.h"
#include "httpclientflow.h"

using namespace Messages;
using namespace Factories;

// UPnP Message _Signatures_ (NOT messages)
enum EMessageTypeId 
	{        
    ESigParamSet			= 1,		
    EUPnPFlow				= 2,
    EClientFlow				= 3,
    EServerFlow				= 4,
    EUdpFlow				= 5,
    ESigNumberVersionMemChunk = 6,
    ESigNumberMemChunk		= 7,
    ESigParamSetFlow 		= 8,
    ESigUPnPRequestInfo		= 9,
    ESigUPnPResponseInfo	= 10,
    ESigUPnPSubscriptionInfo = 11,
    ESigRMemChunkFlow      = 12,
    ESigRMBufInt			= 13,
    ESigSsdpInfo			= 14,
    ESigParamSetAny			= 15,
    ESigTUint8Size       	= 16,
    ESigUPnPClientRequestInfo		= 17,
    ESigUPnPSubscribeReqInfo = 18,
    ESigUPnPEventNotifyReqInfo = 19,
	};


EXPORT_START_ATTRIBUTE_TABLE_AND_FN ( TSigParamSet, KUpnpMessagesImplementationUid, ESigParamSet )
	REGISTER_ATTRIBUTE ( TSigParamSet, iParamSet, TMeta<CSubConExtensionParameterSet*> )
END_ATTRIBUTE_TABLE_BASE ( TSignatureBase, ESignatureVoid )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN ( TUPnPFlowQuery, KUpnpMessagesImplementationUid, EUPnPFlow )
	REGISTER_ATTRIBUTE ( TUPnPFlowQuery, iSCPRId, TMeta<TNodeId> )
	REGISTER_ATTRIBUTE ( TUPnPFlowQuery, iDataClientType, TMeta<TInt> )
	REGISTER_ATTRIBUTE ( TUPnPFlowQuery, iChunkManager, TMeta<TAny*> )
END_ATTRIBUTE_TABLE ()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN ( THttpClientFlowQuery, KUpnpMessagesImplementationUid, EClientFlow )
	REGISTER_ATTRIBUTE ( THttpClientFlowQuery, iSockAddr, TMeta<TAppProtAddr> )
	REGISTER_ATTRIBUTE ( THttpClientFlowQuery, iClientFlags, TMeta<TInt> )
END_ATTRIBUTE_TABLE_BASE ( TUPnPFlowQuery, EUPnPFlow )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( THttpServerFlowQuery, KUpnpMessagesImplementationUid, EServerFlow )
	REGISTER_ATTRIBUTE ( THttpServerFlowQuery, iUri, TMeta<TUint8*> )
	REGISTER_ATTRIBUTE ( THttpServerFlowQuery, iLength, TMeta<TInt> )
END_ATTRIBUTE_TABLE_BASE(TUPnPFlowQuery, EUPnPFlow)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( THttpUdpFlowQuery, KUpnpMessagesImplementationUid, EUdpFlow )
END_ATTRIBUTE_TABLE_BASE(TUPnPFlowQuery, EUPnPFlow)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNumberVersionMemChunk, KUpnpMessagesImplementationUid, ESigNumberVersionMemChunk )
	REGISTER_ATTRIBUTE ( TSigNumberVersionMemChunk, iStatus, TMeta<TInt> )
	REGISTER_ATTRIBUTE ( TSigNumberVersionMemChunk, iVersion, TMeta<TVersion> )
	REGISTER_ATTRIBUTE ( TSigNumberVersionMemChunk, iBodyData, TMeta<RMemChunk> )
END_ATTRIBUTE_TABLE_BASE ( TSignatureBase, ESignatureVoid )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigNumberMemChunk, KUpnpMessagesImplementationUid, ESigNumberMemChunk )
	REGISTER_ATTRIBUTE ( TSigNumberMemChunk, iTimeOut, TMeta<TInt> )
	REGISTER_ATTRIBUTE ( TSigNumberMemChunk, iURN, TMeta<RMemChunk> )
END_ATTRIBUTE_TABLE_BASE ( TSigNumberVersionMemChunk, ESigNumberVersionMemChunk )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN ( TSigParamSetFlow, KUpnpMessagesImplementationUid, ESigParamSetFlow )
	REGISTER_ATTRIBUTE ( TSigParamSetFlow, iParamSet, TMeta<CSubConExtensionParameterSet*> )
	REGISTER_ATTRIBUTE ( TSigParamSetFlow, iFlowId, TMeta<TNodeId> )	
END_ATTRIBUTE_TABLE_BASE ( TSignatureBase, ESignatureVoid )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN ( TSigUPnPResponseInfo, KUpnpMessagesImplementationUid, ESigUPnPResponseInfo )
	REGISTER_ATTRIBUTE ( TSigUPnPResponseInfo, iTrans, TMeta<CTransaction*> )
	REGISTER_ATTRIBUTE ( TSigUPnPResponseInfo, iStatus, TMeta<TInt> )	
	REGISTER_ATTRIBUTE ( TSigUPnPResponseInfo, iInfo, TMetaPtr<SMetaDataNetCtorExt> )
END_ATTRIBUTE_TABLE_BASE ( TSignatureBase, ESignatureVoid )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN ( TSigRMemChunkFlow, KUpnpMessagesImplementationUid, ESigRMemChunkFlow )
	REGISTER_ATTRIBUTE ( TSigRMemChunkFlow, iMemChunk, TMeta<RMemChunk> )
	REGISTER_ATTRIBUTE ( TSigRMemChunkFlow, iFlowId, TMeta<TNodeId> )	
END_ATTRIBUTE_TABLE_BASE ( TSignatureBase, ESignatureVoid )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN ( TSubsribeResponseInfo, KUpnpMessagesImplementationUid, ESigUPnPSubscriptionInfo )
	REGISTER_ATTRIBUTE ( TSubsribeResponseInfo, iSid, TMeta<RMemChunk> )
	REGISTER_ATTRIBUTE ( TSubsribeResponseInfo, iTimeout, TMeta<TInt> )
END_ATTRIBUTE_TABLE ()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN ( TSigSsdpInfo, KUpnpMessagesImplementationUid, ESigSsdpInfo )
	REGISTER_ATTRIBUTE ( TSigSsdpInfo, iSsdpInfo, TMeta<TSsdpInfo> )
END_ATTRIBUTE_TABLE_BASE ( TSignatureBase, ESignatureVoid )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN ( TSigParamSetAny, KUpnpMessagesImplementationUid, ESigParamSetAny )
	REGISTER_ATTRIBUTE ( TSigParamSetAny, iParamSet, TMeta<CSubConExtensionParameterSet*> )
	REGISTER_ATTRIBUTE ( TSigParamSetAny, iPtr, TMeta<TAny*> )
END_ATTRIBUTE_TABLE_BASE ( TSignatureBase, ESignatureVoid )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN ( TSigTUint8Size, KUpnpMessagesImplementationUid, ESigTUint8Size )
	REGISTER_ATTRIBUTE ( TSigTUint8Size, iPtr, TMeta<TUint8*> )
	REGISTER_ATTRIBUTE ( TSigTUint8Size, iSize, TMeta<TInt> )
END_ATTRIBUTE_TABLE_BASE ( TSignatureBase, ESignatureVoid )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN ( TSigUPnPClientRequestInfo, KUpnpMessagesImplementationUid, ESigUPnPClientRequestInfo )
	REGISTER_ATTRIBUTE ( TSigUPnPClientRequestInfo, iRequestMethod, TMeta<TInt> )
	REGISTER_ATTRIBUTE ( TSigUPnPClientRequestInfo, iUri, TMeta<RMemChunk> )	
	REGISTER_ATTRIBUTE ( TSigUPnPClientRequestInfo, iBody, TMeta<RMemChunk> )
	REGISTER_ATTRIBUTE ( TSigUPnPClientRequestInfo, iInfo, TMetaPtr<SMetaDataNetCtorExt> )
END_ATTRIBUTE_TABLE_BASE ( TSignatureBase, ESignatureVoid )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN ( TSubscribeRequestInfo, KUpnpMessagesImplementationUid, ESigUPnPSubscribeReqInfo )
	REGISTER_ATTRIBUTE ( TSubscribeRequestInfo, iTimeOut, TMeta<TInt> )
	REGISTER_ATTRIBUTE ( TSubscribeRequestInfo, iCallback, TMeta<RMemChunk> )	
	REGISTER_ATTRIBUTE ( TSubscribeRequestInfo, iSid, TMeta<RMemChunk> )
END_ATTRIBUTE_TABLE_BASE ( TSignatureBase, ESignatureVoid )

EXPORT_START_ATTRIBUTE_TABLE_AND_FN ( TEventNotifyRequestInfo, KUpnpMessagesImplementationUid, ESigUPnPEventNotifyReqInfo )
	REGISTER_ATTRIBUTE ( TEventNotifyRequestInfo, iSeqKey, TMeta<TInt> )
	REGISTER_ATTRIBUTE ( TEventNotifyRequestInfo, iSid, TMeta<RMemChunk> )
END_ATTRIBUTE_TABLE_BASE ( TSignatureBase, ESignatureVoid )


DEFINE_MVIP_CTR ( TSigParamSet );
DEFINE_MVIP_CTR ( TUPnPFlowQuery);
DEFINE_MVIP_CTR ( THttpClientFlowQuery );
DEFINE_MVIP_CTR ( THttpServerFlowQuery );
DEFINE_MVIP_CTR ( THttpUdpFlowQuery );
DEFINE_MVIP_CTR ( TSigNumberVersionMemChunk );
DEFINE_MVIP_CTR ( TSigNumberMemChunk );
DEFINE_MVIP_CTR ( TSigParamSetFlow );
DEFINE_MVIP_CTR ( TSigUPnPResponseInfo );
DEFINE_MVIP_CTR ( TSubsribeResponseInfo );
DEFINE_MVIP_CTR ( TSigRMemChunkFlow );
DEFINE_MVIP_CTR ( TSigSsdpInfo );
DEFINE_MVIP_CTR ( TSigParamSetAny );
DEFINE_MVIP_CTR ( TSigTUint8Size );
DEFINE_MVIP_CTR ( TSigUPnPClientRequestInfo );
DEFINE_MVIP_CTR ( TSubscribeRequestInfo );
DEFINE_MVIP_CTR ( TEventNotifyRequestInfo );


const TImplementationProxy SignatureImplementationTable[] = 
	{
	//NOTE: Entries must be sorted for the binary search to work efficiently!
	MVIP_CTR_ENTRY ( ESigParamSet, TSigParamSet ),
	MVIP_CTR_ENTRY ( EUPnPFlow, TUPnPFlowQuery ),
	MVIP_CTR_ENTRY ( EClientFlow, THttpClientFlowQuery ),
	MVIP_CTR_ENTRY ( EServerFlow, THttpServerFlowQuery ),
	MVIP_CTR_ENTRY ( EUdpFlow, THttpUdpFlowQuery ),
	MVIP_CTR_ENTRY ( ESigNumberVersionMemChunk, TSigNumberVersionMemChunk ),
	MVIP_CTR_ENTRY ( ESigNumberMemChunk, TSigNumberMemChunk ),
	MVIP_CTR_ENTRY ( ESigParamSetFlow, TSigParamSetFlow ),
	MVIP_CTR_ENTRY ( ESigUPnPResponseInfo, TSigUPnPResponseInfo ),	
	MVIP_CTR_ENTRY ( ESigUPnPSubscriptionInfo, TSubsribeResponseInfo ),
	MVIP_CTR_ENTRY ( ESigRMemChunkFlow, TSigRMemChunkFlow ),
	MVIP_CTR_ENTRY ( ESigSsdpInfo, TSigSsdpInfo ),
	MVIP_CTR_ENTRY ( ESigParamSetAny, TSigParamSetAny ),
	MVIP_CTR_ENTRY ( ESigTUint8Size, TSigTUint8Size ),
	MVIP_CTR_ENTRY ( ESigUPnPClientRequestInfo, TSigUPnPClientRequestInfo ),
	MVIP_CTR_ENTRY ( ESigUPnPSubscribeReqInfo, TSubscribeRequestInfo ),
	MVIP_CTR_ENTRY ( ESigUPnPEventNotifyReqInfo, TEventNotifyRequestInfo ),
	};


/*
Note we need to register if a message needs to carries completely new set of parameters that is not supported by 
TCFMessage....for this refer TCFUmtsIfMessageSig in IPProtoMessages.h
*/
EXPORT_C void TUpnpMessage::RegisterL ( Den::CCommonWorkerThread *aWorkerThread )
	{
	ASSERT ( aWorkerThread );	
	aWorkerThread->TransportGlobals().RegisterInterfaceL ( TUid::Uid( KUpnpMessagesImplementationUid), sizeof(SignatureImplementationTable) / sizeof(SignatureImplementationTable[0] ), SignatureImplementationTable );	
	}

EXPORT_C void TUpnpMessage::DeRegister ( )
	{
	CWorkerThread* worker = SockManGlobals::Get()->SelfWorker();
	ASSERT ( worker );
	worker->TransportGlobals().DeregisterInterface( TUid::Uid ( KUpnpMessagesImplementationUid ) );
	}

EXPORT_C MFactoryQuery::TMatchResult TUPnPFlowQuery::Match( TFactoryObjectInfo& aObjectInfo )
	{
	CUPnPFlowBase* flow = static_cast<CUPnPFlowBase*>(aObjectInfo.iInfo.iFactoryObject);
	if( ( iDataClientType == EHttpClientFlow ) && ( flow->FlowType() == EHttpClientFlow ) )
		{
		return static_cast<THttpClientFlowQuery&>(*this).Compare(aObjectInfo);
		}
	if( ( iDataClientType == EHttpServerFlow ) && ( flow->FlowType() == EHttpServerFlow ) )
		{
		return static_cast<THttpServerFlowQuery&>(*this).Compare(aObjectInfo);
		}
	if( ( iDataClientType == EHttpUdpFlow ) && ( flow->FlowType() == EHttpUdpFlow ) )
		{
		return static_cast<THttpUdpFlowQuery&>(*this).Compare(aObjectInfo);
		}
	if( ( iDataClientType == EHttpMUdpFlow ) && ( flow->FlowType() == EHttpMUdpFlow ) )
		{
		return static_cast<THttpUdpFlowQuery&>(*this).Compare(aObjectInfo);
		}
	return MFactoryQuery::EContinue;
	}

MFactoryQuery::TMatchResult THttpServerFlowQuery::Compare( TFactoryObjectInfo& aFactoryObjectInfo )
	{
	CHttpServerFlow* flow = static_cast<CHttpServerFlow*>(aFactoryObjectInfo.iInfo.iFactoryObject);
	TPtrC8 uri(iUri);
	if(flow->Uri() == uri)
		{
		return MFactoryQuery::EMatch;
		}
	return MFactoryQuery::EContinue;
	}

MFactoryQuery::TMatchResult THttpUdpFlowQuery::Compare(TFactoryObjectInfo& /*aFactoryObjectInfo*/ )
	{
	return MFactoryQuery::EMatch;
	}

MFactoryQuery::TMatchResult THttpClientFlowQuery::Compare(TFactoryObjectInfo& aFactoryObjectInfo )
	{
	const CHttpClientFlow* flow = static_cast<const CHttpClientFlow*>(aFactoryObjectInfo.iInfo.iFactoryObject);
	
	// iClientFlags == ECreateNew is handled by factory, other needs to be checked here	
 	TInetAddr remName;
   	flow->RemName( remName );
  	if ( remName.Address ( ) == iSockAddr.iAddr && remName.Port ( ) ==  iSockAddr.iPort && flow->Status( ) == KErrNone )
		{
		return MFactoryQuery::EMatch;
		}
	return MFactoryQuery::EContinue;
	}

EXPORT_C SMetaDataNetCtor* SMetaDataNetCtorExt::LoadL(TPtrC8& aDes)
	{
	TUint8* ptr = const_cast<TUint8*> ( aDes.Ptr() );
	TPtr8 ptr2 ( ptr, aDes.Length() );
	RBuf8 buf;
	buf.CreateL(aDes);
	
	TPtrC8 ptr3 (buf.Ptr(), buf.Length());
	CMetaDataVirtualCtorInPlace* vctr = SockManGlobals::Get()->SelfWorker()->TransportGlobals().VirtualCtor();
	SMetaDataNetCtor* meta = vctr->New(ptr3, ptr2);
	TInt len = buf.Length() - ptr3.Length();
	aDes.Set(aDes.Ptr() + len, aDes.Length() - len);
	buf.Close();
	return meta;
	}

