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
// cupnpservicediscoveryimpl.cpp
// 
//

#include <connpref.h>
#include "cupnpservicediscoveryimpl.h"
#include <upnp/upnpconstants.h>

CUPnPServiceDiscoveryImpl::CUPnPServiceDiscoveryImpl ()
	{
	}
CUPnPServiceDiscoveryImpl* CUPnPServiceDiscoveryImpl::NewL()
	{
	CUPnPServiceDiscoveryImpl* self = new (ELeave) CUPnPServiceDiscoveryImpl;
	return self;
	}
CUPnPServiceDiscoveryImpl::~CUPnPServiceDiscoveryImpl ()
	{

	delete	iEventNotifier;
	delete 	iUPnPSearch;
	delete 	iUPnPDescribe;
	delete 	iUPnPSubscribe;
	delete 	iUPnPRegister;

	iHostResolver.Close();
	iSubConnection.Close();
	iConnection.Close();	
	iSocketServer.Close();
	}

// Explicitly start the connection
TInt CUPnPServiceDiscoveryImpl::Construct( TUint aTierId )
	{	
	TInt err;
	//Connect the Socket Server
	if((err = iSocketServer.Connect()) != KErrNone)
		return err;

	//const TUint KUPnPTierId = 0x2000D054;
	// Open the connection passing the socket server and the tier Id
	if((err = iConnection.Open( iSocketServer, aTierId )) != KErrNone)
		return err;

	//Start the connection using the given preferences
	TConnSnapPref prefs ( KDiscoveryConnPref );
	if((err = iConnection.Start( prefs )) != KErrNone)
		return err;

	//Create a new sub-connection on the started connection
	err = iSubConnection.Open(iSocketServer, RSubConnection::ECreateNew, iConnection);
	if ( err != KErrNone )
		return err;

	err = iHostResolver.Open( iSocketServer, KAfInet , KProtocolInetTcp);// , iConnection);
	if (err != KErrNone )
		return err;
	
	iEventNotifier = new CUPnPEventNotifier ( *this , iSubConnection );
	if( !iEventNotifier )
		return KErrNoMemory ;
	
	iEventNotifier->Notify();
	return KErrNone;
	}

// Sends the M-SEARCH request
void CUPnPServiceDiscoveryImpl::Discover ( const RPnPParameterBundle& aServiceInfo )
	{
	TRAPD( err, DiscoverL( aServiceInfo ));
	if(err != KErrNone )
		aServiceInfo.PnPObserver()->OnPnPError(err);
	}

void CUPnPServiceDiscoveryImpl::DiscoverL ( const RPnPParameterBundle& aServiceInfo )
	{
	if( !iUPnPSearch )
		iUPnPSearch = CUPnPSearch::NewL( iSubConnection );
	
	__ASSERT_DEBUG( iUPnPSearch->DiscoverInProgress()== EFalse,User::Panic(KUPnPDiscoveryInProgress,KUPnPErrDiscoveryInProgress) );
	iUPnPSearch->SubmitRequestL( aServiceInfo );
	}


void CUPnPServiceDiscoveryImpl::Describe(  const RPnPParameterBundle& aServiceInfo )
	{
	TRAPD( err, DescribeL( aServiceInfo ));
	if(err != KErrNone )
		aServiceInfo.PnPObserver()->OnPnPError(err);
	}
// Sends the Describe request
void CUPnPServiceDiscoveryImpl::DescribeL ( const RPnPParameterBundle& aServiceInfo )
	{
	if( !iUPnPDescribe )
		iUPnPDescribe = new (ELeave) CUPnPDescribe(iSubConnection ,iHostResolver);
	iUPnPDescribe->SubmitRequestL(aServiceInfo);
	}
//Sends the Subscribe request
void CUPnPServiceDiscoveryImpl::Subscribe ( const RPnPParameterBundle& aServiceInfo )
	{
	TRAPD( err, SubscribeL( aServiceInfo ));
	if(err != KErrNone )
		aServiceInfo.PnPObserver()->OnPnPError(err);
	}

void CUPnPServiceDiscoveryImpl::SubscribeL ( const RPnPParameterBundle& aServiceInfo )
	{
	if( !iUPnPSubscribe )
		iUPnPSubscribe = CUPnPSubscribe::NewL( iSubConnection,  iHostResolver );
	
	iUPnPSubscribe->SubmitRequestL( aServiceInfo );
	}

void CUPnPServiceDiscoveryImpl::RegisterNotify(  const RPnPParameterBundle& aServiceInfo )//done
	{
	TRAPD( err, RegisterNotifyL( aServiceInfo ));
	if(err != KErrNone )
		aServiceInfo.PnPObserver()->OnPnPError(err);
	}
// Used to register for advertisements of specific service type or ssdp:all
void CUPnPServiceDiscoveryImpl::RegisterNotifyL ( const RPnPParameterBundle& aServiceInfo )//done
	{
	if( !iUPnPRegister )
		iUPnPRegister = new (ELeave) CUPnPRegisterNotify(iSubConnection );
	iUPnPRegister->SubmitRequestL(aServiceInfo);
	}

/* Used to cancel previous requests of control point */
void CUPnPServiceDiscoveryImpl::Cancel ( const RPnPParameterBundle& aServiceInfo )//done
	{
	//Ignore the error.
	TRAP_IGNORE(ProcessCancelL(aServiceInfo));
	}

/* Used to call the NotifyResults method (which sends the responses back to the
  callback of the respective classes. In case , responses are coming for requests
  then they're ignored as they'll mostly be acknowlegements.
 @param afamilyId The family according to which NotifyResults of respective class
 				  is called
 */
void CUPnPServiceDiscoveryImpl::OnNotificationL (TUint32 afamilyId)
	{	
	RPnPParameterBundleBase pnpBundle;
	CleanupClosePushL(pnpBundle);
	// Get the subcon parameters
	User::LeaveIfError(iSubConnection.GetParameters(pnpBundle));

	// Send a clear request
	RPnPParameterBundleBase pBundle;
	User::LeaveIfError(pBundle.Open());
	CleanupClosePushL(pBundle);	
	RParameterFamily family = pBundle.CreateFamilyL(EUPnPClearParamSet);
	User::LeaveIfError ( iSubConnection.SetParameters(pBundle) );	
	CleanupStack::PopAndDestroy(&pBundle);
	CleanupStack::Pop(&pnpBundle);
	
	switch(afamilyId)
		{
		case EUPnPDiscoverResponseParamSet:
			iUPnPSearch->NotifyResultsL(pnpBundle);
		break;

		case EUPnPDescribeResponseParamSet:
			iUPnPDescribe->NotifyResultsL(pnpBundle);
		break;

		case EUPnPPresenceAnnouncementParamSet:
		case EUPnPAbsenceAnnouncementParamSet:
			iUPnPRegister->NotifyResultsL(afamilyId, pnpBundle);
		break;
		
		case EUPnPSubscribeResponseParamSet:
		case EUPnPNotifyEventParamSet:
			iUPnPSubscribe->NotifyResultsL(afamilyId, pnpBundle);
		break;
		
		default:
		__ASSERT_DEBUG(0, User::Panic(KUPnPInvalidFamily,KUPnPErrInvalidFamily));	
		break;
		}	
	}

TInt CUPnPServiceDiscoveryImpl::InitiateControl ( RControlChannel& /*aControlChannel*/, const TDesC8& aUri )
	{
	TInetAddr addr;
	TRAPD ( err, CUPnPUtils::ResolveHostAddressL ( iHostResolver, aUri, addr ) );
	
	if ( err == KErrNone )
		{
		TConnectionDetails connDetails ( &addr, aUri );
		//err = aControlChannel.Open ( iSocketServer, iSubConnection, KAfInet, EFalse, KProtocolInetTcp, connDetails );
		}
		
	return err;
	}

void CUPnPServiceDiscoveryImpl::ProcessCancelL( const RPnPParameterBundle& aServiceInfo)
	{
	RParameterFamily family=const_cast<RPnPParameterBundle&>(aServiceInfo).GetFamilyAtIndex(0);
	
	switch(family.Id())
		{
		case EUPnPCancelDiscoverParamSet:
		if( iUPnPSearch )
			iUPnPSearch->CancelSearchL( aServiceInfo );
		break;

		case EUPnPCancelDescribeParamSet:
		if ( iUPnPDescribe )
			iUPnPDescribe->CancelDescribeL( aServiceInfo );
		break;

		case EUPnPCancelRegisterNotifyParamSet:
		if( iUPnPRegister )
			iUPnPRegister->CancelNotifyAnnouncementL( aServiceInfo );
		break;

		case EUPnPCancelSubscribeParamSet:
		if ( iUPnPSubscribe )
			iUPnPSubscribe->CancelSubscribeL( aServiceInfo );
		break;

		default:
		__ASSERT_DEBUG(0, User::Panic(KUPnPInvalidFamily,KUPnPErrInvalidFamily));	
		break;
		}
	}
