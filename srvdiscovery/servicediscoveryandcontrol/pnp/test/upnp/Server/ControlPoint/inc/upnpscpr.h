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

#ifndef __UPNPSCPR_H_
#define __UPNPSCPR_H_

#include <comms-infras/corescpr.h>
#include "upnpcpr.h"

class CUPnPSubConnectionProvider : public CCoreSubConnectionProvider
	{
public:
	
	virtual ~CUPnPSubConnectionProvider();
	CUPnPConnectionProvider& BaseConnectionProvider ( );
protected:
	CUPnPSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap);
	
	};


#endif /*__UPNPSCPR_H_*/
