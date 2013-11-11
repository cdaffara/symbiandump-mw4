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

#include <ss_glob.h>
#include <ss_thread.h>

#include "upnpflowfactory.h"
#include "httpclientflow.h"
#include "httpserverflow.h"
#include "httpudpflow.h"
#include "httpmudpflow.h"

using namespace ESock;
using namespace Messages;

CUPnPFlowFactory* CUPnPFlowFactory::NewL(TAny* aConstructionParameters)
	{
	CUPnPFlowFactory* factory = new (ELeave) CUPnPFlowFactory(TUid::Uid(CUPnPFlowFactory::iUid), *(reinterpret_cast<CSubConnectionFlowFactoryContainer*>(aConstructionParameters)));
	CleanupStack::PushL(factory);
	factory->ConstructL();
	CleanupStack::Pop();
	return factory;
	}

CUPnPFlowFactory::CUPnPFlowFactory(TUid aFactoryId, CSubConnectionFlowFactoryContainer& aParentContainer)
	: CSubConnectionFlowFactoryBase(aFactoryId, aParentContainer)
	{
//	LOG_NODE_CREATE(KESockDataFactTag, CUPnPFlowFactory)
	}

void CUPnPFlowFactory::ConstructL()
	{
	// Register with comms transport to be able to receive our
	// custom Upnp messages	
	TUpnpMessage::RegisterL(SockManGlobals::Get ()->SelfWorker ());	
	}

CProtocolIntfFactoryBase* CUPnPFlowFactory::CreateProtocolIntfFactoryL(CProtocolIntfFactoryContainer& aParentContainer)
    {
   	ESock::CProtocolIntfFactoryBase* factory = CUPnPProtocolIntfFactory::NewL(Uid(), aParentContainer);
    return factory;
    }

CUPnPFlowFactory::~CUPnPFlowFactory()
	{
//	LOG_NODE_DESTROY(KESockDataFactTag, CUPnPFlowFactory)
	TUpnpMessage::DeRegister();	
 	}
 	 	
CSubConnectionFlowBase* CUPnPFlowFactory::DoCreateFlowL(CProtocolIntfBase* aProtocolIntf, TFactoryQueryBase& aQuery)
	{
	CSubConnectionFlowBase * flow = NULL;
	const TUPnPFlowQuery& queryBase = static_cast<const TUPnPFlowQuery&>( aQuery );
	CChunkManager* chunkMgr = reinterpret_cast<CChunkManager*>(queryBase.iChunkManager);

	if(chunkMgr != NULL)
		{
		static_cast<CUPnPProtocolIntfBase *>(aProtocolIntf)->SetChunkManager(chunkMgr);
		}
	
	switch( (static_cast < const TUPnPFlowQuery& >( aQuery )).iDataClientType)
		{
		case  EHttpClientFlow:
			{
			const THttpClientFlowQuery& clientData = static_cast<const THttpClientFlowQuery&>( aQuery );
			flow = CHttpClientFlow::NewL(*this, aProtocolIntf, queryBase.iSCPRId, clientData.iSockAddr);
			}
			break;

		case EHttpServerFlow:
			{
			const THttpServerFlowQuery& serverData = static_cast<const THttpServerFlowQuery&>( aQuery );
			TPtr8 reqUri ( const_cast<TUint8*>( serverData.iUri ), serverData.iLength, serverData.iLength );
			flow = CHttpServerFlow::NewL(*this, aProtocolIntf, reqUri, queryBase.iSCPRId );
			}
			break;

		case EHttpUdpFlow:
			{
			flow = CHttpUdpFlow::NewL( *this, static_cast<CUPnPProtocolIntfBase *>(aProtocolIntf), queryBase.iSCPRId );
			}
			break;

		case EHttpMUdpFlow:
			{
			flow = CHttpMUdpFlow::NewL( *this, static_cast<CUPnPProtocolIntfBase *>(aProtocolIntf), queryBase.iSCPRId );
			}
			break;

		default:
			ASSERT (0);
		}
	return flow;
	}

ACommsFactoryNodeId* CUPnPFlowFactory::DoFindOrCreateObjectL( TFactoryQueryBase& aQuery )
	{
	//Find the flow first
	const TUPnPFlowQuery& query = static_cast<const TUPnPFlowQuery&> ( aQuery );
	ACommsFactoryNodeId* flow = NULL;

	if ( query.iDataClientType == EHttpClientFlow )
		{
		const THttpClientFlowQuery& clientData = static_cast<const THttpClientFlowQuery&> ( query );

		if ( clientData.iClientFlags == THttpClientFlowQuery::ECreateNew )
			{ // For UPnP POST request, control plane always creates new flow for attaching it to CSocket.
			flow = CreateL ( aQuery );
			}
		else if ( clientData.iClientFlags == THttpClientFlowQuery::EUseExisting ) // HOST should match
			{
			flow = Find ( aQuery );
			if ( flow == NULL )
				flow = CreateL ( aQuery );			
			}
		static_cast<CUPnPFlowBase*>(flow)->AddControlClientL ( query.iSCPRId );
		}
	else if ( query.iDataClientType == EHttpServerFlow )
		{
		flow = Find ( aQuery );
		if ( flow != NULL )
			User::Leave ( KErrAlreadyExists );
		else
			flow = CreateL ( aQuery );
		}
	else
		{
		//Could be either udp flow or mudp flow.
		flow = Find ( aQuery );
		if ( flow == NULL )
			flow = CreateL ( aQuery );
		static_cast<CUPnPFlowBase*>(flow)->AddControlClientL ( query.iSCPRId );
		}
	return flow;
	}

CSubConnectionFlowBase* CUPnPFlowFactory::CreateL ( TFactoryQueryBase& aQuery )
	{
	TDefaultProtocolIntfFactoryQuery pintfQuery ( TNodeId::NullId () ); /*flowQuery.iCprId*/
	CProtocolIntfBase* proto = SockManGlobals::Get ()->iProtocolIntfFactories->FindOrCreateProtocolIntfL ( Uid (), pintfQuery );
	
	if ( NULL == proto )
    	{
    	User::Leave ( KErrNotFound );
    	}
    CSubConnectionFlowBase* flow = DoCreateFlowL ( proto, aQuery );
	CleanupStack::PushL(flow);

	AddManagedObjectL(*flow);

	CleanupStack::Pop(flow);
	return flow;
	}
