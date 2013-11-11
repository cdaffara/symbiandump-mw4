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

#ifndef __UPNPCONTROLSCPRACTIVITIES_H
#define __UPNPCONTROLSCPRACTIVITIES_H

#include <comms-infras/corescpractivities.h>
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_corepractivities.h>
#include <http/thttptable.h>

#include "upnpctrlscpr.h"
#include "upnp_cf_msgs.h"


using namespace ESock;
using namespace MeshMachine;

namespace UPnPControlSCprActivities
	{	
	DECLARE_ACTIVITY_MAP ( activityMap )
	}

class CUPnPActivityBase : public CNodeParallelActivityBase
	{
protected:
	CUPnPActivityBase ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aActivitiesCount )
		:CNodeParallelActivityBase ( aActivitySig, aNode, aActivitiesCount )	
		{ }
	
	~CUPnPActivityBase ( ); 	
 	virtual TBool Next ( TNodeContextBase& aContext );
 
 public:
 	TNodeId iFlowId;
 	CUPnPRequestParamSetBase* iRequestParamSet;
 
 public:
 	typedef TNodeContext<CUPnPControlSubConnectionProvider, CoreStates::TContext> TContext;
 	
 	//States
	DECLARE_SMELEMENT_HEADER ( TStopCurrentActivities, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUPnPActivityBase::TContext )
    	virtual void DoL ( );    	
    DECLARE_SMELEMENT_FOOTER ( TStopCurrentActivities)    
   
	//TState Forks
	DECLARE_SMELEMENT_HEADER ( TStopActivityOrStopDataClient, TStateFork<TContext>, NetStateMachine::MStateFork, CUPnPActivityBase::TContext )
		virtual TInt TransitionTag ( );
	DECLARE_SMELEMENT_FOOTER ( TStopActivityOrStopDataClient )	

	//Transitions
	DECLARE_SMELEMENT_HEADER ( TAwaitingResponseToStopActivities, TState<TContext>, NetStateMachine::MState, CUPnPActivityBase::TContext )
		virtual TBool Accept ( );
	DECLARE_SMELEMENT_FOOTER ( TAwaitingResponseToStopActivities )

    DECLARE_AGGREGATED_TRANSITION2(
       TProcessClientLeave,
       TRemoveClient,
       CoreNetStates::TSendLeaveCompleteIfRequest
	   )
	};
//---------------------------------------Control Point Search Request Activity-------------------------------------
class CUPnPSearchActivity: public CUPnPActivityBase							 
	{
public:	    
	static CNodeActivityBase* NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );
	~CUPnPSearchActivity ( );
     
private:
	CUPnPSearchActivity ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aActivitiesCount )
		:CUPnPActivityBase ( aActivitySig, aNode, aActivitiesCount )
	  	{	  	
	  	}		
		
public:
 	typedef TNodeContext<CUPnPControlSubConnectionProvider, CoreStates::TContext> TContext;
 
	//States
	typedef TAwaitingMessageState<TUpnpMessage::TSearchRequest> TAwaitingSearchRequest;

	DECLARE_SMELEMENT_HEADER ( TAwaitingReceiveSearchResponse, TState<TContext>, NetStateMachine::MState, CUPnPSearchActivity::TContext )
	virtual TBool Accept ( );
	DECLARE_SMELEMENT_FOOTER ( TAwaitingReceiveSearchResponse )
	
	typedef TAwaiting2MessagesState<TUpnpMessage::TCancelled, TUpnpMessage::TUPnPSearchResponse> TAwaitingDataClientStopResponse;

	//Transitions
    DECLARE_SMELEMENT_HEADER ( TSendSearchRequest, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUPnPSearchActivity::TContext )
    	virtual void DoL ( );    	
    DECLARE_SMELEMENT_FOOTER ( TSendSearchRequest )    
	
	DECLARE_SMELEMENT_HEADER ( TProcessResponse, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUPnPSearchActivity::TContext )
    	virtual void DoL ( );    	
    DECLARE_SMELEMENT_FOOTER ( TProcessResponse )    

	DECLARE_SMELEMENT_HEADER ( TCleanupActivity, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUPnPSearchActivity::TContext )
    	virtual void DoL ( );    	
	DECLARE_SMELEMENT_FOOTER ( TCleanupActivity)    

	DECLARE_SMELEMENT_HEADER ( TCancelSearchRequest, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUPnPSearchActivity::TContext )
    	virtual void DoL ( );
	DECLARE_SMELEMENT_FOOTER ( TCancelSearchRequest )
 	};

//---------------------------------------Control Point Register Notify Request Activity-------------------------------------
class CUPnPRegisterNotifyActivity: public CUPnPActivityBase							 
	{
public:	    
	static CNodeActivityBase* NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );
	~CUPnPRegisterNotifyActivity ( );
     
private:
	CUPnPRegisterNotifyActivity ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aActivitiesCount )
		:CUPnPActivityBase ( aActivitySig, aNode, aActivitiesCount )
	  	{	  	
	  	}		
		
public:
	typedef TNodeContext<CUPnPControlSubConnectionProvider, CoreStates::TContext> TContext;
 
	//States
	typedef TAwaitingMessageState<TUpnpMessage::TRegisterNotifyRequest> TAwaitingRegisterNotifyRequest;

	DECLARE_SMELEMENT_HEADER ( TAwaitingReceiveNotifivation, TState<TContext>, NetStateMachine::MState, CUPnPRegisterNotifyActivity::TContext )
	virtual TBool Accept ( );
	DECLARE_SMELEMENT_FOOTER ( TAwaitingReceiveNotifivation )
	
	typedef TAwaitingMessageState<TCFDataClient::TStarted> TAwaitingServerStarted;
	typedef TAwaiting2MessagesState<TUpnpMessage::TCancelled, TUpnpMessage::TUPnPSearchResponse> TAwaitingDataClientStopResponse;
	
	//Transitions        
    DECLARE_SMELEMENT_HEADER ( TStartServer, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUPnPRegisterNotifyActivity::TContext )
    	virtual void DoL ( );
    DECLARE_SMELEMENT_FOOTER ( TStartServer )    

	DECLARE_SMELEMENT_HEADER ( TSendRegisterRequest, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUPnPRegisterNotifyActivity::TContext )
    	virtual void DoL ( );    	
    DECLARE_SMELEMENT_FOOTER ( TSendRegisterRequest )    

	DECLARE_SMELEMENT_HEADER ( TProcessNotification, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUPnPRegisterNotifyActivity::TContext )
    	virtual void DoL ( );    	
    DECLARE_SMELEMENT_FOOTER ( TProcessNotification )    

	DECLARE_SMELEMENT_HEADER ( TCancelRegisterRequest, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUPnPRegisterNotifyActivity::TContext )
    	virtual void DoL ( );    	
    DECLARE_SMELEMENT_FOOTER ( TCancelRegisterRequest )    

 	DECLARE_SMELEMENT_HEADER ( TCleanupActivity, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUPnPRegisterNotifyActivity::TContext )
    	virtual void DoL ( );    	
	DECLARE_SMELEMENT_FOOTER ( TCleanupActivity )
	};

//---------------------------------------Control Point Describe Request Activity-------------------------------------
class CUpnpDescribeActivity: public CUPnPActivityBase
	{
public:
	static CNodeActivityBase* NewL( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );
	~CUpnpDescribeActivity();
	
private:
	CUpnpDescribeActivity( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aActivityCount )
		:CUPnPActivityBase ( aActivitySig, aNode, aActivityCount )
		{ }
		
public:
	typedef TNodeContext<CUPnPControlSubConnectionProvider, CoreStates::TContext> TContext;
 
 	// States
	typedef TAwaitingMessageState<TUpnpMessage::TDescribeRequest> TAwaitingDescribeRequest;
	
	DECLARE_SMELEMENT_HEADER( TAwaitingErrorOrResponseOrCancel, TState<TContext>, NetStateMachine::MState, CUpnpDescribeActivity::TContext )
		TBool Accept();
	DECLARE_SMELEMENT_FOOTER ( TAwaitingErrorOrResponseOrCancel )

	DECLARE_SMELEMENT_HEADER ( TAwaitingDataClientStopResponse, TState<TContext>, NetStateMachine::MState, CUPnPRegisterNotifyActivity::TContext )
		virtual TBool Accept ( );
	DECLARE_SMELEMENT_FOOTER ( TAwaitingDataClientStopResponse )

	// Transitions
	DECLARE_SMELEMENT_HEADER ( TSendDescribeRequest, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUpnpDescribeActivity::TContext )
		void DoL();
	DECLARE_SMELEMENT_FOOTER( TSendDescribeRequest )

	DECLARE_SMELEMENT_HEADER ( TProcessResponse, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUpnpDescribeActivity::TContext )
		void DoL();
		void Error( TInt aError );
	DECLARE_SMELEMENT_FOOTER ( TProcessResponse )

	DECLARE_SMELEMENT_HEADER ( TReportError, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUpnpDescribeActivity::TContext )
		void DoL();
	DECLARE_SMELEMENT_FOOTER( TReportError )

	DECLARE_SMELEMENT_HEADER ( TCleanupActivity, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUpnpDescribeActivity::TContext )
		void DoL();
	DECLARE_SMELEMENT_FOOTER( TCleanupActivity )

	DECLARE_SMELEMENT_HEADER ( TDataClientStop, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUpnpDescribeActivity::TContext )
    	virtual void DoL ( );    	
	DECLARE_SMELEMENT_FOOTER ( TDataClientStop)    
	};


//---------------------------------------Control Point Subscribe Request Activity-------------------------------------
class CUpnpSubscribeActivity: public CUPnPActivityBase
	{
public:
	static CNodeActivityBase* NewL( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );
	~CUpnpSubscribeActivity();
	
private:
	CUpnpSubscribeActivity( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aActivityCount )
		:CUPnPActivityBase ( aActivitySig, aNode, aActivityCount )
		{ }
	
public :
	CSubscriptionInfo*  iSubscriptionInfo; // Not owned
	
public: // States	
	typedef TNodeContext<CUPnPControlSubConnectionProvider, CoreStates::TContext> TContext;
 
	typedef TAwaitingMessageState<TUpnpMessage::TSubscribeRequest> TAwaitingSubscribeRequest;
			
	DECLARE_SMELEMENT_HEADER ( TAwaitingErrorOrResponseOrCancel, TState<TContext>, NetStateMachine::MState, CUpnpSubscribeActivity::TContext )
		TBool Accept();
	DECLARE_SMELEMENT_FOOTER ( TAwaitingErrorOrResponseOrCancel )

public: // Transitions
	DECLARE_SMELEMENT_HEADER ( TSendSubscribeRequest, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUpnpSubscribeActivity::TContext )
		void DoL();
	DECLARE_SMELEMENT_FOOTER( TSendSubscribeRequest )

	DECLARE_SMELEMENT_HEADER ( TProcessSubscribeResponse, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUpnpSubscribeActivity::TContext )
		void DoL();
		void Error( TInt aError );
	DECLARE_SMELEMENT_FOOTER ( ProcessSubscribeResponse )

	DECLARE_SMELEMENT_HEADER ( TReportError, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUpnpSubscribeActivity::TContext )
		void DoL();
	DECLARE_SMELEMENT_FOOTER( TReportError )
	
	DECLARE_SMELEMENT_HEADER ( TSendUnsubscribeRequest, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUpnpSubscribeActivity::TContext )
		void DoL();
	DECLARE_SMELEMENT_FOOTER( TSendUnsubscribeRequest )
	
	DECLARE_SMELEMENT_HEADER ( TProcessUnsubscribeResponse, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUpnpSubscribeActivity::TContext )
		void DoL();
	DECLARE_SMELEMENT_FOOTER ( TProcessUnsubscribeResponse )
	};

class CUpnpUnSubscribeActivity: public CUPnPActivityBase
	{
public:
	static CNodeActivityBase* NewL( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );
	~CUpnpUnSubscribeActivity();
		
private:
	CUpnpUnSubscribeActivity( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aActivityCount )
		:CUPnPActivityBase ( aActivitySig, aNode, aActivityCount )
		{ }
	
private:
	CSubscriptionInfo*  iSubscriptionInfo; // Not Owned
	
public:
	typedef TNodeContext<CUPnPControlSubConnectionProvider, CoreStates::TContext> TContext;

	// States
	DECLARE_SMELEMENT_HEADER ( TAwaitingUnSubscribeRequest, TState<TContext>, NetStateMachine::MState, CUpnpUnSubscribeActivity::TContext )
		TBool Accept();
	DECLARE_SMELEMENT_FOOTER( TAwaitingUnSubscribeRequest )
	
	typedef TAwaiting2MessagesState<TEBase::TError, TUpnpMessage::TResponse> TAwaitingErrorOrResponse;

public: // Transitions				
	DECLARE_SMELEMENT_HEADER ( TSendUnSubscribeRequest, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUpnpUnSubscribeActivity::TContext )
		void DoL();
	DECLARE_SMELEMENT_FOOTER( TSendUnSubscribeRequest )

	DECLARE_SMELEMENT_HEADER ( TProcessResponse, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUpnpUnSubscribeActivity::TContext )
		void DoL();		
	DECLARE_SMELEMENT_FOOTER ( TProcessResponse )
	};

	
class CUpnpNotificationActivity: public CNodeParallelActivityBase
	{
public:
	static CNodeActivityBase* NewL( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );
	~CUpnpNotificationActivity();
	
private:
	CUpnpNotificationActivity( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aActivityCount );
	
private:
	RBuf8	   iBodyBuf;
	TPtrC8	   iPublisherUrl;
	CTransaction* iTransaction;
	TInt		iResponseStatus;
	
private: // Context
	typedef TNodeContext<CUPnPControlSubConnectionProvider, CoreStates::TContext> TContext;

public: 
	// Transitions
	DECLARE_SMELEMENT_HEADER ( TProcessNotifyRequest, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUpnpNotificationActivity::TContext )
		void DoL();
	DECLARE_SMELEMENT_FOOTER( TProcessNotifyRequest )

	DECLARE_SMELEMENT_HEADER ( TSendResponse, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUpnpNotificationActivity::TContext )
		void DoL();
	DECLARE_SMELEMENT_FOOTER( TSendResponse )	
	};	
 
#endif // UPNPCONTROLSCPRACTIVITIES_H
