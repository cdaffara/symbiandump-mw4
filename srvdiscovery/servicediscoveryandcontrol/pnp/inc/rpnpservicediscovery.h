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

#ifndef __RPNPPSERVICEDISCOVER_H__
#define __RPNPPSERVICEDISCOVER_H__

#include <e32base.h>

class RControlChannel;
class CPnPServiceDiscoveryBase;
class RPnPParameterBundle;

/**
A generic class which provides the functionality to provision the applications who want to discover services
and listen for service related notifications. The service discovery and related notification are technology specific.
@publishedPartner
@prototype
*/
class RPnPServiceDiscovery
	{
public:
	/**
	Constructor
	*/
	IMPORT_C RPnPServiceDiscovery( );

	/**
	Resolves the technology.
	@param aTierID implementation UID. Identifies technology for which service discovery is requested.
	@return KErrNone if successful. Returns system wide error codes on failure
	*/
	IMPORT_C TInt Open ( TUint aTierID );

	/**
	Issues a discovery request. The technology specific discovery information is packed
	in aServiceInfo parameter. The caller should set a callback function of type MPnPCallBack in aServiceInfo
	to retrieve discovery responses.
	When the responses are received, MPnPObserver::OnPnPEventL is called for each response and calling clients
	must provide the implementation to handle the responses.

	In case of invalid input is provided in the form of corrupted uris for searching or low memory conditions,the application returns
	synchronously with MPnPObserver::OnPnPError
	@param aServiceInfo Parameter bundle contains the discovery information.	
	*/
	IMPORT_C void  Discover ( const RPnPParameterBundle& aServiceInfo );

	/**
	Retrieves the description of the remote service. The technology specific description information is packed
	in aServiceInfo parameter. The caller should set a callback function of type MPnPObserver
	in aServiceInfo to retrieve description responses.
    When the responses are received MPnPObserver ::OnPnPEventL is called for each response and calling clients must provide
	the implementation to handle the responses.

	In case invalid input is provided in the form of corrupted uris for describe or low memory conditions,the application returns
	synchronously with MPnPObserver::OnPnPError.However application is not able to filter out all invalid URLs
	@param Parameter bundle contains the information about remote service(s) to be described.	
	*/
	IMPORT_C void Describe ( const RPnPParameterBundle& aServiceInfo );

	/**
	Subscribes for event notifications to the remote service or device .
	The technology specific description information is packed in aServiceInfo parameter.
	The caller should set a callback function of type MPnPObserver in aServiceInfo to retrieve description responses.
    When the responses are received MPnPObserver ::OnPnPEventL is called for each response and calling clients must provide
	the implementation to handle the responses.

	In case invalid input is provided in the form of corrupted uris for subscribe or low memory conditions,the application returns
	synchronously with MPnPObserver::OnPnPError.However application is not able to filter out all invalid URLs

	@param Parameter bundle contains the information about remote service(s) to be described.	
	*/

	IMPORT_C void Subscribe ( const RPnPParameterBundle& aServiceInfo );

	/**
	Registers for events of remote service. The technology specific registration  information
	is packed in aServiceInfo. The caller can set a callback function of type MPnPObserver in aServiceInfo
	to retrieve response of registration.


	In case invalid input is provided in the form of corrupted uris for registration or low memory conditions,the application returns
	synchronously with MPnPObserver::OnPnPError.

	@param Parameter bundle contains the register information to be sent to remote service.	
	*/
	IMPORT_C void  RegisterNotify ( const RPnPParameterBundle& aServiceInfo );

	/**
	Issues a cancel request to ongoing discoveries, describe , subscribe requests .This also cancels notify
	of previously registered events from remote service. The technology specific discovery cancellation information
	is packed in aServiceInfo.

	@param Parameter bundle contains the cancel requests.	
	*/
	IMPORT_C void  Cancel ( const RPnPParameterBundle& aServiceInfo );

    /**
	It enables to send and receive messages.
	@return KErrNone if successful. Returns system wide error codes on failure.
	*/
	IMPORT_C TInt InitiateControl ( RControlChannel& aControlChannel, const TDesC8& aUri );

	/**
	Close technology specific session.
	*/
	IMPORT_C void Close ( );

private:
	CPnPServiceDiscoveryBase* iPnPServiceDiscoveryImpl;	
	};

#endif /*__RPNPPSERVICEDISCOVER_H__*/
