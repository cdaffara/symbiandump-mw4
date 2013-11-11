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
//

#include "cpnpservicepublisherbase.h"
#include <ecom/ecomresolverparams.h>
#include <ecom/ecom.h>
#include <upnpconstants.h>

CPnPServicePublisherBase* CPnPServicePublisherBase::NewL( TUint aTierID )
	{
	_LIT8(KServer,"/Server");
	TBuf8<128> tierId;
	tierId.Num(aTierID);
	tierId.Append(KServer);
	// Set resolving parameters
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(tierId);
	return (REINTERPRET_CAST(CPnPServicePublisherBase*,
		REComSession::CreateImplementationL(KPnPInterfaceId,_FOFF(CPnPServicePublisherBase, iEcomDtorID),resolverParams)));
	}


EXPORT_C CPnPServicePublisherBase::CPnPServicePublisherBase()
	{
	}

EXPORT_C CPnPServicePublisherBase::~CPnPServicePublisherBase()
	{
	if (iEcomDtorID.iUid)
		{
		REComSession::DestroyedImplementation(iEcomDtorID);
		}
	}
