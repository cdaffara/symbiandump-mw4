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

#ifndef __UPNPCONTROLDEFAULTSCPR_H
#define __UPNPCONTROLDEFAULTSCPR_H

#include <comms-infras/corescpr.h>
#include "upnpscpr.h"

class CUPnPControlDefaultSubConnectionProvider : public CUPnPSubConnectionProvider
	{	
public:
	static CUPnPControlDefaultSubConnectionProvider* NewL(ESock::CSubConnectionProviderFactoryBase& aFactory);
	virtual ~CUPnPControlDefaultSubConnectionProvider(); 	
 	
protected:
	CUPnPControlDefaultSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap);
	void ConstructL();
	virtual void ReceivedL ( const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage );
	};
	
#endif //UPNPCONTROLDEFAULTSCPR_H
