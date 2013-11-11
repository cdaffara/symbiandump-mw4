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

#ifndef __UPNPCONTROLSCPR_H
#define __UPNPCONTROLSCPR_H

#include <comms-infras/corescpr.h>
#include <comms-infras/ss_nodemessages.h>

#include <upnpparamset.h>
#include <upnp/pnpextparameterfamily.h>
#include <rmemchunk.h>

#include "upnpctrlcpr.h"
#include "upnpscpr.h"

using namespace Messages;


//Forward Declarations
namespace UPnPControlSCprStates
	{
	class TStoreRequestParams;
	class TSendRequestParamResponse;
	class TInitiateActivities;
	}


// Subscription Information Element
class CSubscriptionInfo: public CBase
	{
public:
	CSubscriptionInfo ( TNodeId& aFlowId )
		: iSeqKey ( -1 ), iDataClient ( aFlowId )
		{  }
		
	~CSubscriptionInfo ( )
		{
		iPublishUrl.Close ( );
		iSid.Close ( );
		}
		
	void SetPublishUrlL ( const TDesC8& aUrl )
		{
		iPublishUrl.Close ( );
		iPublishUrl.CreateL ( aUrl );
		}
		
	void SetSidL ( RMemChunk& aSid )
		{
		iSid.CreateMaxL ( aSid.Length ( ) );
		aSid.CopyOut ( iSid );
		}
	
	TInt		iSeqKey;
	TNodeId 	iDataClient;
	RBuf8 		iPublishUrl;
	RBuf8	 	iSid;
	};

class CUPnPControlSubConnectionProvider : public CUPnPSubConnectionProvider										  
	{
	friend class UPnPControlSCprStates::TStoreRequestParams;
	friend class UPnPControlSCprStates::TSendRequestParamResponse;
	friend class UPnPControlSCprStates::TInitiateActivities;	
	
public:
	static CUPnPControlSubConnectionProvider* NewL ( CSubConnectionProviderFactoryBase& aFactory );
	virtual ~CUPnPControlSubConnectionProvider ( );     
    
    
    TNodeId UdpClientFlow ( );
	TNodeId UdpServerFlow ( );
	TNodeId TcpServerFlow ( );
	void SetUdpServerFlow ( const TNodeId& aCommsId );
	void SetTcpServerFlow ( const TNodeId& aCommsId );
	void SetUdpClientFlow ( const TNodeId& aCommsId );
	
	inline TBool SetClosing ( );
	inline TBool IsClosing ( );
	inline const TDesC8& Callback() const;
	
	void AddResponseParamSetL ( XParameterSetBase* aResponseSet, const TUint32 aFamilyId);
	
	void AddSubscriptionInfoL(CSubscriptionInfo* aSubscriptionInfo);	
	void RemoveSubscriptionInfo ( const TDesC8& aSid );
	CSubscriptionInfo* GetSubscriptionInfoByUrl( const TDesC8& aPublishUrl );
	CSubscriptionInfo* GetSubscriptionInfoBySid ( const TDesC8& aSid );
	CUPnPControlConnectionProvider& ConnectionProvider ( );	
	
private:
	CUPnPControlSubConnectionProvider ( CSubConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap );
	void ConstructL ( );
	virtual void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
    void HandlePassThroughMessage ( MeshMachine::TNodeContextBase& aContext );
    
    void InitiateActivitiesL ( );
    void InitiateRequestL ( RParameterFamily& aReqFamily );
    void ClearResponseParamL ( );
    void NotifySubConnectionIfResultsAvailable ( );
    TBool CheckForResultsAvailability ( TUint32& aFamilyId );
	RParameterFamily GetUPnPFamilyById ( TUint32 aFamilyId );
	void CleanupEventQueuedResponses ( const TDesC8& aUrl, TUint aFamilyId );
	
	// Enqueue's event for CSubConnection, if not closing
	void NotifySubConnectionL ( const TUint32 aResponseFamilyId );
	
	void GenerateCallbackUrlL ( );
	
public:
	struct TFlowInfo
		{
		TAppProtAddr iAppProtAddr;
		TNodeId	 	 iDataClient;
		};
	RArray<TFlowInfo>	iFlowInfo;
	TBool		iTcpSrvFlowCreateRequested;
	
private:
	RCFParameterFamilyBundle	iResponseBundle;
	RParameterFamilyBundle	iRequestBundle;
	RParameterFamilyBundle 	iQueueBundle;

	
	TBool 	iResponseBlocked;
	TNodeId	iUdpClientFlow;
	TNodeId	iUdpServerFlow;	
	TNodeId	iTcpServerFlow;
		
	RBuf8								iCallbackUrl;
	RPointerArray<CSubscriptionInfo>	iSubscriptions;
};

inline TNodeId CUPnPControlSubConnectionProvider::UdpClientFlow ( )
	{
	return iUdpClientFlow;
	}

inline void CUPnPControlSubConnectionProvider::SetUdpClientFlow ( const TNodeId& aCommsId )
	{
	iUdpClientFlow = aCommsId;
	}
	
inline TNodeId CUPnPControlSubConnectionProvider::UdpServerFlow ( )
	{
	return iUdpServerFlow;
	}
	
inline void CUPnPControlSubConnectionProvider::SetUdpServerFlow ( const TNodeId& aCommsId )
	{
	iUdpServerFlow = aCommsId;
	}
	
inline TNodeId CUPnPControlSubConnectionProvider::TcpServerFlow ( )
	{
	return iTcpServerFlow;
	}
	
inline void CUPnPControlSubConnectionProvider::SetTcpServerFlow ( const TNodeId& aCommsId )
	{
	iTcpServerFlow = aCommsId;
	}
	
const TDesC8& CUPnPControlSubConnectionProvider::Callback ( ) const
	{
	return iCallbackUrl;
	}
	
TBool CUPnPControlSubConnectionProvider::IsClosing ( )
	{
	// Note: this API is not helping as UPnP parallel activities
	// may have same id value
	// return CountActivities ( ECFActivityDestroy ) == 1 ? ETrue : EFalse;
	// check if control client i.e CSubConnection is joined or not
	RNodeInterface* cc = GetFirstClient<TDefaultClientMatchPolicy> ( TCFClientType::ECtrl );		
	return cc == NULL ? ETrue : EFalse;
	}	

#endif // UPNPCONTROLSCPR_H
