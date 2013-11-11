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

#ifndef __CUPNPEVENTNOTIFIER_H_
#define __CUPNPEVENTNOTIFIER_H_
#include <e32base.h>
#include <es_sock.h>
#include <e32debug.h>
#include "mupnpeventnotification.h"
/*
 Active object which monitors the sub connection for incoming packets and passes
 it to the respective classes NotifyResults
 @internalComponent
 */
class CUPnPEventNotifier:public CActive
	{
public:
	CUPnPEventNotifier( MUPnPEventNotification& aEvent , RSubConnection& aSubConnection);
	void Notify();
	virtual void RunL();
	virtual TInt RunError( TInt aError );
	virtual void DoCancel();
	virtual ~CUPnPEventNotifier();
private:	
	TNotificationEventBuf iEventNotificationBuf;
	MUPnPEventNotification& iNotification;
	RSubConnection& iSubConnection;
	TBool iClearParams;
	};
#endif /*CUPNPEVENTNOTIFIER_H_*/
