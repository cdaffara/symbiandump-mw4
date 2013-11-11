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
// upnpsubconnevents.cpp
// 
//

#include <comms-infras/metadata.h>
#include <comms-infras/metatype.h>
#include "upnpsubconneventsfactory.h"
#include "upnpsubconnevents.h"


START_ATTRIBUTE_TABLE( CUPnPSubConnEvent, KUPnPSubConnEventsUid, KUPnPSubConnEvent )	
	REGISTER_ATTRIBUTE( CUPnPSubConnEvent, iFamilyId, TMetaNumber)	
END_ATTRIBUTE_TABLE()


CSubConNotificationEvent* CUPnPSubConnEventsFactory::NewL(TAny* aConstructionParameters)
	{
	TInt32 type = reinterpret_cast<TInt32>(aConstructionParameters);
	switch (type)
		{
		case (KUPnPSubConnEvent):
			return new (ELeave) CUPnPSubConnEvent;
		default:
			ASSERT(0);
			break;
		}		
	return NULL;
	}


