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

#ifndef __UPNPSERVICEDEFAULTSCPR_H
#define __UPNPSERVICEDEFAULTSCPR_H

#include <comms-infras/corescpr.h>
#include <comms-infras/corescprstates.h>
#include <comms-infras/corescpractivities.h>


#include "cupnpdescriptioncomposer.h"
#include "upnpsppublishinfo.h"
#include "upnpservicecpr.h"
#include "upnpscpr.h"

using namespace Messages;


class CUPnPServiceDefaultSubConnectionProvider: public CUPnPSubConnectionProvider
	{
public:
	static CUPnPServiceDefaultSubConnectionProvider* NewL ( CSubConnectionProviderFactoryBase& aFactory );
	~CUPnPServiceDefaultSubConnectionProvider ( );
 	
 	void SetUdpServerFlow ( const TNodeId& aCommsId );
	void SetUdpClientFlow ( const TNodeId& aCommsId );
	TNodeId UdpClientFlow ( );
	TNodeId UdpServerFlow ( );
	const TDesC8& Key ( );	
	void SetPublishInfoL ( CUPnPPublishInfoElement* aPublishInfo );
	CUPnPPublishInfoElement* PublishInfo ( );
	const TDesC8& RootDeviceUrn ( );
	void SetRootDeviceUrnL ( const TDesC8& aDeviceType );
	const TDesC8& RootDeviceUsn ( );	
	CUPnPDescriptionComposer& DeviceDescriptionComposerL ( );	
	CUPnPServiceConnectionProvider& ConnectionProvider ( );	
	TBool RootDevicePublished ( );
	void SetRootDevicePublished ( TBool aValue );	
	TBool IsClosing ( );		
	void SetClosing ( );
		
private:
	CUPnPServiceDefaultSubConnectionProvider ( ESock::CSubConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap );
	void ConstructL ( );
    virtual void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );	
    
private:
	CUPnPDescriptionComposer* 	iDeviceDescriptionComposer;
	CUPnPPublishInfoElement*	iRootDevicePublishInfo;
	RBuf8						iRootDeviceUrn; // = urn:domain-name-device:deviceType:v
	RBuf8						iRootDeviceUsn; // = upnp:rootdevice::uuid:device-UUID
    TNodeId						iUdpClientFlow;
	TNodeId 					iUdpServerFlow;
	TBool						iRootDevicePublished;
	TBool						iClosing;
	};
	
	
inline TBool CUPnPServiceDefaultSubConnectionProvider::IsClosing ( )
	{
	return iClosing;
	}
		
inline void CUPnPServiceDefaultSubConnectionProvider::SetClosing ( )
	{
	iClosing = ETrue;
	}
	
inline TNodeId CUPnPServiceDefaultSubConnectionProvider::UdpClientFlow ( )
	{
	return iUdpClientFlow;
	}
	
inline void CUPnPServiceDefaultSubConnectionProvider::SetUdpClientFlow ( const TNodeId& aCommsId )
	{
	iUdpClientFlow = aCommsId;
	}
	
inline TNodeId CUPnPServiceDefaultSubConnectionProvider::UdpServerFlow ( )
	{
	return iUdpServerFlow;
	}
	
inline void CUPnPServiceDefaultSubConnectionProvider::SetUdpServerFlow ( const TNodeId& aCommsId )
	{
	iUdpServerFlow = aCommsId;
	}

inline const TDesC8& CUPnPServiceDefaultSubConnectionProvider::Key ( )
	{
	return iRootDevicePublishInfo->Key( );
	}

inline CUPnPPublishInfoElement* CUPnPServiceDefaultSubConnectionProvider::PublishInfo ( )
	{
	return iRootDevicePublishInfo;
	}
	
inline const TDesC8& CUPnPServiceDefaultSubConnectionProvider::RootDeviceUrn ( )
	{
	return iRootDeviceUrn;
	}

inline void CUPnPServiceDefaultSubConnectionProvider::SetRootDeviceUrnL ( const TDesC8& aDeviceUrn )
	{
	iRootDeviceUrn.Close ( );
	iRootDeviceUrn.CreateL ( aDeviceUrn );
	}
	
inline const TDesC8& CUPnPServiceDefaultSubConnectionProvider::RootDeviceUsn ( )
	{
	return iRootDeviceUsn;
	}

inline TBool CUPnPServiceDefaultSubConnectionProvider::RootDevicePublished ( )
	{
	return iRootDevicePublished;
	}

inline void CUPnPServiceDefaultSubConnectionProvider::SetRootDevicePublished ( TBool aValue )
	{
 	iRootDevicePublished = aValue;
	}	
	


#endif //UPNPSERVICEDEFAULTSCPR_H
