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

#include <f32file.h>
#include <inetprottextutils.h>

#include "upnpsppublishinfocontainer.h"
#include "upnpdescriptionschema.h"
#include "cupnpdescriptionparser.h"


CUPnPRootDeviceInfoContainer* CUPnPRootDeviceInfoContainer::NewL ( )
	{
	CUPnPRootDeviceInfoContainer* self = new ( ELeave ) CUPnPRootDeviceInfoContainer (  );
	CleanupStack::PushL ( self );
	self->ConstructL ( );
	CleanupStack::Pop ( self );
	return self;
	}

CUPnPRootDeviceInfoContainer::CUPnPRootDeviceInfoContainer ( )
	{
	}

CUPnPRootDeviceInfoContainer::~CUPnPRootDeviceInfoContainer ( )
	{
	iStringPoolMgr->Release( );
	delete iRootDeviceDescription;
	}

void CUPnPRootDeviceInfoContainer::ConstructL ( )
	{
	iRootDeviceDescription = CUPnPDeviceDescription::NewL ( );

	RFile	fileHandle;
	RFs		fs;
	User::LeaveIfError ( fs.Connect ( ) );
    CleanupClosePushL ( fs );
	
	TInt err = fileHandle.Open ( fs, _L("z:\\private\\101F7989\\upnp\\device.xml"), EFileShareReadersOnly );
	// ..remove Hard coded
	User::LeaveIfError ( err );
	CleanupClosePushL ( fileHandle );

    iStringPoolMgr = CStringPoolManager::NewL ( );
    const RStringPool& stringPool = iStringPoolMgr->StringPool();
    
	CUPnPDescriptionParser* parser = CUPnPDescriptionParser::NewL (stringPool, CUPnPDescriptionParser::EDevice);
    CleanupStack::PushL ( parser );

    TInt fileSize = 0;
	fileHandle.Size ( fileSize );

	HBufC8* buf = HBufC8::NewLC ( fileSize );
	TPtr8 ptr = buf->Des( );

	User::LeaveIfError ( fileHandle.Read ( ptr, fileSize ) );

    iRootDeviceDescription = static_cast<CUPnPDeviceDescription*> (parser->ParseDescriptionBufL( ptr ));

    CleanupStack::PopAndDestroy ( buf );
    CleanupStack::PopAndDestroy ( parser );
    CleanupStack::PopAndDestroy ( &fileHandle );
    CleanupStack::PopAndDestroy ( &fs );
	}


CUPnPDeviceDescription& CUPnPRootDeviceInfoContainer::GetRootDeviceDesciption ( ) const
	{
	return *iRootDeviceDescription;
	}

const TDesC8& CUPnPRootDeviceInfoContainer::GetRootdeviceUid() const
	{
	CUPnPDevice* device = iRootDeviceDescription->DeviceObject( );
	RStringPool& sp = iStringPoolMgr->StringPool( );
	
	return device->Property( sp.String ( UPNPDESCRIPTIONXMLTAGS::EUdn,iStringPoolMgr->GetUPnPTable() ) );
	}

CStringPoolManager& CUPnPRootDeviceInfoContainer::StringPoolManager ( ) const
	{
	return *iStringPoolMgr;
	}

TInt CUPnPRootDeviceInfoContainer::PerformValidation ( const TDesC8& aDeviceUid, const TDesC8& aServiceType )
	{
	RStringPool& sp = iStringPoolMgr->StringPool( );

	const CUPnPDevice* device = GetDeviceByUuid ( aDeviceUid );
	if ( device )
		{
		// device is present now look for service type;
		const CUPnPServiceInfo* servInfo = NULL;
		for ( TInt i = 0;  i < device->CountOfServiceInfoTable( ); i++ )
			{
			servInfo = device->AtServiceInfoTable( i );
			if ( aServiceType.Compare ( servInfo->Property( sp.String ( UPNPDESCRIPTIONXMLTAGS::EServiceType,iStringPoolMgr->GetUPnPTable()) ) ) == 0 )
				return KErrAlreadyExists;
			}
		}
	else
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

void CUPnPRootDeviceInfoContainer::AppendEmbeddedDeviceL ( const TDesC8& aParentUuid, CUPnPDevice* aDevice )
	{
	CUPnPDevice* parentDevice = GetDeviceByUuid ( aParentUuid );
	__ASSERT_DEBUG ( parentDevice != NULL, User::Invariant ( ) );  // Device to which service has to added is Not Found

	parentDevice->AppendToEmbeddedDeviceInfoTableL( aDevice );
	}

void CUPnPRootDeviceInfoContainer::AppendServiceInfoL ( const TDesC8& aParentUuid, CUPnPServiceInfo* aServiceInfo )
	{
	CUPnPDevice* device = iRootDeviceDescription->DeviceObject( );
	TBool res = AppendServiceL ( aParentUuid, aServiceInfo, device );

	__ASSERT_DEBUG ( res, User::Invariant ( ) );  // Device to which service has to added is Not Found
	}

TBool CUPnPRootDeviceInfoContainer::AppendServiceL ( const TDesC8& aParentUuid, const CUPnPServiceInfo* aServiceInfo, CUPnPDevice* aDevice )
	{
	RStringPool& sp = iStringPoolMgr->StringPool( );

	const TDesC8& uuid = aDevice->Property( sp.String ( UPNPDESCRIPTIONXMLTAGS::EUdn,iStringPoolMgr->GetUPnPTable() ) );
	if ( uuid.Compare ( aParentUuid ) == 0 )
		{
		aDevice->AppendToServiceInfoTableL( aServiceInfo );
		return ETrue;
		}
	else
		{
		for ( TInt i=0; i < aDevice->CountOfEmbeddedDeviceInfoTable( ); i++ )
			{
			if ( AppendServiceL ( aParentUuid, aServiceInfo, aDevice->AtEmbeddedDeviceInfoTable( i ) ) )
				return ETrue;
			}
		}

	return EFalse;
	}

void CUPnPRootDeviceInfoContainer::DeleteDeviceInfoL ( const TDesC8& aUuid )
	{
	CUPnPDevice* device = iRootDeviceDescription->DeviceObject( );
	TBool res = DeleteDeviceL ( aUuid, device );
	if ( res==EFalse )
		User::Leave ( KErrNotFound );
	}

TBool CUPnPRootDeviceInfoContainer::DeleteDeviceL ( const TDesC8& aUuid, CUPnPDevice* aDevice )
	{
	RStringPool& sp = iStringPoolMgr->StringPool( );

	for ( TInt i = 0; i < aDevice->CountOfEmbeddedDeviceInfoTable( ); i++)
		{
		if ( aUuid.Compare ( aDevice->AtEmbeddedDeviceInfoTable( i )->Property( sp.String ( UPNPDESCRIPTIONXMLTAGS::EUdn, iStringPoolMgr->GetUPnPTable() ) ) ) == 0 )
			{
			aDevice->DeleteEmbeddedDeviceInfoAtIndexL( i );
			return ETrue;
			}
		else
			{
			if ( DeleteDeviceL ( aUuid, aDevice->AtEmbeddedDeviceInfoTable( i ) ) == 1 )
				{
				return ETrue;
				}
			}
		}
	return EFalse;
	}

void CUPnPRootDeviceInfoContainer::DeleteServiceInfoL ( const TDesC8& aScpdUrl )
	{
	CUPnPDevice* device = iRootDeviceDescription->DeviceObject( );
	TBool res = DeleteServiceL ( aScpdUrl, device );
	if ( res == EFalse )
		User::Leave ( KErrNotFound );
	}

TBool CUPnPRootDeviceInfoContainer::DeleteServiceL ( const TDesC8& aScpdUrl, CUPnPDevice* aDevice )
	{
	RStringPool& sp = iStringPoolMgr->StringPool( );
    const CUPnPServiceInfo* serviceInfo = NULL;
    TPtrC8 urlPtr ( NULL, 0 );
    
    for ( TInt i = 0; i < aDevice->CountOfServiceInfoTable( ); i++ )
    	{
    	serviceInfo = aDevice->AtServiceInfoTable( i );
    	urlPtr.Set ( serviceInfo->Property( sp.String ( UPNPDESCRIPTIONXMLTAGS::EScpdUrl,iStringPoolMgr->GetUPnPTable() ) ).Mid (1) );
    	
    	if ( aScpdUrl.Compare ( urlPtr ) == 0 )
    		{
    		aDevice->DeleteServiceInfoAtIndexL( i );
    		return ETrue;
    		}
    	}

	for ( TInt i=0; i < aDevice->CountOfEmbeddedDeviceInfoTable( ); i++ )
		{
		if ( DeleteServiceL ( aScpdUrl, aDevice->AtEmbeddedDeviceInfoTable( i ) ) == 1 )
			return ETrue;
		}

	return EFalse;
	}


CUPnPDevice* CUPnPRootDeviceInfoContainer::GetDeviceByUuid ( const TDesC8& aDeviceUuid )
	{
	CUPnPDevice* device = iRootDeviceDescription->DeviceObject( );
	if ( SearchDevice ( device, aDeviceUuid ) )
		return device;
	else
		return NULL;
	}

const CUPnPDevice* CUPnPRootDeviceInfoContainer::SearchDevice ( const CUPnPDevice* aDevice, const TDesC8& aDeviceUuid )
	{
	RStringPool& sp = iStringPoolMgr->StringPool( );

	if ( aDeviceUuid.Compare ( aDevice->Property( sp.String ( UPNPDESCRIPTIONXMLTAGS::EUdn,iStringPoolMgr->GetUPnPTable() ) ) ) == 0 )
		{
		return aDevice;
		}
	else
		{
		for ( TInt i = 0; i < aDevice->CountOfEmbeddedDeviceInfoTable( ); i++ )
			{
			if ( SearchDevice ( aDevice->AtEmbeddedDeviceInfoTable( i ), aDeviceUuid ) != NULL )
				{
				return aDevice->AtEmbeddedDeviceInfoTable( i );
				}
			}
		}
	return NULL;
	}

