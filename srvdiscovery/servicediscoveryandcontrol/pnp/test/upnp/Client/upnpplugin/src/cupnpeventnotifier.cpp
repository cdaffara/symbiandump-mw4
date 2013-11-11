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


#include <upnpparamset.h>
#include "upnpsubconnevents.h"
#include "cupnpeventnotifier.h"

CUPnPEventNotifier::CUPnPEventNotifier(MUPnPEventNotification& aEvent,RSubConnection& aSubConnection)
	: CActive( EPriorityStandard ),
	iNotification( aEvent ), iSubConnection ( aSubConnection ), iClearParams(EFalse)
	{
	CActiveScheduler::Add(this);
	}

/* Issues the active call for monitoring the subconnection */
void CUPnPEventNotifier::Notify()
	{
	if(IsActive())
		return ;
	
	// Issues the active request.The incoming data is stored in iEventNotificationBuf
	iSubConnection.EventNotification(iEventNotificationBuf, EFalse, iStatus);
	SetActive();
	}

/* When a packet arrives in the sub-connection then the familyId is extracted
 and calls OnNotification of CUPnPServiceDiscovery/CUPnPServicePublisher which calls
 NotifyResults of respective class
 */ 
void CUPnPEventNotifier::RunL()
	{
	User::LeaveIfError( iStatus.Int() );
	CSubConNotificationEvent* event = CSubConNotificationEvent::NewL(iEventNotificationBuf);
	CUPnPSubConnEvent* responseEvent = static_cast<CUPnPSubConnEvent*>(event);		
	CleanupStack::PushL (TCleanupItem(CUPnPSubConnEvent::CleanupSubConnEvent,responseEvent));
	TUint32 familyId = responseEvent->GetFamily();
	
	Notify ();
	
	iNotification.OnNotificationL(familyId);
	CleanupStack::PopAndDestroy(responseEvent);	
	}

/* Issues the asynchronous cancel request for event notification*/	
void CUPnPEventNotifier::DoCancel()
	{
	iSubConnection.CancelEventNotification();
	}
	
TInt CUPnPEventNotifier::RunError( TInt aError )
	{
	_LIT(KErrorLog, "SubConnection Notification Error %d");
	// upnplogger code to be added
	RDebug::Print( KErrorLog , aError );
	return KErrNone ;
	}

CUPnPEventNotifier::~CUPnPEventNotifier()
	{
	Cancel();
	}
