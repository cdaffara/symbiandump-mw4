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

#include <rpnpservicepublisher.h>
#include <rcontrolchannel.h>
#include <upnp/upnpconstants.h>

EXPORT_C RPnPServicePublisher::RPnPServicePublisher( ):
	iUPnPServicePublisherImpl ( NULL )
	{
	}

EXPORT_C TInt RPnPServicePublisher::Open ( TUint aTierID )
	{
	if (!iUPnPServicePublisherImpl )
		{
		TInt err;
		if (!iUPnPServicePublisherImpl )
			{
			TRAP(err,iUPnPServicePublisherImpl = CPnPServicePublisherBase::NewL( aTierID ));
			if ( err != KErrNone )
				return err;
			}
		err = iUPnPServicePublisherImpl->Construct( aTierID );
		if ( err != KErrNone )
			{
			Close();
			}
		return err;
		}
	else
		return KErrAlreadyExists;
				
	}

EXPORT_C void RPnPServicePublisher::Close ( )
	{
	delete	iUPnPServicePublisherImpl;
	iUPnPServicePublisherImpl = NULL;
	}

EXPORT_C void RPnPServicePublisher::Publish ( const  RPnPParameterBundle& aServiceInfo )
	{
	
	__ASSERT_DEBUG( iUPnPServicePublisherImpl, User::Panic( KUPnPHandleNotOpen, KUPnPErrNotOpen ) );
	__ASSERT_DEBUG(aServiceInfo.PnPObserver()!= NULL, User::Panic(KUPnPNoObserver,KUPnPErrNoObserver));
	__ASSERT_DEBUG(aServiceInfo.CountParameterFamilies() == 1, User::Panic(KUPnPInvalidFamilyCount,KUPnPErrInvalidFamilyCount));
	
	RParameterFamily family = const_cast<RPnPParameterBundle&>(aServiceInfo).GetFamilyAtIndex(0);
	__ASSERT_DEBUG(family.CountParameterSets(RParameterFamily::ERequested) > 0, User::Panic(KUPnPNoParamSet,KUPnPErrNoParamSet));
	__ASSERT_DEBUG(family.CountParameterSets(RParameterFamily::ERequested) == 1, User::Panic(KUPnPMultiplePublish,KUPnPErrMultiplePublishParamSet));
	iUPnPServicePublisherImpl->Publish ( aServiceInfo );
	}

EXPORT_C void RPnPServicePublisher::SendNotify ( const  RPnPParameterBundle& aServiceInfo )
	{
	__ASSERT_DEBUG( iUPnPServicePublisherImpl, User::Panic( KUPnPHandleNotOpen, KUPnPErrNotOpen ) );
	__ASSERT_DEBUG(aServiceInfo.CountParameterFamilies() == 1, User::Panic(KUPnPInvalidFamilyCount,KUPnPErrInvalidFamilyCount));
	
	TUint32 param=EUPnPServiceNotificationParamSet;
	RParameterFamily family = const_cast<RPnPParameterBundle&>(aServiceInfo).FindFamily(param);	
	__ASSERT_DEBUG(!family.IsNull (), User::Panic(KUPnPInvalidFamily,KUPnPErrInvalidFamily));	
	__ASSERT_DEBUG(family.CountParameterSets(RParameterFamily::ERequested) > 0, User::Panic(KUPnPNoParamSet,KUPnPErrNoParamSet));
	
	iUPnPServicePublisherImpl->SendNotify ( aServiceInfo );
	}

EXPORT_C TInt RPnPServicePublisher::InitiateControl( RControlChannel& aControlChannel )
	{
	__ASSERT_DEBUG( iUPnPServicePublisherImpl, User::Panic( KUPnPHandleNotOpen, KUPnPErrNotOpen ) );	
	return iUPnPServicePublisherImpl->InitiateControl( aControlChannel );
	}

