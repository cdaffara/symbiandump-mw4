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

#include "cupnppublisher.h"
#include "pnputils.h"
#include <upnp/upnpconstants.h>

const TInt KStdTimeOutVal = 300;

CUPnPPublisher::CUPnPPublisher( RSubConnection& aSubConnection)
	//Constructor
	: iSubConnection ( aSubConnection )
	{
	}

CUPnPPublisher* CUPnPPublisher::NewL (RSubConnection& aSubConnection)
	{
	CUPnPPublisher* self = new (ELeave) CUPnPPublisher( aSubConnection );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CUPnPPublisher::ConstructL()
	// Constructs the periodic timer which is used for refreshment of
	// ssdp:alive
	{
	iPeriodTimer = CPeriodic::NewL( CActive::EPriorityStandard );
	}


CUPnPPublisher::~CUPnPPublisher()
	// Does all the associated clean up
	{
	iPeriodTimer->Cancel();
	delete iPeriodTimer;
	}

/* Sends an advertisement to the network.The validity of the uris is done.
 */
void CUPnPPublisher::SubmitRequestL( const RPnPParameterBundle& aServiceInfo )
// Sends a publish request to the network
	{
	_LIT8(KUuid,"uuid");
	iObserver = NULL;
	const TInt KLowerCacheLimit = 1800;
	const TInt KUpperCacheLimit = 6000;
	iObserver=aServiceInfo.PnPObserver();
	CUPnPServiceRegisterParamSet* serviceParamSet = NULL;
	CUPnPDeviceRegisterParamSet* deviceParamSet = NULL;
	
	RParameterFamily family = const_cast<RPnPParameterBundle&>(aServiceInfo).GetFamilyAtIndex(0);
	
	TTimeIntervalMicroSeconds32 period;
	TInt paramIndex = 0;
	switch(family.Id())
		{
		case EUPnPServiceRegisterParamSet:
			// If param set is of type Service Register
			{
			
			serviceParamSet = static_cast<CUPnPServiceRegisterParamSet*> ( family.GetParameterSetAtIndex ( paramIndex, RParameterFamily::ERequested ) );
			// The passed bundle will contain only one param set. Hence we don't need to
			// iterate through them			
			iService = ETrue;
			// Check validity of NT parameter for service
			User::LeaveIfError(CUPnPUtils::GenericUriValidity(serviceParamSet->Uri(),CUPnPUtils::EService));
			if ( serviceParamSet->Uri().Locate('#') != KErrNotFound )
				User::Leave(KErrCorrupt);
			/* Extract the Cache control time. If less than miminum specified value
			   assign it to minimum*/
			iTime = serviceParamSet->CacheControlData();
			if (iTime < KLowerCacheLimit || iTime > KUpperCacheLimit)
				iTime = KLowerCacheLimit;

			// Calculate the period according to given time in cache control.For
			// convenience its kept to 1/3rd of the cache time. However if the
			// period exceeds the max possible time in periodic timer then keep
			// it to lower period limit
			iPeriod = CalculatePeriod(iTime);
			period = iPeriod * KMicroTime;

			// Set the period timer
			TCallBack callBack(AdvertisementRefresh, this );
			//Change the value from 60000000 to period after.This is for integration testing
			iPeriodTimer->Start( period, period, callBack );
			
			//Validate the timeout parameter
			if(serviceParamSet->SubscriptionDuration() < KStdTimeOutVal)
				{
				serviceParamSet->SetSubscriptionDuration(KStdTimeOutVal); 
				}
		
			User::LeaveIfError(iSubConnection.SetParameters(aServiceInfo));
			break;
			}


		case EUPnPDeviceRegisterParamSet:
			// If param set is of type Device Register
			{
			deviceParamSet = static_cast<CUPnPDeviceRegisterParamSet*> ( family.GetParameterSetAtIndex ( paramIndex, RParameterFamily::ERequested ) );
			// The passed bundle will contain only one param set. Hence we don't need to
			// iterate through them			
			iService = EFalse;
			// Check validity of NT parameter
			// If uri is of form uuid:device-uid or urn:schemas-org....
			const TDesC8& ntUri = deviceParamSet->Uri();
			// This is to avoid a panic in case descriptor is not of minimum length
			if(ntUri.Length() < 5)
				User::Leave(KErrCorrupt);

			if( ntUri.CompareF(KUuid) )
				User::LeaveIfError(CUPnPUtils::GenericUriValidity ( ntUri , CUPnPUtils::EDevice));
			else if ( ntUri[4] != ':')
				User::Leave(KErrCorrupt);

			// Validate all the remaining fields
			ValidateOtherFieldsL( *deviceParamSet );
			
			/* Extract the Cache control time. If less than miminum specified value
			   assign it to minimum*/
			iTime = deviceParamSet->CacheControlData();
			if (iTime < KLowerCacheLimit || iTime > KUpperCacheLimit)
				iTime = KLowerCacheLimit;


			// Calculate the period according to given time in cache control.For
			// convenience its kept to 1/3rd of the cache time. However if the
			// period exceeds the max possible time in periodic timer then keep
			// it to lower period limit
			iPeriod = CalculatePeriod(iTime);
			period = iPeriod * KMicroTime;

			// Set the period timer
			TCallBack callBack(AdvertisementRefresh, this );
			iPeriodTimer->Start( period, period, callBack );
			
			RPnPParameterBundleBase bundle;
			User::LeaveIfError ( bundle.Open () );
			CleanupClosePushL ( bundle );			
			CopyWithIconInfoL ( *deviceParamSet, bundle );
			
			User::LeaveIfError(iSubConnection.SetParameters(bundle));
			CleanupStack::PopAndDestroy ( &bundle );
			break;
			}

		default:
			User::Leave(KErrCorrupt);
		} 	
	}

/* Used to notify results back to the client application */
void CUPnPPublisher::NotifyResultsL(RPnPParameterBundleBase& aBundle)
	{
	User::LeaveIfNull(iObserver);
	iObserver->OnPnPEventL(aBundle) ;
	}

// Callback function which sends a refresh packet everytime its called
TInt CUPnPPublisher::AdvertisementRefresh( TAny* aPtr )
	{
	RPnPParameterBundleBase pBundle ;
	pBundle.Open ();
	//Allocate the family on the bundle	
	RParameterFamily family;
	TRAPD(err, family = pBundle.CreateFamilyL(EUPnPServiceRefreshParamSet) );
	
	if( err != KErrNone )
		return err;
	
	CUPnPPublisher* publishObject = static_cast<CUPnPPublisher*>(aPtr);	
	// Finally send refresh request to the subconnection
	err = publishObject->SubConnection().SetParameters(pBundle );
	pBundle.Close();
	return err;	
	}

void CUPnPPublisher::ValidateOtherFieldsL( const CUPnPDeviceRegisterParamSet& aDeviceParam)
	{		
	//mandatory fields validation.
	__ASSERT_DEBUG(( aDeviceParam.FriendlyName().Length() > 0) && (aDeviceParam.FriendlyName().Length() < 64 ), User::Panic( KUPnPInvalidFriendlyName, KUPnPErrInvalidFriendlyName ) );
	__ASSERT_DEBUG(( aDeviceParam.Manufacturer().Length() > 0 ) && (aDeviceParam.Manufacturer().Length() < 64 ), User::Panic( KUPnPInvalidManufactureName, KUPnPErrInvalidManufactureName ) );
	__ASSERT_DEBUG(( aDeviceParam.ModelName().Length() > 0 ) && (aDeviceParam.ModelName().Length() < 32 ), User::Panic( KUPnPInvalidModelName, KUPnPErrInvalidModelName ) );
	
	// UDN validation 
	_LIT8(KUuid,"uuid:");
	__ASSERT_DEBUG( aDeviceParam.UDN().Length() > KUuid().Length(), User::Panic( KUPnPInvalidUDN, KUPnPErrInvalidUDN ) );
	TPtrC8 ptr;
	ptr.Set( aDeviceParam.UDN().Left( KUuid().Length() ) );
	if ( ptr.CompareF(KUuid) != 0 )
		{
		__ASSERT_DEBUG( 0, User::Panic( KUPnPInvalidUDN, KUPnPErrInvalidUDN ) );
		}
	
	//optional fields validation
	if ( aDeviceParam.ModelDesc() != KNullDesC8 )
		__ASSERT_DEBUG( aDeviceParam.ModelDesc().Length() < 128 , User::Panic( KUPnPInvalidModelDescription, KUPnPErrInvalidModelDescription ) );

	if ( aDeviceParam.ModelNumber() != KNullDesC8 )
		__ASSERT_DEBUG( aDeviceParam.ModelNumber().Length() < 32, User::Panic( KUPnPInvalidModelNumber, KUPnPErrInvalidModelNumber ) );
	
	if ( aDeviceParam.SerialNumber() != KNullDesC8 )
		__ASSERT_DEBUG( aDeviceParam.SerialNumber().Length() < 64, User::Panic( KUPnPInvalidSerialNumber, KUPnPErrInvalidSerialNumber ) );

	if (aDeviceParam.UPC() != KNullDesC8)
		__ASSERT_DEBUG( aDeviceParam.UPC().Length() == 12, User::Panic( KUPnPInvalidUPC, KUPnPErrInvalidUPC ) );
	
	ValidateIconList ( aDeviceParam );
	}
	
	
void CUPnPPublisher::ValidateIconList ( const  CUPnPDeviceRegisterParamSet& aDeviceParam )
	{
	const CUPnPIconList& iconList = aDeviceParam.IconList ();
	for ( TInt i = 0; i < iconList.Count (); i++ )
		{
		const TDesC8& mimeType = iconList[i]->MimeType ().Mid ( 0, 6 );		
		__ASSERT_DEBUG ( mimeType.CompareF ( _L8 ( "image/" ) ) == 0, User::Panic ( KUPnPInvalidIconInfo, KUPnPErrInvalidMimeType ) );
		}
	}

void CUPnPPublisher::CopyWithIconInfoL ( const CUPnPDeviceRegisterParamSet& aDeviceParam, RPnPParameterBundleBase& aBundle )
	{
	RParameterFamily family = aBundle.CreateFamilyL ( EUPnPDeviceRegisterParamSetExt );
	CUPnPDeviceRegisterParamSetExt* newParamSet = CUPnPDeviceRegisterParamSetExt::NewL ( family );
	newParamSet->SetUriL ( aDeviceParam.Uri () );
	newParamSet->SetUPCL ( aDeviceParam.UPC () );
	newParamSet->SetCacheControlData ( aDeviceParam.CacheControlData () );
	newParamSet->SetParentDeviceUidL ( aDeviceParam.ParentDeviceUid () );
	newParamSet->SetFriendlyNameL ( aDeviceParam.FriendlyName () );
	newParamSet->SetManufacturerL ( aDeviceParam.Manufacturer () );
	newParamSet->SetManufactureUrlL ( aDeviceParam.ManufacturerUrl () );
 	newParamSet->SetModelDescL ( aDeviceParam.ModelDesc () );
	newParamSet->SetModelNameL ( aDeviceParam.ModelName () );
 	newParamSet->SetModelNumberL ( aDeviceParam.ModelNumber () );
 	newParamSet->SetModelUriL ( aDeviceParam.ModelUri () );
 	newParamSet->SetPresentationL ( aDeviceParam.PresentationUri () );
 	newParamSet->SetSerialNumberL ( aDeviceParam.SerialNumber () );
 	newParamSet->SetUDNL ( aDeviceParam.UDN () );
	newParamSet->SetUPCL ( aDeviceParam.UPC () );
	
	RBuf8 iconData;
	CleanupClosePushL ( iconData );
	StoreIconListL ( aDeviceParam.IconList (), iconData );
	newParamSet->SetIconDataL ( iconData );
	CleanupStack::PopAndDestroy ( &iconData );
	}	

void CUPnPPublisher::StoreIconListL ( const CUPnPIconList& aIconList, RBuf8& aBuffer )
	{	
	CUPnPIconParams* iconInfo = NULL;	
	TInt length = sizeof ( TUint32 );	// icon list count	
	for ( TInt i = 0; i < aIconList.Count (); i++ )
		{
		iconInfo = aIconList[i];
		length  += ( iconInfo->MimeType ().Length () 
					+ 	iconInfo->Url ().Length ()			
					+ 	( sizeof ( TUint32 ) * 2 ) // length delimiters for url and mimetype
					+	( sizeof ( TUint32 ) * 3 ) ); // w,h,d	
		}	
	aBuffer.CreateL ( length );    
	aBuffer.AppendNumFixedWidth ( aIconList.Count(), EDecimal, sizeof ( TUint32 ) );
	
	// manual copy of all upnp icon params
	for ( TInt i = 0; i < aIconList.Count (); i++ )
		{
		iconInfo = aIconList[i];
				
		aBuffer.AppendNumFixedWidth ( iconInfo->MimeType ().Length (), EDecimal, sizeof ( TUint32 ) );
		aBuffer.Append ( iconInfo->MimeType () );
		
		aBuffer.AppendNumFixedWidth ( iconInfo->Url ().Length (), EDecimal, sizeof ( TUint32 ) );
		aBuffer.Append ( iconInfo->Url () );
		
		aBuffer.AppendNumFixedWidth ( iconInfo->Width (), EDecimal, sizeof ( TUint32 ) );
		aBuffer.AppendNumFixedWidth ( iconInfo->Height (), EDecimal, sizeof ( TUint32 ) );
		aBuffer.AppendNumFixedWidth ( iconInfo->Depth (), EDecimal, sizeof ( TUint32 ) );
		}	
	}
