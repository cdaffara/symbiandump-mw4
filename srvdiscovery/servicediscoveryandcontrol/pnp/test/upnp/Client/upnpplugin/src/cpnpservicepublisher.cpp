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
// cupnpservicepublisherimpl.cpp
// 
//


#include "cupnpservicepublisherimpl.h"
#include "pnputils.h"

CUPnPServicePublisherImpl::CUPnPServicePublisherImpl ()
	{
	}
CUPnPServicePublisherImpl* CUPnPServicePublisherImpl::NewL()
	{
	return new ( ELeave ) CUPnPServicePublisherImpl;
	}
CUPnPServicePublisherImpl::~CUPnPServicePublisherImpl ()
	{
	delete 	iEventNotifier;
	delete	iUPnPEventHandler;
	delete	iUPnPPublisher;	

	iSubConnection.Close();
	iConnection.Close();
	iSocketServer.Close();
	}

void CUPnPServicePublisherImpl::Publish ( const RPnPParameterBundle& aServiceInfo )
	{
	__ASSERT_DEBUG( !iUPnPPublisher, User::Panic( KUPnPAlreadyPublished, KUPnPErrPublish));
	TRAPD(err, PublishL( aServiceInfo ));
	if(err != KErrNone )
		aServiceInfo.PnPObserver()->OnPnPError(err);
	}

void CUPnPServicePublisherImpl::PublishL ( const RPnPParameterBundle& aServiceInfo )
	{
	iUPnPPublisher = CUPnPPublisher::NewL( iSubConnection );
	iUPnPPublisher->SubmitRequestL( aServiceInfo );
	}

void CUPnPServicePublisherImpl::SendNotify ( const RPnPParameterBundle& aServiceInfo )
	{
	__ASSERT_DEBUG( iUPnPPublisher, User::Panic(KUPnPServiceNotPublished,KUPnPErrPublish));
	__ASSERT_DEBUG( iUPnPPublisher->IsService(), User::Panic(KUPnPDeviceSendingNotification,KUPnPErrPublish));
	TRAPD( err, SendNotifyL( aServiceInfo ));
	if ( err != KErrNone )
		aServiceInfo.PnPObserver()->OnPnPError(err);
	}
void CUPnPServicePublisherImpl::SendNotifyL ( const RPnPParameterBundle& aServiceInfo )
	{
	if( !iUPnPEventHandler )
		iUPnPEventHandler = new (ELeave) CUPnPEventHandler(iSubConnection);
	iUPnPEventHandler->SubmitRequestL( aServiceInfo );
	}
// Explicitly start the connection
TInt CUPnPServicePublisherImpl::Construct(TUint aTierId )
	{
	TInt err;
	//Connect the Socket Server
	if((err = iSocketServer.Connect()) != KErrNone)
		return err;

	// Open the connection passing the socket server and the tier Id
	if((err = iConnection.Open( iSocketServer, aTierId )) != KErrNone)
		return err;

	//Start the connection using the given preferences
	TConnSnapPref prefs ( KPublishConnPref );
	if((err = iConnection.Start( prefs )) != KErrNone)
		return err;

	//Create a new sub-connection on the started connection
	err = iSubConnection.Open(iSocketServer, RSubConnection::ECreateNew, iConnection);
	if ( err != KErrNone )
		return err ;
	
	iEventNotifier = new CUPnPEventNotifier ( *this , iSubConnection );
	if( !iEventNotifier )
		return KErrNoMemory ;

	iEventNotifier->Notify();
	return KErrNone;
	}


// Invoke NotifyResults of respective classes
void CUPnPServicePublisherImpl::OnNotificationL (TUint32 aFamilyId)
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
	
	switch( aFamilyId )
		{
		case EUPnPPublishResponseParamSet:
			iUPnPPublisher->NotifyResultsL(pnpBundle);
		break;
		
		default:
		__ASSERT_DEBUG(0, User::Panic(KUPnPInvalidFamily,KUPnPErrInvalidFamily));	
		break;		
		}	
	}

TInt CUPnPServicePublisherImpl::InitiateControl ( RControlChannel& /*aControlChannel*/ )
	{
	TSockAddr sockAddr;
	TConnectionDetails connInfo ( &sockAddr, KNullDesC8 () ); // Note! dummy connect will happen in control channel
						// so it will be able to recv request packets, or else 
						// immediate recv call after socket open will fail with KErrNotReady
	//return aControlChannel.Open ( iSocketServer, iSubConnection, KAfInet, ETrue, KProtocolInetTcp, connInfo );
	return KErrNone;
	}


