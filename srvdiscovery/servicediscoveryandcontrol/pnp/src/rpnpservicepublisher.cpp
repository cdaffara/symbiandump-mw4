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

#include <pnp/cpnpservicepublisherbase.h>
#include "rpnpservicepublisher.h"


EXPORT_C RPnPServicePublisher::RPnPServicePublisher( ):
	iPnPServicePublisherImpl ( NULL )
	{
	}

EXPORT_C TInt RPnPServicePublisher::Open ( TUint aTierID )
	{
	if (!iPnPServicePublisherImpl )
		{
		TInt err = KErrNone;
		TRAP(err,iPnPServicePublisherImpl = CPnPServicePublisherBase::NewL( aTierID ));
		if ( err != KErrNone )
			return err;
			
		err = iPnPServicePublisherImpl->Construct( aTierID );
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
	delete	iPnPServicePublisherImpl;
	iPnPServicePublisherImpl = NULL;
	}

EXPORT_C void RPnPServicePublisher::Publish ( const  RPnPParameterBundle& aServiceInfo )
	{	
	__ASSERT_ALWAYS( iPnPServicePublisherImpl, User::Panic( KPnPHandleNotOpen, KErrPnPNotOpen) );
	iPnPServicePublisherImpl->Publish ( aServiceInfo );
	}

EXPORT_C void RPnPServicePublisher::SendNotify ( const  RPnPParameterBundle& aServiceInfo )
	{
	__ASSERT_ALWAYS( iPnPServicePublisherImpl, User::Panic( KPnPHandleNotOpen, KErrPnPNotOpen) );
	iPnPServicePublisherImpl->SendNotify ( aServiceInfo );
	}

EXPORT_C TInt RPnPServicePublisher::InitiateControl( RControlChannel& aControlChannel )
	{
	__ASSERT_ALWAYS( iPnPServicePublisherImpl, User::Panic( KPnPHandleNotOpen, KErrPnPNotOpen) );
    CControlChannelBase* channelBase = NULL;
	TRAPD (err , channelBase = iPnPServicePublisherImpl->InitiateControlL());
    if ( err == KErrNone)
        {    	
    	aControlChannel.Attach(*channelBase);  
        }
    return err;		
	}

