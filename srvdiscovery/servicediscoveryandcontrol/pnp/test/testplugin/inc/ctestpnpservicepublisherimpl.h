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

#ifndef __CTESTPNPPSERVICEPUBLISHERIMPL_H__
#define __CTESTPNPPSERVICEPUBLISHERIMPL_H__


#include <pnp/cpnpservicepublisherbase.h>
#include <pnp/mpnpobserver.h>

//User includes
#include "testpnpconstants.h"
#include "testpnpparamset.h"


/* Body of RPnPServicePublisher class and calls the SubmitRequest and NotifyResults
   of respective classes
 */
class CTestPnPServicePublisherImpl :public CPnPServicePublisherBase 
	{
public:
	static CTestPnPServicePublisherImpl* NewL();
	~CTestPnPServicePublisherImpl ( );
		
private:
	TInt Construct ( TUint aTierId );
	void Publish ( const RPnPParameterBundle& aServiceInfo );
	void SendNotify ( const RPnPParameterBundle& aServiceInfo );
	CControlChannelBase* InitiateControlL ();


private:
	CTestPnPServicePublisherImpl ( );
	void PublishL ( const RPnPParameterBundle& aServiceInfo );
	void SendNotifyL ( const RPnPParameterBundle& aServiceInfo );

	};

#endif // __CTESTPNPPSERVICEPUBLISHERIMPL_H__
