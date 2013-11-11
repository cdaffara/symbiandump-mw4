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
#include <rcontrolchannel.h>
#include "cpnpservicediscoverybase.h"

class CUPnPServiceDiscoveryImpl;
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
	Resolves the access point.
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

	The uris provided for registraion of advertisements must conform to the schemas specified in UPnPDeviceArchitecture 1.0

	In case invalid input is provided in the form of corrupted uris for searching or low memory conditions,the application returns
	synchronously with MPnPObserver::OnPnPError
	@param aServiceInfo Parameter bundle contains the discovery information.
	
	@panic 1 The RPnPServiceDiscovery::Open() has not been called
	@panic 2 Invalid observer object supplied. It may be null or corrupted
	@panic 3 More than one family supplied in bundle
	@panic 4 Invalid family
	@panic 5 No parameter set used
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
	
	@panic 1 The RPnPServiceDiscovery::Open() has not been called
	@panic 2 Invalid observer object supplied. It may be null or corrupted
	@panic 3 More than one family supplied in bundle
	@panic 4 Invalid family
	@panic 5 No parameter set used
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
		
	@panic 1 The RPnPServiceDiscovery::Open() has not been called
	@panic 2 Invalid observer object supplied. It may be null or corrupted
	@panic 3 More than one family supplied in bundle
	@panic 4 Invalid family
	@panic 5 No parameter set used
	*/

	IMPORT_C void Subscribe ( const RPnPParameterBundle& aServiceInfo );

	/**
	Registers for events of remote service. The technology specific registration  information
	is packed in aServiceInfo. The caller can set a callback function of type MPnPObserver in aServiceInfo
	to retrieve response of registration.

	The uris provided for registraion of advertisements must conform to the schemas specified in UPnPDeviceArchitecture 1.0
	The scheme followed for register notify uris is the same as that followed for the NT parameter of service/device publish
	of UPnP Device Architecture 1.0

	In case invalid input is provided in the form of corrupted uris for registration or low memory conditions,the application returns
	synchronously with MPnPObserver::OnPnPError.

	@param Parameter bundle contains the register information to be sent to remote service.
	
	@panic 1 The RPnPServiceDiscovery::Open() has not been called
	@panic 2 Invalid observer object supplied. It may be null or corrupted
	@panic 3 More than one family supplied in bundle
	@panic 4 Invalid family
	@panic 5 No parameter set used
	*/
	IMPORT_C void  RegisterNotify ( const RPnPParameterBundle& aServiceInfo );

	/**
	Issues a cancel request to ongoing discoveries, describe , subscribe requests .This also cancels notify
	of previously registered events from remote service. The technology specific discovery cancellation information
	is packed in aServiceInfo.

	@param Parameter bundle contains the cancel requests.
	
	@panic 1 The RPnPServiceDiscovery::Open() has not been called
	@panic 3 More than one family supplied in bundle
	@panic 5 No parameter set used
	*/
	IMPORT_C void  Cancel ( const RPnPParameterBundle& aServiceInfo );

	/**
	 Starts the Connection and attaches the sub-connection to it.This also
	 creates the socket and enables it to send and receive messages.
	 @return KErrNone if successful. Returns system wide error codes on failure
	 */
	IMPORT_C TInt InitiateControl ( RControlChannel& aControlChannel, const TDesC8& aUri );

	/**
	Close technology specific session.
	*/
	IMPORT_C void Close ( );

private:
	CPnPServiceDiscoveryBase *iUPnPServiceDiscoveryImpl;
	
	};

#endif //__RPNPPSERVICEDISCOVER_H__
