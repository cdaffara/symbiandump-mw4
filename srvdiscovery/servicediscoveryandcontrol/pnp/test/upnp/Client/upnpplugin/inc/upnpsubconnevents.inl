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

CUPnPSubConnEvent* CUPnPSubConnEvent::NewL()
	{
	STypeId typeId(KUPnPSubConnEventsUid, KUPnPSubConnEvent);
	return static_cast<CUPnPSubConnEvent*>(CSubConNotificationEvent::NewL(typeId));
	}
		
TUint32 CUPnPSubConnEvent::GetFamily() const
	{
	return iFamilyId;
	}

void CUPnPSubConnEvent::SetFamily(TUint32 aFamilyId)
	{
	iFamilyId = aFamilyId;
	}
	
void CUPnPSubConnEvent::CleanupSubConnEvent ( TAny* aAny )
	{
	CUPnPSubConnEvent* event = (CUPnPSubConnEvent*)aAny;
	delete event;
	}
	