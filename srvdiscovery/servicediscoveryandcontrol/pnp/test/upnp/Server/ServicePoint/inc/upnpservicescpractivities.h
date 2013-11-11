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

#ifndef __UPNPSERVICESCPRACTIVITIES_H
#define __UPNPSERVICESCPRACTIVITIES_H

// System Include
#include <comms-infras/corescpractivities.h>

// User Include
#include "upnpservicescpr.h"

using namespace ESock;
using namespace MeshMachine;

namespace UPnPServiceSCprActivities
	{
	DECLARE_ACTIVITY_MAP(activityMap)
	
		
	//-------------------New Service Registration using SCRP----------------------------
	class CUpnpServiceRegistrationActivity: public CNodeActivityBase
		{
	public:
		static CNodeActivityBase* NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );
		~CUpnpServiceRegistrationActivity ( );
				
	private:
		CUpnpServiceRegistrationActivity ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );			
		
		void GenerateServiceIdL ( CStringPoolManager& aStringPoolMgr, const TDesC8& aUri, RBuf8& aServiceId );
		void CreateRelativeUrlL ( const TDesC8& aServiceId, const TDesC8& aUri, RBuf8& aResultUrl );
		
	private:
		CUPnPServiceRegisterParamSet*	iParamSet;
		CUPnPServiceInfo*				iServiceInfo; // Dont delete ownership will be passed to CPR		
		
	public:			
		typedef TNodeContext<CUPnPServiceSubConnectionProvider, CoreStates::TContext> TContext;
	
		// States
		typedef TAwaitingMessageState<TUpnpMessage::TServiceRegistration> TAwaitingServiceRegistration;
				
		
		//Transitions
		DECLARE_SMELEMENT_HEADER ( TRequestServiceJoin, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TRequestServiceJoin )
		
		DECLARE_SMELEMENT_HEADER ( TBuildPublishAndServiceInfo, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TBuildPublishAndServiceInfo )
		
		DECLARE_SMELEMENT_HEADER ( TRegisterServiceInfoWithCPR, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TRegisterServiceInfoWithCPR )
		
		DECLARE_SMELEMENT_HEADER ( TSendPublishRequest, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TSendPublishRequest )
		
		DECLARE_SMELEMENT_HEADER ( TRegisterSsdpInfoWithServer, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TRegisterSsdpInfoWithServer )
				
		DECLARE_SMELEMENT_HEADER ( TInitiateTcpFlowCreation, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TInitiateTcpFlowCreation )
				
		};
			
	
	//-------------------New Device Registration using SCRP----------------------------
	class CUpnpDeviceRegistrationActivity: public CNodeActivityBase			
		{
	public:
		static CNodeActivityBase* NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );
		~CUpnpDeviceRegistrationActivity ( );
				
	private:
		CUpnpDeviceRegistrationActivity ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );		

	private:
		CUPnPDeviceRegisterParamSet *iParamSet;
		CUPnPDevice*				iDeviceInfo; // Dont delete ownership will be passed to CPR
		RBuf8						iIconUrl;
		TInt						iIconUrlIdx;
	public:
		typedef TNodeContext<CUPnPServiceSubConnectionProvider, CoreStates::TContext> TContext;
	
		// States 		
		typedef TAwaitingMessageState<TUpnpMessage::TDeviceRegistration> TAwaitingDeviceRegistration;
		
		// Transitions
		DECLARE_SMELEMENT_HEADER ( TRequestDeviceJoin, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TRequestDeviceJoin )
		
		DECLARE_SMELEMENT_HEADER ( TBuildPublishAndDeviceInfo, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TBuildPublishAndDeviceInfo )
		
		DECLARE_SMELEMENT_HEADER ( TRegisterDeviceInfoWithCPR, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TRegisterDeviceInfoWithCPR )
		
		DECLARE_SMELEMENT_HEADER ( TSendPublishRequest, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TSendPublishRequest )
		
		DECLARE_SMELEMENT_HEADER ( TRegisterSsdpInfoWithServer, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			void DoL ( );
			void RegisterForMSearchL ( const TDesC8& aSearchTarget );
		DECLARE_SMELEMENT_FOOTER ( TRegisterSsdpInfoWithServer )	
		
		DECLARE_SMELEMENT_HEADER ( TCreateIconFlow, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			void DoL ();
		DECLARE_SMELEMENT_FOOTER ( TCreateIconFlow )
				
		// StateForks
		DECLARE_SMELEMENT_HEADER ( TIconCreateOrNoTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
			TInt TransitionTag ();
		DECLARE_SMELEMENT_FOOTER ( TIconCreateOrNoTag )
		
		DECLARE_SMELEMENT_HEADER ( TIconOrNoTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
			TInt TransitionTag ();
		DECLARE_SMELEMENT_FOOTER ( TIconOrNoTag )
		};
							
	
	//-------------------TCP ( Get/Subscribe/UnSubscribe ) Response Activity----------------------------
	class CUpnpSPRequestInfoActivity: public CNodeActivityBase
		{
	public:
		static CNodeActivityBase* NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );
		
		~CUpnpSPRequestInfoActivity ( );
			
	private:
		CUpnpSPRequestInfoActivity::CUpnpSPRequestInfoActivity ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
		: CNodeActivityBase ( aActivitySig, aNode )
		{  }
				
		TInt HandleSubscribeRequestL ( CRequest& aRequest, TSubsribeResponseInfo& aSubscribeResponse, CUPnPPublishInfoElement& aPublishInfo );
		void ProcessCallBackUrlL ( const TDesC8& aUrl, CUPnPSubscribeInfoElement* aSubscribeElement );
		
		TInt HandleResubscribeRequestL ( CRequest& aRequest, TSubsribeResponseInfo& aSubscribeResponse, CUPnPPublishInfoElement& aPublishInfo );			
		TInt GetHeaderValue ( CRequest& aRequest, TInt aFieldIndex, THTTPHdrVal& aFieldVal, const TStringTable& aTable );
			
	private: // Context
		typedef TNodeContext<CUPnPServiceSubConnectionProvider, CoreStates::TContext> TContext;								

	public: // Transitions
		
		DECLARE_SMELEMENT_HEADER ( TSendDescribeResponse, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TSendDescribeResponse )

		DECLARE_SMELEMENT_HEADER ( TSendSubscribeResponse, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			void DoL ( );
			TBool IsFirstSubscription ( );		
		DECLARE_SMELEMENT_FOOTER ( TSendSubscribeResponse )
		
		DECLARE_SMELEMENT_HEADER ( TSendUnsubscribeResponse, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
			void DoL ( );
			TBool IsLastSubscription ( );		
		DECLARE_SMELEMENT_FOOTER ( TSendUnsubscribeResponse )				
		};
		
		
	class CNotifyFlowFinder: public CNodeParallelActivityBase
		{
	public:
		static CNodeActivityBase* NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );
		~CNotifyFlowFinder ( );
		
	private:
		CNotifyFlowFinder ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aActivityCount );
		
	private:
		CUPnPSubscribeInfoElement*	iSubscribeInfo; // don't delete, not owned
	
		// Context
		typedef TNodeContext<CUPnPServiceSubConnectionProvider, CoreStates::TContext> TContext;
	
	public:
		// States
		typedef TAwaitingMessageState<TUpnpMessage::TCreateNotifyFlow> TAwaitingFlowCreationRequest;
	
		// Transitions
		DECLARE_SMELEMENT_HEADER ( TFindOrCreateFlow, TStateTransition<TContext>, NetStateMachine::MStateTransition, CNotifyFlowFinder::TContext )
			void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TFindOrCreateFlow )
	
		DECLARE_SMELEMENT_HEADER ( TProcessDataClient, TStateTransition<TContext>, NetStateMachine::MStateTransition, CNotifyFlowFinder::TContext )
			void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TProcessDataClient )
		};
		
		
	class CUpnpEventNotificationActivity: public CNodeParallelActivityBase
		{
	public:
		static CNodeActivityBase* NewL ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );
		~CUpnpEventNotificationActivity ( ) 
		{ }
		
	private:
		CUpnpEventNotificationActivity ( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aActivityCount );
		
	private:		
		// Context
		typedef TNodeContext<CUPnPServiceSubConnectionProvider, CoreStates::TContext> TContext;
		
	public: 
		// States
		typedef TAwaitingMessageState<TUpnpMessage::TNotifyRequest> TAwaitingNotifyRequest;
		typedef TAwaiting2MessagesState<TEBase::TError, TUpnpMessage::TResponse> TAwaitingErrorOrResponse;
			
		// Transitions		
		DECLARE_SMELEMENT_HEADER ( TSendRequest, TStateTransition<TContext>, NetStateMachine::MStateTransition, CUpnpEventNotificationActivity::TContext )
			void DoL ( );
		DECLARE_SMELEMENT_FOOTER ( TSendRequest )			
		};	
	}

#endif // UPNPSERVICESCPRACTIVITIES_H
