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

#ifndef __CUPNPPSERVICEPUBLISHERIMPL_H__
#define __CUPNPPSERVICEPUBLISHERIMPL_H__
#include <es_sock.h>
#include <connpref.h>
#include <upnp/upnpconstants.h>
#include <cpnpservicepublisherbase.h>

#include "cupnppublisher.h"
#include "cupnpeventnotifier.h"
#include "cupnpeventhandler.h"
#include "mupnpeventnotification.h"

//Connection Preference for RPnPPublisher
const TUint KPublishConnPref = 52;
const TInt KUPnPPublisherUid = 0x2001DC6A;
class CPnPParameterBundle;

/* Body of RPnPServicePublisher class and calls the SubmitRequest and NotifyResults
   of respective classes
   @internalComponent
 */
class CUPnPServicePublisherImpl :public CPnPServicePublisherBase , public MUPnPEventNotification 
	{
public:
	static CUPnPServicePublisherImpl* NewL();
	~CUPnPServicePublisherImpl ( );
		
private:
	TInt Construct ( TUint aTierId );
	void Publish ( const RPnPParameterBundle& aServiceInfo );
	void SendNotify ( const RPnPParameterBundle& aServiceInfo );
	TInt InitiateControl ( RControlChannel& /*aControlChannel*/ );


private:
	CUPnPServicePublisherImpl ( );
    void OnNotificationL(TUint32);

	void PublishL ( const RPnPParameterBundle& aServiceInfo );
	void SendNotifyL ( const RPnPParameterBundle& aServiceInfo );

private:
	RConnection 			iConnection;
	RSubConnection 			iSubConnection;
	CUPnPEventNotifier* 	iEventNotifier;
	CUPnPPublisher*			iUPnPPublisher;
	CUPnPEventHandler*		iUPnPEventHandler;
	RSocketServ 			iSocketServer;
	TBool					iAlreadyPublished;
	};

#endif // __CUPNPPSERVICEPUBLISHERIMPL_H__
