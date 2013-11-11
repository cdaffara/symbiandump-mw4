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

#include <pnp/cpnpservicediscoverybase.h>
#include "rpnpservicediscovery.h"

EXPORT_C RPnPServiceDiscovery::RPnPServiceDiscovery( ):
	iPnPServiceDiscoveryImpl ( NULL )
	{
	}  

EXPORT_C TInt RPnPServiceDiscovery::Open ( TUint aTierID )
	{
	if (!iPnPServiceDiscoveryImpl )
		{
		TInt err = KErrNone;
		TRAP(err,iPnPServiceDiscoveryImpl = CPnPServiceDiscoveryBase::NewL( aTierID ));
		if ( err != KErrNone )
			return err;
			
		err = iPnPServiceDiscoveryImpl->Construct( aTierID );
		if ( err != KErrNone )
			{
			Close();
			}
		return err;
		}
	else
		return KErrAlreadyExists;
				
	}

EXPORT_C void  RPnPServiceDiscovery::Discover ( const RPnPParameterBundle& aServiceInfo )
	{	
	__ASSERT_ALWAYS( iPnPServiceDiscoveryImpl, User::Panic( KPnPHandleNotOpen, KErrPnPNotOpen) );
	iPnPServiceDiscoveryImpl->Discover ( aServiceInfo );
	}

EXPORT_C void RPnPServiceDiscovery::Describe ( const RPnPParameterBundle& aServiceInfo )
	{	
	__ASSERT_ALWAYS( iPnPServiceDiscoveryImpl, User::Panic( KPnPHandleNotOpen, KErrPnPNotOpen) );
	iPnPServiceDiscoveryImpl->Describe ( aServiceInfo );
	}

EXPORT_C void RPnPServiceDiscovery::Subscribe ( const RPnPParameterBundle& aServiceInfo )
	{
	__ASSERT_ALWAYS( iPnPServiceDiscoveryImpl, User::Panic( KPnPHandleNotOpen, KErrPnPNotOpen) );
	iPnPServiceDiscoveryImpl->Subscribe ( aServiceInfo );
	}

EXPORT_C void  RPnPServiceDiscovery::RegisterNotify ( const RPnPParameterBundle& aServiceInfo )
	{	
	__ASSERT_ALWAYS( iPnPServiceDiscoveryImpl, User::Panic( KPnPHandleNotOpen, KErrPnPNotOpen) );
	iPnPServiceDiscoveryImpl->RegisterNotify ( aServiceInfo );
	}


EXPORT_C void  RPnPServiceDiscovery::Cancel ( const RPnPParameterBundle& aServiceInfo )
	{
	__ASSERT_ALWAYS( iPnPServiceDiscoveryImpl, User::Panic( KPnPHandleNotOpen, KErrPnPNotOpen) );
	iPnPServiceDiscoveryImpl->Cancel ( aServiceInfo );
	}

EXPORT_C TInt RPnPServiceDiscovery::InitiateControl( RControlChannel& aControlChannel, const TDesC8& aUri)
	{
	__ASSERT_ALWAYS( iPnPServiceDiscoveryImpl, User::Panic( KPnPHandleNotOpen, KErrPnPNotOpen) );
    CControlChannelBase* channelBase = NULL;
	TRAPD (err , channelBase = iPnPServiceDiscoveryImpl->InitiateControlL(aUri));
    if ( err == KErrNone)
        {    	
    	aControlChannel.Attach(*channelBase);  
        }
    return err;	
	}

EXPORT_C void RPnPServiceDiscovery::Close ( )
	{
	delete	iPnPServiceDiscoveryImpl;
	iPnPServiceDiscoveryImpl = NULL;
	}


