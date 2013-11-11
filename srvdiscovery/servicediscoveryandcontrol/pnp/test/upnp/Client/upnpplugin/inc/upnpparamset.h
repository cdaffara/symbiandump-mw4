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

#ifndef __UPNPPARAMSET_H__
#define __UPNPPARAMSET_H__

#include <es_sock.h>
#include <pnpextparameterfamily.h>
#include <appprotintf/app_protintf_msgs.h>

const TInt KUPnPParamsFactoryUid = 0x2000D05E;

/**
UPnP ParameterSet Family Identifiers.
@publishedPartner
@prototype
*/
enum TUPnPParamSetFamily
	{
	// Used by Control Point
	EUPnPRegisterAnnouncementParamSet 	= 1,
	EUPnPAbsenceAnnouncementParamSet 	= 2,
	EUPnPPresenceAnnouncementParamSet	= 3,
	EUPnPDiscoverRequestParamSet		= 4,
	EUPnPDiscoverResponseParamSet		= 5,
	//Do not use the 6 as it is used by family which carries implementation UID of SCPR.
	EUPnPDescribeRequestParamSet		= 7,
	EUPnPDescribeResponseParamSet		= 8,
	EUPnPSubscribeRequestParamSet		= 9,
	EUPnPSubscribeResponseParamSet		= 10,
	EUPnPNotifyEventParamSet			= 11,
	EUPnPCancelDiscoverParamSet			= 12,
	EUPnPCancelDescribeParamSet			= 13,
	EUPnPCancelSubscribeParamSet		= 14,
	EUPnPCancelRegisterNotifyParamSet	= 15,
	EUPnPRequestParamSet				= 16,
	EUPnPResponseParamSet				= 17,

	// Used by Service Point
	EUPnPServiceRegisterParamSet		= 18,
	EUPnPDeviceRegisterParamSet			= 19,
	EUPnPServiceNotificationParamSet	= 20,
	EUPnPServiceRefreshParamSet			= 21,
	EUPnPPublishResponseParamSet		= 22,

	//@internalTechnology
	//@prototype
	//Used by UPnP Server to clear responses handled to UPnP Client with GetParameters
	EUPnPClearParamSet,
	EUPnPDeviceRegisterParamSetBase,
	EUPnPDeviceRegisterParamSetExt,
	};

/**
class TUPnPEvent
UPnPEvent class defines the reason code for an event callback.
@publishedPartner
@prototype
*/
NONSHARABLE_CLASS(TUPnPEvent)
	{
public:
	/** Type of UPnP Event. */
	enum TEventType
	{
		/** This signifies that the operation completed successfully. */
		ESuccess,
		/** This is a 'catch-all' for communicating failures. The actual error reason is
			shown in error code. */
		EFail,
		/** This signifies a discovery search timeout */
		EDiscoveryTimeout,
		/** This signifies a subscription request timeout */
		ESubscriptionTimeout
	};

public:
	/** The event status. */
	TInt iStatus;
	/** The error code value. */
	TInt iErrorCode;
	};




/**
The base class for all UPnP ParamSet classes.

@internalTechnology
@prototype
*/
NONSHARABLE_CLASS (CUPnPParamSetBase) : public CSubConExtensionParameterSet
	{
	public:		
		inline static CUPnPParamSetBase* NewL ( RParameterFamily& aFamily, TUPnPParamSetFamily aFamilyType );
	protected:
		inline CUPnPParamSetBase ();
	};


// -------------------------------------------------------------------

/**
Parameter set class containing a Uri. All UPnP Request ParamSet classes derive from this class.

@internalTechnology
@prototype
*/
NONSHARABLE_CLASS ( CUPnPRequestParamSetBase ) : public CUPnPParamSetBase
	{
	protected:
		RBuf8 iUri;
	public:
		inline void SetUriL ( const TDesC8& aUri );
		inline const TDesC8& Uri () const;
	protected:
		inline CUPnPRequestParamSetBase ();
		inline ~CUPnPRequestParamSetBase ();
	protected:
		DATA_VTABLE
	};

// ----------------------------------------------------------------------

/**
Parameter set class containing a UPnP Event. All UPnP Response ParamSet classes derive from this class.

@internalTechnology
@prototype 
*/
NONSHARABLE_CLASS ( CUPnPResponseParamSetBase ) : public CUPnPRequestParamSetBase
	{
	protected:
		TUPnPEvent iUPnPEvent;
	public:
		inline void SetUPnPEvent ( const TUPnPEvent& aEvent );
		inline const TUPnPEvent& UPnPEvent () const;
	protected:
		inline CUPnPResponseParamSetBase ();
	protected:
		DATA_VTABLE
	};

// -------------------------------------------------------------------------

/** UPnP Control Point ParameterSet classes. */

/**
Parameter set class for discovering services/devices.
Calling client should set the ServiceType/DeviceType and maximum wait time.

@publishedPartner
@prototype
*/
NONSHARABLE_CLASS(CUPnPDiscoverRequestParamSet) : public CUPnPRequestParamSetBase
	{
	private:
		TInt iTimeToDelayResponse;
	public :
		inline static CUPnPDiscoverRequestParamSet* NewL( RParameterFamily& aFamily );
		inline void SetResponseTime(TInt aResponseTime);
		inline TInt ResponseTime( ) const ;
	protected:
		DATA_VTABLE
	};

// ---------------------------------------------------------------------------

/**
Parameter set class for subscribing to remote service. Client should set the ServiceLocation and Duration
of subscription.

@publishedPartner
@prototype
*/
NONSHARABLE_CLASS(CUPnPSubscribeRequestParamSet) : public CUPnPRequestParamSetBase
	{
protected:
	TInt iTimeoutSeconds;
	TAppProtAddr iRemoteHost;

public:
	inline static CUPnPSubscribeRequestParamSet* NewL( RParameterFamily& aFamily );
	inline void SetDuration(TInt aDuration);
	inline TInt Duration( ) const ;

	//@internalTechnology
	//@prototype
	inline void SetRemoteHost( const TInetAddr& aHost );
	//@internalTechnology
	//@prototype 
	inline const TAppProtAddr& RemoteHost ( ) const;
protected:
		DATA_VTABLE
	};

// ----------------------------------------------------------------------------

/**
Parameter set class for registering SSDP:Alive request of a service/device. Caller should send the
ServiceType/DeviceType to register for announcements.

@publishedPartner
@prototype 
*/
NONSHARABLE_CLASS(CUPnPRegisterAnnouncementParamSet) : public CUPnPRequestParamSetBase
	{
	public:
		TBool iRegister;
		inline static CUPnPRegisterAnnouncementParamSet* NewL ( RParameterFamily& aFamily );
		inline void SetRegisterVariable(TBool aValue);
		inline TBool RegisterVariable();
	protected:
		DATA_VTABLE
	};

// -----------------------------------------------------------------------------

/**
Parameter set class for describe request. Caller should set the ServiceType/DeviceType to be described.

@publishedPartner
@prototype
*/
NONSHARABLE_CLASS(CUPnPDescribeRequestParamSet) : public CUPnPRequestParamSetBase
	{
	private:
		TAppProtAddr iHostAddr;

	public:
		inline static CUPnPDescribeRequestParamSet* NewL ( RParameterFamily& aFamily );

		//@internalTechnology
		//@prototype
		inline void SetRemoteHost( const TInetAddr& aHost );
		//@internalTechnology
		//@prototype
		inline const TAppProtAddr& RemoteHost ( ) const;

	protected:
		DATA_VTABLE
	};
// -----------------------------------------------------------------------------
/**
Parameter set class for cancel discover request.
@publishedPartner
@prototype
*/

NONSHARABLE_CLASS(CUPnPCancelDiscoverParamSet) : public CUPnPRequestParamSetBase
	{
	public:		
		inline static CUPnPCancelDiscoverParamSet* NewL( RParameterFamily& aFamily );

	protected:
		DATA_VTABLE
	};

// -----------------------------------------------------------------------------
/**
Parameter set class for cancel describe request.
@publishedPartner
@prototype
*/

NONSHARABLE_CLASS(CUPnPCancelDescribeParamSet) : public CUPnPRequestParamSetBase
	{
	public:
		inline static CUPnPCancelDescribeParamSet* NewL( RParameterFamily& aFamily );

	protected:
		DATA_VTABLE
	};

// -----------------------------------------------------------------------------
/**
Parameter set class for cancel subscribe request.
@publishedPartner
@prototype
*/

NONSHARABLE_CLASS(CUPnPCancelSubscribeParamSet) : public CUPnPRequestParamSetBase
	{
	public:
		inline static CUPnPCancelSubscribeParamSet* NewL( RParameterFamily& aFamily );

	protected:
		DATA_VTABLE
	};

// -----------------------------------------------------------------------------
/**
Parameter set class for cancel register notify request.
@publishedPartner
@prototype
*/
NONSHARABLE_CLASS(CUPnPCancelRegisterNotifyParamSet) : public CUPnPRequestParamSetBase
	{
	public:
		inline static CUPnPCancelRegisterNotifyParamSet* NewL( RParameterFamily& aFamily );

	protected:
		DATA_VTABLE
	};

// -----------------------------------------------------------------------------

/**
Parameter set class for retrieving the responses of discovery requests. Client can extract remote device
information such as Device Location, Unique Service Name (USN) and Max age of the advertisement in
callback function.

@publishedPartner
@prototype
*/
NONSHARABLE_CLASS(CUPnPDiscoverResponseParamSet) : public CUPnPResponseParamSetBase
	{
private:
	TInt iExpiration;
	RBuf8 iDeviceLocation;
	RBuf8 iUSN;
public :

	inline TInt ExpiryTime ( ) const ;
	inline const TDesC8& DeviceLocation ( ) const ;
	inline const TDesC8& USN ( ) const ;

	// Internal APIs
	inline static CUPnPDiscoverResponseParamSet* NewL( RParameterFamily& aFamily );
	inline void SetExpiryTime( TInt aExpiryTime );
	inline void SetDeviceLocationL( const TDesC8& aDeviceLocation );
	inline void SetUSNL( const TDesC8& aUSN );

	// D'tor
	inline ~CUPnPDiscoverResponseParamSet ();
protected:
		DATA_VTABLE
	};

// -------------------------------------------------------------------------------------

/**
Parameter set class for retrieving responses of describe requests. Client can extract the device description
in callback function.

@publishedPartner
@prototype 
*/
NONSHARABLE_CLASS(CUPnPDescribeResponseParamSet) : public CUPnPResponseParamSetBase
	{
private:
	RBuf8 iDescription;
public:
	inline const TDesC8& Description( ) const ;
	inline static CUPnPDescribeResponseParamSet* NewL( RParameterFamily& aFamily );
	inline void SetDescriptionL( const TDesC8& aDescription );
	inline ~CUPnPDescribeResponseParamSet ();
protected:
		DATA_VTABLE
	};
// -------------------------------------------------------------------------------------

/**
Parameter set class for retrieving response of subscription request. Calling client can extract the
subscription status in callback function.

@publishedPartner
@prototype
*/
NONSHARABLE_CLASS(CUPnPSubscribeResponseParamSet) : public CUPnPResponseParamSetBase
	{
private:
	TInt iTimeoutSeconds;
public:
	inline TInt GetTimerExpiry() const;
	inline void SetTimerExpiry(TInt aExpiryTime);
	inline static CUPnPSubscribeResponseParamSet* NewL( RParameterFamily& aFamily );
protected:
		DATA_VTABLE
	};
// -------------------------------------------------------------------------------------

/**
Parameter set class for retrieving the event notification. Calling client can extract the
notification information in callback function. If notification order is not matched an error code
is returned to the client.

@publishedPartner
@prototype
*/
NONSHARABLE_CLASS(CUPnPNotifyEventParamSet) : public CUPnPResponseParamSetBase
	{
private:
	RBuf8 iNotifyData;
public:
	inline const TDesC8& Notification( ) const ;
	inline static CUPnPNotifyEventParamSet* NewL( RParameterFamily& aFamily );
	inline void SetNotificationL ( const TDesC8& aNotification );
	inline ~CUPnPNotifyEventParamSet ();
protected:
		DATA_VTABLE
	};
// -------------------------------------------------------------------------------------

/**
Parameter set class for retrieving the SSDP:ByeBye message. Calling client can extract the
published information in callback function.

@publishedPartner
@prototype
*/
NONSHARABLE_CLASS(CUPnPAbsenceAnnouncementParamSet) : public CUPnPResponseParamSetBase
	{
protected:
	RBuf8 iUSN;
public:
	inline const TDesC8& USN( ) const ;
	inline static CUPnPAbsenceAnnouncementParamSet* NewL( RParameterFamily& aFamily );
	inline void SetUSNL(const TDesC8& aUSN);

	// D'tor
	inline ~CUPnPAbsenceAnnouncementParamSet ();
protected:
		DATA_VTABLE
	};
// -------------------------------------------------------------------------------------

/**
Parameter set class for retrieving SSDP:Alive message. Calling client can extract the
published information in callback function.

@publishedPartner
@prototype
*/
NONSHARABLE_CLASS(CUPnPPresenceAnnouncementParamSet) : public CUPnPAbsenceAnnouncementParamSet
	{
private:
	TInt iExpiration;
	RBuf8 iLocation;
public:
	inline TInt Duration( ) const ;
	inline const TDesC8& ServiceLocation( ) const ;
	// Internal APIs
	inline static CUPnPPresenceAnnouncementParamSet* NewL( RParameterFamily& aFamily );
	inline void SetDuration( TInt aDuration );
	inline void SetServiceLocationL( const TDesC8& aServiceLocation );

	// D'tor
	inline ~CUPnPPresenceAnnouncementParamSet ();
protected:
		DATA_VTABLE
	};
// -------------------------------------------------------------------------------------

/** UPnP Service Point ParameterSet classes. */

/**
Parameter set class for registering the service and publishing the service information. Service Client
should set the following fields :

CUPnPRequestParamSetBase::SetUriL 	ServiceType
SetDeviceUidL						Root device uid
SetCacheControlData					Max age of notification
SetServiceDescriptionL				Scdp url (url for description)
SetInitialMessageL 					Initial Eventing message ( This message is the same as the initial eventing message which is sent along with subscribe response
										See UPnP Device Architecture 1.0 )

Refer to UPnP Device Architecture 1.0

@publishedPartner
@prototype
*/
NONSHARABLE_CLASS( CUPnPServiceRegisterParamSet ) : public CUPnPRequestParamSetBase
	{

	private:
		RBuf8 iDeviceUid;
		TInt  iCacheControl;
		RBuf8 iServiceDescription;
		RBuf8 iInitialEventMessage;
		TInt iTimeOut;

	public:
		inline ~CUPnPServiceRegisterParamSet ();
		inline static CUPnPServiceRegisterParamSet* NewL(RParameterFamily& aFamily);
		inline void SetDeviceUidL(const TDesC8& aUSN);
		inline void SetCacheControlData(TInt aCacheControlData);
		inline void SetServiceDescriptionL(const TDesC8& aServiceDescription);
		inline void SetInitialMessageL(const TDesC8& aInitialMessage);
		inline void SetSubscriptionDuration(TInt aTimeOut);

		inline const TDesC8& DeviceUid( ) const ;
		inline TInt CacheControlData( ) const ;
		inline const TDesC8& ServiceDescription( ) const ;
		inline const TDesC8& InitialMessage() const;
		inline TInt SubscriptionDuration( ) const;

	protected:
		DATA_VTABLE
	};

/** Parameter class to register Icon for embedded device.User should set the required parameters
 SetMimeTypeL   Format of the Image
 SetUrlL		Url to the Icon (Relative to Base Url)
 SetWidth		Horizontal pixels
 SetHeight		Vertical pixels
 SetDepth		Color Depth
 
 Refer to UPnP Device Architecture 1.0
@publishedPartner
@prototype
*/
class CUPnPIconParams: public CBase
	{
public:
	inline static CUPnPIconParams* NewL();
	inline ~CUPnPIconParams ();
	
	inline void SetMimeTypeL ( const TDesC8& aMimeType );
	inline void SetUrlL ( const TDesC8& aUrl );
	inline void SetWidth ( TInt  aWidth );
	inline void SetHeight ( TInt aHeight );
	inline void SetDepth ( TInt aDepth );
	
	inline const TDesC8& MimeType () const;
	inline const TDesC8& Url () const;
	inline TInt Width () const;
	inline TInt Height () const;
	inline TInt Depth () const;
	
private:
	RBuf8	iMimeType;
	RBuf8 	iUrl;
	TInt	iWidth;
	TInt	iHeight;
	TInt	iDepth;
	};

typedef RPointerArray < CUPnPIconParams > CUPnPIconList;	

/** Parameter set base class for registering the device.User should set the required parameters
 SetCacheControlData   	Time for which notification is valid
 SetParentDeviceUidL	If the device is an embedded device then its parent device should be specified here
 SetFriendlyNameL		Friendly name ( length < 64 chars)
 SetManufacturerL		Manufacturers name ( length < 64 chars)
 SetManufactureUrlL		Manufacturer url(optional)
 SetModelDescL			Short model description (optional ,length < 128 chars)
 SetModelNameL			Model name ( length < 32 chars)
 SetModelNumberL		Optional (length < 32 chars)
 SetModelUriL			Web site for model ( optional )
 SetPresentationL		This is the url of the  GUI of the service point if any provided
 SetSerialNumberL		Serial Number (optional, length < 64 chars)
 SetUDNL				Unique device name
 SetUPCL				Universal product code ( optional, 12 digits)

 Refer to UPnP Device Architecture 1.0
@publishedPartner
@prototype
*/
NONSHARABLE_CLASS( CUPnPDeviceRegisterParamSetBase ) : public CUPnPRequestParamSetBase
	{

protected:
	TInt  iCacheControl;
	RBuf8 iParentDeviceUid;
	RBuf8 iFriendlyName;
	RBuf8 iManufacturer;
	RBuf8 iManufacturerURL;
	RBuf8 iModelDescription;
	RBuf8 iModelName;
	RBuf8 iModelNumber;
	RBuf8 iModelURL;
	RBuf8 iPresentationURL;
	RBuf8 iSerialNumber;
	RBuf8 iUDN;
	RBuf8 iUPC;	

protected:
	inline CUPnPDeviceRegisterParamSetBase ();
	inline ~CUPnPDeviceRegisterParamSetBase ();

public:
	inline void SetCacheControlData (TInt aCacheControlData);
	inline void SetParentDeviceUidL ( const TDesC8& aParentDeviceUid );
	inline void SetFriendlyNameL ( const TDesC8& aFriendlyName );
	inline void SetManufacturerL ( const TDesC8& aManufacturer );
	inline void SetManufactureUrlL ( const TDesC8& aManUrl );
	inline void SetModelDescL ( const TDesC8& aModelDesc );
	inline void SetModelNameL ( const TDesC8& aModelName );
	inline void SetModelNumberL ( const TDesC8& aModelNumber );
	inline void SetModelUriL ( const TDesC8& aModelUri );
	inline void SetPresentationL ( const TDesC8& aPresentationUri );
	inline void SetSerialNumberL ( const TDesC8& aSerialNumber );
	inline void SetUDNL ( const TDesC8& aUri );
	inline void SetUPCL ( const TDesC8& aUri );	

	inline TInt CacheControlData( ) const ;
	inline const TDesC8& ParentDeviceUid() const;
	inline const TDesC8& FriendlyName() const;
	inline const TDesC8& Manufacturer() const;
	inline const TDesC8& ManufacturerUrl() const;
	inline const TDesC8& ModelDesc() const;
	inline const TDesC8& ModelName() const;
	inline const TDesC8& ModelNumber() const;
	inline const TDesC8& ModelUri() const;
	inline const TDesC8& PresentationUri() const;
	inline const TDesC8& SerialNumber() const;
	inline const TDesC8& UDN() const;
	inline const TDesC8& UPC() const;	
		
protected:
	DATA_VTABLE
	};

/** Parameter set class for registering the device.User should set the required parameters
 	SetIconInfoL	Icons related to the embedded device
 Refer to UPnP Device Architecture 1.0
@publishedPartner
@prototype
*/
NONSHARABLE_CLASS( CUPnPDeviceRegisterParamSet ) : public CUPnPDeviceRegisterParamSetBase
	{
private:
	CUPnPIconList iIconList;
	
public:	
	inline static CUPnPDeviceRegisterParamSet* NewL(RParameterFamily& aFamily);	
	inline ~CUPnPDeviceRegisterParamSet();
		
	inline void SetIconInfoL ( CUPnPIconParams* aIconInfo );
	inline const CUPnPIconList& IconList () const;

protected:
	DATA_VTABLE	
	};

/**
Parameter set class for sending event notifications.

@publishedPartner
@prototype
*/
NONSHARABLE_CLASS(CUPnPServiceNotificationParamSet) : public CUPnPRequestParamSetBase
	{
private:
	RBuf8 iNotifyData;
public:
	inline const TDesC8& Notification( ) const ;
	inline static CUPnPServiceNotificationParamSet* NewL( RParameterFamily& aFamily );
	inline void SetNotificationL( const TDesC8& aNotification );
	inline ~CUPnPServiceNotificationParamSet ();
protected:
		DATA_VTABLE
	};

/** Parameter set class for receiving success or failure for service or device
  registration

 @publishedPartner
@prototype
*/
NONSHARABLE_CLASS(CUPnPPublishResponseParamSet) : public CUPnPResponseParamSetBase
	{
public:
	inline static CUPnPPublishResponseParamSet* NewL( RParameterFamily& aFamily);
protected:
	DATA_VTABLE

	};

// -------------------------------------------------------------------------------------



// @internalTechnology
// @prototype
// Used by UPnP Client/Server serailize Icon List
NONSHARABLE_CLASS( CUPnPDeviceRegisterParamSetExt ) : public CUPnPDeviceRegisterParamSetBase
	{
private:
	RBuf8 iIconData;	

public:
	inline ~CUPnPDeviceRegisterParamSetExt();
	inline static CUPnPDeviceRegisterParamSetExt* NewL(RParameterFamily& aFamily);	

	inline void SetIconDataL ( const TDesC8& aIconData );	
	inline const TDesC8& IconData () const;
	
protected:
	DATA_VTABLE
	};
// -------------------------------------------------------------------------------------	
	

#include "upnpparamset.inl"

#endif // __UPNPPARAMSET_H__
