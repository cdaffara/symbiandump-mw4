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
// CTestPnPServiceDiscoveryImpl.cpp
// 
//

#include "ctestpnpservicediscoveryimpl.h"
#include "ctestcontrolchannel.h"


CTestPnPServiceDiscoveryImpl::CTestPnPServiceDiscoveryImpl ()
	{
	}
	
		
CTestPnPServiceDiscoveryImpl* CTestPnPServiceDiscoveryImpl::NewL()
	{
	CTestPnPServiceDiscoveryImpl* self = new (ELeave) CTestPnPServiceDiscoveryImpl;
	return self;
	}
	
	
CTestPnPServiceDiscoveryImpl::~CTestPnPServiceDiscoveryImpl ()
	{

	}


TInt CTestPnPServiceDiscoveryImpl::Construct(TUint)
	{	
    return KErrNone;
	}


void CTestPnPServiceDiscoveryImpl::Discover ( const RPnPParameterBundle& aServiceInfo )
	{
	TRAPD( err, DiscoverL( aServiceInfo ));
	if(err != KErrNone )
		aServiceInfo.PnPObserver()->OnPnPError(err);
	}
	

void CTestPnPServiceDiscoveryImpl::DiscoverL ( const RPnPParameterBundle& aServiceInfo )
	{
    RPnPParameterBundle pnpBundle;
	pnpBundle.Open ();
	CleanupClosePushL( pnpBundle );
	RParameterFamily pnpFamily = pnpBundle.CreateFamilyL (ETestPnPDiscoverResponseParamSet);
	aServiceInfo.PnPObserver()->OnPnPEventL( pnpBundle );
	CleanupStack::PopAndDestroy(&pnpBundle);	
	}


void CTestPnPServiceDiscoveryImpl::Describe(  const RPnPParameterBundle& aServiceInfo )
	{
	TRAPD( err, DescribeL( aServiceInfo ));
	if(err != KErrNone )
		aServiceInfo.PnPObserver()->OnPnPError(err);
	}
	
// Sends the Describe request
void CTestPnPServiceDiscoveryImpl::DescribeL ( const RPnPParameterBundle& aServiceInfo)
	{
    RPnPParameterBundle pnpBundle;
	pnpBundle.Open ();
	CleanupClosePushL( pnpBundle );
	RParameterFamily pnpFamily = pnpBundle.CreateFamilyL (ETestPnPDescribeResponseParamSet);
	aServiceInfo.PnPObserver()->OnPnPEventL( pnpBundle );
	CleanupStack::PopAndDestroy(&pnpBundle);
	}
	
//Sends the Subscribe request
void CTestPnPServiceDiscoveryImpl::Subscribe ( const RPnPParameterBundle& aServiceInfo )
	{
	TRAPD( err, SubscribeL( aServiceInfo ));
	if(err != KErrNone )
		aServiceInfo.PnPObserver()->OnPnPError(err);
	}

void CTestPnPServiceDiscoveryImpl::SubscribeL ( const RPnPParameterBundle& aServiceInfo )
	{		
    RPnPParameterBundle pnpBundle;
	pnpBundle.Open ();
	CleanupClosePushL( pnpBundle );
	RParameterFamily pnpFamily = pnpBundle.CreateFamilyL (ETestPnPSubscribeResponseParamSet);
	aServiceInfo.PnPObserver()->OnPnPEventL( pnpBundle );
	CleanupStack::PopAndDestroy(&pnpBundle);
	}

void CTestPnPServiceDiscoveryImpl::RegisterNotify(  const RPnPParameterBundle& aServiceInfo )//done
	{
	TRAPD( err, RegisterNotifyL( aServiceInfo ));
	if(err != KErrNone )
		aServiceInfo.PnPObserver()->OnPnPError(err);
	}
	
	
// Used to register for advertisements of specific service type or ssdp:all
void CTestPnPServiceDiscoveryImpl::RegisterNotifyL ( const RPnPParameterBundle& aServiceInfo )//done
	{
	RPnPParameterBundle pnpBundle;
	pnpBundle.Open ();
	CleanupClosePushL( pnpBundle );
	RParameterFamily pnpFamily = pnpBundle.CreateFamilyL (ETestPnPPresenceAnnouncementParamSet);
	aServiceInfo.PnPObserver()->OnPnPEventL( pnpBundle );
	CleanupStack::PopAndDestroy(&pnpBundle);
	}


/* Used to cancel previous requests of control point */
void CTestPnPServiceDiscoveryImpl::Cancel ( const RPnPParameterBundle& aServiceInfo )//done
	{
	RPnPParameterBundle pnpBundle;
	pnpBundle.Open ();
	CleanupClosePushL( pnpBundle );
	RParameterFamily pnpFamily = pnpBundle.CreateFamilyL (ETestPnPCancelDiscoverParamSet);
	aServiceInfo.PnPObserver()->OnPnPEventL( pnpBundle );
	CleanupStack::PopAndDestroy(&pnpBundle);
	}


CControlChannelBase* CTestPnPServiceDiscoveryImpl::InitiateControlL ( const TDesC8& /*aUri*/ )
	{
	CTestControlChannel* ctrlChannel = CTestControlChannel::NewL();
    return  ctrlChannel;
	}

