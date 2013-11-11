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

#ifndef __UPNPTIERMANAGER_H
#define __UPNPTIERMANAGER_H

#include <comms-infras/coretiermanager.h>


class CUPnPTierManager : public CCoreTierManager
	{
public:		
	static CUPnPTierManager* NewL(ESock::CTierManagerFactoryBase& aFactory);
	virtual ~CUPnPTierManager();

protected:
	CUPnPTierManager(ESock::CTierManagerFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap);
	virtual ESock::MProviderSelector* DoCreateProviderSelectorL(const Meta::SMetaData& aSelectionPreferences);
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	};
	
#endif // UPNPTIERMANAGER_H