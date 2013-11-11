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

#ifndef __UPNPSERVICECPR_H
#define __UPNPSERVICECPR_H

#include <comms-infras/corecpr.h>
#include <comms-infras/ss_linkcprnodemessages.h>
#include <comms-infras/ss_api_ext.h>
#include <comms-infras/ss_platsec_apiext.h>

#include "upnpcpr.h"

#include "upnpsppublishinfocontainer.h"


using namespace ESock;

//Forward Declarations
	
class CUPnPServiceConnectionProvider : 	public CUPnPConnectionProvider
  	{
public:
	static CUPnPServiceConnectionProvider* NewL ( CConnectionProviderFactoryBase& aFactory );
	virtual ~CUPnPServiceConnectionProvider ( );
		
	static TInt RootDeviceRepublishTimerCallbackL ( TAny* aPtr );
	inline TInt IsRootDeviceRegistered ( );
	inline void SetRootDeviceRegistered ( TBool aValue );
	void ProcessRootDeviceRegisteredL ( );	
	CUPnPRootDeviceInfoContainer& GetRootDeviceInfoContainer ( );
	CStringPoolManager& StringPoolManager ( );
	inline const CUPnPDevice& RootDevice ();
		
private:
	CUPnPServiceConnectionProvider ( CConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap );
	void ConstructL ( );	
    virtual void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
#ifdef SYMBIAN_NETWORKING_UPS
    TInt GetProcessAndThreadId ( TProcessId& aProcessId, TThreadId& aThreadId ) const;
#endif
 	 	
private:
 	CUPnPRootDeviceInfoContainer*	iRootDeviceInfoContainer;
	CPeriodic*						iRepublishTimer;
	TBool							iRootDeviceRegistered;
	};

TBool CUPnPServiceConnectionProvider::IsRootDeviceRegistered ( )
   	{
   	return iRootDeviceRegistered;
   	}
   
void CUPnPServiceConnectionProvider::SetRootDeviceRegistered ( TBool aValue )
   	{
   	iRootDeviceRegistered = aValue;
   	}
   	
const CUPnPDevice& CUPnPServiceConnectionProvider::RootDevice ()
	{	
	CUPnPDeviceDescription& deviceDesc = iRootDeviceInfoContainer->GetRootDeviceDesciption (); 
	return *deviceDesc.DeviceObject ();
	}
	
	
	
#endif // UPNPSERVICECPR_H
