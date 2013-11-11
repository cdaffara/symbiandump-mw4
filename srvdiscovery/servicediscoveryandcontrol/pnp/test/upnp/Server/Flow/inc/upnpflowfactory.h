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

#ifndef __UPNPFLOWFACTORY_H_
#define __UPNPFLOWFACTORY_H_


#include <comms-infras/ss_subconnflow.h>
#include "upnppint.h"

using namespace ESock;


class CUPnPFlowFactory : public CSubConnectionFlowFactoryBase
/**
Factory for creating CUPnP(TCP/UDP)Flow objects.
*/
	{
public:
	// ECOM interface implementation ID
	static const TInt iUid = 0x2000D05A;
	   	
   	static CUPnPFlowFactory* NewL(TAny* aConstructionParameters);
	~CUPnPFlowFactory();
	
private:
	CUPnPFlowFactory(TUid aFactoryId,CSubConnectionFlowFactoryContainer& aParentContainer);
	void ConstructL();
	
	// from CSubConnectionFlowFactoryBase
	CProtocolIntfFactoryBase* CreateProtocolIntfFactoryL(CProtocolIntfFactoryContainer& aParentContainer);
	virtual ACommsFactoryNodeId* DoFindOrCreateObjectL(TFactoryQueryBase& aQuery);
	virtual CSubConnectionFlowBase* DoCreateFlowL(CProtocolIntfBase* aProtocolIntf, TFactoryQueryBase& aQuery);
	
	CSubConnectionFlowBase* CreateL ( TFactoryQueryBase& aQuery );
	};

#endif //UPNPFLOWFACTORY_H

