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

#ifndef __CTESTPNPPSERVICEDISCOVERYIMPL_H__
#define __CTESTPNPPSERVICEDISCOVERYIMPL_H__


#include <pnp/cpnpservicediscoverybase.h>
#include <pnp/mpnpobserver.h>

//User includes
#include "testpnpconstants.h"
#include "testpnpparamset.h"


/* Body of RPnPServiceDiscovery class and calls the SubmitRequest and NotifyResults
   of respective classes
 */
class CTestPnPServiceDiscoveryImpl : public CPnPServiceDiscoveryBase
	{
public:
	static CTestPnPServiceDiscoveryImpl* NewL();
	~CTestPnPServiceDiscoveryImpl ();
		
private:
	// From CPnPServiceDiscoveryBase
	TInt Construct( TUint aTierId);
	void Discover ( const RPnPParameterBundle& aServiceInfo );
	void Describe ( const RPnPParameterBundle& aServiceInfo );
	void Subscribe ( const RPnPParameterBundle& aServiceInfo );
	void RegisterNotify ( const RPnPParameterBundle& aServiceInfo );
	void Cancel ( const RPnPParameterBundle& aServiceInfo );
	CControlChannelBase* InitiateControlL ( const TDesC8& aUri );

private:
	CTestPnPServiceDiscoveryImpl();
	void DiscoverL ( const RPnPParameterBundle& aServiceInfo );
	void RegisterNotifyL ( const RPnPParameterBundle& aServiceInfo );
	void SubscribeL ( const RPnPParameterBundle& aServiceInfo );
	void DescribeL ( const RPnPParameterBundle& aServiceInfo );		
	
	};

#endif // __CTESTPNPPSERVICEDISCOVERYIMPL_H__
