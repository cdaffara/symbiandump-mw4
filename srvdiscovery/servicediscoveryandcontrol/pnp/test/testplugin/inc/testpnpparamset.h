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

#ifndef __TESTPNPPARAMSET_H__
#define __TESTPNPPARAMSET_H__

#include <es_sock.h>
#include <in_sock.h>
#include <elements/metadata.h>
#include <comms-infras/es_parameterfamily.h>
#include <comms-infras/es_parameterbundle.h>

#include "testpnpconstants.h"

/**
TestPnP ParameterSet Family Identifiers.
*/
enum TTestPnPParamSetFamily
	{
	// Used by Control Point
	ETestPnPRegisterAnnouncementParamSet 	= 1,
	ETestPnPAbsenceAnnouncementParamSet 	= 2,
	ETestPnPPresenceAnnouncementParamSet	= 3,
	ETestPnPDiscoverRequestParamSet		= 4,
	ETestPnPDiscoverResponseParamSet		= 5,
	//Do not use the 6 as it is used by family which carries implementation UID of SCPR.
	ETestPnPDescribeRequestParamSet		= 7,
	ETestPnPDescribeResponseParamSet		= 8,
	ETestPnPSubscribeRequestParamSet		= 9,
	ETestPnPSubscribeResponseParamSet		= 10,
	ETestPnPNotifyEventParamSet			= 11,
	ETestPnPCancelDiscoverParamSet			= 12,
	ETestPnPCancelDescribeParamSet			= 13,
	ETestPnPCancelSubscribeParamSet		= 14,
	ETestPnPCancelRegisterNotifyParamSet	= 15,
	ETestPnPRequestParamSet				= 16,
	ETestPnPResponseParamSet				= 17,

	// Used by Service Point
	ETestPnPServiceRegisterParamSet		= 18,
	ETestPnPServiceNotificationParamSet	= 19,
	ETestPnPPublishResponseParamSet		= 20,
	};

/**
class TTestPnPEvent
TestPnPEvent class defines the reason code for an event callback.
*/
NONSHARABLE_CLASS(TTestPnPEvent)
	{
public:
	/** Type of TestPnP Event. */
	enum TEventType
	{
		/** This signifies that the operation completed successfully. */
		ESuccess,
		/** This is a 'catch-all' for communicating failures. The actual error reason is
			shown in error code. */
		EFail,
	};

public:
	/** The event status. */
	TInt iStatus;
	/** The error code value. */
	TInt iErrorCode;
	};




/**
The base class for all TestPnP ParamSet classes.
*/
NONSHARABLE_CLASS (CTestPnPParamSetBase) : public CSubConExtensionParameterSet
	{
	public:		
		inline static CTestPnPParamSetBase* NewL ( RParameterFamily& aFamily, TTestPnPParamSetFamily aFamilyType );
	protected:
		inline CTestPnPParamSetBase ();
	};


// -------------------------------------------------------------------

/**
Parameter set class containing a Uri. All TestPnP Request ParamSet classes derive from this class.
*/
NONSHARABLE_CLASS ( CTestPnPRequestParamSetBase ) : public CTestPnPParamSetBase
	{
	protected:
		RBuf8 iUri;
	public:
		inline void SetUriL ( const TDesC8& aUri );
		inline const TDesC8& Uri () const;
	protected:
		inline CTestPnPRequestParamSetBase ();
		inline ~CTestPnPRequestParamSetBase ();
	protected:
		DATA_VTABLE
	};

// ----------------------------------------------------------------------

/**
Parameter set class containing a TestPnP Event. All TestPnP Response ParamSet classes derive from this class.
*/
NONSHARABLE_CLASS ( CTestPnPResponseParamSetBase ) : public CTestPnPRequestParamSetBase
	{
	protected:
		TTestPnPEvent iTestPnPEvent;
	public:
		inline void SetTestPnPEvent ( const TTestPnPEvent& aEvent );
		inline const TTestPnPEvent& TestPnPEvent () const;
	protected:
		inline CTestPnPResponseParamSetBase ();
	protected:
		DATA_VTABLE
	};

// -------------------------------------------------------------------------

/** TestPnP Control Point ParameterSet classes. */

/**
Parameter set class for discovering services/devices.
Calling client should set the ServiceType/DeviceType and maximum wait time.
*/
NONSHARABLE_CLASS(CTestPnPDiscoverRequestParamSet) : public CTestPnPRequestParamSetBase
	{
	private:
		TInt iTimeToDelayResponse;
	public :
		inline static CTestPnPDiscoverRequestParamSet* NewL( RParameterFamily& aFamily );
		inline void SetResponseTime(TInt aResponseTime);
		inline TInt ResponseTime( ) const ;
	protected:
		DATA_VTABLE
	};

// ---------------------------------------------------------------------------

/**
Parameter set class for subscribing to remote service. Client should set the ServiceLocation and Duration
of subscription.
*/
NONSHARABLE_CLASS(CTestPnPSubscribeRequestParamSet) : public CTestPnPRequestParamSetBase
	{
protected:

public:
	inline static CTestPnPSubscribeRequestParamSet* NewL( RParameterFamily& aFamily );

protected:
		DATA_VTABLE
	};

// ----------------------------------------------------------------------------

/**
Parameter set class for registering SSDP:Alive request of a service/device. Caller should send the
ServiceType/DeviceType to register for announcements.
*/
NONSHARABLE_CLASS(CTestPnPRegisterAnnouncementParamSet) : public CTestPnPRequestParamSetBase
	{
	public:
		TBool iRegister;
		inline static CTestPnPRegisterAnnouncementParamSet* NewL ( RParameterFamily& aFamily );
		inline void SetRegisterVariable(TBool aValue);
		inline TBool RegisterVariable();
	protected:
		DATA_VTABLE
	};

// -----------------------------------------------------------------------------

/**
Parameter set class for describe request. Caller should set the ServiceType/DeviceType to be described.
*/
NONSHARABLE_CLASS(CTestPnPDescribeRequestParamSet) : public CTestPnPRequestParamSetBase
	{

	public:
		inline static CTestPnPDescribeRequestParamSet* NewL ( RParameterFamily& aFamily );


	protected:
		DATA_VTABLE
	};
// -----------------------------------------------------------------------------
/**
Parameter set class for cancel discover request.

*/

NONSHARABLE_CLASS(CTestPnPCancelDiscoverParamSet) : public CTestPnPRequestParamSetBase
	{
	public:		
		inline static CTestPnPCancelDiscoverParamSet* NewL( RParameterFamily& aFamily );

	protected:
		DATA_VTABLE
	};

// -----------------------------------------------------------------------------
/**
Parameter set class for cancel describe request.
*/

NONSHARABLE_CLASS(CTestPnPCancelDescribeParamSet) : public CTestPnPRequestParamSetBase
	{
	public:
		inline static CTestPnPCancelDescribeParamSet* NewL( RParameterFamily& aFamily );

	protected:
		DATA_VTABLE
	};

// -----------------------------------------------------------------------------
/**
Parameter set class for cancel subscribe request.
*/

NONSHARABLE_CLASS(CTestPnPCancelSubscribeParamSet) : public CTestPnPRequestParamSetBase
	{
	public:
		inline static CTestPnPCancelSubscribeParamSet* NewL( RParameterFamily& aFamily );

	protected:
		DATA_VTABLE
	};

// -----------------------------------------------------------------------------
/**
Parameter set class for cancel register notify request.
*/
NONSHARABLE_CLASS(CTestPnPCancelRegisterNotifyParamSet) : public CTestPnPRequestParamSetBase
	{
	public:
		inline static CTestPnPCancelRegisterNotifyParamSet* NewL( RParameterFamily& aFamily );

	protected:
		DATA_VTABLE
	};

// -----------------------------------------------------------------------------

/**
Parameter set class for retrieving the responses of discovery requests. Client can extract remote device
information such as Device Location, Unique Service Name (USN) and Max age of the advertisement in
callback function.

*/
NONSHARABLE_CLASS(CTestPnPDiscoverResponseParamSet) : public CTestPnPResponseParamSetBase
	{
public :
	// Internal APIs
	inline static CTestPnPDiscoverResponseParamSet* NewL( RParameterFamily& aFamily );

	// D'tor
	inline ~CTestPnPDiscoverResponseParamSet ();
protected:
		DATA_VTABLE
	};

// -------------------------------------------------------------------------------------

/**
Parameter set class for retrieving responses of describe requests. Client can extract the device description
in callback function.

*/
NONSHARABLE_CLASS(CTestPnPDescribeResponseParamSet) : public CTestPnPResponseParamSetBase
	{
public:
	inline static CTestPnPDescribeResponseParamSet* NewL( RParameterFamily& aFamily );
	inline ~CTestPnPDescribeResponseParamSet ();
protected:
		DATA_VTABLE
	};
// -------------------------------------------------------------------------------------

/**
Parameter set class for retrieving response of subscription request. Calling client can extract the
subscription status in callback function.

*/
NONSHARABLE_CLASS(CTestPnPSubscribeResponseParamSet) : public CTestPnPResponseParamSetBase
	{
public:
	inline static CTestPnPSubscribeResponseParamSet* NewL( RParameterFamily& aFamily );
protected:
		DATA_VTABLE
	};
// -------------------------------------------------------------------------------------

/**
Parameter set class for retrieving the event notification. Calling client can extract the
notification information in callback function. If notification order is not matched an error code
is returned to the client.

*/
NONSHARABLE_CLASS(CTestPnPNotifyEventParamSet) : public CTestPnPResponseParamSetBase
	{
public:
	inline static CTestPnPNotifyEventParamSet* NewL( RParameterFamily& aFamily );
	inline ~CTestPnPNotifyEventParamSet ();
protected:
		DATA_VTABLE
	};
// -------------------------------------------------------------------------------------

/**
Parameter set class for retrieving the SSDP:ByeBye message. Calling client can extract the
published information in callback function.

*/
NONSHARABLE_CLASS(CTestPnPAbsenceAnnouncementParamSet) : public CTestPnPResponseParamSetBase
	{
public:
	inline static CTestPnPAbsenceAnnouncementParamSet* NewL( RParameterFamily& aFamily );

	// D'tor
	inline ~CTestPnPAbsenceAnnouncementParamSet ();
protected:
		DATA_VTABLE
	};
// -------------------------------------------------------------------------------------

/**
Parameter set class for retrieving SSDP:Alive message. Calling client can extract the
published information in callback function.

*/
NONSHARABLE_CLASS(CTestPnPPresenceAnnouncementParamSet) : public CTestPnPAbsenceAnnouncementParamSet
	{
public:
	inline static CTestPnPPresenceAnnouncementParamSet* NewL( RParameterFamily& aFamily );

	// D'tor
	inline ~CTestPnPPresenceAnnouncementParamSet ();
protected:
		DATA_VTABLE
	};
// -------------------------------------------------------------------------------------

/** TestPnP Service Point ParameterSet classes. */

/**
Parameter set class for registering the service and publishing the service information. Service Client
should set the following fields :
*/
NONSHARABLE_CLASS( CTestPnPServiceRegisterParamSet ) : public CTestPnPRequestParamSetBase
	{
	public:
		inline ~CTestPnPServiceRegisterParamSet ();
		inline static CTestPnPServiceRegisterParamSet* NewL(RParameterFamily& aFamily);

	protected:
		DATA_VTABLE
	};


/** Parameter set class for receiving success or failure for service or device
  registration

*/
NONSHARABLE_CLASS(CTestPnPPublishResponseParamSet) : public CTestPnPResponseParamSetBase
	{
public:
	inline static CTestPnPPublishResponseParamSet* NewL( RParameterFamily& aFamily);
protected:
	DATA_VTABLE

	};


#include "testpnpparamset.inl"

#endif // __TESTPNPPARAMSET_H__
