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

#include "upnpscpr.h"

using namespace Messages;

CUPnPSubConnectionProvider::CUPnPSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap)
: CCoreSubConnectionProvider(aFactory, aActivityMap)
	{
	
	}

CUPnPSubConnectionProvider::~CUPnPSubConnectionProvider()
	{
	
	}

CUPnPConnectionProvider& CUPnPSubConnectionProvider::BaseConnectionProvider ( )
	{
#if !defined(__GCCXML__)
	CConnectionProviderBase& cprBase = mnode_cast<CConnectionProviderBase>(ControlProvider()->RecipientId().Node());	
#else
	CConnectionProviderBase& cprBase =
		reinterpret_cast<CConnectionProviderBase&>(ControlProvider()->RecipientId().Node());		
#endif

	return static_cast<CUPnPConnectionProvider&>(cprBase);
	}


