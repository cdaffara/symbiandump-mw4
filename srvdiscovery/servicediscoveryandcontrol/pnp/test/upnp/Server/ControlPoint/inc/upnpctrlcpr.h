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

#ifndef __UPNPCONTROLCPR_H
#define __UPNPCONTROLCPR_H

#include <comms-infras/corecpr.h>
#include <comms-infras/ss_api_ext.h>
#include <comms-infras/ss_platsec_apiext.h>
#include "upnpcpr.h"


using namespace ESock;

class CUPnPControlConnectionProvider : 	public CUPnPConnectionProvider 
	{
public:
	static CUPnPControlConnectionProvider* NewL ( CConnectionProviderFactoryBase& aFactory );
	virtual ~CUPnPControlConnectionProvider ( );	
	
private:
	CUPnPControlConnectionProvider ( CConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap );
	void ConstructL ( );
    virtual void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
#ifdef SYMBIAN_NETWORKING_UPS
    TInt GetProcessAndThreadId ( TProcessId& aProcessId, TThreadId& aThreadId ) const;
#endif
			
	};
	
	
#endif // UPNPCONTROLCPR_H
