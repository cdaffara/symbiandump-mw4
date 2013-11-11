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

#include "upnptiermanager.h"
#include "upnptiermanageractivities.h"
#include "upnptiermanagerselector.h"

#if defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
#define KUPnPTmTag KESockTierTag
#endif

using namespace ESock;
using namespace Messages;
using namespace MeshMachine;

CUPnPTierManager* CUPnPTierManager::NewL(ESock::CTierManagerFactoryBase& aFactory)
	{
	return new (ELeave) CUPnPTierManager(aFactory, UPnPTierManagerActivities::activityMap::Self());
	}
	
CUPnPTierManager::~CUPnPTierManager()
	{
	LOG_NODE_DESTROY(KUPnPTmTag, CUPnPTierManager);	
	}


CUPnPTierManager::CUPnPTierManager(CTierManagerFactoryBase& aFactory, const TNodeActivityMap& aActivityMap)
	: CCoreTierManager(aFactory, aActivityMap)
	{
	LOG_NODE_CREATE(KUPnPTmTag, CUPnPTierManager);    
	}


MProviderSelector* CUPnPTierManager::DoCreateProviderSelectorL(const Meta::SMetaData& aSelectionPreferences)
	{
	return TUPnPProviderSelectorFactory::NewSelectorL(aSelectionPreferences);		
	}

void CUPnPTierManager::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
	{
   	TNodeContext<CUPnPTierManager> ctx (*this, aMessage, aSender, aRecipient);
   	CCoreTierManager::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}
