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

#ifndef __CUPNPPSERVICEDISCOVERYIMPL_H__
#define __CUPNPPSERVICEDISCOVERYIMPL_H__
#include <es_sock.h>
#include <cpnpservicediscoverybase.h>

#include "mupnpeventnotification.h"
#include "cupnpeventnotifier.h"
#include "cupnpsearch.h"
#include "cupnpdescribe.h"
#include "cupnpsubscribe.h"
#include "cupnpregisternotify.h"


//Connection Preference for RPnPDiscovery
const TUint KDiscoveryConnPref = 51;
const TInt KUPnPDiscoveryUid = 0x2001DC69;

/* Body of RPnPServiceDiscovery class and calls the SubmitRequest and NotifyResults
   of respective classes
   @internalComponent
 */
class CUPnPServiceDiscoveryImpl : public CPnPServiceDiscoveryBase, public MUPnPEventNotification
	{
public:
	static CUPnPServiceDiscoveryImpl* NewL();
	~CUPnPServiceDiscoveryImpl ();
		
private:
	// From CPnPServiceDiscoveryBase
	TInt Construct( TUint aTierId);
	void Discover ( const RPnPParameterBundle& aServiceInfo );
	void Describe ( const RPnPParameterBundle& aServiceInfo );
	void Subscribe ( const RPnPParameterBundle& aServiceInfo );
	void RegisterNotify ( const RPnPParameterBundle& aServiceInfo );
	void Cancel ( const RPnPParameterBundle& aServiceInfo );
	TInt InitiateControl ( RControlChannel& /*aControlChannel*/, const TDesC8& aUri );

private:
	CUPnPServiceDiscoveryImpl ();

	// From MUPnPEventNotification
	void  OnNotificationL (TUint32);

	void DiscoverL ( const RPnPParameterBundle& aServiceInfo );
	void ProcessCancelL( const RPnPParameterBundle& aServiceInfo);
	void RegisterNotifyL ( const RPnPParameterBundle& aServiceInfo );
	void SubscribeL ( const RPnPParameterBundle& aServiceInfo );
	void DescribeL ( const RPnPParameterBundle& aServiceInfo );
	
private:
	RConnection 			iConnection;
	RSubConnection 			iSubConnection;
	RSocketServ 			iSocketServer;	
	RHostResolver			iHostResolver;

	CUPnPEventNotifier* 	iEventNotifier;
	CUPnPSearch* 			iUPnPSearch;
	CUPnPDescribe* 			iUPnPDescribe;
	CUPnPSubscribe* 		iUPnPSubscribe;
	CUPnPRegisterNotify* 	iUPnPRegister;
	};

#endif // __CUPNPPSERVICEDISCOVERYIMPL_H__
