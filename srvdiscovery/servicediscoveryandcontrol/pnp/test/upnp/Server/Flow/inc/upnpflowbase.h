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

#ifndef __UPNPFLOWBASE_H_
#define __UPNPFLOWBASE_H_

//System Includes
#include <comms-infras/ss_subconnflow.h>
#include <comms-infras/ss_flowbinders.h>
#include <comms-infras/ss_protflow.h>
#include <ss_fact.h>

using namespace ESock;
using namespace Messages;

enum TFlowType
	{
	EHttpClientFlow,
	EHttpServerFlow,
	EHttpUdpFlow,
	EHttpMUdpFlow,
	};

// control channel message option flags
struct TCHMessageOption
	{
//	public:
	TCHMessageOption()
	: iId(0),
	iValue(0)
	{
	}

	TCHMessageOption ( TUint aId, TInt aValue )
	: iId ( aId ),
	iValue ( aValue )
	{
	}

	TUint iId;
	TInt  iValue;
	};



class CUPnPFlowBase: public CSubConnectionFlowBase
	{
	friend class CUPnPFlowFactory;
public:
	inline TUint FlowType();

//From CSubConnectionFlowBase
	//virtual TInt ReceivedL(TCFSignatureBase& aCFMessage);
 	inline virtual MFlowBinderControl* DoGetBinderControlL();
protected:
	CUPnPFlowBase( CSubConnectionFlowFactoryBase& aFactory, CProtocolIntfBase* aProtocolIntf, TFlowType aFlowType, const TNodeId& aSubConnId );
	virtual ~CUPnPFlowBase();
	void AddControlClientL ( const TNodeId& aId );
	TInt RemoveControlClient ( TInt aIndex );
	TInt FindControlClient ( const TNodeId& aId );
	TInt CountControlClients ( );
	void SetClientLeaving ( const TNodeId& aId );
	TBool IsClientLeaving ( const TNodeId& aId );
	TBool CheckIfAllClientsLeaving ( );
	void PostDataClientStopped ( TNodeCtxId aChannelId );

protected:
	TFlowType iFlowType;
	RArray<RNodeInterface>		iClients;
	};

TUint CUPnPFlowBase::FlowType()
	{
	return iFlowType;
	}

MFlowBinderControl* CUPnPFlowBase::DoGetBinderControlL()
	{
	//Not supported.
	ASSERT(0);
	MFlowBinderControl *aBinderControl = NULL;
	return aBinderControl;
 	}

#endif // UPNPFLOWBASE_H
