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

#ifndef __UPNPSTATEMACHINE_H_
#define __UPNPSTATEMACHINE_H_

//System Includes
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/corescprstates.h>
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/ss_corepractivities.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <ss_glob.h>

//Local Includes
#include "upnpctrlscpr.h"
#include "upnpservicescpr.h"
#include "upnpservicedeftscpr.h"
#include "upnpflowfactory.h"
#include "upnpflowbase.h"

namespace UPnPStateMachine
{
	//Should never be used except for the MM itself
	const TInt KSendRequest 		= NetStateMachine::KExecuteAlways - 5;	
	const TInt KProcessResponse		= NetStateMachine::KExecuteAlways - 6;
	const TInt KDescribeRequest   	= NetStateMachine::KExecuteAlways - 7;
	const TInt KSubscribeRequest  	= NetStateMachine::KExecuteAlways - 8;
	const TInt KUnsubscribeRequest	= NetStateMachine::KExecuteAlways - 9;
	const TInt KNotifyRequest     	= NetStateMachine::KExecuteAlways - 10;
	const TInt KDeviceTag	     	= NetStateMachine::KExecuteAlways - 11;
	const TInt KServiceTag			= NetStateMachine::KExecuteAlways - 12;
	const TInt KRootRegisteredTag	= NetStateMachine::KExecuteAlways - 13;	
	const TInt KStart				= NetStateMachine::KExecuteAlways - 14;	
	const TInt KStop				= NetStateMachine::KExecuteAlways - 15;
	const TInt KActivityStop		= NetStateMachine::KExecuteAlways - 16;
	const TInt KCreateIconTag		= NetStateMachine::KExecuteAlways - 17;
	const TInt KIconCreatedTag		= NetStateMachine::KExecuteAlways - 18;
	const TInt KIconRequest		= NetStateMachine::KExecuteAlways - 19;
	
enum TUpnpServerFlowType
	{
	ECallback,
	ERootDescription,
	EDescription,
	ESubscription,
	EControl,
	};

typedef TNodeContext<CUPnPSubConnectionProvider, PRStates::TContext> TContext;

DECLARE_SMELEMENT_HEADER ( TAwaitingSetParamRequest, TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept ( );
DECLARE_SMELEMENT_FOOTER ( TAwaitingSetParamRequest )
	
typedef TAwaitingMessageState<TUpnpMessage::TCreateServerFlow> TAwaitingServerFlowRequest;
typedef TAwaitingMessageState<TUpnpMessage::TUPnPRequestInfo> TAwaitingUpnpRequestInfo;
typedef TAwaiting2MessagesState<TUpnpMessage::TLocalHost, TEBase::TError> TAwaitingLocalHostFromMCpr;
typedef TAwaitingMessageState<TUpnpMessage::TUPnPSearchRequest> TAwaitingSearchRequest;


DECLARE_SMELEMENT_HEADER ( TFindOrCreateUdpClientFlow, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TFindOrCreateUdpClientFlow )


DECLARE_SMELEMENT_HEADER ( TFindOrCreateUdpServerFlow, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TFindOrCreateUdpServerFlow )

	
template<TUpnpServerFlowType FLOWTYPE>
class TCreateServerFlow : public TStateTransition<PRStates::TContext>
    {
public:
	NETSM_TPL_DECLARE_CTR ( TCreateServerFlow, NetStateMachine::MStateTransition, PRStates::TContext )

    TCreateServerFlow ( PRStates::TContext& aContext )
    	: TStateTransition<PRStates::TContext> ( aContext )
        {
        }
        
	virtual void DoL ( )
		{
		ASSERT ( this->iContext.iNodeActivity );
		
		const TUint8* uriPtr = NULL;
		TUint size = 0;
		switch ( FLOWTYPE )
			{
		case ECallback:
			{
			CUPnPControlSubConnectionProvider& node = static_cast<CUPnPControlSubConnectionProvider&> ( iContext.Node() );
			uriPtr = node.Callback( ).Ptr ( );
			size = node.Callback( ).Size ( );
			}
			break;
				
		case EDescription:
			{
			CUPnPServiceSubConnectionProvider& node1 = static_cast<CUPnPServiceSubConnectionProvider&> ( iContext.Node ( ) );
			uriPtr = node1.Key ( ).Ptr ( );
			size = node1.Key ( ).Size ( );
			}
			break;
				
		case ESubscription:
			{
			CUPnPServiceSubConnectionProvider& node2 = static_cast<CUPnPServiceSubConnectionProvider&> ( iContext.Node ( ) );
			uriPtr = node2.EventUrl ( ).Ptr ( );
			size = node2.EventUrl ( ).Size ( );
			}
			break;
				
		case EControl:
			{
			CUPnPServiceSubConnectionProvider& node3 = static_cast<CUPnPServiceSubConnectionProvider&> ( iContext.Node ( ) );
			uriPtr = node3.ControlUrl ( ).Ptr ( );
			size = node3.ControlUrl ( ).Size ( );
			}
			break;
				
		case ERootDescription:
			{
			CUPnPServiceDefaultSubConnectionProvider& node4 = static_cast<CUPnPServiceDefaultSubConnectionProvider&> (iContext.Node());
			uriPtr = node4.Key ( ).Ptr ( );
			size = node4.Key ( ).Size ( );		
			}
			break;
				
		default:
			break;
			}
			
		const TUid requestedUid = { CUPnPFlowFactory::iUid };
		THttpServerFlowQuery flowQuery ( uriPtr,
										 size,
										 iContext.NodeId (),
										 EHttpServerFlow );
		
		ESock::TCFFactory::TFindOrCreatePeer msg ( TCFPlayerRole::EDataPlane, requestedUid, &flowQuery );
		
		TCFPlayerRole playerRole ( TCFPlayerRole::EDataPlane );
		Messages::TNodeId factoryContainer = SockManGlobals::Get()->GetPlaneFC( playerRole );
		iContext.iNodeActivity->PostRequestTo( factoryContainer, msg );
		}
    };


DECLARE_SMELEMENT_HEADER ( TStartServerFlow, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TStartServerFlow )
    
DECLARE_AGGREGATED_TRANSITION2 (
	TProcessServerFlowCreation,
	PRStates::TProcessDataClientCreation,
	TStartServerFlow	
	)

DECLARE_SMELEMENT_HEADER ( THandleControlChannelDataClientIdle, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( THandleControlChannelDataClientIdle )
		
	
DECLARE_SMELEMENT_HEADER ( TRequestLocalHost, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TRequestLocalHost )


DECLARE_SMELEMENT_HEADER ( TSendIgnoreResponse, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TSendIgnoreResponse )

    
DECLARE_SMELEMENT_HEADER ( TRequestOrIgnoreTag, TStateFork<PRStates::TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag ( );
DECLARE_SMELEMENT_FOOTER ( TRequestOrIgnoreTag )


DECLARE_SMELEMENT_HEADER ( TDiscardSearchRequestData, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TDiscardSearchRequestData )


DECLARE_SMELEMENT_HEADER ( TDestroyDataClients, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TDestroyDataClients )


DECLARE_SMELEMENT_HEADER ( TStopDataClients, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TStopDataClients )
	

DECLARE_SMELEMENT_HEADER ( TAwaitingToStopDataClients, TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept ( );
DECLARE_SMELEMENT_FOOTER ( TAwaitingToStopDataClients )

DECLARE_SMELEMENT_HEADER ( TAwaitingBindToComplete, TState<TContext>, NetStateMachine::MState, TContext )
virtual TBool Accept ( );
DECLARE_SMELEMENT_FOOTER ( TAwaitingBindToComplete )

DECLARE_SMELEMENT_HEADER ( TStoreBindToOriginator, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL ( );
DECLARE_SMELEMENT_FOOTER ( TStoreBindToOriginator )




// State Forks..

struct TCheckIfAllActivitiesStopped
	{
	// graceful closing of activites is not needed in case of service point,
	// since the State machines are simple and there are no lo
	static TBool IsBlocked ( TNodeContextBase& aContext )
		{
		// wait untill all upnp activites are done with protocol specific transactions
		return aContext.iNode.CountAllActivities ( ) == 1 ? EFalse : ETrue;
		}
	};
	
DECLARE_SERIALIZABLE_STATE(
	TBlockedByAllActivitiesStopped,
    TCheckIfAllActivitiesStopped,
    TNoTag    
    )

// Utiliy API's
class CUPnPUtils
	{
public:
	static TInt ExtractVersionNumber ( const TDesC8& aSearchTarget, TInt& aUriIndex );	
	static void CleanupMBufChain ( TAny* aAny );
	static void FindAndSendIconDataL ( CTransaction* aTransaction, TInt& aStatus, RMemoryAllocator& aAllocator );
	};

}

#endif /*__UPNPSTATEMACHINE_H_*/
