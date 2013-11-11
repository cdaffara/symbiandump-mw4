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


//Local Includes
#include "upnpflowbase.h"
#include "upnppint.h"

using namespace Messages;


CUPnPFlowBase::CUPnPFlowBase(CSubConnectionFlowFactoryBase& aFactory, ESock::CProtocolIntfBase* aProtocolIntf, TFlowType aFlowType, const TNodeId& aSubConnId)
		: CSubConnectionFlowBase(aFactory, aSubConnId, aProtocolIntf), iFlowType ( aFlowType )
	{
	}

CUPnPFlowBase::~CUPnPFlowBase()
	{
	iClients.Close ( );
	}

/*
//From CSubConnectionFlowBase MNode
TInt CUPnPFlowBase::ReceivedL(TCFSignatureBase& )
	{
	//This is just a base flow and should not receive any messages.
	ASSERT(0);
	return KErrNone;
	}
*/

void CUPnPFlowBase::AddControlClientL ( const TNodeId& aId )
	{
	if ( KErrNotFound == FindControlClient ( aId ) )
		{
		RNodeInterface client;
    	client.Open ( aId );
    	iClients.AppendL ( client );
		}
	}

TInt CUPnPFlowBase::RemoveControlClient ( TInt aIndex )
	{
	__ASSERT_DEBUG ( aIndex >= 0 && aIndex < iClients.Count () , User::Invariant ( ) );
	
	iClients.Remove ( aIndex );
	return iClients.Count ( );
	}

TInt CUPnPFlowBase::FindControlClient ( const TNodeId& aId )
	{
	for ( TInt i = 0; i < iClients.Count ( ); i++ )
		{
		if ( aId == iClients[i].RecipientId ( ) )
			{
			return i;
			}
		}
	return KErrNotFound;
	}

TInt CUPnPFlowBase::CountControlClients ( )
	{
	return iClients.Count ( );
	}

void CUPnPFlowBase::SetClientLeaving ( const TNodeId& aId )
	{
	TInt idx = FindControlClient ( aId );
	__ASSERT_DEBUG ( idx != KErrNotFound, User::Invariant ( ) );

	iClients[idx].SetFlags ( TClientType::ELeaving );
	}

TBool CUPnPFlowBase::IsClientLeaving ( const TNodeId& aId )
	{
	TInt idx = FindControlClient ( aId );
	__ASSERT_DEBUG ( idx != KErrNotFound, User::Invariant ( ) );

	if ( iClients[idx].Flags () == TClientType::ELeaving )
		{
		return ETrue;
		}
	return EFalse;
	}

TBool CUPnPFlowBase::CheckIfAllClientsLeaving ( )
	{
	TUint count = CountControlClients ( );
	for ( TInt i = 0; i < count; i++ )
		{
		if ( iClients[i].Flags () != TClientType::ELeaving )
			{
			return EFalse;
			}
		}
	return ETrue;
	}

void CUPnPFlowBase::PostDataClientStopped ( TNodeCtxId aSenderId )
	{
	//ESock::TCFDataClient::TDataClientStopped dataClientStopped ( NodeId (), aChannelId.iActivityId, 0 );
	//dataClientStopped.PostTo ( aChannelId.iNodeId );
	RClientInterface::OpenPostMessageClose(Id(), aSenderId, ESock::TCFDataClient::TStopped(0).CRef() );
	}


