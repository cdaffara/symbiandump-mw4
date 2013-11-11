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

#include <rpnpservicediscovery.h>
#include <upnp/upnpconstants.h>

EXPORT_C RPnPServiceDiscovery::RPnPServiceDiscovery( ):
	iUPnPServiceDiscoveryImpl ( NULL )
	{
	}  

EXPORT_C TInt RPnPServiceDiscovery::Open ( TUint aTierID )
	{
	if (!iUPnPServiceDiscoveryImpl )
		{
		TInt err;
		if (!iUPnPServiceDiscoveryImpl )
			{
			TRAP(err,iUPnPServiceDiscoveryImpl = CPnPServiceDiscoveryBase::NewL( aTierID ));
			if ( err != KErrNone )
				return err;
			}
		err = iUPnPServiceDiscoveryImpl->Construct( aTierID );
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
	__ASSERT_DEBUG( iUPnPServiceDiscoveryImpl, User::Panic( KUPnPHandleNotOpen, KUPnPErrNotOpen));
	__ASSERT_DEBUG(aServiceInfo.PnPObserver()!= NULL, User::Panic(KUPnPNoObserver,KUPnPErrNoObserver));

	__ASSERT_DEBUG(aServiceInfo.CountParameterFamilies() == 1, User::Panic(KUPnPInvalidFamilyCount,KUPnPErrInvalidFamilyCount));	
	TUint32 param=EUPnPDiscoverRequestParamSet;
	RParameterFamily family = const_cast<RPnPParameterBundle&>(aServiceInfo).FindFamily( param );		
	__ASSERT_DEBUG(!family.IsNull (), User::Panic(KUPnPInvalidFamily,KUPnPErrInvalidFamily)); 	
	TInt paramSetCount = family.CountParameterSets(RParameterFamily::ERequested);
	__ASSERT_DEBUG(family.CountParameterSets(RParameterFamily::ERequested) > 0, User::Panic(KUPnPNoParamSet,KUPnPErrNoParamSet));

	iUPnPServiceDiscoveryImpl->Discover ( aServiceInfo );
	}

EXPORT_C void RPnPServiceDiscovery::Describe ( const RPnPParameterBundle& aServiceInfo )
	{
	__ASSERT_ALWAYS( iUPnPServiceDiscoveryImpl, User::Panic( KUPnPHandleNotOpen,KUPnPErrNotOpen ) );
	__ASSERT_DEBUG(aServiceInfo.PnPObserver()!= NULL, User::Panic(KUPnPNoObserver,KUPnPErrNoObserver));
	
	__ASSERT_DEBUG(aServiceInfo.CountParameterFamilies() == 1, User::Panic(KUPnPInvalidFamilyCount,KUPnPErrInvalidFamilyCount));
	
	TUint32 param=EUPnPDescribeRequestParamSet;
	RParameterFamily family = const_cast<RPnPParameterBundle&>(aServiceInfo).FindFamily( param );
	__ASSERT_DEBUG(!family.IsNull (), User::Panic(KUPnPInvalidFamily,KUPnPErrInvalidFamily));	
	__ASSERT_DEBUG(family.CountParameterSets(RParameterFamily::ERequested) > 0, User::Panic(KUPnPNoParamSet,KUPnPErrNoParamSet));
	iUPnPServiceDiscoveryImpl->Describe ( aServiceInfo );
	}

EXPORT_C void RPnPServiceDiscovery::Subscribe ( const RPnPParameterBundle& aServiceInfo )
	{
	__ASSERT_ALWAYS( iUPnPServiceDiscoveryImpl, User::Panic( KUPnPHandleNotOpen,KUPnPErrNotOpen ) );
	__ASSERT_DEBUG(aServiceInfo.PnPObserver()!= NULL, User::Panic(KUPnPNoObserver,KUPnPErrNoObserver));
	__ASSERT_DEBUG(aServiceInfo.CountParameterFamilies() == 1, User::Panic(KUPnPInvalidFamilyCount,KUPnPErrInvalidFamilyCount));
	
	TUint32 param = EUPnPSubscribeRequestParamSet;
	RParameterFamily family = const_cast<RPnPParameterBundle&>(aServiceInfo).FindFamily( param );	
	__ASSERT_DEBUG(!family.IsNull (), User::Panic(KUPnPInvalidFamily,KUPnPErrInvalidFamily));
	__ASSERT_DEBUG(family.CountParameterSets(RParameterFamily::ERequested) > 0, User::Panic(KUPnPNoParamSet,KUPnPErrNoParamSet));
	iUPnPServiceDiscoveryImpl->Subscribe ( aServiceInfo );
	}

EXPORT_C void  RPnPServiceDiscovery::RegisterNotify ( const RPnPParameterBundle& aServiceInfo )
	{
	__ASSERT_ALWAYS( iUPnPServiceDiscoveryImpl, User::Panic( KUPnPHandleNotOpen, KUPnPErrNotOpen) );
	__ASSERT_DEBUG(aServiceInfo.PnPObserver()!= NULL, User::Panic(KUPnPNoObserver,KUPnPErrNoObserver));
	__ASSERT_DEBUG(aServiceInfo.CountParameterFamilies() == 1, User::Panic(KUPnPInvalidFamilyCount,KUPnPErrInvalidFamilyCount));
	
	TUint32 param = EUPnPRegisterAnnouncementParamSet;
	RParameterFamily family = const_cast<RPnPParameterBundle&>(aServiceInfo).FindFamily( param );
	__ASSERT_DEBUG(!family.IsNull (), User::Panic(KUPnPInvalidFamily,KUPnPErrInvalidFamily));
	__ASSERT_DEBUG(family.CountParameterSets(RParameterFamily::ERequested) > 0, User::Panic(KUPnPNoParamSet,KUPnPErrNoParamSet));
	iUPnPServiceDiscoveryImpl->RegisterNotify ( aServiceInfo );
	}


EXPORT_C void  RPnPServiceDiscovery::Cancel ( const RPnPParameterBundle& aServiceInfo )
	{
	__ASSERT_ALWAYS( iUPnPServiceDiscoveryImpl, User::Panic( KUPnPHandleNotOpen, KUPnPErrNotOpen) );
	__ASSERT_DEBUG(aServiceInfo.CountParameterFamilies() == 1, User::Panic(KUPnPInvalidFamilyCount,KUPnPErrInvalidFamilyCount));
	RParameterFamily family=const_cast<RPnPParameterBundle&>(aServiceInfo).GetFamilyAtIndex(0);
	
	__ASSERT_DEBUG(family.CountParameterSets(RParameterFamily::ERequested) > 0, User::Panic(KUPnPNoParamSet,KUPnPErrNoParamSet));
	iUPnPServiceDiscoveryImpl->Cancel ( aServiceInfo );
	}

EXPORT_C TInt RPnPServiceDiscovery::InitiateControl( RControlChannel& aControlChannel, const TDesC8& aUri)
	{
	__ASSERT_ALWAYS( iUPnPServiceDiscoveryImpl, User::Panic( KUPnPHandleNotOpen, KUPnPErrNotOpen) );
	return iUPnPServiceDiscoveryImpl->InitiateControl( aControlChannel, aUri);
	}

EXPORT_C void RPnPServiceDiscovery::Close ( )
	{
	delete	iUPnPServiceDiscoveryImpl;
	iUPnPServiceDiscoveryImpl = NULL;
	}


