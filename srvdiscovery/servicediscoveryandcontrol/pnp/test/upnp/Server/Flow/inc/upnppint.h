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

#ifndef __UPNPPINT_H_
#define __UPNPPINT_H_

#include <comms-infras/ss_protflow.h> 
#include <comms-infras/ss_nodemessages.h> 
#include <cchunkmanager.h>
#include "cprotocolhandler.h"
#include "genericcontainer.h"

class CUPnPProtocolIntfBase;
class CUPnPProtocolIntfFactory : public ESock::CProtocolIntfFactoryBase
/**
UPnP Protocol-Interface Factory
*/
	{		
public:	
	static CUPnPProtocolIntfFactory* CUPnPProtocolIntfFactory::NewL(TUid aFactoryId, ESock::CProtocolIntfFactoryContainer& aParentContainer);					
protected:
	CUPnPProtocolIntfFactory(TUid /*aFactoryId*/, ESock::CProtocolIntfFactoryContainer& /*aParentContainer*/);
	
	//from CProtocolIntfFactoryBase
	virtual ESock::CProtocolIntfBase* DoCreateProtocolIntfL(ESock::TFactoryQueryBase& aQuery);
	};


// ==========================================================================================
/** Base Class for implementing UPnP Protocol Interfaces.
*/

class CHttpServer;
class CUPnPProtocolIntfBase : public ESock::CProtocolIntfBase
	{	
public:
	static CUPnPProtocolIntfBase * NewL(ESock::CProtocolIntfFactoryBase& aFactory, const Messages::TNodeId& aId);
	virtual ~CUPnPProtocolIntfBase();	
	void AddServiceUriL ( const TDesC8& aUri, MHttpEventObserver& aObserver,  Messages::TNodeCtxId aServiceId, Messages::TNodeCtxId aControlProviderId );
	void RemoveServiceUri ( const TDesC8& aUri,  Messages::TNodeCtxId aServiceId, Messages::TNodeCtxId aControlProviderId );
	const Messages::TNodeId& GetAppProtIntfContainerId();
	void SetChunkManager(CChunkManager* aChunkManager);
	CChunkManager* GetMemoryChunkManager();
protected:
	// from CProtocolIntfBase
	CUPnPProtocolIntfBase(ESock::CProtocolIntfFactoryBase& aFactory, const Messages::TNodeId& aId);
	void ConstructL();
	inline void DoFlowCreated(ESock::CSubConnectionFlowBase& aFlow);
	inline void DoFlowBeingDeleted(ESock::CSubConnectionFlowBase& aFlow);
		
private:
	CHttpServer*			iHttpServer;
	CChunkManager*  		iMemChunkManager;
	CAppProtIntfFactoryContainer* iAppProtIntfFactoryContainer;
	};

void CUPnPProtocolIntfBase::DoFlowCreated ( ESock::CSubConnectionFlowBase& /*aFlow*/ )
	{
	}

void CUPnPProtocolIntfBase::DoFlowBeingDeleted ( ESock::CSubConnectionFlowBase& /*aFlow*/ )
	{	
	}


#endif //UPNPPINT_H
