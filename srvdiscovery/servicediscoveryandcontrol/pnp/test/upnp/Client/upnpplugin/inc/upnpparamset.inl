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

CUPnPParamSetBase* CUPnPParamSetBase::NewL ( RParameterFamily& aFamily, TUPnPParamSetFamily aFamilyType )
	{
	STypeId typeId( KUPnPParamsFactoryUid, aFamilyType );
	CUPnPParamSetBase* self = static_cast < CUPnPParamSetBase* > ( CSubConParameterSet::NewL( typeId ) );
	TBool containerValue = aFamily.IsNull();
	if(!containerValue)
		{
		CleanupStack::PushL ( self );
		aFamily.AddParameterSetL ( self, RParameterFamily::ERequested );			
		CleanupStack::Pop ( self );
		}
	return self;	
	}
CUPnPParamSetBase::CUPnPParamSetBase ()
	{
	}

// ------------------------------------------
CUPnPRequestParamSetBase::CUPnPRequestParamSetBase ()
	{
	}

void CUPnPRequestParamSetBase::SetUriL ( const TDesC8& aUri )
	{
	iUri.Close ();
	iUri.CreateL ( aUri );
	}

const TDesC8& CUPnPRequestParamSetBase::Uri () const
	{
	return iUri;
	}

CUPnPRequestParamSetBase::~CUPnPRequestParamSetBase ()
	{
	iUri.Close ();
	}

// -------------------------------------------------------------------------
CUPnPResponseParamSetBase::CUPnPResponseParamSetBase ()
	{
	}

void CUPnPResponseParamSetBase::SetUPnPEvent ( const TUPnPEvent& aEvent )
	{
	iUPnPEvent = aEvent;
	}

const TUPnPEvent& CUPnPResponseParamSetBase::UPnPEvent () const
	{
	return iUPnPEvent;		
	}


// -------------------------------------------------------------------------
CUPnPDiscoverRequestParamSet* CUPnPDiscoverRequestParamSet::NewL ( RParameterFamily& aFamily )
 	{	 
 	return static_cast < CUPnPDiscoverRequestParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPDiscoverRequestParamSet ) );
 	}

void CUPnPDiscoverRequestParamSet::SetResponseTime(TInt aResponseTime)
	{
	iTimeToDelayResponse = aResponseTime;
	}

TInt CUPnPDiscoverRequestParamSet::ResponseTime( ) const
	{
	return iTimeToDelayResponse;
	}

// ----------------------------------------------------------------------------

CUPnPSubscribeRequestParamSet* CUPnPSubscribeRequestParamSet::NewL( RParameterFamily& aFamily )
	{
	return static_cast < CUPnPSubscribeRequestParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPSubscribeRequestParamSet ) );
	}

void CUPnPSubscribeRequestParamSet::SetDuration(TInt aDuration)
	{
	iTimeoutSeconds = aDuration;
	}


TInt CUPnPSubscribeRequestParamSet::Duration( ) const
	{
	return iTimeoutSeconds;
	}

void CUPnPSubscribeRequestParamSet::SetRemoteHost( const TInetAddr& aHost )
	{
	iRemoteHost.iAddr = aHost.Address();
	iRemoteHost.iPort = aHost.Port();
	}

const TAppProtAddr& CUPnPSubscribeRequestParamSet::RemoteHost( ) const
	{
	return iRemoteHost;
	}
	
// ----------------------------------------------------------------------------

CUPnPRegisterAnnouncementParamSet* CUPnPRegisterAnnouncementParamSet::NewL ( RParameterFamily& aFamily )
	{
	return static_cast < CUPnPRegisterAnnouncementParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPRegisterAnnouncementParamSet ) );
	}

void CUPnPRegisterAnnouncementParamSet::SetRegisterVariable(TBool aValue)
	{
	iRegister=aValue;
	}

TBool CUPnPRegisterAnnouncementParamSet::RegisterVariable()
	{
	return iRegister;
	}
// -----------------------------------------------------------------------------

CUPnPDescribeRequestParamSet* CUPnPDescribeRequestParamSet::NewL ( RParameterFamily& aFamily )
	{
	return static_cast < CUPnPDescribeRequestParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPDescribeRequestParamSet ) );
	}

void CUPnPDescribeRequestParamSet::SetRemoteHost( const TInetAddr& aHost )
	{	
	iHostAddr.iAddr = aHost.Address();
	iHostAddr.iPort = aHost.Port();
	}

const TAppProtAddr& CUPnPDescribeRequestParamSet::RemoteHost( ) const
	{
	return iHostAddr;
	}
	

// -----------------------------------------------------------------------------

CUPnPCancelDiscoverParamSet* CUPnPCancelDiscoverParamSet::NewL ( RParameterFamily& aFamily)
	{
	return static_cast < CUPnPCancelDiscoverParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPCancelDiscoverParamSet ) );
	}
// -----------------------------------------------------------------------------

CUPnPCancelDescribeParamSet* CUPnPCancelDescribeParamSet::NewL ( RParameterFamily& aFamily)
	{
	return static_cast < CUPnPCancelDescribeParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPCancelDescribeParamSet ) );
	}

// -----------------------------------------------------------------------------
CUPnPCancelSubscribeParamSet* CUPnPCancelSubscribeParamSet::NewL ( RParameterFamily& aFamily)
	{
	return static_cast < CUPnPCancelSubscribeParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPCancelSubscribeParamSet ) );
	}

// -----------------------------------------------------------------------------
CUPnPCancelRegisterNotifyParamSet* CUPnPCancelRegisterNotifyParamSet::NewL ( RParameterFamily& aFamily)
	{
	return static_cast < CUPnPCancelRegisterNotifyParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPCancelRegisterNotifyParamSet ) );
	}
// -----------------------------------------------------------------------------
CUPnPDiscoverResponseParamSet* CUPnPDiscoverResponseParamSet::NewL( RParameterFamily& aFamily )
	{
	return static_cast < CUPnPDiscoverResponseParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPDiscoverResponseParamSet ) );
	}

// D'tor
CUPnPDiscoverResponseParamSet::~CUPnPDiscoverResponseParamSet ()
	{
	iDeviceLocation.Close ();
	iUSN.Close ();
	}

void CUPnPDiscoverResponseParamSet::SetExpiryTime( TInt aExpiryTime )
	{
	iExpiration = aExpiryTime;
	}

void CUPnPDiscoverResponseParamSet::SetDeviceLocationL( const TDesC8& aDeviceLocation )
	{
	iDeviceLocation.Close ();
	iDeviceLocation.CreateL ( aDeviceLocation );
	}

void CUPnPDiscoverResponseParamSet::SetUSNL( const TDesC8& aUSN )
	{
	iUSN.Close ();
	iUSN.CreateL ( aUSN );
	}

TInt CUPnPDiscoverResponseParamSet::ExpiryTime( ) const
	{
	return iExpiration;
	}

const TDesC8& CUPnPDiscoverResponseParamSet::DeviceLocation( ) const
	{
	return iDeviceLocation;
	}

const TDesC8& CUPnPDiscoverResponseParamSet::USN( ) const
	{
	return iUSN;
	}

// -----------------------------------------------------------------------------
CUPnPDescribeResponseParamSet* CUPnPDescribeResponseParamSet::NewL( RParameterFamily& aFamily )
	{
	return static_cast < CUPnPDescribeResponseParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPDescribeResponseParamSet ) );
	}
CUPnPDescribeResponseParamSet::~CUPnPDescribeResponseParamSet ()
	{
	iDescription.Close ();
	}

void CUPnPDescribeResponseParamSet::SetDescriptionL( const TDesC8& aDescription )
	{
	iDescription.Close ();
	iDescription.CreateL ( aDescription );
	}

const TDesC8& CUPnPDescribeResponseParamSet::Description( ) const
	{
	return iDescription;
	}	

// -----------------------------------------------------------------------------
CUPnPSubscribeResponseParamSet* CUPnPSubscribeResponseParamSet::NewL( RParameterFamily& aFamily )
	{
	return static_cast < CUPnPSubscribeResponseParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPSubscribeResponseParamSet ) );
	}
TInt CUPnPSubscribeResponseParamSet::GetTimerExpiry() const
	{
	return iTimeoutSeconds;
	}

void CUPnPSubscribeResponseParamSet::SetTimerExpiry( TInt aExpiryTime )
	{
	iTimeoutSeconds = aExpiryTime;
	}

// -------------------------------------------------------------------------------------
CUPnPNotifyEventParamSet* CUPnPNotifyEventParamSet::NewL( RParameterFamily& aFamily )
	{
	return static_cast < CUPnPNotifyEventParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPNotifyEventParamSet ) );
	}
void CUPnPNotifyEventParamSet::SetNotificationL ( const TDesC8& aNotification )
	{
	iNotifyData.Close ();
	iNotifyData.CreateL ( aNotification );
	}

CUPnPNotifyEventParamSet::~CUPnPNotifyEventParamSet ()
	{
	iNotifyData.Close ();
	}


const TDesC8& CUPnPNotifyEventParamSet::Notification( ) const
	{
	return iNotifyData;
	}

// -------------------------------------------------------------------------------------
CUPnPAbsenceAnnouncementParamSet* CUPnPAbsenceAnnouncementParamSet::NewL( RParameterFamily& aFamily )
	{
	return static_cast < CUPnPAbsenceAnnouncementParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPAbsenceAnnouncementParamSet ) );
	}
void CUPnPAbsenceAnnouncementParamSet::SetUSNL(const TDesC8& aUSN)
	{
	iUSN.Close ();
	iUSN.CreateL ( aUSN );
	}

CUPnPAbsenceAnnouncementParamSet::~CUPnPAbsenceAnnouncementParamSet ()
	{
	iUSN.Close ();
	}

const TDesC8& CUPnPAbsenceAnnouncementParamSet::USN( ) const
	{
	return iUSN;
	}

// -------------------------------------------------------------------------------------

CUPnPPresenceAnnouncementParamSet* CUPnPPresenceAnnouncementParamSet::NewL( RParameterFamily& aFamily )
	{
	return static_cast < CUPnPPresenceAnnouncementParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPPresenceAnnouncementParamSet ) );
	}

CUPnPPresenceAnnouncementParamSet::~CUPnPPresenceAnnouncementParamSet ()
	{
	iLocation.Close ();
	}

void CUPnPPresenceAnnouncementParamSet::SetDuration( TInt aDuration )
	{
	iExpiration = aDuration;
	}

void CUPnPPresenceAnnouncementParamSet::SetServiceLocationL( const TDesC8& aServiceLocation )
	{
	iLocation.Close ();
	iLocation.CreateL ( aServiceLocation );
	}

TInt CUPnPPresenceAnnouncementParamSet::Duration( ) const
	{
	return iExpiration;
	}

const TDesC8& CUPnPPresenceAnnouncementParamSet::ServiceLocation( ) const 
	{
	return iLocation;
	}

// -------------------------------------------------------------------------------------

CUPnPServiceRegisterParamSet::~CUPnPServiceRegisterParamSet ()
	{
	iDeviceUid.Close ();
	iServiceDescription.Close ();
	iInitialEventMessage.Close();
	}

CUPnPServiceRegisterParamSet* CUPnPServiceRegisterParamSet::NewL( RParameterFamily& aFamily )
	{
	return static_cast < CUPnPServiceRegisterParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPServiceRegisterParamSet ) );
	}
void CUPnPServiceRegisterParamSet::SetDeviceUidL(const TDesC8& aUSN)
	{
	iDeviceUid.Close ();
	iDeviceUid.CreateL ( aUSN );
	}

void CUPnPServiceRegisterParamSet::SetCacheControlData(TInt aCacheControlData)
	{
	iCacheControl = aCacheControlData;
	}

void CUPnPServiceRegisterParamSet::SetServiceDescriptionL(const TDesC8& aServiceDescription)
	{
	iServiceDescription.Close ();
	iServiceDescription.CreateL ( aServiceDescription );
	}
void CUPnPServiceRegisterParamSet::SetInitialMessageL( const TDesC8& aInitialMessage)
	{
	iInitialEventMessage.Close();
	iInitialEventMessage.CreateL( aInitialMessage );
	}


void CUPnPServiceRegisterParamSet::SetSubscriptionDuration(TInt aTimeOut)
	{
	iTimeOut = aTimeOut;
	}


const TDesC8& CUPnPServiceRegisterParamSet::DeviceUid( ) const
	{
	return iDeviceUid;
	}


TInt CUPnPServiceRegisterParamSet::CacheControlData( ) const
	{
	return iCacheControl;
	}

const TDesC8& CUPnPServiceRegisterParamSet::ServiceDescription( ) const
	{
	return iServiceDescription;
	}

const TDesC8& CUPnPServiceRegisterParamSet::InitialMessage( ) const
	{
	return iInitialEventMessage;
	}

TInt CUPnPServiceRegisterParamSet::SubscriptionDuration() const
	{
	return iTimeOut;
	}
// ----------------------------------------------------------------------------

CUPnPIconParams* CUPnPIconParams::NewL ()
	{
	return new ( ELeave ) CUPnPIconParams;
	}
	
CUPnPIconParams::~CUPnPIconParams ()
	{
	iMimeType.Close ();
	iUrl.Close ();
	}
	
void CUPnPIconParams::SetMimeTypeL ( const TDesC8& aMimeType )
	{
	iMimeType.Close ();
	iMimeType.CreateL ( aMimeType );
	}

void CUPnPIconParams::SetUrlL ( const TDesC8& aUrl )
	{
	iUrl.Close ();
	iUrl.CreateL ( aUrl );
	}
	
void CUPnPIconParams::SetWidth ( TInt aWidth )
	{
	iWidth = aWidth;
	}
	
void CUPnPIconParams::SetHeight ( TInt aHeight )
	{
	iHeight = aHeight;
	}
	
void CUPnPIconParams::SetDepth ( TInt aDepth )
	{
	iDepth = aDepth;
	}
	
const TDesC8& CUPnPIconParams::MimeType () const
	{
	return iMimeType;
	}

const TDesC8& CUPnPIconParams::Url () const
	{
	return iUrl;
	}
	
TInt CUPnPIconParams::Width () const
	{
	return iWidth;
	}

TInt CUPnPIconParams::Height () const
	{
	return iHeight;
	}

TInt CUPnPIconParams::Depth () const
	{
	return iDepth;
	}

// ----------------------------------------------------------------------------

CUPnPDeviceRegisterParamSetBase::CUPnPDeviceRegisterParamSetBase ()
	{
	}

CUPnPDeviceRegisterParamSetBase::~CUPnPDeviceRegisterParamSetBase()
	{
	iParentDeviceUid.Close();
	iFriendlyName.Close();
	iManufacturer.Close();
	iManufacturerURL.Close();
	iModelDescription.Close();
	iModelName.Close();
	iModelNumber.Close();
	iModelURL.Close();
	iPresentationURL.Close();
	iSerialNumber.Close();
	iUDN.Close();
	iUPC.Close();
	}

void CUPnPDeviceRegisterParamSetBase::SetCacheControlData(TInt aCacheControlData)
	{
	iCacheControl = aCacheControlData;
	}

void CUPnPDeviceRegisterParamSetBase::SetParentDeviceUidL( const TDesC8& aParentDeviceUid )
	{
	iParentDeviceUid.Close();
	iParentDeviceUid.CreateL( aParentDeviceUid );
	}

void CUPnPDeviceRegisterParamSetBase::SetFriendlyNameL ( const TDesC8& aFriendlyName )
	{
	iFriendlyName.Close();
	iFriendlyName.CreateL( aFriendlyName );
	}

void CUPnPDeviceRegisterParamSetBase::SetManufacturerL ( const TDesC8& aManufacturer )
	{
	iManufacturer.Close();
	iManufacturer.CreateL( aManufacturer );
	}

void CUPnPDeviceRegisterParamSetBase::SetManufactureUrlL ( const TDesC8& aManUrl )
	{
	iManufacturerURL.Close();
	iManufacturerURL.CreateL( aManUrl );
	}

void CUPnPDeviceRegisterParamSetBase::SetModelDescL ( const TDesC8& aModelDesc )
	{
	iModelDescription.Close();
	iModelDescription.CreateL( aModelDesc );
	}

void CUPnPDeviceRegisterParamSetBase::SetModelNameL ( const TDesC8& aModelName )
	{
	iModelName.Close();
	iModelName.CreateL( aModelName );
	}

void CUPnPDeviceRegisterParamSetBase::SetModelNumberL ( const TDesC8& aModelNumber )
	{
	iModelNumber.Close();
	iModelNumber.CreateL( aModelNumber );
	}

void CUPnPDeviceRegisterParamSetBase::SetModelUriL ( const TDesC8& aModelUri )
	{
	iModelURL.Close();
	iModelURL.CreateL( aModelUri );
	}

void CUPnPDeviceRegisterParamSetBase::SetPresentationL ( const TDesC8& aPresentationUri )
	{
	iPresentationURL.Close();
	iPresentationURL.CreateL( aPresentationUri );
	}

void CUPnPDeviceRegisterParamSetBase::SetSerialNumberL ( const TDesC8& aSerialNumber )
	{
	iSerialNumber.Close();
	iSerialNumber.CreateL( aSerialNumber );
	}


void CUPnPDeviceRegisterParamSetBase::SetUDNL ( const TDesC8& aUri )
	{
	iUDN.Close();
	iUDN.CreateL( aUri );
	}

void CUPnPDeviceRegisterParamSetBase::SetUPCL ( const TDesC8& aUri )
	{
	iUPC.Close();
	iUPC.CreateL( aUri );
	}
	
TInt CUPnPDeviceRegisterParamSetBase::CacheControlData( ) const
	{
	return iCacheControl;
	}
	
const TDesC8& CUPnPDeviceRegisterParamSetBase::ParentDeviceUid() const
	{
	return iParentDeviceUid;
	}

const TDesC8& CUPnPDeviceRegisterParamSetBase::FriendlyName() const
	{
	return iFriendlyName;
	}
const TDesC8& CUPnPDeviceRegisterParamSetBase::Manufacturer() const
	{
	return iManufacturer;
	}

const TDesC8& CUPnPDeviceRegisterParamSetBase::ManufacturerUrl() const
	{
	return iManufacturerURL;
	}

const TDesC8& CUPnPDeviceRegisterParamSetBase::ModelDesc() const
	{
	return iModelDescription;
	}

const TDesC8& CUPnPDeviceRegisterParamSetBase::ModelName() const
	{
	return iModelName;
	}

const TDesC8& CUPnPDeviceRegisterParamSetBase::ModelNumber() const
	{
	return iModelNumber;
	}

const TDesC8& CUPnPDeviceRegisterParamSetBase::ModelUri() const
	{
	return iModelURL;
	}

const TDesC8& CUPnPDeviceRegisterParamSetBase::PresentationUri() const
	{
	return iPresentationURL;
	}

const TDesC8& CUPnPDeviceRegisterParamSetBase::SerialNumber() const
	{
	return iSerialNumber;
	}

const TDesC8& CUPnPDeviceRegisterParamSetBase::UDN() const
	{
	return iUDN;
	}

const TDesC8& CUPnPDeviceRegisterParamSetBase::UPC() const
	{
	return iUPC;
	}	

// ----------------------------------------------------------------------------
CUPnPDeviceRegisterParamSet* CUPnPDeviceRegisterParamSet::NewL(RParameterFamily& aFamily)
	{
	return static_cast < CUPnPDeviceRegisterParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPDeviceRegisterParamSet ) );
	}

CUPnPDeviceRegisterParamSet::~CUPnPDeviceRegisterParamSet()
	{
	iIconList.ResetAndDestroy();
	}

void CUPnPDeviceRegisterParamSet::SetIconInfoL ( CUPnPIconParams* aIconInfo )
	{
	iIconList.Append ( aIconInfo );
	}

const CUPnPIconList& CUPnPDeviceRegisterParamSet::IconList () const
	{
	return iIconList;
	}

// ----------------------------------------------------------------------------
CUPnPDeviceRegisterParamSetExt* CUPnPDeviceRegisterParamSetExt::NewL(RParameterFamily& aFamily)
	{
	return static_cast < CUPnPDeviceRegisterParamSetExt* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPDeviceRegisterParamSetExt ) );
	}

CUPnPDeviceRegisterParamSetExt::~CUPnPDeviceRegisterParamSetExt()
	{
	iIconData.Close ();
	}

void CUPnPDeviceRegisterParamSetExt::SetIconDataL ( const TDesC8& aIconData )
	{
	iIconData.Close ();
	iIconData.CreateL ( aIconData );
	}

const TDesC8& CUPnPDeviceRegisterParamSetExt::IconData () const
	{
	return iIconData;
	}

// ----------------------------------------------------------------------------
CUPnPServiceNotificationParamSet* CUPnPServiceNotificationParamSet::NewL(RParameterFamily& aFamily)
	{
	return static_cast < CUPnPServiceNotificationParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPServiceNotificationParamSet ) );
	}

void CUPnPServiceNotificationParamSet::SetNotificationL ( const TDesC8& aNotification )
	{
	iNotifyData.Close ();
	iNotifyData.CreateL ( aNotification );
	}

CUPnPServiceNotificationParamSet::~CUPnPServiceNotificationParamSet ()
	{
	iNotifyData.Close ();
	}


const TDesC8& CUPnPServiceNotificationParamSet::Notification( ) const
	{
	return iNotifyData;
	}

// -------------------------------------------------------------------------------------

CUPnPPublishResponseParamSet* CUPnPPublishResponseParamSet::NewL(RParameterFamily& aFamily)
	{
	return static_cast < CUPnPPublishResponseParamSet* > ( CUPnPParamSetBase::NewL( aFamily, EUPnPPublishResponseParamSet ) );
	}
