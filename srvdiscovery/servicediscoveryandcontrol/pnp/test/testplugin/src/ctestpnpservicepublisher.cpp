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
// ctestpnpservicepublisherimpl.cpp
// 
//


#include "ctestpnpservicepublisherimpl.h"
#include "ctestcontrolchannel.h"



CTestPnPServicePublisherImpl::CTestPnPServicePublisherImpl ()
	{
	}

	
CTestPnPServicePublisherImpl* CTestPnPServicePublisherImpl::NewL()
	{
	return new ( ELeave ) CTestPnPServicePublisherImpl;
	}
	
CTestPnPServicePublisherImpl::~CTestPnPServicePublisherImpl ()
	{
	}


TInt CTestPnPServicePublisherImpl::Construct(TUint)
	{
	return KErrNone;
	}


void CTestPnPServicePublisherImpl::Publish ( const RPnPParameterBundle& aServiceInfo )
	{
	TRAPD(err, PublishL( aServiceInfo ));
	if(err != KErrNone )
		aServiceInfo.PnPObserver()->OnPnPError(err);
	}

void CTestPnPServicePublisherImpl::PublishL ( const RPnPParameterBundle& aServiceInfo )
	{
	RPnPParameterBundle pnpBundle;
	pnpBundle.Open ();
	CleanupClosePushL( pnpBundle );
	RParameterFamily pnpFamily = pnpBundle.CreateFamilyL (ETestPnPPublishResponseParamSet);
	aServiceInfo.PnPObserver()->OnPnPEventL( pnpBundle );
	CleanupStack::PopAndDestroy(&pnpBundle);
	}


void CTestPnPServicePublisherImpl::SendNotify ( const RPnPParameterBundle& aServiceInfo )
	{
	TRAPD( err, SendNotifyL( aServiceInfo ));
	if ( err != KErrNone )
		aServiceInfo.PnPObserver()->OnPnPError(err);
	}
	
	
void CTestPnPServicePublisherImpl::SendNotifyL ( const RPnPParameterBundle& /*aServiceInfo*/ )
	{
	RPnPParameterBundle pnpBundle;
	pnpBundle.Open ();
	CleanupClosePushL( pnpBundle );
	CleanupStack::PopAndDestroy(&pnpBundle);
	}
	

CControlChannelBase* CTestPnPServicePublisherImpl::InitiateControlL ()
	{
	CTestControlChannel* ctrlChannel = CTestControlChannel::NewL();
    return  ctrlChannel;
	}


