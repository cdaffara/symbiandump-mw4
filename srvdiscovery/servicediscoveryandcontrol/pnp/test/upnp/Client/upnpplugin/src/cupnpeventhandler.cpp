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

#include "cupnpeventhandler.h"
#include "pnputils.h"

CUPnPEventHandler::CUPnPEventHandler( RSubConnection& aSubConnection )
	: iSubConnection ( aSubConnection )
	{
	}

// Sends the eventing message to the network
void CUPnPEventHandler::SubmitRequestL( const RPnPParameterBundle& aBundle )
	{
	User::LeaveIfError(iSubConnection.SetParameters(aBundle));
	}
// Holds no resources
CUPnPEventHandler::~CUPnPEventHandler()
	{
	}
