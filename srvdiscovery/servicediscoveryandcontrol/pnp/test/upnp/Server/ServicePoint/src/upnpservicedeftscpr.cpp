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

//System Includes
#include <comms-infras/ss_msgintercept.h>

#include "upnpservicedeftscpr.h"
#include "upnpservicedeftscpractivities.h"
#include "upnpserverconstants.h"

#if defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
#define KUPnPDefaultSCprTag KESockSubConnectionTag
#endif

using namespace ESock;


CUPnPServiceDefaultSubConnectionProvider* CUPnPServiceDefaultSubConnectionProvider::NewL ( CSubConnectionProviderFactoryBase& aFactory )
	{
	CUPnPServiceDefaultSubConnectionProvider* provider = new ( ELeave ) CUPnPServiceDefaultSubConnectionProvider ( aFactory, UPnPServiceDefaultActivities::activityMap::Self ( ) );
	CleanupStack::PushL ( provider );
	provider->ConstructL( );
	CleanupStack::Pop ( provider );
	return provider;
	}

CUPnPServiceDefaultSubConnectionProvider::~CUPnPServiceDefaultSubConnectionProvider ( )
	{
	LOG_NODE_DESTROY ( KUPnPDefaultSCprTag, CUPnPServiceDefaultSubConnectionProvider );
	iRootDeviceUrn.Close ( );
	delete iRootDevicePublishInfo;
	delete iDeviceDescriptionComposer;
	}

CUPnPServiceDefaultSubConnectionProvider::CUPnPServiceDefaultSubConnectionProvider ( CSubConnectionProviderFactoryBase& aFactory, const TNodeActivityMap& aActivityMap )
	: CUPnPSubConnectionProvider ( aFactory, aActivityMap ), iRootDevicePublished ( EFalse ), iClosing ( EFalse )
	{
	LOG_NODE_CREATE ( KUPnPDefaultSCprTag, CUPnPServiceDefaultSubConnectionProvider );
	}

void CUPnPServiceDefaultSubConnectionProvider::ConstructL ( )
	{
	CCoreSubConnectionProvider::ConstructL ( );
	}

void CUPnPServiceDefaultSubConnectionProvider::ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage )
	{
	ESOCK_DEBUG_MESSAGE_INTERCEPT ( aSender, aMessage, aRecipient );	
	TNodeContext<CUPnPServiceDefaultSubConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);	
    CCoreSubConnectionProvider::ReceivedL ( aSender, aRecipient, aMessage );
    User::LeaveIfError ( ctx.iReturn );
	}

void CUPnPServiceDefaultSubConnectionProvider::SetPublishInfoL ( CUPnPPublishInfoElement* aPublishInfo )
	{
	iRootDevicePublishInfo = aPublishInfo;
	
	// RootDevice Specific Publish Info -- upnp:rootdevice::uuid:device-UUID
	TInt size = aPublishInfo->Uuid( ).Size ( ) + KSeperator.iTypeLength + aPublishInfo->SearchTarget( ).Size ( );
	iRootDeviceUsn.CreateMaxL ( size );
	
	iRootDeviceUsn.Copy ( aPublishInfo->Uuid ( ) );
	iRootDeviceUsn.Append ( KSeperator ( ) );
	iRootDeviceUsn.Append ( aPublishInfo->SearchTarget ( ) );
	}

CUPnPDescriptionComposer& CUPnPServiceDefaultSubConnectionProvider::DeviceDescriptionComposerL ( )
	{
	if ( !iDeviceDescriptionComposer )
		{
		CStringPoolManager& stringpoolmgr =  ConnectionProvider().StringPoolManager();
		const RStringPool& pool = stringpoolmgr.StringPool();
		iDeviceDescriptionComposer = CUPnPDescriptionComposer::NewL ( pool );
		return *iDeviceDescriptionComposer;
		}
	else
		return *iDeviceDescriptionComposer;
	}

CUPnPServiceConnectionProvider& CUPnPServiceDefaultSubConnectionProvider::ConnectionProvider ( )
	{
	CUPnPConnectionProvider& cprBase = BaseConnectionProvider();
	return static_cast<CUPnPServiceConnectionProvider&>(cprBase);
	}

