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

#ifndef __UPNPSERVICESCPR_H
#define __UPNPSERVICESCPR_H

#include <comms-infras/corescpr.h>

#include "upnpservicecpr.h"
#include "upnpspsubscribeinfocontainer.h"
#include "upnpparamset.h"
#include "upnp_cf_msgs.h"
#include "upnpsppublishinfo.h"
#include "upnpscpr.h"


using namespace Messages;


//Forward Declarations
namespace UPnPServiceSCprStates
	{
	class TStoreRequestParams;
	class TSendRequestParamResponse;
	class TInitiateActivities;
	}

#ifdef __X86GCC__
 	inline ESock::CSubConnectionProviderBase* __x86gcc_subconnection_provider_base_cast(Messages::ANode* aNode)
     	{
 	    return static_cast<ESock::CSubConnectionProviderBase*>(ESock::__x86gcc_mm_comms_provider_base_cast(aNode));
     	}
 
 	inline ESock::CSubConnectionProviderBase& __x86gcc_subconnection_provider_base_cast(Messages::ANode& aNode)
     	{
 	    return static_cast<ESock::CSubConnectionProviderBase&>(ESock::__x86gcc_mm_comms_provider_base_cast(aNode));
     	}
namespace Messages
 	{
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
  	// functions to do the cast.
 	template <>
 	inline ESock::CSubConnectionProviderBase* mnode_cast<ESock::CSubConnectionProviderBase>(Messages::ANode* aNode)
 		{
 		return __x86gcc_subconnection_provider_base_cast(aNode);
 		}
 
 	template <>
 	inline ESock::CSubConnectionProviderBase& mnode_cast<ESock::CSubConnectionProviderBase>(Messages::ANode& aNode)
 		{
 		return __x86gcc_subconnection_provider_base_cast(aNode);
 		}
 	}
#endif

class CUPnPServiceSubConnectionProvider : public CUPnPSubConnectionProvider
	{
	friend class UPnPServiceSCprStates::TStoreRequestParams;
	friend class UPnPServiceSCprStates::TSendRequestParamResponse;
	friend class UPnPServiceSCprStates::TInitiateActivities;

#ifdef __X86GCC__
  	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
  	// functions to do the cast.
    friend  ESock::CSubConnectionProviderBase* __x86gcc_subconnection_provider_base_cast(Messages::ANode* aNode);
    friend  ESock::CSubConnectionProviderBase& __x86gcc_subconnection_provider_base_cast(Messages::ANode& aNode);
#elif !defined(__GCCXML__)
    friend  CSubConnectionProviderBase* mcfnode_cast<CSubConnectionProviderBase>(Messages::ANode* aNode);
    friend  CSubConnectionProviderBase& mcfnode_cast<CSubConnectionProviderBase>(Messages::ANode& aNode);
#endif

public:
	static CUPnPServiceSubConnectionProvider* NewL ( ESock::CSubConnectionProviderFactoryBase& aFactory );
	virtual ~CUPnPServiceSubConnectionProvider ( );
	
	void SetKeyL ( const TDesC8& aKey );
	const TDesC8& Key ( );
	void SetEventUrlL ( const TDesC8& aEventUrl );
	const TDesC8& EventUrl ( );
	void SetControlUrlL ( const TDesC8& aControlUrl );
	const TDesC8& ControlUrl ( );
	void SetDevice ( const TBool aIsDevice );	
	TBool IsDevice ( );
	const TDesC8& SCPDData ( );
	TNodeId UdpClientFlow ( );
	TNodeId UdpServerFlow ( );
	void SetUdpServerFlow ( const TNodeId& aCommsId );
	void SetUdpClientFlow ( const TNodeId& aCommsId );
	void SetPublishInfo ( CUPnPPublishInfoElement* aPublishInfo );
	CUPnPPublishInfoElement* PublishInfo ( );
	CUPnPSubscribeInfoContainer* GetSubscribeInfoContainer ( );	
	CUPnPServiceConnectionProvider& ConnectionProvider ( );	
	TBool IsClosing ( );
	void SetClosing ( );	
	
	void AddResponseParamSetL ( TUPnPEvent aEvent );
	void ClearResponseParamL ( );
	
	void DoPublishL ( const TDesC8& aSearchTarget, const TDesC8& aUsn, TInt aMaxAge );    
	void DeprecateExpiredSubscriptions ( );	
    void DestroyDataClients ( );
		
private:	
	CUPnPServiceSubConnectionProvider ( ESock::CSubConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap );
	void ConstructL ( );
    virtual void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );	   
	
	void NotifySubConnectionL ( );		
	void SendNotifyToRemoteClientsL ( RParameterFamily& aReqFamily );
    void InitiateActivitiesL ( );
    void InitiateRequestL ( RParameterFamily& aReqFamily );
    void CreatePropertyUrlL ( const TDesC8& aRelativeUrl, RBuf8& aTargetUrl );
    
    CUPnPDeviceRegisterParamSet* CopyWithIconInfoL ( CUPnPDeviceRegisterParamSetExt* aDeviceParam );
    void LoadIconListL ( const TDesC8& aIconData, CUPnPDeviceRegisterParamSet* aDeviceParamSet );
    void ReadTInt ( TPtrC8& aBuffer, TInt& aValue );    
    
private:
	RParameterFamilyBundle 	iRequestBundle;	
	RCFParameterFamilyBundle	iResponseBundle;
		
	CUPnPSubscribeInfoContainer*	iSubscribeInfoCont;
	CUPnPPublishInfoElement*		iPublishInfo;
	RBuf8							iDescUrl;
	RBuf8							iControlUrl; 
	RBuf8							iEventUrl;
	TBool							iDevice;
	
	TNodeId 						iUdpClientFlow;
	TNodeId 						iUdpServerFlow;
	TBool 							iClosing;	
	};

inline TBool CUPnPServiceSubConnectionProvider::IsClosing ( )
	{
    return iClosing;
    }
    
inline void CUPnPServiceSubConnectionProvider::SetClosing ( )
	{
	iClosing = ETrue;
	}    

inline TNodeId CUPnPServiceSubConnectionProvider::UdpClientFlow ( )
	{
	return iUdpClientFlow;
	}
	
inline void CUPnPServiceSubConnectionProvider::SetUdpClientFlow ( const TNodeId& aCommsId )
	{
	iUdpClientFlow = aCommsId;
	}
	
inline TNodeId CUPnPServiceSubConnectionProvider::UdpServerFlow ( )
	{
	return iUdpServerFlow;
	}
	
inline void CUPnPServiceSubConnectionProvider::SetUdpServerFlow ( const TNodeId& aCommsId )
	{
	iUdpServerFlow = aCommsId;
	}


#endif // UPNPSERVICESCPR_H
