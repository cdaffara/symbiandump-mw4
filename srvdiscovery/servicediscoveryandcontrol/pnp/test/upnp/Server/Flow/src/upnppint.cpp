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
// System Includes
// 
//

#include<ss_glob.h>
#include <ecom/ecom.h>

//Local Includes
#include "upnppint.h"
#include "upnpflowbase.h"
#include "upnpflowfactory.h"
#include "httpserver.h"
#include "app_protintf_msgs.h"

using namespace ESock;
using namespace Messages;

CUPnPProtocolIntfBase* CUPnPProtocolIntfBase::NewL(CProtocolIntfFactoryBase& aFactory,const TNodeId& aId)
	{
	CUPnPProtocolIntfBase* protProv = new(ELeave) CUPnPProtocolIntfBase(aFactory, aId);
	CleanupStack::PushL(protProv);
	protProv->ConstructL();
	CleanupStack::Pop(protProv);
	return protProv;
	}

CUPnPProtocolIntfBase::CUPnPProtocolIntfBase ( CProtocolIntfFactoryBase& aFactory,const TNodeId& aId )
										: CProtocolIntfBase ( aFactory, aId )							
	{	}

void CUPnPProtocolIntfBase::ConstructL()
	{
	//The genreric listener. Should be moved to seperate library once EC120 is done.
    iAppProtIntfFactoryContainer = CAppProtIntfFactoryContainer::NewL();
    TAppProtIntfMessage::RegisterL();
	}

CUPnPProtocolIntfBase::~CUPnPProtocolIntfBase ()
	{
	delete iAppProtIntfFactoryContainer;
	TAppProtIntfMessage::DeRegister();
	}	

void CUPnPProtocolIntfBase::AddServiceUriL ( const TDesC8& aUri, MHttpEventObserver& aObserver,  TNodeCtxId aServiceId, TNodeCtxId aControlProviderId )
	{
	if ( !iHttpServer )
		iHttpServer = CHttpServer::NewL (this);
	
	iHttpServer->AddServiceUriL( aUri, aObserver, aServiceId, aControlProviderId );
	}

void CUPnPProtocolIntfBase::RemoveServiceUri ( const TDesC8& aUri,  TNodeCtxId aServiceId, TNodeCtxId aControlProviderId )
	{
	if ( iHttpServer->RemoveServiceUri( aUri, aServiceId, aControlProviderId ) )
		{
		iHttpServer = NULL; // httpserver makes self deletion, once listener leave completes
		}
	}

void CUPnPProtocolIntfBase::SetChunkManager(CChunkManager* aChunkManager)
	{
	iMemChunkManager = aChunkManager;
	}

CChunkManager* CUPnPProtocolIntfBase::GetMemoryChunkManager()
	{
	return iMemChunkManager;
	}

const TNodeId& CUPnPProtocolIntfBase::GetAppProtIntfContainerId()
	{
	return iAppProtIntfFactoryContainer->Id();
	}


