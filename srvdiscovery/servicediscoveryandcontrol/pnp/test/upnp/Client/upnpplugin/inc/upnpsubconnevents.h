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


#ifndef __UPNPSUBCONNEVENTS_H__
#define __UPNPSUBCONNEVENTS_H__

#include <es_sock.h>

// UID for UPnP Sub Connection Events Family
const TInt KUPnPSubConnEventsUid = 0x2000D05F;
const TUint32 KUPnPSubConnEvent =  0x1;

class CUPnPSubConnEvent : public CSubConNotificationEvent
	{
	public:
		inline static CUPnPSubConnEvent* NewL();
		inline TUint32 GetFamily() const;
		inline void SetFamily(TUint32 aFamily);	
		inline static void CleanupSubConnEvent( TAny* aAny );
	private:			
		TUint32 iFamilyId;
	protected:
		DATA_VTABLE
	};

#include "upnpsubconnevents.inl"

#endif // __UPNPSUBCONNEVENTS_H__
